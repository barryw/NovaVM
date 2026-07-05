SUMMARY = "NovaVM host — 6502 SoC userspace host (boot, FIO, audio, NovaHost servers, boot splash)"
DESCRIPTION = "Compiles the NovaVM Linux host (novavm) from the NovaVM source tree and \
installs it + its SysV init script. Built from source so the image is reproducible."
LICENSE = "CLOSED"
PV = "1.0"

# Location of the NovaVM checkout (override if it lives elsewhere).
NOVAVM_REPO ?= "/home/barry/NovaVM"
NOVAVM_SRC   = "${NOVAVM_REPO}/e6502.FPGA/boards/arty_z7/linux/novavm"

# We build directly from the external NovaVM tree — nothing to fetch/unpack.
SRC_URI = ""
S = "${WORKDIR}"
B = "${WORKDIR}/build"

do_fetch[noexec]     = "1"
do_unpack[noexec]    = "1"
do_patch[noexec]     = "1"
do_configure[noexec] = "1"
# External tree: always recompile (no sstate caching of the source).
do_compile[nostamp]  = "1"

inherit update-rc.d
INITSCRIPT_NAME   = "novavm"
# Early in rcS (S04, right after nova-firstboot seeds /data) so the hold-screen +
# 6502 boot to BASIC happen in seconds, not after the full Linux boot. The init
# script mknod's /dev/mem itself (udev doesn't create it until its slow coldplug).
INITSCRIPT_PARAMS = "start 04 S ."

# Absolute source paths embed into the binary's debug strings; harmless here.
INSANE_SKIP:${PN} = "buildpaths"

do_compile() {
    mkdir -p ${B}
    # naudio.c #includes ../../ps_fio/src/{sid.c,thirdparty/*}; those resolve
    # relative to each source file, so absolute paths on the command line are fine.
    ${CC} ${CFLAGS} ${LDFLAGS} -O2 -pthread -I${NOVAVM_SRC} \
        ${NOVAVM_SRC}/novavm.c ${NOVAVM_SRC}/naudio.c ${NOVAVM_SRC}/nservers.c \
        ${NOVAVM_SRC}/nkbd.c ${NOVAVM_SRC}/nmouse.c ${NOVAVM_SRC}/nfio.c ${NOVAVM_SRC}/nsplash.c \
        ${NOVAVM_SRC}/nbootcfg.c ${NOVAVM_SRC}/nosd.c ${NOVAVM_SRC}/cJSON.c \
        -lm -o ${B}/novavm
    ${CC} ${CFLAGS} ${LDFLAGS} -O2 ${NOVAVM_SRC}/novacap-gadget.c -o ${B}/novacap-gadget
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/novavm ${D}${bindir}/novavm
    install -m 0755 ${B}/novacap-gadget ${D}${bindir}/novacap-gadget
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${NOVAVM_SRC}/novavm.init ${D}${sysconfdir}/init.d/novavm
}

FILES:${PN} = "${bindir}/novavm ${bindir}/novacap-gadget ${sysconfdir}/init.d/novavm"
