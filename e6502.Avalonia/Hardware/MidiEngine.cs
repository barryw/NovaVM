using System.Collections.Generic;
using System.Linq;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Shared MIDI-to-SID core: channel analysis, selection, instrument mapping,
/// duration quantization, and MML generation. Used by both MIDPLAY (real-time)
/// and mid2bas (offline converter).
/// </summary>
public static class MidiEngine
{
    public sealed class ChannelInfo
    {
        public int Channel;
        public int NoteCount;
        public int GmProgram;
        public int MaxPolyphony;
        public bool IsDrums => Channel == 9;
    }

    /// <summary>Analyze all 16 MIDI channels, counting notes, max polyphony, and extracting GM programs.</summary>
    public static ChannelInfo[] AnalyzeChannels(MidiFile midi)
    {
        var channels = new ChannelInfo[16];
        for (int i = 0; i < 16; i++)
            channels[i] = new ChannelInfo { Channel = i };

        // Track active notes per channel to compute max polyphony
        var activeNotes = new HashSet<int>[16];
        for (int i = 0; i < 16; i++)
            activeNotes[i] = new HashSet<int>();

        // Collect all note events sorted by time
        var allEvents = new List<(long time, int channel, int note, bool isOn)>();
        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                switch (timedEvent.Event)
                {
                    case NoteOnEvent noteOn when noteOn.Velocity > 0:
                        channels[noteOn.Channel]!.NoteCount++;
                        allEvents.Add((timedEvent.Time, noteOn.Channel, noteOn.NoteNumber, true));
                        break;
                    case NoteOnEvent noteOn:
                        allEvents.Add((timedEvent.Time, noteOn.Channel, noteOn.NoteNumber, false));
                        break;
                    case NoteOffEvent noteOff:
                        allEvents.Add((timedEvent.Time, noteOff.Channel, noteOff.NoteNumber, false));
                        break;
                    case ProgramChangeEvent pc:
                        channels[pc.Channel]!.GmProgram = pc.ProgramNumber;
                        break;
                }
            }
        }

        allEvents.Sort((a, b) => a.time.CompareTo(b.time));
        foreach (var (_, ch, note, isOn) in allEvents)
        {
            if (isOn) activeNotes[ch].Add(note);
            else activeNotes[ch].Remove(note);
            if (activeNotes[ch].Count > channels[ch].MaxPolyphony)
                channels[ch].MaxPolyphony = activeNotes[ch].Count;
        }

        return channels;
    }

    public sealed class InstrumentBucket
    {
        public byte Waveform;
        public byte Attack;
        public byte Decay;
        public byte Sustain;
        public byte Release;
        public string Name = "";
    }

    private static readonly InstrumentBucket[] Buckets =
    {
        new() { Waveform = 0x40, Attack = 0, Decay = 4, Sustain = 7, Release = 6, Name = "Pulse Piano" },
        new() { Waveform = 0x40, Attack = 0, Decay = 9, Sustain = 0, Release = 6, Name = "Pulse Lead" },
        new() { Waveform = 0x20, Attack = 4, Decay = 6, Sustain = 10, Release = 8, Name = "Saw Strings" },
        new() { Waveform = 0x20, Attack = 2, Decay = 4, Sustain = 10, Release = 4, Name = "Saw Brass" },
        new() { Waveform = 0x10, Attack = 2, Decay = 6, Sustain = 12, Release = 8, Name = "Tri Flute" },
        new() { Waveform = 0x10, Attack = 0, Decay = 4, Sustain = 10, Release = 4, Name = "Tri Bass" },
        new() { Waveform = 0x80, Attack = 0, Decay = 3, Sustain = 0, Release = 2, Name = "Noise Drums" },
        new() { Waveform = 0x20, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Saw Default" },
        new() { Waveform = 0x20, Attack = 3, Decay = 4, Sustain = 12, Release = 5, Name = "Saw Tuba" },
        new() { Waveform = 0x40, Attack = 2, Decay = 5, Sustain = 10, Release = 6, Name = "Pulse Reed" },
        new() { Waveform = 0x40, Attack = 1, Decay = 4, Sustain = 9, Release = 4, Name = "Pulse Sax" },
        new() { Waveform = 0x10, Attack = 2, Decay = 5, Sustain = 10, Release = 5, Name = "Tri Bassoon" },
    };

    private const int BucketPulsePiano = 0;
    private const int BucketPulseLead = 1;
    private const int BucketSawStrings = 2;
    private const int BucketSawBrass = 3;
    private const int BucketTriFlute = 4;
    private const int BucketTriBass = 5;
    private const int BucketNoiseDrums = 6;
    private const int BucketSawDefault = 7;
    private const int BucketSawTuba = 8;
    private const int BucketPulseReed = 9;
    private const int BucketPulseSax = 10;
    private const int BucketTriBassoon = 11;

    public static InstrumentBucket GetInstrumentBucket(int gmProgram, bool isDrums)
    {
        if (isDrums) return Buckets[BucketNoiseDrums];

        return gmProgram switch
        {
            >= 0 and <= 7   => Buckets[BucketPulsePiano],   // Piano family
            >= 8 and <= 15  => Buckets[BucketPulsePiano],   // Chromatic percussion
            >= 16 and <= 23 => Buckets[BucketPulsePiano],   // Organ
            >= 24 and <= 31 => Buckets[BucketPulsePiano],   // Guitar (acoustic)
            >= 32 and <= 39 => Buckets[BucketTriBass],      // Bass
            >= 40 and <= 55 => Buckets[BucketSawStrings],   // Strings + Ensemble
            56 or 57 or 59  => Buckets[BucketSawBrass],     // Trumpet, Trombone, Muted Trumpet
            58              => Buckets[BucketSawTuba],       // Tuba
            60 or 61        => Buckets[BucketSawBrass],     // French Horn, Brass Section
            >= 62 and <= 63 => Buckets[BucketSawBrass],     // Synth Brass
            >= 64 and <= 67 => Buckets[BucketPulseSax],     // Soprano/Alto/Tenor/Baritone Sax
            68 or 69        => Buckets[BucketPulseReed],    // Oboe, English Horn
            70              => Buckets[BucketTriBassoon],   // Bassoon
            71              => Buckets[BucketPulseReed],    // Clarinet
            >= 72 and <= 79 => Buckets[BucketTriFlute],     // Pipe
            >= 80 and <= 95 => Buckets[BucketPulseLead],    // Synth lead + pad
            _               => Buckets[BucketSawDefault],   // Everything else
        };
    }

    public static int VelocityToVolume(int velocity) =>
        Math.Clamp((int)Math.Round(velocity * 15.0 / 127), 0, 15);

    private const int MmlTicksPerQuarter = 96;

    // Valid MML durations: (denominator, ticks, dotted ticks)
    private static readonly (int denom, int ticks, int dottedTicks)[] MmlDurations =
    {
        (1, 384, 576),
        (2, 192, 288),
        (4, 96, 144),
        (8, 48, 72),
        (16, 24, 36),
        (32, 12, 18),
    };

    public static int MidiTicksToMmlTicks(long midiTicks, int ppqn) =>
        (int)Math.Round((double)midiTicks * MmlTicksPerQuarter / ppqn);

    /// <summary>
    /// Convert MML ticks to a duration string using note values and ties.
    /// E.g., 96 → "4", 144 → "4.", 120 → "4&16"
    /// </summary>
    public static string QuantizeDuration(int mmlTicks)
    {
        if (mmlTicks <= 0) return "32";

        var parts = new List<string>();
        int remaining = mmlTicks;

        while (remaining >= 6) // below 6 ticks = discard
        {
            string? best = null;
            int bestTicks = 0;

            foreach (var (denom, ticks, dottedTicks) in MmlDurations)
            {
                if (dottedTicks <= remaining && dottedTicks > bestTicks)
                {
                    best = $"{denom}.";
                    bestTicks = dottedTicks;
                }
                if (ticks <= remaining && ticks > bestTicks)
                {
                    best = $"{denom}";
                    bestTicks = ticks;
                }
            }

            if (best is null) break;
            parts.Add(best);
            remaining -= bestTicks;
        }

        return parts.Count > 0 ? string.Join("&", parts) : "32";
    }

    /// <summary>
    /// Select up to maxVoices channels, ranked by note count.
    /// If explicitMapping is provided (voice 1-based -> channel 0-based), those are used first.
    /// </summary>
    /// <summary>
    /// Select channels and allocate voices. A polyphonic channel gets multiple voices.
    /// Returns an array where index = voice, value = MIDI channel.
    /// A channel may appear multiple times (e.g., [0, 0, 0] = 3 voices for channel 0).
    /// </summary>
    public static int[] SelectChannels(MidiFile midi, int maxVoices = 6,
        Dictionary<int, int>? explicitMapping = null)
    {
        var analysis = AnalyzeChannels(midi);

        if (explicitMapping is { Count: > 0 })
        {
            return explicitMapping
                .OrderBy(kv => kv.Key)
                .Select(kv => kv.Value)
                .Take(maxVoices)
                .ToArray();
        }

        // Rank channels by note count, allocate voices proportional to polyphony
        var activeChannels = analysis
            .Where(c => c.NoteCount > 0)
            .OrderByDescending(c => c.NoteCount)
            .ToList();

        if (activeChannels.Count == 0)
            return Array.Empty<int>();

        var result = new List<int>();

        // First pass: give each channel at least 1 voice
        foreach (var ch in activeChannels)
        {
            if (result.Count >= maxVoices) break;
            result.Add(ch.Channel);
        }

        // Second pass: distribute remaining voices to polyphonic channels
        int remaining = maxVoices - result.Count;
        foreach (var ch in activeChannels)
        {
            if (remaining <= 0) break;
            int extraVoices = Math.Min(remaining, Math.Max(0, ch.MaxPolyphony - 1));
            for (int i = 0; i < extraVoices; i++)
                result.Add(ch.Channel);
            remaining -= extraVoices;
        }

        return result.ToArray();
    }

    private static readonly string[] NoteNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    /// <summary>
    /// Generate an MML string from a single MIDI channel's events.
    /// Handles notes, rests, velocity (V), octave tracking (O/>/&lt;), tempo (T), and
    /// top-note-wins polyphony reduction.
    /// </summary>
    public static string GenerateMml(MidiFile midi, int channel, int ppqn)
    {
        var noteEvents = new List<(long time, int midi, int velocity, bool isOn)>();
        long initialTempo = 500000; // default 120 BPM

        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                switch (timedEvent.Event)
                {
                    case NoteOnEvent noteOn when noteOn.Channel == channel && noteOn.Velocity > 0:
                        noteEvents.Add((timedEvent.Time, noteOn.NoteNumber, noteOn.Velocity, true));
                        break;
                    case NoteOnEvent noteOn when noteOn.Channel == channel:
                        noteEvents.Add((timedEvent.Time, noteOn.NoteNumber, 0, false));
                        break;
                    case NoteOffEvent noteOff when noteOff.Channel == channel:
                        noteEvents.Add((timedEvent.Time, noteOff.NoteNumber, 0, false));
                        break;
                    case SetTempoEvent tempo:
                        initialTempo = tempo.MicrosecondsPerQuarterNote;
                        break;
                }
            }
        }

        noteEvents.Sort((a, b) => a.time.CompareTo(b.time));

        var notes = ReduceToMonophonic(noteEvents, ppqn);

        var sb = new System.Text.StringBuilder();
        int bpm = (int)Math.Round(60_000_000.0 / initialTempo);
        sb.Append($"T{bpm}");

        int currentOctave = -1;
        int currentVelocity = -1;

        foreach (var note in notes)
        {
            if (note.IsRest)
            {
                sb.Append('R');
                sb.Append(QuantizeDuration(note.MmlTicks));
                continue;
            }

            int vol = VelocityToVolume(note.Velocity);
            if (vol != currentVelocity)
            {
                sb.Append('V');
                sb.Append(vol);
                currentVelocity = vol;
            }

            int noteOctave = (note.Midi / 12) - 1;
            if (noteOctave != currentOctave)
            {
                if (currentOctave >= 0 && noteOctave == currentOctave + 1)
                    sb.Append('>');
                else if (currentOctave >= 0 && noteOctave == currentOctave - 1)
                    sb.Append('<');
                else
                {
                    sb.Append('O');
                    sb.Append(noteOctave);
                }
                currentOctave = noteOctave;
            }

            int semitone = note.Midi % 12;
            sb.Append(NoteNames[semitone]);
            sb.Append(QuantizeDuration(note.MmlTicks));
        }

        return sb.ToString();
    }

    /// <summary>
    /// Generate a complete NovaBASIC .bas program from a MIDI file.
    /// </summary>
    public static string GenerateBasProgram(MidiFile midi,
        string title = "", string subtitle = "",
        int maxLineLen = 200, int maxVoices = 6,
        Dictionary<int, int>? explicitMapping = null)
    {
        int ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        var analysis = AnalyzeChannels(midi);
        var selectedChannels = SelectChannels(midi, maxVoices, explicitMapping);

        var lines = new List<string>();
        int lineNum = 10;

        // Header
        lines.Add($"{lineNum} REM ================================"); lineNum += 10;
        if (!string.IsNullOrEmpty(title))
        {
            lines.Add($"{lineNum} REM  {title}"); lineNum += 10;
        }
        if (!string.IsNullOrEmpty(subtitle))
        {
            lines.Add($"{lineNum} REM  {subtitle}"); lineNum += 10;
        }
        lines.Add($"{lineNum} REM ================================"); lineNum += 10;

        // Instrument definitions
        var usedBuckets = new Dictionary<int, int>(); // bucket index → instrument slot
        int nextSlot = 1;

        for (int v = 0; v < selectedChannels.Length; v++)
        {
            int ch = selectedChannels[v];
            var info = analysis[ch];
            var bucket = GetInstrumentBucket(info.GmProgram, info.IsDrums);
            int bucketIdx = Array.IndexOf(Buckets, bucket);

            if (!usedBuckets.ContainsKey(bucketIdx))
            {
                usedBuckets[bucketIdx] = nextSlot;
                lines.Add($"{lineNum} INSTRUMENT {nextSlot},{bucket.Waveform},{bucket.Attack},{bucket.Decay},{bucket.Sustain},{bucket.Release}:REM {bucket.Name}");
                lineNum += 10;
                nextSlot++;
            }
        }

        // MML sequences per voice
        for (int v = 0; v < selectedChannels.Length; v++)
        {
            int ch = selectedChannels[v];
            var info = analysis[ch];
            var bucket = GetInstrumentBucket(info.GmProgram, info.IsDrums);
            int bucketIdx = Array.IndexOf(Buckets, bucket);
            int instSlot = usedBuckets[bucketIdx];

            string mml = GenerateMml(midi, ch, ppqn);
            // Prepend instrument selection
            mml = $"I{instSlot}{mml}";

            int voice = v + 1;
            var mmlLines = SplitMml(mml, maxLineLen, voice);
            foreach (var ml in mmlLines)
            {
                lines.Add($"{lineNum} {ml}");
                lineNum += 10;
            }
        }

        // Playback
        lines.Add($"{lineNum} VOLUME 15"); lineNum += 10;
        lines.Add($"{lineNum} MUSIC PLAY"); lineNum += 10;

        return string.Join("\n", lines) + "\n";
    }

    /// <summary>Split a long MML string into multiple MUSIC voice,"..." lines.</summary>
    private static List<string> SplitMml(string mml, int maxLineLen, int voice)
    {
        var result = new List<string>();
        string prefix = $"MUSIC {voice},\"";
        int maxMml = maxLineLen - prefix.Length - 1; // -1 for closing quote

        int pos = 0;
        while (pos < mml.Length)
        {
            int len = Math.Min(maxMml, mml.Length - pos);
            result.Add($"{prefix}{mml.Substring(pos, len)}\"");
            pos += len;
        }

        return result;
    }

    public static InstrumentBucket[] GetAllBuckets() => Buckets;

    private sealed class MonoNote
    {
        public bool IsRest;
        public int Midi;
        public int Velocity;
        public int MmlTicks;
    }

    private static List<MonoNote> ReduceToMonophonic(
        List<(long time, int midi, int velocity, bool isOn)> events, int ppqn)
    {
        var result = new List<MonoNote>();
        var activeNotes = new SortedDictionary<int, int>(); // midi pitch → velocity
        long cursor = 0;

        int i = 0;
        while (i < events.Count)
        {
            long eventTime = events[i].time;

            // Emit rest if there is a gap before this event
            if (eventTime > cursor)
            {
                int restTicks = MidiTicksToMmlTicks(eventTime - cursor, ppqn);
                if (restTicks > 0)
                    result.Add(new MonoNote { IsRest = true, MmlTicks = restTicks });
                cursor = eventTime;
            }

            // Process all events at the same absolute time
            while (i < events.Count && events[i].time == eventTime)
            {
                var e = events[i];
                if (e.isOn)
                    activeNotes[e.midi] = e.velocity;
                else
                    activeNotes.Remove(e.midi);
                i++;
            }

            // Determine next event time to know duration of current state
            long nextTime = i < events.Count ? events[i].time : eventTime;
            int mmlTicks = MidiTicksToMmlTicks(nextTime - eventTime, ppqn);

            if (mmlTicks <= 0)
                continue;

            if (activeNotes.Count > 0)
            {
                int topMidi = activeNotes.Keys.Last();
                int topVel = activeNotes[topMidi];
                result.Add(new MonoNote { Midi = topMidi, Velocity = topVel, MmlTicks = mmlTicks });
            }
            else
            {
                result.Add(new MonoNote { IsRest = true, MmlTicks = mmlTicks });
            }

            cursor = nextTime;
        }

        return result;
    }
}
