// Browser-compatible drop-in replacement for the native OpenAL renderer.
// Same class name + constructor signature so linked SidChip.cs and WavetableSynth.cs
// compile without changes. Uses Web Audio API via JS interop.

using System.Runtime.InteropServices.JavaScript;

namespace e6502.Avalonia.Hardware;

internal sealed partial class OpenAlRenderer : IDisposable
{
    private static readonly List<OpenAlRenderer> _instances = new();
    private static bool _audioInitialized;
    private static System.Threading.Timer? _pumpTimer;

    private readonly Func<int, short[]> _sampleProvider;
    private readonly int _sampleRate;
    private readonly bool _stereo;
    private readonly int _instanceId;
    private bool _disposed;

    public OpenAlRenderer(Func<int, short[]> sampleProvider, int sampleRate, bool stereo = false)
    {
        _sampleProvider = sampleProvider;
        _sampleRate = sampleRate;
        _stereo = stereo;

        lock (_instances)
        {
            _instanceId = _instances.Count;
            _instances.Add(this);
        }

        if (!_audioInitialized)
        {
            _audioInitialized = true;
            _pumpTimer = new System.Threading.Timer(_ => PumpAllAudio(), null, 500, 20);
        }
    }

    [JSExport]
    public static double[] RenderMixedAudio(int sampleCount)
    {
        double[] output = new double[sampleCount * 2];

        lock (_instances)
        {
            foreach (var instance in _instances)
            {
                if (instance._disposed) continue;

                try
                {
                    short[] raw = instance._sampleProvider(sampleCount);
                    if (instance._stereo)
                    {
                        int count = Math.Min(sampleCount * 2, raw.Length);
                        for (int i = 0; i < count; i++)
                            output[i] += raw[i] / 32768.0;
                    }
                    else
                    {
                        int count = Math.Min(sampleCount, raw.Length);
                        for (int i = 0; i < count; i++)
                        {
                            double sample = raw[i] / 32768.0;
                            output[i * 2] += sample;
                            output[i * 2 + 1] += sample;
                        }
                    }
                }
                catch
                {
                    // Don't let one broken renderer kill all audio
                }
            }
        }

        for (int i = 0; i < output.Length; i++)
            output[i] = Math.Clamp(output[i], -1.0, 1.0);

        return output;
    }

    private static void PumpAllAudio()
    {
        try
        {
            WebAudioInterop.PumpAudio();
        }
        catch
        {
            // Audio not initialized yet
        }
    }

    public void Dispose()
    {
        _disposed = true;
    }

    // JS interop — must be in a partial class for source generation
    internal static partial class WebAudioInterop
    {
        [JSImport("globalThis.novaAudio.init")]
        internal static partial void Init(int sampleRate);

        [JSImport("globalThis.novaAudio.resume")]
        internal static partial void Resume();

        [JSImport("globalThis.novaAudio.pump")]
        internal static partial void PumpAudio();
    }

    public static void InitializeWebAudio(int sampleRate = 44100)
    {
        try { WebAudioInterop.Init(sampleRate); }
        catch { /* JS not loaded yet */ }
    }

    public static void ResumeWebAudio()
    {
        try { WebAudioInterop.Resume(); }
        catch { /* ignore */ }
    }
}
