// NovaHost — ESP32 host services for the e6502 FPGA computer
// Communicates with FPGA via UART (Serial2: wifi_rxd/wifi_txd)
// Provides: WiFi, SD card FAT32, Bluetooth keyboard, MIDI, MCP server
//
// Debug logs streamed over WiFi: connect with `nc <ip> 23`
// Board: ESP32 on ULX3S (select WEMOS LOLIN32 in Arduino IDE)

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <HardwareSerial.h>

// =========================================================================
// Configuration
// =========================================================================

// WiFi credentials — kept in wifi_config.h (gitignored)
#include "wifi_config.h"

// FPGA UART — uses ESP32 Serial2 (GPIO16=RX from FPGA, GPIO17=TX to FPGA)
#define FPGA_SERIAL Serial2
#define FPGA_BAUD   115200
#define FPGA_RX_PIN 16
#define FPGA_TX_PIN 17

// Debug log server — telnet-style TCP on port 23
#define LOG_PORT 23

// Protocol: newline-delimited JSON messages (same as Avalonia TCP protocol)
// FPGA sends: {"cmd":"load","path":"HELLO.BAS"}
// ESP32 replies: {"ok":true,"size":123,"data":"..."}

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
// State
// =========================================================================
bool wifi_connected = false;
String fpga_rx_buf = "";

// =========================================================================
// WiFi setup
// =========================================================================
void setupWiFi() {
    logLn("Connecting to WiFi: %s", WIFI_SSID);
    WiFi.mode(WIFI_STA);
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
            logLn("mDNS: novahost.local");
        }

        // Start log server
        logServer.begin();
        logLn("Debug log server on port %d", LOG_PORT);
        logLn("Connect with: nc %s %d", WiFi.localIP().toString().c_str(), LOG_PORT);
    } else {
        Serial.println("\nWiFi connection failed — running without network");
    }
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
    Serial.println("\n=== NovaHost v0.2 ===");
    Serial.printf("Chip: %s rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("CPU: %d MHz, %d cores\n", ESP.getCpuFreqMHz(), ESP.getChipCores());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));

    // FPGA UART
    FPGA_SERIAL.begin(FPGA_BAUD, SERIAL_8N1, FPGA_RX_PIN, FPGA_TX_PIN);
    Serial.println("FPGA UART initialized on GPIO16/17");

    // WiFi + debug log server
    setupWiFi();

    // Send ready signal to FPGA
    FPGA_SERIAL.println("{\"event\":\"ready\",\"version\":\"0.2\"}");

    logLn("NovaHost ready.");
}

// =========================================================================
// Main loop
// =========================================================================
void loop() {
    // Accept new log viewer connections
    if (wifi_connected) {
        handleLogClients();
    }

    // Read commands from FPGA
    while (FPGA_SERIAL.available()) {
        char c = FPGA_SERIAL.read();
        if (c == '\n') {
            if (fpga_rx_buf.length() > 0) {
                processCommand(fpga_rx_buf);
                fpga_rx_buf = "";
            }
        } else if (c != '\r') {
            fpga_rx_buf += c;
        }
    }

    // Check WiFi reconnection
    static unsigned long lastWifiCheck = 0;
    if (wifi_connected && WiFi.status() != WL_CONNECTED && millis() - lastWifiCheck > 10000) {
        lastWifiCheck = millis();
        logLn("WiFi lost — reconnecting...");
        WiFi.reconnect();
    }
}

// =========================================================================
// Command processor
// =========================================================================
void processCommand(const String& json) {
    logLn("FPGA> %s", json.c_str());

    if (json.indexOf("\"ping\"") >= 0) {
        FPGA_SERIAL.println("{\"ok\":true,\"pong\":true}");
        logLn("  -> pong");
    }
    else if (json.indexOf("\"info\"") >= 0) {
        FPGA_SERIAL.printf("{\"ok\":true,\"heap\":%d,\"wifi\":%s,\"ip\":\"%s\"}\n",
            ESP.getFreeHeap(),
            wifi_connected ? "true" : "false",
            wifi_connected ? WiFi.localIP().toString().c_str() : "none");
        logLn("  -> info sent");
    }
    else {
        FPGA_SERIAL.println("{\"ok\":false,\"error\":\"unknown command\"}");
        logLn("  -> unknown command");
    }
}
