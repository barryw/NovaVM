using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
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
    private const ushort AppEntry = 0x7200;
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

    private const byte CtrlA = 0x01, CtrlC = 0x03, CtrlG = 0x07, CtrlK = 0x0B, CtrlQ = 0x11, CtrlV = 0x16, CtrlX = 0x18;
    private const byte KeyRight = 0x1D, KeyUp = 0x1E, KeyDown = 0x1F, KeyHome = 0x02, KeyBackspace = 0x08, KeyEnter = 0x0D;
    private const byte KeyEsc = 0x1B, KeyCtrlHome = 0x80, KeyCtrlEnd = 0x81;

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

    private static Harness Boot(byte saveRet = SaveOk, byte indent = 0, byte hlMark = 0x1F,
                               byte[]? seedText = null)
    {
        var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        LoadApp(bus);
        bus.WriteRam(TestSaveRet, saveRet);
        bus.WriteRam(TestIndent, indent);
        bus.WriteRam(TestHlMark, hlMark);
        bus.WriteRam(TestDone, 0x00);
        bus.WriteRam(TestSaveCnt, 0x00);
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
        string path = Path.Combine(RepoRoot(), "assembly", "apps", "editbuf_test", "editbuf_test.bin");
        Assert.IsTrue(File.Exists(path), $"Build the harness first: {path}");
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
            if (File.Exists(Path.Combine(dir, "runtime", "asm", "editbuf.s")))
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

    private static void Step(Harness h, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = h.Cpu.ClocksForNext();
            h.Cpu.ExecuteNext();
            h.Bus.AdvanceCycles(cycles);
        }
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
    public void CtrlKSCallsSaveHookAndClearsDirtyOnOk()
    {
        var h = Boot(saveRet: SaveOk);
        Type(h, (byte)'X');
        Assert.IsTrue(TitleDirty(h), "Pre-condition: dirty after edit.");
        Type(h, CtrlK, (byte)'s');
        Assert.AreEqual(1, h.Bus.ReadRam(TestSaveCnt), "Save hook should fire exactly once.");
        Assert.AreEqual(6, h.Bus.ReadRam(TestSaveLenL), "Save hook should see the post-edit length (5+1).");
        Assert.IsFalse(TitleDirty(h), $"A successful save should clear the dirty marker.\n{Snapshot(h)}");
    }

    [TestMethod]
    public void FileMenuShowsCurrentShortcutLabels()
    {
        var h = Boot();
        h.Editor.QueueInput(KeyEsc);
        h.Editor.QueueInput((byte)'f');
        Step(h, Settle);

        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains("Save ^K S", StringComparison.Ordinal),
            $"File menu should advertise the Ctrl+K S save binding.\n{screen}");
        Assert.IsTrue(screen.Contains("Exit Alt-X", StringComparison.Ordinal),
            $"File menu should advertise the Alt+X exit binding.\n{screen}");
        Assert.IsFalse(screen.Contains("Open", StringComparison.Ordinal),
            $"Default native editor menu should not advertise Open until a real picker exists.\n{screen}");
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
        Assert.IsTrue(screen.Contains("Mod:", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("E exit", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("S save", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("C cancel", StringComparison.Ordinal), screen);
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Dialog open: editor must not have exited.");
    }

    [TestMethod]
    public void DialogCancelReturnsToEditorWithTextRestored()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, KeyEsc, (byte)'x');
        Assert.IsTrue(ScreenContains(h, "Mod:"), "Pre-condition: dirty-exit prompt open.");
        Type(h, (byte)'c'); // Cancel
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Cancel must keep the editor open.");
        Assert.IsFalse(ScreenContains(h, "Mod:"), "Dirty-exit prompt should be dismissed after cancel.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop),
            $"Body must be restored under the dialog after cancel.\n{Snapshot(h)}");
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Draft text must be intact after cancel.");
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
        Assert.IsFalse(ScreenContains(h, "Mod:"), "Dirty-exit prompt should close after a (failed) save.");
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
    public void CtrlQFPromptsAndFindsText()
    {
        var h = Boot();
        Type(h, CtrlQ, (byte)'f', (byte)'C', (byte)'D', KeyEnter);

        Assert.AreEqual(0, CursorX(h), $"Find should place the cursor at the C in CD.\n{Snapshot(h)}");
        Assert.AreEqual(ViewTop + 1, CursorY(h), $"Find should move to the second logical line.\n{Snapshot(h)}");
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
