set script_dir [file dirname [file normalize [info script]]]
set build_dir [file join $script_dir build]
set bitstream [file join $build_dir arty_z7_blinky.bit]

create_project -force arty_z7_blinky $build_dir -part xc7z020clg400-1
add_files [file join $script_dir arty_z7_blinky.v]
add_files -fileset constrs_1 [file join $script_dir arty_z7_blinky.xdc]

update_compile_order -fileset sources_1
synth_design -top arty_z7_blinky -part xc7z020clg400-1
opt_design
place_design
route_design
file mkdir $build_dir
write_bitstream -force $bitstream

open_hw_manager
connect_hw_server -allow_non_jtag
set targets [get_hw_targets]
if {[llength $targets] == 0} {
    error "No Vivado hardware targets found"
}

current_hw_target [lindex $targets 0]
open_hw_target

set devices [get_hw_devices xc7z020*]
if {[llength $devices] == 0} {
    error "No xc7z020 hardware device found"
}

current_hw_device [lindex $devices 0]
set_property PROGRAM.FILE $bitstream [current_hw_device]
program_hw_devices [current_hw_device]
refresh_hw_device [current_hw_device]

close_hw_target
disconnect_hw_server

puts "PROGRAMMED_ARTY_Z7_BLINKY=$bitstream"
