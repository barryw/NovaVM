# ULX3S board notes

## Version detection caveat

**The silkscreen and FTDI product-string version are NOT reliable for pin layout.**

Boards sold as "v3.0.8" (silkscreen and `lsusb` both report this) have been
observed with the **v3.1.x physical pinout**. The ESP32 reset/boot control
lines specifically were remapped between early and late v3.x board revisions:

| Signal        | v2.x / early v3.0.x   | v3.1.x+ (this board)    |
| ------------- | --------------------- | ----------------------- |
| `wifi_en`     | F1                    | **J5**                  |
| `wifi_gpio0`  | L2                    | **F1**                  |
| `wifi_rxd`    | K3                    | K3                      |
| `wifi_txd`    | K4                    | K4                      |

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
