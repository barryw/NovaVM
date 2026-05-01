// Verilator testbench: 6502 + VGC with SDL2 display + TCP server
// TCP server speaks same JSON protocol as Avalonia EmulatorTcpServer
// Architecture: main loop runs frames, TCP commands serviced between frames.
// Long-running commands (wait_ready, watch) use frame-level polling.

#include "Vtop.h"
#include "verilated.h"

#include <SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <queue>
#include <sstream>
#include <cctype>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#if defined(VIDEO_720X480)
static const int H_ACTIVE=720, H_TOTAL=858;
static const int V_ACTIVE=480, V_TOTAL=525;
#elif defined(VGC_TIMING_5994)
static const int H_ACTIVE=640, H_TOTAL=796;
static const int V_ACTIVE=480, V_TOTAL=524;
#else
static const int H_ACTIVE=640, H_TOTAL=794;
static const int V_ACTIVE=480, V_TOTAL=525;
#endif
static const int FRAME_CLKS=H_TOTAL*V_TOTAL;
static const int DISP_W=H_ACTIVE, DISP_H=V_ACTIVE, SCALE=2;
static const int SCREEN_COLS=80, SCREEN_ROWS=50;
static const int CURSOR_X_REG=0xA003, CURSOR_Y_REG=0xA004;
static const int VRAM_PLANE_CHAR=1, VRAM_PLANE_COLOR=2, VRAM_PLANE_GFX=3, VRAM_PLANE_SPRITE=4, VRAM_PLANE_TILE=6;
static const int VRAM_DEBUG_MAX_BLOCK=4096;
static const int DEFAULT_TCP_PORT=6503;

static uint32_t framebuf[DISP_W * DISP_H];
static Vtop *top = nullptr;
static std::queue<uint8_t> key_queue;

static const int CPU_STATE_DECODE = 12;
static const int SIM_BREAKPOINTS = 4;
static const int SIM_TRACE_DEPTH = 64;
static const int SIM_TRACE_RECORD_BYTES = 12;

struct SimBreakpoint {
    bool enabled = false;
    uint16_t address = 0;
};

struct SimTraceRecord {
    uint16_t pc = 0;
    uint16_t addr = 0;
    uint8_t din = 0;
    uint8_t dout = 0;
    uint8_t a = 0;
    uint8_t sp = 0;
    uint8_t flags = 0;
    uint8_t state = 0;
    uint8_t ir = 0;
    uint8_t ctrl = 0;
};

static SimBreakpoint sim_breakpoints[SIM_BREAKPOINTS];
static bool sim_bp_hit_latched = false;
static int sim_bp_hit_slot = 0;
static bool sim_bp_suppress = false;
static uint16_t sim_bp_suppress_pc = 0;
static bool sim_step_active = false;
static SimTraceRecord sim_trace[SIM_TRACE_DEPTH];
static int sim_trace_wr = 0;
static int sim_trace_count = 0;

// ── Audio ──
// SID outputs signed 18-bit audio at ~1 MHz. We downsample to 44100 Hz
// by averaging samples. Pixel clock / 44100 ≈ 570 clocks per audio sample.
static const int AUDIO_RATE = 44100;
static const int AUDIO_BUFSIZE = 2048;  // SDL audio buffer size in samples
static const int PIX_PER_SAMPLE = 25000000 / AUDIO_RATE;  // ~567

// Ring buffer for audio output (stereo interleaved int16)
static const int AUDIO_RING_SIZE = 16384;
static int16_t audio_ring[AUDIO_RING_SIZE * 2];  // stereo
static volatile int audio_ring_rd = 0;
static volatile int audio_ring_wr = 0;

static void audio_callback(void*, Uint8 *stream, int len) {
    int16_t *out = (int16_t*)stream;
    int samples = len / 4;  // stereo int16 = 4 bytes per sample
    for (int i = 0; i < samples; i++) {
        int rd = audio_ring_rd;
        if (rd != audio_ring_wr) {
            out[i*2]   = audio_ring[rd*2];
            out[i*2+1] = audio_ring[rd*2+1];
            audio_ring_rd = (rd + 1) % AUDIO_RING_SIZE;
        } else {
            out[i*2] = 0;
            out[i*2+1] = 0;
        }
    }
}

// Accumulator for downsampling
static int32_t audio_acc_l = 0, audio_acc_r = 0;
static int audio_acc_count = 0;

static void audio_push_sample(int32_t left18, int32_t right18) {
    // Sign-extend 18-bit to 32-bit
    if (left18  & 0x20000) left18  |= ~0x3FFFF;
    if (right18 & 0x20000) right18 |= ~0x3FFFF;
    audio_acc_l += left18;
    audio_acc_r += right18;
    audio_acc_count++;
    if (audio_acc_count >= PIX_PER_SAMPLE) {
        int32_t avg_l = audio_acc_l / audio_acc_count;
        int32_t avg_r = audio_acc_r / audio_acc_count;
        // Scale 18-bit range to 16-bit
        int16_t sl = (int16_t)(avg_l >> 2);
        int16_t sr = (int16_t)(avg_r >> 2);
        int next_wr = (audio_ring_wr + 1) % AUDIO_RING_SIZE;
        if (next_wr != audio_ring_rd) {
            audio_ring[audio_ring_wr*2]   = sl;
            audio_ring[audio_ring_wr*2+1] = sr;
            audio_ring_wr = next_wr;
        }
        audio_acc_l = audio_acc_r = audio_acc_count = 0;
    }
}

// ── Key translation ──
static int sdl_key_to_ascii(SDL_Keycode key, uint16_t mod) {
    bool shift = (mod & KMOD_SHIFT) != 0;
    if (key >= SDLK_a && key <= SDLK_z) { int b=key-SDLK_a+'a'; return shift?(b-32):b; }
    if (key >= SDLK_0 && key <= SDLK_9) { if (shift) { const char *s=")!@#$%^&*("; return s[key-SDLK_0]; } return key; }
    switch (key) {
        case SDLK_RETURN: return 0x0D; case SDLK_BACKSPACE: return 0x08;
        case SDLK_ESCAPE: return 0x1B; case SDLK_SPACE: return ' '; case SDLK_TAB: return 0x09;
        case SDLK_MINUS: return shift?'_':'-'; case SDLK_EQUALS: return shift?'+':'=';
        case SDLK_LEFTBRACKET: return shift?'{':'['; case SDLK_RIGHTBRACKET: return shift?'}':']';
        case SDLK_BACKSLASH: return shift?'|':'\\'; case SDLK_SEMICOLON: return shift?':':';';
        case SDLK_QUOTE: return shift?'"':'\''; case SDLK_BACKQUOTE: return shift?'~':'`';
        case SDLK_COMMA: return shift?'<':','; case SDLK_PERIOD: return shift?'>':'.';
        case SDLK_SLASH: return shift?'?':'/'; default: return -1;
    }
}

// ── JSON helpers ──
static std::string json_unescape(const std::string &s) {
    std::string out; out.reserve(s.size());
    for (size_t i=0; i<s.size(); i++) {
        if (s[i]=='\\' && i+1<s.size()) {
            char c=s[i+1];
            if (c=='"'||c=='\\'||c=='/') { out+=c; i++; }
            else if (c=='n') { out+='\n'; i++; }
            else if (c=='r') { out+='\r'; i++; }
            else if (c=='t') { out+='\t'; i++; }
            else if (c=='u' && i+5<s.size()) {
                // \uXXXX — parse 4 hex digits, emit as ASCII if <128
                char hex[5]={s[i+2],s[i+3],s[i+4],s[i+5],0};
                int cp=(int)strtol(hex,NULL,16);
                if (cp>0 && cp<128) out+=(char)cp;
                i+=5;
            } else out+=s[i];
        } else out+=s[i];
    }
    return out;
}
static std::string jgs(const std::string &j, const std::string &k) {
    auto p=j.find("\""+k+"\""); if (p==std::string::npos) return "";
    p=j.find(':',p); if (p==std::string::npos) return "";
    p=j.find('"',p+1); if (p==std::string::npos) return "";
    auto e=j.find('"',p+1); if (e==std::string::npos) return "";
    return json_unescape(j.substr(p+1,e-p-1));
}
static int jgi(const std::string &j, const std::string &k, int d=-1) {
    auto p=j.find("\""+k+"\""); if (p==std::string::npos) return d;
    p=j.find(':',p); if (p==std::string::npos) return d; p++;
    while (p<j.size()&&(j[p]==' '||j[p]=='\t')) p++;
    bool neg=false; if (p<j.size()&&j[p]=='-') { neg=true; p++; }
    int v=0; while (p<j.size()&&j[p]>='0'&&j[p]<='9') { v=v*10+(j[p]-'0'); p++; }
    return neg?-v:v;
}

// ── Sim helpers ──
static bool sim_decode_boundary() {
    return top && top->dbg_cpu_rdy && ((int)top->dbg_cpu_state == CPU_STATE_DECODE);
}

static int sim_matching_breakpoint() {
    if (!sim_decode_boundary()) return -1;
    uint16_t pc = (uint16_t)top->dbg_cpu_pc;
    for (int i=0; i<SIM_BREAKPOINTS; i++) {
        if (sim_breakpoints[i].enabled && sim_breakpoints[i].address == pc)
            return i;
    }
    return -1;
}

static void sim_after_posedge() {
    if (!top) return;

    if (top->dbg_cpu_rdy && !top->rst) {
        SimTraceRecord &r = sim_trace[sim_trace_wr];
        r.pc = (uint16_t)top->dbg_cpu_pc;
        r.addr = (uint16_t)top->dbg_cpu_addr;
        r.din = (uint8_t)top->dbg_cpu_din;
        r.dout = (uint8_t)top->dbg_cpu_dout;
        r.a = (uint8_t)top->dbg_cpu_a;
        r.sp = (uint8_t)top->dbg_cpu_sp;
        r.flags = (uint8_t)top->dbg_cpu_flags;
        r.state = (uint8_t)(top->dbg_cpu_state & 0x3F);
        r.ir = (uint8_t)top->dbg_cpu_ir;
        r.ctrl = (top->dbg_cpu_rdy ? 0x01 : 0)
               | (top->dbg_cpu_we  ? 0x02 : 0)
               | (top->dbg_cpu_irq ? 0x04 : 0)
               | (top->dbg_cpu_nmi ? 0x08 : 0);
        sim_trace_wr = (sim_trace_wr + 1) % SIM_TRACE_DEPTH;
        if (sim_trace_count < SIM_TRACE_DEPTH) sim_trace_count++;
    }

    if (!sim_decode_boundary()) return;

    if (sim_bp_suppress && top->dbg_cpu_pc != sim_bp_suppress_pc)
        sim_bp_suppress = false;

    int slot = sim_matching_breakpoint();
    if (slot >= 0 && !sim_bp_suppress) {
        sim_bp_hit_latched = true;
        sim_bp_hit_slot = slot;
        top->dbg_pause = 1;
    }
}

static void step_clock() {
    top->clk=0; top->eval();
    top->clk=1; top->eval();
    sim_after_posedge();
}

static uint8_t peek_ram(uint16_t addr) {
    top->dbg_peek_en=1; top->dbg_peek_addr=addr; top->eval();
    step_clock();
    uint8_t v=top->dbg_peek_data; top->dbg_peek_en=0; top->eval(); return v;
}
static void poke_ram(uint16_t addr, uint8_t val) {
    top->dbg_poke_en=1; top->dbg_poke_addr=addr; top->dbg_poke_data=val;
    step_clock(); top->dbg_poke_en=0;
}
static void dbg_vram_write(uint8_t space, uint32_t addr, uint8_t val) {
    top->dbg_vmem_space = space;
    top->dbg_vmem_addr = addr;
    top->dbg_vmem_data = val;
    top->dbg_vmem_we = 1;
    step_clock();
    top->dbg_vmem_we = 0;
}
static uint8_t dbg_vram_read(uint8_t space, uint32_t addr) {
    top->dbg_vram_read_en = 1;
    top->dbg_vram_space = space;
    top->dbg_vram_addr = addr;
    top->eval();
    uint8_t v = top->dbg_vram_rdata;
    top->dbg_vram_read_en = 0;
    return v;
}

static std::string read_screen_line(int row) {
    if (row<0||row>=SCREEN_ROWS) return "";
    std::string line; line.reserve(SCREEN_COLS);
    for (int c=0; c<SCREEN_COLS; c++) {
        uint8_t ch=dbg_vram_read(VRAM_PLANE_CHAR,row*SCREEN_COLS+c);
        line += (ch>=0x20)?(char)ch:' ';
    }
    while (!line.empty()&&line.back()==' ') line.pop_back();
    return line;
}

static bool screen_contains(const std::string &text) {
    std::string lt=text; for (auto &c:lt) c=tolower(c);
    for (int r=0; r<SCREEN_ROWS; r++) {
        std::string line=read_screen_line(r);
        std::string ll=line; for (auto &c:ll) c=tolower(c);
        if (ll.find(lt)!=std::string::npos) return true;
    }
    return false;
}

static int screen_find_row(const std::string &text) {
    std::string lt=text; for (auto &c:lt) c=tolower(c);
    for (int r=0; r<SCREEN_ROWS; r++) {
        std::string line=read_screen_line(r);
        std::string ll=line; for (auto &c:ll) c=tolower(c);
        if (ll.find(lt)!=std::string::npos) return r;
    }
    return -1;
}

static uint8_t sim_status_byte() {
    return (top->dbg_cpu_waiting ? 0x01 : 0)
         | (top->dbg_cpu_stopped ? 0x02 : 0)
         | (top->dbg_pause ? 0x04 : 0)
         | (sim_bp_hit_latched ? 0x08 : 0)
         | (sim_step_active ? 0x10 : 0);
}

static std::string sim_cpu_state_json() {
    char buf[768];
    uint8_t status = sim_status_byte();
    snprintf(buf,sizeof(buf),"{\"ok\":true,\"a\":%d,\"x\":%d,\"y\":%d,\"sp\":%d,\"pc\":%d,"
        "\"nf\":%d,\"vf\":%d,\"df\":%d,\"if\":%d,\"zf\":%d,\"cf\":%d,"
        "\"waiting\":%s,\"stopped\":%s,\"paused\":%s,"
        "\"breakpoint_hit\":%s,\"step_active\":%s,\"status\":%d,"
        "\"state\":%d,\"ir\":%d,\"addr\":%d,\"din\":%d,\"dout\":%d,\"we\":%s,\"rdy\":%s,\"irq\":%s,\"nmi\":%s}\n",
        top->dbg_cpu_a,top->dbg_cpu_x,top->dbg_cpu_y,top->dbg_cpu_sp,top->dbg_cpu_pc,
        (top->dbg_cpu_flags>>7)&1,(top->dbg_cpu_flags>>6)&1,(top->dbg_cpu_flags>>3)&1,
        (top->dbg_cpu_flags>>2)&1,(top->dbg_cpu_flags>>1)&1,(top->dbg_cpu_flags>>0)&1,
        top->dbg_cpu_waiting ? "true" : "false",
        top->dbg_cpu_stopped ? "true" : "false",
        top->dbg_pause ? "true" : "false",
        sim_bp_hit_latched ? "true" : "false",
        sim_step_active ? "true" : "false",
        status,
        top->dbg_cpu_state,top->dbg_cpu_ir,top->dbg_cpu_addr,top->dbg_cpu_din,top->dbg_cpu_dout,
        top->dbg_cpu_we ? "true" : "false",
        top->dbg_cpu_rdy ? "true" : "false",
        top->dbg_cpu_irq ? "true" : "false",
        top->dbg_cpu_nmi ? "true" : "false");
    return buf;
}

static int sim_choose_breakpoint_slot(uint16_t addr) {
    for (int i=0; i<SIM_BREAKPOINTS; i++)
        if (sim_breakpoints[i].enabled && sim_breakpoints[i].address == addr)
            return i;
    for (int i=0; i<SIM_BREAKPOINTS; i++)
        if (!sim_breakpoints[i].enabled)
            return i;
    return -1;
}

static std::string sim_break_list_json() {
    int current = sim_matching_breakpoint();
    std::ostringstream ss;
    ss<<"{\"ok\":true,\"flags\":"<<(int)((top->dbg_pause ? 0x01 : 0)
        | (current >= 0 ? 0x02 : 0)
        | (sim_bp_hit_latched ? 0x04 : 0)
        | (sim_bp_suppress ? 0x08 : 0))
      <<",\"paused\":"<<(top->dbg_pause ? "true" : "false")
      <<",\"current_match\":"<<(current >= 0 ? "true" : "false")
      <<",\"breakpoint_hit\":"<<(sim_bp_hit_latched ? "true" : "false")
      <<",\"suppress\":"<<(sim_bp_suppress ? "true" : "false")
      <<",\"hit_slot\":"<<sim_bp_hit_slot
      <<",\"breakpoints\":[";
    for (int i=0; i<SIM_BREAKPOINTS; i++) {
        if (i) ss<<",";
        ss<<"{\"slot\":"<<i
          <<",\"enabled\":"<<(sim_breakpoints[i].enabled ? "true" : "false")
          <<",\"address\":"<<sim_breakpoints[i].address<<"}";
    }
    ss<<"]}\n";
    return ss.str();
}

static std::string sim_trace_json(int requested) {
    if (requested <= 0 || requested > SIM_TRACE_DEPTH)
        requested = SIM_TRACE_DEPTH;
    if (requested > sim_trace_count)
        requested = sim_trace_count;

    int start = (sim_trace_wr - requested + SIM_TRACE_DEPTH) % SIM_TRACE_DEPTH;
    std::ostringstream ss;
    ss<<"{\"ok\":true,\"record_bytes\":"<<SIM_TRACE_RECORD_BYTES<<",\"records\":[";
    for (int i=0; i<requested; i++) {
        int idx = (start + i) % SIM_TRACE_DEPTH;
        const SimTraceRecord &r = sim_trace[idx];
        if (i) ss<<",";
        ss<<"{\"pc\":"<<r.pc
          <<",\"addr\":"<<r.addr
          <<",\"din\":"<<(int)r.din
          <<",\"dout\":"<<(int)r.dout
          <<",\"a\":"<<(int)r.a
          <<",\"sp\":"<<(int)r.sp
          <<",\"flags\":"<<(int)r.flags
          <<",\"state\":"<<(int)r.state
          <<",\"ir\":"<<(int)r.ir
          <<",\"ctrl\":"<<(int)r.ctrl
          <<",\"rdy\":"<<((r.ctrl & 0x01) ? "true" : "false")
          <<",\"we\":"<<((r.ctrl & 0x02) ? "true" : "false")
          <<",\"irq\":"<<((r.ctrl & 0x04) ? "true" : "false")
          <<",\"nmi\":"<<((r.ctrl & 0x08) ? "true" : "false")
          <<"}";
    }
    ss<<"]}\n";
    return ss.str();
}

// ── Pending async operation ──
struct PendingOp {
    enum Type { NONE, WAIT_READY, WATCH } type = NONE;
    std::string wait_text;
    std::chrono::steady_clock::time_point deadline;
    int watch_addr = 0;
    int watch_value = 0;
};
static PendingOp pending_op;
static std::string pending_response;

// ── TCP command handler (instant commands only) ──
static std::string process_command(const std::string &json) {
    std::string cmd = jgs(json, "command");
    char buf[512];

    if (cmd=="peek") {
        int a=jgi(json,"address"); if (a<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        snprintf(buf,sizeof(buf),"{\"ok\":true,\"address\":%d,\"value\":%d}\n",a,peek_ram((uint16_t)a));
        return buf;
    }
    if (cmd=="peek_block") {
        int a=jgi(json,"address"), count=jgi(json,"count");
        if (a<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (count<0) return "{\"ok\":false,\"error\":\"Missing 'count'\"}\n";
        if (count>256) return "{\"ok\":false,\"error\":\"'count' must be 0..256\"}\n";
        int n = (count == 0) ? 256 : count;
        std::ostringstream ss;
        ss<<"{\"ok\":true,\"address\":"<<a<<",\"count\":"<<n<<",\"values\":[";
        for (int i=0;i<n;i++) {
            if (i) ss<<",";
            ss<<(int)peek_ram((uint16_t)(a+i));
        }
        ss<<"]}\n";
        return ss.str();
    }
    if (cmd=="poke") {
        int a=jgi(json,"address"),v=jgi(json,"value");
        if (a<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (v<0) return "{\"ok\":false,\"error\":\"Missing 'value'\"}\n";
        poke_ram((uint16_t)a,(uint8_t)v); return "{\"ok\":true}\n";
    }
    if (cmd=="read_vram") {
        int space=jgi(json,"space"),addr=jgi(json,"address"),len=jgi(json,"length",1);
        if (space<0) return "{\"ok\":false,\"error\":\"Missing 'space'\"}\n";
        if (addr<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (len<1) return "{\"ok\":false,\"error\":\"Invalid 'length'\"}\n";
        if (len>VRAM_DEBUG_MAX_BLOCK) return "{\"ok\":false,\"error\":\"length too large\"}\n";

        std::ostringstream ss;
        if (len==1) {
            ss<<"{\"ok\":true,\"space\":"<<space<<",\"address\":"<<addr
              <<",\"value\":"<<(int)dbg_vram_read((uint8_t)space,(uint32_t)addr)<<"}\n";
        } else {
            ss<<"{\"ok\":true,\"space\":"<<space<<",\"address\":"<<addr<<",\"data\":[";
            for (int i=0;i<len;i++) {
                if (i) ss<<",";
                ss<<(int)dbg_vram_read((uint8_t)space,(uint32_t)(addr+i));
            }
            ss<<"]}\n";
        }
        return ss.str();
    }
    if (cmd=="fill_vram") {
        int space=jgi(json,"space"),addr=jgi(json,"address"),value=jgi(json,"value"),len=jgi(json,"length",256);
        if (space<0) return "{\"ok\":false,\"error\":\"Missing 'space'\"}\n";
        if (addr<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (value<0) return "{\"ok\":false,\"error\":\"Missing 'value'\"}\n";
        if (value>255) return "{\"ok\":false,\"error\":\"value must be 0..255\"}\n";
        if (len<1) return "{\"ok\":false,\"error\":\"Invalid 'length'\"}\n";
        if (len>65536) return "{\"ok\":false,\"error\":\"length too large\"}\n";
        for (int i=0;i<len;i++)
            dbg_vram_write((uint8_t)space,(uint32_t)(addr+i),(uint8_t)value);
        std::ostringstream ss;
        ss<<"{\"ok\":true,\"space\":"<<space<<",\"address\":"<<addr<<",\"length\":"<<len<<"}\n";
        return ss.str();
    }
    if (cmd=="type_text") { for (char c:jgs(json,"text")) key_queue.push((uint8_t)c); return "{\"ok\":true}\n"; }
    if (cmd=="send_key") {
        std::string k=jgs(json,"key");
        if (k=="ENTER"||k=="CR"||k=="RETURN") key_queue.push(0x0D);
        else if (k=="BACKSPACE"||k=="BS") key_queue.push(0x08);
        else if (k=="CTRL-C"||k=="BREAK") key_queue.push(0x03);
        else if (k.size()==1) key_queue.push((uint8_t)k[0]);
        return "{\"ok\":true}\n";
    }
    if (cmd=="read_screen") {
        std::ostringstream ss; ss<<"{\"ok\":true,\"lines\":[";
        for (int r=0;r<SCREEN_ROWS;r++) {
            if (r) ss<<","; ss<<"\"";
            for (char c:read_screen_line(r)) { if (c=='"') ss<<"\\\""; else if (c=='\\') ss<<"\\\\"; else ss<<c; }
            ss<<"\"";
        }
        ss<<"],\"cursor_x\":"<<(int)peek_ram(CURSOR_X_REG)<<",\"cursor_y\":"<<(int)peek_ram(CURSOR_Y_REG)<<"}\n";
        return ss.str();
    }
    if (cmd=="read_line") {
        int r=jgi(json,"row"); if (r<0) r=peek_ram(CURSOR_Y_REG);
        std::string t=read_screen_line(r); std::ostringstream ss;
        ss<<"{\"ok\":true,\"row\":"<<r<<",\"text\":\"";
        for (char c:t) { if (c=='"') ss<<"\\\""; else if (c=='\\') ss<<"\\\\"; else ss<<c; }
        ss<<"\"}\n"; return ss.str();
    }
    if (cmd=="get_cursor") {
        snprintf(buf,sizeof(buf),"{\"ok\":true,\"x\":%d,\"y\":%d}\n",(int)peek_ram(CURSOR_X_REG),(int)peek_ram(CURSOR_Y_REG));
        return buf;
    }
    if (cmd=="cold_start") {
        top->rst=1; top->dbg_system_reset=1; key_queue=std::queue<uint8_t>();
        for (int i=0;i<100;i++) step_clock();
        top->rst=0; top->dbg_system_reset=0; top->dbg_pause=0; top->dbg_cpu_resume=0;
        // Run enough frames for EhBASIC to fully initialize and display "Ready".
        // Keep this under sim6502's per-command socket timeout.
        for (int f=0;f<15;f++)
            for (int i=0;i<FRAME_CLKS;i++) step_clock();
        return "{\"ok\":true}\n";
    }
    if (cmd=="dbg_state") {
        return sim_cpu_state_json();
    }
    if (cmd=="dbg_pause") { top->dbg_pause=1; return process_command("{\"command\":\"dbg_state\"}"); }
    if (cmd=="dbg_resume") {
        top->dbg_pause=0;
        sim_bp_hit_latched=false;
        sim_bp_suppress=true;
        sim_bp_suppress_pc=(uint16_t)top->dbg_cpu_pc;
        top->dbg_cpu_resume=1;
        step_clock();
        top->dbg_cpu_resume=0;
        return "{\"ok\":true}\n";
    }
    if (cmd=="dbg_step") {
        top->dbg_pause=0;
        sim_bp_hit_latched=false;
        sim_bp_suppress=true;
        sim_bp_suppress_pc=(uint16_t)top->dbg_cpu_pc;
        sim_step_active=true;
        top->dbg_cpu_resume=1;
        step_clock();
        top->dbg_cpu_resume=0;

        bool left_decode = !sim_decode_boundary();
        bool done = false;
        for (int i=0; i<1000000; i++) {
            step_clock();
            if (!sim_decode_boundary()) {
                left_decode = true;
            } else if (left_decode) {
                done = true;
                break;
            }
        }

        top->dbg_pause=1;
        sim_step_active=false;
        if (!done) return "{\"ok\":false,\"error\":\"dbg_step timeout\"}\n";
        return sim_cpu_state_json();
    }
    if (cmd=="dbg_break_set") {
        int a=jgi(json,"address"), slot=jgi(json,"slot",-1), enabled=jgi(json,"enabled",1);
        if (a<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (slot > 3) return "{\"ok\":false,\"error\":\"'slot' must be 0..3\"}\n";
        if (slot < 0) slot = sim_choose_breakpoint_slot((uint16_t)a);
        if (slot < 0) return "{\"ok\":false,\"error\":\"No free breakpoint slots\"}\n";
        sim_breakpoints[slot].address = (uint16_t)a;
        sim_breakpoints[slot].enabled = enabled != 0;
        sim_bp_suppress = false;
        snprintf(buf,sizeof(buf),"{\"ok\":true,\"slot\":%d,\"address\":%d,\"enabled\":%s}\n",
            slot,a,enabled != 0 ? "true" : "false");
        return buf;
    }
    if (cmd=="dbg_break_clear") {
        int slot=jgi(json,"slot",-1), addr=jgi(json,"address",-1);
        bool removed=false;
        if (slot >= 0) {
            if (slot > 3) return "{\"ok\":false,\"error\":\"'slot' must be 0..3\"}\n";
            removed = sim_breakpoints[slot].enabled;
            sim_breakpoints[slot].enabled = false;
        } else if (addr >= 0) {
            for (int i=0; i<SIM_BREAKPOINTS; i++) {
                if (sim_breakpoints[i].enabled && sim_breakpoints[i].address == (uint16_t)addr) {
                    sim_breakpoints[i].enabled = false;
                    removed = true;
                }
            }
        } else {
            return "{\"ok\":false,\"error\":\"Missing 'slot' or 'address'\"}\n";
        }
        snprintf(buf,sizeof(buf),"{\"ok\":true,\"removed\":%s}\n",removed ? "true" : "false");
        return buf;
    }
    if (cmd=="dbg_break_clear_all") {
        for (int i=0; i<SIM_BREAKPOINTS; i++) sim_breakpoints[i].enabled = false;
        sim_bp_hit_latched=false;
        sim_bp_suppress=false;
        return "{\"ok\":true}\n";
    }
    if (cmd=="dbg_break_list") {
        return sim_break_list_json();
    }
    if (cmd=="dbg_trace") {
        int count=jgi(json,"count",SIM_TRACE_DEPTH);
        if (count < 1 || count > SIM_TRACE_DEPTH) return "{\"ok\":false,\"error\":\"'count' must be 1..64\"}\n";
        return sim_trace_json(count);
    }
    if (cmd=="dbg_read_memory") {
        int a=jgi(json,"address"), len=jgi(json,"length");
        if (a<0||len<0) return "{\"ok\":false,\"error\":\"Need address, length\"}\n";
        if (len<1||len>4096) return "{\"ok\":false,\"error\":\"length must be 1..4096\"}\n";
        static const char hex[]="0123456789ABCDEF";
        std::string out; out.reserve((size_t)len*2);
        for (int i=0;i<len;i++) {
            uint8_t v=peek_ram((uint16_t)(a+i));
            out.push_back(hex[v>>4]); out.push_back(hex[v&0x0F]);
        }
        std::ostringstream ss;
        ss<<"{\"ok\":true,\"address\":"<<a<<",\"length\":"<<len<<",\"hex\":\""<<out<<"\"}\n";
        return ss.str();
    }
    if (cmd=="run_cycles") {
        int cy=jgi(json,"cycles"); if (cy<0) return "{\"ok\":false,\"error\":\"Missing 'cycles'\"}\n";
        top->dbg_pause=0; top->dbg_cpu_resume=0; for (int i=0;i<cy;i++) step_clock(); top->dbg_pause=1;
        snprintf(buf,sizeof(buf),"{\"ok\":true,\"cycles_executed\":%d,\"a\":%d,\"x\":%d,\"y\":%d,"
            "\"sp\":%d,\"pc\":%d,\"paused\":true}\n",cy,
            top->dbg_cpu_a,top->dbg_cpu_x,top->dbg_cpu_y,top->dbg_cpu_sp,top->dbg_cpu_pc);
        return buf;
    }
    // Async commands: wait_ready and watch — start polling, respond later
    if (cmd=="wait_ready") {
        pending_op.type = PendingOp::WAIT_READY;
        pending_op.wait_text = jgs(json,"text"); if (pending_op.wait_text.empty()) pending_op.wait_text="Ready";
        int tmo = jgi(json,"timeout_ms",5000);
        pending_op.deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(tmo);
        // Check immediately
        int row = screen_find_row(pending_op.wait_text);
        if (row >= 0) {
            pending_op.type = PendingOp::NONE;
            snprintf(buf,sizeof(buf),"{\"ok\":true,\"found\":true,\"row\":%d}\n",row);
            return buf;
        }
        return ""; // empty = deferred response
    }
    if (cmd=="watch") {
        int a=jgi(json,"address"),v=jgi(json,"value");
        if (a<0) return "{\"ok\":false,\"error\":\"Missing 'address'\"}\n";
        if (v<0) return "{\"ok\":false,\"error\":\"Missing 'value'\"}\n";
        pending_op.type = PendingOp::WATCH;
        pending_op.watch_addr = a; pending_op.watch_value = v;
        int tmo=jgi(json,"timeout_ms",5000);
        pending_op.deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(tmo);
        // Check immediately
        uint8_t actual = peek_ram((uint16_t)a);
        if (actual == (uint8_t)v) {
            pending_op.type = PendingOp::NONE;
            snprintf(buf,sizeof(buf),"{\"ok\":true,\"matched\":true,\"address\":%d,\"expected\":%d,\"actual\":%d}\n",a,v,actual);
            return buf;
        }
        return ""; // deferred
    }
    return "{\"ok\":false,\"error\":\"Unknown command: " + cmd + "\"}\n";
}

// Check pending async operations between frames
static void check_pending() {
    if (pending_op.type == PendingOp::NONE) return;
    auto now = std::chrono::steady_clock::now();
    bool timed_out = (now >= pending_op.deadline);
    char buf[256];
    if (pending_op.type == PendingOp::WAIT_READY) {
        int row = screen_find_row(pending_op.wait_text);
        if (row >= 0) {
            snprintf(buf,sizeof(buf),"{\"ok\":true,\"found\":true,\"row\":%d}\n",row);
            pending_response = buf; pending_op.type = PendingOp::NONE;
        } else if (timed_out) {
            pending_response = "{\"ok\":true,\"found\":false}\n";
            pending_op.type = PendingOp::NONE;
        }
    } else if (pending_op.type == PendingOp::WATCH) {
        uint8_t actual = peek_ram((uint16_t)pending_op.watch_addr);
        if (actual == (uint8_t)pending_op.watch_value) {
            snprintf(buf,sizeof(buf),"{\"ok\":true,\"matched\":true,\"address\":%d,\"expected\":%d,\"actual\":%d}\n",
                pending_op.watch_addr,pending_op.watch_value,actual);
            pending_response = buf; pending_op.type = PendingOp::NONE;
        } else if (timed_out) {
            snprintf(buf,sizeof(buf),"{\"ok\":true,\"matched\":false,\"address\":%d,\"expected\":%d,\"actual\":%d}\n",
                pending_op.watch_addr,pending_op.watch_value,actual);
            pending_response = buf; pending_op.type = PendingOp::NONE;
        }
    }
}

// ── TCP server ──
static int tcp_listen_fd=-1, tcp_client_fd=-1;
static std::string tcp_recv_buf;

static void tcp_init(int port) {
    tcp_listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if (tcp_listen_fd<0) { perror("socket"); return; }
    int opt=1; setsockopt(tcp_listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in addr={}; addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK); addr.sin_port=htons(port);
    if (bind(tcp_listen_fd,(struct sockaddr*)&addr,sizeof(addr))<0) {
        perror("bind"); close(tcp_listen_fd); tcp_listen_fd=-1; return;
    }
    listen(tcp_listen_fd,1); fcntl(tcp_listen_fd,F_SETFL,O_NONBLOCK);
    fprintf(stderr,"TCP server listening on port %d\n",port); fflush(stderr);
}

static void tcp_send(const std::string &resp) {
    if (tcp_client_fd<0||resp.empty()) return;
    ssize_t sent=0;
    while (sent<(ssize_t)resp.size()) {
        ssize_t w=send(tcp_client_fd,resp.c_str()+sent,resp.size()-sent,0);
        if (w<=0) { close(tcp_client_fd); tcp_client_fd=-1; return; }
        sent+=w;
    }
}

static void tcp_poll() {
    if (tcp_listen_fd<0) return;

    // Send deferred response if ready
    if (!pending_response.empty()) {
        tcp_send(pending_response);
        pending_response.clear();
    }

    // Accept
    if (tcp_client_fd<0) {
        tcp_client_fd=accept(tcp_listen_fd,NULL,NULL);
        if (tcp_client_fd>=0) { fcntl(tcp_client_fd,F_SETFL,O_NONBLOCK); tcp_recv_buf.clear(); }
    }
    if (tcp_client_fd<0) return;

    // Don't process new commands while an async op is pending
    if (pending_op.type != PendingOp::NONE) return;

    // Read
    char buf[4096]; ssize_t n=recv(tcp_client_fd,buf,sizeof(buf),0);
    if (n>0) tcp_recv_buf.append(buf,n);
    else if (n==0) { close(tcp_client_fd); tcp_client_fd=-1; tcp_recv_buf.clear(); return; }

    // Process one complete line
    size_t pos;
    if ((pos=tcp_recv_buf.find('\n'))!=std::string::npos) {
        std::string line=tcp_recv_buf.substr(0,pos); tcp_recv_buf.erase(0,pos+1);
        if (!line.empty()) {
            std::string resp=process_command(line);
            if (!resp.empty()) tcp_send(resp); // empty = deferred (async)
        }
    }
}
static void tcp_cleanup() { if (tcp_client_fd>=0) close(tcp_client_fd); if (tcp_listen_fd>=0) close(tcp_listen_fd); }

// ── Main ──
int main(int argc, char **argv) {
    Verilated::commandArgs(argc,argv);
    int tcp_port=DEFAULT_TCP_PORT;
    for (int i=1;i<argc;i++) if (strcmp(argv[i],"--port")==0&&i+1<argc) tcp_port=atoi(argv[++i]);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0) { fprintf(stderr,"SDL init failed: %s\n",SDL_GetError()); return 1; }

    // Init audio
    SDL_AudioSpec want={}, have={};
    want.freq=AUDIO_RATE; want.format=AUDIO_S16SYS; want.channels=2;
    want.samples=AUDIO_BUFSIZE; want.callback=audio_callback;
    SDL_AudioDeviceID audio_dev=SDL_OpenAudioDevice(NULL,0,&want,&have,0);
    if (audio_dev>0) { SDL_PauseAudioDevice(audio_dev,0); fprintf(stderr,"Audio: %dHz stereo\n",have.freq); }
    else fprintf(stderr,"Audio init failed: %s\n",SDL_GetError());
    SDL_Window *win=SDL_CreateWindow("e6502 FPGA",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,DISP_W*SCALE,DISP_H*SCALE,SDL_WINDOW_SHOWN);
    SDL_Renderer *ren=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren,DISP_W,DISP_H);
    SDL_Texture *tex=SDL_CreateTexture(ren,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,DISP_W,DISP_H);

    top=new Vtop;
    top->rst=1;top->irq_n=1;top->nmi_n=1;top->key_valid=0;top->key_data=0;
    top->dbg_peek_en=0;top->dbg_poke_en=0;top->dbg_pause=0;top->dbg_cpu_resume=0;
    top->dbg_vmem_we=0;top->dbg_vmem_re=0;top->dbg_vmem_space=0;top->dbg_vmem_addr=0;top->dbg_vmem_data=0;
    top->dbg_rom_we=0;top->dbg_rom_idx=0;top->dbg_rom_addr=0;top->dbg_rom_data=0;
    top->dbg_cpu_reset=0;top->dbg_system_reset=0;top->brg_sdram_b_we=0;top->brg_sdram_b_addr=0;top->brg_sdram_b_din=0;
    top->dbg_vram_read_en=0;top->dbg_vram_space=0;top->dbg_vram_addr=0;
    for (int i=0;i<100;i++) step_clock();
    top->rst=0;

    tcp_init(tcp_port);

    bool running=true; int pending_char=-1, key_cooldown=0;
    const char *paste_buf=NULL; int paste_pos=0;
    int h_pos=0,v_pos=0; bool prev_vsync=true;
    memset(framebuf,0,sizeof(framebuf));

    while (running) {
        tcp_poll();
        check_pending();

        for (int i=0;i<FRAME_CLKS;i++) {
            step_clock();
            audio_push_sample(top->audio_l, top->audio_r);
            if (top->vid_de) {
                if (h_pos<DISP_W&&v_pos<DISP_H) {
                    uint8_t r=top->vid_r,g=top->vid_g,b=top->vid_b;
                    framebuf[v_pos*DISP_W+h_pos]=0xFF000000|((uint32_t)(r|(r<<4))<<16)|((uint32_t)(g|(g<<4))<<8)|(uint32_t)(b|(b<<4));
                }
                h_pos++;
            }
            if (!top->vid_de&&h_pos>0) { h_pos=0; v_pos++; }
            bool cv=top->vid_vsync; if (prev_vsync&&!cv) { v_pos=0;h_pos=0; } prev_vsync=cv;

            if (key_cooldown>0) { key_cooldown--; top->key_valid=0; }
            else if (!key_queue.empty()) { top->key_valid=1;top->key_data=key_queue.front();key_queue.pop();key_cooldown=5000; }
            else if (pending_char>=0) { top->key_valid=1;top->key_data=pending_char;pending_char=-1;key_cooldown=5000; }
            else if (paste_buf&&paste_buf[paste_pos]) {
                char c=paste_buf[paste_pos++]; if (c=='\n') c='\r';
                top->key_valid=1;top->key_data=(uint8_t)c;key_cooldown=15000;
            } else { if (paste_buf) { SDL_free((void*)paste_buf);paste_buf=NULL;paste_pos=0; } top->key_valid=0; }
        }

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type==SDL_QUIT) running=false;
            else if (ev.type==SDL_KEYDOWN) {
                if (ev.key.keysym.sym==SDLK_F12) running=false;
                else if (ev.key.keysym.sym==SDLK_v&&(ev.key.keysym.mod&KMOD_GUI)) {
                    if (SDL_HasClipboardText()) { if (paste_buf) SDL_free((void*)paste_buf);paste_buf=SDL_GetClipboardText();paste_pos=0; }
                } else if (!paste_buf&&pending_char<0&&key_queue.empty()) {
                    int ch=sdl_key_to_ascii(ev.key.keysym.sym,ev.key.keysym.mod); if (ch>=0) pending_char=ch;
                }
            }
        }
        SDL_UpdateTexture(tex,NULL,framebuf,DISP_W*sizeof(uint32_t));
        SDL_RenderClear(ren);SDL_RenderCopy(ren,tex,NULL,NULL);SDL_RenderPresent(ren);
    }
    tcp_cleanup();
    if (audio_dev>0) SDL_CloseAudioDevice(audio_dev);
    delete top;
    SDL_DestroyTexture(tex);SDL_DestroyRenderer(ren);SDL_DestroyWindow(win);SDL_Quit();
    return 0;
}
