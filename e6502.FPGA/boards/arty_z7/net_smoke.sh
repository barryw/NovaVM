#!/usr/bin/env bash
# net_smoke.sh -- Arty Z7 PS Ethernet smoke test (HW-in-the-loop; NOT a CI test).
#
# Proves the PS GEM0 + RTL8211F + lwIP data path end to end: ICMP (RX+TX) and the
# TCP-6502 file-upload path (RX under load -> SD). Run it with the Arty booted into
# ps_fio (boot_fio_por.tcl / boot_fio_noreset.tcl) and on the same LAN. Get the IP
# from the Arty's serial console line: "[net] DHCP IP: x.x.x.x".
#
# Usage: net_smoke.sh <arty-ip> [serial-dev]   (serial-dev defaults to /dev/ttyUSB1)
#
# Background: the RX bug here was NET_CFG bit2 (NVLANDISC) + the RTL8211F speed read;
# see ps_fio/src/net.c and vitis/build_ps_fio.py (patch_physpeed). This script is the
# regression check that the fix still moves real frames.
set -euo pipefail

IP="${1:?usage: net_smoke.sh <arty-ip> [serial-dev]}"
SER="${2:-/dev/ttyUSB1}"

echo "== 1. ICMP (RX+TX) to $IP =="
ping -c 3 -W 2 "$IP"

echo "== 2. TCP 6502 upload roundtrip =="
TF="$(mktemp)"; trap 'rm -f "$TF"' EXIT
dd if=/dev/urandom of="$TF" bs=1024 count=128 status=none
N="$(wc -c < "$TF")"

SERLOG=""
if [ -e "$SER" ]; then
    SERLOG="$(mktemp)"
    stty -F "$SER" 115200 cs8 -cstopb -parenb -crtscts raw -echo 2>/dev/null || true
    ( timeout 12 cat "$SER" > "$SERLOG" 2>/dev/null & )
    sleep 1
fi

cat "$TF" > "/dev/tcp/$IP/6502"
sleep 3

if [ -n "$SERLOG" ]; then
    if tr -d '\r' < "$SERLOG" | grep -q "upload complete: $N bytes"; then
        echo "PASS: Arty reported upload complete ($N bytes -> 0:/UPLOAD.NDI)"
    else
        echo "FAIL: no 'upload complete: $N bytes' on $SER"; rm -f "$SERLOG"; exit 1
    fi
    rm -f "$SERLOG"
else
    echo "(no serial at $SER; $N bytes sent -- check the Arty console for"
    echo " '[net] upload complete: $N bytes')"
fi

echo "SMOKE OK"
