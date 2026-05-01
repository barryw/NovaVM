#!/usr/bin/env bash
# Run the NovaBASIC hardware integration suite through NovaHost.
#
# This is the supported entry point for live-board BASIC/VGC integration tests.
# It makes the implicit hardware assumptions explicit: NovaHost HTTP is up,
# the debug bridge is accepting TCP connections, the SD card is mounted, ROMs
# have been reloaded after flashing, and each suite has a bounded runtime.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEFAULT_SIM="$HOME/Git/sim6502/sim6502/bin/Release/net10.0/Sim6502TestRunner"

SIM6502="${SIM6502:-$DEFAULT_SIM}"
NOVAHOST="${NOVAHOST:-192.168.1.65}"
NOVAHOST_PORT="${NOVAHOST_PORT:-6503}"
HTTP_PORT="${HTTP_PORT:-80}"
CONNECT_TIMEOUT="${CONNECT_TIMEOUT:-2}"
PRECHECK_TIMEOUT="${PRECHECK_TIMEOUT:-20}"
DEBUG_RESPONSE_TIMEOUT="${DEBUG_RESPONSE_TIMEOUT:-15}"
SUITE_TIMEOUT="${SUITE_TIMEOUT:-180}"
RELOAD_ROM="${RELOAD_ROM:-1}"
COLD_START="${COLD_START:-1}"
REQUIRE_SD="${REQUIRE_SD:-1}"
RUN_LOG_DIR="${RUN_LOG_DIR:-/tmp}"
CAPTURE_HDMI_ON_FAIL="${CAPTURE_HDMI_ON_FAIL:-1}"
HDMI_CAPTURE_TOOL="${HDMI_CAPTURE_TOOL:-$REPO_ROOT/tools/capture-hdmi-frame.sh}"
SNAPSHOT_ON_FAIL="${SNAPSHOT_ON_FAIL:-1}"
SNAPSHOT_TOOL="${SNAPSHOT_TOOL:-$REPO_ROOT/tools/snapshot-novavm-state.py}"
RUN_STAMP="$(date +%Y%m%d_%H%M%S)"

DEFAULT_SUITES=(
    text
    vgc
    sprites
    blitter
    dma
    xram
    sid
    copper
    tiles
    math
    fileio
    sd
)

usage() {
    cat <<EOF
usage: tools/run-basic-integration.sh [suite ...]

Runs tests/integration/*.6502 against live Nova hardware through NovaHost.

Environment:
  NOVAHOST=$NOVAHOST
  NOVAHOST_PORT=$NOVAHOST_PORT
  SIM6502=$SIM6502
  PRECHECK_TIMEOUT=$PRECHECK_TIMEOUT
  SUITE_TIMEOUT=$SUITE_TIMEOUT
  RELOAD_ROM=$RELOAD_ROM
  COLD_START=$COLD_START
  REQUIRE_SD=$REQUIRE_SD
  CAPTURE_HDMI_ON_FAIL=$CAPTURE_HDMI_ON_FAIL
  SNAPSHOT_ON_FAIL=$SNAPSHOT_ON_FAIL

Examples:
  tools/run-basic-integration.sh
  tools/run-basic-integration.sh text sprites
  NOVAHOST=192.168.1.65 SUITE_TIMEOUT=240 tools/run-basic-integration.sh sd
EOF
}

die() {
    echo "error: $*" >&2
    exit 1
}

have_nc_connect_timeout() {
    nc -h 2>&1 | grep -q -- "-G "
}

tcp_probe() {
    local host="$1"
    local port="$2"

    if have_nc_connect_timeout; then
        nc -G "$CONNECT_TIMEOUT" -z "$host" "$port" >/dev/null 2>&1
    else
        nc -w "$CONNECT_TIMEOUT" -z "$host" "$port" >/dev/null 2>&1
    fi
}

wait_for_tcp() {
    local host="$1"
    local port="$2"
    local deadline=$((SECONDS + PRECHECK_TIMEOUT))

    until tcp_probe "$host" "$port"; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            return 1
        fi
        sleep 1
    done
}

http_get() {
    local path="$1"

    curl -fsS \
        --connect-timeout "$CONNECT_TIMEOUT" \
        --max-time "$DEBUG_RESPONSE_TIMEOUT" \
        "http://$NOVAHOST:$HTTP_PORT$path"
}

wait_for_http() {
    local deadline=$((SECONDS + PRECHECK_TIMEOUT))

    until http_get /health >/dev/null 2>&1; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            return 1
        fi
        sleep 1
    done
}

wait_for_fpga_bridge_available() {
    local deadline=$((SECONDS + PRECHECK_TIMEOUT))
    local health=""

    until health="$(http_get /health 2>/dev/null)" &&
          { ! printf '%s' "$health" | grep -q 'fpgaBridgeAvailable' ||
            printf '%s' "$health" | grep -q '"fpgaBridgeAvailable":true'; }; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "last /health: ${health:-<no response>}" >&2
            return 1
        fi
        sleep 1
    done
}

debug_cmd() {
    local payload="$1"

    python3 - "$NOVAHOST" "$NOVAHOST_PORT" "$DEBUG_RESPONSE_TIMEOUT" "$payload" <<'PY'
import socket
import sys

host = sys.argv[1]
port = int(sys.argv[2])
timeout = float(sys.argv[3])
payload = sys.argv[4].encode("utf-8") + b"\n"

with socket.create_connection((host, port), timeout=timeout) as sock:
    sock.settimeout(timeout)
    sock.sendall(payload)
    data = b""
    while not data.endswith(b"\n"):
        chunk = sock.recv(4096)
        if not chunk:
            break
        data += chunk

sys.stdout.write(data.decode("utf-8", errors="replace"))
PY
}

run_with_timeout() {
    local timeout="$1"
    shift

    "$@" &
    local pid=$!
    local deadline=$((SECONDS + timeout))

    while kill -0 "$pid" 2>/dev/null; do
        if [ "$SECONDS" -ge "$deadline" ]; then
            echo "error: command timed out after ${timeout}s: $*" >&2
            kill "$pid" 2>/dev/null || true
            sleep 2
            kill -9 "$pid" 2>/dev/null || true
            wait "$pid" 2>/dev/null || true
            return 124
        fi
        sleep 1
    done

    wait "$pid"
}

capture_hdmi_failure() {
    local suite="$1"
    local out="$RUN_LOG_DIR/nova_basic_integration_${RUN_STAMP}_${suite}_failure.png"

    if [ "$CAPTURE_HDMI_ON_FAIL" != "1" ]; then
        return 0
    fi
    if [ ! -x "$HDMI_CAPTURE_TOOL" ]; then
        echo "warning: HDMI capture tool not executable: $HDMI_CAPTURE_TOOL" >&2
        return 0
    fi

    echo "Capturing HDMI failure frame: $out"
    if ! "$HDMI_CAPTURE_TOOL" "$out"; then
        echo "warning: HDMI failure capture failed" >&2
    fi
}

capture_failure_snapshot() {
    local suite="$1"
    local out="$RUN_LOG_DIR/nova_basic_integration_${RUN_STAMP}_${suite}_snapshot"
    local args=(
        --host "$NOVAHOST"
        --port "$NOVAHOST_PORT"
        --http-port "$HTTP_PORT"
        --timeout "$DEBUG_RESPONSE_TIMEOUT"
        --out "$out"
        --label "$suite"
        --resume
    )

    if [ "$SNAPSHOT_ON_FAIL" != "1" ]; then
        return 1
    fi
    if [ ! -x "$SNAPSHOT_TOOL" ]; then
        echo "warning: snapshot tool not executable: $SNAPSHOT_TOOL" >&2
        return 1
    fi
    if [ "$CAPTURE_HDMI_ON_FAIL" != "1" ]; then
        args+=(--no-hdmi)
    fi

    echo "Capturing hardware failure snapshot: $out"
    "$SNAPSHOT_TOOL" "${args[@]}"
}

target_reachable() {
    http_get /health >/dev/null 2>&1 && tcp_probe "$NOVAHOST" "$NOVAHOST_PORT"
}

normalize_suite() {
    local suite="$1"

    suite="${suite#tests/integration/}"
    suite="${suite%.6502}"
    printf '%s\n' "$suite"
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    usage
    exit 0
fi

if [ ! -x "$SIM6502" ]; then
    if command -v Sim6502TestRunner >/dev/null 2>&1; then
        SIM6502="$(command -v Sim6502TestRunner)"
    elif command -v sim6502 >/dev/null 2>&1; then
        SIM6502="$(command -v sim6502)"
    else
        die "Sim6502TestRunner not found; set SIM6502=/path/to/Sim6502TestRunner"
    fi
fi

mkdir -p "$RUN_LOG_DIR"
LOG="$RUN_LOG_DIR/nova_basic_integration_$RUN_STAMP.log"
exec > >(tee "$LOG") 2>&1

if [ "$#" -gt 0 ]; then
    SUITES=()
    for suite in "$@"; do
        SUITES+=("$(normalize_suite "$suite")")
    done
else
    SUITES=("${DEFAULT_SUITES[@]}")
fi

echo "NovaBASIC hardware integration"
echo "  host:        $NOVAHOST:$NOVAHOST_PORT"
echo "  http:        http://$NOVAHOST:$HTTP_PORT"
echo "  runner:      $SIM6502"
echo "  repo:        $REPO_ROOT"
echo "  log:         $LOG"
echo "  suite limit: ${SUITE_TIMEOUT}s"
echo

echo "Preflight: NovaHost HTTP /health"
if ! wait_for_http; then
    die "NovaHost HTTP did not become reachable within ${PRECHECK_TIMEOUT}s"
fi

echo "Preflight: NovaHost SD /sd-status"
SD_STATUS="$(http_get /sd-status || true)"
if [ -z "$SD_STATUS" ]; then
    die "NovaHost /sd-status did not respond"
fi
echo "$SD_STATUS"
if [ "$REQUIRE_SD" = "1" ] && ! printf '%s' "$SD_STATUS" | grep -q 'SD.begin()=true'; then
    die "SD card is not mounted according to /sd-status"
fi
if printf '%s\n' "${SUITES[@]}" | grep -qx 'sd' &&
   ! printf '%s' "$SD_STATUS" | grep -q 'hd_mounts=1'; then
    die "sd.6502 requires hd0.ndi auto-mounted; /sd-status does not show hd_mounts=1"
fi

echo "Preflight: NovaHost debug bridge TCP"
if ! wait_for_tcp "$NOVAHOST" "$NOVAHOST_PORT"; then
    die "debug bridge $NOVAHOST:$NOVAHOST_PORT did not become reachable within ${PRECHECK_TIMEOUT}s"
fi

echo "Preflight: FPGA bridge availability"
if ! wait_for_fpga_bridge_available; then
    die "FPGA bridge did not become available within ${PRECHECK_TIMEOUT}s"
fi

if [ "$RELOAD_ROM" = "1" ]; then
    echo "Preflight: reload ROMs from SD"
    reload_resp="$(debug_cmd '{"command":"reload_rom"}' || true)"
    echo "$reload_resp"
    printf '%s' "$reload_resp" | grep -q '"ok":true' || die "reload_rom failed"
fi

if [ "$COLD_START" = "1" ]; then
    echo "Preflight: cold start"
    cold_resp="$(debug_cmd '{"command":"cold_start"}' || true)"
    echo "$cold_resp"
    printf '%s' "$cold_resp" | grep -q '"ok":true' || die "cold_start failed"
fi

echo
suite_failures=0

for suite in "${SUITES[@]}"; do
    test_file="$REPO_ROOT/tests/integration/$suite.6502"
    [ -f "$test_file" ] || die "suite file not found: $test_file"

    echo "===== $suite ====="
    if run_with_timeout "$SUITE_TIMEOUT" \
        "$SIM6502" \
        --backend novavm \
        --novavm-host "$NOVAHOST" \
        --novavm-port "$NOVAHOST_PORT" \
        -s "$test_file"; then
        echo "===== $suite PASS ====="
    else
        rc=$?
        echo "===== $suite FAIL rc=$rc ====="
        capture_failure_snapshot "$suite" || capture_hdmi_failure "$suite"
        suite_failures=$((suite_failures + 1))
        if ! target_reachable; then
            echo "error: target became unreachable after '$suite'; aborting remaining suites" >&2
            break
        fi
    fi
done

echo
if [ "$suite_failures" -eq 0 ]; then
    echo "All requested suites passed."
    echo "Log: $LOG"
    exit 0
fi

echo "$suite_failures suite(s) failed."
echo "Log: $LOG"
exit 1
