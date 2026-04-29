#!/usr/bin/env bash
# Build/deploy the ULX3S stack in the order that keeps FPGA + NovaHost in sync.
#
# Default flow:
#   1. Rebuild ROM hex files used by FPGA bitstream init.
#   2. Synthesize on beast and pull back a timestamped bitstream.
#   3. Flash FPGA config flash.
#   4. Upload/reboot NovaHost so it streams matching ROMs into the new FPGA.
#
# Common overrides:
#   BITSTREAM=/path/to/e6502.bit   Use an existing bitstream instead of synth.
#   SYNTH=0                        Use latest bit_backups/*.bit if BITSTREAM unset.
#   LABEL=boot_hardened            Label for beast-synth output.
#   ESP_UPLOAD=ota|serial|skip     NovaHost upload method (default: ota).
#   NOVAHOST=novahost.local        OTA target.
#   SERIAL=/dev/cu.usbserial-...   Serial target when ESP_UPLOAD=serial.
#   SERIAL_VIA_PASSTHRU=1          Load FPGA passthru SRAM image for serial ESP upload.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FPGA_DIR="$REPO_ROOT/e6502.FPGA"
BIT_BACKUPS="$FPGA_DIR/fpga/bit_backups"

BITSTREAM="${BITSTREAM:-}"
SYNTH="${SYNTH:-1}"
LABEL="${LABEL:-stack_boot_hardened}"
ESP_UPLOAD="${ESP_UPLOAD:-ota}"
NOVAHOST="${NOVAHOST:-novahost.local}"
SERIAL="${SERIAL:-/dev/cu.usbserial-D01457}"
SERIAL_VIA_PASSTHRU="${SERIAL_VIA_PASSTHRU:-1}"

latest_bitstream() {
    find "$BIT_BACKUPS" -type f -name 'e6502.*.bit' -print0 |
        xargs -0 ls -t |
        head -n 1
}

echo "=== [1/4] refreshing ROM hex"
make --no-print-directory -C "$FPGA_DIR" hex

if [ -z "$BITSTREAM" ]; then
    if [ "$SYNTH" = "0" ]; then
        echo "=== [2/4] selecting latest existing bitstream"
        BITSTREAM="$(latest_bitstream)"
    else
        echo "=== [2/4] synthesizing FPGA on beast"
        synth_log="$(mktemp "${TMPDIR:-/tmp}/e6502-beast-synth.XXXXXX")"
        "$REPO_ROOT/tools/beast-synth.sh" bitstream "$LABEL" | tee "$synth_log"
        BITSTREAM="$(awk '/^bitstream:/ {print $2}' "$synth_log" | tail -n 1)"
    fi
else
    echo "=== [2/4] using provided bitstream"
fi

if [ -z "$BITSTREAM" ] || [ ! -f "$BITSTREAM" ]; then
    echo "error: bitstream not found: ${BITSTREAM:-<empty>}" >&2
    exit 1
fi

echo "bitstream: $BITSTREAM"

echo "=== [3/4] flashing FPGA config flash"
openFPGALoader --board ulx3s -f "$BITSTREAM"

echo "=== [4/4] uploading NovaHost ($ESP_UPLOAD)"
case "$ESP_UPLOAD" in
    ota)
        make --no-print-directory -C "$REPO_ROOT/e6502.ESP32/novahost" NOVAHOST="$NOVAHOST" upload
        ;;
    serial)
        if [ "$SERIAL_VIA_PASSTHRU" = "1" ]; then
            echo "=== [4a/4] loading ESP serial passthru into FPGA SRAM"
            (
                cd "$FPGA_DIR/fpga"
                mkdir -p build
                /opt/homebrew/oss-cad-suite/bin/yosys -q -p \
                    "read_verilog -sv passthru.sv; synth_ecp5 -top passthru -json build/passthru.json"
                /opt/homebrew/oss-cad-suite/bin/nextpnr-ecp5 --85k --package CABGA381 \
                    --textcfg build/passthru.config --json build/passthru.json --lpf passthru.lpf -q
                /opt/homebrew/oss-cad-suite/bin/ecppack build/passthru.config build/passthru.bit
                openFPGALoader --board ulx3s build/passthru.bit
            )
        fi
        make --no-print-directory -C "$REPO_ROOT/e6502.ESP32/novahost" SERIAL="$SERIAL" serial
        if [ "$SERIAL_VIA_PASSTHRU" = "1" ]; then
            echo "=== [4b/4] reloading FPGA from config flash"
            openFPGALoader --board ulx3s -r
        fi
        ;;
    skip)
        echo "ESP upload skipped"
        ;;
    *)
        echo "error: ESP_UPLOAD must be ota, serial, or skip (got '$ESP_UPLOAD')" >&2
        exit 1
        ;;
esac

echo "ULX3S stack deploy complete"
