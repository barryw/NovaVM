usb_hid_host_fs is vendored from:

https://github.com/m1nl/usb_hid_host

Vendored commit:

8b72f038c6fb9cbca5f6cfe86d679d1fb5242c22

License:

Apache-2.0, preserved in LICENSE.

Nova integration notes:

- This fork supports low-speed and full-speed USB HID devices without an
  external PHY.
- Nova drives FULL_SPEED=1 at 48 MHz, giving the USB engine 4 clock ticks per
  full-speed bit. Low-speed devices are handled through the core's internal
  prescaler.
- usb_hid_host_rom.v was modified locally to make the ROM file path work from
  Nova's FPGA build and test working directories.
- usb_hid_host.v was modified locally to parameterize the full-speed clock
  rate. The vendored default is now 48 MHz for Nova timing closure; designs
  that want the upstream-style 60 MHz behavior can set FULL_SPEED_CLK_HZ to
  60000000.
