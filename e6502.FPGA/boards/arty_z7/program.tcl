# program.tcl — program the Arty Z7-20 PL over JTAG (volatile, SRAM config).
# Requires a running hw_server (the Makefile starts one if needed).
#   vivado -mode batch -source program.tcl
# Override the bitstream with:  vivado -mode batch -source program.tcl -tclargs <path.bit>

set bit "build/arty_z7_smoke.bit"
if { $argc >= 1 } { set bit [lindex $argv 0] }

if { ![file exists $bit] } {
    puts "ERROR: bitstream not found: $bit (run 'make' first)"
    exit 1
}

open_hw_manager
connect_hw_server -url localhost:3121
current_hw_target [lindex [get_hw_targets] 0]
open_hw_target

# The xc7z020 PL device (skip arm_dap_0).
set dev [lindex [get_hw_devices xc7z020*] 0]
current_hw_device $dev
refresh_hw_device -update_hw_probes false $dev

set_property PROGRAM.FILE $bit $dev
puts "=== programming $dev with $bit ==="
program_hw_devices $dev
refresh_hw_device $dev

puts "=== done; DONE pin should be high ==="
close_hw_target
disconnect_hw_server
exit 0
