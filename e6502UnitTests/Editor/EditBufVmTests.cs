using System;
using System.IO;
using System.Linq;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

/// <summary>
/// Logo-independent execution tests for the shared editbuf engine. They load
/// the standalone harness app (assembly/apps/editbuf_test) into RAM, run it on
/// the real VGC, drive keystrokes, and read text/color VRAM plus the harness's
/// observable RAM cells. No Logo, no bank switching, no document model.
///
/// The editor renders incrementally (title first, body last), so tests queue
/// keys then "settle" a fixed number of cycles before asserting on the final
/// stable screen/buffer state rather than polling an intermediate signal.
/// </summary>
[TestClass]
public class EditBufVmTests
{
    private const ushort AppEntry = 0x5000;
    private const ushort TextBuf = 0x0400;
    private const ushort TestResult = 0x0300;
    private const ushort TestDone = 0x0301;
    private const ushort TestSaveCnt = 0x0302;
    private const ushort TestSaveRet = 0x0303;
    private const ushort TestSaveLenL = 0x0304;
    private const ushort TestIndent = 0x0306;
    private const ushort TestHlMark = 0x0307;
    private const ushort TestSeedLenL = 0x0308;   // nonzero => harness uses test-injected buffer
    private const ushort TestSeedLenH = 0x0309;
    private const ushort TestCmdCnt = 0x030A;
    private const ushort TestLastCmd = 0x030B;
    private const ushort TestDocMode = 0x030C;
    private const ushort TestActiveDoc = 0x030D;
    private const ushort TestDocFullLenL = 0x030E;
    private const ushort TestWindowLenL = 0x0310;
    private const ushort TestWindowOffL = 0x0312;
    private const ushort TestMenuMode = 0x0314;
    private const ushort TestHlCount = 0x0315;
    private const int DocbufXramBase = 0x055000;

    private const byte CtrlA = 0x01, CtrlB = 0x02, CtrlC = 0x03, CtrlF = 0x06, CtrlG = 0x07, CtrlK = 0x0B, CtrlN = 0x0E, CtrlO = 0x0F, CtrlQ = 0x11, CtrlS = 0x13, CtrlV = 0x16, CtrlX = 0x18, CtrlY = 0x19, CtrlZ = 0x1A;
    private const byte KeyLeft = 0x1C, KeyRight = 0x1D, KeyUp = 0x1E, KeyDown = 0x1F, KeyBackspace = 0x08, KeyEnter = 0x0D;
    private const byte KeyEsc = 0x1B, KeyCtrlHome = 0x80, KeyCtrlEnd = 0x81, KeyF3 = 0x82, KeyHome = 0x83, KeyF6 = 0x84, KeyShiftF6 = 0x85;
    private const byte CmdOpen = 0x02, CmdBufferNext = 0x14, CmdBufferPrevious = 0x15, CmdBufferList = 0x16;

    private const byte ExitQuit = 0x00, ExitSaved = 0x01, ExitDiscard = 0x02;
    private const byte SaveOk = 0x00, SaveError = 0x02;

    // Body view origin (EDITBUF_VIEW_LEFT / EDITBUF_VIEW_TOP).
    // New layout: row 0 = menu bar, rows 1..48 = body (80 cols), row 49 = status bar.
    private const int ViewLeft = 0, ViewTop = 1;
    private const int MenuRow = 0;
    private const int DirtyCol = VgcConstants.ScreenCols - 2;   // '*' marker cell on the menu bar (78)

    // Cycles allowed for the editor to consume queued keys and fully re-render.
    private const int Settle = 2_500_000;

    private sealed record Harness(CompositeBusDevice Bus, Cpu Cpu, ScreenEditor Editor);
    private sealed record SourceCase(string FileName, NdiFileType FileType, string Text, string Needle, string Replacement, int Line, int Col);

    private static Harness Boot(byte saveRet = SaveOk, byte indent = 0, byte hlMark = 0x1F,
                               byte[]? seedText = null, bool docMode = false, bool windowedDocMode = false,
                               bool testMenuMode = false)
    {
        var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        LoadApp(bus);
        bus.WriteRam(TestSaveRet, saveRet);
        bus.WriteRam(TestIndent, indent);
        bus.WriteRam(TestHlMark, hlMark);
        bus.WriteRam(TestDone, 0x00);
        bus.WriteRam(TestSaveCnt, 0x00);
        bus.WriteRam(TestCmdCnt, 0x00);
        bus.WriteRam(TestLastCmd, 0x00);
        bus.WriteRam(TestDocMode, windowedDocMode ? (byte)0x02 : docMode ? (byte)0x01 : (byte)0x00);
        bus.WriteRam(TestActiveDoc, 0x00);
        bus.WriteRam(TestDocFullLenL, 0x00);
        bus.WriteRam((ushort)(TestDocFullLenL + 1), 0x00);
        bus.WriteRam(TestMenuMode, testMenuMode ? (byte)0x01 : (byte)0x00);
        bus.WriteRam(TestHlCount, 0x00);
        if (seedText != null)
        {
            for (int i = 0; i < seedText.Length; i++)
                bus.WriteRam((ushort)(TextBuf + i), seedText[i]);
            bus.WriteRam(TestSeedLenL, (byte)(seedText.Length & 0xFF));
            bus.WriteRam(TestSeedLenH, (byte)(seedText.Length >> 8));
        }
        var cpu = new Cpu(bus);
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        cpu.Boot(AppEntry);
        var h = new Harness(bus, cpu, editor);
        Step(h, Settle); // let the initial shell + body render
        return h;
    }

    private static void LoadApp(CompositeBusDevice bus)
    {
        string path = Path.Combine(RepoRoot(), "software", "assembly", "apps", "editbuf_test", "editbuf_test.bin");
        Assert.IsTrue(File.Exists(path), $"Build the harness first: {path}");

        // The harness links the editor out of nova.lib. A stale binary makes
        // every test here judge code that is no longer in the tree, which is
        // worse than a failure: it passes or fails for the wrong reason.
        string library = Path.Combine(RepoRoot(), "software", "runtime", "asm", "build", "nova.lib");
        if (File.Exists(library))
            Assert.IsTrue(File.GetLastWriteTimeUtc(path) >= File.GetLastWriteTimeUtc(library),
                $"{path} is older than nova.lib; rebuild it with " +
                "'make -C software/assembly editbuf-test' before trusting these tests.");
        byte[] prg = File.ReadAllBytes(path);
        ushort load = (ushort)(prg[0] | (prg[1] << 8));
        for (int i = 2; i < prg.Length; i++)
            bus.WriteRam((ushort)(load + i - 2), prg[i]);
    }

    private static string RepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir, "software", "runtime", "asm", "editbuf.s")))
                return dir;
            dir = Path.GetDirectoryName(dir);
        }
        throw new InvalidOperationException("Could not locate repo root (runtime/asm/editbuf.s).");
    }

    private static void Type(Harness h, params byte[] keys)
    {
        foreach (byte k in keys)
            h.Editor.QueueInput(k);
        Step(h, Settle);
    }

    private static void TypeText(Harness h, string text)
    {
        foreach (char ch in text)
            h.Editor.QueueInput((byte)ch);
        Step(h, Settle);
    }

    private static void Step(Harness h, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = h.Cpu.ClocksForNext();
            h.Cpu.ExecuteNext();
            h.Bus.AdvanceCycles(cycles);
        }
    }

    private static void WaitFrames(Harness h, int frames)
    {
        long target = h.Bus.TotalFrames + frames;
        int cyclesPerFrame = h.Bus.CpuHz / h.Bus.FrameRateHz;
        for (int i = 0; i < frames; i++)
        {
            h.Bus.AdvanceCycles(cyclesPerFrame);
            Step(h, 256);
        }
        Assert.IsTrue(h.Bus.TotalFrames >= target, $"Timed out waiting for {frames} frames.");
    }

    private static void RunUntilDone(Harness h, int maxSteps = 12_000_000)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = h.Cpu.ClocksForNext();
            h.Cpu.ExecuteNext();
            h.Bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && h.Bus.ReadRam(TestDone) == 0xAA)
                return;
        }
        Assert.Fail($"Editor did not exit.\n{Snapshot(h)}");
    }

    private static int PhysRow(Harness h, int row) => h.Bus.Vgc.PhysicalTextRow(row);

    private static char ChAt(Harness h, int col, int row) =>
        (char)h.Bus.Vgc.GetScreenChar(col, PhysRow(h, row));

    private static byte ColorAt(Harness h, int col, int row) =>
        h.Bus.Vgc.GetScreenColor(col, PhysRow(h, row));

    private static string Row(Harness h, int row)
    {
        var sb = new StringBuilder();
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            byte ch = h.Bus.Vgc.GetScreenChar(col, PhysRow(h, row));
            sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
        }
        return sb.ToString();
    }

    private static string Snapshot(Harness h)
    {
        var sb = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            sb.Append(Row(h, row)).Append('\n');
        return sb.ToString();
    }

    private static string SavedBuffer(Harness h)
    {
        int len = h.Bus.ReadRam(TestSaveLenL) | (h.Bus.ReadRam((ushort)(TestSaveLenL + 1)) << 8);
        var bytes = new byte[len];
        for (int i = 0; i < len; i++)
            bytes[i] = h.Bus.ReadRam((ushort)(TextBuf + i));
        return Encoding.ASCII.GetString(bytes);
    }

    private static int SavedLength(Harness h) =>
        h.Bus.ReadRam(TestSaveLenL) | (h.Bus.ReadRam((ushort)(TestSaveLenL + 1)) << 8);

    private static int WindowLength(Harness h) =>
        h.Bus.ReadRam(TestWindowLenL) | (h.Bus.ReadRam((ushort)(TestWindowLenL + 1)) << 8);

    private static int FullDocumentLength(Harness h) =>
        h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);

    private static int WindowOffset(Harness h) =>
        h.Bus.ReadRam(TestWindowOffL) | (h.Bus.ReadRam((ushort)(TestWindowOffL + 1)) << 8);

    private static bool ScreenContains(Harness h, string marker) =>
        Snapshot(h).Contains(marker, StringComparison.Ordinal);

    // Box-drawing glyphs used by the (now-removed) frame/title band: single
    // ($B3,$C0,$C4,$D9,$DA,$BF) and double ($BA,$BB,$BC,$C8,$C9,$CD) lines.
    private static readonly byte[] FrameChars =
        { 0xB3, 0xC0, 0xC4, 0xD9, 0xDA, 0xBF, 0xBA, 0xBB, 0xBC, 0xC8, 0xC9, 0xCD };

    private static bool HasFrameChars(Harness h)
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = h.Bus.Vgc.GetScreenChar(col, PhysRow(h, row));
                if (Array.IndexOf(FrameChars, ch) >= 0)
                    return true;
            }
        return false;
    }

    // Dirty marker is a single '*' cell on the menu bar (right side), shown only
    // when the buffer is dirty.
    private static bool TitleDirty(Harness h) => ChAt(h, DirtyCol, MenuRow) == '*';

    private static byte CursorX(Harness h) => h.Bus.Read((ushort)VgcConstants.RegCursorX);

    private static byte CursorY(Harness h) => h.Bus.Read((ushort)VgcConstants.RegCursorY);

    // -----------------------------------------------------------------

    [TestMethod]
    public void InitialRenderShowsTitleAndBuffer()
    {
        var h = Boot();
        // Title and menu share the menu bar (row 0); no separate title band.
        Assert.IsTrue(Row(h, MenuRow).Contains("SQUARE", StringComparison.Ordinal),
            $"Title 'SQUARE' should appear on the menu bar (row 0).\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, MenuRow).Contains("File", StringComparison.Ordinal),
            $"Menu bar should render File/Edit/Search.\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, MenuRow).Contains("Search", StringComparison.Ordinal),
            $"Menu bar should render File/Edit/Search.\n{Snapshot(h)}");
        // Body now starts at row 1, col 0 (no frame, no title band in between).
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), Snapshot(h));
        Assert.AreEqual('B', ChAt(h, ViewLeft + 1, ViewTop), Snapshot(h));
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), Snapshot(h));
        Assert.AreEqual('D', ChAt(h, ViewLeft + 1, ViewTop + 1), Snapshot(h));
        // No box-drawing frame characters anywhere (the middle chrome is gone).
        Assert.IsFalse(HasFrameChars(h),
            $"Editor must have no frame/box characters.\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, MenuRow).Contains("test/SQUARE.4th", StringComparison.Ordinal),
            $"Title should show parent directory plus filename, not the whole path.\n{Snapshot(h)}");
        Assert.IsFalse(Row(h, MenuRow).Contains("forth/test", StringComparison.Ordinal),
            $"Title should not show the full source path.\n{Snapshot(h)}");
        string status = Row(h, VgcConstants.ScreenRows - 1);
        Assert.IsTrue(status.Contains("X:1 Y:1", StringComparison.Ordinal), status);
        Assert.IsTrue(status.Contains("B:5", StringComparison.Ordinal), status);
        Assert.IsTrue(status.Contains("L:2", StringComparison.Ordinal), status);
        Assert.IsTrue(status.Contains("T:Test", StringComparison.Ordinal), status);
        Assert.IsFalse(status.Contains("Ctrl-S", StringComparison.Ordinal), status);
        Assert.IsFalse(TitleDirty(h), "Freshly loaded buffer must not show the dirty marker.");
    }

    [TestMethod]
    public void TypingInsertsCharacterAndMarksDirty()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Inserted char should be at buffer start.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop), $"Body should show X first.\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, VgcConstants.ScreenRows - 1).Contains("B:6", StringComparison.Ordinal),
            $"Status bar should update file size after edits.\n{Snapshot(h)}");
        Assert.IsTrue(TitleDirty(h), "Editing should set the shared dirty marker.");
    }

    [TestMethod]
    public void CtrlSCallsSaveHookAndClearsDirtyOnOk()
    {
        var h = Boot(saveRet: SaveOk);
        Type(h, (byte)'X');
        Assert.IsTrue(TitleDirty(h), "Pre-condition: dirty after edit.");
        Type(h, CtrlS);
        Assert.AreEqual(1, h.Bus.ReadRam(TestSaveCnt), "Save hook should fire exactly once.");
        Assert.AreEqual(6, h.Bus.ReadRam(TestSaveLenL), "Save hook should see the post-edit length (5+1).");
        Assert.IsFalse(TitleDirty(h), $"A successful save should clear the dirty marker.\n{Snapshot(h)}");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop), $"Ctrl-S must not blank the edited body.\n{Snapshot(h)}");
        Assert.AreEqual('A', ChAt(h, ViewLeft + 1, ViewTop), $"Ctrl-S must preserve the original body text.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlSShowsTransientSaveStatusThenRestoresMetadata()
    {
        var h = Boot(saveRet: SaveOk);
        Type(h, (byte)'X');

        Type(h, CtrlS);

        string saved = Row(h, VgcConstants.ScreenRows - 1);
        Assert.IsTrue(saved.Contains("Saved", StringComparison.Ordinal),
            $"Ctrl-S should show an editor-owned transient save message before returning to metadata.\n{Snapshot(h)}");

        WaitFrames(h, 620);

        string restored = Row(h, VgcConstants.ScreenRows - 1);
        Assert.IsFalse(restored.Contains("Saved", StringComparison.Ordinal), restored);
        Assert.IsTrue(restored.Contains("X:2 Y:1", StringComparison.Ordinal), restored);
        Assert.IsTrue(restored.Contains("B:6", StringComparison.Ordinal), restored);
        Assert.IsTrue(restored.Contains("T:Test", StringComparison.Ordinal), restored);
    }

    [TestMethod]
    public void CtrlNAndCtrlOInvokeHostDocumentCommandHook()
    {
        var h = Boot();

        Type(h, CtrlN);
        Assert.AreEqual(1, h.Bus.ReadRam(TestCmdCnt), "Ctrl-N must delegate New to the host command hook.");
        Assert.AreEqual(0x01, h.Bus.ReadRam(TestLastCmd), "Ctrl-N must report EDITUI_CMD_NEW.");

        Type(h, CtrlO);
        Assert.AreEqual(2, h.Bus.ReadRam(TestCmdCnt), "Ctrl-O must delegate Open to the host command hook.");
        Assert.AreEqual(0x02, h.Bus.ReadRam(TestLastCmd), "Ctrl-O must report EDITUI_CMD_OPEN.");
    }

    [TestMethod]
    public void BufferSwitchKeysInvokeHostDocumentCommandHook()
    {
        var h = Boot();

        Type(h, KeyF6);
        Assert.AreEqual(1, h.Bus.ReadRam(TestCmdCnt), "F6 must delegate next-buffer to the host command hook.");
        Assert.AreEqual(CmdBufferNext, h.Bus.ReadRam(TestLastCmd), "F6 must report EDITUI_CMD_BUFFER_NEXT.");

        Type(h, KeyShiftF6);
        Assert.AreEqual(2, h.Bus.ReadRam(TestCmdCnt), "Shift-F6 must delegate previous-buffer to the host command hook.");
        Assert.AreEqual(CmdBufferPrevious, h.Bus.ReadRam(TestLastCmd), "Shift-F6 must report EDITUI_CMD_BUFFER_PREVIOUS.");

        Type(h, CtrlB);
        Assert.AreEqual(3, h.Bus.ReadRam(TestCmdCnt), "Ctrl-B must delegate buffer-list to the host command hook.");
        Assert.AreEqual(CmdBufferList, h.Bus.ReadRam(TestLastCmd), "Ctrl-B must report EDITUI_CMD_BUFFER_LIST.");

        Type(h, KeyEsc, (byte)'0');
        Assert.AreEqual(4, h.Bus.ReadRam(TestCmdCnt), "Alt-0 must delegate buffer-list to the host command hook.");
        Assert.AreEqual(CmdBufferList, h.Bus.ReadRam(TestLastCmd), "Alt-0 must report EDITUI_CMD_BUFFER_LIST.");
    }

    [TestMethod]
    public void PhysicalHomeStillMovesToLineStartAfterCtrlBIsReservedForBuffers()
    {
        var h = Boot();

        Type(h, KeyRight, KeyRight, KeyHome, (byte)'Z', CtrlS);

        Assert.AreEqual("ZAB\nCD", SavedBuffer(h),
            $"Home must keep moving to line start after Ctrl-B is assigned to buffer list.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void F6SwitchesToNextXramDocBuffer()
    {
        var h = Boot(docMode: true);

        Type(h, KeyF6, CtrlS);

        Assert.AreEqual(1, h.Bus.ReadRam(TestActiveDoc), "F6 must make the next XRAM document active.");
        Assert.AreEqual("TWO\n", SavedBuffer(h),
            $"Save after F6 must operate on the newly active document.\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, MenuRow).Contains("TWO.LOGO", StringComparison.Ordinal),
            $"The title should follow the active XRAM document.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void ShiftF6WrapsToPreviousXramDocBuffer()
    {
        var h = Boot(docMode: true);

        Type(h, KeyShiftF6, CtrlS);

        Assert.AreEqual(2, h.Bus.ReadRam(TestActiveDoc), "Shift-F6 at the first buffer must wrap to the last XRAM document.");
        Assert.AreEqual("THREE\n", SavedBuffer(h),
            $"Save after wrapped Shift-F6 must operate on the last document.\n{Snapshot(h)}");
        Assert.IsTrue(Row(h, MenuRow).Contains("THREE.4th", StringComparison.Ordinal),
            $"The title should follow the wrapped XRAM document.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void BufferListDialogSwitchesSelectedXramDocBuffer()
    {
        var h = Boot(docMode: true);

        Type(h, CtrlB);
        Assert.IsTrue(ScreenContains(h, "Open Buffers"),
            $"Ctrl-B must open the shared buffer list dialog.\n{Snapshot(h)}");
        Assert.IsTrue(ScreenContains(h, "ONE.PAS"), $"The buffer list should include Pascal source.\n{Snapshot(h)}");
        Assert.IsTrue(ScreenContains(h, "TWO.LOGO"), $"The buffer list should include Logo source.\n{Snapshot(h)}");

        Type(h, KeyDown, KeyEnter, CtrlS);

        Assert.AreEqual(1, h.Bus.ReadRam(TestActiveDoc), "Selecting row 2 should activate the second XRAM document.");
        Assert.AreEqual("TWO\n", SavedBuffer(h),
            $"Save after selecting from the buffer list must operate on the chosen document.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void BufferListEscKeepsCurrentXramDocBuffer()
    {
        var h = Boot(docMode: true);

        Type(h, CtrlB);
        Assert.IsTrue(ScreenContains(h, "Open Buffers"),
            $"Ctrl-B must open the shared buffer list dialog.\n{Snapshot(h)}");

        Type(h, KeyEsc);
        Type(h, CtrlS);

        Assert.AreEqual(0, h.Bus.ReadRam(TestActiveDoc), "Esc must cancel the buffer picker without switching documents.");
        Assert.AreEqual("ONE\n", SavedBuffer(h),
            $"Esc from the buffer picker must leave the active document unchanged.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void SwitchingBuffersPreservesEditedTextInXram()
    {
        var h = Boot(docMode: true);

        Type(h, (byte)'Z', KeyF6);
        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase),
            "Switching away must snapshot the edited document into the docbuf XRAM slot.");

        Type(h, KeyShiftF6, CtrlS);

        Assert.AreEqual(0, h.Bus.ReadRam(TestActiveDoc), "Returning with Shift-F6 should reactivate the edited first document.");
        Assert.AreEqual("ZONE\n", SavedBuffer(h),
            $"Edited text must be stored in XRAM when switching away and restored when switching back.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void ActiveXramDocUpdatesOnEditBeforeSwitchOrSave()
    {
        var h = Boot(docMode: true);

        Type(h, (byte)'Z');

        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase + 0),
            "The active document's canonical XRAM slot must update when editing, before save or buffer switch.");
        Assert.AreEqual((byte)'O', h.Bus.ReadXram(DocbufXramBase + 1),
            "The active XRAM slot should contain the edited document bytes.");
        Assert.AreEqual((byte)'N', h.Bus.ReadXram(DocbufXramBase + 2),
            "The active XRAM slot should contain the edited document bytes.");
        Assert.AreEqual((byte)'E', h.Bus.ReadXram(DocbufXramBase + 3),
            "The active XRAM slot should contain the edited document bytes.");
        Assert.AreEqual((byte)'\n', h.Bus.ReadXram(DocbufXramBase + 4),
            "The active XRAM slot should contain the edited document bytes.");
    }

    [TestMethod]
    public void WindowedXramDocEditsDocumentLargerThanRamWindow()
    {
        var h = Boot(windowedDocMode: true);
        int originalLen = h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);
        Assert.IsTrue(originalLen > 2048, "Precondition: windowed doc mode must seed a document larger than the RAM edit buffer.");
        Assert.AreEqual((byte)'A', h.Bus.ReadXram(DocbufXramBase + 0), "Precondition: first window starts with A bytes.");
        Assert.AreEqual((byte)'B', h.Bus.ReadXram(DocbufXramBase + 1024), "Precondition: byte after the first window is outside RAM and starts the B run.");

        Type(h, (byte)'Z');

        int editedLen = h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);
        Assert.AreEqual(originalLen + 1, editedLen,
            "Typing in the RAM window must grow the canonical XRAM document, not just the local edit buffer.");
        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase + 0),
            "The inserted byte should be written into the active XRAM document.");
        Assert.AreEqual((byte)'A', h.Bus.ReadXram(DocbufXramBase + 1024),
            "The last byte of the edited RAM window should remain before shifted tail bytes.");
        Assert.AreEqual((byte)'B', h.Bus.ReadXram(DocbufXramBase + 1025),
            "Bytes beyond the RAM window must shift in XRAM after a window edit.");
    }

    [TestMethod]
    public void WindowedXramDocPagesRightAtRamWindowEnd()
    {
        var h = Boot(windowedDocMode: true);

        Type(h, KeyCtrlEnd, KeyRight);

        Assert.AreEqual(1024, WindowOffset(h), "Moving right from the end of the loaded RAM window should load the next XRAM slice.");
        Assert.AreEqual((byte)'B', h.Bus.ReadRam(TextBuf), "The next RAM window should start with the next canonical XRAM byte.");
    }

    [TestMethod]
    public void WindowedXramDocPagesLeftAtRamWindowStart()
    {
        var h = Boot(windowedDocMode: true);
        Type(h, KeyCtrlEnd, KeyRight);

        Type(h, KeyLeft);

        Assert.AreEqual(0, WindowOffset(h), "Moving left from the start of a loaded RAM window should load the previous XRAM slice.");
        Type(h, (byte)'Z');
        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase + 1023),
            "After crossing left, inserting should happen before the previous window's final byte.");
    }

    [TestMethod]
    public void WindowedXramDocBackspaceDeletesAcrossRamWindowBoundary()
    {
        var h = Boot(windowedDocMode: true);
        Type(h, KeyCtrlEnd, KeyRight);
        Assert.AreEqual(1024, WindowOffset(h), "Precondition: test must start in the second RAM window.");
        int originalLen = h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);

        Type(h, KeyBackspace);

        int editedLen = h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);
        Assert.AreEqual(originalLen - 1, editedLen, "Backspace at a RAM-window boundary should delete from the canonical XRAM document.");
        Assert.AreEqual(0, WindowOffset(h), "Backspace across the boundary should leave the previous window loaded.");
        Assert.AreEqual((byte)'B', h.Bus.ReadXram(DocbufXramBase + 1023), "The shifted next-window byte should now occupy the deleted boundary byte.");
    }

    [TestMethod]
    public void WindowedXramDocPagesDownAtRamWindowEnd()
    {
        var h = Boot(windowedDocMode: true);

        Type(h, KeyCtrlEnd, KeyDown);

        Assert.AreEqual(1024, WindowOffset(h), "Moving down at the end of the loaded RAM window should load the next XRAM slice.");
        Assert.AreEqual((byte)'B', h.Bus.ReadRam(TextBuf), "The next RAM window should be visible after paging down.");
    }

    [TestMethod]
    public void WindowedXramDocPagesUpAtRamWindowStart()
    {
        var h = Boot(windowedDocMode: true);
        Type(h, KeyCtrlEnd, KeyDown);
        Assert.AreEqual(1024, WindowOffset(h), "Precondition: test must start in the second RAM window.");

        Type(h, KeyUp);

        Assert.AreEqual(0, WindowOffset(h), "Moving up at the start of the loaded RAM window should load the previous XRAM slice.");
        Assert.AreEqual((byte)'A', h.Bus.ReadRam(TextBuf), "The previous RAM window should be visible after paging up.");
    }

    [TestMethod]
    public void WindowedXramDocSaveReportsFullCanonicalLengthAfterPagedEdit()
    {
        var h = Boot(windowedDocMode: true);
        int originalLen = h.Bus.ReadRam(TestDocFullLenL) | (h.Bus.ReadRam((ushort)(TestDocFullLenL + 1)) << 8);

        Type(h, KeyCtrlEnd, KeyRight, (byte)'Z', CtrlS);

        Assert.AreEqual(1, h.Bus.ReadRam(TestSaveCnt), "Save should still route through the host save hook.");
        Assert.AreEqual(originalLen + 1, SavedLength(h), "Windowed save should report the full canonical XRAM document length, not the loaded RAM slice.");
        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase + 1024), "Save should preserve the edit made outside the first RAM window.");
    }

    [TestMethod]
    public void WindowedXramDocOpenReloadsSourceIntoCanonicalXram()
    {
        var h = Boot(windowedDocMode: true);
        Type(h, (byte)'Z');
        Assert.AreEqual((byte)'Z', h.Bus.ReadXram(DocbufXramBase), "Precondition: edit changed canonical XRAM.");

        Type(h, CtrlO);

        Assert.AreEqual(0, WindowOffset(h), "Open should reload the source at the first window.");
        Assert.AreEqual(1024, WindowLength(h), "Open should reload the initial RAM source window.");
        Assert.AreEqual((byte)'A', h.Bus.ReadXram(DocbufXramBase), "Open should replace the canonical XRAM document with the loaded source.");
        Assert.AreEqual((byte)'A', h.Bus.ReadRam(TextBuf), "Open should refresh the visible RAM window from canonical XRAM.");
    }

    [TestMethod]
    public void CtrlKSDoesNotSave()
    {
        var h = Boot(saveRet: SaveOk);
        Type(h, (byte)'X');
        Type(h, CtrlK, (byte)'s');
        Assert.AreEqual(0, h.Bus.ReadRam(TestSaveCnt), "Removed Ctrl+K S chord must not call the save hook.");
        Assert.IsTrue(TitleDirty(h), "Removed Ctrl+K S chord must leave the edited buffer dirty.");
    }

    [TestMethod]
    public void FileMenuShowsCurrentShortcutLabels()
    {
        var h = Boot();
        h.Editor.QueueInput(KeyEsc);
        h.Editor.QueueInput((byte)'f');
        Step(h, Settle);

        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains("Save ^S", StringComparison.Ordinal),
            $"File menu should advertise the Ctrl+S save binding.\n{screen}");
        Assert.IsTrue(screen.Contains("Exit ^Q/Alt-X", StringComparison.Ordinal),
            $"File menu should advertise both reliable exit bindings.\n{screen}");
        Assert.IsFalse(screen.Contains("Open", StringComparison.Ordinal),
            $"Default native editor menu should not advertise Open until a real picker exists.\n{screen}");
    }

    [DataTestMethod]
    [DataRow((byte)'f', "Save ^S", "Exit ^Q/Alt-X")]
    [DataRow((byte)'e', "Cut ^X", "Copy ^C")]
    [DataRow((byte)'s', "Find ^F", "Replace")]
    public void AltMenuHotkeysOpenExpectedMenus(byte hotkey, string firstItem, string secondItem)
    {
        var h = Boot();

        Type(h, KeyEsc, hotkey);

        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains(firstItem, StringComparison.Ordinal),
            $"Alt-{(char)hotkey} should open a menu containing {firstItem}.\n{screen}");
        Assert.IsTrue(screen.Contains(secondItem, StringComparison.Ordinal),
            $"Alt-{(char)hotkey} should open a menu containing {secondItem}.\n{screen}");
    }

    [TestMethod]
    public void AltMenuHotkeysAreCaseInsensitive()
    {
        var h = Boot();

        Type(h, KeyEsc, (byte)'F');

        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains("Save ^S", StringComparison.Ordinal),
            $"Alt-F should normalize to the File menu hotkey.\n{screen}");
    }

    [TestMethod]
    public void BareEscPrefixTimesOutWithoutStealingNextKey()
    {
        var h = Boot();

        Type(h, KeyEsc);
        WaitFrames(h, 20);
        Type(h, (byte)'x');

        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone),
            $"A dropped Alt-prefix byte must not strand the editor or turn the next normal key into Alt-X.\n{Snapshot(h)}");
        Assert.AreEqual((byte)'x', h.Bus.ReadRam(TextBuf),
            $"After the Alt-prefix timeout, the next printable key should edit the buffer normally.\n{Snapshot(h)}");
        Assert.IsTrue(TitleDirty(h), "Typing after a bare Esc timeout should dirty the buffer.");
    }

    [TestMethod]
    public void AltXAllowsShortPhysicalModifierDelayBeforeLetter()
    {
        var h = Boot();

        h.Editor.QueueInput(KeyEsc);
        WaitFrames(h, 2);
        h.Editor.QueueInput((byte)'x');

        RunUntilDone(h);

        Assert.AreEqual(ExitQuit, h.Bus.ReadRam(TestResult),
            "Alt-X should still work when the physical keyboard bridge spaces ESC and X by a short delay.");
    }

    [TestMethod]
    public void CtrlQQuitsCleanEditor()
    {
        var h = Boot();

        Type(h, CtrlQ);
        RunUntilDone(h);

        Assert.AreEqual(ExitQuit, h.Bus.ReadRam(TestResult),
            "Ctrl+Q must remain a reliable exit path when a desktop consumes Alt shortcuts.");
    }

    [TestMethod]
    public void OpenMenuRightAndLeftSwitchBetweenTopLevelMenus()
    {
        var h = Boot();

        Type(h, KeyEsc, (byte)'f', KeyRight);
        string right = Snapshot(h);
        Assert.IsTrue(right.Contains("Undo ^Z", StringComparison.Ordinal),
            $"Right from File should open the Edit menu.\n{right}");
        Assert.IsFalse(right.Contains("Save ^S", StringComparison.Ordinal),
            $"Switching menus should close the previous File dropdown.\n{right}");

        Type(h, KeyLeft);
        string left = Snapshot(h);
        Assert.IsTrue(left.Contains("Save ^S", StringComparison.Ordinal),
            $"Left from Edit should reopen the File menu.\n{left}");
        Assert.IsFalse(left.Contains("Undo ^Z", StringComparison.Ordinal),
            $"Switching back should close the Edit dropdown.\n{left}");
    }

    [TestMethod]
    public void OpenMenuPreservesEditorCursorPosition()
    {
        var h = Boot();

        byte beforeX = CursorX(h);
        byte beforeY = CursorY(h);

        Type(h, KeyEsc, (byte)'f');

        Assert.AreEqual(beforeX, CursorX(h),
            $"Menus are overlays; drawing one must not move the editor cursor into the menu chrome.\n{Snapshot(h)}");
        Assert.AreEqual(beforeY, CursorY(h),
            $"Menus are overlays; drawing one must not move the editor cursor into the menu chrome.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void MenuDownSkipsDisabledItems()
    {
        var h = Boot(testMenuMode: true);

        Type(h, KeyEsc, (byte)'t', KeyDown, KeyEnter);

        Assert.AreEqual(1, h.Bus.ReadRam(TestCmdCnt),
            $"Down from the first item should skip the disabled row and activate the second command.\n{Snapshot(h)}");
        Assert.AreEqual(CmdOpen, h.Bus.ReadRam(TestLastCmd),
            "The harness menu's second active item reports EDITUI_CMD_OPEN.");
    }

    [TestMethod]
    public void MenuUpSkipsDisabledItems()
    {
        var h = Boot(testMenuMode: true);

        Type(h, KeyEsc, (byte)'t', KeyUp, KeyEnter);

        Assert.AreEqual(1, h.Bus.ReadRam(TestCmdCnt),
            $"Up from the first item should wrap past the disabled row and activate the second command.\n{Snapshot(h)}");
        Assert.AreEqual(CmdOpen, h.Bus.ReadRam(TestLastCmd),
            "The harness menu's second active item reports EDITUI_CMD_OPEN.");
    }

    [TestMethod]
    public void AltXExitsCleanlyWhenNotDirty()
    {
        var h = Boot();
        h.Editor.QueueInput(KeyEsc);
        h.Editor.QueueInput((byte)'x');
        RunUntilDone(h);
        Assert.AreEqual(ExitQuit, h.Bus.ReadRam(TestResult), "Clean Alt-X should exit with QUIT.");
    }

    [TestMethod]
    public void AltXWhileDirtyShowsThreeChoicePrompt()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains("Modified", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("Save changes before closing?", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains(" Save ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains(" Discard ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains(" Cancel ", StringComparison.Ordinal), screen);
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Dialog open: editor must not have exited.");
    }

    [TestMethod]
    public void DialogCancelReturnsToEditorWithTextRestored()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        Assert.IsTrue(ScreenContains(h, "Modified"), "Pre-condition: dirty-exit prompt open.");
        Type(h, (byte)'c'); // Cancel
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Cancel must keep the editor open.");
        Assert.IsFalse(ScreenContains(h, "Modified"), "Dirty-exit prompt should be dismissed after cancel.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop),
            $"Body must be restored under the dialog after cancel.\n{Snapshot(h)}");
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Draft text must be intact after cancel.");
    }

    [TestMethod]
    public void DialogEscCancelsDirtyExitPrompt()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        Assert.IsTrue(ScreenContains(h, "Modified"), "Pre-condition: dirty-exit prompt open.");

        Type(h, KeyEsc);

        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Esc must keep the editor open.");
        Assert.IsFalse(ScreenContains(h, "Modified"), "Esc should dismiss the dirty-exit prompt.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop),
            $"Body must be restored under the dialog after Esc.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void DialogExitAnywayDiscardsButPreservesDraft()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        h.Editor.QueueInput((byte)'e'); // Exit anyway
        RunUntilDone(h);
        Assert.AreEqual(ExitDiscard, h.Bus.ReadRam(TestResult), "Exit Anyway should report DISCARD.");
        Assert.AreEqual(0, h.Bus.ReadRam(TestSaveCnt), "Exit Anyway must not install via the save hook.");
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Draft text must be preserved on Exit Anyway.");
    }

    [TestMethod]
    public void DialogSaveFirstValidExitsSaved()
    {
        var h = Boot(saveRet: SaveOk);
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        h.Editor.QueueInput((byte)'s'); // Save first
        RunUntilDone(h);
        Assert.AreEqual(ExitSaved, h.Bus.ReadRam(TestResult), "Save First (valid) should report SAVED.");
        Assert.IsTrue(h.Bus.ReadRam(TestSaveCnt) >= 1, "Save First must invoke the save hook.");
    }

    [TestMethod]
    public void DialogSaveFirstInvalidStaysInEditor()
    {
        var h = Boot(saveRet: SaveError);
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        Type(h, (byte)'s'); // Save first -> validation fails
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone),
            "A failed Save First must keep the editor open, not exit.");
        Assert.IsTrue(h.Bus.ReadRam(TestSaveCnt) >= 1, "Save hook must have been attempted.");
        Assert.IsFalse(ScreenContains(h, "Modified"), "Dirty-exit prompt should close after a (failed) save.");
    }

    [TestMethod]
    public void AutoIndentHookInsertsLeadingSpaces()
    {
        var h = Boot(indent: 2);
        Type(h, KeyEnter); // newline at cursor 0 -> indent hook adds 2 spaces
        // Buffer should now be: \n ' ' ' ' A B \n C D
        Assert.AreEqual(0x0A, h.Bus.ReadRam(TextBuf), "First byte should be the inserted newline.");
        Assert.AreEqual((byte)' ', h.Bus.ReadRam((ushort)(TextBuf + 1)), "Auto-indent space 1.");
        Assert.AreEqual((byte)' ', h.Bus.ReadRam((ushort)(TextBuf + 2)), "Auto-indent space 2.");
        Assert.AreEqual((byte)'A', h.Bus.ReadRam((ushort)(TextBuf + 3)), "Original text follows the indent.");
    }

    [TestMethod]
    public void SyntaxHighlightHookColorsFirstCharacter()
    {
        var h = Boot(hlMark: 0x2A);
        Assert.AreEqual(0x2A, ColorAt(h, ViewLeft, ViewTop),
            "The highlight hook should color the first character of the line.");
    }

    [TestMethod]
    public void WindowBoundaryArrowNoOpDoesNotRepaintTheEditor()
    {
        var h = Boot(windowedDocMode: true);

        byte topPaints = h.Bus.ReadRam(TestHlCount);
        Type(h, KeyUp);
        Assert.AreEqual(topPaints, h.Bus.ReadRam(TestHlCount),
            "Up at the first XRAM window must not repaint an unchanged editor.");

        while (WindowOffset(h) + WindowLength(h) < FullDocumentLength(h))
            Type(h, KeyCtrlEnd, KeyDown);
        Type(h, KeyCtrlEnd);
        byte bottomPaints = h.Bus.ReadRam(TestHlCount);
        Type(h, KeyDown);
        Assert.AreEqual(bottomPaints, h.Bus.ReadRam(TestHlCount),
            "Down at the final XRAM window must not repaint an unchanged editor.");
    }

    [TestMethod]
    public void CutClearsTheBuffer()
    {
        var h = Boot();
        Type(h, CtrlA, CtrlX); // select all, cut
        Assert.AreEqual(' ', ChAt(h, ViewLeft, ViewTop), $"Cut should empty the body.\n{Snapshot(h)}");
        Assert.AreEqual(' ', ChAt(h, ViewLeft, ViewTop + 1), "Second line should also be cleared.");
    }

    [TestMethod]
    public void CopyThenPasteAppendsClipboard()
    {
        var h = Boot();
        // Select all (cursor at end), copy, deselect with Right (cursor stays at end), paste.
        Type(h, CtrlA, CtrlC, KeyRight, CtrlV);
        // Buffer "AB\nCD" + "AB\nCD" => lines "AB","CDAB","CD"
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), $"row8 col0\n{Snapshot(h)}");
        Assert.AreEqual('D', ChAt(h, ViewLeft + 1, ViewTop + 1), $"row8 col1\n{Snapshot(h)}");
        Assert.AreEqual('A', ChAt(h, ViewLeft + 2, ViewTop + 1), $"row8 col2\n{Snapshot(h)}");
        Assert.AreEqual('B', ChAt(h, ViewLeft + 3, ViewTop + 1), $"row8 col3\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CopyPasteHandlesBlocksLargerThanLegacyLocalClipboard()
    {
        string chunk = string.Concat(Enumerable.Range(0, 320).Select(i => (char)('A' + (i % 26))));
        var h = Boot(seedText: Encoding.ASCII.GetBytes(chunk));

        Type(h, CtrlA, CtrlC, KeyRight, CtrlV, CtrlS);

        Assert.AreEqual(chunk + chunk, SavedBuffer(h),
            "The shared clipboard must handle source blocks larger than the old 256-byte editbuf-local clipboard.");
    }

    [TestMethod]
    public void CtrlFUsesSharedNuiTextDialog()
    {
        var h = Boot();
        Type(h, CtrlF);

        Assert.AreEqual(0xC9, h.Bus.Vgc.GetScreenChar(14, PhysRow(h, 17)),
            $"Find should open a shared CP437 NUI dialog.\n{Snapshot(h)}");
        Assert.IsTrue(ScreenContains(h, "Text:"), $"Find dialog should show a field label.\n{Snapshot(h)}");
        Assert.IsFalse(ScreenContains(h, "TAB NEXT"), $"Find dialog should not print help text inside the modal.\n{Snapshot(h)}");

        Type(h, KeyEsc);
        Assert.IsFalse(HasFrameChars(h), $"Esc should dismiss the dialog and repaint the editor.\n{Snapshot(h)}");
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), $"Body must be restored after cancelling find.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlFPromptsAndFindsText()
    {
        var h = Boot();
        Type(h, CtrlF, (byte)'C', (byte)'D', KeyEnter);

        Assert.AreEqual(0, CursorX(h), $"Find should place the cursor at the C in CD.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 1, CursorY(h), $"Find should move to the second logical line.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlFMissingTextLeavesCursorInPlace()
    {
        var h = Boot();

        Type(h, CtrlF, (byte)'Z', (byte)'Z', KeyEnter);

        Assert.AreEqual(0, CursorX(h), $"Find miss should not move the cursor.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop, CursorY(h), $"Find miss should leave the cursor on the original row.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void F3RepeatsLastFindAndWraps()
    {
        var h = Boot(seedText: Encoding.ASCII.GetBytes("Axx\nBxx\nCxx"));

        Type(h, CtrlF, (byte)'x', (byte)'x', KeyEnter);
        Assert.AreEqual(1, CursorX(h), $"Initial find should land on the first match.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop, CursorY(h), $"Initial find should land on the first row.\n{Snapshot(h)}");

        Type(h, KeyF3);
        Assert.AreEqual(1, CursorX(h), $"F3 should keep matching the same column.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 1, CursorY(h), $"F3 should advance to the second match.\n{Snapshot(h)}");

        Type(h, KeyF3);
        Assert.AreEqual(ViewTop + 2, CursorY(h), $"F3 should advance to the third match.\n{Snapshot(h)}");

        Type(h, KeyF3);
        Assert.AreEqual(ViewTop, CursorY(h), $"F3 should wrap to the first match after EOF.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlZAndCtrlYUndoRedoTyping()
    {
        const string original = "AB\nCD";
        var h = Boot(seedText: Encoding.ASCII.GetBytes(original));

        Type(h, (byte)'X');
        Type(h, CtrlZ, CtrlS);
        Assert.AreEqual(original, SavedBuffer(h), $"Ctrl-Z should restore the pre-edit buffer.\n{Snapshot(h)}");

        Type(h, CtrlY, CtrlS);
        Assert.AreEqual("XAB\nCD", SavedBuffer(h), $"Ctrl-Y should redo the undone insert.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlZAndCtrlYWalkMultipleHistoryEntries()
    {
        const string original = "AB\nCD";
        var h = Boot(seedText: Encoding.ASCII.GetBytes(original));

        Type(h, (byte)'1');
        Type(h, (byte)'2');
        Type(h, (byte)'3');

        Type(h, CtrlZ, CtrlS);
        Assert.AreEqual("12" + original, SavedBuffer(h), $"First Ctrl-Z should remove only the last edit.\n{Snapshot(h)}");

        Type(h, CtrlZ, CtrlS);
        Assert.AreEqual("1" + original, SavedBuffer(h), $"Second Ctrl-Z should restore the previous history entry.\n{Snapshot(h)}");

        Type(h, CtrlZ, CtrlS);
        Assert.AreEqual(original, SavedBuffer(h), $"Third Ctrl-Z should restore the original buffer.\n{Snapshot(h)}");

        Type(h, CtrlY, CtrlS);
        Assert.AreEqual("1" + original, SavedBuffer(h), $"First Ctrl-Y should redo the first undone edit.\n{Snapshot(h)}");

        Type(h, CtrlY, CtrlS);
        Assert.AreEqual("12" + original, SavedBuffer(h), $"Second Ctrl-Y should redo the next history entry.\n{Snapshot(h)}");

        Type(h, CtrlY, CtrlS);
        Assert.AreEqual("123" + original, SavedBuffer(h), $"Third Ctrl-Y should redo the latest history entry.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlZAndCtrlYWithoutHistoryAreNoOps()
    {
        const string original = "AB\nCD";
        var h = Boot(seedText: Encoding.ASCII.GetBytes(original));

        Type(h, CtrlZ, CtrlY);

        Assert.IsFalse(TitleDirty(h), $"Undo/redo with no history must not mark a clean buffer dirty.\n{Snapshot(h)}");
        Type(h, CtrlS);
        Assert.AreEqual(original, SavedBuffer(h), $"Undo/redo with no history must leave the buffer unchanged.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void ReplaceAllReplacesEveryMatchAndUndoRedoRestores()
    {
        const string original = "foo one\nfoo two\nnope foo";
        const string replaced = "bar one\nbar two\nnope bar";
        var h = Boot(seedText: Encoding.ASCII.GetBytes(original));

        Type(h, KeyEsc, (byte)'s', (byte)'a');
        Assert.IsTrue(ScreenContains(h, "Replace All"), $"Search/Replace All menu item should open the Replace All dialog.\n{Snapshot(h)}");
        TypeText(h, "foo");
        Type(h, KeyEnter);
        Assert.IsTrue(ScreenContains(h, "With:"), $"Replace All should prompt for replacement text after the find term.\n{Snapshot(h)}");
        TypeText(h, "bar");
        Type(h, KeyEnter, CtrlS);

        Assert.AreEqual(replaced, SavedBuffer(h), $"Replace All should update every match in one command.\n{Snapshot(h)}");

        Type(h, CtrlZ, CtrlS);
        Assert.AreEqual(original, SavedBuffer(h), $"One Ctrl-Z should undo the entire Replace All command.\n{Snapshot(h)}");

        Type(h, CtrlY, CtrlS);
        Assert.AreEqual(replaced, SavedBuffer(h), $"Ctrl-Y should redo the entire Replace All command.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void ReplaceAllWithNoMatchesLeavesBufferCleanAndUnchanged()
    {
        const string original = "foo one\nfoo two";
        var h = Boot(seedText: Encoding.ASCII.GetBytes(original));

        Type(h, KeyEsc, (byte)'s', (byte)'a');
        TypeText(h, "missing");
        Type(h, KeyEnter);
        TypeText(h, "bar");
        Type(h, KeyEnter);

        Assert.IsFalse(TitleDirty(h), $"Replace All with no matches should not dirty the buffer.\n{Snapshot(h)}");
        Type(h, CtrlS);
        Assert.AreEqual(original, SavedBuffer(h), $"Replace All with no matches should leave text unchanged.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void LargeSourceFiles_RoundTripThroughDiskEditorFindReplaceSaveReload()
    {
        string path = Path.Combine(Path.GetTempPath(), $"editor-src-{Guid.NewGuid():N}.ndi");
        SourceCase[] cases =
        [
            BuildSourceCase("UNIT1.pas", NdiFileType.Pascal, "Pascal", "NEEDLE_PASCAL_ALPHA", "UPDATED_PASCAL_ALPHA"),
            BuildSourceCase("TURTLE.logo", NdiFileType.Logo, "Logo", "NEEDLE_LOGO_ALPHA", "UPDATED_LOGO_ALPHA"),
            BuildSourceCase("WORDS.4th", NdiFileType.Forth, "Forth", "NEEDLE_FORTH_ALPHA", "UPDATED_FORTH_ALPHA"),
        ];

        try
        {
            NdiImage.CreateFormatted(path, "SRC", 800);
            using var image = NdiImage.Open(path);
            foreach (SourceCase source in cases)
                image.WriteFile(source.FileName, source.FileType, 0xFFFF, Encoding.ASCII.GetBytes(source.Text));

            foreach (SourceCase source in cases)
            {
                var entry = image.ListDirectory(0xFFFF).Single(e => e.Filename == source.FileName);
                Assert.AreEqual(source.FileType, entry.FileType, $"{source.FileName} must keep its source file type on disk.");

                byte[] loaded = image.ReadFile(source.FileName, 0xFFFF);
                var h = Boot(seedText: loaded);

                Type(h, CtrlF);
                Assert.IsTrue(ScreenContains(h, "Find"), $"Find dialog should open for {source.FileName}.\n{Snapshot(h)}");
                Assert.IsTrue(ScreenContains(h, "Text:"), $"Find dialog should expose the text field for {source.FileName}.\n{Snapshot(h)}");
                TypeText(h, source.Needle);
                Type(h, KeyEnter);
                Assert.AreEqual(source.Col, CursorX(h), $"Find should land on the target column in {source.FileName}.\n{Snapshot(h)}");
                Assert.AreEqual(ViewTop + source.Line, CursorY(h), $"Find should land on the target line in {source.FileName}.\n{Snapshot(h)}");

                Type(h, KeyEsc, (byte)'s', (byte)'r');
                Assert.IsTrue(ScreenContains(h, "Replace"), $"Replace dialog should open for {source.FileName}.\n{Snapshot(h)}");
                Assert.IsTrue(ScreenContains(h, "Text:"), $"Replace first prompt should ask for search text in {source.FileName}.\n{Snapshot(h)}");
                TypeText(h, source.Needle);
                Type(h, KeyEnter);
                Assert.IsTrue(ScreenContains(h, "With:"), $"Replace second prompt should ask for replacement text in {source.FileName}.\n{Snapshot(h)}");
                TypeText(h, source.Replacement);
                Type(h, KeyEnter);

                Type(h, CtrlS);
                Assert.AreEqual(1, h.Bus.ReadRam(TestSaveCnt), $"Save hook should fire once for {source.FileName}.");
                string saved = SavedBuffer(h);
                Assert.IsFalse(saved.Contains(source.Needle, StringComparison.Ordinal), $"{source.FileName} should no longer contain the old text.");
                Assert.IsTrue(saved.Contains(source.Replacement, StringComparison.Ordinal), $"{source.FileName} should contain the replacement text.");

                image.WriteFile(source.FileName, source.FileType, 0xFFFF, Encoding.ASCII.GetBytes(saved));
                var savedEntry = image.ListDirectory(0xFFFF).Single(e => e.Filename == source.FileName);
                Assert.AreEqual(source.FileType, savedEntry.FileType, $"{source.FileName} must keep its file type after save.");
                string reloaded = Encoding.ASCII.GetString(image.ReadFile(source.FileName, 0xFFFF));
                Assert.AreEqual(saved, reloaded, $"{source.FileName} must reload exactly what the editor saved.");
            }
        }
        finally
        {
            File.Delete(path);
        }
    }

    private static SourceCase BuildSourceCase(string fileName, NdiFileType fileType, string language, string needle, string replacement)
    {
        const int targetLine = 18;
        var sb = new StringBuilder();
        for (int i = 0; i < 24; i++)
        {
            if (i == targetLine)
                sb.Append("  marker := '").Append(needle).Append("';");
            else
                sb.Append("  line ").Append(i.ToString("00")).Append(" for ").Append(language)
                  .Append(" keeps the editor busy with enough source text.");
            sb.Append('\n');
        }

        int lineStart = sb.ToString().Split('\n').Take(targetLine).Sum(line => line.Length + 1);
        int col = sb.ToString().IndexOf(needle, StringComparison.Ordinal) - lineStart;
        return new SourceCase(fileName, fileType, sb.ToString(), needle, replacement, targetLine, col);
    }

    [TestMethod]
    public void CtrlGGotoLineUsesOneBasedLineNumbers()
    {
        var h = Boot(seedText: Encoding.ASCII.GetBytes("AB\nCD\nEF"));
        Type(h, CtrlG, (byte)'2', KeyEnter);

        Assert.AreEqual(0, CursorX(h), $"Goto line 2 should move to column 1 of the CD line.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 1, CursorY(h), $"Goto line 2 should move to the second logical line.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlGOutOfRangeGotoClampsToEndOfFile()
    {
        var h = Boot(seedText: Encoding.ASCII.GetBytes("AB\nCD\nEF"));
        Type(h, CtrlG, (byte)'9', (byte)'9', (byte)'9', KeyEnter);

        Assert.AreEqual(2, CursorX(h), $"Out-of-range goto should clamp to EOF after EF.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 2, CursorY(h), $"Out-of-range goto should land on the last logical line.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void CtrlHomeAndCtrlEndMoveToFileBoundaries()
    {
        var h = Boot(seedText: Encoding.ASCII.GetBytes("AB\nCD\nEF"));

        Type(h, KeyCtrlEnd);
        Assert.AreEqual(2, CursorX(h), $"Ctrl-End should move to EOF after EF.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 2, CursorY(h), $"Ctrl-End should land on the last logical line.\n{Snapshot(h)}");

        Type(h, KeyCtrlHome);
        Assert.AreEqual(0, CursorX(h), $"Ctrl-Home should move to start of file.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop, CursorY(h), $"Ctrl-Home should land on the first logical line.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void InsertMidLineKeepsRowCorrect()
    {
        var h = Boot();
        // Cursor at 0 ("A"). Move right onto "B", insert 'X' -> line "AXB".
        // The partial-row repaint starts at the insertion column; the unchanged
        // 'A' to its left and the 'C','D' on the next row must stay intact.
        Type(h, KeyRight, (byte)'X');
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), $"col0\n{Snapshot(h)}");
        Assert.AreEqual('X', ChAt(h, ViewLeft + 1, ViewTop), $"col1\n{Snapshot(h)}");
        Assert.AreEqual('B', ChAt(h, ViewLeft + 2, ViewTop), $"col2\n{Snapshot(h)}");
        Assert.AreEqual(' ', ChAt(h, ViewLeft + 3, ViewTop), $"col3 blank\n{Snapshot(h)}");
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), "row below row0 unchanged");
        Assert.AreEqual('D', ChAt(h, ViewLeft + 1, ViewTop + 1), "row below row0 unchanged");
    }

    [TestMethod]
    public void DeleteMidLineShiftsLeftAndClearsTail()
    {
        var h = Boot();
        // Cursor at 0 ("A"). Move right onto "B", delete -> line "A".
        // The shifted-left run must clear the vacated trailing cell, and the
        // second line stays put.
        Type(h, KeyRight);
        Type(h, 0x7F); // DELETE
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), $"col0\n{Snapshot(h)}");
        Assert.AreEqual(' ', ChAt(h, ViewLeft + 1, ViewTop), $"tail cleared\n{Snapshot(h)}");
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), "second line intact");
        Assert.AreEqual('D', ChAt(h, ViewLeft + 1, ViewTop + 1), "second line intact");
    }

    [TestMethod]
    public void BackspaceMidLineShiftsLeftAndClearsTail()
    {
        var h = Boot();
        // "AB": move right twice (cursor after B), backspace removes 'B' -> "A".
        Type(h, KeyRight, KeyRight, KeyBackspace);
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), $"col0\n{Snapshot(h)}");
        Assert.AreEqual(' ', ChAt(h, ViewLeft + 1, ViewTop), $"tail cleared\n{Snapshot(h)}");
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), "second line intact");
    }

    [TestMethod]
    public void BackspaceAtLineStartJoinsLines()
    {
        var h = Boot(seedText: Encoding.ASCII.GetBytes("AB\nCD\nEF"));
        // Cursor starts at 0. Move down to line 2 ("CD"), home, backspace deletes
        // the newline joining the lines into "ABCD"; the following line must
        // pull up to row 1 rather than leaving a visual gap.
        Type(h, KeyDown, KeyHome, KeyBackspace);
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), Snapshot(h));
        Assert.AreEqual('B', ChAt(h, ViewLeft + 1, ViewTop), Snapshot(h));
        Assert.AreEqual('C', ChAt(h, ViewLeft + 2, ViewTop), Snapshot(h));
        Assert.AreEqual('D', ChAt(h, ViewLeft + 3, ViewTop), Snapshot(h));
        Assert.AreEqual('E', ChAt(h, ViewLeft, ViewTop + 1), Snapshot(h));
        Assert.AreEqual('F', ChAt(h, ViewLeft + 1, ViewTop + 1), Snapshot(h));
    }

    // "00\n01\n...\n(count-1)" — every line is its own two-digit index.
    private static byte[] BuildNumberedLines(int count)
    {
        var bytes = new System.Collections.Generic.List<byte>();
        for (int i = 0; i < count; i++)
        {
            if (i > 0) bytes.Add(0x0A);
            bytes.Add((byte)('0' + (i / 10)));
            bytes.Add((byte)('0' + (i % 10)));
        }
        return bytes.ToArray();
    }

    private static string TwoChars(Harness h, int row) => $"{ChAt(h, 0, row)}{ChAt(h, 1, row)}";

    [TestMethod]
    public void ScrollingTallBufferRendersCorrectRowsViaWindow()
    {
        // 60 numbered lines "00".."59". The body shows 48 rows, so reaching the
        // last line must scroll. Exercises the direct-window cell-offset math
        // across the FULL body (row 47 -> window offset 48*80=3840+) and the
        // repaint-on-scroll path — the existing tests never scroll.
        var h = Boot(seedText: BuildNumberedLines(60));
        // Top of buffer: first body row shows line 0; last body row shows line 47.
        Assert.AreEqual("00", TwoChars(h, ViewTop), $"top body row = line 0\n{Snapshot(h)}");
        Assert.AreEqual("47", TwoChars(h, ViewTop + 47), $"bottom body row = line 47\n{Snapshot(h)}");
        // Move the cursor to the last line; the view scrolls so line 59 lands on
        // the bottom body row and line 12 (= 59-47) on the top body row.
        for (int i = 0; i < 59; i++) h.Editor.QueueInput(KeyDown);
        Step(h, 12_000_000);
        Assert.AreEqual("59", TwoChars(h, ViewTop + 47), $"after scroll: bottom = line 59\n{Snapshot(h)}");
        Assert.AreEqual("12", TwoChars(h, ViewTop), $"after scroll: top = line 12\n{Snapshot(h)}");
        Assert.AreEqual(1, h.Bus.Read((ushort)VgcConstants.RegTextScrollStart),
            "The editor should hardware-scroll only the body rows below the menu bar.");
        Assert.AreEqual(48, h.Bus.Read((ushort)VgcConstants.RegTextScrollRows),
            "The editor body ring should exclude both menu and status bars.");
        Assert.AreEqual(12, h.Bus.Read((ushort)VgcConstants.RegTextTopRow),
            "Tall-buffer cursor scrolling should use the VGC text ring-scroll base instead of repainting/copying the whole viewport.");
        Assert.AreEqual(0, h.Bus.Vgc.PhysicalTextRow(0), "Menu bar row must stay physically fixed while the body scrolls.");
        Assert.AreEqual(49, h.Bus.Vgc.PhysicalTextRow(49), "Status bar row must stay physically fixed while the body scrolls.");
    }

    [TestMethod]
    public void BackspaceLineJoinNearBottomRepaintsExposedRow()
    {
        var h = Boot(seedText: BuildNumberedLines(60));
        for (int i = 0; i < 59; i++) h.Editor.QueueInput(KeyDown);
        h.Editor.QueueInput(KeyUp); // line 58 at body row 46, line 59 at row 47
        Step(h, 12_000_000);
        Assert.AreEqual("58", TwoChars(h, ViewTop + 46), $"precondition row 46\n{Snapshot(h)}");
        Assert.AreEqual("59", TwoChars(h, ViewTop + 47), $"precondition row 47\n{Snapshot(h)}");

        Type(h, KeyEnter);
        Assert.AreEqual("  ", TwoChars(h, ViewTop + 46), $"split row should be blank\n{Snapshot(h)}");
        Assert.AreEqual("58", TwoChars(h, ViewTop + 47), $"split text should move down\n{Snapshot(h)}");

        Type(h, KeyBackspace);
        Assert.AreEqual("58", TwoChars(h, ViewTop + 46), $"joined row should pull back up\n{Snapshot(h)}");
        Assert.AreEqual("59", TwoChars(h, ViewTop + 47),
            $"exposed bottom row must be repainted from the document, not left as stale copied text\n{Snapshot(h)}");
    }
}
