# boot_fio_noreset.tcl -- boot ps_fio WITHOUT a full system reset.
# boot_fio_por.tcl does `rst -srst` which bounces the PS GEM/PHY and degrades the
# Ethernet link (U-Boot, which only does `rst -processor`, keeps a clean 2-LED link
# and RXes fine). This variant mirrors U-Boot: processor-only reset, ps7_init, load
# the PL bitstream (GEM is PS so this shouldn't disturb the PHY), then run ps_fio.
connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 800
source [file join [file dirname [info script]] ps7_init.tcl]
ps7_init
ps7_post_config
fpga -file build/ps_full/ps_full.runs/impl_1/arty_z7_full.bit
after 1000
dow /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf
con
after 4000
puts "BOOT_FIO_DONE"
exit
