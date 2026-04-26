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
// Embedded EhBASIC + extension ROM binaries — auto-generated from
// ehbasic/basic.bin and ehbasic/extension.bin by tools/bin2header.py.
// NovaHost streams these into FPGA BRAM at boot, so ROM iteration costs
// ~5s OTA instead of a 17-min bitstream rebuild.
#include "ehbasic_rom.h"
#include "extension_rom.h"
#include "sid_curve_rom.h"

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
// Boot-time ROM load into FPGA BRAM via debug bridge
// =========================================================================
// At 115200 baud with bulk CMD_POKE_ROM_BLK (one ack per 256-byte block),
// full 32 KB ROM load takes ~3 seconds.
bool loadRomsToFPGA() {
    logLn("Loading ROM into FPGA BRAM (ehbasic=%u bytes, extension=%u bytes)...",
          (unsigned)EHBASIC_ROM_LEN, (unsigned)EXTENSION_ROM_LEN);
    unsigned long t0 = millis();

    // Drain any stale bytes from a previous session.
    fpgaBridge.drain();

    // The FPGA may still be configuring from flash when ESP32 setup() runs —
    // retry resetHold several times before giving up. A single attempt is
    // ~220 ms with the byte timeout, so 20 attempts gives the FPGA ~4 s to
    // come up before we bail.
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

    if (!fpgaBridge.loadRom(0, EHBASIC_ROM, EHBASIC_ROM_LEN)) {
        logLn("ROM load FAILED: basic_rom streaming aborted");
        return false;
    }

    if (!fpgaBridge.loadRom(1, EXTENSION_ROM, EXTENSION_ROM_LEN)) {
        logLn("ROM load FAILED: ext_rom streaming aborted");
        return false;
    }

    // Stream the 6581 filter curve into SDRAM at the CURVE_BASE offset
    // hardcoded in rtl/sid/sid_curve_reader.sv (25'h0_8_00_00 = $800000).
    // Done before resetRelease so sid_curve_reader (which is held in reset
    // by dbg_cpu_reset) sees valid data on its first read.
    // Not a hard dependency: only SID 6581 filter uses the curve, and an
    // older bitstream won't support CMD_POKE_SDRAM_BLK. Log and continue
    // so the CPU still boots EhBASIC.
    const uint32_t CURVE_BASE = 0x800000;
    if (!fpgaBridge.loadSdram(CURVE_BASE, SID_CURVE_ROM, SID_CURVE_ROM_LEN)) {
        logLn("WARN: SID curve streaming failed (old bitstream? SID 6581 filter will be silent)");
    } else {
        logLn("SID curve streamed (%u bytes @ SDRAM $%06X)",
              (unsigned)SID_CURVE_ROM_LEN, (unsigned)CURVE_BASE);
    }

    if (!fpgaBridge.resetRelease()) {
        logLn("ROM load FAILED: resetRelease did not ack");
        return false;
    }

    unsigned long dt = millis() - t0;
    logLn("ROM load OK — CPU released (took %lu ms)", dt);
    return true;
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

    // Stream ROM into FPGA BRAM. FPGA's debug_bridge boots with dbg_cpu_reset=1,
    // so the CPU stays held in reset until we've loaded ROM + released. If the
    // FPGA isn't wired up (or is running the old bitstream), this logs a
    // failure and we continue — OTA still works via WiFi for recovery.
    loadRomsToFPGA();

    // SD card mount via SPI mode. ULX3S routes SD to ESP32 GPIOs in a
    // way that's incompatible with SDIO 1-bit/4-bit on this board (verified
    // 2026-04-25 — SD_MMC.begin() returns false even with proper setPins,
    // 1-bit mode, 400kHz init, fresh FAT32 format, and full power cycle;
    // a parallel SD.begin() probe at the same time successfully reads the
    // card as SDHC). Pinout (SCK=GPIO14, MISO=GPIO2/sd_d[0], MOSI=GPIO15/
    // sd_cmd, CS=GPIO13/sd_d[3]) per emard/ulx3s_v20.lpf.
    // Throughput tradeoff: SPI ~3-6 MB/s vs SDIO 25 MB/s — fine for BASIC
    // file I/O. SD.h doesn't auto-format, so card MUST be FAT32 already.
    logLn("SD: mounting via SPI (SCK=14 MISO=2 MOSI=15 CS=13)...");
    SPI.begin(14, 2, 15, 13);
    bool sd_ok = SD.begin(13, SPI, 4000000);  // 4 MHz init
    char diag_buf[256];
    if (!sd_ok) {
        snprintf(diag_buf, sizeof(diag_buf),
                 "SD.begin()=false — check FAT32 format + card seating");
        g_sd_diag = String(diag_buf);
        logLn("SD: mount FAILED. %s", diag_buf);
    } else {
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
        logLn("SD: mount OK — %s", diag_buf);

        int hd_count = deviceManager.auto_mount_hds();
        logLn("SD: auto-mounted %d hd*.ndi disk image(s)", hd_count);
        char mount_buf[48];
        snprintf(mount_buf, sizeof(mount_buf), " | hd_mounts=%d", hd_count);
        g_sd_diag += String(mount_buf);
    }

    // Wire the FIO event reader → dispatcher trampoline. Whenever the
    // FPGA emits 0xFE 0xE0 on the bridge, the reader fires the
    // dispatcher to drain registers + execute the file op.
    fioEventReader.onEvent(FioDispatcher::onFioEventStatic, &fioDispatcher);

    // WiFi + servers
    setupWiFi();

    // SD HTTP file server — must come AFTER setupWiFi() since the
    // AsyncWebServer needs the WiFi stack live.
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
    }

    // Check WiFi reconnection
    static unsigned long lastWifiCheck = 0;
    if (wifi_connected && WiFi.status() != WL_CONNECTED && millis() - lastWifiCheck > 10000) {
        lastWifiCheck = millis();
        logLn("WiFi lost — reconnecting...");
        WiFi.reconnect();
    }
}
