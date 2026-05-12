#include "nic_dispatcher.h"
#include <ArduinoJson.h>
#include <SD.h>
#include <string.h>

extern void logLn(const char* fmt, ...);

void NicDispatcher::poll() {
    for (uint8_t slot = 0; slot < MAX_SLOTS; slot++)
        poll_socket(slot);

    poll_pending();
    if (_messageLengthPending)
        retry_message_length();
    if (!_messageLengthPending && _commandClearPending)
        retry_clear_command();
    if (!_messageLengthPending && !_commandClearPending)
        poll_command_register();
    sync_status();
}

void NicDispatcher::poll_pending() {
    if (_handling || !_pending)
        return;

    handle_event();
}

void NicDispatcher::handle_event() {
    if (_handling) {
        _pending = true;
        return;
    }

    _pending = false;
    _handling = true;

    if (!_bridge.peekBlock(BANK_BASE, BANK_SIZE, _bank)) {
        Serial.println("[nic] peekBlock failed - command dropped");
        _handling = false;
        return;
    }

    uint8_t cmd = _bank[OFF_CMD];
    uint8_t seq = _bank[OFF_CMDSEQ];
    if (cmd == 0 && seq != _lastCommandSeq)
        cmd = _bank[OFF_CMDSHADOW];
    uint8_t slot = selected_slot();
    _lastCommand = cmd;
    _lastSlot = slot;
    _lastCommandSeq = seq;

    switch (cmd) {
        case CMD_CONNECT:    handle_connect(slot); break;
        case CMD_DISCONNECT: handle_disconnect(slot); break;
        case CMD_SEND:       handle_send(slot); break;
        case CMD_RECV:       handle_recv(slot); break;
        case CMD_LISTEN:     handle_unsupported(slot, "LISTEN"); break;
        case CMD_ACCEPT:     handle_unsupported(slot, "ACCEPT"); break;
        case 0: break;
        default:
            logLn("[nic] unknown cmd 0x%02X slot=%u", (unsigned)cmd, (unsigned)slot);
            _slots[slot].error = true;
            clear_command();
            sync_status(true);
            break;
    }

    _handling = false;
}

void NicDispatcher::handle_connect(uint8_t slot) {
    char requestedHost[NAME_LIMIT + 1];
    char host[64];
    uint16_t port = remote_port();
    read_hostname(requestedHost, sizeof(requestedHost));
    resolve_endpoint(requestedHost, port, host, sizeof(host), port);
    strlcpy(_lastRequestedHost, requestedHost, sizeof(_lastRequestedHost));
    strlcpy(_lastResolvedHost, host, sizeof(_lastResolvedHost));
    _lastPort = port;
    _lastConnectOk = false;
    _lastError[0] = 0;

    reset_slot(slot);
    logLn("[nic] connect slot=%u host=%s port=%u", (unsigned)slot, host, (unsigned)port);

    if (WiFi.status() != WL_CONNECTED) {
        logLn("[nic] connect failed: WiFi is not connected");
        _slots[slot].error = true;
        strlcpy(_lastError, "wifi not connected", sizeof(_lastError));
        clear_command();
        sync_status(true);
        return;
    }

    if (_slots[slot].client.connect(host, port)) {
        _slots[slot].client.setNoDelay(true);
        _slots[slot].connected = true;
        _slots[slot].error = false;
        _slots[slot].remoteClosed = false;
        _lastConnectOk = true;
    } else {
        logLn("[nic] connect failed slot=%u host=%s port=%u",
              (unsigned)slot, host, (unsigned)port);
        _slots[slot].error = true;
        strlcpy(_lastError, "connect failed", sizeof(_lastError));
    }

    clear_command();
    sync_status(true);
}

void NicDispatcher::handle_disconnect(uint8_t slot) {
    reset_slot(slot);
    clear_command();
    sync_status(true);
}

void NicDispatcher::handle_send(uint8_t slot) {
    SlotState& s = _slots[slot];
    uint16_t len = dma_length();
    uint8_t buf[MAX_MSG_SIZE];
    _lastSendLen = len;
    _lastSendHeaderBytes = 0;
    _lastSendPayloadBytes = 0;
    memset(_lastSendFirst, 0, sizeof(_lastSendFirst));

    if (len == 0)
        len = MAX_MSG_SIZE;
    _lastSendLen = len;

    if (!s.connected || len > MAX_MSG_SIZE) {
        s.error = true;
        strlcpy(_lastError, "send not connected", sizeof(_lastError));
        clear_command();
        sync_status(true);
        return;
    }

    uint8_t wireCount = (len == MAX_MSG_SIZE) ? 0 : (uint8_t)len;
    uint8_t dmaStatus = _bank[OFF_DMASTATUS];
    if ((dmaStatus & DMAST_ERROR) != 0 || (dmaStatus & DMAST_TX_READY) == 0) {
        s.error = true;
        strlcpy(_lastError, "send dma failed", sizeof(_lastError));
        clear_command();
        sync_status(true);
        return;
    }

    if (!_bridge.nicReadTxBlock(0, wireCount, buf)) {
        s.error = true;
        strlcpy(_lastError, "send tx buffer read failed", sizeof(_lastError));
        clear_command();
        sync_status(true);
        return;
    }
    memcpy(_lastSendFirst, buf, min((size_t)len, sizeof(_lastSendFirst)));

    uint8_t lenByte = (len == MAX_MSG_SIZE) ? 0 : (uint8_t)len;
    size_t wroteHeader = s.client.write(&lenByte, 1);
    size_t wrotePayload = s.client.write(buf, len);
    _lastSendHeaderBytes = wroteHeader;
    _lastSendPayloadBytes = wrotePayload;
    s.client.flush();

    if (wroteHeader != 1 || wrotePayload != len) {
        s.error = true;
        s.remoteClosed = true;
        s.connected = false;
        s.client.stop();
        strlcpy(_lastError, "send write failed", sizeof(_lastError));
    }

    clear_command();
    sync_status(true);
}

void NicDispatcher::handle_recv(uint8_t slot) {
    QueuedMessage msg;
    if (!dequeue(slot, msg)) {
        if (set_message_length(0))
            clear_command();
        sync_status(true);
        return;
    }

    uint16_t wireCount = (msg.len == MAX_MSG_SIZE) ? 0 : msg.len;
    if (!_bridge.nicWriteRxBlock(0, msg.data, wireCount)) {
        _slots[slot].error = true;
        strlcpy(_lastError, "recv rx buffer write failed", sizeof(_lastError));
        if (set_message_length(0))
            clear_command();
        sync_status(true);
        return;
    }

    if (!write_message_length_immediate(msg.len)) {
        _slots[slot].error = true;
        strlcpy(_lastError, "recv length write failed", sizeof(_lastError));
        clear_command();
        sync_status(true);
        return;
    }

    if (!_bridge.poke(BANK_BASE + OFF_HOSTCTRL, HOSTCTRL_RX_START) ||
        !wait_dma_complete(DMAST_RX_DONE)) {
        _slots[slot].error = true;
        strlcpy(_lastError, "recv dma failed", sizeof(_lastError));
        if (set_message_length(0))
            clear_command();
        sync_status(true);
        return;
    }

    clear_command();
    sync_status(true);
}

void NicDispatcher::handle_unsupported(uint8_t slot, const char* name) {
    logLn("[nic] %s is not implemented on NovaHost yet", name);
    _slots[slot].error = true;
    clear_command();
    sync_status(true);
}

void NicDispatcher::poll_command_register() {
    if (_handling || _pending)
        return;

    unsigned long now = millis();
    if (now - _lastCommandPollMs < 10)
        return;
    _lastCommandPollMs = now;
    _fallbackCommandPolls++;

    if (!_bridge.peekBlock(BANK_BASE, BANK_SIZE, _bank))
        return;

    uint8_t cmd = _bank[OFF_CMD];
    uint8_t seq = _bank[OFF_CMDSEQ];
    if (cmd == 0 && !(seq != _lastCommandSeq && _bank[OFF_CMDSHADOW] != 0))
        return;

    _fallbackCommands++;
    _pending = true;
    poll_pending();
}

void NicDispatcher::poll_socket(uint8_t slot) {
    SlotState& s = _slots[slot];
    if (!s.connected)
        return;

    if (!s.client.connected() && s.client.available() == 0) {
        s.connected = false;
        s.remoteClosed = true;
        sync_status();
        return;
    }

    bool changed = false;
    while (s.client.available() > 0) {
        if (!s.rxActive) {
            int lenByte = s.client.read();
            if (lenByte < 0)
                break;
            s.rxExpected = (lenByte == 0) ? MAX_MSG_SIZE : (uint16_t)lenByte;
            s.rxReceived = 0;
            s.rxActive = true;
        }

        int available = s.client.available();
        if (available <= 0)
            break;

        int want = (int)(s.rxExpected - s.rxReceived);
        if (want > available)
            want = available;
        int read = s.client.read(s.rxBuffer + s.rxReceived, want);
        if (read <= 0)
            break;

        s.rxReceived += (uint16_t)read;
        if (s.rxReceived >= s.rxExpected) {
            enqueue(slot, s.rxBuffer, s.rxExpected);
            s.rxActive = false;
            changed = true;
        }
    }

    if (changed)
        sync_status();
}

bool NicDispatcher::enqueue(uint8_t slot, const uint8_t* data, uint16_t len) {
    SlotState& s = _slots[slot];
    if (len > MAX_MSG_SIZE)
        return false;

    if (s.count >= MAX_QUEUE) {
        s.head = (uint8_t)((s.head + 1) % MAX_QUEUE);
        s.count--;
        s.error = true;
    }

    QueuedMessage& msg = s.queue[s.tail];
    msg.len = len;
    memcpy(msg.data, data, len);
    s.tail = (uint8_t)((s.tail + 1) % MAX_QUEUE);
    s.count++;
    return true;
}

bool NicDispatcher::dequeue(uint8_t slot, QueuedMessage& msg) {
    SlotState& s = _slots[slot];
    if (s.count == 0)
        return false;

    msg = s.queue[s.head];
    s.head = (uint8_t)((s.head + 1) % MAX_QUEUE);
    s.count--;
    return true;
}

void NicDispatcher::reset_slot(uint8_t slot) {
    SlotState& s = _slots[slot];
    s.client.stop();
    s.connected = false;
    s.error = false;
    s.remoteClosed = false;
    s.rxActive = false;
    s.rxExpected = 0;
    s.rxReceived = 0;
    s.head = 0;
    s.tail = 0;
    s.count = 0;
}

uint8_t NicDispatcher::build_slot_status(uint8_t slot) const {
    const SlotState& s = _slots[slot];
    uint8_t status = SLOT_SENDREADY;
    if (s.connected)
        status |= SLOT_CONNECTED;
    if (s.count > 0)
        status |= SLOT_DATAREADY;
    if (s.error)
        status |= SLOT_ERROR;
    if (s.remoteClosed)
        status |= SLOT_REMOTE_CLOSED;
    return status;
}

uint8_t NicDispatcher::build_global_status() const {
    uint8_t status = ST_READY;
    for (uint8_t slot = 0; slot < MAX_SLOTS; slot++) {
        uint8_t slotStatus = build_slot_status(slot);
        if ((slotStatus & SLOT_DATAREADY) != 0)
            status |= ST_ANYDATA;
        if ((slotStatus & SLOT_ERROR) != 0)
            status |= ST_ANYERROR;
    }
    return status;
}

void NicDispatcher::sync_status(bool force) {
    write_global_status(force);
    for (uint8_t slot = 0; slot < MAX_SLOTS; slot++)
        write_slot_status(slot, force);
}

void NicDispatcher::write_slot_status(uint8_t slot, bool force) {
    uint8_t status = build_slot_status(slot);
    SlotState& s = _slots[slot];
    if (!force && s.lastStatus == status)
        return;
    if (_bridge.poke(BANK_BASE + OFF_SLOTST0 + slot, status))
        s.lastStatus = status;
}

void NicDispatcher::write_global_status(bool force) {
    uint8_t status = build_global_status();
    if (!force && _lastGlobalStatus == status)
        return;
    if (_bridge.poke(BANK_BASE + OFF_STATUS, status))
        _lastGlobalStatus = status;
}

void NicDispatcher::clear_command() {
    for (int attempt = 0; attempt < 3; attempt++) {
        if (_bridge.poke(BANK_BASE + OFF_CMD, 0)) {
            _commandClearPending = false;
            return;
        }

        uint8_t cmd = 0;
        if (!_bridge.peek(BANK_BASE + OFF_CMD, cmd)) {
            delay(1);
            continue;
        }

        if (cmd == 0) {
            _commandClearPending = false;
            return;
        }

        uint8_t seq = 0;
        if (_bridge.peek(BANK_BASE + OFF_CMDSEQ, seq) && is_new_command(cmd, seq)) {
            _commandClearPending = false;
            _pending = true;
            return;
        }

        delay(1);
    }

    _commandClearPending = true;
}

void NicDispatcher::retry_clear_command() {
    if (!_bridge.peekBlock(BANK_BASE, BANK_SIZE, _bank))
        return;

    uint8_t cmd = _bank[OFF_CMD];
    if (cmd == 0) {
        _commandClearPending = false;
        return;
    }

    if (is_new_command(cmd, _bank[OFF_CMDSEQ])) {
        _commandClearPending = false;
        _pending = true;
        return;
    }

    clear_command();
}

bool NicDispatcher::is_new_command(uint8_t cmd, uint8_t seq) const {
    return cmd != _lastCommand || seq != _lastCommandSeq;
}

bool NicDispatcher::set_message_length(uint16_t len) {
    uint8_t value = (len == MAX_MSG_SIZE) ? 0 : (uint8_t)len;
    if (_bridge.poke(BANK_BASE + OFF_MSGLEN, value)) {
        _messageLengthPending = false;
        return true;
    }

    _pendingMessageLength = len;
    _messageLengthPending = true;
    return false;
}

bool NicDispatcher::write_message_length_immediate(uint16_t len) {
    uint8_t value = (len == MAX_MSG_SIZE) ? 0 : (uint8_t)len;
    for (int attempt = 0; attempt < 3; attempt++) {
        if (_bridge.poke(BANK_BASE + OFF_MSGLEN, value)) {
            _messageLengthPending = false;
            return true;
        }
        delay(1);
    }

    return false;
}

bool NicDispatcher::wait_dma_complete(uint8_t expectedReadyBit) {
    for (int attempt = 0; attempt < 8; attempt++) {
        uint8_t status = 0;
        if (!_bridge.peek(BANK_BASE + OFF_DMASTATUS, status)) {
            delay(1);
            continue;
        }

        if ((status & DMAST_BUSY) != 0) {
            delay(1);
            continue;
        }

        if ((status & DMAST_ERROR) != 0)
            return false;

        return (status & expectedReadyBit) != 0;
    }

    return false;
}

void NicDispatcher::retry_message_length() {
    if (set_message_length(_pendingMessageLength))
        clear_command();
}

uint8_t NicDispatcher::selected_slot() const {
    return _bank[OFF_SLOT] & 0x03;
}

uint16_t NicDispatcher::remote_port() const {
    return (uint16_t)_bank[OFF_RPORTL] | ((uint16_t)_bank[OFF_RPORTH] << 8);
}

uint16_t NicDispatcher::dma_address() const {
    return (uint16_t)_bank[OFF_DMAL] | ((uint16_t)_bank[OFF_DMAH] << 8);
}

uint16_t NicDispatcher::dma_length() const {
    uint8_t len = _bank[OFF_DMALEN];
    return len == 0 ? MAX_MSG_SIZE : (uint16_t)len;
}

void NicDispatcher::read_hostname(char* out, size_t out_size) const {
    if (out_size == 0)
        return;

    size_t i = 0;
    while (i + 1 < out_size && i < NAME_LIMIT) {
        char ch = (char)_bank[OFF_NAME + i];
        if (ch == 0)
            break;
        out[i++] = ch;
    }
    out[i] = 0;
}

void NicDispatcher::resolve_endpoint(
    const char* requestedHost,
    uint16_t requestedPort,
    char* hostOut,
    size_t hostOutSize,
    uint16_t& portOut)
{
    load_config();
    if (strcasecmp(requestedHost, "nova-game-server") == 0) {
        if (_configLoadedOk && _gameServerHost[0] != 0) {
            strlcpy(hostOut, _gameServerHost, hostOutSize);
            portOut = _gameServerPort;
        } else {
            strlcpy(hostOut, requestedHost, hostOutSize);
            portOut = requestedPort;
        }
        return;
    }

    strlcpy(hostOut, requestedHost, hostOutSize);
    portOut = requestedPort;
}

void NicDispatcher::load_config() {
    _configLoaded = true;
    _configLoadedOk = false;
    _gameServerHost[0] = 0;
    _gameServerPort = 6503;
    _lastError[0] = 0;

    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg) {
        strlcpy(_lastError, "boot config open failed", sizeof(_lastError));
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, cfg);
    cfg.close();
    if (err) {
        strlcpy(_lastError, "boot config parse failed", sizeof(_lastError));
        return;
    }

    JsonObjectConst network = doc["network"].as<JsonObjectConst>();
    JsonObjectConst gameServer = network["gameServer"].as<JsonObjectConst>();
    const char* host = gameServer["host"].as<const char*>();
    int port = gameServer["port"] | 0;
    if (host && host[0] != 0) {
        strlcpy(_gameServerHost, host, sizeof(_gameServerHost));
        _configLoadedOk = true;
    } else {
        strlcpy(_lastError, "game server config missing", sizeof(_lastError));
    }
    if (port > 0 && port <= 65535)
        _gameServerPort = (uint16_t)port;
}

void NicDispatcher::write_debug_json(char* out, size_t out_size) const {
    if (!out || out_size == 0)
        return;

    snprintf(out, out_size,
             "{\"ok\":true,\"lastCommand\":%u,\"lastCommandSeq\":%u,"
             "\"commandShadow\":%u,"
             "\"lastSlot\":%u,"
             "\"lastRequestedHost\":\"%s\",\"lastResolvedHost\":\"%s\","
             "\"lastPort\":%u,\"lastConnectOk\":%s,"
             "\"lastSendLen\":%u,\"lastSendHeaderBytes\":%u,"
             "\"lastSendPayloadBytes\":%u,"
             "\"lastSendFirst\":[%u,%u,%u,%u,%u,%u,%u,%u],"
             "\"commandClearPending\":%s,\"messageLengthPending\":%s,"
             "\"pendingMessageLength\":%u,"
             "\"fallbackCommandPolls\":%u,\"fallbackCommands\":%u,"
             "\"configLoaded\":%s,\"configLoadedOk\":%s,"
             "\"configuredGameServerHost\":\"%s\",\"configuredGameServerPort\":%u,"
             "\"lastError\":\"%s\",\"globalStatus\":%u,"
             "\"slotStatus\":[%u,%u,%u,%u]}",
             (unsigned)_lastCommand,
             (unsigned)_lastCommandSeq,
             (unsigned)_bank[OFF_CMDSHADOW],
             (unsigned)_lastSlot,
             _lastRequestedHost,
             _lastResolvedHost,
             (unsigned)_lastPort,
             _lastConnectOk ? "true" : "false",
             (unsigned)_lastSendLen,
             (unsigned)_lastSendHeaderBytes,
             (unsigned)_lastSendPayloadBytes,
             (unsigned)_lastSendFirst[0],
             (unsigned)_lastSendFirst[1],
             (unsigned)_lastSendFirst[2],
             (unsigned)_lastSendFirst[3],
             (unsigned)_lastSendFirst[4],
             (unsigned)_lastSendFirst[5],
             (unsigned)_lastSendFirst[6],
             (unsigned)_lastSendFirst[7],
             _commandClearPending ? "true" : "false",
             _messageLengthPending ? "true" : "false",
             (unsigned)_pendingMessageLength,
             (unsigned)_fallbackCommandPolls,
             (unsigned)_fallbackCommands,
             _configLoaded ? "true" : "false",
             _configLoadedOk ? "true" : "false",
             _gameServerHost,
             (unsigned)_gameServerPort,
             _lastError,
             (unsigned)build_global_status(),
             (unsigned)build_slot_status(0),
             (unsigned)build_slot_status(1),
             (unsigned)build_slot_status(2),
             (unsigned)build_slot_status(3));
}
