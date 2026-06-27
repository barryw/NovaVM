# Lean appliance: keep server/dev cruft this appliance never uses out of the
# image -- not installed, not started at boot. This lives in a meta-user image
# bbappend (parsed directly from the in-repo layer every build) so the cut is
# fully reproducible. petalinuxbsp.conf does NOT work for this: PetaLinux feeds
# the build from a generated build/conf snapshot that doesn't track project-spec
# edits, so a :remove there silently never reaches do_rootfs.
#
#   nfs-utils      -- we share via samba, never NFS. Dropping it also removes
#                     rpcbind + the S15mountnfs.sh boot stall + nfscommon/nfsserver.
#   tcf-agent      -- the Xilinx TCF debug agent (dev-only, S99).
#   busybox-inetd  -- runs inetd serving ONLY telnetd + ftpd (plaintext, insecure,
#                     unused -- ssh + samba cover us). It's a busybox RRECOMMENDS
#                     (soft dep), so it needs BAD_RECOMMENDATIONS, not :remove.
IMAGE_INSTALL:remove = " nfs-utils tcf-agent"
BAD_RECOMMENDATIONS:append = " busybox-inetd"
