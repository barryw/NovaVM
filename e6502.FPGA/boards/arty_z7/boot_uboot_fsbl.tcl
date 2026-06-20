connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 500
dow /tmp/nova_fio_ws/nova_fio_plat/zynq_fsbl/build/fsbl.elf
con
after 3000
stop
after 500
dow /tmp/u-boot-xlnx/u-boot.elf
con
after 3000
puts "UBOOT_FSBL_DONE"
exit
