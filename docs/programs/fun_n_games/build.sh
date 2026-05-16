#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
SRC="$ROOT/docs/programs/fun_n_games"
BUILD="${TMPDIR:-/tmp}/fun_n_games_build_$$"
IMAGE="$ROOT/docs/programs/fun_n_games.ndi"

mkdir -p "$BUILD"

cp "$SRC/common.bas" "$BUILD/common.bas"
for stub in "$SRC"/stubs/*.bas; do
    name="$(basename "$stub")"
    awk 'FNR==1 && NR!=1 { print "" } { print }' "$stub" "$SRC/common.bas" > "$BUILD/$name"
done

dotnet run --project "$ROOT/e6502.NDI" -- create "$IMAGE" --size 800 --label FUNNGAMES
for source in "$BUILD"/*.bas; do
    dotnet run --project "$ROOT/e6502.NDI" -- import "$IMAGE" "$source" / --tokenize >/dev/null
done
dotnet run --project "$ROOT/e6502.NDI" -- validate "$IMAGE"
