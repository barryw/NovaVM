# Arty Z7-20 constraints for arty_z7_vgc (VGC -> HDMI OUT / J11).
# Pin data from Digilent Arty-Z7-20-Master.xdc (Rev.B).

## 125 MHz system clock (H16)
set_property -dict { PACKAGE_PIN H16  IOSTANDARD LVCMOS33 } [get_ports { clk }]
create_clock -add -name sys_clk -period 8.00 -waveform {0 4} [get_ports { clk }]

## TODO(audit finding #5): clk_audio is a ~48 kHz fabric-divided clock (from
## clk_pixel) that clocks the HDMI audio-regen sampling of a clk_pixel-domain word.
## It's currently unconstrained. Adding create_clock + set_clock_groups -asynchronous
## needs the post-synth net/clock names verified against the timing report (the
## get_nets/get_pins filters are netlist-name sensitive). Low severity (audio only,
## works in practice — the edges are a phase-aligned subset of clk_pixel). Deferred.

## Push buttons (btn[0] = reset)
set_property -dict { PACKAGE_PIN D19  IOSTANDARD LVCMOS33 } [get_ports { btn[0] }]
set_property -dict { PACKAGE_PIN D20  IOSTANDARD LVCMOS33 } [get_ports { btn[1] }]
set_property -dict { PACKAGE_PIN L20  IOSTANDARD LVCMOS33 } [get_ports { btn[2] }]
set_property -dict { PACKAGE_PIN L19  IOSTANDARD LVCMOS33 } [get_ports { btn[3] }]

## User LEDs LD0-LD3
set_property -dict { PACKAGE_PIN R14  IOSTANDARD LVCMOS33 } [get_ports { led[0] }]
set_property -dict { PACKAGE_PIN P14  IOSTANDARD LVCMOS33 } [get_ports { led[1] }]
set_property -dict { PACKAGE_PIN N16  IOSTANDARD LVCMOS33 } [get_ports { led[2] }]
set_property -dict { PACKAGE_PIN M14  IOSTANDARD LVCMOS33 } [get_ports { led[3] }]

## RGB LED LD4: led4_b heartbeat, led4_g = MMCM locked
set_property -dict { PACKAGE_PIN L15  IOSTANDARD LVCMOS33 } [get_ports { led4_b }]
set_property -dict { PACKAGE_PIN G17  IOSTANDARD LVCMOS33 } [get_ports { led4_g }]

## HDMI TX / source (J11) — TMDS_33 differential pairs
set_property -dict { PACKAGE_PIN L16  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_clk_p }]
set_property -dict { PACKAGE_PIN L17  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_clk_n }]
set_property -dict { PACKAGE_PIN K17  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_p[0] }]
set_property -dict { PACKAGE_PIN K18  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_n[0] }]
set_property -dict { PACKAGE_PIN K19  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_p[1] }]
set_property -dict { PACKAGE_PIN J19  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_n[1] }]
set_property -dict { PACKAGE_PIN J18  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_p[2] }]
set_property -dict { PACKAGE_PIN H18  IOSTANDARD TMDS_33 } [get_ports { hdmi_tx_d_n[2] }]

## HDMI TX hot-plug-detect / output enable (LVCMOS33 single-ended)
set_property -dict { PACKAGE_PIN R19  IOSTANDARD LVCMOS33 } [get_ports { hdmi_tx_hpdn }]

## Bitstream housekeeping
set_property CFGBVS VCCO        [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
