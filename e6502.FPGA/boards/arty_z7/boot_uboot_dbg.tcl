connect
after 500
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
rst -processor
after 500
source /tmp/nova_ws2/hello/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
after 500
dow /tmp/u-boot-xlnx/u-boot.elf
con
after 4000
stop
puts "PC=[lindex [rrd pc] end]"
puts "regs:"
catch { puts [rrd] }
exit
