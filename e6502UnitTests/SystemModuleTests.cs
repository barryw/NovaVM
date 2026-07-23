using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
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
///   $C007  fn count      ($1D  retired EDIT slot + WAIT/WAITVBL/TIMER + RNG8/16/32 +
///                              DIALOG_DEFAULTS/DIALOG/DIALOG_WAIT/DIALOG_ERROR/WAIT_KEY +
///                              OVL_LOAD/UNLOAD/INIT/MAIN/TICK + ADDR_LOOKUP + SCREEN_READLINE +
///                              NUI save-under/picker/full save-under/style/file picker/text input/drain +
///                              console clear-to-end-of-line)
/// The header bytes are defined by runtime/asm/libmod.inc + libabi.inc + libsystem.inc;
/// this is the byte-exact guard the loader (lib_call) depends on when paging SYSTEM.
/// </summary>
[TestClass]
public class SystemModuleTests
{
    [TestMethod]
    public void SystemModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("software", "modules", "system", "system.bin"));

        Assert.AreEqual(16384, img.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x03, img[5]);   // MODULE_ID_SYSTEM
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x1D, img[7]);   // SYS_FN_COUNT (through SYS_CONSOLE_CLEAR_EOL)
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
    private const ushort FN_ID = 0x0301, STATUS = 0x0302, ARG0 = 0x0303, ARG1 = 0x0307,
                         ARG2 = 0x030B, ARG3 = 0x030F, RESULT = 0x0313;
    private const byte   SYS_DIALOG = 0x08;
    private const byte   SYS_WAIT_KEY = 0x0B;
    private const byte   SYS_SCREEN_READLINE = 0x12;
    private const byte   SYS_NUI_SAVE_UNDER = 0x13, SYS_NUI_RESTORE_UNDER = 0x14;
    private const byte   SYS_NUI_PICK_LIST = 0x15;
    private const byte   SYS_NUI_SAVE_UNDER_FULL = 0x16, SYS_NUI_RESTORE_UNDER_FULL = 0x17;
    private const byte   SYS_NUI_SET_STYLE = 0x18;
    private const byte   SYS_NUI_FILE_PICKER = 0x19, SYS_NUI_TEXT_INPUT = 0x1A, SYS_NUI_DRAIN_KEYS = 0x1B;
    private const byte   SYS_CONSOLE_CLEAR_EOL = 0x1C;
    private const byte   LERR_OK = 0x00;
    private const ushort Sentinel = 0xFFF9;          // module RTS lands here; loop stops
    // VGC register / window addresses (runtime/asm/nova.inc).
    private const ushort VGC_CURSX = 0xA003, VGC_CURSY = 0xA004, VGC_TEXT_TOPROW = 0xA0ED;
    private const ushort VGC_CURSEN = 0xA00A, VGC_CHAROUT = 0xA00E;
    private const ushort VGC_SCREENWIN = 0xA200, VGC_SCREENWIN_PLANE = 0xB1A0;
    private const byte   VGC_SCREENWIN_CHAR = 0x00, VGC_SCREENWIN_COLOR = 0x01, VGC_SCREENWIN_ATTR = 0x02;
    private const int    ScreenCols = 80, ScreenRows = 50;

    private static CompositeBusDevice MakeSystemBus()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.LoadExtensionRomBytesForTest(File.ReadAllBytes(RepoPath("software", "modules", "system", "system.bin")));
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
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }
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
        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        int baseAddr = VGC_SCREENWIN + row * ScreenCols + col0;
        var sb = new System.Text.StringBuilder(count);
        for (int i = 0; i < count; i++)
            sb.Append((char)bus.Read((ushort)(baseAddr + i)));
        return sb.ToString();
    }

    private static string ReadRamAscii(CompositeBusDevice bus, ushort address, int count)
    {
        var sb = new System.Text.StringBuilder(count);
        for (int i = 0; i < count; i++)
            sb.Append((char)bus.ReadRam((ushort)(address + i)));
        return sb.ToString();
    }

    // Fill a screen-window row with spaces (a blank row the editor types onto).
    private static void BlankScreenWindowRow(CompositeBusDevice bus, int row)
    {
        int baseAddr = VGC_SCREENWIN + row * ScreenCols;
        for (int i = 0; i < ScreenCols; i++)
            bus.Write((ushort)(baseAddr + i), 0x20);
    }

    private static void WriteScreenWindowCell(CompositeBusDevice bus, byte plane, int row, int col, byte value)
    {
        bus.Write(VGC_SCREENWIN_PLANE, plane);
        bus.Write((ushort)(VGC_SCREENWIN + row * ScreenCols + col), value);
    }

    private static byte ReadScreenWindowCell(CompositeBusDevice bus, byte plane, int row, int col)
    {
        bus.Write(VGC_SCREENWIN_PLANE, plane);
        return bus.Read((ushort)(VGC_SCREENWIN + row * ScreenCols + col));
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
    /// NUI.WAIT_KEY must consume exactly one queued keyboard byte per call.
    /// Modal controls use this for text entry; draining the queue drops pasted
    /// or serial-injected descriptions before the UI can store them.
    /// </summary>
    [TestMethod]
    public void NuiWaitKey_ReturnsQueuedBytesInOrder()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        QueueText(editor, "ab");

        RunFn(bus, SYS_WAIT_KEY);
        Assert.AreEqual((byte)'a', bus.ReadRam(RESULT), "first wait-key call must return the first queued byte");

        RunFn(bus, SYS_WAIT_KEY);
        Assert.AreEqual((byte)'b', bus.ReadRam(RESULT), "second wait-key call must return the next queued byte");
    }

    [TestMethod]
    public void NuiDrainKeys_ConsumesPendingKeyboardBytes()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        QueueKeys(editor, 0x0D, 0x1B);
        RunFn(bus, SYS_NUI_DRAIN_KEYS);

        Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "drain_keys must report success.");
        Assert.IsFalse(editor.HasQueuedInput,
            "drain_keys must consume pending modal-transition keys before the next control receives focus.");
    }

    /// <summary>
    /// Clear-to-EOL must use a bounded cell count: CHAROUT wraps column 79 to
    /// zero, so polling the cursor for column 80 would never terminate.
    /// </summary>
    [TestMethod]
    public void ConsoleClearEol_ClearsRemainderAndPreservesCursor()
    {
        using var bus = MakeSystemBus();
        WriteScreenWindowRow(bus, 7, 0, new string('X', ScreenCols));
        bus.Write(VGC_CURSX, 5);
        bus.Write(VGC_CURSY, 7);

        RunFn(bus, SYS_CONSOLE_CLEAR_EOL);

        Assert.AreEqual("XXXXX" + new string(' ', ScreenCols - 5),
            ReadScreenWindowRow(bus, 7, 0, ScreenCols));
        Assert.AreEqual(5, bus.Read(VGC_CURSX));
        Assert.AreEqual(7, bus.Read(VGC_CURSY));
    }

    private static void WriteRamBytes(CompositeBusDevice bus, ushort address, ReadOnlySpan<byte> bytes)
    {
        for (int i = 0; i < bytes.Length; i++)
            bus.WriteRam((ushort)(address + i), bytes[i]);
    }

    private static void WriteRamString(CompositeBusDevice bus, ushort address, string text)
    {
        for (int i = 0; i < text.Length; i++)
            bus.WriteRam((ushort)(address + i), (byte)text[i]);
        bus.WriteRam((ushort)(address + text.Length), 0);
    }

    /// <summary>
    /// NUI save-under is the reusable primitive for modal overlays in mixed
    /// text/graphics runtimes: it must preserve char, color, and text-attribute
    /// cells by copying the covered rectangle through caller-owned XRAM buffers.
    /// </summary>
    [TestMethod]
    public void NuiSaveUnder_RoundTripsTextPlanesThroughCallerXram()
    {
        using var bus = MakeSystemBus();

        const int left = 7, top = 9, width = 5, height = 2;
        const int cellCount = width * height;
        const int charBase = 0x010000;
        const int colorBase = charBase + cellCount;
        const int attrBase = colorBase + cellCount;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int i = y * width + x;
                WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x, (byte)('A' + i));
                WriteScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x, (byte)(0x20 + i));
                WriteScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x, (byte)(0x80 + i));
            }
        }

        SetArg(bus, ARG0, left | (top << 8) | (width << 16) | (height << 24));
        SetArg(bus, ARG1, charBase);
        SetArg(bus, ARG2, colorBase);
        SetArg(bus, ARG3, attrBase);
        RunFn(bus, SYS_NUI_SAVE_UNDER);

        for (int i = 0; i < cellCount; i++)
        {
            Assert.AreEqual((byte)('A' + i), bus.ReadXram(charBase + i),
                $"char plane byte {i} must be saved to caller XRAM");
            Assert.AreEqual((byte)(0x20 + i), bus.ReadXram(colorBase + i),
                $"color plane byte {i} must be saved to caller XRAM");
            Assert.AreEqual((byte)(0x80 + i), bus.ReadXram(attrBase + i),
                $"text-attr plane byte {i} must be saved to caller XRAM");
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x, (byte)'?');
                WriteScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x, 0x00);
                WriteScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x, 0x00);
            }
        }

        SetArg(bus, ARG0, left | (top << 8) | (width << 16) | (height << 24));
        SetArg(bus, ARG1, charBase);
        SetArg(bus, ARG2, colorBase);
        SetArg(bus, ARG3, attrBase);
        RunFn(bus, SYS_NUI_RESTORE_UNDER);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int i = y * width + x;
                Assert.AreEqual((byte)('A' + i),
                    ReadScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x),
                    $"char plane cell {i} must be restored");
                Assert.AreEqual((byte)(0x20 + i),
                    ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x),
                    $"color plane cell {i} must be restored");
                Assert.AreEqual((byte)(0x80 + i),
                    ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x),
                    $"text-attr plane cell {i} must be restored");
            }
        }
    }

    /// <summary>
    /// Mixed text/graphics runtimes draw modal controls with graphics-plane fills
    /// and text-plane labels. Full save-under must preserve both layers, or closing
    /// a dialog leaves colored graphics rectangles behind the restored text.
    /// </summary>
    [TestMethod]
    public void NuiSaveUnderFull_RoundTripsTextAndGraphicsPlanesThroughCallerXram()
    {
        using var bus = MakeSystemBus();

        const int left = 3, top = 4, width = 2, height = 2;
        const int cellCount = width * height;
        const int gfxX = left * 4, gfxY = top * 4, gfxWidth = width * 4, gfxHeight = height * 4;
        const int gfxCount = gfxWidth * gfxHeight;
        const int charBase = 0x010000;
        const int colorBase = charBase + cellCount;
        const int attrBase = colorBase + cellCount;
        const int gfxBase = attrBase + cellCount;
        const ushort config = 0x0680;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int i = y * width + x;
                WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x, (byte)('K' + i));
                WriteScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x, (byte)(0x30 + i));
                WriteScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x, (byte)(0x40 + i));
            }
        }

        for (int y = 0; y < gfxHeight; y++)
            for (int x = 0; x < gfxWidth; x++)
                bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx,
                    (gfxY + y) * VgcConstants.GfxWidth + gfxX + x,
                    (byte)(0x50 + y * gfxWidth + x));

        WriteRamBytes(bus, config, new byte[]
        {
            left, top, width, height,
            (byte)(charBase & 0xFF), (byte)((charBase >> 8) & 0xFF), (byte)((charBase >> 16) & 0xFF),
            (byte)(colorBase & 0xFF), (byte)((colorBase >> 8) & 0xFF), (byte)((colorBase >> 16) & 0xFF),
            (byte)(attrBase & 0xFF), (byte)((attrBase >> 8) & 0xFF), (byte)((attrBase >> 16) & 0xFF),
            (byte)(gfxBase & 0xFF), (byte)((gfxBase >> 8) & 0xFF), (byte)((gfxBase >> 16) & 0xFF),
        });
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_SAVE_UNDER_FULL);

        for (int i = 0; i < cellCount; i++)
        {
            Assert.AreEqual((byte)('K' + i), bus.ReadXram(charBase + i), $"char plane byte {i} must be saved");
            Assert.AreEqual((byte)(0x30 + i), bus.ReadXram(colorBase + i), $"color plane byte {i} must be saved");
            Assert.AreEqual((byte)(0x40 + i), bus.ReadXram(attrBase + i), $"text-attr plane byte {i} must be saved");
        }

        for (int i = 0; i < gfxCount; i++)
            Assert.AreEqual((byte)(0x50 + i), bus.ReadXram(gfxBase + i),
                $"graphics pixel {i} must be saved row-tightly to caller XRAM");

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x, (byte)'?');
                WriteScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x, 0x00);
                WriteScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x, 0x00);
            }
        }

        for (int y = 0; y < gfxHeight; y++)
            for (int x = 0; x < gfxWidth; x++)
                bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx,
                    (gfxY + y) * VgcConstants.GfxWidth + gfxX + x,
                    0x00);

        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_RESTORE_UNDER_FULL);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int i = y * width + x;
                Assert.AreEqual((byte)('K' + i), ReadScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + y, left + x));
                Assert.AreEqual((byte)(0x30 + i), ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + y, left + x));
                Assert.AreEqual((byte)(0x40 + i), ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + y, left + x));
            }
        }

        for (int y = 0; y < gfxHeight; y++)
        {
            for (int x = 0; x < gfxWidth; x++)
            {
                Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx,
                    (gfxY + y) * VgcConstants.GfxWidth + gfxX + x,
                    out byte actual));
                Assert.AreEqual((byte)(0x50 + y * gfxWidth + x), actual,
                    $"graphics pixel ({x},{y}) must be restored");
            }
        }
    }

    /// <summary>
    /// NUI style belongs to the shared control layer: callers pass palette indexes
    /// that make sense for their active palette, and the dialog applies them to
    /// graphics chrome plus text cells without knowing what the colors mean.
    /// It also clears the text cells under the graphics-only shadow so mixed
    /// text/graphics overlays do not leak stale text around the right/bottom edge.
    /// </summary>
    [TestMethod]
    public void NuiDialog_UsesCallerStyleAndClearsShadowTextCells()
    {
        using var bus = MakeSystemBus();

        const ushort title = 0x0640;
        const ushort message = 0x0660;
        const ushort footer = 0x0680;
        const byte left = 20, top = 10, width = 24, height = 8;
        const int gfxLeft = left * 4, gfxTop = top * 4;
        const int gfxWidth = width * 4, gfxHeight = height * 4;

        WriteRamString(bus, title, "SAVE GAME");
        WriteRamString(bus, message, "Description:");
        WriteRamString(bus, footer, "ENTER OK");

        WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + 3, left + width, (byte)'?');
        WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + height, left + 3, (byte)'?');

        SetArg(bus, ARG0, unchecked((int)0xCD0C0F00)); // shadow=0, top/left border=$0F, panel=$0C, text=$CD.
        RunFn(bus, SYS_NUI_SET_STYLE);

        SetArg(bus, ARG0, title);
        SetArg(bus, ARG1, message);
        SetArg(bus, ARG2, footer);
        SetArg(bus, ARG3, left | (top << 8) | (width << 16) | (height << 24));
        RunFn(bus, SYS_DIALOG);

        Assert.AreEqual("Description:", ReadScreenWindowRow(bus, top + 3, left + 2, 12),
            "dialog message must still render through the shared text path");
        Assert.AreEqual(0xCD, ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + 3, left + 2),
            "dialog text color must use caller-supplied packed bg/fg indexes");
        Assert.AreEqual((byte)' ', ReadScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + 3, left + width),
            "right shadow text cell must be cleared so stale underlying text cannot leak through");
        Assert.AreEqual(0x0D, ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + 3, left + width),
            "right shadow text cell must expose the graphics-plane shadow instead of covering it with panel background");
        Assert.AreEqual((byte)' ', ReadScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + height, left + 3),
            "bottom shadow text cell must be cleared so stale underlying text cannot leak through");
        Assert.AreEqual(0x0D, ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, top + height, left + 3),
            "bottom shadow text cell must expose the graphics-plane shadow instead of covering it with panel background");

        byte Gfx(int x, int y)
        {
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx,
                y * VgcConstants.GfxWidth + x, out byte value),
                $"graphics pixel ({x},{y}) must be readable");
            return value;
        }

        Assert.AreEqual(0x0F, Gfx(gfxLeft + 2, gfxTop),
            "dialog top highlight must use caller-supplied border color index");
        Assert.AreEqual(0x0F, Gfx(gfxLeft, gfxTop + 2),
            "dialog left highlight must use caller-supplied border color index");
        Assert.AreEqual(0x0C, Gfx(gfxLeft + 2, gfxTop + 2),
            "dialog panel fill must use caller-supplied panel color index");
        Assert.AreEqual(0x0C, Gfx(gfxLeft + gfxWidth - 1, gfxTop + 2),
            "dialog right edge must remain panel-colored, not border-colored");
        Assert.AreEqual(0x0C, Gfx(gfxLeft + 2, gfxTop + gfxHeight - 1),
            "dialog bottom edge must remain panel-colored, not border-colored");
        Assert.AreEqual(0x00, Gfx(gfxLeft + gfxWidth + 1, gfxTop + 2),
            "dialog right shadow must use caller-supplied shadow color index");
        Assert.AreEqual(0x00, Gfx(gfxLeft + 2, gfxTop + gfxHeight + 1),
            "dialog bottom shadow must use caller-supplied shadow color index");
    }

    /// <summary>
    /// The shared NUI picker is the control NovaZ save/load overlays will use:
    /// callers provide fixed-width rows and the control owns navigation,
    /// reverse-video selection, and OK/Cancel result reporting.
    /// </summary>
    [TestMethod]
    public void NuiPickList_ReturnsSelectionAndHighlightsActiveRow()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x0600;
        const ushort title = 0x0640;
        const ushort footer = 0x0660;
        const ushort rows = 0x0700;
        const byte rowWidth = 12;
        const byte left = 10, top = 8, width = 20, height = 8;

        WriteRamString(bus, title, "SAVES");
        WriteRamString(bus, footer, "ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, rows, System.Text.Encoding.ASCII.GetBytes(
            "SAVE 0      " +
            "SAVE 1      " +
            "SAVE 2      "));
        for (int i = 0; i < 6; i++)
            WriteScreenWindowCell(bus, VGC_SCREENWIN_CHAR, top + height - 3, left + 2 + i, (byte)'?');

        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(rows & 0xFF), (byte)(rows >> 8),
            rowWidth, 3, 0,
            left, top, width, height,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        SetArg(bus, ARG0, config);
        QueueKeys(editor, 0x1B, (byte)'[', (byte)'B', 0x0D); // ANSI Down, Enter.

        RunFn(bus, SYS_NUI_PICK_LIST);

        Assert.AreEqual(1, bus.ReadRam(RESULT), "picker must return the selected row");
        Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK");
        Assert.AreEqual("SAVE 1", ReadScreenWindowRow(bus, top + 3, left + 2, 6),
            "the second row must be rendered after moving down once");
        Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + 3, left + 2),
            "the active row must use reverse-video text attributes");
        StringAssert.Contains(ReadScreenWindowRow(bus, top + height - 3, left + 2, 18), "  OK  ",
            "picker must render a Borland-style OK button below the list");
        StringAssert.Contains(ReadScreenWindowRow(bus, top + height - 3, left + 2, 18), " Cancel ",
            "picker must render a Borland-style Cancel button below the list");
    }

    [TestMethod]
    public void NuiPickList_EscReportsCancel()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x0600;
        const ushort title = 0x0640;
        const ushort footer = 0x0660;
        const ushort rows = 0x0700;

        WriteRamString(bus, title, "SAVES");
        WriteRamString(bus, footer, "ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, rows, System.Text.Encoding.ASCII.GetBytes(
            "SAVE 0      " +
            "SAVE 1      "));
        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(rows & 0xFF), (byte)(rows >> 8),
            12, 2, 1,
            10, 8, 20, 8,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        QueueKeys(editor, 0x1B);
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_PICK_LIST);

        Assert.AreEqual(1, bus.ReadRam(RESULT), "Esc must leave the caller's selected row intact.");
        Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)), "Esc must report NUI_RESULT_CANCEL.");
    }

    [TestMethod]
    public void NuiPickList_TabFocusesCancelButtonAndReturnsCancel()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x0600;
        const ushort title = 0x0640;
        const ushort footer = 0x0660;
        const ushort rows = 0x0700;

        WriteRamString(bus, title, "SAVES");
        WriteRamString(bus, footer, "TAB NEXT  ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, rows, System.Text.Encoding.ASCII.GetBytes(
            "SAVE 0      " +
            "SAVE 1      "));
        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(rows & 0xFF), (byte)(rows >> 8),
            12, 2, 0,
            10, 8, 20, 8,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        QueueKeys(editor, 0x09, 0x09, 0x0D);
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_PICK_LIST);

        Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)),
            "Tab to Cancel then Enter must report NUI_RESULT_CANCEL.");
        Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 13, 19),
            "focused Cancel button must use reverse-video text attributes.");
    }

    [TestMethod]
    public void NuiFilePicker_ShowsDrivesDirsAndReturnsFilteredFile()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root, image =>
            {
                CreateImageDirectory(image, 0xFFFF, "SRC");
                WriteImageText(image, 0xFFFF, "ROOT.4th", ": ROOT ;");
                WriteImageText(image, 0xFFFF, "SKIP.bas", "10 PRINT 1");
            });
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;
            const byte rowWidth = 36;
            const byte selectedRootForth = 2; // .., SRC/, ROOT

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, selectedRootForth,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x0D);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(8, bus.ReadRam(RESULT), "picker must return selected filename length");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK");
            Assert.AreEqual("ROOT.4th", ReadRamAscii(bus, output, 8), "picker must copy the selected filename to caller RAM");
            Assert.AreEqual((char)0xC9, ReadScreenWindowRow(bus, 10, 20, 1)[0],
                "picker must draw a double-line CP437 dialog frame instead of a bare floating list");
            Assert.AreEqual("Filename", ReadScreenWindowRow(bus, 13, 22, 8),
                "picker must render a separate filename field label above the file list");
            Assert.AreEqual(new string(' ', 36), ReadScreenWindowRow(bus, 14, 22, 36),
                "picker must leave breathing room between the filename field and file list");
            Assert.IsFalse(ReadScreenWindowRow(bus, 32, 20, 40).Contains("Help", StringComparison.Ordinal),
                "file picker must not render a Help button");
            Assert.IsFalse(ReadScreenWindowRow(bus, 15, 20, 40).Contains("Files", StringComparison.Ordinal),
                "file picker must not label the obvious file list");
            string bottomRow = ReadScreenWindowRow(bus, 35, 20, 40);
            Assert.IsFalse(bottomRow.Contains("ENTER OPEN", StringComparison.Ordinal),
                "picker must not print footer help text inside the dialog frame");
            Assert.AreEqual("..", ReadRamAscii(bus, rows, 2),
                "current directory view must start with parent navigation");
            Assert.AreNotEqual("FD0:", ReadRamAscii(bus, rows, 4),
                "drive rows must not clutter the current directory listing");
            Assert.AreEqual("SRC/", ReadRamAscii(bus, (ushort)(rows + 1 * rowWidth), 4),
                "directories must remain visible even when filtering files by type");
            Assert.AreEqual("ROOT.4th", ReadRamAscii(bus, (ushort)(rows + 2 * rowWidth), 8),
                "matching files must be listed after parent/directory rows");
            Assert.AreNotEqual("SKIP.bas", ReadRamAscii(bus, (ushort)(rows + 3 * rowWidth), 8),
                "non-matching file types must not consume picker rows");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_FiltersMultipleSourceTypesWithSixteenBitMask()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root, image =>
            {
                WriteImageText(image, 0xFFFF, "MAIN.asm", ".byte $EA\n");
                WriteImageText(image, 0xFFFF, "TURTLE.logo", "TO SQUARE\nEND\n");
                WriteImageText(image, 0xFFFF, "UNIT1.pas", "program Unit1;\n");
                WriteImageText(image, 0xFFFF, "WORDS.4th", ": SQUARE DUP * ;\n");
                WriteImageText(image, 0xFFFF, "SKIP.bas", "10 PRINT 1\n");
                WriteImageText(image, 0xFFFF, "TUNE.sid", "sid\n");
            });
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;
            const byte rowWidth = 36;
            const byte selectedUnit = 3; // .., MAIN, TURTLE, UNIT1, WORDS

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0xC0, 0x03, selectedUnit,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x0D);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(9, bus.ReadRam(RESULT), "picker must return selected Pascal filename length.");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK.");
            Assert.AreEqual("UNIT1.pas", ReadRamAscii(bus, output, 9),
                "a mask containing Pascal, Logo, Forth, and Assembly must allow selecting Pascal source.");
            Assert.AreEqual("..", ReadRamAscii(bus, rows, 2),
                "source-filtered file lists must keep parent navigation visible.");
            Assert.AreEqual("MAIN.asm", ReadRamAscii(bus, (ushort)(rows + 1 * rowWidth), 8),
                "Assembly source must be visible with the source file mask.");
            Assert.AreEqual("TURTLE.logo", ReadRamAscii(bus, (ushort)(rows + 2 * rowWidth), 11),
                "Logo source must be visible with the source file mask.");
            Assert.AreEqual("UNIT1.pas", ReadRamAscii(bus, (ushort)(rows + 3 * rowWidth), 9),
                "Pascal source must be visible with the high mask byte.");
            Assert.AreEqual("WORDS.4th", ReadRamAscii(bus, (ushort)(rows + 4 * rowWidth), 9),
                "Forth source must remain visible with the source file mask.");
            Assert.AreNotEqual("SKIP.bas", ReadRamAscii(bus, (ushort)(rows + 5 * rowWidth), 8),
                "BASIC files must not consume source-filtered picker rows.");
            Assert.AreNotEqual("TUNE.sid", ReadRamAscii(bus, (ushort)(rows + 5 * rowWidth), 8),
                "SID files must not consume source-filtered picker rows.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_ZeroTypeMaskShowsAnyFileType()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root, image =>
            {
                WriteImageText(image, 0xFFFF, "ALPHA.bas", "10 PRINT 1\n");
                WriteImageText(image, 0xFFFF, "BETA.sid", "sid\n");
                WriteImageText(image, 0xFFFF, "MAIN.asm", ".byte $EA\n");
            });
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;
            const byte rowWidth = 36;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0, 0, 1,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x0D);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(9, bus.ReadRam(RESULT), "picker must return selected filename length.");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK.");
            Assert.AreEqual("ALPHA.bas", ReadRamAscii(bus, output, 9),
                "a zero type mask must allow selecting any file type.");
            Assert.AreEqual("ALPHA.bas", ReadRamAscii(bus, (ushort)(rows + 1 * rowWidth), 9),
                "BASIC files must be listed when the mask is zero.");
            Assert.AreEqual("BETA.sid", ReadRamAscii(bus, (ushort)(rows + 2 * rowWidth), 8),
                "SID files must be listed when the mask is zero.");
            Assert.AreEqual("MAIN.asm", ReadRamAscii(bus, (ushort)(rows + 3 * rowWidth), 8),
                "Assembly files must be listed when the mask is zero.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_KeepsParentVisibleWhenDirectoryListScrolls()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root, image =>
            {
                for (int i = 0; i < 20; i++)
                    WriteImageText(image, 0xFFFF, $"FILE{i:00}.4th", ": X ;");
            });
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, 18,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x1B);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)), "Esc must cancel the scrolled picker.");
            Assert.AreEqual("..", ReadScreenWindowRow(bus, 16, 22, 2),
                "directory file lists must keep parent navigation visible even when scrolled.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_EnteringDirectoryShowsThatDirectory()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root, image =>
            {
                ushort src = CreateImageDirectory(image, 0xFFFF, "SRC");
                WriteImageText(image, src, "NEST.4th", ": NEST ;");
            });
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, 1,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x0D, 0x1F, 0x0D);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(8, bus.ReadRam(RESULT), "picker must return nested filename length");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK");
            Assert.AreEqual("NEST.4th", ReadRamAscii(bus, output, 8),
                "entering a directory must show and select files from that directory");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_SaveModePromptsForNewFilename()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root);
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;
            WriteRamString(bus, title, "Save File As");
            WriteRamString(bus, footer, "ENTER CHOOSE  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 1, 0x40, 0, 0,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueText(editor, "NEWUNIT");
            QueueKeys(editor, 0x0D);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(7, bus.ReadRam(RESULT), "save-as prompt must return typed filename length");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "ENTER must report NUI_RESULT_OK");
            Assert.AreEqual("NEWUNIT", ReadRamAscii(bus, output, 7),
                "save-as prompt must copy the typed filename to caller RAM");
            Assert.AreEqual("..", ReadRamAscii(bus, rows, 2),
                "save mode must keep parent navigation in the file list");
            Assert.AreNotEqual("Save as.", ReadRamAscii(bus, (ushort)(rows + 36), 8),
                "save mode must not fake a Save as... row inside the file list");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_ParentAtRootShowsVolumes()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateHd0Image(root);
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;
            const byte rowWidth = 36;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, 0,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x0D, 0x1B);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(0, bus.ReadRam(RESULT), "Esc after opening volume list must return no selected path bytes.");
            Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)), "Esc must cancel from the volume list.");
            Assert.AreEqual("FD0:", ReadRamAscii(bus, rows, 4),
                "pressing .. at a device root must redraw the picker as a volume list");
            Assert.AreEqual("HD1:", ReadRamAscii(bus, (ushort)(rows + 3 * rowWidth), 4),
                "volume list must expose the hard disk slots as direct choices");
            Assert.AreEqual(0x3C, ReadScreenWindowCell(bus, VGC_SCREENWIN_COLOR, 16, 22),
                "detached FD0 must stay visible but render as a disabled volume choice.");
            Assert.AreEqual(0x00, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 16, 22),
                "detached FD0 must not receive the selectable reverse-video highlight.");
            Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 18, 22),
                "selection must skip detached floppy slots and land on the first mounted volume.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_EscReportsCancel()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root);
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, 0,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x1B);
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(0, bus.ReadRam(RESULT), "Esc cancel must return no selected path bytes.");
            Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)), "Esc must report NUI_RESULT_CANCEL.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_OpenModeTabFocusesCancelButton()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root);
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;

            WriteRamString(bus, title, "Open");
            WriteRamString(bus, footer, "ENTER OPEN  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 0, 0x40, 0, 0,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueKeys(editor, 0x09, 0x09, 0x0D); // list -> OK -> Cancel, Enter
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(0, bus.ReadRam(RESULT), "Cancel must not return a selected filename.");
            Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)),
                "Tab to Cancel then Enter must report NUI_RESULT_CANCEL.");
            Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 32, 43),
                "focused Cancel button must use reverse-video text attributes.");
            Assert.AreEqual(0x00, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 32, 31),
                "OK button must not remain focused after tabbing to Cancel.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiFilePicker_SaveModeTabsFromFilenameToOk()
    {
        string? previousRoot = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        string root = Path.Combine(Path.GetTempPath(), "e6502-filepicker-" + Guid.NewGuid().ToString("N"));
        try
        {
            CreateFd0Image(root);
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", root);

            using var bus = MakeSystemBus();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            const ushort config = 0x1000;
            const ushort title = 0x1100;
            const ushort footer = 0x1120;
            const ushort output = 0x1140;
            const ushort rows = 0x1200;

            WriteRamString(bus, title, "Save File As");
            WriteRamString(bus, footer, "ENTER CHOOSE  ESC CANCEL");
            WriteRamBytes(bus, config, new byte[]
            {
                (byte)(title & 0xFF), (byte)(title >> 8),
                (byte)(output & 0xFF), (byte)(output >> 8),
                32, 1, 0x40, 0, 0,
                0, 0, 0,
                (byte)(footer & 0xFF), (byte)(footer >> 8),
                (byte)(rows & 0xFF), (byte)(rows >> 8),
            });

            QueueText(editor, "UNIT1");
            QueueKeys(editor, 0x09, 0x09, 0x0D); // filename -> list -> OK, Enter
            SetArg(bus, ARG0, config);
            RunFn(bus, SYS_NUI_FILE_PICKER);

            Assert.AreEqual(5, bus.ReadRam(RESULT), "save-as must return the typed filename length.");
            Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)),
                "Tab to OK then Enter must report NUI_RESULT_OK.");
            Assert.AreEqual("UNIT1", ReadRamAscii(bus, output, 5),
                "save-as must keep the filename typed before tab traversal.");
            Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, 32, 31),
                "focused OK button must use reverse-video text attributes.");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", previousRoot);
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void NuiTextInput_TabFocusesOkButtonAndReturnsTypedText()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x1000;
        const ushort title = 0x1100;
        const ushort label = 0x1120;
        const ushort output = 0x1140;
        const ushort footer = 0x1160;

        WriteRamString(bus, title, "Find");
        WriteRamString(bus, label, "Text:   ");
        WriteRamString(bus, footer, "TAB NEXT  ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(label & 0xFF), (byte)(label >> 8),
            (byte)(output & 0xFF), (byte)(output >> 8),
            32, 14, 12, 52, 12,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        QueueText(editor, "HELLO");
        QueueKeys(editor, 0x09, 0x0D);
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_TEXT_INPUT);

        Assert.AreEqual(5, bus.ReadRam(RESULT), "text input must return typed text length");
        Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)), "Enter on focused OK button must report NUI_RESULT_OK");
        Assert.AreEqual("HELLO", ReadRamAscii(bus, output, 5),
            "text input must copy typed bytes to caller RAM");
        Assert.AreEqual((char)0xC9, ReadScreenWindowRow(bus, 12, 14, 1)[0],
            "text input must use the shared CP437 dialog frame");
        string bottomRow = ReadScreenWindowRow(bus, 23, 14, 52);
        Assert.IsFalse(bottomRow.Contains("TAB NEXT", StringComparison.Ordinal),
            "text input must not print footer help text inside the dialog frame");
    }

    [TestMethod]
    public void NuiTextInput_AcceptsInitialOutputText()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x1000;
        const ushort title = 0x1100;
        const ushort label = 0x1120;
        const ushort output = 0x1140;
        const ushort footer = 0x1160;

        WriteRamString(bus, title, "Description");
        WriteRamString(bus, label, "Text:   ");
        WriteRamString(bus, output, "SAVED");
        WriteRamString(bus, footer, "TAB NEXT  ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(label & 0xFF), (byte)(label >> 8),
            (byte)(output & 0xFF), (byte)(output >> 8),
            32, 14, 12, 52, 12,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        QueueKeys(editor, 0x09, 0x0D, 0x1B);
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_TEXT_INPUT);

        Assert.AreEqual(5, bus.ReadRam(RESULT),
            "text input must preserve and return a caller-seeded default value.");
        Assert.AreEqual(0, bus.ReadRam((ushort)(RESULT + 1)),
            "Enter on OK must accept a caller-seeded default value.");
        Assert.AreEqual("SAVED", ReadRamAscii(bus, output, 5),
            "text input must not erase the caller's initial output text.");
    }

    [TestMethod]
    public void NuiTextInput_EscReportsCancel()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        const ushort config = 0x1000;
        const ushort title = 0x1100;
        const ushort label = 0x1120;
        const ushort output = 0x1140;
        const ushort footer = 0x1160;

        WriteRamString(bus, title, "Find");
        WriteRamString(bus, label, "Text:   ");
        WriteRamString(bus, footer, "TAB NEXT  ENTER OK  ESC CANCEL");
        WriteRamBytes(bus, config, new byte[]
        {
            (byte)(title & 0xFF), (byte)(title >> 8),
            (byte)(label & 0xFF), (byte)(label >> 8),
            (byte)(output & 0xFF), (byte)(output >> 8),
            32, 14, 12, 52, 12,
            (byte)(footer & 0xFF), (byte)(footer >> 8),
        });

        QueueText(editor, "HELLO");
        QueueKeys(editor, 0x1B);
        SetArg(bus, ARG0, config);
        RunFn(bus, SYS_NUI_TEXT_INPUT);

        Assert.AreEqual(0, bus.ReadRam(RESULT), "Esc cancel must return no committed text bytes.");
        Assert.AreEqual(1, bus.ReadRam((ushort)(RESULT + 1)), "Esc must report NUI_RESULT_CANCEL.");
    }

    /// <summary>
    /// Buffer-based reader: it does NOT read the screen — it returns exactly the keys
    /// the user types, ignoring whatever stale text is on the cursor row. (The old
    /// C64 read-the-row-under-the-cursor behavior was removed: its walk-up WAS the
    /// Arty "wedge", ingesting a prior program's leftover wrapped screen text.)
    /// </summary>
    [TestMethod]
    public void ScreenReadline_TypedLineIgnoresPrePaintedScreen()
    {
        using var bus = MakeSystemBus();

        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        // Pre-paint stale text on the cursor row; the buffer-based reader must ignore
        // it (it never reads the screen — it returns only the keys typed).
        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 12;
        WriteScreenWindowRow(bus, row, 0, "STALETEXT");

        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        // Caller buffer in low RAM (page 2, like BASIC's Ibuffs), poisoned so an
        // untouched trailing byte is observable. b2 = max length $7F.
        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));   // b0/b1 = ptr, b2 = maxlen

        QueueText(editor, "PRINT 7");
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        // The submitted line is the TYPED "PRINT 7" (7 chars), not the pre-painted text.
        Assert.AreEqual(7, bus.ReadRam(RESULT), "RESULT b0 must be the typed line length (7)");
        var got = new char[7];
        for (int i = 0; i < 7; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("PRINT 7", new string(got), "buffer holds the typed line, not the pre-painted screen text");
        Assert.AreEqual(0xAA, bus.ReadRam((ushort)(buf + 7)),
            "no trailing NUL or padding: the byte past the typed line must be untouched");
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
    /// Buffer-based reader: arrow keys (28-31, control bytes &lt; $20) are ignored —
    /// the reader accumulates only printable keys. Typing "AB", then arrows, then "C"
    /// yields "ABC" in order. (In-place arrow cursor editing was part of the removed
    /// C64 screen-reader, whose screen walk-up was the Arty wedge.)
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ArrowsIgnoredByBufferReader()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        for (int r = 0; r < ScreenRows; r++) BlankScreenWindowRow(bus, r);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, 0);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueText(editor, "AB");
        QueueKeys(editor, 30, 28, 29, 31);   // Up/Left/Right/Down -- all ignored
        QueueText(editor, "C");
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(3, bus.ReadRam(RESULT), "only the 3 printable keys are buffered");
        var got = new char[3];
        for (int i = 0; i < 3; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("ABC", new string(got), "arrows are ignored; buffer holds the typed chars in order");
    }

    /// <summary>
    /// Task 4 — clamp at the top-left corner: Up and Left from (0,0) stay at (0,0),
    /// proven by a typed marker landing at cell (0,0).
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ArrowsClampAtTopLeft()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        BlankScreenWindowRow(bus, 0);
        BlankScreenWindowRow(bus, 1);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, 0);

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueKeys(editor, 30, 28);   // Up, Left -- both clamp at the corner
        editor.QueueInput((byte)'X');
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual("X", ReadScreenWindowRow(bus, 0, 0, 1),
            "Up/Left clamp at (0,0): the marker char proves it");
    }

    /// <summary>
    /// Task 5 — destructive backspace erases the char before the cursor and moves
    /// left. Type "PRIM", backspace (drops the M), then "NT" -> "PRINT" on screen
    /// and in the buffer.
    /// (The plan's "PRIMT"+BS+"NT" is a typo; that yields "PRIMNT". "PRIM"+BS+"NT"
    /// is the input that genuinely exercises a destructive backspace into "PRINT".)
    /// </summary>
    [TestMethod]
    public void ScreenReadline_BackspaceErasesAndMovesLeft()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 6;
        BlankScreenWindowRow(bus, row);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueText(editor, "PRIM");
        editor.QueueInput(0x08);     // backspace -> erase M, cursor back to col 3
        QueueText(editor, "NT");
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual("PRINT", ReadScreenWindowRow(bus, row, 0, 5),
            "destructive backspace must leave PRINT on the screen window");
        Assert.AreEqual(5, bus.ReadRam(RESULT), "returned line length is 5 (PRINT)");
        var got = new char[5];
        for (int i = 0; i < 5; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("PRINT", new string(got), "buffer must hold PRINT");
    }

    /// <summary>
    /// Task 6 — after ENTER on a row above the bottom, the cursor advances to
    /// column 0 of the next row (via the VGC's own CR/LF, the same path BASIC's
    /// output uses).
    /// </summary>
    [TestMethod]
    public void ScreenReadline_EnterAdvancesToNextLine()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 10;
        BlankScreenWindowRow(bus, row);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueText(editor, "AB");
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(0, bus.Read(VGC_CURSX), "cursor returns to column 0 after ENTER");
        Assert.AreEqual(row + 1, bus.Read(VGC_CURSY), "cursor advances to the next row after ENTER");
    }

    /// <summary>
    /// Task 6 — ENTER on the bottom row scrolls the screen up one row through the
    /// VGC's own ScrollUp (the same mechanism BASIC's output uses), leaving the
    /// cursor at (0, last row) and the prior content shifted up by one physical row.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ScrollsAtBottom()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        int last = ScreenRows - 1;           // physical bottom row = 49
        BlankScreenWindowRow(bus, last - 1); // row 48
        BlankScreenWindowRow(bus, last);     // row 49
        WriteScreenWindowRow(bus, last - 1, 0, "LINE48");
        WriteScreenWindowRow(bus, last, 0, "LINE49");

        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)last);    // cursor on the bottom row

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        editor.QueueInput(0x0D);             // submit row 49

        RunFn(bus, SYS_SCREEN_READLINE);

        // The submitted row was read correctly.
        Assert.AreEqual("LINE49", ReadScreenWindowRow(bus, last - 1, 0, 6),
            "after scroll, the submitted bottom row is now one row up (48)");
        Assert.AreEqual("LINE48", ReadScreenWindowRow(bus, last - 2, 0, 6),
            "after scroll, the prior row 48 content moved up to row 47");
        Assert.AreEqual("      ", ReadScreenWindowRow(bus, last, 0, 6),
            "the newly exposed bottom row is cleared to spaces");
        // Cursor stays at the bottom row, column 0.
        Assert.AreEqual(0, bus.Read(VGC_CURSX), "cursor at column 0 after scroll");
        Assert.AreEqual(last, bus.Read(VGC_CURSY), "cursor stays on the bottom row after scroll");
    }

    /// <summary>
    /// Multi-row read: a logical line wider than 80 columns wraps onto a second
    /// physical row (its first row is "full" — col 79 non-space). On ENTER, with the
    /// cursor on the SECOND row, SYS_SCREEN_READLINE walks UP to the line's start row
    /// and reads the whole wrapped line back into the buffer as one logical line.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_EnterReadsFullWrappedMultiRowLine()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int startRow = 10;
        for (int r = startRow - 1; r <= startRow + 2; r++) BlankScreenWindowRow(bus, r);
        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)startRow);

        // A 100-char line: the echo wraps across rows via VGC_CHAROUT, but the
        // buffer-based reader accumulates ALL 100 typed chars (maxlen $7F=127).
        var line = new System.Text.StringBuilder(100);
        for (int i = 0; i < 100; i++)
            line.Append((char)('A' + (i % 26)));
        string expected = line.ToString();

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA); // poison
        SetArg(bus, ARG0, buf | (0x7F << 16));   // b0/b1 = ptr, b2 = maxlen ($7F)

        QueueText(editor, expected);
        editor.QueueInput(0x0D);
        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(100, bus.ReadRam(RESULT), "RESULT b0 must be the full 100-char typed-line length");
        var got = new char[100];
        for (int i = 0; i < 100; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual(expected, new string(got), "buffer must hold the full typed line, in order");
        Assert.AreEqual(0xAA, bus.ReadRam((ushort)(buf + 100)),
            "no padding past the line: the byte after the 100th char must be untouched");
    }

    /// <summary>
    /// Regression for the single-row case under the multi-row reader: a line that
    /// does NOT wrap (row-above is not full, this row's col 79 is a space) must read
    /// as exactly one row. Guards against the walk-up over-climbing into a prior
    /// full row, or the forward read over-running into the next row.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_EnterReadsSingleRowWhenNotWrapped()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        const int row = 15;
        BlankScreenWindowRow(bus, row - 1);
        BlankScreenWindowRow(bus, row);
        BlankScreenWindowRow(bus, row + 1);
        WriteScreenWindowRow(bus, row - 1, 0, "PRIOR LINE");   // stale; must be ignored

        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        QueueText(editor, "GOTO 100");
        editor.QueueInput(0x0D);
        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(8, bus.ReadRam(RESULT), "a short typed line is exactly its 8 chars");
        var got = new char[8];
        for (int i = 0; i < 8; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("GOTO 100", new string(got), "buffer holds the typed line, not the prior screen line");
        Assert.AreEqual(0xAA, bus.ReadRam((ushort)(buf + 8)), "no over-read / padding past the typed line");
    }

    /// <summary>
    /// Echo at the bottom row: typing a line wider than 80 columns when the cursor is
    /// on the LAST physical row must SCROLL the screen up and continue the wrapped
    /// continuation on the freshly exposed bottom row — not clamp and overwrite the
    /// same row. This is the bug that dropped multi-row program lines typed at the
    /// bottom of a scrolled screen (e.g. invention8 line 370). After ENTER the whole
    /// wrapped line reads back intact across rows 48+49.
    /// </summary>
    [TestMethod]
    public void ScreenReadline_BottomRowWrapScrollsAndKeepsFullLine()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        int last = ScreenRows - 1;          // 49
        for (int r = 0; r < ScreenRows; r++) BlankScreenWindowRow(bus, r);

        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)last);   // start typing on the bottom row

        // 90-char line: 80 fill the bottom row (forcing a wrap+scroll), 10 continue.
        var line = new System.Text.StringBuilder(90);
        for (int i = 0; i < 90; i++) line.Append((char)('0' + (i % 10)));
        string expected = line.ToString();
        QueueText(editor, expected);
        editor.QueueInput(0x0D);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(90, bus.ReadRam(RESULT),
            "the full 90-char line survives the bottom-row wrap-scroll");
        var got = new char[90];
        for (int i = 0; i < 90; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual(expected, new string(got),
            "buffer holds the full wrapped line typed across the scroll boundary");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }

    private static void CreateFd0Image(string root, Action<NdiImage>? seed = null)
        => CreateDiskImage(root, "fd0.ndi", "FD0", seed);

    private static void CreateHd0Image(string root, Action<NdiImage>? seed = null)
        => CreateDiskImage(root, "hd0.ndi", "HD0", seed);

    private static void CreateDiskImage(string root, string imageName, string label, Action<NdiImage>? seed)
    {
        string disks = Path.Combine(root, "disks");
        Directory.CreateDirectory(disks);
        string imagePath = Path.Combine(disks, imageName);
        NdiImage.CreateFormatted(imagePath, label, 800);
        if (seed is null)
            return;

        using var image = NdiImage.Open(imagePath);
        seed(image);
    }

    private static ushort CreateImageDirectory(NdiImage image, ushort parent, string name)
        => (ushort)image.MakeDirectory(name, parent);

    private static void WriteImageText(NdiImage image, ushort parent, string filename, string text)
        => image.WriteFile(filename, ImageTypeFor(filename), parent, System.Text.Encoding.ASCII.GetBytes(text));

    private static NdiFileType ImageTypeFor(string filename)
    {
        string ext = Path.GetExtension(filename);
        if (ext.Equals(".bas", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Bas;
        if (ext.Equals(".pas", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Pascal;
        if (ext.Equals(".logo", StringComparison.OrdinalIgnoreCase) || ext.Equals(".lgo", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Logo;
        if (ext.Equals(".4th", StringComparison.OrdinalIgnoreCase) || ext.Equals(".fth", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Forth;
        if (ext.Equals(".asm", StringComparison.OrdinalIgnoreCase) || ext.Equals(".s", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Assembly;
        if (ext.Equals(".sid", StringComparison.OrdinalIgnoreCase)) return NdiFileType.Sid;
        return NdiFileType.Bin;
    }
}
