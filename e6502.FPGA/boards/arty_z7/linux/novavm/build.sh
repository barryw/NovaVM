#!/bin/sh
# Build the NovaVM Linux host for the Arty Z7-20 (Zynq-7020, armv7).
# Userspace port of the bare-metal ps_fio: 6502 boot + FIO service + audio +
# physical USB keyboard + the NovaHost mgmt/debug/upload servers.
#
#   novavm.c    main, boot (ROM + $0320 loader + LOAD_MODULE), FIO LOAD/SAVE/DIR
#   naudio.c    software WTS/MIDI + PL SID feeder; VOLUME/MIDPLAY/SFLOAD (needs -lm)
#   nservers.c  mgmt 6504 (CBOR) / debug 6503 (JSON) / upload 6502 (BSD sockets)
#   nkbd.c      physical USB keyboard via evdev -> R_KEY (Ctrl-skip-autoboot)
set -e
cd "$(dirname "$0")"
CC="${CC:-arm-linux-gnueabihf-gcc}"
$CC -O2 -static -pthread -I. -o novavm \
    novavm.c naudio.c nservers.c nkbd.c nfio.c nsplash.c nbootcfg.c nosd.c cJSON.c -lm
$CC --version >/dev/null
arm-linux-gnueabihf-size novavm | tail -1
echo "built: $(pwd)/novavm"
