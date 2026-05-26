#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE="$ROOT/docs/programs/demo.ndi"
TMP="${TMPDIR:-/tmp}/nova-showcase-build-$$"

mkdir -p "$TMP"

nova() {
    dotnet run --project "$ROOT/e6502.Nova" -- "$@"
}

import_file() {
    local host_file="$1"
    local dest_dir="$2"
    nova import "$IMAGE" "$host_file" "$dest_dir"
}

import_midi() {
    local host_file="$1"
    local dest_dir="$2"
    nova import "$IMAGE" "$host_file" "$dest_dir" --raw-midi
}

import_sid() {
    local source_name="$1"
    local dest_name="$2"
    local dest_dir="$3"

    cp "$ROOT/docs/programs/sid/$source_name" "$TMP/$dest_name"
    import_file "$TMP/$dest_name" "$dest_dir"
}

make -C "$ROOT/assembly" keyboard demo

nova create "$IMAGE" --size 4096 --label SHOWCASE
for dir in featured 2sid sid wts arcade; do
    nova mkdir "$IMAGE" "/$dir"
done

cp "$ROOT/assembly/apps/demo/demo.bin" "$TMP/AUTOBOOT.bin"
cp "$ROOT/assembly/apps/keyboard/keyboard.bin" "$TMP/KEYBOARD.bin"
import_file "$TMP/AUTOBOOT.bin" /
import_file "$TMP/KEYBOARD.bin" /

# Featured: deliberately ordered to match the scene labels in assembly/apps/demo/demo.s.
import_sid Love_Fileosophy_2SID.sid Love_Fileosophy_2SID.sid /featured
import_sid Fratres_2SID.sid Fratres_2SID.sid /featured
import_sid commando.sid commando.sid /featured
import_sid monty-on-the-run.sid monty-on-the-run.sid /featured
import_sid tubular_bells_ii_shake_airwolf_style.sid tubular-airwolf.sid /featured
import_midi "$ROOT/docs/programs/midi/sousa-stars-stripes.mid" /featured
import_midi "$ROOT/docs/programs/midi/bach-toccata-dm.mid" /featured
import_midi "$ROOT/docs/programs/midi/joplin-entertainer.mid" /featured
import_midi "$ROOT/docs/programs/midi/miami-vice.mid" /featured
import_midi "$ROOT/docs/programs/midi/tetris-theme.mid" /featured
import_midi "$ROOT/docs/programs/midi/castlevania-bloody-tears.mid" /featured
import_midi "$ROOT/docs/programs/midi/star-wars.mid" /featured

# Stereo SID bench.
import_sid Love_Fileosophy_2SID.sid Love_Fileosophy_2SID.sid /2sid
import_sid Fratres_2SID.sid Fratres_2SID.sid /2sid
import_sid Popel_Premiere_2SID.sid Popel_Premiere_2SID.sid /2sid

# Classic SID bench.
for sid in \
    commando.sid \
    monty-on-the-run.sid \
    parallax.sid \
    wizball.sid \
    crazy-comets.sid \
    delta.sid \
    cybernoid.sid \
    sanxion.sid \
    lightforce.sid \
    master-of-magic.sid; do
    import_sid "$sid" "$sid" /sid
done

# WTS/MIDI bench. Imported raw so the current MIDPLAY path can play it.
for midi in \
    sousa-stars-stripes.mid \
    bach-toccata-dm.mid \
    joplin-entertainer.mid \
    debussy-clair-de-lune.mid \
    miami-vice.mid \
    tetris-theme.mid \
    castlevania-bloody-tears.mid \
    star-wars.mid \
    grieg-mountain-king.mid \
    pink-panther.mid; do
    import_midi "$ROOT/docs/programs/midi/$midi" /wts
done

# High-energy game and screen-theme set.
for midi in \
    sonic-green-hill.mid \
    super-mario-bros.mid \
    zelda-overworld.mid \
    megaman2-wily.mid \
    street-fighter-2.mid \
    contra-theme.mid \
    mission-impossible.mid \
    x-files.mid \
    ghostbusters.mid \
    hawaii-five-o.mid; do
    import_midi "$ROOT/docs/programs/midi/$midi" /arcade
done

nova validate "$IMAGE"
nova dir "$IMAGE"
nova dir "$IMAGE" /featured
