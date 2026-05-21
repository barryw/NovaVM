# Arty Z7 Blinky

Minimal Vivado hardware sanity test for an Arty Z7-20.

The design uses the 125 MHz `sys_clk` oscillator and drives:

- `leds_4bits_tri_o[3:0]`: four user LEDs
- `rgb_led_tri_o[5:0]`: two RGB LEDs

Build and program from a machine with Vivado:

```sh
vivado -mode batch -source build_and_program.tcl
```

