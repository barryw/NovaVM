# Nova BASIC Full-Screen Editor Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a Turbo Pascal-style full-screen editor for BASIC programs, activated by F5, with syntax highlighting, run-return-to-editor loop, and all standard editing operations.

**Architecture:** A new `BasicEditor` class in `e6502.Avalonia/Editor/` renders directly to VGC character/color RAM (same pattern as `NccEditor`). F5 in BASIC mode activates it; F5 inside the editor tokenizes the buffer into BASIC memory and runs the program; when the program ends (Ready prompt detected), the editor re-activates. Esc exits to BASIC immediate mode. Uses `BasicTokenizer` from `e6502.Storage` to round-trip between text and tokenized BASIC.

**Tech Stack:** C# (Avalonia), VGC character/color RAM, BasicTokenizer, EhBASIC memory layout

---

## Reference: Key Files and Patterns

Before starting, read and understand these files:

| File | Why |
|------|-----|
| `e6502.Avalonia/Editor/NccEditor.cs` | **Primary pattern.** Full-screen editor with VGC rendering, cursor management, keyboard handling, Running mode, syntax highlighting. The BasicEditor follows this exact pattern. |
| `e6502.Avalonia/Editor/SyntaxHighlighter.cs` | NCC syntax highlighter. BasicEditor needs its own BASIC-specific one. |
| `e6502.Avalonia/Rendering/EmulatorCanvas.cs:86-198` | Keyboard routing. F5 must be intercepted here before it reaches the ScreenEditor. BasicEditor routing follows the NccEditor pattern. |
| `e6502.Avalonia/MainWindow.axaml.cs:43,78-114,220-225,352-374` | NccEditor lifecycle: creation, assignment to canvas, 60Hz timer checks for program-end detection, toggle activation. BasicEditor needs the same wiring. |
| `e6502.Avalonia/Input/ScreenEditor.cs` | BASIC input handler. Must NOT be changed except that F5 needs to be intercepted before keys reach it. |
| `e6502.Storage/BasicTokenizer.cs` | Tokenize (text→binary) and Detokenize (binary→text). Used to load/save BASIC programs from/to memory. |
| `e6502.Avalonia/Hardware/VgcConstants.cs` | Screen dimensions, CharRamBase, ColorRamBase, register addresses. |
| `e6502.Avalonia/Hardware/CompositeBusDevice.cs` | Bus device with RAM access. Read $002B/$002C for program start, $002D/$002E for program end. |

## Reference: BASIC Memory Layout

```
$002B/$002C  Program start pointer (typically $0401)
$002D/$002E  Program end pointer (first byte after program)
$0401+       Tokenized BASIC program data

Tokenized format per line:
  [next-ptr: 2B LE] [line-num: 2B LE] [tokenized body] [$00]
Program terminates with $00 $00
```

## Reference: Screen Layout

```
Row 0:    Title bar     "NOVA EDIT  F5:Run  Esc:Exit"
Rows 1-23: Code area    5-char gutter (line number) + 1 space + 74 chars code
Row 24:   Status bar    "Ln 10 Col 5  INS  Free: 38214  READY"
```

- Gutter: 5 chars right-justified line number in dim color, non-editable
- Code: 74 chars of editable text per visible line
- Total visible code lines: 23
- Background color: dark blue (VGC color 6), same as NccEditor

## Reference: Color Scheme

```
GutterFg    = 12  (grey)       - line numbers
KeywordFg   = 1   (white)      - BASIC keywords (PRINT, FOR, IF, etc.)
StringFg    = 5   (green)      - quoted strings
NumberFg    = 8   (orange)     - numeric literals
CommentFg   = 12  (grey)       - REM statements
DefaultFg   = 14  (light blue) - variables, operators
TitleFg     = 1   (white)      - title bar text
StatusFg    = 14  (light blue) - status bar text
BorderBg    = 6   (blue)       - background
```

---

## Task 1: BasicSyntaxHighlighter

Create a BASIC-specific syntax highlighter that colorizes line text (without the line number, which is handled by the gutter).

**Files:**
- Create: `e6502.Avalonia/Editor/BasicSyntaxHighlighter.cs`
- Test: `e6502UnitTests/BasicSyntaxHighlighterTests.cs`

**Step 1: Write tests**

```csharp
[TestClass]
public class BasicSyntaxHighlighterTests
{
    private readonly BasicSyntaxHighlighter _hl = new();

    [TestMethod]
    public void Keywords_AreHighlighted()
    {
        byte[] colors = _hl.HighlightLine("PRINT \"HELLO\"");
        // PRINT = 5 chars, all keyword color
        for (int i = 0; i < 5; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i]);
    }

    [TestMethod]
    public void Strings_AreHighlighted()
    {
        byte[] colors = _hl.HighlightLine("PRINT \"HELLO\"");
        // "HELLO" starts at index 6 (the quote), length 7 including quotes
        for (int i = 6; i <= 12; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorString, colors[i]);
    }

    [TestMethod]
    public void Numbers_AreHighlighted()
    {
        byte[] colors = _hl.HighlightLine("X=42");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorNumber, colors[2]);
        Assert.AreEqual(BasicSyntaxHighlighter.ColorNumber, colors[3]);
    }

    [TestMethod]
    public void Rem_GreysOutRest()
    {
        byte[] colors = _hl.HighlightLine("REM THIS IS A COMMENT");
        for (int i = 0; i < colors.Length; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorComment, colors[i]);
    }

    [TestMethod]
    public void Variables_AreDefaultColor()
    {
        byte[] colors = _hl.HighlightLine("X=Y+Z");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[0]); // X
    }

    [TestMethod]
    public void KeywordsAreCaseInsensitive()
    {
        byte[] colors = _hl.HighlightLine("print \"hi\"");
        for (int i = 0; i < 5; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i]);
    }

    [TestMethod]
    public void EmptyLine_ReturnsEmptyArray()
    {
        byte[] colors = _hl.HighlightLine("");
        Assert.AreEqual(0, colors.Length);
    }

    [TestMethod]
    public void DataKeyword_GreysOutRest()
    {
        byte[] colors = _hl.HighlightLine("DATA 1,2,3,HELLO");
        // DATA keyword itself
        for (int i = 0; i < 4; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i]);
        // Rest after DATA is data literal color (same as comment/grey for readability)
        for (int i = 5; i < colors.Length; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorComment, colors[i]);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "BasicSyntaxHighlighter" --verbosity normal`
Expected: Build error — class does not exist

**Step 3: Implement BasicSyntaxHighlighter**

```csharp
namespace e6502.Avalonia.Editor;

/// <summary>
/// Per-line syntax highlighter for BASIC source code.
/// Input is the code portion of a line (without the line number).
/// Returns a color byte per character for VGC color RAM.
/// </summary>
public sealed class BasicSyntaxHighlighter
{
    public const byte ColorDefault  = 14;  // light blue — variables, operators
    public const byte ColorKeyword  = 1;   // white
    public const byte ColorString   = 5;   // green
    public const byte ColorNumber   = 8;   // orange
    public const byte ColorComment  = 12;  // grey

    // EhBASIC keywords — must be sorted longest-first for greedy matching
    private static readonly string[] Keywords = [
        "DIROPEN", "DIRNAM$", "DIRNEXT", "DIRTYP", "DIRSIZ",
        "MIDPLAY", "MIDSTOP", "SIDPLAY", "SIDSTOP",
        "RETURN", "RESTORE", "RENUM",
        "PRINT", "GOSUB", "INPUT", "WHILE", "WEND",
        "MUSIC", "MOUNT", "MKDIR", "RMDIR",
        "COLOR", "SLEEP",
        "THEN", "ELSE", "STEP", "NEXT", "GOTO", "READ",
        "LOAD", "SAVE", "POKE", "PEEK", "CALL",
        "META$", "DIR",
        "FOR", "TO", "IF", "ON", "OR", "LET", "DIM", "DEF",
        "END", "RUN", "NEW", "CLR", "REM", "NOT", "AND",
        "CHR$", "STR$", "LEFT$", "RIGHT$", "MID$", "LEN", "VAL",
        "ASC", "SPC", "TAB", "FN", "ABS", "SGN", "INT", "SQR",
        "RND", "LOG", "EXP", "COS", "SIN", "TAN", "ATN",
        "FRE", "POS", "DATA", "CONT", "LIST", "STOP",
        "GET", "PUT", "WAIT", "USR", "DEL", "PWD", "CD",
    ];

    public byte[] HighlightLine(string line)
    {
        var colors = new byte[line.Length];
        if (line.Length == 0) return colors;

        int i = 0;

        while (i < line.Length)
        {
            // Check for REM — everything after is comment
            if (i + 3 <= line.Length &&
                string.Compare(line, i, "REM", 0, 3, StringComparison.OrdinalIgnoreCase) == 0 &&
                (i == 0 || !char.IsLetterOrDigit(line[i - 1])) &&
                (i + 3 >= line.Length || !char.IsLetterOrDigit(line[i + 3])))
            {
                for (int j = i; j < line.Length; j++)
                    colors[j] = ColorComment;
                return colors;
            }

            // Check for DATA — keyword colored, rest is grey
            if (i + 4 <= line.Length &&
                string.Compare(line, i, "DATA", 0, 4, StringComparison.OrdinalIgnoreCase) == 0 &&
                (i == 0 || !char.IsLetterOrDigit(line[i - 1])) &&
                (i + 4 >= line.Length || !char.IsLetterOrDigit(line[i + 4])))
            {
                for (int j = i; j < i + 4; j++)
                    colors[j] = ColorKeyword;
                for (int j = i + 4; j < line.Length; j++)
                    colors[j] = ColorComment;
                return colors;
            }

            // String literal
            if (line[i] == '"')
            {
                colors[i] = ColorString;
                i++;
                while (i < line.Length)
                {
                    colors[i] = ColorString;
                    if (line[i] == '"') { i++; break; }
                    i++;
                }
                continue;
            }

            // Number
            if (char.IsDigit(line[i]) || (line[i] == '.' && i + 1 < line.Length && char.IsDigit(line[i + 1])))
            {
                // Don't color numbers that are part of variable names (e.g. A1)
                if (i > 0 && char.IsLetter(line[i - 1]))
                {
                    colors[i] = ColorDefault;
                    i++;
                    continue;
                }
                while (i < line.Length && (char.IsDigit(line[i]) || line[i] == '.'))
                {
                    colors[i] = ColorNumber;
                    i++;
                }
                continue;
            }

            // Keyword check (only at word boundary)
            if (char.IsLetter(line[i]))
            {
                bool matched = false;
                foreach (string kw in Keywords)
                {
                    if (i + kw.Length > line.Length) continue;
                    if (string.Compare(line, i, kw, 0, kw.Length, StringComparison.OrdinalIgnoreCase) != 0)
                        continue;
                    // Check trailing boundary: keyword must not be followed by a letter
                    // (but can be followed by digit, space, operator, paren, etc.)
                    // Exception: keywords ending in $ are already bounded
                    if (!kw.EndsWith('$') && i + kw.Length < line.Length && char.IsLetter(line[i + kw.Length]))
                        continue;
                    for (int j = 0; j < kw.Length; j++)
                        colors[i + j] = ColorKeyword;
                    i += kw.Length;
                    matched = true;
                    break;
                }
                if (matched) continue;

                // Variable name — skip to end of identifier
                colors[i] = ColorDefault;
                i++;
                while (i < line.Length && (char.IsLetterOrDigit(line[i]) || line[i] == '$' || line[i] == '%'))
                {
                    colors[i] = ColorDefault;
                    i++;
                }
                continue;
            }

            // Operators, spaces, punctuation
            colors[i] = ColorDefault;
            i++;
        }

        return colors;
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "BasicSyntaxHighlighter" --verbosity normal`
Expected: All tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicSyntaxHighlighter.cs e6502UnitTests/BasicSyntaxHighlighterTests.cs
git commit -m "feat: add BASIC syntax highlighter for full-screen editor"
```

---

## Task 2: BasicEditor Core — Text Buffer and Display

Create the `BasicEditor` class with text buffer, cursor, scrolling, and VGC rendering. No keyboard handling yet — just the data model and display.

**Files:**
- Create: `e6502.Avalonia/Editor/BasicEditor.cs`
- Test: `e6502UnitTests/BasicEditorTests.cs`

**Step 1: Write tests**

```csharp
[TestClass]
public class BasicEditorTests
{
    private BusDevice _rawBus = null!;
    private BasicEditor _editor = null!;

    [TestInitialize]
    public void Setup()
    {
        _rawBus = new BusDevice();
        _editor = new BasicEditor(_rawBus);
    }

    [TestMethod]
    public void LoadProgram_ParsesLinesWithNumbers()
    {
        _editor.LoadText(["10 PRINT \"HELLO\"", "20 GOTO 10"]);
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual(10, _editor.GetLineNumber(0));
        Assert.AreEqual("PRINT \"HELLO\"", _editor.GetLineText(0));
        Assert.AreEqual(20, _editor.GetLineNumber(1));
        Assert.AreEqual("GOTO 10", _editor.GetLineText(1));
    }

    [TestMethod]
    public void LoadProgram_EmptyProgram_HasOneLine()
    {
        _editor.LoadText([]);
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual(10, _editor.GetLineNumber(0));
        Assert.AreEqual("", _editor.GetLineText(0));
    }

    [TestMethod]
    public void InsertNewline_SplitsLine()
    {
        _editor.LoadText(["10 PRINT \"HELLO\""]);
        _editor.SetCursor(0, 6); // after "PRINT "
        _editor.InsertNewline();
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual(10, _editor.GetLineNumber(0));
        Assert.AreEqual("PRINT ", _editor.GetLineText(0));
        Assert.AreEqual(15, _editor.GetLineNumber(1)); // auto-numbered
        Assert.AreEqual("\"HELLO\"", _editor.GetLineText(1));
    }

    [TestMethod]
    public void InsertNewline_BetweenLines_AutoNumbers()
    {
        _editor.LoadText(["100 A=1", "110 B=2"]);
        _editor.SetCursor(0, 3); // end of line 0 text
        _editor.InsertNewline();
        Assert.AreEqual(3, _editor.LineCount);
        Assert.AreEqual(105, _editor.GetLineNumber(1)); // midpoint
    }

    [TestMethod]
    public void InsertNewline_NoSpace_Renumbers()
    {
        _editor.LoadText(["100 A=1", "101 B=2"]);
        _editor.SetCursor(0, 3);
        _editor.InsertNewline();
        // Should renumber: 100, 105, 110 (or similar with spacing)
        Assert.IsTrue(_editor.GetLineNumber(0) < _editor.GetLineNumber(1));
        Assert.IsTrue(_editor.GetLineNumber(1) < _editor.GetLineNumber(2));
    }

    [TestMethod]
    public void DeleteLine_RemovesLine()
    {
        _editor.LoadText(["10 A=1", "20 B=2", "30 C=3"]);
        _editor.SetCursor(1, 0);
        _editor.DeleteCurrentLine();
        Assert.AreEqual(2, _editor.LineCount);
        Assert.AreEqual(10, _editor.GetLineNumber(0));
        Assert.AreEqual(30, _editor.GetLineNumber(1));
    }

    [TestMethod]
    public void ToTextLines_ReassemblesWithLineNumbers()
    {
        _editor.LoadText(["10 PRINT \"HI\"", "20 END"]);
        string[] lines = _editor.ToTextLines();
        Assert.AreEqual("10 PRINT \"HI\"", lines[0]);
        Assert.AreEqual("20 END", lines[1]);
    }

    [TestMethod]
    public void InsertChar_InsertsAtCursor()
    {
        _editor.LoadText(["10 AB"]);
        _editor.SetCursor(0, 1); // between A and B
        _editor.InsertChar('X');
        Assert.AreEqual("AXB", _editor.GetLineText(0));
    }

    [TestMethod]
    public void Backspace_DeletesCharBefore()
    {
        _editor.LoadText(["10 ABC"]);
        _editor.SetCursor(0, 2); // after B
        _editor.Backspace();
        Assert.AreEqual("AC", _editor.GetLineText(0));
    }

    [TestMethod]
    public void Backspace_AtStartOfLine_JoinsToPrevious()
    {
        _editor.LoadText(["10 ABC", "20 DEF"]);
        _editor.SetCursor(1, 0); // start of line 2
        _editor.Backspace();
        Assert.AreEqual(1, _editor.LineCount);
        Assert.AreEqual("ABCDEF", _editor.GetLineText(0));
    }
}
```

**Step 2: Run tests — expect build failure**

**Step 3: Implement BasicEditor core**

Create `e6502.Avalonia/Editor/BasicEditor.cs`. This is a large class. Key sections:

**Data model:**
- `List<(int LineNumber, string Text)> _lines` — the program buffer
- `_cursorLine`, `_cursorCol`, `_scrollY` — cursor and scroll state
- `_modified` flag

**Line number management:**
- `LoadText(string[] lines)` — parse "10 PRINT ..." into (lineNum, text) tuples
- `InsertNewline()` — split at cursor, auto-number the new line as midpoint of neighbors; if no space, trigger renumber
- `Renumber(int start, int step)` — renumber all lines from start with given step
- `ToTextLines()` — reassemble "10 PRINT ..." strings

**Text editing:**
- `InsertChar(char c)` — insert at cursor position in current line's text
- `Backspace()` — delete before cursor; at line start, join with previous
- `Delete()` — delete at cursor; at line end, join with next
- `DeleteCurrentLine()` — remove entire line (Ctrl+Y)

**Display (VGC rendering):**
- Follow `NccEditor` pattern exactly: `WriteChar()`, `ClearScreen()`, `Redraw()`, `RedrawCode()`, `RedrawTitleBar()`, `RedrawStatusBar()`
- Gutter renders line numbers right-justified in 5 chars + 1 space, using `GutterFg` color
- Code area renders with `BasicSyntaxHighlighter` colors
- Cursor rendered via color inversion (same as NccEditor)
- `UpdateCursor()` — draw cursor at `_cursorLine`/`_cursorCol` mapped to screen coordinates

**Scrolling:**
- Vertical: `_scrollY` adjusts when cursor moves above/below visible area
- Horizontal: BASIC lines are short enough that horizontal scroll is unlikely needed, but support it with `_scrollX` if a line exceeds 74 chars

**Constructor:** `BasicEditor(IBusDevice bus)` — takes the bus for VGC writes. No CPU reference needed (unlike NccEditor which compiles/runs NCC code). The CPU interaction will be added in Task 4.

The class should implement properties `IsActive`, `Mode` (using the existing `EditorMode` enum values `Edit` and `Running`), and methods `Activate()`, `Deactivate()`.

`Activate()`:
- Set `IsActive = true`
- Disable VGC cursor: `_bus.Write(VgcConstants.RegCursorEnable, 0)`
- Set blue background: `_bus.Write(VgcConstants.RegBgCol, 6)`
- Read current BASIC program from memory (detokenize)
- Call `Redraw()`

`Deactivate()`:
- Set `IsActive = false`
- Tokenize buffer back into BASIC memory
- Restore black background: `_bus.Write(VgcConstants.RegBgCol, 0)`
- Clear screen
- Re-enable VGC cursor: `_bus.Write(VgcConstants.RegCursorEnable, 1)`

**Step 4: Run tests — all pass**

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs e6502UnitTests/BasicEditorTests.cs
git commit -m "feat: BasicEditor core — text buffer, line numbers, display rendering"
```

---

## Task 3: BasicEditor Keyboard Handling

Add keyboard input handling to BasicEditor (cursor movement, text editing, page navigation).

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`
- Test: `e6502UnitTests/BasicEditorTests.cs` (add tests)

**Step 1: Write tests for cursor movement**

```csharp
[TestMethod]
public void MoveCursorDown_MovesToNextLine()
{
    _editor.LoadText(["10 A=1", "20 B=2"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursor(0, 1);
    Assert.AreEqual(1, _editor.CursorLine);
}

[TestMethod]
public void MoveCursorUp_AtTop_StaysAtTop()
{
    _editor.LoadText(["10 A=1", "20 B=2"]);
    _editor.SetCursor(0, 0);
    _editor.MoveCursor(0, -1);
    Assert.AreEqual(0, _editor.CursorLine);
}

[TestMethod]
public void Home_MovesCursorToStartOfLine()
{
    _editor.LoadText(["10 PRINT X"]);
    _editor.SetCursor(0, 5);
    _editor.Home();
    Assert.AreEqual(0, _editor.CursorCol);
}

[TestMethod]
public void End_MovesCursorToEndOfLine()
{
    _editor.LoadText(["10 PRINT X"]);
    _editor.SetCursor(0, 0);
    _editor.End();
    Assert.AreEqual(7, _editor.CursorCol); // "PRINT X" length
}

[TestMethod]
public void PageDown_ScrollsByVisibleLines()
{
    var lines = new string[50];
    for (int i = 0; i < 50; i++)
        lines[i] = $"{(i + 1) * 10} REM LINE {i + 1}";
    _editor.LoadText(lines);
    _editor.SetCursor(0, 0);
    _editor.PageDown();
    Assert.AreEqual(23, _editor.CursorLine); // VisibleLines = 23
}
```

**Step 2: Run tests — expect failures**

**Step 3: Implement keyboard handling**

Add `HandleKeyDown(Key key, KeyModifiers modifiers)` method returning `bool` (consumed) and `HandleTextInput(string text)` method. Follow NccEditor's pattern exactly.

Key bindings:
```
Arrows          → MoveCursor (up/down/left/right)
Home            → Start of line text
End             → End of line text
Ctrl+Home       → Top of file
Ctrl+End        → Bottom of file
PageUp/PageDown → Scroll by VisibleLines (23)
Enter           → InsertNewline (split at cursor)
Backspace       → Delete char before cursor / join lines
Delete          → Delete char at cursor / join lines
Ctrl+Y          → Delete current line
Ctrl+D          → Duplicate current line
Tab             → Insert 2 spaces
Esc             → Tokenize to memory + exit to BASIC
F5              → Tokenize to memory + RUN (handled in Task 4)
Text input      → InsertChar for printable ASCII
```

**Step 4: Run tests — all pass**

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs e6502UnitTests/BasicEditorTests.cs
git commit -m "feat: BasicEditor keyboard handling — cursor, editing, navigation"
```

---

## Task 4: BASIC Memory Round-Trip (Tokenize/Detokenize)

Wire BasicEditor to read the current BASIC program from 6502 memory on activation, and write it back on deactivation or run. Uses `BasicTokenizer` from `e6502.Storage`.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`
- Test: `e6502UnitTests/BasicEditorTests.cs` (add tests)

**Step 1: Write tests**

```csharp
[TestMethod]
public void ReadFromMemory_DetokenizesProgram()
{
    // Write a tokenized program to bus memory
    var tokenizer = BasicTokenizer.FromJsonFile(FindTokensJson());
    string[] source = ["10 PRINT \"HELLO\"", "20 END"];
    byte[] tokenized = tokenizer.Tokenize(source, 0x0401);

    // Write load address header + body into bus
    _rawBus.Write(0x002B, 0x01); // program start low
    _rawBus.Write(0x002C, 0x04); // program start high
    for (int i = 0; i < tokenized.Length; i++)
        _rawBus.Write((ushort)(0x0401 + i), tokenized[i]);
    // Set program end pointer
    ushort end = (ushort)(0x0401 + tokenized.Length);
    _rawBus.Write(0x002D, (byte)(end & 0xFF));
    _rawBus.Write(0x002E, (byte)(end >> 8));

    _editor.ReadFromMemory(tokenizer);
    Assert.AreEqual(2, _editor.LineCount);
    Assert.AreEqual(10, _editor.GetLineNumber(0));
    Assert.AreEqual(20, _editor.GetLineNumber(1));
}

[TestMethod]
public void WriteToMemory_TokenizesProgram()
{
    var tokenizer = BasicTokenizer.FromJsonFile(FindTokensJson());
    _editor.LoadText(["10 PRINT \"HELLO\"", "20 END"]);

    // Write program start pointer
    _rawBus.Write(0x002B, 0x01);
    _rawBus.Write(0x002C, 0x04);

    _editor.WriteToMemory(tokenizer);

    // Read back and detokenize to verify round-trip
    ushort endPtr = (ushort)(_rawBus.Read(0x002D) | (_rawBus.Read(0x002E) << 8));
    Assert.IsTrue(endPtr > 0x0401);

    int len = endPtr - 0x0401;
    byte[] data = new byte[len];
    for (int i = 0; i < len; i++)
        data[i] = _rawBus.Read((ushort)(0x0401 + i));

    string[] lines = tokenizer.Detokenize(data);
    Assert.AreEqual(2, lines.Length);
    Assert.IsTrue(lines[0].Contains("PRINT"));
}

private static string FindTokensJson()
{
    string? dir = Directory.GetCurrentDirectory();
    while (dir != null)
    {
        string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
        if (File.Exists(candidate)) return candidate;
        string? parent = Path.GetDirectoryName(dir);
        if (parent == dir) break;
        dir = parent;
    }
    throw new FileNotFoundException("tokens.json not found");
}
```

**Step 2: Run tests — expect failures**

**Step 3: Implement memory round-trip**

Add to `BasicEditor`:

```csharp
/// <summary>
/// Reads the current BASIC program from 6502 memory and loads it into the editor buffer.
/// </summary>
public void ReadFromMemory(BasicTokenizer tokenizer)
{
    ushort start = (ushort)(_bus.Read(0x002B) | (_bus.Read(0x002C) << 8));
    ushort end   = (ushort)(_bus.Read(0x002D) | (_bus.Read(0x002E) << 8));

    if (end <= start)
    {
        LoadText([]);
        return;
    }

    int len = end - start;
    byte[] data = new byte[len];
    for (int i = 0; i < len; i++)
        data[i] = _bus.Read((ushort)(start + i));

    string[] lines = tokenizer.Detokenize(data);
    LoadText(lines);
    _modified = false;
}

/// <summary>
/// Tokenizes the editor buffer and writes it to 6502 memory.
/// Updates the program end pointer at $002D/$002E.
/// </summary>
public void WriteToMemory(BasicTokenizer tokenizer)
{
    ushort start = (ushort)(_bus.Read(0x002B) | (_bus.Read(0x002C) << 8));
    string[] lines = ToTextLines();
    byte[] tokenized = tokenizer.Tokenize(lines, start);

    for (int i = 0; i < tokenized.Length; i++)
        _bus.Write((ushort)(start + i), tokenized[i]);

    ushort end = (ushort)(start + tokenized.Length);
    _bus.Write(0x002D, (byte)(end & 0xFF));
    _bus.Write(0x002E, (byte)(end >> 8));
}
```

Also add a static method to find tokens.json at runtime (same pattern as NDI tool's `FindTokensJson`), and store the `BasicTokenizer` instance as a field initialized on first use.

**Step 4: Run tests — all pass**

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs e6502UnitTests/BasicEditorTests.cs
git commit -m "feat: BasicEditor BASIC memory round-trip via BasicTokenizer"
```

---

## Task 5: F5 Run-Return-to-Editor Loop

Implement the F5 workflow: tokenize buffer → inject RUN → detect program end → re-activate editor.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`

**Step 1: Implement Run mode in BasicEditor**

Add to BasicEditor:

```csharp
private ScreenEditor? _screenEditor;
private Cpu? _cpu;

/// <summary>Set by MainWindow after construction.</summary>
public void SetRunDependencies(Cpu cpu, ScreenEditor screenEditor)
{
    _cpu = cpu;
    _screenEditor = screenEditor;
}

/// <summary>
/// Tokenizes the buffer to memory, clears the screen, re-enables VGC cursor,
/// and injects "RUN\r" into the ScreenEditor input queue.
/// Sets Mode = Running so that keyboard input passes through to BASIC.
/// </summary>
public void Run()
{
    WriteToMemory(_tokenizer);

    // Restore normal BASIC display
    _bus.Write(VgcConstants.RegBgCol, 0);
    ClearScreen();
    _bus.Write(VgcConstants.RegCursorEnable, 1);

    Mode = EditorMode.Running;

    // Inject RUN command into BASIC input queue
    foreach (byte b in "RUN\r"u8)
        _screenEditor!.QueueInput(b);
}

/// <summary>
/// Called when "Ready" is detected on screen after a program run.
/// Re-activates the editor, re-reads the program from memory (in case it self-modified).
/// </summary>
public void ReturnFromRun()
{
    Mode = EditorMode.Edit;
    _bus.Write(VgcConstants.RegCursorEnable, 0);
    _bus.Write(VgcConstants.RegBgCol, 6);
    ReadFromMemory(_tokenizer);
    Redraw();
}
```

**Step 2: Wire into MainWindow 60Hz timer**

In `MainWindow.axaml.cs`, add a `_basicEditor` field alongside `_nccEditor`. In the 60Hz timer callback, add Ready detection:

```csharp
// In the timer callback, after NCC editor checks:
if (_basicEditor is { IsActive: true, Mode: EditorMode.Running })
{
    // Check if "Ready" has appeared on screen
    if (DetectReadyPrompt())
    {
        _basicEditor.ReturnFromRun();
    }
}
```

The `DetectReadyPrompt()` method scans VGC character RAM for "Ready" text (same approach as `EmulatorTcpServer.CmdWaitReady`):

```csharp
private bool DetectReadyPrompt()
{
    var vgc = _bus.Vgc;
    for (int row = 0; row < VgcConstants.ScreenRows; row++)
    {
        for (int col = 0; col <= VgcConstants.ScreenCols - 5; col++)
        {
            if (vgc.GetScreenChar(col, row) == 'R' &&
                vgc.GetScreenChar(col + 1, row) == 'e' &&
                vgc.GetScreenChar(col + 2, row) == 'a' &&
                vgc.GetScreenChar(col + 3, row) == 'd' &&
                vgc.GetScreenChar(col + 4, row) == 'y')
                return true;
        }
    }
    return false;
}
```

**Step 3: Test manually** — build and run the emulator, type a simple program, press F5 to enter editor, press F5 again to run, verify it returns to editor after "Ready".

**Step 4: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: F5 run-return-to-editor loop with Ready prompt detection"
```

---

## Task 6: Wire F5 into EmulatorCanvas and MainWindow

Connect the F5 key press to toggle the BasicEditor, and route keyboard input correctly.

**Files:**
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`

**Step 1: Add BasicEditor property to EmulatorCanvas**

In `EmulatorCanvas.cs`, add:
```csharp
public BasicEditor? BasicEditor { get; set; }
```

**Step 2: Add BasicEditor routing to OnKeyDown**

In `EmulatorCanvas.OnKeyDown()`, add a check at the top (after the NccEditor check, before the Ctrl+V check):

```csharp
// Route to BASIC editor when active (but not in Running mode — program gets ScreenEditor input)
if (BasicEditor is { IsActive: true, Mode: not EditorMode.Running })
{
    bool consumed = BasicEditor.HandleKeyDown(e.Key, e.KeyModifiers);
    if (consumed)
        e.Handled = true;
    base.OnKeyDown(e);
    return;
}

// F5: activate BASIC editor (only when NCC editor is not active)
if (e.Key == Key.F5 && NccEditor is not { IsActive: true })
{
    BasicEditor?.ToggleActivation();
    e.Handled = true;
    base.OnKeyDown(e);
    return;
}
```

**Step 3: Add BasicEditor routing to OnTextInput**

In `EmulatorCanvas.OnTextInput()`, add after the NccEditor routing:

```csharp
// Route to BASIC editor when active
if (BasicEditor is { IsActive: true, Mode: not EditorMode.Running })
{
    if (!string.IsNullOrEmpty(e.Text))
    {
        BasicEditor.HandleTextInput(e.Text);
        e.Handled = true;
    }
    base.OnTextInput(e);
    return;
}
```

**Step 4: Wire up in MainWindow**

In `MainWindow` constructor (after NccEditor creation):

```csharp
_basicEditor = new BasicEditor(_bus);
_basicEditor.SetRunDependencies(_cpu, _editor);
_canvas.BasicEditor = _basicEditor;
```

**Step 5: Build and test manually**

Run: `dotnet build && dotnet run --project e6502.Avalonia`
- Verify F5 opens the editor
- Verify typing works in the editor
- Verify Esc exits back to BASIC prompt
- Verify F5 in editor runs the program
- Verify program returns to editor after Ready

**Step 6: Commit**

```bash
git add e6502.Avalonia/Rendering/EmulatorCanvas.cs e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: wire F5 key to BasicEditor activation and keyboard routing"
```

---

## Task 7: Search (Ctrl+F) and Go To Line (Ctrl+G)

Add find functionality and go-to-line-number.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`

**Step 1: Implement prompt system**

Follow NccEditor's prompt pattern exactly — use `_prompting`, `_promptText`, `_promptInput`, `_promptCallback` fields. When prompting, draw the prompt on the status/message row and capture input until Enter/Esc.

**Step 2: Implement Ctrl+F (Find)**

- Prompt: "Find: " on the message row
- On Enter: search forward from cursor position for the text (case-insensitive)
- If found: move cursor to the match position, highlight the match
- If not found: show "Not found" message
- Ctrl+F again with empty input: repeat last search (find next)

**Step 3: Implement Ctrl+G (Go To Line)**

- Prompt: "Go to line: " on the message row
- On Enter: parse as integer, find the line with that line number, move cursor there
- If not found: show "Line not found" message

**Step 4: Test manually**

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs
git commit -m "feat: BasicEditor Ctrl+F find and Ctrl+G go to line"
```

---

## Task 8: Free Memory Display and Status Bar Polish

Show free BASIC memory in the status bar, insert/overwrite mode indicator, and modified flag.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`

**Step 1: Calculate free memory**

Free memory = top of BASIC RAM ($9FFF) minus program end pointer ($002D/$002E). Display as "Free: XXXXX" in the status bar.

**Step 2: Insert/Overwrite toggle**

Add `_insertMode` bool (default true). Toggle on Insert key. Display "INS" or "OVR" in status bar. When in overwrite mode, `InsertChar` replaces the character at cursor instead of inserting.

**Step 3: Modified flag**

Show `[*]` in the title bar when `_modified` is true. Set `_modified = true` on any text change. Set `_modified = false` after `WriteToMemory`.

**Step 4: Status bar format**

```
Ln 10 Col 5  INS  Free: 38214  [*]
```

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs
git commit -m "feat: BasicEditor status bar — free memory, INS/OVR, modified flag"
```

---

## Task 9: Clipboard Support (Ctrl+C/X/V) and Ctrl+Y Delete Line

Add clipboard operations for cutting/copying/pasting whole lines.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`

**Step 1: Implement line clipboard**

Keep a `List<string> _clipboard` buffer internal to the editor (no system clipboard needed for MVP).

- **Ctrl+C**: Copy current line text to clipboard
- **Ctrl+X**: Cut current line (copy + delete)
- **Ctrl+V**: Paste clipboard lines after current line, auto-numbering each
- **Ctrl+Y**: Delete current line (already implemented in Task 3, just ensure it's wired)

Show brief messages: "Copied", "Cut", "Pasted N lines", etc.

**Step 2: Test manually**

**Step 3: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs
git commit -m "feat: BasicEditor clipboard Ctrl+C/X/V and Ctrl+Y delete line"
```

---

## Task 10: Edge Cases and Polish

Handle edge cases, ensure robustness, final polish.

**Files:**
- Modify: `e6502.Avalonia/Editor/BasicEditor.cs`
- Test: `e6502UnitTests/BasicEditorTests.cs` (add edge case tests)

**Step 1: Edge case tests**

```csharp
[TestMethod]
public void DeleteLine_LastLine_LeavesEmptyLine()
{
    _editor.LoadText(["10 A=1"]);
    _editor.SetCursor(0, 0);
    _editor.DeleteCurrentLine();
    Assert.AreEqual(1, _editor.LineCount);
    Assert.AreEqual("", _editor.GetLineText(0));
}

[TestMethod]
public void InsertNewline_AtEndOfLastLine_AppendsNewLine()
{
    _editor.LoadText(["10 A=1"]);
    _editor.SetCursor(0, 3);
    _editor.InsertNewline();
    Assert.AreEqual(2, _editor.LineCount);
    Assert.AreEqual(20, _editor.GetLineNumber(1)); // default step
}

[TestMethod]
public void LineNumber_NeverExceeds63999()
{
    _editor.LoadText(["63990 A=1"]);
    _editor.SetCursor(0, 3);
    _editor.InsertNewline();
    Assert.IsTrue(_editor.GetLineNumber(1) <= 63999);
}

[TestMethod]
public void EmptyProgram_RoundTrips()
{
    var tokenizer = BasicTokenizer.FromJsonFile(FindTokensJson());
    _editor.LoadText([]);

    _rawBus.Write(0x002B, 0x01);
    _rawBus.Write(0x002C, 0x04);

    _editor.WriteToMemory(tokenizer);
    _editor.ReadFromMemory(tokenizer);

    // Should have one empty line
    Assert.AreEqual(1, _editor.LineCount);
}
```

**Step 2: Fix any failing tests**

**Step 3: Additional polish**

- Ensure cursor doesn't go past end of line text
- Ensure scroll position stays valid after delete/insert operations
- Ensure title bar shows "NOVA EDIT" with F5/Esc hints
- Handle the case where user types F5 when no program is loaded (just runs NEW then returns)
- Ensure the editor doesn't interfere with NccEditor (mutual exclusion — if NccEditor is active, F5 doesn't activate BasicEditor, and vice versa)

**Step 4: Run full test suite**

Run: `dotnet test --verbosity normal`
Expected: All tests pass (existing + new)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Editor/BasicEditor.cs e6502UnitTests/BasicEditorTests.cs
git commit -m "feat: BasicEditor edge cases and polish"
```

---

## Summary

| Task | Description | Key Files |
|------|-------------|-----------|
| 1 | BASIC syntax highlighter | `BasicSyntaxHighlighter.cs` |
| 2 | Core text buffer + VGC display | `BasicEditor.cs` |
| 3 | Keyboard handling | `BasicEditor.cs` |
| 4 | BASIC memory round-trip | `BasicEditor.cs`, `BasicTokenizer` |
| 5 | F5 run-return-to-editor loop | `BasicEditor.cs`, `MainWindow.axaml.cs` |
| 6 | Wire F5 into EmulatorCanvas | `EmulatorCanvas.cs`, `MainWindow.axaml.cs` |
| 7 | Search (Ctrl+F) + Go To Line | `BasicEditor.cs` |
| 8 | Status bar polish | `BasicEditor.cs` |
| 9 | Clipboard Ctrl+C/X/V | `BasicEditor.cs` |
| 10 | Edge cases and polish | `BasicEditor.cs` |
