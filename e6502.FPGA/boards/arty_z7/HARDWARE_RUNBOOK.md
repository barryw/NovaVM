# Arty Z7 Hardware Runbook

Use this when working with the live Arty Z7-20 board. Do not guess the IP, do
not copy stale binaries by hand, and do not debug hardware until the status
check below passes.

## Current Target

As of 2026-07-02, the live Arty is:

```bash
192.168.1.188
```

`192.168.1.213` was a stale DHCP lease from an older session. Do not use it
unless a fresh status check or serial log proves the board moved back there.

## First Check

```bash
rtk dotnet run --project e6502.Nova -c Release -- device status --remote 192.168.1.188
```

Known-good signs:

- `health.ok` is `true`
- `network.ip` is `192.168.1.188`
- management is connected on port `6504`
- firmware currently reports `Jun 29 2026 15:51:10`

If this fails, find the board before doing anything else:

```bash
rtk proxy /bin/bash -lc 'for i in $(seq 1 254); do ip=192.168.1.$i; (nc -z -w1 "$ip" 6504 >/dev/null 2>&1 && echo "$ip nova6504") & (nc -z -w1 "$ip" 22 >/dev/null 2>&1 && echo "$ip ssh") & done; wait'
```

Prefer the host that answers on both `6504` and `22`.

## Deploy The Editor Demo

The canonical editor-demo deploy path is:

```bash
rtk dotnet run --project e6502.Nova -c Release -- arty deploy-editor-demo --remote 192.168.1.188
```

That command:

- rebuilds all 6502 payloads through `nova arty sync-payloads`
- rebuilds the Linux `novavm` host
- stages `novavm` in `/run` while SSH is still healthy
- copies `ehbasic.bin` and the language/editor demo disk images under `/data/nova`
- remounts `/` writable, installs `novavm` to `/usr/bin/novavm`, then
  remounts `/` read-only
- packages `editbuf_demo.bin` into `/data/nova/disks/floppy/editor-demo.ndi`
  and mounts it as `fd0`
- restarts `novavm` in the final remote command and tails `/run/novavm.log`

Stage files before stopping/restarting `novavm`. On 2026-07-02 the board
dropped off Ethernet after `/etc/init.d/novavm stop`, so stopping first lost
SSH before the replacement binary could be copied.

For a host-only deploy without the editor demo autoboot:

```bash
rtk dotnet run --project e6502.Nova -c Release -- arty deploy-linux-host --remote 192.168.1.188
```

## SSH Host Keys

The Nova CLI Arty deploy commands use:

```text
BatchMode=yes
ConnectTimeout=3
StrictHostKeyChecking=accept-new
```

This accepts a new Arty host key without an interactive prompt, which is what
blocked deployment on 2026-07-02. If OpenSSH reports that the key changed,
clear just this host and rerun:

```bash
ssh-keygen -R 192.168.1.188
```

Do not work around this by changing the deploy target to an old IP.

## Stale-Code Rule

All hardware deploy/build entry points must regenerate 6502 payloads before
they build or copy host artifacts.

Current guarded entry points:

- `e6502.Nova`: `nova arty sync-payloads`
- `e6502.Nova`: `nova arty build-linux-host`
- `e6502.Nova`: `nova arty deploy-linux-host`
- `e6502.Nova`: `nova arty deploy-editor-demo`
- `e6502.Nova`: `nova arty make-boot-bin`
- `e6502.Nova`: `nova arty build-ps-fio`

The shared guard is now a Nova CLI command:

```bash
rtk dotnet run --project e6502.Nova -c Release -- arty sync-payloads
```

If you bypass these entry points, run the guard first. Stale ROMs, modules, or
embedded headers are a failed build, not a debugging lead.

## Quick Triage

Board alive, SSH failing:

```bash
rtk proxy /bin/bash -lc 'ssh -o BatchMode=yes -o ConnectTimeout=3 -o StrictHostKeyChecking=accept-new root@192.168.1.188 true'
```

Board alive, filesystem check:

```bash
rtk dotnet run --project e6502.Nova -c Release -- ls --remote 192.168.1.188
```

Root filesystem check:

```bash
rtk proxy /bin/bash -lc 'ssh -o BatchMode=yes -o ConnectTimeout=3 -o StrictHostKeyChecking=accept-new root@192.168.1.188 "mount | grep \" on / \""'
```

Expected: `/dev/root on / type ext4 (ro,relatime)`. Nova CLI deploy handles this
by staging in `/run` and doing a short remount for `/usr/bin/novavm`.

Serial fallback:

```bash
rtk proxy /bin/bash -lc 'stty -F /dev/ttyUSB1 115200 raw -echo 2>/dev/null; timeout 15 cat /dev/ttyUSB1'
```

If the board vanishes after a JTAG/reset cycle, power-cycle it and re-check the
IP. Avoid assuming the old DHCP lease survived.

If the board vanishes immediately after stopping/restarting `novavm`, check:

```bash
rtk ip neigh show 192.168.1.188
rtk proxy /bin/bash -lc 'for p in 22 6504; do nc -z -w2 192.168.1.188 "$p" >/dev/null 2>&1 && echo "$p open" || echo "$p closed"; done'
```

If ARP is `INCOMPLETE` and the subnet scan no longer shows a `nova6504` host,
the board needs a physical power-cycle. USB/JTAG may still enumerate while
Linux networking is gone.
