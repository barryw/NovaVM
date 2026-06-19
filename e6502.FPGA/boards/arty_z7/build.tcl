# build.tcl — non-project Vivado flow for the Arty Z7-20 smoke test.
# Run from this directory:  vivado -mode batch -source build.tcl
# Produces build/arty_z7_smoke.bit

set part      "xc7z020clg400-1"
set top       "arty_z7_smoke"
set build_dir "build"
set rtl_dir   "../../rtl"

file mkdir $build_dir

puts "=== reading sources ==="
read_verilog -sv [list \
    "arty_z7_smoke.sv" \
    "$rtl_dir/uart_rx.sv" \
    "$rtl_dir/uart_tx.sv" \
]
read_xdc "arty_z7_smoke.xdc"

puts "=== synth_design ==="
synth_design -top $top -part $part -flatten_hierarchy rebuilt
write_checkpoint -force "$build_dir/${top}_synth.dcp"
report_utilization -file "$build_dir/${top}_util_synth.rpt"

puts "=== opt / place / route ==="
opt_design
place_design
phys_opt_design
route_design

write_checkpoint -force "$build_dir/${top}_routed.dcp"
report_timing_summary -file "$build_dir/${top}_timing.rpt"
report_utilization     -file "$build_dir/${top}_util.rpt"
report_drc             -file "$build_dir/${top}_drc.rpt"

puts "=== write_bitstream ==="
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
write_bitstream -force "$build_dir/${top}.bit"

# Emit worst-case slack so the batch log makes timing closure obvious.
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1 -setup]]
puts "=== DONE: $build_dir/${top}.bit  (setup WNS = $wns ns) ==="
