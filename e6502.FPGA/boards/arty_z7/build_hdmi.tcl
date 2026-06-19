# build_hdmi.tcl — non-project Vivado flow for the Arty Z7-20 HDMI test.
#   vivado -mode batch -source build_hdmi.tcl
# Produces build/arty_z7_hdmi_test.bit
#
# IMPORTANT: SYNTHESIS must be defined (and LATTICE_ECP5 must NOT be) so the
# hdl-util/hdmi serializer selects the Xilinx OSERDESE2 path.

set part      "xc7z020clg400-1"
set top       "arty_z7_hdmi_test"
set build_dir "build"
set rtl_dir   "../../rtl"
set hdmi_dir  "$rtl_dir/thirdparty/hdl-util-hdmi"

file mkdir $build_dir

puts "=== reading sources ==="
read_verilog -sv [list \
    "arty_z7_hdmi_test.sv" \
    "$hdmi_dir/hdmi.sv" \
    "$hdmi_dir/tmds_channel.sv" \
    "$hdmi_dir/serializer.sv" \
    "$hdmi_dir/packet_assembler.sv" \
    "$hdmi_dir/packet_picker.sv" \
    "$hdmi_dir/audio_clock_regeneration_packet.sv" \
    "$hdmi_dir/audio_info_frame.sv" \
    "$hdmi_dir/audio_sample_packet.sv" \
    "$hdmi_dir/auxiliary_video_information_info_frame.sv" \
    "$hdmi_dir/source_product_description_info_frame.sv" \
]
read_xdc "arty_z7_hdmi_test.xdc"

puts "=== synth_design (SYNTHESIS defined, LATTICE_ECP5 undefined) ==="
synth_design -top $top -part $part -verilog_define SYNTHESIS=1 -flatten_hierarchy rebuilt
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

set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1 -setup]]
puts "=== DONE: $build_dir/${top}.bit  (setup WNS = $wns ns) ==="
