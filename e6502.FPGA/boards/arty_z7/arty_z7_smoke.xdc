# Arty Z7-20 (XC7Z020-1CLG400C) constraints for arty_z7_smoke.
# Pin data from Digilent Arty-Z7-20-Master.xdc (Rev.B). Only the ports used
# by the smoke test are uncommented.

## 125 MHz system clock (H16)
set_property -dict { PACKAGE_PIN H16  IOSTANDARD LVCMOS33 } [get_ports { clk }]
create_clock -add -name sys_clk -period 8.00 -waveform {0 4} [get_ports { clk }]

## User LEDs LD0-LD3
set_property -dict { PACKAGE_PIN R14  IOSTANDARD LVCMOS33 } [get_ports { led[0] }]
set_property -dict { PACKAGE_PIN P14  IOSTANDARD LVCMOS33 } [get_ports { led[1] }]
set_property -dict { PACKAGE_PIN N16  IOSTANDARD LVCMOS33 } [get_ports { led[2] }]
set_property -dict { PACKAGE_PIN M14  IOSTANDARD LVCMOS33 } [get_ports { led[3] }]

## RGB LED LD4
set_property -dict { PACKAGE_PIN N15  IOSTANDARD LVCMOS33 } [get_ports { led4_r }]
set_property -dict { PACKAGE_PIN G17  IOSTANDARD LVCMOS33 } [get_ports { led4_g }]
set_property -dict { PACKAGE_PIN L15  IOSTANDARD LVCMOS33 } [get_ports { led4_b }]

## RGB LED LD5
set_property -dict { PACKAGE_PIN M15  IOSTANDARD LVCMOS33 } [get_ports { led5_r }]
set_property -dict { PACKAGE_PIN L14  IOSTANDARD LVCMOS33 } [get_ports { led5_g }]
set_property -dict { PACKAGE_PIN G14  IOSTANDARD LVCMOS33 } [get_ports { led5_b }]

## Push buttons BTN0-BTN3 (active-high)
set_property -dict { PACKAGE_PIN D19  IOSTANDARD LVCMOS33 } [get_ports { btn[0] }]
set_property -dict { PACKAGE_PIN D20  IOSTANDARD LVCMOS33 } [get_ports { btn[1] }]
set_property -dict { PACKAGE_PIN L20  IOSTANDARD LVCMOS33 } [get_ports { btn[2] }]
set_property -dict { PACKAGE_PIN L19  IOSTANDARD LVCMOS33 } [get_ports { btn[3] }]

## Slide switches SW0-SW1
set_property -dict { PACKAGE_PIN M20  IOSTANDARD LVCMOS33 } [get_ports { sw[0] }]
set_property -dict { PACKAGE_PIN M19  IOSTANDARD LVCMOS33 } [get_ports { sw[1] }]

## UART on Pmod JA (PL-reachable; onboard USB-UART is PS-only).
##   uart_rx_in  = JA1 (Y18)  <- USB-UART adapter TX
##   uart_tx_out = JA2 (Y19)  -> USB-UART adapter RX
##   also wire adapter GND -> JA5, and (if needed) adapter 3V3 -> JA6
set_property -dict { PACKAGE_PIN Y18  IOSTANDARD LVCMOS33 } [get_ports { uart_rx_in }]
set_property -dict { PACKAGE_PIN Y19  IOSTANDARD LVCMOS33 } [get_ports { uart_tx_out }]

## The async UART RX pin is not timed against sys_clk (double-registered in RTL).
set_false_path -from [get_ports uart_rx_in]

## Bitstream housekeeping
set_property CFGBVS VCCO        [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
