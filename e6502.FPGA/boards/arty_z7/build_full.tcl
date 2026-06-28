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

# Vivado auto-sets INCREMENTAL_CHECKPOINT to the previous synth result after every
# build, so the next build runs incremental synthesis -- which once silently kept
# a stale vgc_copper across a real logic change, shipping PL logic that did NOT
# match the source even though a clean sim had proven the fix (cost a whole
# build+flash+power-cycle to catch). On an FPGA that is never worth the speedup.
# Clear it so every build is a full synth from source.
set_property INCREMENTAL_CHECKPOINT "" [get_runs synth_1]

# The project persists across builds: drop the silent sid_stubs.sv that earlier
# builds added, or it duplicates sid_chip/sid_curve_reader and Vivado keeps the
# stub (-> no SID audio).
remove_files -quiet [get_files -quiet *sid_stubs.sv]
remove_files -quiet [get_files -quiet *osd_overlay.sv]   ;# drop OSD compositor cruft from persistent project

# --- reDIP-SID: the real cycle-accurate core in place of sid_stubs.sv. Vivado
# rejects the cores' SystemVerilog, so synth from sed'd COPIES (default_nettype
# none->wire in .sv + .svh; repo-root-relative $readmemh paths -> the copies).
# Originals (ULX3S/yosys) untouched.
set sdst build/sid_full_src
file delete -force $sdst
file mkdir $sdst
exec cp -r $rtl/vendor/redip_sid $sdst/redip
exec cp $rtl/sid/nova_redip_sid_api.sv $rtl/sid/sid_chip.sv $rtl/sid/sid_curve_reader.sv $sdst/
set redipabs [file normalize $sdst/redip]
foreach f [concat [glob -nocomplain $sdst/redip/*.sv] [glob -nocomplain $sdst/redip/*.svh] \
                   [glob -nocomplain $sdst/*.sv]] {
    exec sed -i {s/`default_nettype none/`default_nettype wire/} $f
    exec sed -i "s|rtl/vendor/redip_sid/|$redipabs/|g" $f
}
add_files -norecurse [list \
    $sdst/redip/sid_pkg.sv $sdst/redip/muladd.sv $sdst/redip/sid_dac.sv $sdst/redip/sid_control.sv \
    $sdst/redip/sid_waveform.sv $sdst/redip/sid_envelope.sv $sdst/redip/sid_voice.sv \
    $sdst/redip/sid_filter.sv $sdst/redip/sid_pot.sv \
    $sdst/nova_redip_sid_api.sv $sdst/sid_chip.sv $sdst/sid_curve_reader.sv]

add_files -norecurse [list \
    "arty_z7_full.sv" "axi_xram.sv" "fio_bridge.sv" "osd_overlay.sv" "hdmi_bb.sv" \
    "$rtl/dpram.sv" "$rtl/dpram_dc.sv" \
    "$rtl/vgc.sv" "$rtl/vgc_timing.sv" "$rtl/vgc_text.sv" "$rtl/vgc_gfx.sv" \
    "$rtl/artist.sv" "$rtl/vgc_sprites.sv" "$rtl/vgc_copper.sv" \
    "$rtl/blitter.sv" "$rtl/dma.sv" \
    "$rtl/page_in_ctrl.sv" "$rtl/page_dma.sv" "$rtl/fio.sv" \
    "$rtl/rom_load_cdc_fifo.sv" "$rtl/math_copro.sv" "$rtl/wts_chip.sv" "$rtl/nic.sv" \
    "$rtl/audio_pcm_fifo.sv" "$rtl/sid_hdmi_audio.sv" \
    "$rtl/sdram/xram_sdram.sv" "$rtl/sdram/debug_sdram_port_b_cdc.sv" \
    "$rtl/top.sv" \
    "$rtl/arlet_6502/cpu.v" "$rtl/arlet_6502/ALU.v" "$rtl/thirdparty/sfifo.v" \
]
# hdmi is synthesized out-of-context (synth_hdmi_ooc.tcl -> build/hdmi_ooc.dcp)
# because Vivado synth intermittently crashes on hdl-util-hdmi tmds_channel
# (8-6156, no cause) in the full design once fio_bridge is present; OOC isolates
# it (hdmi synthesizes cleanly alone). hdmi_bb.sv is the black-box stub used in
# synth; the dcp supplies the netlist at implementation only.
add_files -norecurse build/hdmi_ooc.dcp
set_property used_in_synthesis false [get_files build/hdmi_ooc.dcp]
add_files -fileset constrs_1 -norecurse arty_z7_vgc.xdc

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

# SYNTHESIS / VIDEO_720X480 defines for hdl-util-hdmi (OSERDESE2 + 720x480).
set_property -name {STEPS.SYNTH_DESIGN.ARGS.MORE OPTIONS} \
    -value {-verilog_define SYNTHESIS=1 -verilog_define VIDEO_720X480=1} \
    -objects [get_runs synth_1]

# Link the OOC hdmi checkpoint into the black-box cell before opt_design (a manual
# dcp added as a source is NOT auto-linked in the launch_runs flow -> DRC INBB-3).
set impl_pre [file normalize impl_pre_hdmi.tcl]
set ph [open $impl_pre w]
puts $ph "read_checkpoint -cell hdmi_inst [file normalize build/hdmi_ooc.dcp]"
close $ph
set_property STEPS.OPT_DESIGN.TCL.PRE $impl_pre [get_runs impl_1]

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
