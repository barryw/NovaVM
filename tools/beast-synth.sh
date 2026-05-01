#!/usr/bin/env bash
# beast-synth.sh — single-command FPGA synth on beast with bitstream pull.
#
# Replaces the error-prone 3-step "rsync, ssh make, rsync back" dance.
# Always mirrors the full e6502.FPGA/{rtl,fpga,rom} tree (no per-file rsync
# syntax to get wrong), runs the build target on beast, pulls the bitstream
# back to bit_backups/ with a date-versioned name, writes metadata, prunes old
# script-managed artifacts, and prints timing.
#
# Usage:
#   tools/beast-synth.sh                    # bitstream (full synth + pnr + ecppack)
#   tools/beast-synth.sh diag               # yosys-only, ~3-4 min, dumps post_synth.v
#   tools/beast-synth.sh diag-vgc           # yosys-only VGC subsystem
#   tools/beast-synth.sh diag-sprites       # yosys-only sprite engine
#   tools/beast-synth.sh bitstream foo      # bitstream, saved as bit_backups/e6502.YYYYmmdd_HHMMSS.foo.bit
#   tools/beast-synth.sh bitstream foo "-DNAME -DOTHER"
#
# Env overrides:
#   BEAST_HOST=beast
#   BEAST_WS=~/synth_ws
#   YOSYS_BIN=$HOME/oss-cad-suite/bin
#   BEAST_CLEAN=0          # preserve remote build/ for faster iteration
#   NEXTPNR_THREADS=8      # optional nextpnr thread count
#   NEXTPNR_SEED=1         # deterministic by default
#   CHECK_TIMING=0         # skip local nextpnr report timing gate
#   TIMING_MARGIN_MHZ=1.0  # require extra timing headroom
#   BITSTREAM_RETENTION=7  # keep this many script-managed bitstreams
#   BITSTREAM_NOTES='...'  # human note stored in metadata

set -euo pipefail

BEAST_HOST="${BEAST_HOST:-beast}"
BEAST_WS="${BEAST_WS:-/home/barry/e6502_synth}"
YOSYS_BIN="${YOSYS_BIN:-/home/barry/oss-cad-suite/bin}"
DEFAULT_EXTRA_DEFINES="-DVIDEO_720X480 -DGPDI_P_ONLY"
BEAST_CLEAN="${BEAST_CLEAN:-1}"
NEXTPNR_THREADS="${NEXTPNR_THREADS:-}"
NEXTPNR_SEED="${NEXTPNR_SEED:-1}"
CHECK_TIMING="${CHECK_TIMING:-1}"
TIMING_MARGIN_MHZ="${TIMING_MARGIN_MHZ:-0.0}"
BITSTREAM_RETENTION="${BITSTREAM_RETENTION:-7}"
BITSTREAM_NOTES="${BITSTREAM_NOTES:-}"

TARGET="${1:-bitstream}"
LABEL="${2:-}"
EXTRA_DEFINES="${EXTRA_DEFINES:-${3:-$DEFAULT_EXTRA_DEFINES}}"

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOCAL_FPGA="$REPO_ROOT/e6502.FPGA"
BACKUP_DIR="$LOCAL_FPGA/fpga/bit_backups"

if [ ! -d "$LOCAL_FPGA" ]; then
    echo "error: e6502.FPGA dir not found at $LOCAL_FPGA" >&2
    exit 1
fi

ts="$(date +%Y%m%d_%H%M%S)"
built_at_utc="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
safe_label="$(printf '%s' "$LABEL" | tr -cs 'A-Za-z0-9._-' '_' | sed 's/^_//; s/_$//')"
label_part=""
[ -n "$safe_label" ] && label_part=".$safe_label"

commit="$(git -C "$REPO_ROOT" rev-parse --short HEAD 2>/dev/null || echo unknown)"
branch="$(git -C "$REPO_ROOT" rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
dirty_count="$(git -C "$REPO_ROOT" status --porcelain -- e6502.FPGA tools/beast-synth.sh 2>/dev/null | wc -l | tr -d ' ')"
dirty_state="clean"
[ "$dirty_count" != "0" ] && dirty_state="dirty"

file_mtime() {
    stat -f %m "$1" 2>/dev/null || stat -c %Y "$1" 2>/dev/null || echo 0
}

prune_bitstream_backups() {
    local keep="$1"

    case "$keep" in
        ''|*[!0-9]*)
            echo "warning: BITSTREAM_RETENTION must be a non-negative integer; got '$keep'" >&2
            return 0
            ;;
    esac

    if [ "$keep" -eq 0 ]; then
        echo "bitstream retention disabled"
        return 0
    fi

    # Only prune artifacts produced by this script version. Legacy/manual
    # backups do not have .metadata.json and are intentionally left alone.
    find "$BACKUP_DIR" -maxdepth 1 -type f -name 'e6502.*.metadata.json' -print0 |
        while IFS= read -r -d '' metadata; do
            printf '%s\t%s\n' "$(file_mtime "$metadata")" "$metadata"
        done |
        sort -rn |
        awk -v keep="$keep" 'NR > keep { sub(/^[^\t]*\t/, ""); print }' |
        while IFS= read -r metadata; do
            base="${metadata%.metadata.json}"
            echo "pruning old bitstream artifact: ${base}.bit"
            rm -f "${base}.bit" \
                  "${base}.config" \
                  "${base}.log" \
                  "${base}.manifest" \
                  "${base}.metadata.json" \
                  "${base}.nextpnr-report.json"
        done
}

echo "=== [1/4] mirroring $LOCAL_FPGA → $BEAST_HOST:$BEAST_WS/e6502.FPGA"
rsync -az --delete \
      "$LOCAL_FPGA/rtl/"  "$BEAST_HOST:$BEAST_WS/e6502.FPGA/rtl/"
rsync -az --delete \
      "$LOCAL_FPGA/fpga/" "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/" \
      --exclude='build/' --exclude='bit_backups/'
rsync -az --delete \
      "$LOCAL_FPGA/rom/"  "$BEAST_HOST:$BEAST_WS/e6502.FPGA/rom/"

echo "=== [2/4] running 'make $TARGET' on $BEAST_HOST"
echo "target:        $TARGET"
echo "label:         ${LABEL:-none}"
echo "extra_defines: ${EXTRA_DEFINES:-none}"
echo "clean_build:   $BEAST_CLEAN"
echo "nextpnr_seed:  $NEXTPNR_SEED"
echo "nextpnr_jobs:  ${NEXTPNR_THREADS:-default}"
echo "check_timing:  $CHECK_TIMING"
echo "retention:     $BITSTREAM_RETENTION"
echo "commit:        $commit ($dirty_state)"
remote_clean_cmd=""
if [ "$BEAST_CLEAN" != "0" ]; then
    remote_clean_cmd="rm -rf build &&"
fi
ssh "$BEAST_HOST" "cd $BEAST_WS/e6502.FPGA/fpga && \
    $remote_clean_cmd \
    make OSS_CAD_SUITE_BIN=$YOSYS_BIN EXTRA_DEFINES='$EXTRA_DEFINES' NEXTPNR_SEED=$NEXTPNR_SEED NEXTPNR_THREADS='$NEXTPNR_THREADS' -o ehbasic $TARGET 2>&1 | tee /tmp/beast-synth.log | tail -200"

echo "=== [3/4] timing summary"
ssh "$BEAST_HOST" "grep -E '(Max frequency|Info: Device utilisation|CCU2C|DP16KD|LUT4|MULT18X18D|PFUMX|TRELLIS_DPR16X4|TRELLIS_FF|Total)' /tmp/beast-synth.log | tail -40" || true

if [ "$TARGET" = "bitstream" ]; then
    echo "=== [4/4] pulling bitstream"
    mkdir -p "$BACKUP_DIR"
    out="$BACKUP_DIR/e6502.${ts}${label_part}.bit"
    config_out="${out%.bit}.config"
    log_out="${out%.bit}.log"
    metadata_out="${out%.bit}.metadata.json"
    manifest_out="${out%.bit}.manifest"
    report_out="${out%.bit}.nextpnr-report.json"
    rsync -az "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/build/e6502.bit" "$out"
    rsync -az "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/build/e6502.config" "$config_out"
    rsync -az "$BEAST_HOST:/tmp/beast-synth.log" "$log_out"
    if ssh "$BEAST_HOST" "test -f $BEAST_WS/e6502.FPGA/fpga/build/nextpnr-report.json"; then
        rsync -az "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/build/nextpnr-report.json" "$report_out"
    else
        echo "warning: nextpnr report was not produced" >&2
    fi
    md5="$(md5 -q "$out" 2>/dev/null || md5sum "$out" | cut -d' ' -f1)"
    size_bytes="$(wc -c < "$out" | tr -d ' ')"
    changed_files="$(git -C "$REPO_ROOT" status --porcelain -- e6502.FPGA tools/beast-synth.sh tools/flash-ulx3s-stack.sh 2>/dev/null || true)"
    diff_stat="$(git -C "$REPO_ROOT" diff --stat -- e6502.FPGA tools/beast-synth.sh tools/flash-ulx3s-stack.sh 2>/dev/null || true)"
    export METADATA_OUT="$metadata_out"
    export REPO_ROOT="$REPO_ROOT"
    export BITSTREAM_OUT="$out"
    export BITSTREAM_CONFIG_OUT="$config_out"
    export BITSTREAM_LOG_OUT="$log_out"
    export BITSTREAM_REPORT_OUT="$report_out"
    export BITSTREAM_MD5="$md5"
    export BITSTREAM_SIZE_BYTES="$size_bytes"
    export BITSTREAM_TARGET="$TARGET"
    export BITSTREAM_LABEL="$LABEL"
    export BITSTREAM_SAFE_LABEL="$safe_label"
    export BITSTREAM_EXTRA_DEFINES="$EXTRA_DEFINES"
    export BITSTREAM_CLEAN_BUILD="$BEAST_CLEAN"
    export BITSTREAM_NEXTPNR_SEED="$NEXTPNR_SEED"
    export BITSTREAM_NEXTPNR_THREADS="${NEXTPNR_THREADS:-default}"
    export BITSTREAM_CHECK_TIMING="$CHECK_TIMING"
    export BITSTREAM_TIMING_MARGIN_MHZ="$TIMING_MARGIN_MHZ"
    export BITSTREAM_COMMIT="$commit"
    export BITSTREAM_BRANCH="$branch"
    export BITSTREAM_DIRTY_STATE="$dirty_state"
    export BITSTREAM_DIRTY_COUNT="$dirty_count"
    export BITSTREAM_CHANGED_FILES="$changed_files"
    export BITSTREAM_DIFF_STAT="$diff_stat"
    export BITSTREAM_BEAST_HOST="$BEAST_HOST"
    export BITSTREAM_BEAST_WORKSPACE="$BEAST_WS"
    export BITSTREAM_YOSYS_BIN="$YOSYS_BIN"
    export BITSTREAM_BUILT_AT_LOCAL="$ts"
    export BITSTREAM_BUILT_AT_UTC="$built_at_utc"
    python3 - <<'PY'
import json
import os
from pathlib import Path
import hashlib

def env(name: str, default: str = "") -> str:
    return os.environ.get(name, default)

repo_root = Path(env("REPO_ROOT")).resolve()

def file_info(path: Path) -> dict:
    rel = path.relative_to(repo_root).as_posix()
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return {
        "path": rel,
        "size_bytes": path.stat().st_size,
        "sha256": h.hexdigest(),
    }

def collect_inputs() -> list[dict]:
    roots = [
        repo_root / "e6502.FPGA" / "rtl",
        repo_root / "e6502.FPGA" / "fpga",
    ]
    suffixes = {".sv", ".v", ".vh", ".lpf", ".mk"}
    names = {"Makefile"}
    out = []
    for root in roots:
        if not root.exists():
            continue
        for path in sorted(root.rglob("*")):
            rel_parts = path.relative_to(root).parts
            if not path.is_file():
                continue
            if "build" in rel_parts or "bit_backups" in rel_parts:
                continue
            if path.suffix in suffixes or path.name in names:
                out.append(file_info(path))
    return out

def collect_roms() -> list[dict]:
    rom_dir = repo_root / "e6502.FPGA" / "rom"
    if not rom_dir.exists():
        return []
    return [
        file_info(path)
        for path in sorted(rom_dir.glob("*.hex"))
        if path.is_file()
    ]

def timing_summary(report_path: str) -> dict:
    path = Path(report_path)
    if not path.exists():
        return {}
    try:
        report = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        return {"parse_error": str(exc)}
    return {
        "fmax": report.get("fmax", {}),
        "utilization": report.get("utilization", {}),
    }

changed_files = [
    line
    for line in env("BITSTREAM_CHANGED_FILES").splitlines()
    if line.strip()
]

metadata = {
    "schema": 1,
    "project": "e6502",
    "artifact": {
        "bitstream": env("BITSTREAM_OUT"),
        "config": env("BITSTREAM_CONFIG_OUT"),
        "log": env("BITSTREAM_LOG_OUT"),
        "nextpnr_report": env("BITSTREAM_REPORT_OUT"),
        "md5": env("BITSTREAM_MD5"),
        "size_bytes": int(env("BITSTREAM_SIZE_BYTES", "0") or "0"),
        "built_at_local": env("BITSTREAM_BUILT_AT_LOCAL"),
        "built_at_utc": env("BITSTREAM_BUILT_AT_UTC"),
    },
    "build": {
        "target": env("BITSTREAM_TARGET"),
        "label": env("BITSTREAM_LABEL"),
        "safe_label": env("BITSTREAM_SAFE_LABEL"),
        "notes": env("BITSTREAM_NOTES"),
        "extra_defines": env("BITSTREAM_EXTRA_DEFINES"),
        "clean_build": env("BITSTREAM_CLEAN_BUILD"),
        "nextpnr_seed": env("BITSTREAM_NEXTPNR_SEED"),
        "nextpnr_threads": env("BITSTREAM_NEXTPNR_THREADS"),
        "check_timing": env("BITSTREAM_CHECK_TIMING"),
        "timing_margin_mhz": env("BITSTREAM_TIMING_MARGIN_MHZ"),
    },
    "source": {
        "commit": env("BITSTREAM_COMMIT"),
        "branch": env("BITSTREAM_BRANCH"),
        "dirty_state": env("BITSTREAM_DIRTY_STATE"),
        "dirty_count": int(env("BITSTREAM_DIRTY_COUNT", "0") or "0"),
        "changed_files": changed_files,
        "diff_stat": env("BITSTREAM_DIFF_STAT"),
        "rtl_inputs": collect_inputs(),
        "rom_inputs": collect_roms(),
    },
    "timing": timing_summary(env("BITSTREAM_REPORT_OUT")),
    "remote": {
        "host": env("BITSTREAM_BEAST_HOST"),
        "workspace": env("BITSTREAM_BEAST_WORKSPACE"),
        "yosys_bin": env("BITSTREAM_YOSYS_BIN"),
    },
}

with open(env("METADATA_OUT"), "w", encoding="utf-8") as f:
    json.dump(metadata, f, indent=2, sort_keys=True)
    f.write("\n")
PY
    {
        echo "bitstream: $out"
        echo "config: $config_out"
        echo "log: $log_out"
        echo "metadata: $metadata_out"
        echo "nextpnr_report: $report_out"
        echo "md5: $md5"
        echo "size_bytes: $size_bytes"
        echo "target: $TARGET"
        echo "label: ${LABEL:-}"
        echo "notes: ${BITSTREAM_NOTES:-}"
        echo "extra_defines: ${EXTRA_DEFINES:-}"
        echo "clean_build: $BEAST_CLEAN"
        echo "nextpnr_seed: $NEXTPNR_SEED"
        echo "nextpnr_threads: ${NEXTPNR_THREADS:-default}"
        echo "check_timing: $CHECK_TIMING"
        echo "timing_margin_mhz: $TIMING_MARGIN_MHZ"
        echo "commit: $commit"
        echo "branch: $branch"
        echo "dirty_state: $dirty_state"
        echo "dirty_count: $dirty_count"
        echo "beast_host: $BEAST_HOST"
        echo "beast_workspace: $BEAST_WS"
        echo "yosys_bin: $YOSYS_BIN"
        echo "built_at_local: $ts"
        echo "built_at_utc: $built_at_utc"
    } > "$manifest_out"
    echo "bitstream: $out"
    echo "md5:       $md5"
    echo "config:    $config_out"
    echo "log:       $log_out"
    echo "report:    $report_out"
    echo "metadata:  $metadata_out"
    echo "manifest:  $manifest_out"
    if [ "$CHECK_TIMING" != "0" ] && [ -f "$report_out" ]; then
        echo
        echo "timing gate:"
        python3 "$REPO_ROOT/tools/check-fpga-timing.py" "$report_out" --margin-mhz "$TIMING_MARGIN_MHZ"
    fi
    prune_bitstream_backups "$BITSTREAM_RETENTION"
    echo
    echo "to flash:  openFPGALoader --board ulx3s -f $out"
else
    echo "=== [4/4] (skipped — target was '$TARGET', no bitstream)"
    case "$TARGET" in
        diag-sprites)
            echo "post_synth.v on beast: $BEAST_WS/e6502.FPGA/fpga/build/post_synth_sprites.v"
            ;;
        diag-vgc)
            echo "post_synth.v on beast: $BEAST_WS/e6502.FPGA/fpga/build/post_synth_vgc.v"
            ;;
        *)
            echo "post_synth.v on beast: $BEAST_WS/e6502.FPGA/fpga/build/post_synth.v"
            ;;
    esac
    echo "log:                   $BEAST_HOST:/tmp/beast-synth.log"
fi
