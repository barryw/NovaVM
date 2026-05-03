using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Controls how MIDI channels are routed to SID and WTS voices.
/// </summary>
public enum MidiRoutingMode
{
    /// <summary>Up to 14 voices: first 8 busiest channels to WTS (6-13), overflow to SID (0-5).</summary>
    Auto,
    /// <summary>Reserved for future channel routing table. Currently behaves like Auto.</summary>
    Manual,
    /// <summary>Original 6-voice SID-only mode.</summary>
    SidOnly
}

/// <summary>
/// Real-time MIDI playback driver. Ticked at 60Hz by the main loop.
/// Converts MIDI events into direct MusicEngine voice commands.
/// </summary>
public sealed class MidiPlayback
{
    private readonly MusicEngine _engine;
    private readonly int _frameRateHz;

    private const int MaxVoices = 14;
    private const int SidVoiceCount = 6;
    private const int WtsVoiceCount = 8;
    private const int FirstWtsVoice = 6;

    private sealed class TimelineEvent
    {
        public long MidiTick;
        public int Voice;       // 0-13 (-1 for tempo)
        public int MidiNote;    // -1 for note-off
        public int Velocity;
        public int InstrumentId;
        public int Bpm;         // for tempo events
    }

    private List<TimelineEvent>? _timeline;
    private int _timelineIndex;
    private double _midiTickAccum;
    private double _midiTicksPerFrame;
    private int _ppqn;
    private bool _playing;
    private int _activeVoiceCount;

    // Display hold: when a note is on+off in the same tick, hold it for 1 frame
    private readonly bool[] _heldForDisplay = new bool[MaxVoices];

    public bool IsPlaying => _playing;
    public MidiRoutingMode RoutingMode { get; set; } = MidiRoutingMode.Auto;
    public int AvailableVoiceCount => RoutingMode == MidiRoutingMode.SidOnly ? SidVoiceCount : MaxVoices;

    public MidiPlayback(MusicEngine engine, int frameRateHz = VgcConstants.FrameRateHz)
    {
        _engine = engine;
        _frameRateHz = frameRateHz;
        Reset();
    }

    /// <summary>
    /// Prepare and start playback of a MIDI file.
    /// voiceToChannel maps voice index (0-5) to MIDI channel.
    /// instrumentSlots maps voice index (0-5) to MusicEngine instrument slot.
    /// </summary>
    public void Play(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        _activeVoiceCount = voiceToChannel.Length;
        _ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        _timeline = BuildTimeline(midi, voiceToChannel, instrumentSlots);
        _timelineIndex = 0;
        _midiTickAccum = 0;

        // Default 120 BPM
        SetTempo(120);

        if (_timeline.Count > 0)
        {
            _engine.SetTotalFrames(ComputeTotalFrames());
        }

        _engine.EnterMidiMode();
        _playing = true;
    }

    public void Stop()
    {
        _playing = false;
        _timeline = null;
        _engine.ExitMidiMode();
    }

    public void Reset()
    {
        _playing = false;
        _timeline = null;
        _timelineIndex = 0;
        _midiTickAccum = 0;
        _midiTicksPerFrame = 0;
        _ppqn = 0;
        _activeVoiceCount = 0;
        RoutingMode = MidiRoutingMode.Auto;
        Array.Clear(_heldForDisplay);
        _engine.ExitMidiMode();
    }

    /// <summary>Called at the system frame rate. Advance the timeline and fire events.</summary>
    public void Tick()
    {
        if (!_playing || _timeline is null) return;

        // Clear any display-held notes from previous tick
        int vc = _activeVoiceCount;
        for (int i = 0; i < vc; i++)
        {
            if (_heldForDisplay[i])
            {
                _engine.SetVoiceNoteExternal(i, -1);
                _heldForDisplay[i] = false;
            }
        }

        _midiTickAccum += _midiTicksPerFrame;

        // Track last note-on per voice this tick
        int[] lastNoteOn = new int[vc];
        for (int i = 0; i < vc; i++) lastNoteOn[i] = -1;

        while (_timelineIndex < _timeline.Count)
        {
            var ev = _timeline[_timelineIndex];
            if (ev.MidiTick > _midiTickAccum) break;

            _timelineIndex++;

            if (ev.Voice == -1)
            {
                // Tempo change
                SetTempo(ev.Bpm);
                continue;
            }

            if (ev.MidiNote < 0)
                _engine.DirectNoteOff(ev.Voice);
            else
            {
                _engine.DirectNoteOn(ev.Voice, ev.MidiNote, ev.Velocity, ev.InstrumentId);
                if (ev.Voice < vc) lastNoteOn[ev.Voice] = ev.MidiNote;
            }
        }

        // Hold notes for display if they were on+off in the same tick
        for (int i = 0; i < vc; i++)
        {
            if (lastNoteOn[i] > 0 && _engine.GetVoiceNote(i) == 0)
            {
                _engine.SetVoiceNoteExternal(i, lastNoteOn[i]);
                _heldForDisplay[i] = true;
            }
        }

        if (_timelineIndex >= _timeline.Count)
        {
            Stop();
        }
    }

    private void SetTempo(int bpm)
    {
        _midiTicksPerFrame = (double)_ppqn * bpm / (_frameRateHz * 60.0);
    }

    /// <summary>
    /// Walk the timeline accounting for tempo changes to compute accurate total frames.
    /// </summary>
    private int ComputeTotalFrames()
    {
        if (_timeline == null || _timeline.Count == 0) return 0;

        double ticksPerFrame = _midiTicksPerFrame; // starts at current (default 120 BPM)
        long prevTick = 0;
        double totalFrames = 0;

        foreach (var ev in _timeline)
        {
            long deltaTicks = ev.MidiTick - prevTick;
            if (deltaTicks > 0 && ticksPerFrame > 0)
                totalFrames += deltaTicks / ticksPerFrame;
            prevTick = ev.MidiTick;

            if (ev.Voice == -1) // tempo change
                ticksPerFrame = (double)_ppqn * ev.Bpm / (_frameRateHz * 60.0);
        }

        return (int)totalFrames;
    }

    private List<TimelineEvent> BuildTimeline(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        // Build channel → list of available voices mapping
        var channelVoices = new Dictionary<int, List<int>>();
        for (int v = 0; v < voiceToChannel.Length; v++)
        {
            int ch = voiceToChannel[v];
            if (!channelVoices.ContainsKey(ch))
                channelVoices[ch] = new List<int>();
            channelVoices[ch].Add(v);
        }

        var events = new List<TimelineEvent>();

        // Tempo events
        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                if (timedEvent.Event is SetTempoEvent tempo)
                {
                    int bpm = (int)(60_000_000.0 / tempo.MicrosecondsPerQuarterNote);
                    events.Add(new TimelineEvent { MidiTick = timedEvent.Time, Voice = -1, Bpm = bpm });
                }
            }
        }

        // Collect note events for channels we care about
        var channelEvents = new List<(long time, int channel, int midi, int velocity, bool isOn)>();

        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                int ch = -1; int midiNote = 0; int vel = 0; bool isOn = false;

                switch (timedEvent.Event)
                {
                    case NoteOnEvent noteOn when noteOn.Velocity > 0:
                        ch = noteOn.Channel; midiNote = noteOn.NoteNumber; vel = noteOn.Velocity; isOn = true; break;
                    case NoteOnEvent noteOn when noteOn.Velocity == 0:
                        ch = noteOn.Channel; midiNote = noteOn.NoteNumber; break;
                    case NoteOffEvent noteOff:
                        ch = noteOff.Channel; midiNote = noteOff.NoteNumber; break;
                }

                if (ch >= 0 && channelVoices.ContainsKey(ch))
                    channelEvents.Add((timedEvent.Time, ch, midiNote, vel, isOn));
            }
        }

        channelEvents.Sort((a, b) => a.time.CompareTo(b.time));

        // Polyphonic voice allocator: per channel, track which voice plays which note
        // noteToVoice[ch] maps active MIDI note → assigned voice index
        var noteToVoice = new Dictionary<int, Dictionary<int, int>>();
        // voiceNote[voice] = currently playing MIDI note (-1 = free)
        var voiceNote = new int[voiceToChannel.Length];
        // voiceStartTick[voice] = when the current note started (for steal priority)
        var voiceStartTick = new long[voiceToChannel.Length];
        for (int i = 0; i < voiceNote.Length; i++)
            voiceNote[i] = -1;

        foreach (var (time, channel, midiNote, velocity, isOn) in channelEvents)
        {
            if (!channelVoices.TryGetValue(channel, out var voices)) continue;

            if (!noteToVoice.ContainsKey(channel))
                noteToVoice[channel] = new Dictionary<int, int>();
            var chNoteMap = noteToVoice[channel];

            if (isOn)
            {
                // If this note is already playing on a voice, do legato slide
                if (chNoteMap.TryGetValue(midiNote, out int existingVoice))
                {
                    // Same note re-triggered — re-gate for articulation
                    events.Add(new TimelineEvent
                    {
                        MidiTick = time, Voice = existingVoice, MidiNote = midiNote,
                        Velocity = velocity, InstrumentId = instrumentSlots[existingVoice]
                    });
                    voiceStartTick[existingVoice] = time;
                    continue;
                }

                // Find a free voice for this channel
                int assignedVoice = -1;
                foreach (int v in voices)
                {
                    if (voiceNote[v] < 0)
                    {
                        assignedVoice = v;
                        break;
                    }
                }

                // No free voice — steal the oldest one on this channel
                if (assignedVoice < 0)
                {
                    long oldestTick = long.MaxValue;
                    foreach (int v in voices)
                    {
                        if (voiceStartTick[v] < oldestTick)
                        {
                            oldestTick = voiceStartTick[v];
                            assignedVoice = v;
                        }
                    }

                    // Remove the stolen note from the map
                    int stolenNote = voiceNote[assignedVoice];
                    chNoteMap.Remove(stolenNote);
                }

                chNoteMap[midiNote] = assignedVoice;
                voiceNote[assignedVoice] = midiNote;
                voiceStartTick[assignedVoice] = time;

                events.Add(new TimelineEvent
                {
                    MidiTick = time, Voice = assignedVoice, MidiNote = midiNote,
                    Velocity = velocity, InstrumentId = instrumentSlots[assignedVoice]
                });
            }
            else
            {
                // Note off — release the voice playing this note
                if (chNoteMap.TryGetValue(midiNote, out int releasedVoice))
                {
                    chNoteMap.Remove(midiNote);
                    voiceNote[releasedVoice] = -1;
                    events.Add(new TimelineEvent { MidiTick = time, Voice = releasedVoice, MidiNote = -1 });
                }
            }
        }

        events.Sort((a, b) => a.MidiTick.CompareTo(b.MidiTick));
        return events;
    }
}
