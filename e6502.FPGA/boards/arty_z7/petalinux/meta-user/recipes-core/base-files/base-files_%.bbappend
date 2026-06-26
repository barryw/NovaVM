# Read-only-rootfs appliance fstab:
#  - force / to mount read-only (the kernel cmdline says ro, but the stock fstab
#    '/' line says 'defaults', so checkroot remounts it rw -- pin it to ro so the
#    OS partition never goes dirty -> no fsck/journal recovery on a power-yank).
#  - mount the writable ext4 /data partition (nova-sd.wks p3) every boot.
# /run + /var/volatile are already tmpfs; nova-rwbind binds /var/lib to tmpfs.
do_install:append() {
    sed -i '/^\/dev\/root/ s/defaults/ro/' ${D}${sysconfdir}/fstab
    printf 'LABEL=data\t/data\text4\tdefaults\t0\t2\n' >> ${D}${sysconfdir}/fstab
}
