#!/usr/bin/env bash
# Capture one real HDMI frame through a macOS AVFoundation capture device.
#
# Defaults match the current NovaVM lab setup:
#   - Cam Link 4K is AVFoundation video device 0.
#   - NovaVM HDMI is currently 720x480 at 30fps through the capture path.

set -euo pipefail

if [ "${1:-}" = "--list" ]; then
    ffmpeg -hide_banner -f avfoundation -list_devices true -i "" || true
    exit 0
fi

DEVICE="${HDMI_DEVICE:-0:none}"
SIZE="${HDMI_SIZE:-720x480}"
FRAMERATE="${HDMI_FRAMERATE:-30}"
OUT="${1:-screenshots/hardware/novavm-hdmi-$(date +%Y%m%d_%H%M%S).png}"

mkdir -p "$(dirname "$OUT")"

ffmpeg \
    -hide_banner \
    -loglevel warning \
    -y \
    -f avfoundation \
    -pixel_format uyvy422 \
    -framerate "$FRAMERATE" \
    -video_size "$SIZE" \
    -i "$DEVICE" \
    -frames:v 1 \
    -update 1 \
    "$OUT"

echo "$OUT"
