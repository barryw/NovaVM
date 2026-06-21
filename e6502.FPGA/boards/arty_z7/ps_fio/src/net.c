// net.c — PS Gigabit Ethernet (GEM0) bring-up for the NovaVM FIO host.
//
// lwIP RAW API (NO_SYS), DHCP. A TCP server on port 6502 receives a file upload
// and streams it to the microSD (FatFs). Intended for pushing .ndi disk images
// onto the card from a PC, e.g.:  nc <arty-ip> 6502 < disk.ndi
//
// PS-side only: GEM0 + USB0 are already enabled in the PS7 block design, so this
// needs no bitstream change. Call net_init() once (after the SD is mounted) and
// net_poll() frequently from the main loop.

#include <stdio.h>
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xiltimer.h"
#include "ff.h"

// lwIP time source (ms). Required with NO_SYS_NO_TIMERS=0; the Xilinx port omits
// it when timers are off, so provide it from the A9 64-bit global timer.
// extern "C": the whole firmware now compiles as C++ (for the reused ndi_image.cpp),
// but lwIP declares sys_now() with C linkage.
#ifdef __cplusplus
extern "C"
#endif
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
#include "xil_cache.h"

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

    // RX FIX: clear NVLANDISC in NET_CFG. GEM register offsets are NET_CTRL=0x0 and
    // NET_CFG=0x4 (xemacps_hw.h). NET_CFG bit2 = XEMACPS_NWCFG_NVLANDISC_MASK
    // ("receive only VLAN frames") makes the MAC discard every non-VLAN frame -- all
    // DHCP/ARP/normal traffic -- so RX reads zero while TX is fine. Speed detection +
    // the RTL8211F RGMII-id delays are handled in the BSP driver's
    // get_Realtek_phy_speed (patched for the F by vitis/build_ps_fio.py), and the lwIP
    // adapter already enables RX/TX in NET_CTRL -- so all that's needed here is to make
    // sure NVLANDISC is not set.
    {
        u32_t base = XPAR_XEMACPS_0_BASEADDR;
        XEmacPs_WriteReg(base, 0x4, XEmacPs_ReadReg(base, 0x4) & ~0x00000004u);
        xil_printf("[net] NET_CTRL=%08x NET_CFG=%08x (NVLANDISC cleared)\r\n",
                   (unsigned)XEmacPs_ReadReg(base, 0x0), (unsigned)XEmacPs_ReadReg(base, 0x4));
    }

    dhcp_start(&nif);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb && tcp_bind(pcb, IP_ANY_TYPE, 6502) == ERR_OK) {
        pcb = tcp_listen(pcb);
        tcp_accept(pcb, on_accept);
    }
    xil_printf("[net] lwIP up; DHCP requesting... (TCP 6502 -> %s)\r\n", UPLOAD_PATH);
}

// Current DHCP IPv4 address as text ("0.0.0.0" before DHCP completes). Used by
// the 6504 management server (mgmt.c) for GetStatus.
void net_ip(char *out, int n)
{
    const ip4_addr_t *a = netif_ip4_addr(&nif);
    snprintf(out, n, "%d.%d.%d.%d",
             ip4_addr1(a), ip4_addr2(a), ip4_addr3(a), ip4_addr4(a));
}

void net_poll(void)
{
    eth_link_detect(&nif);     // canonical Xilinx-lwIP app call: completes link-up bring-up (arms RX)
    xemacif_input(&nif);
    sys_check_timeouts();
    if (!dhcp_done && dhcp_supplied_address(&nif)) {
        dhcp_done = 1;
        const ip4_addr_t *a = netif_ip4_addr(&nif);
        xil_printf("[net] DHCP IP: %d.%d.%d.%d  (upload: nc <ip> 6502 < disk.ndi)\r\n",
                   ip4_addr1(a), ip4_addr2(a), ip4_addr3(a), ip4_addr4(a));
    }
    // Light heartbeat: link + DHCP state + RX/TX frame counts. (The full GEM register
    // dump used during bring-up is gone now that RX works.)
    static unsigned beats = 0;
    if ((++beats % 4000000u) == 0) {
        u32_t base = XPAR_XEMACPS_0_BASEADDR;
        struct dhcp *d = netif_dhcp_data(&nif);
        xil_printf("[net] link=%d dhcpst=%d TX=%u RX=%u\r\n",
                   netif_is_link_up(&nif), d ? d->state : -1,
                   (unsigned)XEmacPs_ReadReg(base, 0x108), (unsigned)XEmacPs_ReadReg(base, 0x158));
    }
}
