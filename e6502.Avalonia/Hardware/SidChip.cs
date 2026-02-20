// MOS 6581 SID chip emulation
// Ported from cadaver's (Lasse Oorni) SID.cs — MIT License
// https://github.com/cadaver/oldschoolengine2/blob/master/Assets/Scripts/SID.cs
//
// Waveform generation and filter implementation based on jsSID
// jsSID by Hermit (Mihaly Horvath) : a javascript SID emulator and player for the Web Audio API
// (Year 2016) http://hermit.sidrip.com

namespace e6502.Avalonia.Hardware;

public sealed class SidChip : IDisposable
{
    public const ushort BaseAddress = 0xD400;
    public const ushort EndAddress = 0xD41C;
    private const int RegisterCount = EndAddress - BaseAddress + 1; // 29 bytes
    private const int SampleRate = 44100;
    private const double CpuClockRate = 985248.0; // PAL C64

    private readonly byte[] _registers = new byte[RegisterCount];
    private readonly SidChannel[] _channels = new SidChannel[3];
    private readonly object _lock = new();
    private readonly OpenAlRenderer? _renderer;

    private readonly float _cyclesPerSample;
    private float _cycleAccumulator;
    private float _prevBandPass;
    private float _prevLowPass;

    // Accumulated samples from Clock() calls, consumed by RenderSamples
    private readonly Queue<float> _sampleBuffer = new();

    public SidChip(bool enableAudio = false)
    {
        for (int i = 0; i < 3; i++)
            _channels[i] = new SidChannel();

        _channels[0].SyncTarget = _channels[1];
        _channels[1].SyncTarget = _channels[2];
        _channels[2].SyncTarget = _channels[0];
        _channels[0].SyncSource = _channels[2];
        _channels[1].SyncSource = _channels[0];
        _channels[2].SyncSource = _channels[1];

        _cyclesPerSample = (float)(CpuClockRate / SampleRate);

        if (enableAudio)
        {
            try
            {
                _renderer = new OpenAlRenderer(RenderSamples, SampleRate);
            }
            catch
            {
                _renderer = null;
            }
        }
    }

    public bool OwnsAddress(ushort address) =>
        address >= BaseAddress && address <= EndAddress;

    public byte Read(ushort address) =>
        _registers[address - BaseAddress];

    public void Write(ushort address, byte data) =>
        _registers[address - BaseAddress] = data;

    /// <summary>
    /// Advance the SID by the given number of CPU cycles, producing audio samples.
    /// </summary>
    public void Clock(int cycles)
    {
        if (cycles <= 0) return;

        lock (_lock)
        {
            ReadRegistersIntoChannels();
            float masterVol = (_registers[0x18] & 0xF) / 22.5f;
            byte filterSelect = (byte)(_registers[0x18] & 0x70);
            byte filterCtrl = _registers[0x17];

            float cutoff = 0.05f + 0.85f * (MathF.Sin((_registers[0x16] / 255.0f - 0.5f) * MathF.PI) * 0.5f + 0.5f);
            cutoff = MathF.Pow(cutoff, 1.3f);
            float resonance = (_registers[0x17] > 0x3F) ? 7.0f / (_registers[0x17] >> 4) : 1.75f;

            int remaining = cycles;
            while (remaining > 0)
            {
                int cyclesToRun = Math.Min(remaining, (int)MathF.Ceiling(_cyclesPerSample - _cycleAccumulator));

                for (int j = 0; j < 3; j++)
                    _channels[j].Clock(cyclesToRun);
                for (int j = 0; j < 3; j++)
                {
                    if (_channels[j].DoSync && (_channels[j].SyncTarget.Waveform & 0x2) != 0)
                        _channels[j].SyncTarget.ResetAccumulator();
                }

                _cycleAccumulator += cyclesToRun;

                if (_cycleAccumulator >= _cyclesPerSample)
                {
                    _cycleAccumulator -= _cyclesPerSample;
                    float sample = MixSample(filterCtrl, filterSelect, cutoff, resonance, masterVol);
                    _sampleBuffer.Enqueue(sample);
                }

                remaining -= cyclesToRun;
            }
        }
    }

    /// <summary>
    /// Render samples for the audio callback (OpenAlRenderer).
    /// Drains the internal sample buffer, padding with silence if needed.
    /// </summary>
    private short[] RenderSamples(int count)
    {
        var pcm = new short[count];

        lock (_lock)
        {
            int available = Math.Min(count, _sampleBuffer.Count);
            for (int i = 0; i < available; i++)
            {
                float clamped = Math.Clamp(_sampleBuffer.Dequeue(), -1f, 1f);
                pcm[i] = (short)(clamped * short.MaxValue);
            }
            // Remaining samples stay at 0 (silence)
        }

        return pcm;
    }

    /// <summary>
    /// Test-only: render samples by clocking internally and returning PCM16.
    /// Does not require OpenAL.
    /// </summary>
    internal short[] RenderSamplesForTest(int count)
    {
        var pcm = new short[count];

        lock (_lock)
        {
            ReadRegistersIntoChannels();
            float masterVol = (_registers[0x18] & 0xF) / 22.5f;
            byte filterSelect = (byte)(_registers[0x18] & 0x70);
            byte filterCtrl = _registers[0x17];

            float cutoff = 0.05f + 0.85f * (MathF.Sin((_registers[0x16] / 255.0f - 0.5f) * MathF.PI) * 0.5f + 0.5f);
            cutoff = MathF.Pow(cutoff, 1.3f);
            float resonance = (_registers[0x17] > 0x3F) ? 7.0f / (_registers[0x17] >> 4) : 1.75f;

            for (int i = 0; i < count; i++)
            {
                int cyclesToRun = (int)MathF.Ceiling(_cyclesPerSample);

                for (int j = 0; j < 3; j++)
                    _channels[j].Clock(cyclesToRun);
                for (int j = 0; j < 3; j++)
                {
                    if (_channels[j].DoSync && (_channels[j].SyncTarget.Waveform & 0x2) != 0)
                        _channels[j].SyncTarget.ResetAccumulator();
                }

                float sample = MixSample(filterCtrl, filterSelect, cutoff, resonance, masterVol);
                float clamped = Math.Clamp(sample, -1f, 1f);
                pcm[i] = (short)(clamped * short.MaxValue);
            }
        }

        return pcm;
    }

    public void Dispose() =>
        _renderer?.Dispose();

    private void ReadRegistersIntoChannels()
    {
        for (int i = 0; i < 3; i++)
        {
            int ofs = i * 7;
            _channels[i].Frequency = (ushort)(_registers[ofs] | (_registers[ofs + 1] << 8));
            _channels[i].Pulse = (ushort)(_registers[ofs + 2] | (_registers[ofs + 3] << 8));
            _channels[i].Waveform = _registers[ofs + 4];
            _channels[i].Ad = _registers[ofs + 5];
            _channels[i].Sr = _registers[ofs + 6];
        }
    }

    private float MixSample(byte filterCtrl, byte filterSelect, float cutoff, float resonance, float masterVol)
    {
        float output = 0f;
        float filterInput = 0f;

        if ((filterCtrl & 1) == 0)
            output += _channels[0].GetOutput();
        else
            filterInput += _channels[0].GetOutput();

        if ((filterCtrl & 2) == 0)
            output += _channels[1].GetOutput();
        else
            filterInput += _channels[1].GetOutput();

        if ((filterCtrl & 4) == 0)
            output += _channels[2].GetOutput();
        else
            filterInput += _channels[2].GetOutput();

        // Highpass
        float temp = filterInput + _prevBandPass * resonance + _prevLowPass;
        if ((filterSelect & 0x40) != 0)
            output -= temp;
        // Bandpass
        temp = _prevBandPass - temp * cutoff;
        _prevBandPass = temp;
        if ((filterSelect & 0x20) != 0)
            output -= temp;
        // Lowpass
        temp = _prevLowPass + temp * cutoff;
        _prevLowPass = temp;
        if ((filterSelect & 0x10) != 0)
            output += temp;

        output *= masterVol;
        return output;
    }

    // -------------------------------------------------------------------------
    // SID Channel — one of 3 voices
    // -------------------------------------------------------------------------

    private sealed class SidChannel
    {
        public SidChannel SyncTarget = null!;
        public SidChannel SyncSource = null!;

        public ushort Frequency;
        public byte Ad;
        public byte Sr;
        public ushort Pulse;
        public byte Waveform;
        public bool DoSync;

        private enum AdsrState { Attack, Decay, Release }

        private AdsrState _state = AdsrState.Release;
        private uint _accumulator;
        private uint _noiseGenerator = 0x7FFFF8;
        private ushort _adsrCounter;
        private byte _adsrExpCounter;
        private byte _volumeLevel;

        private static readonly ushort[] AdsrRateTable =
            { 9, 32, 63, 95, 149, 220, 267, 313, 392, 977, 1954, 3126, 3907, 11720, 19532, 31251 };

        private static readonly byte[] SustainLevels =
            { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

        private static readonly byte[] ExpTargetTable =
        {
            1,30,30,30,30,30,16,16,16,16,16,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
        };

        public void Clock(int cycles)
        {
            if ((Waveform & 0x1) != 0)
            {
                if (_state == AdsrState.Release)
                    _state = AdsrState.Attack;
            }
            else
            {
                _state = AdsrState.Release;
            }

            int adsrCycles = cycles;
            while (adsrCycles > 0)
            {
                ushort adsrTarget = _state switch
                {
                    AdsrState.Attack => AdsrRateTable[Ad >> 4],
                    AdsrState.Decay => AdsrRateTable[Ad & 0xF],
                    _ => AdsrRateTable[Sr & 0xF]
                };

                int adsrCyclesNow = Math.Min(adsrCycles,
                    _adsrCounter < adsrTarget
                        ? adsrTarget - _adsrCounter
                        : 0x8000 + adsrTarget - _adsrCounter);

                _adsrCounter += (ushort)adsrCyclesNow;
                _adsrCounter &= 0x7FFF;

                if (_adsrCounter == adsrTarget)
                {
                    _adsrCounter = 0;

                    switch (_state)
                    {
                        case AdsrState.Attack:
                            _adsrExpCounter = 0;
                            ++_volumeLevel;
                            if (_volumeLevel == 0xFF)
                                _state = AdsrState.Decay;
                            break;

                        case AdsrState.Decay:
                        {
                            byte adsrExpTarget = _volumeLevel < 0x5D ? ExpTargetTable[_volumeLevel] : (byte)1;
                            ++_adsrExpCounter;
                            if (_adsrExpCounter >= adsrExpTarget)
                            {
                                _adsrExpCounter = 0;
                                if (_volumeLevel > SustainLevels[Sr >> 4])
                                    --_volumeLevel;
                            }
                            break;
                        }

                        case AdsrState.Release:
                        {
                            if (_volumeLevel > 0)
                            {
                                byte adsrExpTarget = _volumeLevel < 0x5D ? ExpTargetTable[_volumeLevel] : (byte)1;
                                ++_adsrExpCounter;
                                if (_adsrExpCounter >= adsrExpTarget)
                                {
                                    _adsrExpCounter = 0;
                                    --_volumeLevel;
                                }
                            }
                            break;
                        }
                    }
                }

                adsrCycles -= adsrCyclesNow;
            }

            // Test bit
            if ((Waveform & 0x8) != 0)
            {
                _accumulator = 0;
                return;
            }

            // If frequency 0, no-op
            if (Frequency == 0)
                return;

            // Fast path: no noise and no sync target
            if ((Waveform & 0x80) == 0 && (SyncTarget.Waveform & 0x2) == 0)
            {
                _accumulator += Frequency * (uint)cycles;
                _accumulator &= 0xFFFFFF;
            }
            else
            {
                int accumulatorCycles = cycles;

                while (accumulatorCycles > 0)
                {
                    int accumulatorCyclesNow = accumulatorCycles;

                    if ((Waveform & 0x80) != 0)
                    {
                        if ((_accumulator & 0xFFFFF) < 0x80000)
                            accumulatorCyclesNow = Math.Min(accumulatorCyclesNow,
                                (int)(0x80000 - (_accumulator & 0xFFFFF)) / Frequency + 1);
                        else
                            accumulatorCyclesNow = Math.Min(accumulatorCyclesNow,
                                (int)(0x180000 - (_accumulator & 0xFFFFF)) / Frequency + 1);
                    }

                    if ((SyncTarget.Waveform & 0x2) != 0)
                    {
                        if (_accumulator < 0x800000)
                            accumulatorCyclesNow = Math.Min(accumulatorCyclesNow,
                                (int)(0x800000 - _accumulator) / Frequency + 1);
                        else
                            accumulatorCyclesNow = Math.Min(accumulatorCyclesNow,
                                (int)(0x1800000 - _accumulator) / Frequency + 1);
                    }

                    uint lastAccumulator = _accumulator;

                    _accumulator += Frequency * (uint)accumulatorCyclesNow;
                    _accumulator &= 0xFFFFFF;

                    if ((Waveform & 0x80) != 0 &&
                        (lastAccumulator & 0x80000) == 0 &&
                        (_accumulator & 0x80000) != 0)
                    {
                        uint temp = _noiseGenerator;
                        uint step = (temp & 0x400000) ^ ((temp & 0x20000) << 5);
                        temp <<= 1;
                        if (step > 0) temp |= 1;
                        _noiseGenerator = temp & 0x7FFFFF;
                    }

                    // Sync
                    DoSync = (lastAccumulator & 0x800000) == 0 && (_accumulator & 0x800000) != 0;

                    accumulatorCycles -= accumulatorCyclesNow;
                }
            }
        }

        public void ResetAccumulator() => _accumulator = 0;

        public float GetOutput()
        {
            if (_volumeLevel == 0)
                return 0f;

            uint waveOutput = 0;

            switch (Waveform & 0xF0)
            {
                case 0x10:
                    waveOutput = Triangle();
                    break;
                case 0x20:
                    waveOutput = Sawtooth();
                    break;
                case 0x40:
                    waveOutput = PulseWave();
                    break;
                case 0x50:
                {
                    uint tri = Triangle();
                    uint pul = PulseWave();
                    waveOutput = ((pul & tri & (tri >> 1)) & (tri << 1)) << 1;
                    if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                    break;
                }
                case 0x60:
                {
                    uint saw = Sawtooth();
                    uint pul = PulseWave();
                    waveOutput = ((pul & saw & (saw >> 1)) & (saw << 1)) << 1;
                    if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                    break;
                }
                case 0x70:
                {
                    uint triSaw = Triangle() & Sawtooth();
                    uint pul = PulseWave();
                    waveOutput = ((pul & triSaw & (triSaw >> 1)) & (triSaw << 1)) << 1;
                    if (waveOutput > 0xFFFF) waveOutput = 0xFFFF;
                    break;
                }
                case 0x80:
                    waveOutput = Noise();
                    break;
            }

            return ((int)waveOutput - 0x8000) * _volumeLevel / 16777216f;
        }

        private uint Triangle()
        {
            uint temp = _accumulator ^ ((Waveform & 0x4) != 0 ? SyncSource._accumulator : 0);
            return ((temp >= 0x800000 ? (_accumulator ^ 0xFFFFFF) : _accumulator) >> 7) & 0xFFFF;
        }

        private uint Sawtooth() => _accumulator >> 8;

        private uint PulseWave() =>
            (uint)((_accumulator >> 12) >= (Pulse & 0xFFF) ? 0xFFFF : 0x0);

        private uint Noise() =>
            ((_noiseGenerator & 0x100000) >> 5) +
            ((_noiseGenerator & 0x40000) >> 4) +
            ((_noiseGenerator & 0x4000) >> 1) +
            ((_noiseGenerator & 0x800) << 1) +
            ((_noiseGenerator & 0x200) << 2) +
            ((_noiseGenerator & 0x20) << 5) +
            ((_noiseGenerator & 0x04) << 7) +
            ((_noiseGenerator & 0x01) << 8);
    }
}
