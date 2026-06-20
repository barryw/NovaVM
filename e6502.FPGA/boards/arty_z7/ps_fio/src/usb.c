// usb.c — bare-metal USB host (EHCI/ChipIdea) for the Zynq-7000 PS USB0, reading
// a USB HID boot-protocol keyboard. Xilinx's usbps driver is device-mode only, so
// this drives the controller registers + EHCI schedules directly.
//
//   Phase 1: host mode + ULPI-driven VBUS + device detect          [done]
//   Phase 2: port reset + EHCI control transfers + enumeration     [here]
//   Phase 3: HID interrupt IN polling -> ASCII -> key inject        [here]
//
// Zynq-7000 USB0 @ 0xE0002000, ChipIdea core (does FS/LS on the root port directly,
// no TT/companion needed). Cap regs at +0x100 (CAPLENGTH=0x40) -> op regs at +0x140.

#include "xil_io.h"
#include "xil_cache.h"
#include "xil_mmu.h"
#include "xil_printf.h"

#define USB0_BASE     0xE0002000u
#define USB_CAPLENGTH (USB0_BASE + 0x100u)
#define USB_USBCMD    (USB0_BASE + 0x140u)
#define USB_USBSTS    (USB0_BASE + 0x144u)
#define USB_USBINTR   (USB0_BASE + 0x148u)
#define USB_ASYNCLIST (USB0_BASE + 0x158u)
#define USB_ULPIVP    (USB0_BASE + 0x170u)
#define USB_PORTSC1   (USB0_BASE + 0x184u)
#define USB_OTGSC     (USB0_BASE + 0x1A4u)
#define USB_USBMODE   (USB0_BASE + 0x1A8u)

#define CMD_RS        0x00000001u
#define CMD_RST       0x00000002u
#define CMD_ASE       0x00000020u   // async schedule enable
#define STS_ASS       0x00008000u   // async schedule status
#define MODE_CM_HOST  0x00000003u
#define PORTSC_CCS    (1u << 0)
#define PORTSC_PED    (1u << 2)
#define PORTSC_PR     (1u << 8)
#define PORTSC_PP     (1u << 12)

extern void kb_emit(unsigned char c);   // main.c -> fio_bridge KEY

// ---- EHCI structures (32-byte aligned; we cache-manage them) ----------------
typedef struct {
    volatile u32 next;
    volatile u32 alt;
    volatile u32 token;
    volatile u32 bufp[5];
    u32 _pad[8];                 // 3+5+8 = 16 u32 = 64B (cache line)
} qtd_t __attribute__((aligned(64)));

typedef struct {
    volatile u32 link;
    volatile u32 ep_char;
    volatile u32 ep_cap;
    volatile u32 cur_qtd;
    volatile u32 ov_next;
    volatile u32 ov_alt;
    volatile u32 ov_token;
    volatile u32 ov_bufp[5];
    u32 _pad[4];                 // pad to 64B
} qh_t __attribute__((aligned(64)));

// All EHCI DMA structures live in a dedicated 1MB region marked NON-CACHEABLE at
// init (Xil_SetTlbAttributes), so the controller and CPU always agree -- no cache
// flush/invalidate needed.
__attribute__((aligned(0x100000))) static u8 usb_region[0x100000];
static qh_t  *const g_qh_p = (qh_t  *)(usb_region + 0x000);
static qtd_t *const g_td   = (qtd_t *)(usb_region + 0x100);
static u8    *const g_buf  = (u8    *)(usb_region + 0x400);
#define g_qh (*g_qh_p)

#define FLUSH(p,n) do { (void)(p); (void)(n); } while (0)   // memory is non-cached
#define INVAL(p,n) do { (void)(p); (void)(n); } while (0)
#define QTD_T  0x1u                  // terminate bit

static u8  dev_addr   = 0;
static u8  ep0_mps    = 8;
static u8  kbd_ep     = 0;           // HID interrupt IN endpoint
static int kbd_ready  = 0;

static void ulpi_write(u8 addr, u8 data)
{
    Xil_Out32(USB_ULPIVP, (1u << 30) | (1u << 29) | ((u32)addr << 16) | ((u32)data << 8));
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 30)); i++) {}
}

static u8 ulpi_read(u8 addr)
{
    Xil_Out32(USB_ULPIVP, (1u << 30) | ((u32)addr << 16));   // RUN, RW=0 (read)
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 30)); i++) {}
    return (u8)(Xil_In32(USB_ULPIVP) & 0xFF);
}

static void udelay(unsigned us) { for (volatile unsigned i = 0; i < us * 60u; i++) {} }

// One control transfer on EP0 of `dev_addr`. setup[8] is the SETUP packet; for an
// IN transfer, up to len bytes land in g_buf. Returns bytes transferred or -1.
static int control_xfer(const u8 *setup, int dir_in, u8 *data, int len)
{
    // qTD0 = SETUP, qTD1 = DATA (optional), qTD2 = STATUS
    for (int i = 0; i < 3; i++) { g_td[i].next = QTD_T; g_td[i].alt = QTD_T; g_td[i].token = 0; }

    // SETUP stage
    for (int i = 0; i < 8; i++) g_buf[i] = setup[i];
    g_td[0].bufp[0] = (u32)(UINTPTR)g_buf;
    g_td[0].token = (8u << 16) | (3u << 10) | (2u << 8) | 0x80u;   // 8B, CERR=3, PID=SETUP, Active

    int nstage = 1;
    int data_td = -1;
    if (len > 0) {
        data_td = nstage;
        g_td[0].next = (u32)(UINTPTR)&g_td[1];
        g_td[1].bufp[0] = (u32)(UINTPTR)(g_buf + 16);
        g_td[1].token = (1u << 31) | ((u32)len << 16) | (3u << 10) |
                        ((dir_in ? 1u : 0u) << 8) | 0x80u;          // toggle=1, IN/OUT
        if (!dir_in && data) for (int i = 0; i < len; i++) g_buf[16 + i] = data[i];
        nstage++;
    }
    // STATUS stage (opposite direction, zero length, toggle=1)
    int st = nstage;
    g_td[(data_td < 0) ? 0 : 1].next = (u32)(UINTPTR)&g_td[st];
    g_td[st].token = (1u << 31) | (3u << 10) | ((dir_in ? 0u : 1u) << 8) | 0x80u;

    // Point the QH overlay at the first qTD.
    g_qh.cur_qtd = QTD_T;
    g_qh.ov_next = (u32)(UINTPTR)&g_td[0];
    g_qh.ov_alt  = QTD_T;
    g_qh.ov_token = 0;
    g_qh.ep_char = (u32)dev_addr | (0u << 8) | (0u << 12) | (1u << 14) | (1u << 15) |
                   ((u32)ep0_mps << 16) | (1u << 27);   // addr, EP0, FS, DTC, H, mps, C
    g_qh.ep_cap = (1u << 30);                            // mult=1

    FLUSH(g_buf, sizeof(g_buf));
    FLUSH(g_td, sizeof(g_td));
    FLUSH(&g_qh, sizeof(g_qh));

    // Wait for completion (poll the last qTD's Active bit clear).
    int timeout = 200000;
    while (timeout-- > 0) {
        INVAL(&g_td[st], sizeof(g_td[st]));
        if (!(g_td[st].token & 0x80u)) break;
        udelay(5);
    }
    if (timeout <= 0) {
        INVAL(&g_qh, sizeof(g_qh));
        xil_printf("[usb] ctrl timeout: STS=%08x QHtok=%08x td0=%08x\r\n",
                   (unsigned)Xil_In32(USB_USBSTS), (unsigned)g_qh.ov_token, (unsigned)g_td[0].token);
        return -1;
    }

    // Check for errors (halted / errors) on any stage.
    for (int i = 0; i <= st; i++) {
        INVAL(&g_td[i], sizeof(g_td[i]));
        if (g_td[i].token & 0x40u) { xil_printf("[usb] ctrl halted tok=%08x\r\n", g_td[i].token); return -1; }
    }
    if (dir_in && data && len > 0) {
        INVAL(g_buf, sizeof(g_buf));
        int got = len - (int)((g_td[data_td].token >> 16) & 0x7FFF);  // residue
        for (int i = 0; i < got && i < len; i++) data[i] = g_buf[16 + i];
        return got;
    }
    return 0;
}

static void mk_setup(u8 *s, u8 bmReqType, u8 bReq, u16 wVal, u16 wIdx, u16 wLen)
{
    s[0]=bmReqType; s[1]=bReq; s[2]=wVal&0xFF; s[3]=wVal>>8;
    s[4]=wIdx&0xFF; s[5]=wIdx>>8; s[6]=wLen&0xFF; s[7]=wLen>>8;
}

static int last_ccs = -1;

void usb_init(void)
{
    u32 cap = Xil_In8(USB_CAPLENGTH);
    Xil_SetTlbAttributes((INTPTR)usb_region, NORM_NONCACHE);   // DMA-coherent region

    Xil_Out32(USB_USBCMD, CMD_RST);
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_USBCMD) & CMD_RST); i++) {}
    Xil_Out32(USB_USBINTR, 0);
    Xil_Out32(USB_USBMODE, MODE_CM_HOST | 0x10u);   // host + SDIS (stream disable)
    ulpi_write(0x0A, 0x60);   // drive VBUS (detect works with this)

    // Async schedule: one QH, self-linked, head of reclamation.
    g_qh.link = (u32)(UINTPTR)&g_qh | 0x2u;             // self, typ=QH
    g_qh.ep_char = (1u << 15);                          // H bit
    g_qh.ep_cap = (1u << 30);
    g_qh.cur_qtd = QTD_T; g_qh.ov_next = QTD_T; g_qh.ov_alt = QTD_T; g_qh.ov_token = 0;
    FLUSH(&g_qh, sizeof(g_qh));
    Xil_Out32(USB_ASYNCLIST, (u32)(UINTPTR)&g_qh);

    Xil_Out32(USB_PORTSC1, Xil_In32(USB_PORTSC1) | PORTSC_PP);
    Xil_Out32(USB_USBCMD, CMD_RS | CMD_ASE);           // run + async schedule
    for (volatile int i = 0; i < 1000000 && !(Xil_In32(USB_USBSTS) & STS_ASS); i++) {}
    xil_printf("[usb] host started: CAP=%02x MODE=%x PORTSC=%08x STS=%08x(ASS=%d)\r\n",
               (unsigned)cap, (unsigned)Xil_In32(USB_USBMODE), (unsigned)Xil_In32(USB_PORTSC1),
               (unsigned)Xil_In32(USB_USBSTS), (Xil_In32(USB_USBSTS) & STS_ASS) ? 1 : 0);
}

#define PORTSC_PFSC   (1u << 24)   // port force full-speed connect (no HS chirp)

static void port_reset(void)
{
    u32 p = Xil_In32(USB_PORTSC1);
    Xil_Out32(USB_PORTSC1, (p & ~PORTSC_PED) | PORTSC_PFSC | PORTSC_PR);
    udelay(60000);                                   // hold reset >= 50ms
    Xil_Out32(USB_PORTSC1, Xil_In32(USB_PORTSC1) & ~PORTSC_PR);
    udelay(100000);                                  // recovery (>=10ms; be generous)
}

static void enumerate(void)
{
    u8 s[8], desc[64];

    udelay(150000);                                  // device debounce/power settle
    xil_printf("[usb] PHY before reset: FUNC(0x04)=%02x OTG(0x0A)=%02x IFC(0x07)=%02x\r\n",
               ulpi_read(0x04), ulpi_read(0x0A), ulpi_read(0x07));
    port_reset();
    u32 p = Xil_In32(USB_PORTSC1);
    xil_printf("[usb] post-reset PORTSC=%08x (PED=%d speed=%d) PHY FUNC=%02x OTG=%02x\r\n",
               p, (p & PORTSC_PED) ? 1 : 0, (int)((p >> 26) & 3), ulpi_read(0x04), ulpi_read(0x0A));

    dev_addr = 0; ep0_mps = 8;
    mk_setup(s, 0x80, 6, 0x0100, 0, 8);              // GET_DESCRIPTOR(device, 8)
    int r = -1;
    for (int attempt = 0; attempt < 5 && r < 8; attempt++) {
        r = control_xfer(s, 1, desc, 8);
        if (r < 8) { udelay(50000); if (attempt == 2) { port_reset(); } }
    }
    if (r < 8) { xil_printf("[usb] GET_DESC(dev,8) failed after retries\r\n"); return; }
    ep0_mps = desc[7];
    xil_printf("[usb] device descr: mps0=%d vid=%02x%02x pid=%02x%02x\r\n",
               ep0_mps, desc[9], desc[8], desc[11], desc[10]);
}

void usb_poll(void)
{
    u32 p = Xil_In32(USB_PORTSC1);
    int ccs = (p & PORTSC_CCS) ? 1 : 0;
    if (ccs != last_ccs) {
        last_ccs = ccs;
        if (ccs) { xil_printf("[usb] connected, enumerating...\r\n"); enumerate(); }
        else     { xil_printf("[usb] disconnected\r\n"); kbd_ready = 0; }
    }
}
