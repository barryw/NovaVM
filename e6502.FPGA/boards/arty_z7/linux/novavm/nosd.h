/* nosd.h — On-Screen Display config menu (BIOS-style), driven by the 4 Arty
 * push buttons. The OSD is composited in the HDMI path (separate from the VGC);
 * novavm owns all the logic here: button decode, menu FSM, framebuffer writes,
 * mount/unmount/boot, and the global system-pause while it's open.
 *
 * See boards/arty_z7/OSD_CONFIG_SPEC.md. Until the OSD-compositor bitstream
 * lands, the register backend is stubbed (osd_selftest drives the FSM and dumps
 * the rendered grid to stdout) so the menu logic is validated host-side. */
#ifndef NOSD_H
#define NOSD_H

/* Spawn the ~60 Hz button-poll thread (active once the bitstream exposes the
 * BUTTONS register). Safe to call now: it no-ops the hardware path until the
 * compositor exists. */
void osd_init(void);

/* Feed one button press into the menu FSM. b: 1=toggle/close, 2=up, 3=down,
 * 4=select. Exposed for the poll thread and for osd_selftest. */
void osd_button(int b);

/* 1 while the OSD is open (the machine is paused). */
int  osd_is_open(void);

/* Step-1 validation: run a scripted button sequence against the real
 * /data/nova/disks + boot.json, dumping each rendered grid to stdout. The
 * mount-and-boot action is dry-run (logs instead of resetting the board). */
void osd_selftest(void);

#endif /* NOSD_H */
