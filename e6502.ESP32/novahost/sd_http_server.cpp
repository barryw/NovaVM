#include "sd_http_server.h"
#include <ArduinoJson.h>
#include <string.h>

extern void logLn(const char* fmt, ...);
extern String g_sd_diag;
extern bool g_sd_mounted;
extern const char* novaBootPhaseName();
extern bool novaFpgaBridgeAvailable();
extern uint8_t novaHostStatusFlags();
extern void novaWifiStateChanged();

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

    handle_client(client);
    client.stop();
}

void SdHttpServer::handle_client(WiFiClient& client) {
    char line[LINE_BUF_BYTES];
    if (!read_line(client, line, sizeof(line))) return;

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
    while (read_line(client, line, sizeof(line))) {
        if (line[0] == 0) break;
        if (strncasecmp(line, "Content-Length:", 15) == 0) {
            content_len = (uint32_t)strtoul(line + 15, nullptr, 10);
            have_content_len = true;
        }
    }

    if (strcmp(method, "GET") == 0 && strcmp(url, "/health") == 0) {
        uint8_t host_status_flags = novaHostStatusFlags();
        char body[256];
        snprintf(body, sizeof(body),
                 "{\"ok\":true,\"bootPhase\":\"%s\",\"sdMounted\":%s,"
                 "\"fpgaBridgeAvailable\":%s,"
                 "\"hostStatusFlags\":%u,\"hostStatusHex\":\"0x%02X\"}",
                 novaBootPhaseName(),
                 g_sd_mounted ? "true" : "false",
                 novaFpgaBridgeAvailable() ? "true" : "false",
                 (unsigned)host_status_flags,
                 (unsigned)host_status_flags);
        send_json(client, 200, body);
        return;
    }
    if (strcmp(method, "GET") == 0 && strcmp(url, "/sd-status") == 0) {
        handle_status(client);
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
    return n > 0;
}

bool SdHttpServer::read_body_to_file(WiFiClient& client, File& file,
                                     uint32_t content_len) {
    uint8_t buf[IO_BUF_BYTES];
    uint32_t remaining = content_len;
    uint32_t deadline = millis() + 10000;

    while (remaining > 0) {
        if (!client.connected() && !client.available()) return false;
        if (!client.available()) {
            if (millis() > deadline) return false;
            delay(1);
            continue;
        }

        size_t want = remaining > sizeof(buf) ? sizeof(buf) : remaining;
        size_t got = client.read(buf, want);
        if (got == 0) continue;
        deadline = millis() + 10000;

        size_t wrote = file.write(buf, got);
        if (wrote != got) return false;
        remaining -= (uint32_t)got;
        yield();
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
                             size_t len) {
    size_t off = 0;
    uint32_t deadline = millis() + 10000;
    while (off < len) {
        if (!client.connected()) return false;
        size_t wrote = client.write(data + off, len - off);
        if (wrote == 0) {
            if (millis() > deadline) return false;
            delay(1);
            continue;
        }
        deadline = millis() + 10000;
        off += wrote;
        yield();
    }
    return true;
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
    client.print("{\"configured\":");
    client.print(st.configured ? "true" : "false");
    client.print(",\"connected\":");
    client.print(st.connected ? "true" : "false");
    client.print(",\"wantConnected\":");
    client.print(st.wantConnected ? "true" : "false");
    client.print(",\"ssid\":");
    write_json_string(client, st.ssid.c_str());
    client.print(",\"passwordSet\":");
    client.print(st.passwordSet ? "true" : "false");
    client.print(",\"useStatic\":");
    client.print(st.useStatic ? "true" : "false");
    client.print(",\"staticIp\":");
    write_json_string(client, st.staticIp.c_str());
    client.print(",\"gateway\":");
    write_json_string(client, st.gateway.c_str());
    client.print(",\"subnet\":");
    write_json_string(client, st.subnet.c_str());
    client.print(",\"dns\":");
    write_json_string(client, st.dns.c_str());
    client.print(",\"localIp\":");
    write_json_string(client, st.localIp.c_str());
    client.print(",\"localGateway\":");
    write_json_string(client, st.localGateway.c_str());
    client.print(",\"localSubnet\":");
    write_json_string(client, st.localSubnet.c_str());
    client.print(",\"localDns\":");
    write_json_string(client, st.localDns.c_str());
    client.print(",\"mac\":");
    write_json_string(client, st.mac.c_str());
    client.printf(",\"rssi\":%ld,\"wifiStatus\":", (long)st.rssi);
    write_json_string(client, NovaWifiManager::statusName(st.wifiStatus));
    client.print(",\"lastError\":");
    write_json_string(client, st.lastError.c_str());
    client.print('}');
}

void SdHttpServer::handle_wifi_scan(WiFiClient& client) {
    int count = WiFi.scanNetworks();

    send_headers(client, 200, "application/json");
    client.print('[');
    for (int i = 0; i < count; i++) {
        if (i > 0) client.print(',');
        client.print("{\"ssid\":");
        write_json_string(client, WiFi.SSID(i).c_str());
        client.printf(",\"rssi\":%ld,\"channel\":%d,\"encrypted\":%s}",
                      (long)WiFi.RSSI(i),
                      WiFi.channel(i),
                      WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true");
    }
    client.print(']');
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
        client.flush();
        delay(100);
        _wifi.connect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "disconnect") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        client.flush();
        delay(100);
        _wifi.disconnect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "reconnect") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        client.flush();
        delay(100);
        _wifi.reconnect();
        novaWifiStateChanged();
        return;
    }
    if (strcmp(action, "forget") == 0) {
        send_json(client, 202, "{\"ok\":true,\"accepted\":true}");
        client.flush();
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
    send_headers(client, 200, "application/json");
    client.print('[');
    for (int slot = 0; slot < DeviceManager::NUM_SLOTS; slot++) {
        if (slot > 0) client.print(',');
        client.print("{\"slot\":");
        write_json_string(client, DeviceManager::prefix_for_slot(slot));
        client.print(",\"mounted\":");
        client.print(_dm.is_mounted(slot) ? "true" : "false");
        client.print(",\"currentPath\":");
        write_json_string(client, _dm.current_path(slot));
        client.print('}');
    }
    client.print(']');
}

void SdHttpServer::handle_drive_mount(WiFiClient& client, int slot,
                                      bool have_content_len,
                                      uint32_t content_len) {
    if (!g_sd_mounted) {
        send_error(client, 503, "sd not mounted");
        return;
    }

    const char* prefix = DeviceManager::prefix_for_slot(slot);
    char sd_path[300];
    snprintf(sd_path, sizeof(sd_path), "/%s.ndi", prefix);

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
        if (!v.isNull()) {
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
        }
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

    if (!_dm.mount(slot, sd_path)) {
        send_error(client, 500, "mount failed");
        return;
    }

    logLn("[sdhttp] MOUNT %s -> %s", prefix, sd_path);
    send_headers(client, 200, "application/json");
    client.print("{\"ok\":true,\"slot\":");
    write_json_string(client, prefix);
    client.print(",\"mounted\":true,\"path\":");
    write_json_string(client, sd_path);
    client.print('}');
}

void SdHttpServer::handle_drive_unmount(WiFiClient& client, int slot) {
    const char* prefix = DeviceManager::prefix_for_slot(slot);
    _dm.unmount(slot);

    logLn("[sdhttp] UNMOUNT %s", prefix);
    send_headers(client, 200, "application/json");
    client.print("{\"ok\":true,\"slot\":");
    write_json_string(client, prefix);
    client.print(",\"mounted\":false}");
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

    File file = SD.open(full, FILE_WRITE, true);
    if (!file) {
        send_error(client, 500, "open failed");
        return;
    }

    bool ok = read_body_to_file(client, file, content_len);
    file.close();
    if (!ok) {
        send_error(client, 500, "write failed");
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
    client.print("{\"cardType\":");
    write_json_string(client, type_name);
    client.printf(",\"cardTypeId\":%u,\"mounted\":%s,\"bootPhase\":",
                  (unsigned)ct,
                  g_sd_mounted ? "true" : "false");
    write_json_string(client, novaBootPhaseName());
    client.printf(",\"cardSize\":%llu,\"totalBytes\":%llu,\"usedBytes\":%llu,"
                  "\"bootDiag\":",
                  (unsigned long long)card_sz,
                  (unsigned long long)total_sz,
                  (unsigned long long)used_sz);
    write_json_string(client, g_sd_diag.c_str());
    client.print('}');
}

void SdHttpServer::handle_reboot(WiFiClient& client) {
    send_json(client, 200, "{\"ok\":true,\"rebooting\":true}");
    client.flush();
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
    client.print('[');
    bool first = true;
    File entry;
    while ((entry = dir.openNextFile())) {
        if (!first) client.print(',');
        first = false;
        client.print("{\"name\":");
        write_json_string(client, entry.name());
        client.print(",\"size\":");
        client.print((uint32_t)entry.size());
        client.print(",\"dir\":");
        client.print(entry.isDirectory() ? "true" : "false");
        client.print('}');
        entry.close();
    }
    client.print(']');
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
        if (!write_all(client, buf, got)) break;
        yield();
    }
    file.close();
}

void SdHttpServer::send_json(WiFiClient& client, int code, const char* body) {
    send_headers(client, code, "application/json", (int32_t)strlen(body));
    client.print(body);
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
    client.printf("HTTP/1.1 %d %s\r\n", code, reason_phrase(code));
    client.printf("Content-Type: %s\r\n", content_type);
    if (content_len >= 0) client.printf("Content-Length: %ld\r\n", (long)content_len);
    client.print("Connection: close\r\n\r\n");
}

void SdHttpServer::write_json_string(WiFiClient& client, const char* value) {
    static const char hex[] = "0123456789ABCDEF";
    client.print('"');
    if (value) {
        for (const unsigned char* p = (const unsigned char*)value; *p; p++) {
            switch (*p) {
                case '"':  client.print("\\\""); break;
                case '\\': client.print("\\\\"); break;
                case '\b': client.print("\\b");  break;
                case '\f': client.print("\\f");  break;
                case '\n': client.print("\\n");  break;
                case '\r': client.print("\\r");  break;
                case '\t': client.print("\\t");  break;
                default:
                    if (*p < 0x20) {
                        client.print("\\u00");
                        client.write(hex[*p >> 4]);
                        client.write(hex[*p & 0x0F]);
                    } else {
                        client.write(*p);
                    }
                    break;
            }
        }
    }
    client.print('"');
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
    String lower(path);
    lower.toLowerCase();
    if (!lower.endsWith(".ndi")) return true;

    for (int s = 0; s < DeviceManager::NUM_SLOTS; s++) {
        if (!_dm.is_mounted(s)) continue;
        const char* prefix = DeviceManager::prefix_for_slot(s);
        if (!prefix) continue;
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
        case 411: return "Length Required";
        case 503: return "Service Unavailable";
        case 500: return "Internal Server Error";
        default:  return "OK";
    }
}
