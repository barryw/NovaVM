connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 500
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
mwr -force 0x10000000 0xDEADBEEF
mwr -force 0x10000004 0xCAFEF00D
puts "DDR_TEST 0x10000000 = [mrd -value -force 0x10000000]"
puts "DDR_TEST 0x10000004 = [mrd -value -force 0x10000004]"
# Stage the FILES module image into XRAM slot 0 ($060000 -> DDR 0x10060000) so
# the lib_call(FILES) shelf HIT pages it in (shelf dir seeded in the loader image).
set ff [open "/home/barry/NovaVM/modules/files/files.bin" rb]
set fdata [read $ff]; close $ff
binary scan $fdata i* fwords
mwr -force 0x10060000 $fwords [llength $fwords]
puts "FILES staged @ 0x10060000: [llength $fwords] words; readback [mrd -value -force 0x10060000]"
fpga -file build/ps_full/ps_full.runs/impl_1/arty_z7_full.bit
after 3000
puts "BOOT_DONE"
exit
