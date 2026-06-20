connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 500
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
puts "PS7 BAUDGEN(0x18)=[mrd -value 0xE0000018]  BAUDDIV(0x34)=[mrd -value 0xE0000034]"
after 300
mwr 0xF8000008 0xDF0D
dow /tmp/u-boot-xlnx/u-boot.elf
con
after 4000
puts "UBOOT BAUDGEN(0x18)=[mrd -value 0xE0000018]  BAUDDIV(0x34)=[mrd -value 0xE0000034]"
puts "UBOOT MR(0x04)=[mrd -value 0xE0000004]  CR(0x00)=[mrd -value 0xE0000000]"
exit
