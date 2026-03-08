# Editor Refactor: Unified ScreenTextEditor Base Class

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Extract a shared `ScreenTextEditor` abstract base class from `BasicEditor` and `NccEditor`, unifying all common editor mechanics and giving both editors the same visual style, controls, and feature set.

**Architecture:** Both editors render to VGC character/color RAM at 80x25. The base class owns the text buffer, cursor, selection, clipboard, search, prompt system, file browser, and rendering framework (box borders, title/status/message bars, code area with virtual gutter). `BasicEditor` adds BASIC line numbers, tokenizer memory round-trip, and screen save/restore lifecycle. `NccEditor` adds NCC compiler, debugger, and CPU reboot lifecycle. Both use an `ISyntaxHighlighter` interface for language-specific coloring.

**Tech Stack:** C#/.NET 10, Avalonia, MSTest, VGC character/color RAM rendering

---

## Reference: Key Files

| File | Purpose |
|------|---------|
| `e6502.Avalonia/Editor/BasicEditor.cs` | Current BASIC editor (~997 lines) |
| `e6502.Avalonia/Editor/NccEditor.cs` | Current NCC editor (~1202 lines) |
| `e6502.Avalonia/Editor/BasicSyntaxHighlighter.cs` | BASIC keyword highlighter |
| `e6502.Avalonia/Editor/SyntaxHighlighter.cs` | C syntax highlighter |
| `e6502.Avalonia/Rendering/EmulatorCanvas.cs` | Keyboard routing to editors |
| `e6502.Avalonia/MainWindow.axaml.cs` | Editor lifecycle wiring |
| `e6502.Avalonia/Hardware/VgcConstants.cs` | Screen/register constants |
| `e6502.Avalonia/Hardware/CompositeBusDevice.cs` | Has `DeviceManager` property |
| `e6502.Storage/DeviceManager.cs` | 6-device storage manager (HD0/HD1/FD0-FD3) |
| `e6502.Storage/IStorageDevice.cs` | Device interface with Load/Save/ListDirectory |
| `e6502UnitTests/BasicEditorTests.cs` | 31 existing BasicEditor tests |
| `e6502UnitTests/Editor/NccEditorTests.cs` | Existing NccEditor tests |
| `e6502UnitTests/Editor/BasicSyntaxHighlighterTests.cs` | Existing highlighter tests |

## Reference: Screen Layout (Unified for Both Editors)

```
Row 0:  ╔═ Title Bar (virtual GetTitle()) ═════════════╗
Row 1:  ║ [gutter] code line 1                         ║
Row 2:  ║ [gutter] code line 2                         ║
 ...    ║ [gutter] ...                                  ║
Row 22: ║ [gutter] code line 22                        ║
Row 23: ╠═ Ln N Col N  INS  Free: NNNNN ══════════════╣
Row 24: ╚═ Help keys / prompt / message ═══════════════╝
```

- Box borders use CP437 double-line characters (╔═╗║╚╝╠╣)
- 22 visible code lines (rows 1-22), code starts at column 1, ends at column 78
- Gutter width from `GetGutterWidth()`: BasicEditor=6 (line numbers), NccEditor=0 (edit) or 5 (debug)
- Hardware cursor positioned via VGC registers, cursor cell colored bright red (color 2)
- NccEditor debug mode: code rows 1-18, separator row 19, watch panel rows 20-22

## Reference: Virtual/Abstract Methods

```csharp
// Required
abstract string GetTitle();                          // title bar text
abstract string GetStatusExtra();                    // extra status bar info
abstract string GetHelpText();                       // message row default help
abstract ISyntaxHighlighter GetHighlighter();        // language highlighter
abstract int GetGutterWidth();                       // gutter char width
abstract void RenderGutterCell(int lineIndex, int row);  // draw gutter for one line
abstract string GetFileExtension();                  // ".bas" or ".c"

// Serialization for file save/load
abstract byte[] SerializeForSave(string filename);   // buffer → file bytes
abstract void DeserializeOnLoad(byte[] data, string filename); // file bytes → buffer

// Lifecycle hooks
virtual bool HandleEditorKey(Key, KeyModifiers);     // subclass keybindings
virtual void OnActivate();                           // after base activation
virtual void OnDeactivate();                         // after base deactivation
virtual void OnAfterRedrawCode();                    // hook for debug watch panel
virtual int GetVisibleLineCount();                   // override for debug mode (18 vs 22)
```

## Reference: Selection Model

- `_selAnchorLine`, `_selAnchorCol` — where shift was first held
- `_selActive` — whether selection exists
- Range = anchor to cursor (ordered by position)
- Shift+movement extends selection; non-shift movement clears it
- Typing/backspace/delete with selection: delete selection first
- Ctrl+C copies selection (or whole line if none); Ctrl+X cuts; Ctrl+V pastes
- Selected chars rendered with distinct highlight color (e.g. color 3/cyan fg on inverted bg)

## Reference: File Browser

Uses `DeviceManager` directly (not through 6502 registers). Layout:

```
╔═ Open File ══════════════════════════════════════════╗
║ [HD0:] /games                                        ║
║ ──────────────────────────────────────────────────── ║
║   ../                                                ║
║   subdir/                                            ║
║ > pacman.bas          1024 bytes                     ║
║   snake.bas            512 bytes                     ║
╠═ ↑↓:Navigate  Enter:Open  Tab:Device  Esc:Cancel ═══╣
╚══════════════════════════════════════════════════════╝
```

- Tab cycles through mounted devices
- Enter on directory navigates in; `../` goes up
- Enter on file opens it (calls `DeserializeOnLoad`)
- Directories listed first, then files filtered by `GetFileExtension()`
- Ctrl+S saves via `SerializeForSave` to current device/directory

## Reference: Keyboard Map (Base Class)

| Key | Action |
|-----|--------|
| Arrow keys | Move cursor (clamp to bounds, wrap at line ends) |
| Shift+Arrow | Extend/start selection |
| Ctrl+Arrow | Word jump |
| Home/End | Start/end of line |
| Shift+Home/End | Select to start/end of line |
| Ctrl+Home/End | Top/bottom of file |
| Shift+Up/Down | Extend selection by line |
| PageUp/PageDown | Scroll by visible lines |
| Enter | Insert newline |
| Backspace | Delete char before cursor (or join lines, or delete selection) |
| Delete | Delete char at cursor (or join lines, or delete selection) |
| Tab | Insert 2 spaces |
| Insert | Toggle insert/overwrite mode |
| Ctrl+F | Find (prompt) |
| Ctrl+G | Go to line (prompt) |
| Ctrl+C | Copy selection (or line) |
| Ctrl+X | Cut selection (or line) |
| Ctrl+V | Paste |
| Ctrl+Y | Delete current line |
| Ctrl+D | Duplicate current line |
| Ctrl+S | Save file |
| Ctrl+O | Open file (prompt) |
| Ctrl+N | New file |
| F3 | File browser |
| Escape | Clear selection / cancel prompt / subclass behavior |
| Ctrl+Q | Exit with confirmation |
| Other F-keys | Passed to subclass `HandleEditorKey()` |

---

## Task 1: Create ISyntaxHighlighter Interface

**Files:**
- Create: `e6502.Avalonia/Editor/ISyntaxHighlighter.cs`
- Modify: `e6502.Avalonia/Editor/BasicSyntaxHighlighter.cs`
- Modify: `e6502.Avalonia/Editor/SyntaxHighlighter.cs`

**Step 1: Create the interface**

```csharp
namespace e6502.Avalonia.Editor;

public interface ISyntaxHighlighter
{
    byte[] HighlightLine(string line);
    void Reset();
}
```

**Step 2: Make BasicSyntaxHighlighter implement it**

Add `: ISyntaxHighlighter` to the class declaration. Add a no-op `Reset()` method:

```csharp
public sealed class BasicSyntaxHighlighter : ISyntaxHighlighter
{
    // ... existing code ...
    public void Reset() { }
}
```

**Step 3: Make SyntaxHighlighter implement it**

Add `: ISyntaxHighlighter` to the class declaration. It already has `Reset()` and `HighlightLine()`.

```csharp
public sealed class SyntaxHighlighter : ISyntaxHighlighter
```

**Step 4: Run tests**

```bash
dotnet test --filter "SyntaxHighlighter" --verbosity normal
```

Expected: All existing highlighter tests pass unchanged.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/ISyntaxHighlighter.cs e6502.Avalonia/Editor/BasicSyntaxHighlighter.cs e6502.Avalonia/Editor/SyntaxHighlighter.cs
git commit -m "refactor: extract ISyntaxHighlighter interface"
```

---

## Task 2: Create ScreenTextEditor Base Class — Text Buffer & Cursor

This is the core task. Create the abstract base class with the text buffer (plain `List<string>`, no line numbers), cursor state, and movement. Do NOT touch BasicEditor or NccEditor yet — they continue working independently. We'll migrate them in later tasks.

**Files:**
- Create: `e6502.Avalonia/Editor/ScreenTextEditor.cs`
- Create: `e6502UnitTests/Editor/ScreenTextEditorTests.cs`

**Step 1: Write failing tests for text buffer basics**

Create a test stub class and test file. The stub is a minimal concrete subclass:

```csharp
using Avalonia.Input;
using e6502.Avalonia.Editor;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

/// <summary>Minimal concrete subclass for testing the base class.</summary>
internal sealed class TestEditor : ScreenTextEditor
{
    private readonly BasicSyntaxHighlighter _hl = new();

    public TestEditor(IBusDevice bus) : base(bus) { }

    // For testing: allow setting DeviceManager to null
    public TestEditor(IBusDevice bus, bool _) : base(bus) { }

    protected override string GetTitle() => "TEST EDITOR";
    protected override string GetStatusExtra() => "";
    protected override string GetHelpText() => "Test help";
    protected override ISyntaxHighlighter GetHighlighter() => _hl;
    protected override int GetGutterWidth() => 0;
    protected override void RenderGutterCell(int lineIndex, int row) { }
    protected override string GetFileExtension() => ".txt";
    protected override byte[] SerializeForSave(string filename)
        => System.Text.Encoding.ASCII.GetBytes(string.Join('\n', Lines));
    protected override void DeserializeOnLoad(byte[] data, string filename)
    {
        var text = System.Text.Encoding.ASCII.GetString(data);
        LoadLines(text.Split('\n'));
    }
}

[TestClass]
public class ScreenTextEditorTests
{
    private BusDevice _bus = null!;
    private TestEditor _editor = null!;

    [TestInitialize]
    public void Setup()
    {
        _bus = new BusDevice(new byte[65536], 0);
        _editor = new TestEditor(_bus);
    }

    [TestMethod]
    public void NewEditor_HasOneEmptyLine()
    {
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("", _editor.GetLineText(0));
    }

    [TestMethod]
    public void InsertChar_AppendsToLine()
    {
        _editor.InsertChar('H');
        _editor.InsertChar('i');
        Assert.AreEqual("Hi", _editor.GetLineText(0));
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void InsertNewline_SplitsLine()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.SetCursor(0, 2);
        _editor.InsertNewline();
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual("AB", _editor.GetLineText(0));
        Assert.AreEqual("C", _editor.GetLineText(1));
        Assert.AreEqual(1, _editor.CursorLine);
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void Backspace_DeletesCharBefore()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.Backspace();
        Assert.AreEqual("AB", _editor.GetLineText(0));
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void Backspace_AtStartOfLine_JoinsWithPrevious()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(1, 0);
        _editor.Backspace();
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("AB", _editor.GetLineText(0));
        Assert.AreEqual(0, _editor.CursorLine);
        Assert.AreEqual(1, _editor.CursorCol);
    }

    [TestMethod]
    public void Delete_RemovesCharAtCursor()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.SetCursor(0, 1);
        _editor.Delete();
        Assert.AreEqual("AC", _editor.GetLineText(0));
    }

    [TestMethod]
    public void Delete_AtEndOfLine_JoinsWithNext()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(0, 1);
        _editor.Delete();
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("AB", _editor.GetLineText(0));
    }

    [TestMethod]
    public void DeleteCurrentLine_RemovesLine()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(0, 0);
        _editor.DeleteCurrentLine();
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("B", _editor.GetLineText(0));
    }

    [TestMethod]
    public void DeleteCurrentLine_LastLine_LeavesEmptyLine()
    {
        _editor.InsertChar('A');
        _editor.DeleteCurrentLine();
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("", _editor.GetLineText(0));
    }

    [TestMethod]
    public void DuplicateLine_CreatesNewLine()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.DuplicateLine();
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual("AB", _editor.GetLineText(0));
        Assert.AreEqual("AB", _editor.GetLineText(1));
        Assert.AreEqual(1, _editor.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_DownUp()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(0, 0);
        _editor.MoveCursor(0, 1);
        Assert.AreEqual(1, _editor.CursorLine);
        _editor.MoveCursor(0, -1);
        Assert.AreEqual(0, _editor.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_UpAtTop_StaysAtTop()
    {
        _editor.MoveCursor(0, -1);
        Assert.AreEqual(0, _editor.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_LeftAtStartOfLine_WrapsUp()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(1, 0);
        _editor.MoveCursor(-1, 0);
        Assert.AreEqual(0, _editor.CursorLine);
        Assert.AreEqual(1, _editor.CursorCol);
    }

    [TestMethod]
    public void MoveCursor_RightAtEndOfLine_WrapsDown()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.SetCursor(0, 1);
        _editor.MoveCursor(1, 0);
        Assert.AreEqual(1, _editor.CursorLine);
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void Home_MovesCursorToStart()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.Home();
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void End_MovesCursorToEnd()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.SetCursor(0, 0);
        _editor.End();
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void PageDown_MovesVisibleLines()
    {
        for (int i = 0; i < 30; i++)
        {
            _editor.InsertChar((char)('A' + (i % 26)));
            _editor.InsertNewline();
        }
        _editor.SetCursor(0, 0);
        _editor.PageDown();
        Assert.AreEqual(22, _editor.CursorLine);
    }

    [TestMethod]
    public void CursorCol_ClampedToLineLength()
    {
        _editor.LoadLines(["LONGLINE", "AB"]);
        _editor.SetCursor(0, 8);
        _editor.MoveCursor(0, 1);
        Assert.AreEqual(1, _editor.CursorLine);
        Assert.IsTrue(_editor.CursorCol <= 2);
    }

    [TestMethod]
    public void InsertOverwriteMode_Toggle()
    {
        Assert.IsTrue(_editor.InsertMode);
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.SetCursor(0, 1);
        // Switch to overwrite
        _editor.ToggleInsertMode();
        Assert.IsFalse(_editor.InsertMode);
        _editor.InsertChar('X');
        Assert.AreEqual("AXC", _editor.GetLineText(0));
    }

    [TestMethod]
    public void Modified_SetOnTextChange()
    {
        Assert.IsFalse(_editor.Modified);
        _editor.InsertChar('X');
        Assert.IsTrue(_editor.Modified);
    }

    [TestMethod]
    public void Modified_SetOnInsertNewline()
    {
        Assert.IsFalse(_editor.Modified);
        _editor.InsertNewline();
        Assert.IsTrue(_editor.Modified);
    }

    [TestMethod]
    public void Modified_SetOnDelete()
    {
        _editor.InsertChar('A');
        _editor.ClearModified();
        Assert.IsFalse(_editor.Modified);
        _editor.SetCursor(0, 0);
        _editor.Delete();
        Assert.IsTrue(_editor.Modified);
    }

    [TestMethod]
    public void LoadLines_SetsBuffer()
    {
        _editor.LoadLines(["Hello", "World"]);
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual("Hello", _editor.GetLineText(0));
        Assert.AreEqual("World", _editor.GetLineText(1));
        Assert.IsFalse(_editor.Modified);
    }

    [TestMethod]
    public void LoadLines_Empty_HasOneEmptyLine()
    {
        _editor.LoadLines([]);
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("", _editor.GetLineText(0));
    }

    [TestMethod]
    public void WordJumpRight_SkipsToNextWord()
    {
        _editor.LoadLines(["hello world test"]);
        _editor.SetCursor(0, 0);
        _editor.MoveWordRight();
        // Should skip "hello" and whitespace, land on "world"
        Assert.AreEqual(6, _editor.CursorCol);
    }

    [TestMethod]
    public void WordJumpLeft_SkipsToPreviousWord()
    {
        _editor.LoadLines(["hello world test"]);
        _editor.SetCursor(0, 11);
        _editor.MoveWordLeft();
        // Should land on start of "world"
        Assert.AreEqual(6, _editor.CursorCol);
    }

    [TestMethod]
    public void HorizontalScroll_UpdatesWhenCursorExceedsWidth()
    {
        string longLine = new string('X', 100);
        _editor.LoadLines([longLine]);
        _editor.SetCursor(0, 90);
        _editor.EnsureVisible();
        Assert.IsTrue(_editor.ScrollX > 0);
    }
}
```

**Step 2: Implement the ScreenTextEditor base class**

Create `e6502.Avalonia/Editor/ScreenTextEditor.cs`. This is a large file. Key sections:

- **Constants**: Screen layout (ScreenCols=80, ScreenRows=25, TitleRow=0, CodeStartRow=1, CodeEndRow=22, StatusRow=23, MessageRow=24, VisibleLines=22), CP437 box chars, colors (EditorBg=6, BorderFg=14, TitleFg=1, StatusFg=14, MessageFg=7, ErrorFg=2, CursorFg=2, DefaultFg=1)
- **State**: `_lines` (List<string>), `_cursorLine`, `_cursorCol`, `_scrollY`, `_scrollX`, `_modified`, `_insertMode`, selection state, prompt state, clipboard, message
- **Abstract/virtual methods**: As specified in the Reference section above
- **Text operations**: InsertChar, InsertNewline, Backspace, Delete, DeleteCurrentLine, DuplicateLine — all operate on plain strings (no line numbers)
- **Cursor**: MoveCursor, Home, End, PageUp, PageDown, MoveWordRight, MoveWordLeft, EnsureVisible
- **Public accessors**: LineCount, GetLineText, SetCursor, CursorLine, CursorCol, Modified, InsertMode, ScrollX, Lines (IReadOnlyList<string>), LoadLines, ClearModified, ToggleInsertMode

The text buffer operations are nearly identical to NccEditor's current implementation (plain strings). Port them directly but make rendering methods call the virtual hooks.

**Important implementation notes:**
- `MoveCursor` must call `RedrawCode()` + `RedrawStatusBar()` (not just UpdateCursor) to handle scrolling and cursor color
- `InsertChar` should respect `_insertMode`
- All text-modifying operations set `_modified = true` and call `RedrawCode()` + `RedrawStatusBar()`
- `LoadLines(string[] lines)` replaces buffer, resets cursor/scroll, clears modified
- `EnsureVisible()` handles both vertical and horizontal scrolling

**Step 3: Run tests**

```bash
dotnet test --filter "ScreenTextEditor" --verbosity normal
```

Expected: All tests pass.

**Step 4: Commit**

```bash
git add e6502.Avalonia/Editor/ScreenTextEditor.cs e6502UnitTests/Editor/ScreenTextEditorTests.cs
git commit -m "feat: ScreenTextEditor base class with text buffer and cursor"
```

---

## Task 3: Add Selection Model to ScreenTextEditor

**Files:**
- Modify: `e6502.Avalonia/Editor/ScreenTextEditor.cs`
- Modify: `e6502UnitTests/Editor/ScreenTextEditorTests.cs`

**Step 1: Write failing tests**

Add to `ScreenTextEditorTests.cs`:

```csharp
// ── Selection tests ──────────────────────────────────────────────────────

[TestMethod]
public void ShiftRight_StartsSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursorWithSelection(1, 0);
    Assert.IsTrue(_editor.HasSelection);
    Assert.AreEqual("H", _editor.GetSelectedText());
}

[TestMethod]
public void ShiftRight_ExtendsSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    Assert.AreEqual("He", _editor.GetSelectedText());
}

[TestMethod]
public void NonShiftMove_ClearsSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursorWithSelection(1, 0);
    Assert.IsTrue(_editor.HasSelection);
    _editor.MoveCursor(1, 0);
    Assert.IsFalse(_editor.HasSelection);
}

[TestMethod]
public void ShiftDown_SelectsAcrossLines()
{
    _editor.LoadLines(["Hello", "World"]);
    _editor.SetCursor(0, 3);
    _editor.MoveCursorWithSelection(0, 1);
    string selected = _editor.GetSelectedText();
    Assert.AreEqual("lo\nWor", selected);
}

[TestMethod]
public void ShiftHome_SelectsToStartOfLine()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 3);
    _editor.SelectToHome();
    Assert.AreEqual("Hel", _editor.GetSelectedText());
}

[TestMethod]
public void ShiftEnd_SelectsToEndOfLine()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 1);
    _editor.SelectToEnd();
    Assert.AreEqual("ello", _editor.GetSelectedText());
}

[TestMethod]
public void InsertChar_WithSelection_ReplacesSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 1);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    // Selected "el"
    _editor.InsertChar('X');
    Assert.AreEqual("HXlo", _editor.GetLineText(0));
}

[TestMethod]
public void Backspace_WithSelection_DeletesSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 1);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.Backspace();
    Assert.AreEqual("Hlo", _editor.GetLineText(0));
    Assert.AreEqual(1, _editor.CursorCol);
}

[TestMethod]
public void Delete_WithSelection_DeletesSelection()
{
    _editor.LoadLines(["Hello"]);
    _editor.SetCursor(0, 1);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.Delete();
    Assert.AreEqual("Hlo", _editor.GetLineText(0));
}

[TestMethod]
public void MultiLineSelection_Delete()
{
    _editor.LoadLines(["Hello", "World", "Test"]);
    _editor.SetCursor(0, 3);
    _editor.MoveCursorWithSelection(0, 1);
    _editor.MoveCursorWithSelection(0, 1);
    // Selected from (0,3) to (2,3) = "lo\nWorld\nTes"
    _editor.Delete();
    Assert.AreEqual(1, _editor.LineCount);
    Assert.AreEqual("Helt", _editor.GetLineText(0));
}

[TestMethod]
public void CopyLine_NoSelection_CopiesWholeLine()
{
    _editor.LoadLines(["Hello", "World"]);
    _editor.SetCursor(0, 0);
    _editor.CopySelection();
    _editor.SetCursor(1, 5);
    _editor.PasteClipboard();
    // Whole line paste inserts a new line
    Assert.AreEqual(3, _editor.LineCount);
    Assert.AreEqual("Hello", _editor.GetLineText(0));
    Assert.AreEqual("World", _editor.GetLineText(1));
    Assert.AreEqual("Hello", _editor.GetLineText(2));
}

[TestMethod]
public void CopySelection_PastesSelectedText()
{
    _editor.LoadLines(["Hello World"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    // Selected "Hello"
    _editor.CopySelection();
    _editor.ClearSelection();
    _editor.SetCursor(0, 11);
    _editor.PasteClipboard();
    Assert.AreEqual("Hello WorldHello", _editor.GetLineText(0));
}

[TestMethod]
public void CutSelection_RemovesAndCopies()
{
    _editor.LoadLines(["Hello World"]);
    _editor.SetCursor(0, 5);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    _editor.MoveCursorWithSelection(1, 0);
    // Selected " World"
    _editor.CutSelection();
    Assert.AreEqual("Hello", _editor.GetLineText(0));
    // Paste it back
    _editor.PasteClipboard();
    Assert.AreEqual("Hello World", _editor.GetLineText(0));
}
```

**Step 2: Implement selection**

Add to `ScreenTextEditor`:

- Fields: `_selAnchorLine`, `_selAnchorCol`, `_selActive`
- `MoveCursorWithSelection(dx, dy)` — sets anchor on first call, then moves cursor
- `SelectToHome()`, `SelectToEnd()` — shift+home/end
- `GetSelectedText()` — returns text between anchor and cursor
- `DeleteSelection()` — removes selected text, positions cursor at start
- `HasSelection` property, `ClearSelection()` method
- Modify `InsertChar`, `Backspace`, `Delete` to call `DeleteSelection()` first if `_selActive`
- `CopySelection()`, `CutSelection()`, `PasteClipboard()` — clipboard ops with selection awareness
- `RedrawCode()` must render selected characters with highlight color

**Step 3: Update HandleKeyDown for shift+movement**

In the base class keyboard handler, detect `KeyModifiers.Shift` on arrow/home/end keys and call `MoveCursorWithSelection` instead of `MoveCursor`.

**Step 4: Run tests**

```bash
dotnet test --filter "ScreenTextEditor" --verbosity normal
```

**Step 5: Commit**

```bash
git commit -m "feat: text selection with shift+movement, clipboard integration"
```

---

## Task 4: Add Rendering to ScreenTextEditor

**Files:**
- Modify: `e6502.Avalonia/Editor/ScreenTextEditor.cs`

**Step 1: Implement rendering methods**

Port the rendering code from NccEditor's box-border style:

- `Redraw()` — calls RedrawTitleBar, RedrawCode, RedrawStatusBar, RedrawMessageRow
- `RedrawTitleBar()` — box top row with `GetTitle()` text
- `RedrawCode()` — box-bordered code area with gutter (calls `RenderGutterCell`), syntax highlighting (calls `GetHighlighter()`), selection highlighting, then calls `UpdateCursor()` and `OnAfterRedrawCode()`
- `RedrawStatusBar()` — box middle row with Ln/Col, insert mode, `GetStatusExtra()`
- `RedrawMessageRow()` — box bottom row with `GetHelpText()`, prompt text, or message
- `UpdateCursor()` — position hardware cursor, set cursor cell color to red (color 2)
- `WriteChar(col, row, ch, fg)` — write to VGC char+color RAM
- `ClearScreen()` — fill screen with spaces
- `WriteBorderedRow(row, text, fg)` — content row with left/right box borders

**Step 2: Implement screen save/restore**

- Fields: `_savedCharRam[2000]`, `_savedColorRam[2000]`, `_savedBgColor`, `_savedCursorEnable`, `_savedCursorX`, `_savedCursorY`
- `SaveScreen()` — reads all char/color RAM + cursor state from bus
- `RestoreScreen()` — writes saved state back to bus
- `Activate()` — sets `IsActive`, calls `SaveScreen()`, sets bg color, clears screen, calls `OnActivate()`, calls `Redraw()`
- `Deactivate()` — calls `OnDeactivate()`, sets `IsActive=false`

The subclass `OnDeactivate()` decides whether to call `RestoreScreen()` (BasicEditor) or reboot CPU (NccEditor).

**Step 3: Implement prompt system**

Port from current BasicEditor/NccEditor:

- `StartPrompt(text, callback)`, `HandlePromptKey(key)`, `ShowMessage(msg)`
- Prompt renders in the message row (row 24)

**Step 4: Implement search**

- `StartFind()` — prompts for search term, calls `FindNext()`
- `FindNext(term)` — searches forward from cursor with wrap-around
- `StartGoToLine()` — prompts for line number, jumps to it

**Step 5: Run tests and verify build**

```bash
dotnet build
dotnet test --filter "ScreenTextEditor" --verbosity normal
```

**Step 6: Commit**

```bash
git commit -m "feat: ScreenTextEditor rendering, screen save/restore, prompt, search"
```

---

## Task 5: Add File Browser to ScreenTextEditor

**Files:**
- Modify: `e6502.Avalonia/Editor/ScreenTextEditor.cs`
- Modify: `e6502UnitTests/Editor/ScreenTextEditorTests.cs`

**Step 1: Add DeviceManager dependency**

Add a `DeviceManager?` property that can be set after construction (like `SetRunDependencies` pattern). The file browser only works when this is set.

```csharp
public DeviceManager? DeviceManager { get; set; }
```

**Step 2: Implement file browser mode**

- `OpenFileBrowser()` — sets `Mode = EditorMode.FileBrowser`, populates file list from current device
- `HandleFileBrowserKey(key)` — Up/Down navigate, Enter opens file or enters directory, Tab cycles devices, Escape cancels, `../` navigates up
- `RedrawFileBrowser()` — renders device/path header, directory entries first, then files filtered by `GetFileExtension()`, with size column
- File list comes from `device.ListDirectory(null)` filtered and sorted

**Step 3: Implement save/load**

- `SaveFile()` — if no filename, prompts for one. Calls `SerializeForSave(filename)` to get bytes, then `device.Save(name, data, ext)`
- `LoadFileFromDevice(device, name)` — calls `device.Load(name, ext)`, then `DeserializeOnLoad(data, name)`
- Ctrl+S, Ctrl+O, F3 key bindings in base class HandleKeyDown

**Step 4: Write tests**

Test file browser navigation logic (device cycling, directory entry filtering) using mock/stub if needed. At minimum test that the mode transitions work:

```csharp
[TestMethod]
public void CtrlS_WithoutDeviceManager_ShowsMessage()
{
    // DeviceManager is null — save should show error message, not crash
    _editor.SaveFile();
    // Just verify no exception
}
```

**Step 5: Run tests**

```bash
dotnet test --filter "ScreenTextEditor" --verbosity normal
```

**Step 6: Commit**

```bash
git commit -m "feat: device-aware file browser with save/load"
```

---

## Task 6: Add HandleKeyDown and HandleTextInput to ScreenTextEditor

**Files:**
- Modify: `e6502.Avalonia/Editor/ScreenTextEditor.cs`

**Step 1: Implement the full keyboard handler**

The base class `HandleKeyDown(Key, KeyModifiers)` handles:

1. If `_prompting` → delegate to `HandlePromptKey`
2. If `Mode == FileBrowser` → delegate to `HandleFileBrowserKey`
3. If exit prompt showing → handle Y/N
4. Escape handling (clear selection, clear message, or delegate to subclass)
5. Ctrl+Q → exit with confirmation (if modified, show Y/N prompt)
6. Call `HandleEditorKey(key, modifiers)` — if subclass handles it, return true
7. Ctrl combos: F (find), G (goto), C (copy), X (cut), V (paste), Y (delete line), D (duplicate), S (save), O (open), N (new), Home (top), End (bottom)
8. Shift+movement → selection methods
9. Ctrl+Arrow → word jump
10. Arrow/Home/End/PageUp/PageDown → movement
11. Enter, Backspace, Delete, Tab, Insert

`HandleTextInput(string)` handles printable character insertion (routes to prompt or InsertChar).

**Step 2: Verify build**

```bash
dotnet build
```

**Step 3: Commit**

```bash
git commit -m "feat: ScreenTextEditor keyboard handling with all shortcuts"
```

---

## Task 7: Migrate BasicEditor to Inherit from ScreenTextEditor

This is the key migration task. `BasicEditor` becomes a thin subclass.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs` (rewrite to ~200 lines)
- Modify: `e6502UnitTests/BasicEditorTests.cs` (keep only BASIC-specific tests)
- Modify: `e6502.Avalonia/MainWindow.axaml.cs` (wire DeviceManager)

**Step 1: Rewrite BasicEditor as subclass**

```csharp
using Avalonia.Input;
using e6502.Avalonia.Hardware;
using e6502.Storage;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

public sealed class BasicEditor : ScreenTextEditor
{
    private readonly BasicSyntaxHighlighter _highlighter = new();
    private BasicTokenizer? _tokenizer;
    private ScreenEditor? _screenEditor;
    private Cpu? _cpu;

    // BASIC line numbers stored parallel to base class _lines
    private readonly List<int> _lineNumbers = new();
    private const int MaxLineNumber = 63999;
    private const int DefaultStep = 10;

    public BasicEditor(IBusDevice bus) : base(bus) { }

    public void SetRunDependencies(Cpu cpu, ScreenEditor screenEditor)
    {
        _cpu = cpu;
        _screenEditor = screenEditor;
    }

    // ── Abstract overrides ──────────────────────────────────────────────────

    protected override string GetTitle()
    {
        string mod = Modified ? " [*]" : "";
        return $"NOVA EDIT{mod}";
    }

    protected override string GetStatusExtra()
    {
        int freeBytes = 0;
        try
        {
            ushort progEnd = (ushort)(Bus.Read(VgcConstants.ZpSvarl) | (Bus.Read(VgcConstants.ZpSvarh) << 8));
            int available = VgcConstants.BasicEnd - progEnd;
            if (available > 0) freeBytes = available;
        }
        catch { }
        return $"Free: {freeBytes}";
    }

    protected override string GetHelpText() => "F5:Run  Esc:Exit  ^F:Find  ^G:Goto  ^S:Save  F3:Open";

    protected override ISyntaxHighlighter GetHighlighter() => _highlighter;

    protected override int GetGutterWidth() => 6; // 5-digit line number + space

    protected override void RenderGutterCell(int lineIndex, int row)
    {
        if (lineIndex < _lineNumbers.Count)
        {
            string num = _lineNumbers[lineIndex].ToString().PadLeft(5);
            for (int g = 0; g < 5; g++)
                WriteChar(1 + g, row, (byte)num[g], GutterFg);
            WriteChar(6, row, (byte)' ', GutterFg);
        }
    }

    protected override string GetFileExtension() => ".bas";

    protected override byte[] SerializeForSave(string filename)
    {
        var tok = GetTokenizer();
        if (tok == null) return [];
        string[] textLines = ToNumberedTextLines();
        return tok.Tokenize(textLines, 0x0301); // standard BASIC base
    }

    protected override void DeserializeOnLoad(byte[] data, string filename)
    {
        var tok = GetTokenizer();
        if (tok == null) return;
        string[] textLines = tok.Detokenize(data);
        LoadFromNumberedLines(textLines);
    }

    // ── Lifecycle hooks ──────────────────────────────────────────────────

    protected override void OnActivate()
    {
        var tok = GetTokenizer();
        if (tok != null) ReadFromMemory(tok);
    }

    protected override void OnDeactivate()
    {
        var tok = GetTokenizer();
        if (tok != null) WriteToMemory(tok);
        RestoreScreen();
    }

    // ── Subclass key handling ────────────────────────────────────────────

    protected override bool HandleEditorKey(Key key, KeyModifiers modifiers)
    {
        if (key == Key.F5) { Run(); return true; }
        if (key == Key.Escape) { Deactivate(); return true; }
        return false;
    }

    // ── BASIC-specific: line numbers ────────────────────────────────────

    // Override LoadLines to also manage line numbers
    // Override InsertNewline to auto-number new lines
    // etc.

    // ── Run mode ──────────────────────────────────────────────────────────

    public void Run() { /* same as current */ }
    public void ReturnFromRun() { /* same as current */ }

    // ── Memory round-trip ────────────────────────────────────────────────

    internal void ReadFromMemory(BasicTokenizer tokenizer) { /* same as current */ }
    internal void WriteToMemory(BasicTokenizer tokenizer) { /* same as current */ }
}
```

Key points:
- BASIC line numbers stored in a parallel `List<int> _lineNumbers` synced with the base `_lines`
- Override `InsertNewline()` to auto-number with midpoint calculation
- Override `DeleteCurrentLine()` to remove from `_lineNumbers` too
- `LoadFromNumberedLines(string[])` parses "10 PRINT ..." format, populates both lists
- `ToNumberedTextLines()` reassembles "10 PRINT ..." format
- `RenderGutterCell` draws the 5-digit right-justified line number
- `OnActivate` reads from memory; `OnDeactivate` writes to memory + restores screen
- `HandleEditorKey` handles F5 (run) and Escape (deactivate) — these are BasicEditor-specific behaviors
- Ctrl+Q exit confirmation is handled by the base class

**Step 2: Update BasicEditorTests**

Remove tests that moved to `ScreenTextEditorTests` (cursor movement, basic text ops, modified flag). Keep only:
- Line number management tests (auto-numbering, renumber, max 63999)
- Memory round-trip tests (ReadFromMemory, WriteToMemory)
- `ToNumberedTextLines` / `LoadFromNumberedLines` tests
- Run/ReturnFromRun tests
- Serialize/Deserialize tests

**Step 3: Wire DeviceManager in MainWindow**

In `MainWindow.axaml.cs`, after creating `_basicEditor`, set:

```csharp
_basicEditor.DeviceManager = _compositeBus.DeviceManager;
```

**Step 4: Update EmulatorCanvas**

Change `BasicEditor` property type to `ScreenTextEditor` if possible, or keep as `BasicEditor`. The keyboard routing should work unchanged since the `HandleKeyDown`/`HandleTextInput`/`IsActive`/`Mode` API is the same.

**Step 5: Run all tests**

```bash
dotnet test --verbosity normal
```

Expected: All existing tests pass (some relocated, none lost).

**Step 6: Commit**

```bash
git commit -m "refactor: BasicEditor inherits from ScreenTextEditor"
```

---

## Task 8: Migrate NccEditor to Inherit from ScreenTextEditor

**Files:**
- Modify: `e6502.Avalonia/Editor/NccEditor.cs` (rewrite to ~300 lines)
- Modify: `e6502UnitTests/Editor/NccEditorTests.cs` (keep only NCC-specific tests)
- Modify: `e6502.Avalonia/MainWindow.axaml.cs` (wire DeviceManager)

**Step 1: Rewrite NccEditor as subclass**

```csharp
public sealed class NccEditor : ScreenTextEditor
{
    private readonly SyntaxHighlighter _highlighter = new();
    private readonly DebuggerService _debugger;
    private readonly Cpu _cpu;

    // Compile/debug state
    private CompileResult? _lastCompileResult;
    private Dictionary<int, ushort>? _lineMap;
    private readonly HashSet<int> _breakpointLines = new();
    private bool _isRunning;
    private string _filename = "untitled";

    public NccEditor(IBusDevice bus, DebuggerService debugger, Cpu cpu) : base(bus)
    {
        _debugger = debugger;
        _cpu = cpu;
    }

    // ── Abstract overrides ──────────────────────────────────────────────────

    protected override string GetTitle()
    {
        string mod = Modified ? " [modified]" : "";
        return $"Nova C Compiler v1.0 - {_filename}{mod}";
    }

    protected override string GetStatusExtra()
    {
        string modeStr = Mode switch
        {
            EditorMode.Debug => "DEBUG",
            EditorMode.Running => "RUNNING",
            _ => "EDIT"
        };
        return modeStr;
    }

    protected override string GetHelpText()
    {
        if (Mode == EditorMode.Debug)
            return "F2:Brk F5:Run F7:Into F8:Step Esc:Edit ^Q:BASIC";
        return "F3:Open F5:Run F6:Debug F9:Build ^S:Save ^N:New ^Q:BASIC";
    }

    protected override ISyntaxHighlighter GetHighlighter() => _highlighter;

    protected override int GetGutterWidth() => Mode == EditorMode.Debug ? 5 : 0;

    protected override void RenderGutterCell(int lineIndex, int row)
    {
        if (Mode != EditorMode.Debug) return;
        // line number + breakpoint marker + execution indicator
        string lineNum = (lineIndex + 1).ToString().PadLeft(3);
        char marker = _breakpointLines.Contains(lineIndex) ? '*' : ' ';
        bool isCurrent = lineIndex == GetSourceLineFromPC(_cpu.Pc);
        char indicator = isCurrent ? '>' : ' ';
        string gutter = $"{lineNum}{marker}{indicator}";
        for (int c = 0; c < 5 && c < gutter.Length; c++)
        {
            byte fg = GutterFg;
            if (c == 3 && marker == '*') fg = BreakpointFg;
            if (c == 4 && indicator == '>') fg = CurrentLineFg;
            WriteChar(1 + c, row, (byte)gutter[c], fg);
        }
    }

    protected override string GetFileExtension() => ".c";

    protected override byte[] SerializeForSave(string filename)
    {
        _filename = Path.GetFileName(filename);
        return System.Text.Encoding.UTF8.GetBytes(string.Join('\n', Lines));
    }

    protected override void DeserializeOnLoad(byte[] data, string filename)
    {
        _filename = Path.GetFileName(filename);
        string content = System.Text.Encoding.UTF8.GetString(data);
        LoadLines(content.Split('\n'));
        _highlighter.Reset();
    }

    // ── Lifecycle ────────────────────────────────────────────────────────

    protected override void OnActivate()
    {
        _debugger.Pause();
    }

    protected override void OnDeactivate()
    {
        // Restore black background, reboot CPU to BASIC
        Bus.Write(VgcConstants.RegBgCol, 0);
        ClearScreen();
        Bus.Write(VgcConstants.RegCursorEnable, 1);
        _cpu.Boot();
        _debugger.Resume();
    }

    // ── Subclass key handling ────────────────────────────────────────────

    protected override bool HandleEditorKey(Key key, KeyModifiers modifiers)
    {
        if (Mode == EditorMode.Edit)
        {
            switch (key)
            {
                case Key.F5: CompileAndRun(); return true;
                case Key.F6: EnterDebugMode(); return true;
                case Key.F9: CompileOnly(); return true;
            }
        }
        if (Mode == EditorMode.Debug)
        {
            switch (key)
            {
                case Key.F2: ToggleBreakpoint(); return true;
                case Key.F5: DebugContinue(); return true;
                case Key.F7: DebugStepInto(); return true;
                case Key.F8: DebugStepLine(); return true;
                case Key.Escape: ExitDebugMode(); return true;
            }
        }
        if (Mode == EditorMode.Running && key == Key.Escape)
        {
            StopRunning(); return true;
        }
        return false;
    }

    protected override int GetVisibleLineCount()
    {
        return Mode == EditorMode.Debug ? 18 : base.GetVisibleLineCount();
    }

    protected override void OnAfterRedrawCode()
    {
        if (Mode == EditorMode.Debug)
        {
            RedrawDebugSeparator();
            RedrawWatchPanel();
        }
    }

    // ── Compile, Run, Debug methods (same as current) ──────────────────
    // CompileOnly, CompileAndRun, LoadAndRun, StopRunning
    // EnterDebugMode, ExitDebugMode, ToggleBreakpoint
    // DebugContinue, DebugStepLine, DebugStepInto
    // GetSourceLineFromPC, CheckDebugBreak, JumpToErrorLine
    // RedrawDebugSeparator, RedrawWatchPanel
}
```

Key points:
- NccEditor's `_lines` becomes the base class `_lines` (already plain strings)
- Debug mode uses `GetVisibleLineCount()` override to return 18
- `OnAfterRedrawCode()` draws the debug separator and watch panel
- `OnDeactivate()` reboots CPU (does NOT call `RestoreScreen()`)
- All compile/debug methods remain NccEditor-specific
- Gains search, clipboard, selection, insert/overwrite, horizontal scroll for free from base class

**Step 2: Update NccEditorTests**

Remove tests that are now in `ScreenTextEditorTests` (InsertChar, Backspace, Delete, InsertNewline, cursor movement, Home/End, PageUp/Down, ScrollY). Keep:
- HandleKeyDown return value tests (F5, character keys, inactive)
- Exit confirmation tests (Ctrl+Q, Y/N)
- Compile-specific tests if any

**Step 3: Wire DeviceManager**

In `MainWindow.axaml.cs`:

```csharp
_nccEditor.DeviceManager = _compositeBus.DeviceManager;
```

**Step 4: Update EmulatorCanvas**

Consider changing `NccEditor` property type to `ScreenTextEditor`. Or better: have a single `ScreenTextEditor?` property that can hold either editor. The routing code checks `IsActive` and `Mode` which are on the base class.

**Step 5: Run all tests**

```bash
dotnet test --verbosity normal
```

Expected: All tests pass.

**Step 6: Commit**

```bash
git commit -m "refactor: NccEditor inherits from ScreenTextEditor"
```

---

## Task 9: Unify EmulatorCanvas Routing

**Files:**
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs`

**Step 1: Simplify editor references**

Replace the two separate properties:

```csharp
// Before:
public NccEditor? NccEditor { get; set; }
public BasicEditor? BasicEditor { get; set; }

// After:
private readonly List<ScreenTextEditor> _editors = new();

public void AddEditor(ScreenTextEditor editor) => _editors.Add(editor);
```

**Step 2: Simplify keyboard routing**

In `OnKeyDown`, find the first active editor:

```csharp
var activeEditor = _editors.FirstOrDefault(e => e is { IsActive: true, Mode: not EditorMode.Running });
if (activeEditor != null)
{
    if (activeEditor.HandleKeyDown(e.Key, e.KeyModifiers))
        e.Handled = true;
    base.OnKeyDown(e);
    return;
}
```

Similarly for `OnTextInput`.

The F5 activation and Ctrl+N activation remain in MainWindow (since they need to know which specific editor to activate and enforce mutual exclusion).

**Step 3: Update MainWindow wiring**

```csharp
_canvas.AddEditor(_basicEditor);
_canvas.AddEditor(_nccEditor);
```

**Step 4: Run all tests and manually verify**

```bash
dotnet build
dotnet test --verbosity normal
```

**Step 5: Commit**

```bash
git commit -m "refactor: EmulatorCanvas uses ScreenTextEditor base for routing"
```

---

## Task 10: Delete Dead Code and Final Cleanup

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs` — remove any duplicated methods
- Modify: `e6502.Avalonia/Editor/NccEditor.cs` — remove any duplicated methods
- Modify: `e6502.Avalonia/Editor/ScreenTextEditor.cs` — any final adjustments

**Step 1: Review for dead code**

Search for any methods in BasicEditor or NccEditor that are now in the base class but weren't removed. Remove them. Check for unused `using` statements, unused fields, etc.

**Step 2: Verify no duplicate constants**

Colors, screen layout constants, box-drawing chars should only exist in `ScreenTextEditor`. Subclasses may define their own constants (like `BreakpointFg`, `CurrentLineFg`, `WatchFg` in NccEditor) but should not duplicate base constants.

**Step 3: Run full test suite**

```bash
dotnet test --verbosity normal
```

Expected: All tests pass, 0 warnings related to the editor code.

**Step 4: Commit**

```bash
git commit -m "chore: remove dead code from editor refactor"
```

---

## Task Summary

| Task | Description | Est. Lines Changed |
|------|-------------|-------------------|
| 1 | ISyntaxHighlighter interface | ~10 |
| 2 | ScreenTextEditor base: text buffer + cursor | ~400 create, ~60 test |
| 3 | Selection model | ~200 add, ~80 test |
| 4 | Rendering, screen save/restore, prompt, search | ~300 add |
| 5 | File browser with DeviceManager | ~200 add |
| 6 | HandleKeyDown / HandleTextInput | ~100 add |
| 7 | Migrate BasicEditor | ~200 rewrite, ~50 test adjust |
| 8 | Migrate NccEditor | ~300 rewrite, ~30 test adjust |
| 9 | Unify EmulatorCanvas routing | ~30 modify |
| 10 | Dead code cleanup | ~50 delete |

**Dependencies:** Tasks must be done in order (1→2→3→4→5→6→7→8→9→10). Tasks 7 and 8 could theoretically be done in parallel but should be sequential to avoid merge conflicts.
