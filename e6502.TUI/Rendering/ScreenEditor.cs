using System.Collections.Concurrent;
using e6502.TUI.Hardware;

namespace e6502.TUI.Rendering;

public class ScreenEditor
{
    private readonly VirtualGraphicsController _vgc;
    private readonly ConcurrentQueue<byte> _inputQueue = new();

    public ScreenEditor(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
    }

    // -------------------------------------------------------------------------
    // Cursor movement
    // -------------------------------------------------------------------------

    public void CursorRight()
    {
        int cx = _vgc.GetCursorX();
        if (cx < VgcConstants.ScreenCols - 1)
            _vgc.Write(VgcConstants.RegCursorX, (byte)(cx + 1));
    }

    public void CursorLeft()
    {
        int cx = _vgc.GetCursorX();
        if (cx > 0)
            _vgc.Write(VgcConstants.RegCursorX, (byte)(cx - 1));
    }

    public void CursorDown()
    {
        int cy = _vgc.GetCursorY();
        if (cy < VgcConstants.ScreenRows - 1)
            _vgc.Write(VgcConstants.RegCursorY, (byte)(cy + 1));
    }

    public void CursorUp()
    {
        int cy = _vgc.GetCursorY();
        if (cy > 0)
            _vgc.Write(VgcConstants.RegCursorY, (byte)(cy - 1));
    }

    // -------------------------------------------------------------------------
    // Queue a byte for the CPU to read via CHARIN ($A00F).
    // EhBASIC uses a serial-terminal model: characters go straight to input,
    // and EhBASIC echoes them back via CHAROUT.
    // -------------------------------------------------------------------------

    public void QueueInput(byte ch) => _inputQueue.Enqueue(ch);

    // -------------------------------------------------------------------------
    // Input queue
    // -------------------------------------------------------------------------

    public bool HasQueuedInput => !_inputQueue.IsEmpty;

    public byte DequeueInput() =>
        _inputQueue.TryDequeue(out byte b) ? b : (byte)0;

    // -------------------------------------------------------------------------
    // Screen RAM read helper
    // -------------------------------------------------------------------------

    public string ReadLineFromScreen()
    {
        int row = _vgc.GetCursorY();
        var sb = new System.Text.StringBuilder(VgcConstants.ScreenCols);
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            byte b = _vgc.GetScreenChar(col, row);
            sb.Append(b >= 0x20 ? (char)b : ' ');
        }
        return sb.ToString().TrimEnd();
    }
}
