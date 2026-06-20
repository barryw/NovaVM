// usb.c — bare-metal USB host (EHCI/ChipIdea) for the Zynq-7000 PS USB0, to read
// a USB HID boot-protocol keyboard. Xilinx's usbps driver is device-mode only, so
// this drives the controller registers directly.
//
// PHASE 1 (this file, for now): controller -> host mode, power the port, and
// detect a device connecting (PORTSC.CCS) + its speed. This is the feasibility
// gate (VBUS/PHY/host-mode) before the full enumeration + HID stack.
//
// Zynq-7000 USB0 @ 0xE0002000. ChipIdea core: capability regs at +0x100
// (CAPLENGTH=0x40), so the EHCI operational regs start at +0x140.

#include "xil_io.h"
#include "xil_printf.h"

#define USB0_BASE     0xE0002000u
#define USB_CAPLENGTH (USB0_BASE + 0x100u)   // 8-bit
#define USB_USBCMD    (USB0_BASE + 0x140u)
#define USB_USBSTS    (USB0_BASE + 0x144u)
#define USB_USBINTR   (USB0_BASE + 0x148u)
#define USB_ULPIVP    (USB0_BASE + 0x170u)   // ULPI viewport (PHY register access)
#define USB_PORTSC1   (USB0_BASE + 0x184u)
#define USB_OTGSC     (USB0_BASE + 0x1A4u)
#define USB_USBMODE   (USB0_BASE + 0x1A8u)

#define CMD_RS        0x00000001u   // run/stop
#define CMD_RST       0x00000002u   // controller reset
#define MODE_CM_HOST  0x00000003u   // USBMODE.CM = host
#define PORTSC_CCS    (1u << 0)     // current connect status
#define PORTSC_PED    (1u << 2)     // port enabled
#define PORTSC_PR     (1u << 8)     // port reset
#define PORTSC_PP     (1u << 12)    // port power
#define PORTSC_PSPD_SH 26          // port speed [27:26] 0=full 1=low 2=high

static int last_ccs = -1;

// Write a USB3320 ULPI PHY register via the ChipIdea ULPI viewport.
// VIEWPORT: bit30=RUN (self-clears when done), bit29=RW(1=write), [23:16]=addr,
// [15:8]=write data.
static void ulpi_write(u8 addr, u8 data)
{
    Xil_Out32(USB_ULPIVP, (1u << 30) | (1u << 29) | ((u32)addr << 16) | ((u32)data << 8));
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_ULPIVP) & (1u << 30)); i++) {}
}

void usb_init(void)
{
    u32 cap = Xil_In8(USB_CAPLENGTH);

    // Reset the controller, wait for self-clear.
    Xil_Out32(USB_USBCMD, CMD_RST);
    for (volatile int i = 0; i < 1000000 && (Xil_In32(USB_USBCMD) & CMD_RST); i++) {}

    Xil_Out32(USB_USBINTR, 0);                 // poll, no interrupts (phase 1)
    Xil_Out32(USB_USBMODE, MODE_CM_HOST);      // host mode

    // Tell the ULPI PHY (USB3320) to drive VBUS via the external power switch:
    // OTG Control reg 0x0A, bit6=DrvVbusExternal (CPEN->switch), bit5=DrvVbus.
    // PORTSC.PP alone often doesn't trip the board's VBUS switch.
    ulpi_write(0x0A, 0x60);

    Xil_Out32(USB_PORTSC1, Xil_In32(USB_PORTSC1) | PORTSC_PP);  // port power
    Xil_Out32(USB_USBCMD, CMD_RS);             // run

    xil_printf("[usb] host started: CAPLENGTH=%02x USBMODE=%x PORTSC=%08x OTGSC=%08x\r\n",
               (unsigned)cap, (unsigned)Xil_In32(USB_USBMODE),
               (unsigned)Xil_In32(USB_PORTSC1), (unsigned)Xil_In32(USB_OTGSC));
    xil_printf("[usb] waiting for a USB keyboard on J9 (OTG adapter)...\r\n");
}

void usb_poll(void)
{
    u32 p = Xil_In32(USB_PORTSC1);
    int ccs = (p & PORTSC_CCS) ? 1 : 0;
    if (ccs != last_ccs) {
        last_ccs = ccs;
        if (ccs) {
            const char *sp[] = {"full", "low", "high", "?"};
            xil_printf("[usb] DEVICE CONNECTED  PORTSC=%08x speed=%s\r\n",
                       (unsigned)p, sp[(p >> PORTSC_PSPD_SH) & 3]);
        } else {
            xil_printf("[usb] device disconnected\r\n");
        }
    }
}
