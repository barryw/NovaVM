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

# Enable FatFs (xilffs) in the standalone domain BSP for microSD access.
try:
    domain = plat.get_domain(name=DOMAIN)
    domain.set_lib(lib_name="xilffs")
    # Enable Long File Name support so f_open matches names like "system.nmod"
    # (without LFN, FatFs only sees 8.3 short names like SYSTEM~1.NMO).
    domain.set_config(option="lib", param="XILFFS_use_lfn", value="2", lib_name="xilffs")
    domain.regenerate()   # regenerate BSP sources (ffconf.h) with the new config
    print("xilffs + LFN enabled on", DOMAIN)
except Exception as e:
    print("WARN: could not set xilffs/LFN via get_domain:", e)

plat.build()
xpfm = glob.glob(f"{WS}/nova_fio_plat/export/nova_fio_plat/nova_fio_plat.xpfm")[0]
print("XPFM:", xpfm)

app = client.create_app_component(
    name="ps_fio", platform=xpfm, domain=DOMAIN, template="empty_application")

# Import our sources into the app's src/.
for f in ("main.c", "loader_bin.h", "modules_embedded.h"):
    app.import_files(from_loc=SRC, files=[f], dest_dir_in_cmp="src")

app.build()
print("ELF:", glob.glob(f"{WS}/ps_fio/build/*.elf"))
print("VITIS_PY_DONE")
