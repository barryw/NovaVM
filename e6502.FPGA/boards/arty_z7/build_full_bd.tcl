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
# TTC0: required by the FreeRTOS BSP for the systick (xiltimer); internal clock,
# no EMIO/PL pins. The A9 SCU timer alone doesn't satisfy the FreeRTOS DRC.
# WDT: expose the PS system watchdog to the generated BSP/FSBL so XWdtPs-based
# watchdog shutdown code is compiled in, in addition to the app-level SWDT guard.
set_property -dict [list \
    CONFIG.PCW_USE_S_AXI_HP0 {1} \
    CONFIG.PCW_USE_M_AXI_GP0 {1} \
    CONFIG.PCW_EN_TTC0 {1} \
    CONFIG.PCW_TTC0_PERIPHERAL_ENABLE {1} \
    CONFIG.PCW_WDT_PERIPHERAL_ENABLE {1} \
] [get_bd_cells ps7]

# --- AXI SmartConnect: 32-bit AXI4 slaves (PL) -> 64-bit AXI3 HP0 -----------
create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smc
set_property -dict [list CONFIG.NUM_SI {2} CONFIG.NUM_MI {1}] [get_bd_cells smc]
connect_bd_intf_net [get_bd_intf_pins smc/M00_AXI] [get_bd_intf_pins ps7/S_AXI_HP0]

# --- Clocks/resets ----------------------------------------------------------
# PL-driven AXI clock for the HP path + SmartConnect + the PL axi_xram master.
# Everything (HP0, GP0, fio_bridge) runs on hp_aclk = the PL pixel clock so the
# whole PS<->PL fabric shares the 6502's clock domain (no CDC).
create_bd_port -dir I -type clk hp_aclk
connect_bd_net [get_bd_ports hp_aclk] [get_bd_pins ps7/M_AXI_GP0_ACLK]
connect_bd_net [get_bd_ports hp_aclk] [get_bd_pins ps7/S_AXI_HP0_ACLK]
connect_bd_net [get_bd_ports hp_aclk] [get_bd_pins smc/aclk]

# Reset for the SmartConnect, synced to hp_aclk from PS reset.
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset rstgen
connect_bd_net [get_bd_ports hp_aclk]            [get_bd_pins rstgen/slowest_sync_clk]
connect_bd_net [get_bd_pins ps7/FCLK_RESET0_N]   [get_bd_pins rstgen/ext_reset_in]
connect_bd_net [get_bd_pins rstgen/interconnect_aresetn] [get_bd_pins smc/aresetn]

# --- GP0 -> fio_bridge: AXI3 -> AXI4-Lite via a protocol converter ----------
# PS M_AXI_GP0 (AXI3) drives the PL fio_bridge (AXI4-Lite). Convert + expose the
# AXI4-Lite master externally for the RTL top to wire to fio_bridge.
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_protocol_converter conv
set_property -dict [list CONFIG.SI_PROTOCOL {AXI3} CONFIG.MI_PROTOCOL {AXI4LITE}] [get_bd_cells conv]
connect_bd_intf_net [get_bd_intf_pins ps7/M_AXI_GP0] [get_bd_intf_pins conv/S_AXI]
connect_bd_net [get_bd_ports hp_aclk]                    [get_bd_pins conv/aclk]
connect_bd_net [get_bd_pins rstgen/peripheral_aresetn]  [get_bd_pins conv/aresetn]
make_bd_intf_pins_external -name M_AXI_FIO [get_bd_intf_pins conv/M_AXI]
# (fio_bridge is reset from the PL ~reset in arty_z7_full — no BD reset port.)

# Expose SmartConnect 32-bit slaves for the PL axi_xram and HDMI capture masters.
make_bd_intf_pins_external -name S_AXI_XRAM [get_bd_intf_pins smc/S00_AXI]
make_bd_intf_pins_external -name S_AXI_CAPTURE [get_bd_intf_pins smc/S01_AXI]
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
set cspace [get_bd_addr_spaces -quiet *S_AXI_CAPTURE*]
puts "CSPACE: $cspace"
assign_bd_address -force -target_address_space $cspace \
    [get_bd_addr_segs ps7/S_AXI_HP0/HP0_DDR_LOWOCM] \
    -range 1G -offset 0x00000000
puts "=== assigned segs ==="
foreach seg [get_bd_addr_segs -of_objects $xspace] { puts "SEG: $seg" }

# Map the fio_bridge (external GP0 slave) at 0x40000000 in the PS GP0 space so
# the PS A9 reaches it at that base (PS app FIO_BASE = 0x40000000).
assign_bd_address -target_address_space [get_bd_addr_spaces ps7/Data] \
    [get_bd_addr_segs M_AXI_FIO/Reg] -range 128K -offset 0x40000000

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
