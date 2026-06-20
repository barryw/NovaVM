connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
catch { rst -srst }
after 1500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 800
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
fpga -file build/ps_full/ps_full.runs/impl_1/arty_z7_full.bit
after 1000
dow /tmp/nova_fio_ws/ps_fio/build/ps_fio.elf
con
after 4000
puts "BOOT_FIO_DONE"
exit
