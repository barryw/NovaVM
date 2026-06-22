# OOC synth of reDIP-SID (sid_chip) -> build/sid_ooc.dcp. Same isolation trick as
# the HDMI core. Vivado (unlike yosys) rejects the cores' `default_nettype none`
# with ANSI `logic` ports (Synth 8-6735), so we synth from sed'd COPIES that flip
# none->wire -- the originals (used by the ULX3S/yosys build) are untouched.
set rtl ../../rtl
set dst build/sid_ooc_src
exec rm -rf $dst
file mkdir $dst
exec cp -r $rtl/vendor/redip_sid $dst/redip
exec cp $rtl/sid/nova_redip_sid_api.sv $rtl/sid/sid_chip.sv $rtl/sid/sid_curve_reader.sv $dst/
# reDIP-SID's $readmemh paths are repo-root-relative ("rtl/vendor/redip_sid/..");
# they don't resolve from the Arty run dir. Point them at the copied .hex tables.
set redipabs [file normalize $dst/redip]
foreach f [concat [glob -nocomplain $dst/redip/*.sv] [glob -nocomplain $dst/redip/*.svh] \
                   [glob -nocomplain $dst/*.sv]] {
    exec sed -i {s/`default_nettype none/`default_nettype wire/} $f
    exec sed -i "s|rtl/vendor/redip_sid/|$redipabs/|g" $f
}
read_verilog -sv [list \
  $dst/redip/sid_pkg.sv $dst/redip/muladd.sv $dst/redip/sid_dac.sv $dst/redip/sid_control.sv \
  $dst/redip/sid_waveform.sv $dst/redip/sid_envelope.sv $dst/redip/sid_voice.sv \
  $dst/redip/sid_filter.sv $dst/redip/sid_pot.sv \
  $dst/nova_redip_sid_api.sv $dst/sid_chip.sv $dst/sid_curve_reader.sv ]
synth_design -mode out_of_context -top sid_chip -part xc7z020clg400-1 -verilog_define SYNTHESIS=1
write_checkpoint -force build/sid_ooc.dcp
puts "=== SID OOC DONE ==="
exit
