# build_ps_fio.py — Vitis 2024.2 build of the NovaVM PS FIO host.
#   vitis -s build_ps_fio.py
#
# Builds a standalone platform from the full integration XSA (PS7 + fio_bridge),
# enables the xilffs (FatFs) library for microSD access, and compiles the ps_fio
# app (boards/arty_z7/ps_fio/src) -> ELF. Use a FRESH workspace dir.
#
# Prereq: build/arty_z7_full.xsa (from build_full.tcl).

import vitis, glob, os

HERE = os.path.dirname(os.path.abspath(__file__))
WS   = os.environ.get("PS_WS", "/tmp/nova_fio_ws")
XSA  = os.path.abspath(os.path.join(HERE, "..", "build", "arty_z7_full.xsa"))
SRC  = os.path.abspath(os.path.join(HERE, "..", "ps_fio", "src"))
DOMAIN = "standalone_ps7_cortexa9_0"

client = vitis.create_client()
client.set_workspace(WS)

# Bare-metal (standalone). Audio is driven by a high-priority TTC0 interrupt (see
# main.c), not an RTOS task -- a hardware IRQ preempts net/FIO/USB so audio never
# starves, without touching the raw-API lwIP net stack.
plat = client.create_platform_component(
    name="nova_fio_plat", hw_design=XSA, os="standalone", cpu="ps7_cortexa9_0")

# Enable FatFs (xilffs) + lwIP (GEM0) in the standalone domain BSP.
domain = plat.get_domain(name=DOMAIN)
domain.set_lib(lib_name="xilffs")
domain.set_lib(lib_name="lwip220")

def cfg(param, value, lib):
    # Each config independent + non-fatal so one bad param can't skip regenerate().
    try:
        domain.set_config(option="lib", param=param, value=value, lib_name=lib)
        print("  cfg ok:", lib, param, "=", value)
    except Exception as e:
        print("  cfg FAILED:", lib, param, "=", value, "->", e)

cfg("XILFFS_use_lfn", "2", "xilffs")           # LFN (8.3 fallback otherwise)
domain.regenerate()
print("BSP regenerated on", DOMAIN)

import re as _re
def patch_lwipopts():
    # lwip220's api_mode/lwip_dhcp are legacy TCL params the new Vitis Python API
    # can't set, so patch the generated lwipopts.h directly: enable DHCP + lwIP's
    # internal timers (NO_SYS=1/RAW is already the default; NO_SYS_NO_TIMERS=1 would
    # disable sys_check_timeouts + TCP timers).
    n = 0
    for lo in glob.glob(f"{WS}/**/lwipopts.h", recursive=True):
        t = open(lo).read()
        t = _re.sub(r'#define\s+NO_SYS_NO_TIMERS\s+\d+', '#define NO_SYS_NO_TIMERS 0', t)
        t = _re.sub(r'#define\s+LWIP_DHCP\s+\d+',        '#define LWIP_DHCP 1',        t)
        # Keep AUTODETECT: forcing a speed makes the driver re-drive the PHY and the
        # link bounces (DMA dies). Let it autonegotiate (this board settles at 10FD,
        # matching a known-good U-Boot). net.c then only corrects the MAC NET_CFG.
        t = _re.sub(r'#define\s+CONFIG_LINKSPEED_AUTODETECT\s+\d+', '#define CONFIG_LINKSPEED_AUTODETECT 1', t)
        # Enable link stats so the app can see TX/RX/drop counts (RX-vs-TX debug).
        t = _re.sub(r'#define\s+LWIP_STATS\s+\d+', '#define LWIP_STATS 1', t)
        t = _re.sub(r'#define\s+LINK_STATS\s+\d+', '#define LINK_STATS 1', t)
        open(lo, "w").write(t); n += 1
    print("  patched", n, "lwipopts.h")

def patch_physpeed():
    # The Arty Z7 has a Realtek RTL8211F (id1=0x1c, id2=0xc916). Stock
    # get_Realtek_phy_speed() reads the RTL8211E PHYSR (reg 0x11) for link/speed;
    # on the F the link bit (0x400) never sets -> returns XST_FAILURE -> "Phy setup
    # error" -> init_emacps aborts the GEM bring-up (dead data path). Fix: read the
    # RTL8211F PHYSR1 (page 0xa43 reg 0x1a: bit2=link, bits[5:4]=speed 0/1/2 =
    # 10/100/1000) so the driver resolves the real speed and finishes init. This
    # board links at GIGABIT (U-Boot confirms 1000M with RX working), so KEEP the
    # gigabit advertisement -- do NOT drop to 100. NB: Marvell's get_*_phy_speed
    # shares identical lines, so isolate the Realtek function by slice before
    # replacing (a global count=1 replace would hit Marvell).
    n = 0
    for ps in glob.glob(f"{WS}/**/xemacpsif_physpeed.c", recursive=True):
        t = open(ps).read()
        if "NOVA_RTL8211F" in t:
            continue
        # print the detected PHY address + identity at the dispatch (diagnostic).
        disp = "\tif (phy_identity == PHY_TI_IDENTIFIER) {"
        if disp in t:
            t = t.replace(disp,
                "\txil_printf(\"NOVA PHY addr=%d id1=0x%x\\r\\n\", (int)phy_addr, phy_identity);\n" + disp, 1)
        i = t.find("static u32_t get_Realtek_phy_speed")
        if i >= 0:
            j = t.find("\n}\n", i)
            fn = t[i:j]
            # KEEP gigabit advertisement (do not touch control |= ADVERTISE_1000).
            # Only fix the speed READ: RTL8211F PHYSR1 (page 0xa43 reg 0x1a), not the
            # E PHYSR (reg 0x11). NOVA_RTL8211F sentinel guards re-patching.
            fn = fn.replace(
                "XEmacPs_PhyRead(xemacpsp, phy_addr,IEEE_SPECIFIC_STATUS_REG,\n\t\t\t\t\t&status_speed);",
                "/* NOVA_RTL8211F: read PHYSR1 (page 0xa43 reg 0x1a) */\n"
                "\tXEmacPs_PhyWrite(xemacpsp, phy_addr, 0x1f, 0xa43);\n"
                "\tXEmacPs_PhyRead(xemacpsp, phy_addr, 0x1a, &status_speed);\n"
                "\tXEmacPs_PhyWrite(xemacpsp, phy_addr, 0x1f, 0x0);\n"
                "\txil_printf(\"RTL8211F PHYSR1=0x%x\\r\\n\", status_speed);")
            fn = fn.replace(
                "if (status_speed & 0x400) {\n\t\ttemp_speed = status_speed & IEEE_SPEED_MASK;",
                "if (status_speed & 0x4) {\n\t\ttemp_speed = (status_speed >> 4) & 0x3;")
            fn = fn.replace("if (temp_speed == IEEE_SPEED_1000)", "if (temp_speed == 2)")
            fn = fn.replace("else if(temp_speed == IEEE_SPEED_100)", "else if (temp_speed == 1)")
            # Program the RGMII-id delays AFTER the PHY soft-reset but BEFORE the link
            # finishes negotiating, so the RTL8211F latches the RX delay at link-up. A
            # post-link write (what net.c did) reads back set (reg0x15=0x0019) but never
            # takes effect -> MAC RXes nothing. Page 0xd08: reg0x11 bit8=TXDLY, 0x15 bit3=RXDLY.
            fn = fn.replace(
                "\t}\n\n\tXEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);\n\n\txil_printf(\"Waiting for PHY to complete autonegotiation",
                "\t}\n\n"
                "\t{ u16_t _d; /* NOVA_RTL8211F rgmii-id delays before link-up */\n"
                "\t  XEmacPs_PhyWrite(xemacpsp, phy_addr, 0x1f, 0xd08);\n"
                "\t  XEmacPs_PhyRead(xemacpsp, phy_addr, 0x11, &_d); XEmacPs_PhyWrite(xemacpsp, phy_addr, 0x11, (u16_t)(_d | 0x0100));\n"
                "\t  XEmacPs_PhyRead(xemacpsp, phy_addr, 0x15, &_d); XEmacPs_PhyWrite(xemacpsp, phy_addr, 0x15, (u16_t)(_d | 0x0008));\n"
                "\t  XEmacPs_PhyWrite(xemacpsp, phy_addr, 0x1f, 0x0); }\n\n"
                "\tXEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);\n\n\txil_printf(\"Waiting for PHY to complete autonegotiation")
            t = t[:i] + fn + t[j:]
        open(ps, "w").write(t); n += 1
    # Force recompile: cmake doesn't always notice a .c edit. Delete the stale
    # object + the archive so the 2nd build rebuilds physpeed + relinks liblwip220.
    for o in glob.glob(f"{WS}/**/xemacpsif_physpeed.c.obj", recursive=True): os.remove(o)
    for a in glob.glob(f"{WS}/**/liblwip220.a", recursive=True): os.remove(a)
    print("  patched", n, "physpeed.c for RTL8211F (+ removed stale obj/.a)")

def patch_fsbl_watchdog():
    # Vitis' Zynq FSBL starts the PS system watchdog during SD boot. The stock
    # stop path clears WDEN but leaves the reset-output bits set, which makes
    # boot-state diagnosis ambiguous and has left SD boots resetting during app
    # init. Force the hardware ZMR to a fully disabled value before bitstream
    # load and immediately before handoff; keep the app-level guard as a backup.
    n = 0
    for hooks in glob.glob(f"{WS}/**/zynq_fsbl/fsbl_hooks.c", recursive=True):
        t = open(hooks).read()
        if "NOVA_SWDT_FORCE_DISABLED" in t:
            continue
        t = t.replace('#include "fsbl_hooks.h"', '#include "fsbl_hooks.h"\n#include "xil_io.h"', 1)
        helper = (
            "\n#define NOVA_SWDT_FORCE_DISABLED 1\n"
            "static void NovaDisableSystemWatchdog(void)\n"
            "{\n"
            "\tXil_Out32(0xF8005000U, 0x00ABC000U);\n"
            "\tXil_Out32(0xF8005008U, 0x00001999U);\n"
            "}\n"
        )
        t = t.replace('#include "xil_io.h"', '#include "xil_io.h"' + helper, 1)
        t = t.replace(
            '\tfsbl_printf(DEBUG_INFO,"In FsblHookBeforeBitstreamDload function \\r\\n");',
            '\tNovaDisableSystemWatchdog();\n\tfsbl_printf(DEBUG_INFO,"NOVA: SWDT forced off before bitstream, ZMR=0x%08lx \\r\\n", Xil_In32(0xF8005000U));',
            1)
        t = t.replace(
            '\tfsbl_printf(DEBUG_INFO,"In FsblHookBeforeHandoff function \\r\\n");',
            '\tNovaDisableSystemWatchdog();\n\tfsbl_printf(DEBUG_INFO,"NOVA: SWDT forced off before handoff, ZMR=0x%08lx \\r\\n", Xil_In32(0xF8005000U));',
            1)
        open(hooks, "w").write(t); n += 1
    for o in glob.glob(f"{WS}/**/fsbl_hooks.c.obj", recursive=True): os.remove(o)
    for e in glob.glob(f"{WS}/**/fsbl.elf", recursive=True): os.remove(e)
    print("  patched", n, "FSBL hooks to force SWDT off")

plat.build()                 # 1st build regenerates lwipopts.h (DHCP=0)
patch_fsbl_watchdog()        # force PS SWDT fully off in the FSBL SD-boot path
patch_lwipopts()             # force DHCP=1 + timers
patch_physpeed()             # RTL8211E link/speed-resolved retry
plat.build()                 # rebuild: sources changed -> lib recompiles
print("=== post-2nd-build lwipopts (bsp/include) ===")
import subprocess as _sp
_sp.run(["grep","-nE","define (LWIP_DHCP|NO_SYS_NO_TIMERS)\\b",
         f"{WS}/nova_fio_plat/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/lwipopts.h"])
xpfm = glob.glob(f"{WS}/nova_fio_plat/export/nova_fio_plat/nova_fio_plat.xpfm")[0]
print("XPFM:", xpfm)

app = client.create_app_component(
    name="ps_fio", platform=xpfm, domain=DOMAIN, template="empty_application")

# Import our sources into the app's src/.
for f in ("main.c", "net.c", "mgmt.c", "debug.c", "drives.c", "drives.h", "ndi.h", "ndi_shim.cpp", "usb.c", "audio.c", "audio.h", "sid.c", "sid.h", "sidplay.cpp", "sidplay.h", "loader_bin.h", "modules_embedded.h", "ehbasic_rom.h", "boot_logo_nvg.h"):
    app.import_files(from_loc=SRC, files=[f], dest_dir_in_cmp="src")

# Reuse the ESP NovaHost's SID VM verbatim (the sandboxed 6502 + SID-poke mirror
# that already drives .sid playback on the ULX3S). No copy -- single source.
ESP_NOVAHOST = os.path.abspath(os.path.join(HERE, "..", "..", "..", "..", "e6502.ESP32", "novahost"))
for f in ("sid_vm.cpp", "sid_vm.h"):
    app.import_files(from_loc=ESP_NOVAHOST, files=[f], dest_dir_in_cmp="src")

# TinySoundFont (SF2 synth) + TinyMIDILoader, single-header libs (audio.c
# instantiates them). Kept under src/thirdparty/.
for f in ("tsf.h", "tml.h"):
    app.import_files(from_loc=os.path.join(SRC, "thirdparty"), files=[f], dest_dir_in_cmp="src/thirdparty")

# Reuse the cross-platform NDI parser straight from the ESP32 NovaHost tree
# (single source of truth — compiled here unchanged, NOT duplicated). ndi_shim.cpp
# binds it to FatFs + the C FIO host.
NOVAHOST = os.path.abspath(os.path.join(HERE, "..", "..", "..", "..", "e6502.ESP32", "novahost"))
for f in ("ndi_image.cpp", "ndi_image.h"):
    app.import_files(from_loc=NOVAHOST, files=[f], dest_dir_in_cmp="src")

# TSF loads a ~30 MB SF2 and converts its samples to float -> bump the bare-metal
# heap from the 8 KB default to 128 MB. DDR is 512 MB and the app + heap live
# below XRAM (0x10000000 = 256 MB), so 128 MB fits. _HEAP_SIZE is overridable in
# the generated lscript.ld (DEFINED(_HEAP_SIZE) ? ... : 0x2000).
_bumped = False
for _ld in glob.glob(f"{WS}/**/lscript.ld", recursive=True):
    _t = open(_ld).read()
    _t2 = _re.sub(r'(_HEAP_SIZE\s*=\s*DEFINED\(_HEAP_SIZE\)\s*\?\s*_HEAP_SIZE\s*:\s*)0x[0-9A-Fa-f]+',
                  r'\g<1>0x8000000', _t)
    if _t2 != _t:
        open(_ld, "w").write(_t2); _bumped = True
        print("bumped _HEAP_SIZE -> 0x8000000 in", _ld)
if not _bumped:
    print("WARNING: no lscript.ld _HEAP_SIZE bumped -- SF2 malloc may fail")

app.build()
print("ELF:", glob.glob(f"{WS}/ps_fio/build/*.elf"))
print("VITIS_PY_DONE")
