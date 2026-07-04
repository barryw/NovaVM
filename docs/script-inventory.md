# Script Inventory

NovaVM does not use shell or Python files as user command entry points. Use
`nova` commands instead.

Allowed remaining Python files are integration hooks:

- `e6502.FPGA/boards/ulx3s/place_hdmi_serializer.py`: nextpnr `--pre-place`
  hook, selected by the ULX3S Makefile.
- `e6502.FPGA/boards/arty_z7/vitis/build_ps_fio.py`: Vitis Python hook invoked
  by `nova arty build-ps-fio`.
- `software/examples/novachess/vendor/chess6502-engine/tools/*.py`: vendored
  upstream chess-engine utilities, not NovaVM command entry points.

Do not add new `.sh` or `.py` files for repo operations. Add or reuse a `nova`
command.
