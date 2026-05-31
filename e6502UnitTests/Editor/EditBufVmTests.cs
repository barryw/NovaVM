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

    private const byte CtrlA = 0x01, CtrlC = 0x03, CtrlQ = 0x11, CtrlS = 0x13, CtrlV = 0x16, CtrlX = 0x18;
    private const byte KeyRight = 0x1D, KeyDown = 0x1F, KeyHome = 0x02, KeyBackspace = 0x08, KeyEnter = 0x0D;

    private const byte ExitQuit = 0x00, ExitSaved = 0x01, ExitDiscard = 0x02;
    private const byte SaveOk = 0x00, SaveError = 0x02;

    // Body view origin (EDITBUF_VIEW_LEFT / EDITBUF_VIEW_TOP).
    private const int ViewLeft = 2, ViewTop = 7;
    private const int TitleRow = 3;

    // Cycles allowed for the editor to consume queued keys and fully re-render.
    private const int Settle = 2_500_000;

    private sealed record Harness(CompositeBusDevice Bus, Cpu Cpu, ScreenEditor Editor);

    private static Harness Boot(byte saveRet = SaveOk, byte indent = 0, byte hlMark = 0x1F)
    {
        var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        LoadApp(bus);
        bus.WriteRam(TestSaveRet, saveRet);
        bus.WriteRam(TestIndent, indent);
        bus.WriteRam(TestHlMark, hlMark);
        bus.WriteRam(TestDone, 0x00);
        bus.WriteRam(TestSaveCnt, 0x00);
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

    private static char ChAt(Harness h, int col, int row) => (char)h.Bus.Vgc.GetScreenChar(col, row);

    private static string Row(Harness h, int row)
    {
        var sb = new StringBuilder();
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            byte ch = h.Bus.Vgc.GetScreenChar(col, row);
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

    private static bool TitleDirty(Harness h) =>
        Row(h, TitleRow).Contains("SQUARE*", StringComparison.Ordinal);

    // -----------------------------------------------------------------

    [TestMethod]
    public void InitialRenderShowsTitleAndBuffer()
    {
        var h = Boot();
        Assert.IsTrue(Row(h, TitleRow).Contains("SQUARE", StringComparison.Ordinal),
            $"Title 'SQUARE' should be centered in the title band.\n{Snapshot(h)}");
        Assert.IsTrue(ScreenContains(h, "File"), "Menu bar should render.");
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), Snapshot(h));
        Assert.AreEqual('B', ChAt(h, ViewLeft + 1, ViewTop), Snapshot(h));
        Assert.AreEqual('C', ChAt(h, ViewLeft, ViewTop + 1), Snapshot(h));
        Assert.AreEqual('D', ChAt(h, ViewLeft + 1, ViewTop + 1), Snapshot(h));
        Assert.IsFalse(TitleDirty(h), "Freshly loaded buffer must not show the dirty marker.");
    }

    [TestMethod]
    public void TypingInsertsCharacterAndMarksDirty()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Inserted char should be at buffer start.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop), $"Body should show X first.\n{Snapshot(h)}");
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
    }

    [TestMethod]
    public void CtrlQExitsCleanlyWhenNotDirty()
    {
        var h = Boot();
        h.Editor.QueueInput(CtrlQ);
        RunUntilDone(h);
        Assert.AreEqual(ExitQuit, h.Bus.ReadRam(TestResult), "Clean Ctrl-Q should exit with QUIT.");
    }

    [TestMethod]
    public void CtrlQWhileDirtyShowsThreeChoiceDialog()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, CtrlQ);
        string screen = Snapshot(h);
        Assert.IsTrue(screen.Contains("Workspace Modified", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("Exit Anyway", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("Save First", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("Cancel", StringComparison.Ordinal), screen);
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Dialog open: editor must not have exited.");
    }

    [TestMethod]
    public void DialogCancelReturnsToEditorWithTextRestored()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, CtrlQ);
        Assert.IsTrue(ScreenContains(h, "Exit Anyway"), "Pre-condition: dialog open.");
        Type(h, (byte)'c'); // Cancel
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone), "Cancel must keep the editor open.");
        Assert.IsFalse(ScreenContains(h, "Exit Anyway"), "Dialog should be dismissed after cancel.");
        Assert.AreEqual('X', ChAt(h, ViewLeft, ViewTop),
            $"Body must be restored under the dialog after cancel.\n{Snapshot(h)}");
        Assert.AreEqual((byte)'X', h.Bus.ReadRam(TextBuf), "Draft text must be intact after cancel.");
    }

    [TestMethod]
    public void DialogExitAnywayDiscardsButPreservesDraft()
    {
        var h = Boot();
        Type(h, (byte)'X');
        Type(h, CtrlQ);
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
        Type(h, CtrlQ);
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
        Type(h, CtrlQ);
        Type(h, (byte)'s'); // Save first -> validation fails
        Assert.AreNotEqual(0xAA, h.Bus.ReadRam(TestDone),
            "A failed Save First must keep the editor open, not exit.");
        Assert.IsTrue(h.Bus.ReadRam(TestSaveCnt) >= 1, "Save hook must have been attempted.");
        Assert.IsFalse(ScreenContains(h, "Exit Anyway"), "Dialog should close after a (failed) save.");
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
        Assert.AreEqual(0x2A, h.Bus.Vgc.GetScreenColor(ViewLeft, ViewTop),
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
        var h = Boot();
        // Cursor starts at 0. Move down to line 2 ("CD"), home, backspace deletes
        // the newline joining the lines into "ABCD".
        Type(h, KeyDown, KeyHome, KeyBackspace);
        Assert.AreEqual('A', ChAt(h, ViewLeft, ViewTop), Snapshot(h));
        Assert.AreEqual('B', ChAt(h, ViewLeft + 1, ViewTop), Snapshot(h));
        Assert.AreEqual('C', ChAt(h, ViewLeft + 2, ViewTop), Snapshot(h));
        Assert.AreEqual('D', ChAt(h, ViewLeft + 3, ViewTop), Snapshot(h));
    }
}
