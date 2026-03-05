using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Real-time MIDI playback driver. Ticked at 60Hz by the main loop.
/// Converts MIDI events into direct MusicEngine voice commands.
/// </summary>
public sealed class MidiPlayback
{
    private readonly MusicEngine _engine;

    private sealed class TimelineEvent
    {
        public long MidiTick;
        public int Voice;       // 0-5 (-1 for tempo)
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

    public bool IsPlaying => _playing;

    public MidiPlayback(MusicEngine engine)
    {
        _engine = engine;
    }

    /// <summary>
    /// Prepare and start playback of a MIDI file.
    /// voiceToChannel maps voice index (0-5) to MIDI channel.
    /// instrumentSlots maps voice index (0-5) to MusicEngine instrument slot.
    /// </summary>
    public void Play(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        _ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        _timeline = BuildTimeline(midi, voiceToChannel, instrumentSlots);
        _timelineIndex = 0;
        _midiTickAccum = 0;

        // Default 120 BPM
        SetTempo(120);

        _engine.EnterMidiMode();
        _playing = true;
    }

    public void Stop()
    {
        _playing = false;
        _timeline = null;
        _engine.ExitMidiMode();
    }

    /// <summary>Called at 60Hz. Advance the timeline and fire events.</summary>
    public void Tick()
    {
        if (!_playing || _timeline is null) return;

        _midiTickAccum += _midiTicksPerFrame;

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
                _engine.DirectNoteOn(ev.Voice, ev.MidiNote, ev.Velocity, ev.InstrumentId);
        }

        if (_timelineIndex >= _timeline.Count)
        {
            Stop();
        }
    }

    private void SetTempo(int bpm)
    {
        _midiTicksPerFrame = (double)_ppqn * bpm / (60.0 * 60.0);
    }

    private List<TimelineEvent> BuildTimeline(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        var channelToVoice = new Dictionary<int, int>();
        for (int v = 0; v < voiceToChannel.Length; v++)
            channelToVoice[voiceToChannel[v]] = v;

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

        // Track active notes per voice for top-note-wins
        var activePerVoice = new Dictionary<int, SortedSet<int>>();
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

                if (ch >= 0 && channelToVoice.ContainsKey(ch))
                    channelEvents.Add((timedEvent.Time, ch, midiNote, vel, isOn));
            }
        }

        channelEvents.Sort((a, b) => a.time.CompareTo(b.time));

        foreach (var (time, channel, midiNote, velocity, isOn) in channelEvents)
        {
            if (!channelToVoice.TryGetValue(channel, out int voice)) continue;

            if (!activePerVoice.ContainsKey(voice))
                activePerVoice[voice] = new SortedSet<int>();

            var active = activePerVoice[voice];
            int prevTop = active.Count > 0 ? active.Max : -1;

            if (isOn) active.Add(midiNote);
            else active.Remove(midiNote);

            int newTop = active.Count > 0 ? active.Max : -1;

            if (newTop != prevTop)
            {
                if (prevTop >= 0)
                    events.Add(new TimelineEvent { MidiTick = time, Voice = voice, MidiNote = -1 });

                if (newTop >= 0)
                    events.Add(new TimelineEvent
                    {
                        MidiTick = time, Voice = voice, MidiNote = newTop,
                        Velocity = velocity > 0 ? velocity : 100,
                        InstrumentId = instrumentSlots[voice]
                    });
            }
        }

        events.Sort((a, b) => a.MidiTick.CompareTo(b.MidiTick));
        return events;
    }
}
