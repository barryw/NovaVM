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
        public bool IsDrums => Channel == 9;
    }

    /// <summary>Analyze all 16 MIDI channels, counting notes and extracting GM programs.</summary>
    public static ChannelInfo[] AnalyzeChannels(MidiFile midi)
    {
        var channels = new ChannelInfo[16];
        for (int i = 0; i < 16; i++)
            channels[i] = new ChannelInfo { Channel = i };

        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                if (timedEvent.Event is NoteOnEvent noteOn && noteOn.Velocity > 0)
                    channels[noteOn.Channel]!.NoteCount++;
                else if (timedEvent.Event is ProgramChangeEvent pc)
                    channels[pc.Channel]!.GmProgram = pc.ProgramNumber;
            }
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
        new() { Waveform = 0x20, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Saw Brass" },
        new() { Waveform = 0x10, Attack = 2, Decay = 6, Sustain = 12, Release = 8, Name = "Tri Flute" },
        new() { Waveform = 0x10, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Tri Bass" },
        new() { Waveform = 0x80, Attack = 0, Decay = 3, Sustain = 0, Release = 2, Name = "Noise Drums" },
        new() { Waveform = 0x20, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Saw Default" },
    };

    private const int BucketPulsePiano = 0;
    private const int BucketPulseLead = 1;
    private const int BucketSawStrings = 2;
    private const int BucketSawBrass = 3;
    private const int BucketTriFlute = 4;
    private const int BucketTriBass = 5;
    private const int BucketNoiseDrums = 6;
    private const int BucketSawDefault = 7;

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
            >= 56 and <= 71 => Buckets[BucketSawBrass],     // Brass + Reed
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

        return analysis
            .Where(c => c.NoteCount > 0)
            .OrderByDescending(c => c.NoteCount)
            .Take(maxVoices)
            .Select(c => c.Channel)
            .ToArray();
    }
}
