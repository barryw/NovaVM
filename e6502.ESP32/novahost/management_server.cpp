#include "management_server.h"
#include <stdarg.h>
#include <string.h>

extern void logLn(const char* fmt, ...);
extern String g_sd_diag;
extern bool g_sd_mounted;
extern const char* novaBootPhaseName();
extern bool novaFpgaBridgeAvailable();
extern bool novaVmReset();
extern uint8_t novaHostStatusFlags();
extern bool novaHttpTaskRunning();
extern bool novaHttpTaskLoopSeen();
extern uint32_t novaHttpServiceAgeMs();
extern uint32_t novaHttpFallbackCount();
extern bool novaBridgeTaskRunning();
extern bool novaBridgeTaskLoopSeen();
extern uint32_t novaBridgeServiceAgeMs();
extern uint32_t novaBridgeFallbackCount();
extern bool novaSystemTaskRunning();
extern bool novaSystemTaskLoopSeen();
extern uint32_t novaSystemServiceAgeMs();
extern bool novaNicDispatcherAvailable();
extern bool novaBeginStorageMutation();
extern void novaEndStorageMutation();
extern void novaReleaseIdleAudioCacheForStorage();
extern void novaAudioStatusJson(char* out, size_t out_len);
extern void novaAudioStop();
extern void novaWifiStateChanged();

static uint16_t read_be16(const uint8_t* p) {
    return ((uint16_t)p[0] << 8) | p[1];
}

static uint32_t read_be32(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static void write_be16(uint8_t* p, uint16_t value) {
    p[0] = (uint8_t)(value >> 8);
    p[1] = (uint8_t)value;
}

static void write_be32(uint8_t* p, uint32_t value) {
    p[0] = (uint8_t)(value >> 24);
    p[1] = (uint8_t)(value >> 16);
    p[2] = (uint8_t)(value >> 8);
    p[3] = (uint8_t)value;
}

class CborWriter {
public:
    std::vector<uint8_t> bytes;

    void map(uint64_t count) { type(5, count); }
    void array(uint64_t count) { type(4, count); }
    void uint_value(uint64_t value) { type(0, value); }
    void int_value(int64_t value) {
        if (value >= 0)
            type(0, (uint64_t)value);
        else
            type(1, (uint64_t)(-1 - value));
    }
    void bool_value(bool value) { bytes.push_back(value ? 0xF5 : 0xF4); }
    void null_value() { bytes.push_back(0xF6); }
    void text(const char* value) {
        if (!value)
            value = "";
        size_t len = strlen(value);
        type(3, len);
        bytes.insert(bytes.end(), value, value + len);
    }
    void text(const String& value) { text(value.c_str()); }
    void bytes_value(const uint8_t* data, size_t len) {
        type(2, len);
        bytes.insert(bytes.end(), data, data + len);
    }

private:
    void type(uint8_t major, uint64_t value) {
        uint8_t prefix = (uint8_t)(major << 5);
        if (value < 24) {
            bytes.push_back(prefix | (uint8_t)value);
        } else if (value <= 0xFF) {
            bytes.push_back(prefix | 24);
            bytes.push_back((uint8_t)value);
        } else if (value <= 0xFFFF) {
            bytes.push_back(prefix | 25);
            uint8_t b[2];
            write_be16(b, (uint16_t)value);
            bytes.insert(bytes.end(), b, b + 2);
        } else if (value <= 0xFFFFFFFFULL) {
            bytes.push_back(prefix | 26);
            uint8_t b[4];
            write_be32(b, (uint32_t)value);
            bytes.insert(bytes.end(), b, b + 4);
        } else {
            bytes.push_back(prefix | 27);
            for (int shift = 56; shift >= 0; shift -= 8)
                bytes.push_back((uint8_t)(value >> shift));
        }
    }
};

class CborReader {
public:
    CborReader(const uint8_t* data, size_t len) : _data(data), _len(len) {}

    bool map(uint64_t& count) { return typed(5, count); }
    bool array(uint64_t& count) { return typed(4, count); }
    bool text(String& out) {
        uint64_t len = 0;
        if (!typed(3, len) || remaining() < len)
            return false;
        out = "";
        out.reserve((unsigned int)len);
        for (uint64_t i = 0; i < len; i++)
            out += (char)_data[_pos + i];
        _pos += (size_t)len;
        return true;
    }
    bool uint_value(uint32_t& out) {
        uint64_t value = 0;
        if (!typed(0, value) || value > 0xFFFFFFFFULL)
            return false;
        out = (uint32_t)value;
        return true;
    }
    bool bool_value(bool& out) {
        if (remaining() < 1)
            return false;
        uint8_t b = _data[_pos++];
        if (b == 0xF4) {
            out = false;
            return true;
        }
        if (b == 0xF5) {
            out = true;
            return true;
        }
        return false;
    }
    bool skip() {
        if (remaining() < 1)
            return false;
        uint8_t initial = _data[_pos++];
        uint8_t major = initial >> 5;
        uint8_t add = initial & 0x1F;
        if (major == 7) {
            if (add < 24)
                return true;
            if (add == 24 && remaining() >= 1) {
                _pos++;
                return true;
            }
            return false;
        }
        uint64_t value = 0;
        if (!read_arg(add, value))
            return false;

        if (major == 0 || major == 1)
            return true;
        if (major == 2 || major == 3) {
            if (remaining() < value)
                return false;
            _pos += (size_t)value;
            return true;
        }
        if (major == 4) {
            for (uint64_t i = 0; i < value; i++)
                if (!skip())
                    return false;
            return true;
        }
        if (major == 5) {
            for (uint64_t i = 0; i < value * 2; i++)
                if (!skip())
                    return false;
            return true;
        }
        return false;
    }

private:
    const uint8_t* _data;
    size_t _len;
    size_t _pos = 0;

    size_t remaining() const { return _pos <= _len ? _len - _pos : 0; }

    bool typed(uint8_t expected_major, uint64_t& value) {
        if (remaining() < 1)
            return false;
        uint8_t initial = _data[_pos++];
        uint8_t major = initial >> 5;
        uint8_t add = initial & 0x1F;
        if (major != expected_major)
            return false;
        return read_arg(add, value);
    }

    bool read_arg(uint8_t add, uint64_t& value) {
        if (add < 24) {
            value = add;
            return true;
        }
        if (add == 24 && remaining() >= 1) {
            value = _data[_pos++];
            return true;
        }
        if (add == 25 && remaining() >= 2) {
            value = read_be16(_data + _pos);
            _pos += 2;
            return true;
        }
        if (add == 26 && remaining() >= 4) {
            value = read_be32(_data + _pos);
            _pos += 4;
            return true;
        }
        if (add == 27 && remaining() >= 8) {
            value = 0;
            for (int i = 0; i < 8; i++)
                value = (value << 8) | _data[_pos++];
            return true;
        }
        return false;
    }
};

static void envelope_start(CborWriter& w, bool ok, const char* code,
                           const char* error) {
    w.map(4);
    w.text("ok");
    w.bool_value(ok);
    w.text("error");
    if (error)
        w.text(error);
    else
        w.null_value();
    w.text("code");
    w.text(code ? code : "");
    w.text("data");
}

static bool read_payload_string(const uint8_t* payload, size_t payload_len,
                                const char* name, String& out) {
    CborReader reader(payload, payload_len);
    uint64_t count = 0;
    if (!reader.map(count))
        return false;
    for (uint64_t i = 0; i < count; i++) {
        String key;
        if (!reader.text(key))
            return false;
        if (key == name)
            return reader.text(out);
        if (!reader.skip())
            return false;
    }
    return false;
}

static bool read_payload_u32(const uint8_t* payload, size_t payload_len,
                             const char* name, uint32_t& out) {
    CborReader reader(payload, payload_len);
    uint64_t count = 0;
    if (!reader.map(count))
        return false;
    for (uint64_t i = 0; i < count; i++) {
        String key;
        if (!reader.text(key))
            return false;
        if (key == name)
            return reader.uint_value(out);
        if (!reader.skip())
            return false;
    }
    return false;
}

void ManagementServer::begin() {
    if (_started)
        return;
    _started = true;
    _server.begin();
    logLn("[mgmt] TCP management server up on port 6504");
}

void ManagementServer::loop() {
    if (_server.hasClient()) {
        WiFiClient next = _server.available();
        if (!_client || !_client.connected()) {
            close_client();
            _client = next;
            logLn("[mgmt] client connected from %s",
                  _client.remoteIP().toString().c_str());
        } else {
            next.stop();
        }
    }

    if (!_client)
        return;
    if (!_client.connected()) {
        close_client();
        return;
    }
    if (_client.available() < (int)FRAME_HEADER_BYTES)
        return;

    Frame frame;
    if (!read_frame_header(frame)) {
        close_client();
        return;
    }
    if (frame.payload_len > MAX_CBOR_BYTES || frame.raw_len > MAX_RAW_BYTES) {
        send_error(frame.command, frame.request_id, "bad_frame",
                   "frame too large");
        close_client();
        return;
    }

    std::vector<uint8_t> payload;
    std::vector<uint8_t> raw;
    payload.resize(frame.payload_len);
    raw.resize(frame.raw_len);
    if ((frame.payload_len && !read_exact(payload.data(), payload.size(), 5000)) ||
        (frame.raw_len && !read_exact(raw.data(), raw.size(), 10000))) {
        close_client();
        return;
    }
    handle_frame(frame, payload.data(), payload.size(), raw.data(), raw.size());
}

void ManagementServer::close_client() {
    cleanup_upload();
    if (_client)
        _client.stop();
}

void ManagementServer::cleanup_upload() {
    if (_upload_file)
        _upload_file.close();
    if (_upload_active && _upload_tmp_path[0])
        SD.remove(_upload_tmp_path);
    if (_upload_locked)
        novaEndStorageMutation();
    _upload_active = false;
    _upload_locked = false;
    _upload_path[0] = 0;
    _upload_tmp_path[0] = 0;
    _upload_expected = 0;
    _upload_written = 0;
}

bool ManagementServer::read_exact(uint8_t* out, size_t len,
                                  uint32_t timeout_ms) {
    size_t got = 0;
    uint32_t deadline = millis() + timeout_ms;
    while (got < len && millis() < deadline && _client.connected()) {
        int available = _client.available();
        if (available <= 0) {
            delay(1);
            continue;
        }
        int n = _client.read(out + got, len - got);
        if (n > 0)
            got += (size_t)n;
    }
    return got == len;
}

bool ManagementServer::read_frame_header(Frame& frame) {
    uint8_t header[FRAME_HEADER_BYTES];
    if (!read_exact(header, sizeof(header), 1000))
        return false;
    if (memcmp(header, "NVH1", 4) != 0)
        return false;
    if (read_be16(header + 4) != PROTOCOL_VERSION)
        return false;
    frame.command = read_be16(header + 6);
    frame.flags = read_be16(header + 8);
    frame.request_id = read_be32(header + 10);
    frame.payload_len = read_be32(header + 14);
    frame.raw_len = read_be32(header + 18);
    return true;
}

bool ManagementServer::write_all(const uint8_t* data, size_t len) {
    size_t off = 0;
    uint32_t deadline = millis() + 3000;
    while (off < len && millis() < deadline && _client.connected()) {
        size_t n = _client.write(data + off, len - off);
        if (n == 0) {
            delay(1);
            continue;
        }
        off += n;
    }
    return off == len;
}

void ManagementServer::send_frame(uint16_t command, uint32_t request_id,
                                  const std::vector<uint8_t>& payload,
                                  const uint8_t* raw, size_t raw_len) {
    uint8_t header[FRAME_HEADER_BYTES] = {0};
    memcpy(header, "NVH1", 4);
    write_be16(header + 4, PROTOCOL_VERSION);
    write_be16(header + 6, command);
    write_be16(header + 8, 0);
    write_be32(header + 10, request_id);
    write_be32(header + 14, (uint32_t)payload.size());
    write_be32(header + 18, (uint32_t)raw_len);
    write_all(header, sizeof(header));
    if (!payload.empty())
        write_all(payload.data(), payload.size());
    if (raw && raw_len)
        write_all(raw, raw_len);
}

void ManagementServer::send_error(uint16_t command, uint32_t request_id,
                                  const char* code, const char* message) {
    CborWriter w;
    envelope_start(w, false, code, message);
    w.null_value();
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_frame(const Frame& frame, const uint8_t* payload,
                                    size_t payload_len, const uint8_t* raw,
                                    size_t raw_len) {
    switch (frame.command) {
        case CMD_HELLO: handle_hello(frame.request_id); break;
        case CMD_GET_STATUS: handle_get_status(frame.request_id); break;
        case CMD_LIST_DIRECTORY:
            handle_list_directory(frame.command, frame.request_id, payload,
                                  payload_len);
            break;
        case CMD_READ_FILE:
            handle_read_file(frame.command, frame.request_id, payload,
                             payload_len);
            break;
        case CMD_WRITE_FILE_BEGIN:
            handle_write_begin(frame.command, frame.request_id, payload,
                               payload_len);
            break;
        case CMD_WRITE_FILE_CHUNK:
            handle_write_chunk(frame.command, frame.request_id, payload,
                               payload_len, raw, raw_len);
            break;
        case CMD_WRITE_FILE_COMMIT:
            handle_write_commit(frame.command, frame.request_id);
            break;
        case CMD_WRITE_FILE_ABORT:
            handle_write_abort(frame.command, frame.request_id);
            break;
        case CMD_DELETE_PATH:
            handle_delete_path(frame.command, frame.request_id, payload,
                               payload_len);
            break;
        case CMD_MOUNT_DRIVE:
            handle_mount_drive(frame.command, frame.request_id, payload,
                               payload_len);
            break;
        case CMD_UNMOUNT_DRIVE:
            handle_unmount_drive(frame.command, frame.request_id, payload,
                                 payload_len);
            break;
        case CMD_SET_RUNTIME_CONFIG:
            handle_runtime_config(frame.command, frame.request_id, payload,
                                  payload_len);
            break;
        case CMD_AUDIO_STOP:
            handle_audio_stop(frame.command, frame.request_id);
            break;
        case CMD_VM_RESET:
            handle_vm_reset(frame.command, frame.request_id);
            break;
        case CMD_HOST_REBOOT:
            handle_host_reboot(frame.command, frame.request_id);
            break;
        case CMD_WIFI_SCAN:
            handle_wifi_scan(frame.command, frame.request_id);
            break;
        case CMD_WIFI_CONFIG:
            handle_wifi_config(frame.command, frame.request_id, payload,
                               payload_len);
            break;
        case CMD_WIFI_ACTION:
            handle_wifi_action(frame.command, frame.request_id, payload,
                               payload_len);
            break;
        case CMD_READ_FILE_CHUNK:
            handle_read_file_chunk(frame.command, frame.request_id, payload,
                                   payload_len);
            break;
        default:
            send_error(frame.command, frame.request_id, "unknown_command",
                       "unknown command");
            break;
    }
}

void ManagementServer::handle_hello(uint32_t request_id) {
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(6);
    w.text("protocol");
    w.text("NVH1");
    w.text("version");
    w.uint_value(PROTOCOL_VERSION);
    w.text("firmware");
    w.text(String(__DATE__) + " " + __TIME__);
    w.text("maxPayload");
    w.uint_value(MAX_CBOR_BYTES);
    w.text("maxRaw");
    w.uint_value(MAX_RAW_BYTES);
    w.text("capabilities");
    w.map(9);
    w.text("status"); w.bool_value(true);
    w.text("listing"); w.bool_value(true);
    w.text("upload"); w.bool_value(true);
    w.text("drives"); w.bool_value(true);
    w.text("runtime"); w.bool_value(true);
    w.text("vmReset"); w.bool_value(true);
    w.text("hostReboot"); w.bool_value(true);
    w.text("wifi"); w.bool_value(true);
    w.text("downloadChunks"); w.bool_value(true);
    send_frame(CMD_HELLO, request_id, w.bytes);
}

void ManagementServer::handle_get_status(uint32_t request_id) {
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(6);

    w.text("health");
    w.map(9);
    w.text("ok"); w.bool_value(true);
    w.text("bootPhase"); w.text(novaBootPhaseName());
    w.text("sdMounted"); w.bool_value(g_sd_mounted);
    w.text("fpgaBridgeAvailable"); w.bool_value(novaFpgaBridgeAvailable());
    w.text("hostStatusFlags"); w.uint_value(novaHostStatusFlags());
    w.text("hostStatusHex");
    char hex[8];
    snprintf(hex, sizeof(hex), "0x%02X", (unsigned)novaHostStatusFlags());
    w.text(hex);
    w.text("http");
    w.map(4);
    w.text("taskRunning"); w.bool_value(novaHttpTaskRunning());
    w.text("taskLoopSeen"); w.bool_value(novaHttpTaskLoopSeen());
    w.text("serviceAgeMs"); w.uint_value(novaHttpServiceAgeMs());
    w.text("fallbackCount"); w.uint_value(novaHttpFallbackCount());
    w.text("bridge");
    w.map(4);
    w.text("taskRunning"); w.bool_value(novaBridgeTaskRunning());
    w.text("taskLoopSeen"); w.bool_value(novaBridgeTaskLoopSeen());
    w.text("serviceAgeMs"); w.uint_value(novaBridgeServiceAgeMs());
    w.text("fallbackCount"); w.uint_value(novaBridgeFallbackCount());
    w.text("system");
    w.map(3);
    w.text("taskRunning"); w.bool_value(novaSystemTaskRunning());
    w.text("taskLoopSeen"); w.bool_value(novaSystemTaskLoopSeen());
    w.text("serviceAgeMs"); w.uint_value(novaSystemServiceAgeMs());

    uint8_t ct = g_sd_mounted ? SD.cardType() : CARD_NONE;
    const char* type_name =
        ct == CARD_NONE ? "NONE" :
        ct == CARD_MMC  ? "MMC"  :
        ct == CARD_SD   ? "SDSC" :
        ct == CARD_SDHC ? "SDHC" : "UNKNOWN";
    w.text("sdStatus");
    w.map(7);
    w.text("cardType"); w.text(type_name);
    w.text("cardTypeId"); w.uint_value(ct);
    w.text("mounted"); w.bool_value(g_sd_mounted);
    w.text("bootPhase"); w.text(novaBootPhaseName());
    w.text("cardSize"); w.uint_value(g_sd_mounted ? SD.cardSize() : 0);
    w.text("totalBytes"); w.uint_value(g_sd_mounted ? SD.totalBytes() : 0);
    w.text("usedBytes"); w.uint_value(g_sd_mounted ? SD.usedBytes() : 0);

    NovaWifiManager::Status st = _wifi.status();
    w.text("wifi");
    w.map(18);
    w.text("configured"); w.bool_value(st.configured);
    w.text("connected"); w.bool_value(st.connected);
    w.text("wantConnected"); w.bool_value(st.wantConnected);
    w.text("ssid"); w.text(st.ssid);
    w.text("passwordSet"); w.bool_value(st.passwordSet);
    w.text("useStatic"); w.bool_value(st.useStatic);
    w.text("staticIp"); w.text(st.staticIp);
    w.text("gateway"); w.text(st.gateway);
    w.text("subnet"); w.text(st.subnet);
    w.text("dns"); w.text(st.dns);
    w.text("localIp"); w.text(st.localIp);
    w.text("localGateway"); w.text(st.localGateway);
    w.text("localSubnet"); w.text(st.localSubnet);
    w.text("localDns"); w.text(st.localDns);
    w.text("mac"); w.text(st.mac);
    w.text("rssi"); w.int_value(st.rssi);
    w.text("wifiStatus"); w.text(NovaWifiManager::statusName(st.wifiStatus));
    w.text("lastError"); w.text(st.lastError);

    char audio[2048];
    novaAudioStatusJson(audio, sizeof(audio));
    w.text("audioJson");
    w.text(audio);

    w.text("drives");
    static const int list_order[] = {
        DeviceManager::FD0, DeviceManager::FD1,
        DeviceManager::FD2, DeviceManager::FD3,
        DeviceManager::HD0, DeviceManager::HD1
    };
    w.array(DeviceManager::NUM_SLOTS);
    for (int i = 0; i < DeviceManager::NUM_SLOTS; i++) {
        int slot = list_order[i];
        const char* prefix = DeviceManager::prefix_for_slot(slot);
        char configured[300];
        w.map(4);
        w.text("slot"); w.text(prefix);
        w.text("mounted"); w.bool_value(_dm.is_mounted(slot));
        w.text("currentPath"); w.text(_dm.current_path(slot));
        w.text("configuredPath");
        w.text(load_drive_mount_config(prefix, configured, sizeof(configured))
            ? configured : "");
    }

    String boot_config;
    read_boot_config_text(boot_config);
    w.text("bootConfigJson");
    w.text(boot_config);
    send_frame(CMD_GET_STATUS, request_id, w.bytes);
}

void ManagementServer::handle_list_directory(uint16_t command,
                                             uint32_t request_id,
                                             const uint8_t* payload,
                                             size_t payload_len) {
    if (!g_sd_mounted) {
        send_error(command, request_id, "sd_missing", "sd not mounted");
        return;
    }
    String path;
    if (!read_payload_string(payload, payload_len, "path", path)) {
        send_error(command, request_id, "bad_request", "path is required");
        return;
    }
    while (path.startsWith("/"))
        path.remove(0, 1);
    if (!path_sane(path.c_str(), true)) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }

    char full[300];
    snprintf(full, sizeof(full), "/%s", path.c_str());
    File dir = SD.open(full);
    if (!dir || !dir.isDirectory()) {
        if (dir) dir.close();
        send_error(command, request_id, "not_directory", "not a directory");
        return;
    }

    struct Row { String name; uint32_t size; bool dir; };
    std::vector<Row> rows;
    File entry;
    while ((entry = dir.openNextFile())) {
        rows.push_back({String(entry.name()), (uint32_t)entry.size(),
                        entry.isDirectory()});
        entry.close();
    }
    dir.close();

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.array(rows.size());
    for (const Row& row : rows) {
        w.map(3);
        w.text("name"); w.text(row.name);
        w.text("size"); w.uint_value(row.size);
        w.text("dir"); w.bool_value(row.dir);
    }
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_read_file(uint16_t command, uint32_t request_id,
                                        const uint8_t* payload,
                                        size_t payload_len) {
    if (!g_sd_mounted) {
        send_error(command, request_id, "sd_missing", "sd not mounted");
        return;
    }
    String path;
    if (!read_payload_string(payload, payload_len, "path", path)) {
        send_error(command, request_id, "bad_request", "path is required");
        return;
    }
    while (path.startsWith("/"))
        path.remove(0, 1);
    if (!path_sane(path.c_str(), false)) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }
    char full[300];
    snprintf(full, sizeof(full), "/%s", path.c_str());
    File file = SD.open(full, FILE_READ);
    if (!file) {
        send_error(command, request_id, "not_found", "not found");
        return;
    }
    size_t size = file.size();
    if (size > MAX_CBOR_BYTES) {
        file.close();
        send_error(command, request_id, "too_large", "file too large");
        return;
    }
    std::vector<uint8_t> raw;
    raw.resize(size);
    if (size)
        file.read(raw.data(), size);
    file.close();

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("path"); w.text(full);
    w.text("size"); w.uint_value(size);
    send_frame(command, request_id, w.bytes, raw.data(), raw.size());
}

void ManagementServer::handle_read_file_chunk(uint16_t command,
                                              uint32_t request_id,
                                              const uint8_t* payload,
                                              size_t payload_len) {
    if (!g_sd_mounted) {
        send_error(command, request_id, "sd_missing", "sd not mounted");
        return;
    }

    String path;
    uint32_t offset = 0;
    uint32_t length = 0;
    if (!read_payload_string(payload, payload_len, "path", path) ||
        !read_payload_u32(payload, payload_len, "offset", offset) ||
        !read_payload_u32(payload, payload_len, "length", length)) {
        send_error(command, request_id, "bad_request",
                   "path, offset, and length are required");
        return;
    }
    while (path.startsWith("/"))
        path.remove(0, 1);
    if (!path_sane(path.c_str(), false)) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }

    char full[300];
    snprintf(full, sizeof(full), "/%s", path.c_str());
    File file = SD.open(full, FILE_READ);
    if (!file) {
        send_error(command, request_id, "not_found", "not found");
        return;
    }

    uint32_t file_size = (uint32_t)file.size();
    if (offset > file_size) {
        file.close();
        send_error(command, request_id, "bad_offset", "offset past EOF");
        return;
    }
    if (length == 0 || length > MAX_RAW_BYTES)
        length = MAX_RAW_BYTES;
    uint32_t remaining = file_size - offset;
    uint32_t want = remaining < length ? remaining : length;
    if (!file.seek(offset)) {
        file.close();
        send_error(command, request_id, "seek_failed", "seek failed");
        return;
    }

    std::vector<uint8_t> raw;
    raw.resize(want);
    size_t got = want ? file.read(raw.data(), want) : 0;
    file.close();
    raw.resize(got);

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(5);
    w.text("path"); w.text(full);
    w.text("offset"); w.uint_value(offset);
    w.text("length"); w.uint_value(got);
    w.text("size"); w.uint_value(file_size);
    w.text("eof"); w.bool_value(offset + got >= file_size);
    send_frame(command, request_id, w.bytes, raw.data(), raw.size());
}

void ManagementServer::handle_write_begin(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len) {
    cleanup_upload();
    if (!g_sd_mounted) {
        send_error(command, request_id, "sd_missing", "sd not mounted");
        return;
    }
    String path;
    uint32_t size = 0;
    if (!read_payload_string(payload, payload_len, "path", path) ||
        !read_payload_u32(payload, payload_len, "size", size)) {
        send_error(command, request_id, "bad_request", "path and size required");
        return;
    }
    while (path.startsWith("/"))
        path.remove(0, 1);
    if (!path_sane(path.c_str(), false) || !path_safe_for_write(path.c_str())) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }
    snprintf(_upload_path, sizeof(_upload_path), "/%s", path.c_str());
    if (!ensure_parent_dirs(_upload_path)) {
        send_error(command, request_id, "mkdir_failed", "mkdir failed");
        return;
    }
    if (snprintf(_upload_tmp_path, sizeof(_upload_tmp_path), "%s.tmp",
                 _upload_path) >= (int)sizeof(_upload_tmp_path)) {
        send_error(command, request_id, "path_too_long", "path too long");
        return;
    }
    _upload_locked = novaBeginStorageMutation();
    if (!_upload_locked) {
        send_error(command, request_id, "storage_busy", "storage busy");
        return;
    }
    if (SD.exists(_upload_tmp_path) && !SD.remove(_upload_tmp_path)) {
        cleanup_upload();
        send_error(command, request_id, "temp_remove_failed",
                   "temp remove failed");
        return;
    }
    novaReleaseIdleAudioCacheForStorage();
    _upload_file = SD.open(_upload_tmp_path, "wb", true);
    if (!_upload_file) {
        cleanup_upload();
        send_error(command, request_id, "open_failed", "open failed");
        return;
    }
    _upload_file.setBufferSize(4096);
    _upload_active = true;
    _upload_expected = size;
    _upload_written = 0;

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("path"); w.text(_upload_path);
    w.text("size"); w.uint_value(size);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_write_chunk(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len,
                                          const uint8_t* raw,
                                          size_t raw_len) {
    uint32_t offset = 0;
    if (!_upload_active || !read_payload_u32(payload, payload_len, "offset",
                                            offset)) {
        send_error(command, request_id, "no_upload", "no upload active");
        return;
    }
    if (offset != _upload_written) {
        send_error(command, request_id, "bad_offset", "bad upload offset");
        return;
    }
    size_t written = _upload_file.write(raw, raw_len);
    if (written != raw_len) {
        cleanup_upload();
        send_error(command, request_id, "write_failed", "write failed");
        return;
    }
    _upload_written += (uint32_t)written;
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("written"); w.uint_value(_upload_written);
    w.text("size"); w.uint_value(_upload_expected);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_write_commit(uint16_t command,
                                           uint32_t request_id) {
    if (!_upload_active) {
        send_error(command, request_id, "no_upload", "no upload active");
        return;
    }
    _upload_file.close();
    char error[96] = {0};
    if (_upload_written != _upload_expected ||
        !verify_sd_file_size(_upload_tmp_path, _upload_expected, error,
                             sizeof(error))) {
        cleanup_upload();
        send_error(command, request_id, "size_mismatch",
                   error[0] ? error : "size mismatch");
        return;
    }
    if (SD.exists(_upload_path) && !SD.remove(_upload_path)) {
        cleanup_upload();
        send_error(command, request_id, "replace_remove_failed",
                   "replace remove failed");
        return;
    }
    if (!SD.rename(_upload_tmp_path, _upload_path)) {
        cleanup_upload();
        send_error(command, request_id, "replace_rename_failed",
                   "replace rename failed");
        return;
    }
    if (_upload_locked)
        novaEndStorageMutation();
    _upload_locked = false;
    _upload_active = false;

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("path"); w.text(_upload_path);
    w.text("size"); w.uint_value(_upload_expected);
    send_frame(command, request_id, w.bytes);
    logLn("[mgmt] WRITE %s (%lu bytes)", _upload_path,
          (unsigned long)_upload_expected);
}

void ManagementServer::handle_write_abort(uint16_t command,
                                          uint32_t request_id) {
    cleanup_upload();
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("aborted"); w.bool_value(true);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_delete_path(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len) {
    String path;
    if (!g_sd_mounted) {
        send_error(command, request_id, "sd_missing", "sd not mounted");
        return;
    }
    if (!read_payload_string(payload, payload_len, "path", path)) {
        send_error(command, request_id, "bad_request", "path is required");
        return;
    }
    while (path.startsWith("/"))
        path.remove(0, 1);
    if (!path_sane(path.c_str(), false) || !path_safe_for_write(path.c_str())) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }
    if (!novaBeginStorageMutation()) {
        send_error(command, request_id, "storage_busy", "storage busy");
        return;
    }
    char full[300];
    snprintf(full, sizeof(full), "/%s", path.c_str());
    File entry = SD.open(full);
    if (!entry) {
        novaEndStorageMutation();
        send_error(command, request_id, "not_found", "not found");
        return;
    }
    bool is_dir = entry.isDirectory();
    entry.close();
    bool ok = is_dir ? SD.rmdir(full) : SD.remove(full);
    novaEndStorageMutation();
    if (!ok) {
        send_error(command, request_id, "delete_failed", "delete failed");
        return;
    }
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("path"); w.text(full);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_mount_drive(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len) {
    String slot_name;
    String path;
    if (!read_payload_string(payload, payload_len, "slot", slot_name)) {
        send_error(command, request_id, "bad_request", "slot is required");
        return;
    }
    int slot = DeviceManager::slot_for_prefix(slot_name.c_str());
    if (slot < 0) {
        send_error(command, request_id, "bad_slot", "bad slot");
        return;
    }
    const char* prefix = DeviceManager::prefix_for_slot(slot);
    bool have_path = read_payload_string(payload, payload_len, "path", path);
    char sd_path[300] = {0};
    if (have_path && path.length() > 0) {
        if (path[0] == '/')
            snprintf(sd_path, sizeof(sd_path), "%s", path.c_str());
        else
            snprintf(sd_path, sizeof(sd_path), "/%s", path.c_str());
    } else if (!load_drive_mount_config(prefix, sd_path, sizeof(sd_path))) {
        send_error(command, request_id, "missing_path", "missing path");
        return;
    }
    if (!path_sane(sd_path + 1, false)) {
        send_error(command, request_id, "bad_path", "bad path");
        return;
    }
    String lower(sd_path);
    lower.toLowerCase();
    if (!lower.endsWith(".ndi")) {
        send_error(command, request_id, "bad_path", "path must end in .ndi");
        return;
    }
    File entry = SD.open(sd_path, FILE_READ);
    if (!entry) {
        send_error(command, request_id, "not_found", "not found");
        return;
    }
    entry.close();

    if (!novaBeginStorageMutation()) {
        send_error(command, request_id, "storage_busy", "storage busy");
        return;
    }

    novaReleaseIdleAudioCacheForStorage();
    if (!_dm.mount(slot, sd_path)) {
        novaEndStorageMutation();
        send_error(command, request_id, "mount_failed",
                   _dm.last_mount_error());
        return;
    }
    bool config_ok = persist_drive_mount_config(prefix, sd_path);
    int boot_slot = _dm.select_boot_slot();
    _dm.set_default_slot(boot_slot);
    const char* boot_prefix = DeviceManager::prefix_for_slot(boot_slot);
    novaEndStorageMutation();

    if (!config_ok) {
        send_error(command, request_id, "write_failed",
                   "failed to update boot config");
        return;
    }

    logLn("[mgmt] mount %s -> %s; boot default=%s",
          prefix, sd_path, boot_prefix ? boot_prefix : "?");

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(4);
    w.text("slot"); w.text(prefix);
    w.text("mounted"); w.bool_value(true);
    w.text("path"); w.text(sd_path);
    w.text("bootDefault"); w.text(boot_prefix ? boot_prefix : "");
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_unmount_drive(uint16_t command,
                                            uint32_t request_id,
                                            const uint8_t* payload,
                                            size_t payload_len) {
    String slot_name;
    if (!read_payload_string(payload, payload_len, "slot", slot_name)) {
        send_error(command, request_id, "bad_request", "slot is required");
        return;
    }
    int slot = DeviceManager::slot_for_prefix(slot_name.c_str());
    if (slot < 0) {
        send_error(command, request_id, "bad_slot", "bad slot");
        return;
    }
    const char* prefix = DeviceManager::prefix_for_slot(slot);

    if (!novaBeginStorageMutation()) {
        send_error(command, request_id, "storage_busy", "storage busy");
        return;
    }

    _dm.unmount(slot);
    bool config_ok = persist_drive_mount_config(prefix, "");
    int boot_slot = _dm.select_boot_slot();
    _dm.set_default_slot(boot_slot);
    const char* boot_prefix = DeviceManager::prefix_for_slot(boot_slot);
    novaEndStorageMutation();

    if (!config_ok) {
        send_error(command, request_id, "write_failed",
                   "failed to update boot config");
        return;
    }

    logLn("[mgmt] unmount %s; boot default=%s",
          prefix, boot_prefix ? boot_prefix : "?");

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(3);
    w.text("slot"); w.text(prefix);
    w.text("mounted"); w.bool_value(false);
    w.text("bootDefault"); w.text(boot_prefix ? boot_prefix : "");
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_runtime_config(uint16_t command,
                                             uint32_t request_id,
                                             const uint8_t* payload,
                                             size_t payload_len) {
    CborReader reader(payload, payload_len);
    uint64_t count = 0;
    String action, name, rom, extension_rom;
    bool make_active = false;
    if (!reader.map(count)) {
        send_error(command, request_id, "bad_request", "bad payload");
        return;
    }
    for (uint64_t i = 0; i < count; i++) {
        String key;
        if (!reader.text(key)) {
            send_error(command, request_id, "bad_request", "bad key");
            return;
        }
        if (key == "action") reader.text(action);
        else if (key == "name") reader.text(name);
        else if (key == "rom") reader.text(rom);
        else if (key == "extensionRom") reader.text(extension_rom);
        else if (key == "makeActive") reader.bool_value(make_active);
        else reader.skip();
    }
    if (name.length() == 0 || action.length() == 0) {
        send_error(command, request_id, "bad_request",
                   "action and name are required");
        return;
    }

    JsonDocument doc;
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (cfg) {
        DeserializationError err = deserializeJson(doc, cfg);
        cfg.close();
        if (err)
            doc.clear();
    }
    JsonObject vm = doc["vm"].as<JsonObject>();
    if (vm.isNull())
        vm = doc["vm"].to<JsonObject>();
    JsonObject languages = doc["languages"].as<JsonObject>();
    if (languages.isNull())
        languages = doc["languages"].to<JsonObject>();

    if (action == "set") {
        if (languages[name].isNull()) {
            send_error(command, request_id, "not_configured",
                       "runtime is not configured");
            return;
        }
        vm["defaultRuntime"] = name;
    } else if (action == "add") {
        if (rom.length() == 0) {
            send_error(command, request_id, "bad_request", "rom is required");
            return;
        }
        JsonObject entry = languages[name].to<JsonObject>();
        entry["rom"] = rom;
        if (extension_rom.length() > 0)
            entry["extensionRom"] = extension_rom;
        else
            entry.remove("extensionRom");
        if (make_active)
            vm["defaultRuntime"] = name;
    } else if (action == "remove") {
        const char* active = vm["defaultRuntime"] | "";
        if (name == active) {
            send_error(command, request_id, "active_runtime",
                       "active runtime cannot be removed");
            return;
        }
        languages.remove(name);
    } else {
        send_error(command, request_id, "bad_action", "bad action");
        return;
    }

    if (!write_boot_config(doc)) {
        send_error(command, request_id, "write_failed",
                   "boot config write failed");
        return;
    }

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("runtime"); w.text(name);
    w.text("action"); w.text(action);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_audio_stop(uint16_t command,
                                         uint32_t request_id) {
    novaAudioStop();
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("audioStopped"); w.bool_value(true);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_vm_reset(uint16_t command, uint32_t request_id) {
    if (!novaVmReset()) {
        send_error(command, request_id, "vm_reset_unavailable",
                   "vm reset unavailable");
        return;
    }
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("vmReset"); w.bool_value(true);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_host_reboot(uint16_t command,
                                          uint32_t request_id) {
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("rebooting"); w.bool_value(true);
    send_frame(command, request_id, w.bytes);
    delay(100);
    ESP.restart();
}

void ManagementServer::handle_wifi_scan(uint16_t command,
                                        uint32_t request_id) {
    int count = WiFi.scanNetworks();
    if (count < 0)
        count = 0;

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.array(count);
    for (int i = 0; i < count; i++) {
        w.map(4);
        w.text("ssid"); w.text(WiFi.SSID(i));
        w.text("rssi"); w.int_value(WiFi.RSSI(i));
        w.text("channel"); w.uint_value((uint32_t)WiFi.channel(i));
        w.text("encrypted");
        w.bool_value(WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }
    send_frame(command, request_id, w.bytes);
    WiFi.scanDelete();
}

void ManagementServer::handle_wifi_config(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len) {
    CborReader reader(payload, payload_len);
    uint64_t count = 0;
    if (!reader.map(count)) {
        send_error(command, request_id, "bad_request", "bad payload");
        return;
    }

    NovaWifiManager::Config cfg = _wifi.config();
    bool changed = false;
    for (uint64_t i = 0; i < count; i++) {
        String key;
        if (!reader.text(key)) {
            send_error(command, request_id, "bad_request", "bad key");
            return;
        }

        if (key == "ssid") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request", "bad ssid");
                return;
            }
            cfg.ssid = value;
            changed = true;
        } else if (key == "password") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request",
                           "bad password");
                return;
            }
            cfg.password = value;
            changed = true;
        } else if (key == "dhcp") {
            bool value = false;
            if (!reader.bool_value(value)) {
                send_error(command, request_id, "bad_request", "bad dhcp");
                return;
            }
            cfg.useStatic = !value;
            changed = true;
        } else if (key == "useStatic") {
            bool value = false;
            if (!reader.bool_value(value)) {
                send_error(command, request_id, "bad_request",
                           "bad useStatic");
                return;
            }
            cfg.useStatic = value;
            changed = true;
        } else if (key == "staticIp") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request",
                           "bad staticIp");
                return;
            }
            cfg.staticIp = value;
            changed = true;
        } else if (key == "gateway") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request",
                           "bad gateway");
                return;
            }
            cfg.gateway = value;
            changed = true;
        } else if (key == "subnet") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request", "bad subnet");
                return;
            }
            cfg.subnet = value;
            changed = true;
        } else if (key == "dns") {
            String value;
            if (!reader.text(value)) {
                send_error(command, request_id, "bad_request", "bad dns");
                return;
            }
            cfg.dns = value;
            changed = true;
        } else if (!reader.skip()) {
            send_error(command, request_id, "bad_request", "bad value");
            return;
        }
    }

    if (!changed) {
        send_error(command, request_id, "bad_request", "no settings");
        return;
    }

    String save_error;
    if (!_wifi.saveConfig(cfg, &save_error)) {
        send_error(command, request_id, "bad_wifi_config",
                   save_error.c_str());
        return;
    }

    novaWifiStateChanged();
    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(1);
    w.text("configured"); w.bool_value(true);
    send_frame(command, request_id, w.bytes);
}

void ManagementServer::handle_wifi_action(uint16_t command,
                                          uint32_t request_id,
                                          const uint8_t* payload,
                                          size_t payload_len) {
    String action;
    if (!read_payload_string(payload, payload_len, "action", action)) {
        send_error(command, request_id, "bad_request", "action is required");
        return;
    }
    action.toLowerCase();

    if (action != "connect" && action != "disconnect" &&
        action != "reconnect" && action != "forget") {
        send_error(command, request_id, "bad_action", "bad action");
        return;
    }

    CborWriter w;
    envelope_start(w, true, "ok", nullptr);
    w.map(2);
    w.text("accepted"); w.bool_value(true);
    w.text("action"); w.text(action);
    send_frame(command, request_id, w.bytes);
    delay(100);

    if (action == "connect") {
        _wifi.connect();
    } else if (action == "disconnect") {
        _wifi.disconnect();
    } else if (action == "reconnect") {
        _wifi.reconnect();
    } else if (action == "forget") {
        _wifi.forget();
    }

    novaWifiStateChanged();
}

bool ManagementServer::path_sane(const char* path, bool allow_empty) {
    if (!path) return false;
    if (!allow_empty && path[0] == 0) return false;
    if (path[0] == '/') return false;
    if (strstr(path, "..")) return false;
    if (strchr(path, '\\')) return false;
    return true;
}

static String normalized_sd_path(const char* path) {
    String value(path ? path : "");
    value.trim();
    value.toLowerCase();
    while (value.startsWith("/"))
        value.remove(0, 1);
    return value;
}

bool ManagementServer::path_safe_for_write(const char* path) {
    String lower = normalized_sd_path(path);
    if (!lower.endsWith(".ndi")) return true;
    for (int s = 0; s < DeviceManager::NUM_SLOTS; s++) {
        if (!_dm.is_mounted(s)) continue;
        const char* prefix = DeviceManager::prefix_for_slot(s);
        if (!prefix) continue;
        char configured[300];
        if (load_drive_mount_config(prefix, configured, sizeof(configured)) &&
            lower == normalized_sd_path(configured))
            return false;
        String needle = String(prefix) + ".ndi";
        if (lower.endsWith(needle)) return false;
    }
    return true;
}

bool ManagementServer::ensure_parent_dirs(const char* full_path) {
    char tmp[300];
    strncpy(tmp, full_path, sizeof(tmp));
    tmp[sizeof(tmp) - 1] = 0;
    for (char* p = tmp + 1; *p; p++) {
        if (*p != '/') continue;
        *p = 0;
        if (!SD.exists(tmp) && !SD.mkdir(tmp)) return false;
        *p = '/';
    }
    return true;
}

bool ManagementServer::persist_drive_mount_config(const char* prefix,
                                                  const char* sd_path) {
    JsonDocument doc;
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (cfg) {
        if (deserializeJson(doc, cfg))
            doc.clear();
        cfg.close();
    }
    JsonObject mounts = doc["mounts"].as<JsonObject>();
    if (mounts.isNull())
        mounts = doc["mounts"].to<JsonObject>();
    mounts[prefix] = sd_path ? sd_path : "";
    return write_boot_config(doc);
}

bool ManagementServer::load_drive_mount_config(const char* prefix, char* out,
                                               size_t out_len) {
    if (!prefix || !out || out_len == 0)
        return false;
    out[0] = 0;
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg)
        return false;
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, cfg);
    cfg.close();
    if (err)
        return false;
    const char* path = doc["mounts"][prefix] | "";
    if (!path[0])
        return false;
    strncpy(out, path, out_len);
    out[out_len - 1] = 0;
    return true;
}

bool ManagementServer::write_boot_config(JsonDocument& doc) {
    if (!SD.exists("/config") && !SD.mkdir("/config"))
        return false;
    const char* tmp = "/config/boot.json.tmp";
    if (SD.exists(tmp) && !SD.remove(tmp))
        return false;
    File out = SD.open(tmp, FILE_WRITE, true);
    if (!out)
        return false;
    serializeJsonPretty(doc, out);
    out.println();
    out.close();
    if (SD.exists("/config/boot.json") && !SD.remove("/config/boot.json")) {
        SD.remove(tmp);
        return false;
    }
    if (!SD.rename(tmp, "/config/boot.json")) {
        SD.remove(tmp);
        return false;
    }
    return true;
}

bool ManagementServer::read_boot_config_text(String& out) {
    out = "";
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg)
        return false;
    while (cfg.available())
        out += (char)cfg.read();
    cfg.close();
    return true;
}

size_t ManagementServer::sd_file_size(const char* path) {
    File verify = SD.open(path, FILE_READ);
    size_t size = verify ? verify.size() : 0;
    if (verify)
        verify.close();
    return size;
}

bool ManagementServer::verify_sd_file_size(const char* path,
                                           uint32_t expected, char* error,
                                           size_t error_len) {
    size_t committed = 0;
    for (uint8_t attempt = 0; attempt < 10; attempt++) {
        committed = sd_file_size(path);
        if (committed == (size_t)expected)
            return true;
        delay(10);
    }
    if (error && error_len > 0) {
        snprintf(error, error_len, "size mismatch after write: %lu/%lu bytes",
                 (unsigned long)committed, (unsigned long)expected);
    }
    return false;
}
