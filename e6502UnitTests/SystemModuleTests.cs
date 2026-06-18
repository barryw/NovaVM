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
///   $C007  fn count      ($19  retired EDIT slot + WAIT/WAITVBL/TIMER + RNG8/16/32 +
///                              DIALOG_DEFAULTS/DIALOG/DIALOG_WAIT/DIALOG_ERROR/WAIT_KEY +
///                              OVL_LOAD/UNLOAD/INIT/MAIN/TICK + ADDR_LOOKUP + SCREEN_READLINE +
///                              NUI save-under/picker/full save-under/style)
/// The header bytes are defined by runtime/asm/libmod.inc + libabi.inc + libsystem.inc;
/// this is the byte-exact guard the loader (lib_call) depends on when paging SYSTEM.
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
        Assert.AreEqual(0x19, img[7]);   // SYS_FN_COUNT (retired EDIT slot + WAIT/WAITVBL/TIMER + RNG + DIALOG + OVL + ADDR_LOOKUP + SCREEN_READLINE + NUI save-under + picker + full save-under + style)
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
        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
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
        Assert.AreEqual("SAVE 1", ReadScreenWindowRow(bus, top + 4, left + 2, 6),
            "the second row must be rendered after moving down once");
        Assert.AreEqual(0x02, ReadScreenWindowCell(bus, VGC_SCREENWIN_ATTR, top + 4, left + 2),
            "the active row must use reverse-video text attributes");
        Assert.AreEqual("      ", ReadScreenWindowRow(bus, top + height - 3, left + 2, 6),
            "picker must reserve a blank spacer row between list items and footer instructions");
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
    /// Up,Up,Left,Right,Down -> (5,4). The arrow-driven cursor position is verified
    /// by typing a marker char and asserting the cell it lands in (ENTER's CR/LF
    /// advance, added in Task 6, would otherwise mask the post-arrow cursor regs).
    /// </summary>
    [TestMethod]
    public void ScreenReadline_ArrowsMoveCursorClamped()
    {
        using var bus = MakeSystemBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        bus.Write(VGC_SCREENWIN_PLANE, VGC_SCREENWIN_CHAR);
        for (int r = 0; r < ScreenRows; r++) BlankScreenWindowRow(bus, r);
        bus.Write(VGC_CURSX, 5);
        bus.Write(VGC_CURSY, 5);

        const ushort buf = 0x0275;
        SetArg(bus, ARG0, buf | (0x7F << 16));

        // 30=Up, 28=Left, 29=Right, 31=Down. (5,5)->Up(5,4)->Up(5,3)->Left(4,3)
        // ->Right(5,3)->Down(5,4). A typed 'X' must land at cell (5,4).
        QueueKeys(editor, 30, 30, 28, 29, 31);
        editor.QueueInput((byte)'X');
        editor.QueueInput(0x0D);

        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual("X", ReadScreenWindowRow(bus, 4, 5, 1),
            "arrows must land the cursor at (5,4): the marker char proves it");
        Assert.AreEqual(" ", ReadScreenWindowRow(bus, 5, 5, 1),
            "the starting cell (5,5) must be untouched");
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
        const int startRow = 10;          // logical line starts here
        BlankScreenWindowRow(bus, startRow - 1); // row above the line is NOT full
        BlankScreenWindowRow(bus, startRow);
        BlankScreenWindowRow(bus, startRow + 1);

        // A 100-char logical line: 80 chars fill row 10 (col 79 non-space => "full"
        // => wraps), the remaining 20 land on row 11. Built so the content is unique
        // per column to catch any mis-ordered assembly.
        var line = new System.Text.StringBuilder(100);
        for (int i = 0; i < 100; i++)
            line.Append((char)('A' + (i % 26)));
        string expected = line.ToString();
        WriteScreenWindowRow(bus, startRow, 0, expected.Substring(0, ScreenCols));   // 80 chars
        WriteScreenWindowRow(bus, startRow + 1, 0, expected.Substring(ScreenCols));  // 20 chars

        // Cursor on the SECOND row of the wrapped line (where the user would land
        // after typing past col 79). ENTER must still ingest the whole line.
        bus.Write(VGC_CURSX, 20);
        bus.Write(VGC_CURSY, (byte)(startRow + 1));

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA); // poison
        SetArg(bus, ARG0, buf | (0x7F << 16));   // b0/b1 = ptr, b2 = maxlen ($7F)

        editor.QueueInput(0x0D);
        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(100, bus.ReadRam(RESULT), "RESULT b0 must be the full 100-char wrapped-line length");
        var got = new char[100];
        for (int i = 0; i < 100; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual(expected, new string(got), "buffer must hold the full wrapped logical line, in order");
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
        // The row above holds an unrelated short line (NOT full: col 79 is a space),
        // so the walk-up must stop at the cursor row and the read must NOT merge the
        // prior line. The cursor row's own col 79 is a space too, so the forward read
        // must stop after this single row (no over-run into row 16).
        BlankScreenWindowRow(bus, row - 1);
        BlankScreenWindowRow(bus, row);
        BlankScreenWindowRow(bus, row + 1);
        WriteScreenWindowRow(bus, row - 1, 0, "PRIOR LINE");
        WriteScreenWindowRow(bus, row, 0, "GOTO 100");

        bus.Write(VGC_CURSX, 0);
        bus.Write(VGC_CURSY, (byte)row);

        const ushort buf = 0x0275;
        for (int i = 0; i < 0x80; i++) bus.WriteRam((ushort)(buf + i), 0xAA);
        SetArg(bus, ARG0, buf | (0x7F << 16));

        editor.QueueInput(0x0D);
        RunFn(bus, SYS_SCREEN_READLINE);

        Assert.AreEqual(8, bus.ReadRam(RESULT), "single non-wrapped row reads exactly its 8 chars");
        var got = new char[8];
        for (int i = 0; i < 8; i++) got[i] = (char)bus.ReadRam((ushort)(buf + i));
        Assert.AreEqual("GOTO 100", new string(got), "buffer holds only the cursor row, not the prior line");
        Assert.AreEqual(0xAA, bus.ReadRam((ushort)(buf + 8)), "no over-read past the single row");
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
}
