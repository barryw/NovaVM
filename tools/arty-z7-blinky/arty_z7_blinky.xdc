set_property PACKAGE_PIN H16 [get_ports sys_clk]
set_property IOSTANDARD LVCMOS33 [get_ports sys_clk]
create_clock -period 8.000 -name sys_clk [get_ports sys_clk]

set_property PACKAGE_PIN R14 [get_ports {leds_4bits_tri_o[0]}]
set_property PACKAGE_PIN P14 [get_ports {leds_4bits_tri_o[1]}]
set_property PACKAGE_PIN N16 [get_ports {leds_4bits_tri_o[2]}]
set_property PACKAGE_PIN M14 [get_ports {leds_4bits_tri_o[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {leds_4bits_tri_o[*]}]

set_property PACKAGE_PIN L15 [get_ports {rgb_led_tri_o[0]}]
set_property PACKAGE_PIN G17 [get_ports {rgb_led_tri_o[1]}]
set_property PACKAGE_PIN N15 [get_ports {rgb_led_tri_o[2]}]
set_property PACKAGE_PIN G14 [get_ports {rgb_led_tri_o[3]}]
set_property PACKAGE_PIN L14 [get_ports {rgb_led_tri_o[4]}]
set_property PACKAGE_PIN M15 [get_ports {rgb_led_tri_o[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {rgb_led_tri_o[*]}]

