#!/usr/bin/env bash
# Inspect script-managed FPGA bitstream metadata.
#
# Usage:
#   tools/bitstream-info.sh
#   tools/bitstream-info.sh list
#   tools/bitstream-info.sh show latest
#   tools/bitstream-info.sh show e6502.FPGA/fpga/bit_backups/e6502.20260430_010203.label.bit
#   tools/bitstream-info.sh show e6502.FPGA/fpga/bit_backups/e6502.20260430_010203.label.metadata.json

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIT_BACKUPS="${BIT_BACKUPS:-$REPO_ROOT/e6502.FPGA/fpga/bit_backups}"
COMMAND="${1:-list}"
ARG="${2:-latest}"

latest_metadata() {
    python3 - "$BIT_BACKUPS" <<'PY'
import sys
from pathlib import Path

root = Path(sys.argv[1])
metadata_files = sorted(
    root.glob("e6502.*.metadata.json"),
    key=lambda p: p.stat().st_mtime,
    reverse=True,
)
if metadata_files:
    print(metadata_files[0])
PY
}

case "$COMMAND" in
    list)
        python3 - "$BIT_BACKUPS" <<'PY'
import json
import sys
from pathlib import Path

root = Path(sys.argv[1])
metadata_files = sorted(
    root.glob("e6502.*.metadata.json"),
    key=lambda p: p.stat().st_mtime,
    reverse=True,
)

if not metadata_files:
    print(f"No script-managed bitstreams found under {root}")
    sys.exit(0)

print("built_at_utc          label                         commit     dirty  clk_pixel_mhz  DP16KD       bitstream")
for path in metadata_files:
    data = json.loads(path.read_text(encoding="utf-8"))
    build = data.get("build", {})
    source = data.get("source", {})
    artifact = data.get("artifact", {})
    timing = data.get("timing", {})
    fmax = timing.get("fmax", {})
    util = timing.get("utilization", {})
    clk = fmax.get("$glbnet$clk_pixel", {})
    dp16 = util.get("DP16KD", {})
    label = (build.get("label") or "-")[:28]
    commit = source.get("commit", "-")
    dirty = source.get("dirty_state", "-")
    mhz = clk.get("achieved", "-")
    if isinstance(mhz, (int, float)):
        mhz = f"{mhz:.2f}"
    used = dp16.get("used", "-")
    avail = dp16.get("available", "-")
    bit = Path(artifact.get("bitstream", "")).name
    print(f"{artifact.get('built_at_utc', '-'):<21} {label:<28} {commit:<10} {dirty:<6} {mhz:<14} {used}/{avail:<10} {bit}")
PY
        ;;
    show)
        if [ "$ARG" = "latest" ]; then
            metadata="$(latest_metadata)"
        else
            metadata="$ARG"
            case "$metadata" in
                *.bit)
                    metadata="${metadata%.bit}.metadata.json"
                    ;;
            esac
        fi

        if [ -z "${metadata:-}" ] || [ ! -f "$metadata" ]; then
            echo "error: metadata not found: ${metadata:-latest}" >&2
            exit 1
        fi

        python3 - "$metadata" <<'PY'
import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
data = json.loads(path.read_text(encoding="utf-8"))
artifact = data.get("artifact", {})
build = data.get("build", {})
source = data.get("source", {})
remote = data.get("remote", {})
timing = data.get("timing", {})
fmax = timing.get("fmax", {})
util = timing.get("utilization", {})

print(f"metadata: {path}")
print(f"bitstream: {artifact.get('bitstream', '-')}")
print(f"config: {artifact.get('config', '-')}")
print(f"log: {artifact.get('log', '-')}")
print(f"report: {artifact.get('nextpnr_report', '-')}")
print(f"built_at_utc: {artifact.get('built_at_utc', '-')}")
print(f"md5: {artifact.get('md5', '-')}")
print(f"size_bytes: {artifact.get('size_bytes', '-')}")
print()
print(f"label: {build.get('label', '-')}")
print(f"notes: {build.get('notes', '')}")
print(f"defines: {build.get('extra_defines', '-')}")
print(f"seed: {build.get('nextpnr_seed', '-')}")
print(f"threads: {build.get('nextpnr_threads', '-')}")
print()
print(f"branch: {source.get('branch', '-')}")
print(f"commit: {source.get('commit', '-')}")
print(f"dirty: {source.get('dirty_state', '-')} ({source.get('dirty_count', 0)} files)")
print(f"remote: {remote.get('host', '-')}: {remote.get('workspace', '-')}")

if fmax:
    print()
    print("fmax:")
    for name, values in sorted(fmax.items()):
        achieved = values.get("achieved", "-")
        constraint = values.get("constraint", "-")
        if isinstance(achieved, (int, float)):
            achieved = f"{achieved:.2f}"
        if isinstance(constraint, (int, float)):
            constraint = f"{constraint:.2f}"
        print(f"  {name}: achieved {achieved} MHz, constraint {constraint} MHz")

if util:
    print()
    print("utilization:")
    for key in ("TRELLIS_COMB", "TRELLIS_FF", "DP16KD", "MULT18X18D", "TRELLIS_IO"):
        values = util.get(key)
        if values:
            print(f"  {key}: {values.get('used', '-')}/{values.get('available', '-')}")

roms = source.get("rom_inputs", [])
if roms:
    print()
    print("rom inputs:")
    for item in roms:
        print(f"  {item['path']}  {item['size_bytes']} bytes  sha256={item['sha256'][:16]}...")

changed = source.get("changed_files", [])
if changed:
    print()
    print("dirty files:")
    for line in changed:
        print(f"  {line}")

diff_stat = source.get("diff_stat", "")
if diff_stat:
    print()
    print("diff stat:")
    print(diff_stat)
PY
        ;;
    *)
        echo "usage: $0 [list|show] [latest|BITSTREAM|METADATA]" >&2
        exit 2
        ;;
esac
