namespace e6502.Avalonia.Hardware;

/// <summary>
/// MusicEngine: manages SID-based SFX playback and a 3-voice MML music sequencer.
/// Tick() must be called at 60Hz.
/// </summary>
public sealed class MusicEngine
{
    // -------------------------------------------------------------------------
    // Constants
    // -------------------------------------------------------------------------

    private const int VoiceCount = 3;
    private const int InstrumentSlots = 16;
    private const double CpuClock = 985248.0; // PAL
    private const ushort SidBase = 0xD400;

    // -------------------------------------------------------------------------
    // SID register helpers
    // -------------------------------------------------------------------------

    // Voice base register offset: voice 0 → 0, voice 1 → 7, voice 2 → 14
    private static int VoiceBase(int v) => v * 7;

    private static ushort FreqLo(int v) => (ushort)(SidBase + VoiceBase(v) + 0);
    private static ushort FreqHi(int v) => (ushort)(SidBase + VoiceBase(v) + 1);
    private static ushort PwLo(int v)   => (ushort)(SidBase + VoiceBase(v) + 2);
    private static ushort PwHi(int v)   => (ushort)(SidBase + VoiceBase(v) + 3);
    private static ushort Ctrl(int v)   => (ushort)(SidBase + VoiceBase(v) + 4);
    private static ushort Ad(int v)     => (ushort)(SidBase + VoiceBase(v) + 5);
    private static ushort Sr(int v)     => (ushort)(SidBase + VoiceBase(v) + 6);

    private const ushort RegFilterLo  = SidBase + 0x15;
    private const ushort RegFilterHi  = SidBase + 0x16;
    private const ushort RegResonRoute = SidBase + 0x17;
    private const ushort RegVolMode   = SidBase + 0x18;

    // -------------------------------------------------------------------------
    // MIDI → SID frequency
    // -------------------------------------------------------------------------

    private static int MidiToSid(int midi)
    {
        double hz = 440.0 * Math.Pow(2.0, (midi - 69) / 12.0);
        return (int)(hz * 16777216.0 / CpuClock);
    }

    private void WriteFreq(int voice, int sidFreq)
    {
        _sid.Write(FreqLo(voice), (byte)(sidFreq & 0xFF));
        _sid.Write(FreqHi(voice), (byte)((sidFreq >> 8) & 0xFF));
    }

    private void WritePulse(int voice, int pw)
    {
        pw = Math.Clamp(pw, 0, 4095);
        _sid.Write(PwLo(voice), (byte)(pw & 0xFF));
        _sid.Write(PwHi(voice), (byte)((pw >> 8) & 0x0F));
    }

    // -------------------------------------------------------------------------
    // Fields
    // -------------------------------------------------------------------------

    private readonly SidChip _sid;

    // Instrument table
    private readonly SidInstrument[] _instruments = new SidInstrument[InstrumentSlots];

    // SFX state
    private int _sfxVoice = -1;
    private int _sfxFrames;
    private bool _sfxCompleted;

    // Music sequencer
    private bool _musicPlaying;
    private int _bpm = 120;
    private bool _loop;
    private int[] _stealPriority = { 2, 1, 0 }; // steal voice[0] last

    // Per-voice sequencer state
    private readonly VoiceState[] _voices = new VoiceState[VoiceCount];

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------

    public MusicEngine(CompositeBusDevice bus)
    {
        _sid = bus.Sid;

        // Default instrument 0: pulse, A=0, D=9, S=0, R=6
        _instruments[0] = new SidInstrument
        {
            Waveform = 0x40, // pulse
            Attack   = 0,
            Decay    = 9,
            Sustain  = 0,
            Release  = 6,
            PulseWidth = 2048
        };

        for (int i = 1; i < InstrumentSlots; i++)
            _instruments[i] = _instruments[0]; // default copy

        for (int i = 0; i < VoiceCount; i++)
            _voices[i] = new VoiceState();
    }

    // -------------------------------------------------------------------------
    // Public properties
    // -------------------------------------------------------------------------

    public bool IsPlaying => _sfxVoice >= 0 && _sfxFrames > 0;

    public bool SfxCompleted
    {
        get
        {
            bool v = _sfxCompleted;
            _sfxCompleted = false;
            return v;
        }
    }

    public bool IsMusicPlaying => _musicPlaying;

    // -------------------------------------------------------------------------
    // Instrument API
    // -------------------------------------------------------------------------

    public void DefineInstrument(int id, byte waveform, byte attack, byte decay, byte sustain, byte release,
        ushort pulseWidth = 2048)
    {
        if (id < 0 || id >= InstrumentSlots) return;
        _instruments[id] = new SidInstrument
        {
            Waveform   = waveform,
            Attack     = attack,
            Decay      = decay,
            Sustain    = sustain,
            Release    = release,
            PulseWidth = pulseWidth
        };
    }

    public SidInstrument GetInstrument(int id)
    {
        if (id < 0 || id >= InstrumentSlots || _instruments[id] == null)
            return _instruments[0];
        return _instruments[id];
    }

    // -------------------------------------------------------------------------
    // Volume
    // -------------------------------------------------------------------------

    public void SetVolume(int level)
    {
        level = Math.Clamp(level, 0, 15);
        byte current = _sid.Read(RegVolMode);
        byte next = (byte)((current & 0xF0) | (level & 0x0F));
        _sid.Write(RegVolMode, next);
    }

    // -------------------------------------------------------------------------
    // SFX
    // -------------------------------------------------------------------------

    public void PlaySound(int midiNote, int durationFrames, int instrumentId = 0)
    {
        int voice = AllocateSfxVoice();
        if (voice < 0) return; // no voice available

        _sfxVoice  = voice;
        _sfxFrames = durationFrames;

        SidInstrument inst = GetInstrument(instrumentId);
        WriteVoice(voice, midiNote, inst);
    }

    // -------------------------------------------------------------------------
    // Music sequencer API
    // -------------------------------------------------------------------------

    public void SetSequence(int voiceIndex, string mml)
    {
        if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
        _voices[voiceIndex].Events = MmlParser.Parse(mml);
        _voices[voiceIndex].Reset(_bpm);
    }

    public void MusicPlay()
    {
        _musicPlaying = true;
        for (int i = 0; i < VoiceCount; i++)
        {
            _voices[i].Reset(_bpm);
            _voices[i].CurrentInstrument = GetInstrument(0);
        }
    }

    public void MusicStop()
    {
        _musicPlaying = false;
        // Gate off all music voices (not the SFX voice)
        for (int i = 0; i < VoiceCount; i++)
        {
            if (i == _sfxVoice) continue;
            byte current = _sid.Read(Ctrl(i));
            _sid.Write(Ctrl(i), (byte)(current & ~0x01)); // clear gate
        }
    }

    public void SetTempo(int bpm)
    {
        _bpm = bpm;
        for (int i = 0; i < VoiceCount; i++)
            _voices[i].UpdateTempo(bpm);
    }

    public void SetLoop(bool loop) => _loop = loop;

    public void SetPriority(int[] voices) => _stealPriority = voices;

    // -------------------------------------------------------------------------
    // Tick — called at 60Hz
    // -------------------------------------------------------------------------

    public void Tick()
    {
        TickSfx();
        if (_musicPlaying)
            TickMusic();
    }

    // -------------------------------------------------------------------------
    // Private: SFX tick
    // -------------------------------------------------------------------------

    private void TickSfx()
    {
        if (_sfxVoice < 0) return;

        if (_sfxFrames > 0)
            _sfxFrames--;

        if (_sfxFrames == 0)
        {
            // Clear gate, keep waveform
            byte current = _sid.Read(Ctrl(_sfxVoice));
            _sid.Write(Ctrl(_sfxVoice), (byte)(current & ~0x01));
            _sfxCompleted = true;
            _sfxVoice = -1;
        }
    }

    // -------------------------------------------------------------------------
    // Private: music tick
    // -------------------------------------------------------------------------

    private void TickMusic()
    {
        bool anyActive = false;

        for (int vi = 0; vi < VoiceCount; vi++)
        {
            if (vi == _sfxVoice) continue; // voice stolen by SFX
            var vs = _voices[vi];
            if (vs.Events == null || vs.Events.Count == 0) continue;
            if (vs.Done) continue;

            anyActive = true;
            TickVoice(vi, vs);
        }

        if (!anyActive)
        {
            if (_loop)
            {
                // Restart all voices
                for (int i = 0; i < VoiceCount; i++)
                    _voices[i].Reset(_bpm);
            }
            else
            {
                _musicPlaying = false;
            }
        }
    }

    private void TickVoice(int vi, VoiceState vs)
    {
        // Accumulate ticks
        vs.TickAccum += vs.TicksPerFrame;

        // Per-frame effects
        TickArp(vi, vs);
        TickPwm(vi, vs);
        TickVibrato(vi, vs);
        TickPortamento(vi, vs);
        TickFilterSweep(vs);

        // Process events while we have accumulated ticks
        while (vs.WaitTicks <= 0 && vs.EventIndex < vs.Events!.Count)
        {
            ProcessEvent(vi, vs);
        }

        // Consume accumulated ticks
        if (vs.WaitTicks > 0 && vs.TickAccum >= 1.0)
        {
            double consume = Math.Min(vs.TickAccum, vs.WaitTicks);
            vs.WaitTicks -= consume;
            vs.TickAccum -= consume;
        }

        // Check if done
        if (vs.EventIndex >= vs.Events!.Count && vs.WaitTicks <= 0)
            vs.Done = true;
    }

    private void ProcessEvent(int vi, VoiceState vs)
    {
        var ev = vs.Events![vs.EventIndex++];

        switch (ev.Type)
        {
            case MmlEventType.NoteOn:
            {
                vs.CurrentMidi = ev.Param1;
                vs.WaitTicks   = ev.Param2;
                vs.ArpNotes    = null;
                vs.Portamento  = false;

                if (vs.PortamentoActive)
                {
                    // Slide from current freq toward target
                    vs.PortamentoTarget = MidiToSid(ev.Param1);
                    vs.PortamentoActive = false;
                }
                else
                {
                    int sidFreq = MidiToSid(ev.Param1);
                    vs.CurrentSidFreq = sidFreq;
                    WriteVoice(vi, ev.Param1, vs.CurrentInstrument);
                }
                break;
            }

            case MmlEventType.Rest:
            {
                vs.WaitTicks  = ev.Param2;
                vs.ArpNotes   = null;
                vs.CurrentMidi = -1;
                // Gate off
                byte current = _sid.Read(Ctrl(vi));
                _sid.Write(Ctrl(vi), (byte)(current & ~0x01));
                break;
            }

            case MmlEventType.ArpStart:
            {
                vs.WaitTicks = ev.Param2;
                vs.ArpNotes  = ev.ArpNotes;
                vs.ArpIndex  = 0;
                if (vs.ArpNotes != null && vs.ArpNotes.Length > 0)
                {
                    vs.CurrentMidi = vs.ArpNotes[0];
                    WriteVoice(vi, vs.ArpNotes[0], vs.CurrentInstrument);
                }
                break;
            }

            case MmlEventType.SetTempo:
                _bpm = ev.Param1;
                for (int i = 0; i < VoiceCount; i++)
                    _voices[i].UpdateTempo(_bpm);
                break;

            case MmlEventType.SetInstrument:
                vs.CurrentInstrument = GetInstrument(ev.Param1);
                break;

            case MmlEventType.SetOctave:
            case MmlEventType.OctaveUp:
            case MmlEventType.OctaveDown:
            case MmlEventType.SetDefaultLen:
                // State tracked by parser; engine ignores
                break;

            case MmlEventType.SetVibrato:
                vs.VibratoDepth = ev.Param1;
                vs.VibratoPhase = 0;
                break;

            case MmlEventType.SetPulseWidth:
                vs.PulseWidth = ev.Param1;
                vs.PwmDir     = 0;
                WritePulse(vi, vs.PulseWidth);
                break;

            case MmlEventType.PwmSweep:
                vs.PwmDir = ev.Param1;
                break;

            case MmlEventType.Portamento:
                vs.Portamento = true;
                vs.PortamentoActive = true;
                break;

            case MmlEventType.SetFilterCutoff:
            {
                int cutoff    = Math.Clamp(ev.Param1, 0, 2047);
                int resonance = Math.Clamp(ev.Param2, 0, 15);
                _sid.Write(RegFilterLo,   (byte)(cutoff & 0x07));
                _sid.Write(RegFilterHi,   (byte)((cutoff >> 3) & 0xFF));
                byte rr = _sid.Read(RegResonRoute);
                rr = (byte)((rr & 0x0F) | (resonance << 4));
                _sid.Write(RegResonRoute, rr);
                break;
            }

            case MmlEventType.FilterMode:
            {
                // Param1: 0=off,1=LP,2=BP,4=HP
                int mode = ev.Param1;
                byte vol = _sid.Read(RegVolMode);
                // Bits 4-6 of $D418 control filter mode (LP=bit4,BP=bit5,HP=bit6)
                byte next = (byte)((vol & 0x8F) | ((mode & 0x7) << 4));
                _sid.Write(RegVolMode, next);
                break;
            }

            case MmlEventType.FilterSweep:
                vs.FilterSweepDir = ev.Param1;
                break;

            case MmlEventType.LoopStart:
            case MmlEventType.LoopEnd:
                // Loops already expanded by parser; sentinels just mark structure
                break;
        }
    }

    // -------------------------------------------------------------------------
    // Per-frame effects
    // -------------------------------------------------------------------------

    private void TickArp(int vi, VoiceState vs)
    {
        if (vs.ArpNotes == null || vs.ArpNotes.Length == 0) return;
        vs.ArpIndex = (vs.ArpIndex + 1) % vs.ArpNotes.Length;
        int note = vs.ArpNotes[vs.ArpIndex];
        WriteFreq(vi, MidiToSid(note));
    }

    private void TickPwm(int vi, VoiceState vs)
    {
        if (vs.PwmDir == 0) return;
        vs.PulseWidth += vs.PwmDir * 32;
        vs.PulseWidth  = Math.Clamp(vs.PulseWidth, 0, 4095);
        WritePulse(vi, vs.PulseWidth);
    }

    private void TickVibrato(int vi, VoiceState vs)
    {
        if (vs.VibratoDepth == 0 || vs.CurrentMidi < 0) return;
        vs.VibratoPhase += 0.15; // ~2.9Hz at 60fps ... close enough
        double offset = Math.Sin(vs.VibratoPhase) * vs.VibratoDepth;
        int modFreq = (int)(vs.CurrentSidFreq + offset * vs.CurrentSidFreq / 100.0);
        WriteFreq(vi, Math.Max(0, modFreq));
    }

    private void TickPortamento(int vi, VoiceState vs)
    {
        if (vs.PortamentoTarget < 0) return;
        int step = Math.Max(1, Math.Abs(vs.PortamentoTarget - vs.CurrentSidFreq) / 8);
        if (vs.CurrentSidFreq < vs.PortamentoTarget)
            vs.CurrentSidFreq = Math.Min(vs.CurrentSidFreq + step, vs.PortamentoTarget);
        else if (vs.CurrentSidFreq > vs.PortamentoTarget)
            vs.CurrentSidFreq = Math.Max(vs.CurrentSidFreq - step, vs.PortamentoTarget);

        WriteFreq(vi, vs.CurrentSidFreq);

        if (vs.CurrentSidFreq == vs.PortamentoTarget)
            vs.PortamentoTarget = -1;
    }

    private void TickFilterSweep(VoiceState vs)
    {
        if (vs.FilterSweepDir == 0) return;
        byte lo = _sid.Read(RegFilterLo);
        byte hi = _sid.Read(RegFilterHi);
        int cutoff = ((hi << 3) | (lo & 0x07)) + vs.FilterSweepDir * 8;
        cutoff = Math.Clamp(cutoff, 0, 2047);
        _sid.Write(RegFilterLo, (byte)(cutoff & 0x07));
        _sid.Write(RegFilterHi, (byte)((cutoff >> 3) & 0xFF));
    }

    // -------------------------------------------------------------------------
    // Private: write a full voice setup
    // -------------------------------------------------------------------------

    private void WriteVoice(int voice, int midiNote, SidInstrument inst)
    {
        int sidFreq = MidiToSid(midiNote);
        _voices[voice].CurrentSidFreq = sidFreq;

        WriteFreq(voice, sidFreq);
        WritePulse(voice, inst.PulseWidth);
        _sid.Write(Ad(voice), inst.Ad);
        _sid.Write(Sr(voice), inst.Sr);
        _sid.Write(Ctrl(voice), (byte)(inst.Waveform | 0x01)); // gate on
    }

    // -------------------------------------------------------------------------
    // Private: voice allocation
    // -------------------------------------------------------------------------

    private int AllocateSfxVoice()
    {
        // Prefer a voice with no music sequence
        for (int i = 0; i < VoiceCount; i++)
        {
            if (_voices[i].Events == null || _voices[i].Events!.Count == 0)
                return i;
        }

        // Steal by priority order (last element = highest steal preference)
        foreach (int v in _stealPriority)
        {
            if (v >= 0 && v < VoiceCount)
                return v;
        }

        return 0; // fallback
    }

    // -------------------------------------------------------------------------
    // VoiceState inner class
    // -------------------------------------------------------------------------

    private sealed class VoiceState
    {
        public List<MmlEvent>?  Events;
        public int              EventIndex;
        public double           TickAccum;
        public double           TicksPerFrame;
        public double           WaitTicks;
        public bool             Done;

        public SidInstrument    CurrentInstrument = null!;
        public int              CurrentMidi       = -1;
        public int              CurrentSidFreq;

        // Arp
        public int[]? ArpNotes;
        public int    ArpIndex;

        // PWM
        public int PulseWidth = 2048;
        public int PwmDir;

        // Vibrato
        public int    VibratoDepth;
        public double VibratoPhase;

        // Portamento
        public bool Portamento;
        public bool PortamentoActive;
        public int  PortamentoTarget = -1;

        // Filter sweep
        public int FilterSweepDir;

        public void Reset(int bpm)
        {
            EventIndex    = 0;
            TickAccum     = 0;
            WaitTicks     = 0;
            Done          = false;
            ArpNotes      = null;
            ArpIndex      = 0;
            PwmDir        = 0;
            VibratoDepth  = 0;
            VibratoPhase  = 0;
            Portamento    = false;
            PortamentoActive = false;
            PortamentoTarget = -1;
            FilterSweepDir   = 0;
            UpdateTempo(bpm);
        }

        public void UpdateTempo(int bpm)
        {
            // ticks per frame = ticks-per-beat * bpm / frames-per-minute
            // ticks-per-beat = 96, frames-per-minute = 60 * 60 = 3600
            TicksPerFrame = 96.0 * bpm / 3600.0;
        }
    }
}
