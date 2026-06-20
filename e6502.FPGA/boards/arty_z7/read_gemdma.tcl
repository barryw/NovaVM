connect
after 300
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
stop
puts "PSFIO RXQBASE(0x18)=[mrd -value 0xE000B018]  TXQBASE(0x1C)=[mrd -value 0xE000B01C]"
puts "PSFIO DMACFG(0x10)=[mrd -value 0xE000B010]  NETCTRL(0x04)=[mrd -value 0xE000B004]"
puts "PSFIO ISR(0x24)=[mrd -value 0xE000B024]  RXSTAT(0x20)=[mrd -value 0xE000B020]  TXSTAT(0x14)=[mrd -value 0xE000B014]"
con
after 100
exit
