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
bpadd -addr 0x04001b9c
bpadd -addr 0x04002864
bpadd -addr 0x0402bba4
bpadd -addr 0x04003c34
bpadd -addr 0x04030918
bpadd -addr 0x040657f0
bpadd -addr 0x0402c0ac
for {set i 0} {$i < 8} {incr i} {
  con
  after 700
  catch {stop}
  puts "STOP$i PC=[lindex [rrd pc] end]"
}
exit
