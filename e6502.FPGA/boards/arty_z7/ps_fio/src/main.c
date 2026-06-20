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

    for (;;) {
        // --- FIO host service ---
        if (fio_pending()) {
            fio_clear();
            unsigned char cmd = peek(FIO_CMD);
            if (cmd == FIO_CMD_LOAD_MODULE) {
                int id = peek(FIO_SRC_LO), slot = peek(FIO_END_LO);
                if (load_module(id, slot) == 0) { poke(FIO_ERRCODE,0); poke(FIO_STATUS,FIO_OK); }
                else { poke(FIO_ERRCODE,1); poke(FIO_STATUS,FIO_ERR); }
            } else if (cmd != 0) { poke(FIO_ERRCODE,1); poke(FIO_STATUS,FIO_ERR); }
        }
        // --- console keyboard: forward serial bytes into the VGC key queue ---
        int ch = uart_getc();
        if (ch >= 0) {
            if (ch == '\n') ch = '\r';
            else if (ch == 0x7F) ch = 0x08;
            key_send((unsigned char)ch);
        }
    }
    return 0;
}
