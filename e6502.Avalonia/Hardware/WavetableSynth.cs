using System;

namespace e6502.Avalonia.Hardware;

public sealed class WavetableSynth : IDisposable
{
    public const int VoiceCount = 8;
    private const int SampleRate = 44100;
    private const float MinTime = 0.0001f;

    private readonly WtsVoice[] _voices = new WtsVoice[VoiceCount];
    private SampleBank? _bank;
    private readonly ReverbEffect _reverb = new(SampleRate);
    private readonly ChorusEffect _chorus = new(SampleRate);
    private readonly OpenAlRenderer? _renderer;
    private readonly object _lock = new();

    // Per-voice latch state for register protocol
    private readonly byte[] _latchVelocity = new byte[VoiceCount];
    private readonly byte[] _latchInstrument = new byte[VoiceCount];
    private readonly byte[] _latchPitchBendLo = new byte[VoiceCount];
    private readonly byte[] _latchPitchBendHi = new byte[VoiceCount];

    // Instrument enumeration state
    private byte _enumBank, _enumProgram;
    private byte[] _enumName = new byte[29];

    public byte ReverbLevel { get; set; } = 80;
    public byte ChorusLevel { get; set; } = 40;
    public byte MasterVolume { get; set; } = 255;

    public WavetableSynth(bool enableAudio = false)
    {
        for (int i = 0; i < VoiceCount; i++)
        {
            _voices[i] = new WtsVoice();
            _latchPitchBendHi[i] = 0x80;
        }

        if (enableAudio)
        {
            try
            {
                _renderer = new OpenAlRenderer(RenderSamples, SampleRate, stereo: true);
            }
            catch
            {
                _renderer = null;
            }
        }
    }

    public void LoadBank(SampleBank bank)
    {
        lock (_lock)
        {
            AllNotesOff();
            _bank = bank;
        }
    }

    public int InstrumentCount => _bank?.Instruments.Count ?? 0;

    public string? GetInstrumentName(int index)
    {
        if (_bank == null || index < 0 || index >= _bank.Instruments.Count)
            return null;
        return _bank.Instruments[index].Name;
    }

    public int GetInstrumentBank(int index)
    {
        if (_bank == null || index < 0 || index >= _bank.Instruments.Count)
            return 0;
        return _bank.Instruments[index].MidiBank;
    }

    public int GetInstrumentProgram(int index)
    {
        if (_bank == null || index < 0 || index >= _bank.Instruments.Count)
            return 0;
        return _bank.Instruments[index].MidiProgram;
    }

    public void NoteOn(int voice, int note, int velocity, int instrument)
    {
        lock (_lock)
        {
            if ((uint)voice >= VoiceCount) return;
            if (_bank == null) return;

            // Resolve instrument: if high bits encode a bank, use bank+program lookup
            // Format: (bank << 8) | program — bank 0 = melodic, bank 128 = drums
            int bank = (instrument >> 8) & 0xFF;
            int program = instrument & 0x7F;
            int idx = _bank.FindByProgram(bank, program);
            if (idx < 0)
            {
                // Fall back to bank 0 lookup, then direct index
                idx = _bank.FindByProgram(0, program);
                if (idx < 0)
                {
                    if (instrument >= 0 && instrument < _bank.Instruments.Count)
                        idx = instrument;
                    else
                        return;
                }
            }

            var inst = _bank.Instruments[idx];
            var region = inst.FindRegion(note, velocity);
            if (region == null) return;

            var v = _voices[voice];
            v.Phase = 0;
            v.Note = note;
            v.Velocity = velocity;
            v.InstrumentIndex = idx;
            v.Region = region;
            v.Active = true;
            v.Releasing = false;
            v.EnvLevel = 0f;
            v.EnvStage = EnvStage.Attack;

            // Compute AHDSR rates with key-based scaling
            float holdTime = region.HoldTime;
            float decayTime = region.DecayTime;
            if (region.KeynumToHoldTC != 0)
            {
                int holdTC = (int)(Math.Log2(holdTime) * 1200) - region.KeynumToHoldTC * (note - 60);
                holdTime = Math.Clamp((float)Math.Pow(2, holdTC / 1200.0), 0.001f, 100f);
            }
            if (region.KeynumToDecayTC != 0)
            {
                int decayTC = (int)(Math.Log2(decayTime) * 1200) - region.KeynumToDecayTC * (note - 60);
                decayTime = Math.Clamp((float)Math.Pow(2, decayTC / 1200.0), 0.001f, 100f);
            }
            v.HoldSamples = (int)(holdTime * SampleRate);

            if (region.AttackTime < MinTime)
            {
                v.EnvLevel = 1f;
                v.EnvStage = v.HoldSamples > 0 ? EnvStage.Hold : EnvStage.Decay;
                v.AttackRate = 0;
            }
            else
            {
                v.AttackRate = 1f / (region.AttackTime * SampleRate);
            }

            if (decayTime < MinTime)
            {
                if (v.EnvStage == EnvStage.Decay)
                {
                    v.EnvLevel = region.SustainLevel;
                    v.EnvStage = EnvStage.Sustain;
                }
                v.DecayRate = 0;
            }
            else
            {
                v.DecayRate = (1f - region.SustainLevel) / (decayTime * SampleRate);
            }

            v.SustainLevel = region.SustainLevel;

            if (region.ReleaseTime < MinTime)
                v.ReleaseRate = 100f; // instant
            else
                v.ReleaseRate = 0; // computed at NoteOff time
        }
    }

    public void NoteOff(int voice)
    {
        lock (_lock)
        {
            if ((uint)voice >= VoiceCount) return;
            var v = _voices[voice];
            if (!v.Active) return;

            v.Releasing = true;
            v.EnvStage = EnvStage.Release;

            var region = v.Region;
            if (region != null && region.ReleaseTime >= MinTime)
                v.ReleaseRate = v.EnvLevel / (region.ReleaseTime * SampleRate);
            else
                v.ReleaseRate = 100f;

            if (v.ReleaseRate <= 0)
            {
                v.Active = false;
                v.EnvStage = EnvStage.Off;
            }
        }
    }

    public void SetVolume(int voice, byte volume)
    {
        if ((uint)voice < VoiceCount)
            _voices[voice].Volume = volume;
    }

    public void SetPanning(int voice, byte pan)
    {
        if ((uint)voice < VoiceCount)
            _voices[voice].Pan = pan;
    }

    public void SetPitchBend(int voice, ushort bend)
    {
        if ((uint)voice < VoiceCount)
            _voices[voice].PitchBend = bend;
    }

    public byte ActiveVoiceMask
    {
        get
        {
            byte mask = 0;
            for (int i = 0; i < VoiceCount; i++)
                if (_voices[i].Active)
                    mask |= (byte)(1 << i);
            return mask;
        }
    }

    public short[] RenderSamples(int frameCount)
    {
        lock (_lock)
        {
        return RenderSamplesCore(frameCount);
        }
    }


    private short[] RenderSamplesCore(int frameCount)
    {
        var output = new short[frameCount * 2];
        float masterScale = MasterVolume / 255f;
        // Mix headroom: scale down to prevent clipping with multiple voices
        const float MixScale = 1f / 2f;
        float[] mixL = new float[frameCount];
        float[] mixR = new float[frameCount];

        for (int f = 0; f < frameCount; f++)
        {
            float sumL = 0, sumR = 0;

            for (int vi = 0; vi < VoiceCount; vi++)
            {
                var v = _voices[vi];
                if (!v.Active || v.Region == null) continue;

                var region = v.Region;
                var data = region.SampleData;
                if (data.Length == 0) { v.Active = false; continue; }

                // Interpolate sample
                int idx0 = (int)v.Phase;
                int idx1 = idx0 + 1;
                float frac = (float)(v.Phase - idx0);

                if (idx0 >= data.Length)
                {
                    v.Active = false;
                    v.EnvStage = EnvStage.Off;
                    continue;
                }
                if (idx1 >= data.Length) idx1 = idx0;

                float sample = data[idx0] + (data[idx1] - data[idx0]) * frac;

                // Apply envelope, velocity, volume
                float amplitude = sample * v.EnvLevel * (v.Velocity / 127f) * (v.Volume / 255f);

                // Panning (equal-power)
                float panF = v.Pan / 255f;
                float left = MathF.Cos(panF * MathF.PI / 2f);
                float right = MathF.Sin(panF * MathF.PI / 2f);

                sumL += amplitude * left * MixScale;
                sumR += amplitude * right * MixScale;

                // Advance phase
                double bendSemitones = ((v.PitchBend - 0x8000) / 8192.0) * 2.0;
                double tuning = region.TuningCents / 100.0; // cents → semitones
                double pitchRatio = Math.Pow(2.0, (v.Note - region.RootKey + bendSemitones + tuning) / 12.0);
                double phaseInc = region.SampleRate * pitchRatio / (double)SampleRate;
                v.Phase += phaseInc;

                // Loop handling
                if (region.LoopEnabled)
                {
                    if (v.Phase >= region.LoopEnd)
                        v.Phase = region.LoopStart + (v.Phase - region.LoopEnd);
                }
                else if (v.Phase >= data.Length)
                {
                    v.Active = false;
                    v.EnvStage = EnvStage.Off;
                    continue;
                }

                // Advance envelope
                AdvanceEnvelope(v);
            }

            mixL[f] = sumL;
            mixR[f] = sumR;
        }

        // Apply effects
        float[] wetL = new float[frameCount], wetR = new float[frameCount];

        // Chorus
        float chorusMix = ChorusLevel / 255f;
        if (chorusMix > 0)
        {
            Array.Copy(mixL, wetL, frameCount);
            Array.Copy(mixR, wetR, frameCount);
            _chorus.Process(wetL, wetR);
            for (int i = 0; i < frameCount; i++)
            {
                mixL[i] = mixL[i] * (1 - chorusMix) + wetL[i] * chorusMix;
                mixR[i] = mixR[i] * (1 - chorusMix) + wetR[i] * chorusMix;
            }
        }

        // Reverb
        float reverbMix = ReverbLevel / 255f;
        if (reverbMix > 0)
        {
            Array.Copy(mixL, wetL, frameCount);
            Array.Copy(mixR, wetR, frameCount);
            _reverb.Process(wetL, wetR);
            for (int i = 0; i < frameCount; i++)
            {
                mixL[i] = mixL[i] * (1 - reverbMix) + wetL[i] * reverbMix;
                mixR[i] = mixR[i] * (1 - reverbMix) + wetR[i] * reverbMix;
            }
        }

        // Master volume, clamp, and convert to short
        for (int f = 0; f < frameCount; f++)
        {
            float l = Math.Clamp(mixL[f] * masterScale, -1f, 1f);
            float r = Math.Clamp(mixR[f] * masterScale, -1f, 1f);
            output[f * 2] = (short)(l * 32767f);
            output[f * 2 + 1] = (short)(r * 32767f);
        }

        return output;
    }

    private static void AdvanceEnvelope(WtsVoice v)
    {
        switch (v.EnvStage)
        {
            case EnvStage.Attack:
                v.EnvLevel += v.AttackRate;
                if (v.EnvLevel >= 1f)
                {
                    v.EnvLevel = 1f;
                    if (v.HoldSamples > 0)
                        v.EnvStage = EnvStage.Hold;
                    else
                    {
                        v.EnvStage = EnvStage.Decay;
                        if (v.DecayRate <= 0)
                        {
                            v.EnvLevel = v.SustainLevel;
                            v.EnvStage = EnvStage.Sustain;
                        }
                    }
                }
                break;

            case EnvStage.Hold:
                v.HoldSamples--;
                if (v.HoldSamples <= 0)
                {
                    v.EnvStage = EnvStage.Decay;
                    if (v.DecayRate <= 0)
                    {
                        v.EnvLevel = v.SustainLevel;
                        v.EnvStage = EnvStage.Sustain;
                    }
                }
                break;

            case EnvStage.Decay:
                v.EnvLevel -= v.DecayRate;
                if (v.EnvLevel <= v.SustainLevel)
                {
                    v.EnvLevel = v.SustainLevel;
                    v.EnvStage = EnvStage.Sustain;
                }
                break;

            case EnvStage.Sustain:
                // Hold
                break;

            case EnvStage.Release:
                v.EnvLevel -= v.ReleaseRate;
                if (v.EnvLevel <= 0f)
                {
                    v.EnvLevel = 0f;
                    v.Active = false;
                    v.EnvStage = EnvStage.Off;
                }
                break;
        }
    }

    public void AllNotesOff()
    {
        lock (_lock)
        {
            for (int i = 0; i < VoiceCount; i++)
            {
                _voices[i].Active = false;
                _voices[i].EnvStage = EnvStage.Off;
                _voices[i].EnvLevel = 0f;
                _voices[i].Releasing = false;
                _voices[i].Region = null;
            }
        }
    }

    public byte ReadRegister(ushort address)
    {
        // Per-voice registers
        if (address >= VgcConstants.WtsVoiceBase && address <= VgcConstants.WtsVoiceEnd)
        {
            int voice = (address - VgcConstants.WtsVoiceBase) / VgcConstants.WtsVoiceStride;
            int offset = (address - VgcConstants.WtsVoiceBase) % VgcConstants.WtsVoiceStride;
            if (voice < 0 || voice >= VoiceCount) return 0;
            return offset switch
            {
                VgcConstants.WtsVoiceVolume => _voices[voice].Volume,
                VgcConstants.WtsVoicePanning => _voices[voice].Pan,
                VgcConstants.WtsVoiceStatus => (byte)(
                    (_voices[voice].Active ? 0x01 : 0) |
                    (_voices[voice].Releasing ? 0x02 : 0)),
                _ => 0
            };
        }

        // Global registers
        if (address == VgcConstants.WtsReverbLevel) return ReverbLevel;
        if (address == VgcConstants.WtsChorusLevel) return ChorusLevel;
        if (address == VgcConstants.WtsMasterVolume) return MasterVolume;
        if (address == VgcConstants.WtsSoundfontStatus)
            return (byte)(_bank != null ? 1 : 0);
        if (address == VgcConstants.WtsInstrumentCount)
            return (byte)InstrumentCount;
        if (address == VgcConstants.WtsActiveVoices) return ActiveVoiceMask;

        // Instrument enumeration
        if (address == VgcConstants.WtsEnumBank) return _enumBank;
        if (address == VgcConstants.WtsEnumProgram) return _enumProgram;
        if (address >= VgcConstants.WtsEnumName && address <= VgcConstants.WtsEnumEnd)
        {
            int idx = address - VgcConstants.WtsEnumName;
            return idx < _enumName.Length ? (byte)_enumName[idx] : (byte)0;
        }

        return 0;
    }

    public void WriteRegister(ushort address, byte value)
    {
        // Per-voice registers
        if (address >= VgcConstants.WtsVoiceBase && address <= VgcConstants.WtsVoiceEnd)
        {
            int voice = (address - VgcConstants.WtsVoiceBase) / VgcConstants.WtsVoiceStride;
            int offset = (address - VgcConstants.WtsVoiceBase) % VgcConstants.WtsVoiceStride;
            if (voice < 0 || voice >= VoiceCount) return;

            switch (offset)
            {
                case VgcConstants.WtsVoiceNote:
                    if (value > 0)
                        NoteOn(voice, value, _latchVelocity[voice], _latchInstrument[voice]);
                    else
                        NoteOff(voice);
                    break;
                case VgcConstants.WtsVoiceVelocity:
                    _latchVelocity[voice] = value;
                    break;
                case VgcConstants.WtsVoiceInstrument:
                    _latchInstrument[voice] = value;
                    break;
                case VgcConstants.WtsVoiceVolume:
                    SetVolume(voice, value);
                    break;
                case VgcConstants.WtsVoicePanning:
                    SetPanning(voice, value);
                    break;
                case VgcConstants.WtsVoicePitchBendLo:
                    _latchPitchBendLo[voice] = value;
                    SetPitchBend(voice, (ushort)(_latchPitchBendLo[voice] | (_latchPitchBendHi[voice] << 8)));
                    break;
                case VgcConstants.WtsVoicePitchBendHi:
                    _latchPitchBendHi[voice] = value;
                    SetPitchBend(voice, (ushort)(_latchPitchBendLo[voice] | (_latchPitchBendHi[voice] << 8)));
                    break;
            }
            return;
        }

        // Global registers
        if (address == VgcConstants.WtsReverbLevel) { ReverbLevel = value; return; }
        if (address == VgcConstants.WtsChorusLevel) { ChorusLevel = value; return; }
        if (address == VgcConstants.WtsMasterVolume) { MasterVolume = value; return; }
        if (address == VgcConstants.WtsCommand)
        {
            switch (value)
            {
                case VgcConstants.WtsCmdAllNotesOff:
                    AllNotesOff();
                    break;
                case VgcConstants.WtsCmdResetEffects:
                    ReverbLevel = 80;
                    ChorusLevel = 40;
                    MasterVolume = 255;
                    break;
            }
            return;
        }

        // Instrument enumeration
        if (address == VgcConstants.WtsEnumIndex)
        {
            Array.Clear(_enumName);
            _enumBank = 0;
            _enumProgram = 0;

            if (_bank != null && value < _bank.Instruments.Count)
            {
                var inst = _bank.Instruments[value];
                _enumBank = (byte)inst.MidiBank;
                _enumProgram = (byte)inst.MidiProgram;
                string name = inst.Name ?? "";
                int len = Math.Min(name.Length, 28);
                for (int i = 0; i < len; i++)
                    _enumName[i] = (byte)name[i];
                // null terminator already in place from Array.Clear
            }
        }
    }

    public void Dispose()
    {
        AllNotesOff();
        _renderer?.Dispose();
    }

    private enum EnvStage { Attack, Hold, Decay, Sustain, Release, Off }

    private sealed class WtsVoice
    {
        public double Phase;
        public int Note, Velocity, InstrumentIndex;
        public byte Volume = 255;
        public byte Pan = 128;
        public ushort PitchBend = 0x8000;
        public SampleRegion? Region;
        public bool Active, Releasing;
        public float EnvLevel;
        public EnvStage EnvStage = EnvStage.Off;
        public float AttackRate, DecayRate, SustainLevel, ReleaseRate;
        public int HoldSamples;
    }
}
