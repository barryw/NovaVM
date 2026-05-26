#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
rid="${1:-}"

if [[ -z "$rid" ]]; then
    case "$(uname -s)-$(uname -m)" in
        Darwin-arm64) rid="osx-arm64" ;;
        Darwin-x86_64) rid="osx-x64" ;;
        Linux-x86_64) rid="linux-x64" ;;
        Linux-aarch64|Linux-arm64) rid="linux-arm64" ;;
        *)
            echo "error: pass a runtime identifier, e.g. osx-arm64, linux-x64, linux-arm64, win-x64" >&2
            exit 1
            ;;
    esac
fi

out="$repo_root/artifacts/nova-cli/$rid"
exe_name="nova"

if [[ "$rid" == win-* ]]; then
    exe_name="nova.exe"
fi

dotnet publish "$repo_root/e6502.Nova/e6502.Nova.csproj" \
    -c Release \
    -r "$rid" \
    --self-contained true \
    -p:PublishAot=true \
    --tl:off \
    -o "$out"

published="$out/$exe_name"

if [[ ! -f "$published" ]]; then
    echo "error: NativeAOT publish completed without producing $published" >&2
    exit 1
fi

echo "Published Nova CLI: $published"
