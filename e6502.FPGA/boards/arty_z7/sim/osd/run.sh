#!/bin/bash
# Stage-1 OSD sim gate (Verilator), run from this dir:
#   ./run.sh
# 1) renders the compositor menu to osd.png (visual proof of osd_overlay.sv)
# 2) unit-tests the parked debounce + system_pause gates (osd_logic.sv)
set -e
cd "$(dirname "$0")"

cp -f ../../osd_overlay.sv osd_overlay.sv      # the live compositor (no stale copy in git)
ln -sf ../rom rom                              # VGC font for $readmemh "rom/fonts.hex"

VFLAGS="-Wno-WIDTH -Wno-UNUSED -Wno-CASEINCOMPLETE -Wno-UNOPTFLAT -Wno-fatal"

echo "=== compositor render -> osd.png ==="
verilator --cc --exe --build $VFLAGS --Mdir obj_overlay \
  --top-module osd_overlay osd_overlay.sv osd_tb.cpp -o osd_sim
./obj_overlay/osd_sim
python3 -c "from PIL import Image; Image.open('osd.ppm').save('osd.png')" 2>/dev/null \
  && echo "wrote osd.png" || echo "(PIL missing; osd.ppm only)"

echo "=== debounce + system_pause unit tests ==="
verilator --cc --exe --build $VFLAGS --Mdir obj_logic \
  --top-module osd_logic osd_logic.sv osd_logic_tb.cpp -o osd_logic_sim
./obj_logic/osd_logic_sim
