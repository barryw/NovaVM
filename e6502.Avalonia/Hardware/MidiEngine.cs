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
