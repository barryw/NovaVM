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
    static constexpr int SCREEN_COLS = 80;
    static constexpr int SCREEN_ROWS = 50;
    static constexpr int SCREEN_BYTES = SCREEN_COLS * SCREEN_ROWS;

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
    uint8_t _screenBuf[SCREEN_BYTES];

    // Command dispatch
    void handleCommand(const String& json);
    void handleAsync();

    // Command handlers
    void cmdPeek(const String& json);
    void cmdPeekBlock(const String& json);
    void cmdReadSdram(const String& json);
    void cmdWriteSdram(const String& json);
    void cmdPoke(const String& json);
    void cmdReadVram(const String& json);
    void cmdFillVram(const String& json);
    void cmdSendKey(const String& json);
    void cmdTypeText(const String& json);
    void cmdReadScreen();
    void cmdReadLine(const String& json);
    void cmdGetCursor();
    void cmdDbgState();
    void cmdDbgPause();
    void cmdDbgResume();
    void cmdDbgStep();
    void cmdDbgBreakSet(const String& json);
    void cmdDbgBreakClear(const String& json);
    void cmdDbgBreakClearAll();
    void cmdDbgBreakList();
    void cmdDbgTrace(const String& json);
    void cmdColdStart(const String& json);
    void cmdWaitReady(const String& json);
    void cmdWatch(const String& json);
    void cmdRunCycles(const String& json);
    void cmdReloadRom();

    // Response helpers
    void respond(const char* json);
    void respondOk();
    void respondError(const char* msg);

    // JSON extraction
    static String extractString(const String& json, const char* key);
    static int    extractInt(const String& json, const char* key, int defaultVal = -1);
    static const char* jsonPayload(const String& json);

    // Screen helpers
    int  findTextOnScreen(const char* text);
    void formatScreenLine(int row, char* out, int maxLen);
};

#endif
