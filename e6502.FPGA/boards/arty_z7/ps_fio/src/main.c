// main.c — NovaVM PS FIO host for the Arty Z7-20.
//
// The Zynq PS A9 is the NovaVM host (replacing the ESP32 NovaHost). It talks to
// the PL fio_bridge over M_AXI_GP0 (mapped at 0x40000000) to:
//   1. stage the resident lib_call loader into CPU RAM $0320 while the 6502 is
//      held in reset, then release the CPU,
//   2. service fio_event: on FIO_CMD_LOAD_MODULE, stream the requested library
//      module (.nmod) from the microSD into the XRAM shelf slot (PS DDR3),
//   3. NAK unimplemented FIO commands for now (file I/O / keyboard come next).
//
// XRAM lives in PS DDR3 (axi_xram maps XRAM byte addr -> 0x10000000+addr), so a
// module load is a plain DDR write + cache flush (so the PL HP0 port sees it).

#include "xil_io.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "ff.h"
#include "xuartps_hw.h"
#include "loader_bin.h"
#include "modules_embedded.h"   // EMBEDDED_MOD[1..8], 16KB each

void net_init(void);            // net.c — PS Ethernet (lwIP + DHCP + TCP upload)
void net_poll(void);
void usb_init(void);            // usb.c — PS USB host (HID keyboard)
void usb_poll(void);

// ---- fio_bridge register map (AXI4-Lite @ 0x40000000) ----------------------
#define FIO_BASE     0x40000000u
#define R_POKE       (FIO_BASE + 0x00)   // W {addr[15:0]<<8 | data[7:0]}
#define R_PEEK_ADDR  (FIO_BASE + 0x04)   // W addr[15:0]
#define R_PEEK_DATA  (FIO_BASE + 0x08)   // R data[7:0]
#define R_KEY        (FIO_BASE + 0x0C)   // W key[7:0]
#define R_CTRL       (FIO_BASE + 0x10)   // RW bit0 = cpu_reset (1=hold)
#define R_STATUS     (FIO_BASE + 0x14)   // R {key_ready,fio_event}; W bit0 clears event
#define R_CPU_PC     (FIO_BASE + 0x18)   // R live 6502 PC
#define R_DBG        (FIO_BASE + 0x1C)   // R [0]rdy [1]sreq [2]sbusy [3]svalid [4]sdone [5]sready [6]we
#define R_AUX        (FIO_BASE + 0x20)   // R {stream_words[13:0], stream_left[13:0]}
#define R_ROMW       (FIO_BASE + 0x24)   // W {idx@22, addr[13:0]@8, data[7:0]} -> bank ROM write

// ---- 6502 FIO register bank ($B9A0) ----------------------------------------
#define FIO_CMD      0xB9A0
#define FIO_STATUS   0xB9A1
#define FIO_ERRCODE  0xB9A2
#define FIO_SRC_LO   0xB9A4
#define FIO_END_LO   0xB9A6
#define FIO_OK       0x02
#define FIO_ERR      0x03
#define FIO_CMD_LOAD_MODULE 0x2C

// ---- FIO file I/O register map (mirrors Avalonia FileIoController ABI) -------
#define FIO_NAMELEN  0xB9A3
#define FIO_SRC_HI   0xB9A5
#define FIO_END_HI   0xB9A7
#define FIO_SIZE_LO  0xB9A8
#define FIO_SIZE_HI  0xB9A9
#define FIO_SIZE2    0xB9AA   // FioGSpace: dir size byte 2
#define FIO_DIRTYPE  0xB9AF   // 0=BAS 1=SID 2=BIN 3=MID 4=GFX 5=DIR 6=FORTH
#define FIO_NAME     0xB9B0   // 64-byte filename buffer
#define FIO_CMD_SAVE     0x01
#define FIO_CMD_LOAD     0x02
#define FIO_CMD_DIROPEN  0x03
#define FIO_CMD_DIRREAD  0x04
#define FIO_CMD_DELETE   0x05
#define FIO_ERR_NOTFOUND 1
#define FIO_ERR_IO       2
#define FIO_ERR_EOD      3
#define DT_BAS 0
#define DT_SID 1
#define DT_BIN 2
#define DT_MID 3
#define DT_GFX 4
#define DT_DIR 5
#define DT_FORTH 6

// ---- XRAM (PS DDR3) library shelf ------------------------------------------
#define XRAM_DDR_BASE  0x10000000u
#define SHELF_BASE     0x00060000u
#define SHELF_SLOT     0x00004000u       // 16 KB
#define MODULE_BYTES   16384u

static inline void     poke(unsigned a, unsigned char d){ Xil_Out32(R_POKE, (a << 8) | d); }
static inline unsigned char peek(unsigned a){ Xil_Out32(R_PEEK_ADDR, a); return (unsigned char)Xil_In32(R_PEEK_DATA); }
static inline void     cpu_hold(int hold){ Xil_Out32(R_CTRL, hold ? 1u : 0u); }
static inline int      fio_pending(void){ return Xil_In32(R_STATUS) & 1u; }
static inline void     fio_clear(void){ Xil_Out32(R_STATUS, 1u); }
static inline int      key_ready(void){ return (Xil_In32(R_STATUS) >> 1) & 1u; }
static inline void     key_send(unsigned char k){ Xil_Out32(R_KEY, k); }
void kb_emit(unsigned char c){ Xil_Out32(R_KEY, c); }   // usb.c HID -> VGC key queue

// Non-blocking read of one byte from the console UART (same UART as xil_printf).
static int uart_getc(void) {
    if (XUartPs_IsReceiveData(STDOUT_BASEADDRESS))
        return (int)(XUartPs_ReadReg(STDOUT_BASEADDRESS, XUARTPS_FIFO_OFFSET) & 0xFF);
    return -1;
}

// Module id -> 8.3 name prefix. Modules may appear as <NAME>.MOD (raw 16KB image)
// or <NAME>~1.NMO (.nmod: 16KB image + doc trailer; we read the first 16KB). We
// scan /lib then / matching the prefix + .MOD/.NMO (xilffs is built without LFN).
static const char *module_prefix(int id) {
    switch (id) {
        case 1: return "GRAPHI";   // graphics
        case 2: return "SOUND";    // sound
        case 3: return "SYSTEM";   // system
        case 4: return "FILES";    // files
        case 5: return "MEMORY";   // memory
        case 6: return "NET";      // net
        case 7: return "TURTLE";   // turtle
        case 8: return "EDITOR";   // editor
        default: return 0;
    }
}

static int is_mod_ext(const char *name) {
    const char *d = strrchr(name, '.');
    if (!d) return 0;
    return (strncasecmp(d, ".MOD", 4) == 0) || (strncasecmp(d, ".NMO", 4) == 0);
}

static int read_to_slot(const char *path, int slot) {
    FIL f;
    if (f_open(&f, path, FA_READ) != FR_OK) return -1;
    UINTPTR ddr = XRAM_DDR_BASE + SHELF_BASE + (unsigned)slot * SHELF_SLOT;
    UINT br = 0;
    FRESULT r = f_read(&f, (void *)ddr, MODULE_BYTES, &br);
    f_close(&f);
    if (r != FR_OK || br < MODULE_BYTES) {
        xil_printf("[fio] read %s: %u/%u failed\r\n", path, br, MODULE_BYTES);
        return -1;
    }
    Xil_DCacheFlushRange(ddr, MODULE_BYTES);   // make the write visible to PL HP0
    xil_printf("[fio] %s -> XRAM slot %d (0x%08x)\r\n", path, slot, (unsigned)ddr);
    return 0;
}

static int scan_load(const char *dir, const char *pfx, int slot) {
    DIR d; FILINFO fno; size_t n = strlen(pfx);
    if (f_opendir(&d, dir) != FR_OK) return -1;
    int rc = -1;
    for (;;) {
        if (f_readdir(&d, &fno) != FR_OK || fno.fname[0] == 0) break;
        if (fno.fattrib & AM_DIR) continue;
        if (strncasecmp(fno.fname, pfx, n) == 0 && is_mod_ext(fno.fname)) {
            char path[96];
            snprintf(path, sizeof(path), "%s/%s", dir, fno.fname);
            rc = read_to_slot(path, slot);
            break;
        }
    }
    f_closedir(&d);
    return rc;
}

// ARTY PAGE-IN: write the module image DIRECTLY into the 6502's bank-1 (ext_rom)
// via the fio_bridge ROM-load port (idx=1). The loader's subsequent PGD page-in
// is a no-op (axi_xram stream instant-completes), so bank-1 keeps this image.
// Reliable GP0 single-writes -- avoids the flaky HP0 burst-read DMA entirely.
static int load_module(int id, int slot) {
    (void)slot;
    if (id < 1 || id > 8 || !EMBEDDED_MOD[id]) {
        xil_printf("[fio] module %d not embedded\r\n", id);
        return -1;
    }
    const unsigned char *img = EMBEDDED_MOD[id];
    for (unsigned a = 0; a < MODULE_BYTES; a++)
        Xil_Out32(R_ROMW, (1u << 22) | (a << 8) | img[a]);   // idx=1 (ext_rom)
    xil_printf("[fio] module %d -> bank-1 (PS direct, %u bytes)\r\n", id, (unsigned)MODULE_BYTES);
    return 0;
}

// ---- FIO file I/O (LOAD/SAVE/DIR/DELETE from microSD) -----------------------
// The FIO registers live in 6502 RAM ($B9A0+); the PS reads params via peek() and
// writes results via poke() (matching the Avalonia FileIoController semantics).
static unsigned char g_fbuf[65536];     // file <-> 6502-RAM staging buffer
static DIR  g_dir;                       // open directory for DIROPEN/DIRREAD
static int  g_dir_open = 0;

static void fio_ok(void)        { poke(FIO_ERRCODE, 0);    poke(FIO_STATUS, FIO_OK);  poke(FIO_CMD, 0); }
static void fio_fail(int code)  { poke(FIO_ERRCODE, code); poke(FIO_STATUS, FIO_ERR); poke(FIO_CMD, 0); }

// Read the filename from FioNameLen/FioName into out (NUL-terminated). <0 on bad len.
static int fio_read_name(char *out, int maxlen) {
    int n = peek(FIO_NAMELEN);
    if (n < 1 || n > 63 || n >= maxlen) return -1;
    for (int i = 0; i < n; i++) out[i] = (char)peek(FIO_NAME + i);
    out[n] = 0;
    while (n > 0 && (out[n-1] == ' ' || out[n-1] == 0)) out[--n] = 0;
    return n;
}

// Build "0:/<name>" -- append .bas if the name has no extension (BASIC default).
static void fio_path(const char *name, char *path, int sz) {
    if (strrchr(name, '.')) snprintf(path, sz, "0:/%s", name);
    else                    snprintf(path, sz, "0:/%s.bas", name);
}

static int fio_dirtype_for(const FILINFO *fno) {
    if (fno->fattrib & AM_DIR) return DT_DIR;
    const char *d = strrchr(fno->fname, '.');
    if (!d) return DT_BIN;
    if (!strncasecmp(d, ".bas", 5)) return DT_BAS;
    if (!strncasecmp(d, ".bin", 5)) return DT_BIN;
    if (!strncasecmp(d, ".sid", 5)) return DT_SID;
    if (!strncasecmp(d, ".mid", 5)) return DT_MID;
    if (!strncasecmp(d, ".gfx", 5)) return DT_GFX;
    if (!strncasecmp(d, ".4th", 5)) return DT_FORTH;
    return DT_BIN;
}

static void fio_load(void) {
    char name[80], path[96];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    fio_path(name, path, sizeof path);
    int is_bin = 0; { const char *d = strrchr(path, '.'); if (d && !strncasecmp(d, ".bin", 5)) is_bin = 1; }

    FIL f;
    if (f_open(&f, path, FA_READ) != FR_OK) { fio_fail(FIO_ERR_NOTFOUND); return; }
    UINT br = 0;
    FRESULT r = f_read(&f, g_fbuf, sizeof g_fbuf, &br);
    f_close(&f);
    if (r != FR_OK || br < 2) { fio_fail(FIO_ERR_IO); return; }

    // Files carry a 2-byte load-address prefix. .bin loads at the file's address
    // (write it back to FioSrc); everything else loads at the caller's FioSrc.
    unsigned dst = is_bin ? (g_fbuf[0] | (g_fbuf[1] << 8))
                          : (peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8));
    unsigned len = br - 2;
    if (dst + len > 0x10000) { fio_fail(FIO_ERR_IO); return; }
    for (unsigned i = 0; i < len; i++) poke((dst + i) & 0xFFFF, g_fbuf[2 + i]);
    if (is_bin) { poke(FIO_SRC_LO, dst & 0xFF); poke(FIO_SRC_HI, (dst >> 8) & 0xFF); }
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
    poke(FIO_DIRTYPE, is_bin ? DT_BIN : DT_BAS);
    xil_printf("[fio] LOAD %s -> $%04x (%u bytes)\r\n", path, dst, len);
    fio_ok();
}

static void fio_save(void) {
    char name[80], path[96];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    fio_path(name, path, sizeof path);
    unsigned src = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8);
    unsigned end = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
    if (end <= src) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = end - src;
    g_fbuf[0] = src & 0xFF; g_fbuf[1] = (src >> 8) & 0xFF;     // 2-byte load-address prefix
    for (unsigned i = 0; i < len; i++) g_fbuf[2 + i] = peek((src + i) & 0xFFFF);

    FIL f;
    if (f_open(&f, path, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) { fio_fail(FIO_ERR_IO); return; }
    UINT bw = 0;
    FRESULT r = f_write(&f, g_fbuf, len + 2, &bw);
    f_close(&f);
    if (r != FR_OK || bw != len + 2) { fio_fail(FIO_ERR_IO); return; }
    xil_printf("[fio] SAVE %s ($%04x-$%04x, %u bytes)\r\n", path, src, end, len);
    fio_ok();
}

// Write a directory entry to the FIO regs: display name (base, no ext), type, size.
static void fio_populate(const FILINFO *fno) {
    int type = fio_dirtype_for(fno);
    poke(FIO_DIRTYPE, type);
    char disp[64]; int dl = 0;
    for (; fno->fname[dl] && dl < 63; dl++) disp[dl] = fno->fname[dl];
    disp[dl] = 0;
    if (!(fno->fattrib & AM_DIR)) { char *dot = strrchr(disp, '.'); if (dot) { *dot = 0; dl = (int)(dot - disp); } }
    poke(FIO_NAMELEN, dl);
    for (int i = 0; i < dl; i++) poke(FIO_NAME + i, disp[i]);
    xil_printf("[fio] dirent: %-20s type=%d\r\n", disp, type);
    unsigned sz = (unsigned)fno->fsize;
    if (!(fno->fattrib & AM_DIR) && (type == DT_BAS || type == DT_BIN)) sz = sz >= 2 ? sz - 2 : 0;
    poke(FIO_SIZE_LO, sz & 0xFF); poke(FIO_SIZE_HI, (sz >> 8) & 0xFF); poke(FIO_SIZE2, (sz >> 16) & 0xFF);
}

static int fio_next_entry(FILINFO *fno) {
    for (;;) {
        if (f_readdir(&g_dir, fno) != FR_OK || fno->fname[0] == 0) return 0;
        if (fno->fname[0] == '.') continue;       // skip ., .., .Spotlight-V100 etc.
        return 1;
    }
}

static void fio_diropen(void) {
    if (g_dir_open) { f_closedir(&g_dir); g_dir_open = 0; }
    FRESULT od = f_opendir(&g_dir, "0:/");
    xil_printf("[fio] DIROPEN f_opendir=%d\r\n", od);
    if (od != FR_OK) { fio_fail(FIO_ERR_IO); return; }
    g_dir_open = 1;
    FILINFO fno;
    if (fio_next_entry(&fno)) { fio_populate(&fno); fio_ok(); }      // DIROPEN returns entry[0]
    else { f_closedir(&g_dir); g_dir_open = 0; fio_fail(FIO_ERR_EOD); }
}

static void fio_dirread(void) {
    if (!g_dir_open) { fio_fail(FIO_ERR_EOD); return; }
    FILINFO fno;
    if (fio_next_entry(&fno)) { fio_populate(&fno); fio_ok(); }
    else { f_closedir(&g_dir); g_dir_open = 0; fio_fail(FIO_ERR_EOD); }
}

static void fio_delete(void) {
    char name[80], path[96];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    fio_path(name, path, sizeof path);
    if (f_unlink(path) != FR_OK) { fio_fail(FIO_ERR_NOTFOUND); return; }
    fio_ok();
}

static void list_dir(const char *path) {
    DIR d; FILINFO fno;
    xil_printf("[fio] dir %s:\r\n", path);
    if (f_opendir(&d, path) != FR_OK) { xil_printf("  (open failed)\r\n"); return; }
    for (;;) {
        if (f_readdir(&d, &fno) != FR_OK || fno.fname[0] == 0) break;
        xil_printf("  %s %-20s %u\r\n", (fno.fattrib & AM_DIR) ? "<DIR>" : "     ",
                   fno.fname, (unsigned)fno.fsize);
    }
    f_closedir(&d);
}

int main(void) {
    static FATFS fs;
    Xil_DCacheEnable();
    xil_printf("\r\n[NovaVM PS FIO host] starting\r\n");

    cpu_hold(1);                                // keep the 6502 in reset

    if (f_mount(&fs, "0:/", 1) != FR_OK)
        xil_printf("[fio] WARNING: SD mount failed (module loads will NAK)\r\n");
    else {
        xil_printf("[fio] microSD mounted\r\n");
    }

    // Stage the resident lib_call loader into CPU RAM $0320.
    for (unsigned i = 0; i < sizeof(LOADER_BIN); i++)
        poke(0x0320 + i, LOADER_BIN[i]);
    xil_printf("[fio] resident loader staged @ $0320 (%u bytes)\r\n", (unsigned)sizeof(LOADER_BIN));

    // Make sure the console UART RX path is enabled (for console keyboard input).
    {
        u32 cr = XUartPs_ReadReg(STDOUT_BASEADDRESS, XUARTPS_CR_OFFSET);
        cr &= ~((u32)(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS));
        cr |=  (u32)(XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN);
        XUartPs_WriteReg(STDOUT_BASEADDRESS, XUARTPS_CR_OFFSET, cr);
    }

    cpu_hold(0);                                // release the 6502 -> boots to READY
    xil_printf("[fio] 6502 released; servicing FIO events + console keys\r\n");

    net_init();                                 // bring up PS Ethernet (DHCP + TCP upload)
    usb_init();                                 // bring up PS USB host (HID keyboard)

    for (;;) {
        // --- FIO host service ---
        if (fio_pending()) {
            fio_clear();
            unsigned char cmd = peek(FIO_CMD);
            if (cmd != 0) { xil_printf("[fio] cmd=0x%02x\r\n", cmd); poke(FIO_STATUS, 0); }  // log + mark busy
            if      (cmd == FIO_CMD_LOAD)    fio_load();
            else if (cmd == FIO_CMD_SAVE)    fio_save();
            else if (cmd == FIO_CMD_DIROPEN) fio_diropen();
            else if (cmd == FIO_CMD_DIRREAD) fio_dirread();
            else if (cmd == FIO_CMD_DELETE)  fio_delete();
            else if (cmd == FIO_CMD_LOAD_MODULE) {
                int id = peek(FIO_SRC_LO), slot = peek(FIO_END_LO);
                if (load_module(id, slot) == 0) {
                    // Cap the loader's 4-slot module shelf ($0418..$041B) at one
                    // entry (the resident). The page-in is a no-op (PS wrote bank-1),
                    // so a stale shelf-HIT on a module SWITCH would skip the load and
                    // leave the wrong module resident. Clearing the tags forces every
                    // switch to MISS -> FIO -> PS rewrites bank-1; repeats still hit
                    // the fast LIB_RESIDENT path before the shelf scan. The loader
                    // re-records the victim slot after FIO_STATUS=OK -> exactly 1 tag.
                    for (int s = 0; s < 4; s++) poke(0x0418 + s, 0);
                    poke(FIO_ERRCODE,0); poke(FIO_STATUS,FIO_OK);
                } else { poke(FIO_ERRCODE,1); poke(FIO_STATUS,FIO_ERR); }
            } else if (cmd != 0) { poke(FIO_ERRCODE,1); poke(FIO_STATUS,FIO_ERR); }
        }
        // --- console keyboard: forward serial bytes into the VGC key queue ---
        int ch = uart_getc();
        if (ch >= 0) {
            if (ch == '\n') ch = '\r';
            else if (ch == 0x7F) ch = 0x08;
            key_send((unsigned char)ch);
        }
        // --- PS Ethernet: service lwIP (RX + TCP timers + DHCP) ---
        net_poll();
        // --- PS USB host: detect/poll the HID keyboard ---
        usb_poll();
    }
    return 0;
}
