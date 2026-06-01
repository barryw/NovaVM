// SdHttpServer -- tiny HTTP liveness endpoint.
//
// Endpoint:
//   GET /health -> NovaHost liveness, independent of SD/FPGA state

#ifndef NOVAHOST_SD_HTTP_SERVER_H
#define NOVAHOST_SD_HTTP_SERVER_H

#include <Arduino.h>
#include <WiFi.h>

class SdHttpServer {
public:
    explicit SdHttpServer(uint16_t port = 80) : _server(port) {}

    void begin();
    void loop();

private:
    static constexpr size_t LINE_BUF_BYTES = 512;
    static constexpr uint32_t REQUEST_LINE_TIMEOUT_MS = 750;
    static constexpr uint32_t HEADER_LINE_TIMEOUT_MS = 1000;
    static constexpr uint32_t RESPONSE_WRITE_TIMEOUT_MS = 750;

    WiFiServer _server;
    bool _started = false;

    void handle_client(WiFiClient& client);

    bool read_line(WiFiClient& client, char* out, size_t out_len,
                   uint32_t timeout_ms = 5000);
    bool write_all(WiFiClient& client, const uint8_t* data, size_t len);
    void send_json(WiFiClient& client, int code, const char* body);
    void send_error(WiFiClient& client, int code, const char* message);
    void send_headers(WiFiClient& client, int code, const char* content_type,
                      int32_t content_len = -1);

    static const char* reason_phrase(int code);
};

#endif
