# build_cpu.tcl — full NovaVM SoC (top.sv) -> HDMI OUT on Arty Z7-20.
#   vivado -mode batch -source build_cpu.tcl  ->  build/arty_z7_cpu.bit
#
# Defines SYNTHESIS (BRAM inference + hdmi OSERDESE2 path) and VIDEO_720X480
# (VGC 858x525 timing). LATTICE_ECP5 stays undefined. redip_sid needs its dir
# on the include path (`include of .svh tables). $readmemh "rom/*" via ./rom.

set part      "xc7z020clg400-1"
set top       "arty_z7_cpu"
set build_dir "build"
set rtl       "../../rtl"
set redip     "$rtl/vendor/redip_sid"
set hdmi_dir  "$rtl/thirdparty/hdl-util-hdmi"

file mkdir $build_dir

puts "=== reading SystemVerilog sources ==="
# NOTE: the real reDIP SID (rtl/sid/*, rtl/vendor/redip_sid/*) is replaced by
# silent stubs (sid_stubs.sv) for the Arty bring-up — its package-typed ports
# don't elaborate under Vivado. SID/audio isn't needed to boot EhBASIC + video.
read_verilog -sv [list \
    "sid_stubs.sv" \
    "$rtl/dpram.sv" "$rtl/dpram_dc.sv" \
    "$rtl/vgc.sv" "$rtl/vgc_timing.sv" "$rtl/vgc_text.sv" "$rtl/vgc_gfx.sv" \
    "$rtl/artist.sv" "$rtl/vgc_sprites.sv" "$rtl/vgc_copper.sv" \
    "$rtl/blitter.sv" "$rtl/dma.sv" \
    "$rtl/page_in_ctrl.sv" "$rtl/page_dma.sv" "$rtl/fio.sv" \
    "$rtl/rom_load_cdc_fifo.sv" "$rtl/math_copro.sv" "$rtl/wts_chip.sv" "$rtl/nic.sv" \
    "$rtl/sdram/xram_sdram.sv" "$rtl/sdram/debug_sdram_port_b_cdc.sv" \
    "$rtl/top.sv" \
    "arty_z7_cpu.sv" \
    "$hdmi_dir/hdmi.sv" "$hdmi_dir/tmds_channel.sv" "$hdmi_dir/serializer.sv" \
    "$hdmi_dir/packet_assembler.sv" "$hdmi_dir/packet_picker.sv" \
    "$hdmi_dir/audio_clock_regeneration_packet.sv" "$hdmi_dir/audio_info_frame.sv" \
    "$hdmi_dir/audio_sample_packet.sv" \
    "$hdmi_dir/auxiliary_video_information_info_frame.sv" \
    "$hdmi_dir/source_product_description_info_frame.sv" \
]

puts "=== reading Verilog sources ==="
read_verilog [list \
    "$rtl/arlet_6502/cpu.v" "$rtl/arlet_6502/ALU.v" "$rtl/thirdparty/sfifo.v" \
]

read_xdc "arty_z7_vgc.xdc"   ;# same ports (clk/btn/led/hdmi_tx_*)

puts "=== synth_design (SYNTHESIS + VIDEO_720X480, redip on include path) ==="
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
report_utilization     -file "$build_dir/${top}_util.rpt"
report_drc             -file "$build_dir/${top}_drc.rpt"

puts "=== write_bitstream ==="
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
write_bitstream -force "$build_dir/${top}.bit"
set wns [get_property SLACK [get_timing_paths -max_paths 1 -nworst 1 -setup]]
puts "=== DONE: $build_dir/${top}.bit  (setup WNS = $wns ns) ==="
