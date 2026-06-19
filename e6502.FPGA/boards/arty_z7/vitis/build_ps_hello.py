# build_ps_hello.py — Vitis 2024.2 PS smoke test (Hello World on Cortex-A9).
#   vitis -s build_ps_hello.py
#
# Uses the NEW unified Vitis Python CLI (the classic xsct platform/app flow is
# NOT supported by this install — "--classic ... only with full Vitis install").
# Builds a standalone platform from the PS7 XSA + a Hello World app -> ELF.
#
# Prereqs: build/arty_z7_ps.xsa from build_ps_bd.tcl. Use a FRESH workspace dir
# (the CLI rejects a workspace created by classic xsct).

import vitis, glob, os

WS  = os.environ.get("PS_WS", "/tmp/nova_ws2")
XSA = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "build", "arty_z7_ps.xsa"))

client = vitis.create_client()
client.set_workspace(WS)

plat = client.create_platform_component(
    name="nova_ps", hw_design=XSA, os="standalone", cpu="ps7_cortexa9_0")
plat.build()
xpfm = glob.glob(f"{WS}/nova_ps/export/nova_ps/nova_ps.xpfm")[0]
print("XPFM:", xpfm)

app = client.create_app_component(
    name="hello", platform=xpfm,
    domain="standalone_ps7_cortexa9_0", template="hello_world")
app.build()
print("ELF:", glob.glob(f"{WS}/hello/build/*.elf"))
print("VITIS_PY_DONE")
