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
#   ESP_UPLOAD=ota|ota-cli|serial|skip
#                                  NovaHost upload method (default: direct OTA).
#   NOVAHOST=192.168.1.65         OTA/HTTP/debug target.
#   OTA_PORT=3232                  ArduinoOTA port for direct OTA.
#   ESPOTA=/path/to/espota.py      Override espota.py path if auto-detect fails.
#   SERIAL=/dev/cu.usbserial-...   Serial target when ESP_UPLOAD=serial.
#   SERIAL_VIA_PASSTHRU=1          Load FPGA passthru SRAM image for serial ESP upload.
#   SERIAL_REBOOT_AFTER_FPGA=1     Reboot ESP after FPGA exits passthru.
#   SD_SYNC=1|0                    Upload staged SD assets after ESP upload.
#   SD_SYNC_HOST=192.168.1.65      HTTP host for SD asset sync.
#   SD_SYNC_TIMEOUT=45             Seconds to wait for SD HTTP server.
#   SD_SYNC_RETRIES=8              Per-file HTTP PUT retry count.
#   ROM_RELOAD=1|0                 Reload ROMs from SD after SD sync.
#   ALLOW_TIMING_FAIL=1            Flash even if nextpnr timing report fails.
#   TIMING_MARGIN_MHZ=0.0          Extra fmax headroom required before flashing.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FPGA_DIR="$REPO_ROOT/e6502.FPGA"
ESP_DIR="$REPO_ROOT/e6502.ESP32/novahost"
BIT_BACKUPS="$FPGA_DIR/fpga/bit_backups"

BITSTREAM="${BITSTREAM:-}"
SYNTH="${SYNTH:-1}"
LABEL="${LABEL:-stack_boot_hardened}"
EXTRA_DEFINES="${EXTRA_DEFINES:--DVIDEO_720X480 -DGPDI_P_ONLY}"
export EXTRA_DEFINES
FQBN="${FQBN:-esp32:esp32:lolin32:PartitionScheme=min_spiffs}"
ESP_UPLOAD="${ESP_UPLOAD:-ota}"
NOVAHOST="${NOVAHOST:-192.168.1.65}"
OTA_PORT="${OTA_PORT:-3232}"
ESPOTA="${ESPOTA:-}"
ESP_BUILD_PATH="${ESP_BUILD_PATH:-$ESP_DIR/build/ota-direct}"
SERIAL="${SERIAL:-/dev/cu.usbserial-D01457}"
SERIAL_VIA_PASSTHRU="${SERIAL_VIA_PASSTHRU:-1}"
SERIAL_REBOOT_AFTER_FPGA="${SERIAL_REBOOT_AFTER_FPGA:-1}"
SD_SYNC="${SD_SYNC:-1}"
SD_SYNC_HOST="${SD_SYNC_HOST:-$NOVAHOST}"
SD_SYNC_TIMEOUT="${SD_SYNC_TIMEOUT:-45}"
SD_SYNC_RETRIES="${SD_SYNC_RETRIES:-8}"
ROM_RELOAD="${ROM_RELOAD:-1}"
ALLOW_TIMING_FAIL="${ALLOW_TIMING_FAIL:-0}"
TIMING_MARGIN_MHZ="${TIMING_MARGIN_MHZ:-0.0}"

latest_bitstream() {
    python3 - "$BIT_BACKUPS" <<'PY'
import sys
from pathlib import Path

root = Path(sys.argv[1])
metadata_files = sorted(
    root.glob("e6502.*.metadata.json"),
    key=lambda p: p.stat().st_mtime,
    reverse=True,
)
for metadata in metadata_files:
    bitstream = metadata.with_suffix("").with_suffix(".bit")
    if bitstream.is_file():
        print(bitstream)
        raise SystemExit(0)

bitstreams = sorted(
    root.glob("e6502.*.bit"),
    key=lambda p: p.stat().st_mtime,
    reverse=True,
)
if bitstreams:
    print(bitstreams[0])
PY
}

wait_for_sd_http() {
    local host="$1"
    local deadline=$((SECONDS + SD_SYNC_TIMEOUT))
    local status=""

    echo "Waiting for NovaHost SD HTTP at http://$host/sd-status"
    until status="$(curl -fsS --max-time 2 "http://$host/sd-status" 2>/dev/null)" &&
          printf '%s' "$status" | grep -q 'SD.begin()=true'; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "error: NovaHost SD HTTP did not report a mounted SD card within ${SD_SYNC_TIMEOUT}s" >&2
            if [ -n "$status" ]; then
                echo "last /sd-status: $status" >&2
            fi
            return 1
        fi
        sleep 1
    done
}

wait_for_http_health() {
    local host="$1"
    local deadline=$((SECONDS + SD_SYNC_TIMEOUT))

    echo "Waiting for NovaHost HTTP at http://$host/health"
    until curl -fsS --max-time 2 "http://$host/health" >/dev/null; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "error: NovaHost HTTP did not become ready within ${SD_SYNC_TIMEOUT}s" >&2
            return 1
        fi
        sleep 1
    done
}

wait_for_fpga_bridge_available() {
    local host="$1"
    local deadline=$((SECONDS + SD_SYNC_TIMEOUT))
    local status=""

    echo "Waiting for NovaHost FPGA bridge availability at http://$host/health"
    until status="$(curl -fsS --max-time 2 "http://$host/health" 2>/dev/null)" &&
          { ! printf '%s' "$status" | grep -q 'fpgaBridgeAvailable' ||
            printf '%s' "$status" | grep -q '"fpgaBridgeAvailable":true'; }; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "error: NovaHost FPGA bridge did not become available within ${SD_SYNC_TIMEOUT}s" >&2
            if [ -n "$status" ]; then
                echo "last /health: $status" >&2
            fi
            return 1
        fi
        sleep 1
    done
}

http_put_with_retry() {
    local src="$1"
    local url="$2"
    local attempt=1

    while [ "$attempt" -le "$SD_SYNC_RETRIES" ]; do
        if curl -fsS --max-time 30 -X PUT --data-binary "@$src" "$url" >/dev/null; then
            return 0
        fi
        echo "warning: PUT failed (attempt $attempt/$SD_SYNC_RETRIES): $url" >&2
        sleep 1
        attempt=$((attempt + 1))
    done

    echo "error: PUT failed after $SD_SYNC_RETRIES attempts: $url" >&2
    return 1
}

sync_sd_assets() {
    local host="$1"
    local sd_root="$ESP_DIR/build/sd"

    echo "=== [5/5] syncing SD assets to NovaHost"
    make --no-print-directory -C "$ESP_DIR" sd-assets
    wait_for_sd_http "$host"

    while IFS= read -r -d '' src; do
        local rel="${src#$sd_root/}"
        echo "PUT /sd/$rel"
        http_put_with_retry "$src" "http://$host/sd/$rel"
    done < <(find "$sd_root" -type f -print0)
}

reload_rom_from_sd() {
    local host="$1"

    if [ "$ROM_RELOAD" != "1" ]; then
        return 0
    fi

    echo "=== [5b/5] reloading ROMs from SD"
    wait_for_fpga_bridge_available "$host"
    python3 - "$host" <<'PY'
import json
import socket
import sys

host = sys.argv[1]
req = json.dumps({"command": "reload_rom"}).encode("utf-8") + b"\n"

with socket.create_connection((host, 6503), timeout=15) as sock:
    sock.settimeout(30)
    sock.sendall(req)
    data = b""
    while not data.endswith(b"\n"):
        chunk = sock.recv(4096)
        if not chunk:
            break
        data += chunk

line = data.decode("utf-8", errors="replace").strip()
print(line)
res = json.loads(line or "{}")
if not res.get("ok"):
    sys.exit(1)
PY
}

find_espota() {
    local candidate=""

    if [ -n "$ESPOTA" ]; then
        if [ -f "$ESPOTA" ]; then
            printf '%s\n' "$ESPOTA"
            return 0
        fi
        echo "error: ESPOTA is set but does not exist: $ESPOTA" >&2
        return 1
    fi

    for root in "$HOME/Library/Arduino15/packages/esp32/hardware/esp32" \
                "$HOME/.arduino15/packages/esp32/hardware/esp32"; do
        [ -d "$root" ] || continue
        candidate="$(find "$root" -path '*/tools/espota.py' -type f -print | sort | tail -n 1)"
        if [ -n "$candidate" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done

    echo "error: espota.py not found; set ESPOTA=/path/to/espota.py" >&2
    return 1
}

upload_esp_ota_direct() {
    local espota
    local bin

    espota="$(find_espota)"
    bin="$ESP_BUILD_PATH/novahost.ino.bin"

    echo "=== [4a/5] compiling NovaHost for direct OTA"
    (
        cd "$ESP_DIR"
        make --no-print-directory sd-assets
        arduino-cli compile --fqbn "$FQBN" --build-path "$ESP_BUILD_PATH" .
    )

    if [ ! -f "$bin" ]; then
        echo "error: compiled ESP binary not found: $bin" >&2
        exit 1
    fi

    echo "=== [4b/5] direct OTA to $NOVAHOST:$OTA_PORT"
    python3 "$espota" -i "$NOVAHOST" -p "$OTA_PORT" -f "$bin"
}

request_esp_reboot() {
    local host="$1"

    echo "=== [4c/5] rebooting NovaHost after FPGA reload"
    wait_for_http_health "$host"
    curl -fsS --max-time 5 -X POST "http://$host/reboot" >/dev/null
    sleep 2
    wait_for_http_health "$host"
}

check_timing_before_flash() {
    local bitstream="$1"
    local report="${2:-}"

    if [ -z "$report" ]; then
        report="${bitstream%.bit}.nextpnr-report.json"
    fi

    if [ ! -f "$report" ]; then
        echo "warning: no nextpnr timing report found for $bitstream" >&2
        echo "         expected: $report" >&2
        if [ "$ALLOW_TIMING_FAIL" != "1" ]; then
            echo "error: refusing to flash without a timing report; set ALLOW_TIMING_FAIL=1 to override" >&2
            exit 1
        fi
        return 0
    fi

    if ! python3 "$REPO_ROOT/tools/check-fpga-timing.py" "$report" --margin-mhz "$TIMING_MARGIN_MHZ"; then
        if [ "$ALLOW_TIMING_FAIL" = "1" ]; then
            echo "warning: timing failed, continuing because ALLOW_TIMING_FAIL=1" >&2
        else
            echo "error: refusing to flash timing-failed bitstream; set ALLOW_TIMING_FAIL=1 to override" >&2
            exit 1
        fi
    fi
}

echo "=== [1/5] refreshing ROM hex"
make --no-print-directory -C "$FPGA_DIR" hex

if [ -z "$BITSTREAM" ]; then
    if [ "$SYNTH" = "0" ]; then
        echo "=== [2/5] selecting latest existing bitstream"
        BITSTREAM="$(latest_bitstream)"
        TIMING_REPORT="${BITSTREAM%.bit}.nextpnr-report.json"
    else
        echo "=== [2/5] synthesizing FPGA on beast"
        synth_log="$(mktemp "${TMPDIR:-/tmp}/e6502-beast-synth.XXXXXX")"
        "$REPO_ROOT/tools/beast-synth.sh" bitstream "$LABEL" | tee "$synth_log"
        BITSTREAM="$(awk '/^bitstream:/ {print $2}' "$synth_log" | tail -n 1)"
        TIMING_REPORT="$(awk '/^report:/ {print $2}' "$synth_log" | tail -n 1)"
    fi
else
    echo "=== [2/5] using provided bitstream"
    TIMING_REPORT="${TIMING_REPORT:-${BITSTREAM%.bit}.nextpnr-report.json}"
fi

if [ -z "$BITSTREAM" ] || [ ! -f "$BITSTREAM" ]; then
    echo "error: bitstream not found: ${BITSTREAM:-<empty>}" >&2
    exit 1
fi

echo "bitstream: $BITSTREAM"
check_timing_before_flash "$BITSTREAM" "$TIMING_REPORT"

echo "=== [3/5] flashing FPGA config flash"
openFPGALoader --board ulx3s -f "$BITSTREAM"

echo "=== [4/5] uploading NovaHost ($ESP_UPLOAD)"
case "$ESP_UPLOAD" in
    ota)
        upload_esp_ota_direct
        ;;
    ota-cli)
        make --no-print-directory -C "$ESP_DIR" NOVAHOST="$NOVAHOST" upload-cli
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
            if [ "$SERIAL_REBOOT_AFTER_FPGA" = "1" ]; then
                request_esp_reboot "$SD_SYNC_HOST"
            fi
        fi
        ;;
    skip)
        echo "ESP upload skipped"
        ;;
    *)
        echo "error: ESP_UPLOAD must be ota, ota-cli, serial, or skip (got '$ESP_UPLOAD')" >&2
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
