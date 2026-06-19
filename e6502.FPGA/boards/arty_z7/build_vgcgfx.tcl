# build_vgcgfx.tcl — VGC graphics demo (mode 3) -> HDMI OUT on Arty Z7-20.
#   vivado -mode batch -source build_vgcgfx.tcl  ->  build/arty_z7_vgc_gfx.bit
# Same source set / defines as build_vgc.tcl; reuses arty_z7_vgc.xdc (same ports).

set part      "xc7z020clg400-1"
set top       "arty_z7_vgc_gfx"
set build_dir "build"
set rtl_dir   "../../rtl"
set hdmi_dir  "$rtl_dir/thirdparty/hdl-util-hdmi"

file mkdir $build_dir

puts "=== reading sources ==="
read_verilog -sv [list \
    "arty_z7_vgc_gfx.sv" \
    "$rtl_dir/vgc.sv" "$rtl_dir/vgc_timing.sv" "$rtl_dir/vgc_text.sv" \
    "$rtl_dir/vgc_gfx.sv" "$rtl_dir/vgc_sprites.sv" "$rtl_dir/vgc_copper.sv" \
    "$rtl_dir/artist.sv" "$rtl_dir/dpram.sv" "$rtl_dir/thirdparty/sfifo.v" \
    "$hdmi_dir/hdmi.sv" "$hdmi_dir/tmds_channel.sv" "$hdmi_dir/serializer.sv" \
    "$hdmi_dir/packet_assembler.sv" "$hdmi_dir/packet_picker.sv" \
    "$hdmi_dir/audio_clock_regeneration_packet.sv" "$hdmi_dir/audio_info_frame.sv" \
    "$hdmi_dir/audio_sample_packet.sv" \
    "$hdmi_dir/auxiliary_video_information_info_frame.sv" \
    "$hdmi_dir/source_product_description_info_frame.sv" \
]
read_xdc "arty_z7_vgc.xdc"

puts "=== synth_design (SYNTHESIS + VIDEO_720X480) ==="
synth_design -top $top -part $part \
    -verilog_define SYNTHESIS=1 -verilog_define VIDEO_720X480=1 \
    -flatten_hierarchy rebuilt
report_utilization -file "$build_dir/${top}_util_synth.rpt"

puts "=== opt / place / route ==="
opt_design
place_design
phys_opt_design
route_design
report_timing_summary -file "$build_dir/${top}_timing.rpt"
report_drc            -file "$build_dir/${top}_drc.rpt"

puts "=== write_bitstream ==="
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
write_bitstream -force "$build_dir/${top}.bit"
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1 -setup]]
puts "=== DONE: $build_dir/${top}.bit  (setup WNS = $wns ns) ==="
