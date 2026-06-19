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
#include "loader_bin.h"

// ---- fio_bridge register map (AXI4-Lite @ 0x40000000) ----------------------
#define FIO_BASE     0x40000000u
#define R_POKE       (FIO_BASE + 0x00)   // W {addr[15:0]<<8 | data[7:0]}
#define R_PEEK_ADDR  (FIO_BASE + 0x04)   // W addr[15:0]
#define R_PEEK_DATA  (FIO_BASE + 0x08)   // R data[7:0]
#define R_KEY        (FIO_BASE + 0x0C)   // W key[7:0]
#define R_CTRL       (FIO_BASE + 0x10)   // RW bit0 = cpu_reset (1=hold)
#define R_STATUS     (FIO_BASE + 0x14)   // R {key_ready,fio_event}; W bit0 clears event

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

static const char *module_path(int id) {
    switch (id) {
        case 1: return "0:/lib/graphics.nmod";
        case 2: return "0:/lib/sound.nmod";
        case 3: return "0:/lib/system.nmod";
        case 4: return "0:/lib/files.nmod";
        case 5: return "0:/lib/memory.nmod";
        case 6: return "0:/lib/net.nmod";
        case 7: return "0:/lib/turtle.nmod";
        case 8: return "0:/lib/editor.nmod";
        default: return 0;
    }
}

// Stream a module image from SD into the XRAM shelf slot (DDR), flush cache.
static int load_module(int id, int slot) {
    const char *fn = module_path(id);
    if (!fn || slot < 0 || slot > 3) return -1;
    FIL f;
    if (f_open(&f, fn, FA_READ) != FR_OK) {
        xil_printf("[fio] module %d: open %s failed\r\n", id, fn);
        return -1;
    }
    UINTPTR ddr = XRAM_DDR_BASE + SHELF_BASE + (unsigned)slot * SHELF_SLOT;
    UINT br = 0;
    FRESULT r = f_read(&f, (void *)ddr, MODULE_BYTES, &br);
    f_close(&f);
    if (r != FR_OK || br < MODULE_BYTES) {
        xil_printf("[fio] module %d: read %u/%u failed\r\n", id, br, MODULE_BYTES);
        return -1;
    }
    Xil_DCacheFlushRange(ddr, MODULE_BYTES);   // make the write visible to PL HP0
    xil_printf("[fio] module %d (%s) -> XRAM slot %d (0x%08x)\r\n", id, fn, slot, (unsigned)ddr);
    return 0;
}

int main(void) {
    static FATFS fs;
    Xil_DCacheEnable();
    xil_printf("\r\n[NovaVM PS FIO host] starting\r\n");

    cpu_hold(1);                                // keep the 6502 in reset

    if (f_mount(&fs, "0:/", 1) != FR_OK)
        xil_printf("[fio] WARNING: SD mount failed (module loads will NAK)\r\n");
    else
        xil_printf("[fio] microSD mounted\r\n");

    // Stage the resident lib_call loader into CPU RAM $0320.
    for (unsigned i = 0; i < sizeof(LOADER_BIN); i++)
        poke(0x0320 + i, LOADER_BIN[i]);
    xil_printf("[fio] resident loader staged @ $0320 (%u bytes)\r\n", (unsigned)sizeof(LOADER_BIN));

    cpu_hold(0);                                // release the 6502 -> boots to READY
    xil_printf("[fio] 6502 released; servicing FIO events\r\n");

    for (;;) {
        if (!fio_pending()) continue;
        fio_clear();
        unsigned char cmd = peek(FIO_CMD);
        if (cmd == FIO_CMD_LOAD_MODULE) {
            int id   = peek(FIO_SRC_LO);
            int slot = peek(FIO_END_LO);
            if (load_module(id, slot) == 0) { poke(FIO_ERRCODE, 0); poke(FIO_STATUS, FIO_OK); }
            else                            { poke(FIO_ERRCODE, 1); poke(FIO_STATUS, FIO_ERR); }
        } else if (cmd != 0) {
            // Unimplemented (file I/O etc.) — NAK so the runtime fails gracefully.
            poke(FIO_ERRCODE, 1);
            poke(FIO_STATUS, FIO_ERR);
        }
    }
    return 0;
}
