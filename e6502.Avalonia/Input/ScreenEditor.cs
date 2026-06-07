using System.Collections.Concurrent;
using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Input;

public class ScreenEditor : IScreenInput
{
    private readonly VirtualGraphicsController _vgc;
    private readonly ConcurrentQueue<byte> _inputQueue = new();

    public ScreenEditor(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
    }

    public void QueueInput(byte ch) => _inputQueue.Enqueue(ch);

    public void ClearInputQueue() => _inputQueue.Clear();

    public bool HasQueuedInput => !_inputQueue.IsEmpty;

    public byte DequeueInput() =>
        _inputQueue.TryDequeue(out byte b) ? b : (byte)0;


}
