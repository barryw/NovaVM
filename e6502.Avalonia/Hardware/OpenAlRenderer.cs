using System.Runtime.InteropServices;

namespace e6502.Avalonia.Hardware;

internal sealed class OpenAlRenderer : IDisposable
{
    private const int AlFormatMono16 = 0x1101;
    private const int AlBuffersQueued = 0x1015;
    private const int AlBuffersProcessed = 0x1016;
    private const int AlSourceState = 0x1010;
    private const int AlPlaying = 0x1012;
    private const int AlPosition = 0x1004;
    private const int AlSourceRelative = 0x0202;
    private const int AlRolloffFactor = 0x1021;
    private const int AlDistanceModelNone = 0;
    private const int BufferCount = 8;
    private const int SamplesPerBuffer = 1024;

    // Shared OpenAL device/context — multiple SidChip instances share a single
    // device and context. Each instance gets its own source + buffers.
    // alcMakeContextCurrent is process-global; a second context stomps the first.
    private static readonly object SharedLock = new();
    private static nint _sharedLibHandle;
    private static nint _sharedDevice;
    private static nint _sharedContext;
    private static int _refCount;

    private readonly Func<int, short[]> _sampleProvider;
    private readonly int _sampleRate;
    private readonly bool _stereo;

    // Mono mode: single source. Stereo mode: two mono sources (L + R).
    private readonly uint _source;
    private readonly uint _sourceR;   // only used in stereo mode
    private readonly Queue<uint> _freeBuffers = new();
    private readonly Queue<uint> _freeBuffersR = new(); // only used in stereo mode
    private readonly Thread _thread;
    private volatile bool _running = true;

    private readonly AlcOpenDevice _alcOpenDevice;
    private readonly AlcCloseDevice _alcCloseDevice;
    private readonly AlcCreateContext _alcCreateContext;
    private readonly AlcDestroyContext _alcDestroyContext;
    private readonly AlcMakeContextCurrent _alcMakeContextCurrent;
    private readonly AlGenSources _alGenSources;
    private readonly AlDeleteSources _alDeleteSources;
    private readonly AlGenBuffers _alGenBuffers;
    private readonly AlDeleteBuffers _alDeleteBuffers;
    private readonly AlBufferData _alBufferData;
    private readonly AlSourceQueueBuffers _alSourceQueueBuffers;
    private readonly AlSourceUnqueueBuffers _alSourceUnqueueBuffers;
    private readonly AlGetSourcei _alGetSourcei;
    private readonly AlSourcei _alSourcei;
    private readonly AlSource3f _alSource3f;
    private readonly AlSourcef _alSourcef;
    private readonly AlDistanceModel _alDistanceModel;
    private readonly AlSourcePlay _alSourcePlay;
    private readonly AlSourceStop _alSourceStop;

    public OpenAlRenderer(Func<int, short[]> sampleProvider, int sampleRate, bool stereo = false)
    {
        _sampleProvider = sampleProvider;
        _sampleRate = sampleRate;
        _stereo = stereo;

        lock (SharedLock)
        {
            if (_refCount == 0)
            {
                _sharedLibHandle = LoadOpenAlLibrary();
                var openDev = LoadFn<AlcOpenDevice>(_sharedLibHandle, "alcOpenDevice");
                var createCtx = LoadFn<AlcCreateContext>(_sharedLibHandle, "alcCreateContext");
                var makeCurrent = LoadFn<AlcMakeContextCurrent>(_sharedLibHandle, "alcMakeContextCurrent");

                _sharedDevice = openDev(nint.Zero);
                if (_sharedDevice == nint.Zero)
                    throw new InvalidOperationException("Unable to open OpenAL device.");

                _sharedContext = createCtx(_sharedDevice, nint.Zero);
                if (_sharedContext == nint.Zero || makeCurrent(_sharedContext) == 0)
                    throw new InvalidOperationException("Unable to create OpenAL context.");
            }
            _refCount++;
        }

        _alcOpenDevice = LoadFn<AlcOpenDevice>(_sharedLibHandle, "alcOpenDevice");
        _alcCloseDevice = LoadFn<AlcCloseDevice>(_sharedLibHandle, "alcCloseDevice");
        _alcCreateContext = LoadFn<AlcCreateContext>(_sharedLibHandle, "alcCreateContext");
        _alcDestroyContext = LoadFn<AlcDestroyContext>(_sharedLibHandle, "alcDestroyContext");
        _alcMakeContextCurrent = LoadFn<AlcMakeContextCurrent>(_sharedLibHandle, "alcMakeContextCurrent");
        _alGenSources = LoadFn<AlGenSources>(_sharedLibHandle, "alGenSources");
        _alDeleteSources = LoadFn<AlDeleteSources>(_sharedLibHandle, "alDeleteSources");
        _alGenBuffers = LoadFn<AlGenBuffers>(_sharedLibHandle, "alGenBuffers");
        _alDeleteBuffers = LoadFn<AlDeleteBuffers>(_sharedLibHandle, "alDeleteBuffers");
        _alBufferData = LoadFn<AlBufferData>(_sharedLibHandle, "alBufferData");
        _alSourceQueueBuffers = LoadFn<AlSourceQueueBuffers>(_sharedLibHandle, "alSourceQueueBuffers");
        _alSourceUnqueueBuffers = LoadFn<AlSourceUnqueueBuffers>(_sharedLibHandle, "alSourceUnqueueBuffers");
        _alGetSourcei = LoadFn<AlGetSourcei>(_sharedLibHandle, "alGetSourcei");
        _alSourcei = LoadFn<AlSourcei>(_sharedLibHandle, "alSourcei");
        _alSource3f = LoadFn<AlSource3f>(_sharedLibHandle, "alSource3f");
        _alSourcef = LoadFn<AlSourcef>(_sharedLibHandle, "alSourcef");
        _alDistanceModel = LoadFn<AlDistanceModel>(_sharedLibHandle, "alDistanceModel");
        _alSourcePlay = LoadFn<AlSourcePlay>(_sharedLibHandle, "alSourcePlay");
        _alSourceStop = LoadFn<AlSourceStop>(_sharedLibHandle, "alSourceStop");

        // Create primary source (mono, or left channel for stereo)
        _alGenSources(1, out _source);
        if (_source == 0)
            throw new InvalidOperationException("Unable to allocate OpenAL source.");

        for (int i = 0; i < BufferCount; i++)
        {
            _alGenBuffers(1, out uint buffer);
            if (buffer != 0)
                _freeBuffers.Enqueue(buffer);
        }

        if (_stereo)
        {
            // Create second source for right channel
            _alGenSources(1, out _sourceR);
            if (_sourceR == 0)
                throw new InvalidOperationException("Unable to allocate right OpenAL source.");

            for (int i = 0; i < BufferCount; i++)
            {
                _alGenBuffers(1, out uint buffer);
                if (buffer != 0)
                    _freeBuffersR.Enqueue(buffer);
            }

            // Disable all 3D processing to prevent cave-like reverb
            _alDistanceModel(AlDistanceModelNone);

            // Position sources hard-left and hard-right (listener-relative)
            _alSourcei(_source, AlSourceRelative, 1);
            _alSource3f(_source, AlPosition, -1f, 0f, 0f);
            _alSourcef(_source, AlRolloffFactor, 0f);
            _alSourcei(_sourceR, AlSourceRelative, 1);
            _alSource3f(_sourceR, AlPosition, 1f, 0f, 0f);
            _alSourcef(_sourceR, AlRolloffFactor, 0f);
        }

        if (_freeBuffers.Count == 0)
            throw new InvalidOperationException("Unable to allocate OpenAL buffers.");

        _thread = new Thread(AudioLoop) { IsBackground = true, Name = "OpenAlAudioThread" };
        _thread.Start();
    }

    public void Dispose()
    {
        _running = false;
        _thread.Join(200);

        DisposeSource(_source, _freeBuffers);
        if (_stereo)
            DisposeSource(_sourceR, _freeBuffersR);

        lock (SharedLock)
        {
            _refCount--;
            if (_refCount == 0)
            {
                _alcMakeContextCurrent(nint.Zero);
                _alcDestroyContext(_sharedContext);
                _alcCloseDevice(_sharedDevice);
                NativeLibrary.Free(_sharedLibHandle);
                _sharedDevice = nint.Zero;
                _sharedContext = nint.Zero;
                _sharedLibHandle = nint.Zero;
            }
        }
    }

    private void DisposeSource(uint source, Queue<uint> freeBuffers)
    {
        _alSourceStop(source);

        _alGetSourcei(source, AlBuffersQueued, out int queued);
        for (int i = 0; i < queued; i++)
        {
            _alSourceUnqueueBuffers(source, 1, out uint buffer);
            if (buffer != 0)
                freeBuffers.Enqueue(buffer);
        }

        while (freeBuffers.Count > 0)
        {
            uint buffer = freeBuffers.Dequeue();
            _alDeleteBuffers(1, ref buffer);
        }

        uint src = source;
        _alDeleteSources(1, ref src);
    }

    private void AudioLoop()
    {
        while (_running)
        {
            try
            {
                if (_stereo)
                    AudioLoopStereo();
                else
                    AudioLoopMono();
                Thread.Sleep(4);
            }
            catch
            {
                Thread.Sleep(16);
            }
        }
    }

    private void AudioLoopMono()
    {
        ReclaimProcessedBuffers(_source, _freeBuffers);

        _alGetSourcei(_source, AlBuffersQueued, out int queued);
        while (queued < 4 && _freeBuffers.Count > 0)
        {
            uint buffer = _freeBuffers.Dequeue();
            short[] samples = _sampleProvider(SamplesPerBuffer);
            QueueMonoBuffer(buffer, samples, _source);
            queued++;
        }

        EnsurePlaying(_source);
    }

    private void AudioLoopStereo()
    {
        ReclaimProcessedBuffers(_source, _freeBuffers);
        ReclaimProcessedBuffers(_sourceR, _freeBuffersR);

        _alGetSourcei(_source, AlBuffersQueued, out int queuedL);
        _alGetSourcei(_sourceR, AlBuffersQueued, out int queuedR);
        int queued = Math.Min(queuedL, queuedR);

        while (queued < 4 && _freeBuffers.Count > 0 && _freeBuffersR.Count > 0)
        {
            // Get interleaved stereo samples from provider
            short[] stereo = _sampleProvider(SamplesPerBuffer);

            // Deinterleave into L and R mono buffers
            short[] left = new short[SamplesPerBuffer];
            short[] right = new short[SamplesPerBuffer];
            for (int i = 0; i < SamplesPerBuffer; i++)
            {
                left[i] = stereo[i * 2];
                right[i] = stereo[i * 2 + 1];
            }

            uint bufL = _freeBuffers.Dequeue();
            uint bufR = _freeBuffersR.Dequeue();
            QueueMonoBuffer(bufL, left, _source);
            QueueMonoBuffer(bufR, right, _sourceR);
            queued++;
        }

        EnsurePlaying(_source);
        EnsurePlaying(_sourceR);
    }

    private void QueueMonoBuffer(uint buffer, short[] samples, uint source)
    {
        int byteSize = samples.Length * sizeof(short);
        var handle = GCHandle.Alloc(samples, GCHandleType.Pinned);
        try
        {
            _alBufferData(buffer, AlFormatMono16, handle.AddrOfPinnedObject(), byteSize, _sampleRate);
        }
        finally
        {
            handle.Free();
        }
        _alSourceQueueBuffers(source, 1, ref buffer);
    }

    private void ReclaimProcessedBuffers(uint source, Queue<uint> freeBuffers)
    {
        _alGetSourcei(source, AlBuffersProcessed, out int processed);
        for (int i = 0; i < processed; i++)
        {
            _alSourceUnqueueBuffers(source, 1, out uint buffer);
            if (buffer != 0)
                freeBuffers.Enqueue(buffer);
        }
    }

    private void EnsurePlaying(uint source)
    {
        _alGetSourcei(source, AlSourceState, out int state);
        if (state != AlPlaying)
            _alSourcePlay(source);
    }

    private static T LoadFn<T>(nint libHandle, string symbol) where T : Delegate
    {
        nint ptr = NativeLibrary.GetExport(libHandle, symbol);
        return Marshal.GetDelegateForFunctionPointer<T>(ptr);
    }

    private static nint LoadOpenAlLibrary()
    {
        string[] names =
        [
            "openal32.dll",
            "libopenal.so.1",
            "libopenal.so",
            "/opt/homebrew/opt/openal-soft/lib/libopenal.dylib",    // OpenAL Soft (homebrew ARM)
            "/usr/local/opt/openal-soft/lib/libopenal.dylib",       // OpenAL Soft (homebrew Intel)
            "/System/Library/Frameworks/OpenAL.framework/OpenAL"    // Apple fallback
        ];

        foreach (string name in names)
        {
            if (NativeLibrary.TryLoad(name, out nint handle))
                return handle;
        }

        throw new DllNotFoundException("OpenAL library not found.");
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate nint AlcOpenDevice(nint deviceName);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate byte AlcCloseDevice(nint device);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate nint AlcCreateContext(nint device, nint attrList);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlcDestroyContext(nint context);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate byte AlcMakeContextCurrent(nint context);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlGenSources(int n, out uint sources);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlDeleteSources(int n, ref uint sources);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlGenBuffers(int n, out uint buffers);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlDeleteBuffers(int n, ref uint buffers);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlBufferData(uint buffer, int format, nint data, int size, int freq);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourceQueueBuffers(uint source, int n, ref uint buffers);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourceUnqueueBuffers(uint source, int n, out uint buffers);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlGetSourcei(uint source, int param, out int value);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourcei(uint source, int param, int value);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlDistanceModel(int model);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourcef(uint source, int param, float value);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSource3f(uint source, int param, float v1, float v2, float v3);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourcePlay(uint source);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourceStop(uint source);
}
