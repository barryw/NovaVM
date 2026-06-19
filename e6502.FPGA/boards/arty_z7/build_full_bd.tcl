# build_full_bd.tcl — PS7 + S_AXI_HP0 + SmartConnect block design for the full
# NovaVM-on-Zynq integration. Exposes:
#   - a 32-bit AXI4 SLAVE (for the PL axi_xram master) via a SmartConnect that
#     adapts to the 64-bit AXI3 S_AXI_HP0 DDR port,
#   - hp_aclk : AXI clock INPUT (driven by the PL MMCM pixel clock),
#   - FCLK_CLK0 / FCLK_RESET0_N outputs (PS fabric clock/reset),
#   - DDR / FIXED_IO external.
# Generates a wrapper (ps_full_wrapper) the RTL top instantiates.
#   vivado -mode batch -source build_full_bd.tcl   (BD + wrapper only)

set part  "xc7z020clg400-1"
set board "digilentinc.com:arty-z7-20:part0:1.1"
set pd    "build/ps_full"

file mkdir build
create_project -force ps_full $pd -part $part
set_property board_part $board [current_project]

create_bd_design "ps_full"

# --- Zynq PS7 ---------------------------------------------------------------
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7 ps7
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 \
    -config { make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" } \
    [get_bd_cells ps7]
# Enable one HP slave port (PL -> DDR) and keep GP0 (future FIO bridge).
set_property -dict [list \
    CONFIG.PCW_USE_S_AXI_HP0 {1} \
    CONFIG.PCW_USE_M_AXI_GP0 {1} \
] [get_bd_cells ps7]

# --- AXI SmartConnect: 32-bit AXI4 slave (PL) -> 64-bit AXI3 HP0 ------------
create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smc
set_property -dict [list CONFIG.NUM_SI {1} CONFIG.NUM_MI {1}] [get_bd_cells smc]
connect_bd_intf_net [get_bd_intf_pins smc/M00_AXI] [get_bd_intf_pins ps7/S_AXI_HP0]

# --- Clocks/resets ----------------------------------------------------------
# PL-driven AXI clock for the HP path + SmartConnect + the PL axi_xram master.
create_bd_port -dir I -type clk hp_aclk
# GP0 ACLK from PS fabric clock (GP0 unused for now but needs a clock).
connect_bd_net [get_bd_pins ps7/FCLK_CLK0] [get_bd_pins ps7/M_AXI_GP0_ACLK]
# HP0 + SmartConnect on the PL clock.
connect_bd_net [get_bd_ports hp_aclk] [get_bd_pins ps7/S_AXI_HP0_ACLK]
connect_bd_net [get_bd_ports hp_aclk] [get_bd_pins smc/aclk]

# Reset for the SmartConnect, synced to hp_aclk from PS reset.
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rstgen
connect_bd_net [get_bd_ports hp_aclk]            [get_bd_pins rstgen/slowest_sync_clk]
connect_bd_net [get_bd_pins ps7/FCLK_RESET0_N]   [get_bd_pins rstgen/ext_reset_in]
connect_bd_net [get_bd_pins rstgen/interconnect_aresetn] [get_bd_pins smc/aresetn]

# Expose the SmartConnect 32-bit slave for the PL axi_xram master.
make_bd_intf_pins_external -name S_AXI_XRAM [get_bd_intf_pins smc/S00_AXI]
# Expose PS fabric clock + reset for the PL.
make_bd_pins_external -name fclk_clk0    [get_bd_pins ps7/FCLK_CLK0]
make_bd_pins_external -name fclk_resetn  [get_bd_pins ps7/FCLK_RESET0_N]

# Map the external XRAM master's address space onto the HP0 DDR segment (full
# 1 GB DDR range so XRAM_BASE=0x10000000 is reachable). The master is external,
# so Vivado won't auto-assign — force it into the S_AXI_XRAM external space.
puts "=== addr spaces ==="
foreach s [get_bd_addr_spaces] { puts "SPACE: $s" }
set xspace [get_bd_addr_spaces -quiet *S_AXI_XRAM*]
puts "XSPACE: $xspace"
assign_bd_address -force -target_address_space $xspace \
    [get_bd_addr_segs ps7/S_AXI_HP0/HP0_DDR_LOWOCM] \
    -range 1G -offset 0x00000000
puts "=== assigned segs ==="
foreach seg [get_bd_addr_segs -of_objects $xspace] { puts "SEG: $seg" }

regenerate_bd_layout
validate_bd_design
save_bd_design

make_wrapper -files [get_files ${pd}/ps_full.srcs/sources_1/bd/ps_full/ps_full.bd] -top
puts "WRAPPER: ${pd}/ps_full.gen/sources_1/bd/ps_full/hdl/ps_full_wrapper.v"
puts "=== exposed wrapper ports ==="
# Print the wrapper port list so the RTL top can match the S_AXI_XRAM signal names.
set wf [glob ${pd}/ps_full.gen/sources_1/bd/ps_full/hdl/ps_full_wrapper.v]
set fh [open $wf r]; set txt [read $fh]; close $fh
foreach line [split $txt "\n"] {
    if {[regexp {(input|output|inout)} $line]} { puts "PORT: [string trim $line]" }
}
puts "=== BD DONE ==="
exit
