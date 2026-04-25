// SdHttpServer — minimal REST file server backed by SD_MMC.
//
// Endpoints (all under /sd/...):
//   GET  /sd/                     → JSON listing of SD root
//   GET  /sd/<dir>/               → JSON listing of subdirectory
//   GET  /sd/<path>               → file body (Content-Length set)
//   PUT  /sd/<path>               → write file (body bytes); creates
//                                   parent dirs as needed
//   DELETE /sd/<path>             → remove file (or empty directory)
//
// Returns 200 on success, 404 for missing files, 409 for write-while-
// mounted, 500 for I/O errors. Always JSON for list responses.
//
// Designed to be the remote backend for `e6502.NDI --remote=host ...`.
// Treats files as opaque blobs — does NOT parse .ndi internally. The
// CLI does the NDI manipulation locally, this server just stores bytes.
//
// Deps: ESPAsyncWebServer (lacamera fork) + AsyncTCP. Must call
// `begin()` after WiFi is up.

#ifndef NOVAHOST_SD_HTTP_SERVER_H
#define NOVAHOST_SD_HTTP_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "device_manager.h"

class SdHttpServer {
public:
    SdHttpServer(DeviceManager& dm, uint16_t port = 80)
        : _dm(dm), _server(port) {}

    void begin();

private:
    DeviceManager&   _dm;
    AsyncWebServer   _server;

    // Active upload state. ESPAsyncWebServer body callbacks give us
    // chunks of the PUT body across multiple invocations; we hold the
    // open File between chunks here.
    struct Upload {
        bool   active;
        File   file;
        String path;
    } _upload = { false, File(), "" };

    // Handlers — separate methods so they're easier to read + test.
    void handle_list(AsyncWebServerRequest* req, const String& path);
    void handle_get(AsyncWebServerRequest* req, const String& path);
    void handle_delete(AsyncWebServerRequest* req, const String& path);
    void handle_put_begin(AsyncWebServerRequest* req, const String& path);
    void handle_put_chunk(uint8_t* data, size_t len, size_t index, size_t total);
    void handle_put_done();

    // Refuses writes/deletes that would clobber an actively mounted
    // hd*.ndi or fd*.ndi. Returns true if path is safe to modify.
    bool path_safe_for_write(const String& path);

    // Helpers.
    static String path_after_sd(AsyncWebServerRequest* req);
    static void   send_json_listing(AsyncWebServerRequest* req,
                                     const String& dir_path);
};

#endif
