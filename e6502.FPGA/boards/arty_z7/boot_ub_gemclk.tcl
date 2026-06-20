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
after 10000
stop
puts "UBOOT GEM0_CLK_CTRL(0xF8000140)=[mrd -value 0xF8000140]"
con
after 100
exit
