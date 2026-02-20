namespace e6502.Avalonia.Hardware;

public sealed class VirtualSoundController : IDisposable
{
    private const int ChannelCount = 4;
    private const int SampleRate = 44100;
    private readonly byte[] _registers = new byte[VgcConstants.VscEnd - VgcConstants.VscBase + 1];
    private readonly ChannelState[] _channels =
    [
        new(), new(), new(), new()
    ];
    private readonly object _lock = new();
    private readonly OpenAlRenderer? _renderer;
    private byte _masterVolume = 12;

    public VirtualSoundController(bool enableAudio = false)
    {
        _registers[Reg(VgcConstants.VscMasterVol)] = _masterVolume;

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
        address >= VgcConstants.VscBase && address <= VgcConstants.VscEnd;

    public byte Read(ushort address) =>
        _registers[address - VgcConstants.VscBase];

    public void Write(ushort address, byte data)
    {
        _registers[address - VgcConstants.VscBase] = data;

        if (address == VgcConstants.VscCmd)
            ExecuteCommand(data);
    }

    public void Dispose() =>
        _renderer?.Dispose();

    private static int Reg(int absoluteAddr) =>
        absoluteAddr - VgcConstants.VscBase;

    private void ExecuteCommand(byte cmd)
    {
        lock (_lock)
        {
            switch (cmd)
            {
                case VgcConstants.VscCmdSound:
                    DoSound();
                    break;
                case VgcConstants.VscCmdVolume:
                    DoVolume();
                    break;
                case VgcConstants.VscCmdEnvelope:
                    DoEnvelope();
                    break;
                case VgcConstants.VscCmdWave:
                    DoWave();
                    break;
            }

            UpdateActiveMaskNoLock();
        }
    }

    private void DoSound()
    {
        int ch = _registers[Reg(VgcConstants.VscP0)] & (ChannelCount - 1);
        int freq = _registers[Reg(VgcConstants.VscP1)] | (_registers[Reg(VgcConstants.VscP2)] << 8);
        int durationTicks = _registers[Reg(VgcConstants.VscP3)] | (_registers[Reg(VgcConstants.VscP4)] << 8);

        var channel = _channels[ch];
        if (freq <= 0 || durationTicks <= 0)
        {
            channel.Active = false;
            channel.NotePos = 0;
            channel.NoteTotalSamples = 0;
            return;
        }

        channel.Frequency = Math.Clamp(freq, 16, 12000);
        channel.NoteTotalSamples = Math.Max(1, durationTicks * SampleRate / 60);
        channel.NotePos = 0;
        channel.Phase = 0.0;
        channel.Active = true;
    }

    private void DoVolume()
    {
        _masterVolume = (byte)(_registers[Reg(VgcConstants.VscP0)] & 0x0F);
        _registers[Reg(VgcConstants.VscMasterVol)] = _masterVolume;
    }

    private void DoEnvelope()
    {
        int ch = _registers[Reg(VgcConstants.VscP0)] & (ChannelCount - 1);
        var channel = _channels[ch];

        channel.AttackSamples = EnvelopeParamToSamples(_registers[Reg(VgcConstants.VscP1)]);
        channel.DecaySamples = EnvelopeParamToSamples(_registers[Reg(VgcConstants.VscP2)]);
        channel.SustainLevel = (_registers[Reg(VgcConstants.VscP3)] & 0x0F) / 15.0;
        channel.ReleaseSamples = EnvelopeParamToSamples(_registers[Reg(VgcConstants.VscP4)]);
    }

    private void DoWave()
    {
        int ch = _registers[Reg(VgcConstants.VscP0)] & (ChannelCount - 1);
        _channels[ch].Waveform = _registers[Reg(VgcConstants.VscP1)] % 5;
    }

    private static int EnvelopeParamToSamples(byte value) =>
        Math.Max(1, value * SampleRate / 240);

    private short[] RenderSamples(int sampleCount)
    {
        var pcm = new short[sampleCount];

        lock (_lock)
        {
            double masterGain = (_masterVolume / 15.0) * 0.28;

            for (int i = 0; i < sampleCount; i++)
            {
                double mix = 0.0;
                for (int ch = 0; ch < ChannelCount; ch++)
                    mix += NextChannelSample(_channels[ch]);

                double outSample = Math.Clamp(mix * masterGain, -1.0, 1.0);
                pcm[i] = (short)(outSample * short.MaxValue);
            }

            UpdateActiveMaskNoLock();
        }

        return pcm;
    }

    private static double NextChannelSample(ChannelState channel)
    {
        if (!channel.Active)
            return 0.0;

        if (channel.NotePos >= channel.NoteTotalSamples)
        {
            channel.Active = false;
            return 0.0;
        }

        double env = EnvelopeAt(channel);
        double wave = WaveAt(channel);

        channel.NotePos++;
        channel.Phase += channel.Frequency / (double)SampleRate;
        channel.Phase -= Math.Floor(channel.Phase);

        return wave * env;
    }

    private static double EnvelopeAt(ChannelState channel)
    {
        int t = channel.NotePos;
        int total = Math.Max(1, channel.NoteTotalSamples);
        int attack = channel.AttackSamples;
        int decay = channel.DecaySamples;
        int release = channel.ReleaseSamples;
        double sustain = channel.SustainLevel;

        if (t < attack)
            return t / (double)Math.Max(1, attack);

        if (t < attack + decay)
        {
            double k = (t - attack) / (double)Math.Max(1, decay);
            return 1.0 - (1.0 - sustain) * k;
        }

        int releaseStart = Math.Max(attack + decay, total - release);
        if (t < releaseStart)
            return sustain;

        double releaseLen = Math.Max(1, total - releaseStart);
        double r = (t - releaseStart) / releaseLen;
        return sustain * (1.0 - r);
    }

    private static double WaveAt(ChannelState channel)
    {
        double p = channel.Phase;

        return channel.Waveform switch
        {
            0 => p < 0.5 ? 1.0 : -1.0,                      // square
            1 => (2.0 * p) - 1.0,                           // saw
            2 => 1.0 - 4.0 * Math.Abs(p - 0.5),            // triangle
            3 => channel.NextNoiseSample(),                 // noise
            4 => Math.Sin(p * Math.PI * 2.0),              // sine
            _ => p < 0.5 ? 1.0 : -1.0
        };
    }

    private void UpdateActiveMaskNoLock()
    {
        byte mask = 0;
        for (int i = 0; i < ChannelCount; i++)
            if (_channels[i].Active)
                mask |= (byte)(1 << i);

        _registers[Reg(VgcConstants.VscActiveMask)] = mask;
    }

    private sealed class ChannelState
    {
        public int Frequency = 440;
        public int Waveform; // 0=square, 1=saw, 2=triangle, 3=noise, 4=sine
        public bool Active;
        public int NoteTotalSamples;
        public int NotePos;
        public int AttackSamples = SampleRate / 120;
        public int DecaySamples = SampleRate / 80;
        public int ReleaseSamples = SampleRate / 80;
        public double SustainLevel = 0.65;
        public double Phase;
        private uint _noiseState = 0x1234ABCD;

        public double NextNoiseSample()
        {
            _noiseState = 1664525u * _noiseState + 1013904223u;
            return ((_noiseState >> 31) & 1) == 1 ? 1.0 : -1.0;
        }
    }

}
