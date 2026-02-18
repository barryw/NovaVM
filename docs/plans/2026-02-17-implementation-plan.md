# Virtual Hardware Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add virtual graphics/sound chips, C64-style screen editor, and custom BASIC commands to the e6502 emulator with a Terminal.Gui TUI.

**Architecture:** Memory-mapped virtual hardware chips (VGC at $A000-$B39F, VSC stub at $A100-$A1FF) accessed via a composite bus device that routes reads/writes by address. All screen I/O unified through VGC. EhBASIC extended with new tokens and 6502 handlers that marshal arguments to VGC registers. Terminal.Gui renders VGC state at 30fps.

**Tech Stack:** C# / .NET 8.0, Terminal.Gui v2, ca65/ld65 (cc65 toolchain), MSTest

**Reference:** See `docs/plans/2026-02-17-virtual-hardware-design.md` for full design spec.

---

## Task 1: Upgrade TUI Project to .NET 8 and Terminal.Gui v2

The TUI project targets net7.0 (EOL) and Terminal.Gui 1.x (legacy API). Upgrade both before building on top of them.

**Files:**
- Modify: `e6502.TUI/e6502.TUI.csproj`

**Step 1: Update csproj**

Change TargetFramework to `net8.0` and Terminal.Gui to latest v2:

```xml
<Project Sdk="Microsoft.NET.Sdk">
    <PropertyGroup>
        <OutputType>Exe</OutputType>
        <TargetFramework>net8.0</TargetFramework>
        <ImplicitUsings>enable</ImplicitUsings>
        <Nullable>enable</Nullable>
    </PropertyGroup>
    <ItemGroup>
      <PackageReference Include="Terminal.Gui" Version="2.*" />
    </ItemGroup>
    <ItemGroup>
      <ProjectReference Include="..\e6502\e6502.csproj" />
    </ItemGroup>
    <ItemGroup>
      <None Update="Resources\ehbasic.bin">
        <CopyToOutputDirectory>Always</CopyToOutputDirectory>
      </None>
    </ItemGroup>
</Project>
```

**Step 2: Verify it builds**

Run: `dotnet restore e6502.TUI && dotnet build e6502.TUI`
Expected: Build succeeds (existing TUI code may have warnings from API changes — that's fine, we're replacing it all)

**Step 3: Commit**

```bash
git add e6502.TUI/e6502.TUI.csproj
git commit -m "chore: upgrade TUI to .NET 8 and Terminal.Gui v2"
```

---

## Task 2: VGC Register Constants and Address Map

Create a shared constants class defining the entire memory map. This is referenced by every other component.

**Files:**
- Create: `e6502.TUI/Hardware/VgcConstants.cs`
- Test: `e6502UnitTests/VgcConstantsTests.cs`

**Step 1: Write the test**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;

namespace e6502UnitTests;

[TestClass]
public class VgcConstantsTests
{
    [TestMethod]
    public void ScreenRamDoesNotOverlapColorRam()
    {
        Assert.IsTrue(VgcConstants.ColorRamStart >= VgcConstants.ScreenRamStart + VgcConstants.ScreenSize);
    }

    [TestMethod]
    public void ColorRamDoesNotOverlapFreeSpace()
    {
        Assert.IsTrue(VgcConstants.ColorRamStart + VgcConstants.ScreenSize <= 0xB3A0);
    }

    [TestMethod]
    public void SpriteRegistersDoNotOverlapGfxRegisters()
    {
        Assert.IsTrue(VgcConstants.SpriteRegStart >= VgcConstants.GfxCmdReg + 9);
    }

    [TestMethod]
    public void SpriteShapeDataFits16Sprites()
    {
        Assert.AreEqual(16 * 32, VgcConstants.SpriteShapeEnd - VgcConstants.SpriteShapeStart);
    }
}
```

**Step 2: Run test to verify it fails**

Run: `dotnet test --filter "VgcConstantsTests" --verbosity normal`
Expected: FAIL — class not found

**Step 3: Write VgcConstants**

```csharp
namespace e6502.TUI.Hardware;

public static class VgcConstants
{
    // Screen dimensions
    public const int ScreenCols = 80;
    public const int ScreenRows = 25;
    public const int ScreenSize = ScreenCols * ScreenRows; // 2000

    // Block graphics dimensions (2x vertical resolution via half-blocks)
    public const int GfxWidth = 160;
    public const int GfxHeight = 50;

    // VGC register base
    public const ushort VgcBase = 0xA000;

    // Individual registers
    public const ushort RegMode     = 0xA000;
    public const ushort RegBgCol    = 0xA001;
    public const ushort RegFgCol    = 0xA002;
    public const ushort RegCursX    = 0xA003;
    public const ushort RegCursY    = 0xA004;
    public const ushort RegScrollX  = 0xA005;
    public const ushort RegScrollY  = 0xA006;
    public const ushort RegBank     = 0xA007;
    public const ushort RegStatus   = 0xA008;
    public const ushort RegSprEn    = 0xA009;
    public const ushort RegSprEnH   = 0xA00A;
    public const ushort RegCollSt   = 0xA00B;
    public const ushort RegCollBg   = 0xA00C;
    public const ushort RegBorder   = 0xA00D;
    public const ushort RegCharOut  = 0xA00E;
    public const ushort RegCharIn   = 0xA00F;

    // Sprite registers: 16 sprites × 6 bytes ($A010-$A06F)
    public const ushort SpriteRegStart = 0xA010;
    public const int SpriteRegSize = 6;
    public const int MaxSprites = 16;

    // Graphics command registers ($A070-$A078)
    public const ushort GfxCmdReg   = 0xA070;
    public const ushort GfxP0       = 0xA071;
    public const ushort GfxP0H      = 0xA072;
    public const ushort GfxP1       = 0xA073;
    public const ushort GfxP1H      = 0xA074;
    public const ushort GfxP2       = 0xA075;
    public const ushort GfxP3       = 0xA076;
    public const ushort GfxP4       = 0xA077;
    public const ushort GfxP5       = 0xA078;

    // Graphics command bytes
    public const byte GfxCmdPlot    = 0x01;
    public const byte GfxCmdUnplot  = 0x02;
    public const byte GfxCmdLine    = 0x03;
    public const byte GfxCmdCircle  = 0x04;
    public const byte GfxCmdRect    = 0x05;
    public const byte GfxCmdFill    = 0x06;
    public const byte GfxCmdGcls    = 0x07;
    public const byte GfxCmdGcolor  = 0x08;

    // VSC stub base
    public const ushort VscBase     = 0xA100;
    public const ushort VscEnd      = 0xA1FF;

    // Sprite shape data: 16 × 32 bytes ($A200-$A3FF)
    public const ushort SpriteShapeStart = 0xA200;
    public const ushort SpriteShapeEnd   = 0xA400;
    public const int SpriteShapeSize = 32; // 16x16: 2 bytes/row × 16 rows

    // Screen RAM ($A400-$ABCF)
    public const ushort ScreenRamStart = 0xA400;

    // Color RAM ($ABD0-$B39F)
    public const ushort ColorRamStart = 0xABD0;

    // EhBASIC ROM
    public const ushort RomStart = 0xC000;

    // Address range checks
    public const ushort VgcStart = VgcBase;
    public const ushort VgcEnd   = 0xB3A0; // exclusive

    // Color palette (C64-inspired)
    public const int PaletteSize = 16;
}
```

**Step 4: Add project reference from test project**

Add `<ProjectReference Include="..\e6502.TUI\e6502.TUI.csproj" />` to `e6502UnitTests/e6502UnitTests.csproj`. Also upgrade test project to `net8.0`.

**Step 5: Run tests**

Run: `dotnet test --filter "VgcConstantsTests" --verbosity normal`
Expected: All 4 PASS

**Step 6: Commit**

```bash
git add e6502.TUI/Hardware/VgcConstants.cs e6502UnitTests/VgcConstantsTests.cs e6502UnitTests/e6502UnitTests.csproj
git commit -m "feat: add VGC register constants and memory map"
```

---

## Task 3: VirtualGraphicsController — Core State

The VGC holds all video state: registers, screen RAM, color RAM, sprite shapes. This task implements the register bank and screen/color RAM with read/write access. No rendering yet.

**Files:**
- Create: `e6502.TUI/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/VgcTests.cs`

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;

namespace e6502UnitTests;

[TestClass]
public class VgcTests
{
    [TestMethod]
    public void InitialModeIsText()
    {
        var vgc = new VirtualGraphicsController();
        Assert.AreEqual(0, vgc.Read(VgcConstants.RegMode));
    }

    [TestMethod]
    public void InitialForegroundColorIsWhite()
    {
        var vgc = new VirtualGraphicsController();
        Assert.AreEqual(1, vgc.Read(VgcConstants.RegFgCol));
    }

    [TestMethod]
    public void WriteAndReadRegister()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegBgCol, 5);
        Assert.AreEqual(5, vgc.Read(VgcConstants.RegBgCol));
    }

    [TestMethod]
    public void ScreenRamReadWrite()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.ScreenRamStart, 65); // 'A'
        Assert.AreEqual(65, vgc.Read(VgcConstants.ScreenRamStart));
    }

    [TestMethod]
    public void ColorRamReadWrite()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.ColorRamStart, 7);
        Assert.AreEqual(7, vgc.Read(VgcConstants.ColorRamStart));
    }

    [TestMethod]
    public void ScreenRamInitializedToSpaces()
    {
        var vgc = new VirtualGraphicsController();
        Assert.AreEqual(0x20, vgc.Read(VgcConstants.ScreenRamStart));
        Assert.AreEqual(0x20, vgc.Read(VgcConstants.ScreenRamStart + 1999));
    }

    [TestMethod]
    public void SpriteShapeReadWrite()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.SpriteShapeStart, 0xFF);
        Assert.AreEqual(0xFF, vgc.Read(VgcConstants.SpriteShapeStart));
    }

    [TestMethod]
    public void CollisionRegisterClearsOnRead()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCollSt, 0x03); // simulate collision
        var val = vgc.Read(VgcConstants.RegCollSt);
        Assert.AreEqual(0x03, val);
        Assert.AreEqual(0x00, vgc.Read(VgcConstants.RegCollSt)); // cleared
    }
}
```

**Step 2: Run tests to verify failure**

Run: `dotnet test --filter "VgcTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement VirtualGraphicsController**

```csharp
namespace e6502.TUI.Hardware;

public class VirtualGraphicsController
{
    private readonly byte[] _registers = new byte[0x10]; // $A000-$A00F
    private readonly byte[] _spriteRegs = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteRegSize];
    private readonly byte[] _gfxRegs = new byte[9]; // $A070-$A078
    private readonly byte[] _spriteShapes = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteShapeSize];
    private readonly byte[] _screenRam = new byte[VgcConstants.ScreenSize];
    private readonly byte[] _colorRam = new byte[VgcConstants.ScreenSize];

    // Block graphics bitmap (in .NET memory, not 6502-addressable)
    private readonly byte[,] _gfxBitmap = new byte[VgcConstants.GfxWidth, VgcConstants.GfxHeight];
    private byte _gfxDrawColor;

    // Input buffer for CHARIN
    private byte _charIn;

    public VirtualGraphicsController()
    {
        // Initialize screen to spaces
        Array.Fill(_screenRam, (byte)0x20);
        // Default foreground = white (1)
        _registers[VgcConstants.RegFgCol - VgcConstants.VgcBase] = 1;
        // Default color RAM = white on black
        Array.Fill(_colorRam, (byte)1);
    }

    public bool OwnsAddress(ushort address)
    {
        return address >= VgcConstants.VgcStart && address < VgcConstants.VgcEnd;
    }

    public byte Read(ushort address)
    {
        // Registers $A000-$A00F
        if (address >= VgcConstants.VgcBase && address <= VgcConstants.RegCharIn)
        {
            int offset = address - VgcConstants.VgcBase;

            // CHARIN: return and clear
            if (address == VgcConstants.RegCharIn)
            {
                var val = _charIn;
                _charIn = 0;
                return val;
            }

            // Collision registers: read and clear
            if (address == VgcConstants.RegCollSt || address == VgcConstants.RegCollBg)
            {
                var val = _registers[offset];
                _registers[offset] = 0;
                return val;
            }

            return _registers[offset];
        }

        // Sprite registers $A010-$A06F
        if (address >= VgcConstants.SpriteRegStart &&
            address < VgcConstants.SpriteRegStart + _spriteRegs.Length)
        {
            return _spriteRegs[address - VgcConstants.SpriteRegStart];
        }

        // GFX command registers $A070-$A078
        if (address >= VgcConstants.GfxCmdReg &&
            address <= VgcConstants.GfxP5)
        {
            return _gfxRegs[address - VgcConstants.GfxCmdReg];
        }

        // Sprite shapes $A200-$A3FF
        if (address >= VgcConstants.SpriteShapeStart && address < VgcConstants.SpriteShapeEnd)
        {
            return _spriteShapes[address - VgcConstants.SpriteShapeStart];
        }

        // Screen RAM $A400-$ABCF
        if (address >= VgcConstants.ScreenRamStart &&
            address < VgcConstants.ScreenRamStart + VgcConstants.ScreenSize)
        {
            return _screenRam[address - VgcConstants.ScreenRamStart];
        }

        // Color RAM $ABD0-$B39F
        if (address >= VgcConstants.ColorRamStart &&
            address < VgcConstants.ColorRamStart + VgcConstants.ScreenSize)
        {
            return _colorRam[address - VgcConstants.ColorRamStart];
        }

        return 0;
    }

    public void Write(ushort address, byte data)
    {
        // Registers $A000-$A00F
        if (address >= VgcConstants.VgcBase && address <= VgcConstants.RegCharIn)
        {
            int offset = address - VgcConstants.VgcBase;

            if (address == VgcConstants.RegCharOut)
            {
                HandleCharOut(data);
                return;
            }

            if (address == VgcConstants.RegCharIn)
            {
                _charIn = data;
                return;
            }

            // Status is read-only
            if (address == VgcConstants.RegStatus)
                return;

            _registers[offset] = data;
            return;
        }

        // Sprite registers
        if (address >= VgcConstants.SpriteRegStart &&
            address < VgcConstants.SpriteRegStart + _spriteRegs.Length)
        {
            _spriteRegs[address - VgcConstants.SpriteRegStart] = data;
            return;
        }

        // GFX command registers
        if (address >= VgcConstants.GfxCmdReg && address <= VgcConstants.GfxP5)
        {
            _gfxRegs[address - VgcConstants.GfxCmdReg] = data;
            if (address == VgcConstants.GfxCmdReg)
                ExecuteGfxCommand(data);
            return;
        }

        // Sprite shapes
        if (address >= VgcConstants.SpriteShapeStart && address < VgcConstants.SpriteShapeEnd)
        {
            _spriteShapes[address - VgcConstants.SpriteShapeStart] = data;
            return;
        }

        // Screen RAM
        if (address >= VgcConstants.ScreenRamStart &&
            address < VgcConstants.ScreenRamStart + VgcConstants.ScreenSize)
        {
            _screenRam[address - VgcConstants.ScreenRamStart] = data;
            return;
        }

        // Color RAM
        if (address >= VgcConstants.ColorRamStart &&
            address < VgcConstants.ColorRamStart + VgcConstants.ScreenSize)
        {
            _colorRam[address - VgcConstants.ColorRamStart] = data;
            return;
        }
    }

    // Public accessors for renderer
    public byte GetScreenChar(int col, int row) => _screenRam[row * VgcConstants.ScreenCols + col];
    public byte GetScreenColor(int col, int row) => _colorRam[row * VgcConstants.ScreenCols + col];
    public byte GetMode() => _registers[VgcConstants.RegMode - VgcConstants.VgcBase];
    public byte GetBgColor() => _registers[VgcConstants.RegBgCol - VgcConstants.VgcBase];
    public byte GetBorderColor() => _registers[VgcConstants.RegBorder - VgcConstants.VgcBase];
    public byte GetCursorX() => _registers[VgcConstants.RegCursX - VgcConstants.VgcBase];
    public byte GetCursorY() => _registers[VgcConstants.RegCursY - VgcConstants.VgcBase];
    public byte GetGfxPixel(int x, int y) => _gfxBitmap[x, y];

    public ReadOnlySpan<byte> GetSpriteShape(int index) =>
        _spriteShapes.AsSpan(index * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

    public (int x, int y, byte color, bool enabled, byte shapeIdx, byte flags, byte priority) GetSpriteState(int index)
    {
        int baseAddr = index * VgcConstants.SpriteRegSize;
        int xLow = _spriteRegs[baseAddr];
        int xHigh = _spriteRegs[baseAddr + 1] & 0x01;
        int x = xLow | (xHigh << 8);
        int y = _spriteRegs[baseAddr + 2];
        byte color = _spriteRegs[baseAddr + 3];
        byte priority = _spriteRegs[baseAddr + 4];
        byte shapeIdx = _spriteRegs[baseAddr + 5];
        byte flags = _spriteRegs[baseAddr + 1]; // bits 6-7 are flip flags

        // Check enable bits
        bool enabled;
        if (index < 8)
            enabled = (_registers[VgcConstants.RegSprEn - VgcConstants.VgcBase] & (1 << index)) != 0;
        else
            enabled = (_registers[VgcConstants.RegSprEnH - VgcConstants.VgcBase] & (1 << (index - 8))) != 0;

        return (x, y, color, enabled, shapeIdx, flags, priority);
    }

    public void FeedInput(byte key) => _charIn = key;

    private void HandleCharOut(byte data)
    {
        // Implemented in Task 4
    }

    private void ExecuteGfxCommand(byte cmd)
    {
        // Implemented in Task 8
    }
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "VgcTests" --verbosity normal`
Expected: All 8 PASS

**Step 5: Commit**

```bash
git add e6502.TUI/Hardware/VirtualGraphicsController.cs e6502UnitTests/VgcTests.cs
git commit -m "feat: add VGC core state — registers, screen/color RAM, sprite data"
```

---

## Task 4: VGC Character Output (CHAROUT)

Implement the CHAROUT register ($A00E) — writing a byte here puts a character on screen, advances the cursor, handles control codes, and scrolls.

**Files:**
- Modify: `e6502.TUI/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/VgcCharOutTests.cs`

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;

namespace e6502UnitTests;

[TestClass]
public class VgcCharOutTests
{
    [TestMethod]
    public void PrintableCharWritesToScreenRam()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 65); // 'A'
        Assert.AreEqual(65, vgc.GetScreenChar(0, 0));
    }

    [TestMethod]
    public void CursorAdvancesAfterPrintable()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 65);
        Assert.AreEqual(1, vgc.GetCursorX());
        Assert.AreEqual(0, vgc.GetCursorY());
    }

    [TestMethod]
    public void ColorRamSetToCurrentFgColor()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegFgCol, 5); // green
        vgc.Write(VgcConstants.RegCharOut, 65);
        Assert.AreEqual(5, vgc.GetScreenColor(0, 0));
    }

    [TestMethod]
    public void CursorWrapsAtEndOfLine()
    {
        var vgc = new VirtualGraphicsController();
        for (int i = 0; i < 80; i++)
            vgc.Write(VgcConstants.RegCharOut, 65);
        Assert.AreEqual(0, vgc.GetCursorX());
        Assert.AreEqual(1, vgc.GetCursorY());
    }

    [TestMethod]
    public void CarriageReturnMovesCursorToNextLine()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 65); // 'A' at col 0
        vgc.Write(VgcConstants.RegCharOut, 0x0D); // CR
        Assert.AreEqual(0, vgc.GetCursorX());
        Assert.AreEqual(1, vgc.GetCursorY());
    }

    [TestMethod]
    public void BackspaceErasesPreviousChar()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 65); // 'A'
        vgc.Write(VgcConstants.RegCharOut, 0x08); // BS
        Assert.AreEqual(0, vgc.GetCursorX());
        Assert.AreEqual(0x20, vgc.GetScreenChar(0, 0)); // space
    }

    [TestMethod]
    public void FormFeedClearsScreenAndHomesCursor()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 65);
        vgc.Write(VgcConstants.RegCharOut, 0x0C); // FF
        Assert.AreEqual(0, vgc.GetCursorX());
        Assert.AreEqual(0, vgc.GetCursorY());
        Assert.AreEqual(0x20, vgc.GetScreenChar(0, 0));
    }

    [TestMethod]
    public void ScrollsWhenCursorPassesLastRow()
    {
        var vgc = new VirtualGraphicsController();
        // Put 'A' on first row
        vgc.Write(VgcConstants.RegCharOut, 65);
        // Move cursor to row 24
        vgc.Write(VgcConstants.RegCursX, 0);
        vgc.Write(VgcConstants.RegCursY, 24);
        // CR should trigger scroll
        vgc.Write(VgcConstants.RegCharOut, 0x0D);
        // Row 0 should now have what was row 1 (spaces)
        Assert.AreEqual(0x20, vgc.GetScreenChar(0, 0));
        // Cursor on row 24
        Assert.AreEqual(24, vgc.GetCursorY());
    }

    [TestMethod]
    public void LineFeedAdvancesRow()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCharOut, 0x0A); // LF
        Assert.AreEqual(1, vgc.GetCursorY());
    }

    [TestMethod]
    public void HomeMovesCursorToOrigin()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegCursX, 40);
        vgc.Write(VgcConstants.RegCursY, 12);
        vgc.Write(VgcConstants.RegCharOut, 0x13); // HOME
        Assert.AreEqual(0, vgc.GetCursorX());
        Assert.AreEqual(0, vgc.GetCursorY());
    }
}
```

**Step 2: Run to verify failure**

Run: `dotnet test --filter "VgcCharOutTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement HandleCharOut in VirtualGraphicsController**

Replace the empty `HandleCharOut` method:

```csharp
private void HandleCharOut(byte data)
{
    int cx = _registers[VgcConstants.RegCursX - VgcConstants.VgcBase];
    int cy = _registers[VgcConstants.RegCursY - VgcConstants.VgcBase];

    switch (data)
    {
        case 0x08: // Backspace
            if (cx > 0)
            {
                cx--;
                _screenRam[cy * VgcConstants.ScreenCols + cx] = 0x20;
                _colorRam[cy * VgcConstants.ScreenCols + cx] =
                    _registers[VgcConstants.RegFgCol - VgcConstants.VgcBase];
            }
            break;

        case 0x0A: // Line Feed
            cy++;
            if (cy >= VgcConstants.ScreenRows)
            {
                ScrollUp();
                cy = VgcConstants.ScreenRows - 1;
            }
            break;

        case 0x0C: // Form Feed — clear screen, home cursor
            Array.Fill(_screenRam, (byte)0x20);
            Array.Fill(_colorRam, _registers[VgcConstants.RegFgCol - VgcConstants.VgcBase]);
            cx = 0;
            cy = 0;
            break;

        case 0x0D: // Carriage Return
            cx = 0;
            cy++;
            if (cy >= VgcConstants.ScreenRows)
            {
                ScrollUp();
                cy = VgcConstants.ScreenRows - 1;
            }
            break;

        case 0x13: // HOME
            cx = 0;
            cy = 0;
            break;

        default: // Printable character
            if (data >= 0x20)
            {
                _screenRam[cy * VgcConstants.ScreenCols + cx] = data;
                _colorRam[cy * VgcConstants.ScreenCols + cx] =
                    _registers[VgcConstants.RegFgCol - VgcConstants.VgcBase];
                cx++;
                if (cx >= VgcConstants.ScreenCols)
                {
                    cx = 0;
                    cy++;
                    if (cy >= VgcConstants.ScreenRows)
                    {
                        ScrollUp();
                        cy = VgcConstants.ScreenRows - 1;
                    }
                }
            }
            break;
    }

    _registers[VgcConstants.RegCursX - VgcConstants.VgcBase] = (byte)cx;
    _registers[VgcConstants.RegCursY - VgcConstants.VgcBase] = (byte)cy;
}

private void ScrollUp()
{
    // Shift screen and color RAM up by one row
    Array.Copy(_screenRam, VgcConstants.ScreenCols, _screenRam, 0,
        VgcConstants.ScreenSize - VgcConstants.ScreenCols);
    Array.Copy(_colorRam, VgcConstants.ScreenCols, _colorRam, 0,
        VgcConstants.ScreenSize - VgcConstants.ScreenCols);

    // Clear last row
    int lastRowStart = (VgcConstants.ScreenRows - 1) * VgcConstants.ScreenCols;
    Array.Fill(_screenRam, (byte)0x20, lastRowStart, VgcConstants.ScreenCols);
    Array.Fill(_colorRam, _registers[VgcConstants.RegBgCol - VgcConstants.VgcBase],
        lastRowStart, VgcConstants.ScreenCols);
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "VgcCharOutTests" --verbosity normal`
Expected: All 10 PASS

**Step 5: Commit**

```bash
git add e6502.TUI/Hardware/VirtualGraphicsController.cs e6502UnitTests/VgcCharOutTests.cs
git commit -m "feat: implement CHAROUT — cursor, control codes, scrolling"
```

---

## Task 5: CompositeBusDevice

The bus device that routes reads/writes to the correct hardware component by address range.

**Files:**
- Create: `e6502.TUI/Hardware/CompositeBusDevice.cs`
- Create: `e6502.TUI/Hardware/VirtualSoundController.cs`
- Test: `e6502UnitTests/CompositeBusTests.cs`

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;

namespace e6502UnitTests;

[TestClass]
public class CompositeBusTests
{
    [TestMethod]
    public void RamReadWrite()
    {
        var bus = new CompositeBusDevice();
        bus.Write(0x0200, 42);
        Assert.AreEqual(42, bus.Read(0x0200));
    }

    [TestMethod]
    public void VgcRegisterAccessViabus()
    {
        var bus = new CompositeBusDevice();
        bus.Write(VgcConstants.RegBgCol, 3);
        Assert.AreEqual(3, bus.Read(VgcConstants.RegBgCol));
    }

    [TestMethod]
    public void CharOutWritesToVgcScreenRam()
    {
        var bus = new CompositeBusDevice();
        bus.Write(VgcConstants.RegCharOut, 65); // 'A'
        Assert.AreEqual(65, bus.Read(VgcConstants.ScreenRamStart)); // first char
    }

    [TestMethod]
    public void RomRegionLoadsEhBasic()
    {
        var bus = new CompositeBusDevice();
        // EhBASIC should be loaded at $C000 — first bytes should be non-zero
        byte first = bus.Read(0xC000);
        Assert.AreNotEqual(0, first);
    }

    [TestMethod]
    public void WritesToRomAreIgnored()
    {
        var bus = new CompositeBusDevice();
        byte original = bus.Read(0xC000);
        bus.Write(0xC000, 0xFF);
        Assert.AreEqual(original, bus.Read(0xC000));
    }

    [TestMethod]
    public void VscStubAcceptsWrites()
    {
        var bus = new CompositeBusDevice();
        bus.Write(VgcConstants.VscBase, 42); // should not throw
        Assert.AreEqual(42, bus.Read(VgcConstants.VscBase));
    }

    [TestMethod]
    public void CharInReadClearsValue()
    {
        var bus = new CompositeBusDevice();
        bus.Write(VgcConstants.RegCharIn, 65);
        Assert.AreEqual(65, bus.Read(VgcConstants.RegCharIn));
        Assert.AreEqual(0, bus.Read(VgcConstants.RegCharIn)); // cleared
    }
}
```

**Step 2: Run to verify failure**

Run: `dotnet test --filter "CompositeBusTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement VirtualSoundController (stub)**

```csharp
namespace e6502.TUI.Hardware;

public class VirtualSoundController
{
    private readonly byte[] _registers = new byte[VgcConstants.VscEnd - VgcConstants.VscBase + 1];

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.VscBase && address <= VgcConstants.VscEnd;

    public byte Read(ushort address) => _registers[address - VgcConstants.VscBase];

    public void Write(ushort address, byte data) => _registers[address - VgcConstants.VscBase] = data;
}
```

**Step 4: Implement CompositeBusDevice**

```csharp
using KDS.e6502;

namespace e6502.TUI.Hardware;

public class CompositeBusDevice : IBusDevice
{
    private readonly byte[] _ram = new byte[0x10000];
    private readonly VirtualGraphicsController _vgc;
    private readonly VirtualSoundController _vsc;

    public VirtualGraphicsController Vgc => _vgc;

    public CompositeBusDevice()
    {
        _vgc = new VirtualGraphicsController();
        _vsc = new VirtualSoundController();

        // Load EhBASIC ROM into RAM backing store at $C000
        var basicPath = Path.Combine(AppContext.BaseDirectory, "Resources", "ehbasic.bin");
        if (File.Exists(basicPath))
        {
            var rom = File.ReadAllBytes(basicPath);
            rom.CopyTo(_ram, VgcConstants.RomStart);
        }
    }

    public byte Read(ushort address)
    {
        if (_vgc.OwnsAddress(address))
            return _vgc.Read(address);

        if (_vsc.OwnsAddress(address))
            return _vsc.Read(address);

        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_vgc.OwnsAddress(address))
        {
            _vgc.Write(address, data);
            return;
        }

        if (_vsc.OwnsAddress(address))
        {
            _vsc.Write(address, data);
            return;
        }

        // ROM protection: writes above $C000 are ignored
        if (address >= VgcConstants.RomStart)
            return;

        _ram[address] = data;
    }
}
```

**Step 5: Copy ehbasic.bin to test output**

Add to `e6502UnitTests/e6502UnitTests.csproj`:

```xml
<None Update="..\e6502.TUI\Resources\ehbasic.bin">
    <Link>Resources\ehbasic.bin</Link>
    <CopyToOutputDirectory>Always</CopyToOutputDirectory>
</None>
```

**Step 6: Run tests**

Run: `dotnet test --filter "CompositeBusTests" --verbosity normal`
Expected: All 7 PASS

Also run existing CPU tests to ensure nothing broke:

Run: `dotnet test --filter "E6502Tests" --verbosity normal`
Expected: All 5 PASS

**Step 7: Commit**

```bash
git add e6502.TUI/Hardware/CompositeBusDevice.cs e6502.TUI/Hardware/VirtualSoundController.cs e6502UnitTests/CompositeBusTests.cs e6502UnitTests/e6502UnitTests.csproj
git commit -m "feat: add CompositeBusDevice with VGC/VSC routing and ROM loading"
```

---

## Task 6: Terminal Renderer and DisplayView

Build the Terminal.Gui view that renders VGC state to the terminal. This replaces the broken ConsoleView.

**Files:**
- Create: `e6502.TUI/Rendering/DisplayView.cs`
- Create: `e6502.TUI/Rendering/ColorPalette.cs`
- Modify: `e6502.TUI/Tui.cs` (replace entirely)
- Delete: `e6502.TUI/ConsoleView.cs`
- Delete: `e6502.TUI/TuiBasicBusDevice.cs`

**Step 1: Implement ColorPalette**

```csharp
using Terminal.Gui;

namespace e6502.TUI.Rendering;

public static class ColorPalette
{
    // C64-inspired 16-color palette mapped to Terminal.Gui colors
    private static readonly Color[] Colors =
    {
        new(0, 0, 0),          // 0  Black
        new(255, 255, 255),    // 1  White
        new(136, 0, 0),        // 2  Red
        new(170, 255, 238),    // 3  Cyan
        new(204, 68, 204),     // 4  Purple
        new(0, 204, 85),       // 5  Green
        new(0, 0, 170),        // 6  Blue
        new(238, 238, 119),    // 7  Yellow
        new(221, 136, 85),     // 8  Orange
        new(102, 68, 0),       // 9  Brown
        new(255, 119, 119),    // 10 Light Red
        new(51, 51, 51),       // 11 Grey Dark
        new(119, 119, 119),    // 12 Grey Med
        new(170, 255, 102),    // 13 Light Green
        new(0, 136, 255),      // 14 Light Blue
        new(187, 187, 187),    // 15 Grey Light
    };

    public static Color Get(int index) => Colors[index & 0x0F];
}
```

**Step 2: Implement DisplayView**

This is the main Terminal.Gui view. It reads VGC state and renders the text layer. Block graphics and sprites will be added in later tasks.

```csharp
using Terminal.Gui;
using e6502.TUI.Hardware;

namespace e6502.TUI.Rendering;

public class DisplayView : View
{
    private readonly VirtualGraphicsController _vgc;
    private bool _cursorVisible = true;
    private readonly object _lock = new();

    public DisplayView(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
        Width = VgcConstants.ScreenCols;
        Height = VgcConstants.ScreenRows;
        CanFocus = true;
    }

    public override void OnDrawContent(Rectangle viewport)
    {
        lock (_lock)
        {
            var bgColor = ColorPalette.Get(_vgc.GetBgColor());

            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte ch = _vgc.GetScreenChar(col, row);
                    byte colorIdx = _vgc.GetScreenColor(col, row);
                    var fg = ColorPalette.Get(colorIdx);

                    // Cursor: invert at cursor position
                    bool isCursor = _cursorVisible &&
                                    col == _vgc.GetCursorX() &&
                                    row == _vgc.GetCursorY();

                    var attr = isCursor
                        ? new Attribute(bgColor, fg)
                        : new Attribute(fg, bgColor);

                    Driver.SetAttribute(attr);
                    Move(col, row);
                    Driver.AddRune((Rune)ch);
                }
            }
        }
    }

    public void ToggleCursor() => _cursorVisible = !_cursorVisible;

    public void FeedKey(byte key)
    {
        lock (_lock)
        {
            _vgc.FeedInput(key);
        }
    }
}
```

**Step 3: Rewrite Tui.cs**

```csharp
using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using KDS.e6502;
using Terminal.Gui;

var app = Application.Init();

var bus = new CompositeBusDevice();
var cpu = new Cpu(bus);
cpu.Boot();

var display = new DisplayView(bus.Vgc)
{
    X = 0,
    Y = 1
};

var window = new Window
{
    Title = "e6502 Enhanced BASIC",
    Width = Dim.Fill(),
    Height = Dim.Fill()
};

var menu = new MenuBar
{
    Menus =
    [
        new MenuBarItem("_File",
        [
            new MenuItem("_Quit", "", () => Application.RequestStop())
        ])
    ]
};

window.Add(display);

// Keyboard handler
display.KeyDown += (s, e) =>
{
    byte key = e.KeyCode switch
    {
        KeyCode.Enter     => 0x0D,
        KeyCode.Backspace => 0x08,
        KeyCode.Escape    => 0x03, // CTRL-C
        _ => 0
    };

    if (key == 0 && e.AsRune != default)
    {
        var rune = e.AsRune;
        if (rune.Value is >= 0x20 and <= 0x7E)
            key = (byte)rune.Value;
    }

    if (key != 0)
    {
        display.FeedKey(key);
        e.Handled = true;
    }
};

// CPU thread
var cpuThread = new Thread(() =>
{
    while (Application.Top != null)
    {
        cpu.ExecuteNext();
    }
})
{
    IsBackground = true
};
cpuThread.Start();

// Refresh timer (30fps)
var timer = Application.AddTimeout(TimeSpan.FromMilliseconds(33), () =>
{
    display.SetNeedsDisplay();
    return true;
});

// Cursor blink timer (500ms)
var cursorTimer = Application.AddTimeout(TimeSpan.FromMilliseconds(500), () =>
{
    display.ToggleCursor();
    return true;
});

Application.Top.Add(menu, window);
display.SetFocus();
Application.Run();
Application.Shutdown();
```

**Step 4: Delete old files**

Delete `e6502.TUI/ConsoleView.cs` and `e6502.TUI/TuiBasicBusDevice.cs`.

**Step 5: Build and manually test**

Run: `dotnet build e6502.TUI`
Expected: Builds without errors.

Run: `dotnet run --project e6502.TUI`
Expected: Window appears with "6502 EhBASIC [C]old/[W]arm ?" prompt. Press C, see "Memory size?" prompt. Press Enter for default. See "Enhanced BASIC" banner and Ready prompt. Type `PRINT "HELLO"` and see output.

**Note:** Terminal.Gui v2 API may differ from what's written here. Adjust `OnDrawContent`, `Application.Init`, `Application.AddTimeout`, `KeyDown` event signatures, and `Attribute` constructor as needed to match the actual v2 API. The important thing is the architecture — the specifics of the Terminal.Gui API should be adapted at implementation time.

**Step 6: Commit**

```bash
git rm e6502.TUI/ConsoleView.cs e6502.TUI/TuiBasicBusDevice.cs
git add e6502.TUI/Rendering/DisplayView.cs e6502.TUI/Rendering/ColorPalette.cs e6502.TUI/Tui.cs
git commit -m "feat: Terminal.Gui display with VGC-backed rendering and keyboard input"
```

---

## Task 7: Screen Editor (C64-style)

Implement cursor movement via arrow keys and RETURN-reads-line-from-screen behavior.

**Files:**
- Create: `e6502.TUI/Rendering/ScreenEditor.cs`
- Test: `e6502UnitTests/ScreenEditorTests.cs`
- Modify: `e6502.TUI/Tui.cs` (wire up arrow keys)
- Modify: `e6502.TUI/Rendering/DisplayView.cs` (delegate key handling)

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;
using e6502.TUI.Rendering;

namespace e6502UnitTests;

[TestClass]
public class ScreenEditorTests
{
    [TestMethod]
    public void CursorRightMovesCursor()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);
        editor.CursorRight();
        Assert.AreEqual(1, vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorLeftMovesCursor()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);
        vgc.Write(VgcConstants.RegCursX, 5);
        editor.CursorLeft();
        Assert.AreEqual(4, vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorLeftStopsAtZero()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);
        editor.CursorLeft();
        Assert.AreEqual(0, vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorDownMovesCursor()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);
        editor.CursorDown();
        Assert.AreEqual(1, vgc.GetCursorY());
    }

    [TestMethod]
    public void CursorUpMovesCursor()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);
        vgc.Write(VgcConstants.RegCursY, 5);
        editor.CursorUp();
        Assert.AreEqual(4, vgc.GetCursorY());
    }

    [TestMethod]
    public void ReturnReadsLineFromScreenRam()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);

        // Write "HELLO" to row 0 of screen RAM
        vgc.Write(VgcConstants.ScreenRamStart + 0, (byte)'H');
        vgc.Write(VgcConstants.ScreenRamStart + 1, (byte)'E');
        vgc.Write(VgcConstants.ScreenRamStart + 2, (byte)'L');
        vgc.Write(VgcConstants.ScreenRamStart + 3, (byte)'L');
        vgc.Write(VgcConstants.ScreenRamStart + 4, (byte)'O');

        // Cursor is on row 0
        vgc.Write(VgcConstants.RegCursY, 0);

        // Press RETURN — should read "HELLO" + CR into input queue
        var line = editor.ReadLineFromScreen();
        Assert.AreEqual("HELLO", line);
    }

    [TestMethod]
    public void ReturnTrimsTrailingSpaces()
    {
        var vgc = new VirtualGraphicsController();
        var editor = new ScreenEditor(vgc);

        vgc.Write(VgcConstants.ScreenRamStart + 0, (byte)'H');
        vgc.Write(VgcConstants.ScreenRamStart + 1, (byte)'I');
        // Rest is spaces (default)

        vgc.Write(VgcConstants.RegCursY, 0);

        var line = editor.ReadLineFromScreen();
        Assert.AreEqual("HI", line);
    }
}
```

**Step 2: Run to verify failure**

Run: `dotnet test --filter "ScreenEditorTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement ScreenEditor**

```csharp
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

    public void CursorLeft()
    {
        int x = _vgc.GetCursorX();
        if (x > 0)
            _vgc.Write(VgcConstants.RegCursX, (byte)(x - 1));
    }

    public void CursorRight()
    {
        int x = _vgc.GetCursorX();
        if (x < VgcConstants.ScreenCols - 1)
            _vgc.Write(VgcConstants.RegCursX, (byte)(x + 1));
    }

    public void CursorUp()
    {
        int y = _vgc.GetCursorY();
        if (y > 0)
            _vgc.Write(VgcConstants.RegCursY, (byte)(y - 1));
    }

    public void CursorDown()
    {
        int y = _vgc.GetCursorY();
        if (y < VgcConstants.ScreenRows - 1)
            _vgc.Write(VgcConstants.RegCursY, (byte)(y + 1));
    }

    public string ReadLineFromScreen()
    {
        int row = _vgc.GetCursorY();
        var chars = new char[VgcConstants.ScreenCols];

        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            chars[col] = (char)_vgc.GetScreenChar(col, row);

        return new string(chars).TrimEnd();
    }

    /// <summary>
    /// Handle RETURN key: read current line from screen, queue it as input, move cursor.
    /// </summary>
    public void HandleReturn()
    {
        var line = ReadLineFromScreen();

        // Queue each character of the line + CR
        foreach (var ch in line)
            _inputQueue.Enqueue((byte)ch);
        _inputQueue.Enqueue(0x0D);

        // Move cursor to next line
        _vgc.Write(VgcConstants.RegCharOut, 0x0D);
    }

    /// <summary>
    /// Handle a typed character: write to screen RAM at cursor, advance cursor.
    /// </summary>
    public void HandleTypedChar(byte ch)
    {
        // Write directly to screen RAM at cursor position (not through CHAROUT,
        // so EhBASIC doesn't see it as output)
        int cx = _vgc.GetCursorX();
        int cy = _vgc.GetCursorY();
        int offset = cy * VgcConstants.ScreenCols + cx;
        _vgc.Write((ushort)(VgcConstants.ScreenRamStart + offset), ch);
        _vgc.Write((ushort)(VgcConstants.ColorRamStart + offset), _vgc.Read(VgcConstants.RegFgCol));

        // Advance cursor
        cx++;
        if (cx >= VgcConstants.ScreenCols)
        {
            cx = 0;
            int newY = cy + 1;
            if (newY >= VgcConstants.ScreenRows)
                newY = VgcConstants.ScreenRows - 1;
            _vgc.Write(VgcConstants.RegCursY, (byte)newY);
        }
        _vgc.Write(VgcConstants.RegCursX, (byte)cx);
    }

    /// <summary>
    /// Called by the VGC's CHARIN read path — if there's queued input from a
    /// RETURN read-back, return the next byte. Otherwise return 0.
    /// </summary>
    public byte DequeueInput()
    {
        return _inputQueue.Count > 0 ? _inputQueue.Dequeue() : (byte)0;
    }

    public bool HasQueuedInput => _inputQueue.Count > 0;
}
```

**Step 4: Wire ScreenEditor into VGC and DisplayView**

Modify `VirtualGraphicsController` to accept an optional `ScreenEditor` reference (or use an event/callback) so that CHARIN reads can pull from the editor's queue. Modify `DisplayView` key handler:

- Arrow keys → `editor.CursorLeft/Right/Up/Down()`
- RETURN → `editor.HandleReturn()` (reads screen, queues input)
- Printable chars → `editor.HandleTypedChar(key)` (writes to screen at cursor)
- Backspace → `editor.CursorLeft()` + erase char at cursor

The CHARIN register ($A00F) read path should check the editor's queue first, then fall back to direct key input.

**Step 5: Run tests**

Run: `dotnet test --filter "ScreenEditorTests" --verbosity normal`
Expected: All PASS

**Step 6: Manual test**

Run the TUI. Type `PRINT "HELLO"`. Cursor up to the output line. Edit it. Press RETURN. Verify it re-enters.

**Step 7: Commit**

```bash
git add e6502.TUI/Rendering/ScreenEditor.cs e6502UnitTests/ScreenEditorTests.cs e6502.TUI/Rendering/DisplayView.cs e6502.TUI/Tui.cs e6502.TUI/Hardware/VirtualGraphicsController.cs
git commit -m "feat: C64-style screen editor with cursor movement and RETURN read-back"
```

---

## Task 8: Block Graphics Engine

Implement the block graphics bitmap, drawing algorithms, and rendering via Unicode half-blocks.

**Files:**
- Create: `e6502.TUI/Rendering/BlockGraphics.cs`
- Test: `e6502UnitTests/BlockGraphicsTests.cs`
- Modify: `e6502.TUI/Hardware/VirtualGraphicsController.cs` (implement `ExecuteGfxCommand`)
- Modify: `e6502.TUI/Rendering/DisplayView.cs` (composite gfx layer)

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Rendering;

namespace e6502UnitTests;

[TestClass]
public class BlockGraphicsTests
{
    [TestMethod]
    public void PlotSetsPixel()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Plot(bmp, 10, 20, 1);
        Assert.AreEqual(1, bmp[10, 20]);
    }

    [TestMethod]
    public void PlotOutOfBoundsIsIgnored()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Plot(bmp, 200, 60, 1); // should not throw
    }

    [TestMethod]
    public void LineHorizontal()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Line(bmp, 0, 0, 9, 0, 1);
        for (int x = 0; x <= 9; x++)
            Assert.AreEqual(1, bmp[x, 0], $"Pixel at {x},0");
    }

    [TestMethod]
    public void LineVertical()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Line(bmp, 5, 0, 5, 9, 1);
        for (int y = 0; y <= 9; y++)
            Assert.AreEqual(1, bmp[5, y], $"Pixel at 5,{y}");
    }

    [TestMethod]
    public void CirclePlotsSomething()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Circle(bmp, 40, 25, 10, 1);
        // Verify at least some pixels set on the circle boundary
        bool anySet = false;
        for (int x = 0; x < 160; x++)
            for (int y = 0; y < 50; y++)
                if (bmp[x, y] != 0) { anySet = true; break; }
        Assert.IsTrue(anySet);
    }

    [TestMethod]
    public void RectDrawsFourSides()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Rect(bmp, 10, 10, 20, 20, 1);
        // Top edge
        Assert.AreEqual(1, bmp[15, 10]);
        // Bottom edge
        Assert.AreEqual(1, bmp[15, 20]);
        // Left edge
        Assert.AreEqual(1, bmp[10, 15]);
        // Right edge
        Assert.AreEqual(1, bmp[20, 15]);
        // Interior should be empty
        Assert.AreEqual(0, bmp[15, 15]);
    }

    [TestMethod]
    public void FillFillsInterior()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Fill(bmp, 10, 10, 20, 20, 1);
        Assert.AreEqual(1, bmp[15, 15]); // center
        Assert.AreEqual(1, bmp[10, 10]); // corner
        Assert.AreEqual(0, bmp[9, 9]);   // outside
    }

    [TestMethod]
    public void ClearZerosEntireBitmap()
    {
        var bmp = new byte[160, 50];
        BlockGraphics.Plot(bmp, 10, 10, 1);
        BlockGraphics.Clear(bmp);
        Assert.AreEqual(0, bmp[10, 10]);
    }
}
```

**Step 2: Run to verify failure**

Run: `dotnet test --filter "BlockGraphicsTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement BlockGraphics**

```csharp
namespace e6502.TUI.Rendering;

public static class BlockGraphics
{
    public static void Plot(byte[,] bmp, int x, int y, byte color)
    {
        if (x >= 0 && x < bmp.GetLength(0) && y >= 0 && y < bmp.GetLength(1))
            bmp[x, y] = color;
    }

    public static void Clear(byte[,] bmp) => Array.Clear(bmp);

    public static void Line(byte[,] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        // Bresenham's line algorithm
        int dx = Math.Abs(x1 - x0);
        int dy = -Math.Abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true)
        {
            Plot(bmp, x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    public static void Circle(byte[,] bmp, int cx, int cy, int r, byte color)
    {
        // Midpoint circle algorithm
        int x = r, y = 0;
        int d = 1 - r;

        while (x >= y)
        {
            Plot(bmp, cx + x, cy + y, color);
            Plot(bmp, cx - x, cy + y, color);
            Plot(bmp, cx + x, cy - y, color);
            Plot(bmp, cx - x, cy - y, color);
            Plot(bmp, cx + y, cy + x, color);
            Plot(bmp, cx - y, cy + x, color);
            Plot(bmp, cx + y, cy - x, color);
            Plot(bmp, cx - y, cy - x, color);

            y++;
            if (d <= 0)
            {
                d += 2 * y + 1;
            }
            else
            {
                x--;
                d += 2 * (y - x) + 1;
            }
        }
    }

    public static void Rect(byte[,] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        Line(bmp, x0, y0, x1, y0, color); // top
        Line(bmp, x0, y1, x1, y1, color); // bottom
        Line(bmp, x0, y0, x0, y1, color); // left
        Line(bmp, x1, y0, x1, y1, color); // right
    }

    public static void Fill(byte[,] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        int minX = Math.Max(0, Math.Min(x0, x1));
        int maxX = Math.Min(bmp.GetLength(0) - 1, Math.Max(x0, x1));
        int minY = Math.Max(0, Math.Min(y0, y1));
        int maxY = Math.Min(bmp.GetLength(1) - 1, Math.Max(y0, y1));

        for (int y = minY; y <= maxY; y++)
            for (int x = minX; x <= maxX; x++)
                bmp[x, y] = color;
    }
}
```

**Step 4: Implement ExecuteGfxCommand in VGC**

Replace the empty `ExecuteGfxCommand`:

```csharp
private void ExecuteGfxCommand(byte cmd)
{
    int p0 = _gfxRegs[1] | (_gfxRegs[2] << 8);
    int p1 = _gfxRegs[3] | (_gfxRegs[4] << 8);
    int p2 = _gfxRegs[5];
    int p3 = _gfxRegs[6];

    switch (cmd)
    {
        case VgcConstants.GfxCmdPlot:
            BlockGraphics.Plot(_gfxBitmap, p0, p1, _gfxDrawColor);
            break;
        case VgcConstants.GfxCmdUnplot:
            BlockGraphics.Plot(_gfxBitmap, p0, p1, 0);
            break;
        case VgcConstants.GfxCmdLine:
            BlockGraphics.Line(_gfxBitmap, p0, p1, p2, p3, _gfxDrawColor);
            break;
        case VgcConstants.GfxCmdCircle:
            BlockGraphics.Circle(_gfxBitmap, p0, p1, p2, _gfxDrawColor);
            break;
        case VgcConstants.GfxCmdRect:
            BlockGraphics.Rect(_gfxBitmap, p0, p1, p2, p3, _gfxDrawColor);
            break;
        case VgcConstants.GfxCmdFill:
            BlockGraphics.Fill(_gfxBitmap, p0, p1, p2, p3, _gfxDrawColor);
            break;
        case VgcConstants.GfxCmdGcls:
            BlockGraphics.Clear(_gfxBitmap);
            break;
        case VgcConstants.GfxCmdGcolor:
            _gfxDrawColor = (byte)(p0 & 0x0F);
            break;
    }
}
```

**Step 5: Add block graphics compositing to DisplayView.OnDrawContent**

After drawing the character layer, if mode is 1 or 2, composite the block graphics:

```csharp
// Inside OnDrawContent, after character rendering loop
if (_vgc.GetMode() >= 1)
{
    for (int row = 0; row < VgcConstants.ScreenRows; row++)
    {
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            int gfxX = col * 2; // 160/80 = 2 pixels per char column
            int gfxYTop = row * 2;
            int gfxYBot = row * 2 + 1;

            byte topPixel = _vgc.GetGfxPixel(gfxX, gfxYTop);
            // Sample both halves of the 2-wide pixel
            byte topPixel2 = _vgc.GetGfxPixel(gfxX + 1, gfxYTop);
            byte botPixel = _vgc.GetGfxPixel(gfxX, gfxYBot);
            byte botPixel2 = _vgc.GetGfxPixel(gfxX + 1, gfxYBot);

            // Use dominant pixel (simplification — take left pixel)
            bool top = topPixel != 0;
            bool bot = botPixel != 0;

            if (!top && !bot) continue; // transparent, show text layer

            Rune block;
            Color fg, bg;

            if (top && bot)
            {
                block = new Rune('█');
                fg = ColorPalette.Get(topPixel);
                bg = ColorPalette.Get(_vgc.GetBgColor());
            }
            else if (top)
            {
                block = new Rune('▀');
                fg = ColorPalette.Get(topPixel);
                bg = ColorPalette.Get(_vgc.GetBgColor());
            }
            else
            {
                block = new Rune('▄');
                fg = ColorPalette.Get(botPixel);
                bg = ColorPalette.Get(_vgc.GetBgColor());
            }

            Driver.SetAttribute(new Attribute(fg, bg));
            Move(col, row);
            Driver.AddRune(block);
        }
    }
}
```

**Step 6: Run tests**

Run: `dotnet test --filter "BlockGraphicsTests" --verbosity normal`
Expected: All 8 PASS

**Step 7: Commit**

```bash
git add e6502.TUI/Rendering/BlockGraphics.cs e6502UnitTests/BlockGraphicsTests.cs e6502.TUI/Hardware/VirtualGraphicsController.cs e6502.TUI/Rendering/DisplayView.cs
git commit -m "feat: block graphics engine with Bresenham line, midpoint circle, rect, fill"
```

---

## Task 9: Sprite Rendering and Collision Detection

**Files:**
- Create: `e6502.TUI/Rendering/SpriteRenderer.cs`
- Test: `e6502UnitTests/SpriteRendererTests.cs`
- Modify: `e6502.TUI/Rendering/DisplayView.cs` (composite sprites)

**Step 1: Write failing tests**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;
using e6502.TUI.Rendering;

namespace e6502UnitTests;

[TestClass]
public class SpriteRendererTests
{
    [TestMethod]
    public void DisabledSpriteProducesNoPixels()
    {
        var vgc = new VirtualGraphicsController();
        // Sprite 0 not enabled, has shape data
        vgc.Write(VgcConstants.SpriteShapeStart, 0xFF);
        var pixels = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.AreEqual(0, pixels.Count);
    }

    [TestMethod]
    public void EnabledSpriteProducesPixels()
    {
        var vgc = new VirtualGraphicsController();
        // Enable sprite 0
        vgc.Write(VgcConstants.RegSprEn, 0x01);
        // Set shape: first row = $FF $FF (all 16 bits set)
        vgc.Write(VgcConstants.SpriteShapeStart, 0xFF);
        vgc.Write((ushort)(VgcConstants.SpriteShapeStart + 1), 0xFF);
        // Position at 0,0, color 1
        vgc.Write(VgcConstants.SpriteRegStart + 3, 1);
        vgc.Write(VgcConstants.SpriteRegStart + 5, 0); // shape 0

        var pixels = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.IsTrue(pixels.Count > 0);
    }

    [TestMethod]
    public void CollisionDetectedBetweenOverlappingSprites()
    {
        var vgc = new VirtualGraphicsController();
        // Enable sprites 0 and 1
        vgc.Write(VgcConstants.RegSprEn, 0x03);

        // Both use shape 0: first row all set
        vgc.Write(VgcConstants.SpriteShapeStart, 0xFF);
        vgc.Write((ushort)(VgcConstants.SpriteShapeStart + 1), 0xFF);

        // Sprite 0 at (0, 0)
        vgc.Write(VgcConstants.SpriteRegStart + 3, 1);
        vgc.Write(VgcConstants.SpriteRegStart + 5, 0);

        // Sprite 1 at (0, 0) — same position
        vgc.Write((ushort)(VgcConstants.SpriteRegStart + 6 + 3), 2);
        vgc.Write((ushort)(VgcConstants.SpriteRegStart + 6 + 5), 0);

        var collisions = SpriteRenderer.DetectCollisions(vgc);
        Assert.IsTrue(collisions.spriteToSprite != 0);
    }
}
```

**Step 2: Run to verify failure**

Run: `dotnet test --filter "SpriteRendererTests" --verbosity normal`
Expected: FAIL

**Step 3: Implement SpriteRenderer**

```csharp
using e6502.TUI.Hardware;

namespace e6502.TUI.Rendering;

public static class SpriteRenderer
{
    public struct SpritePixel
    {
        public int ScreenX;
        public int ScreenY;
        public byte Color;
    }

    /// <summary>
    /// Get all visible pixels for a sprite in block-pixel coordinates.
    /// </summary>
    public static List<SpritePixel> GetSpritePixels(VirtualGraphicsController vgc, int index)
    {
        var result = new List<SpritePixel>();
        var state = vgc.GetSpriteState(index);
        if (!state.enabled) return result;

        var shape = vgc.GetSpriteShape(state.shapeIdx);
        bool xFlip = (state.flags & 0x80) != 0;
        bool yFlip = (state.flags & 0x40) != 0;

        for (int row = 0; row < 16; row++)
        {
            int srcRow = yFlip ? (15 - row) : row;
            byte hi = shape[srcRow * 2];
            byte lo = shape[srcRow * 2 + 1];
            ushort bits = (ushort)((hi << 8) | lo);

            for (int col = 0; col < 16; col++)
            {
                int srcCol = xFlip ? (15 - col) : col;
                if ((bits & (0x8000 >> srcCol)) != 0)
                {
                    result.Add(new SpritePixel
                    {
                        ScreenX = state.x + col,
                        ScreenY = state.y + row,
                        Color = state.color
                    });
                }
            }
        }

        return result;
    }

    /// <summary>
    /// Detect sprite-sprite and sprite-background collisions.
    /// Returns bitmasks for each collision type.
    /// </summary>
    public static (ushort spriteToSprite, ushort spriteToBg) DetectCollisions(
        VirtualGraphicsController vgc)
    {
        ushort ss = 0, sb = 0;

        // Collect all sprite pixel sets
        var allPixels = new List<(int index, HashSet<(int x, int y)> pixels)>();

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var pixels = GetSpritePixels(vgc, i);
            if (pixels.Count == 0) continue;

            var set = new HashSet<(int x, int y)>();
            foreach (var p in pixels)
            {
                set.Add((p.ScreenX, p.ScreenY));

                // Check background collision (block graphics pixel)
                if (p.ScreenX >= 0 && p.ScreenX < VgcConstants.GfxWidth &&
                    p.ScreenY >= 0 && p.ScreenY < VgcConstants.GfxHeight &&
                    vgc.GetGfxPixel(p.ScreenX, p.ScreenY) != 0)
                {
                    sb |= (ushort)(1 << i);
                }
            }

            allPixels.Add((i, set));
        }

        // Sprite-sprite collision: check pair overlaps
        for (int a = 0; a < allPixels.Count; a++)
        {
            for (int b = a + 1; b < allPixels.Count; b++)
            {
                if (allPixels[a].pixels.Overlaps(allPixels[b].pixels))
                {
                    ss |= (ushort)(1 << allPixels[a].index);
                    ss |= (ushort)(1 << allPixels[b].index);
                }
            }
        }

        return (ss, sb);
    }
}
```

**Step 4: Wire collision detection into the renderer**

In the 30fps render loop (DisplayView or the timer callback), after compositing sprites call `SpriteRenderer.DetectCollisions()` and write results to the VGC collision registers.

**Step 5: Add sprite compositing to DisplayView.OnDrawContent**

After block graphics layer, composite sprite pixels. Convert sprite block-pixel coordinates to terminal cell coordinates using the same half-block approach as graphics.

**Step 6: Run tests**

Run: `dotnet test --filter "SpriteRendererTests" --verbosity normal`
Expected: All 3 PASS

**Step 7: Commit**

```bash
git add e6502.TUI/Rendering/SpriteRenderer.cs e6502UnitTests/SpriteRendererTests.cs e6502.TUI/Rendering/DisplayView.cs
git commit -m "feat: sprite rendering with 16x16 shapes, flip, and collision detection"
```

---

## Task 10: EhBASIC Assembly — New Tokens and Keyword Tables

Modify `basic.asm` to add new command/function tokens and keyword table entries. No 6502 handlers yet — just the tokenizer changes.

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add token constants**

After `TK_NMI = TK_IRQ+1` (line 363), before the secondary token section (line 367), add:

```asm
; --- VGC Graphics Commands ---
TK_CLS            = TK_NMI+1         ; CLS token
TK_COLOR          = TK_CLS+1         ; COLOR token
TK_LOCATE         = TK_COLOR+1       ; LOCATE token
TK_PLOT           = TK_LOCATE+1      ; PLOT token
TK_UNPLOT         = TK_PLOT+1        ; UNPLOT token
TK_LINE           = TK_UNPLOT+1      ; LINE token
TK_CIRCLE         = TK_LINE+1        ; CIRCLE token
TK_RECT           = TK_CIRCLE+1      ; RECT token
TK_FILLRECT       = TK_RECT+1        ; FILL token
TK_GMODE          = TK_FILLRECT+1    ; MODE token
TK_SPRCMD         = TK_GMODE+1       ; SPRITE token
TK_SPRSHAPE       = TK_SPRCMD+1      ; SPRITESHAPE token
TK_SPRCOLOR       = TK_SPRSHAPE+1    ; SPRITECOLOR token
TK_SPRDATA        = TK_SPRCOLOR+1    ; SPRITEDATA token
TK_SOUND          = TK_SPRDATA+1     ; SOUND token (stub)
TK_VOLUME         = TK_SOUND+1       ; VOLUME token (stub)
TK_ENVELOPE       = TK_VOLUME+1      ; ENVELOPE token (stub)
TK_WAVE           = TK_ENVELOPE+1    ; WAVE token (stub)
```

Then update the secondary token chain:

```asm
TK_TAB            = TK_WAVE+1        ; TAB token (was TK_NMI+1)
```

**Step 2: Add dispatch table entries**

In `LAB_CTBL` (line 8118), after `.word LAB_NMI-1` (line 8161), add:

```asm
      .word LAB_CLS-1          ; CLS
      .word LAB_COLOR-1        ; COLOR
      .word LAB_LOCATE-1       ; LOCATE
      .word LAB_PLOT-1         ; PLOT
      .word LAB_UNPLOT-1       ; UNPLOT
      .word LAB_LINE-1         ; LINE
      .word LAB_CIRCLE-1       ; CIRCLE
      .word LAB_RECT-1         ; RECT
      .word LAB_FILLRECT-1     ; FILL
      .word LAB_GMODE-1        ; MODE
      .word LAB_SPRCMD-1       ; SPRITE
      .word LAB_SPRSHAPE-1     ; SPRITESHAPE
      .word LAB_SPRCOLOR-1     ; SPRITECOLOR
      .word LAB_SPRDATA-1      ; SPRITEDATA
      .word LAB_SOUND-1        ; SOUND
      .word LAB_VOLUME-1       ; VOLUME
      .word LAB_ENVELOPE-1     ; ENVELOPE
      .word LAB_WAVE-1         ; WAVE
```

**Step 3: Add keyword table entries**

Add to the appropriate first-character tables. Examples:

In `TAB_ASCC` (line 8392), add before the `$00` terminator. Note: longer keywords before shorter:

```asm
LBB_CIRCLE
      .byte "IRCLE",TK_CIRCLE  ; CIRCLE
LBB_CLS
      .byte "LS",TK_CLS        ; CLS
LBB_COLOR
      .byte "OLOR",TK_COLOR    ; COLOR
```

In `TAB_ASCL`, add:

```asm
LBB_LINE
      .byte "INE",TK_LINE      ; LINE  (must come before "LIST")
LBB_LOCATE
      .byte "OCATE",TK_LOCATE  ; LOCATE (must come before "LOOP","LOAD","LOG(")
```

In `TAB_ASCM`, add:

```asm
LBB_MODE
      .byte "ODE",TK_GMODE     ; MODE (must come before "MID$(")
```

In `TAB_ASCR`, add:

```asm
LBB_RECT
      .byte "ECT",TK_RECT      ; RECT (must come before "READ","REM","RESTORE",...)
```

In `TAB_ASCS`, add:

```asm
LBB_SOUND
      .byte "OUND",TK_SOUND    ; SOUND (before "SWAP")
LBB_SPRITECOLOR
      .byte "PRITECOLOR",TK_SPRCOLOR  ; SPRITECOLOR (before SPRITEDATA, SPRITESHAPE, SPRITE)
LBB_SPRITEDATA
      .byte "PRITEDATA",TK_SPRDATA    ; SPRITEDATA
LBB_SPRITESHAPE
      .byte "PRITESHAPE",TK_SPRSHAPE  ; SPRITESHAPE
LBB_SPRITE
      .byte "PRITE",TK_SPRCMD        ; SPRITE (shortest last)
```

In `TAB_ASCP`, add:

```asm
LBB_PLOT
      .byte "LOT",TK_PLOT      ; PLOT (before "POKE","POS(","PRINT")
```

In `TAB_ASCF`, add:

```asm
LBB_FILL
      .byte "ILL",TK_FILLRECT  ; FILL (before "FN","FOR","FRE(")
```

In `TAB_ASCU`, add:

```asm
LBB_UNPLOT
      .byte "NPLOT",TK_UNPLOT  ; UNPLOT (before "NTIL","SR(")
```

In `TAB_ASCV`, add:

```asm
LBB_VOLUME
      .byte "OLUME",TK_VOLUME  ; VOLUME (before "AL(","ARPTR(")
```

In `TAB_ASCE`, add:

```asm
LBB_ENVELOPE
      .byte "NVELOPE",TK_ENVELOPE  ; ENVELOPE (before "LSE","ND","OR","XP(")
```

In `TAB_ASCW`, add:

```asm
LBB_WAVE
      .byte "AVE",TK_WAVE      ; WAVE (before "HILE","IDTH")
```

**CRITICAL:** In each table, ensure longer keywords with shared prefixes come FIRST. E.g. "SPRITECOLOR" before "SPRITEDATA" before "SPRITESHAPE" before "SPRITE". Check every table carefully.

**Step 4: Add new function tokens (for SPRITEX, SPRITEY, COLLISION, BUMPED)**

After `TK_MIDS` (line 431), add:

```asm
TK_SPRITEX        = TK_MIDS+1        ; SPRITEX( token
TK_SPRITEY        = TK_SPRITEX+1     ; SPRITEY( token
TK_COLLISION      = TK_SPRITEY+1     ; COLLISION( token
TK_BUMPED         = TK_COLLISION+1    ; BUMPED( token
```

Add to function pre-process table (`LAB_FTPL`) and function table (`LAB_FTBL`):

```asm
; In LAB_FTPL, after MID$() entry:
      .word LAB_PPFN-1        ; SPRITEX(n)
      .word LAB_PPFN-1        ; SPRITEY(n)
      .word LAB_PPFN-1        ; COLLISION(n)
      .word LAB_PPFN-1        ; BUMPED(n)

; In LAB_FTBL, after MID$() entry:
      .word LAB_SPRITEX-1     ; SPRITEX()
      .word LAB_SPRITEY-1     ; SPRITEY()
      .word LAB_COLLISION-1   ; COLLISION()
      .word LAB_BUMPED-1      ; BUMPED()
```

Add keyword table entries for functions:

In `TAB_ASCS`:
```asm
LBB_SPRITEX
      .byte "PRITEX(",TK_SPRITEX      ; SPRITEX( (before SPRITEY)
LBB_SPRITEY
      .byte "PRITEY(",TK_SPRITEY      ; SPRITEY(
```

In `TAB_ASCC`:
```asm
LBB_COLLISION
      .byte "OLLISION(",TK_COLLISION  ; COLLISION( (before "ONT")
```

In `TAB_ASCB`:
```asm
LBB_BUMPED
      .byte "UMPED(",TK_BUMPED        ; BUMPED( (before all other B entries? Check ordering)
```

**Step 5: Add stub handlers (so it assembles)**

At the end of basic.asm, before the keyword tables, add stub routines that just RTS:

```asm
; --- VGC command stubs (implemented in Task 11) ---
LAB_CLS
LAB_COLOR
LAB_LOCATE
LAB_PLOT
LAB_UNPLOT
LAB_LINE
LAB_CIRCLE
LAB_RECT
LAB_FILLRECT
LAB_GMODE
LAB_SPRCMD
LAB_SPRSHAPE
LAB_SPRCOLOR
LAB_SPRDATA
LAB_SOUND
LAB_VOLUME
LAB_ENVELOPE
LAB_WAVE
      RTS

; --- VGC function stubs (implemented in Task 11) ---
LAB_SPRITEX
LAB_SPRITEY
LAB_COLLISION
LAB_BUMPED
      LDA   #$00
      TAY
      JMP   LAB_AYFC           ; return 0
```

**Step 6: Build**

Run: `cd ehbasic && make clean && make`
Expected: Assembles without errors. If it fails, fix ordering issues in keyword tables.

**Step 7: Update min_mon.asm I/O ports**

Change the ACIA addresses to point to VGC CHAROUT/CHARIN:

```asm
ACIAsimwr   = $A00E     ; was IO_AREA+$01 ($F001)
ACIAsimrd   = $A00F     ; was IO_AREA+$04 ($F004)
```

**Step 8: Rebuild and test**

Run: `cd ehbasic && make clean && make`
Expected: Assembles without errors.

Copy the new binary to TUI resources:

Run: `cp ehbasic/basic.bin e6502.TUI/Resources/ehbasic.bin`

Run: `dotnet run --project e6502.TUI`
Expected: EhBASIC boots. Typing `CLS` should not crash (does nothing yet — stub returns). Typing `PRINT "HELLO"` still works.

**Step 9: Commit**

```bash
git add ehbasic/basic.asm ehbasic/min_mon.asm e6502.TUI/Resources/ehbasic.bin
git commit -m "feat: add VGC tokens, keyword tables, and stub handlers to EhBASIC"
```

---

## Task 11: EhBASIC 6502 Command Handlers

Implement the actual 6502 assembly handlers for each new BASIC command. Each handler evaluates arguments and writes to VGC I/O registers.

**Files:**
- Modify: `ehbasic/basic.asm` (replace stubs with real handlers)

**Step 1: Implement CLS**

```asm
LAB_CLS
      LDA   #$0C               ; form feed
      STA   $A00E              ; write to CHAROUT — clears screen
      RTS
```

**Step 2: Implement COLOR fg [,bg]**

```asm
LAB_COLOR
      JSR   LAB_GTBY           ; get byte (fg color)
      TXA
      STA   $A002              ; write to FGCOL register
      JSR   LAB_GBYT           ; get current byte
      CMP   #','               ; comma?
      BNE   @done              ; no, done
      JSR   LAB_IGBY           ; skip comma
      JSR   LAB_GTBY           ; get byte (bg color)
      TXA
      STA   $A001              ; write to BGCOL register
@done
      RTS
```

**Step 3: Implement LOCATE x, y**

```asm
LAB_LOCATE
      JSR   LAB_GTBY           ; get byte (x)
      TXA
      STA   $A003              ; CURSX
      JSR   LAB_1C01           ; scan for comma
      JSR   LAB_GTBY           ; get byte (y)
      TXA
      STA   $A004              ; CURSY
      RTS
```

**Step 4: Implement PLOT x, y**

```asm
LAB_PLOT
      JSR   LAB_EVNM           ; evaluate x
      JSR   LAB_F2FX           ; to integer in Itempl/h
      LDA   Itempl
      STA   $A071              ; GFX P0 low
      LDA   Itemph
      STA   $A072              ; GFX P0 high
      JSR   LAB_1C01           ; comma
      JSR   LAB_EVNM           ; evaluate y
      JSR   LAB_F2FX
      LDA   Itempl
      STA   $A073              ; GFX P1 low
      LDA   Itemph
      STA   $A074              ; GFX P1 high
      LDA   #$01               ; PLOT command
      STA   $A070              ; trigger
      RTS
```

**Step 5: Implement remaining graphics commands**

Follow the same pattern for UNPLOT (cmd $02), LINE (cmd $03), CIRCLE (cmd $04), RECT (cmd $05), FILL (cmd $06). Each evaluates its arguments, stores them in $A071-$A078, then writes the command byte to $A070.

LINE example:

```asm
LAB_LINE
      JSR   LAB_EVNM           ; x1
      JSR   LAB_F2FX
      LDA   Itempl
      STA   $A071
      LDA   Itemph
      STA   $A072
      JSR   LAB_1C01           ; comma
      JSR   LAB_EVNM           ; y1
      JSR   LAB_F2FX
      LDA   Itempl
      STA   $A073
      LDA   Itemph
      STA   $A074
      JSR   LAB_1C01           ; comma
      JSR   LAB_GTBY           ; x2 (byte range, 0-159)
      TXA
      STA   $A075
      JSR   LAB_1C01           ; comma
      JSR   LAB_GTBY           ; y2 (byte range, 0-49)
      TXA
      STA   $A076
      LDA   #$03               ; LINE command
      STA   $A070
      RTS
```

**Step 6: Implement MODE**

```asm
LAB_GMODE
      JSR   LAB_GTBY           ; get mode byte
      TXA
      STA   $A000              ; MODE register
      RTS
```

**Step 7: Implement SPRITE commands**

SPRITE n, ON/OFF and SPRITE n, x, y need to distinguish between two forms. Use the token after the sprite number to decide:

```asm
LAB_SPRCMD
      JSR   LAB_GTBY           ; sprite number (0-15)
      TXA
      PHA                     ; save sprite number
      JSR   LAB_1C01           ; comma
      JSR   LAB_GBYT           ; peek at next byte
      CMP   #TK_ON             ; ON token?
      BEQ   @enable
      CMP   #TK_OFF            ; OFF token?
      BEQ   @disable
      ; Otherwise it's x,y position
      PLA
      PHA
      ; Calculate sprite register base: $A010 + n*6
      ASL                     ; *2
      STA   @temp
      ASL                     ; *4
      CLC
      ADC   @temp              ; *6
      TAX
      JSR   LAB_EVNM           ; x position
      JSR   LAB_F2FX
      LDA   Itempl
      STA   $A010,X            ; sprite X low
      LDA   Itemph
      AND   #$01
      STA   $A011,X            ; sprite X high
      JSR   LAB_1C01           ; comma
      JSR   LAB_GTBY           ; y position
      TXA
      PLA                     ; restore sprite number (but X now clobbered)
      ; recalculate offset... (store offset earlier)
      ; This will need careful register management — implement at coding time
      RTS
@enable
      JSR   LAB_IGBY           ; skip ON token
      PLA                     ; sprite number
      ; Set bit in SPREN/SPRENH
      CMP   #$08
      BCS   @hiEn
      TAX
      LDA   #$01
@shiftEn
      DEX
      BMI   @doneShiftEn
      ASL
      JMP   @shiftEn
@doneShiftEn
      ORA   $A009
      STA   $A009
      RTS
@hiEn
      SEC
      SBC   #$08
      TAX
      LDA   #$01
@shiftEnH
      DEX
      BMI   @doneShiftEnH
      ASL
      JMP   @shiftEnH
@doneShiftEnH
      ORA   $A00A
      STA   $A00A
      RTS
@disable
      JSR   LAB_IGBY           ; skip OFF token
      PLA                     ; sprite number
      ; Clear bit in SPREN/SPRENH — invert the enable logic
      CMP   #$08
      BCS   @hiDis
      TAX
      LDA   #$01
@shiftDis
      DEX
      BMI   @doneShiftDis
      ASL
      JMP   @shiftDis
@doneShiftDis
      EOR   #$FF
      AND   $A009
      STA   $A009
      RTS
@hiDis
      SEC
      SBC   #$08
      TAX
      LDA   #$01
@shiftDisH
      DEX
      BMI   @doneShiftDisH
      ASL
      JMP   @shiftDisH
@doneShiftDisH
      EOR   #$FF
      AND   $A00A
      STA   $A00A
      RTS
@temp .byte $00
```

**Note:** The SPRITE command handler above is pseudocode-quality. The actual implementation will need careful 6502 register management (A/X/Y are all only 8-bit, stack is limited). Implement and debug carefully at coding time.

**Step 8: Implement function handlers**

```asm
LAB_SPRITEX
      JSR   LAB_F2FX           ; sprite number in Itempl
      LDA   Itempl
      ; Calculate sprite reg base: $A010 + n*6
      ASL
      STA   @temp
      ASL
      CLC
      ADC   @temp
      TAX
      LDA   $A011,X            ; X high byte
      AND   #$01
      TAY                     ; Y = high byte
      LDA   $A010,X            ; X low byte
      JMP   LAB_AYFC           ; convert AY to FAC1 and return
@temp .byte $00

LAB_SPRITEY
      JSR   LAB_F2FX
      LDA   Itempl
      ASL
      STA   @temp
      ASL
      CLC
      ADC   @temp
      TAX
      LDA   $A012,X            ; Y position
      TAY
      LDA   #$00
      JMP   LAB_AYFC
@temp .byte $00

LAB_COLLISION
      JSR   LAB_F2FX
      LDA   $A00B              ; COLLST register (read clears)
      TAY
      LDA   #$00
      JMP   LAB_AYFC

LAB_BUMPED
      JSR   LAB_F2FX
      LDA   $A00C              ; COLLBG register (read clears)
      TAY
      LDA   #$00
      JMP   LAB_AYFC
```

**Step 9: Implement remaining stub commands**

SPRITECOLOR, SPRITESHAPE, SPRITEDATA, SOUND, VOLUME, ENVELOPE, WAVE — follow patterns above. Sound commands are stubs that just parse and discard arguments.

**Step 10: Build and test**

Run: `cd ehbasic && make clean && make`
Expected: Assembles without errors.

Run: `cp ehbasic/basic.bin e6502.TUI/Resources/ehbasic.bin`
Run: `dotnet run --project e6502.TUI`
Expected: Boot EhBASIC. Test:
```
MODE 2
COLOR 2
CIRCLE 80,25,20
LINE 0,0,159,49
CLS
```

**Step 11: Commit**

```bash
git add ehbasic/basic.asm e6502.TUI/Resources/ehbasic.bin
git commit -m "feat: implement EhBASIC 6502 handlers for all VGC commands"
```

---

## Task 12: Integration Testing and Polish

End-to-end testing, bug fixes, and cleanup.

**Files:**
- Create: `e6502UnitTests/IntegrationTests.cs`
- Modify: various (bug fixes)

**Step 1: Write integration test — BASIC program runs through CPU**

```csharp
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;
using KDS.e6502;

namespace e6502UnitTests;

[TestClass]
public class IntegrationTests
{
    [TestMethod]
    public void CpuBootsWithCompositeBus()
    {
        var bus = new CompositeBusDevice();
        var cpu = new Cpu(bus);
        cpu.Boot();
        // Reset vector should point into EhBASIC code (>= $C000)
        Assert.IsTrue(cpu.Pc >= 0xFF80, $"PC should be in ROM, got ${cpu.Pc:X4}");
    }

    [TestMethod]
    public void CharOutProducesScreenContent()
    {
        var bus = new CompositeBusDevice();
        // Simulate EhBASIC writing "Hi" via CHAROUT
        bus.Write(VgcConstants.RegCharOut, (byte)'H');
        bus.Write(VgcConstants.RegCharOut, (byte)'i');
        Assert.AreEqual((byte)'H', bus.Read(VgcConstants.ScreenRamStart));
        Assert.AreEqual((byte)'i', bus.Read((ushort)(VgcConstants.ScreenRamStart + 1)));
    }

    [TestMethod]
    public void CharInRoundTrip()
    {
        var bus = new CompositeBusDevice();
        bus.Write(VgcConstants.RegCharIn, 65); // simulate keypress
        Assert.AreEqual(65, bus.Read(VgcConstants.RegCharIn)); // CPU reads it
        Assert.AreEqual(0, bus.Read(VgcConstants.RegCharIn));  // cleared
    }

    [TestMethod]
    public void GfxCommandViabus()
    {
        var bus = new CompositeBusDevice();
        // Set draw color
        bus.Write(VgcConstants.GfxP0, 1);
        bus.Write(VgcConstants.GfxCmdReg, VgcConstants.GfxCmdGcolor);
        // Plot pixel at 10,20
        bus.Write(VgcConstants.GfxP0, 10);
        bus.Write(VgcConstants.GfxP0H, 0);
        bus.Write(VgcConstants.GfxP1, 20);
        bus.Write(VgcConstants.GfxP1H, 0);
        bus.Write(VgcConstants.GfxCmdReg, VgcConstants.GfxCmdPlot);
        Assert.AreEqual(1, bus.Vgc.GetGfxPixel(10, 20));
    }

    [TestMethod]
    public void ExistingCpuTestsStillPass()
    {
        // Smoke test: run AllSuiteA with original BusDevice
        var bus = new KDS.e6502.BusDevice(
            File.ReadAllBytes("Resources/AllSuiteA.bin"), 0x4000);
        var cpu = new Cpu(bus, E6502Type.Cmos);
        cpu.Boot(0x0400);
        ushort prevPc;
        do
        {
            prevPc = cpu.Pc;
            cpu.ExecuteNext();
        } while (prevPc != cpu.Pc);
        Assert.AreEqual(0x45c0, cpu.Pc);
    }
}
```

**Step 2: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: All tests PASS (existing CPU tests + all new VGC/bus/graphics/sprite/editor/integration tests).

**Step 3: Manual test checklist**

Run: `dotnet run --project e6502.TUI`

Test each feature:
- [ ] Cold start: Press C, Enter → "Ready" prompt
- [ ] `PRINT "HELLO WORLD"` → text appears
- [ ] `CLS` → screen clears
- [ ] `COLOR 2` then `PRINT "RED"` → red text
- [ ] `LOCATE 40,12` then `PRINT "CENTER"` → centered text
- [ ] `MODE 2` → mixed mode
- [ ] `CIRCLE 80,25,15` → circle drawn
- [ ] `LINE 0,0,159,49` → diagonal line
- [ ] `RECT 10,10,30,30` → rectangle
- [ ] `FILL 50,10,70,30` → filled rectangle
- [ ] Arrow keys move cursor on screen
- [ ] Cursor up to listed line, edit, RETURN → re-enters line
- [ ] `SPRITE 0,ON` then `SPRITE 0,80,25` → sprite visible (if shape data set)
- [ ] `SPRITEDATA 0,0,255,255` → defines shape row

**Step 4: Fix any bugs found during manual testing**

**Step 5: Final commit**

```bash
git add -A
git commit -m "feat: integration tests and polish for VGC system"
```

---

## Task 13: EhBASIC Build Integration

Add a pre-build step so `dotnet build` automatically rebuilds ehbasic.bin.

**Files:**
- Modify: `e6502.TUI/e6502.TUI.csproj`

**Step 1: Add pre-build target**

```xml
<Target Name="BuildEhBasic" BeforeTargets="Build">
    <Exec Command="make -C $(ProjectDir)../ehbasic" />
    <Copy SourceFiles="$(ProjectDir)../ehbasic/basic.bin"
          DestinationFiles="$(ProjectDir)Resources/ehbasic.bin" />
</Target>
```

**Step 2: Test**

Run: `dotnet build e6502.TUI`
Expected: ehbasic assembles, binary copied, TUI builds.

**Step 3: Commit**

```bash
git add e6502.TUI/e6502.TUI.csproj
git commit -m "chore: auto-build EhBASIC from source on dotnet build"
```
