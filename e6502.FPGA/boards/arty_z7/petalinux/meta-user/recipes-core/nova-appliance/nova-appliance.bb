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
# Early in rcS (after S03mountall mounts /data, before S04novavm) so /data is
# grown + seeded before novavm reads the ROM from it.
INITSCRIPT_NAME = "nova-firstboot"
INITSCRIPT_PARAMS = "start 03 S ."

do_install() {
    install -d -m 0700 ${D}/home/root/.ssh
    install -m 0600 ${WORKDIR}/authorized_keys ${D}/home/root/.ssh/authorized_keys
    install -d ${D}${sysconfdir}/avahi/services
    install -m 0644 ${WORKDIR}/smb.service ${D}${sysconfdir}/avahi/services/smb.service
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/nova-firstboot ${D}${sysconfdir}/init.d/nova-firstboot

    # --- nova-fs essentials, staged as a READ-ONLY TEMPLATE on the rootfs
    # (${datadir}/nova-fs). nova-firstboot copies it onto the writable /data
    # partition on first boot (the rootfs is read-only, so it can't be /data). ---
    NFS=${D}${datadir}/nova-fs
    install -d ${NFS}/roms ${NFS}/config ${NFS}/assets/boot ${NFS}/disks ${NFS}/programs ${NFS}/soundfonts
    install -m 0644 ${NOVAVM_REPO}/e6502.FPGA/boards/arty_z7/rom/ehbasic.bin        ${NFS}/roms/ehbasic.bin
    install -m 0644 ${NOVAVM_REPO}/e6502.ESP32/novahost/assets/config/boot.json     ${NFS}/config/boot.json
    install -m 0644 ${NOVAVM_REPO}/e6502.ESP32/novahost/assets/boot/novavm_logo.nvg ${NFS}/assets/boot/novavm_logo.nvg
    install -m 0644 ${NOVAVM_REPO}/software/languages/novaforth/novaforth.ndi        ${NFS}/disks/novaforth.ndi
    install -m 0644 ${NOVAVM_REPO}/software/languages/novalogo/novalogo.ndi          ${NFS}/disks/novalogo.ndi

    # /data is just a mountpoint for the separate writable ext4 partition
    # (nova-sd.wks). Its fstab entry is added by the base-files bbappend, so it
    # doesn't collide with base-files owning /etc/fstab.
    install -d ${D}/data
}

FILES:${PN} += "/home/root/.ssh/authorized_keys /data ${datadir}/nova-fs"
