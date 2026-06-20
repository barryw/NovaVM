// net.c — PS Gigabit Ethernet (GEM0) bring-up for the NovaVM FIO host.
//
// lwIP RAW API (NO_SYS), DHCP. A TCP server on port 6502 receives a file upload
// and streams it to the microSD (FatFs). Intended for pushing .ndi disk images
// onto the card from a PC, e.g.:  nc <arty-ip> 6502 < disk.ndi
//
// PS-side only: GEM0 + USB0 are already enabled in the PS7 block design, so this
// needs no bitstream change. Call net_init() once (after the SD is mounted) and
// net_poll() frequently from the main loop.

#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xiltimer.h"
#include "ff.h"

// lwIP time source (ms). Required with NO_SYS_NO_TIMERS=0; the Xilinx port omits
// it when timers are off, so provide it from the A9 64-bit global timer.
unsigned int sys_now(void)
{
    XTime t; XTime_GetTime(&t);
    return (unsigned int)(t / (COUNTS_PER_SECOND / 1000U));
}

#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"
#include "netif/xadapter.h"
#include "netif/xemacpsif.h"
#include "xemacps.h"

#ifndef PLATFORM_EMAC_BASEADDR
#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#endif

static struct netif nif;
static XScuGic    gic;
// The lwIP Xilinx adapter (xemacpsif) connects the EMAC interrupt through this.
XScuGic *xInterruptController = &gic;

static const char *UPLOAD_PATH = "0:/UPLOAD.NDI";
static FIL  upf;
static int  up_open  = 0;
static unsigned up_bytes = 0;
static int  dhcp_done = 0;

// ---- TCP upload: stream incoming bytes straight to the SD --------------------
static err_t on_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    (void)arg; (void)err;
    if (p == NULL) {                       // remote closed -> finish the file
        if (up_open) {
            f_close(&upf); up_open = 0;
            xil_printf("[net] upload complete: %u bytes -> %s\r\n", up_bytes, UPLOAD_PATH);
        }
        tcp_close(pcb);
        return ERR_OK;
    }
    if (!up_open) {
        if (f_open(&upf, UPLOAD_PATH, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
            up_open = 1; up_bytes = 0;
            xil_printf("[net] upload start -> %s\r\n", UPLOAD_PATH);
        } else {
            xil_printf("[net] ERROR: cannot open %s\r\n", UPLOAD_PATH);
        }
    }
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        if (up_open) { UINT bw; f_write(&upf, q->payload, q->len, &bw); up_bytes += bw; }
    }
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

static err_t on_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    (void)arg; (void)err;
    xil_printf("[net] client connected\r\n");
    tcp_recv(pcb, on_recv);
    return ERR_OK;
}

void net_init(void)
{
    ip_addr_t ip, mask, gw;
    unsigned char mac[6] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x77 };

    // NOTE: do NOT init the GIC here. The Xilinx lwIP SDT adapter sets up the EMAC
    // interrupt itself via XSetupInterruptSystem (full GIC init/connect/enable). A
    // second app-side XScuGic instance steals the IRQ exception vector -> EMAC IRQ
    // routes to the wrong instance -> no RX -> no DHCP.

    ip_addr_set_zero(&ip); ip_addr_set_zero(&mask); ip_addr_set_zero(&gw);
    lwip_init();
    if (!xemac_add(&nif, &ip, &mask, &gw, mac, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("[net] xemac_add failed (PHY/link?)\r\n");
        return;
    }
    netif_set_default(&nif);
    netif_set_up(&nif);

    // The Xilinx driver forces the MAC to 100Mbps (the RTL8211 autoneg speed-read
    // is broken, PHYSR reads 0), but the PHY still autonegotiates 1000 with a
    // gigabit switch -> speed mismatch -> no data. Stop advertising 1000 so the
    // PHY renegotiates to 100FD, matching the forced MAC.
    {
        struct xemac_s *xemac = (struct xemac_s *)nif.state;
        xemacpsif_s *xs = (xemacpsif_s *)xemac->state;
        u16_t v = 0;

        // RTL8211F (PHY ID2=0xC916): the Xilinx driver treats it as an RTL8211E and
        // never configures the RGMII clock delays -> MAC<->PHY data path is broken
        // (TX frames invalid, error-free TX count stays 0). Enable the PHY's internal
        // RGMII TX + RX delays (page 0xd08: reg0x11 bit8 = TX delay, reg0x15 bit3 = RX
        // delay), i.e. "rgmii-id" mode.
        XEmacPs_PhyWrite(&xs->emacps, 1, 0x1f, 0x0d08);
        XEmacPs_PhyRead (&xs->emacps, 1, 0x11, &v);
        XEmacPs_PhyWrite(&xs->emacps, 1, 0x11, (u16_t)(v | 0x0100));   // TX delay on
        XEmacPs_PhyRead (&xs->emacps, 1, 0x15, &v);
        XEmacPs_PhyWrite(&xs->emacps, 1, 0x15, (u16_t)(v | 0x0008));   // RX delay on
        XEmacPs_PhyWrite(&xs->emacps, 1, 0x1f, 0x0000);                // back to page 0
        xil_printf("[net] RTL8211F: RGMII TX+RX delays enabled\r\n");

        // Drop 1000 advertise so the PHY negotiates 100FD to match the forced MAC.
        u16_t gbcr = 0, bmcr = 0;
        XEmacPs_PhyRead(&xs->emacps, 1, 9, &gbcr);
        XEmacPs_PhyWrite(&xs->emacps, 1, 9, (u16_t)(gbcr & ~0x0300));
        XEmacPs_PhyRead(&xs->emacps, 1, 0, &bmcr);
        XEmacPs_PhyWrite(&xs->emacps, 1, 0, (u16_t)(bmcr | 0x1200));   // autoneg restart
        xil_printf("[net] PHY: 1000 advertise off, autoneg restarted\r\n");
    }

    dhcp_start(&nif);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb && tcp_bind(pcb, IP_ANY_TYPE, 6502) == ERR_OK) {
        pcb = tcp_listen(pcb);
        tcp_accept(pcb, on_accept);
    }
    xil_printf("[net] lwIP up; DHCP requesting... (TCP 6502 -> %s)\r\n", UPLOAD_PATH);
}

void net_poll(void)
{
    xemacif_input(&nif);
    sys_check_timeouts();
    if (!dhcp_done && dhcp_supplied_address(&nif)) {
        dhcp_done = 1;
        const ip4_addr_t *a = netif_ip4_addr(&nif);
        xil_printf("[net] DHCP IP: %d.%d.%d.%d  (upload: nc <ip> 6502 < disk.ndi)\r\n",
                   ip4_addr1(a), ip4_addr2(a), ip4_addr3(a), ip4_addr4(a));
    }
    // Periodic diagnostic: is lwIP's link up + are we receiving anything?
    static unsigned beats = 0;
    if ((++beats % 1500000u) == 0) {
        u32_t base = XPAR_XEMACPS_0_BASEADDR;
        u32_t txf = XEmacPs_ReadReg(base, 0x108);   // error-free frames TX'd
        u32_t rxf = XEmacPs_ReadReg(base, 0x158);   // error-free frames RX'd
        u32_t netcfg = XEmacPs_ReadReg(base, 0x0);  // bit0=100,bit10=gige
        struct xemac_s *xemac = (struct xemac_s *)nif.state;
        xemacpsif_s *xs = (xemacpsif_s *)xemac->state;
        u32_t netctrl = XEmacPs_ReadReg(base, 0x4);  // bit3=TXEN bit2=RXEN
        struct dhcp *d = netif_dhcp_data(&nif);
        int dst = d ? d->state : -1;                  // 0=OFF 6=SELECTING 5=BOUND ...
        (void)xs; (void)netcfg;
        xil_printf("[net] link=%d dhcpst=%d TX=%u RX=%u NETCTRL=%03x t=%u\r\n",
                   netif_is_link_up(&nif), dst, txf, rxf, netctrl, sys_now());
    }
}
