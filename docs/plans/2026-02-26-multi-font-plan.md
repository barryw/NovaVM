# Multi-Font System Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add switchable bitmap font slots to Nova so programs can use PETSCII and other retro character sets via a `FONT n` BASIC command.

**Architecture:** BitmapFont grows from a single 2048-byte array to 8 font slots. A new VGC register (`RegFont` at `$A007`) selects the active font. The renderer, keyboard input, and help panel all consult this register. EhBASIC gets a `FONT` token that writes to the register.

**Tech Stack:** C#/.NET (Avalonia), 6502 assembly (EhBASIC), MSTest

**Design doc:** `docs/plans/2026-02-26-multi-font-design.md`

---

### Task 1: BitmapFont Multi-Slot Support

**Files:**
- Modify: `e6502.Avalonia/Rendering/BitmapFont.cs`
- Test: `e6502UnitTests/BitmapFontTests.cs`

**Step 1: Write failing tests for multi-font BitmapFont**

```csharp
// Add to BitmapFontTests.cs

[TestMethod]
public void MultiFontConstructor_LoadsMultipleFonts()
{
    var font0 = new byte[2048];
    var font1 = new byte[2048];
    font1[0] = 0xFF; // first row of char 0 = all pixels set
    var font = new BitmapFont(new[] { font0, font1 });
    Assert.AreEqual(0x00, font.GetRow(0, 0, 0)); // font 0, char 0, row 0
    Assert.AreEqual(0xFF, font.GetRow(1, 0, 0)); // font 1, char 0, row 0
}

[TestMethod]
public void MultiFontGetRow_InvalidSlot_FallsBackToSlot0()
{
    var font0 = new byte[2048];
    font0[0] = 0xAA;
    var font = new BitmapFont(new[] { font0 });
    Assert.AreEqual(0xAA, font.GetRow(5, 0, 0)); // slot 5 empty → falls back to 0
}

[TestMethod]
public void MultiFontGetPixel_UsesCorrectSlot()
{
    var font0 = new byte[2048];
    var font1 = new byte[2048];
    font1[0x41 * 8] = 0x80; // char 0x41, row 0, leftmost pixel set
    var font = new BitmapFont(new[] { font0, font1 });
    Assert.IsFalse(font.GetPixel(0, 0x41, 0, 0)); // font 0: not set
    Assert.IsTrue(font.GetPixel(1, 0x41, 0, 0));  // font 1: set
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~BitmapFontTests" --verbosity normal`
Expected: FAIL — `GetRow` and `GetPixel` don't accept font index parameters yet.

**Step 3: Implement multi-slot BitmapFont**

Replace `BitmapFont.cs` with:

```csharp
namespace e6502.Avalonia.Rendering;

public class BitmapFont
{
    public const int MaxSlots = 8;
    public const int GlyphWidth = 8;
    public const int GlyphHeight = 8;
    public const int GlyphCount = 256;
    public const int FontDataSize = GlyphCount * GlyphHeight; // 2048

    private readonly byte[][] _slots;

    /// <summary>Single-font constructor (backward compatible).</summary>
    public BitmapFont(byte[] fontData) : this(new[] { fontData }) { }

    /// <summary>Multi-font constructor. Slot 0 is the default/fallback.</summary>
    public BitmapFont(byte[][] fontSlots)
    {
        if (fontSlots.Length == 0)
            throw new ArgumentException("At least one font slot required");
        if (fontSlots.Length > MaxSlots)
            throw new ArgumentException($"Maximum {MaxSlots} font slots");
        foreach (var slot in fontSlots)
            if (slot.Length != FontDataSize)
                throw new ArgumentException($"Font data must be {FontDataSize} bytes");
        _slots = fontSlots;
    }

    public int SlotCount => _slots.Length;

    public static BitmapFont LoadFromFile(string path) =>
        new(File.ReadAllBytes(path));

    /// <summary>Returns true if pixel (x,y) is set in slot 0.</summary>
    public bool GetPixel(byte charCode, int x, int y) =>
        GetPixel(0, charCode, x, y);

    /// <summary>Returns true if pixel (x,y) is set in the given font slot.</summary>
    public bool GetPixel(int slot, byte charCode, int x, int y)
    {
        byte row = GetRow(slot, charCode, y);
        return (row & (0x80 >> x)) != 0;
    }

    /// <summary>Returns raw row byte from slot 0.</summary>
    public byte GetRow(byte charCode, int y) =>
        GetRow(0, charCode, y);

    /// <summary>Returns raw row byte from the given font slot (falls back to slot 0).</summary>
    public byte GetRow(int slot, byte charCode, int y)
    {
        if (slot < 0 || slot >= _slots.Length)
            slot = 0;
        return _slots[slot][charCode * GlyphHeight + y];
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~BitmapFontTests" --verbosity normal`
Expected: All tests PASS (old tests still work via backward-compatible overloads).

**Step 5: Commit**

```bash
git add e6502.Avalonia/Rendering/BitmapFont.cs e6502UnitTests/BitmapFontTests.cs
git commit -m "feat(font): multi-slot BitmapFont with fallback to slot 0"
```

---

### Task 2: VGC RegFont Register

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/VgcRegisterTests.cs` (or appropriate existing test file)

**Step 1: Write failing test for font register**

```csharp
[TestMethod]
public void RegFont_DefaultsToZero()
{
    // After VGC init, font register should be 0 (CP437)
    var vgc = CreateVgc(); // use whatever factory pattern tests use
    Assert.AreEqual(0, vgc.GetFontIndex());
}

[TestMethod]
public void RegFont_WriteThenRead_Masked()
{
    var vgc = CreateVgc();
    vgc.Write(VgcConstants.RegFont, 0xFF); // only low 3 bits should stick
    Assert.AreEqual(7, vgc.GetFontIndex());
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "RegFont" --verbosity normal`
Expected: FAIL — `RegFont` constant and `GetFontIndex()` don't exist.

**Step 3: Add RegFont constant and accessor**

In `VgcConstants.cs`, rename `RegBank` to `RegFont`:
```csharp
public const int RegFont            = 0xA007;   // active font slot (0-7)
```

In `VirtualGraphicsController.cs`, add accessor:
```csharp
public byte GetFontIndex() =>
    (byte)(_regs[VgcConstants.RegFont - VgcConstants.VgcBase] & 0x07);
```

If VGC Write handler masks or intercepts register writes, ensure `$A007` writes are masked to 3 bits. Check how other registers are handled and follow the same pattern.

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "RegFont" --verbosity normal`
Expected: PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/
git commit -m "feat(font): add RegFont VGC register at \$A007"
```

---

### Task 3: PETSCII Font Binaries

**Files:**
- Create: `e6502.Avalonia/Resources/petscii_upper.bin` (2048 bytes)
- Create: `e6502.Avalonia/Resources/petscii_lower.bin` (2048 bytes)
- Modify: `e6502.Avalonia/e6502.Avalonia.csproj` (ensure `Resources/*.bin` copies — already covered by existing wildcard)

**Step 1: Source PETSCII font data**

The C64 character ROM is 4096 bytes: first 2048 = uppercase/graphics (set 1), second 2048 = lowercase/uppercase (set 2). The raw ROM uses screen code order (not PETSCII code order).

**Important:** We need the font indexed by the byte values our system stores in character RAM (ASCII-ish). Two approaches:

**Option A (recommended):** Remap the C64 glyphs into ASCII-compatible positions:
- Positions `$20-$7E`: Map corresponding PETSCII printable glyphs (letters, digits, punctuation)
- Positions `$80-$BF`: Shifted graphic characters (64 glyphs — the "Shift+key" characters)
- Positions `$C0-$DF`: Commodore-key graphic characters (32 glyphs — the "C=+key" characters)
- Positions `$00-$1F`, `$E0-$FF`: Fill with additional graphic characters or leave as copies

This means normal text (`PRINT "HELLO"`) renders correctly because 'H' at `$48` shows a PETSCII H.

**Option B:** Use the raw C64 ROM order and remap at input/output time. More authentic but more invasive.

Write a Python script (`tools/make_petscii_font.py`) to create the remapped font binaries from a C64 character ROM source. This script reads the 4K C64 ROM, remaps glyphs to ASCII-compatible positions, and writes two 2048-byte output files.

**Step 2: Verify font files**

```csharp
[TestMethod]
public void PetsciiUpperFont_IsCorrectSize()
{
    var data = File.ReadAllBytes(Path.Combine(AppContext.BaseDirectory, "Resources", "petscii_upper.bin"));
    Assert.AreEqual(2048, data.Length);
}

[TestMethod]
public void PetsciiLowerFont_IsCorrectSize()
{
    var data = File.ReadAllBytes(Path.Combine(AppContext.BaseDirectory, "Resources", "petscii_lower.bin"));
    Assert.AreEqual(2048, data.Length);
}
```

**Step 3: Run tests**

Run: `dotnet test --filter "Petscii" --verbosity normal`
Expected: PASS

**Step 4: Commit**

```bash
git add e6502.Avalonia/Resources/petscii_upper.bin e6502.Avalonia/Resources/petscii_lower.bin tools/make_petscii_font.py e6502UnitTests/
git commit -m "feat(font): add PETSCII font binaries (upper/graphics + lower/upper)"
```

---

### Task 4: Load Fonts at Startup & Wire to Renderer

**Files:**
- Modify: `e6502.Avalonia/MainWindow.axaml.cs` (font loading)
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs` (renderer uses font index)

**Step 1: Write failing test for renderer font slot usage**

A rendering test may be hard to unit test without the full canvas. Instead, verify the integration path: the `TrySampleTextPixel` path uses `_font.GetRow(fontIndex, ch, glyphY)` instead of `_font.GetRow(ch, glyphY)`.

This can be verified by inspection after the change, or by a targeted test that creates a BitmapFont with two different slots and confirms different pixels render.

**Step 2: Update MainWindow to load all fonts into BitmapFont**

In `MainWindow.axaml.cs`, change font loading from:
```csharp
var fontPath = Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin");
var font = BitmapFont.LoadFromFile(fontPath);
```
To:
```csharp
var resDir = Path.Combine(AppContext.BaseDirectory, "Resources");
var fontSlots = new List<byte[]>();
fontSlots.Add(File.ReadAllBytes(Path.Combine(resDir, "cp437.bin")));

var petUpper = Path.Combine(resDir, "petscii_upper.bin");
if (File.Exists(petUpper))
    fontSlots.Add(File.ReadAllBytes(petUpper));

var petLower = Path.Combine(resDir, "petscii_lower.bin");
if (File.Exists(petLower))
    fontSlots.Add(File.ReadAllBytes(petLower));

var font = new BitmapFont(fontSlots.ToArray());
```

**Step 3: Update EmulatorCanvas to use font index from VGC**

In `EmulatorCanvas.cs`, change `RenderVideoState` to include font index:
```csharp
private struct RenderVideoState
{
    public byte Mode;
    public int ScrollX;
    public int ScrollY;
    public byte BgColor;
    public int FontIndex;

    public static RenderVideoState FromVgc(VirtualGraphicsController vgc) =>
        new()
        {
            Mode = vgc.GetMode(),
            ScrollX = vgc.GetScrollX(),
            ScrollY = vgc.GetScrollY(),
            BgColor = vgc.GetBgColor(),
            FontIndex = vgc.GetFontIndex()
        };

    // Also update Apply() to handle RegFont copper events
}
```

In `TrySampleTextPixel`, change:
```csharp
byte rowBits = _font.GetRow(ch, glyphY);
```
To:
```csharp
byte rowBits = _font.GetRow(state.FontIndex, ch, glyphY);
```

Pass `state` (or `state.FontIndex`) to `TrySampleTextPixel` — it already receives `state`.

**Step 4: Build and verify**

Run: `dotnet build && dotnet test --verbosity normal`
Expected: All tests PASS. Running the app should show CP437 by default, `POKE 40967,1` should switch to PETSCII.

**Step 5: Commit**

```bash
git add e6502.Avalonia/MainWindow.axaml.cs e6502.Avalonia/Rendering/EmulatorCanvas.cs
git commit -m "feat(font): wire font slots to renderer via RegFont register"
```

---

### Task 5: EhBASIC FONT Keyword

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Understand the token/keyword pattern**

Follow the exact pattern used by `MODE` (token `TK_MODE = $A8`):
- Token defined in token list
- Keyword "MODE" in the "M" character table (`TAB_ASCM`)
- Handler address in command dispatch table
- Handler: `JSR LAB_GTBY` (get byte arg), `STX $A000` (write to register), `RTS`

**Step 2: Add FONT token and keyword**

1. Define a new token. Find the last primary token value and add one:
   ```asm
   TK_FONT    = <next_available_token>
   ```

2. Add "ONT" to the "F" character table (`TAB_ASCF`), pointing to `TK_FONT`:
   ```asm
   LBB_FONT
         .byte "ONT",TK_FONT     ; FONT
   ```
   (The "F" is already consumed by the first-character scanner.)

3. Add handler address to command dispatch table at the correct position (matching token index):
   ```asm
   .word LAB_FONT-1        ; FONT            VGC font select
   ```

4. Implement handler:
   ```asm
   LAB_FONT
         JSR   LAB_GTBY          ; get font index (0-7) into X
         STX   VGC_FONT          ; write to $A007
         RTS
   ```

5. Define the VGC address constant near the other VGC labels:
   ```asm
   VGC_FONT    = $A007            ; font register
   ```

**Step 3: Build and test**

```bash
make -C ehbasic
dotnet build
dotnet run --project e6502.Avalonia
```

In the emulator, type: `FONT 1` — screen should switch to PETSCII. `FONT 0` should return to CP437.

**Step 4: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin
git commit -m "feat(font): add FONT keyword to EhBASIC"
```

---

### Task 6: Keyboard Mapping for PETSCII

**Files:**
- Create: `e6502.Avalonia/Input/FontKeymap.cs`
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs` (intercept Shift/Ctrl + letter)
- Test: `e6502UnitTests/FontKeymapTests.cs`

**Step 1: Write failing tests for keymap**

```csharp
[TestClass]
public class FontKeymapTests
{
    [TestMethod]
    public void PetsciiUpper_ShiftA_ReturnsGraphicChar()
    {
        var keymap = FontKeymap.PetsciiUpper;
        Assert.IsTrue(keymap.TryMap(KeyModifier.Shift, 'A', out byte code));
        Assert.AreEqual(0x80, code); // or whatever the mapped value is
    }

    [TestMethod]
    public void PetsciiUpper_CtrlA_ReturnsGraphicChar()
    {
        var keymap = FontKeymap.PetsciiUpper;
        Assert.IsTrue(keymap.TryMap(KeyModifier.Ctrl, 'A', out byte code));
        Assert.AreEqual(0xC0, code); // or whatever the mapped value is
    }

    [TestMethod]
    public void Cp437_NoKeymap_ReturnsFalse()
    {
        var keymap = FontKeymap.None;
        Assert.IsFalse(keymap.TryMap(KeyModifier.Shift, 'A', out _));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FontKeymap" --verbosity normal`
Expected: FAIL

**Step 3: Implement FontKeymap**

```csharp
namespace e6502.Avalonia.Input;

public enum KeyModifier { None, Shift, Ctrl }

public class FontKeymap
{
    public static readonly FontKeymap None = new();

    private readonly Dictionary<(KeyModifier mod, char key), byte>? _map;

    private FontKeymap() { _map = null; }

    private FontKeymap(Dictionary<(KeyModifier, char), byte> map) { _map = map; }

    public bool TryMap(KeyModifier mod, char key, out byte charCode)
    {
        charCode = 0;
        if (_map is null) return false;
        return _map.TryGetValue((mod, char.ToUpper(key)), out charCode);
    }

    /// <summary>PETSCII Upper/Graphics keymap (Shift = graphic set 1, Ctrl = graphic set 2).</summary>
    public static FontKeymap PetsciiUpper => BuildPetsciiUpperKeymap();

    /// <summary>PETSCII Lower/Upper keymap.</summary>
    public static FontKeymap PetsciiLower => BuildPetsciiLowerKeymap();

    private static FontKeymap BuildPetsciiUpperKeymap()
    {
        var map = new Dictionary<(KeyModifier, char), byte>();
        // Map Shift+A through Shift+Z to graphic characters at $80-$99
        for (int i = 0; i < 26; i++)
            map[(KeyModifier.Shift, (char)('A' + i))] = (byte)(0x80 + i);
        // Map Ctrl+A through Ctrl+Z to graphic characters at $C0-$D9
        for (int i = 0; i < 26; i++)
            map[(KeyModifier.Ctrl, (char)('A' + i))] = (byte)(0xC0 + i);
        return new FontKeymap(map);
    }

    private static FontKeymap BuildPetsciiLowerKeymap()
    {
        // Similar mapping for lowercase/uppercase set
        var map = new Dictionary<(KeyModifier, char), byte>();
        for (int i = 0; i < 26; i++)
            map[(KeyModifier.Shift, (char)('A' + i))] = (byte)(0x80 + i);
        for (int i = 0; i < 26; i++)
            map[(KeyModifier.Ctrl, (char)('A' + i))] = (byte)(0xC0 + i);
        return new FontKeymap(map);
    }
}
```

**Note:** The exact byte values (`$80`, `$C0`, etc.) must match the positions chosen in the font binary (Task 3). Adjust these values when the font mapping is finalized.

**Step 4: Wire keymap into EmulatorCanvas**

The `BitmapFont` needs to expose keymaps per slot. Add to `BitmapFont`:
```csharp
private readonly FontKeymap[] _keymaps;
public FontKeymap GetKeymap(int slot) =>
    slot >= 0 && slot < _keymaps.Length ? _keymaps[slot] : FontKeymap.None;
```

Keymaps are passed in at construction alongside font data (or associated after construction).

In `EmulatorCanvas.OnKeyDown`, before existing handling, add:
```csharp
int fontIndex = _vgc.GetFontIndex();
var keymap = _font.GetKeymap(fontIndex);
if (e.Key >= Key.A && e.Key <= Key.Z)
{
    KeyModifier mod = KeyModifier.None;
    if (e.KeyModifiers.HasFlag(KeyModifiers.Shift)) mod = KeyModifier.Shift;
    else if (e.KeyModifiers.HasFlag(KeyModifiers.Control)) mod = KeyModifier.Ctrl;

    if (mod != KeyModifier.None && keymap.TryMap(mod, (char)('A' + (e.Key - Key.A)), out byte ch))
    {
        _editor.QueueInput(ch);
        e.Handled = true;
        return;
    }
}
```

This intercepts Shift+letter and Ctrl+letter *only* when the active font has a keymap, before `OnTextInput` fires.

**Step 5: Run tests**

Run: `dotnet test --verbosity normal`
Expected: All PASS

**Step 6: Commit**

```bash
git add e6502.Avalonia/Input/FontKeymap.cs e6502.Avalonia/Rendering/BitmapFont.cs e6502.Avalonia/Rendering/EmulatorCanvas.cs e6502UnitTests/FontKeymapTests.cs
git commit -m "feat(font): PETSCII keyboard mapping for Shift+key and Ctrl+key"
```

---

### Task 7: Help Panel Character Map

**Files:**
- Create: `docs/help/reference/commands/font.md`
- Create: `docs/help/guides/petscii-characters.md` (character map content)

**Step 1: Create FONT command help topic**

```markdown
---
title: "FONT"
type: command
category: Display
keywords: [font, petscii, character set, charset]
syntax: "FONT n"
see_also: [graphics-and-display]
---

Switches the active character font for the entire screen.

## Syntax

```
FONT n
```

**n** — Font slot number (0-7):

| Slot | Font | Description |
|------|------|-------------|
| 0 | CP437 | DOS/IBM character set (default) |
| 1 | PETSCII Upper | C64 uppercase + graphic characters |
| 2 | PETSCII Lower | C64 lowercase + uppercase |

## Examples

```basic
10 FONT 1
20 PRINT "HELLO IN PETSCII"
30 FONT 0
```

## Notes

- Font affects display only — program data is unchanged
- Use Shift+letter for graphic characters (when PETSCII active)
- Use Ctrl+letter for additional graphic characters
- See "PETSCII Characters" guide for the full character map
```

**Step 2: Create PETSCII character map guide**

This will be a help topic that shows all PETSCII graphic characters and their key combinations. The exact content depends on the final font mapping from Task 3. Structure:

```markdown
---
title: "PETSCII Characters"
type: guide
category: Display
keywords: [petscii, characters, graphics, keyboard, c64, commodore]
---

# PETSCII Character Map

Use these key combinations when FONT 1 or FONT 2 is active.

## Shift + Key

| Key | Char | Key | Char | Key | Char |
|-----|------|-----|------|-----|------|
| Shift+A | [glyph] | Shift+J | [glyph] | Shift+S | [glyph] |
...
```

The `[glyph]` placeholders will be filled with actual character descriptions once the font binary is created. The help panel renders this as text, so we describe the glyphs rather than trying to display them inline.

**Step 3: Auto-show character map on font switch**

This is optional polish. The help panel could listen for font register changes and auto-navigate to the PETSCII Characters topic. This can be done by:
1. Having `HelpPanel` expose a `ShowPetsciiMap()` method
2. Calling it from wherever the VGC font register change is detected (e.g., in the main tick loop or via an event)

Alternatively, keep it simple: the user opens help and searches "PETSCII". The auto-show is a nice-to-have that can be added after the core feature works.

**Step 4: Commit**

```bash
git add docs/help/reference/commands/font.md docs/help/guides/petscii-characters.md
git commit -m "docs(font): add FONT command and PETSCII character map help topics"
```

---

### Task 8: MCP/TCP SetFont Command

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`
- Modify: `e6502.MCP/EmulatorTools.cs`

**Step 1: Add TCP command**

In `EmulatorTcpServer.cs`, add to the command switch:
```csharp
"set_font" => CmdSetFont(req),
```

Implement:
```csharp
private string CmdSetFont(JsonNode req)
{
    int index = req["index"]?.GetValue<int>() ?? 0;
    _bus.Vgc.Write(VgcConstants.RegFont, (byte)(index & 0x07));
    return Ok();
}
```

**Step 2: Add MCP tool**

In `EmulatorTools.cs`, add:
```csharp
[McpServerTool, Description("Set the active font slot (0=CP437, 1=PETSCII Upper, 2=PETSCII Lower)")]
public static async Task<string> SetFont(
    EmulatorClient client,
    [Description("Font slot index (0-7)")] int index)
{
    await client.SendAsync(new JsonObject
    {
        ["command"] = "set_font",
        ["index"] = index
    });
    return $"Font set to slot {index}";
}
```

**Step 3: Build and test**

Run: `dotnet build`
Test via MCP or direct TCP: `{"command": "set_font", "index": 1}`

**Step 4: Commit**

```bash
git add e6502.Avalonia/Ipc/EmulatorTcpServer.cs e6502.MCP/EmulatorTools.cs
git commit -m "feat(font): add set_font TCP command and MCP tool"
```

---

### Task 9: Update Documentation

**Files:**
- Modify: `docs/help/guides/memory-map.md` (update $A007 from "Reserved" to "RegFont")
- Modify: `docs/plans/2026-02-26-multi-font-design.md` (mark as implemented)

**Step 1: Update memory map docs**

Change the $A007 entry from:
```
| $A007 | RegBank | R/W | Reserved. |
```
To:
```
| $A007 | RegFont | R/W | Active font slot (0-7). 0=CP437, 1=PETSCII Upper, 2=PETSCII Lower. |
```

**Step 2: Final regression test**

Run: `dotnet test --verbosity normal`
Expected: All tests PASS.

**Step 3: Commit**

```bash
git add docs/
git commit -m "docs: update memory map for RegFont, mark multi-font design as implemented"
```

---

## Task Dependency Order

```
Task 1 (BitmapFont) ──┐
Task 2 (VGC register) ─┼── Task 4 (Wire to renderer) ── Task 5 (EhBASIC FONT) ── Task 9 (Docs)
Task 3 (Font binaries) ┘         │
                                  ├── Task 6 (Keyboard mapping)
                                  ├── Task 7 (Help panel)
                                  └── Task 8 (MCP/TCP)
```

Tasks 1, 2, 3 can run in parallel. Task 4 depends on all three. Tasks 5-8 depend on Task 4 and can run in parallel. Task 9 is last.
