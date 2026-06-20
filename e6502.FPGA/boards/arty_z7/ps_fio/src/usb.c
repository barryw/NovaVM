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

// A USB device on the bus. speed: 0=FS, 1=LS, 2=HS (matches EHCI QH EPS encoding).
// For a FS/LS device behind a HS hub, hub_addr/hub_port drive the EHCI split-
// transaction fields so the hub's transaction translator relays for us.
typedef struct { u8 addr; u8 speed; u8 mps; u8 hub_addr; u8 hub_port; } usbdev_t;

static int kbd_ready  = 0;
static u8  kbd_ep     = 0;           // HID interrupt IN endpoint number
static int kbd_mps    = 8;           // its max packet size
static u8  kbd_iface  = 0;           // the HID interface (for SET_PROTOCOL/SET_IDLE)
static int kbd_toggle = 0;           // interrupt IN data toggle
static int kbd_armed  = 0;           // an IN qTD is pending
static u8  prev_keys[8];             // previous boot report (for key-down edges)

// ULPI viewport (Zynq): bit31 WU, bit30 RUN, bit29 RW(1=write), bit27 SS(sync),
// bits23:16 addr, bits15:8 read-data, bits7:0 write-data. Per U-Boot ulpi-viewport.c.
static void ulpi_wakeup(void)
{
    if (Xil_In32(USB_ULPIVP) & (1u << 27)) return;          // already in sync state
    Xil_Out32(USB_ULPIVP, (1u << 31));                      // WU
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 31)); i++) {}
}

static void ulpi_write(u8 addr, u8 data)
{
    ulpi_wakeup();
    Xil_Out32(USB_ULPIVP, (1u << 30) | (1u << 29) | ((u32)addr << 16) | (u32)data);  // data in [7:0]
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 30)); i++) {}
}

static u8 ulpi_read(u8 addr)
{
    ulpi_wakeup();
    Xil_Out32(USB_ULPIVP, (1u << 30) | ((u32)addr << 16));  // RUN, RW=0 (read)
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 30)); i++) {}
    return (u8)((Xil_In32(USB_ULPIVP) >> 8) & 0xFF);        // read data in [15:8]
}

static void udelay(unsigned us) { for (volatile unsigned i = 0; i < us * 60u; i++) {} }

// One control transfer on EP0 of `dev_addr`. setup[8] is the SETUP packet; for an
// IN transfer, up to len bytes land in g_buf. Returns bytes transferred or -1.
static int control_xfer(usbdev_t *d, const u8 *setup, int dir_in, u8 *data, int len)
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
    u32 eps = (u32)d->speed << 12;                       // 0=FS 1=LS 2=HS
    u32 c   = (d->speed != 2) ? (1u << 27) : 0u;         // C bit: non-HS control endpoint
    g_qh.ep_char = (u32)d->addr | (0u << 8) | eps | (1u << 14) | (1u << 15) |
                   ((u32)d->mps << 16) | c;              // addr, EP0, EPS, DTC, H, mps, C
    g_qh.ep_cap = (1u << 30);                            // mult=1
    if (d->speed != 2)                                   // split: FS/LS behind a HS hub
        g_qh.ep_cap |= ((u32)d->hub_addr << 16) | ((u32)d->hub_port << 23);

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

    // Correct order (mine was backwards): reset the controller FIRST -- a CMD_RST
    // re-syncs the ULPI *link*, so any PHY config written before it is lost. Then
    // host mode, THEN configure the PHY (link is up), matching U-Boot ehci-zynq.c.
    // 1) Controller reset + host mode.
    Xil_Out32(USB_USBCMD, CMD_RST);
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_USBCMD) & CMD_RST); i++) {}
    Xil_Out32(USB_USBINTR, 0);
    Xil_Out32(USB_USBMODE, MODE_CM_HOST | 0x10u);   // host + SDIS
    udelay(1000);

    // 2) PHY reset, then host config (U-Boot otg=0x86 with pull-downs + ExtVbusInd).
    ulpi_write(0x05, 0x20);                          // Function Control SET: PHY RESET
    udelay(2000);                                    // reset completes (ulpi_read poll unreliable)
    ulpi_write(0x0A, 0x86);   // OTG Control: DpPulldown | DmPulldown | ExtVbusInd
    ulpi_write(0x04, 0x41);   // Function Control: FULL_SPEED | OpMode normal | SuspendM
    ulpi_write(0x07, 0x00);   // Interface Control: 0
    ulpi_write(0x0B, 0x60);   // OTG Control SET: DrvVbus | DrvVbusExternal (power port)
    udelay(2000);
    // Diagnostic: read the PHY back (vendor id should be 0424 = SMSC USB3320).
    xil_printf("[usb] ULPI vid=%02x%02x otg=%02x func=%02x\r\n",
               ulpi_read(0x01), ulpi_read(0x00), ulpi_read(0x0A), ulpi_read(0x04));

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
    // CONFIGFLAG=1: route all ports to this EHCI controller. U-Boot notes the root
    // hub won't detect new devices until RUN is set + CONFIGFLAG taken.
    Xil_Out32(USB0_BASE + 0x180u, 1u);
    udelay(5000);
    xil_printf("[usb] host started: CAP=%02x MODE=%x PORTSC=%08x STS=%08x(ASS=%d)\r\n",
               (unsigned)cap, (unsigned)Xil_In32(USB_USBMODE), (unsigned)Xil_In32(USB_PORTSC1),
               (unsigned)Xil_In32(USB_USBSTS), (Xil_In32(USB_USBSTS) & STS_ASS) ? 1 : 0);
}

#define PORTSC_PFSC   (1u << 24)   // port force full-speed connect (no HS chirp)

static void port_reset(void)
{
    // NO PFSC: let the EHCI reset run the HS chirp so a HS device (the Apple
    // keyboard's internal hub) is detected at high speed.
    u32 p = Xil_In32(USB_PORTSC1);
    Xil_Out32(USB_PORTSC1, (p & ~PORTSC_PED) | PORTSC_PR);
    udelay(60000);                                   // hold reset >= 50ms
    Xil_Out32(USB_PORTSC1, Xil_In32(USB_PORTSC1) & ~PORTSC_PR);
    udelay(100000);                                  // recovery (>=10ms; be generous)
}

static usbdev_t g_root;   // the root-port device (the Apple keyboard's internal hub)

static void enumerate(void)
{
    u8 s[8], desc[64];

    udelay(150000);                                  // device debounce/power settle

    // Reset until the port enables (PED=1). The reset runs the HS chirp, so PORTSC
    // PSPD reflects the device's real speed afterward.
    u32 p = 0;
    for (int i = 0; i < 6; i++) {
        port_reset();
        p = Xil_In32(USB_PORTSC1);
        if (p & PORTSC_PED) break;
    }
    int speed = (int)((p >> 26) & 3);                // PSPD: 0=FS 1=LS 2=HS
    xil_printf("[usb] post-reset PORTSC=%08x (PED=%d speed=%d)\r\n",
               p, (p & PORTSC_PED) ? 1 : 0, speed);

    // Root-port device at address 0. Default EP0 max packet: 64 for HS, else 8.
    g_root.addr = 0; g_root.speed = (u8)speed; g_root.mps = (speed == 2) ? 64 : 8;
    g_root.hub_addr = 0; g_root.hub_port = 0;

    mk_setup(s, 0x80, 6, 0x0100, 0, 8);              // GET_DESCRIPTOR(device, first 8)
    int r = -1;
    for (int a = 0; a < 5 && r < 8; a++) {
        r = control_xfer(&g_root, s, 1, desc, 8);
        if (r < 8) { udelay(50000); if (a == 2) port_reset(); }
    }
    if (r < 8) { xil_printf("[usb] GET_DESC(dev,8) failed\r\n"); return; }
    g_root.mps = desc[7];                            // real bMaxPacketSize0
    xil_printf("[usb] dev0: mps0=%d class=%d\r\n", g_root.mps, desc[4]);

    mk_setup(s, 0x00, 5, 1, 0, 0);                   // SET_ADDRESS(1)
    if (control_xfer(&g_root, s, 0, 0, 0) < 0) { xil_printf("[usb] SET_ADDRESS failed\r\n"); return; }
    udelay(5000);
    g_root.addr = 1;

    mk_setup(s, 0x80, 6, 0x0100, 0, 18);             // GET_DESCRIPTOR(device, full)
    r = control_xfer(&g_root, s, 1, desc, 18);
    if (r < 18) { xil_printf("[usb] GET_DESC(dev,18)@addr1 failed (r=%d)\r\n", r); return; }
    xil_printf("[usb] dev1: class=%d sub=%d vid=%04x pid=%04x\r\n",
               desc[4], desc[5], (desc[9] << 8) | desc[8], (desc[11] << 8) | desc[10]);

    // --- read the configuration descriptor, find the HID interrupt-IN endpoint ---
    mk_setup(s, 0x80, 6, 0x0200, 0, 9);              // GET_DESCRIPTOR(config, header)
    if (control_xfer(&g_root, s, 1, desc, 9) < 9) { xil_printf("[usb] GET cfg hdr failed\r\n"); return; }
    int total = desc[2] | (desc[3] << 8);
    if (total > 64) total = 64;
    mk_setup(s, 0x80, 6, 0x0200, 0, total);          // GET_DESCRIPTOR(config, full)
    int cl = control_xfer(&g_root, s, 1, desc, total);
    if (cl < 4) { xil_printf("[usb] GET cfg full failed (%d)\r\n", cl); return; }
    u8 cfgval = desc[5];

    int i = 9, cur_class = 0, cur_proto = 0;
    kbd_ep = 0;
    while (i + 2 <= cl) {
        int blen = desc[i], btype = desc[i + 1];
        if (blen < 2) break;
        if (btype == 0x04) {                         // interface descriptor
            cur_class = desc[i + 5]; cur_proto = desc[i + 7];
            if (cur_class == 3 && cur_proto == 1) kbd_iface = desc[i + 2];  // boot keyboard iface
        } else if (btype == 0x05 && cur_class == 3) { // endpoint under an HID interface
            u8 epa = desc[i + 2], attr = desc[i + 3];
            if ((epa & 0x80) && (attr & 3) == 3) {   // interrupt IN
                if (kbd_ep == 0 || cur_proto == 1) { // prefer the keyboard (protocol 1)
                    kbd_ep  = epa & 0x0F;
                    kbd_mps = desc[i + 4] | (desc[i + 5] << 8);
                }
            }
        }
        i += blen;
    }
    if (kbd_ep == 0) { xil_printf("[usb] no HID interrupt-IN endpoint\r\n"); return; }
    xil_printf("[usb] HID kbd: iface=%d ep=%d mps=%d cfg=%d\r\n", kbd_iface, kbd_ep, kbd_mps, cfgval);

    mk_setup(s, 0x00, 9, cfgval, 0, 0);              // SET_CONFIGURATION
    control_xfer(&g_root, s, 0, 0, 0); udelay(5000);
    mk_setup(s, 0x21, 0x0B, 0, kbd_iface, 0);        // SET_PROTOCOL(boot=0)
    control_xfer(&g_root, s, 0, 0, 0);
    mk_setup(s, 0x21, 0x0A, 0, kbd_iface, 0);        // SET_IDLE(0)
    control_xfer(&g_root, s, 0, 0, 0);

    for (int k = 0; k < 8; k++) prev_keys[k] = 0;
    kbd_toggle = 0; kbd_armed = 0; kbd_ready = 1;
    xil_printf("[usb] keyboard ready -- type!\r\n");
}

// HID boot-keyboard usage -> ASCII. Letters lowercase by default, shift -> uppercase
// (standard keyboard). Shift also selects the symbol set. Enter=CR, Backspace=0x08.
static char hid2ascii(u8 u, int shift)
{
    if (u >= 0x04 && u <= 0x1D) return (char)((shift ? 'A' : 'a') + (u - 0x04));  // letters
    if (u >= 0x1E && u <= 0x26) { static const char sh[] = "!@#$%^&*("; return shift ? sh[u - 0x1E] : (char)('1' + (u - 0x1E)); }
    if (u == 0x27) return shift ? ')' : '0';
    if (u == 0x28) return '\r';                                 // Enter
    if (u == 0x29) return 0x1B;                                 // Esc
    if (u == 0x2A) return 0x08;                                 // Backspace
    if (u == 0x2B) return '\t';                                 // Tab
    if (u == 0x2C) return ' ';                                  // Space
    if (u == 0x2D) return shift ? '_' : '-';
    if (u == 0x2E) return shift ? '+' : '=';
    if (u == 0x2F) return shift ? '{' : '[';
    if (u == 0x30) return shift ? '}' : ']';
    if (u == 0x31) return shift ? '|' : '\\';
    if (u == 0x33) return shift ? ':' : ';';
    if (u == 0x34) return shift ? '"' : '\'';
    if (u == 0x35) return shift ? '~' : '`';
    if (u == 0x36) return shift ? '<' : ',';
    if (u == 0x37) return shift ? '>' : '.';
    if (u == 0x38) return shift ? '?' : '/';
    return 0;
}

// Decode an 8-byte boot report, emitting only newly-pressed keys (key-down edges).
static void kbd_report(u8 *r, int len)
{
    if (len < 3) return;
    int shift = (r[0] & 0x22) ? 1 : 0;                          // L-shift(0x02) | R-shift(0x20)
    int ctrl  = (r[0] & 0x11) ? 1 : 0;                          // L-ctrl(0x01) | R-ctrl(0x10)
    for (int i = 2; i < len && i < 8; i++) {
        u8 k = r[i];
        if (k == 0) continue;
        int held = 0;
        for (int j = 2; j < 8; j++) if (prev_keys[j] == k) { held = 1; break; }
        if (!held) {
            char c;
            if (ctrl && k >= 0x04 && k <= 0x1D) c = (char)(0x01 + (k - 0x04));  // Ctrl-A..Z -> 0x01..0x1A (Ctrl-C=0x03 break)
            else c = hid2ascii(k, shift);
            if (c) kb_emit((unsigned char)c);
        }
    }
    for (int i = 0; i < 8 && i < len; i++) prev_keys[i] = r[i];
}

// Arm one interrupt-IN qTD on the keyboard's endpoint (async schedule, no split for FS).
static void kbd_arm(void)
{
    g_td[0].next = QTD_T; g_td[0].alt = QTD_T;
    g_td[0].bufp[0] = (u32)(UINTPTR)g_buf;
    g_td[0].token = ((u32)kbd_toggle << 31) | ((u32)kbd_mps << 16) | (3u << 10) | (1u << 8) | 0x80u;
    g_qh.cur_qtd = QTD_T;
    g_qh.ov_next = (u32)(UINTPTR)&g_td[0];
    g_qh.ov_alt  = QTD_T;
    g_qh.ov_token = 0;
    u32 eps = (u32)g_root.speed << 12;
    g_qh.ep_char = (u32)g_root.addr | ((u32)kbd_ep << 8) | eps | (1u << 14) | (1u << 15) |
                   ((u32)kbd_mps << 16);
    g_qh.ep_cap = (1u << 30);
    if (g_root.speed != 2) g_qh.ep_cap |= ((u32)g_root.hub_addr << 16) | ((u32)g_root.hub_port << 23);
    kbd_armed = 1;
}

void usb_poll(void)
{
    u32 p = Xil_In32(USB_PORTSC1);
    int ccs = (p & PORTSC_CCS) ? 1 : 0;
    if (ccs != last_ccs) {
        last_ccs = ccs;
        if (ccs) { xil_printf("[usb] connected, enumerating...\r\n"); enumerate(); }
        else     { xil_printf("[usb] disconnected\r\n"); kbd_ready = 0; kbd_armed = 0; }
    }
    if (!kbd_ready) return;
    if (!kbd_armed) { kbd_arm(); return; }
    if (g_td[0].token & 0x80u) return;                 // still active (NAK = no key yet)
    if (!(g_td[0].token & 0x40u)) {                    // completed OK
        int got = kbd_mps - (int)((g_td[0].token >> 16) & 0x7FFF);
        kbd_report(g_buf, got);
        kbd_toggle ^= 1;
    } else {
        kbd_toggle = 0;                                // halted: resync toggle
    }
    kbd_armed = 0;                                     // re-arm next poll
}
