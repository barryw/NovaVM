#include "sd_http_server.h"
#include <ArduinoJson.h>
#include <stdarg.h>
#include <string.h>

extern void logLn(const char* fmt, ...);
extern String g_sd_diag;
extern bool g_sd_mounted;
extern const char* novaBootPhaseName();
extern bool novaFpgaBridgeAvailable();
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
extern bool novaStorageBusy();
extern bool novaBeginStorageMutation();
extern void novaEndStorageMutation();
extern void novaReleaseIdleAudioCacheForStorage();
extern void novaAudioStatusJson(char* out, size_t out_len);
extern void novaAudioStop();
extern void novaWifiStateChanged();

static bool persistDriveMountConfig(const char* prefix, const char* sd_path) {
    if (!prefix || prefix[0] == 0)
        return false;

    JsonDocument doc;
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (cfg) {
        DeserializationError err = deserializeJson(doc, cfg);
        cfg.close();
        if (err) {
            logLn("[sdhttp] boot config parse failed while persisting mount: %s",
                  err.c_str());
            doc.clear();
        }
    }

    JsonObject mounts = doc["mounts"].as<JsonObject>();
    if (mounts.isNull())
        mounts = doc["mounts"].to<JsonObject>();
    mounts[prefix] = sd_path ? sd_path : "";

    if (!SD.exists("/config") && !SD.mkdir("/config"))
        return false;

    const char* tmp_path = "/config/boot.json.tmp";
    if (SD.exists(tmp_path) && !SD.remove(tmp_path))
        return false;

    File out = SD.open(tmp_path, FILE_WRITE, true);
    if (!out)
        return false;

    serializeJsonPretty(doc, out);
    out.println();
    out.close();

    if (SD.exists("/config/boot.json") && !SD.remove("/config/boot.json")) {
        SD.remove(tmp_path);
        return false;
    }
    if (!SD.rename(tmp_path, "/config/boot.json")) {
        SD.remove(tmp_path);
        return false;
    }
    return true;
}

static bool loadDriveMountConfig(const char* prefix, char* out,
                                 size_t out_len) {
    if (!prefix || prefix[0] == 0 || !out || out_len == 0)
        return false;
    out[0] = 0;

    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg)
        return false;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, cfg);
    cfg.close();
    if (err) {
        logLn("[sdhttp] boot config parse failed while reading mount: %s",
              err.c_str());
        return false;
    }

    JsonVariantConst value = doc["mounts"][prefix];
    if (value.isNull())
        return false;

    const char* path = value.as<const char*>();
    if (!path || path[0] == 0)
        return false;

    strncpy(out, path, out_len);
    out[out_len - 1] = 0;
    return true;
}

static String normalizedSdPath(const char* path) {
    String value(path ? path : "");
    value.trim();
    value.toLowerCase();
    while (value.startsWith("/"))
        value.remove(0, 1);
    return value;
}

class StorageMutationGuard {
public:
    StorageMutationGuard() : _locked(novaBeginStorageMutation()) {}
    ~StorageMutationGuard() {
        if (_locked)
            novaEndStorageMutation();
    }
    bool locked() const { return _locked; }

private:
    bool _locked;
};

static size_t sdFileSize(const char* path) {
    File verify = SD.open(path, FILE_READ);
    size_t size = verify ? verify.size() : 0;
    if (verify)
        verify.close();
    return size;
}

static bool verifySdFileSize(const char* path, uint32_t expected,
                             char* error, size_t error_len) {
    size_t committed_size = 0;
    for (uint8_t attempt = 0; attempt < 10; attempt++) {
        committed_size = sdFileSize(path);
        if (committed_size == (size_t)expected)
            return true;

        delay(10);
    }

    if (error && error_len > 0) {
        snprintf(error, error_len, "size mismatch after write: %lu/%lu bytes",
                 (unsigned long)committed_size,
                 (unsigned long)expected);
    }
    return false;
}

void SdHttpServer::begin() {
    if (_started) return;
    _started = true;
    _server.begin();
    Serial.println("[sdhttp] HTTP server up on port 80");
    Serial.println("[sdhttp]   GET    /health   -> NovaHost liveness");
    Serial.println("[sdhttp]   GET    /wifi    -> WiFi status/config");
    Serial.println("[sdhttp]   PUT    /wifi    -> update WiFi config JSON");
    Serial.println("[sdhttp]   POST   /wifi/connect|disconnect|reconnect|forget");
    Serial.println("[sdhttp]   GET    /wifi/scan -> nearby WiFi networks");
    Serial.println("[sdhttp]   GET    /drives  -> drive mount status");
    Serial.println("[sdhttp]   POST   /drives/slot/mount|unmount");
    Serial.println("[sdhttp]   GET    /sd-status -> SD/boot diagnostic JSON");
    Serial.println("[sdhttp]   GET    /sd/       -> list root");
    Serial.println("[sdhttp]   GET    /sd/path   -> fetch file");
    Serial.println("[sdhttp]   PUT    /sd/path   -> upload whole file");
    Serial.println("[sdhttp]   DELETE /sd/path   -> remove");
}

void SdHttpServer::loop() {
    WiFiClient client = _server.available();
    if (!client) return;

    client.setNoDelay(true);
    client.setTimeout(1000);
    handle_client(client);
    client.stop();
}

void SdHttpServer::handle_client(WiFiClient& client) {
    char line[LINE_BUF_BYTES];
    if (!read_line(client, line, sizeof(line), REQUEST_LINE_TIMEOUT_MS))
        return;

    char method[8] = {0};
    char url[256] = {0};
    char* method_end = strchr(line, ' ');
    if (!method_end) {
        send_error(client, 400, "bad request");
        return;
    }
    *method_end = 0;

    char* url_start = method_end + 1;
    while (*url_start == ' ') url_start++;
    char* url_end = strchr(url_start, ' ');
    if (url_end) *url_end = 0;
    if (line[0] == 0 || url_start[0] == 0) {
        send_error(client, 400, "bad request");
        return;
    }

    strncpy(method, line, sizeof(method));
    method[sizeof(method) - 1] = 0;
    strncpy(url, url_start, sizeof(url));
    url[sizeof(url) - 1] = 0;

    uint32_t content_len = 0;
    bool have_content_len = false;
    while (read_line(client, line, sizeof(line), HEADER_LINE_TIMEOUT_MS)) {
        if (line[0] == 0) break;
        if (strncasecmp(line, "Content-Length:", 15) == 0) {
            content_len = (uint32_t)strtoul(line + 15, nullptr, 10);
            have_content_len = true;
        }
    }

    if (strcmp(method, "GET") == 0 && strcmp(url, "/health") == 0) {
        uint8_t host_status_flags = novaHostStatusFlags();
        char body[760];
        snprintf(body, sizeof(body),
                 "{\"ok\":true,\"bootPhase\":\"%s\",\"sdMounted\":%s,"
                 "\"fpgaBridgeAvailable\":%s,"
                 "\"hostStatusFlags\":%u,\"hostStatusHex\":\"0x%02X\","
                 "\"http\":{\"taskRunning\":%s,\"taskLoopSeen\":%s,"
                 "\"serviceAgeMs\":%lu,\"fallbackCount\":%lu},"
                 "\"bridge\":{\"taskRunning\":%s,\"taskLoopSeen\":%s,"
                 "\"serviceAgeMs\":%lu,\"fallbackCount\":%lu},"
                 "\"system\":{\"taskRunning\":%s,\"taskLoopSeen\":%s,"
                 "\"serviceAgeMs\":%lu},"
                 "\"capabilities\":{\"nicDispatcher\":%s}}",
                 novaBootPhaseName(),
                 g_sd_mounted ? "true" : "false",
                 novaFpgaBridgeAvailable() ? "true" : "false",
                 (unsigned)host_status_flags,
                 (unsigned)host_status_flags,
                 novaHttpTaskRunning() ? "true" : "false",
                 novaHttpTaskLoopSeen() ? "true" : "false",
                 (unsigned long)novaHttpServiceAgeMs(),
                 (unsigned long)novaHttpFallbackCount(),
                 novaBridgeTaskRunning() ? "true" : "false",
                 novaBridgeTaskLoopSeen() ? "true" : "false",
                 (unsigned long)novaBridgeServiceAgeMs(),
                 (unsigned long)novaBridgeFallbackCount(),
                 novaSystemTaskRunning() ? "true" : "false",
                 novaSystemTaskLoopSeen() ? "true" : "false",
                 (unsigned long)novaSystemServiceAgeMs(),
                 novaNicDispatcherAvailable() ? "true" : "false");
        send_json(client, 200, body);
        return;
    }
    if (strcmp(method, "GET") == 0 && strcmp(url, "/sd-status") == 0) {
        handle_status(client);
        return;
    }
    if (strcmp(method, "GET") == 0 && strcmp(url, "/audio-status") == 0) {
        char body[2048];
        novaAudioStatusJson(body, sizeof(body));
        send_json(client, 200, body);
        return;
    }
    if (strcmp(method, "POST") == 0 && strcmp(url, "/audio-stop") == 0) {
        novaAudioStop();
        send_json(client, 200, "{\"ok\":true,\"audioStopped\":true}");
        return;
    }
    if (strcmp(method, "POST") == 0 && strcmp(url, "/reboot") == 0) {
        handle_reboot(client);
        return;
    }
    if (strncmp(url, "/wifi", 5) == 0 &&
        (url[5] == 0 || url[5] == '/')) {
        handle_wifi(client, method, url, have_content_len, content_len);
        return;
    }
    bool storage_mutation =
        (strncmp(url, "/drives", 7) == 0 &&
         (url[7] == 0 || url[7] == '/') &&
         strcmp(method, "POST") == 0) ||
        ((strncmp(url, "/sd/", 4) == 0 || strcmp(url, "/sd") == 0) &&
         (strcmp(method, "PUT") == 0 || strcmp(method, "DELETE") == 0));
    if (storage_mutation) {
        if (novaStorageBusy()) {
            send_error(client, 423, "storage busy");
            return;
        }
    }
    if (strncmp(url, "/drives", 7) == 0 &&
        (url[7] == 0 || url[7] == '/')) {
        handle_drives(client, method, url, have_content_len, content_len);
        return;
    }

    char path[256];
    if (!path_after_sd(url, path, sizeof(path))) {
        send_error(client, 404, "not found");
        return;
    }

    if (strcmp(method, "GET") == 0) {
        handle_get(client, path);
    } else if (strcmp(method, "PUT") == 0) {
        if (!have_content_len) {
            send_error(client, 411, "content-length required");
            return;
        }
        handle_put(client, path, content_len);
    } else if (strcmp(method, "DELETE") == 0) {
        handle_delete(client, path);
    } else {
        send_error(client, 405, "method not allowed");
    }
}

bool SdHttpServer::read_line(WiFiClient& client, char* out, size_t out_len,
                             uint32_t timeout_ms) {
    size_t n = 0;
    uint32_t deadline = millis() + timeout_ms;
    while (millis() < deadline && client.connected()) {
        while (client.available()) {
            char c = (char)client.read();
            if (c == '\r') continue;
            if (c == '\n') {
                out[n] = 0;
                return true;
            }
            if (n + 1 < out_len) out[n++] = c;
        }
        delay(1);
    }

    out[n] = 0;
    return false;
}

bool SdHttpServer::read_body_to_file(WiFiClient& client, const char* path,
                                     uint32_t content_len,
                                     uint32_t& bytes_written,
                                     char* error, size_t error_len) {
    uint8_t buf[IO_BUF_BYTES];
    uint32_t remaining = content_len;
    uint32_t deadline = millis() + 10000;
    uint32_t checkpoint_start = 0;
    size_t checkpoint_len = 0;
    static constexpr uint32_t CHECKPOINT_BYTES = 32768UL;
    static uint8_t checkpoint_buf[CHECKPOINT_BYTES];

    bytes_written = 0;
    if (error && error_len > 0)
        error[0] = 0;

    File file = SD.open(path, "wb", true);
    if (!file) {
        if (error && error_len > 0)
            snprintf(error, error_len, "open failed");
        return false;
    }
    file.setBufferSize(4096);
    if (remaining == 0) {
        file.close();
        return verifySdFileSize(path, 0, error, error_len);
    }

    auto reopen_at = [&](uint32_t offset, const char* action) -> bool {
        file = SD.open(path, "r+b");
        if (!file) {
            if (error && error_len > 0)
                snprintf(error, error_len,
                         "%s reopen failed at %lu/%lu bytes",
                         action,
                         (unsigned long)offset,
                         (unsigned long)content_len);
            return false;
        }
        file.setBufferSize(4096);
        if (!file.seek(offset)) {
            file.close();
            if (error && error_len > 0)
                snprintf(error, error_len,
                         "%s seek failed at %lu/%lu bytes",
                         action,
                         (unsigned long)offset,
                         (unsigned long)content_len);
            return false;
        }
        return true;
    };

    auto commit_checkpoint = [&](bool reopen_after) -> bool {
        uint32_t expected = bytes_written;
        file.flush();
        file.close();

        if (!verifySdFileSize(path, expected, nullptr, 0)) {
            size_t committed_size = sdFileSize(path);
            if (committed_size < (size_t)checkpoint_start ||
                committed_size > (size_t)expected) {
                if (error && error_len > 0)
                    snprintf(error, error_len,
                             "size mismatch after write: %lu/%lu bytes",
                             (unsigned long)committed_size,
                             (unsigned long)expected);
                return false;
            }

            size_t replay_offset = committed_size - (size_t)checkpoint_start;
            size_t replay_len = (size_t)expected - committed_size;
            if (replay_len > checkpoint_len || replay_offset > checkpoint_len) {
                if (error && error_len > 0)
                    snprintf(error, error_len,
                             "checkpoint replay out of range: %lu/%lu bytes",
                             (unsigned long)committed_size,
                             (unsigned long)expected);
                return false;
            }

            if (replay_len > 0) {
                if (!reopen_at((uint32_t)committed_size, "repair"))
                    return false;
                uint32_t replay_written = (uint32_t)committed_size;
                if (!write_file_all(file,
                                    checkpoint_buf + replay_offset,
                                    replay_len,
                                    replay_written,
                                    error,
                                    error_len)) {
                    file.close();
                    return false;
                }
                file.flush();
                file.close();
                if (!verifySdFileSize(path, expected, error, error_len))
                    return false;
            }
        }

        checkpoint_start = expected;
        checkpoint_len = 0;
        if (reopen_after)
            return reopen_at(expected, "append");
        return true;
    };

    while (remaining > 0) {
        if (!client.connected() && !client.available()) {
            file.close();
            if (error && error_len > 0)
                snprintf(error, error_len, "client closed at %lu/%lu bytes",
                         (unsigned long)bytes_written,
                         (unsigned long)content_len);
            return false;
        }
        if (!client.available()) {
            if (millis() > deadline) {
                file.close();
                if (error && error_len > 0)
                    snprintf(error, error_len,
                             "body timeout at %lu/%lu bytes",
                             (unsigned long)bytes_written,
                             (unsigned long)content_len);
                return false;
            }
            delay(1);
            continue;
        }

        size_t want = remaining > sizeof(buf) ? sizeof(buf) : remaining;
        int read_count = client.read(buf, want);
        if (read_count < 0) {
            file.close();
            if (error && error_len > 0)
                snprintf(error, error_len, "client read failed at %lu/%lu bytes",
                         (unsigned long)bytes_written,
                         (unsigned long)content_len);
            return false;
        }
        size_t got = (size_t)read_count;
        if (got == 0) continue;
        deadline = millis() + 10000;

        if (checkpoint_len > 0 &&
            checkpoint_len + got > CHECKPOINT_BYTES) {
            if (!commit_checkpoint(true))
                return false;
        }

        memcpy(checkpoint_buf + checkpoint_len, buf, got);
        if (!write_file_all(file, buf, got, bytes_written, error, error_len)) {
            file.close();
            return false;
        }
        checkpoint_len += got;
        remaining -= (uint32_t)got;

        if (checkpoint_len >= CHECKPOINT_BYTES || remaining == 0) {
            if (!commit_checkpoint(remaining > 0))
                return false;
        }
        yield();
    }

    return true;
}

bool SdHttpServer::write_file_all(File& file, const uint8_t* data, size_t len,
                                  uint32_t& bytes_written, char* error,
                                  size_t error_len) {
    size_t off = 0;
    uint32_t deadline = millis() + 5000;

    while (off < len) {
        size_t wrote = file.write(data + off, len - off);
        if (wrote > 0) {
            off += wrote;
            bytes_written += (uint32_t)wrote;
            deadline = millis() + 5000;
            yield();
            continue;
        }

        if (millis() > deadline) {
            if (error && error_len > 0)
                snprintf(error, error_len, "sd write stalled at %lu bytes",
                         (unsigned long)bytes_written);
            return false;
        }
        delay(1);
    }

    return true;
}

bool SdHttpServer::read_body_to_string(WiFiClient& client, String& out,
                                       uint32_t content_len) {
    if (content_len > JSON_BUF_BYTES)
        return false;

    out = "";
    out.reserve(content_len + 1);
    uint32_t remaining = content_len;
    uint32_t deadline = millis() + 5000;

    while (remaining > 0) {
        if (!client.connected() && !client.available()) return false;
        if (!client.available()) {
            if (millis() > deadline) return false;
            delay(1);
            continue;
        }

        out += (char)client.read();
        remaining--;
        deadline = millis() + 5000;
    }

    return true;
}

bool SdHttpServer::write_all(WiFiClient& client, const uint8_t* data,
                             size_t len, bool allow_probe_write) {
    size_t off = 0;
    uint32_t deadline = millis() + RESPONSE_WRITE_TIMEOUT_MS;
    while (off < len) {
        if (!client.connected()) return false;
        size_t chunk = len - off;
        int writable = client.availableForWrite();
        if (writable > 0 && chunk > (size_t)writable)
            chunk = (size_t)writable;
        else if (writable <= 0) {
            if (!allow_probe_write) {
                if (millis() > deadline) {
                    client.stop();
                    return false;
                }
                delay(1);
                continue;
            }
            if (chunk > 64)
                chunk = 64;
        }
        if (chunk > IO_BUF_BYTES)
            chunk = IO_BUF_BYTES;

        size_t wrote = client.write(data + off, chunk);
        if (wrote == 0) {
            if (millis() > deadline) {
                client.stop();
                return false;
            }
            delay(1);
            continue;
        }
        off += wrote;
        yield();
    }
    return true;
}

bool SdHttpServer::write_str(WiFiClient& client, const char* value) {
    if (!value)
        return true;
    return write_all(client, (const uint8_t*)value, strlen(value));
}

bool SdHttpServer::write_char(WiFiClient& client, char value) {
    return write_all(client, (const uint8_t*)&value, 1);
}

bool SdHttpServer::write_fmt(WiFiClient& client, const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return write_str(client, buf);
}

void SdHttpServer::handle_wifi(WiFiClient& client, const char* method,
                               const char* url, bool have_content_len,
                               uint32_t content_len) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/wifi") == 0) {
        handle_wifi_status(client);
        return;
    }
    if (strcmp(method, "GET") == 0 && strcmp(url, "/wifi/scan") == 0) {
        handle_wifi_scan(client);
        return;
    }
    if (strcmp(method, "PUT") == 0 && strcmp(url, "/wifi") == 0) {
        if (!have_content_len) {
            send_error(client, 411, "content-length required");
            return;
        }
        handle_wifi_put(client, content_len);
        return;
    }
    if (strcmp(method, "POST") == 0 && strncmp(url, "/wifi/", 6) == 0) {
        handle_wifi_action(client, url + 6);
        return;
    }

    send_error(client, 404, "not found");
}

void SdHttpServer::handle_wifi_status(WiFiClient& client) {
    NovaWifiManager::Status st = _wifi.status();

    send_headers(client, 200, "application/json");
    write_str(client, "{\"configured\":");
    write_str(client, st.configured ? "true" : "false");
    write_str(client, ",\"connected\":");
    write_str(client, st.connected ? "true" : "false");
    write_str(client, ",\"wantConnected\":");
    write_str(client, st.wantConnected ? "true" : "false");
    write_str(client, ",\"ssid\":");
    write_json_string(client, st.ssid.c_str());
    write_str(client, ",\"passwordSet\":");
    write_str(client, st.passwordSet ? "true" : "false");
    write_str(client, ",\"useStatic\":");
    write_str(client, st.useStatic ? "true" : "false");
    write_str(client, ",\"staticIp\":");
    write_json_string(client, st.staticIp.c_str());
    write_str(client, ",\"gateway\":");
    write_json_string(client, st.gateway.c_str());
    write_str(client, ",\"subnet\":");
    write_json_string(client, st.subnet.c_str());
    write_str(client, ",\"dns\":");
    write_json_string(client, st.dns.c_str());
    write_str(client, ",\"localIp\":");
    write_json_string(client, st.localIp.c_str());
    write_str(client, ",\"localGateway\":");
    write_json_string(client, st.localGateway.c_str());
    write_str(client, ",\"localSubnet\":");
    write_json_string(client, st.localSubnet.c_str());
    write_str(client, ",\"localDns\":");
    write_json_string(client, st.localDns.c_str());
    write_str(client, ",\"mac\":");
    write_json_string(client, st.mac.c_str());
    write_fmt(client, ",\"rssi\":%ld,\"wifiStatus\":", (long)st.rssi);
    write_json_string(client, NovaWifiManager::statusName(st.wifiStatus));
    write_str(client, ",\"lastError\":");
    write_json_string(client, st.lastError.c_str());
    write_char(client, '}');
}

void SdHttpServer::handle_wifi_scan(WiFiClient& client) {
    int count = WiFi.scanNetworks();

    send_headers(client, 200, "application/json");
    write_char(client, '[');
    for (int i = 0; i < count; i++) {
        if (i > 0) write_char(client, ',');
        write_str(client, "{\"ssid\":");
        write_json_string(client, WiFi.SSID(i).c_str());
        write_fmt(client,
                  ",\"rssi\":%ld,\"channel\":%d,\"encrypted\":%s}",
                  (long)WiFi.RSSI(i),
                  WiFi.channel(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true");
    }
    write_char(client, ']');
    WiFi.scanDelete();
}

void SdHttpServer::handle_wifi_put(WiFiClient& client, uint32_t content_len) {
    String body;
    if (!read_body_to_string(client, body, content_len)) {
        send_error(client, 400, "bad body");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        send_error(client, 400, "bad json");
        return;
    }

    NovaWifiManager::Config cfg = _wifi.config();
    JsonVariantConst v;

    v = doc["ssid"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.ssid = s ? String(s) : String();
    }

    v = doc["password"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.password = s ? String(s) : String();
    }

    v = doc["dhcp"];
    if (!v.isNull())
        cfg.useStatic = !v.as<bool>();

    v = doc["useStatic"];
    if (!v.isNull())
        cfg.useStatic = v.as<bool>();

    v = doc["staticIp"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.staticIp = s ? String(s) : String();
    }

    v = doc["gateway"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.gateway = s ? String(s) : String();
    }

    v = doc["subnet"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.subnet = s ? String(s) : String();
    }

    v = doc["dns"];
    if (!v.isNull()) {
        const char* s = v.as<const char*>();
        cfg.dns = s ? String(s) : String();
    }

    String save_error;
    if (!_wifi.saveConfig(cfg, &save_error)) {
        send_error(client, 400, save_error.c_str());
        return;
    }

    novaWifiStateChanged();
    send_json(client, 200, "{\"ok\":true}");
}

void SdHttpServer::handle_wifi_action(WiFiClient& client, const char* action) {
    if (strcmp(action, "connect") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        delay(100);
        _wifi.connect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "disconnect") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        delay(100);
        _wifi.disconnect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "reconnect") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        delay(100);
        _wifi.reconnect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "forget") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        delay(100);
        _wifi.forget();
        novaWifiStateChanged();
        return;
    }

    send_error(client, 404, "not found");
}

void SdHttpServer::handle_drives(WiFiClient& client, const char* method,
                                 const char* url, bool have_content_len,
                                 uint32_t content_len) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/drives") == 0) {
        handle_drives_list(client);
        return;
    }

    char slot_name[8];
    char action[16];
    if (!parse_drive_action(url, slot_name, sizeof(slot_name),
                            action, sizeof(action))) {
        send_error(client, 404, "not found");
        return;
    }

    int slot = DeviceManager::slot_for_prefix(slot_name);
    if (slot < 0) {
        send_error(client, 400, "bad slot");
        return;
    }

    if (strcmp(method, "POST") != 0) {
        send_error(client, 405, "method not allowed");
        return;
    }
    if (strcmp(action, "mount") == 0) {
        handle_drive_mount(client, slot, have_content_len, content_len);
        return;
    }
    if (strcmp(action, "unmount") == 0) {
        handle_drive_unmount(client, slot);
        return;
    }

    send_error(client, 404, "not found");
}

void SdHttpServer::handle_drives_list(WiFiClient& client) {
    static const int list_order[] = {
        DeviceManager::FD0, DeviceManager::FD1,
        DeviceManager::FD2, DeviceManager::FD3,
        DeviceManager::HD0, DeviceManager::HD1
    };

    send_headers(client, 200, "application/json");
    write_char(client, '[');
    for (int i = 0; i < DeviceManager::NUM_SLOTS; i++) {
        int slot = list_order[i];
        const char* prefix = DeviceManager::prefix_for_slot(slot);
        char configured_path[300];

        if (i > 0) write_char(client, ',');
        write_str(client, "{\"slot\":");
        write_json_string(client, prefix);
        write_str(client, ",\"mounted\":");
        write_str(client, _dm.is_mounted(slot) ? "true" : "false");
        write_str(client, ",\"currentPath\":");
        write_json_string(client, _dm.current_path(slot));
        write_str(client, ",\"configuredPath\":");
        if (loadDriveMountConfig(prefix, configured_path,
                                 sizeof(configured_path)))
            write_json_string(client, configured_path);
        else
            write_json_string(client, "");
        write_char(client, '}');
    }
    write_char(client, ']');
}

void SdHttpServer::handle_drive_mount(WiFiClient& client, int slot,
                                      bool have_content_len,
                                      uint32_t content_len) {
    if (!g_sd_mounted) {
        send_error(client, 503, "sd not mounted");
        return;
    }

    const char* prefix = DeviceManager::prefix_for_slot(slot);
    char sd_path[300] = {0};
    bool have_path = false;

    if (have_content_len && content_len > 0) {
        String body;
        if (!read_body_to_string(client, body, content_len)) {
            send_error(client, 400, "bad body");
            return;
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        if (err) {
            send_error(client, 400, "bad json");
            return;
        }

        JsonVariantConst v = doc["path"];
        const char* path = v.as<const char*>();
        if (!path || path[0] == 0) {
            send_error(client, 400, "bad path");
            return;
        }
        if (path[0] == '/') {
            strncpy(sd_path, path, sizeof(sd_path));
            sd_path[sizeof(sd_path) - 1] = 0;
        } else {
            snprintf(sd_path, sizeof(sd_path), "/%s", path);
        }
        have_path = true;
    } else if (loadDriveMountConfig(prefix, sd_path, sizeof(sd_path))) {
        have_path = true;
    }

    if (!have_path || sd_path[0] == 0) {
        send_error(client, 400, "missing path");
        return;
    }
    if (sd_path[0] != '/') {
        char normalized[300];
        snprintf(normalized, sizeof(normalized), "/%s", sd_path);
        strncpy(sd_path, normalized, sizeof(sd_path));
        sd_path[sizeof(sd_path) - 1] = 0;
    }

    if (!path_sane(sd_path + 1, false)) {
        send_error(client, 400, "bad path");
        return;
    }
    String lower(sd_path);
    lower.toLowerCase();
    if (!lower.endsWith(".ndi")) {
        send_error(client, 400, "path must end in .ndi");
        return;
    }

    File entry = SD.open(sd_path, FILE_READ);
    if (!entry) {
        send_error(client, 404, "not found");
        return;
    }
    entry.close();

    novaReleaseIdleAudioCacheForStorage();
    if (!_dm.mount(slot, sd_path)) {
        send_error(client, 500, _dm.last_mount_error());
        return;
    }

    if (!persistDriveMountConfig(prefix, sd_path))
        logLn("[sdhttp] WARN: failed to persist mount config for %s", prefix);

    logLn("[sdhttp] MOUNT %s -> %s", prefix, sd_path);
    send_headers(client, 200, "application/json");
    write_str(client, "{\"ok\":true,\"slot\":");
    write_json_string(client, prefix);
    write_str(client, ",\"mounted\":true,\"path\":");
    write_json_string(client, sd_path);
    write_char(client, '}');
}

void SdHttpServer::handle_drive_unmount(WiFiClient& client, int slot) {
    const char* prefix = DeviceManager::prefix_for_slot(slot);
    _dm.unmount(slot);

    if (!persistDriveMountConfig(prefix, ""))
        logLn("[sdhttp] WARN: failed to persist unmount config for %s", prefix);

    logLn("[sdhttp] UNMOUNT %s", prefix);
    send_headers(client, 200, "application/json");
    write_str(client, "{\"ok\":true,\"slot\":");
    write_json_string(client, prefix);
    write_str(client, ",\"mounted\":false}");
}

void SdHttpServer::handle_get(WiFiClient& client, const char* path) {
    if (!g_sd_mounted) {
        send_error(client, 503, "sd not mounted");
        return;
    }
    if (!path_sane(path, true)) {
        send_error(client, 400, "bad path");
        return;
    }

    char full[300];
    snprintf(full, sizeof(full), "/%s", path);

    File entry = SD.open(full);
    if (!entry) {
        send_error(client, 404, "not found");
        return;
    }

    bool is_dir = entry.isDirectory();
    entry.close();
    if (is_dir) {
        send_listing(client, path);
    } else {
        send_file(client, full);
    }
}

void SdHttpServer::handle_put(WiFiClient& client, const char* path,
                              uint32_t content_len) {
    if (!g_sd_mounted) {
        send_error(client, 503, "sd not mounted");
        return;
    }
    if (!path_sane(path, false)) {
        send_error(client, 400, "bad path");
        return;
    }

    StorageMutationGuard storage_guard;
    if (!storage_guard.locked()) {
        send_error(client, 423, "storage busy");
        return;
    }

    if (!path_safe_for_write(path)) {
        send_error(client, 409, "file is mounted; UNMOUNT first");
        return;
    }

    char full[300];
    snprintf(full, sizeof(full), "/%s", path);
    if (!ensure_parent_dirs(full)) {
        send_error(client, 500, "mkdir failed");
        return;
    }

    char tmp[320];
    if (snprintf(tmp, sizeof(tmp), "%s.tmp", full) >= (int)sizeof(tmp)) {
        send_error(client, 414, "path too long");
        return;
    }

    if (SD.exists(tmp) && !SD.remove(tmp)) {
        send_error(client, 500, "temp remove failed");
        return;
    }

    novaReleaseIdleAudioCacheForStorage();

    uint32_t bytes_written = 0;
    char write_error[96];
    bool ok = read_body_to_file(client, tmp, content_len, bytes_written,
                                write_error, sizeof(write_error));
    if (!ok) {
        logLn("[sdhttp] PUT %s failed after %lu/%lu bytes: %s",
              full,
              (unsigned long)bytes_written,
              (unsigned long)content_len,
              write_error[0] ? write_error : "write failed");
        SD.remove(tmp);
        send_error(client, 500, write_error[0] ? write_error : "write failed");
        return;
    }

    if (SD.exists(full) && !SD.remove(full)) {
        SD.remove(tmp);
        send_error(client, 500, "replace remove failed");
        return;
    }

    if (!SD.rename(tmp, full)) {
        SD.remove(tmp);
        send_error(client, 500, "replace rename failed");
        return;
    }

    logLn("[sdhttp] PUT %s (%u bytes)", full, (unsigned)content_len);
    send_json(client, 200, "{\"ok\":true}");
}

void SdHttpServer::handle_delete(WiFiClient& client, const char* path) {
    if (!g_sd_mounted) {
        send_error(client, 503, "sd not mounted");
        return;
    }
    if (!path_sane(path, false)) {
        send_error(client, 400, "bad path");
        return;
    }
    if (!path_safe_for_write(path)) {
        send_error(client, 409, "file is mounted; UNMOUNT first");
        return;
    }

    StorageMutationGuard storage_guard;
    if (!storage_guard.locked()) {
        send_error(client, 423, "storage busy");
        return;
    }

    char full[300];
    snprintf(full, sizeof(full), "/%s", path);
    File entry = SD.open(full);
    if (!entry) {
        send_error(client, 404, "not found");
        return;
    }

    bool is_dir = entry.isDirectory();
    entry.close();
    bool ok = is_dir ? SD.rmdir(full) : SD.remove(full);
    if (!ok) {
        send_error(client, 500, "delete failed");
        return;
    }

    logLn("[sdhttp] DELETE %s", full);
    send_json(client, 200, "{\"ok\":true}");
}

void SdHttpServer::handle_status(WiFiClient& client) {
    uint8_t ct = g_sd_mounted ? SD.cardType() : CARD_NONE;
    uint64_t card_sz = g_sd_mounted ? SD.cardSize() : 0;
    uint64_t total_sz = g_sd_mounted ? SD.totalBytes() : 0;
    uint64_t used_sz = g_sd_mounted ? SD.usedBytes() : 0;
    const char* type_name =
        ct == CARD_NONE ? "NONE" :
        ct == CARD_MMC  ? "MMC"  :
        ct == CARD_SD   ? "SDSC" :
        ct == CARD_SDHC ? "SDHC" :
        "UNKNOWN";

    send_headers(client, 200, "application/json");
    write_str(client, "{\"cardType\":");
    write_json_string(client, type_name);
    write_fmt(client, ",\"cardTypeId\":%u,\"mounted\":%s,\"bootPhase\":",
              (unsigned)ct,
              g_sd_mounted ? "true" : "false");
    write_json_string(client, novaBootPhaseName());
    write_fmt(client,
              ",\"cardSize\":%llu,\"totalBytes\":%llu,\"usedBytes\":%llu,"
              "\"bootDiag\":",
              (unsigned long long)card_sz,
              (unsigned long long)total_sz,
              (unsigned long long)used_sz);
    write_json_string(client, g_sd_diag.c_str());
    write_char(client, '}');
}

void SdHttpServer::handle_reboot(WiFiClient& client) {
    send_json(client, 200, "{\"ok\":true,\"rebooting\":true}");
    delay(100);
    ESP.restart();
}

void SdHttpServer::send_listing(WiFiClient& client, const char* path) {
    char full[300];
    snprintf(full, sizeof(full), "/%s", path);

    File dir = SD.open(full);
    if (!dir || !dir.isDirectory()) {
        if (dir) dir.close();
        send_error(client, 404, "not a directory");
        return;
    }

    send_headers(client, 200, "application/json");
    write_char(client, '[');
    bool first = true;
    File entry;
    while ((entry = dir.openNextFile())) {
        if (!first) write_char(client, ',');
        first = false;
        write_str(client, "{\"name\":");
        write_json_string(client, entry.name());
        write_fmt(client, ",\"size\":%lu",
                  (unsigned long)entry.size());
        write_str(client, ",\"dir\":");
        write_str(client, entry.isDirectory() ? "true" : "false");
        write_char(client, '}');
        entry.close();
    }
    write_char(client, ']');
    dir.close();
}

void SdHttpServer::send_file(WiFiClient& client, const char* path) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        send_error(client, 404, "not found");
        return;
    }

    send_headers(client, 200, "application/octet-stream",
                 (int32_t)file.size());
    uint8_t buf[IO_BUF_BYTES];
    while (file.available()) {
        size_t got = file.read(buf, sizeof(buf));
        if (got == 0) break;
        if (!write_all(client, buf, got, true)) break;
        yield();
    }
    file.close();
}

void SdHttpServer::send_json(WiFiClient& client, int code, const char* body) {
    send_headers(client, code, "application/json", (int32_t)strlen(body));
    write_str(client, body);
}

void SdHttpServer::send_error(WiFiClient& client, int code,
                              const char* message) {
    char body[160];
    snprintf(body, sizeof(body), "{\"error\":\"%s\"}", message);
    send_json(client, code, body);
}

void SdHttpServer::send_headers(WiFiClient& client, int code,
                                const char* content_type,
                                int32_t content_len) {
    write_fmt(client, "HTTP/1.1 %d %s\r\n", code, reason_phrase(code));
    write_fmt(client, "Content-Type: %s\r\n", content_type);
    if (content_len >= 0)
        write_fmt(client, "Content-Length: %ld\r\n", (long)content_len);
    write_str(client, "Connection: close\r\n\r\n");
}

void SdHttpServer::write_json_string(WiFiClient& client, const char* value) {
    static const char hex[] = "0123456789ABCDEF";
    char out[96];
    size_t n = 0;
    auto flush = [&]() {
        if (n == 0)
            return;
        write_all(client, (const uint8_t*)out, n);
        n = 0;
    };
    auto append = [&](char ch) {
        if (n >= sizeof(out))
            flush();
        out[n++] = ch;
    };
    auto append_str = [&](const char* text) {
        while (*text)
            append(*text++);
    };

    append('"');
    if (value) {
        for (const unsigned char* p = (const unsigned char*)value; *p; p++) {
            switch (*p) {
                case '"':  append_str("\\\""); break;
                case '\\': append_str("\\\\"); break;
                case '\b': append_str("\\b");  break;
                case '\f': append_str("\\f");  break;
                case '\n': append_str("\\n");  break;
                case '\r': append_str("\\r");  break;
                case '\t': append_str("\\t");  break;
                default:
                    if (*p < 0x20) {
                        append_str("\\u00");
                        append(hex[*p >> 4]);
                        append(hex[*p & 0x0F]);
                    } else {
                        append((char)*p);
                    }
                    break;
            }
        }
    }
    append('"');
    flush();
}

bool SdHttpServer::path_sane(const char* path, bool allow_empty) {
    if (!path) return false;
    if (!allow_empty && path[0] == 0) return false;
    if (path[0] == '/') return false;
    if (strstr(path, "..")) return false;
    if (strchr(path, '\\')) return false;
    return true;
}

bool SdHttpServer::path_safe_for_write(const char* path) {
    String lower = normalizedSdPath(path);
    if (!lower.endsWith(".ndi")) return true;

    for (int s = 0; s < DeviceManager::NUM_SLOTS; s++) {
        if (!_dm.is_mounted(s)) continue;
        const char* prefix = DeviceManager::prefix_for_slot(s);
        if (!prefix) continue;

        char configured_path[300];
        if (loadDriveMountConfig(prefix, configured_path,
                                 sizeof(configured_path)) &&
            lower == normalizedSdPath(configured_path))
            return false;

        String needle = String(prefix) + ".ndi";
        if (lower.endsWith(needle)) return false;
    }
    return true;
}

bool SdHttpServer::ensure_parent_dirs(const char* full_path) {
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

const char* SdHttpServer::path_after_sd(const char* url, char* out,
                                        size_t out_len) {
    if (!url || out_len == 0) return nullptr;

    const char* src = nullptr;
    if (strcmp(url, "/sd") == 0) {
        src = "";
    } else if (strncmp(url, "/sd/", 4) == 0) {
        src = url + 4;
    } else {
        return nullptr;
    }

    size_t n = 0;
    while (src[n] && src[n] != '?' && n + 1 < out_len) {
        out[n] = src[n];
        n++;
    }
    out[n] = 0;
    return out;
}

bool SdHttpServer::parse_drive_action(const char* url, char* slot,
                                      size_t slot_len, char* action,
                                      size_t action_len) {
    if (!url || strncmp(url, "/drives/", 8) != 0 ||
        slot_len == 0 || action_len == 0) {
        return false;
    }

    const char* p = url + 8;
    size_t n = 0;
    while (p[n] && p[n] != '/' && p[n] != '?' && n + 1 < slot_len) {
        slot[n] = p[n];
        n++;
    }
    slot[n] = 0;
    if (n == 0 || p[n] != '/') return false;

    p += n + 1;
    n = 0;
    while (p[n] && p[n] != '/' && p[n] != '?' && n + 1 < action_len) {
        action[n] = p[n];
        n++;
    }
    action[n] = 0;
    if (n == 0) return false;
    return p[n] == 0 || p[n] == '?';
}

const char* SdHttpServer::reason_phrase(int code) {
    switch (code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 409: return "Conflict";
        case 423: return "Locked";
        case 411: return "Length Required";
        case 503: return "Service Unavailable";
        case 500: return "Internal Server Error";
        default:  return "OK";
    }
}
