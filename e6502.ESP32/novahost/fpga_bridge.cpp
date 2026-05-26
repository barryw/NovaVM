// FpgaBridge — binary debug protocol implementation
// See debug_bridge.sv for the FPGA side of this protocol.

#include "fpga_bridge.h"

// Binary command bytes (must match debug_bridge.sv)
#define CMD_PEEK        0x01
#define CMD_POKE        0x02
#define CMD_SEND_KEY    0x03
#define CMD_CPU_STATE   0x06
#define CMD_PAUSE       0x07
#define CMD_RESUME      0x08
#define CMD_PEEK_BLOCK  0x09
#define CMD_POKE_ROM    0x0A
#define CMD_RESET_HOLD  0x0B
#define CMD_RESET_REL   0x0C
#define CMD_POKE_ROM_BLK 0x0D
#define CMD_POKE_SDRAM_BLK 0x0E
#define CMD_POKE_BLOCK   0x0F
#define CMD_POKE_VGC_BLK 0x10
#define CMD_FILL_VGC_BLK 0x11
#define CMD_READ_VGC_BLK 0x12
#define CMD_BREAK_SET    0x13
#define CMD_BREAK_CLR    0x14
#define CMD_BREAK_LIST   0x15
#define CMD_STEP         0x16
#define CMD_TRACE_READ   0x17
#define CMD_SYS_RESET_HOLD 0x18
#define CMD_SYS_RESET_REL  0x19
#define CMD_READ_SDRAM_BLK 0x1A
#define CMD_HOST_STATUS    0x1B
#define CMD_NIC_TX_READ_BLK 0x1C
#define CMD_NIC_RX_WRITE_BLK 0x1D
#define CMD_POKE_SDRAM_STREAM 0x1E
#define CMD_WRITE_AUDIO_PCM 0x1F
#define CMD_AUDIO_PCM_STATUS 0x20
#define CMD_BRIDGE_CAPS    0x21
#define CMD_POKE_MULTI     0x22
#define CMD_WRITE_WTS_EVENTS 0x23
#define CMD_WRITE_KEYS     0x24

#define CAP_POKE_MULTI     0x01
#define CAP_WTS_EVENT_STREAM 0x02
#define CAP_KEY_STREAM     0x04

#define EVENT_MARKER       0xFE

#define VGC_SPACE_CHAR  0x01
#define VGC_SPACE_COLOR 0x02
#define TEXT_BYTES      4000

class FpgaBridgeCommandGuard {
public:
    explicit FpgaBridgeCommandGuard(FpgaBridge& bridge)
        : _bridge(bridge), _locked(bridge.lockCommand()) {}

    ~FpgaBridgeCommandGuard() {
        if (_locked)
            _bridge.unlockCommand();
    }

    bool locked() const { return _locked; }

private:
    FpgaBridge& _bridge;
    bool _locked;
};

#define FPGA_LOCK_OR_RETURN_FALSE() \
    FpgaBridgeCommandGuard fpgaLock(*this); \
    if (!fpgaLock.locked()) return false

#define FPGA_LOCK_OR_RETURN_VOID() \
    FpgaBridgeCommandGuard fpgaLock(*this); \
    if (!fpgaLock.locked()) return

// =========================================================================
// Low-level transport helpers
// =========================================================================

bool FpgaBridge::beginSpi(SPIClass& spi, uint8_t csPin, uint32_t hz,
                          uint8_t peerCsPin) {
    _spi = &spi;
    _spiCsPin = csPin;
    _spiPeerCsPin = peerCsPin;
    _spiHz = hz;
    _spiReadHz = (hz > FPGA_SPI_READ_HZ) ? FPGA_SPI_READ_HZ : hz;
    _capabilitiesKnown = false;
    _supportsPokeMulti = false;
    _supportsWtsEventStream = false;
    _supportsKeyStream = false;
    pinMode(_spiCsPin, OUTPUT);
    digitalWrite(_spiCsPin, HIGH);
    if (_spiPeerCsPin != 255) {
        pinMode(_spiPeerCsPin, OUTPUT);
        digitalWrite(_spiPeerCsPin, HIGH);
    }
#if defined(ARDUINO)
    if (!_commandMutex)
        _commandMutex = xSemaphoreCreateRecursiveMutex();
#endif
    _spiEnabled = true;
    return true;
}

const char* FpgaBridge::transportName() const {
    return "spi";
}

uint32_t FpgaBridge::actualSpiHzFor(uint32_t hz) const {
    spi_t* bus = _spi ? _spi->bus() : nullptr;
    uint32_t div = spiFrequencyToClockDiv(bus, hz);
    return spiClockDivToFrequency(bus, div);
}

uint32_t FpgaBridge::actualWriteHz() const {
    return actualSpiHzFor(_spiHz);
}

uint32_t FpgaBridge::actualReadHz() const {
    return actualSpiHzFor(_spiReadHz);
}

bool FpgaBridge::lockCommand() {
#if defined(ARDUINO)
    if (!_commandMutex)
        return true;
    return xSemaphoreTakeRecursive(_commandMutex, portMAX_DELAY) == pdTRUE;
#else
    return true;
#endif
}

void FpgaBridge::unlockCommand() {
#if defined(ARDUINO)
    if (_commandMutex)
        xSemaphoreGiveRecursive(_commandMutex);
#endif
}

bool FpgaBridge::lockSharedBus() {
    return lockCommand();
}

void FpgaBridge::unlockSharedBus() {
    unlockCommand();
}

bool FpgaBridge::probeCapabilities() {
    FPGA_LOCK_OR_RETURN_FALSE();
    _capabilitiesKnown = true;
    _supportsPokeMulti = false;
    _supportsWtsEventStream = false;
    _supportsKeyStream = false;

    drain();
    writeByte(CMD_BRIDGE_CAPS);
    if (!recvStatus())
        return false;

    int caps = recvByte();
    if (caps < 0)
        return false;

    _supportsPokeMulti = (((uint8_t)caps) & CAP_POKE_MULTI) != 0;
    _supportsWtsEventStream = (((uint8_t)caps) & CAP_WTS_EVENT_STREAM) != 0;
    _supportsKeyStream = (((uint8_t)caps) & CAP_KEY_STREAM) != 0;
    return true;
}

bool FpgaBridge::supportsPokeMulti() {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!_capabilitiesKnown)
        probeCapabilities();
    return _supportsPokeMulti;
}

bool FpgaBridge::supportsWtsEventStream() {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!_capabilitiesKnown)
        probeCapabilities();
    return _supportsWtsEventStream;
}

bool FpgaBridge::supportsKeyStream() {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!_capabilitiesKnown)
        probeCapabilities();
    return _supportsKeyStream;
}

void FpgaBridge::drain() {
    FPGA_LOCK_OR_RETURN_VOID();
    uint8_t value = 0;
    for (int i = 0; i < SPI_DRAIN_LIMIT; i++) {
        if (!tryRecvSpiByte(value))
            break;
        if (_drainHandler) {
            _drainHandler(_drainUser, value);
        }
    }
}

int FpgaBridge::recvByte() {
    return recvByte(true);
}

int FpgaBridge::recvByte(bool wait) {
    return recvSpiByte(wait);
}

bool FpgaBridge::recvBytes(uint8_t* buf, int count) {
    unsigned long start = millis();
    for (int i = 0; i < count; i++) {
        while (true) {
            int b = recvByte(false);
            if (b >= 0) {
                buf[i] = (uint8_t)b;
                break;
            }

            if (millis() - start >= BULK_TIMEOUT_MS)
                return false;
            delayMicroseconds(5);
            if (((uint32_t)i & 0x3F) == 0)
                yield();
        }
    }
    return true;
}

bool FpgaBridge::tryRecvSpiByte(uint8_t& value) {
    int b = recvSpiByte(false);
    if (b < 0)
        return false;
    value = (uint8_t)b;
    return true;
}

int FpgaBridge::recvSpiByte(bool wait) {
    if (!spiReady())
        return -1;

    unsigned long start = millis();
    while (true) {
        int result = -1;

        _spi->beginTransaction(spiReadSettings());
        if (_spiPeerCsPin != 255)
            digitalWrite(_spiPeerCsPin, HIGH);
        digitalWrite(_spiCsPin, LOW);
        _spi->transfer(SPI_READ_OP);
        for (uint8_t polls = 0; polls < SPI_READ_POLL_BYTES; polls++) {
            uint8_t token = _spi->transfer(0x00);
            if (token == SPI_TOKEN_DATA) {
                result = _spi->transfer(0x00);
                break;
            }
        }
        digitalWrite(_spiCsPin, HIGH);
        _spi->endTransaction();

        if (result >= 0)
            return result;
        if (!wait || millis() - start >= BYTE_TIMEOUT_MS)
            return -1;
        delayMicroseconds(5);
        yield();
    }
}

void FpgaBridge::writeByte(uint8_t value) {
    writeBytes(&value, 1);
}

void FpgaBridge::writeBytes(const uint8_t* data, size_t len) {
    writeBytesWithSettings(data, len, spiControlWriteSettings());
}

void FpgaBridge::writeBytes(const uint8_t* first, size_t firstLen,
                            const uint8_t* second, size_t secondLen) {
    writeBytesWithSettings(first, firstLen, second, secondLen,
                           spiControlWriteSettings());
}

void FpgaBridge::writeBytesBulk(const uint8_t* first, size_t firstLen,
                                const uint8_t* second, size_t secondLen) {
    writeBytesWithSettings(first, firstLen, second, secondLen,
                           spiWriteSettings());
}

void FpgaBridge::writeBytesWithSettings(const uint8_t* data, size_t len,
                                        const SPISettings& settings) {
    if (len == 0)
        return;

    if (!spiReady())
        return;

    _spi->beginTransaction(settings);
    if (_spiPeerCsPin != 255)
        digitalWrite(_spiPeerCsPin, HIGH);
    digitalWrite(_spiCsPin, LOW);
    _spi->transfer(SPI_WRITE_OP);
    _spi->transferBytes(data, nullptr, (uint32_t)len);
    digitalWrite(_spiCsPin, HIGH);
    _spi->endTransaction();
}

void FpgaBridge::writeBytesWithSettings(const uint8_t* first, size_t firstLen,
                                        const uint8_t* second,
                                        size_t secondLen,
                                        const SPISettings& settings) {
    if (firstLen == 0 && secondLen == 0)
        return;

    if (!spiReady())
        return;

    _spi->beginTransaction(settings);
    if (_spiPeerCsPin != 255)
        digitalWrite(_spiPeerCsPin, HIGH);
    digitalWrite(_spiCsPin, LOW);
    _spi->transfer(SPI_WRITE_OP);
    if (firstLen != 0)
        _spi->transferBytes(first, nullptr, (uint32_t)firstLen);
    if (secondLen != 0)
        _spi->transferBytes(second, nullptr, (uint32_t)secondLen);
    digitalWrite(_spiCsPin, HIGH);
    _spi->endTransaction();
}

bool FpgaBridge::recvStatus() {
    while (true) {
        int s = recvByte();
        if (s < 0)
            return false;

        // Host-service events share the bridge response stream. If a CPU-side
        // FIO/NIC event arrives just before the debug-bridge status byte,
        // dispatch it and keep waiting for the actual response instead of
        // turning the event marker into a false command failure.
        if (s == EVENT_MARKER) {
            int eventType = recvByte();
            if (eventType < 0)
                return false;
            if (_drainHandler) {
                _drainHandler(_drainUser, (uint8_t)s);
                _drainHandler(_drainUser, (uint8_t)eventType);
            }
            continue;
        }

        return s == 0x00;
    }
}

// =========================================================================
// Commands
// =========================================================================

bool FpgaBridge::peek(uint16_t addr, uint8_t& value) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t buf[3] = { CMD_PEEK, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF) };
    writeBytes(buf, 3);
    if (!recvStatus()) return false;
    int b = recvByte();
    if (b < 0) return false;
    value = (uint8_t)b;
    return true;
}

bool FpgaBridge::poke(uint16_t addr, uint8_t value) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t buf[4] = { CMD_POKE, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), value };
    writeBytes(buf, 4);
    return recvStatus();
}

bool FpgaBridge::pokeMulti(const uint16_t* addrs, const uint8_t* values,
                           uint8_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count == 0)
        return true;
    if (!addrs || !values || !supportsPokeMulti())
        return false;

    constexpr uint8_t MAX_BATCH = 32;
    uint8_t off = 0;
    while (off < count) {
        uint8_t batch = count - off;
        if (batch > MAX_BATCH)
            batch = MAX_BATCH;

        uint8_t buf[2 + MAX_BATCH * 3];
        buf[0] = CMD_POKE_MULTI;
        buf[1] = batch;
        size_t pos = 2;
        for (uint8_t i = 0; i < batch; i++) {
            uint16_t addr = addrs[off + i];
            buf[pos++] = (uint8_t)(addr >> 8);
            buf[pos++] = (uint8_t)(addr & 0xFF);
            buf[pos++] = values[off + i];
        }

        drain();
        writeBytes(buf, pos);
        if (!recvStatus())
            return false;
        off += batch;
    }
    return true;
}

bool FpgaBridge::sendKey(uint8_t key) {
    return sendKeys(&key, 1);
}

bool FpgaBridge::sendKeys(const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count == 0)
        return true;
    if (!data || !supportsKeyStream())
        return false;

    uint16_t off = 0;
    while (off < count) {
        uint16_t chunk = count - off;
        if (chunk > 256)
            chunk = 256;

        uint8_t header[2] = {
            CMD_WRITE_KEYS,
            (uint8_t)((chunk == 256) ? 0 : chunk)
        };

        drain();
        writeBytes(header, sizeof(header), data + off, chunk);
        if (!recvStatus())
            return false;
        off += chunk;
    }
    return true;
}

bool FpgaBridge::readScreen(uint8_t* buf) {
    const uint16_t BLOCK = 256;
    for (uint16_t off = 0; off < TEXT_BYTES; off += BLOCK) {
        uint16_t chunk = (TEXT_BYTES - off >= BLOCK) ? BLOCK : (TEXT_BYTES - off);
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!readVgcBlock(VGC_SPACE_CHAR, off, wire_count, buf + off))
            return false;
    }
    return true;
}

bool FpgaBridge::readColor(uint8_t* buf) {
    const uint16_t BLOCK = 256;
    for (uint16_t off = 0; off < TEXT_BYTES; off += BLOCK) {
        uint16_t chunk = (TEXT_BYTES - off >= BLOCK) ? BLOCK : (TEXT_BYTES - off);
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!readVgcBlock(VGC_SPACE_COLOR, off, wire_count, buf + off))
            return false;
    }
    return true;
}

bool FpgaBridge::cpuState(CpuState& st) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_CPU_STATE);
    if (!recvStatus()) return false;
    uint8_t buf[8];
    if (!recvBytes(buf, 8)) return false;
    st.pc    = ((uint16_t)buf[0] << 8) | buf[1];
    st.a     = buf[2];
    st.x     = buf[3];
    st.y     = buf[4];
    st.sp    = buf[5];
    st.flags = buf[6];
    st.status = buf[7];
    st.waiting = (buf[7] & 0x01) != 0;
    st.stopped = (buf[7] & 0x02) != 0;
    st.paused  = (buf[7] & 0x04) != 0;
    st.breakpointHit = (buf[7] & 0x08) != 0;
    st.stepActive = (buf[7] & 0x10) != 0;
    return true;
}

bool FpgaBridge::step(CpuState& st) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_STEP);
    if (!recvStatus()) return false;
    uint8_t buf[8];
    if (!recvBytes(buf, 8)) return false;
    st.pc    = ((uint16_t)buf[0] << 8) | buf[1];
    st.a     = buf[2];
    st.x     = buf[3];
    st.y     = buf[4];
    st.sp    = buf[5];
    st.flags = buf[6];
    st.status = buf[7];
    st.waiting = (buf[7] & 0x01) != 0;
    st.stopped = (buf[7] & 0x02) != 0;
    st.paused  = (buf[7] & 0x04) != 0;
    st.breakpointHit = (buf[7] & 0x08) != 0;
    st.stepActive = (buf[7] & 0x10) != 0;
    return true;
}

bool FpgaBridge::breakSet(uint8_t slot, uint16_t addr, bool enabled) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (slot >= 4) return false;
    drain();
    uint8_t buf[5] = {
        CMD_BREAK_SET,
        slot,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        enabled ? (uint8_t)1 : (uint8_t)0
    };
    writeBytes(buf, 5);
    return recvStatus();
}

bool FpgaBridge::breakClear(uint8_t slot) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (slot >= 4) return false;
    drain();
    uint8_t buf[2] = { CMD_BREAK_CLR, slot };
    writeBytes(buf, 2);
    return recvStatus();
}

bool FpgaBridge::breakList(BreakpointState& state) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_BREAK_LIST);
    if (!recvStatus()) return false;
    uint8_t buf[14];
    if (!recvBytes(buf, 14)) return false;
    state.flags = buf[0];
    state.hitSlot = buf[1] & 0x03;
    for (int i = 0; i < 4; i++) {
        int base = 2 + i * 3;
        state.slots[i].enabled = (buf[base] & 0x01) != 0;
        state.slots[i].address = ((uint16_t)buf[base + 1] << 8) | buf[base + 2];
    }
    return true;
}

bool FpgaBridge::traceRead(uint8_t count, uint8_t* buf, uint16_t& bytesRead) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count > TRACE_RECORDS) return false;
    drain();
    uint8_t cmd[2] = { CMD_TRACE_READ, count };
    writeBytes(cmd, 2);
    if (!recvStatus()) return false;
    uint8_t actualRecords = (count == 0) ? TRACE_RECORDS : count;
    bytesRead = (uint16_t)actualRecords * TRACE_RECORD_BYTES;
    return recvBytes(buf, bytesRead);
}

bool FpgaBridge::pause() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_PAUSE);
    return recvStatus();
}

bool FpgaBridge::resume() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_RESUME);
    return recvStatus();
}

bool FpgaBridge::hostStatus(uint8_t flags) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t buf[2] = { CMD_HOST_STATUS, flags };
    writeBytes(buf, 2);
    return recvStatus();
}

bool FpgaBridge::peekBlock(uint16_t addr, uint8_t count, uint8_t* buf) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t cmd[4] = { CMD_PEEK_BLOCK, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), count };
    writeBytes(cmd, 4);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}

bool FpgaBridge::nicReadTxBlock(uint8_t offset, uint8_t count, uint8_t* buf) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t cmd[3] = { CMD_NIC_TX_READ_BLK, offset, count };
    writeBytes(cmd, 3);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}

bool FpgaBridge::nicWriteRxBlock(uint8_t offset, const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count > 256) return false;
    drain();
    uint8_t header[3] = {
        CMD_NIC_RX_WRITE_BLK,
        offset,
        (uint8_t)(count & 0xFF)          // 256 encodes as 0
    };
    writeBytes(header, 3, data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::resetHold() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_RESET_HOLD);
    return recvStatus();
}

bool FpgaBridge::resetRelease() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_RESET_REL);
    return recvStatus();
}

bool FpgaBridge::systemResetHold() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_SYS_RESET_HOLD);
    return recvStatus();
}

bool FpgaBridge::systemResetRelease() {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_SYS_RESET_REL);
    return recvStatus();
}

bool FpgaBridge::pokeRom(uint8_t idx, uint16_t addr, uint8_t value) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t buf[5] = {
        CMD_POKE_ROM,
        (uint8_t)(idx & 0x01),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        value
    };
    writeBytes(buf, 5);
    return recvStatus();
}

bool FpgaBridge::pokeRomBlock(uint8_t idx, uint16_t start_addr,
                              const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    // count==0 means 256 in the wire protocol; reject anything larger.
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_ROM_BLK,
        (uint8_t)(idx & 0x01),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        (uint8_t)(count & 0xFF)     // 256 encodes as 0
    };
    writeBytes(header, 5, data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::loadRom(uint8_t idx, const uint8_t* data, size_t len) {
    // Split into 256-byte bridge blocks.
    const size_t BLOCK = 256;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!pokeRomBlock(idx, (uint16_t)off, data + off, wire_count))
            return false;
    }
    return true;
}

bool FpgaBridge::pokeSdramBlock(uint32_t addr, const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    // count==0 means 256 in the wire protocol; reject anything larger.
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_SDRAM_BLK,
        (uint8_t)((addr >> 16) & 0xFF),
        (uint8_t)((addr >>  8) & 0xFF),
        (uint8_t)( addr        & 0xFF),
        (uint8_t)(count & 0xFF)        // 256 encodes as 0
    };
    writeBytes(header, 5, data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::pokeSdramStream(uint32_t addr, const uint8_t* data, uint16_t count) {
    if (!data || count == 0) return false;

    uint16_t off = 0;
    while (off < count) {
        uint16_t chunk = count - off;
        if (chunk > SDRAM_STREAM_MAX_BYTES)
            chunk = SDRAM_STREAM_MAX_BYTES;
        if (!pokeSdramStreamChunk(addr + off, data + off, chunk))
            return false;
        off += chunk;
        yield();
    }

    return true;
}

bool FpgaBridge::pokeSdramStreamChunk(uint32_t addr, const uint8_t* data,
                                      uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!data || count == 0 || count > SDRAM_STREAM_MAX_BYTES)
        return false;
    drain();
    uint8_t header[6] = {
        CMD_POKE_SDRAM_STREAM,
        (uint8_t)((addr >> 16) & 0xFF),
        (uint8_t)((addr >>  8) & 0xFF),
        (uint8_t)( addr        & 0xFF),
        (uint8_t)((count >> 8) & 0xFF),
        (uint8_t)( count       & 0xFF)
    };
    writeBytesBulk(header, 6, data, count);
    return recvStatus();
}

bool FpgaBridge::audioPcmStatus(AudioPcmStatus& status) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    writeByte(CMD_AUDIO_PCM_STATUS);
    if (!recvStatus())
        return false;

    uint8_t buf[4];
    if (!recvBytes(buf, 4))
        return false;

    status.bytesFree = ((uint16_t)buf[0] << 8) | buf[1];
    status.underruns = ((uint16_t)buf[2] << 8) | buf[3];
    return true;
}

bool FpgaBridge::writeAudioPcm(const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!data || count == 0)
        return false;

    drain();
    uint8_t header[3] = {
        CMD_WRITE_AUDIO_PCM,
        (uint8_t)((count >> 8) & 0xFF),
        (uint8_t)( count       & 0xFF)
    };
    writeBytes(header, 3, data, count);
    return recvStatus();
}

bool FpgaBridge::streamAudioPcm(const uint8_t* data, size_t len) {
    if (!data && len != 0)
        return false;

    size_t off = 0;
    while (off < len) {
        AudioPcmStatus status;
        if (!audioPcmStatus(status))
            return false;

        uint16_t bytes_free = status.bytesFree & (uint16_t)~0x0003u;
        if (bytes_free == 0) {
            delay(1);
            yield();
            continue;
        }

        size_t chunk = len - off;
        if (chunk > bytes_free)
            chunk = bytes_free;
        if (chunk > 8192)
            chunk = 8192;
        chunk &= ~(size_t)0x03;
        if (chunk == 0) {
            delay(1);
            yield();
            continue;
        }

        if (!writeAudioPcm(data + off, (uint16_t)chunk))
            return false;
        off += chunk;
        yield();
    }
    return true;
}

bool FpgaBridge::writeWtsEvents(const uint8_t* data, uint16_t count) {
    if (!data || count == 0 || (count % 6) != 0)
        return false;
    if (!supportsWtsEventStream())
        return false;

    uint16_t off = 0;
    while (off < count) {
        uint16_t chunk = count - off;
        if (chunk > WTS_EVENT_STREAM_MAX_BYTES)
            chunk = WTS_EVENT_STREAM_MAX_BYTES;
        chunk = (uint16_t)((chunk / 6) * 6);
        if (chunk == 0)
            return false;

        if (!writeWtsEventsChunk(data + off, chunk))
            return false;
        off += chunk;
        yield();
    }

    return true;
}

bool FpgaBridge::writeWtsEventsChunk(const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (!data || count == 0 || count > WTS_EVENT_STREAM_MAX_BYTES ||
        (count % 6) != 0) {
        return false;
    }

    drain();
    uint8_t header[3] = {
        CMD_WRITE_WTS_EVENTS,
        (uint8_t)((count >> 8) & 0xFF),
        (uint8_t)( count       & 0xFF)
    };
    writeBytesBulk(header, 3, data, count);
    return recvStatus();
}

bool FpgaBridge::readSdramBlock(uint32_t addr, uint8_t count, uint8_t* buf) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t header[5] = {
        CMD_READ_SDRAM_BLK,
        (uint8_t)((addr >> 16) & 0xFF),
        (uint8_t)((addr >>  8) & 0xFF),
        (uint8_t)( addr        & 0xFF),
        count
    };
    writeBytes(header, 5);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}

bool FpgaBridge::loadSdram(uint32_t base_addr, const uint8_t* data, size_t len) {
    const size_t BLOCK = 32768;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        if (!pokeSdramStream(base_addr + off, data + off, (uint16_t)chunk))
            return false;
    }
    return true;
}

bool FpgaBridge::pokeBlock(uint16_t addr, const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count > 256) return false;
    drain();
    uint8_t header[4] = {
        CMD_POKE_BLOCK,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        (uint8_t)(count & 0xFF)            // 256 encodes as 0
    };
    writeBytes(header, 4, data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::pokeVgcBlock(uint8_t space, uint16_t start_addr,
                              const uint8_t* data, uint16_t count) {
    FPGA_LOCK_OR_RETURN_FALSE();
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        (uint8_t)(count & 0xFF)            // 256 encodes as 0
    };
    writeBytes(header, 5, data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::fillVgcBlock(uint8_t space, uint16_t start_addr, uint8_t value) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t header[5] = {
        CMD_FILL_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        value
    };
    writeBytes(header, 5);
    return recvStatus();
}

bool FpgaBridge::readVgcBlock(uint8_t space, uint16_t start_addr,
                              uint8_t count, uint8_t* buf) {
    FPGA_LOCK_OR_RETURN_FALSE();
    drain();
    uint8_t header[5] = {
        CMD_READ_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        count
    };
    writeBytes(header, 5);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}

bool FpgaBridge::loadRam(uint16_t base_addr, const uint8_t* data, size_t len) {
    const size_t BLOCK = 256;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!pokeBlock((uint16_t)(base_addr + off), data + off, wire_count))
            return false;
    }
    return true;
}
