# Re-run impl only (synth_1 already passed) with the OOC hdmi dcp linked into the
# black-box cell before opt_design.
open_project build/ps_full/ps_full.xpr
reset_run impl_1
set impl_pre [file normalize impl_pre_hdmi.tcl]
set ph [open $impl_pre w]
puts $ph "read_checkpoint -cell hdmi_inst [file normalize build/hdmi_ooc.dcp]"
close $ph
set_property STEPS.OPT_DESIGN.TCL.PRE $impl_pre [get_runs impl_1]
launch_runs impl_1 -to_step write_bitstream -jobs 6
wait_on_run impl_1
if {[get_property PROGRESS [get_runs impl_1]] != "100%"} { puts "=== IMPL FAILED ==="; exit 1 }
write_hw_platform -fixed -include_bit -force build/arty_z7_full.xsa
puts "=== IMPL DONE: [glob -nocomplain build/ps_full/ps_full.runs/impl_1/*.bit] ==="
exit 0
