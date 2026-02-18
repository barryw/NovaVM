using System.Collections.Generic;
using e6502.TUI.Hardware;

namespace e6502.TUI.Rendering;

public class ScreenEditor
{
    private readonly VirtualGraphicsController _vgc;
    private readonly Queue<byte> _inputQueue = new();

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
    // RETURN — C64-style read-back of current line
    // -------------------------------------------------------------------------

    public void HandleReturn()
    {
        string line = ReadLineFromScreen();
        foreach (char c in line)
            _inputQueue.Enqueue((byte)c);
        _inputQueue.Enqueue(0x0D); // CR terminator

        // Advance cursor to next line via CHAROUT
        _vgc.Write(VgcConstants.RegCharOut, 0x0D);
    }

    // -------------------------------------------------------------------------
    // Typed character — write directly to screen RAM, advance cursor
    // -------------------------------------------------------------------------

    public void HandleTypedChar(byte ch)
    {
        int cx = _vgc.GetCursorX();
        int cy = _vgc.GetCursorY();

        ushort screenAddr = (ushort)(VgcConstants.CharRamBase + cy * VgcConstants.ScreenCols + cx);
        ushort colorAddr  = (ushort)(VgcConstants.ColorRamBase + cy * VgcConstants.ScreenCols + cx);

        byte fgcol = _vgc.Read(VgcConstants.RegFgCol);

        _vgc.Write(screenAddr, ch);
        _vgc.Write(colorAddr, fgcol);

        // Advance cursor — wrap to next line if past col 79
        cx++;
        if (cx >= VgcConstants.ScreenCols)
        {
            cx = 0;
            int newCy = cy + 1;
            if (newCy >= VgcConstants.ScreenRows)
                newCy = VgcConstants.ScreenRows - 1;
            _vgc.Write(VgcConstants.RegCursorX, (byte)cx);
            _vgc.Write(VgcConstants.RegCursorY, (byte)newCy);
        }
        else
        {
            _vgc.Write(VgcConstants.RegCursorX, (byte)cx);
        }
    }

    // -------------------------------------------------------------------------
    // Input queue
    // -------------------------------------------------------------------------

    public bool HasQueuedInput => _inputQueue.Count > 0;

    public byte DequeueInput() =>
        _inputQueue.Count > 0 ? _inputQueue.Dequeue() : (byte)0;

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
