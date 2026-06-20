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
        # Force 100Mbps full-duplex: the RTL8211E autoneg speed-read (reg 0x11)
        # fails in the Xilinx driver, but the forced path (configure_IEEE_phy_speed)
        # skips it. 100BASE-TX can be forced (1000 can't). Plenty for NDI uploads.
        t = _re.sub(r'#define\s+CONFIG_LINKSPEED_AUTODETECT\s+\d+', '#define CONFIG_LINKSPEED100 1', t)
        # Enable link stats so the app can see TX/RX/drop counts (RX-vs-TX debug).
        t = _re.sub(r'#define\s+LWIP_STATS\s+\d+', '#define LWIP_STATS 1', t)
        t = _re.sub(r'#define\s+LINK_STATS\s+\d+', '#define LINK_STATS 1', t)
        open(lo, "w").write(t); n += 1
    print("  patched", n, "lwipopts.h")

def patch_physpeed():
    # Arty Z7 RTL8211E: get_Realtek_phy_speed reads reg 0x11 ONCE right after
    # autoneg-complete and fails if the link/speed-resolved bit (0x400) isn't set
    # yet (it lags autoneg-complete). Retry the read, and print the reg value.
    n = 0
    for ps in glob.glob(f"{WS}/**/xemacpsif_physpeed.c", recursive=True):
        t = open(ps).read()
        if "NOVA_RTL_RETRY" in t:
            continue
        # (1) print the detected PHY address + identity at the dispatch.
        disp = "\tif (phy_identity == PHY_TI_IDENTIFIER) {"
        if disp in t:
            t = t.replace(disp,
                "\t/* NOVA_RTL_RETRY */ xil_printf(\"NOVA PHY addr=%d id1=0x%x\\r\\n\", (int)phy_addr, phy_identity);\n" + disp, 1)
        # (2) Realtek: retry the link/speed-resolved read after autoneg.
        marker = "\tif (status_speed & 0x400) {"
        if marker in t:
            ins = ("\t{ u32_t _rt = 0;\n"
                   "\t  while (!(status_speed & 0x400) && _rt++ < 10) { sleep(1);\n"
                   "\t    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_SPECIFIC_STATUS_REG, &status_speed); } }\n"
                   "\txil_printf(\"Realtek reg0x11=0x%x\\r\\n\", status_speed);\n"
                   + marker)
            t = t.replace(marker, ins, 1)
        open(ps, "w").write(t); n += 1
    # Force recompile: cmake doesn't always notice a .c edit (it did for the
    # header-triggered lwipopts recompile, not for this source). Delete the stale
    # object + the archive so the 2nd build rebuilds physpeed + relinks liblwip220.
    for o in glob.glob(f"{WS}/**/xemacpsif_physpeed.c.obj", recursive=True): os.remove(o)
    for a in glob.glob(f"{WS}/**/liblwip220.a", recursive=True): os.remove(a)
    print("  patched", n, "physpeed.c (+ removed stale obj/.a)")

plat.build()                 # 1st build regenerates lwipopts.h (DHCP=0)
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
for f in ("main.c", "net.c", "loader_bin.h", "modules_embedded.h"):
    app.import_files(from_loc=SRC, files=[f], dest_dir_in_cmp="src")

app.build()
print("ELF:", glob.glob(f"{WS}/ps_fio/build/*.elf"))
print("VITIS_PY_DONE")
