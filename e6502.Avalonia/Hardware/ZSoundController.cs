namespace e6502.Avalonia.Hardware;

/// <summary>
/// Plays Z-machine sampled sounds (sound_effect number >= 3) from a decoded
/// SOUND.PAK. Completion is cycle-driven (deterministic in the headless smoke,
/// real-time in the GUI, and portable to the ESP32) so the V5 finish-routine
/// callback fires reliably. Actual audio output via OpenAL is best-effort and
/// only attempted when sound is enabled.
///
/// SOUND.PAK: "NZSP" ver(1) count(1); index[count]: number(1) rate(2 LE)
/// offset(4 LE) len(4 LE); then concatenated signed 8-bit mono PCM.
/// </summary>
public sealed class ZSoundController : IDisposable
{
    // Matches the NovaZ runtime's 12 MHz timing assumption so a sound's
    // duration in CPU cycles lines up with the runtime's sense of time.
    private const int CpuHz = 12_000_000;
    private const int OutRate = 22050;

    private readonly bool _enableSound;
    private readonly Dictionary<int, Sample> _samples = new();
    private bool _packLoaded;

    private readonly object _lock = new();
    private byte[]? _curPcm;
    private int _curRate;
    private double _pos;
    private float _gain = 1f;
    private bool _loop;
    private bool _playing;

    private long _remainingCycles;   // CPU-thread only; 0 = idle or looping
    private bool _finished;          // sticky until read via ReadStatus

    public int LastPlayed { get; private set; }
    public int PlayCount { get; private set; }

    private OpenAlRenderer? _renderer;

    private readonly record struct Sample(int Rate, byte[] Pcm);

    public ZSoundController(bool enableSound) => _enableSound = enableSound;

    public bool PackLoaded => _packLoaded;

    public void LoadPack(byte[] pak)
    {
        _samples.Clear();
        _packLoaded = false;
        if (pak.Length < 6 || pak[0] != 'N' || pak[1] != 'Z' || pak[2] != 'S' || pak[3] != 'P')
            return;
        int count = pak[5];
        int headerSize = 6 + count * 11;
        for (int i = 0; i < count; i++)
        {
            int o = 6 + i * 11;
            int num = pak[o];
            int rate = pak[o + 1] | (pak[o + 2] << 8);
            int off = pak[o + 3] | (pak[o + 4] << 8) | (pak[o + 5] << 16) | (pak[o + 6] << 24);
            int len = pak[o + 7] | (pak[o + 8] << 8) | (pak[o + 9] << 16) | (pak[o + 10] << 24);
            if (headerSize + off + len > pak.Length)
                continue;
            var pcm = new byte[len];
            Array.Copy(pak, headerSize + off, pcm, 0, len);
            _samples[num] = new Sample(rate <= 0 ? 8000 : rate, pcm);
        }
        _packLoaded = true;
        EnsureRenderer();
    }

    public void Play(int number, float gain, bool loop)
    {
        LastPlayed = number;
        PlayCount++;
        if (!_samples.TryGetValue(number, out var s))
        {
            // Unknown sample: finish immediately so a V5 callback still fires.
            lock (_lock) { _playing = false; _curPcm = null; }
            _remainingCycles = 0;
            _finished = true;
            return;
        }
        lock (_lock)
        {
            _curPcm = s.Pcm;
            _curRate = s.Rate;
            _pos = 0;
            _gain = gain;
            _loop = loop;
            _playing = true;
        }
        _remainingCycles = loop ? 0 : (long)((double)s.Pcm.Length / s.Rate * CpuHz);
        _finished = false;
        EnsureRenderer();
    }

    public void Stop()
    {
        lock (_lock) { _playing = false; _curPcm = null; }
        _remainingCycles = 0;
    }

    public void AdvanceCycles(int cycles)
    {
        if (_remainingCycles <= 0) return;
        _remainingCycles -= cycles;
        if (_remainingCycles <= 0)
        {
            _remainingCycles = 0;
            _finished = true;
            lock (_lock) { _playing = false; }
        }
    }

    /// <summary>$BA76: bit0 = a non-looping sound finished since the last read
    /// (clears on read), bit1 = a sound is currently playing.</summary>
    public byte ReadStatus()
    {
        byte v = 0;
        if (_finished) { v |= 0x01; _finished = false; }
        lock (_lock) { if (_playing) v |= 0x02; }
        return v;
    }

    public void Reset()
    {
        Stop();
        _finished = false;
        LastPlayed = 0;
        PlayCount = 0;
    }

    private void EnsureRenderer()
    {
        if (!_enableSound || _renderer != null) return;
        try { _renderer = new OpenAlRenderer(Provide, OutRate, stereo: false); }
        catch { _renderer = null; }
    }

    // OpenAL pull callback (audio thread). Streams the active sound, resampled
    // from its native rate to OutRate, 8-bit signed -> 16-bit, scaled by gain.
    private short[] Provide(int frames)
    {
        var buf = new short[frames];
        lock (_lock)
        {
            if (!_playing || _curPcm == null)
                return buf;
            double step = (double)_curRate / OutRate;
            for (int i = 0; i < frames; i++)
            {
                int idx = (int)_pos;
                if (idx >= _curPcm.Length)
                {
                    if (_loop) { _pos = 0; idx = 0; }
                    else { _playing = false; break; }
                }
                sbyte sample = (sbyte)_curPcm[idx];
                buf[i] = (short)(sample * 256 * _gain);
                _pos += step;
            }
        }
        return buf;
    }

    public void Dispose() => _renderer?.Dispose();
}
