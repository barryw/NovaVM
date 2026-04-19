// DebugServer — JSON command protocol matching sim_main.cpp
// Test suites connect to novahost.local:6503 and speak the same
// line-delimited JSON as the Verilator simulator.

#include "debug_server.h"

// VGC register addresses
#define REG_CURSOR_X 0xA003
#define REG_CURSOR_Y 0xA004

// Logging (defined in novahost.ino)
extern void logLn(const char* fmt, ...);

// =========================================================================
// Construction and setup
// =========================================================================

DebugServer::DebugServer(FpgaBridge& bridge, uint16_t port)
    : _bridge(bridge), _server(port), _paused(false),
      _asyncOp(ASYNC_NONE), _asyncDeadline(0), _asyncNextPoll(0) {}

void DebugServer::begin() {
    _server.begin();
    logLn("Debug server listening on port 6503");
}

// =========================================================================
// Main loop — accept clients, read JSON lines, dispatch commands
// =========================================================================

void DebugServer::loop() {
    // Accept new client (one at a time)
    if (!_client || !_client.connected()) {
        WiFiClient newClient = _server.available();
        if (newClient) {
            _client = newClient;
            _rxBuf = "";
            _asyncOp = ASYNC_NONE;
            logLn("Debug client connected from %s",
                  _client.remoteIP().toString().c_str());
        }
    }

    if (!_client || !_client.connected()) return;

    // Handle pending async operation
    if (_asyncOp != ASYNC_NONE) {
        handleAsync();
        return;
    }

    // Read incoming data, process complete JSON lines
    while (_client.available()) {
        char c = _client.read();
        if (c == '\n') {
            if (_rxBuf.length() > 0) {
                handleCommand(_rxBuf);
                _rxBuf = "";
            }
        } else if (c != '\r') {
            _rxBuf += c;
        }
    }
}

// =========================================================================
// Command dispatch
// =========================================================================

void DebugServer::handleCommand(const String& json) {
    String cmd = extractString(json, "command");

    if      (cmd == "peek")        cmdPeek(json);
    else if (cmd == "poke")        cmdPoke(json);
    else if (cmd == "send_key")    cmdSendKey(json);
    else if (cmd == "type_text")   cmdTypeText(json);
    else if (cmd == "read_screen") cmdReadScreen();
    else if (cmd == "read_line")   cmdReadLine(json);
    else if (cmd == "get_cursor")  cmdGetCursor();
    else if (cmd == "dbg_state")   cmdDbgState();
    else if (cmd == "dbg_pause")   cmdDbgPause();
    else if (cmd == "dbg_resume")  cmdDbgResume();
    else if (cmd == "cold_start")  cmdColdStart();
    else if (cmd == "wait_ready")  cmdWaitReady(json);
    else if (cmd == "watch")       cmdWatch(json);
    else if (cmd == "run_cycles")  cmdRunCycles(json);
    else {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "{\"ok\":false,\"error\":\"Unknown command: %s\"}", cmd.c_str());
        respond(buf);
    }
}

// =========================================================================
// Command handlers — Priority 1 (direct FPGA operations)
// =========================================================================

void DebugServer::cmdPeek(const String& json) {
    int addr = extractInt(json, "address");
    if (addr < 0) { respondError("Missing 'address'"); return; }
    uint8_t val;
    if (!_bridge.peek((uint16_t)addr, val)) {
        respondError("FPGA peek failed");
        return;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "{\"ok\":true,\"address\":%d,\"value\":%d}", addr, val);
    respond(buf);
}

void DebugServer::cmdPoke(const String& json) {
    int addr = extractInt(json, "address");
    int val  = extractInt(json, "value");
    if (addr < 0) { respondError("Missing 'address'"); return; }
    if (val  < 0) { respondError("Missing 'value'"); return; }
    if (!_bridge.poke((uint16_t)addr, (uint8_t)val)) {
        respondError("FPGA poke failed");
        return;
    }
    respondOk();
}

void DebugServer::cmdSendKey(const String& json) {
    String key = extractString(json, "key");
    uint8_t code = 0;
    if (key == "ENTER" || key == "CR" || key == "RETURN") code = 0x0D;
    else if (key == "BACKSPACE" || key == "BS")           code = 0x08;
    else if (key == "CTRL-C" || key == "BREAK")           code = 0x03;
    else if (key.length() == 1)                           code = key[0];
    else { respondOk(); return; }  // unknown key name — ignore

    if (!_bridge.sendKey(code)) { respondError("FPGA send_key failed"); return; }
    respondOk();
}

void DebugServer::cmdTypeText(const String& json) {
    String text = extractString(json, "text");
    for (unsigned int i = 0; i < text.length(); i++) {
        _bridge.sendKey((uint8_t)text[i]);
        delay(1);  // give screen editor time to process
    }
    respondOk();
}

void DebugServer::cmdReadScreen() {
    if (!_bridge.readScreen(_screenBuf)) {
        respondError("FPGA read_screen failed");
        return;
    }

    // Read cursor position
    uint8_t cx = 0, cy = 0;
    _bridge.peek(REG_CURSOR_X, cx);
    _bridge.peek(REG_CURSOR_Y, cy);

    // Build JSON response with 25 lines
    String resp;
    resp.reserve(3000);
    resp = "{\"ok\":true,\"lines\":[";

    char line[82];
    for (int row = 0; row < 25; row++) {
        formatScreenLine(row, line, sizeof(line));
        if (row > 0) resp += ',';
        resp += '"';
        // Escape special JSON characters
        for (int i = 0; line[i]; i++) {
            char ch = line[i];
            if (ch == '"')       resp += "\\\"";
            else if (ch == '\\') resp += "\\\\";
            else                 resp += ch;
        }
        resp += '"';
    }

    char tail[48];
    snprintf(tail, sizeof(tail), "],\"cursor_x\":%d,\"cursor_y\":%d}", cx, cy);
    resp += tail;

    respond(resp.c_str());
}

void DebugServer::cmdReadLine(const String& json) {
    int row = extractInt(json, "row");
    if (row < 0) {
        // Default to cursor Y
        uint8_t cy = 0;
        _bridge.peek(REG_CURSOR_Y, cy);
        row = cy;
    }
    if (row < 0 || row >= 25) { respondError("Invalid row"); return; }

    if (!_bridge.readScreen(_screenBuf)) {
        respondError("FPGA read_screen failed");
        return;
    }

    char line[82];
    formatScreenLine(row, line, sizeof(line));

    String resp;
    resp.reserve(160);
    resp = "{\"ok\":true,\"row\":";
    resp += String(row);
    resp += ",\"text\":\"";
    for (int i = 0; line[i]; i++) {
        char ch = line[i];
        if (ch == '"')       resp += "\\\"";
        else if (ch == '\\') resp += "\\\\";
        else                 resp += ch;
    }
    resp += "\"}";
    respond(resp.c_str());
}

void DebugServer::cmdGetCursor() {
    uint8_t cx = 0, cy = 0;
    _bridge.peek(REG_CURSOR_X, cx);
    _bridge.peek(REG_CURSOR_Y, cy);
    char buf[48];
    snprintf(buf, sizeof(buf), "{\"ok\":true,\"x\":%d,\"y\":%d}", cx, cy);
    respond(buf);
}

void DebugServer::cmdDbgState() {
    FpgaBridge::CpuState st;
    if (!_bridge.cpuState(st)) { respondError("FPGA cpu_state failed"); return; }
    char buf[192];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"a\":%d,\"x\":%d,\"y\":%d,\"sp\":%d,\"pc\":%d,"
        "\"nf\":%d,\"vf\":%d,\"df\":%d,\"if\":%d,\"zf\":%d,\"cf\":%d,"
        "\"paused\":%s}",
        st.a, st.x, st.y, st.sp, st.pc,
        (st.flags >> 7) & 1,   // N
        (st.flags >> 6) & 1,   // V
        (st.flags >> 3) & 1,   // D
        (st.flags >> 2) & 1,   // I
        (st.flags >> 1) & 1,   // Z
        st.flags & 1,          // C
        _paused ? "true" : "false");
    respond(buf);
}

void DebugServer::cmdDbgPause() {
    if (!_bridge.pause()) { respondError("FPGA pause failed"); return; }
    _paused = true;
    // Return CPU state (matches sim_main behavior)
    cmdDbgState();
}

void DebugServer::cmdDbgResume() {
    if (!_bridge.resume()) { respondError("FPGA resume failed"); return; }
    _paused = false;
    respondOk();
}

// =========================================================================
// Command handlers — Priority 2 (ESP32-side logic)
// =========================================================================

void DebugServer::cmdColdStart() {
    // Resume if paused
    _bridge.resume();
    _paused = false;

    // Send CTRL-C to break any running program
    _bridge.sendKey(0x03);
    delay(100);

    // Wait for "Ready" prompt (up to 5 seconds)
    unsigned long deadline = millis() + 5000;
    while (millis() < deadline) {
        if (_bridge.readScreen(_screenBuf)) {
            if (findTextOnScreen("Ready") >= 0) {
                respondOk();
                return;
            }
        }
        delay(50);
    }

    // Timed out but still report OK (system may be in a different state)
    respondOk();
}

void DebugServer::cmdWaitReady(const String& json) {
    String text = extractString(json, "text");
    if (text.length() == 0) text = "Ready";
    int timeout = extractInt(json, "timeout_ms", 5000);

    // Check immediately
    if (_bridge.readScreen(_screenBuf)) {
        int row = findTextOnScreen(text.c_str());
        if (row >= 0) {
            char buf[64];
            snprintf(buf, sizeof(buf), "{\"ok\":true,\"found\":true,\"row\":%d}", row);
            respond(buf);
            return;
        }
    }

    // Start async polling
    _asyncOp = ASYNC_WAIT_READY;
    _asyncText = text;
    _asyncDeadline = millis() + timeout;
    _asyncNextPoll = millis() + 50;
}

void DebugServer::cmdWatch(const String& json) {
    int addr  = extractInt(json, "address");
    int value = extractInt(json, "value");
    if (addr < 0)  { respondError("Missing 'address'"); return; }
    if (value < 0) { respondError("Missing 'value'"); return; }
    int timeout = extractInt(json, "timeout_ms", 5000);

    // Check immediately
    uint8_t actual;
    if (_bridge.peek((uint16_t)addr, actual) && actual == (uint8_t)value) {
        char buf[96];
        snprintf(buf, sizeof(buf),
            "{\"ok\":true,\"matched\":true,\"address\":%d,\"expected\":%d,\"actual\":%d}",
            addr, value, actual);
        respond(buf);
        return;
    }

    // Start async polling
    _asyncOp = ASYNC_WATCH;
    _asyncAddr = (uint16_t)addr;
    _asyncValue = (uint8_t)value;
    _asyncDeadline = millis() + timeout;
    _asyncNextPoll = millis() + 10;
}

void DebugServer::cmdRunCycles(const String& json) {
    int cycles = extractInt(json, "cycles");
    if (cycles < 0) { respondError("Missing 'cycles'"); return; }

    // Approximate: resume, delay, pause, return state
    _bridge.resume();
    // At 25 MHz, 1 cycle = 40ns. Delay in microseconds.
    unsigned long us = (unsigned long)cycles / 25;
    if (us < 1) us = 1;
    if (us > 1000000) us = 1000000;
    delayMicroseconds(us);
    _bridge.pause();
    _paused = true;

    FpgaBridge::CpuState st;
    _bridge.cpuState(st);

    char buf[128];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"cycles_executed\":%d,\"a\":%d,\"x\":%d,\"y\":%d,"
        "\"sp\":%d,\"pc\":%d,\"paused\":true}",
        cycles, st.a, st.x, st.y, st.sp, st.pc);
    respond(buf);
}

// =========================================================================
// Async polling handler
// =========================================================================

void DebugServer::handleAsync() {
    if (!_client || !_client.connected()) {
        _asyncOp = ASYNC_NONE;
        return;
    }

    if (millis() < _asyncNextPoll) return;

    switch (_asyncOp) {
        case ASYNC_WAIT_READY: {
            if (_bridge.readScreen(_screenBuf)) {
                int row = findTextOnScreen(_asyncText.c_str());
                if (row >= 0) {
                    char buf[64];
                    snprintf(buf, sizeof(buf),
                             "{\"ok\":true,\"found\":true,\"row\":%d}", row);
                    respond(buf);
                    _asyncOp = ASYNC_NONE;
                    return;
                }
            }
            if (millis() >= _asyncDeadline) {
                respond("{\"ok\":true,\"found\":false}");
                _asyncOp = ASYNC_NONE;
                return;
            }
            _asyncNextPoll = millis() + 50;
            break;
        }

        case ASYNC_WATCH: {
            uint8_t actual = 0;
            if (_bridge.peek(_asyncAddr, actual)) {
                if (actual == _asyncValue) {
                    char buf[96];
                    snprintf(buf, sizeof(buf),
                        "{\"ok\":true,\"matched\":true,\"address\":%d,"
                        "\"expected\":%d,\"actual\":%d}",
                        _asyncAddr, _asyncValue, actual);
                    respond(buf);
                    _asyncOp = ASYNC_NONE;
                    return;
                }
            }
            if (millis() >= _asyncDeadline) {
                char buf[96];
                snprintf(buf, sizeof(buf),
                    "{\"ok\":true,\"matched\":false,\"address\":%d,"
                    "\"expected\":%d,\"actual\":%d}",
                    _asyncAddr, _asyncValue, actual);
                respond(buf);
                _asyncOp = ASYNC_NONE;
                return;
            }
            _asyncNextPoll = millis() + 10;
            break;
        }

        default:
            _asyncOp = ASYNC_NONE;
            break;
    }
}

// =========================================================================
// Response helpers
// =========================================================================

void DebugServer::respond(const char* json) {
    if (_client && _client.connected()) {
        _client.println(json);
    }
}

void DebugServer::respondOk() {
    respond("{\"ok\":true}");
}

void DebugServer::respondError(const char* msg) {
    char buf[128];
    snprintf(buf, sizeof(buf), "{\"ok\":false,\"error\":\"%s\"}", msg);
    respond(buf);
}

// =========================================================================
// JSON extraction helpers
// =========================================================================

String DebugServer::extractString(const String& json, const char* key) {
    // Find "key" then the colon, then optional whitespace, then the quoted value.
    String needle = String("\"") + key + "\"";
    int idx = json.indexOf(needle);
    if (idx < 0) return "";
    idx += needle.length();
    // Skip whitespace before colon
    while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\t')) idx++;
    if (idx >= (int)json.length() || json[idx] != ':') return "";
    idx++;
    // Skip whitespace after colon
    while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\t')) idx++;
    if (idx >= (int)json.length() || json[idx] != '"') return "";
    idx++;
    int end = json.indexOf('"', idx);
    if (end < 0) return "";
    return json.substring(idx, end);
}

int DebugServer::extractInt(const String& json, const char* key, int defaultVal) {
    // Find "key" then colon, then optional whitespace, then the number.
    String needle = String("\"") + key + "\"";
    int idx = json.indexOf(needle);
    if (idx < 0) return defaultVal;
    idx += needle.length();
    // Skip whitespace before colon
    while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\t')) idx++;
    if (idx >= (int)json.length() || json[idx] != ':') return defaultVal;
    idx++;
    // Skip whitespace after colon
    while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\t')) idx++;
    // Parse digits (with optional leading minus)
    String num;
    if (idx < (int)json.length() && json[idx] == '-') num += json[idx++];
    while (idx < (int)json.length() && isDigit(json[idx])) num += json[idx++];
    if (num.length() == 0) return defaultVal;
    return num.toInt();
}

// =========================================================================
// Screen helpers
// =========================================================================

void DebugServer::formatScreenLine(int row, char* out, int maxLen) {
    int base = row * 80;
    int lastNonSpace = -1;

    int limit = 80;
    if (limit > maxLen - 1) limit = maxLen - 1;

    for (int col = 0; col < limit; col++) {
        uint8_t ch = _screenBuf[base + col];
        if (ch < 32 || ch > 126) ch = ' ';
        out[col] = (char)ch;
        if (ch != ' ') lastNonSpace = col;
    }
    out[lastNonSpace + 1] = '\0';
}

int DebugServer::findTextOnScreen(const char* text) {
    // Case-insensitive search across all screen rows
    String target = String(text);
    target.toLowerCase();

    char line[82];
    for (int row = 0; row < 25; row++) {
        formatScreenLine(row, line, sizeof(line));
        String lineStr = String(line);
        lineStr.toLowerCase();
        if (lineStr.indexOf(target) >= 0) return row;
    }
    return -1;
}
