FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://smb.conf"
do_install:append() {
    install -d ${D}${sysconfdir}/samba
    install -m 0644 ${WORKDIR}/smb.conf ${D}${sysconfdir}/samba/smb.conf
}
