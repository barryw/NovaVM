// NovaHost — ESP32 host services for the e6502 FPGA computer
// Communicates with FPGA via SPI on the shared ULX3S ESP/SD bus
// Provides: WiFi, debug bridge (TCP:6503 ↔ FPGA binary protocol)
//
// Debug logs streamed over WiFi: connect with `nc <ip> 23`
// Test connection: connect with test suite to 192.168.1.65:6503
// Board: ESP32 on ULX3S (select WEMOS LOLIN32 in Arduino IDE)

#include <WiFi.h>
#include <WiFiClient.h>
#ifndef NOVA_ENABLE_OTA
#define NOVA_ENABLE_OTA 1
#endif
#if NOVA_ENABLE_OTA
#include <ArduinoOTA.h>
#endif
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <ctype.h>
#include <string.h>
#include "fpga_bridge.h"
#include "debug_server.h"
#include "device_manager.h"
#include "fio_event_reader.h"
#include "fio_dispatcher.h"
#include "nic_dispatcher.h"
#include "sd_http_server.h"
#include "management_server.h"
#include "nova_wifi.h"
#include "boot_config_parser.h"
#include "shelf_alloc.h"
// Boot ROMs and bulky assets live on the SD card. The FPGA bitstream still
// carries fallback ROM init data, but NovaHost no longer embeds those blobs.

// =========================================================================
// Configuration
// =========================================================================

// Optional first-boot WiFi defaults. Runtime config is stored in NVS and can
// be managed over USB serial or REST once WiFi is reachable.
#if __has_include("wifi_config.h")
#include "wifi_config.h"
#else
static const char* WIFI_SSID = "";
static const char* WIFI_PASSWORD = "";
#endif

// Shared SPI bus on ULX3S SD-card pins. The SD card uses SPI mode with
// CS=GPIO13. The FPGA debug bridge uses GPIO4 (sd_d[1]) as its own CS.
// GPIO12/sd_d[2] is intentionally unused.
#define SD_SPI_SCK_PIN  14
#define SD_SPI_MISO_PIN 2
#define SD_SPI_MOSI_PIN 15
#define SD_CS_PIN       13
#define FPGA_SPI_CS_PIN 4
#ifndef FPGA_SPI_HZ
// Bulk FPGA writes are timing-clean at 40MHz in the current ULX3S bitstream.
// Reads are capped separately in FpgaBridge because GPIO-matrix MISO is slower.
#define FPGA_SPI_HZ     40000000
#endif

// Debug log server — telnet-style TCP on port 23
#define LOG_PORT 23

// =========================================================================
// Debug logging — streams to WiFi TCP clients + Serial
// =========================================================================
WiFiServer logServer(LOG_PORT);
WiFiClient logClients[3];  // up to 3 simultaneous log viewers

void writeLogClient(WiFiClient& client, const char* data, size_t len) {
    if (!client || !client.connected() || !data || len == 0)
        return;

    int writable = client.availableForWrite();
    if (writable < 0 || (size_t)writable < len) {
        client.stop();
        return;
    }

    size_t wrote = client.write((const uint8_t*)data, len);
    if (wrote != len)
        client.stop();
}

void writeLogClientLine(WiFiClient& client, const char* text) {
    writeLogClient(client, text, strlen(text));
    writeLogClient(client, "\r\n", 2);
}

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
            writeLogClient(logClients[i], buf, strlen(buf));
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
            writeLogClientLine(logClients[i], buf);
        }
    }
}

// =========================================================================
// Global objects
// =========================================================================
bool wifi_connected = false;
FpgaBridge fpgaBridge;
DebugServer debugServer(fpgaBridge);
NovaWifiManager novaWifi;

// File I/O subsystem — SD card, mount manager, dispatcher, event reader.
DeviceManager   deviceManager;
FioDispatcher   fioDispatcher(fpgaBridge, deviceManager);
FioEventReader  fioEventReader;
NicDispatcher   nicDispatcher(fpgaBridge);
SdHttpServer    sdHttpServer;
ManagementServer managementServer(deviceManager, novaWifi);

// SD mount diagnostic — captured at boot, exposed via /sd-status JSON.
String g_sd_diag = "(boot in progress)";
bool g_sd_mounted = false;

enum NovaBootPhase : uint8_t {
    BOOT_PHASE_POWER_ON = 0,
    BOOT_PHASE_INFRA_READY,
    BOOT_PHASE_SD_MOUNT,
    BOOT_PHASE_SPLASH,
    BOOT_PHASE_ROM_LOAD,
    BOOT_PHASE_READY,
    BOOT_PHASE_DEGRADED,
    BOOT_PHASE_FAILED
};

volatile uint8_t g_boot_phase = BOOT_PHASE_POWER_ON;
volatile bool g_fpga_bridge_owned_by_boot = false;
TaskHandle_t g_boot_task = nullptr;
TaskHandle_t g_audio_task = nullptr;
TaskHandle_t g_http_task = nullptr;
TaskHandle_t g_bridge_task = nullptr;
TaskHandle_t g_system_task = nullptr;
volatile uint32_t g_http_service_heartbeat_ms = 0;
volatile uint32_t g_http_fallback_count = 0;
volatile bool g_http_task_loop_seen = false;
volatile uint32_t g_bridge_service_heartbeat_ms = 0;
volatile uint32_t g_bridge_fallback_count = 0;
volatile bool g_bridge_task_loop_seen = false;
volatile uint32_t g_system_service_heartbeat_ms = 0;
volatile bool g_system_task_loop_seen = false;

constexpr UBaseType_t NOVA_HTTP_TASK_PRIORITY = 5;
constexpr UBaseType_t NOVA_AUDIO_TASK_PRIORITY = 6;
constexpr UBaseType_t NOVA_BRIDGE_TASK_PRIORITY = 3;
constexpr UBaseType_t NOVA_SYSTEM_TASK_PRIORITY = 2;
constexpr UBaseType_t NOVA_BOOT_TASK_PRIORITY = 1;

constexpr uint32_t NOVA_BOOT_TASK_STACK = 12288;
constexpr uint32_t NOVA_AUDIO_TASK_STACK = 4096;
constexpr uint32_t NOVA_BRIDGE_TASK_STACK = 6144;
constexpr uint32_t NOVA_HTTP_TASK_STACK = 8192;
constexpr uint32_t NOVA_SYSTEM_TASK_STACK = 3072;

#if CONFIG_FREERTOS_UNICORE
constexpr BaseType_t NOVA_HTTP_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_AUDIO_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_BRIDGE_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_SYSTEM_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_BOOT_TASK_CORE = ARDUINO_RUNNING_CORE;
#else
constexpr BaseType_t NOVA_NETWORK_TASK_CORE =
    (ARDUINO_RUNNING_CORE == 0) ? 1 : 0;
constexpr BaseType_t NOVA_HTTP_TASK_CORE = NOVA_NETWORK_TASK_CORE;
constexpr BaseType_t NOVA_AUDIO_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_BRIDGE_TASK_CORE = ARDUINO_RUNNING_CORE;
constexpr BaseType_t NOVA_SYSTEM_TASK_CORE = NOVA_NETWORK_TASK_CORE;
constexpr BaseType_t NOVA_BOOT_TASK_CORE = ARDUINO_RUNNING_CORE;
#endif

static constexpr uint8_t HOST_STATUS_WIFI_CONNECTED = 0x01;
static constexpr uint8_t HOST_STATUS_FIO_ACTIVE     = 0x02;
static constexpr uint8_t HOST_STATUS_FIO_ERROR      = 0x04;
static constexpr uint8_t HOST_STATUS_SD_MOUNTED     = 0x08;
static constexpr uint8_t HOST_STATUS_BOOT_READY     = 0x10;
static constexpr uint8_t HOST_STATUS_BOOT_DEGRADED  = 0x20;
static constexpr uint8_t HOST_STATUS_WIFI_CONFIGURED = 0x40;
static constexpr uint8_t HOST_STATUS_HOST_SEEN      = 0x80;

volatile uint8_t g_host_status_flags = HOST_STATUS_HOST_SEEN;
uint8_t g_last_host_status_flags = 0;
bool g_host_status_sent = false;
bool g_network_services_started = false;
volatile bool g_storage_mutation_active = false;

const char* bootPhaseName(uint8_t phase) {
    switch (phase) {
        case BOOT_PHASE_POWER_ON:    return "power_on";
        case BOOT_PHASE_INFRA_READY: return "infra_ready";
        case BOOT_PHASE_SD_MOUNT:    return "sd_mount";
        case BOOT_PHASE_SPLASH:      return "splash";
        case BOOT_PHASE_ROM_LOAD:    return "rom_load";
        case BOOT_PHASE_READY:       return "ready";
        case BOOT_PHASE_DEGRADED:    return "degraded";
        case BOOT_PHASE_FAILED:      return "failed";
        default:                     return "unknown";
    }
}

const char* novaBootPhaseName() {
    return bootPhaseName(g_boot_phase);
}

bool novaFpgaBridgeAvailable() {
    return !g_fpga_bridge_owned_by_boot;
}

bool showBootSplash();
bool loadRomsToFPGA();
void publishHostStatusToFpga(bool force);
void setHostStatusMask(uint8_t set_mask, uint8_t clear_mask, bool publish);

bool novaVmReset() {
    if (!novaFpgaBridgeAvailable())
        return false;

    logLn("VM cold boot requested via management");
    fioDispatcher.stop_audio();
    // A cold boot may be a different disk/game; drop the cached Z-sound
    // soundfont so sound_effect reloads ZSOUND.NSF from the mounted disk.
    fioDispatcher.invalidate_zsound_bank();

    if (g_sd_mounted) {
        int boot_slot = deviceManager.select_boot_slot();
        deviceManager.set_default_slot(boot_slot);
        const char* boot_prefix = DeviceManager::prefix_for_slot(boot_slot);
        logLn("VM cold boot default device=%s",
              boot_prefix ? boot_prefix : "?");
    }

    g_fpga_bridge_owned_by_boot = true;
    setHostStatusMask(0, HOST_STATUS_BOOT_READY | HOST_STATUS_BOOT_DEGRADED,
                      false);

    bool splash_ok = false;
    if (g_sd_mounted) {
        g_boot_phase = BOOT_PHASE_SPLASH;
        splash_ok = showBootSplash();
    } else {
        logLn("VM cold boot splash skipped: SD is not mounted");
    }

    g_boot_phase = BOOT_PHASE_ROM_LOAD;
    bool rom_ok = loadRomsToFPGA();
    g_boot_phase = rom_ok ? BOOT_PHASE_READY : BOOT_PHASE_DEGRADED;
    g_fpga_bridge_owned_by_boot = false;
    setHostStatusMask(rom_ok ? HOST_STATUS_BOOT_READY : HOST_STATUS_BOOT_DEGRADED,
                      rom_ok ? HOST_STATUS_BOOT_DEGRADED : HOST_STATUS_BOOT_READY,
                      false);
    publishHostStatusToFpga(true);

    if (!rom_ok) {
        logLn("VM cold boot failed: splash=%s romsLoaded=false",
              splash_ok ? "true" : "false");
        return false;
    }

    logLn("VM cold boot complete: splash=%s", splash_ok ? "true" : "false");
    return true;
}

uint8_t novaHostStatusFlags() {
    return (uint8_t)g_host_status_flags;
}

bool novaHttpTaskRunning() {
    return g_http_task != nullptr;
}

bool novaHttpTaskLoopSeen() {
    return g_http_task_loop_seen;
}

uint32_t novaHttpServiceAgeMs() {
    uint32_t heartbeat = g_http_service_heartbeat_ms;
    if (heartbeat == 0)
        return 0xFFFFFFFFUL;
    return millis() - heartbeat;
}

uint32_t novaHttpFallbackCount() {
    return g_http_fallback_count;
}

bool novaBridgeTaskRunning() {
    return g_bridge_task != nullptr;
}

bool novaBridgeTaskLoopSeen() {
    return g_bridge_task_loop_seen;
}

uint32_t novaBridgeServiceAgeMs() {
    uint32_t heartbeat = g_bridge_service_heartbeat_ms;
    if (heartbeat == 0)
        return 0xFFFFFFFFUL;
    return millis() - heartbeat;
}

uint32_t novaBridgeFallbackCount() {
    return g_bridge_fallback_count;
}

bool novaSystemTaskRunning() {
    return g_system_task != nullptr;
}

bool novaSystemTaskLoopSeen() {
    return g_system_task_loop_seen;
}

uint32_t novaSystemServiceAgeMs() {
    uint32_t heartbeat = g_system_service_heartbeat_ms;
    if (heartbeat == 0)
        return 0xFFFFFFFFUL;
    return millis() - heartbeat;
}

bool novaNicDispatcherAvailable() {
    return true;
}

bool novaStorageBusy() {
    return g_storage_mutation_active ||
           !novaFpgaBridgeAvailable() ||
           fioDispatcher.storage_busy();
}

void novaReleaseIdleAudioCacheForStorage() {
    fioDispatcher.release_idle_wts_sample_cache();
}

bool novaBeginStorageMutation() {
    if (g_storage_mutation_active ||
        !novaFpgaBridgeAvailable() ||
        fioDispatcher.storage_busy()) {
        return false;
    }
    if (!fpgaBridge.lockSharedBus())
        return false;
    g_storage_mutation_active = true;
    digitalWrite(FPGA_SPI_CS_PIN, HIGH);
    return true;
}

void novaEndStorageMutation() {
    if (!g_storage_mutation_active)
        return;
    g_storage_mutation_active = false;
    fpgaBridge.unlockSharedBus();
}

void novaNicDebugJson(char* out, size_t out_size) {
    nicDispatcher.write_debug_json(out, out_size);
}

void publishHostStatusToFpga(bool force = false) {
    uint8_t flags = (uint8_t)g_host_status_flags;
    if (!force && g_host_status_sent && flags == g_last_host_status_flags)
        return;

    if (fpgaBridge.hostStatus(flags)) {
        g_last_host_status_flags = flags;
        g_host_status_sent = true;
    } else {
        logLn("WARN: FPGA host-status LED update failed (flags=0x%02X)",
              (unsigned)flags);
    }
}

void setHostStatusMask(uint8_t set_mask, uint8_t clear_mask,
                       bool publish = true) {
    uint8_t current = (uint8_t)g_host_status_flags;
    uint8_t next = (current & (uint8_t)~clear_mask) | set_mask;
    if (next == current)
        return;

    g_host_status_flags = next;
    if (publish && novaFpgaBridgeAvailable())
        publishHostStatusToFpga(false);
}

void setHostStatusBit(uint8_t bit, bool enabled, bool publish = true) {
    setHostStatusMask(enabled ? bit : 0, enabled ? 0 : bit, publish);
}

NovaWifiManager::Config makeDefaultWifiConfig() {
    NovaWifiManager::Config cfg;
    cfg.ssid = WIFI_SSID ? String(WIFI_SSID) : String();
    cfg.password = WIFI_PASSWORD ? String(WIFI_PASSWORD) : String();
#ifdef WIFI_STATIC_IP
    cfg.useStatic = true;
    cfg.staticIp = WIFI_STATIC_IP;
    cfg.gateway = WIFI_GATEWAY;
    cfg.subnet = WIFI_SUBNET;
    cfg.dns = WIFI_DNS;
#endif
    return cfg;
}

void startNetworkServicesIfNeeded() {
    if (!novaWifi.connected() || g_network_services_started)
        return;

    logServer.begin();
    logLn("Debug log server on port %d", LOG_PORT);
    logLn("Connect with: nc %s %d",
          WiFi.localIP().toString().c_str(), LOG_PORT);

    debugServer.begin();

#if NOVA_ENABLE_OTA
    ArduinoOTA.setHostname("novahost");
    ArduinoOTA.onStart([]() {
        logLn("OTA: upload starting (%s)",
              ArduinoOTA.getCommand() == U_FLASH ? "flash" : "filesystem");
    });
    ArduinoOTA.onEnd([]() { logLn("OTA: upload complete, rebooting"); });
    ArduinoOTA.onError([](ota_error_t err) { logLn("OTA: error %u", err); });
    ArduinoOTA.begin();
    logLn("OTA enabled on %s:3232", WiFi.localIP().toString().c_str());
#else
    logLn("OTA disabled in this Novahost build");
#endif

    sdHttpServer.begin();
    logLn("HTTP server ready: http://%s/",
          WiFi.localIP().toString().c_str());
    managementServer.begin();
    logLn("Management server ready: %s:6504",
          WiFi.localIP().toString().c_str());

    g_network_services_started = true;
}

void novaWifiStateChanged() {
    wifi_connected = novaWifi.connected();

    uint8_t set_mask = 0;
    if (novaWifi.configured())
        set_mask |= HOST_STATUS_WIFI_CONFIGURED;
    if (novaWifi.connected())
        set_mask |= HOST_STATUS_WIFI_CONNECTED;

    setHostStatusMask(set_mask,
                      HOST_STATUS_WIFI_CONFIGURED |
                      HOST_STATUS_WIFI_CONNECTED);
    startNetworkServicesIfNeeded();
}

void novaHostFioActivityStarted() {
    setHostStatusMask(HOST_STATUS_FIO_ACTIVE, HOST_STATUS_FIO_ERROR);
}

void novaHostFioActivityFinished(bool ok) {
    setHostStatusMask(ok ? 0 : HOST_STATUS_FIO_ERROR,
                      HOST_STATUS_FIO_ACTIVE);
}

static const size_t BOOT_ROM_LEN       = 16 * 1024;
static const size_t SID_CURVE_ROM_LEN  = 8 * 1024;
static const uint32_t SID_CURVE_BASE   = 0x080000;
// Re-page shelf: where the active runtime's extension ROM is mirrored in XRAM so
// the 6502 loader's ensure_ext_resident can restore it into the bank-1 overlay
// after a lib_call(MODULE) page-in clobbers $C000. Must match the runtime ABI
// HOST_EXT_XRAM_L/M/H = $07C000 (runtime/asm/libabi.inc) and the emulator's
// CompositeBusDevice.LoadExtBankStatic staging.
static const uint32_t HOST_EXT_XRAM_BASE = 0x07C000;
// CPU RAM address of the resident lib_call loader band (runtime/asm/libabi.inc
// LIB_LOADER_BAND). lib_call == `JSR $0320`; the loader must live here or the
// first module call runs BRK (=$00) and warm-starts the runtime.
static const uint16_t LIB_LOADER_BAND = 0x0320;
static const uint16_t SHELF_DIR_BASE = 0x0418;   // SHELF_TAG; SHELF_LRU follows at $041C
static const size_t BOOT_LOGO_GFX_LEN  = 320UL * 200UL;
static const size_t VGC_TEXT_LEN       = 80UL * 50UL;

static const uint16_t VGC_MODE          = 0xA000;
static const uint16_t VGC_BGCOL         = 0xA001;
static const uint16_t VGC_FGCOL         = 0xA002;
static const uint16_t VGC_CURSOR_X      = 0xA003;
static const uint16_t VGC_CURSOR_Y      = 0xA004;
static const uint16_t VGC_BORDER        = 0xA00D;
static const uint16_t VGC_CURSOR_ENABLE = 0xA00A;
static const uint16_t VGC_DISPLAY_DIM   = 0xA0E5;
static const uint16_t VGC_PALETTE_INDEX = 0xA0F4;
static const uint16_t VGC_PALETTE_DATA  = 0xA0F5;
static const uint8_t  VGC_SPACE_CHAR    = 0x01;
static const uint8_t  VGC_SPACE_COLOR   = 0x02;
static const uint8_t  VGC_SPACE_GFX     = 0x03;
static const uint8_t  VGC_SPACE_TEXTATTR = 0x07;
static const uint8_t  VGC_MODE_GFX_ONLY = 0x03;
static const uint16_t NVG_HEADER_BYTES   = 16;
static const uint16_t NVG_PALETTE_BYTES  = 48;
static const uint8_t  NVG_FLAG_TRANSPARENT = 0x01;
static const uint8_t  NVG_FLAG_PALETTE     = 0x02;
static const uint8_t VGC_DEFAULT_C64_PALETTE[NVG_PALETTE_BYTES] = {
    0x00,0x00,0x00, 0xFF,0xFF,0xFF, 0x88,0x00,0x00, 0xAA,0xFF,0xEE,
    0xCC,0x44,0xCC, 0x00,0xCC,0x55, 0x00,0x00,0xAA, 0xEE,0xEE,0x77,
    0xDD,0x88,0x55, 0x66,0x44,0x00, 0xFF,0x77,0x77, 0x33,0x33,0x33,
    0x77,0x77,0x77, 0xAA,0xFF,0x66, 0x00,0x88,0xFF, 0xBB,0xBB,0xBB,
};

static BootRuntimeConfig g_runtime_config = {};
static void parseRuntimeConfigFromFile();

static const char* const BASIC_ROM_PATHS[] = {
    "/roms/novabasic.bin",
    "/roms/basic.bin",
    "/roms/ehbasic.bin"
};

static const char* const EXTENSION_ROM_PATHS[] = {
    "/roms/extension.bin",
    "/roms/ext.bin"
};

// Resident lib_call loader: the canonical 157-byte position-dependent blob
// (runtime/asm/libcall.bin, built at ORG $0320) that every runtime's lib_call
// enters via `JSR $0320`. Runtime-agnostic (one image for all runtimes).
static const char* const LIBCALL_PATHS[] = {
    "/roms/libcall.bin"
};

static const char* const SID_CURVE_PATHS[] = {
    "/assets/sid/f6581_curve.bin",
    "/assets/f6581_curve.bin"
};

static const char* const BOOT_LOGO_NVG_PATHS[] = {
    BOOT_SPLASH_DEFAULT_ASSET
};

// =========================================================================
// SD card setup and boot asset helpers
// =========================================================================
void initSharedSpiBus() {
    pinMode(SD_CS_PIN, OUTPUT);
    pinMode(FPGA_SPI_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    digitalWrite(FPGA_SPI_CS_PIN, HIGH);
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_CS_PIN);
}

bool normalizeMountPath(const char* raw, char* out, size_t out_len) {
    if (!raw || raw[0] == 0 || out_len == 0)
        return false;

    if (raw[0] == '/')
        snprintf(out, out_len, "%s", raw);
    else
        snprintf(out, out_len, "/%s", raw);
    out[out_len - 1] = 0;

    String lower(out);
    lower.toLowerCase();
    return lower.endsWith(".ndi");
}

bool writeBootConfig(JsonDocument& doc) {
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

bool mountConfiguredDrive(int slot, JsonObject mounts, bool& config_dirty) {
    const char* prefix = DeviceManager::prefix_for_slot(slot);
    if (!prefix)
        return false;

    JsonVariantConst value = mounts[prefix];
    if (value.isNull())
        return false;

    const char* configured_path = value.as<const char*>();
    char sd_path[300];
    if (!normalizeMountPath(configured_path, sd_path, sizeof(sd_path))) {
        logLn("Boot config mount cleared: %s has invalid path", prefix);
        mounts[prefix] = "";
        config_dirty = true;
        return false;
    }

    File entry = SD.open(sd_path, FILE_READ);
    if (!entry) {
        logLn("Boot config mount cleared: %s -> %s not found",
              prefix, sd_path);
        mounts[prefix] = "";
        config_dirty = true;
        return false;
    }
    entry.close();

    if (!deviceManager.mount(slot, sd_path)) {
        logLn("Boot config mount failed: %s -> %s", prefix, sd_path);
        return false;
    }

    logLn("Boot config mounted %s -> %s", prefix, sd_path);
    return true;
}

void mountConfiguredDrives(int& fd_count, int& hd_count) {
    fd_count = 0;
    hd_count = 0;

    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg) {
        logLn("Boot config missing; no disk images auto-mounted");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, cfg);
    cfg.close();
    if (err) {
        logLn("Boot config parse failed: %s; no disk images auto-mounted",
              err.c_str());
        return;
    }

    JsonObject mounts = doc["mounts"].as<JsonObject>();
    if (mounts.isNull()) {
        logLn("Boot config has no mounts object; no disk images auto-mounted");
        return;
    }

    static const int mount_order[] = {
        DeviceManager::FD0, DeviceManager::FD1,
        DeviceManager::FD2, DeviceManager::FD3,
        DeviceManager::HD0, DeviceManager::HD1
    };
    bool config_dirty = false;
    for (int slot : mount_order) {
        if (!mountConfiguredDrive(slot, mounts, config_dirty))
            continue;
        if (slot >= DeviceManager::FD0)
            fd_count++;
        else
            hd_count++;
    }

    if (config_dirty && !writeBootConfig(doc))
        logLn("WARN: failed to persist cleaned boot mount config");
}

bool mountSdCard() {
    // SD card mount via SPI mode. ULX3S routes SD to ESP32 GPIOs in a
    // way that's incompatible with SDIO 1-bit/4-bit on this board (verified
    // 2026-04-25). Pinout: SCK=GPIO14, MISO=GPIO2/sd_d[0],
    // MOSI=GPIO15/sd_cmd, CS=GPIO13/sd_d[3].
    logLn("SD: mounting via SPI (SCK=%d MISO=%d MOSI=%d CS=%d)...",
          SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_CS_PIN);
    digitalWrite(FPGA_SPI_CS_PIN, HIGH);
    g_sd_mounted = SD.begin(SD_CS_PIN, SPI, 4000000);

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

    int fd_count = 0;
    int hd_count = 0;
    mountConfiguredDrives(fd_count, hd_count);
    int boot_slot = deviceManager.select_boot_slot();
    deviceManager.set_default_slot(boot_slot);
    const char* boot_prefix = DeviceManager::prefix_for_slot(boot_slot);
    logLn("SD: config-mounted %d floppy and %d hard disk image(s); boot default=%s",
          fd_count, hd_count, boot_prefix ? boot_prefix : "?");
    char mount_buf[80];
    snprintf(mount_buf, sizeof(mount_buf), " | fd_mounts=%d hd_mounts=%d boot=%s",
             fd_count, hd_count, boot_prefix ? boot_prefix : "?");
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
    // A LARGER file is fine: a paged-library .nmod carries a documentation
    // trailer after its `expected_len`-byte image (see `nova module pack`). We
    // stream only the first expected_len bytes; the trailer stays on the SD card
    // for the nova CLI / web UI. Only a SHORT file is an error.
    if (actual < expected_len) {
        logLn("Boot asset too short: %s at %s is %u bytes, expected >= %u",
              label, path, (unsigned)actual, (unsigned)expected_len);
        f.close();
        return false;
    }

    logLn("Streaming %s from %s (%u of %u bytes)", label, path,
          (unsigned)expected_len, (unsigned)actual);
    uint8_t* buf = (uint8_t*)malloc(4096);
    if (!buf) {
        logLn("Boot asset buffer allocation failed: %s", label);
        f.close();
        return false;
    }
    for (size_t off = 0; off < expected_len; ) {
        size_t want = (expected_len - off >= 4096)
            ? 4096
            : expected_len - off;
        size_t got = f.read(buf, want);
        if (got != want) {
            logLn("Boot asset read failed: %s at offset %u", label, (unsigned)off);
            f.close();
            free(buf);
            return false;
        }
        if (!fpgaBridge.pokeSdramStream(base_addr + off, buf, (uint16_t)got)) {
            logLn("FPGA SDRAM stream failed: %s at offset %u", label, (unsigned)off);
            f.close();
            free(buf);
            return false;
        }
        off += got;
    }

    f.close();
    free(buf);
    return true;
}

// Stage paged-library modules from SD into XRAM (SDRAM) at boot. Each entry in
// the boot.json "libraries" array names a 16 KB module image; the boot-time shelf
// allocator assigns the XRAM slot base so the runtime's lib_call loader can page
// it into the bank-1 ROM window on demand.
//
// GRACEFUL DEGRADATION: this is best-effort. Missing config, parse failure, a
// missing/short file, or a stream error are all logged and skipped — the caller
// MUST continue to the reset-release regardless (an old bitstream + new firmware
// must never wedge the CPU). One bad entry does not skip the rest.
//
// Mirrors mountConfiguredDrives() for the ArduinoJson read and reuses
// streamSdramAsset() (single-path) for the SD->SDRAM stream.
int stageConfiguredLibraries() {
    File cfg = SD.open("/config/boot.json", FILE_READ);
    if (!cfg) {
        logLn("no boot libraries to stage: /config/boot.json missing");
        return 0;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, cfg);
    cfg.close();
    if (err) {
        logLn("WARN: boot libraries parse failed: %s; none staged", err.c_str());
        return 0;
    }

    JsonArray libs = doc["libraries"].as<JsonArray>();
    if (libs.isNull() || libs.size() == 0) {
        logLn("no boot libraries to stage");
        return 0;
    }

    ShelfPlan plan;
    shelf_plan_reset(&plan);
    int staged = 0;
    for (JsonObject lib : libs) {
        const char* name = lib["name"] | "";
        const char* path = lib["path"] | "";
        int      id   = lib["id"] | -1;
        uint32_t size = lib["size"] | (uint32_t)BOOT_ROM_LEN;   // .nmod image is 16K
        if (path[0] == '\0' || id < 0) {
            logLn("WARN: boot library '%s' missing path/id; skipped", name[0] ? name : "?");
            continue;
        }
        int slot = shelf_plan_add(&plan, (uint8_t)id);
        if (slot < 0) { logLn("WARN: shelf full (%d slots); '%s' skipped", SHELF_N, name); continue; }
        uint32_t base = plan.base[slot];
        if (!streamSdramAsset(base, name[0] ? name : path, &path, 1, (size_t)size)) {
            logLn("WARN: boot library '%s' (%s) not staged; continuing", name[0] ? name : "?", path);
            plan.count--; plan.tag[slot] = 0;   // roll back the failed slot
            continue;
        }
        logLn("Boot library staged: %s id=%d slot=%d (%u bytes @ XRAM $%06X)",
              name[0] ? name : path, id, slot, (unsigned)size, (unsigned)base);
        staged++;
    }

    uint8_t dir[SHELF_N * 2];
    shelf_plan_dir(&plan, dir);
    if (!fpgaBridge.pokeBlock(SHELF_DIR_BASE, dir, sizeof(dir)))
        logLn("WARN: shelf directory poke failed @ $%04X", (unsigned)SHELF_DIR_BASE);
    else
        logLn("Shelf directory seeded: %d module(s) @ RAM $%04X", staged, (unsigned)SHELF_DIR_BASE);

    logLn("Boot libraries staged: %d", staged);
    return staged;
}

// Stage the resident lib_call loader into CPU RAM at $0320 (LIB_LOADER_BAND).
// Every runtime issues a paged-library call as `JSR $0320`; the 157-byte
// position-dependent loader (runtime/asm/libcall.bin, ORG $0320) MUST be resident
// in main RAM or that JSR runs BRK (=$00) and the ROM warm-starts (turtle/graphics
// silently do nothing + the banner reprints). The emulator pokes it via
// CompositeBusDevice.PokeResidentLoader; on hardware NovaHost is the host that
// must install it. Written via a single CMD_POKE_BLOCK (main_ram, port-A debug
// poke) while the CPU is held in reset — no contention with cold_start's RAM init,
// and main_ram is a plain dpram (no reset) so it survives the system-reset pulse
// before CPU release. cold_start never touches the reserved $0320-$041F band.
// Best-effort (WARN-and-continue): a missing loader only breaks module lib_calls.
bool stageResidentLoader() {
    const char* path = nullptr;
    File f = openFirstAsset("resident lib_call loader", LIBCALL_PATHS,
                            sizeof(LIBCALL_PATHS) / sizeof(LIBCALL_PATHS[0]), path);
    if (!f) return false;

    size_t actual = f.size();
    // CMD_POKE_BLOCK caps at 256 bytes; the loader is 157. Reject anything that
    // would not fit one block (a larger file means the wrong artifact).
    if (actual == 0 || actual > 256) {
        logLn("Boot asset wrong size: lib_call loader at %s is %u bytes (expected 1..256)",
              path, (unsigned)actual);
        f.close();
        return false;
    }

    uint8_t buf[256];
    size_t got = f.read(buf, actual);
    f.close();
    if (got != actual) {
        logLn("lib_call loader read failed (%u/%u bytes)", (unsigned)got, (unsigned)actual);
        return false;
    }

    if (!fpgaBridge.pokeBlock(LIB_LOADER_BAND, buf, (uint16_t)actual)) {
        logLn("lib_call loader poke failed (CMD_POKE_BLOCK @ $%04X)",
              (unsigned)LIB_LOADER_BAND);
        return false;
    }

    logLn("Resident lib_call loader staged (%u bytes @ RAM $%04X)",
          (unsigned)actual, (unsigned)LIB_LOADER_BAND);
    return true;
}

bool holdFpgaResetForBoot(const char* label) {
    for (int attempt = 0; attempt < 20; attempt++) {
        fpgaBridge.drain();
        if (fpgaBridge.resetHold()) {
            if (attempt > 0)
                logLn("FPGA bridge responded for %s on attempt %d", label, attempt + 1);
            return true;
        }
        delay(200);
    }

    logLn("%s skipped: FPGA bridge never acked resetHold", label);
    return false;
}

bool clearVgcGfx() {
    for (uint32_t off = 0; off < BOOT_LOGO_GFX_LEN; off += 256) {
        if (!fpgaBridge.fillVgcBlock(VGC_SPACE_GFX, (uint16_t)off, 0x00)) {
            logLn("Boot splash clear failed at gfx offset %u", (unsigned)off);
            return false;
        }
    }
    return true;
}

bool pokeBootVgcBytes(uint32_t addr, const uint8_t* data, size_t len,
                      const char* label) {
    size_t off = 0;
    while (off < len) {
        size_t remaining = len - off;
        size_t chunk = remaining > 256 ? 256 : remaining;
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!fpgaBridge.pokeVgcBlock(VGC_SPACE_GFX, (uint16_t)(addr + off),
                                     data + off, wire_count)) {
            logLn("Boot splash %s VGC stream failed at gfx offset %u",
                  label, (unsigned)(addr + off));
            return false;
        }
        off += chunk;
    }
    return true;
}

bool fillVgcRange(uint8_t space, uint16_t start, size_t length,
                  uint8_t value, const char* label) {
    uint8_t tail[256];

    for (size_t off = 0; off < length; ) {
        size_t remaining = length - off;
        size_t chunk = remaining >= 256 ? 256 : remaining;
        uint16_t addr = (uint16_t)(start + off);

        bool ok;
        if (chunk == 256) {
            ok = fpgaBridge.fillVgcBlock(space, addr, value);
        } else {
            memset(tail, value, chunk);
            ok = fpgaBridge.pokeVgcBlock(space, addr, tail, (uint16_t)chunk);
        }

        if (!ok) {
            logLn("Boot text clear failed at %s offset %u",
                  label, (unsigned)off);
            return false;
        }

        off += chunk;
    }

    return true;
}

bool resetVgcDefaultPalette() {
    bool ok =
        fpgaBridge.poke(VGC_PALETTE_INDEX, 0x00);

    for (uint16_t i = 0; i < NVG_PALETTE_BYTES; i++) {
        ok = fpgaBridge.poke(VGC_PALETTE_DATA, VGC_DEFAULT_C64_PALETTE[i]) && ok;
    }

    return ok;
}

bool clearVgcText() {
    bool ok = resetVgcDefaultPalette();
    ok =
        fillVgcRange(VGC_SPACE_CHAR, 0, VGC_TEXT_LEN, 0x20, "chars") &&
        ok;
    ok =
        fillVgcRange(VGC_SPACE_COLOR, 0, VGC_TEXT_LEN, 0x0F, "colors") &&
        ok;
    ok =
        fillVgcRange(VGC_SPACE_TEXTATTR, 0, VGC_TEXT_LEN, 0x00, "attrs") &&
        ok;

    ok = fpgaBridge.poke(VGC_BGCOL, 0x00) && ok;
    ok = fpgaBridge.poke(VGC_FGCOL, 0x0F) && ok;
    ok = fpgaBridge.poke(VGC_BORDER, 0x0B) && ok;
    ok = fpgaBridge.poke(VGC_CURSOR_X, 0x00) && ok;
    ok = fpgaBridge.poke(VGC_CURSOR_Y, 0x00) && ok;

    return ok;
}

bool readByte(File& f, uint8_t& value) {
    int b = f.read();
    if (b < 0) return false;
    value = (uint8_t)b;
    return true;
}

bool readU16(File& f, uint16_t& value) {
    uint8_t lo, hi;
    if (!readByte(f, lo) || !readByte(f, hi)) return false;
    value = (uint16_t)lo | ((uint16_t)hi << 8);
    return true;
}

bool readU32(File& f, uint32_t& value) {
    uint8_t b0, b1, b2, b3;
    if (!readByte(f, b0) || !readByte(f, b1) ||
        !readByte(f, b2) || !readByte(f, b3)) return false;
    value = (uint32_t)b0 |
            ((uint32_t)b1 << 8) |
            ((uint32_t)b2 << 16) |
            ((uint32_t)b3 << 24);
    return true;
}

bool streamBootLogoNvg(File& f, const char* path) {
    uint8_t magic[4];
    if (f.read(magic, sizeof(magic)) != sizeof(magic) ||
        magic[0] != 'N' || magic[1] != 'V' ||
        magic[2] != 'G' || magic[3] != '2') {
        logLn("Boot splash invalid NVG header: %s", path);
        return false;
    }

    uint16_t width = 0, height = 0;
    uint8_t flags = 0;
    uint8_t transparent = 0;
    uint16_t payload_offset = 0;
    uint32_t payload_length = 0;
    if (!readU16(f, width) ||
        !readU16(f, height) ||
        !readByte(f, flags) ||
        !readByte(f, transparent) ||
        !readU16(f, payload_offset) ||
        !readU32(f, payload_length)) {
        logLn("Boot splash truncated NVG header: %s", path);
        return false;
    }
    if (width != 320 || height != 200 ||
        (flags & ~(NVG_FLAG_TRANSPARENT | NVG_FLAG_PALETTE)) != 0 ||
        ((flags & NVG_FLAG_TRANSPARENT) != 0 && transparent > 0x0F) ||
        payload_offset < NVG_HEADER_BYTES) {
        logLn("Boot splash wrong dimensions: %s is %ux%u", path, width, height);
        return false;
    }

    uint16_t row_packed = (uint16_t)((width + 1) / 2);
    uint32_t expected_payload = (uint32_t)row_packed * height;
    if (payload_length != expected_payload || row_packed > 160) {
        logLn("Boot splash invalid NVG payload: %s", path);
        return false;
    }

    if ((flags & NVG_FLAG_PALETTE) != 0) {
        uint8_t palette[NVG_PALETTE_BYTES];
        if (payload_offset < NVG_HEADER_BYTES + NVG_PALETTE_BYTES ||
            f.read(palette, NVG_PALETTE_BYTES) != NVG_PALETTE_BYTES) {
            logLn("Boot splash truncated NVG palette: %s", path);
            return false;
        }

        if (!fpgaBridge.poke(VGC_PALETTE_INDEX, 0x00)) {
            logLn("Boot splash palette index write failed");
            return false;
        }
        for (uint8_t i = 0; i < NVG_PALETTE_BYTES; i++) {
            if (!fpgaBridge.poke(VGC_PALETTE_DATA, palette[i])) {
                logLn("Boot splash palette data write failed at byte %u", (unsigned)i);
                return false;
            }
        }
    }

    if (!f.seek(payload_offset)) {
        logLn("Boot splash payload seek failed: %s", path);
        return false;
    }

    bool keyed = (flags & NVG_FLAG_TRANSPARENT) != 0;
    uint8_t packed[160];
    uint8_t pixels[320];
    for (uint16_t y = 0; y < height; y++) {
        if (f.read(packed, row_packed) != row_packed) {
            logLn("Boot splash truncated NVG row %u in %s", (unsigned)y, path);
            return false;
        }

        for (uint16_t x = 0; x < width; x += 2) {
            uint8_t value = packed[x / 2];
            pixels[x] = (uint8_t)(value >> 4);
            if (x + 1 < width)
                pixels[x + 1] = (uint8_t)(value & 0x0F);
        }

        uint32_t row_addr = (uint32_t)y * 320UL;
        if (!keyed) {
            if (!pokeBootVgcBytes(row_addr, pixels, width, "row"))
                return false;
            continue;
        }

        uint16_t run_start = 0;
        uint16_t run_len = 0;
        for (uint16_t x = 0; x < width; x++) {
            if (pixels[x] == transparent) {
                if (run_len != 0) {
                    if (!pokeBootVgcBytes(row_addr + run_start,
                                          pixels + run_start, run_len, "run"))
                        return false;
                    run_len = 0;
                }
                continue;
            }

            if (run_len == 0)
                run_start = x;
            run_len++;
        }
        if (run_len != 0 &&
            !pokeBootVgcBytes(row_addr + run_start,
                              pixels + run_start, run_len, "run")) {
            return false;
        }
    }

    return true;
}

BootSplashConfig readBootSplashConfig() {
    BootSplashConfig cfg;
    initBootSplashConfigDefaults(cfg);

    if (!g_sd_mounted)
        return cfg;

    File f = SD.open("/config/boot.json", FILE_READ);
    if (!f)
        return cfg;

    size_t len = f.size();
    if (len == 0 || len > BOOT_CONFIG_MAX_BYTES) {
        logLn("Boot splash config: boot.json size %u unsupported, using defaults",
              (unsigned)len);
        f.close();
        return cfg;
    }

    char buf[BOOT_CONFIG_MAX_BYTES + 1];
    size_t got = f.read((uint8_t*)buf, len);
    f.close();
    if (got != len) {
        logLn("Boot splash config: read failed (%u/%u), using defaults",
              (unsigned)got, (unsigned)len);
        return cfg;
    }
    buf[len] = '\0';

    if (!parseBootSplashConfigText(buf, len, cfg)) {
        logLn("Boot splash config: parse failed, using defaults");
        initBootSplashConfigDefaults(cfg);
    }

    return cfg;
}

bool setBootSplashVideoState(uint8_t dim, uint8_t background,
                             uint8_t border) {
    return resetVgcDefaultPalette() &&
           fpgaBridge.poke(VGC_DISPLAY_DIM, dim) &&
           fpgaBridge.poke(VGC_BGCOL, background & 0x0F) &&
           fpgaBridge.poke(VGC_BORDER, border & 0x0F) &&
           fpgaBridge.poke(VGC_CURSOR_ENABLE, 0x00) &&
           fpgaBridge.poke(VGC_MODE, VGC_MODE_GFX_ONLY);
}

void restoreBootSplashVideoState() {
    resetVgcDefaultPalette();
    fpgaBridge.poke(VGC_MODE, 0x00);
    fpgaBridge.poke(VGC_DISPLAY_DIM, 0x0F);
}

void fadeBootSplash(uint8_t from, uint8_t to, uint16_t duration_ms) {
    int step = (to >= from) ? 1 : -1;
    uint8_t steps = (from > to) ? (from - to) : (to - from);
    uint16_t delay_ms = steps == 0 ? duration_ms : duration_ms / steps;

    for (int value = from; ; value += step) {
        fpgaBridge.poke(VGC_DISPLAY_DIM, (uint8_t)value);
        if (value == to) break;
        delay(delay_ms);
    }
}

bool showBootSplash() {
    if (!g_sd_mounted) {
        logLn("Boot splash skipped: SD is not mounted");
        return false;
    }

    BootSplashConfig cfg = readBootSplashConfig();
    if (!cfg.enabled) {
        logLn("Boot splash skipped: disabled in boot.json");
        return false;
    }

    const char* fallback_path = BOOT_LOGO_NVG_PATHS[0];
    const char* paths[2];
    size_t path_count = 0;
    paths[path_count++] = cfg.assetPath[0] ? cfg.assetPath : fallback_path;
    if (strcmp(paths[0], fallback_path) != 0)
        paths[path_count++] = fallback_path;

    const char* path = nullptr;
    File f = openFirstAsset("boot splash NVG", paths, path_count, path);
    if (!f) return false;

    if (!holdFpgaResetForBoot("Boot splash")) {
        f.close();
        return false;
    }

    logLn("Boot splash: drawing %s", path);
    unsigned long t0 = millis();
    if (!setBootSplashVideoState(0, (uint8_t)cfg.background,
                                 (uint8_t)cfg.border) ||
        !clearVgcGfx()) {
        f.close();
        restoreBootSplashVideoState();
        return false;
    }

    bool ok = streamBootLogoNvg(f, path);
    f.close();
    if (!ok) {
        restoreBootSplashVideoState();
        return false;
    }

    fadeBootSplash(0, 15, (uint16_t)cfg.fadeInMs);
    delay(cfg.holdMs);
    fadeBootSplash(15, 0, (uint16_t)cfg.fadeOutMs);
    if (!clearVgcGfx())
        logLn("WARN: boot splash graphics clear failed after fade-out");
    if (!clearVgcText())
        logLn("WARN: boot splash text clear failed after fade-out");
    restoreBootSplashVideoState();

    logLn("Boot splash complete (took %lu ms)", millis() - t0);
    return true;
}

// =========================================================================
// WiFi setup
// =========================================================================
void setupWiFi() {
    novaWifi.begin(makeDefaultWifiConfig());
    novaWifiStateChanged();

    if (!novaWifi.configured()) {
        logLn("WiFi is not configured. Use USB serial: wifi set ssid ...");
        return;
    }

    NovaWifiManager::Config cfg = novaWifi.config();
    logLn("Connecting to WiFi: %s", cfg.ssid.c_str());
    bool ok = novaWifi.connect();
    novaWifiStateChanged();

    if (ok) {
        logLn("WiFi connected!");
        logLn("IP address: %s", WiFi.localIP().toString().c_str());
    } else {
        NovaWifiManager::Status st = novaWifi.status();
        logLn("WiFi connection failed: %s", st.lastError.c_str());
    }
}

String serialCommandLine;

int splitSerialArgs(const String& line, String* args, int maxArgs) {
    int count = 0;
    String current;
    bool inQuote = false;
    char quoteChar = 0;

    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        if (inQuote) {
            if (c == quoteChar) {
                inQuote = false;
            } else {
                current += c;
            }
            continue;
        }

        if (c == '"' || c == '\'') {
            inQuote = true;
            quoteChar = c;
            continue;
        }

        if (isspace((unsigned char)c)) {
            if (current.length() > 0 && count < maxArgs) {
                args[count++] = current;
                current = "";
            }
            continue;
        }

        current += c;
    }

    if (current.length() > 0 && count < maxArgs)
        args[count++] = current;
    return count;
}

void printWifiHelp() {
    Serial.println("WiFi commands:");
    Serial.println("  wifi status");
    Serial.println("  wifi scan");
    Serial.println("  wifi set ssid \"Network Name\"");
    Serial.println("  wifi set password \"secret\"");
    Serial.println("  wifi set dhcp on");
    Serial.println("  wifi set static <ip> <gateway> <subnet> <dns>");
    Serial.println("  wifi connect");
    Serial.println("  wifi disconnect");
    Serial.println("  wifi reconnect");
    Serial.println("  wifi forget");
}

void printSerialHelp() {
    printWifiHelp();
}

void printWifiStatusSerial() {
    NovaWifiManager::Status st = novaWifi.status();
    Serial.printf("configured: %s\n", st.configured ? "yes" : "no");
    Serial.printf("connected: %s\n", st.connected ? "yes" : "no");
    Serial.printf("wantConnected: %s\n", st.wantConnected ? "yes" : "no");
    Serial.printf("ssid: %s\n", st.ssid.c_str());
    Serial.printf("passwordSet: %s\n", st.passwordSet ? "yes" : "no");
    Serial.printf("mode: %s\n", st.useStatic ? "static" : "dhcp");
    if (st.useStatic) {
        Serial.printf("staticIp: %s\n", st.staticIp.c_str());
        Serial.printf("gateway: %s\n", st.gateway.c_str());
        Serial.printf("subnet: %s\n", st.subnet.c_str());
        Serial.printf("dns: %s\n", st.dns.c_str());
    }
    Serial.printf("localIp: %s\n", st.localIp.c_str());
    Serial.printf("rssi: %ld\n", (long)st.rssi);
    Serial.printf("wifiStatus: %s\n",
                  NovaWifiManager::statusName(st.wifiStatus));
    if (st.lastError.length() > 0)
        Serial.printf("lastError: %s\n", st.lastError.c_str());
}

void scanWifiSerial() {
    Serial.println("Scanning...");
    int count = WiFi.scanNetworks();
    for (int i = 0; i < count; i++) {
        Serial.printf("%2d  %4ld dBm  ch %2d  %s  %s\n",
                      i + 1,
                      (long)WiFi.RSSI(i),
                      WiFi.channel(i),
                      WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "secure",
                      WiFi.SSID(i).c_str());
    }
    if (count <= 0)
        Serial.println("No networks found.");
    WiFi.scanDelete();
}

void saveWifiConfigSerial(const NovaWifiManager::Config& cfg) {
    String error;
    if (!novaWifi.saveConfig(cfg, &error)) {
        Serial.printf("ERROR: %s\n", error.c_str());
        return;
    }
    novaWifiStateChanged();
    Serial.println("OK");
}

void handleWifiSerialCommand(String* args, int argc) {
    if (argc < 2 || args[1] == "help" || args[1] == "?") {
        printWifiHelp();
        return;
    }

    String sub = args[1];
    sub.toLowerCase();

    if (sub == "status" || sub == "show") {
        printWifiStatusSerial();
        return;
    }
    if (sub == "scan") {
        scanWifiSerial();
        return;
    }
    if (sub == "connect") {
        bool ok = novaWifi.connect();
        novaWifiStateChanged();
        Serial.println(ok ? "OK connected" : "ERROR connect failed");
        printWifiStatusSerial();
        return;
    }
    if (sub == "disconnect") {
        novaWifi.disconnect();
        novaWifiStateChanged();
        Serial.println("OK disconnected");
        return;
    }
    if (sub == "reconnect") {
        bool ok = novaWifi.reconnect();
        novaWifiStateChanged();
        Serial.println(ok ? "OK connected" : "ERROR reconnect failed");
        printWifiStatusSerial();
        return;
    }
    if (sub == "forget") {
        novaWifi.forget();
        novaWifiStateChanged();
        Serial.println("OK forgotten");
        return;
    }
    if (sub == "set") {
        if (argc < 4) {
            printWifiHelp();
            return;
        }

        NovaWifiManager::Config cfg = novaWifi.config();
        String key = args[2];
        key.toLowerCase();

        if (key == "ssid") {
            cfg.ssid = args[3];
            saveWifiConfigSerial(cfg);
            return;
        }
        if (key == "password") {
            cfg.password = args[3];
            saveWifiConfigSerial(cfg);
            return;
        }
        if (key == "dhcp") {
            String value = args[3];
            value.toLowerCase();
            cfg.useStatic = !(value == "on" || value == "true" || value == "1");
            saveWifiConfigSerial(cfg);
            return;
        }
        if (key == "static" && argc >= 7) {
            cfg.useStatic = true;
            cfg.staticIp = args[3];
            cfg.gateway = args[4];
            cfg.subnet = args[5];
            cfg.dns = args[6];
            saveWifiConfigSerial(cfg);
            return;
        }
    }

    Serial.println("Unknown command.");
    printWifiHelp();
}

void handleSerialCommand(String line) {
    line.trim();
    if (line.length() == 0)
        return;

    String args[10];
    int argc = splitSerialArgs(line, args, 10);
    if (argc == 0)
        return;

    args[0].toLowerCase();
    if (args[0] == "help" || args[0] == "?") {
        printSerialHelp();
        return;
    }
    if (args[0] == "wifi") {
        handleWifiSerialCommand(args, argc);
        return;
    }

    Serial.println("Unknown command. Type: help");
}

void pollSerialConsole() {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\r' || c == '\n') {
            if (serialCommandLine.length() > 0) {
                handleSerialCommand(serialCommandLine);
                serialCommandLine = "";
            }
            continue;
        }
        if ((unsigned char)c >= 0x20 && serialCommandLine.length() < 240)
            serialCommandLine += c;
    }
}

// =========================================================================
// Runtime config from boot.json
// =========================================================================
// Parse runtime config from boot.json using string search (no heap allocation).
// Called from loadRomsToFPGA after heap-heavy operations have freed.
static void parseRuntimeConfigFromFile() {
    g_runtime_config = {};

    File f = SD.open("/config/boot.json", FILE_READ);
    if (!f) {
        logLn("Runtime config: boot.json not found, using defaults");
        return;
    }

    size_t len = f.size();
    if (len == 0 || len > BOOT_CONFIG_MAX_BYTES) {
        logLn("Runtime config: boot.json size %u unsupported, using defaults",
              (unsigned)len);
        f.close();
        return;
    }

    char buf[BOOT_CONFIG_MAX_BYTES + 1];
    size_t got = f.read((uint8_t*)buf, len);
    f.close();
    if (got != len) {
        logLn("Runtime config: read failed (%u/%u bytes), using defaults",
              (unsigned)got, (unsigned)len);
        return;
    }
    buf[len] = '\0';

    if (!parseBootRuntimeConfigText(buf, len, g_runtime_config)) {
        logLn("Runtime config: default runtime '%s' unavailable, using defaults",
              g_runtime_config.runtime[0] ? g_runtime_config.runtime : "novabasic");
        g_runtime_config = {};
        return;
    }

    logLn("Runtime config: loading '%s' rom=%s ext=%s",
          g_runtime_config.runtime,
          g_runtime_config.romPath,
          g_runtime_config.extRomPath[0] ? g_runtime_config.extRomPath : "<none>");
}

// =========================================================================
// Boot-time ROM/assets load into FPGA via debug bridge
// =========================================================================
// Bulk bridge commands keep ROM loading to one ack per 256-byte block.
bool loadRomsToFPGA() {
    logLn("Loading boot assets from SD into FPGA...");
    unsigned long t0 = millis();

    if (!g_sd_mounted) {
        logLn("Boot assets skipped: SD is not mounted; using bitstream ROM fallback");
        fpgaBridge.resetRelease();
        return false;
    }

    // Parse runtime config directly from file (no ArduinoJson heap allocation)
    parseRuntimeConfigFromFile();
    BootRuntimeConfig& rtCfg = g_runtime_config;


    const char* romPaths[3];
    size_t romPathCount;
    const char* extPaths[2];
    size_t extPathCount;

    if (rtCfg.valid) {
        romPaths[0] = rtCfg.romPath;
        romPathCount = 1;
        if (rtCfg.extRomPath[0]) {
            extPaths[0] = rtCfg.extRomPath;
            extPathCount = 1;
        } else {
            extPathCount = 0;
        }
    } else {
        for (size_t i = 0; i < sizeof(BASIC_ROM_PATHS) / sizeof(BASIC_ROM_PATHS[0]); i++)
            romPaths[i] = BASIC_ROM_PATHS[i];
        romPathCount = sizeof(BASIC_ROM_PATHS) / sizeof(BASIC_ROM_PATHS[0]);
        for (size_t i = 0; i < sizeof(EXTENSION_ROM_PATHS) / sizeof(EXTENSION_ROM_PATHS[0]); i++)
            extPaths[i] = EXTENSION_ROM_PATHS[i];
        extPathCount = sizeof(EXTENSION_ROM_PATHS) / sizeof(EXTENSION_ROM_PATHS[0]);
    }

    bool haveBasic = bootAssetAvailable("basic ROM", romPaths, romPathCount,
                                        BOOT_ROM_LEN);
    bool haveExt = (extPathCount > 0)
        ? bootAssetAvailable("extension ROM", extPaths, extPathCount,
                             BOOT_ROM_LEN)
        : false;

    if (!haveBasic) {
        logLn("Boot assets skipped: base ROM not found; using bitstream ROM fallback");
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

        if (!clearVgcText())
            logLn("WARN: ROM load text clear failed before asset streaming");

        if (!streamRomAsset(0, "basic ROM", romPaths, romPathCount,
                            BOOT_ROM_LEN)) {
            logLn("ROM load attempt %d/%d FAILED: basic_rom streaming aborted",
                  loadAttempt, LOAD_ATTEMPTS);
            delay(250);
            continue;
        }

        if (haveExt && !streamRomAsset(1, "extension ROM", extPaths,
                                        extPathCount, BOOT_ROM_LEN)) {
            logLn("ROM load attempt %d/%d FAILED: ext_rom streaming aborted",
                  loadAttempt, LOAD_ATTEMPTS);
            delay(250);
            continue;
        }

        romsLoaded = true;

        // Stream the 6581 filter curve into SDRAM at the SID_CURVE_BASE offset
        // hardcoded in rtl/sid/sid_curve_reader.sv (25'h0_8_00_00 = $080000).
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

        // 4c.0c re-page shelf: mirror the active runtime's extension ROM into XRAM
        // at $07C000 so the 6502 ensure_ext_resident can page it back into bank 1
        // after a lib_call(MODULE) clobbers the $C000 overlay. Without it the
        // turtle/graphics still draw, but any remaining legacy extension-ROM path
        // re-pages garbage the first time it runs after a graphics lib_call.
        // Best-effort (WARN-and-continue) like the SID curve above; only matters
        // when a runtime with an extension ROM also pages in a module.
        if (haveExt) {
            if (!streamSdramAsset(HOST_EXT_XRAM_BASE, "host ext (re-page shelf)",
                                  extPaths, extPathCount, BOOT_ROM_LEN)) {
                logLn("WARN: host ext not staged at XRAM $%06X; legacy ext re-page "
                      "after a graphics lib_call will be unavailable",
                      (unsigned)HOST_EXT_XRAM_BASE);
            } else {
                logLn("Host ext staged for re-page (%u bytes @ XRAM $%06X)",
                      (unsigned)BOOT_ROM_LEN, (unsigned)HOST_EXT_XRAM_BASE);
            }
        }

        // Stage paged-library modules (optional; WARN-and-continue like the SID
        // curve). Done under the same CPU-only reset hold so bridge-owned SDRAM
        // writes are still available. Never early-returns: the loader is
        // best-effort and must not block the reset-release below.
        stageConfiguredLibraries();

        // Install the resident lib_call loader into RAM $0320 (still under
        // CPU-only reset, so no contention with cold_start's RAM init). Without
        // it every module lib_call runs BRK and warm-starts the runtime. Survives
        // the system-reset pulse below (main_ram has no reset). Best-effort.
        if (!stageResidentLoader())
            logLn("WARN: resident lib_call loader not staged; module lib_calls "
                  "(graphics/turtle) will warm-start the runtime");

        // ROM/SDRAM streaming happens under CPU-only reset so bridge-owned
        // SDRAM writes remain available. Right before releasing the CPU, pulse
        // full system reset so every custom chip returns to its power-on state.
        bool released = false;
        if (fpgaBridge.systemResetHold()) {
            delay(10);
            released = fpgaBridge.systemResetRelease();
        } else {
            released = fpgaBridge.resetRelease();
        }

        if (!released) {
            logLn("ROM load attempt %d/%d FAILED: reset release did not ack",
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
        logLn("ROM load FAILED after retries before ROM streaming completed; CPU remains held to avoid booting partial ROM");
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
                logClients[i].setNoDelay(true);
                logLn("Log viewer connected from %s",
                    logClients[i].remoteIP().toString().c_str());
                char line[80];
                writeLogClientLine(logClients[i], "=== NovaHost Debug Log ===");
                snprintf(line, sizeof(line), "Uptime: %lu ms", millis());
                writeLogClientLine(logClients[i], line);
                snprintf(line, sizeof(line), "Free heap: %d bytes",
                         ESP.getFreeHeap());
                writeLogClientLine(logClients[i], line);
                return;
            }
        }
        // No free slots — reject
        WiFiClient reject = logServer.available();
        writeLogClientLine(reject, "Too many log viewers");
        reject.stop();
    }
}

// =========================================================================
// Setup
// =========================================================================
void runBootSequence() {
    g_fpga_bridge_owned_by_boot = true;

    g_boot_phase = BOOT_PHASE_SD_MOUNT;
    bool sd_ok = mountSdCard();
    setHostStatusBit(HOST_STATUS_SD_MOUNTED, sd_ok, false);
    publishHostStatusToFpga(true);

    if (sd_ok) {
        // Optional SD-resident boot logo. CPU remains held in reset while
        // NovaHost writes the VGC graphics plane, fades it in/out, then ROM
        // loading below releases into the configured runtime.
        g_boot_phase = BOOT_PHASE_SPLASH;
        showBootSplash();
    } else {
        logLn("Boot splash skipped: SD is not mounted");
    }

    // Stream ROM/assets into FPGA. FPGA's debug_bridge boots with
    // dbg_cpu_reset=1, so the CPU stays held in reset until we've loaded
    // assets + released. If loading is skipped, resetRelease() falls back to
    // the ROM contents initialized by the bitstream.
    g_boot_phase = BOOT_PHASE_ROM_LOAD;
    bool rom_ok = loadRomsToFPGA();

    g_fpga_bridge_owned_by_boot = false;
    g_boot_phase = rom_ok ? BOOT_PHASE_READY : BOOT_PHASE_DEGRADED;
    setHostStatusMask(rom_ok ? HOST_STATUS_BOOT_READY : HOST_STATUS_BOOT_DEGRADED,
                      rom_ok ? HOST_STATUS_BOOT_DEGRADED : HOST_STATUS_BOOT_READY,
                      false);
    publishHostStatusToFpga(true);
    logLn("NovaHost boot sequence finished: %s", novaBootPhaseName());
}

void bootTaskMain(void*) {
    runBootSequence();
    g_boot_task = nullptr;
    vTaskDelete(nullptr);
}

void startBootTask() {
    g_fpga_bridge_owned_by_boot = true;
    BaseType_t ok = xTaskCreatePinnedToCore(
        bootTaskMain,
        "nova-boot",
        NOVA_BOOT_TASK_STACK,
        nullptr,
        NOVA_BOOT_TASK_PRIORITY,
        &g_boot_task,
        NOVA_BOOT_TASK_CORE);

    if (ok != pdPASS) {
        g_boot_task = nullptr;
        g_fpga_bridge_owned_by_boot = false;
        g_boot_phase = BOOT_PHASE_FAILED;
        logLn("NovaHost boot sequence FAILED: could not create boot task");
    }
}

void audioTaskMain(void*) {
    const TickType_t period = pdMS_TO_TICKS(1);

    while (true) {
        vTaskDelay(period);
        if (novaFpgaBridgeAvailable() && !g_storage_mutation_active) {
            fioDispatcher.pump_audio();
        }
    }
}

void startAudioTask() {
    BaseType_t ok = xTaskCreatePinnedToCore(
        audioTaskMain,
        "nova-audio",
        NOVA_AUDIO_TASK_STACK,
        nullptr,
        NOVA_AUDIO_TASK_PRIORITY,
        &g_audio_task,
        NOVA_AUDIO_TASK_CORE);

    if (ok != pdPASS) {
        g_audio_task = nullptr;
        logLn("NovaHost audio task FAILED: could not create task");
    }
}

void serviceBridgeControlPlane() {
    g_bridge_service_heartbeat_ms = millis();

    if (g_storage_mutation_active)
        return;

    if (novaWifi.connected() && g_network_services_started &&
        novaFpgaBridgeAvailable()) {
        debugServer.loop();
    }

    if (!novaFpgaBridgeAvailable())
        return;

    uint32_t now = millis();
    bool audio_active = fioDispatcher.audio_active();

    static uint32_t last_bridge_drain_ms = 0;
    uint32_t drain_period_ms = audio_active ? 10 : 1;
    if ((uint32_t)(now - last_bridge_drain_ms) >= drain_period_ms) {
        last_bridge_drain_ms = now;
        fpgaBridge.drain();
    }

    static uint32_t last_fio_poll_ms = 0;
    uint32_t fio_poll_period_ms = audio_active ? 20 : 5;
    if ((uint32_t)(now - last_fio_poll_ms) >= fio_poll_period_ms) {
        last_fio_poll_ms = now;
        fioDispatcher.poll_pending();
    }

    static uint32_t last_nic_poll_ms = 0;
    uint32_t nic_poll_period_ms = audio_active ? 10 : 1;
    if ((uint32_t)(now - last_nic_poll_ms) >= nic_poll_period_ms) {
        last_nic_poll_ms = now;
        nicDispatcher.poll();
    }

    static uint32_t lastHostStatusRefresh = 0;
    if ((uint32_t)(now - lastHostStatusRefresh) > 1000) {
        lastHostStatusRefresh = now;
        publishHostStatusToFpga(true);
    }
}

void bridgeTaskMain(void*) {
    g_bridge_task_loop_seen = true;
    while (true) {
        serviceBridgeControlPlane();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void startBridgeTask() {
    BaseType_t ok = xTaskCreatePinnedToCore(
        bridgeTaskMain,
        "nova-bridge",
        NOVA_BRIDGE_TASK_STACK,
        nullptr,
        NOVA_BRIDGE_TASK_PRIORITY,
        &g_bridge_task,
        NOVA_BRIDGE_TASK_CORE);

    if (ok != pdPASS) {
        g_bridge_task = nullptr;
        logLn("NovaHost bridge task FAILED: could not create task");
    } else {
        logLn("NovaHost bridge task started");
    }
}

void serviceHttpControlPlane() {
    g_http_service_heartbeat_ms = millis();

    if (novaWifi.connected() && g_network_services_started) {
#if NOVA_ENABLE_OTA
        ArduinoOTA.handle();
#endif
        handleLogClients();
        managementServer.loop();
        if (novaFpgaBridgeAvailable()) {
            sdHttpServer.loop();
        }
    }
}

void httpTaskMain(void*) {
    g_http_task_loop_seen = true;
    while (true) {
        serviceHttpControlPlane();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void startHttpTask() {
    BaseType_t ok = xTaskCreatePinnedToCore(
        httpTaskMain,
        "nova-http",
        NOVA_HTTP_TASK_STACK,
        nullptr,
        NOVA_HTTP_TASK_PRIORITY,
        &g_http_task,
        NOVA_HTTP_TASK_CORE);

    if (ok != pdPASS) {
        g_http_task = nullptr;
        logLn("NovaHost HTTP task FAILED: could not create task");
    } else {
        logLn("NovaHost HTTP task started");
    }
}

void serviceSystemControlPlane() {
    g_system_service_heartbeat_ms = millis();

    pollSerialConsole();

    static uint32_t lastWifiCheck = 0;
    novaWifi.loop();
    if ((uint32_t)(millis() - lastWifiCheck) > 10000) {
        lastWifiCheck = millis();
        bool now_connected = novaWifi.connected();
        if (now_connected != wifi_connected) {
            novaWifiStateChanged();
            if (now_connected) {
                logLn("WiFi reconnected: %s",
                      WiFi.localIP().toString().c_str());
            } else {
                logLn("WiFi lost");
            }
        } else {
            novaWifiStateChanged();
        }
    }
}

void systemTaskMain(void*) {
    g_system_task_loop_seen = true;
    while (true) {
        serviceSystemControlPlane();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void startSystemTask() {
    BaseType_t ok = xTaskCreatePinnedToCore(
        systemTaskMain,
        "nova-system",
        NOVA_SYSTEM_TASK_STACK,
        nullptr,
        NOVA_SYSTEM_TASK_PRIORITY,
        &g_system_task,
        NOVA_SYSTEM_TASK_CORE);

    if (ok != pdPASS) {
        g_system_task = nullptr;
        logLn("NovaHost system task FAILED: could not create task");
    } else {
        logLn("NovaHost system task started");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== NovaHost v0.3 ===");
    Serial.printf("Chip: %s rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("CPU: %d MHz, %d cores\n", ESP.getCpuFreqMHz(), ESP.getChipCores());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));

    fioDispatcher.reserve_wts_region_store();

    initSharedSpiBus();
    fpgaBridge.beginSpi(SPI, FPGA_SPI_CS_PIN, FPGA_SPI_HZ, SD_CS_PIN);
    Serial.printf("FPGA bridge initialized over SPI (SCK=%d MISO=%d MOSI=%d CS=%d, write %lu/%lu Hz, read %lu/%lu Hz)\n",
                  SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN,
                  FPGA_SPI_CS_PIN,
                  (unsigned long)fpgaBridge.requestedWriteHz(),
                  (unsigned long)fpgaBridge.actualWriteHz(),
                  (unsigned long)fpgaBridge.requestedReadHz(),
                  (unsigned long)fpgaBridge.actualReadHz());
    // Give the FPGA debug bridge a moment to settle before we start poking it.
    delay(100);

    // Wire host-service event reader → dispatchers. FIO events execute
    // immediately; NIC events are marked pending and serviced from loop()
    // so socket polling and bridge traffic stay ordered.
    fioEventReader.onFioEvent(FioDispatcher::onFioEventStatic, &fioDispatcher);
    fioEventReader.onNicEvent(NicDispatcher::onNicEventStatic, &nicDispatcher);
    fpgaBridge.onDrainByte(FioEventReader::onDrainByteStatic, &fioEventReader);

    // Critical infrastructure comes first. The board should be reachable over
    // WiFi even if SD mounting, FPGA bridge handshaking, splash drawing, or ROM
    // loading fails later.
    setupWiFi();
    publishHostStatusToFpga(true);

    g_boot_phase = BOOT_PHASE_INFRA_READY;
    logLn("NovaHost infrastructure ready; starting SD/FPGA boot task.");
    startSystemTask();
    startHttpTask();
    startBridgeTask();
    startBootTask();
    startAudioTask();
}

void novaAudioStatusJson(char* out, size_t out_len) {
    fioDispatcher.write_audio_status_json(out, out_len);
}

void novaAudioStop() {
    fioDispatcher.stop_audio();
}

// =========================================================================
// Main loop
// =========================================================================
void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
