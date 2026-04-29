// NovaHost — ESP32 host services for the e6502 FPGA computer
// Communicates with FPGA via UART (Serial2: wifi_rxd/wifi_txd)
// Provides: WiFi, debug bridge (TCP:6503 ↔ FPGA binary protocol)
//
// Debug logs streamed over WiFi: connect with `nc <ip> 23`
// Test connection: connect with test suite to novahost.local:6503
// Board: ESP32 on ULX3S (select WEMOS LOLIN32 in Arduino IDE)

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <HardwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include "fpga_bridge.h"
#include "debug_server.h"
#include "device_manager.h"
#include "fio_event_reader.h"
#include "fio_dispatcher.h"
#include "sd_http_server.h"
// Boot ROMs and bulky assets live on the SD card. The FPGA bitstream still
// carries fallback ROM init data, but NovaHost no longer embeds those blobs.

// =========================================================================
// Configuration
// =========================================================================

// WiFi credentials — kept in wifi_config.h (gitignored)
#include "wifi_config.h"

// FPGA UART — uses ESP32 Serial2.
// ULX3S v3.1.x: GPIO26/GPIO27 route to FPGA pins L1/N3.
// (v3.0.x used GPIO16/17 on the same FPGA pins — different board revision.)
#define FPGA_SERIAL Serial2
// 115200 — known-reliable on flying wires between ESP32 and FPGA.
// 3.125 Mbaud attempted and failed (bridge commands all timed out; likely
// signal integrity). Can bump again with proper signal conditioning later.
#define FPGA_BAUD   115200
#define FPGA_RX_PIN 26   // ESP32 GPIO26 = Serial2 RX (receives from FPGA L1)
#define FPGA_TX_PIN 27   // ESP32 GPIO27 = Serial2 TX (transmits to FPGA N3)

// Debug log server — telnet-style TCP on port 23
#define LOG_PORT 23

// =========================================================================
// Debug logging — streams to WiFi TCP clients + Serial
// =========================================================================
WiFiServer logServer(LOG_PORT);
WiFiClient logClients[3];  // up to 3 simultaneous log viewers

void logMsg(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    // Always to hardware serial (even if nobody can see it)
    Serial.print(buf);

    // Stream to all connected TCP clients
    for (int i = 0; i < 3; i++) {
        if (logClients[i] && logClients[i].connected()) {
            logClients[i].print(buf);
        }
    }
}

void logLn(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    Serial.println(buf);
    for (int i = 0; i < 3; i++) {
        if (logClients[i] && logClients[i].connected()) {
            logClients[i].println(buf);
        }
    }
}

// =========================================================================
// Global objects
// =========================================================================
bool wifi_connected = false;
FpgaBridge fpgaBridge(FPGA_SERIAL);
DebugServer debugServer(fpgaBridge);

// File I/O subsystem — SD card, mount manager, dispatcher, event reader.
DeviceManager   deviceManager;
FioDispatcher   fioDispatcher(fpgaBridge, deviceManager);
FioEventReader  fioEventReader(FPGA_SERIAL);
SdHttpServer    sdHttpServer(deviceManager);

// SD mount diagnostic — captured at boot, exposed via /sd-status JSON.
String g_sd_diag = "(boot in progress)";
bool g_sd_mounted = false;

static const size_t BOOT_ROM_LEN       = 16 * 1024;
static const size_t SID_CURVE_ROM_LEN  = 8 * 1024;
static const uint32_t SID_CURVE_BASE   = 0x800000;

static const char* const BASIC_ROM_PATHS[] = {
    "/roms/novabasic.bin",
    "/roms/basic.bin",
    "/roms/ehbasic.bin"
};

static const char* const EXTENSION_ROM_PATHS[] = {
    "/roms/extension.bin",
    "/roms/ext.bin"
};

static const char* const SID_CURVE_PATHS[] = {
    "/assets/sid/f6581_curve.bin",
    "/assets/f6581_curve.bin"
};

// =========================================================================
// SD card setup and boot asset helpers
// =========================================================================
bool mountSdCard() {
    // SD card mount via SPI mode. ULX3S routes SD to ESP32 GPIOs in a
    // way that's incompatible with SDIO 1-bit/4-bit on this board (verified
    // 2026-04-25). Pinout: SCK=GPIO14, MISO=GPIO2/sd_d[0],
    // MOSI=GPIO15/sd_cmd, CS=GPIO13/sd_d[3].
    logLn("SD: mounting via SPI (SCK=14 MISO=2 MOSI=15 CS=13)...");
    SPI.begin(14, 2, 15, 13);
    g_sd_mounted = SD.begin(13, SPI, 4000000);

    char diag_buf[256];
    if (!g_sd_mounted) {
        snprintf(diag_buf, sizeof(diag_buf),
                 "SD.begin()=false - check FAT32 format + card seating");
        g_sd_diag = String(diag_buf);
        logLn("SD: mount FAILED. %s", diag_buf);
        return false;
    }

    uint8_t  ct = SD.cardType();
    uint64_t sz = SD.cardSize();
    const char* type_str = "UNKNOWN";
    switch (ct) {
        case CARD_NONE: type_str = "NONE"; break;
        case CARD_MMC:  type_str = "MMC";  break;
        case CARD_SD:   type_str = "SDSC"; break;
        case CARD_SDHC: type_str = "SDHC"; break;
    }

    snprintf(diag_buf, sizeof(diag_buf),
             "SD.begin()=true SPI type=%s size=%llu MB",
             type_str,
             (unsigned long long)(sz / (1024ULL * 1024ULL)));
    g_sd_diag = String(diag_buf);
    logLn("SD: mount OK - %s", diag_buf);

    int hd_count = deviceManager.auto_mount_hds();
    logLn("SD: auto-mounted %d hd*.ndi disk image(s)", hd_count);
    char mount_buf[48];
    snprintf(mount_buf, sizeof(mount_buf), " | hd_mounts=%d", hd_count);
    g_sd_diag += String(mount_buf);
    return true;
}

File openFirstAsset(const char* label, const char* const* paths,
                    size_t path_count, const char*& chosen_path) {
    for (size_t i = 0; i < path_count; i++) {
        File f = SD.open(paths[i], FILE_READ);
        if (f) {
            chosen_path = paths[i];
            return f;
        }
    }

    chosen_path = nullptr;
    logLn("Boot asset missing: %s", label);
    return File();
}

bool bootAssetAvailable(const char* label, const char* const* paths,
                        size_t path_count, size_t expected_len) {
    const char* path = nullptr;
    File f = openFirstAsset(label, paths, path_count, path);
    if (!f) return false;

    size_t actual = f.size();
    f.close();
    if (actual != expected_len) {
        logLn("Boot asset wrong size: %s at %s is %u bytes, expected %u",
              label, path, (unsigned)actual, (unsigned)expected_len);
        return false;
    }
    return true;
}

bool streamRomAsset(uint8_t idx, const char* label, const char* const* paths,
                    size_t path_count, size_t expected_len) {
    const char* path = nullptr;
    File f = openFirstAsset(label, paths, path_count, path);
    if (!f) return false;

    size_t actual = f.size();
    if (actual != expected_len) {
        logLn("Boot asset wrong size: %s at %s is %u bytes, expected %u",
              label, path, (unsigned)actual, (unsigned)expected_len);
        f.close();
        return false;
    }

    logLn("Streaming %s from %s (%u bytes)", label, path, (unsigned)actual);
    uint8_t buf[256];
    for (size_t off = 0; off < expected_len; ) {
        size_t want = (expected_len - off >= sizeof(buf))
            ? sizeof(buf)
            : expected_len - off;
        size_t got = f.read(buf, want);
        if (got != want) {
            logLn("Boot asset read failed: %s at offset %u", label, (unsigned)off);
            f.close();
            return false;
        }
        uint16_t wire_count = (got == 256) ? 0 : (uint16_t)got;
        if (!fpgaBridge.pokeRomBlock(idx, (uint16_t)off, buf, wire_count)) {
            logLn("FPGA ROM stream failed: %s at offset %u", label, (unsigned)off);
            f.close();
            return false;
        }
        off += got;
    }

    f.close();
    return true;
}

bool streamSdramAsset(uint32_t base_addr, const char* label,
                      const char* const* paths, size_t path_count,
                      size_t expected_len) {
    const char* path = nullptr;
    File f = openFirstAsset(label, paths, path_count, path);
    if (!f) return false;

    size_t actual = f.size();
    if (actual != expected_len) {
        logLn("Boot asset wrong size: %s at %s is %u bytes, expected %u",
              label, path, (unsigned)actual, (unsigned)expected_len);
        f.close();
        return false;
    }

    logLn("Streaming %s from %s (%u bytes)", label, path, (unsigned)actual);
    uint8_t buf[256];
    for (size_t off = 0; off < expected_len; ) {
        size_t want = (expected_len - off >= sizeof(buf))
            ? sizeof(buf)
            : expected_len - off;
        size_t got = f.read(buf, want);
        if (got != want) {
            logLn("Boot asset read failed: %s at offset %u", label, (unsigned)off);
            f.close();
            return false;
        }
        uint16_t wire_count = (got == 256) ? 0 : (uint16_t)got;
        if (!fpgaBridge.pokeSdramBlock(base_addr + off, buf, wire_count)) {
            logLn("FPGA SDRAM stream failed: %s at offset %u", label, (unsigned)off);
            f.close();
            return false;
        }
        off += got;
    }

    f.close();
    return true;
}

// =========================================================================
// WiFi setup
// =========================================================================
void setupWiFi() {
    logLn("Connecting to WiFi: %s", WIFI_SSID);
    WiFi.mode(WIFI_STA);

#ifdef WIFI_STATIC_IP
    // Pin a static IP so deploy tooling (OTA, test harness, debug bridge)
    // doesn't depend on mDNS. Must be called before WiFi.begin().
    IPAddress ip, gw, sn, dns;
    if (ip.fromString(WIFI_STATIC_IP) &&
        gw.fromString(WIFI_GATEWAY) &&
        sn.fromString(WIFI_SUBNET) &&
        dns.fromString(WIFI_DNS)) {
        WiFi.config(ip, gw, sn, dns);
        Serial.printf("Static IP requested: %s (gw %s, mask %s, dns %s)\n",
                      WIFI_STATIC_IP, WIFI_GATEWAY, WIFI_SUBNET, WIFI_DNS);
    } else {
        Serial.println("Invalid static-IP config — falling back to DHCP");
    }
#endif

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        logLn("");
        logLn("WiFi connected!");
        logLn("IP address: %s", WiFi.localIP().toString().c_str());

        // mDNS: access as novahost.local
        if (MDNS.begin("novahost")) {
            MDNS.addService("telnet", "tcp", LOG_PORT);
            MDNS.addService("e6502-debug", "tcp", 6503);
            logLn("mDNS: novahost.local");
        }

        // Start servers
        logServer.begin();
        logLn("Debug log server on port %d", LOG_PORT);
        logLn("Connect with: nc %s %d", WiFi.localIP().toString().c_str(), LOG_PORT);

        debugServer.begin();

        // OTA firmware update — push via `arduino-cli upload --port novahost.local`
        ArduinoOTA.setHostname("novahost");
        ArduinoOTA.onStart([]() {
            logLn("OTA: upload starting (%s)",
                  ArduinoOTA.getCommand() == U_FLASH ? "flash" : "filesystem");
        });
        ArduinoOTA.onEnd([]() { logLn("OTA: upload complete, rebooting"); });
        ArduinoOTA.onError([](ota_error_t err) { logLn("OTA: error %u", err); });
        ArduinoOTA.begin();
        logLn("OTA enabled on novahost.local:3232");
    } else {
        Serial.println("\nWiFi connection failed — running without network");
    }
}

// =========================================================================
// Boot-time ROM/assets load into FPGA via debug bridge
// =========================================================================
// At 115200 baud with bulk CMD_POKE_ROM_BLK (one ack per 256-byte block),
// full 32 KB ROM load takes ~3 seconds.
bool loadRomsToFPGA() {
    logLn("Loading boot assets from SD into FPGA...");
    unsigned long t0 = millis();

    if (!g_sd_mounted) {
        logLn("Boot assets skipped: SD is not mounted; using bitstream ROM fallback");
        fpgaBridge.resetRelease();
        return false;
    }

    bool haveBasic = bootAssetAvailable("basic ROM", BASIC_ROM_PATHS,
                                        sizeof(BASIC_ROM_PATHS) / sizeof(BASIC_ROM_PATHS[0]),
                                        BOOT_ROM_LEN);
    bool haveExt = bootAssetAvailable("extension ROM", EXTENSION_ROM_PATHS,
                                      sizeof(EXTENSION_ROM_PATHS) / sizeof(EXTENSION_ROM_PATHS[0]),
                                      BOOT_ROM_LEN);

    if (!haveBasic || !haveExt) {
        logLn("Boot assets skipped: required ROM file(s) missing; using bitstream ROM fallback");
        fpgaBridge.resetRelease();
        return false;
    }

    const int LOAD_ATTEMPTS = 3;
    bool romsLoaded = false;

    for (int loadAttempt = 1; loadAttempt <= LOAD_ATTEMPTS; loadAttempt++) {
        // Drain any stale bytes from a previous session.
        fpgaBridge.drain();

        // The FPGA may still be configuring from flash when ESP32 setup() runs.
        // If resetHold never acks, the hardened FPGA falls back to bitstream-
        // initialized ROM and auto-releases reset on its own.
        bool ack = false;
        for (int attempt = 0; attempt < 20 && !ack; attempt++) {
            fpgaBridge.drain();
            if (fpgaBridge.resetHold()) {
                ack = true;
                if (attempt > 0)
                    logLn("FPGA bridge responded on attempt %d", attempt + 1);
            } else {
                delay(200);
            }
        }
        if (!ack) {
            logLn("ROM load FAILED: FPGA bridge never acked resetHold (wired up? bitstream loaded?)");
            return false;
        }

        if (!streamRomAsset(0, "basic ROM", BASIC_ROM_PATHS,
                            sizeof(BASIC_ROM_PATHS) / sizeof(BASIC_ROM_PATHS[0]),
                            BOOT_ROM_LEN)) {
            logLn("ROM load attempt %d/%d FAILED: basic_rom streaming aborted",
                  loadAttempt, LOAD_ATTEMPTS);
            delay(250);
            continue;
        }

        if (!streamRomAsset(1, "extension ROM", EXTENSION_ROM_PATHS,
                            sizeof(EXTENSION_ROM_PATHS) / sizeof(EXTENSION_ROM_PATHS[0]),
                            BOOT_ROM_LEN)) {
            logLn("ROM load attempt %d/%d FAILED: ext_rom streaming aborted",
                  loadAttempt, LOAD_ATTEMPTS);
            delay(250);
            continue;
        }

        romsLoaded = true;

        // Stream the 6581 filter curve into SDRAM at the SID_CURVE_BASE offset
        // hardcoded in rtl/sid/sid_curve_reader.sv (25'h0_8_00_00 = $800000).
        // Done before resetRelease so sid_curve_reader (which is held in reset
        // by dbg_cpu_reset) sees valid data on its first read.
        // Not a hard dependency: only SID 6581 filter uses the curve. Log and
        // continue so the CPU still boots.
        if (!streamSdramAsset(SID_CURVE_BASE, "SID 6581 curve", SID_CURVE_PATHS,
                              sizeof(SID_CURVE_PATHS) / sizeof(SID_CURVE_PATHS[0]),
                              SID_CURVE_ROM_LEN)) {
            logLn("WARN: SID curve not loaded; 6581 filter curve will be unavailable");
        } else {
            logLn("SID curve streamed (%u bytes @ SDRAM $%06X)",
                  (unsigned)SID_CURVE_ROM_LEN, (unsigned)SID_CURVE_BASE);
        }

        if (!fpgaBridge.resetRelease()) {
            logLn("ROM load attempt %d/%d FAILED: resetRelease did not ack",
                  loadAttempt, LOAD_ATTEMPTS);
            delay(250);
            continue;
        }

        unsigned long dt = millis() - t0;
        logLn("ROM load OK — CPU released (took %lu ms)", dt);
        return true;
    }

    if (romsLoaded) {
        logLn("ROM load FAILED after retries, but ROMs reached FPGA; attempting reset release for recovery");
        fpgaBridge.resetRelease();
    } else {
        logLn("ROM load FAILED after retries before both ROMs completed; CPU remains held to avoid booting partial ROM");
    }
    return false;
}

// =========================================================================
// Accept new log viewer connections
// =========================================================================
void handleLogClients() {
    if (logServer.hasClient()) {
        // Find a free slot
        for (int i = 0; i < 3; i++) {
            if (!logClients[i] || !logClients[i].connected()) {
                logClients[i] = logServer.available();
                logLn("Log viewer connected from %s",
                    logClients[i].remoteIP().toString().c_str());
                logClients[i].println("=== NovaHost Debug Log ===");
                logClients[i].printf("Uptime: %lu ms\n", millis());
                logClients[i].printf("Free heap: %d bytes\n", ESP.getFreeHeap());
                return;
            }
        }
        // No free slots — reject
        WiFiClient reject = logServer.available();
        reject.println("Too many log viewers");
        reject.stop();
    }
}

// =========================================================================
// Setup
// =========================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n=== NovaHost v0.3 ===");
    Serial.printf("Chip: %s rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("CPU: %d MHz, %d cores\n", ESP.getCpuFreqMHz(), ESP.getChipCores());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));

    // FPGA UART — binary debug protocol via debug_bridge.sv
    FPGA_SERIAL.begin(FPGA_BAUD, SERIAL_8N1, FPGA_RX_PIN, FPGA_TX_PIN);
    Serial.printf("FPGA UART initialized on GPIO%d/%d (binary debug protocol)\n",
                  FPGA_RX_PIN, FPGA_TX_PIN);

    // Give the FPGA debug bridge a moment to settle before we start poking it.
    delay(100);

    // SD must mount before boot asset streaming because ROMs and bulky data
    // are SD-resident. If SD is unavailable, the FPGA keeps using the
    // bitstream-initialized fallback ROM.
    mountSdCard();

    // Stream ROM/assets into FPGA. FPGA's debug_bridge boots with
    // dbg_cpu_reset=1, so the CPU stays held in reset until we've loaded
    // assets + released. If loading is skipped, resetRelease() falls back to
    // the ROM contents initialized by the bitstream.
    loadRomsToFPGA();

    // Wire the FIO event reader → dispatcher trampoline. Whenever the
    // FPGA emits 0xFE 0xE0 on the bridge, the reader fires the
    // dispatcher to drain registers + execute the file op.
    fioEventReader.onEvent(FioDispatcher::onFioEventStatic, &fioDispatcher);

    // WiFi + servers
    setupWiFi();

    // SD HTTP file server must start after WiFi is live.
    if (wifi_connected) {
        sdHttpServer.begin();
        logLn("SD HTTP server ready: http://%s/sd/",
              WiFi.localIP().toString().c_str());
    }

    logLn("NovaHost ready.");
}

// =========================================================================
// Main loop
// =========================================================================
void loop() {
    // Drain async FIO events from the FPGA before anything else.
    // poll() consumes serial bytes, dispatches 0xFE 0xE0 sequences to
    // the FioDispatcher, and drops anything else with a log.
    fioEventReader.poll();

    // Accept new log viewer connections
    if (wifi_connected) {
        ArduinoOTA.handle();
        handleLogClients();
        debugServer.loop();
        sdHttpServer.loop();
    }

    // Check WiFi reconnection
    static unsigned long lastWifiCheck = 0;
    if (wifi_connected && WiFi.status() != WL_CONNECTED && millis() - lastWifiCheck > 10000) {
        lastWifiCheck = millis();
        logLn("WiFi lost — reconnecting...");
        WiFi.reconnect();
    }
}
