using System.Runtime.InteropServices;

namespace e6502.Avalonia.Hardware;

internal sealed class OpenAlRenderer : IDisposable
{
    private const int AlFormatMono16 = 0x1101;
    private const int AlBuffersQueued = 0x1015;
    private const int AlBuffersProcessed = 0x1016;
    private const int AlSourceState = 0x1010;
    private const int AlPlaying = 0x1012;
    private const int BufferCount = 8;
    private const int SamplesPerBuffer = 1024;

    private readonly Func<int, short[]> _sampleProvider;
    private readonly int _sampleRate;
    private readonly nint _libHandle;
    private readonly nint _device;
    private readonly nint _context;
    private readonly uint _source;
    private readonly Queue<uint> _freeBuffers = new();
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
    private readonly AlSourcePlay _alSourcePlay;
    private readonly AlSourceStop _alSourceStop;

    public OpenAlRenderer(Func<int, short[]> sampleProvider, int sampleRate)
    {
        _sampleProvider = sampleProvider;
        _sampleRate = sampleRate;

        _libHandle = LoadOpenAlLibrary();
        _alcOpenDevice = LoadFn<AlcOpenDevice>("alcOpenDevice");
        _alcCloseDevice = LoadFn<AlcCloseDevice>("alcCloseDevice");
        _alcCreateContext = LoadFn<AlcCreateContext>("alcCreateContext");
        _alcDestroyContext = LoadFn<AlcDestroyContext>("alcDestroyContext");
        _alcMakeContextCurrent = LoadFn<AlcMakeContextCurrent>("alcMakeContextCurrent");
        _alGenSources = LoadFn<AlGenSources>("alGenSources");
        _alDeleteSources = LoadFn<AlDeleteSources>("alDeleteSources");
        _alGenBuffers = LoadFn<AlGenBuffers>("alGenBuffers");
        _alDeleteBuffers = LoadFn<AlDeleteBuffers>("alDeleteBuffers");
        _alBufferData = LoadFn<AlBufferData>("alBufferData");
        _alSourceQueueBuffers = LoadFn<AlSourceQueueBuffers>("alSourceQueueBuffers");
        _alSourceUnqueueBuffers = LoadFn<AlSourceUnqueueBuffers>("alSourceUnqueueBuffers");
        _alGetSourcei = LoadFn<AlGetSourcei>("alGetSourcei");
        _alSourcePlay = LoadFn<AlSourcePlay>("alSourcePlay");
        _alSourceStop = LoadFn<AlSourceStop>("alSourceStop");

        _device = _alcOpenDevice(nint.Zero);
        if (_device == nint.Zero)
            throw new InvalidOperationException("Unable to open OpenAL device.");

        _context = _alcCreateContext(_device, nint.Zero);
        if (_context == nint.Zero || _alcMakeContextCurrent(_context) == 0)
            throw new InvalidOperationException("Unable to create OpenAL context.");

        _alGenSources(1, out _source);
        if (_source == 0)
            throw new InvalidOperationException("Unable to allocate OpenAL source.");

        for (int i = 0; i < BufferCount; i++)
        {
            _alGenBuffers(1, out uint buffer);
            if (buffer != 0)
                _freeBuffers.Enqueue(buffer);
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

        _alSourceStop(_source);

        _alGetSourcei(_source, AlBuffersQueued, out int queued);
        for (int i = 0; i < queued; i++)
        {
            _alSourceUnqueueBuffers(_source, 1, out uint buffer);
            if (buffer != 0)
                _freeBuffers.Enqueue(buffer);
        }

        while (_freeBuffers.Count > 0)
        {
            uint buffer = _freeBuffers.Dequeue();
            _alDeleteBuffers(1, ref buffer);
        }

        uint source = _source;
        _alDeleteSources(1, ref source);
        _alcMakeContextCurrent(nint.Zero);
        _alcDestroyContext(_context);
        _alcCloseDevice(_device);
        NativeLibrary.Free(_libHandle);
    }

    private void AudioLoop()
    {
        while (_running)
        {
            try
            {
                ReclaimProcessedBuffers();
                QueueMoreBuffers();
                EnsurePlaying();
                Thread.Sleep(4);
            }
            catch
            {
                Thread.Sleep(16);
            }
        }
    }

    private void ReclaimProcessedBuffers()
    {
        _alGetSourcei(_source, AlBuffersProcessed, out int processed);
        for (int i = 0; i < processed; i++)
        {
            _alSourceUnqueueBuffers(_source, 1, out uint buffer);
            if (buffer != 0)
                _freeBuffers.Enqueue(buffer);
        }
    }

    private void QueueMoreBuffers()
    {
        _alGetSourcei(_source, AlBuffersQueued, out int queued);
        while (queued < 4 && _freeBuffers.Count > 0)
        {
            uint buffer = _freeBuffers.Dequeue();
            short[] samples = _sampleProvider(SamplesPerBuffer);
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

            _alSourceQueueBuffers(_source, 1, ref buffer);
            queued++;
        }
    }

    private void EnsurePlaying()
    {
        _alGetSourcei(_source, AlSourceState, out int state);
        if (state != AlPlaying)
            _alSourcePlay(_source);
    }

    private T LoadFn<T>(string symbol) where T : Delegate
    {
        nint ptr = NativeLibrary.GetExport(_libHandle, symbol);
        return Marshal.GetDelegateForFunctionPointer<T>(ptr);
    }

    private static nint LoadOpenAlLibrary()
    {
        string[] names =
        [
            "openal32.dll",
            "libopenal.so.1",
            "libopenal.so",
            "/System/Library/Frameworks/OpenAL.framework/OpenAL"
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
    private delegate void AlSourcePlay(uint source);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void AlSourceStop(uint source);
}
