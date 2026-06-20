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
dow /tmp/u-boot-xlnx/u-boot
bpadd -addr 0x0402bba4
con
after 700
catch {stop}
puts "AT board_init_f PC=[lindex [rrd pc] end]"
for {set i 0} {$i < 30} {incr i} {
  catch {nxt}
  after 150
  puts "N$i PC=[lindex [rrd pc] end]"
}
exit
