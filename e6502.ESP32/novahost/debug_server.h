// DebugServer — TCP JSON protocol server for FPGA test infrastructure
// Listens on port 6503, accepts one client at a time.
// JSON command protocol matches sim_main.cpp for test compatibility.

#ifndef DEBUG_SERVER_H
#define DEBUG_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include "fpga_bridge.h"

class DebugServer {
public:
    DebugServer(FpgaBridge& bridge, uint16_t port = 6503);

    void begin();
    void loop();

private:
    FpgaBridge& _bridge;
    WiFiServer  _server;
    WiFiClient  _client;
    String      _rxBuf;
    bool        _paused;

    // Async operation state (wait_ready / watch)
    enum AsyncOp { ASYNC_NONE, ASYNC_WAIT_READY, ASYNC_WATCH };
    AsyncOp       _asyncOp;
    unsigned long _asyncDeadline;
    unsigned long _asyncNextPoll;
    String        _asyncText;        // wait_ready: text to find
    uint16_t      _asyncAddr;        // watch: address
    uint8_t       _asyncValue;       // watch: expected value

    // Shared screen buffer
    uint8_t _screenBuf[2000];

    // Command dispatch
    void handleCommand(const String& json);
    void handleAsync();

    // Command handlers
    void cmdPeek(const String& json);
    void cmdPoke(const String& json);
    void cmdSendKey(const String& json);
    void cmdTypeText(const String& json);
    void cmdReadScreen();
    void cmdReadLine(const String& json);
    void cmdGetCursor();
    void cmdDbgState();
    void cmdDbgPause();
    void cmdDbgResume();
    void cmdColdStart();
    void cmdWaitReady(const String& json);
    void cmdWatch(const String& json);
    void cmdRunCycles(const String& json);

    // Response helpers
    void respond(const char* json);
    void respondOk();
    void respondError(const char* msg);

    // JSON extraction
    static String extractString(const String& json, const char* key);
    static int    extractInt(const String& json, const char* key, int defaultVal = -1);

    // Screen helpers
    int  findTextOnScreen(const char* text);
    void formatScreenLine(int row, char* out, int maxLen);
};

#endif
