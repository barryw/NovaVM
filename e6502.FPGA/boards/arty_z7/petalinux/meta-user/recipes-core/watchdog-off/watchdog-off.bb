SUMMARY = "Disarm the FSBL-armed Zynq SWDT early in boot (hobby board)"
DESCRIPTION = "Installs an rcS S01 init script that magic-closes /dev/watchdog, \
stopping the FSBL-armed System Watchdog Timer before the udev coldplug that used \
to starve the kernel pinger and trip a 120s cold-boot reset loop. This is a hobby \
8-bit retro-computer that needs no auto-recovery from a hung CPU."
LICENSE = "CLOSED"

SRC_URI = "file://watchdog-off"
S = "${WORKDIR}"

inherit update-rc.d
INITSCRIPT_NAME = "watchdog-off"
# start, sequence 01, runlevel S (sysinit) -> /etc/rcS.d/S01watchdog-off, runs
# before S04udev (coldplug) and everything heavy.
INITSCRIPT_PARAMS = "start 01 S ."

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/watchdog-off ${D}${sysconfdir}/init.d/watchdog-off
}

FILES:${PN} = "${sysconfdir}/init.d/watchdog-off"
