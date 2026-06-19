usb_hid_host is vendored from:

https://github.com/nand2mario/usb_hid_host

Imported files:

- usb_hid_host.v
- usb_hid_host_rom.v
- usb_hid_host_rom.hex
- LICENSE

Nova change:

- usb_hid_host_rom.v reads the ROM hex file through the path used by the
  e6502.FPGA/boards/ulx3s Makefile working directory.
