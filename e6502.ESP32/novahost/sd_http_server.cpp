#include "sd_http_server.h"
#include <SD.h>

extern void logLn(const char* fmt, ...);

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------
void SdHttpServer::begin() {
    // GET handler covers both file-fetch and directory-listing depending
    // on whether the URL ends in '/'.
    _server.on("/sd/*", HTTP_GET, [this](AsyncWebServerRequest* req) {
        String p = path_after_sd(req);
        // Empty or trailing-slash → directory listing.
        if (p.length() == 0 || p.endsWith("/")) {
            handle_list(req, p);
        } else {
            handle_get(req, p);
        }
    });

    // Same prefix for /sd (no trailing slash) — list root.
    _server.on("/sd", HTTP_GET, [this](AsyncWebServerRequest* req) {
        handle_list(req, "");
    });

    _server.on("/sd/*", HTTP_DELETE, [this](AsyncWebServerRequest* req) {
        String p = path_after_sd(req);
        handle_delete(req, p);
    });

    // PUT — three-callback form: route, file-upload (unused, multipart
    // only), and body-stream callback. ESPAsyncWebServer hands us body
    // chunks via the body callback.
    _server.on("/sd/*", HTTP_PUT,
        [this](AsyncWebServerRequest* req) {
            // Final callback after all body chunks delivered.
            bool ok = _upload.active;
            handle_put_done();
            if (ok) {
                req->send(200, "application/json", "{\"ok\":true}");
            } else {
                String err = "{\"error\":\"";
                err += _upload.last_err.length() ? _upload.last_err : String("write failed");
                err += "\"}";
                req->send(500, "application/json", err);
            }
        },
        nullptr,
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len,
               size_t index, size_t total) {
            if (index == 0) {
                String p = path_after_sd(req);
                handle_put_begin(req, p);
            }
            handle_put_chunk(data, len, index, total);
        });

    // Health check.
    _server.on("/health", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // SD status — diagnoses mount/format issues remotely. The bootDiag
    // field captures the SD.begin() outcome so failures are visible
    // post-boot (TCP log server starts after SD mount, so log lines from
    // the mount call would otherwise be invisible).
    extern String g_sd_diag;
    _server.on("/sd-status", HTTP_GET, [](AsyncWebServerRequest* req) {
        uint8_t  ct        = SD.cardType();
        uint64_t card_sz   = SD.cardSize();
        uint64_t total_sz  = SD.totalBytes();
        uint64_t used_sz   = SD.usedBytes();
        const char* type_name =
            ct == CARD_NONE ? "NONE" :
            ct == CARD_MMC  ? "MMC"  :
            ct == CARD_SD   ? "SDSC" :
            ct == CARD_SDHC ? "SDHC" :
            "UNKNOWN";
        char buf[512];
        snprintf(buf, sizeof(buf),
            "{\"cardType\":\"%s\",\"cardTypeId\":%u,"
            "\"cardSize\":%llu,\"totalBytes\":%llu,\"usedBytes\":%llu,"
            "\"bootDiag\":\"%s\"}",
            type_name, (unsigned)ct,
            (unsigned long long)card_sz,
            (unsigned long long)total_sz,
            (unsigned long long)used_sz,
            g_sd_diag.c_str());
        req->send(200, "application/json", buf);
    });

    _server.onNotFound([](AsyncWebServerRequest* req) {
        req->send(404, "application/json",
                  "{\"error\":\"not found\"}");
    });

    _server.begin();
    Serial.println("[sdhttp] HTTP file server up on port 80");
    Serial.println("[sdhttp]   GET  /sd/        → list root");
    Serial.println("[sdhttp]   GET  /sd/path    → fetch file");
    Serial.println("[sdhttp]   PUT  /sd/path    → upload file");
    Serial.println("[sdhttp]   DELETE /sd/path  → remove");
}

// ---------------------------------------------------------------------------
// Path helpers
// ---------------------------------------------------------------------------
String SdHttpServer::path_after_sd(AsyncWebServerRequest* req) {
    // URL is something like /sd/foo/bar.ndi → return "foo/bar.ndi".
    // Or /sd/ or /sd → "".
    String url = req->url();
    if (url.startsWith("/sd/")) return url.substring(4);  // strip "/sd/"
    if (url == "/sd")           return "";
    return url;
}

// Refuse writes/deletes against currently-mounted hd*.ndi / fd*.ndi
// because the on-disk image is in a controlled state owned by the
// dispatcher. Caller should UNMOUNT first.
bool SdHttpServer::path_safe_for_write(const String& path) {
    // Only files matching the auto-mount or active-mount pattern matter.
    // Cheap check: any path ending in .ndi that names a slot we have
    // mounted gets rejected.
    if (!path.endsWith(".ndi") && !path.endsWith(".NDI")) return true;

    // Strip leading dirs — we only auto-mount at SD root, but FD mounts
    // can come from anywhere. Match the full path against each mounted
    // slot's source path (if we tracked it). For now, conservative
    // check: any path containing "hd<N>.ndi" while that HD is mounted.
    String lower = path;
    lower.toLowerCase();
    for (int s = 0; s < DeviceManager::NUM_SLOTS; s++) {
        if (!_dm.is_mounted(s)) continue;
        const char* prefix = DeviceManager::prefix_for_slot(s);
        if (!prefix) continue;
        String needle = String("/") + prefix + ".ndi";
        // path comes in without leading slash from path_after_sd, so
        // also try the un-prefixed form.
        if (lower.endsWith(needle.substring(1)) || lower.endsWith(needle)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// LIST
// ---------------------------------------------------------------------------
void SdHttpServer::send_json_listing(AsyncWebServerRequest* req,
                                      const String& dir_path) {
    String root = "/" + dir_path;
    File dir = SD.open(root);
    if (!dir || !dir.isDirectory()) {
        char err[200];
        snprintf(err, sizeof(err),
                 "{\"error\":\"not a directory\","
                  "\"path\":\"%s\","
                  "\"open\":%d,"
                  "\"isDir\":%d,"
                  "\"exists\":%d,"
                  "\"cardType\":%d}",
                 root.c_str(),
                 (int)(bool)dir,
                 dir ? (int)dir.isDirectory() : -1,
                 (int)SD.exists(root),
                 (int)SD.cardType());
        req->send(404, "application/json", err);
        return;
    }

    String json = "[";
    bool first = true;
    File entry;
    while ((entry = dir.openNextFile())) {
        if (!first) json += ",";
        first = false;
        json += "{\"name\":\"";
        json += entry.name();
        json += "\",\"size\":";
        json += String((uint32_t)entry.size());
        json += ",\"dir\":";
        json += entry.isDirectory() ? "true" : "false";
        json += "}";
        entry.close();
    }
    json += "]";
    dir.close();
    req->send(200, "application/json", json);
}

void SdHttpServer::handle_list(AsyncWebServerRequest* req,
                                const String& path) {
    send_json_listing(req, path);
}

// ---------------------------------------------------------------------------
// GET
// ---------------------------------------------------------------------------
void SdHttpServer::handle_get(AsyncWebServerRequest* req,
                               const String& path) {
    String full = "/" + path;
    if (!SD.exists(full)) {
        req->send(404, "application/json",
                  "{\"error\":\"not found\"}");
        return;
    }
    // ESPAsyncWebServer streams files efficiently via the SD fs::FS.
    AsyncWebServerResponse* resp =
        req->beginResponse(SD, full, "application/octet-stream");
    if (!resp) {
        req->send(500, "application/json",
                  "{\"error\":\"open failed\"}");
        return;
    }
    req->send(resp);
}

// ---------------------------------------------------------------------------
// DELETE
// ---------------------------------------------------------------------------
void SdHttpServer::handle_delete(AsyncWebServerRequest* req,
                                  const String& path) {
    String full = "/" + path;
    if (!SD.exists(full)) {
        req->send(404, "application/json",
                  "{\"error\":\"not found\"}");
        return;
    }
    if (!path_safe_for_write(path)) {
        req->send(409, "application/json",
                  "{\"error\":\"file is mounted; UNMOUNT first\"}");
        return;
    }
    File entry = SD.open(full);
    bool is_dir = entry && entry.isDirectory();
    if (entry) entry.close();

    bool ok = is_dir ? SD.rmdir(full) : SD.remove(full);
    if (!ok) {
        req->send(500, "application/json",
                  "{\"error\":\"delete failed\"}");
        return;
    }
    req->send(200, "application/json", "{\"ok\":true}");
}

// ---------------------------------------------------------------------------
// PUT — three-stage: begin (first chunk), chunk (each), done (after)
// ---------------------------------------------------------------------------
void SdHttpServer::handle_put_begin(AsyncWebServerRequest* req,
                                     const String& path) {
    if (_upload.active) {
        // Stale upload — close to recover.
        if (_upload.file) _upload.file.close();
        _upload.active = false;
    }

    if (!path_safe_for_write(path)) {
        logLn("[sdhttp] PUT refused: /%s is mounted\n", path.c_str());
        req->send(409, "application/json",
                  "{\"error\":\"file is mounted; UNMOUNT first\"}");
        return;
    }

    String full = "/" + path;

    // Make parent dirs if needed (one level of mkdir-p).
    int last_slash = full.lastIndexOf('/');
    if (last_slash > 0) {
        String parent = full.substring(0, last_slash);
        if (parent.length() > 0 && !SD.exists(parent)) {
            SD.mkdir(parent);
        }
    }

    // FS::open default for `create` is false — must pass true here so
    // SD will create the file if it doesn't already exist.
    _upload.file = SD.open(full, FILE_WRITE, true);
    if (!_upload.file) {
        char buf[160];
        snprintf(buf, sizeof(buf),
                 "open(W) failed path=%s exists=%d cardType=%d",
                 full.c_str(),
                 (int)SD.exists(full),
                 (int)SD.cardType());
        _upload.last_err = String(buf);
        logLn("[sdhttp] PUT: %s", buf);
        return;
    }
    _upload.active = true;
    _upload.path   = full;
    logLn("[sdhttp] PUT %s opened\n", full.c_str());
}

void SdHttpServer::handle_put_chunk(uint8_t* data, size_t len,
                                     size_t index, size_t total) {
    (void)index; (void)total;
    if (!_upload.active || !_upload.file) return;
    size_t wrote = _upload.file.write(data, len);
    if (wrote != len) {
        logLn("[sdhttp] PUT %s: short write %u/%u\n",
                      _upload.path.c_str(), (unsigned)wrote, (unsigned)len);
        _upload.file.close();
        _upload.active = false;
    }
}

void SdHttpServer::handle_put_done() {
    if (_upload.active && _upload.file) {
        size_t size = _upload.file.size();
        _upload.file.close();
        logLn("[sdhttp] PUT %s closed (%u bytes)\n",
                      _upload.path.c_str(), (unsigned)size);
    }
    _upload.active = false;
    _upload.path   = "";
}
