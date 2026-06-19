# build_ps_bd.tcl — PS smoke test: minimal Zynq PS7 block design for Arty Z7-20.
#   vivado -mode batch -source build_ps_bd.tcl
# Applies the Arty Z7-20 board preset (DDR3 + standard MIO incl. UART0 + SD0),
# builds a (near-empty PL) bitstream, and exports an XSA for Vitis (FSBL +
# FreeRTOS app). Proves PS config + DDR + boot flow before the AXI-XRAM bridge.

set part       "xc7z020clg400-1"
set board      "digilentinc.com:arty-z7-20:part0:1.1"
set proj_dir   "build/ps_smoke"
set proj       "ps_smoke"

file mkdir build
create_project -force $proj $proj_dir -part $part
set_property board_part $board [current_project]

create_bd_design "ps"
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7 ps7
# Apply the Arty Z7-20 board preset: DDR3 timing/pins + default MIO peripherals
# (UART0/1, SD0, USB, etc.) and external FIXED_IO/DDR ports.
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 \
    -config { make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" } \
    [get_bd_cells ps7]

# Belt-and-suspenders: ensure UART0 is on (PS console -> FTDI /dev/ttyUSB1).
set_property -dict [list CONFIG.PCW_UART0_PERIPHERAL_ENABLE {1}] [get_bd_cells ps7]

# The preset leaves M_AXI_GP0 enabled; with AXI automation disabled its ACLK is
# dangling. Drive it from the PS fabric clock so the BD validates (GP0 stays
# available for the future FIO bridge).
connect_bd_net [get_bd_pins ps7/FCLK_CLK0] [get_bd_pins ps7/M_AXI_GP0_ACLK]

regenerate_bd_layout
validate_bd_design
save_bd_design

make_wrapper -files [get_files ${proj_dir}/${proj}.srcs/sources_1/bd/ps/ps.bd] -top
add_files -norecurse ${proj_dir}/${proj}.gen/sources_1/bd/ps/hdl/ps_wrapper.v
set_property top ps_wrapper [current_fileset]
update_compile_order -fileset sources_1

puts "=== synth + impl + bitstream ==="
launch_runs impl_1 -to_step write_bitstream -jobs 6
wait_on_run impl_1

set bit [glob -nocomplain ${proj_dir}/${proj}.runs/impl_1/*.bit]
puts "=== bitstream: $bit ==="

# Export hardware (XSA) with bitstream for Vitis platform creation.
write_hw_platform -fixed -include_bit -force build/arty_z7_ps.xsa
puts "=== XSA: build/arty_z7_ps.xsa ==="
puts "=== PS BD DONE ==="
