# build_full.tcl — full NovaVM-on-Zynq build: nova_core + axi_xram + PS7 BD.
# Run build_full_bd.tcl FIRST (creates build/ps_full project + ps_full_wrapper).
#   vivado -mode batch -source build_full.tcl
# Produces build/ps_full bitstream + build/arty_z7_full.xsa (for FSBL).

open_project build/ps_full/ps_full.xpr
set rtl  "../../rtl"
set hdmi "$rtl/thirdparty/hdl-util-hdmi"

# Ensure the BD products + wrapper exist and the wrapper is a project source
# (make_wrapper in the BD script generates it but doesn't always add it).
generate_target all [get_files -quiet *ps_full.bd]
add_files -norecurse build/ps_full/ps_full.gen/sources_1/bd/ps_full/hdl/ps_full_wrapper.v
reset_run synth_1

add_files -norecurse [list \
    "arty_z7_full.sv" "axi_xram.sv" "sid_stubs.sv" \
    "$rtl/dpram.sv" "$rtl/dpram_dc.sv" \
    "$rtl/vgc.sv" "$rtl/vgc_timing.sv" "$rtl/vgc_text.sv" "$rtl/vgc_gfx.sv" \
    "$rtl/artist.sv" "$rtl/vgc_sprites.sv" "$rtl/vgc_copper.sv" \
    "$rtl/blitter.sv" "$rtl/dma.sv" \
    "$rtl/page_in_ctrl.sv" "$rtl/page_dma.sv" "$rtl/fio.sv" \
    "$rtl/rom_load_cdc_fifo.sv" "$rtl/math_copro.sv" "$rtl/wts_chip.sv" "$rtl/nic.sv" \
    "$rtl/sdram/xram_sdram.sv" "$rtl/sdram/debug_sdram_port_b_cdc.sv" \
    "$rtl/top.sv" \
    "$rtl/arlet_6502/cpu.v" "$rtl/arlet_6502/ALU.v" "$rtl/thirdparty/sfifo.v" \
    "$hdmi/hdmi.sv" "$hdmi/tmds_channel.sv" "$hdmi/serializer.sv" \
    "$hdmi/packet_assembler.sv" "$hdmi/packet_picker.sv" \
    "$hdmi/audio_clock_regeneration_packet.sv" "$hdmi/audio_info_frame.sv" \
    "$hdmi/audio_sample_packet.sv" \
    "$hdmi/auxiliary_video_information_info_frame.sv" \
    "$hdmi/source_product_description_info_frame.sv" \
]
add_files -fileset constrs_1 -norecurse arty_z7_vgc.xdc

# Debug ILA on the axi_xram handshake (probe13 = awaddr, 32-bit; rest 1-bit).
if {[llength [get_ips -quiet ila_0]] == 0} {
    create_ip -name ila -vendor xilinx.com -library ip -module_name ila_0
}
set_property -dict [list \
    CONFIG.C_NUM_OF_PROBES {11} \
    CONFIG.C_DATA_DEPTH {4096} \
    CONFIG.C_PROBE0_WIDTH {16} \
    CONFIG.C_PROBE1_WIDTH {16} \
] [get_ips ila_0]
generate_target {all} [get_ips ila_0]

set_property top arty_z7_full [current_fileset]
update_compile_order -fileset sources_1

# $readmemh INIT_FILEs use relative "rom/..." paths. Project-mode synthesis runs
# in the run dir (not boards/arty_z7), so without help the BRAMs (EhBASIC ROM,
# fonts, char/color init) silently load nothing -> ROM=0x00 -> CPU BRK loop.
# Pre-synth hook: symlink the real rom/ into the synth run dir.
set rom_abs [file normalize ../../rom]
set pre_tcl [file normalize presyn_rom.tcl]
set fh [open $pre_tcl w]
# COPY the rom .hex into the run dir — do NOT symlink. `reset_run synth_1` cleans
# the run dir with Tcl `file delete -force`, which FOLLOWS a `rom` symlink and
# recursively deletes the real rom/ contents (this silently wiped every .hex ->
# ROM=0 -> CPU BRK loop for many builds). Copies are safe: the clean only removes
# the copies, never the source. rm -rf on the leftover (copy dir or symlink) is
# safe — without a trailing slash it never recurses a symlink's target.
puts $fh "catch { exec rm -rf rom }"
puts $fh "exec cp -r $rom_abs rom"
close $fh
set_property STEPS.SYNTH_DESIGN.TCL.PRE $pre_tcl [get_runs synth_1]

# Pass defines straight to synth_design (the fileset verilog_define property did
# not reach synthesis in project mode -> hdmi.sv SYNTHESIS paths broke). This is
# the same mechanism as the working non-project builds.
set_property -name {STEPS.SYNTH_DESIGN.ARGS.MORE OPTIONS} \
    -value {-verilog_define SYNTHESIS=1 -verilog_define VIDEO_720X480=1} \
    -objects [get_runs synth_1]

puts "=== launch impl -> bitstream ==="
launch_runs impl_1 -to_step write_bitstream -jobs 6
wait_on_run impl_1

if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
    puts "=== IMPL FAILED ==="
    exit 1
}
write_hw_platform -fixed -include_bit -force build/arty_z7_full.xsa
set bit [glob -nocomplain build/ps_full/ps_full.runs/impl_1/*.bit]
puts "=== FULL BUILD DONE: $bit  +  build/arty_z7_full.xsa ==="
exit 0
