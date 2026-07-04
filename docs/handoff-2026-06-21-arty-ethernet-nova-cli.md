# Handoff — 2026-06-21 — Arty Z7 Ethernet (DONE) + nova-CLI-vs-Arty + Python-tool consolidation

Resume point for a clean session. Branch: `feat/arty-z7-port`. Board: Arty Z7-20 on `/dev/ttyUSB1` @115200, JTAG via `xsct`.

---

## TL;DR

1. **DONE + committed + pushed:** Arty PS Ethernet works end-to-end (DHCP, ping, TCP-6502 upload). Commit `3582558` is on GitHub (`origin` switched to SSH: `git@github.com:barryw/NovaVM`).
2. **NEXT (in progress):** port the NovaHost protocol to the Arty PS so the `nova` CLI works `--remote` against it (tasks #19–#22), and consolidate ~18 sprawl Python scripts into the `nova` CLI (task #23). These two are the **same effort** (the migrations need a live NovaHost to test, which the port provides).

---

## 1. What got DONE this session

### Arty PS Ethernet RX — FIXED (the headline; a multi-hour epic)
Symptom going in: TX flowed but RX received zero frames, DHCP never completed. Two root causes, two fixes (both committed in `3582558`):

- **`e6502.FPGA/boards/arty_z7/ps_fio/src/net.c`** — clear `NVLANDISC` in NET_CFG.
  GEM register offsets are **NET_CTRL = 0x0, NET_CFG = 0x4** (verified in `xemacps_hw.h`). NET_CFG bit2 = `XEMACPS_NWCFG_NVLANDISC_MASK` ("receive only VLAN frames") → the MAC discards every non-VLAN frame (all DHCP/ARP/normal traffic) → RX reads zero while TX is fine. The bug was self-inflicted: an earlier "enable RX/TX" line wrote offset 0x4 (NET_CFG, not NET_CTRL) with `|= 0x1C`, setting NVLANDISC. Fix: `XEmacPs_WriteReg(base, 0x4, ReadReg(base,0x4) & ~0x4)`.
- **`e6502.FPGA/boards/arty_z7/vitis/build_ps_fio.py` (`patch_physpeed`)** — the BSP `get_Realtek_phy_speed` reads the RTL8211**E** PHYSR (reg 0x11), which fails on the **F** (this board) → "Phy setup error" → `init_emacps` aborts. Patched to read the F's PHYSR1 (page `0xa43` reg `0x1a`).

**Verified on HW:** `[net] DHCP IP: 192.168.1.213`, `ping` 0% loss, 128 KB file streamed to TCP 6502 → `upload complete` on the SD.

### Tests + tooling added (committed)
- Arty hardware checks now run through `nova`: use `nova device status
  --remote <ip>`, `nova get`/`nova put` for TCP file roundtrips, and
  `nova check spi-bridge --remote <ip>` for SDRAM bridge stress.
- `e6502.FPGA/boards/arty_z7/boot_fio_noreset.tcl` — gentle boot (processor-only reset; `boot_fio_por.tcl`'s `rst -srst` flaps the PHY link).
- `e6502UnitTests/ArtyEthernetRegressionTests.cs` — CI guard asserting both fixes stay in source. Run: `dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release -p:SkipRomBuild=true --filter "FullyQualifiedName~ArtyEthernet"` (2/2 pass).

### Script cleanup status
This earlier Python-tool review is obsolete. Repo operations now belong in
`nova`; the remaining allowed Python hooks are documented in
`docs/script-inventory.md`.

---

## 2. Build / boot / test cheat-sheet (Arty)

```bash
# Build the PS firmware (regenerates BSP + applies the physpeed patch)
nova arty build-ps-fio                                           # ~3 min
ls /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf                       # the ELF

# Boot it (FPGA loads over JTAG; FPGA is blank after a power cycle)
/tools/Xilinx/Vitis/2024.2/bin/xsct boot_fio_por.tcl             # full (rst -srst; flaps PHY)
# or boot_fio_noreset.tcl from a freshly power-cycled board (gentle)

# Watch serial / get the DHCP IP
stty -F /dev/ttyUSB1 115200 cs8 -cstopb -parenb -crtscts raw -echo
timeout 30 cat /dev/ttyUSB1 | tr -d '\r' | grep -E 'DHCP IP|NVLANDISC|link='

# Network checks
nova device status --remote 192.168.1.213
nova check spi-bridge --remote 192.168.1.213
```
Notes: U-Boot oracle at `/tmp/u-boot-xlnx/u-boot.elf` (`boot_uboot_net.tcl`) RXes on this board — the ground-truth reference used to crack the bug. The DHCP IP can change; read it from the serial.

---

## 3. NEXT — make the `nova` CLI work `--remote` against the Arty (tasks #19–#22)

The Arty PS today serves only **TCP 6502** (the dumb file-upload → `0:/UPLOAD.NDI`). The `nova` CLI's `--remote` needs two NovaHost protocols the Arty doesn't speak yet. Phase 1 (#18, **done**) mapped them:

- **6504 management (CBOR):** 24-byte header `NVH1`(4) | version u16 | command u16 | flags u16 | requestId u32 | cborLen u32 | rawLen u32, then CBOR map payload + raw bytes. Dispatch on the **command u16 enum**: `Hello=1, GetStatus=2, ListDirectory=3, ReadFile=4, WriteFileBegin=5, WriteFileChunk=6, WriteFileCommit=7, WriteFileAbort=8, DeletePath=9, MountDrive=10, UnmountDrive=11, SetRuntimeConfig=12, AudioStop=13, VmReset=14, HostReboot=15, WifiScan=16, WifiConfig=17, WifiAction=18, ReadFileChunk=19`. (Files/drives/runtime/device = the `nova disk/drive/runtime/device` deploy surface.)
- **6503 debug (newline JSON):** dispatch on a `"command"` string (`poke/peek/peek_block/read_screen/read_line/read_vram/fill_vram/get_cursor/type_text/send_key/vm_reset/cold_start/run_cycles/wait_ready/dbg_*`). This is the `nova vm/emulator` + `nova keyboard` surface.

**Reference impl to port:** `e6502.ESP32/novahost/{management_server,debug_server,fpga_bridge}.{cpp,h}`. The ESP32 uses an SPI `FpgaBridge`; the Arty uses the AXI FIO bridge — remap the primitives.
**Client/wire spec:** `e6502.NovaHost/NovaHostManagementClient.cs` (6504, `HeaderBytes=24`, `Version=1`, `enum Command`) and `NovaDebugClient.cs` (6503, default port 6503).
**Arty PS primitives already present** (`e6502.FPGA/boards/arty_z7/ps_fio/src/main.c`): `poke`/`peek` (R_POKE 0x40000000 / R_PEEK_ADDR+DATA), `kb_emit` (R_KEY), cpu reset (R_CTRL bit0), `vmem_read` via R_VMEM_ADDR/DATA (space+addr → screen/color/gfx/sprite), `R_CPU_PC`, FatFs `f_open/f_read/f_write/f_opendir`. So handlers wire onto existing functions — no new HW access.

**Phased plan:**
- #19 — add a lwIP TCP server on 6504 + NVH1 framing + a minimal C CBOR encoder + `Hello`/`GetStatus` handshake → `nova device --remote 192.168.1.213` connects.
- #20 — file/drive/runtime/VmReset handlers → `nova disk/drive/runtime` deploy (reuse the FatFs already in ps_fio).
- #21 — 6503 JSON debug server (poke/peek/screen/keyboard) → `nova vm`/`keyboard --remote` works.
- #22 — verify the whole surface against the live `nova` CLI (`dotnet run --project e6502.Nova -- ... --remote 192.168.1.213`), then commit/push.

This is genuinely multi-session (~2700 ref lines) but each milestone is independently testable against the real CLI.

---

## 4. NEXT — Python-tool migration (task #23, "unique tools first")

Order (decided): **converters → snapshot/keyboard/redirect.** Rationale: the converters (`ly2mml`/`xml2mml`/`score2bas` → `nova convert`) are pure file→file transforms, **testable offline with no board**; `snapshot`/`keyboard ftdi` and the `novahostctl` delete+redirect need a **live NovaHost to test** — so pair those with the port above (do them once `nova vm --remote arty` works). The `nova` CLI is one big `e6502.Nova/Program.cs` (custom switch dispatch; add verbs as cases + handler funcs/files; `NovaMusicStreamCompiler.cs` already exists for music). `novahostctl raw` → `nova vm raw <payload> --remote <host>`; its `reload-rom` has no bare nova equiv (use `nova vm raw '{"command":"reload_rom"}'` or add it).

---

## 5. Gotchas / hard-won facts (don't relearn)
- **Verify GEM register OFFSETS against `xemacps_hw.h`** before reasoning about bits — the NET_CTRL(0x0)/NET_CFG(0x4) swap cost ~a day of phantom conclusions ("gige won't stick", "RX/TX disabled", "lwIP doesn't start the GEM" — all wrong).
- **U-Boot is the oracle**: it RXes on this exact board; diffing live GEM regs vs U-Boot is how the bug was found.
- `rst -srst` flaps/degrades the PHY link; processor-only reset doesn't.
- Red herrings (all already correct): RGMII delays (rgmii-id, reg0x11=0x0109/reg0x15=0x0019 = U-Boot), DMA RXSIZE (0b11), GEM clock (125 MHz/1000M), RX BD ring, the "2nd priority queue" (Zynq-7000 has none; 0x440/0x480 read 0). Vitis 2025.2 changelog "RX BD fix" is the error-path, NOT this bug — don't upgrade chasing it.
- `git push` works now (SSH key added; `origin` is SSH). Commit trailer this session: `Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>` (branch history used `(1M context)` — match whichever).
- Standing rule: **one canonical tool = the `nova` CLI**; don't reinvent scripts (memory `use-existing-tools-stop-reinventing`).

## 6. Task state (live tasks)
- #18 ✅ map NovaHost protocol. #19–#22 ⬜ the Arty NovaHost port (Ph2–Ph5). #23 ◧ Python review done, migration pending (unique tools first). #10–#17 are old/stale Arty bring-up — ignore.

## 7. Memory updated
`~/.claude/.../memory/arty-ethernet-wip.md` → SOLVED (root cause + fixes); `MEMORY.md` index flipped to "Arty Ethernet SOLVED".

---

## Run a NovaZ game (or any `.ndi`) on the Arty

Verified 2026-06-23 (bold-on-Arty). The board boots NovaBASIC (ROM) by default; a
`.ndi` launches via its embedded `AUTOBOOT.bin` on **cold-start**.

```bash
# 1. Boot (FPGA is volatile -- blank after a power-cycle). Use the GENTLE script:
cd e6502.FPGA/boards/arty_z7
/tools/Xilinx/Vitis/2024.2/bin/xsct boot_fio_noreset.tcl     # processor-only reset; no PHY flap
stty -F /dev/ttyUSB1 115200 raw -echo; timeout 12 cat /dev/ttyUSB1 | grep 'DHCP IP'   # -> 192.168.1.213

# 2. Launch the disk (N = dotnet run --project e6502.Nova -c Release --)
N="dotnet run --project e6502.Nova -c Release --"
$N put examples/novaz/dist/z4-styles/fd0.ndi z4styles.ndi --remote 192.168.1.213   # -> /sd/z4styles.ndi
$N drive mount fd0 /z4styles.ndi --remote 192.168.1.213
$N vm cold-start --remote 192.168.1.213    # ROM runs fd0 AUTOBOOT.bin -> FIO_CMD_LOADRUNTIME -> JMP($FFFC)
$N vm screen --remote 192.168.1.213        # read the text screen back

# 3. Verify the VGC text attribute (bold/reverse) on hardware
$N vm read-vram 7 <addr> <len> --remote 192.168.1.213   # space 7 = text-attr; 04=bold 02=reverse 06=both
```

**Gotchas (both cost real time):**
- Boot with `boot_fio_noreset.tcl`, **not** `boot_fio_por.tcl` — the latter's `rst -srst`
  flaps the RTL8211F PHY, so the board drops off the network / re-DHCPs to a random IP
  and is unreachable until a power-cycle. (Both scripts also had a stale
  `/tmp/nova_ws2/.../ps7_init.tcl` source path — fixed to the repo-local `ps7_init.tcl`.)
- Upload with `nova put` (6504 WriteFile), **not** the dumb `nc <ip> 6502 < disk.ndi`
  (→ `/UPLOAD.NDI`). The nc file mounts but `vm cold-start` does NOT boot its AUTOBOOT —
  it stays at NovaBASIC. `nova put` + `drive mount fd0` + `vm cold-start` boots the runtime.

Build a disk: `make -C examples/novaz ndi` (or `test-z4-styles` for the styles fixture).
