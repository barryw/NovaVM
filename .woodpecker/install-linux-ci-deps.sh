#!/bin/sh
set -eu

mode="${1:-build}"

apt-get update -qq

case "$mode" in
  build)
    apt-get install -y -qq --no-install-recommends \
      ca-certificates \
      clang \
      curl \
      fonts-lmodern \
      git \
      gzip \
      latexmk \
      lmodern \
      make \
      pandoc \
      python3-yaml \
      tar \
      texlive-fonts-recommended \
      texlive-latex-extra \
      texlive-latex-recommended \
      zlib1g-dev
    ;;
  release)
    apt-get install -y -qq --no-install-recommends \
      ca-certificates \
      curl \
      git \
      gzip \
      tar
    ;;
  *)
    echo "usage: $0 [build|release]" >&2
    exit 2
    ;;
esac

# cc65: the ehbasic ROM build assembles with `ca65 --cpu w65c02` (WDC 65C02).
# Ubuntu's packaged cc65 (2.19, 2020) predates the `w65c02` CPU target and fails
# with `ca65: Invalid CPU: 'w65c02'` (WAL-162 — this build path never ran in CI
# before ci.yaml was served). Build a pinned newer cc65 from source and put
# ca65/ld65 on PATH via /usr/local/bin (which precedes /usr/bin). Build-mode only;
# the release job does not touch ROM sources.
if [ "$mode" = "build" ] && ! ca65 --cpu w65c02 --version >/dev/null 2>&1; then
  cc65_ref="cc3c40c54e51b2d9a22b63c85c418a2b11763377"  # cc65 master @ 2026-05-19
  cc65_src="$(mktemp -d)"
  git clone --filter=blob:none -q https://github.com/cc65/cc65.git "$cc65_src/cc65"
  git -C "$cc65_src/cc65" checkout -q "$cc65_ref"
  make -C "$cc65_src/cc65" -j"$(nproc)" CC=clang >/dev/null
  make -C "$cc65_src/cc65" install PREFIX=/usr/local >/dev/null
  rm -rf "$cc65_src"
fi

if ! command -v cog >/dev/null 2>&1; then
  curl -fsSL https://github.com/cocogitto/cocogitto/releases/download/6.5.0/cocogitto-6.5.0-x86_64-unknown-linux-musl.tar.gz \
    | tar -xz --strip-components=1 -C /usr/local/bin x86_64-unknown-linux-musl/cog
fi

if [ "$mode" = "release" ] && ! command -v gh >/dev/null 2>&1; then
  curl -fsSL https://github.com/cli/cli/releases/download/v2.74.0/gh_2.74.0_linux_amd64.tar.gz \
    | tar -xz --strip-components=2 -C /usr/local/bin gh_2.74.0_linux_amd64/bin/gh
fi

apt-get clean
rm -rf /var/lib/apt/lists/*
