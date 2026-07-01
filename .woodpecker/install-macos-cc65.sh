#!/bin/sh
set -eu

# WAL-164: self-provision cc65 (ca65/ld65) on the macOS Woodpecker agent so the
# tag-release build-macos leg is reproducible and needs no manual host setup.
#
# `make binaries` assembles the EhBASIC/module ROMs with `ca65 --cpu w65c02`.
# The agent had no cc65 on PATH (`ca65: No such file or directory` killed the
# build). This mirrors the linux leg (install-linux-ci-deps.sh): if a
# w65c02-capable ca65 is already present (Homebrew, or a prior run's build),
# reuse it; otherwise build the pinned cc65 from source into a user-writable
# prefix (no sudo) that persists on the local backend, so the cost is one-time.
#
# The caller must add "$HOME/.local/bin" to PATH before `make binaries` — an
# exported PATH in this child script does not propagate back to the parent shell.

# cc65 master @ 2026-05-19 — pinned to the same ref the linux leg builds so both
# platforms assemble identical ROMs.
cc65_ref="cc3c40c54e51b2d9a22b63c85c418a2b11763377"
prefix="${CC65_PREFIX:-$HOME/.local}"

export PATH="$prefix/bin:$PATH"

if ca65 --cpu w65c02 --version >/dev/null 2>&1; then
  echo "cc65 already provides a w65c02-capable ca65 — skipping build"
  ca65 --cpu w65c02 --version || true
  exit 0
fi

echo "Building pinned cc65 ($cc65_ref) from source into $prefix ..."
cc65_src="$(mktemp -d)"
trap 'rm -rf "$cc65_src"' EXIT
git clone --filter=blob:none -q https://github.com/cc65/cc65.git "$cc65_src/cc65"
git -C "$cc65_src/cc65" checkout -q "$cc65_ref"
make -C "$cc65_src/cc65" -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 2)"
make -C "$cc65_src/cc65" install PREFIX="$prefix"

ca65 --cpu w65c02 --version
echo "cc65 installed to $prefix/bin"
