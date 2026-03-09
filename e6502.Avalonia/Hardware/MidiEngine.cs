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
    /// <summary>
    /// Generate an MML string from a single MIDI channel's events.
    /// When <paramref name="compact"/> is true, uses L (default length) to avoid
    /// repeating durations, and strips per-note velocity changes for smaller output.
    /// </summary>
    public static string GenerateMml(MidiFile midi, int channel, int ppqn,
        bool compact = false)
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
        string currentDefaultLen = ""; // tracks the L (default length) in compact mode

        // In compact mode, find the most common duration to use as initial default
        if (compact && notes.Count > 0)
        {
            var durCounts = new Dictionary<string, int>();
            foreach (var n in notes)
            {
                string d = QuantizeDuration(n.MmlTicks);
                if (!d.Contains('&')) // only simple durations can be defaults
                {
                    durCounts.TryGetValue(d, out int c);
                    durCounts[d] = c + 1;
                }
            }
            if (durCounts.Count > 0)
            {
                currentDefaultLen = durCounts.OrderByDescending(kv => kv.Value).First().Key;
                sb.Append('L');
                sb.Append(currentDefaultLen);
            }
        }

        foreach (var note in notes)
        {
            string dur = QuantizeDuration(note.MmlTicks);

            if (note.IsRest)
            {
                sb.Append('R');
                if (!compact || dur != currentDefaultLen)
                    sb.Append(dur);
                continue;
            }

            if (!compact)
            {
                int vol = VelocityToVolume(note.Velocity);
                if (vol != currentVelocity)
                {
                    sb.Append('V');
                    sb.Append(vol);
                    currentVelocity = vol;
                }
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

            if (compact)
            {
                // Only emit duration if it differs from current default
                if (dur != currentDefaultLen)
                {
                    // If this duration is simple (no tie) and will repeat, switch default
                    if (!dur.Contains('&'))
                    {
                        // Look ahead: count how many upcoming notes share this duration
                        // (lightweight: just emit the duration explicitly for tied notes)
                        sb.Append(dur);
                    }
                    else
                    {
                        sb.Append(dur);
                    }
                }
            }
            else
            {
                sb.Append(dur);
            }
        }

        return sb.ToString();
    }

    /// <summary>
    /// Generate a complete NovaBASIC .bas program from a MIDI file.
    /// When <paramref name="wts"/> is true, routes up to 8 channels to WTS voices (7-14)
    /// with GM instrument selection (@I/@D), and overflow channels to SID voices (1-6).
    /// </summary>
    public static string GenerateBasProgram(MidiFile midi,
        string title = "", string subtitle = "",
        int maxLineLen = 200, int maxVoices = 6,
        Dictionary<int, int>? explicitMapping = null,
        bool wts = false, bool compact = false)
    {
        int ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        var analysis = AnalyzeChannels(midi);

        if (wts && maxVoices <= 6)
            maxVoices = 14;

        var selectedChannels = SelectChannels(midi, maxVoices, explicitMapping);

        // Build voice assignment: map each selected channel to a BASIC voice number (1-14).
        // WTS mode: first min(8, count) → voices 7-14 (WTS), remainder → voices 1-6 (SID).
        // SID-only mode: voices 1-6 sequentially.
        int[] voiceNumbers;
        if (wts)
        {
            int wtsCount = Math.Min(selectedChannels.Length, 8);
            int sidCount = selectedChannels.Length - wtsCount;
            voiceNumbers = new int[selectedChannels.Length];
            for (int i = 0; i < wtsCount; i++)
                voiceNumbers[i] = 7 + i; // WTS voices 7-14
            for (int i = 0; i < sidCount; i++)
                voiceNumbers[wtsCount + i] = 1 + i; // SID voices 1-6
        }
        else
        {
            voiceNumbers = new int[selectedChannels.Length];
            for (int i = 0; i < selectedChannels.Length; i++)
                voiceNumbers[i] = i + 1;
        }

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

        // SID instrument definitions (only for SID voices 1-6)
        var usedBuckets = new Dictionary<int, int>(); // bucket index → instrument slot
        int nextSlot = 1;

        for (int v = 0; v < selectedChannels.Length; v++)
        {
            if (voiceNumbers[v] > 6) continue; // WTS voice — no INSTRUMENT needed

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
            int voice = voiceNumbers[v];

            string mml = GenerateMml(midi, ch, ppqn, compact);

            if (voice <= 6)
            {
                // SID voice: prepend I{slot}
                var bucket = GetInstrumentBucket(info.GmProgram, info.IsDrums);
                int bucketIdx = Array.IndexOf(Buckets, bucket);
                int instSlot = usedBuckets[bucketIdx];
                mml = $"I{instSlot}{mml}";
            }
            else
            {
                // WTS voice: prepend @I{gmProgram} or @D{gmProgram}
                if (info.IsDrums)
                    mml = $"@D{info.GmProgram}{mml}";
                else
                    mml = $"@I{info.GmProgram}{mml}";
            }

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

    /// <summary>
    /// Result of routing MIDI channels to engine voices.
    /// </summary>
    public sealed class RoutingResult
    {
        public int[] VoiceToChannel = Array.Empty<int>();
        public int[] InstrumentSlots = Array.Empty<int>();
        public bool SidOnly;
    }

    /// <summary>
    /// Route MIDI channels to engine voices (SID 0-5, WTS 6-13).
    /// Single code path used by both FileIoController and EmulatorTcpServer.
    /// </summary>
    /// <param name="analysis">Channel analysis from AnalyzeChannels.</param>
    /// <param name="rawChannels">Selected channels from SelectChannels.</param>
    /// <param name="sidOnly">True to route only to SID voices 0-5.</param>
    /// <param name="defineInstrument">Callback to register SID instrument (slot, bucket).</param>
    /// <param name="explicitVoiceMapping">Optional voice→channel mapping. When provided,
    /// listed voices are assigned first; remaining channels are auto-routed to remaining voices.</param>
    public static RoutingResult RouteVoices(
        ChannelInfo[] analysis, int[] rawChannels, bool sidOnly,
        Action<int, InstrumentBucket> defineInstrument,
        Dictionary<int, int>? explicitVoiceMapping = null)
    {
        var result = new RoutingResult { SidOnly = sidOnly };

        bool hasExplicit = explicitVoiceMapping != null && explicitVoiceMapping.Count > 0;

        if (hasExplicit && !sidOnly)
        {
            // --- Explicit mapping (non-SID) ---
            int totalVoices = 14;
            result.VoiceToChannel = new int[totalVoices];
            result.InstrumentSlots = new int[totalVoices];
            for (int i = 0; i < totalVoices; i++)
                result.VoiceToChannel[i] = -1;

            // Step 1: Apply explicit assignments
            var usedChannels = new HashSet<int>();
            foreach (var (voice, channel) in explicitVoiceMapping!)
            {
                if (voice >= 0 && voice < totalVoices)
                {
                    result.VoiceToChannel[voice] = channel;
                    usedChannels.Add(channel);
                }
            }

            // Step 2: Auto-route remaining channels to remaining voices (WTS first, then SID)
            var remainingChannels = new List<int>();
            foreach (int ch in rawChannels)
                if (!usedChannels.Contains(ch))
                    remainingChannels.Add(ch);

            int autoIdx = 0;
            // WTS voices 6-13 first
            for (int v = 6; v < totalVoices && autoIdx < remainingChannels.Count; v++)
                if (result.VoiceToChannel[v] == -1)
                    result.VoiceToChannel[v] = remainingChannels[autoIdx++];
            // SID voices 0-5 next
            for (int v = 0; v < 6 && autoIdx < remainingChannels.Count; v++)
                if (result.VoiceToChannel[v] == -1)
                    result.VoiceToChannel[v] = remainingChannels[autoIdx++];

            // Step 3: Assign instruments
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;
            for (int v = 0; v < totalVoices; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;

                if (v >= 6)
                {
                    int bank = analysis[ch].IsDrums ? 128 : 0;
                    result.InstrumentSlots[v] = (bank << 8) | (analysis[ch].GmProgram & 0x7F);
                }
                else
                {
                    var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                    int bucketIdx = Array.IndexOf(Buckets, bucket);
                    if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                    {
                        slot = nextSlot++;
                        registeredBuckets[bucketIdx] = slot;
                        defineInstrument(slot, bucket);
                    }
                    result.InstrumentSlots[v] = slot;
                }
            }
        }
        else if (hasExplicit && sidOnly)
        {
            // --- Explicit mapping (SID only) ---
            int totalVoices = 6;
            result.VoiceToChannel = new int[totalVoices];
            result.InstrumentSlots = new int[totalVoices];
            for (int i = 0; i < totalVoices; i++)
                result.VoiceToChannel[i] = -1;

            var usedChannels = new HashSet<int>();
            foreach (var (voice, channel) in explicitVoiceMapping!)
            {
                if (voice >= 0 && voice < totalVoices)
                {
                    result.VoiceToChannel[voice] = channel;
                    usedChannels.Add(channel);
                }
            }

            // Auto-route remaining channels
            var remainingChannels = new List<int>();
            foreach (int ch in rawChannels)
                if (!usedChannels.Contains(ch))
                    remainingChannels.Add(ch);

            int autoIdx = 0;
            for (int v = 0; v < totalVoices && autoIdx < remainingChannels.Count; v++)
                if (result.VoiceToChannel[v] == -1)
                    result.VoiceToChannel[v] = remainingChannels[autoIdx++];

            // SID instrument buckets
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;
            for (int v = 0; v < totalVoices; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;

                var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                int bucketIdx = Array.IndexOf(Buckets, bucket);
                if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                {
                    slot = nextSlot++;
                    registeredBuckets[bucketIdx] = slot;
                    defineInstrument(slot, bucket);
                }
                result.InstrumentSlots[v] = slot;
            }
        }
        else if (sidOnly)
        {
            result.VoiceToChannel = rawChannels;
            result.InstrumentSlots = new int[rawChannels.Length];
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;

            for (int v = 0; v < rawChannels.Length; v++)
            {
                int ch = rawChannels[v];
                var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                int bucketIdx = Array.IndexOf(Buckets, bucket);

                if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                {
                    slot = nextSlot++;
                    registeredBuckets[bucketIdx] = slot;
                    defineInstrument(slot, bucket);
                }
                result.InstrumentSlots[v] = slot;
            }
        }
        else
        {
            int wtsCount = Math.Min(rawChannels.Length, 8);
            int sidCount = rawChannels.Length - wtsCount;
            int totalVoices = 6 + wtsCount;

            result.VoiceToChannel = new int[totalVoices];
            result.InstrumentSlots = new int[totalVoices];

            // Pad unused SID voices with -1
            for (int i = sidCount; i < 6; i++)
                result.VoiceToChannel[i] = -1;

            // SID overflow (channels[8..]) → voices 0..sidCount-1
            for (int i = 0; i < sidCount; i++)
                result.VoiceToChannel[i] = rawChannels[wtsCount + i];

            // WTS primary (channels[0..7]) → voices 6..6+wtsCount-1
            for (int i = 0; i < wtsCount; i++)
                result.VoiceToChannel[6 + i] = rawChannels[i];

            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;

            for (int v = 0; v < totalVoices; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;

                bool isWtsVoice = v >= 6;
                if (isWtsVoice)
                {
                    // WTS: encode bank and program for FindByProgram lookup
                    // Drums (ch 9) use bank 128, melodic uses bank 0
                    int bank = analysis[ch].IsDrums ? 128 : 0;
                    result.InstrumentSlots[v] = (bank << 8) | (analysis[ch].GmProgram & 0x7F);
                }
                else
                {
                    // SID: use instrument bucket mapping
                    var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                    int bucketIdx = Array.IndexOf(Buckets, bucket);
                    if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                    {
                        slot = nextSlot++;
                        registeredBuckets[bucketIdx] = slot;
                        defineInstrument(slot, bucket);
                    }
                    result.InstrumentSlots[v] = slot;
                }
            }
        }

        return result;
    }

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
