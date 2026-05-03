// SdHttpServer — small blocking REST file server backed by SD.
//
// Endpoints:
//   GET    /health       -> NovaHost liveness, independent of SD/FPGA state
//   GET    /wifi         -> WiFi status/config without password disclosure
//   GET    /wifi/scan    -> nearby WiFi networks
//   PUT    /wifi         -> update WiFi config JSON
//   POST   /wifi/connect|disconnect|reconnect|forget
//   GET    /sd-status    -> SD diagnostic JSON
//   POST   /reboot       -> reboot NovaHost after responding
//   GET    /sd/          -> JSON listing
//   GET    /sd/<path>    -> file body, or listing if path is a directory
//   PUT    /sd/<path>    -> whole-file upload from request body
//   DELETE /sd/<path>    -> remove file or empty directory
//
// This deliberately uses only WiFiServer. NovaHost needs whole-file SD
// management, so a single-request loop is smaller and easier to reason about.

#ifndef NOVAHOST_SD_HTTP_SERVER_H
#define NOVAHOST_SD_HTTP_SERVER_H

#include <Arduino.h>
#include <SD.h>
#include <WiFi.h>
#include "device_manager.h"
#include "nova_wifi.h"

class SdHttpServer {
public:
    SdHttpServer(DeviceManager& dm, NovaWifiManager& wifi, uint16_t port = 80)
        : _dm(dm), _wifi(wifi), _server(port) {}

    void begin();
    void loop();

private:
    static constexpr size_t LINE_BUF_BYTES = 512;
    static constexpr size_t IO_BUF_BYTES   = 1024;
    static constexpr size_t JSON_BUF_BYTES = 1024;

    DeviceManager& _dm;
    NovaWifiManager& _wifi;
    WiFiServer     _server;
    bool           _started = false;

    void handle_client(WiFiClient& client);

    bool read_line(WiFiClient& client, char* out, size_t out_len,
                   uint32_t timeout_ms = 5000);
    bool read_body_to_file(WiFiClient& client, File& file,
                           uint32_t content_len);
    bool read_body_to_string(WiFiClient& client, String& out,
                             uint32_t content_len);
    bool write_all(WiFiClient& client, const uint8_t* data, size_t len);

    void handle_wifi(WiFiClient& client, const char* method, const char* url,
                     bool have_content_len, uint32_t content_len);
    void handle_wifi_status(WiFiClient& client);
    void handle_wifi_scan(WiFiClient& client);
    void handle_wifi_put(WiFiClient& client, uint32_t content_len);
    void handle_wifi_action(WiFiClient& client, const char* action);

    void handle_get(WiFiClient& client, const char* path);
    void handle_put(WiFiClient& client, const char* path,
                    uint32_t content_len);
    void handle_delete(WiFiClient& client, const char* path);
    void handle_status(WiFiClient& client);
    void handle_reboot(WiFiClient& client);

    void send_listing(WiFiClient& client, const char* path);
    void send_file(WiFiClient& client, const char* path);
    void send_json(WiFiClient& client, int code, const char* body);
    void send_error(WiFiClient& client, int code, const char* message);
    void send_headers(WiFiClient& client, int code, const char* content_type,
                      int32_t content_len = -1);
    void write_json_string(WiFiClient& client, const char* value);

    bool path_sane(const char* path, bool allow_empty);
    bool path_safe_for_write(const char* path);
    bool ensure_parent_dirs(const char* full_path);

    static const char* path_after_sd(const char* url, char* out,
                                     size_t out_len);
    static const char* reason_phrase(int code);
};

#endif
