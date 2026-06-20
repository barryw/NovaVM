connect
after 300
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
stop
puts "FRAMES_TX(0x108)=[mrd -value 0xE000B108]"
puts "FRAMES_RX(0x158)=[mrd -value 0xE000B158]"
puts "CARRIER_SENSE_ERR(0x148)=[mrd -value 0xE000B148]"
puts "TX_SINGLE_COLL(0x134)=[mrd -value 0xE000B134]"
puts "TX_LATE_COLL(0x140)=[mrd -value 0xE000B140]"
puts "TX_UNDERRUN(0x134)=[mrd -value 0xE000B114]"
puts "RX_FCS_ERR(0x190)=[mrd -value 0xE000B190]"
puts "RX_SYMBOL_ERR(0x1A0)=[mrd -value 0xE000B1A0]"
puts "RX_ALIGN_ERR(0x194)=[mrd -value 0xE000B194]"
puts "RX_RESOURCE_ERR(0x1A4)=[mrd -value 0xE000B1A4]"
puts "RX_OVERRUN(0x1A8)=[mrd -value 0xE000B1A8]"
con
after 100
exit
