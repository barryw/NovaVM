#!/bin/bash
# Build the NovaVM Arty-Z7 SD image (images/linux/petalinux-sdimage.wic).
#
# Works around a PetaLinux 2024.2 bug. `petalinux-package wic` hardcodes its
# output-file glob to 'rootfs-*.direct' (scripts/libs/package_wic.py:212),
# which only matches the default WKS it auto-generates (rootfs.wks). With our
# 3-partition nova-sd.wks (boot / root / *blank data*), `wic create` correctly
# builds 'nova-sd-*.direct', but the wrapper globs for 'rootfs-*', gets an empty
# list, and does [0] -> IndexError -- crashing BEFORE it copies the (perfectly
# built) image out, and before it cleans up the tmp dir. So we just grab the
# image wic left behind. The real fix would be a 1-line patch to package_wic.py
# (glob '*.direct*'), but that lives in the PetaLinux install, not this repo.
#
# Usage: ./make-wic.sh [PETALINUX_PROJECT_DIR]   (default: $PWD)
set -e
PROOT="${1:-$PWD}"
WKS="$PROOT/project-spec/meta-user/wic/nova-sd.wks"
cd "$PROOT"
echo "[make-wic] building via petalinux-package wic (the IndexError is EXPECTED -- the known glob bug)"
petalinux-package wic -w "$WKS" --force || true
DIRECT=$(ls -t build/wic/wic-tmp/nova-sd-*-*.direct 2>/dev/null | head -1)
[ -z "$DIRECT" ] && { echo "[make-wic] ERROR: no nova-sd-*.direct produced -- wic itself failed"; exit 1; }
cp "$DIRECT" images/linux/petalinux-sdimage.wic
echo "[make-wic] OK: images/linux/petalinux-sdimage.wic ($(numfmt --to=iec $(stat -c%s images/linux/petalinux-sdimage.wic)))"
/sbin/parted -s images/linux/petalinux-sdimage.wic print 2>/dev/null | sed -n '/Number/,$p' || true
