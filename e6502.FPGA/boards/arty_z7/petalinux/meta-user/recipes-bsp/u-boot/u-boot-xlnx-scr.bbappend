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
