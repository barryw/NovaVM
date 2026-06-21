// mgmt.c — NovaHost 6504 management server (NVH1 + CBOR) for the Arty PS.
//
// Speaks the same wire protocol as the ESP32 NovaHost ManagementServer and the
// C# NovaHostManagementClient (e6502.NovaHost/NovaHostManagementClient.cs), so
// the `nova` CLI can drive this board with `--remote <ip>`:
//
//   24-byte frame header:
//     "NVH1"(4) | version u16 | command u16 | flags u16 | requestId u32 |
//     cborLen u32 | rawLen u32 | (2 bytes pad)            -- all big-endian
//   then a CBOR map payload (cborLen bytes) + raw bytes (rawLen bytes).
//
//   Response envelope is a CBOR map: { ok: bool, error: null|text, code: text,
//   data: <value> }.
//
// Transport is lwIP RAW (NO_SYS), so this is callback-driven: net_init() brings
// up lwIP + the GEM, mgmt_init() adds the 6504 listener, and net_poll()'s
// xemacif_input() pump fires our recv/sent callbacks. One client at a time
// (request/response is serialised by the client), matching the ESP32 server.
//
// Phase #19: framing + CBOR + Hello/GetStatus. Later phases add the file/drive/
// runtime/VmReset and debug handlers (see docs/handoff-2026-06-21-...).

#include <string.h>
#include <stdio.h>
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "xil_printf.h"
#include "ff.h"
#include "drives.h"

void net_ip(char *out, int n);          // net.c — current DHCP IPv4 as text
void vm_reset(void);                     // main.c — pulse the 6502 reset line

#define MGMT_PORT       6504
#define MGMT_VERSION    1
#define HDR_BYTES       24
#define RX_CAP          40000u          // accumulate one inbound frame
#define RESP_CAP        16384u          // build one outbound frame

// ---- inbound frame accumulation (single active connection) ------------------
static struct tcp_pcb *g_pcb;
static uint8_t  g_rx[RX_CAP];
static uint32_t g_rxlen;

// ---- outbound response buffer ----------------------------------------------
static uint8_t  g_resp[RESP_CAP];
static uint32_t g_resp_len;             // total bytes to send
static uint32_t g_resp_off;             // bytes handed to lwIP so far
static uint32_t cw;                     // CBOR write cursor (index into g_resp)

// ---- host state ------------------------------------------------------------
static int g_sd_mounted = 0;
void mgmt_set_sd(int mounted) { g_sd_mounted = mounted; }

// ---- upload staging (WriteFileBegin/Chunk/Commit/Abort) --------------------
static FIL      g_up;
static int      g_up_active;
static uint32_t g_up_written, g_up_expected;
static char     g_up_path[300], g_up_tmp[320];

// ---- CBOR encoder (writes into g_resp past the 24-byte header) --------------
static void cb_raw(uint8_t b) { if (cw < RESP_CAP) g_resp[cw++] = b; }

static void cb_type(uint8_t major, unsigned long long v) {
    uint8_t pfx = (uint8_t)(major << 5);
    if (v < 24)               { cb_raw(pfx | (uint8_t)v); }
    else if (v <= 0xFF)       { cb_raw(pfx | 24); cb_raw((uint8_t)v); }
    else if (v <= 0xFFFF)     { cb_raw(pfx | 25); cb_raw((uint8_t)(v >> 8)); cb_raw((uint8_t)v); }
    else if (v <= 0xFFFFFFFFULL) {
        cb_raw(pfx | 26);
        cb_raw((uint8_t)(v >> 24)); cb_raw((uint8_t)(v >> 16));
        cb_raw((uint8_t)(v >> 8));  cb_raw((uint8_t)v);
    } else {
        cb_raw(pfx | 27);
        for (int s = 56; s >= 0; s -= 8) cb_raw((uint8_t)(v >> s));
    }
}
static void cb_uint(unsigned long long v) { cb_type(0, v); }
static void cb_int(long long v) { if (v >= 0) cb_type(0, (unsigned long long)v); else cb_type(1, (unsigned long long)(-1 - v)); }
static void cb_bool(int v) { cb_raw(v ? 0xF5 : 0xF4); }
static void cb_null(void)  { cb_raw(0xF6); }
static void cb_map(uint32_t n) { cb_type(5, n); }
static void cb_arr(uint32_t n) { cb_type(4, n); }
static void cb_text(const char *s) {
    if (!s) s = "";
    size_t n = strlen(s);
    cb_type(3, n);
    for (size_t i = 0; i < n; i++) cb_raw((uint8_t)s[i]);
}
static void cb_kv_text(const char *k, const char *v) { cb_text(k); cb_text(v); }
static void cb_kv_bool(const char *k, int v)          { cb_text(k); cb_bool(v); }
static void cb_kv_uint(const char *k, unsigned long long v) { cb_text(k); cb_uint(v); }

// ---- frame build + send -----------------------------------------------------
static void put_be16(int off, uint16_t v) { g_resp[off] = (uint8_t)(v >> 8); g_resp[off + 1] = (uint8_t)v; }
static void put_be32(int off, uint32_t v) {
    g_resp[off]   = (uint8_t)(v >> 24); g_resp[off + 1] = (uint8_t)(v >> 16);
    g_resp[off + 2] = (uint8_t)(v >> 8); g_resp[off + 3] = (uint8_t)v;
}

// Begin an envelope frame: header placeholder + CBOR map(4){ok,error,code,data:}.
// After this the caller appends the `data` value, then calls resp_send().
static void resp_begin(uint16_t command, uint32_t reqid, int ok,
                       const char *code, const char *error) {
    memcpy(g_resp, "NVH1", 4);
    put_be16(4, MGMT_VERSION);
    put_be16(6, command);
    put_be16(8, 0);                      // flags
    put_be32(10, reqid);
    put_be32(14, 0);                     // cborLen (patched in resp_send)
    put_be32(18, 0);                     // rawLen
    g_resp[22] = g_resp[23] = 0;         // pad
    cw = HDR_BYTES;

    cb_map(4);
    cb_kv_bool("ok", ok);
    cb_text("error"); if (error) cb_text(error); else cb_null();
    cb_kv_text("code", code ? code : "");
    cb_text("data");                     // value appended by caller next
}

static void mgmt_flush(struct tcp_pcb *pcb) {
    while (g_resp_off < g_resp_len) {
        uint32_t rem = g_resp_len - g_resp_off;
        u16_t snd = tcp_sndbuf(pcb);
        if (snd == 0) break;
        uint32_t n = rem < snd ? rem : snd;
        if (tcp_write(pcb, g_resp + g_resp_off, (u16_t)n, TCP_WRITE_FLAG_COPY) != ERR_OK)
            break;
        g_resp_off += n;
    }
    tcp_output(pcb);
}

static void resp_send(struct tcp_pcb *pcb) {
    put_be32(14, cw - HDR_BYTES);        // cborLen = bytes after header
    put_be32(18, 0);                     // rawLen
    g_resp_len = cw;
    g_resp_off = 0;
    mgmt_flush(pcb);
}

// Append raw (non-CBOR) bytes after the CBOR envelope.
static void resp_raw(const uint8_t *d, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) if (cw < RESP_CAP) g_resp[cw++] = d[i];
}

// Finish a frame that carries raw bytes. `cbor_len` is the CBOR length captured
// (cw - HDR_BYTES) right after the envelope, before resp_raw() was called.
static void resp_send_raw(struct tcp_pcb *pcb, uint32_t cbor_len) {
    put_be32(14, cbor_len);
    put_be32(18, (cw - HDR_BYTES) - cbor_len);
    g_resp_len = cw;
    g_resp_off = 0;
    mgmt_flush(pcb);
}

static void send_error(struct tcp_pcb *pcb, uint16_t command, uint32_t reqid,
                       const char *code, const char *msg) {
    resp_begin(command, reqid, 0, code, msg);
    cb_null();                           // data
    resp_send(pcb);
}

// ---- CBOR reader (just enough to pull keys from a request payload map) -------
typedef struct { const uint8_t *d; uint32_t n, pos; } cbr_t;

static int cbr_head(cbr_t *r, uint8_t *major, unsigned long long *arg) {
    if (r->pos >= r->n) return 0;
    uint8_t ib = r->d[r->pos++];
    *major = ib >> 5;
    uint8_t add = ib & 0x1F;
    unsigned long long v;
    if (add < 24) v = add;
    else if (add == 24) { if (r->pos + 1 > r->n) return 0; v = r->d[r->pos++]; }
    else if (add == 25) { if (r->pos + 2 > r->n) return 0; v = ((unsigned long long)r->d[r->pos] << 8) | r->d[r->pos + 1]; r->pos += 2; }
    else if (add == 26) { if (r->pos + 4 > r->n) return 0;
        v = ((unsigned long long)r->d[r->pos] << 24) | ((unsigned long long)r->d[r->pos + 1] << 16) |
            ((unsigned long long)r->d[r->pos + 2] << 8) | r->d[r->pos + 3]; r->pos += 4; }
    else if (add == 27) { if (r->pos + 8 > r->n) return 0; v = 0; for (int i = 0; i < 8; i++) v = (v << 8) | r->d[r->pos++]; }
    else return 0;
    *arg = v;
    return 1;
}

static int cbr_skip(cbr_t *r) {
    if (r->pos >= r->n) return 0;
    uint8_t ib = r->d[r->pos];
    if ((ib >> 5) == 7) { uint8_t add = ib & 0x1F; r->pos++; if (add == 24) r->pos++; return r->pos <= r->n; }
    uint8_t major; unsigned long long arg;
    if (!cbr_head(r, &major, &arg)) return 0;
    if (major == 0 || major == 1) return 1;
    if (major == 2 || major == 3) { if (r->pos + arg > r->n) return 0; r->pos += (uint32_t)arg; return 1; }
    if (major == 4) { for (unsigned long long i = 0; i < arg; i++) if (!cbr_skip(r)) return 0; return 1; }
    if (major == 5) { for (unsigned long long i = 0; i < arg * 2; i++) if (!cbr_skip(r)) return 0; return 1; }
    return 0;
}

// Position the reader at the value for `key` in a top-level map. 1 if found.
static int cbr_find(cbr_t *r, const char *key) {
    r->pos = 0;
    uint8_t major; unsigned long long cnt;
    if (!cbr_head(r, &major, &cnt) || major != 5) return 0;
    size_t klen_want = strlen(key);
    for (unsigned long long i = 0; i < cnt; i++) {
        uint8_t km; unsigned long long klen;
        if (!cbr_head(r, &km, &klen) || km != 3) return 0;
        if (r->pos + klen > r->n) return 0;
        int match = (klen_want == klen) && memcmp(r->d + r->pos, key, klen) == 0;
        r->pos += (uint32_t)klen;
        if (match) return 1;             // reader now at the value
        if (!cbr_skip(r)) return 0;
    }
    return 0;
}

static int payload_get_str(const uint8_t *p, uint32_t n, const char *key, char *out, int cap) {
    cbr_t r = { p, n, 0 };
    if (!cbr_find(&r, key)) return 0;
    uint8_t m; unsigned long long len;
    if (!cbr_head(&r, &m, &len) || m != 3 || r.pos + len > r.n) return 0;
    int k = (int)len; if (k >= cap) k = cap - 1;
    memcpy(out, r.d + r.pos, k); out[k] = 0;
    return 1;
}

static int payload_get_u32(const uint8_t *p, uint32_t n, const char *key, uint32_t *out) {
    cbr_t r = { p, n, 0 };
    if (!cbr_find(&r, key)) return 0;
    uint8_t m; unsigned long long v;
    if (!cbr_head(&r, &m, &v) || m != 0) return 0;
    *out = (uint32_t)v;
    return 1;
}

// ---- path helpers -----------------------------------------------------------
// Client paths arrive as "/foo/bar"; FatFs drive 0 wants "0:/foo/bar".
static void sd_path(const char *p, char *out, int cap) {
    if (!p || !p[0])       snprintf(out, cap, "0:/");
    else if (p[0] == '/')  snprintf(out, cap, "0:%s", p);
    else                   snprintf(out, cap, "0:/%s", p);
}

// f_mkdir each parent segment of a "0:/a/b/c" path (ignores already-exists).
static int ensure_parent_dirs(const char *full) {
    char tmp[320];
    snprintf(tmp, sizeof tmp, "%s", full);
    char *s = strchr(tmp, '/');          // skip past "0:/"
    if (!s) return 1;
    s++;
    for (char *q = s; *q; q++) {
        if (*q == '/') {
            *q = 0;
            FRESULT r = f_mkdir(tmp);
            if (r != FR_OK && r != FR_EXIST) return 0;
            *q = '/';
        }
    }
    return 1;
}

// ---- handlers ---------------------------------------------------------------
static void handle_hello(struct tcp_pcb *pcb, uint32_t reqid) {
    resp_begin(1 /*CMD_HELLO*/, reqid, 1, "ok", NULL);
    cb_map(6);
    cb_kv_text("protocol", "NVH1");
    cb_kv_uint("version", MGMT_VERSION);
    cb_kv_text("firmware", __DATE__ " " __TIME__);
    cb_kv_uint("maxPayload", 32768);
    cb_kv_uint("maxRaw", 16384);
    cb_text("capabilities");
    cb_map(9);
    cb_kv_bool("status", 1);
    cb_kv_bool("listing", 1);
    cb_kv_bool("upload", 1);
    cb_kv_bool("drives", 1);
    cb_kv_bool("runtime", 0);
    cb_kv_bool("vmReset", 1);
    cb_kv_bool("hostReboot", 0);
    cb_kv_bool("wifi", 0);
    cb_kv_bool("downloadChunks", 1);
    resp_send(pcb);
}

static void handle_get_status(struct tcp_pcb *pcb, uint32_t reqid) {
    char ip[20]; net_ip(ip, sizeof ip);

    resp_begin(2 /*CMD_GET_STATUS*/, reqid, 1, "ok", NULL);
    cb_map(5);

    cb_text("health");
    cb_map(4);
    cb_kv_bool("ok", 1);
    cb_kv_text("bootPhase", "running");
    cb_kv_bool("sdMounted", g_sd_mounted);
    cb_kv_bool("fpgaBridgeAvailable", 1);

    cb_text("sdStatus");
    cb_map(4);
    cb_kv_text("cardType", g_sd_mounted ? "SDHC" : "NONE");
    cb_kv_bool("mounted", g_sd_mounted);
    cb_kv_uint("totalBytes", 0);
    cb_kv_uint("usedBytes", 0);

    cb_text("network");
    cb_map(2);
    cb_kv_text("ip", ip);
    cb_kv_text("transport", "ethernet");

    // 6 drive slots (fd0..fd3, hd0, hd1) from the mount table. `nova drive list`
    // reads status["drives"].
    cb_text("drives");
    cb_arr(DRIVE_SLOTS);
    for (int i = 0; i < DRIVE_SLOTS; i++) {
        const char *p = drive_path(i);
        cb_map(4);
        cb_kv_text("slot", drive_slot_name(i));
        cb_kv_bool("mounted", p[0] != 0);
        cb_kv_text("currentPath", p);
        cb_kv_text("configuredPath", p);
    }

    cb_text("wifi");
    cb_map(3);
    cb_kv_bool("configured", 0);
    cb_kv_bool("connected", 0);
    cb_kv_text("localIp", ip);

    resp_send(pcb);
}

static void handle_list_directory(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                                  const uint8_t *payload, uint32_t plen) {
    if (!g_sd_mounted) { send_error(pcb, cmd, reqid, "sd_missing", "sd not mounted"); return; }
    char path[300], full[320];
    if (!payload_get_str(payload, plen, "path", path, sizeof path)) {
        send_error(pcb, cmd, reqid, "bad_request", "path is required"); return;
    }
    sd_path(path, full, sizeof full);
    DIR d; FILINFO fno;
    if (f_opendir(&d, full) != FR_OK) { send_error(pcb, cmd, reqid, "not_directory", "not a directory"); return; }

    // Collect in one forward pass so the CBOR array count always matches the
    // entries emitted (no reliance on f_readdir rewind).
    #define LS_MAX 512
    static struct { char name[64]; uint32_t size; uint8_t dir; } ls[LS_MAX];
    uint32_t n = 0;
    for (;;) {
        if (f_readdir(&d, &fno) != FR_OK || fno.fname[0] == 0) break;
        if (fno.fname[0] == '.') continue;
        if (n >= LS_MAX) break;
        snprintf(ls[n].name, sizeof ls[n].name, "%s", fno.fname);
        ls[n].dir  = (fno.fattrib & AM_DIR) ? 1 : 0;
        ls[n].size = ls[n].dir ? 0 : (uint32_t)fno.fsize;
        n++;
    }
    f_closedir(&d);

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_arr(n);
    for (uint32_t i = 0; i < n; i++) {
        cb_map(3);
        cb_kv_text("name", ls[i].name);
        cb_kv_uint("size", ls[i].size);
        cb_kv_bool("dir", ls[i].dir);
    }
    resp_send(pcb);
}

static void handle_read_file_chunk(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                                   const uint8_t *payload, uint32_t plen) {
    if (!g_sd_mounted) { send_error(pcb, cmd, reqid, "sd_missing", "sd not mounted"); return; }
    char path[300], full[320];
    uint32_t offset = 0, length = 0;
    if (!payload_get_str(payload, plen, "path", path, sizeof path) ||
        !payload_get_u32(payload, plen, "offset", &offset) ||
        !payload_get_u32(payload, plen, "length", &length)) {
        send_error(pcb, cmd, reqid, "bad_request", "path, offset, length required"); return;
    }
    sd_path(path, full, sizeof full);
    FIL f;
    if (f_open(&f, full, FA_READ) != FR_OK) { send_error(pcb, cmd, reqid, "not_found", "not found"); return; }
    uint32_t fsize = (uint32_t)f_size(&f);
    if (offset > fsize) { f_close(&f); send_error(pcb, cmd, reqid, "bad_offset", "offset past EOF"); return; }

    // Cap a chunk well under RESP_CAP (envelope + raw must fit the tx buffer).
    uint32_t cap = RESP_CAP - 1024;
    if (length == 0 || length > cap) length = cap;
    uint32_t rem = fsize - offset;
    uint32_t want = rem < length ? rem : length;

    static uint8_t chunk[RESP_CAP];
    UINT got = 0;
    if (want) { f_lseek(&f, offset); if (f_read(&f, chunk, want, &got) != FR_OK) { f_close(&f); send_error(pcb, cmd, reqid, "io", "read failed"); return; } }
    f_close(&f);

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(5);
    cb_kv_text("path", full);
    cb_kv_uint("offset", offset);
    cb_kv_uint("length", got);
    cb_kv_uint("size", fsize);
    cb_kv_bool("eof", offset + got >= fsize);
    uint32_t cbor_len = cw - HDR_BYTES;
    resp_raw(chunk, got);
    resp_send_raw(pcb, cbor_len);
}

static void upload_cleanup(void) {
    if (g_up_active) { f_close(&g_up); if (g_up_tmp[0]) f_unlink(g_up_tmp); }
    g_up_active = 0; g_up_written = 0; g_up_expected = 0; g_up_path[0] = 0; g_up_tmp[0] = 0;
}

static void handle_write_begin(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    upload_cleanup();
    if (!g_sd_mounted) { send_error(pcb, cmd, reqid, "sd_missing", "sd not mounted"); return; }
    char path[300];
    uint32_t size = 0;
    if (!payload_get_str(payload, plen, "path", path, sizeof path) ||
        !payload_get_u32(payload, plen, "size", &size)) {
        send_error(pcb, cmd, reqid, "bad_request", "path and size required"); return;
    }
    sd_path(path, g_up_path, sizeof g_up_path);
    if (!ensure_parent_dirs(g_up_path)) { send_error(pcb, cmd, reqid, "mkdir_failed", "mkdir failed"); return; }
    if (snprintf(g_up_tmp, sizeof g_up_tmp, "%s.tmp", g_up_path) >= (int)sizeof g_up_tmp) {
        send_error(pcb, cmd, reqid, "path_too_long", "path too long"); return;
    }
    if (f_open(&g_up, g_up_tmp, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        g_up_tmp[0] = 0; send_error(pcb, cmd, reqid, "open_failed", "open failed"); return;
    }
    g_up_active = 1; g_up_expected = size; g_up_written = 0;

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(2);
    cb_kv_text("path", g_up_path);
    cb_kv_uint("size", size);
    resp_send(pcb);
}

static void handle_write_chunk(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen,
                               const uint8_t *raw, uint32_t rlen) {
    uint32_t offset = 0;
    if (!g_up_active || !payload_get_u32(payload, plen, "offset", &offset)) {
        send_error(pcb, cmd, reqid, "no_upload", "no upload active"); return;
    }
    if (offset != g_up_written) { send_error(pcb, cmd, reqid, "bad_offset", "bad upload offset"); return; }
    UINT bw = 0;
    if (f_write(&g_up, raw, rlen, &bw) != FR_OK || bw != rlen) {
        upload_cleanup(); send_error(pcb, cmd, reqid, "write_failed", "write failed"); return;
    }
    g_up_written += bw;

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(2);
    cb_kv_uint("written", g_up_written);
    cb_kv_uint("size", g_up_expected);
    resp_send(pcb);
}

static void handle_write_commit(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid) {
    if (!g_up_active) { send_error(pcb, cmd, reqid, "no_upload", "no upload active"); return; }
    f_close(&g_up);
    if (g_up_written != g_up_expected) {
        f_unlink(g_up_tmp); g_up_active = 0;
        send_error(pcb, cmd, reqid, "size_mismatch", "size mismatch"); return;
    }
    f_unlink(g_up_path);                 // remove existing (rename won't overwrite)
    if (f_rename(g_up_tmp, g_up_path) != FR_OK) {
        f_unlink(g_up_tmp); g_up_active = 0;
        send_error(pcb, cmd, reqid, "replace_rename_failed", "rename failed"); return;
    }
    xil_printf("[mgmt] WRITE %s (%u bytes)\r\n", g_up_path, (unsigned)g_up_expected);
    char done_path[300]; snprintf(done_path, sizeof done_path, "%s", g_up_path);
    uint32_t done_size = g_up_expected;
    g_up_active = 0; g_up_path[0] = 0; g_up_tmp[0] = 0;

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(2);
    cb_kv_text("path", done_path);
    cb_kv_uint("size", done_size);
    resp_send(pcb);
}

static void handle_write_abort(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid) {
    upload_cleanup();
    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(1);
    cb_kv_bool("aborted", 1);
    resp_send(pcb);
}

static void handle_delete_path(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    if (!g_sd_mounted) { send_error(pcb, cmd, reqid, "sd_missing", "sd not mounted"); return; }
    char path[300], full[320];
    if (!payload_get_str(payload, plen, "path", path, sizeof path)) {
        send_error(pcb, cmd, reqid, "bad_request", "path is required"); return;
    }
    sd_path(path, full, sizeof full);
    FRESULT r = f_unlink(full);
    if (r != FR_OK) { send_error(pcb, cmd, reqid, "not_found", "not found"); return; }
    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(2);
    cb_kv_text("path", full);
    cb_kv_bool("deleted", 1);
    resp_send(pcb);
}

static int ends_with_ndi(const char *p) {
    size_t n = strlen(p);
    return n >= 4 && strcasecmp(p + n - 4, ".ndi") == 0;
}

static void handle_mount_drive(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    if (!g_sd_mounted) { send_error(pcb, cmd, reqid, "sd_missing", "sd not mounted"); return; }
    char slot[8], path[160], full[200];
    if (!payload_get_str(payload, plen, "slot", slot, sizeof slot)) {
        send_error(pcb, cmd, reqid, "bad_request", "slot is required"); return;
    }
    int idx = drive_slot_index(slot);
    if (idx < 0) { send_error(pcb, cmd, reqid, "bad_slot", "bad slot"); return; }

    int have_path = payload_get_str(payload, plen, "path", path, sizeof path) && path[0];
    if (!have_path) {                                  // remount the configured image
        const char *cur = drive_path(idx);
        if (!cur[0]) { send_error(pcb, cmd, reqid, "missing_path", "missing path"); return; }
        snprintf(path, sizeof path, "%s", cur);
    }
    if (!ends_with_ndi(path)) { send_error(pcb, cmd, reqid, "bad_path", "path must end in .ndi"); return; }

    sd_path(path, full, sizeof full);                  // "/x" -> "0:/x"
    FIL f;
    if (f_open(&f, full, FA_READ) != FR_OK) { send_error(pcb, cmd, reqid, "not_found", "not found"); return; }
    f_close(&f);

    drive_set(idx, path);
    const char *boot = drive_slot_name(drive_boot_slot());
    xil_printf("[mgmt] mount %s = %s (boot=%s)\r\n", drive_slot_name(idx), path, boot ? boot : "-");

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(4);
    cb_kv_text("slot", drive_slot_name(idx));
    cb_kv_bool("mounted", 1);
    cb_kv_text("path", path);
    cb_kv_text("bootDefault", boot ? boot : "");
    resp_send(pcb);
}

static void handle_unmount_drive(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid,
                                 const uint8_t *payload, uint32_t plen) {
    char slot[8];
    if (!payload_get_str(payload, plen, "slot", slot, sizeof slot)) {
        send_error(pcb, cmd, reqid, "bad_request", "slot is required"); return;
    }
    int idx = drive_slot_index(slot);
    if (idx < 0) { send_error(pcb, cmd, reqid, "bad_slot", "bad slot"); return; }
    drive_set(idx, "");
    const char *boot = drive_slot_name(drive_boot_slot());
    xil_printf("[mgmt] unmount %s (boot=%s)\r\n", drive_slot_name(idx), boot ? boot : "-");

    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(3);
    cb_kv_text("slot", drive_slot_name(idx));
    cb_kv_bool("mounted", 0);
    cb_kv_text("bootDefault", boot ? boot : "");
    resp_send(pcb);
}

static void handle_vm_reset(struct tcp_pcb *pcb, uint16_t cmd, uint32_t reqid) {
    vm_reset();
    xil_printf("[mgmt] VM reset\r\n");
    resp_begin(cmd, reqid, 1, "ok", NULL);
    cb_map(1);
    cb_kv_bool("reset", 1);
    resp_send(pcb);
}

// Dispatch one fully-received frame.
static void dispatch(struct tcp_pcb *pcb, const uint8_t *frame, uint32_t clen, uint32_t rlen) {
    uint16_t command = (uint16_t)((frame[6] << 8) | frame[7]);
    uint32_t reqid   = ((uint32_t)frame[10] << 24) | ((uint32_t)frame[11] << 16) |
                       ((uint32_t)frame[12] << 8)  | frame[13];
    const uint8_t *payload = frame + HDR_BYTES;
    const uint8_t *raw     = frame + HDR_BYTES + clen;
    switch (command) {
        case 1:  handle_hello(pcb, reqid); break;
        case 2:  handle_get_status(pcb, reqid); break;
        case 3:  handle_list_directory(pcb, command, reqid, payload, clen); break;
        case 6:  handle_write_chunk(pcb, command, reqid, payload, clen, raw, rlen); break;
        case 5:  handle_write_begin(pcb, command, reqid, payload, clen); break;
        case 7:  handle_write_commit(pcb, command, reqid); break;
        case 8:  handle_write_abort(pcb, command, reqid); break;
        case 9:  handle_delete_path(pcb, command, reqid, payload, clen); break;
        case 10: handle_mount_drive(pcb, command, reqid, payload, clen); break;
        case 11: handle_unmount_drive(pcb, command, reqid, payload, clen); break;
        case 14: handle_vm_reset(pcb, command, reqid); break;
        case 19: handle_read_file_chunk(pcb, command, reqid, payload, clen); break;
        default: send_error(pcb, command, reqid, "not_implemented",
                            "command not implemented yet"); break;
    }
}

// ---- lwIP callbacks ---------------------------------------------------------
static err_t mgmt_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    (void)arg; (void)len;
    if (g_resp_off < g_resp_len) mgmt_flush(pcb);
    return ERR_OK;
}

static err_t mgmt_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    (void)arg; (void)err;
    if (p == NULL) {                     // remote closed
        if (pcb == g_pcb) { g_pcb = NULL; g_rxlen = 0; }
        tcp_close(pcb);
        return ERR_OK;
    }
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        if (g_rxlen + q->len <= RX_CAP) { memcpy(g_rx + g_rxlen, q->payload, q->len); g_rxlen += q->len; }
        else g_rxlen = 0;                // overflow: drop accumulated (resync)
    }
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);

    // Parse as many complete frames as we have.
    for (;;) {
        if (g_rxlen < HDR_BYTES) break;
        if (memcmp(g_rx, "NVH1", 4) != 0) { g_rxlen = 0; break; }   // resync
        uint32_t clen = ((uint32_t)g_rx[14] << 24) | ((uint32_t)g_rx[15] << 16) |
                        ((uint32_t)g_rx[16] << 8)  | g_rx[17];
        uint32_t rlen = ((uint32_t)g_rx[18] << 24) | ((uint32_t)g_rx[19] << 16) |
                        ((uint32_t)g_rx[20] << 8)  | g_rx[21];
        uint32_t total = HDR_BYTES + clen + rlen;
        if (total > RX_CAP) { g_rxlen = 0; break; }                 // too big: drop
        if (g_rxlen < total) break;                                 // wait for more
        dispatch(pcb, g_rx, clen, rlen);
        uint32_t leftover = g_rxlen - total;
        if (leftover) memmove(g_rx, g_rx + total, leftover);
        g_rxlen = leftover;
    }
    return ERR_OK;
}

static err_t mgmt_accept(void *arg, struct tcp_pcb *pcb, err_t err) {
    (void)arg; (void)err;
    if (g_pcb && g_pcb != pcb) { tcp_close(g_pcb); }   // one client at a time
    g_pcb = pcb;
    g_rxlen = 0;
    tcp_recv(pcb, mgmt_recv);
    tcp_sent(pcb, mgmt_sent);
    xil_printf("[mgmt] client connected (6504)\r\n");
    return ERR_OK;
}

void mgmt_init(void) {
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb && tcp_bind(pcb, IP_ANY_TYPE, MGMT_PORT) == ERR_OK) {
        pcb = tcp_listen(pcb);
        tcp_accept(pcb, mgmt_accept);
        xil_printf("[mgmt] NovaHost management server up on TCP %d\r\n", MGMT_PORT);
    } else {
        xil_printf("[mgmt] ERROR: cannot bind TCP %d\r\n", MGMT_PORT);
    }
}
