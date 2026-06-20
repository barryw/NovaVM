connect
after 300
targets -set -nocase -filter {name =~ "*Cortex-A9*#0"}
stop
puts "PSFIO GEM0_CLK_CTRL(0xF8000140)=[mrd -value 0xF8000140]"
con
after 100
exit
