// DebugServer — JSON command protocol matching sim_main.cpp
// Test suites connect to novahost.local:6503 and speak the same
// line-delimited JSON as the Verilator simulator.

#include "debug_server.h"
#include <ArduinoJson.h>

// VGC register addresses
#define REG_CURSOR_X 0xA003
#define REG_CURSOR_Y 0xA004

// Logging (defined in novahost.ino)
extern void logLn(const char* fmt, ...);
extern const char* novaBootPhaseName();
extern bool novaFpgaBridgeAvailable();

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

    if (cmd == "boot_status") {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "{\"ok\":true,\"bootPhase\":\"%s\",\"fpgaBridgeAvailable\":%s}",
                 novaBootPhaseName(),
                 novaFpgaBridgeAvailable() ? "true" : "false");
        respond(buf);
        return;
    }

    if (!novaFpgaBridgeAvailable()) {
        char buf[160];
        snprintf(buf, sizeof(buf),
                 "{\"ok\":false,\"error\":\"FPGA bridge is busy during NovaHost boot\","
                 "\"bootPhase\":\"%s\"}",
                 novaBootPhaseName());
        respond(buf);
        return;
    }

    if      (cmd == "peek")        cmdPeek(json);
    else if (cmd == "peek_block")  cmdPeekBlock(json);
    else if (cmd == "poke")        cmdPoke(json);
    else if (cmd == "read_vram")   cmdReadVram(json);
    else if (cmd == "fill_vram")   cmdFillVram(json);
    else if (cmd == "send_key")    cmdSendKey(json);
    else if (cmd == "type_text")   cmdTypeText(json);
    else if (cmd == "read_screen") cmdReadScreen();
    else if (cmd == "read_line")   cmdReadLine(json);
    else if (cmd == "get_cursor")  cmdGetCursor();
    else if (cmd == "dbg_state")   cmdDbgState();
    else if (cmd == "dbg_pause")   cmdDbgPause();
    else if (cmd == "dbg_resume")  cmdDbgResume();
    else if (cmd == "dbg_step")    cmdDbgStep();
    else if (cmd == "dbg_break_set")   cmdDbgBreakSet(json);
    else if (cmd == "dbg_break_clear") cmdDbgBreakClear(json);
    else if (cmd == "dbg_break_clear_all") cmdDbgBreakClearAll();
    else if (cmd == "dbg_break_list")  cmdDbgBreakList();
    else if (cmd == "dbg_trace")       cmdDbgTrace(json);
    else if (cmd == "cold_start")  cmdColdStart();
    else if (cmd == "wait_ready")  cmdWaitReady(json);
    else if (cmd == "watch")       cmdWatch(json);
    else if (cmd == "run_cycles")  cmdRunCycles(json);
    else if (cmd == "reload_rom")  cmdReloadRom();
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

void DebugServer::cmdPeekBlock(const String& json) {
    int addr = extractInt(json, "address");
    int count = extractInt(json, "count");
    if (addr < 0) { respondError("Missing 'address'"); return; }
    if (count < 0) { respondError("Missing 'count'"); return; }
    if (count > 256) { respondError("'count' must be 0..256"); return; }

    uint8_t wireCount = (count == 256) ? 0 : (uint8_t)count;
    int actualCount = (wireCount == 0) ? 256 : wireCount;
    uint8_t buf[256];
    if (!_bridge.peekBlock((uint16_t)addr, wireCount, buf)) {
        respondError("FPGA peek_block failed");
        return;
    }

    String resp;
    resp.reserve(64 + actualCount * 4);
    resp = "{\"ok\":true,\"address\":";
    resp += String(addr);
    resp += ",\"count\":";
    resp += String(actualCount);
    resp += ",\"values\":[";
    for (int i = 0; i < actualCount; i++) {
        if (i > 0) resp += ',';
        resp += String(buf[i]);
    }
    resp += "]}";
    respond(resp.c_str());
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

void DebugServer::cmdReadVram(const String& json) {
    int space = extractInt(json, "space");
    int addr = extractInt(json, "address");
    int length = extractInt(json, "length", 1);
    if (space < 0) { respondError("Missing 'space'"); return; }
    if (addr < 0) { respondError("Missing 'address'"); return; }
    if (space > 7) { respondError("'space' must be 0..7"); return; }
    if (length < 1 || length > 256) {
        respondError("'length' must be 1..256");
        return;
    }
    if (addr + length > 65536) {
        respondError("'address' + 'length' exceeds VGC debug address range");
        return;
    }

    uint8_t buf[256];
    uint8_t wireCount = (length == 256) ? 0 : (uint8_t)length;
    if (!_bridge.readVgcBlock((uint8_t)space, (uint16_t)addr, wireCount, buf)) {
        respondError("FPGA read_vram failed");
        return;
    }

    String resp;
    resp.reserve(80 + length * 4);
    resp = "{\"ok\":true,\"space\":";
    resp += String(space);
    resp += ",\"address\":";
    resp += String(addr);
    if (length == 1) {
        resp += ",\"value\":";
        resp += String(buf[0]);
    } else {
        resp += ",\"data\":[";
        for (int i = 0; i < length; i++) {
            if (i > 0) resp += ',';
            resp += String(buf[i]);
        }
        resp += "]";
    }
    resp += "}";
    respond(resp.c_str());
}

void DebugServer::cmdFillVram(const String& json) {
    int space = extractInt(json, "space");
    int addr = extractInt(json, "address");
    int value = extractInt(json, "value");
    int length = extractInt(json, "length", 256);
    if (space < 0) { respondError("Missing 'space'"); return; }
    if (addr < 0) { respondError("Missing 'address'"); return; }
    if (value < 0) { respondError("Missing 'value'"); return; }
    if (space > 7) { respondError("'space' must be 0..7"); return; }
    if (value > 255) { respondError("'value' must be 0..255"); return; }
    if (length < 1 || length > 65536) {
        respondError("'length' must be 1..65536");
        return;
    }
    if (addr + length > 65536) {
        respondError("'address' + 'length' exceeds VGC debug address range");
        return;
    }

    uint8_t tail[256];
    for (int i = 0; i < 256; i++) tail[i] = (uint8_t)value;

    int remaining = length;
    uint16_t cur = (uint16_t)addr;
    while (remaining > 0) {
        int chunk = remaining >= 256 ? 256 : remaining;
        bool ok = (chunk == 256)
            ? _bridge.fillVgcBlock((uint8_t)space, cur, (uint8_t)value)
            : _bridge.pokeVgcBlock((uint8_t)space, cur, tail, (uint16_t)chunk);
        if (!ok) {
            respondError("FPGA fill_vram failed");
            return;
        }
        cur = (uint16_t)(cur + chunk);
        remaining -= chunk;
    }

    char resp[96];
    snprintf(resp, sizeof(resp),
             "{\"ok\":true,\"space\":%d,\"address\":%d,\"length\":%d}",
             space, addr, length);
    respond(resp);
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

    // Build JSON response with 50 lines
    String resp;
    resp.reserve(6000);
    resp = "{\"ok\":true,\"lines\":[";

    char line[SCREEN_COLS + 2];
    for (int row = 0; row < SCREEN_ROWS; row++) {
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
    if (row < 0 || row >= SCREEN_ROWS) { respondError("Invalid row"); return; }

    if (!_bridge.readScreen(_screenBuf)) {
        respondError("FPGA read_screen failed");
        return;
    }

    char line[SCREEN_COLS + 2];
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
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"a\":%d,\"x\":%d,\"y\":%d,\"sp\":%d,\"pc\":%d,"
        "\"nf\":%d,\"vf\":%d,\"df\":%d,\"if\":%d,\"zf\":%d,\"cf\":%d,"
        "\"paused\":%s,\"waiting\":%s,\"stopped\":%s,"
        "\"breakpoint_hit\":%s,\"step_active\":%s,\"status\":%d}",
        st.a, st.x, st.y, st.sp, st.pc,
        (st.flags >> 7) & 1,   // N
        (st.flags >> 6) & 1,   // V
        (st.flags >> 3) & 1,   // D
        (st.flags >> 2) & 1,   // I
        (st.flags >> 1) & 1,   // Z
        st.flags & 1,          // C
        st.paused ? "true" : "false",
        st.waiting ? "true" : "false",
        st.stopped ? "true" : "false",
        st.breakpointHit ? "true" : "false",
        st.stepActive ? "true" : "false",
        st.status);
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

void DebugServer::cmdDbgStep() {
    FpgaBridge::CpuState st;
    if (!_bridge.step(st)) { respondError("FPGA dbg_step failed"); return; }
    _paused = st.paused;
    char buf[224];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"a\":%d,\"x\":%d,\"y\":%d,\"sp\":%d,\"pc\":%d,"
        "\"nf\":%d,\"vf\":%d,\"df\":%d,\"if\":%d,\"zf\":%d,\"cf\":%d,"
        "\"paused\":%s,\"waiting\":%s,\"stopped\":%s,"
        "\"breakpoint_hit\":%s,\"step_active\":%s,\"status\":%d}",
        st.a, st.x, st.y, st.sp, st.pc,
        (st.flags >> 7) & 1,
        (st.flags >> 6) & 1,
        (st.flags >> 3) & 1,
        (st.flags >> 2) & 1,
        (st.flags >> 1) & 1,
        st.flags & 1,
        st.paused ? "true" : "false",
        st.waiting ? "true" : "false",
        st.stopped ? "true" : "false",
        st.breakpointHit ? "true" : "false",
        st.stepActive ? "true" : "false",
        st.status);
    respond(buf);
}

void DebugServer::cmdDbgBreakSet(const String& json) {
    int slot = extractInt(json, "slot", -1);
    int addr = extractInt(json, "address");
    int enabled = extractInt(json, "enabled", 1);
    if (addr < 0) { respondError("Missing 'address'"); return; }
    if (slot > 3) { respondError("'slot' must be 0..3"); return; }

    if (slot < 0) {
        FpgaBridge::BreakpointState state;
        if (!_bridge.breakList(state)) { respondError("FPGA break_list failed"); return; }

        for (int i = 0; i < 4; i++) {
            if (state.slots[i].enabled && state.slots[i].address == (uint16_t)addr) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            for (int i = 0; i < 4; i++) {
                if (!state.slots[i].enabled) {
                    slot = i;
                    break;
                }
            }
        }
        if (slot < 0) { respondError("No free breakpoint slots"); return; }
    }

    if (!_bridge.breakSet((uint8_t)slot, (uint16_t)addr, enabled != 0)) {
        respondError("FPGA break_set failed");
        return;
    }

    char buf[96];
    snprintf(buf, sizeof(buf),
        "{\"ok\":true,\"slot\":%d,\"address\":%d,\"enabled\":%s}",
        slot, addr, enabled != 0 ? "true" : "false");
    respond(buf);
}

void DebugServer::cmdDbgBreakClear(const String& json) {
    int slot = extractInt(json, "slot", -1);
    int addr = extractInt(json, "address", -1);

    if (slot >= 0) {
        if (slot > 3) { respondError("'slot' must be 0..3"); return; }
        if (!_bridge.breakClear((uint8_t)slot)) {
            respondError("FPGA break_clear failed");
            return;
        }
        respond("{\"ok\":true,\"removed\":true}");
        return;
    }

    if (addr < 0) { respondError("Missing 'slot' or 'address'"); return; }

    FpgaBridge::BreakpointState state;
    if (!_bridge.breakList(state)) { respondError("FPGA break_list failed"); return; }

    bool removed = false;
    for (int i = 0; i < 4; i++) {
        if (state.slots[i].enabled && state.slots[i].address == (uint16_t)addr) {
            if (!_bridge.breakClear((uint8_t)i)) {
                respondError("FPGA break_clear failed");
                return;
            }
            removed = true;
        }
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "{\"ok\":true,\"removed\":%s}", removed ? "true" : "false");
    respond(buf);
}

void DebugServer::cmdDbgBreakClearAll() {
    for (int i = 0; i < 4; i++) {
        if (!_bridge.breakClear((uint8_t)i)) {
            respondError("FPGA break_clear failed");
            return;
        }
    }
    respondOk();
}

void DebugServer::cmdDbgBreakList() {
    FpgaBridge::BreakpointState state;
    if (!_bridge.breakList(state)) { respondError("FPGA break_list failed"); return; }

    String resp;
    resp.reserve(260);
    resp = "{\"ok\":true,\"flags\":";
    resp += String(state.flags);
    resp += ",\"paused\":";
    resp += (state.flags & 0x01) ? "true" : "false";
    resp += ",\"current_match\":";
    resp += (state.flags & 0x02) ? "true" : "false";
    resp += ",\"breakpoint_hit\":";
    resp += (state.flags & 0x04) ? "true" : "false";
    resp += ",\"suppress\":";
    resp += (state.flags & 0x08) ? "true" : "false";
    resp += ",\"hit_slot\":";
    resp += String(state.hitSlot);
    resp += ",\"breakpoints\":[";
    for (int i = 0; i < 4; i++) {
        if (i > 0) resp += ',';
        resp += "{\"slot\":";
        resp += String(i);
        resp += ",\"enabled\":";
        resp += state.slots[i].enabled ? "true" : "false";
        resp += ",\"address\":";
        resp += String(state.slots[i].address);
        resp += "}";
    }
    resp += "]}";
    respond(resp.c_str());
}

void DebugServer::cmdDbgTrace(const String& json) {
    int count = extractInt(json, "count", FpgaBridge::TRACE_RECORDS);
    if (count < 1 || count > FpgaBridge::TRACE_RECORDS) {
        respondError("'count' must be 1..64");
        return;
    }

    uint8_t buf[FpgaBridge::TRACE_RECORDS * FpgaBridge::TRACE_RECORD_BYTES];
    uint16_t bytesRead = 0;
    if (!_bridge.traceRead((uint8_t)count, buf, bytesRead)) {
        respondError("FPGA trace_read failed");
        return;
    }

    uint16_t records = bytesRead / FpgaBridge::TRACE_RECORD_BYTES;
    String resp;
    resp.reserve(96 + records * 160);
    resp = "{\"ok\":true,\"record_bytes\":";
    resp += String(FpgaBridge::TRACE_RECORD_BYTES);
    resp += ",\"records\":[";
    for (uint16_t i = 0; i < records; i++) {
        uint16_t base = i * FpgaBridge::TRACE_RECORD_BYTES;
        uint16_t pc = ((uint16_t)buf[base] << 8) | buf[base + 1];
        uint16_t addr = ((uint16_t)buf[base + 2] << 8) | buf[base + 3];
        if (i > 0) resp += ',';
        resp += "{\"pc\":";
        resp += String(pc);
        resp += ",\"addr\":";
        resp += String(addr);
        resp += ",\"din\":";
        resp += String(buf[base + 4]);
        resp += ",\"dout\":";
        resp += String(buf[base + 5]);
        resp += ",\"a\":";
        resp += String(buf[base + 6]);
        resp += ",\"sp\":";
        resp += String(buf[base + 7]);
        resp += ",\"flags\":";
        resp += String(buf[base + 8]);
        resp += ",\"state\":";
        resp += String(buf[base + 9] & 0x3F);
        resp += ",\"ir\":";
        resp += String(buf[base + 10]);
        resp += ",\"ctrl\":";
        resp += String(buf[base + 11]);
        resp += ",\"rdy\":";
        resp += (buf[base + 11] & 0x01) ? "true" : "false";
        resp += ",\"we\":";
        resp += (buf[base + 11] & 0x02) ? "true" : "false";
        resp += ",\"irq\":";
        resp += (buf[base + 11] & 0x04) ? "true" : "false";
        resp += ",\"nmi\":";
        resp += (buf[base + 11] & 0x08) ? "true" : "false";
        resp += "}";
    }
    resp += "]}";
    respond(resp.c_str());
}

// Manual ROM reload — re-runs the boot-time load sequence. Useful after a
// bitstream reflash when NovaHost's boot-time load ran against a different
// (now-replaced) bitstream.
extern bool loadRomsToFPGA();

void DebugServer::cmdReloadRom() {
    if (loadRomsToFPGA()) {
        respondOk();
    } else {
        respondError("ROM reload failed (see NovaHost log for details)");
    }
}

// =========================================================================
// Command handlers — Priority 2 (ESP32-side logic)
// =========================================================================

void DebugServer::cmdColdStart() {
    // A cold start is a system reset, not Ctrl-C. Keep the CPU held while the
    // custom chips reset, then release and let hardware scrub VGC state before
    // the reset vector runs.
    bool fullReset = _bridge.systemResetHold();
    if (!fullReset)
        _bridge.resetHold();
    delay(10);
    if (fullReset)
        _bridge.systemResetRelease();
    else
        _bridge.resetRelease();

    // Resume if paused/stopped under the debug bridge.
    _bridge.resume();
    _paused = false;

    // Wait for "Ready" prompt. Do not report success unless the VM actually
    // reached the prompt; otherwise test runners race ahead and hide reset
    // failures behind later transport timeouts.
    unsigned long deadline = millis() + 10000;
    while (millis() < deadline) {
        if (_bridge.readScreen(_screenBuf)) {
            if (findTextOnScreen("Ready") >= 0) {
                respondOk();
                return;
            }
        }
        delay(50);
    }

    respondError("Cold start timed out waiting for Ready");
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

// Use ArduinoJson (Benoit Blanchon, the de-facto Arduino JSON library) for
// proper RFC-8259 string-escape decoding. The previous hand-rolled extractor
// terminated string values at the first raw `"` byte, which broke any text
// containing `\"`, `\\`, `\n`, `\r`, etc. — making it impossible to drive
// BASIC commands like `LOAD "foo"` over the TCP test harness. Each call
// re-parses the line; cheap because protocol traffic is human-paced.
String DebugServer::extractString(const String& json, const char* key) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, jsonPayload(json));
    if (err) return String();
    JsonVariantConst v = doc[key];
    if (v.isNull()) return String();
    const char* s = v.as<const char*>();
    return s ? String(s) : String();
}

int DebugServer::extractInt(const String& json, const char* key, int defaultVal) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, jsonPayload(json));
    if (err) return defaultVal;
    JsonVariantConst v = doc[key];
    if (v.isNull()) return defaultVal;
    return v.as<int>();
}

const char* DebugServer::jsonPayload(const String& json) {
    const char* p = json.c_str();
    const uint8_t* b = reinterpret_cast<const uint8_t*>(p);
    if (b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
        return p + 3;
    }
    return p;
}

// =========================================================================
// Screen helpers
// =========================================================================

void DebugServer::formatScreenLine(int row, char* out, int maxLen) {
    int base = row * SCREEN_COLS;
    int lastNonSpace = -1;

    int limit = SCREEN_COLS;
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

    char line[SCREEN_COLS + 2];
    for (int row = 0; row < SCREEN_ROWS; row++) {
        formatScreenLine(row, line, sizeof(line));
        String lineStr = String(line);
        lineStr.toLowerCase();
        if (lineStr.indexOf(target) >= 0) return row;
    }
    return -1;
}
