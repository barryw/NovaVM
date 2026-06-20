connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 500
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
after 300
mwr 0xF8000008 0xDF0D
dow /tmp/u-boot-xlnx/u-boot.elf
con
after 9000
stop
puts "UBOOT NETCTRL(0x04)=[mrd -value 0xE000B004]  NETCFG(0x00)=[mrd -value 0xE000B000]"
puts "UBOOT DMACFG(0x10)=[mrd -value 0xE000B010]  TXQBASE(0x1C)=[mrd -value 0xE000B01C]  RXQBASE(0x18)=[mrd -value 0xE000B018]"
con
after 100
exit
