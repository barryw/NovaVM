#!/usr/bin/env bash
# beast-synth.sh — single-command FPGA synth on beast with bitstream pull.
#
# Replaces the error-prone 3-step "rsync, ssh make, rsync back" dance.
# Always mirrors the full e6502.FPGA/{rtl,fpga,rom} tree (no per-file rsync
# syntax to get wrong), runs the build target on beast, pulls the bitstream
# back to bit_backups/ with a timestamped name, and prints timing.
#
# Usage:
#   tools/beast-synth.sh                    # bitstream (full synth + pnr + ecppack)
#   tools/beast-synth.sh diag               # yosys-only, ~3-4 min, dumps post_synth.v
#   tools/beast-synth.sh diag-vgc           # yosys-only VGC subsystem
#   tools/beast-synth.sh diag-sprites       # yosys-only sprite engine
#   tools/beast-synth.sh bitstream foo      # bitstream, label saved as bit_backups/e6502.foo_*.bit
#   tools/beast-synth.sh bitstream foo -DNAME
#
# Env overrides:
#   BEAST_HOST=beast
#   BEAST_WS=~/synth_ws
#   YOSYS_BIN=$HOME/oss-cad-suite/bin

set -euo pipefail

BEAST_HOST="${BEAST_HOST:-beast}"
BEAST_WS="${BEAST_WS:-/home/barry/e6502_synth}"
YOSYS_BIN="${YOSYS_BIN:-/home/barry/oss-cad-suite/bin}"
TARGET="${1:-bitstream}"
LABEL="${2:-}"
EXTRA_DEFINES="${EXTRA_DEFINES:-${3:-}}"

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LOCAL_FPGA="$REPO_ROOT/e6502.FPGA"
BACKUP_DIR="$LOCAL_FPGA/fpga/bit_backups"

if [ ! -d "$LOCAL_FPGA" ]; then
    echo "error: e6502.FPGA dir not found at $LOCAL_FPGA" >&2
    exit 1
fi

ts="$(date +%Y%m%d_%H%M%S)"
label_part=""
[ -n "$LABEL" ] && label_part="${LABEL}_"

commit="$(git -C "$REPO_ROOT" rev-parse --short HEAD 2>/dev/null || echo unknown)"
dirty_count="$(git -C "$REPO_ROOT" status --porcelain -- e6502.FPGA tools/beast-synth.sh 2>/dev/null | wc -l | tr -d ' ')"
dirty_state="clean"
[ "$dirty_count" != "0" ] && dirty_state="dirty"

echo "=== [1/4] mirroring $LOCAL_FPGA → $BEAST_HOST:$BEAST_WS/e6502.FPGA"
rsync -az --delete \
      "$LOCAL_FPGA/rtl/"  "$BEAST_HOST:$BEAST_WS/e6502.FPGA/rtl/"
rsync -az --delete \
      "$LOCAL_FPGA/fpga/" "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/" \
      --exclude='build/' --exclude='bit_backups/'
rsync -az --delete \
      "$LOCAL_FPGA/rom/"  "$BEAST_HOST:$BEAST_WS/e6502.FPGA/rom/"

echo "=== [2/4] running clean 'make $TARGET' on $BEAST_HOST"
echo "target:        $TARGET"
echo "label:         ${LABEL:-none}"
echo "extra_defines: ${EXTRA_DEFINES:-none}"
echo "commit:        $commit ($dirty_state)"
ssh "$BEAST_HOST" "cd $BEAST_WS/e6502.FPGA/fpga && \
    rm -rf build && \
    make OSS_CAD_SUITE_BIN=$YOSYS_BIN EXTRA_DEFINES='$EXTRA_DEFINES' -o ehbasic $TARGET 2>&1 | tee /tmp/beast-synth.log | tail -200"

echo "=== [3/4] timing summary"
ssh "$BEAST_HOST" "grep -E '(Max frequency|Info: Device utilisation|CCU2C|DP16KD|LUT4|MULT18X18D|PFUMX|TRELLIS_DPR16X4|TRELLIS_FF|Total)' /tmp/beast-synth.log | tail -40" || true

if [ "$TARGET" = "bitstream" ]; then
    echo "=== [4/4] pulling bitstream"
    mkdir -p "$BACKUP_DIR"
    out="$BACKUP_DIR/e6502.${label_part}${ts}.bit"
    log_out="${out%.bit}.log"
    manifest_out="${out%.bit}.manifest"
    rsync -az "$BEAST_HOST:$BEAST_WS/e6502.FPGA/fpga/build/e6502.bit" "$out"
    rsync -az "$BEAST_HOST:/tmp/beast-synth.log" "$log_out"
    md5="$(md5 -q "$out" 2>/dev/null || md5sum "$out" | cut -d' ' -f1)"
    {
        echo "bitstream: $out"
        echo "log: $log_out"
        echo "md5: $md5"
        echo "target: $TARGET"
        echo "label: ${LABEL:-}"
        echo "extra_defines: ${EXTRA_DEFINES:-}"
        echo "commit: $commit"
        echo "dirty_state: $dirty_state"
        echo "beast_host: $BEAST_HOST"
        echo "beast_workspace: $BEAST_WS"
        echo "yosys_bin: $YOSYS_BIN"
        echo "built_at: $ts"
    } > "$manifest_out"
    echo "bitstream: $out"
    echo "md5:       $md5"
    echo "log:       $log_out"
    echo "manifest:  $manifest_out"
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
