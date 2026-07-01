#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
out_dir="$repo_root/e6502.Browser/wwwroot/rust"

rustup run stable cargo build \
  --manifest-path "$repo_root/e6502.Browser.RustCore/Cargo.toml" \
  --target wasm32-unknown-unknown \
  --release

mkdir -p "$out_dir"
cp "$repo_root/e6502.Browser.RustCore/target/wasm32-unknown-unknown/release/nova_browser_core.wasm" \
  "$out_dir/nova_browser_core.wasm"
cp "$repo_root/e6502.Avalonia/Resources/ehbasic.bin" "$out_dir/ehbasic.bin"
cp "$repo_root/e6502.Avalonia/Resources/novalogo.bin" "$out_dir/novalogo.bin"
cp "$repo_root/e6502.Avalonia/Resources/novaforth.bin" "$out_dir/novaforth.bin"
cp "$repo_root/e6502.Avalonia/Resources/extension.bin" "$out_dir/extension.bin"
cp "$repo_root/e6502.Avalonia/Resources/cp437.bin" "$out_dir/cp437.bin"
cp "$repo_root/e6502.Avalonia/Resources/libcall.bin" "$out_dir/libcall.bin"
cp "$repo_root/e6502.Avalonia/Resources/graphics.bin" "$out_dir/graphics.bin"
cp "$repo_root/e6502.Avalonia/Resources/system.bin" "$out_dir/system.bin"
cp "$repo_root/e6502.Avalonia/Resources/sound.bin" "$out_dir/sound.bin"
cp "$repo_root/e6502.Avalonia/Resources/editor.bin" "$out_dir/editor.bin"
cp "$repo_root/e6502.Avalonia/Resources/files.bin" "$out_dir/files.bin"
cp "$repo_root/e6502.Avalonia/Resources/memory.bin" "$out_dir/memory.bin"
cp "$repo_root/e6502.Avalonia/Resources/net.bin" "$out_dir/net.bin"
cp "$repo_root/e6502.Avalonia/Resources/turtle.bin" "$out_dir/turtle.bin"

forth_disk="$repo_root/e6502.Browser/wwwroot/forth.ndi"
tmp_forth_disk="$(mktemp "${TMPDIR:-/tmp}/novavm-forth.XXXXXX.ndi")"
rm -f "$tmp_forth_disk"

nova=(dotnet run --project "$repo_root/e6502.Nova" --)
"${nova[@]}" create "$tmp_forth_disk" --size 800 --label FORTH >/dev/null

while IFS= read -r dir; do
  rel="${dir#$repo_root/software/languages/novaforth/}"
  "${nova[@]}" mkdir "$tmp_forth_disk" "/$rel" >/dev/null
done < <(find "$repo_root/software/languages/novaforth/forth" -type d | sort)

while IFS= read -r file; do
  rel="${file#$repo_root/software/languages/novaforth/}"
  dest_dir="/${rel%/*}"
  "${nova[@]}" import "$tmp_forth_disk" "$file" "$dest_dir" >/dev/null
done < <(find "$repo_root/software/languages/novaforth/forth" -type f \( -name '*.4th' -o -name '*.fth' -o -name '*.fr' -o -name '*.fs' \) | sort)

"${nova[@]}" validate "$tmp_forth_disk" >/dev/null
cp "$tmp_forth_disk" "$forth_disk"
rm -f "$tmp_forth_disk"

echo "Rust browser core assets written to $out_dir"
