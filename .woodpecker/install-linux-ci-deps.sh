#!/bin/sh
set -eu

mode="${1:-build}"

apt-get update -qq

case "$mode" in
  build)
    apt-get install -y -qq --no-install-recommends \
      ca-certificates \
      cc65 \
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
