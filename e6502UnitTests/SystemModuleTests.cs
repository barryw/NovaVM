using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the SYSTEM module image (id=$03, built from modules/system/system.s) carries
/// the canonical paged-library header at $C000:
///   $C000  JMP dispatch  ($4C ...)
///   $C003  "NL" magic    ($4E $4C)
///   $C005  module id     ($03  MODULE_ID_SYSTEM)
///   $C006  ABI version   ($01  LIB_ABI_VERSION)
///   $C007  fn count      ($13  EDIT + WAIT/WAITVBL/TIMER + RNG8/16/32 +
///                              DIALOG_DEFAULTS/DIALOG/DIALOG_WAIT/DIALOG_ERROR/WAIT_KEY +
///                              OVL_LOAD/UNLOAD/INIT/MAIN/TICK + ADDR_LOOKUP + SCREEN_READLINE)
/// The header bytes are defined by runtime/asm/libmod.inc + libabi.inc + libsystem.inc;
/// this is the byte-exact guard the loader (lib_call) depends on when paging the editor.
/// </summary>
[TestClass]
public class SystemModuleTests
{
    [TestMethod]
    public void SystemModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("modules", "system", "system.bin"));

        Assert.AreEqual(16384, img.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x03, img[5]);   // MODULE_ID_SYSTEM
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x13, img[7]);   // SYS_FN_COUNT (EDIT + WAIT/WAITVBL/TIMER + RNG + DIALOG + OVL + ADDR_LOOKUP + SCREEN_READLINE)
    }

    // =====================================================================
    // SYS_SCREEN_READLINE ($12) — the C64-style full-screen line reader.
    // Axis-2 harness (mirrors GraphicsModuleTests): inject system.bin as the
    // extension ROM, romswap it to $C000, attach a ScreenEditor for key input,
    // then JSR the module dispatch directly (loader bypassed — paging is proven
    // independently in LibCallTests). The real VGC backs the char-plane screen
    // window ($A200), the cursor registers, and the CHARIN key queue.
    // =====================================================================

    // Mailbox cells mirror runtime/asm/libabi.inc.
    private const ushort FN_ID = 0x0301, STATUS = 0x0302, ARG0 = 0x0303, RESULT = 0x0313;
    private const byte   SYS_SCREEN_READLINE = 0x12;
    private const byte   LERR_OK = 0x00;
    private const ushort Sentinel = 0xFFF9;          // module RTS lands here; loop stops
    // VGC register / window addresses (runtime/asm/nova.inc).
    private const ushort VGC_CURSX = 0xA003, VGC_CURSY = 0xA004, VGC_TEXT_TOPROW = 0xA0ED;
    private const ushort VGC_CURSEN = 0xA00A, VGC_CHAROUT = 0xA00E;
    private const ushort VGC_SCREENWIN = 0xA200, VGC_SCREENWIN_PLANE = 0xB1A0;
    private const byte   VGC_SCREENWIN_CHAR = 0x00;
    private const int    ScreenCols = 80, ScreenRows = 50;

    private static CompositeBusDevice MakeSystemBus()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.LoadExtensionRomBytesForTest(File.ReadAllBytes(RepoPath("modules", "system", "system.bin")));
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Extension, bus.CurrentRom);
        Assert.AreEqual(0x4C, bus.Read(0xC000), "module $C000 must be JMP (the header trampoline)");
        return bus;
    }

    // Write a 32-bit LE arg cell.
    private static void SetArg(CompositeBusDevice bus, ushort cell, int value)
    {
        for (int i = 0; i < 4; i++)
            bus.WriteRam((ushort)(cell + i), (byte)((value >> (8 * i)) & 0xFF));
    }

    // Drive the module dispatch directly for fn-id `fn`; assert it RTSes and
    // returns LIB_STATUS = OK.
    private static void RunFn(CompositeBusDevice bus, byte fn)
    {
        bus.WriteRam(FN_ID, fn);
        bus.WriteRam(STATUS, 0xFF);   // poison so a real OK write is observable

        var cpu = new Cpu(bus, E6502Type.Cmos);
        bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
        bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
        var s = cpu.GetState();
        cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                      s.Nf, s.Vf, s.Df, true /*I*/, s.Zf, s.Cf));

        for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
            cpu.ExecuteNext();
        Assert.AreEqual(Sentinel, cpu.Pc, $"fn ${fn:X2} dispatch did not RTS back to the sentinel");
        Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), $"fn ${fn:X2} must set LIB_STATUS = OK");
    }

    // Write a string into the char-plane screen window at (col0,row) via the bus
    // ($A200 + row*ScreenCols + col), exactly as the module reads it.
    private static void WriteScreenWindowRow(CompositeBusDevice bus, int row, int col0, string text)
    {
        int baseAddr = VGC_SCREENWIN + row * ScreenCols + col0;
        for (int i = 0; i < text.Length; i++)
            bus.Write((ushort)(baseAddr + i), (byte)text[i]);
    }

    // Read `count` cells of the char-plane screen window starting at (col0,row).
    private static string ReadScreenWindowRow(CompositeBusDevice bus, int row, int col0, int count)
    {
        int baseAddr = VGC_SCREENWIN + row * ScreenCols + col0;
        var sb = new System.Text.StringBuilder(count);
        for (int i = 0; i < count; i++)
            sb.Append((char)bus.Read((ushort)(baseAddr + i)));
        return sb.ToString();
    }

    // Fill a screen-window row with spaces (a blank row the editor types onto).
    private static void BlankScreenWindowRow(CompositeBusDevice bus, int row)
    {
        int baseAddr = VGC_SCREENWIN + row * ScreenCols;
        for (int i = 0; i < ScreenCols; i++)
            bus.Write((ushort)(baseAddr + i), 0x20);
    }

    // Queue every byte of `keys` (raw bytes) to the editor input queue, in order.
    private static void QueueKeys(ScreenEditor editor, params byte[] keys)
    {
        foreach (byte k in keys) editor.QueueInput(k);
    }

    // Queue each character of `text` as its ASCII byte.
    private static void QueueText(ScreenEditor editor, string text)
    {
        foreach (char c in text) editor.QueueInput((byte)c);
    }

    /// <summary>
    /// Core mechanic: on ENTER, SYS_SCREEN_READLINE reads the physical screen row
    /// under the cursor into the caller buffer and returns its length with trailing
    /// spaces trimmed. Proves the row mapping (window row == CURSY directly).
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ReadsRowUnderCursorOnEnter()
    {
        using var bus = MakeSystemBus();

        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        // Set a NONZERO ring-scroll base. This makes the row mapping falsifiable:
        // the screen window + VGC_CURSY index the physical plane row directly, while
        // VGC_TEXT_TOPROW only remaps display-row -> physical-row at render time. If
        // the module wrongly read (CURSY+TOPROW) mod 50 it would land on a blank row
        // and the text/length asserts below would fail.
        bus.Write(VGC_TEXT_TOPROW, 17);

        // Select the char plane for the screen window, then paint "PRINT 7" at row R.
        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 12;
        WriteScreenWindowRow(bus, row, 0, "PRINT 7");

        // Cursor sits at the start of that row.
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        // Caller buffer in low RAM (page 2, like BASIC's Ibuffs), poisoned so an
        // untouched trailing byte is observable. b2 = max length $7F.
        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));   // b0/b1 = ptr, b2 = maxlen

        // Press ENTER — the only key needed for the read-row mechanic.
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        // The submitted line is "PRINT 7" (7 chars), no trailing NUL, spaces trimmed.
        Assert.AreEqual(7, bus.ReadRam(RESULT), "RESULT b0 must be the trimmed line length (7)");
        var got = new char[7];
        for (int i = 0; i < 7; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("PRINT 7", new string(got), "buffer must hold the row text under the cursor");
        Assert.AreEqual(0xAA, bus.ReadRam((ushort)(buf + 7)),
            "no trailing NUL or padding: the byte past the line must be untouched");
    }

    /// <summary>
    /// Task 3 — echo: typed printable chars land on the screen window at the cursor,
    /// the cursor advances per char, and on ENTER the row reads back as the typed line.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_TypedCharsLandOnScreenThenReturn()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 8;
        BlankScreenWindowRow(bus, row);

        // Cursor starts at column 0 of the blank row.
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueText(editor, "GOTO 10");
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        // The typed chars landed on the screen window row...
        Assert.AreEqual("GOTO 10", ReadScreenWindowRow(bus, row, 0, 7),
            "typed chars must be echoed to the char-plane screen window");
        // ...and the returned buffer is the same line.
        Assert.AreEqual(7, bus.ReadRam(RESULT), "RESULT b0 must be the typed line length");
        var got = new char[7];
        for (int i = 0; i < 7; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("GOTO 10", new string(got), "buffer must hold the typed line");
    }

    /// <summary>
    /// Task 4 — arrows move the cursor and clamp at the screen edges. From (5,5):
    /// Up,Up,Left,Right,Down -> (5,4). From a corner, Up/Left stay put.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ArrowsMoveCursorClamped()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        bus.Write(VGC_CURSX, 5);
        bus.Write(VGC_CURSY, 5);

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        // 30=Up, 28=Left, 29=Right, 31=Down. (5,5)->Up(5,4)->Up(5,3)->Left(4,3)
        // ->Right(5,3)->Down(5,4).
        QueueKeys(editor, 30, 30, 28, 29, 31);
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(5, bus.Read(VGC_CURSX), "final cursor X after arrow moves");
        Assert.AreEqual(4, bus.Read(VGC_CURSY), "final cursor Y after arrow moves");
    }

    /// <summary>
    /// Task 4 — clamp at the top-left corner: Up and Left from (0,0) stay at (0,0).
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ArrowsClampAtTopLeft()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, 0);

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueKeys(editor, 30, 28);   // Up, Left -- both clamp
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(0, bus.Read(VGC_CURSX), "Left clamps at column 0");
        Assert.AreEqual(0, bus.Read(VGC_CURSY), "Up clamps at row 0");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}
