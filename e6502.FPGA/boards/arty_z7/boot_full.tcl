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
set ff [open "/home/barry/NovaVM/software/modules/files/files.bin" rb]
set fdata [read $ff]; close $ff
binary scan $fdata i* fwords
mwr -force 0x10060000 $fwords [llength $fwords]
puts "FILES staged @ 0x10060000: [llength $fwords] words; readback [mrd -value -force 0x10060000]"
# SYSTEM module -> XRAM slot 1 ($064000 -> DDR 0x10064000): READY line-reader +
# cursor (lib_call(SYSTEM)). shelf dir seeds SHELF_TAG[1]=SYSTEM in the loader.
set sf [open "/home/barry/NovaVM/software/modules/system/system.bin" rb]
set sdata [read $sf]; close $sf
binary scan $sdata i* swords
mwr -force 0x10064000 $swords [llength $swords]
puts "SYSTEM staged @ 0x10064000: [llength $swords] words; readback [mrd -value -force 0x10064000]"
fpga -file build/ps_full/ps_full.runs/impl_1/arty_z7_full.bit
after 3000
puts "BOOT_DONE"
exit
