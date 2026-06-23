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

#include <stdio.h>
#include <string.h>
#include "xil_io.h"
#include "xil_cache.h"
#include "xttcps.h"             // TTC0 -> 1 kHz high-priority audio interrupt
#include "xinterrupt_wrap.h"   // XSetupInterruptSystem -- same shared GIC the lwIP
                               // adapter uses (handles the SDT-encoded IntrId)
#include "xil_printf.h"
#include "ff.h"
#include "xuartps_hw.h"
#include "loader_bin.h"
#include "modules_embedded.h"   // EMBEDDED_MOD[1..8], 16KB each
#include "ehbasic_rom.h"        // EHBASIC_ROM[16384] -> basic_rom (idx=0) at boot
#include "ndi.h"                // read-only .ndi image reader
#include "drives.h"             // drive-slot mount table
#include "audio.h"              // TSF + TinyMIDI -> HDMI audio FIFO

void net_init(void);            // net.c — PS Ethernet (lwIP + DHCP + TCP upload)
void net_poll(void);
void usb_init(void);            // usb.c — PS USB host (HID keyboard)
void usb_poll(void);
void mgmt_init(void);           // mgmt.c — NovaHost 6504 management server (NVH1/CBOR)
void mgmt_set_sd(int mounted);
void debug_init(void);          // debug.c — NovaHost 6503 debug server (newline JSON)
void drives_load(void);         // drives.c — load /config/mounts.txt at boot

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
#define R_VMEM_ADDR  (FIO_BASE + 0x28)   // W {space[2:0]@17, addr[16:0]} -> latch VGC vmem read addr
#define R_VMEM_DATA  (FIO_BASE + 0x2C)   // R {24'b0, data[7:0]} from VGC vmem (char/color/gfx/spr)
#define R_AUDIO      (FIO_BASE + 0x30)   // W [7:0] -> push 1 PCM byte to the HDMI audio FIFO
#define R_AUDIO_SPACE (FIO_BASE + 0x34)  // R [15:0] -> free bytes in the HDMI audio FIFO
#define R_AUDIO_EVT  (FIO_BASE + 0x38)   // R {valid@16, index[15:8], data[7:0]} -> pop a SID/WTS reg-write event
#define R_SID_VOL    (FIO_BASE + 0x3C)   // W [7:0] -> reDIP-SID mix level into HDMI (32=x1, 64=x2 default)

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
#define FIO_GSPACE   0xB9AA   // XRAM addr high byte (also size byte2)
#define FIO_GADDR_LO 0xB9AB   // XRAM addr low
#define FIO_GADDR_HI 0xB9AC   // XRAM addr mid
#define FIO_GLEN_LO  0xB9AD   // transfer length low
#define FIO_GLEN_HI  0xB9AE   // transfer length high
#define FIO_CMD_SAVE     0x01
#define FIO_CMD_LOAD     0x02
#define FIO_CMD_DIROPEN  0x03
#define FIO_CMD_DIRREAD  0x04
#define FIO_CMD_DELETE   0x05
#define FIO_CMD_XLOAD    0x18      // stream a file from the mounted image straight into XRAM
#define FIO_CMD_XPAGE    0x29      // stream a file SLICE (offset,len) into XRAM/RAM (story paging)
#define FIO_PAGE_XRAM    0x00      // XPAGE target: flat XRAM
#define FIO_PAGE_RAM     0x01      // XPAGE target: CPU RAM
#define FIO_CMD_LOADRUNTIME 0x28   // stream a 16KB runtime ROM into the $C000 primary bank
#define FIO_CMD_FOPEN    0x2D      // open file -> handle
#define FIO_CMD_FCREATE  0x2E
#define FIO_CMD_FCLOSE   0x2F
#define FIO_CMD_FREAD    0x30      // read chunk -> CPU RAM or XRAM
#define FIO_CMD_FWRITE   0x31
#define FIO_CMD_FSEEK    0x32
#define FIO_CMD_FTELL    0x33
#define FIO_CMD_FSIZE    0x34
#define FIO_CMD_VOLUME   0x0C      // set audio master gain (FIO_SRC_LO 0-255)
#define FIO_CMD_MIDPLAY  0x13      // play a raw .mid from SD through the wavetable
#define FIO_CMD_MIDSTOP  0x14
#define FIO_CMD_SFLOAD   0x15      // load a raw .sf2 soundfont from SD
#define FIO_TARGET_MASK  0x30      // FIO_DIRTYPE high bits: FREAD/FWRITE target
#define FIO_TARGET_XRAM  0x10      // 0x00 = CPU RAM (FIO_END addr), 0x10 = XRAM (FIO_GSPACE/GADDR)
#define FIO_ERR_NOTFOUND 1
#define FIO_ERR_IO       2
#define FIO_ERR_EOD      3
#define FIO_ERR_NOTMOUNTED 5
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
// Read a byte of VGC video memory (space 1=char 2=color 3=gfx 4=sprite 7=textattr).
// The char/etc RAM is internal to the VGC (not 6502-addressable); this rides the
// dbg_vmem read port wired into the fio_bridge. First read settles dpram latency.
static inline unsigned char vmem_read(unsigned space, unsigned addr){
    Xil_Out32(R_VMEM_ADDR, ((space & 7u) << 17) | (addr & 0x1FFFFu));
    (void)Xil_In32(R_VMEM_DATA);
    return (unsigned char)Xil_In32(R_VMEM_DATA);
}
static inline void     cpu_hold(int hold){ Xil_Out32(R_CTRL, hold ? 1u : 0u); }
// Pulse the 6502 reset line: it re-runs from the reset vector and boots to READY.
// The ROM + resident loader staged at boot stay resident, so no restage needed.
void vm_reset(void){ cpu_hold(1); for (volatile int i = 0; i < 200000; i++) {} cpu_hold(0); }
static inline int      fio_pending(void){ return Xil_In32(R_STATUS) & 1u; }
static inline void     fio_clear(void){ Xil_Out32(R_STATUS, 1u); }
static inline int      key_ready(void){ return (Xil_In32(R_STATUS) >> 1) & 1u; }
static inline void     key_send(unsigned char k){ Xil_Out32(R_KEY, k); }
void kb_emit(unsigned char c){ Xil_Out32(R_KEY, c); }   // usb.c HID -> VGC key queue

// HDMI audio FIFO (used by audio.c). The PL embeds this stream into HDMI audio.
void audio_fifo_write(const unsigned char *buf, int n) {
    for (int i = 0; i < n; i++) Xil_Out32(R_AUDIO, buf[i]);
}
int audio_fifo_space(void) { return (int)(Xil_In32(R_AUDIO_SPACE) & 0xFFFFu); }
// Pop one captured 6502 SID/WTS register-write event. Returns the raw word:
// bit16 = valid, [15:8] = index (SID1 0-31, SID2 32-63, WTS 64-223), [7:0] = data.
unsigned audio_evt_read(void) { return Xil_In32(R_AUDIO_EVT); }
// reDIP-SID mix control (R_SID_VOL): [7:0] = level (32=x1, 64=x2), [8] = stereo
// (1 = SID1->L / SID2->R for 2SID tunes, 0 = both SIDs summed to mono).
static unsigned g_sid_lvl = 128, g_sid_stereo = 0;   // x2 with the DC blocker at PCM_GAIN=1
static void sid_vol_write(void) {
    Xil_Out32(R_SID_VOL, ((g_sid_stereo & 1u) << 8) | (g_sid_lvl & 0xFFu));
}
void audio_set_sid_vol(unsigned v)   { g_sid_lvl = v & 0xFFu; sid_vol_write(); }
void audio_set_sid_stereo(int on)    { g_sid_stereo = on ? 1u : 0u; sid_vol_write(); }
// audio.c publishes per-voice notes + playback state into the $BA50 music-status
// block (the keyboard visualizer reads it). poke() reaches the PL music_regs RAM.
void audio_mmio_poke(unsigned addr, unsigned char v) { poke(addr, v); }

// Non-static wrappers so mgmt.c / debug.c can reach the AXI bridge primitives.
unsigned char dbg_peek(unsigned a){ return peek(a); }
void dbg_poke(unsigned a, unsigned char d){ poke(a, d); }
unsigned char dbg_vmem(unsigned space, unsigned addr){ return vmem_read(space, addr); }
// Read a byte of XRAM straight from PS DDR3 (cache-invalidated so we see what the
// PL bus masters see). Debug aid for the NOVAZ6/DMA off-by-one investigation.
unsigned char dbg_xram(unsigned addr){
    UINTPTR a = XRAM_DDR_BASE + (addr & 0x7FFFF);
    Xil_DCacheInvalidateRange(a, 1);
    return *(volatile unsigned char *)a;
}

// Debug: dump the VGC character RAM (80x50) to the serial so we can "see" the HDMI.
// Triggered by Ctrl-\ (0x1C) on the console UART (intercepted in the main loop).
#define VGC_SPACE_CHAR 1u
static void screen_dump(void) {
    unsigned ttr = peek(0xA0ED);                       // text_top_row (ring base)
    xil_printf("\r\n--- SCREEN ttr=%u ---\r\n", ttr);
    for (int row = 0; row < 50; row++) {
        char line[81]; int any = 0;
        for (int col = 0; col < 80; col++) {
            unsigned char ch = vmem_read(VGC_SPACE_CHAR, (unsigned)(row * 80 + col));
            if (ch >= 0x21 && ch < 0x7F) any = 1;
            line[col] = (ch >= 0x20 && ch < 0x7F) ? (char)ch : '.';
        }
        line[80] = 0;
        if (any) xil_printf("%02d|%s|\r\n", row, line);   // skip all-blank rows
    }
    xil_printf("--- END ---\r\n");
}

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

// ---- mounted boot image (.ndi) ---------------------------------------------
// A game disk mounted via `nova drive mount` (drives.c) is read by the 6502
// through the FIO LOAD/FOPEN/FREAD/LOAD_RUNTIME path. We keep the boot slot's
// image open and resolve plain filenames (AUTOBOOT, NOVAZ.BIN, story.bin) inside
// it. SD-root file ops still work when nothing is mounted.
static ndi_t g_img;
static int   g_img_slot = -1;

static ndi_t *boot_image(void) {
    int slot = drive_boot_slot();
    if (slot < 0) { if (g_img_slot >= 0) { ndi_close(&g_img); g_img_slot = -1; } return 0; }
    if (slot != g_img_slot) {
        if (g_img_slot >= 0) ndi_close(&g_img);
        char full[200];
        snprintf(full, sizeof full, "0:%s", drive_path(slot));   // path stored as "/x.ndi"
        if (ndi_open(&g_img, full) != 0) { g_img_slot = -1; return 0; }
        g_img_slot = slot;
        xil_printf("[fio] boot image %s = %s\r\n", drive_slot_name(slot), full);
    }
    return &g_img;
}

// Resolve a 6502 filename inside the mounted image: exact (ci), then "+.bin".
static int img_find(ndi_t *img, const char *name) {
    int idx = ndi_find(img, name, NDI_ROOT);
    if (idx < 0 && !strchr(name, '.')) {
        char tmp[40];
        snprintf(tmp, sizeof tmp, "%s.bin", name);
        idx = ndi_find(img, tmp, NDI_ROOT);
    }
    return idx;
}

// ---- low-level file handles (FOPEN/FREAD/FSEEK on the mounted image) --------
#define FH_MAX 8
static struct {
    int      used;
    int      idx;            // NDI dir index (read handles)
    uint32_t pos;
    uint32_t size;
    int      writing;        // FCREATE write handle: staged in g_wbuf, committed on FCLOSE
    uint8_t  wtype;          // NDI file type for the committed entry
    uint16_t wparent;        // NDI parent index for the committed entry
    char     wname[34];      // committed entry name
} g_fh[FH_MAX];

// Save staging: FWRITE accumulates into g_wbuf; FCLOSE creates the NDI entry and
// streams it in. One writer at a time (the Z-machine save path uses one handle).
#define WBUF_BYTES (256 * 1024)
static unsigned char g_wbuf[WBUF_BYTES];
static int g_wbuf_handle = -1;

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

    // Mounted-image path: resolve the name inside the boot drive's .ndi first
    // (the AUTOBOOT contract). Falls through to SD-root LOAD if not found / no mount.
    {
        ndi_t *img = boot_image();
        if (img) {
            int idx = img_find(img, name);
            if (idx >= 0) {
                ndi_entry_t e; ndi_get(img, idx, &e);
                int n = ndi_read(img, idx, 0, g_fbuf, sizeof g_fbuf);
                if (n < 2) { fio_fail(FIO_ERR_IO); return; }
                int is_bin = (e.type == DT_BIN);
                unsigned dst = is_bin ? (g_fbuf[0] | (g_fbuf[1] << 8))
                                      : (peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8));
                unsigned len = (unsigned)n - 2;
                if (dst + len > 0x10000) { fio_fail(FIO_ERR_IO); return; }
                for (unsigned i = 0; i < len; i++) poke((dst + i) & 0xFFFF, g_fbuf[2 + i]);
                if (is_bin) { poke(FIO_SRC_LO, dst & 0xFF); poke(FIO_SRC_HI, (dst >> 8) & 0xFF); }
                poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
                poke(FIO_DIRTYPE, e.type);
                xil_printf("[fio] LOAD(ndi) %s -> $%04x (%u bytes, type %d)\r\n", e.name, dst, len, e.type);
                fio_ok(); return;
            }
        }
    }

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

// FILE_LOAD_RUNTIME: stream a 16KB runtime ROM (e.g. NOVAZ.BIN) from the mounted
// image into the $C000 primary bank (dbg_rom idx=0). The 6502 caller runs from
// RAM (autoboot @ $7200) and then JMP ($FFFC) into the freshly-loaded runtime.
static void fio_load_runtime(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    if (e.size != 16384) { xil_printf("[fio] LOADRUNTIME %s wrong size %u\r\n", e.name, (unsigned)e.size); fio_fail(FIO_ERR_IO); return; }
    if (ndi_read(img, idx, 0, g_fbuf, 16384) != 16384) { fio_fail(FIO_ERR_IO); return; }
    for (unsigned a = 0; a < 16384; a++)
        Xil_Out32(R_ROMW, (a << 8) | g_fbuf[a]);             // idx=0 (bit22=0) = primary $C000 bank
    poke(FIO_SIZE_LO, 0x00); poke(FIO_SIZE_HI, 0x40);        // 16384
    xil_printf("[fio] LOADRUNTIME %s -> $C000 primary bank (16384 bytes)\r\n", e.name);
    fio_ok();
}

// FILE_XLOAD: stream a file from the mounted image straight into XRAM (PS DDR3).
// The Z-machine runtime loads its story this way (XRAM dest in FIO_GSPACE/GADDR,
// optional length in FIO_GLEN; 0 = whole file).
static void fio_xload(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    unsigned xaddr  = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
    unsigned reqlen = peek(FIO_GLEN_LO)  | (peek(FIO_GLEN_HI) << 8);
    unsigned total  = e.size;
    if (reqlen > 0 && reqlen < total) total = reqlen;

    unsigned done = 0;
    while (done < total) {
        unsigned chunk = total - done;
        if (chunk > sizeof g_fbuf) chunk = sizeof g_fbuf;
        int n = ndi_read(img, idx, done, g_fbuf, chunk);
        if (n <= 0) break;
        UINTPTR ddr = XRAM_DDR_BASE + xaddr + done;
        memcpy((void *)ddr, g_fbuf, (unsigned)n);
        Xil_DCacheFlushRange(ddr, (unsigned)n);            // visible to PL HP0
        done += (unsigned)n;
        if ((unsigned)n < chunk) break;
    }
    poke(FIO_SIZE_LO, done & 0xFF); poke(FIO_SIZE_HI, (done >> 8) & 0xFF); poke(FIO_SIZE2, (done >> 16) & 0xFF);
    xil_printf("[fio] XLOAD %s -> XRAM $%06x (%u bytes)\r\n", e.name, xaddr, done);
    fio_ok();
}

// FILE_XPAGE: stream a SLICE (file offset + length) of a mounted file into XRAM
// or CPU RAM. The Z-machine runtime pages large stories into XRAM this way.
// File offset = FIO_SRC(L/H) | FIO_END_LO<<16 (24-bit); length = FIO_GLEN;
// target = FIO_DIRTYPE (0=XRAM @ FIO_GSPACE/GADDR, 1=CPU RAM @ FIO_GADDR).
static void fio_xpage(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);

    unsigned char target = peek(FIO_DIRTYPE);
    unsigned reqlen = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    unsigned foff   = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8) | (peek(FIO_END_LO) << 16);
    if (reqlen == 0 || foff >= e.size) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = reqlen;
    if (len > e.size - foff) len = e.size - foff;

    if (target == FIO_PAGE_XRAM) {
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        unsigned done = 0;
        while (done < len) {
            unsigned chunk = len - done;
            if (chunk > sizeof g_fbuf) chunk = sizeof g_fbuf;
            int n = ndi_read(img, idx, foff + done, g_fbuf, chunk);
            if (n <= 0) break;
            UINTPTR ddr = XRAM_DDR_BASE + xaddr + done;
            memcpy((void *)ddr, g_fbuf, (unsigned)n);
            Xil_DCacheFlushRange(ddr, (unsigned)n);
            done += (unsigned)n;
            if ((unsigned)n < chunk) break;
        }
        len = done;
    } else if (target == FIO_PAGE_RAM) {
        unsigned addr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8);
        if (len > sizeof g_fbuf) len = sizeof g_fbuf;
        int n = ndi_read(img, idx, foff, g_fbuf, len);
        if (n < 0) { fio_fail(FIO_ERR_IO); return; }
        for (int i = 0; i < n; i++) poke((addr + i) & 0xFFFF, g_fbuf[i]);
        len = (unsigned)n;
    } else {
        fio_fail(FIO_ERR_IO); return;          // VGC / gfx4 picture targets = workstream C
    }
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF); poke(FIO_SIZE2, (len >> 16) & 0xFF);
    fio_ok();
}

static void fio_fopen(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    int h = -1;
    for (int i = 0; i < FH_MAX; i++) if (!g_fh[i].used) { h = i; break; }
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    g_fh[h].used = 1; g_fh[h].idx = idx; g_fh[h].pos = 0; g_fh[h].size = e.size;
    poke(FIO_SRC_LO, (h + 1) & 0xFF); poke(FIO_SRC_HI, 0);   // handle = slot+1
    poke(FIO_SIZE_LO, e.size & 0xFF); poke(FIO_SIZE_HI, (e.size >> 8) & 0xFF); poke(FIO_SIZE2, (e.size >> 16) & 0xFF);
    xil_printf("[fio] FOPEN %s -> h%d (%u bytes)\r\n", e.name, h + 1, (unsigned)e.size);
    fio_ok();
}

static int fh_from_regs(void) {
    int handle = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8);
    int h = handle - 1;
    if (h < 0 || h >= FH_MAX || !g_fh[h].used) return -1;
    return h;
}

static uint8_t fio_name_type(const char *name) {
    const char *d = strrchr(name, '.');
    if (!d) return DT_BIN;
    if (!strncasecmp(d, ".bas", 5)) return DT_BAS;
    if (!strncasecmp(d, ".sid", 5)) return DT_SID;
    if (!strncasecmp(d, ".mid", 5)) return DT_MID;
    if (!strncasecmp(d, ".gfx", 5)) return DT_GFX;
    if (!strncasecmp(d, ".4th", 5)) return DT_FORTH;
    return DT_BIN;
}

// FCREATE: open a write handle. Data is staged in g_wbuf during FWRITE and the
// NDI entry is created + filled on FCLOSE (the final size isn't known up front).
static void fio_fcreate(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    if (g_wbuf_handle >= 0) { fio_fail(FIO_ERR_IO); return; }   // one writer at a time
    int h = -1;
    for (int i = 0; i < FH_MAX; i++) if (!g_fh[i].used) { h = i; break; }
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    g_fh[h].used = 1; g_fh[h].idx = -1; g_fh[h].pos = 0; g_fh[h].size = 0;
    g_fh[h].writing = 1; g_fh[h].wtype = fio_name_type(name); g_fh[h].wparent = NDI_ROOT;
    strncpy(g_fh[h].wname, name, sizeof g_fh[h].wname - 1);
    g_fh[h].wname[sizeof g_fh[h].wname - 1] = 0;
    g_wbuf_handle = h;
    poke(FIO_SRC_LO, (h + 1) & 0xFF); poke(FIO_SRC_HI, 0);      // handle = slot+1
    poke(FIO_SIZE_LO, 0); poke(FIO_SIZE_HI, 0); poke(FIO_SIZE2, 0);
    xil_printf("[fio] FCREATE %s -> h%d (write)\r\n", name, h + 1);
    fio_ok();
}

// FWRITE: append FIO_GLEN bytes from CPU RAM (FIO_END) or XRAM (FIO_GADDR/GSPACE)
// to the write handle's staging buffer.
static void fio_fwrite(void) {
    int h = fh_from_regs();
    if (h < 0 || !g_fh[h].writing) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    if (len == 0) { poke(FIO_SIZE_LO, 0); poke(FIO_SIZE_HI, 0); poke(FIO_SIZE2, 0); fio_ok(); return; }
    if (g_fh[h].pos > WBUF_BYTES || len > WBUF_BYTES - g_fh[h].pos) { fio_fail(FIO_ERR_IO); return; }
    unsigned char target = peek(FIO_DIRTYPE);
    if ((target & FIO_TARGET_MASK) == FIO_TARGET_XRAM) {
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        UINTPTR ddr = XRAM_DDR_BASE + xaddr;
        Xil_DCacheInvalidateRange(ddr, len);                   // fetch fresh PL-written XRAM
        memcpy(g_wbuf + g_fh[h].pos, (void *)ddr, len);
    } else {
        unsigned src = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
        for (unsigned i = 0; i < len; i++) g_wbuf[g_fh[h].pos + i] = peek((src + i) & 0xFFFF);
    }
    g_fh[h].pos += len;
    if (g_fh[h].pos > g_fh[h].size) g_fh[h].size = g_fh[h].pos;
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF); poke(FIO_SIZE2, (len >> 16) & 0xFF);
    fio_ok();
}

// Commit a write handle's staged bytes into the mounted .ndi (replace if the
// name already exists). Returns 0 on success.
static int fio_commit_write(int h) {
    ndi_t *img = boot_image();
    if (!img) return -1;
    uint32_t total = g_fh[h].size;
    ndi_delete(img, g_fh[h].wname, g_fh[h].wparent);           // overwrite existing slot
    int idx = ndi_create(img, g_fh[h].wname, g_fh[h].wtype, g_fh[h].wparent, total ? total : 1);
    if (idx < 0) return -1;
    uint32_t off = 0;
    while (off < total) {
        uint32_t chunk = total - off; if (chunk > 16384) chunk = 16384;
        if (ndi_write(img, idx, off, g_wbuf + off, chunk) != 0) {
            ndi_delete(img, g_fh[h].wname, g_fh[h].wparent); return -1;
        }
        off += chunk;
    }
    ndi_zero_tail(img, idx);
    ndi_flush(img);
    return 0;
}

static void fio_fclose(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    if (g_fh[h].writing) {
        int rc = fio_commit_write(h);
        g_fh[h].used = 0; g_fh[h].writing = 0; g_wbuf_handle = -1;
        if (rc == 0) { xil_printf("[fio] FCLOSE(save) %s (%u bytes)\r\n", g_fh[h].wname, (unsigned)g_fh[h].size); fio_ok(); }
        else         { xil_printf("[fio] FCLOSE(save) %s FAILED\r\n", g_fh[h].wname); fio_fail(FIO_ERR_IO); }
        return;
    }
    g_fh[h].used = 0;
    fio_ok();
}

static void fio_fread(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    unsigned want = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    if (want == 0 || want > sizeof g_fbuf) want = sizeof g_fbuf;
    int n = ndi_read(img, g_fh[h].idx, g_fh[h].pos, g_fbuf, want);
    if (n < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned char dirtype = peek(FIO_DIRTYPE);
    if ((dirtype & FIO_TARGET_MASK) == FIO_TARGET_XRAM) {
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        UINTPTR ddr = XRAM_DDR_BASE + xaddr;
        memcpy((void *)ddr, g_fbuf, (unsigned)n);
        Xil_DCacheFlushRange(ddr, (unsigned)n);              // make visible to PL HP0
    } else {
        unsigned dst = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
        for (int i = 0; i < n; i++) poke((dst + i) & 0xFFFF, g_fbuf[i]);
    }
    g_fh[h].pos += (unsigned)n;
    poke(FIO_SIZE_LO, n & 0xFF); poke(FIO_SIZE_HI, (n >> 8) & 0xFF); poke(FIO_SIZE2, (n >> 16) & 0xFF);
    fio_ok();
}

static void fio_fseek(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned off = peek(FIO_SIZE_LO) | (peek(FIO_SIZE_HI) << 8) | (peek(FIO_SIZE2) << 16);
    if (off > g_fh[h].size) off = g_fh[h].size;
    g_fh[h].pos = off;
    fio_ok();
}

static void fio_ftell(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned p = g_fh[h].pos;
    poke(FIO_SIZE_LO, p & 0xFF); poke(FIO_SIZE_HI, (p >> 8) & 0xFF); poke(FIO_SIZE2, (p >> 16) & 0xFF);
    fio_ok();
}

static void fio_fsize(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned s = g_fh[h].size;
    poke(FIO_SIZE_LO, s & 0xFF); poke(FIO_SIZE_HI, (s >> 8) & 0xFF); poke(FIO_SIZE2, (s >> 16) & 0xFF);
    fio_ok();
}

// ---- audio FIO commands (6502 drives the PS wavetable engine) ---------------
// Build "0:/<name>" from the FIO name buffer, appending ext if the name has none.
static void audio_sd_path(const char *name, const char *ext, char *path, int sz) {
    if (strrchr(name, '.')) snprintf(path, sz, "0:/%s", name);
    else                    snprintf(path, sz, "0:/%s.%s", name, ext);
}
static void fio_midplay(void) {
    char name[80], path[96];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    audio_sd_path(name, "MID", path, sizeof path);
    if (audio_play_midi(path) == 0) fio_ok(); else fio_fail(FIO_ERR_NOTFOUND);
}
static void fio_sfload(void) {
    char name[80], path[96];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    audio_sd_path(name, "SF2", path, sizeof path);
    if (audio_load_soundfont(path) == 0) fio_ok(); else fio_fail(FIO_ERR_IO);
}
static void fio_volume(void) {
    unsigned v = peek(FIO_SRC_LO);                 // 0-255 -> 0.0..4.0 linear gain
    audio_set_gain((float)v * 4.0f / 255.0f);
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

// Audio is driven by a high-priority TTC0 interrupt (1 kHz) rather than the
// cooperative main loop, so it preempts net/FIO/USB/file-I/O -- the HDMI FIFO
// can't underrun no matter what the PS is busy with. A hardware IRQ is higher
// priority than any task could be.
static XTtcPs g_ttc;

// Driver status callback: the TTC driver's interrupt handler reads+clears the
// status (the correct, race-free clear) and calls this with the event bits.
static void audio_tick_cb(void *ref, u32 status) {
    (void)ref;
    if (status & XTTCPS_IXR_INTERVAL_MASK) audio_service();
}

// Set up TTC0 as a 1 kHz interval timer whose ISR feeds the audio FIFO. Call
// AFTER net_init() -- the lwIP adapter initializes the shared GIC + enables IRQs.
static void audio_timer_init(void) {
    XTtcPs_Config *cfg = XTtcPs_LookupConfig(XPAR_XTTCPS_0_BASEADDR);
    if (!cfg || XTtcPs_CfgInitialize(&g_ttc, cfg, cfg->BaseAddress) != XST_SUCCESS) {
        xil_printf("[audio] TTC0 init FAILED -- audio IRQ disabled\r\n");
        return;
    }
    XTtcPs_Stop(&g_ttc);
    XTtcPs_SetOptions(&g_ttc, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
    XInterval interval = 0; u8 prescaler = 0;
    XTtcPs_CalcIntervalFromFreq(&g_ttc, 1000, &interval, &prescaler);   // 1 kHz
    xil_printf("[audio] TTC0 interval=%u prescaler=%u id=%u\r\n",
               (unsigned)interval, (unsigned)prescaler, (unsigned)cfg->IntrId[0]);
    if (interval == 0) { xil_printf("[audio] TTC0 bad interval -- audio IRQ off\r\n"); return; }
    XTtcPs_SetInterval(&g_ttc, interval);
    XTtcPs_SetPrescaler(&g_ttc, prescaler);
    // Driver handler clears the status race-free; our callback does the work.
    XTtcPs_SetStatusHandler(&g_ttc, &g_ttc, (XTtcPs_StatusHandler)audio_tick_cb);
    // Use the SDT interrupt wrapper (same shared GIC the lwIP adapter set up; it
    // decodes cfg->IntrId[0] and connects/enables the TTC0 interrupt for us).
    if (XSetupInterruptSystem(&g_ttc, (void *)XTtcPs_InterruptHandler,
                              cfg->IntrId[0], cfg->IntrParent,
                              XINTERRUPT_DEFAULT_PRIORITY) != XST_SUCCESS) {
        xil_printf("[audio] TTC0 XSetupInterruptSystem FAILED -- audio IRQ off\r\n");
        return;
    }
    XTtcPs_EnableInterrupts(&g_ttc, XTTCPS_IXR_INTERVAL_MASK);
    XTtcPs_Start(&g_ttc);
    xil_printf("[audio] TTC0 1kHz audio IRQ started\r\n");
}

int main(void) {
    static FATFS fs;
    Xil_DCacheEnable();
    xil_printf("\r\n[NovaVM PS FIO host] starting\r\n");

    cpu_hold(1);                                // keep the 6502 in reset

    // PS-load the (patched) BASIC ROM into basic_rom (dbg_rom idx=0) while the CPU
    // is held, so basic.asm changes take effect without a bitstream rebuild. The
    // bitstream's BRAM init is the fallback if this is ever removed.
    for (unsigned a = 0; a < 16384; a++)
        Xil_Out32(R_ROMW, (a << 8) | EHBASIC_ROM[a]);   // idx=0 (bit22=0) = basic_rom
    xil_printf("[fio] basic_rom loaded from PS (16384 bytes)\r\n");

    if (f_mount(&fs, "0:/", 1) != FR_OK) {
        xil_printf("[fio] WARNING: SD mount failed (module loads will NAK)\r\n");
        mgmt_set_sd(0);
    } else {
        xil_printf("[fio] microSD mounted\r\n");
        mgmt_set_sd(1);
        drives_load();                          // restore mounted .ndi slots
    }

    // Stream the 6581 filter curve into XRAM $080000 (DDR) for the reDIP-SID
    // sid_curve_reader, exactly as NovaHost does on the ULX3S. 4096x16 = 8 KB.
    // (8580's filter is linear -- no curve. Non-fatal if missing.)
    {
        FIL cf;
        if (f_open(&cf, "0:/F6581.BIN", FA_READ) == FR_OK) {
            UINT br = 0;
            f_read(&cf, (void *)(XRAM_DDR_BASE + 0x80000u), 8192, &br);
            f_close(&cf);
            Xil_DCacheFlushRange(XRAM_DDR_BASE + 0x80000u, 8192);
            xil_printf("[sid] 6581 filter curve loaded (%u bytes)\r\n", (unsigned)br);
        } else {
            xil_printf("[sid] F6581.BIN not found -- 6581 filter inaccurate\r\n");
        }
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
    mgmt_init();                                // NovaHost 6504 management server (after lwip_init)
    debug_init();                               // NovaHost 6503 debug server (after lwip_init)
    usb_init();                                 // bring up PS USB host (HID keyboard)

    audio_init();                               // reset the software SIDs + mix
    audio_timer_init();                         // start the 1 kHz audio IRQ (after the GIC is up)

    for (;;) {
        // --- FIO host service ---
        if (fio_pending()) {
            fio_clear();
            unsigned char cmd = peek(FIO_CMD);
            // log + mark busy (skip the high-frequency story-paging command to keep serial fast)
            if (cmd != 0) { if (cmd != FIO_CMD_XPAGE) xil_printf("[fio] cmd=0x%02x\r\n", cmd); poke(FIO_STATUS, 0); }
            if      (cmd == FIO_CMD_LOAD)    fio_load();
            else if (cmd == FIO_CMD_SAVE)    fio_save();
            else if (cmd == FIO_CMD_DIROPEN) fio_diropen();
            else if (cmd == FIO_CMD_DIRREAD) fio_dirread();
            else if (cmd == FIO_CMD_DELETE)  fio_delete();
            else if (cmd == FIO_CMD_XLOAD)   fio_xload();
            else if (cmd == FIO_CMD_XPAGE)   fio_xpage();
            else if (cmd == FIO_CMD_LOADRUNTIME) fio_load_runtime();
            else if (cmd == FIO_CMD_FOPEN)   fio_fopen();
            else if (cmd == FIO_CMD_FCREATE) fio_fcreate();
            else if (cmd == FIO_CMD_FWRITE)  fio_fwrite();
            else if (cmd == FIO_CMD_FCLOSE)  fio_fclose();
            else if (cmd == FIO_CMD_FREAD)   fio_fread();
            else if (cmd == FIO_CMD_FSEEK)   fio_fseek();
            else if (cmd == FIO_CMD_FTELL)   fio_ftell();
            else if (cmd == FIO_CMD_FSIZE)   fio_fsize();
            else if (cmd == FIO_CMD_MIDPLAY) fio_midplay();
            else if (cmd == FIO_CMD_MIDSTOP) { audio_stop(); fio_ok(); }
            else if (cmd == FIO_CMD_SFLOAD)  fio_sfload();
            else if (cmd == FIO_CMD_VOLUME)  fio_volume();
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
            if (ch == 0x1C || ch == '~') { screen_dump(); }  // console trigger (if RX ever works)
            else {
                if (ch == '\n') ch = '\r';
                else if (ch == 0x7F) ch = 0x08;
                key_send((unsigned char)ch);
            }
        }
        // --- PS Ethernet: service lwIP (RX + TCP timers + DHCP) ---
        net_poll();
        // --- PS USB host: detect/poll the HID keyboard ---
        usb_poll();
        // (audio is serviced by the high-priority TTC0 audio_isr, which preempts
        //  this loop ~1000x/s — nothing to do for audio here)
    }
}
