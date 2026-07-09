/* novavm.h — shared core API for the NovaVM Linux host modules.
 *
 * The main translation unit (novavm.c) owns g_reg (the mmap'd fio_bridge window)
 * and the boot/service loop. Subsystem modules (naudio.c, nservers.c, nkbd.c,
 * nfio.c) include this header to reach the PL register surface + the FIO mailbox
 * via the same poke/peek/wr/rd primitives. Mirrors ps_fio's register map. */
#ifndef NOVAVM_H
#define NOVAVM_H
#include <stdint.h>

/* ---- fio_bridge registers (M_AXI_GP0 @ 0x40000000) ---- */
#define FIO_BASE       0x40000000u
#define R_POKE         0x00u   /* W {addr[15:0],data[7:0]} -> 1-cyc 6502-RAM write */
#define R_PEEK_ADDR    0x04u   /* W addr[15:0] (held)                              */
#define R_PEEK_DATA    0x08u   /* R 6502-RAM byte at peek addr                     */
#define R_KEY          0x0Cu   /* W [7:0] -> push key into VGC key FIFO            */
#define R_CTRL         0x10u   /* RW bit0=cpu_reset bit1=system_reset              */
#define R_STATUS       0x14u   /* R {key_ready, fio_event_sticky}; W bit0 clears   */
#define R_CPU_PC       0x18u   /* R 6502 PC (debug)                               */
#define R_ROMW         0x24u   /* W {idx,addr[13:0],data[7:0]} -> ROM/ext_rom bank */
#define R_VMEM_ADDR    0x28u   /* W {space[2:0],addr[16:0]} VGC vmem read addr     */
#define R_VMEM_DATA    0x2Cu   /* R VGC vmem byte / W stream byte into VGC memory  */
#define R_AUDIO        0x30u   /* W [7:0] -> push 1 PCM byte to HDMI audio FIFO    */
#define R_AUDIO_SPACE  0x34u   /* R [15:0] -> free bytes in HDMI audio FIFO        */
#define R_AUDIO_EVT    0x38u   /* R {valid,index[7:0],data[7:0]} 6502 SID/WTS write, pops FIFO */
#define R_SID_VOL      0x3Cu   /* W [7:0]=mix level [8]=stereo                     */
/* --- OSD config overlay (added with the OSD-compositor bitstream; see
 *     OSD_CONFIG_SPEC.md). Until that bitstream lands these offsets are
 *     unimplemented in the PL and nosd.c gates them behind g_osd_hw. --- */
#define R_BUTTONS      0x40u   /* R {press_edge[3:0],level[3:0]} front-panel btns  */
#define R_OSD_CTRL     0x44u   /* RW bit0=osd_enable bit1=system_pause             */
#define R_OSD_FB       0x48u   /* W {transp,bg[3:0],fg[3:0],char[7:0],addr[12:0]}  */
#define R_OSD_FILL     0x4Cu   /* W {transp,bg[3:0],fg[3:0],char[7:0]} fill grid   */
#define CTRL_CPU_RESET 0x1u
#define CTRL_SYS_RESET 0x2u
#define VGC_SPACE_CHAR 1u
#define VGC_SPACE_SPRITE 4u

/* ---- FIO mailbox in 6502 RAM ($B9A0+) ---- */
#define FIO_CMD        0xB9A0
#define FIO_STATUS     0xB9A1
#define FIO_ERRCODE    0xB9A2
#define FIO_NAMELEN    0xB9A3
#define FIO_SRC_LO     0xB9A4
#define FIO_SRC_HI     0xB9A5
#define FIO_END_LO     0xB9A6
#define FIO_END_HI     0xB9A7
#define FIO_SIZE_LO    0xB9A8
#define FIO_SIZE_HI    0xB9A9
#define FIO_DIRTYPE    0xB9AF
#define FIO_NAME       0xB9B0
#define AUTOBOOT_SKIP_ADDR 0xB9F0

#define FIO_OK  0x02
#define FIO_ERR 0x03
#define FIO_ERR_NOTFOUND 1
#define FIO_ERR_IO       2
#define FIO_ERR_EOD      3

#define NOVA_FS_ROOT  "/data/nova"

/* ---- PS-DDR XRAM shelf: the 6502's 512KB expansion RAM (DT reserved-memory) ----
 * The PL axi_xram master maps the 6502 XMC window into PS DDR at XRAM_DDR_BASE;
 * novavm mmaps the same region via /dev/mem to stream runtimes/modules/story
 * pages in (FIO XLOAD/XPAGE/LOAD_RUNTIME). g_xram is NULL if the DT reserve is
 * absent (older image) -> the XRAM commands degrade to FIO_ERR rather than
 * corrupting kernel RAM. */
#define XRAM_DDR_BASE 0x10000000u
#define XRAM_BYTES    0x100000u   /* 1 MB reserved: 512KB shelf + +0x80000 config + headroom */
extern volatile uint8_t *g_xram;

/* ---- core register access (g_reg owned by novavm.c) ---- */
extern volatile uint32_t *g_reg;

/* Serialise R_VMEM_ADDR/R_VMEM_DATA (and R_PEEK) across threads. Defined in
 * nservers.c; used by nmouse.c's shape loader so its 384-byte VMEM burst can't
 * be interleaved by a screen dump/peek (which corrupts the shared addr latch). */
void vgc_bridge_lock(void);
void vgc_bridge_unlock(void);
static inline void     wr(unsigned o, uint32_t v) { g_reg[o >> 2] = v; }
static inline uint32_t rd(unsigned o)             { return g_reg[o >> 2]; }
static inline void     poke(unsigned a, unsigned char d) { wr(R_POKE, ((a & 0xFFFFu) << 8) | d); }
/* peek() locks the bridge: R_PEEK_ADDR is a held latch, so a concurrent
 * peek/vmem from another thread landing between the addr write and the data
 * read returns the WRONG byte. Recursive lock (nservers.c) => safe to nest. */
static inline unsigned char peek(unsigned a)      { unsigned char v; vgc_bridge_lock(); wr(R_PEEK_ADDR, a & 0xFFFFu); v = (unsigned char)rd(R_PEEK_DATA); vgc_bridge_unlock(); return v; }
static inline void     cpu_hold(int h)            { wr(R_CTRL, h ? CTRL_CPU_RESET : 0u); }
static inline void     fio_ok(void)               { poke(FIO_ERRCODE, 0); poke(FIO_STATUS, FIO_OK); poke(FIO_CMD, 0); }
static inline void     fio_fail(int c)            { poke(FIO_ERRCODE, c); poke(FIO_STATUS, FIO_ERR); poke(FIO_CMD, 0); }

void host_reboot_vm(void);

#endif /* NOVAVM_H */
