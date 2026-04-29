#!/usr/bin/env bash
# Build/deploy the ULX3S stack in the order that keeps FPGA + NovaHost in sync.
#
# Default flow:
#   1. Rebuild ROM hex files used by FPGA bitstream init.
#   2. Synthesize on beast and pull back a timestamped bitstream.
#   3. Flash FPGA config flash.
#   4. Upload/reboot NovaHost so it streams matching ROMs into the new FPGA.
#   5. Sync SD-resident runtime assets and reload ROMs from SD.
#
# Common overrides:
#   BITSTREAM=/path/to/e6502.bit   Use an existing bitstream instead of synth.
#   SYNTH=0                        Use latest bit_backups/*.bit if BITSTREAM unset.
#   LABEL=boot_hardened            Label for beast-synth output.
#   EXTRA_DEFINES='-D...'          Verilog defines passed to beast-synth.
#   ESP_UPLOAD=ota|serial|skip     NovaHost upload method (default: ota).
#   NOVAHOST=novahost.local        OTA target.
#   SERIAL=/dev/cu.usbserial-...   Serial target when ESP_UPLOAD=serial.
#   SERIAL_VIA_PASSTHRU=1          Load FPGA passthru SRAM image for serial ESP upload.
#   SD_SYNC=1|0                    Upload staged SD assets after ESP upload.
#   SD_SYNC_HOST=novahost.local     HTTP host for SD asset sync.
#   SD_SYNC_TIMEOUT=45             Seconds to wait for SD HTTP server.
#   ROM_RELOAD=1|0                 Reload ROMs from SD after SD sync.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FPGA_DIR="$REPO_ROOT/e6502.FPGA"
BIT_BACKUPS="$FPGA_DIR/fpga/bit_backups"

BITSTREAM="${BITSTREAM:-}"
SYNTH="${SYNTH:-1}"
LABEL="${LABEL:-stack_boot_hardened}"
EXTRA_DEFINES="${EXTRA_DEFINES:--DVIDEO_720X480 -DGPDI_P_ONLY}"
export EXTRA_DEFINES
ESP_UPLOAD="${ESP_UPLOAD:-ota}"
NOVAHOST="${NOVAHOST:-novahost.local}"
SERIAL="${SERIAL:-/dev/cu.usbserial-D01457}"
SERIAL_VIA_PASSTHRU="${SERIAL_VIA_PASSTHRU:-1}"
SD_SYNC="${SD_SYNC:-1}"
SD_SYNC_HOST="${SD_SYNC_HOST:-$NOVAHOST}"
SD_SYNC_TIMEOUT="${SD_SYNC_TIMEOUT:-45}"
ROM_RELOAD="${ROM_RELOAD:-1}"

latest_bitstream() {
    find "$BIT_BACKUPS" -type f -name 'e6502.*.bit' -print0 |
        xargs -0 ls -t |
        head -n 1
}

wait_for_sd_http() {
    local host="$1"
    local deadline=$((SECONDS + SD_SYNC_TIMEOUT))

    echo "Waiting for NovaHost SD HTTP at http://$host/sd-status"
    until curl -fsS --max-time 2 "http://$host/sd-status" >/dev/null; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "error: NovaHost SD HTTP did not become ready within ${SD_SYNC_TIMEOUT}s" >&2
            return 1
        fi
        sleep 1
    done
}

sync_sd_assets() {
    local host="$1"
    local sd_root="$REPO_ROOT/e6502.ESP32/novahost/build/sd"

    echo "=== [5/5] syncing SD assets to NovaHost"
    make --no-print-directory -C "$REPO_ROOT/e6502.ESP32/novahost" sd-assets
    wait_for_sd_http "$host"

    while IFS= read -r -d '' src; do
        local rel="${src#$sd_root/}"
        echo "PUT /sd/$rel"
        curl -fsS --max-time 30 -X PUT --data-binary "@$src" "http://$host/sd/$rel" >/dev/null
    done < <(find "$sd_root" -type f -print0)
}

reload_rom_from_sd() {
    local host="$1"

    if [ "$ROM_RELOAD" != "1" ]; then
        return 0
    fi

    echo "=== [5b/5] reloading ROMs from SD"
    printf '{"command":"reload_rom"}\n' | nc -w 15 "$host" 6503
}

echo "=== [1/5] refreshing ROM hex"
make --no-print-directory -C "$FPGA_DIR" hex

if [ -z "$BITSTREAM" ]; then
    if [ "$SYNTH" = "0" ]; then
        echo "=== [2/5] selecting latest existing bitstream"
        BITSTREAM="$(latest_bitstream)"
    else
        echo "=== [2/5] synthesizing FPGA on beast"
        synth_log="$(mktemp "${TMPDIR:-/tmp}/e6502-beast-synth.XXXXXX")"
        "$REPO_ROOT/tools/beast-synth.sh" bitstream "$LABEL" | tee "$synth_log"
        BITSTREAM="$(awk '/^bitstream:/ {print $2}' "$synth_log" | tail -n 1)"
    fi
else
    echo "=== [2/5] using provided bitstream"
fi

if [ -z "$BITSTREAM" ] || [ ! -f "$BITSTREAM" ]; then
    echo "error: bitstream not found: ${BITSTREAM:-<empty>}" >&2
    exit 1
fi

echo "bitstream: $BITSTREAM"

echo "=== [3/5] flashing FPGA config flash"
openFPGALoader --board ulx3s -f "$BITSTREAM"

echo "=== [4/5] uploading NovaHost ($ESP_UPLOAD)"
case "$ESP_UPLOAD" in
    ota)
        make --no-print-directory -C "$REPO_ROOT/e6502.ESP32/novahost" NOVAHOST="$NOVAHOST" upload
        ;;
    serial)
        if [ "$SERIAL_VIA_PASSTHRU" = "1" ]; then
            echo "=== [4a/5] loading ESP serial passthru into FPGA SRAM"
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
            echo "=== [4b/5] reloading FPGA from config flash"
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

if [ "$SD_SYNC" = "1" ]; then
    sync_sd_assets "$SD_SYNC_HOST"
    reload_rom_from_sd "$SD_SYNC_HOST"
else
    echo "SD asset sync skipped"
fi

echo "ULX3S stack deploy complete"
