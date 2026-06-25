SUMMARY = "NovaVM appliance setup: SSH key, samba share, nova-fs, first-boot grow-rootfs"
LICENSE = "CLOSED"

SRC_URI = "file://authorized_keys \
           file://smb.conf \
           file://smb.service \
           file://nova-firstboot"

S = "${WORKDIR}"

# Location of the NovaVM checkout (the nova-fs essentials are installed from it).
NOVAVM_REPO ?= "/home/barry/NovaVM"

RDEPENDS:${PN} = "samba avahi-daemon parted e2fsprogs-resize2fs util-linux-sfdisk util-linux-partx novavm"

inherit update-rc.d
INITSCRIPT_NAME = "nova-firstboot"
INITSCRIPT_PARAMS = "start 99 S ."

do_install() {
    install -d -m 0700 ${D}/home/root/.ssh
    install -m 0600 ${WORKDIR}/authorized_keys ${D}/home/root/.ssh/authorized_keys
    install -d ${D}${sysconfdir}/avahi/services
    install -m 0644 ${WORKDIR}/smb.service ${D}${sysconfdir}/avahi/services/smb.service
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/nova-firstboot ${D}${sysconfdir}/init.d/nova-firstboot

    # --- nova-fs essentials (lean set): BASIC ROM + boot config + splash logo +
    # the two language floppies. Games/extras are added at runtime over Samba. ---
    install -d ${D}/data/nova/roms ${D}/data/nova/config ${D}/data/nova/assets/boot ${D}/data/nova/hdds
    install -d ${D}/data/nova/fds ${D}/data/nova/programs ${D}/data/nova/soundfonts
    install -m 0644 ${NOVAVM_REPO}/e6502.FPGA/boards/arty_z7/rom/ehbasic.bin       ${D}/data/nova/roms/ehbasic.bin
    install -m 0644 ${NOVAVM_REPO}/e6502.ESP32/novahost/assets/config/boot.json    ${D}/data/nova/config/boot.json
    install -m 0644 ${NOVAVM_REPO}/e6502.ESP32/novahost/assets/boot/novavm_logo.nvg ${D}/data/nova/assets/boot/novavm_logo.nvg
    install -m 0644 ${NOVAVM_REPO}/novaforth/novaforth.ndi                         ${D}/data/nova/hdds/novaforth.ndi
    install -m 0644 ${NOVAVM_REPO}/novalogo/novalogo.ndi                           ${D}/data/nova/hdds/novalogo.ndi
}

FILES:${PN} += "/home/root/.ssh/authorized_keys /data"
