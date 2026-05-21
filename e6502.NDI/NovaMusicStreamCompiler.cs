using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

internal static class NovaMusicStreamCompiler
{
    private const uint NmsMagic = 0x32534D4E; // "NMS2"
    private const ushort NmsVersion = 2;
    private const ushort NmsHeaderSize = 32;
    private const ushort NmsEventRecordSize = 10;
    private const int WtsSampleRate = 48000;
    private const int WtsVoiceCount = 8;

    private const byte EventNoteOff = 0;
    private const byte EventNoteOn = 1;

    public sealed record Result(byte[] Data, int EventCount, uint TotalFrames);

    public static Result Compile(string midiPath)
    {
        MidiFile midi = MidiFile.Read(midiPath);
        if (midi.TimeDivision is not TicksPerQuarterNoteTimeDivision ppqnDivision)
            throw new InvalidOperationException("SMPTE MIDI timing is not supported.");

        int ppqn = ppqnDivision.TicksPerQuarterNote;
        List<RawEvent> raw = CollectRawEvents(midi);
        ChannelInfo[] channels = AnalyzeChannels(raw);
        int[] voiceChannels = SelectChannels(channels, WtsVoiceCount);
        if (voiceChannels.Length == 0)
            throw new InvalidOperationException("MIDI file contains no routed notes.");

        List<TimelineEvent> timeline = BuildTimeline(raw, voiceChannels);
        if (timeline.Count == 0)
            throw new InvalidOperationException("MIDI file contains no playable events.");

        List<byte> events = new();
        double framesPerTick = WtsSampleRate * 60.0 / (ppqn * 120.0);
        double frameAccum = 0.0;
        long tickAnchor = 0;
        uint totalFrames = 0;

        foreach (TimelineEvent ev in timeline)
        {
            if (ev.Tick > tickAnchor)
            {
                frameAccum += (ev.Tick - tickAnchor) * framesPerTick;
                tickAnchor = ev.Tick;
            }

            uint frame = frameAccum > uint.MaxValue
                ? uint.MaxValue
                : (uint)Math.Round(frameAccum);
            totalFrames = Math.Max(totalFrames, frame);

            if (ev.Kind == EventKind.Tempo)
            {
                if (ev.Bpm > 0)
                    framesPerTick = WtsSampleRate * 60.0 / (ppqn * ev.Bpm);
                continue;
            }

            AddSongRecord(events, frame, ev);
        }

        using MemoryStream ms = new();
        using BinaryWriter writer = new(ms);
        writer.Write(NmsMagic);
        writer.Write(NmsVersion);
        writer.Write(NmsHeaderSize);
        writer.Write(NmsEventRecordSize);
        writer.Write((ushort)WtsSampleRate);
        writer.Write((uint)NmsHeaderSize);
        writer.Write((uint)events.Count);
        writer.Write((uint)(events.Count / NmsEventRecordSize));
        writer.Write(totalFrames);
        writer.Write(0u);
        writer.Write(events.ToArray());
        return new Result(ms.ToArray(), events.Count / NmsEventRecordSize, totalFrames);
    }

    private static void AddSongRecord(List<byte> events, uint frame, TimelineEvent ev)
    {
        events.Add((byte)(frame & 0xFF));
        events.Add((byte)((frame >> 8) & 0xFF));
        events.Add((byte)((frame >> 16) & 0xFF));
        events.Add((byte)((frame >> 24) & 0xFF));
        events.Add(ev.Kind == EventKind.NoteOn ? EventNoteOn : EventNoteOff);
        events.Add((byte)ev.Voice);
        events.Add((byte)ev.Channel);
        events.Add((byte)ev.Note);
        events.Add((byte)ev.Velocity);
        events.Add((byte)ev.Program);
    }

    private static List<RawEvent> CollectRawEvents(MidiFile midi)
    {
        List<RawEvent> events = new();
        int order = 0;
        foreach (TrackChunk track in midi.GetTrackChunks())
        {
            foreach (TimedEvent timed in track.GetTimedEvents())
            {
                switch (timed.Event)
                {
                    case SetTempoEvent tempo:
                        events.Add(new RawEvent(timed.Time, RawKind.Tempo, 0, 0, 0, 0,
                            (int)(60_000_000.0 / tempo.MicrosecondsPerQuarterNote), order++));
                        break;
                    case ProgramChangeEvent pc:
                        events.Add(new RawEvent(timed.Time, RawKind.ProgramChange, pc.Channel, 0, 0,
                            pc.ProgramNumber, 0, order++));
                        break;
                    case NoteOnEvent noteOn when noteOn.Velocity > 0:
                        events.Add(new RawEvent(timed.Time, RawKind.NoteOn, noteOn.Channel,
                            noteOn.NoteNumber, noteOn.Velocity, 0, 0, order++));
                        break;
                    case NoteOnEvent noteOn:
                        events.Add(new RawEvent(timed.Time, RawKind.NoteOff, noteOn.Channel,
                            noteOn.NoteNumber, 0, 0, 0, order++));
                        break;
                    case NoteOffEvent noteOff:
                        events.Add(new RawEvent(timed.Time, RawKind.NoteOff, noteOff.Channel,
                            noteOff.NoteNumber, 0, 0, 0, order++));
                        break;
                }
            }
        }

        events.Sort((a, b) =>
        {
            int c = a.Tick.CompareTo(b.Tick);
            if (c != 0)
                return c;
            c = Priority(a.Kind).CompareTo(Priority(b.Kind));
            return c != 0 ? c : a.Order.CompareTo(b.Order);
        });
        return events;
    }

    private static ChannelInfo[] AnalyzeChannels(List<RawEvent> raw)
    {
        ChannelInfo[] channels = Enumerable.Range(0, 16).Select(i => new ChannelInfo((byte)i)).ToArray();
        bool[,] active = new bool[16, 128];
        foreach (RawEvent ev in raw)
        {
            if (ev.Channel >= 16)
                continue;
            switch (ev.Kind)
            {
                case RawKind.NoteOn:
                    channels[ev.Channel].NoteCount++;
                    active[ev.Channel, ev.Note] = true;
                    int poly = 0;
                    for (int n = 0; n < 128; n++)
                    {
                        if (active[ev.Channel, n])
                            poly++;
                    }
                    channels[ev.Channel].MaxPolyphony = Math.Max(channels[ev.Channel].MaxPolyphony, poly);
                    break;
                case RawKind.NoteOff:
                    active[ev.Channel, ev.Note] = false;
                    break;
                case RawKind.ProgramChange:
                    channels[ev.Channel].Program = ev.Program;
                    break;
            }
        }
        return channels;
    }

    private static int[] SelectChannels(ChannelInfo[] channels, int maxVoices)
    {
        List<int> active = channels
            .Where(c => c.NoteCount > 0)
            .OrderByDescending(c => c.NoteCount)
            .ThenBy(c => c.Channel)
            .Select(c => (int)c.Channel)
            .ToList();

        List<int> result = active.Take(maxVoices).ToList();
        int remaining = maxVoices - result.Count;
        foreach (int ch in active)
        {
            if (remaining <= 0)
                break;
            int extra = Math.Min(remaining, Math.Max(0, channels[ch].MaxPolyphony - 1));
            for (int i = 0; i < extra; i++)
                result.Add(ch);
            remaining -= extra;
        }
        return result.ToArray();
    }

    private static List<TimelineEvent> BuildTimeline(List<RawEvent> raw, int[] voiceChannels)
    {
        List<int>[] channelVoices = Enumerable.Range(0, 16).Select(_ => new List<int>()).ToArray();
        for (int v = 0; v < voiceChannels.Length; v++)
        {
            int ch = voiceChannels[v];
            if (ch is >= 0 and < 16)
                channelVoices[ch].Add(v);
        }

        int[,] noteToVoice = new int[16, 128];
        int[] voiceNote = Enumerable.Repeat(-1, WtsVoiceCount).ToArray();
        long[] voiceStartTick = new long[WtsVoiceCount];
        int[] channelProgram = new int[16];
        for (int ch = 0; ch < 16; ch++)
        {
            for (int n = 0; n < 128; n++)
                noteToVoice[ch, n] = -1;
        }

        List<TimelineEvent> timeline = new();
        foreach (RawEvent ev in raw)
        {
            if (ev.Kind == RawKind.Tempo)
            {
                timeline.Add(new TimelineEvent(ev.Tick, EventKind.Tempo, -1, -1, 0, 0, 0, ev.Bpm));
                continue;
            }

            if (ev.Channel >= 16)
                continue;
            if (ev.Kind == RawKind.ProgramChange)
            {
                channelProgram[ev.Channel] = ev.Program & 0x7F;
                continue;
            }

            List<int> voices = channelVoices[ev.Channel];
            if (voices.Count == 0)
                continue;

            if (ev.Kind == RawKind.NoteOn)
            {
                int assignedVoice = noteToVoice[ev.Channel, ev.Note];
                if (assignedVoice < 0)
                {
                    assignedVoice = voices.FirstOrDefault(v => voiceNote[v] < 0, -1);
                    if (assignedVoice < 0)
                    {
                        long oldest = long.MaxValue;
                        foreach (int v in voices)
                        {
                            if (voiceStartTick[v] < oldest)
                            {
                                oldest = voiceStartTick[v];
                                assignedVoice = v;
                            }
                        }

                        int stolenNote = voiceNote[assignedVoice];
                        if (stolenNote is >= 0 and < 128)
                            noteToVoice[ev.Channel, stolenNote] = -1;
                    }

                    noteToVoice[ev.Channel, ev.Note] = assignedVoice;
                    voiceNote[assignedVoice] = ev.Note;
                }

                voiceStartTick[assignedVoice] = ev.Tick;
                int program = ev.Channel == 9 ? 0 : channelProgram[ev.Channel];
                timeline.Add(new TimelineEvent(ev.Tick, EventKind.NoteOn, assignedVoice,
                    ev.Channel, ev.Note, ev.Velocity, program, 0));
            }
            else if (ev.Kind == RawKind.NoteOff)
            {
                int releasedVoice = noteToVoice[ev.Channel, ev.Note];
                if (releasedVoice >= 0)
                {
                    noteToVoice[ev.Channel, ev.Note] = -1;
                    voiceNote[releasedVoice] = -1;
                    timeline.Add(new TimelineEvent(ev.Tick, EventKind.NoteOff, releasedVoice,
                        ev.Channel, ev.Note, 0, 0, 0));
                }
            }
        }

        timeline.Sort((a, b) =>
        {
            int c = a.Tick.CompareTo(b.Tick);
            if (c != 0)
                return c;
            c = a.Voice.CompareTo(b.Voice);
            return c != 0 ? c : a.Note.CompareTo(b.Note);
        });
        return timeline;
    }

    private static int Priority(RawKind kind) => kind switch
    {
        RawKind.Tempo => 0,
        RawKind.ProgramChange => 1,
        RawKind.NoteOff => 2,
        RawKind.NoteOn => 3,
        _ => 4,
    };

    private enum RawKind { NoteOff, NoteOn, ProgramChange, Tempo }
    private enum EventKind { NoteOff, NoteOn, Tempo }
    private sealed record RawEvent(long Tick, RawKind Kind, int Channel, int Note, int Velocity, int Program, int Bpm, int Order);
    private sealed record TimelineEvent(long Tick, EventKind Kind, int Voice, int Channel, int Note, int Velocity, int Program, int Bpm);
    private sealed record ChannelInfo(byte Channel)
    {
        public int NoteCount { get; set; }
        public int Program { get; set; }
        public int MaxPolyphony { get; set; }
    }
}
