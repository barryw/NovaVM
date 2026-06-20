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
after 300
mwr 0xF8000008 0xDF0D
dow /tmp/u-boot-xlnx/u-boot.elf
con
after 3000
puts "UBOOT_BOOTED"
exit
