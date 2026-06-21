// debug.c — NovaHost 6503 debug server (newline-delimited JSON) for the Arty PS.
//
// Speaks the same protocol as the ESP32 NovaHost DebugServer and the C#
// NovaDebugClient (e6502.NovaHost/NovaDebugClient.cs), so the `nova` CLI's
// `vm`/`emulator` and `keyboard` verbs work with `--remote <ip>`:
//
//   request : one JSON object per line, e.g. {"command":"peek","address":40960}
//   reply   : one JSON object + '\n', e.g. {"ok":true,"address":40960,"value":0}
//
// Transport is lwIP RAW (NO_SYS), callback-driven like mgmt.c; net_init() brings
// lwIP up, debug_init() adds the 6503 listener, and net_poll()'s xemacif_input
// pump fires the recv/sent callbacks. One client at a time.
//
// Handlers wire onto the AXI fio_bridge primitives already in main.c (peek/poke,
// kb_emit, vmem_read, vm_reset). VGC video memory is read-only over the bridge,
// so fill_vram and the emulator-only debugger commands report not-supported.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "xil_printf.h"

unsigned char dbg_peek(unsigned a);                 // main.c — AXI bridge primitives
void          dbg_poke(unsigned a, unsigned char d);
unsigned char dbg_vmem(unsigned space, unsigned addr);
void          kb_emit(unsigned char c);
void          vm_reset(void);

#define DBG_PORT   6503
#define RXCAP      8192u
#define TXCAP      8192u
#define SCREEN_COLS 80
#define SCREEN_ROWS 50
#define REG_CURSOR_X      0xA003
#define REG_CURSOR_Y      0xA004
#define REG_CURSOR_ENABLE 0xA005
#define VGC_SPACE_CHAR 1u

static struct tcp_pcb *g_pcb;
static char     g_rx[RXCAP];
static uint32_t g_rxlen;
static char     g_tx[TXCAP];
static uint32_t g_txlen, g_txoff;

// ---- tiny JSON readers (flat request objects) -------------------------------
static const char *json_after_key(const char *s, const char *key) {
    char pat[40];
    snprintf(pat, sizeof pat, "\"%s\"", key);
    const char *p = strstr(s, pat);
    if (!p) return 0;
    p += strlen(pat);
    while (*p == ' ' || *p == '\t') p++;
    if (*p != ':') return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

static int json_int(const char *s, const char *key, int defv) {
    const char *p = json_after_key(s, key);
    if (!p) return defv;
    int neg = 0;
    if (*p == '-') { neg = 1; p++; }
    if (*p < '0' || *p > '9') return defv;
    long v = 0;
    while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); p++; }
    return neg ? -(int)v : (int)v;
}

// Extract a JSON string value, unescaping. Returns 1 if the key was a string.
static int json_str(const char *s, const char *key, char *out, int cap) {
    const char *p = json_after_key(s, key);
    if (!p || *p != '"') { if (cap) out[0] = 0; return 0; }
    p++;
    int n = 0;
    while (*p && *p != '"') {
        char c = *p++;
        if (c == '\\' && *p) {
            char e = *p++;
            switch (e) {
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                case 'b': c = '\b'; break;
                case 'f': c = '\f'; break;
                case '/': c = '/';  break;
                case '"': c = '"';  break;
                case '\\': c = '\\'; break;
                case 'u': {                       // \uXXXX -> low byte
                    int h = 0;
                    for (int i = 0; i < 4 && *p; i++) {
                        char d = *p++;
                        h <<= 4;
                        if (d >= '0' && d <= '9') h |= d - '0';
                        else if (d >= 'a' && d <= 'f') h |= d - 'a' + 10;
                        else if (d >= 'A' && d <= 'F') h |= d - 'A' + 10;
                    }
                    c = (char)(h & 0xFF);
                    break;
                }
                default: c = e; break;
            }
        }
        if (n < cap - 1) out[n++] = c;
    }
    out[n] = 0;
    return 1;
}

// ---- response send (sndbuf-aware drain via tcp_sent) ------------------------
static void dbg_flush(struct tcp_pcb *pcb) {
    while (g_txoff < g_txlen) {
        uint32_t rem = g_txlen - g_txoff;
        u16_t snd = tcp_sndbuf(pcb);
        if (snd == 0) break;
        uint32_t n = rem < snd ? rem : snd;
        if (tcp_write(pcb, g_tx + g_txoff, (u16_t)n, TCP_WRITE_FLAG_COPY) != ERR_OK) break;
        g_txoff += n;
    }
    tcp_output(pcb);
}

static void respond(struct tcp_pcb *pcb, const char *json) {
    int n = snprintf(g_tx, TXCAP, "%s\n", json);
    if (n < 0) return;
    g_txlen = (uint32_t)(n > (int)TXCAP ? (int)TXCAP : n);
    g_txoff = 0;
    dbg_flush(pcb);
}

static void respond_ok(struct tcp_pcb *pcb) { respond(pcb, "{\"ok\":true}"); }
static void respond_err(struct tcp_pcb *pcb, const char *msg) {
    char b[160];
    snprintf(b, sizeof b, "{\"ok\":false,\"error\":\"%s\"}", msg);
    respond(pcb, b);
}

// Append one screen char, JSON-escaped, into buf (printable ASCII only).
static int put_screen_char(char *buf, int pos, int cap, unsigned char ch) {
    char c = (ch >= 0x20 && ch < 0x7F) ? (char)ch : ' ';
    if (c == '"' || c == '\\') { if (pos < cap - 2) { buf[pos++] = '\\'; buf[pos++] = c; } }
    else if (pos < cap - 1) buf[pos++] = c;
    return pos;
}

// ---- key name -> VGC key code (mirror of the ESP32 debug server) ------------
static int key_code(const char *key) {
    char u[24];
    int i = 0;
    for (; key[i] && i < (int)sizeof u - 1; i++) {
        char c = key[i];
        u[i] = (c >= 'a' && c <= 'z') ? (char)(c - 0x20) : c;
    }
    u[i] = 0;
    if (!strcmp(u, "ENTER") || !strcmp(u, "CR") || !strcmp(u, "RETURN")) return 0x0D;
    if (!strcmp(u, "BACKSPACE") || !strcmp(u, "BS"))   return 0x08;
    if (!strcmp(u, "TAB"))                              return 0x09;
    if (!strcmp(u, "ESC") || !strcmp(u, "ESCAPE"))     return 0x1B;
    if (!strcmp(u, "SPACE"))                            return 0x20;
    if (!strcmp(u, "LEFT"))  return 0x1C;
    if (!strcmp(u, "RIGHT")) return 0x1D;
    if (!strcmp(u, "UP"))    return 0x1E;
    if (!strcmp(u, "DOWN"))  return 0x1F;
    if (!strcmp(u, "HOME"))  return 0x02;
    if (!strcmp(u, "END"))   return 0x05;
    if (!strcmp(u, "DELETE") || !strcmp(u, "DEL")) return 0x7F;
    if (!strcmp(u, "CTRL-C") || !strcmp(u, "BREAK")) return 0x03;
    if ((strlen(u) == 6 && !strncmp(u, "CTRL-", 5))) {
        char ch = u[5];
        if (ch >= 'A' && ch <= 'Z') return 1 + ch - 'A';
    }
    if (strlen(key) == 1) return (unsigned char)key[0];
    return -1;                                          // unknown -> ignore
}

// ---- command handlers -------------------------------------------------------
static void do_peek(struct tcp_pcb *pcb, const char *s) {
    int addr = json_int(s, "address", -1);
    if (addr < 0) { respond_err(pcb, "Missing 'address'"); return; }
    char b[64];
    snprintf(b, sizeof b, "{\"ok\":true,\"address\":%d,\"value\":%d}", addr, dbg_peek((unsigned)addr));
    respond(pcb, b);
}

static void do_peek_block(struct tcp_pcb *pcb, const char *s) {
    int addr = json_int(s, "address", -1);
    int count = json_int(s, "count", -1);
    if (addr < 0) { respond_err(pcb, "Missing 'address'"); return; }
    if (count < 1 || count > 256) { respond_err(pcb, "'count' must be 1..256"); return; }
    int pos = snprintf(g_tx, TXCAP, "{\"ok\":true,\"address\":%d,\"count\":%d,\"values\":[", addr, count);
    for (int i = 0; i < count; i++)
        pos += snprintf(g_tx + pos, TXCAP - pos, "%s%d", i ? "," : "", dbg_peek((unsigned)(addr + i)));
    pos += snprintf(g_tx + pos, TXCAP - pos, "]}\n");
    g_txlen = (uint32_t)pos; g_txoff = 0;
    dbg_flush(pcb);
}

static void do_poke(struct tcp_pcb *pcb, const char *s) {
    int addr = json_int(s, "address", -1);
    int val  = json_int(s, "value", -1);
    if (addr < 0) { respond_err(pcb, "Missing 'address'"); return; }
    if (val  < 0) { respond_err(pcb, "Missing 'value'"); return; }
    dbg_poke((unsigned)addr, (unsigned char)val);
    respond_ok(pcb);
}

static void do_read_vram(struct tcp_pcb *pcb, const char *s) {
    int space = json_int(s, "space", -1);
    int addr = json_int(s, "address", -1);
    int length = json_int(s, "length", 1);
    if (space < 0 || space > 7) { respond_err(pcb, "Missing/invalid 'space'"); return; }
    if (addr < 0) { respond_err(pcb, "Missing 'address'"); return; }
    if (length < 1 || length > 256) { respond_err(pcb, "'length' must be 1..256"); return; }
    int pos = snprintf(g_tx, TXCAP, "{\"ok\":true,\"space\":%d,\"address\":%d", space, addr);
    if (length == 1) {
        pos += snprintf(g_tx + pos, TXCAP - pos, ",\"value\":%d}\n", dbg_vmem((unsigned)space, (unsigned)addr));
    } else {
        pos += snprintf(g_tx + pos, TXCAP - pos, ",\"data\":[");
        for (int i = 0; i < length; i++)
            pos += snprintf(g_tx + pos, TXCAP - pos, "%s%d", i ? "," : "", dbg_vmem((unsigned)space, (unsigned)(addr + i)));
        pos += snprintf(g_tx + pos, TXCAP - pos, "]}\n");
    }
    g_txlen = (uint32_t)pos; g_txoff = 0;
    dbg_flush(pcb);
}

static void do_send_key(struct tcp_pcb *pcb, const char *s) {
    char key[24];
    json_str(s, "key", key, sizeof key);
    int code = key_code(key);
    if (code >= 0) kb_emit((unsigned char)code);
    respond_ok(pcb);
}

static void do_type_text(struct tcp_pcb *pcb, const char *s) {
    static char text[4096];
    json_str(s, "text", text, sizeof text);
    int prev_cr = 0;
    for (int i = 0; text[i]; i++) {
        char ch = text[i];
        if (ch == '\n') { if (prev_cr) { prev_cr = 0; continue; } ch = '\r'; }
        prev_cr = (ch == '\r');
        kb_emit((unsigned char)ch);
    }
    respond_ok(pcb);
}

static void do_read_screen(struct tcp_pcb *pcb) {
    int pos = snprintf(g_tx, TXCAP, "{\"ok\":true,\"lines\":[");
    for (int row = 0; row < SCREEN_ROWS; row++) {
        if (row) g_tx[pos++] = ',';
        g_tx[pos++] = '"';
        for (int col = 0; col < SCREEN_COLS; col++)
            pos = put_screen_char(g_tx, pos, TXCAP, dbg_vmem(VGC_SPACE_CHAR, (unsigned)(row * SCREEN_COLS + col)));
        g_tx[pos++] = '"';
    }
    pos += snprintf(g_tx + pos, TXCAP - pos, "],\"cursor_x\":%d,\"cursor_y\":%d}\n",
                    dbg_peek(REG_CURSOR_X), dbg_peek(REG_CURSOR_Y));
    g_txlen = (uint32_t)pos; g_txoff = 0;
    dbg_flush(pcb);
}

static void do_read_line(struct tcp_pcb *pcb, const char *s) {
    int row = json_int(s, "row", -1);
    if (row < 0) row = dbg_peek(REG_CURSOR_Y);
    if (row < 0 || row >= SCREEN_ROWS) { respond_err(pcb, "Invalid row"); return; }
    int pos = snprintf(g_tx, TXCAP, "{\"ok\":true,\"row\":%d,\"text\":\"", row);
    for (int col = 0; col < SCREEN_COLS; col++)
        pos = put_screen_char(g_tx, pos, TXCAP, dbg_vmem(VGC_SPACE_CHAR, (unsigned)(row * SCREEN_COLS + col)));
    pos += snprintf(g_tx + pos, TXCAP - pos, "\"}\n");
    g_txlen = (uint32_t)pos; g_txoff = 0;
    dbg_flush(pcb);
}

static void do_get_cursor(struct tcp_pcb *pcb) {
    char b[80];
    snprintf(b, sizeof b, "{\"ok\":true,\"x\":%d,\"y\":%d,\"enabled\":%s}",
             dbg_peek(REG_CURSOR_X), dbg_peek(REG_CURSOR_Y),
             (dbg_peek(REG_CURSOR_ENABLE) & 0x01) ? "true" : "false");
    respond(pcb, b);
}

static void do_reset(struct tcp_pcb *pcb, const char *s, int cold) {
    (void)s;
    vm_reset();
    // Best-effort settle so a follow-up type_text lands after the READY prompt
    // (the 6502 reaches READY within a few ms; wait_ready is not pollable here).
    for (volatile int i = 0; i < 4000000; i++) {}
    char b[64];
    snprintf(b, sizeof b, "{\"ok\":true,\"%s\":true}", cold ? "cold_start" : "reset");
    respond(pcb, b);
}

// ---- dispatch ---------------------------------------------------------------
static void dispatch(struct tcp_pcb *pcb, const char *line) {
    char cmd[32];
    if (!json_str(line, "command", cmd, sizeof cmd)) { respond_err(pcb, "Missing 'command'"); return; }

    if      (!strcmp(cmd, "peek"))        do_peek(pcb, line);
    else if (!strcmp(cmd, "peek_block"))  do_peek_block(pcb, line);
    else if (!strcmp(cmd, "poke"))        do_poke(pcb, line);
    else if (!strcmp(cmd, "read_vram"))   do_read_vram(pcb, line);
    else if (!strcmp(cmd, "send_key"))    do_send_key(pcb, line);
    else if (!strcmp(cmd, "type_text"))   do_type_text(pcb, line);
    else if (!strcmp(cmd, "read_screen")) do_read_screen(pcb);
    else if (!strcmp(cmd, "read_line"))   do_read_line(pcb, line);
    else if (!strcmp(cmd, "get_cursor"))  do_get_cursor(pcb);
    else if (!strcmp(cmd, "vm_reset"))    do_reset(pcb, line, 0);
    else if (!strcmp(cmd, "cold_start"))  do_reset(pcb, line, 1);
    else if (!strcmp(cmd, "wait_ready"))  respond_ok(pcb);   // not pollable; best-effort
    else if (!strcmp(cmd, "fill_vram"))   respond_err(pcb, "fill_vram not supported on this host");
    else if (!strcmp(cmd, "run_cycles"))  respond_err(pcb, "run_cycles not supported (free-running CPU)");
    else if (!strncmp(cmd, "dbg_", 4))    respond_err(pcb, "debugger not supported on this host");
    else { char b[80]; snprintf(b, sizeof b, "{\"ok\":false,\"error\":\"Unknown command: %s\"}", cmd); respond(pcb, b); }
}

// ---- lwIP callbacks ---------------------------------------------------------
static err_t dbg_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    (void)arg; (void)len;
    if (g_txoff < g_txlen) dbg_flush(pcb);
    return ERR_OK;
}

static err_t dbg_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    (void)arg; (void)err;
    if (p == NULL) { if (pcb == g_pcb) { g_pcb = NULL; g_rxlen = 0; } tcp_close(pcb); return ERR_OK; }
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        if (g_rxlen + q->len < RXCAP) { memcpy(g_rx + g_rxlen, q->payload, q->len); g_rxlen += q->len; }
        else g_rxlen = 0;                                  // overflow: resync
    }
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);

    // Process each complete '\n'-terminated line.
    for (;;) {
        char *nl = (char *)memchr(g_rx, '\n', g_rxlen);
        if (!nl) break;
        *nl = 0;
        dispatch(pcb, g_rx);
        uint32_t consumed = (uint32_t)(nl - g_rx) + 1;
        uint32_t leftover = g_rxlen - consumed;
        if (leftover) memmove(g_rx, g_rx + consumed, leftover);
        g_rxlen = leftover;
    }
    return ERR_OK;
}

static err_t dbg_accept(void *arg, struct tcp_pcb *pcb, err_t err) {
    (void)arg; (void)err;
    if (g_pcb && g_pcb != pcb) tcp_close(g_pcb);           // one client at a time
    g_pcb = pcb;
    g_rxlen = 0;
    tcp_recv(pcb, dbg_recv);
    tcp_sent(pcb, dbg_sent);
    xil_printf("[dbg] client connected (6503)\r\n");
    return ERR_OK;
}

void debug_init(void) {
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb && tcp_bind(pcb, IP_ANY_TYPE, DBG_PORT) == ERR_OK) {
        pcb = tcp_listen(pcb);
        tcp_accept(pcb, dbg_accept);
        xil_printf("[dbg] NovaHost debug server up on TCP %d\r\n", DBG_PORT);
    } else {
        xil_printf("[dbg] ERROR: cannot bind TCP %d\r\n", DBG_PORT);
    }
}
