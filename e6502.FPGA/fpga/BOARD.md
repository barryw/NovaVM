# ULX3S board notes

## Version detection caveat — READ FIRST

**The silkscreen and FTDI product-string version are NOT reliable for pin layout.**

This board enumerates as `ULX3S FPGA 85K v3.0.8`, but ESP32 reset probing
confirms the v3.1.x EN/GPIO0 routing:

- `wifi_en`:    **J5**
- `wifi_gpio0`: **F1**
- `wifi_rxd`:   K3
- `wifi_txd`:   K4

Do **not** use the older v2.x/v3.0.x ESP reset mapping:

- `wifi_en`:    F1
- `wifi_gpio0`: L2

Using the wrong mapping silently — the FPGA drives F1 low thinking it's EN,
but that pin is actually GPIO0 on this hardware, so the ESP32 never resets
and esptool times out with "No serial data received".

If `arduino-cli upload` fails with "Failed to connect to ESP32: No serial data
received", verify EN reset actually reaches the ESP32:

```bash
# Load a bitstream that permanently holds wifi_en low, then check:
ping novahost.local
# If ESP32 keeps responding, the FPGA is driving the wrong pin.
# Swap the LPF to the v3.1.x mapping above and rebuild.
```

## Working passthru

`passthru.sv` + `passthru.lpf` in this directory build a minimal FTDI↔ESP32
serial passthrough with direct-mapping reset:

- `wifi_en = ftdi_nrts`   (RTS asserted → EN low → reset)
- `wifi_gpio0 = ftdi_ndtr` (DTR asserted → GPIO0 low → download mode)

Direct mapping is deliberate — emard's lookup-table passthru has transient
states that cause the ESP32 to boot normally before reaching download mode
when pyserial toggles DTR/RTS non-atomically.

Build + load + flash:

```bash
cd e6502.FPGA/fpga
yosys -q -p "read_verilog -sv passthru.sv; synth_ecp5 -top passthru -json build/passthru.json"
PATH="/opt/homebrew/oss-cad-suite/bin:$PATH" nextpnr-ecp5 --85k --package CABGA381 \
    --textcfg build/passthru.config --json build/passthru.json --lpf passthru.lpf -q
PATH="/opt/homebrew/oss-cad-suite/bin:$PATH" ecppack build/passthru.config build/passthru.bit
openFPGALoader --board ulx3s build/passthru.bit
arduino-cli upload --fqbn esp32:esp32:lolin32 --port /dev/cu.usbserial-D01457 \
    ../../e6502.ESP32/novahost
```

After one serial upload, use OTA for subsequent updates:

```bash
arduino-cli upload --fqbn esp32:esp32:lolin32 --port novahost.local \
    ../../e6502.ESP32/novahost
```
