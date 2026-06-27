# The generated boot.scr stages the FIT (image.ub) at FIT_IMAGE_LOAD_ADDRESS,
# whose default (FIT_IMAGE_OFFSET) is 0x10000000 -- which is EXACTLY NovaVM's
# reserved XRAM region (system-user.dtsi reserved-memory @ 0x10000000, no-map).
#
# u-boot 2024.01's lmb now strictly refuses to `fatload` over a no-map
# reservation ("** Reading file would overwrite reserved memory **"), so the
# kernel never loaded, u-boot fell through to QSPI/USB/network/PXE (all fail),
# and sat at the prompt looking like a dead board. (Diagnosed live over serial.)
#
# Stage the FIT in low free DDR instead. XRAM stays at 0x10000000; the kernel
# inside the FIT extracts to ~0x200000 and runs in low DDR, so 0x08000000 (128M)
# is clear of both. Confirmed booting to BASIC at this address by hand in u-boot.
FIT_IMAGE_LOAD_ADDRESS = "0x08000000"

# --- Power-on hold screen (the "hang in there" monkey) ---
# The bitstream's char-RAM BRAM init does NOT render at power-on (a VGC quirk),
# but WRITES to the VGC do (the path novavm's splash uses -- confirmed live). So
# paint the monkey from u-boot the instant it starts: clear the 80x50 text grid
# and write the monkey cells via the fio_bridge (0x40000000). monkey.cmd is the
# raw mw-command sequence; mkimage wraps it as monkey.scr, which the boot script
# loads + sources as its very first action -- so the monkey is on screen through
# the whole kernel boot, long before novavm's splash -> BASIC. The 6502 is held
# in reset at this point (novavm releases it later), so nothing fights the paint.
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "file://monkey.cmd"

PRE_BOOTENV:forcevariable = "if fatload mmc 0:1 0x05000000 monkey.scr; then source 0x05000000; fi;"

do_compile:append() {
    mkimage -A arm -T script -C none -n "nova-monkey" -d "${WORKDIR}/monkey.cmd" "${WORKDIR}/monkey.scr"
}
do_install:append() {
    install -m 0644 "${WORKDIR}/monkey.scr" ${D}/boot/
}
do_deploy:append() {
    install -m 0644 "${WORKDIR}/monkey.scr" ${DEPLOYDIR}/
}
