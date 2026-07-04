/* nservers.c — NovaHost network servers (Linux/BSD-sockets userspace port).
 *
 * Faithful port of the bare-metal ps_fio lwIP servers to the NovaVM Linux host:
 *
 *   port 6502  upload   raw byte stream -> /data/nova/UPLOAD.NDI   (was net.c)
 *   port 6503  debug    newline-delimited JSON                     (was debug.c)
 *   port 6504  mgmt     NVH1 length-framed CBOR                    (was mgmt.c)
 *
 * The wire protocols are preserved byte-for-byte so the C# clients in
 * e6502.NovaHost (NovaDebugClient / NovaHostManagementClient) talk to this host
 * unchanged:
 *
 *   mgmt frame header (24 bytes, big-endian):
 *     "NVH1"(4) | version u16 | command u16 | flags u16 | requestId u32 |
 *     cborLen u32 | rawLen u32 | (2 bytes pad)
 *   then a CBOR map payload (cborLen) + raw bytes (rawLen). Response envelope is
 *   a CBOR map { ok, error, code, data }.
 *
 *   debug : one JSON object per line in, one JSON object + '\n' out.
 *
 * lwIP's callback model (tcp_recv/tcp_sent/tcp_accept) is replaced by one
 * pthread per port running a blocking accept() loop. One client at a time,
 * matching the bare-metal (the clients serialise request/response). Each
 * connection is drained on the same thread until the peer closes.
 *
 * The servers act on the machine via novavm.h's poke()/peek()/wr()/rd():
 *   - screen read   -> VGC vmem debug path (R_VMEM_ADDR 0x28 / R_VMEM_DATA 0x2C),
 *                      mirroring novavm.c's dump_screen()/vmem_read().
 *   - poke/peek     -> 6502 RAM
 *   - key inject    -> R_KEY
 *   - vm reset      -> R_CTRL (pulse CTRL_CPU_RESET)
 *   - cold start    -> novavm.c host_reboot_vm() (reload ROM/loader, autoboot)
 *   - file mgmt     -> POSIX under /data/nova (was xilffs f_open/f_read).
 *
 * Cross-compile check (must be clean):
 *   arm-linux-gnueabihf-gcc -O2 -pthread -c -I. nservers.c
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "novavm.h"
#include "nbootcfg.h"
#include "nservers.h"

/* ---- ports + caps (mirror the bare-metal) --------------------------------- */
#define MGMT_PORT     6504
#define DBG_PORT      6503
#define UPLOAD_PORT   6502
#define MGMT_VERSION  1
#define HDR_BYTES     24

#define RX_CAP        40000u    /* one inbound mgmt frame                       */
#define RESP_CAP      16384u    /* one outbound mgmt frame                      */
#define DBG_RXCAP     8192u
#define DBG_TXCAP     8192u

#define SCREEN_COLS   80
#define SCREEN_ROWS   50
#define REG_CURSOR_X      0xA003
#define REG_CURSOR_Y      0xA004
#define REG_CURSOR_ENABLE 0xA005

#define UPLOAD_PATH   NOVA_FS_ROOT "/UPLOAD.NDI"
#define MOUNTS_FILE   NOVA_FS_ROOT "/config/mounts.txt"

/* The mgmt/debug servers each serialise their own client, but they run on
 * separate threads and both poke/peek the shared bridge. Serialise all bridge
 * access so a screen dump and a peek can't interleave their R_VMEM_ADDR /
 * R_PEEK_ADDR latches. (Coarse but correct; matches single-threaded bare-metal
 * semantics from the machine's point of view.) */
static pthread_mutex_t g_bridge = PTHREAD_MUTEX_INITIALIZER;
#define BRIDGE_LOCK()   pthread_mutex_lock(&g_bridge)
#define BRIDGE_UNLOCK() pthread_mutex_unlock(&g_bridge)

/* ---- machine access via novavm.h primitives ------------------------------- */

/* VGC video-memory debug read (same sequence as novavm.c's vmem_read): latch
 * {space[2:0],addr[16:0]} then read the data port twice (first read is the
 * pipeline fill). */
static unsigned char vmem_read(unsigned sp, unsigned a) {
    wr(R_VMEM_ADDR, ((sp & 7u) << 17) | (a & 0x1FFFFu));
    (void)rd(R_VMEM_DATA);
    return (unsigned char)rd(R_VMEM_DATA);
}

/* VM reset over R_CTRL. cold==1 also pulses the custom-chip (system) reset, the
 * same combination novavm.c uses at boot. The 6502 re-runs its cold start. */
static void vm_reset_ctrl_locked(int cold) {
    if (cold) { wr(R_CTRL, CTRL_CPU_RESET | CTRL_SYS_RESET); usleep(150000); }
    else      { wr(R_CTRL, CTRL_CPU_RESET); usleep(2000); }
    wr(R_CTRL, 0u);
}

static void vm_reset_ctrl(int cold) {
    BRIDGE_LOCK();
    vm_reset_ctrl_locked(cold);
    BRIDGE_UNLOCK();
}

/* ---- socket helpers ------------------------------------------------------- */

/* Blocking write of all n bytes. Returns 1 on success, 0 on error. */
static int write_all(int fd, const void *buf, size_t n) {
    const uint8_t *p = (const uint8_t *)buf;
    size_t off = 0;
    while (off < n) {
        ssize_t w = send(fd, p + off, n - off, MSG_NOSIGNAL);
        if (w < 0) { if (errno == EINTR) continue; return 0; }
        off += (size_t)w;
    }
    return 1;
}

/* Bind+listen a TCP server socket on `port`, SO_REUSEADDR, INADDR_ANY. */
static int listen_on(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons((uint16_t)port);
    if (bind(fd, (struct sockaddr *)&sa, sizeof sa) != 0) { close(fd); return -1; }
    if (listen(fd, 1) != 0) { close(fd); return -1; }
    return fd;
}

/* First non-loopback IPv4 as text ("0.0.0.0" if none). Used for GetStatus. */
static void host_ip(char *out, int n) {
    snprintf(out, n, "0.0.0.0");
    struct ifaddrs *ifs = NULL;
    if (getifaddrs(&ifs) != 0) return;
    for (struct ifaddrs *ia = ifs; ia; ia = ia->ifa_next) {
        if (!ia->ifa_addr || ia->ifa_addr->sa_family != AF_INET) continue;
        struct sockaddr_in *s = (struct sockaddr_in *)ia->ifa_addr;
        if (s->sin_addr.s_addr == htonl(INADDR_LOOPBACK)) continue;
        inet_ntop(AF_INET, &s->sin_addr, out, n);
        break;
    }
    freeifaddrs(ifs);
}

/* ===========================================================================
 *  PATH HELPERS  (client "/foo" -> /data/nova/foo)
 * =========================================================================== */

/* Map a client path ("/foo/bar" or "foo/bar") to a host path under NOVA_FS_ROOT.
 * Replaces mgmt.c's sd_path() (FatFs "0:/..."). */
static void sd_path(const char *p, char *out, int cap) {
    if (!p || !p[0])        snprintf(out, cap, "%s", NOVA_FS_ROOT);
    else if (p[0] == '/')   snprintf(out, cap, "%s%s", NOVA_FS_ROOT, p);
    else                    snprintf(out, cap, "%s/%s", NOVA_FS_ROOT, p);
}

/* mkdir each parent segment of a "/data/nova/a/b/c" path (ignores EEXIST).
 * Replaces mgmt.c's ensure_parent_dirs(). */
static int ensure_parent_dirs(const char *full) {
    char tmp[512];
    snprintf(tmp, sizeof tmp, "%s", full);
    for (char *q = tmp + 1; *q; q++) {
        if (*q == '/') {
            *q = 0;
            if (mkdir(tmp, 0775) != 0 && errno != EEXIST) { *q = '/'; return 0; }
            *q = '/';
        }
    }
    return 1;
}

static int ends_with_ndi(const char *p) {
    size_t n = strlen(p);
    return n >= 4 && strcasecmp(p + n - 4, ".ndi") == 0;
}

/* ===========================================================================
 *  DRIVE MOUNT TABLE  (port of drives.c -> /data/nova/config/mounts.txt)
 * =========================================================================== */
#define DRIVE_SLOTS 6
static const char *SLOT_NAMES[DRIVE_SLOTS] = { "fd0", "fd1", "fd2", "fd3", "hd0", "hd1" };
static char g_drive_path[DRIVE_SLOTS][160];
static int  g_drives_loaded = 0;

static int drive_slot_index(const char *name) {
    if (!name) return -1;
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (strcasecmp(name, SLOT_NAMES[i]) == 0) return i;
    return -1;
}
static const char *drive_slot_name(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? SLOT_NAMES[idx] : 0;
}
static const char *drive_path(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? g_drive_path[idx] : "";
}
static int drive_boot_slot(void) {
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (g_drive_path[i][0]) return i;
    return -1;
}
static void drives_load(void) {
    /* mounts live in /config/boot.json (the ULX3S schema), read via nbootcfg —
     * the same persist/load the ESP32 management_server uses. */
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (!bootcfg_mount_get(SLOT_NAMES[i], g_drive_path[i], sizeof g_drive_path[i]))
            g_drive_path[i][0] = 0;
    g_drives_loaded = 1;
}
static void drive_set(int idx, const char *path) {
    if (idx < 0 || idx >= DRIVE_SLOTS) return;
    snprintf(g_drive_path[idx], sizeof g_drive_path[idx], "%s", path ? path : "");
    bootcfg_mount_set(SLOT_NAMES[idx], path ? path : "");   /* persist into boot.json.mounts */
}

/* ===========================================================================
 *  MGMT SERVER (6504) — NVH1 + CBOR
 * =========================================================================== */

/* Per-connection state for the mgmt server (one client at a time). */
typedef struct {
    int      fd;
    uint8_t  resp[RESP_CAP];
    uint32_t cw;               /* CBOR write cursor (index into resp)          */
    /* upload staging (WriteFileBegin/Chunk/Commit/Abort) */
    FILE    *up;
    int      up_active;
    uint32_t up_written, up_expected;
    char     up_path[300], up_tmp[320];
} mgmt_ctx;

/* ---- CBOR encoder (writes into ctx->resp past the 24-byte header) ---------- */
static void cb_raw(mgmt_ctx *c, uint8_t b) { if (c->cw < RESP_CAP) c->resp[c->cw++] = b; }

static void cb_type(mgmt_ctx *c, uint8_t major, unsigned long long v) {
    uint8_t pfx = (uint8_t)(major << 5);
    if (v < 24)               { cb_raw(c, pfx | (uint8_t)v); }
    else if (v <= 0xFF)       { cb_raw(c, pfx | 24); cb_raw(c, (uint8_t)v); }
    else if (v <= 0xFFFF)     { cb_raw(c, pfx | 25); cb_raw(c, (uint8_t)(v >> 8)); cb_raw(c, (uint8_t)v); }
    else if (v <= 0xFFFFFFFFULL) {
        cb_raw(c, pfx | 26);
        cb_raw(c, (uint8_t)(v >> 24)); cb_raw(c, (uint8_t)(v >> 16));
        cb_raw(c, (uint8_t)(v >> 8));  cb_raw(c, (uint8_t)v);
    } else {
        cb_raw(c, pfx | 27);
        for (int s = 56; s >= 0; s -= 8) cb_raw(c, (uint8_t)(v >> s));
    }
}
static void cb_uint(mgmt_ctx *c, unsigned long long v) { cb_type(c, 0, v); }
static void cb_bool(mgmt_ctx *c, int v) { cb_raw(c, v ? 0xF5 : 0xF4); }
static void cb_null(mgmt_ctx *c)        { cb_raw(c, 0xF6); }
static void cb_map(mgmt_ctx *c, uint32_t n) { cb_type(c, 5, n); }
static void cb_arr(mgmt_ctx *c, uint32_t n) { cb_type(c, 4, n); }
static void cb_text(mgmt_ctx *c, const char *s) {
    if (!s) s = "";
    size_t n = strlen(s);
    cb_type(c, 3, n);
    for (size_t i = 0; i < n; i++) cb_raw(c, (uint8_t)s[i]);
}
static void cb_kv_text(mgmt_ctx *c, const char *k, const char *v) { cb_text(c, k); cb_text(c, v); }
static void cb_kv_bool(mgmt_ctx *c, const char *k, int v)          { cb_text(c, k); cb_bool(c, v); }
static void cb_kv_uint(mgmt_ctx *c, const char *k, unsigned long long v) { cb_text(c, k); cb_uint(c, v); }

/* ---- frame build + send --------------------------------------------------- */
static void put_be16(uint8_t *b, int off, uint16_t v) { b[off] = (uint8_t)(v >> 8); b[off + 1] = (uint8_t)v; }
static void put_be32(uint8_t *b, int off, uint32_t v) {
    b[off]   = (uint8_t)(v >> 24); b[off + 1] = (uint8_t)(v >> 16);
    b[off + 2] = (uint8_t)(v >> 8); b[off + 3] = (uint8_t)v;
}

/* Begin an envelope frame: header placeholder + CBOR map(4){ok,error,code,data:}.
 * After this the caller appends the `data` value, then resp_send(). */
static void resp_begin(mgmt_ctx *c, uint16_t command, uint32_t reqid, int ok,
                       const char *code, const char *error) {
    memcpy(c->resp, "NVH1", 4);
    put_be16(c->resp, 4, MGMT_VERSION);
    put_be16(c->resp, 6, command);
    put_be16(c->resp, 8, 0);                  /* flags                         */
    put_be32(c->resp, 10, reqid);
    put_be32(c->resp, 14, 0);                 /* cborLen (patched in send)     */
    put_be32(c->resp, 18, 0);                 /* rawLen                        */
    c->resp[22] = c->resp[23] = 0;            /* pad                           */
    c->cw = HDR_BYTES;

    cb_map(c, 4);
    cb_kv_bool(c, "ok", ok);
    cb_text(c, "error"); if (error) cb_text(c, error); else cb_null(c);
    cb_kv_text(c, "code", code ? code : "");
    cb_text(c, "data");                       /* value appended by caller next */
}

static void resp_send(mgmt_ctx *c) {
    put_be32(c->resp, 14, c->cw - HDR_BYTES); /* cborLen = bytes after header  */
    put_be32(c->resp, 18, 0);                 /* rawLen                        */
    write_all(c->fd, c->resp, c->cw);
}

/* Append raw (non-CBOR) bytes after the CBOR envelope. */
static void resp_raw(mgmt_ctx *c, const uint8_t *d, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) if (c->cw < RESP_CAP) c->resp[c->cw++] = d[i];
}

/* Finish a frame that carries raw bytes. cbor_len was captured (cw - HDR_BYTES)
 * right after the envelope, before resp_raw() was called. */
static void resp_send_raw(mgmt_ctx *c, uint32_t cbor_len) {
    put_be32(c->resp, 14, cbor_len);
    put_be32(c->resp, 18, (c->cw - HDR_BYTES) - cbor_len);
    write_all(c->fd, c->resp, c->cw);
}

static void send_error(mgmt_ctx *c, uint16_t command, uint32_t reqid,
                       const char *code, const char *msg) {
    resp_begin(c, command, reqid, 0, code, msg);
    cb_null(c);                               /* data                          */
    resp_send(c);
}

/* ---- CBOR reader (pull keys from a request payload map) -------------------- */
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
        if (match) return 1;
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

/* ---- mgmt handlers -------------------------------------------------------- */
static void handle_hello(mgmt_ctx *c, uint32_t reqid) {
    resp_begin(c, 1, reqid, 1, "ok", NULL);
    cb_map(c, 6);
    cb_kv_text(c, "protocol", "NVH1");
    cb_kv_uint(c, "version", MGMT_VERSION);
    cb_kv_text(c, "firmware", __DATE__ " " __TIME__);
    cb_kv_uint(c, "maxPayload", 32768);
    cb_kv_uint(c, "maxRaw", 16384);
    cb_text(c, "capabilities");
    cb_map(c, 9);
    cb_kv_bool(c, "status", 1);
    cb_kv_bool(c, "listing", 1);
    cb_kv_bool(c, "upload", 1);
    cb_kv_bool(c, "drives", 1);
    cb_kv_bool(c, "runtime", 0);
    cb_kv_bool(c, "vmReset", 1);
    cb_kv_bool(c, "hostReboot", 0);
    cb_kv_bool(c, "wifi", 0);
    cb_kv_bool(c, "downloadChunks", 1);
    resp_send(c);
}

static void handle_get_status(mgmt_ctx *c, uint32_t reqid) {
    char ip[64]; host_ip(ip, sizeof ip);

    resp_begin(c, 2, reqid, 1, "ok", NULL);
    cb_map(c, 5);

    cb_text(c, "health");
    cb_map(c, 4);
    cb_kv_bool(c, "ok", 1);
    cb_kv_text(c, "bootPhase", "running");
    cb_kv_bool(c, "sdMounted", 1);
    cb_kv_bool(c, "fpgaBridgeAvailable", 1);

    cb_text(c, "sdStatus");
    cb_map(c, 4);
    cb_kv_text(c, "cardType", "EMMC");
    cb_kv_bool(c, "mounted", 1);
    cb_kv_uint(c, "totalBytes", 0);
    cb_kv_uint(c, "usedBytes", 0);

    cb_text(c, "network");
    cb_map(c, 2);
    cb_kv_text(c, "ip", ip);
    cb_kv_text(c, "transport", "ethernet");

    cb_text(c, "drives");
    cb_arr(c, DRIVE_SLOTS);
    for (int i = 0; i < DRIVE_SLOTS; i++) {
        const char *p = drive_path(i);
        cb_map(c, 4);
        cb_kv_text(c, "slot", drive_slot_name(i));
        cb_kv_bool(c, "mounted", p[0] != 0);
        cb_kv_text(c, "currentPath", p);
        cb_kv_text(c, "configuredPath", p);
    }

    cb_text(c, "wifi");
    cb_map(c, 3);
    cb_kv_bool(c, "configured", 0);
    cb_kv_bool(c, "connected", 0);
    cb_kv_text(c, "localIp", ip);

    resp_send(c);
}

static void handle_list_directory(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                                  const uint8_t *payload, uint32_t plen) {
    char path[300], full[320];
    if (!payload_get_str(payload, plen, "path", path, sizeof path)) {
        send_error(c, cmd, reqid, "bad_request", "path is required"); return;
    }
    sd_path(path, full, sizeof full);
    DIR *d = opendir(full);
    if (!d) { send_error(c, cmd, reqid, "not_directory", "not a directory"); return; }

    #define LS_MAX 512
    static struct { char name[64]; uint32_t size; uint8_t dir; } ls[LS_MAX];
    uint32_t n = 0;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.') continue;
        if (n >= LS_MAX) break;
        snprintf(ls[n].name, sizeof ls[n].name, "%.63s", de->d_name);
        char child[700];
        snprintf(child, sizeof child, "%.320s/%.255s", full, de->d_name);
        struct stat st;
        if (stat(child, &st) == 0) {
            ls[n].dir  = S_ISDIR(st.st_mode) ? 1 : 0;
            ls[n].size = ls[n].dir ? 0 : (uint32_t)st.st_size;
        } else {
            ls[n].dir = 0; ls[n].size = 0;
        }
        n++;
    }
    closedir(d);

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_arr(c, n);
    for (uint32_t i = 0; i < n; i++) {
        cb_map(c, 3);
        cb_kv_text(c, "name", ls[i].name);
        cb_kv_uint(c, "size", ls[i].size);
        cb_kv_bool(c, "dir", ls[i].dir);
    }
    resp_send(c);
}

static void handle_read_file_chunk(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                                   const uint8_t *payload, uint32_t plen) {
    char path[300], full[320];
    uint32_t offset = 0, length = 0;
    if (!payload_get_str(payload, plen, "path", path, sizeof path) ||
        !payload_get_u32(payload, plen, "offset", &offset) ||
        !payload_get_u32(payload, plen, "length", &length)) {
        send_error(c, cmd, reqid, "bad_request", "path, offset, length required"); return;
    }
    sd_path(path, full, sizeof full);
    FILE *f = fopen(full, "rb");
    if (!f) { send_error(c, cmd, reqid, "not_found", "not found"); return; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) sz = 0;
    uint32_t fsize = (uint32_t)sz;
    if (offset > fsize) { fclose(f); send_error(c, cmd, reqid, "bad_offset", "offset past EOF"); return; }

    uint32_t cap = RESP_CAP - 1024;
    if (length == 0 || length > cap) length = cap;
    uint32_t rem = fsize - offset;
    uint32_t want = rem < length ? rem : length;

    static uint8_t chunk[RESP_CAP];
    uint32_t got = 0;
    if (want) {
        fseek(f, (long)offset, SEEK_SET);
        size_t r = fread(chunk, 1, want, f);
        got = (uint32_t)r;
    }
    fclose(f);

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 5);
    cb_kv_text(c, "path", full);
    cb_kv_uint(c, "offset", offset);
    cb_kv_uint(c, "length", got);
    cb_kv_uint(c, "size", fsize);
    cb_kv_bool(c, "eof", offset + got >= fsize);
    uint32_t cbor_len = c->cw - HDR_BYTES;
    resp_raw(c, chunk, got);
    resp_send_raw(c, cbor_len);
}

static void upload_cleanup(mgmt_ctx *c) {
    if (c->up_active) { if (c->up) fclose(c->up); if (c->up_tmp[0]) unlink(c->up_tmp); }
    c->up = NULL; c->up_active = 0; c->up_written = 0; c->up_expected = 0;
    c->up_path[0] = 0; c->up_tmp[0] = 0;
}

static void handle_write_begin(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    upload_cleanup(c);
    char path[300];
    uint32_t size = 0;
    if (!payload_get_str(payload, plen, "path", path, sizeof path) ||
        !payload_get_u32(payload, plen, "size", &size)) {
        send_error(c, cmd, reqid, "bad_request", "path and size required"); return;
    }
    sd_path(path, c->up_path, sizeof c->up_path);
    if (!ensure_parent_dirs(c->up_path)) { send_error(c, cmd, reqid, "mkdir_failed", "mkdir failed"); return; }
    if (snprintf(c->up_tmp, sizeof c->up_tmp, "%s.tmp", c->up_path) >= (int)sizeof c->up_tmp) {
        send_error(c, cmd, reqid, "path_too_long", "path too long"); return;
    }
    c->up = fopen(c->up_tmp, "wb");
    if (!c->up) { c->up_tmp[0] = 0; send_error(c, cmd, reqid, "open_failed", "open failed"); return; }
    c->up_active = 1; c->up_expected = size; c->up_written = 0;

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 2);
    cb_kv_text(c, "path", c->up_path);
    cb_kv_uint(c, "size", size);
    resp_send(c);
}

static void handle_write_chunk(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen,
                               const uint8_t *raw, uint32_t rlen) {
    uint32_t offset = 0;
    if (!c->up_active || !payload_get_u32(payload, plen, "offset", &offset)) {
        send_error(c, cmd, reqid, "no_upload", "no upload active"); return;
    }
    if (offset != c->up_written) { send_error(c, cmd, reqid, "bad_offset", "bad upload offset"); return; }
    size_t bw = fwrite(raw, 1, rlen, c->up);
    if (bw != rlen) { upload_cleanup(c); send_error(c, cmd, reqid, "write_failed", "write failed"); return; }
    c->up_written += (uint32_t)bw;

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 2);
    cb_kv_uint(c, "written", c->up_written);
    cb_kv_uint(c, "size", c->up_expected);
    resp_send(c);
}

static void handle_write_commit(mgmt_ctx *c, uint16_t cmd, uint32_t reqid) {
    if (!c->up_active) { send_error(c, cmd, reqid, "no_upload", "no upload active"); return; }
    if (c->up) { fclose(c->up); c->up = NULL; }
    if (c->up_written != c->up_expected) {
        unlink(c->up_tmp); c->up_active = 0;
        send_error(c, cmd, reqid, "size_mismatch", "size mismatch"); return;
    }
    unlink(c->up_path);                       /* remove existing (rename clobber) */
    if (rename(c->up_tmp, c->up_path) != 0) {
        unlink(c->up_tmp); c->up_active = 0;
        send_error(c, cmd, reqid, "replace_rename_failed", "rename failed"); return;
    }
    printf("[mgmt] WRITE %s (%u bytes)\n", c->up_path, (unsigned)c->up_expected);
    char done_path[300]; snprintf(done_path, sizeof done_path, "%s", c->up_path);
    uint32_t done_size = c->up_expected;
    c->up_active = 0; c->up_path[0] = 0; c->up_tmp[0] = 0;

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 2);
    cb_kv_text(c, "path", done_path);
    cb_kv_uint(c, "size", done_size);
    resp_send(c);
}

static void handle_write_abort(mgmt_ctx *c, uint16_t cmd, uint32_t reqid) {
    upload_cleanup(c);
    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 1);
    cb_kv_bool(c, "aborted", 1);
    resp_send(c);
}

static void handle_delete_path(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    char path[300], full[320];
    if (!payload_get_str(payload, plen, "path", path, sizeof path)) {
        send_error(c, cmd, reqid, "bad_request", "path is required"); return;
    }
    sd_path(path, full, sizeof full);
    if (unlink(full) != 0) { send_error(c, cmd, reqid, "not_found", "not found"); return; }
    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 2);
    cb_kv_text(c, "path", full);
    cb_kv_bool(c, "deleted", 1);
    resp_send(c);
}

static void handle_mount_drive(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                               const uint8_t *payload, uint32_t plen) {
    char slot[8], path[160], full[200];
    if (!payload_get_str(payload, plen, "slot", slot, sizeof slot)) {
        send_error(c, cmd, reqid, "bad_request", "slot is required"); return;
    }
    int idx = drive_slot_index(slot);
    if (idx < 0) { send_error(c, cmd, reqid, "bad_slot", "bad slot"); return; }

    int have_path = payload_get_str(payload, plen, "path", path, sizeof path) && path[0];
    if (!have_path) {
        const char *cur = drive_path(idx);
        if (!cur[0]) { send_error(c, cmd, reqid, "missing_path", "missing path"); return; }
        snprintf(path, sizeof path, "%s", cur);
    }
    if (!ends_with_ndi(path)) { send_error(c, cmd, reqid, "bad_path", "path must end in .ndi"); return; }

    sd_path(path, full, sizeof full);
    if (access(full, R_OK) != 0) { send_error(c, cmd, reqid, "not_found", "not found"); return; }

    drive_set(idx, path);
    const char *boot = drive_slot_name(drive_boot_slot());
    printf("[mgmt] mount %s = %s (boot=%s)\n", drive_slot_name(idx), path, boot ? boot : "-");

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 4);
    cb_kv_text(c, "slot", drive_slot_name(idx));
    cb_kv_bool(c, "mounted", 1);
    cb_kv_text(c, "path", path);
    cb_kv_text(c, "bootDefault", boot ? boot : "");
    resp_send(c);
}

static void handle_unmount_drive(mgmt_ctx *c, uint16_t cmd, uint32_t reqid,
                                 const uint8_t *payload, uint32_t plen) {
    char slot[8];
    if (!payload_get_str(payload, plen, "slot", slot, sizeof slot)) {
        send_error(c, cmd, reqid, "bad_request", "slot is required"); return;
    }
    int idx = drive_slot_index(slot);
    if (idx < 0) { send_error(c, cmd, reqid, "bad_slot", "bad slot"); return; }
    drive_set(idx, "");
    const char *boot = drive_slot_name(drive_boot_slot());
    printf("[mgmt] unmount %s (boot=%s)\n", drive_slot_name(idx), boot ? boot : "-");

    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 3);
    cb_kv_text(c, "slot", drive_slot_name(idx));
    cb_kv_bool(c, "mounted", 0);
    cb_kv_text(c, "bootDefault", boot ? boot : "");
    resp_send(c);
}

static void handle_vm_reset(mgmt_ctx *c, uint16_t cmd, uint32_t reqid) {
    vm_reset_ctrl(0);
    printf("[mgmt] VM reset\n");
    resp_begin(c, cmd, reqid, 1, "ok", NULL);
    cb_map(c, 1);
    cb_kv_bool(c, "reset", 1);
    resp_send(c);
}

/* Dispatch one fully-received frame. */
static void mgmt_dispatch(mgmt_ctx *c, const uint8_t *frame, uint32_t clen, uint32_t rlen) {
    uint16_t command = (uint16_t)((frame[6] << 8) | frame[7]);
    uint32_t reqid   = ((uint32_t)frame[10] << 24) | ((uint32_t)frame[11] << 16) |
                       ((uint32_t)frame[12] << 8)  | frame[13];
    const uint8_t *payload = frame + HDR_BYTES;
    const uint8_t *raw     = frame + HDR_BYTES + clen;

    BRIDGE_LOCK();
    switch (command) {
        case 1:  handle_hello(c, reqid); break;
        case 2:  handle_get_status(c, reqid); break;
        case 3:  handle_list_directory(c, command, reqid, payload, clen); break;
        case 5:  handle_write_begin(c, command, reqid, payload, clen); break;
        case 6:  handle_write_chunk(c, command, reqid, payload, clen, raw, rlen); break;
        case 7:  handle_write_commit(c, command, reqid); break;
        case 8:  handle_write_abort(c, command, reqid); break;
        case 9:  handle_delete_path(c, command, reqid, payload, clen); break;
        case 10: handle_mount_drive(c, command, reqid, payload, clen); break;
        case 11: handle_unmount_drive(c, command, reqid, payload, clen); break;
        case 14: handle_vm_reset(c, command, reqid); break;
        case 19: handle_read_file_chunk(c, command, reqid, payload, clen); break;
        default: send_error(c, command, reqid, "not_implemented",
                            "command not implemented yet"); break;
    }
    BRIDGE_UNLOCK();
}

/* Serve one mgmt connection: accumulate + parse NVH1 frames until the peer
 * closes. Replaces lwIP's mgmt_recv reassembly loop. */
static void mgmt_serve(int fd) {
    mgmt_ctx *c = calloc(1, sizeof *c);
    if (!c) { close(fd); return; }
    c->fd = fd;

    static uint8_t rx[RX_CAP];                /* one frame; serialised per conn */
    uint32_t rxlen = 0;
    printf("[mgmt] client connected (6504)\n");

    for (;;) {
        /* parse as many complete frames as we currently have */
        int progressed = 1;
        while (progressed) {
            progressed = 0;
            if (rxlen < HDR_BYTES) break;
            if (memcmp(rx, "NVH1", 4) != 0) { rxlen = 0; break; }   /* resync */
            uint32_t clen = ((uint32_t)rx[14] << 24) | ((uint32_t)rx[15] << 16) |
                            ((uint32_t)rx[16] << 8)  | rx[17];
            uint32_t rlen = ((uint32_t)rx[18] << 24) | ((uint32_t)rx[19] << 16) |
                            ((uint32_t)rx[20] << 8)  | rx[21];
            uint32_t total = HDR_BYTES + clen + rlen;
            if (total > RX_CAP) { rxlen = 0; break; }               /* too big */
            if (rxlen < total) break;                               /* need more */
            mgmt_dispatch(c, rx, clen, rlen);
            uint32_t leftover = rxlen - total;
            if (leftover) memmove(rx, rx + total, leftover);
            rxlen = leftover;
            progressed = 1;
        }
        /* pull more bytes */
        if (rxlen >= RX_CAP) { rxlen = 0; }                         /* overflow */
        ssize_t r = recv(fd, rx + rxlen, RX_CAP - rxlen, 0);
        if (r == 0) break;                                          /* peer closed */
        if (r < 0) { if (errno == EINTR) continue; break; }
        rxlen += (uint32_t)r;
    }

    upload_cleanup(c);
    free(c);
    close(fd);
    printf("[mgmt] client disconnected (6504)\n");
}

/* ===========================================================================
 *  DEBUG SERVER (6503) — newline-delimited JSON
 * =========================================================================== */

/* tiny JSON readers (flat request objects) — ported verbatim from debug.c */
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
                case 'u': {
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

/* per-connection tx scratch (one client at a time, but each conn gets its own) */
typedef struct { int fd; char tx[DBG_TXCAP]; } dbg_ctx;

static void dbg_respond(dbg_ctx *c, const char *json) {
    int n = snprintf(c->tx, DBG_TXCAP, "%s\n", json);
    if (n < 0) return;
    if (n > (int)DBG_TXCAP) n = (int)DBG_TXCAP;
    write_all(c->fd, c->tx, (size_t)n);
}
static void dbg_respond_ok(dbg_ctx *c) { dbg_respond(c, "{\"ok\":true}"); }
static void dbg_respond_err(dbg_ctx *c, const char *msg) {
    char b[160];
    snprintf(b, sizeof b, "{\"ok\":false,\"error\":\"%s\"}", msg);
    dbg_respond(c, b);
}

/* Append one screen char, JSON-escaped, into buf (printable ASCII only). */
static int put_screen_char(char *buf, int pos, int cap, unsigned char ch) {
    char c = (ch >= 0x20 && ch < 0x7F) ? (char)ch : ' ';
    if (c == '"' || c == '\\') { if (pos < cap - 2) { buf[pos++] = '\\'; buf[pos++] = c; } }
    else if (pos < cap - 1) buf[pos++] = c;
    return pos;
}

/* key name -> VGC key code (mirror of debug.c / the ESP32 debug server) */
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
    return -1;
}

/* push a key into the VGC key FIFO (R_KEY); mirror of main.c's kb_emit */
static void kb_emit(unsigned char ch) { wr(R_KEY, ch); }

/* ---- debug command handlers ---------------------------------------------- */
static void do_peek(dbg_ctx *c, const char *s) {
    int addr = json_int(s, "address", -1);
    if (addr < 0) { dbg_respond_err(c, "Missing 'address'"); return; }
    char b[64];
    snprintf(b, sizeof b, "{\"ok\":true,\"address\":%d,\"value\":%d}", addr, peek((unsigned)addr));
    dbg_respond(c, b);
}

static void do_peek_block(dbg_ctx *c, const char *s) {
    int addr = json_int(s, "address", -1);
    int count = json_int(s, "count", -1);
    if (addr < 0) { dbg_respond_err(c, "Missing 'address'"); return; }
    if (count < 1 || count > 256) { dbg_respond_err(c, "'count' must be 1..256"); return; }
    int pos = snprintf(c->tx, DBG_TXCAP, "{\"ok\":true,\"address\":%d,\"count\":%d,\"values\":[", addr, count);
    for (int i = 0; i < count; i++)
        pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "%s%d", i ? "," : "", peek((unsigned)(addr + i)));
    pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "]}\n");
    write_all(c->fd, c->tx, (size_t)pos);
}

static void do_poke(dbg_ctx *c, const char *s) {
    int addr = json_int(s, "address", -1);
    int val  = json_int(s, "value", -1);
    if (addr < 0) { dbg_respond_err(c, "Missing 'address'"); return; }
    if (val  < 0) { dbg_respond_err(c, "Missing 'value'"); return; }
    poke((unsigned)addr, (unsigned char)val);
    dbg_respond_ok(c);
}

static void do_read_vram(dbg_ctx *c, const char *s) {
    int space = json_int(s, "space", -1);
    int addr = json_int(s, "address", -1);
    int length = json_int(s, "length", 1);
    if (space < 0 || space > 7) { dbg_respond_err(c, "Missing/invalid 'space'"); return; }
    if (addr < 0) { dbg_respond_err(c, "Missing 'address'"); return; }
    if (length < 1 || length > 256) { dbg_respond_err(c, "'length' must be 1..256"); return; }
    int pos = snprintf(c->tx, DBG_TXCAP, "{\"ok\":true,\"space\":%d,\"address\":%d", space, addr);
    if (length == 1) {
        pos += snprintf(c->tx + pos, DBG_TXCAP - pos, ",\"value\":%d}\n", vmem_read((unsigned)space, (unsigned)addr));
    } else {
        pos += snprintf(c->tx + pos, DBG_TXCAP - pos, ",\"data\":[");
        for (int i = 0; i < length; i++)
            pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "%s%d", i ? "," : "", vmem_read((unsigned)space, (unsigned)(addr + i)));
        pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "]}\n");
    }
    write_all(c->fd, c->tx, (size_t)pos);
}

static void do_send_key(dbg_ctx *c, const char *s) {
    char key[24];
    json_str(s, "key", key, sizeof key);
    if (!strncasecmp(key, "ALT-", 4) && key[4] && !key[5]) {
        char ch = key[4];
        if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + 0x20);
        if (ch >= 'a' && ch <= 'z') {
            kb_emit(0x1B);
            usleep(20000);
            kb_emit((unsigned char)ch);
            usleep(20000);
            dbg_respond_ok(c);
            return;
        }
    }
    int code = key_code(key);
    if (code < 0) { dbg_respond_err(c, "Unknown key"); return; }
    kb_emit((unsigned char)code);
    usleep(20000);
    dbg_respond_ok(c);
}

static void do_type_text(dbg_ctx *c, const char *s) {
    static char text[4096];
    json_str(s, "text", text, sizeof text);
    int prev_cr = 0;
    for (int i = 0; text[i]; i++) {
        char ch = text[i];
        if (ch == '\n') { if (prev_cr) { prev_cr = 0; continue; } ch = '\r'; }
        prev_cr = (ch == '\r');
        kb_emit((unsigned char)ch);
        usleep(20000);
    }
    dbg_respond_ok(c);
}

static void do_read_screen(dbg_ctx *c) {
    int pos = snprintf(c->tx, DBG_TXCAP, "{\"ok\":true,\"lines\":[");
    for (int row = 0; row < SCREEN_ROWS; row++) {
        if (row) c->tx[pos++] = ',';
        c->tx[pos++] = '"';
        for (int col = 0; col < SCREEN_COLS; col++)
            pos = put_screen_char(c->tx, pos, DBG_TXCAP, vmem_read(VGC_SPACE_CHAR, (unsigned)(row * SCREEN_COLS + col)));
        c->tx[pos++] = '"';
    }
    pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "],\"cursor_x\":%d,\"cursor_y\":%d}\n",
                    peek(REG_CURSOR_X), peek(REG_CURSOR_Y));
    write_all(c->fd, c->tx, (size_t)pos);
}

static void do_read_line(dbg_ctx *c, const char *s) {
    int row = json_int(s, "row", -1);
    if (row < 0) row = peek(REG_CURSOR_Y);
    if (row < 0 || row >= SCREEN_ROWS) { dbg_respond_err(c, "Invalid row"); return; }
    int pos = snprintf(c->tx, DBG_TXCAP, "{\"ok\":true,\"row\":%d,\"text\":\"", row);
    for (int col = 0; col < SCREEN_COLS; col++)
        pos = put_screen_char(c->tx, pos, DBG_TXCAP, vmem_read(VGC_SPACE_CHAR, (unsigned)(row * SCREEN_COLS + col)));
    pos += snprintf(c->tx + pos, DBG_TXCAP - pos, "\"}\n");
    write_all(c->fd, c->tx, (size_t)pos);
}

static void do_get_cursor(dbg_ctx *c) {
    char b[80];
    snprintf(b, sizeof b, "{\"ok\":true,\"x\":%d,\"y\":%d,\"enabled\":%s}",
             peek(REG_CURSOR_X), peek(REG_CURSOR_Y),
             (peek(REG_CURSOR_ENABLE) & 0x01) ? "true" : "false");
    dbg_respond(c, b);
}

static void do_reset(dbg_ctx *c) {
    vm_reset_ctrl_locked(0);
    dbg_respond(c, "{\"ok\":true,\"reset\":true}");
}

static void do_cold_start(dbg_ctx *c) {
    host_reboot_vm();
    dbg_respond(c, "{\"ok\":true,\"cold_start\":true}");
}

/* dispatch one JSON line. BRIDGE_LOCK serialises all machine access. */
static void dbg_dispatch(dbg_ctx *c, const char *line) {
    char cmd[32];
    if (!json_str(line, "command", cmd, sizeof cmd)) { dbg_respond_err(c, "Missing 'command'"); return; }

    BRIDGE_LOCK();
    if      (!strcmp(cmd, "peek"))        do_peek(c, line);
    else if (!strcmp(cmd, "peek_block"))  do_peek_block(c, line);
    else if (!strcmp(cmd, "poke"))        do_poke(c, line);
    else if (!strcmp(cmd, "read_vram"))   do_read_vram(c, line);
    else if (!strcmp(cmd, "send_key"))    do_send_key(c, line);
    else if (!strcmp(cmd, "type_text"))   do_type_text(c, line);
    else if (!strcmp(cmd, "read_screen")) do_read_screen(c);
    else if (!strcmp(cmd, "read_line"))   do_read_line(c, line);
    else if (!strcmp(cmd, "get_cursor"))  do_get_cursor(c);
    else if (!strcmp(cmd, "vm_reset"))    do_reset(c);
    else if (!strcmp(cmd, "cold_start"))  do_cold_start(c);
    else if (!strcmp(cmd, "wait_ready"))  dbg_respond_ok(c);   /* not pollable; best-effort */
    /* read_xram: bare-metal read XRAM from PS DDR; no PS-DDR XRAM mirror on the
     * Linux host (XRAM lives in the PL), so report unsupported.  TODO: expose
     * an XRAM debug read path if/when one exists. */
    else if (!strcmp(cmd, "read_xram"))   dbg_respond_err(c, "read_xram not supported on this host");
    /* audio_*: bare-metal forwarded to the TSF/TinyMIDI HDMI audio engine
     * (audio.h). The Linux host only feeds silence (novavm.c audio_feed); there
     * is no naudio module yet, so these report unsupported.  TODO: wire to a
     * Linux audio module when one lands. */
    else if (!strncmp(cmd, "audio_", 6))  dbg_respond_err(c, "audio not supported on this host");
    else if (!strcmp(cmd, "fill_vram"))   dbg_respond_err(c, "fill_vram not supported on this host");
    else if (!strcmp(cmd, "run_cycles"))  dbg_respond_err(c, "run_cycles not supported (free-running CPU)");
    else if (!strncmp(cmd, "dbg_", 4))    dbg_respond_err(c, "debugger not supported on this host");
    else { char b[80]; snprintf(b, sizeof b, "{\"ok\":false,\"error\":\"Unknown command: %s\"}", cmd); dbg_respond(c, b); }
    BRIDGE_UNLOCK();
}

/* Serve one debug connection: process each '\n'-terminated line until close. */
static void dbg_serve(int fd) {
    dbg_ctx *c = calloc(1, sizeof *c);
    if (!c) { close(fd); return; }
    c->fd = fd;
    static char rx[DBG_RXCAP];
    uint32_t rxlen = 0;
    printf("[dbg] client connected (6503)\n");

    for (;;) {
        char *nl;
        while ((nl = memchr(rx, '\n', rxlen)) != NULL) {
            *nl = 0;
            dbg_dispatch(c, rx);
            uint32_t consumed = (uint32_t)(nl - rx) + 1;
            uint32_t leftover = rxlen - consumed;
            if (leftover) memmove(rx, rx + consumed, leftover);
            rxlen = leftover;
        }
        if (rxlen >= DBG_RXCAP - 1) rxlen = 0;                     /* overflow resync */
        ssize_t r = recv(fd, rx + rxlen, (DBG_RXCAP - 1) - rxlen, 0);
        if (r == 0) break;
        if (r < 0) { if (errno == EINTR) continue; break; }
        rxlen += (uint32_t)r;
    }
    free(c);
    close(fd);
    printf("[dbg] client disconnected (6503)\n");
}

/* ===========================================================================
 *  UPLOAD SERVER (6502) — raw byte stream -> /data/nova/UPLOAD.NDI  (was net.c)
 * =========================================================================== */
static void upload_serve(int fd) {
    mkdir(NOVA_FS_ROOT, 0775);
    FILE *f = fopen(UPLOAD_PATH, "wb");
    if (!f) { printf("[net] ERROR: cannot open %s\n", UPLOAD_PATH); close(fd); return; }
    printf("[net] upload start -> %s\n", UPLOAD_PATH);
    unsigned total = 0;
    uint8_t buf[8192];
    for (;;) {
        ssize_t r = recv(fd, buf, sizeof buf, 0);
        if (r == 0) break;
        if (r < 0) { if (errno == EINTR) continue; break; }
        fwrite(buf, 1, (size_t)r, f);
        total += (unsigned)r;
    }
    fclose(f);
    close(fd);
    printf("[net] upload complete: %u bytes -> %s\n", total, UPLOAD_PATH);
}

/* ===========================================================================
 *  LISTENER THREADS
 * =========================================================================== */
typedef void (*serve_fn)(int fd);

typedef struct { int port; serve_fn serve; const char *tag; } listener_arg;

static void *listener_thread(void *vp) {
    listener_arg *la = (listener_arg *)vp;
    int lfd = listen_on(la->port);
    if (lfd < 0) {
        printf("[%s] ERROR: cannot bind TCP %d\n", la->tag, la->port);
        free(la);
        return NULL;
    }
    printf("[%s] NovaHost server up on TCP %d\n", la->tag, la->port);
    for (;;) {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd < 0) { if (errno == EINTR) continue; break; }
        int one = 1;
        setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
        la->serve(cfd);                       /* blocking, one client at a time */
    }
    close(lfd);
    free(la);
    return NULL;
}

static void spawn_listener(int port, serve_fn serve, const char *tag) {
    listener_arg *la = malloc(sizeof *la);
    if (!la) return;
    la->port = port; la->serve = serve; la->tag = tag;
    pthread_t t;
    if (pthread_create(&t, NULL, listener_thread, la) != 0) { free(la); return; }
    pthread_detach(t);
}

void servers_init(void) {
    if (!g_drives_loaded) drives_load();
    spawn_listener(UPLOAD_PORT, upload_serve, "net");
    spawn_listener(DBG_PORT,    dbg_serve,    "dbg");
    spawn_listener(MGMT_PORT,   mgmt_serve,   "mgmt");
    printf("[novavm] NovaHost servers: upload=%d debug=%d mgmt=%d\n",
           UPLOAD_PORT, DBG_PORT, MGMT_PORT);
}
