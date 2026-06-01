#include "sd_http_server.h"
#include <string.h>

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

void SdHttpServer::begin() {
    if (_started)
        return;
    _started = true;
    _server.begin();
    Serial.println("[sdhttp] HTTP liveness server up on port 80");
    Serial.println("[sdhttp]   GET    /health   -> NovaHost liveness");
}

void SdHttpServer::loop() {
    WiFiClient client = _server.available();
    if (!client)
        return;

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
    while (*url_start == ' ')
        url_start++;
    char* url_end = strchr(url_start, ' ');
    if (url_end)
        *url_end = 0;
    if (line[0] == 0 || url_start[0] == 0) {
        send_error(client, 400, "bad request");
        return;
    }

    strncpy(method, line, sizeof(method));
    method[sizeof(method) - 1] = 0;
    strncpy(url, url_start, sizeof(url));
    url[sizeof(url) - 1] = 0;

    while (read_line(client, line, sizeof(line), HEADER_LINE_TIMEOUT_MS)) {
        if (line[0] == 0)
            break;
    }

    if (strcmp(method, "GET") != 0 || strcmp(url, "/health") != 0) {
        send_error(client, 404, "not found");
        return;
    }

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
}

bool SdHttpServer::read_line(WiFiClient& client, char* out, size_t out_len,
                             uint32_t timeout_ms) {
    size_t n = 0;
    uint32_t deadline = millis() + timeout_ms;
    while (millis() < deadline) {
        while (client.available()) {
            char c = (char)client.read();
            if (c == '\r')
                continue;
            if (c == '\n') {
                out[n] = 0;
                return true;
            }
            if (n + 1 < out_len)
                out[n++] = c;
        }
        delay(1);
    }

    out[n] = 0;
    return false;
}

bool SdHttpServer::write_all(WiFiClient& client, const uint8_t* data,
                             size_t len) {
    size_t off = 0;
    uint32_t deadline = millis() + RESPONSE_WRITE_TIMEOUT_MS;
    while (off < len && millis() < deadline) {
        int avail = client.availableForWrite();
        size_t chunk = len - off;
        if (avail > 0 && chunk > (size_t)avail)
            chunk = (size_t)avail;
        else if (avail <= 0 && chunk > 64)
            chunk = 64;
        size_t wrote = client.write(data + off, chunk);
        if (wrote == 0) {
            delay(1);
            continue;
        }
        off += wrote;
        deadline = millis() + RESPONSE_WRITE_TIMEOUT_MS;
    }
    return off == len;
}

void SdHttpServer::send_json(WiFiClient& client, int code, const char* body) {
    send_headers(client, code, "application/json", strlen(body));
    write_all(client, (const uint8_t*)body, strlen(body));
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
    char header[240];
    int n = snprintf(header, sizeof(header),
                     "HTTP/1.1 %d %s\r\nContent-Type: %s\r\n"
                     "Content-Length: %ld\r\nConnection: close\r\n\r\n",
                     code, reason_phrase(code), content_type,
                     (long)(content_len < 0 ? 0 : content_len));
    if (n > 0)
        write_all(client, (const uint8_t*)header, (size_t)n);
}

const char* SdHttpServer::reason_phrase(int code) {
    switch (code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        default:  return "OK";
    }
}
