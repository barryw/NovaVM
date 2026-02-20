#!/usr/bin/env python3
"""Convert MIDI/LilyPond music into a NovaBASIC v1.0 SOUND program.

This transpiler emits a .bas file that plays up to 4 monophonic voices on the
NovaBASIC virtual sound controller.
"""

from __future__ import annotations

import argparse
import math
import shutil
import struct
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import DefaultDict, Iterable
from collections import defaultdict


class MidiParseError(RuntimeError):
    pass


@dataclass(frozen=True)
class TempoEvent:
    tick: int
    bpm: int


@dataclass(frozen=True)
class TimeSignature:
    numerator: int
    denominator: int


@dataclass(frozen=True)
class NoteEvent:
    start_tick: int
    end_tick: int
    channel: int
    note: int
    velocity: int


@dataclass(frozen=True)
class MidiSong:
    ppqn: int
    notes: list[NoteEvent]
    tempos: list[TempoEvent]
    time_signature: TimeSignature
    end_tick: int


@dataclass(frozen=True)
class VoiceConfig:
    wave: int
    attack: int
    decay: int
    sustain: int
    release: int


DEFAULT_VOICES: list[VoiceConfig] = [
    VoiceConfig(wave=2, attack=2, decay=6, sustain=10, release=6),
    VoiceConfig(wave=0, attack=1, decay=4, sustain=12, release=4),
    VoiceConfig(wave=4, attack=2, decay=5, sustain=10, release=6),
    VoiceConfig(wave=1, attack=2, decay=4, sustain=9, release=5),
]

NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]


def read_vlq(data: bytes, offset: int) -> tuple[int, int]:
    value = 0
    while True:
        if offset >= len(data):
            raise MidiParseError("Unexpected EOF while reading VLQ")
        byte = data[offset]
        offset += 1
        value = (value << 7) | (byte & 0x7F)
        if not (byte & 0x80):
            return value, offset


def midi_note_to_freq(note: int) -> int:
    return int(round(440.0 * (2.0 ** ((note - 69) / 12.0))))


def midi_note_to_name(note: int) -> str:
    octave = (note // 12) - 1
    return f"{NOTE_NAMES[note % 12]}{octave}"


def parse_midi(path: Path) -> MidiSong:
    data = path.read_bytes()
    offset = 0

    if data[offset:offset + 4] != b"MThd":
        raise MidiParseError("Missing MThd header")
    offset += 4

    header_len = struct.unpack_from(">I", data, offset)[0]
    offset += 4
    if header_len < 6:
        raise MidiParseError("Invalid MIDI header length")

    fmt, track_count, division = struct.unpack_from(">HHH", data, offset)
    offset += header_len

    if division & 0x8000:
        raise MidiParseError("SMPTE timing MIDI is not supported")

    ppqn = division
    if ppqn <= 0:
        raise MidiParseError("Invalid PPQN")

    notes: list[NoteEvent] = []
    tempos: list[TempoEvent] = []
    time_signature = TimeSignature(4, 4)
    song_end_tick = 0

    for _track_index in range(track_count):
        if data[offset:offset + 4] != b"MTrk":
            raise MidiParseError("Missing MTrk header")
        offset += 4
        track_len = struct.unpack_from(">I", data, offset)[0]
        offset += 4
        track = data[offset:offset + track_len]
        offset += track_len

        running_status: int | None = None
        pos = 0
        tick = 0
        active: DefaultDict[tuple[int, int], list[tuple[int, int]]] = defaultdict(list)

        while pos < len(track):
            delta, pos = read_vlq(track, pos)
            tick += delta

            if pos >= len(track):
                break

            status = track[pos]
            if status >= 0x80:
                pos += 1
                if status < 0xF0:
                    running_status = status
            else:
                if running_status is None:
                    raise MidiParseError("Running status used before status byte")
                status = running_status

            if status == 0xFF:
                if pos >= len(track):
                    raise MidiParseError("Malformed meta event")
                meta_type = track[pos]
                pos += 1
                meta_len, pos = read_vlq(track, pos)
                meta_data = track[pos:pos + meta_len]
                pos += meta_len

                if meta_type == 0x51 and len(meta_data) == 3:
                    us_per_quarter = (meta_data[0] << 16) | (meta_data[1] << 8) | meta_data[2]
                    if us_per_quarter > 0:
                        bpm = int(round(60000000 / us_per_quarter))
                        tempos.append(TempoEvent(tick=tick, bpm=max(1, bpm)))
                elif meta_type == 0x58 and len(meta_data) >= 2:
                    numerator = max(1, meta_data[0])
                    denominator = 2 ** meta_data[1]
                    if denominator > 0:
                        time_signature = TimeSignature(numerator, denominator)
                elif meta_type == 0x2F:
                    break

                continue

            if status in (0xF0, 0xF7):
                sysex_len, pos = read_vlq(track, pos)
                pos += sysex_len
                continue

            event_type = status & 0xF0
            channel = status & 0x0F

            if event_type in (0xC0, 0xD0):
                if pos >= len(track):
                    raise MidiParseError("Malformed MIDI event")
                pos += 1
                continue

            if pos + 1 >= len(track):
                raise MidiParseError("Malformed MIDI event")
            data1 = track[pos]
            data2 = track[pos + 1]
            pos += 2

            key = (channel, data1)
            if event_type == 0x90 and data2 > 0:
                active[key].append((tick, data2))
            elif event_type == 0x80 or (event_type == 0x90 and data2 == 0):
                stack = active.get(key)
                if stack:
                    start_tick, velocity = stack.pop()
                    if tick > start_tick:
                        notes.append(
                            NoteEvent(
                                start_tick=start_tick,
                                end_tick=tick,
                                channel=channel,
                                note=data1,
                                velocity=velocity,
                            )
                        )

        for (channel, note), stack in active.items():
            for start_tick, velocity in stack:
                if tick > start_tick:
                    notes.append(
                        NoteEvent(
                            start_tick=start_tick,
                            end_tick=tick,
                            channel=channel,
                            note=note,
                            velocity=velocity,
                        )
                    )

        song_end_tick = max(song_end_tick, tick)

    if not tempos:
        tempos = [TempoEvent(tick=0, bpm=120)]

    # Keep last tempo for each tick.
    tempo_by_tick: dict[int, int] = {}
    for tempo in tempos:
        tempo_by_tick[tempo.tick] = tempo.bpm
    tempos = [TempoEvent(tick=t, bpm=b) for t, b in sorted(tempo_by_tick.items())]

    return MidiSong(
        ppqn=ppqn,
        notes=notes,
        tempos=tempos,
        time_signature=time_signature,
        end_tick=song_end_tick,
    )


def lilypond_to_midi(input_path: Path, lilypond_bin: str) -> Path:
    lilypond = shutil.which(lilypond_bin)
    if lilypond is None:
        raise RuntimeError(
            "LilyPond input requested but 'lilypond' is not installed. "
            "Install LilyPond or pass a .mid file instead."
        )

    temp_dir = Path(tempfile.mkdtemp(prefix="score2bas-"))
    out_prefix = temp_dir / "score"

    result = subprocess.run(
        [lilypond, "-o", str(out_prefix), str(input_path)],
        cwd=temp_dir,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"LilyPond failed with code {result.returncode}.\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )

    mids = sorted(temp_dir.glob("*.mid"))
    if not mids:
        raise RuntimeError("LilyPond did not produce any .mid files")

    return mids[0]


def parse_channel_list(raw: str | None) -> list[int] | None:
    if raw is None:
        return None

    channels: list[int] = []
    for part in raw.split(","):
        token = part.strip()
        if not token:
            continue
        try:
            value = int(token)
        except ValueError as exc:
            raise ValueError(f"Invalid channel '{token}', expected integers 1-16") from exc
        if not (1 <= value <= 16):
            raise ValueError(f"Invalid channel '{token}', expected range 1-16")
        channels.append(value - 1)

    if not channels:
        return None
    return channels


def choose_voice_channels(notes: Iterable[NoteEvent], voices: int, explicit: list[int] | None) -> list[int]:
    if explicit:
        chosen = explicit[:voices]
        if len(chosen) < voices:
            used = set(chosen)
            remaining = [ch for ch in range(16) if ch not in used]
            chosen.extend(remaining[: voices - len(chosen)])
        return chosen

    by_channel: dict[int, int] = defaultdict(int)
    for note in notes:
        by_channel[note.channel] += 1

    ranked = [ch for ch, _count in sorted(by_channel.items(), key=lambda kv: (-kv[1], kv[0]))]
    if not ranked:
        return list(range(voices))

    if len(ranked) < voices:
        used = set(ranked)
        ranked.extend(ch for ch in range(16) if ch not in used)

    return ranked[:voices]


def select_monophonic_onsets(
    notes: list[NoteEvent],
    channel: int,
    chord_policy: str,
) -> dict[int, NoteEvent]:
    onsets: dict[int, NoteEvent] = {}

    for note in sorted(notes, key=lambda n: (n.start_tick, n.end_tick, n.note)):
        if note.channel != channel:
            continue

        existing = onsets.get(note.start_tick)
        if existing is None:
            onsets[note.start_tick] = note
            continue

        pick = False
        if chord_policy == "high":
            pick = note.note > existing.note
        elif chord_policy == "low":
            pick = note.note < existing.note
        elif chord_policy == "first":
            pick = False

        if pick:
            onsets[note.start_tick] = note

    return onsets


def emit_basic(
    song: MidiSong,
    input_label: str,
    title: str,
    voices: int,
    voice_channels: list[int],
    chord_policy: str,
    tempo_mode: str,
    bpm_override: int | None,
    volume: int,
    tail_ticks: int,
    max_line_len: int,
) -> str:
    onsets_per_voice: list[dict[int, NoteEvent]] = [
        select_monophonic_onsets(song.notes, channel, chord_policy)
        for channel in voice_channels[:voices]
    ]

    all_ticks: set[int] = set()
    for onsets in onsets_per_voice:
        all_ticks.update(onsets.keys())

    tempo_by_tick = {tempo.tick: tempo.bpm for tempo in song.tempos}
    if 0 not in tempo_by_tick:
        tempo_by_tick[0] = song.tempos[0].bpm

    initial_bpm = bpm_override if bpm_override is not None else tempo_by_tick[min(tempo_by_tick.keys())]
    initial_bpm = max(1, initial_bpm)

    if tempo_mode == "track":
        for tick in tempo_by_tick.keys():
            if tick > 0:
                all_ticks.add(tick)

    if song.end_tick > 0:
        all_ticks.add(song.end_tick + max(0, tail_ticks))

    timeline = sorted(all_ticks)
    if not timeline:
        timeline = [0]

    events: list[list[int]] = []
    prev_tick = 0

    for tick in timeline:
        wait_ticks = max(0, tick - prev_tick)
        tempo = 0
        if tempo_mode == "track" and tick in tempo_by_tick and tick != 0:
            tempo = tempo_by_tick[tick]

        row: list[int] = [wait_ticks, tempo]

        for onsets in onsets_per_voice:
            note = onsets.get(tick)
            if note is None:
                row.extend([0, 0])
            else:
                dur_ticks = max(1, note.end_tick - note.start_tick)
                row.extend([midi_note_to_freq(note.note), dur_ticks])

        events.append(row)
        prev_tick = tick

    # Remove a trailing pure wait event if it does nothing useful.
    if events:
        last = events[-1]
        if last[1] == 0 and all(v == 0 for v in last[2:]) and last[0] == 0:
            events.pop()

    lines: list[str] = []
    line_no = 10

    ts = song.time_signature
    ticks_per_bar = int(round(song.ppqn * 4 * ts.numerator / ts.denominator))
    ticks_per_beat = int(round(song.ppqn * 4 / ts.denominator))

    lines.append(f"{line_no} REM NovaBASIC v1.0 - {title}")
    line_no += 10
    lines.append(f"{line_no} REM SOURCE: {input_label}")
    line_no += 10
    lines.append(
        f"{line_no} REM PPQN={song.ppqn} TIME={ts.numerator}/{ts.denominator} "
        f"BPM0={initial_bpm} CHANNELS="
        + ",".join(str(ch + 1) for ch in voice_channels[:voices])
    )
    line_no += 10
    lines.append(f"{line_no} REM DATA: WTICKS,TEMPO," + ",".join(f"F{v},D{v}" for v in range(voices)))
    line_no += 10
    lines.append(f"{line_no} VOLUME {max(0, min(15, volume))}")
    line_no += 10

    for v in range(voices):
        vc = DEFAULT_VOICES[v]
        lines.append(
            f"{line_no} ENVELOPE {v},{vc.attack},{vc.decay},{vc.sustain},{vc.release}:WAVE {v},{vc.wave}"
        )
        line_no += 10

    lines.append(f"{line_no} PP={song.ppqn}:SCALE=100:BPM={initial_bpm}*SCALE/100")
    line_no += 10
    lines.append(f"{line_no} SC=3600/(BPM*PP)")
    line_no += 10
    lines.append(f"{line_no} RESTORE 2000")
    line_no += 10

    read_vars = ["W", "T"]
    for v in range(voices):
        read_vars.extend([f"F{v}", f"D{v}"])
    loop_line = line_no
    lines.append(f"{line_no} READ " + ",".join(read_vars))
    line_no += 10

    lines.append(f"{line_no} IF W<0 THEN 900")
    line_no += 10
    lines.append(f"{line_no} WF=0")
    line_no += 10
    lines.append(f"{line_no} IF W>0 THEN WF=INT(W*SC+0.5)")
    line_no += 10
    lines.append(f"{line_no} IF WF<1 THEN WF=1")
    line_no += 10
    lines.append(f"{line_no} IF W>0 THEN FOR I=1 TO WF:VSYNC:NEXT I")
    line_no += 10
    lines.append(f"{line_no} IF T>0 THEN BPM=T*SCALE/100:SC=3600/(BPM*PP)")
    line_no += 10

    for v in range(voices):
        skip_line = line_no + 40
        lines.append(f"{line_no} IF F{v}=0 THEN {skip_line}")
        line_no += 10
        lines.append(f"{line_no} L=INT(D{v}*SC+0.5)")
        line_no += 10
        lines.append(f"{line_no} IF L<1 THEN L=1")
        line_no += 10
        lines.append(f"{line_no} SOUND {v},F{v},L")
        line_no += 10

    lines.append(f"{line_no} GOTO {loop_line}")
    line_no += 10
    lines.append(f"{line_no} END")

    data_line = 2000
    lines.append(
        f"{data_line} REM M1 TICKS/BAR={ticks_per_bar} TICKS/BEAT={ticks_per_beat} "
        f"POLICY={chord_policy.upper()}"
    )

    data_line += 10
    prefix = lambda ln: f"{ln} DATA "

    chunk: list[str] = []
    current_measure = 1
    prev_tick = 0

    for event in events:
        tick = prev_tick + event[0]
        prev_tick = tick
        event_csv = ",".join(str(x) for x in event)

        if tick >= current_measure * ticks_per_bar:
            lines.append(f"{data_line} REM M{current_measure + 1}")
            data_line += 10
            current_measure = (tick // ticks_per_bar) + 1

        candidate = ",".join(chunk + [event_csv]) if chunk else event_csv
        if len(prefix(data_line)) + len(candidate) > max_line_len:
            if not chunk:
                raise RuntimeError(
                    f"Event row too wide for max-line-len={max_line_len}: {event_csv}"
                )
            lines.append(prefix(data_line) + ",".join(chunk))
            data_line += 10
            chunk = [event_csv]
        else:
            chunk.append(event_csv)

    if chunk:
        lines.append(prefix(data_line) + ",".join(chunk))
        data_line += 10

    sentinel = [-1, 0] + [0] * (voices * 2)
    lines.append(prefix(data_line) + ",".join(str(x) for x in sentinel))

    for src_line in lines:
        if len(src_line) > max_line_len:
            raise RuntimeError(
                f"Generated BASIC line exceeds max-line-len={max_line_len}: {src_line}"
            )

    return "\n".join(lines) + "\n"


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Transpile MIDI/LilyPond into NovaBASIC v1.0 SOUND program",
    )
    parser.add_argument("input", type=Path, help="Input .mid/.midi or .ly file")
    parser.add_argument("-o", "--output", type=Path, help="Output .bas file")
    parser.add_argument("--title", default="SCORE PLAYER", help="Program title in REM header")
    parser.add_argument("--voices", type=int, default=2, choices=range(1, 5), help="Number of VSC voices (1-4)")
    parser.add_argument(
        "--channels",
        help="Comma-separated MIDI channels (1-16) mapped to voice order, e.g. 2,3",
    )
    parser.add_argument(
        "--chord-policy",
        choices=["high", "low", "first"],
        default="high",
        help="If a voice has a chord onset at one tick, pick high/low/first note",
    )
    parser.add_argument(
        "--tempo-mode",
        choices=["fixed", "track"],
        default="fixed",
        help="fixed=single BPM, track=emit MIDI tempo changes into DATA",
    )
    parser.add_argument("--bpm", type=int, help="Override initial BPM")
    parser.add_argument("--volume", type=int, default=12, help="Master volume 0-15")
    parser.add_argument("--tail-ticks", type=int, default=0, help="Extra trailing wait ticks")
    parser.add_argument(
        "--max-line-len",
        type=int,
        default=68,
        help="Maximum emitted BASIC source line length (default 68, safe for NovaBASIC input buffer)",
    )
    parser.add_argument("--lilypond-bin", default="lilypond", help="LilyPond executable for .ly input")

    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    input_path: Path = args.input
    if not input_path.exists():
        print(f"error: input file not found: {input_path}", file=sys.stderr)
        return 1

    if args.output is None:
        args.output = input_path.with_suffix(".bas")

    try:
        explicit_channels = parse_channel_list(args.channels)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    working_midi = input_path
    cleanup_path: Path | None = None

    try:
        suffix = input_path.suffix.lower()
        if suffix in (".ly", ".ily"):
            working_midi = lilypond_to_midi(input_path, args.lilypond_bin)
            cleanup_path = working_midi.parent
        elif suffix not in (".mid", ".midi"):
            print(
                f"error: unsupported input extension '{suffix}'. Use .mid/.midi/.ly",
                file=sys.stderr,
            )
            return 1

        song = parse_midi(working_midi)

        if not song.notes:
            print("error: no note events found in input", file=sys.stderr)
            return 1

        voice_channels = choose_voice_channels(song.notes, args.voices, explicit_channels)

        output_text = emit_basic(
            song=song,
            input_label=input_path.name,
            title=args.title,
            voices=args.voices,
            voice_channels=voice_channels,
            chord_policy=args.chord_policy,
            tempo_mode=args.tempo_mode,
            bpm_override=args.bpm,
            volume=args.volume,
            tail_ticks=args.tail_ticks,
            max_line_len=max(40, args.max_line_len),
        )

        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output_text, encoding="ascii")

        # Print a concise transcription summary.
        ch_counts: dict[int, int] = defaultdict(int)
        min_note = 127
        max_note = 0
        for note in song.notes:
            ch_counts[note.channel] += 1
            min_note = min(min_note, note.note)
            max_note = max(max_note, note.note)

        print(f"Wrote {args.output}")
        print(
            "Summary: "
            f"notes={len(song.notes)} "
            f"ppqn={song.ppqn} "
            f"bpm0={(args.bpm if args.bpm else song.tempos[0].bpm)} "
            f"time={song.time_signature.numerator}/{song.time_signature.denominator} "
            f"voices={args.voices} channels="
            + ",".join(str(ch + 1) for ch in voice_channels[:args.voices])
        )
        print(
            "Pitch range: "
            f"{midi_note_to_name(min_note)}-{midi_note_to_name(max_note)} "
            f"({midi_note_to_freq(min_note)}-{midi_note_to_freq(max_note)} Hz)"
        )
        print(
            "Top MIDI channels by note count: "
            + ", ".join(f"ch{ch + 1}:{count}" for ch, count in sorted(ch_counts.items(), key=lambda kv: (-kv[1], kv[0]))[:8])
        )

        return 0
    except (MidiParseError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    finally:
        if cleanup_path is not None and cleanup_path.exists():
            for child in cleanup_path.glob("*"):
                try:
                    child.unlink()
                except OSError:
                    pass
            try:
                cleanup_path.rmdir()
            except OSError:
                pass


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
