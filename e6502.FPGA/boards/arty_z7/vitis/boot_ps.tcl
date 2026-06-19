connect
targets -set -nocase -filter {name =~ "*Cortex-A9*#0" || name =~ "*ARM*#0"}
rst -processor
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*Cortex-A9*#0" || name =~ "*ARM*#0"}
dow /tmp/nova_ws2/hello/build/hello.elf
con
after 3000
puts "BOOT_DONE"
exit
