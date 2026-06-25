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
INITSCRIPT_PARAMS = "defaults 99"

# Absolute source paths embed into the binary's debug strings; harmless here.
INSANE_SKIP:${PN} = "buildpaths"

do_compile() {
    mkdir -p ${B}
    # naudio.c #includes ../../ps_fio/src/{sid.c,thirdparty/*}; those resolve
    # relative to each source file, so absolute paths on the command line are fine.
    ${CC} ${CFLAGS} ${LDFLAGS} -O2 -pthread -I${NOVAVM_SRC} \
        ${NOVAVM_SRC}/novavm.c ${NOVAVM_SRC}/naudio.c ${NOVAVM_SRC}/nservers.c \
        ${NOVAVM_SRC}/nkbd.c ${NOVAVM_SRC}/nfio.c ${NOVAVM_SRC}/nsplash.c \
        ${NOVAVM_SRC}/nbootcfg.c ${NOVAVM_SRC}/cJSON.c \
        -lm -o ${B}/novavm
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/novavm ${D}${bindir}/novavm
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${NOVAVM_SRC}/novavm.init ${D}${sysconfdir}/init.d/novavm
}

FILES:${PN} = "${bindir}/novavm ${sysconfdir}/init.d/novavm"
