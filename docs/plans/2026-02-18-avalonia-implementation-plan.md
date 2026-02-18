# Avalonia Renderer & Multicolor Sprites Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace Terminal.Gui TUI with an Avalonia-based renderer that provides real pixel graphics, multicolor sprites, and a command-driven VGC interface.

**Architecture:** New `e6502.Avalonia` project replaces `e6502.TUI`. Hardware classes move and get updated (new memory map, multicolor sprites, unified command register). Rendering pipeline writes to a `WriteableBitmap` at 640x200 native resolution, scaled with nearest-neighbor to fill window. All existing tests updated for new namespace/addresses.

**Tech Stack:** .NET 10, Avalonia UI 11.x, SkiaSharp (via Avalonia), MSTest, CP437 bitmap font

**Design Doc:** `docs/plans/2026-02-18-avalonia-renderer-design.md`

---

## Task 1: Create e6502.Avalonia Project Scaffold

**Files:**
- Create: `e6502.Avalonia/e6502.Avalonia.csproj`
- Create: `e6502.Avalonia/Program.cs`
- Create: `e6502.Avalonia/App.axaml`
- Create: `e6502.Avalonia/App.axaml.cs`
- Create: `e6502.Avalonia/MainWindow.axaml`
- Create: `e6502.Avalonia/MainWindow.axaml.cs`
- Create: `e6502.Avalonia/Resources/` (directory)
- Modify: `e6502.sln`

**Step 1: Create the .csproj**

```xml
<Project Sdk="Microsoft.NET.Sdk">

    <PropertyGroup>
        <OutputType>WinExe</OutputType>
        <TargetFramework>net10.0</TargetFramework>
        <ImplicitUsings>enable</ImplicitUsings>
        <Nullable>enable</Nullable>
        <BuiltInComInteropSupport>true</BuiltInComInteropSupport>
        <ApplicationManifest>app.manifest</ApplicationManifest>
    </PropertyGroup>

    <ItemGroup>
        <PackageReference Include="Avalonia" Version="11.2.5" />
        <PackageReference Include="Avalonia.Desktop" Version="11.2.5" />
        <PackageReference Include="Avalonia.Themes.Fluent" Version="11.2.5" />
    </ItemGroup>

    <ItemGroup>
        <ProjectReference Include="..\e6502\e6502.csproj" />
    </ItemGroup>

    <Target Name="BuildEhBasic" BeforeTargets="Build">
        <Exec Command="make -C $(ProjectDir)../ehbasic" />
        <Copy SourceFiles="$(ProjectDir)../ehbasic/basic.bin"
              DestinationFiles="$(ProjectDir)Resources/ehbasic.bin" />
    </Target>

    <ItemGroup>
        <Folder Include="Resources\" />
    </ItemGroup>

    <ItemGroup>
        <None Update="Resources\ehbasic.bin">
            <CopyToOutputDirectory>Always</CopyToOutputDirectory>
        </None>
    </ItemGroup>

</Project>
```

Note: Check the latest Avalonia 11.x version at nuget.org and use that. The version above is a placeholder. Also create `app.manifest` if Avalonia template requires it (check Avalonia docs for .NET 10 — it may not be needed; if not, remove the `ApplicationManifest` line).

**Step 2: Create Program.cs**

```csharp
using Avalonia;

namespace e6502.Avalonia;

class Program
{
    [STAThread]
    public static void Main(string[] args) => BuildAvaloniaApp()
        .StartWithClassicDesktopLifetime(args);

    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .LogToTrace();
}
```

**Step 3: Create App.axaml + App.axaml.cs**

`App.axaml`:
```xml
<Application xmlns="https://github.com/avaloniaui"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
             x:Class="e6502.Avalonia.App">
    <Application.Styles>
        <FluentTheme />
    </Application.Styles>
</Application>
```

`App.axaml.cs`:
```csharp
using Avalonia;
using Avalonia.Markup.Xaml;

namespace e6502.Avalonia;

public partial class App : Application
{
    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }
}
```

**Step 4: Create MainWindow.axaml + MainWindow.axaml.cs**

Minimal shell — just a black window. Canvas added in later tasks.

`MainWindow.axaml`:
```xml
<Window xmlns="https://github.com/avaloniaui"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        x:Class="e6502.Avalonia.MainWindow"
        Title="e6502 Emulator"
        Background="Black"
        Width="800" Height="400">
</Window>
```

`MainWindow.axaml.cs`:
```csharp
using Avalonia.Controls;

namespace e6502.Avalonia;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }
}
```

**Step 5: Add project to solution**

Run: `dotnet sln e6502.sln add e6502.Avalonia/e6502.Avalonia.csproj`

**Step 6: Verify it builds and launches**

Run: `dotnet build e6502.Avalonia`
Expected: BUILD SUCCEEDED

Run: `dotnet run --project e6502.Avalonia` (manual verification — black window appears, Ctrl-C to exit)

**Step 7: Commit**

```bash
git add e6502.Avalonia/ e6502.sln
git commit -m "feat: create e6502.Avalonia project scaffold"
```

---

## Task 2: Move and Update VgcConstants

Move `VgcConstants` from TUI to Avalonia with the new memory map from the design doc. This is the foundation everything else builds on.

**Files:**
- Create: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502UnitTests/e6502UnitTests.csproj` (add reference to Avalonia project)

**Step 1: Create the new VgcConstants**

Create `e6502.Avalonia/Hardware/VgcConstants.cs` with the updated memory map. Key changes from old:
- Namespace: `e6502.Avalonia.Hardware` (was `e6502.TUI.Hardware`)
- Command register: `$A010` (was gfx-only at `$A070`)
- Expanded command params: 14 bytes `$A011-$A01E` (was 8 bytes `$A071-$A078`)
- Sprite registers area (`$A010-$A06F`): **removed** — sprites are command-driven only
- Sprite shapes (`$A200-$A3FF`): **removed from bus** — host-side only
- `RegSpriteEn`/`RegSpriteEnH` at `$A009`/`$A00A`: replaced by `RegSpriteCount` (read-only) at `$A009`
- Screen RAM: `$AA00-$B1CF` (was `$A400-$ABCF`)
- Color RAM: `$B1D0-$B99F` (was `$ABD0-$B39F`)
- New sprite command bytes: `$10-$18`
- `SpriteShapeSize = 128` (was 32 — now 4-bit color, 8 bytes/row × 16 rows)
- New constants: `SpritePixelSize = 16`, `SpriteBytesPerRow = 8`

```csharp
namespace e6502.Avalonia.Hardware;

public static class VgcConstants
{
    // ── Memory map ──────────────────────────────────────────────────────

    public const int ZeroPageBase    = 0x0000;
    public const int ZeroPageEnd     = 0x00FF;

    public const int StackBase       = 0x0100;
    public const int StackEnd        = 0x01FF;

    public const int VectorTableBase = 0x0200;
    public const int VectorTableEnd  = 0x027F;

    public const int BasicBase       = 0x0280;
    public const int BasicEnd        = 0x9FFF;

    public const int VgcBase         = 0xA000;
    public const int VgcRegsEnd      = 0xA01E;   // core regs + command regs

    public const int VscBase         = 0xA100;
    public const int VscEnd          = 0xA1FF;

    // $A200-$A9FF: reserved (sprite shapes are host-side only)

    public const int CharRamBase     = 0xAA00;
    public const int CharRamEnd      = 0xB1CF;   // AA00 + 2000 - 1

    public const int ColorRamBase    = 0xB1D0;
    public const int ColorRamEnd     = 0xB99F;   // B1D0 + 2000 - 1

    public const int FioBase         = 0xB9A0;
    public const int FioEnd          = 0xB9EF;

    public const int FioCmd          = 0xB9A0;
    public const int FioStatus       = 0xB9A1;
    public const int FioErrCode      = 0xB9A2;
    public const int FioNameLen      = 0xB9A3;
    public const int FioSrcL         = 0xB9A4;
    public const int FioSrcH         = 0xB9A5;
    public const int FioEndL         = 0xB9A6;
    public const int FioEndH         = 0xB9A7;
    public const int FioSizeL        = 0xB9A8;
    public const int FioSizeH        = 0xB9A9;
    public const int FioName         = 0xB9B0;
    public const int FioNameEnd      = 0xB9EF;

    public const byte FioCmdSave     = 0x01;
    public const byte FioCmdLoad     = 0x02;

    public const byte FioStatusIdle  = 0x00;
    public const byte FioStatusOk    = 0x02;
    public const byte FioStatusError = 0x03;

    public const byte FioErrNone     = 0x00;
    public const byte FioErrNotFound = 0x01;
    public const byte FioErrIo       = 0x02;

    public const int FreeBase        = 0xBA00;
    public const int FreeEnd         = 0xBFFF;

    public const int RomBase         = 0xC000;
    public const int RomEnd          = 0xFFFF;

    // ── Screen / graphics dimensions ────────────────────────────────────

    public const int ScreenCols      = 80;
    public const int ScreenRows      = 25;
    public const int ScreenSize      = ScreenCols * ScreenRows;  // 2000

    public const int GfxWidth        = 160;
    public const int GfxHeight       = 50;

    public const int MaxSprites      = 16;
    public const int SpritePixelSize = 16;        // 16x16 pixels per sprite
    public const int SpriteBytesPerRow = 8;        // 16 pixels × 4 bits / 8 = 8 bytes/row
    public const int SpriteShapeSize = SpriteBytesPerRow * SpritePixelSize;  // 128 bytes
    public const int PaletteSize     = 16;

    // ── VGC core registers ($A000-$A00F) ────────────────────────────────

    public const int RegMode         = 0xA000;   // 0=text, 1=gfx, 2=mixed
    public const int RegBgCol        = 0xA001;
    public const int RegFgCol        = 0xA002;
    public const int RegCursorX      = 0xA003;   // 0-79
    public const int RegCursorY      = 0xA004;   // 0-24
    public const int RegScrollX      = 0xA005;
    public const int RegScrollY      = 0xA006;
    public const int RegBank         = 0xA007;   // reserved
    public const int RegStatus       = 0xA008;   // frame counter (read-only)
    public const int RegSpriteCount  = 0xA009;   // enabled sprite count (read-only)
    public const int RegReservedA    = 0xA00A;
    public const int RegColSt        = 0xA00B;   // sprite-sprite collision (read-clears)
    public const int RegColBg        = 0xA00C;   // sprite-background collision (read-clears)
    public const int RegBorder       = 0xA00D;
    public const int RegCharOut      = 0xA00E;   // character output
    public const int RegCharIn       = 0xA00F;   // character input

    // ── Command registers ($A010-$A01E) ─────────────────────────────────

    public const int RegCmd          = 0xA010;   // write triggers execution
    public const int RegP0           = 0xA011;
    public const int RegP1           = 0xA012;
    public const int RegP2           = 0xA013;
    public const int RegP3           = 0xA014;
    public const int RegP4           = 0xA015;
    public const int RegP5           = 0xA016;
    public const int RegP6           = 0xA017;
    public const int RegP7           = 0xA018;
    public const int RegP8           = 0xA019;
    public const int RegP9           = 0xA01A;
    public const int RegP10          = 0xA01B;
    public const int RegP11          = 0xA01C;
    public const int RegP12          = 0xA01D;
    public const int RegP13          = 0xA01E;

    // ── Graphics commands ───────────────────────────────────────────────

    public const byte CmdPlot        = 0x01;
    public const byte CmdUnplot      = 0x02;
    public const byte CmdLine        = 0x03;
    public const byte CmdCircle      = 0x04;
    public const byte CmdRect        = 0x05;
    public const byte CmdFill        = 0x06;
    public const byte CmdGcls        = 0x07;
    public const byte CmdGcolor      = 0x08;

    // ── Sprite commands ─────────────────────────────────────────────────

    public const byte CmdSprDef      = 0x10;     // P0:sprite P1:x P2:y P3:color
    public const byte CmdSprRow      = 0x11;     // P0:sprite P1:row P2-P9:data (8 bytes)
    public const byte CmdSprClr      = 0x12;     // P0:sprite
    public const byte CmdSprCopy     = 0x13;     // P0:src P1:dest
    public const byte CmdSprPos      = 0x14;     // P0:sprite P1-P2:x(16) P3:y
    public const byte CmdSprEna      = 0x15;     // P0:sprite
    public const byte CmdSprDis      = 0x16;     // P0:sprite
    public const byte CmdSprFlip     = 0x17;     // P0:sprite P1:flags (bit0=X, bit1=Y)
    public const byte CmdSprPri      = 0x18;     // P0:sprite P1:priority (0/1/2)

    // ── Sprite priority layers ──────────────────────────────────────────

    public const byte SpritePriBehindAll    = 0;  // behind text and graphics
    public const byte SpritePriBetween      = 1;  // between text and graphics
    public const byte SpritePriInFront      = 2;  // in front of everything (default)
}
```

**Step 2: Update test project reference**

Modify `e6502UnitTests/e6502UnitTests.csproj`: add reference to `e6502.Avalonia`, keep TUI reference for now (we'll remove it when TUI is deleted).

```xml
<ProjectReference Include="..\e6502.Avalonia\e6502.Avalonia.csproj" />
```

**Step 3: Verify it builds**

Run: `dotnet build e6502.Avalonia`
Expected: BUILD SUCCEEDED (VgcConstants is just a static class with constants)

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502UnitTests/e6502UnitTests.csproj
git commit -m "feat: add VgcConstants with new memory map for Avalonia"
```

---

## Task 3: VGC Core — Multicolor Sprites and Command Interface

This is the biggest task. Rewrite `VirtualGraphicsController` for the new architecture: unified command register, multicolor sprite shapes (host-side, 4-bit per pixel), and sprite commands.

**Files:**
- Create: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Create: `e6502.Avalonia/Rendering/BlockGraphics.cs` (copy from TUI, update namespace)
- Create: `e6502UnitTests/AvaloniaVgcTests.cs` (new tests for multicolor sprites + commands)

**Step 1: Copy BlockGraphics with new namespace**

Copy `e6502.TUI/Rendering/BlockGraphics.cs` → `e6502.Avalonia/Rendering/BlockGraphics.cs`. Change namespace to `e6502.Avalonia.Rendering`. This is a pure algorithm class with no external dependencies — it just needs the namespace change.

**Step 2: Write failing tests for the new VGC**

Create `e6502UnitTests/AvaloniaVgcTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaVgcTests
{
    private VirtualGraphicsController _vgc = null!;

    [TestInitialize]
    public void Setup() => _vgc = new VirtualGraphicsController();

    // ── Initial state ───────────────────────────────────────────────────

    [TestMethod]
    public void InitialMode_IsZero() =>
        Assert.AreEqual(0, _vgc.GetMode());

    [TestMethod]
    public void InitialFgColor_IsOne() =>
        Assert.AreEqual(1, _vgc.Read(VgcConstants.RegFgCol));

    [TestMethod]
    public void InitialScreenRam_IsSpaces()
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(0x20, _vgc.GetScreenChar(col, row));
    }

    // ── Command register: Graphics ──────────────────────────────────────

    [TestMethod]
    public void GfxPlot_ViaCommandRegister_SetsPixel()
    {
        // GCOLOR 5
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdGcolor);

        // PLOT at (10, 5)
        _vgc.Write(VgcConstants.RegP0, 10);  // x low
        _vgc.Write(VgcConstants.RegP1, 0);   // x high
        _vgc.Write(VgcConstants.RegP2, 5);   // y low
        _vgc.Write(VgcConstants.RegP3, 0);   // y high
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdPlot);

        Assert.IsTrue(_vgc.GetGfxPixel(10, 5));
        Assert.AreEqual(5, _vgc.GetGfxPixelColor(10, 5));
    }

    // ── Sprite commands ─────────────────────────────────────────────────

    [TestMethod]
    public void SprDef_SetsSinglePixel()
    {
        // SDEF: sprite 0, x=8, y=0, color=5
        _vgc.Write(VgcConstants.RegP0, 0);   // sprite index
        _vgc.Write(VgcConstants.RegP1, 8);   // x
        _vgc.Write(VgcConstants.RegP2, 0);   // y
        _vgc.Write(VgcConstants.RegP3, 5);   // color
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDef);

        var shape = _vgc.GetSpriteShape(0);
        // Row 0, pixel 8 → byte 4 (8/2), high nibble (8 is even)
        Assert.AreEqual(0x50, shape[4]); // color 5 in high nibble
    }

    [TestMethod]
    public void SprRow_DefinesFullRow()
    {
        // SROW: sprite 0, row 0, 8 bytes of data
        _vgc.Write(VgcConstants.RegP0, 0);    // sprite index
        _vgc.Write(VgcConstants.RegP1, 0);    // row number
        _vgc.Write(VgcConstants.RegP2, 0x12); // pixels 0-1: colors 1,2
        _vgc.Write(VgcConstants.RegP3, 0x34); // pixels 2-3: colors 3,4
        _vgc.Write(VgcConstants.RegP4, 0x56); // pixels 4-5: colors 5,6
        _vgc.Write(VgcConstants.RegP5, 0x78); // pixels 6-7: colors 7,8
        _vgc.Write(VgcConstants.RegP6, 0x9A); // pixels 8-9: colors 9,10
        _vgc.Write(VgcConstants.RegP7, 0xBC); // pixels 10-11: colors 11,12
        _vgc.Write(VgcConstants.RegP8, 0xDE); // pixels 12-13: colors 13,14
        _vgc.Write(VgcConstants.RegP9, 0xF0); // pixels 14-15: colors 15,0
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var shape = _vgc.GetSpriteShape(0);
        Assert.AreEqual(0x12, shape[0]);
        Assert.AreEqual(0x34, shape[1]);
        Assert.AreEqual(0xF0, shape[7]);
    }

    [TestMethod]
    public void SprClr_ClearsShapeToTransparent()
    {
        // Define a pixel first
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDef);

        // Clear
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprClr);

        var shape = _vgc.GetSpriteShape(0);
        for (int i = 0; i < VgcConstants.SpriteShapeSize; i++)
            Assert.AreEqual(0, shape[i]);
    }

    [TestMethod]
    public void SprCopy_CopiesShapeData()
    {
        // Define row in sprite 0
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0xAB);
        _vgc.Write(VgcConstants.RegP3, 0xCD);
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 0);
        _vgc.Write(VgcConstants.RegP6, 0);
        _vgc.Write(VgcConstants.RegP7, 0);
        _vgc.Write(VgcConstants.RegP8, 0);
        _vgc.Write(VgcConstants.RegP9, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        // Copy sprite 0 → sprite 3
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprCopy);

        var shape3 = _vgc.GetSpriteShape(3);
        Assert.AreEqual(0xAB, shape3[0]);
        Assert.AreEqual(0xCD, shape3[1]);
    }

    [TestMethod]
    public void SprPos_SetsPosition()
    {
        // Enable sprite first
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        // SPOS: sprite 0, x=300 (0x012C), y=25
        _vgc.Write(VgcConstants.RegP0, 0);    // sprite
        _vgc.Write(VgcConstants.RegP1, 0x2C); // x low
        _vgc.Write(VgcConstants.RegP2, 0x01); // x high
        _vgc.Write(VgcConstants.RegP3, 25);   // y
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(300, state.x);
        Assert.AreEqual(25, state.y);
    }

    [TestMethod]
    public void SprEna_EnablesSprite()
    {
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        Assert.IsTrue(_vgc.GetSpriteState(5).enabled);
    }

    [TestMethod]
    public void SprDis_DisablesSprite()
    {
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDis);

        Assert.IsFalse(_vgc.GetSpriteState(5).enabled);
    }

    [TestMethod]
    public void SprFlip_SetsFlags()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0x03); // both X and Y flip
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(0x03, state.flags);
    }

    [TestMethod]
    public void SprPri_SetsPriority()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, VgcConstants.SpritePriBehindAll);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPri);

        Assert.AreEqual(VgcConstants.SpritePriBehindAll, _vgc.GetSpriteState(0).priority);
    }

    [TestMethod]
    public void SpriteCount_ReflectsEnabledSprites()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        Assert.AreEqual(2, _vgc.Read(VgcConstants.RegSpriteCount));
    }

    // ── CharOut still works ─────────────────────────────────────────────

    [TestMethod]
    public void CharOut_WritesCharToScreen()
    {
        _vgc.Write(VgcConstants.RegCharOut, (byte)'A');
        Assert.AreEqual((byte)'A', _vgc.GetScreenChar(0, 0));
        Assert.AreEqual(1, _vgc.GetCursorX());
    }

    // ── Collision registers still clear on read ─────────────────────────

    [TestMethod]
    public void CollisionRegisters_ClearOnRead()
    {
        _vgc.SetCollisionRegisters(0xAB, 0xCD);
        byte ss = _vgc.Read(VgcConstants.RegColSt);
        byte sb = _vgc.Read(VgcConstants.RegColBg);
        Assert.AreEqual(0xAB, ss);
        Assert.AreEqual(0xCD, sb);
        Assert.AreEqual(0, _vgc.Read(VgcConstants.RegColSt));
        Assert.AreEqual(0, _vgc.Read(VgcConstants.RegColBg));
    }

    // ── OwnsAddress with new ranges ─────────────────────────────────────

    [TestMethod]
    public void OwnsAddress_VgcBase_True() =>
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.VgcBase));

    [TestMethod]
    public void OwnsAddress_ColorRamEnd_True() =>
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.ColorRamEnd));

    [TestMethod]
    public void OwnsAddress_BelowRange_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(0x9FFF));

    [TestMethod]
    public void OwnsAddress_FioBase_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(VgcConstants.FioBase));
}
```

**Step 3: Run tests to verify they fail**

Run: `dotnet test --filter "AvaloniaVgcTests" --verbosity normal`
Expected: FAIL (class doesn't exist yet)

**Step 4: Implement VirtualGraphicsController**

Create `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`. This is the core rewrite. Key differences from TUI version:
- Internal sprite state arrays replace bus-addressable sprite registers
- `_spriteShapes` is `byte[MaxSprites * SpriteShapeSize]` (128 bytes each, 4-bit color)
- `_spriteEnabled`, `_spriteX`, `_spriteY`, `_spriteFlags`, `_spritePriority` arrays for per-sprite state
- Command register at `$A010` dispatches both graphics and sprite commands
- `OwnsAddress` range: `$A000-$A01E` (regs) + `$AA00-$B99F` (char+color RAM), skipping VSC at `$A100`
- `GetSpriteShape(int index)` returns `ReadOnlySpan<byte>` of 128 bytes (multicolor)
- `GetSpriteState` returns named tuple with all sprite fields

The VGC should NOT own the range `$A020-$A0FF` or `$A200-$A9FF` — those are reserved/free space. It owns:
- `$A000-$A01E` (registers + command)
- `$AA00-$B1CF` (char RAM)
- `$B1D0-$B99F` (color RAM)

Implementation notes:
- Reuse the existing `HandleCharOut` logic exactly (BS, LF, FF, CR, HOME, printable chars, scroll)
- Reuse `ExecuteGfxCommand` for graphics commands, adapted for new param register offsets
- Add `ExecuteSpriteCommand` for sprite commands
- The `_cmdRegs` array holds command byte + 14 params (15 bytes total, `$A010-$A01E`)
- Graphics command params: P0-P1 are 16-bit x, P2-P3 are 16-bit y, P4-P5 and P6-P7 for second point. Read from `_cmdRegs[1..14]` (offset from RegCmd)
- Sprite command params: P0 = `_cmdRegs[1]`, P1 = `_cmdRegs[2]`, etc.

```csharp
namespace e6502.Avalonia.Hardware;

public class VirtualGraphicsController
{
    // Core registers $A000-$A00F (16 bytes)
    private readonly byte[] _regs = new byte[16];

    // Command registers $A010-$A01E (15 bytes: cmd + 14 params)
    private readonly byte[] _cmdRegs = new byte[15];

    // Screen RAM (2000 bytes)
    private readonly byte[] _screenRam = new byte[VgcConstants.ScreenSize];

    // Color RAM (2000 bytes)
    private readonly byte[] _colorRam = new byte[VgcConstants.ScreenSize];

    // Block graphics bitmap (160x50, NOT 6502-addressable)
    private readonly byte[] _gfxBitmap = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];

    // Sprite shape data — host-side only (16 sprites × 128 bytes, 4-bit color)
    private readonly byte[] _spriteShapes = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteShapeSize];

    // Per-sprite state (all command-driven, not bus-addressable)
    private readonly int[] _spriteX = new int[VgcConstants.MaxSprites];
    private readonly int[] _spriteY = new int[VgcConstants.MaxSprites];
    private readonly bool[] _spriteEnabled = new bool[VgcConstants.MaxSprites];
    private readonly byte[] _spriteFlags = new byte[VgcConstants.MaxSprites];    // bit0=xFlip, bit1=yFlip
    private readonly byte[] _spritePriority = new byte[VgcConstants.MaxSprites]; // 0/1/2

    private byte _gfxDrawColor;

    // ... constructor, Reset(), OwnsAddress(), Read(), Write()
    // ... HandleCharOut(), ExecuteCommand(), ExecuteGfxCommand(), ExecuteSpriteCommand()
    // ... GetSpriteShape(), GetSpriteState(), accessors, ScreenEditor integration
}
```

For `GetSpriteState`, return a named tuple:
```csharp
public (int x, int y, bool enabled, byte flags, byte priority) GetSpriteState(int index)
{
    return (_spriteX[index], _spriteY[index], _spriteEnabled[index],
            _spriteFlags[index], _spritePriority[index]);
}
```

For `OwnsAddress`:
```csharp
public bool OwnsAddress(ushort address) =>
    (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcRegsEnd) ||
    (address >= VgcConstants.CharRamBase && address <= VgcConstants.ColorRamEnd);
```

For the Write dispatch on `RegCmd`, call `ExecuteCommand`:
```csharp
private void ExecuteCommand(byte cmd)
{
    if (cmd >= 0x01 && cmd <= 0x08)
        ExecuteGfxCommand(cmd);
    else if (cmd >= 0x10 && cmd <= 0x18)
        ExecuteSpriteCommand(cmd);
}
```

For `ExecuteSpriteCommand`:
```csharp
protected virtual void ExecuteSpriteCommand(byte cmd)
{
    byte p0 = _cmdRegs[1]; // sprite index for most commands

    switch (cmd)
    {
        case VgcConstants.CmdSprDef:
        {
            int sprite = p0;
            int x = _cmdRegs[2];
            int y = _cmdRegs[3];
            byte color = _cmdRegs[4];
            if (sprite >= VgcConstants.MaxSprites || x >= 16 || y >= 16) return;
            int byteIdx = sprite * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
            if (x % 2 == 0) // high nibble
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
            else // low nibble
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
            break;
        }
        case VgcConstants.CmdSprRow:
        {
            int sprite = p0;
            int row = _cmdRegs[2];
            if (sprite >= VgcConstants.MaxSprites || row >= 16) return;
            int baseIdx = sprite * VgcConstants.SpriteShapeSize + row * VgcConstants.SpriteBytesPerRow;
            for (int i = 0; i < VgcConstants.SpriteBytesPerRow; i++)
                _spriteShapes[baseIdx + i] = _cmdRegs[3 + i]; // P2-P9
            break;
        }
        case VgcConstants.CmdSprClr:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            Array.Clear(_spriteShapes, p0 * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);
            break;
        }
        case VgcConstants.CmdSprCopy:
        {
            byte dest = _cmdRegs[2];
            if (p0 >= VgcConstants.MaxSprites || dest >= VgcConstants.MaxSprites) return;
            Array.Copy(_spriteShapes, p0 * VgcConstants.SpriteShapeSize,
                       _spriteShapes, dest * VgcConstants.SpriteShapeSize,
                       VgcConstants.SpriteShapeSize);
            break;
        }
        case VgcConstants.CmdSprPos:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            _spriteX[p0] = _cmdRegs[2] | (_cmdRegs[3] << 8);
            _spriteY[p0] = _cmdRegs[4];
            break;
        }
        case VgcConstants.CmdSprEna:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            _spriteEnabled[p0] = true;
            UpdateSpriteCount();
            break;
        }
        case VgcConstants.CmdSprDis:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            _spriteEnabled[p0] = false;
            UpdateSpriteCount();
            break;
        }
        case VgcConstants.CmdSprFlip:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            _spriteFlags[p0] = (byte)(_cmdRegs[2] & 0x03);
            break;
        }
        case VgcConstants.CmdSprPri:
        {
            if (p0 >= VgcConstants.MaxSprites) return;
            _spritePriority[p0] = (byte)Math.Min(_cmdRegs[2], 2);
            break;
        }
    }
}

private void UpdateSpriteCount()
{
    int count = 0;
    for (int i = 0; i < VgcConstants.MaxSprites; i++)
        if (_spriteEnabled[i]) count++;
    _regs[VgcConstants.RegSpriteCount - VgcConstants.VgcBase] = (byte)count;
}
```

Note on `ExecuteGfxCommand`: adapt from TUI version but read params from `_cmdRegs[1..14]` instead of `_gfxRegs[1..8]`. The parameter layout is the same (P0 = 16-bit at offsets 1-2, P1 = 16-bit at offsets 3-4, P2 = 8-bit at offset 5, P3 = 8-bit at offset 6), just shifted into the new command register array.

**Step 5: Run tests to verify they pass**

Run: `dotnet test --filter "AvaloniaVgcTests" --verbosity normal`
Expected: ALL PASS

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502.Avalonia/Rendering/BlockGraphics.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: VGC with multicolor sprites and command interface"
```

---

## Task 4: Remaining Hardware — CompositeBusDevice, VSC, FileIO

Move the remaining hardware classes, updated for the new memory map.

**Files:**
- Create: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Create: `e6502.Avalonia/Hardware/VirtualSoundController.cs`
- Create: `e6502.Avalonia/Hardware/FileIoController.cs`
- Create: `e6502UnitTests/AvaloniaCompositeBusTests.cs`

**Step 1: Copy VSC and FileIO with new namespace**

`VirtualSoundController` — copy from TUI, change namespace to `e6502.Avalonia.Hardware`. No other changes needed (VSC addresses unchanged in VgcConstants).

`FileIoController` — copy from TUI, change namespace to `e6502.Avalonia.Hardware`. FIO addresses changed ($B3A0→$B9A0 etc.) but they read from `VgcConstants` so no code changes needed beyond namespace.

**Step 2: Create CompositeBusDevice for new layout**

Same pattern as TUI version but with updated routing. The VGC's `OwnsAddress` now returns `true` for the disjoint ranges (`$A000-$A01E` and `$AA00-$B99F`), so the priority chain just works. VSC ($A100-$A1FF) is checked before VGC, so it intercepts correctly.

```csharp
namespace e6502.Avalonia.Hardware;

using KDS.e6502;

public class CompositeBusDevice : IBusDevice
{
    private readonly byte[] _ram = new byte[65536];
    private readonly VirtualGraphicsController _vgc = new();
    private readonly VirtualSoundController _vsc = new();
    private readonly FileIoController _fio;

    public VirtualGraphicsController Vgc => _vgc;
    public FileIoController Fio => _fio;

    public CompositeBusDevice()
    {
        _fio = new FileIoController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);

        string romPath = Path.Combine(AppContext.BaseDirectory, "Resources", "ehbasic.bin");
        byte[] rom = File.ReadAllBytes(romPath);
        rom.CopyTo(_ram, VgcConstants.RomBase);
    }

    public byte Read(ushort address)
    {
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (_vsc.OwnsAddress(address)) return _vsc.Read(address);
        if (_vgc.OwnsAddress(address)) return _vgc.Read(address);
        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_fio.OwnsAddress(address)) { _fio.Write(address, data); return; }
        if (_vsc.OwnsAddress(address)) { _vsc.Write(address, data); return; }
        if (_vgc.OwnsAddress(address)) { _vgc.Write(address, data); return; }
        if (address >= VgcConstants.RomBase) return;
        _ram[address] = data;
    }
}
```

**Step 3: Write bus integration tests**

Create `e6502UnitTests/AvaloniaCompositeBusTests.cs` — mirror the existing `CompositeBusTests` but using the new Avalonia types and addresses. Test: RAM r/w, VGC register access, CharOut, ROM protection, VSC isolation, command execution through bus.

**Step 4: Run tests**

Run: `dotnet test --filter "AvaloniaCompositeBusTests" --verbosity normal`
Expected: ALL PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/
git commit -m "feat: composite bus, VSC, and file I/O for Avalonia"
```

---

## Task 5: CP437 Font and ColorPalette

**Files:**
- Create: `e6502.Avalonia/Resources/cp437.bin` (generated)
- Create: `e6502.Avalonia/Rendering/BitmapFont.cs`
- Create: `e6502.Avalonia/Rendering/ColorPalette.cs`
- Create: `e6502UnitTests/BitmapFontTests.cs`

**Step 1: Generate cp437.bin**

Write a small C# script or use an existing CP437 font binary. The font is 256 characters × 8 bytes = 2048 bytes. Each byte is one row of an 8×8 glyph (MSB = leftmost pixel). Many public domain CP437 fonts exist — the standard IBM VGA 8×8 ROM font.

A reliable approach: embed the font data as a C# byte array in a static initializer, or download a known-good binary. The implementer should search for "CP437 8x8 font binary" or extract from existing resources.

Add to `.csproj`:
```xml
<None Update="Resources\cp437.bin">
    <CopyToOutputDirectory>Always</CopyToOutputDirectory>
</None>
```

**Step 2: Create BitmapFont class**

```csharp
namespace e6502.Avalonia.Rendering;

public class BitmapFont
{
    private readonly byte[] _data; // 2048 bytes (256 chars × 8 rows)
    public const int GlyphWidth = 8;
    public const int GlyphHeight = 8;
    public const int GlyphCount = 256;

    public BitmapFont(byte[] fontData)
    {
        if (fontData.Length != GlyphCount * GlyphHeight)
            throw new ArgumentException($"Font data must be {GlyphCount * GlyphHeight} bytes");
        _data = fontData;
    }

    public static BitmapFont LoadFromFile(string path) =>
        new(File.ReadAllBytes(path));

    /// <summary>
    /// Returns true if the pixel at (x, y) in the given glyph is set.
    /// x: 0-7 (0=leftmost), y: 0-7 (0=topmost).
    /// </summary>
    public bool GetPixel(byte charCode, int x, int y)
    {
        byte row = _data[charCode * GlyphHeight + y];
        return (row & (0x80 >> x)) != 0;
    }

    /// <summary>Returns the raw byte for one row of a glyph.</summary>
    public byte GetRow(byte charCode, int y) =>
        _data[charCode * GlyphHeight + y];
}
```

**Step 3: Create ColorPalette (Avalonia version)**

Same palette values as TUI, but using `Avalonia.Media.Color` instead of `Terminal.Gui.Color`:

```csharp
using Avalonia.Media;

namespace e6502.Avalonia.Rendering;

public static class ColorPalette
{
    private static readonly Color[] _palette =
    [
        Color.FromRgb(0,   0,   0),     // 0  Black
        Color.FromRgb(255, 255, 255),    // 1  White
        Color.FromRgb(136, 0,   0),      // 2  Red
        Color.FromRgb(170, 255, 238),    // 3  Cyan
        Color.FromRgb(204, 68,  204),    // 4  Purple
        Color.FromRgb(0,   204, 85),     // 5  Green
        Color.FromRgb(0,   0,   170),    // 6  Blue
        Color.FromRgb(238, 238, 119),    // 7  Yellow
        Color.FromRgb(221, 136, 85),     // 8  Orange
        Color.FromRgb(102, 68,  0),      // 9  Brown
        Color.FromRgb(255, 119, 119),    // 10 Light Red
        Color.FromRgb(51,  51,  51),     // 11 Grey Dark
        Color.FromRgb(119, 119, 119),    // 12 Grey Med
        Color.FromRgb(170, 255, 102),    // 13 Light Green
        Color.FromRgb(0,   136, 255),    // 14 Light Blue
        Color.FromRgb(187, 187, 187),    // 15 Grey Light
    ];

    /// <summary>Maps a 4-bit color index to an RGBA Color.</summary>
    public static Color Get(int index) => _palette[index & 0x0F];

    /// <summary>Returns a 32-bit BGRA pixel value for direct bitmap writing.</summary>
    public static uint GetBgra(int index)
    {
        var c = _palette[index & 0x0F];
        return (uint)((c.A << 24) | (c.R << 16) | (c.G << 8) | c.B);
    }
}
```

**Step 4: Write tests for BitmapFont**

```csharp
[TestMethod]
public void GetPixel_SpaceChar_AllFalse()
{
    // Space (0x20) should have all pixels off in a standard CP437 font
    var font = BitmapFont.LoadFromFile(Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin"));
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            Assert.IsFalse(font.GetPixel(0x20, x, y));
}

[TestMethod]
public void GetPixel_FullBlock_AllTrue()
{
    // Full block char 0xDB should have all pixels on
    var font = BitmapFont.LoadFromFile(Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin"));
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            Assert.IsTrue(font.GetPixel(0xDB, x, y));
}
```

**Step 5: Run tests, verify pass, commit**

Run: `dotnet test --filter "BitmapFontTests" --verbosity normal`

```bash
git add e6502.Avalonia/Rendering/ e6502.Avalonia/Resources/cp437.bin e6502UnitTests/BitmapFontTests.cs
git commit -m "feat: CP437 bitmap font and Avalonia color palette"
```

---

## Task 6: SpriteRenderer for Multicolor Sprites

**Files:**
- Create: `e6502.Avalonia/Rendering/SpriteRenderer.cs`
- Create: `e6502UnitTests/AvaloniaSpriRendererTests.cs`

**Step 1: Write failing tests**

Test multicolor pixel extraction (4-bit per pixel), flip handling, and collision detection.

```csharp
[TestMethod]
public void GetSpritePixels_MulticolorSprite_ReturnsCorrectColors()
{
    var vgc = new VirtualGraphicsController();

    // Enable sprite 0, position at (10, 5)
    vgc.Write(VgcConstants.RegP0, 0);
    vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
    vgc.Write(VgcConstants.RegP0, 0);
    vgc.Write(VgcConstants.RegP1, 10);
    vgc.Write(VgcConstants.RegP2, 0);
    vgc.Write(VgcConstants.RegP3, 5);
    vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

    // Define row 0: first two pixels are color 1 and 2
    vgc.Write(VgcConstants.RegP0, 0);
    vgc.Write(VgcConstants.RegP1, 0);
    vgc.Write(VgcConstants.RegP2, 0x12); // pixels 0,1 = colors 1,2
    // rest zeros (transparent)
    for (int i = 3; i <= 9; i++)
        vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
    vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

    var pixels = SpriteRenderer.GetSpritePixels(vgc, 0);

    // Should have 2 non-transparent pixels
    Assert.AreEqual(2, pixels.Count);
    Assert.AreEqual(1, pixels[0].Color); // first pixel, color 1
    Assert.AreEqual(2, pixels[1].Color); // second pixel, color 2
    Assert.AreEqual(10, pixels[0].ScreenX);
    Assert.AreEqual(5, pixels[0].ScreenY);
}
```

**Step 2: Implement SpriteRenderer**

Key difference from TUI: sprites are multicolor. Each pixel is a 4-bit nibble. Color 0 = transparent (skip).

```csharp
namespace e6502.Avalonia.Rendering;

using e6502.Avalonia.Hardware;

public struct SpritePixel
{
    public int ScreenX;
    public int ScreenY;
    public byte Color;
}

public static class SpriteRenderer
{
    public static List<SpritePixel> GetSpritePixels(VirtualGraphicsController vgc, int index)
    {
        var state = vgc.GetSpriteState(index);
        if (!state.enabled) return [];

        ReadOnlySpan<byte> shape = vgc.GetSpriteShape(index);
        bool xFlip = (state.flags & 0x01) != 0;
        bool yFlip = (state.flags & 0x02) != 0;

        var pixels = new List<SpritePixel>(256);

        for (int row = 0; row < 16; row++)
        {
            int srcRow = yFlip ? 15 - row : row;
            int rowBase = srcRow * VgcConstants.SpriteBytesPerRow;

            for (int col = 0; col < 16; col++)
            {
                int srcCol = xFlip ? 15 - col : col;
                int byteIdx = rowBase + srcCol / 2;
                byte color = (srcCol % 2 == 0)
                    ? (byte)((shape[byteIdx] >> 4) & 0x0F)
                    : (byte)(shape[byteIdx] & 0x0F);

                if (color == 0) continue; // transparent

                pixels.Add(new SpritePixel
                {
                    ScreenX = state.x + col,
                    ScreenY = state.y + row,
                    Color = color,
                });
            }
        }

        return pixels;
    }

    public static (ushort spriteToSprite, ushort spriteToBg) DetectCollisions(VirtualGraphicsController vgc)
    {
        // Same algorithm as TUI version — build pixel sets, check overlaps
        // ... (copy from TUI, adapt for multicolor pixel extraction)
    }
}
```

Note on flip flags: the new design uses bit 0 = xFlip, bit 1 = yFlip (was bit 7 and bit 6 in the old system — flags were stored in the XHighFlags byte). Now flags are set via the SFLIP command.

**Step 3: Run tests, verify pass, commit**

```bash
git add e6502.Avalonia/Rendering/SpriteRenderer.cs e6502UnitTests/AvaloniaSpriRendererTests.cs
git commit -m "feat: multicolor sprite renderer with collision detection"
```

---

## Task 7: EmulatorCanvas — The Pixel Rendering Surface

This is the Avalonia custom control that replaces DisplayView.

**Files:**
- Create: `e6502.Avalonia/Rendering/EmulatorCanvas.cs`

**Step 1: Implement EmulatorCanvas**

```csharp
using Avalonia;
using Avalonia.Controls;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Rendering;

public class EmulatorCanvas : Control
{
    private const int NativeWidth = 640;   // 160 * 4 (but text cells are 8px each = 80*8 = 640)
    private const int NativeHeight = 200;  // 50 * 4 (but text rows are 8px each = 25*8 = 200)
    private const double AspectRatio = (double)NativeWidth / NativeHeight; // 3.2

    private readonly WriteableBitmap _framebuffer;
    private readonly VirtualGraphicsController _vgc;
    private readonly BitmapFont _font;
    private readonly object _renderLock = new();
    private bool _cursorVisible = true;

    public EmulatorCanvas(VirtualGraphicsController vgc, BitmapFont font)
    {
        _vgc = vgc;
        _font = font;
        _framebuffer = new WriteableBitmap(
            new PixelSize(NativeWidth, NativeHeight),
            new Vector(96, 96),
            Avalonia.Platform.PixelFormat.Bgra8888,
            AlphaFormat.Opaque);
    }

    public void ToggleCursor()
    {
        lock (_renderLock) { _cursorVisible = !_cursorVisible; }
        InvalidateVisual();
    }

    public void RequestRedraw() => InvalidateVisual();

    public override void Render(DrawingContext context)
    {
        lock (_renderLock)
        {
            RenderFramebuffer();
        }

        // Scale to fit bounds, maintaining aspect ratio, nearest-neighbor
        var bounds = Bounds;
        double scale = Math.Min(bounds.Width / NativeWidth, bounds.Height / NativeHeight);
        double w = NativeWidth * scale;
        double h = NativeHeight * scale;
        double x = (bounds.Width - w) / 2;
        double y = (bounds.Height - h) / 2;

        var destRect = new Rect(x, y, w, h);
        var srcRect = new Rect(0, 0, NativeWidth, NativeHeight);

        using (context.PushRenderOptions(new RenderOptions
        {
            BitmapInterpolationMode = BitmapInterpolationMode.None
        }))
        {
            context.DrawImage(_framebuffer, srcRect, destRect);
        }
    }

    private unsafe void RenderFramebuffer()
    {
        using var fb = _framebuffer.Lock();
        var ptr = (uint*)fb.Address;
        int stride = fb.RowBytes / 4;

        uint bgColor = ColorPalette.GetBgra(_vgc.GetBgColor());

        // Pass 1: Fill background
        for (int i = 0; i < NativeWidth * NativeHeight; i++)
            ptr[i] = bgColor;

        // Pass 2: Priority-0 sprites (behind everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBehindAll);

        // Pass 3: Text layer
        RenderText(ptr, stride, bgColor);

        // Pass 4: Priority-1 sprites (between text and graphics)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBetween);

        // Pass 5: Graphics bitmap
        if (_vgc.GetMode() >= 1)
            RenderGraphics(ptr, stride);

        // Pass 6: Priority-2 sprites (in front of everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriInFront);

        // Collision detection
        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);
        _vgc.SetCollisionRegisters(ss, sb);
    }

    private unsafe void RenderText(uint* ptr, int stride, uint bgColor)
    {
        int cx = _vgc.GetCursorX();
        int cy = _vgc.GetCursorY();

        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = _vgc.GetScreenChar(col, row);
                byte colorIdx = _vgc.GetScreenColor(col, row);

                uint fg = ColorPalette.GetBgra(colorIdx);
                uint bg = bgColor;

                bool isCursor = _cursorVisible && col == cx && row == cy;
                if (isCursor) (fg, bg) = (bg, fg);

                int cellX = col * BitmapFont.GlyphWidth;
                int cellY = row * BitmapFont.GlyphHeight;

                for (int gy = 0; gy < BitmapFont.GlyphHeight; gy++)
                {
                    byte rowBits = _font.GetRow(ch, gy);
                    int py = cellY + gy;
                    for (int gx = 0; gx < BitmapFont.GlyphWidth; gx++)
                    {
                        int px = cellX + gx;
                        bool set = (rowBits & (0x80 >> gx)) != 0;
                        ptr[py * stride + px] = set ? fg : bg;
                    }
                }
            }
        }
    }

    private unsafe void RenderGraphics(uint* ptr, int stride)
    {
        // Each VGC pixel (160x50) maps to 4x4 screen pixels (640x200)
        for (int gy = 0; gy < VgcConstants.GfxHeight; gy++)
        {
            for (int gx = 0; gx < VgcConstants.GfxWidth; gx++)
            {
                byte color = _vgc.GetGfxPixelColor(gx, gy);
                if (color == 0) continue; // transparent

                uint pixel = ColorPalette.GetBgra(color);
                int baseX = gx * 4;
                int baseY = gy * 4;

                for (int dy = 0; dy < 4; dy++)
                    for (int dx = 0; dx < 4; dx++)
                        ptr[(baseY + dy) * stride + (baseX + dx)] = pixel;
            }
        }
    }

    private unsafe void RenderSpritesAtPriority(uint* ptr, int stride, byte priority)
    {
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var state = _vgc.GetSpriteState(i);
            if (!state.enabled || state.priority != priority) continue;

            var pixels = SpriteRenderer.GetSpritePixels(_vgc, i);
            foreach (var sp in pixels)
            {
                // Each sprite pixel = 4x4 screen pixels
                int baseX = sp.ScreenX * 4;
                int baseY = sp.ScreenY * 4;
                uint color = ColorPalette.GetBgra(sp.Color);

                for (int dy = 0; dy < 4; dy++)
                {
                    int py = baseY + dy;
                    if (py < 0 || py >= NativeHeight) continue;
                    for (int dx = 0; dx < 4; dx++)
                    {
                        int px = baseX + dx;
                        if (px < 0 || px >= NativeWidth) continue;
                        ptr[py * stride + px] = color;
                    }
                }
            }
        }
    }
}
```

Note: This uses `unsafe` pointers for performance. Add `<AllowUnsafeBlocks>true</AllowUnsafeBlocks>` to the `.csproj` PropertyGroup.

Also note: The Avalonia `WriteableBitmap.Lock()` API may differ slightly between versions. Check the Avalonia 11 docs — the pattern is `using var fb = bitmap.Lock()` which returns an `ILockedFramebuffer` with `Address` (IntPtr) and `RowBytes` properties.

**Step 2: This task is primarily visual — manual verification**

Run the app and confirm: black window with text rendering. No automated test for rendering correctness (testing pixel output is fragile). The VGC/sprite tests from earlier tasks cover the data layer.

**Step 3: Commit**

```bash
git add e6502.Avalonia/Rendering/EmulatorCanvas.cs
git commit -m "feat: EmulatorCanvas with 6-pass pixel rendering pipeline"
```

---

## Task 8: Input Handling and ScreenEditor

**Files:**
- Create: `e6502.Avalonia/Input/ScreenEditor.cs`

**Step 1: Copy ScreenEditor from TUI, update namespace**

Change namespace to `e6502.Avalonia.Input`. Update `using` to reference `e6502.Avalonia.Hardware`. The class itself is framework-agnostic (uses `ConcurrentQueue`, writes to VGC registers). No changes needed beyond namespace.

**Step 2: Add keyboard handling to MainWindow or EmulatorCanvas**

Add a `KeyDown` handler on `EmulatorCanvas` (or `MainWindow`) that translates Avalonia key events to `ScreenEditor` calls. Same logic as TUI's `DisplayView.OnKeyDown`:

```csharp
// In EmulatorCanvas or MainWindow:
protected override void OnKeyDown(KeyEventArgs e)
{
    switch (e.Key)
    {
        case Key.Left:  _editor.CursorLeft(); e.Handled = true; break;
        case Key.Right: _editor.CursorRight(); e.Handled = true; break;
        case Key.Up:    _editor.CursorUp(); e.Handled = true; break;
        case Key.Down:  _editor.CursorDown(); e.Handled = true; break;
        case Key.Enter: _editor.QueueInput(0x0D); e.Handled = true; break;
        case Key.Back:  _editor.QueueInput(0x08); e.Handled = true; break;
        case Key.Escape: _editor.QueueInput(0x03); e.Handled = true; break;
        default:
            // Printable characters
            if (e.KeySymbol is { Length: 1 } s && s[0] >= 0x20 && s[0] <= 0x7E)
            {
                byte ch = (byte)s[0];
                if (ch == '"') _quoteMode = !_quoteMode;
                if (!_quoteMode && ch >= 0x61 && ch <= 0x7A) ch -= 0x20; // uppercase
                _editor.QueueInput(ch);
                e.Handled = true;
            }
            break;
    }
    base.OnKeyDown(e);
}
```

Note: Avalonia's `KeyEventArgs` has different properties than Terminal.Gui's `Key`. Check `e.Key` for special keys and `e.KeySymbol` (Avalonia 11) or `e.Text` for printable characters. The exact property name may vary — consult Avalonia 11 docs.

**Step 3: Commit**

```bash
git add e6502.Avalonia/Input/
git commit -m "feat: keyboard input handling for Avalonia"
```

---

## Task 9: Wire Up MainWindow — CPU Thread, Timers, Launch

**Files:**
- Modify: `e6502.Avalonia/MainWindow.axaml`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`
- Modify: `e6502.Avalonia/Program.cs` (if needed)

**Step 1: Update MainWindow to wire everything together**

Replace the shell MainWindow with the full wiring — same pattern as `Tui.cs`:

```csharp
using Avalonia.Controls;
using Avalonia.Threading;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;

namespace e6502.Avalonia;

public partial class MainWindow : Window
{
    private readonly CompositeBusDevice _bus;
    private readonly Cpu _cpu;
    private readonly ScreenEditor _editor;
    private readonly EmulatorCanvas _canvas;
    private bool _running = true;

    public MainWindow()
    {
        InitializeComponent();

        _bus = new CompositeBusDevice();
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);

        var fontPath = Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin");
        var font = BitmapFont.LoadFromFile(fontPath);

        _canvas = new EmulatorCanvas(_bus.Vgc, font, _editor);
        Content = _canvas;
        _canvas.Focusable = true;

        // TCP server for MCP
        int tcpPort = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int ep) ? ep : 6502;
        var tcpServer = new Ipc.EmulatorTcpServer(_bus, _editor, _cpu, tcpPort);
        tcpServer.Start();

        // CPU thread
        var cpuThread = new Thread(() =>
        {
            while (_running) _cpu.ExecuteNext();
        }) { IsBackground = true, Name = "CpuThread" };
        cpuThread.Start();

        // 30fps render timer
        DispatcherTimer.Run(() =>
        {
            _bus.Vgc.IncrementFrameCounter();
            _canvas.RequestRedraw();
            return true;
        }, TimeSpan.FromMilliseconds(33));

        // Cursor blink timer
        DispatcherTimer.Run(() =>
        {
            _canvas.ToggleCursor();
            return true;
        }, TimeSpan.FromMilliseconds(500));

        Closing += (_, _) => _running = false;

        // Focus canvas on load for keyboard input
        Opened += (_, _) => _canvas.Focus();
    }
}
```

Update `MainWindow.axaml` to remove the fixed Width/Height (let it size to content or set a reasonable default):
```xml
<Window xmlns="https://github.com/avaloniaui"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        x:Class="e6502.Avalonia.MainWindow"
        Title="e6502 Emulator"
        Background="Black"
        Width="1280" Height="500">
</Window>
```

**Step 2: Manual verification**

Run: `dotnet run --project e6502.Avalonia`

Expected: Window opens, EhBASIC boots, you see the "6502 EhBASIC [C]old/[W]arm ?" prompt rendered with the CP437 font. Keyboard input works. Type `C` for cold start, then BASIC commands.

**Step 3: Commit**

```bash
git add e6502.Avalonia/MainWindow.axaml e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: wire up MainWindow with CPU thread, timers, and canvas"
```

---

## Task 10: Move EmulatorTcpServer and Update for Sprites

**Files:**
- Create: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`

**Step 1: Copy from TUI, update namespace and sprite commands**

Copy `e6502.TUI/Ipc/EmulatorTcpServer.cs` → `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`. Change namespace to `e6502.Avalonia.Ipc`. Update `using` statements to reference `e6502.Avalonia.Hardware` and `e6502.Avalonia.Input`.

Key changes:
- `CmdReadSprites` — update to return multicolor data. Each sprite row is 8 bytes of nibble-packed color. Format the shape as 16 rows of 16 hex-digit strings (one digit per pixel color).
- Add new commands: `sprite_define_row`, `sprite_set_pixel`, `sprite_position`, `sprite_enable`, `sprite_disable`, `sprite_clear`, `sprite_priority`, `sprite_flip`, `sprite_copy`

Each new sprite command writes to the VGC command registers and triggers execution:

```csharp
private string CmdSpriteDefineRow(JsonNode req)
{
    int? sprite = req["sprite"]?.GetValue<int>();
    int? row = req["row"]?.GetValue<int>();
    var data = req["data"]?.AsArray();
    if (sprite is null || row is null || data is null || data.Count != 8)
        return Error("Need sprite, row, and data (8 bytes)");

    _bus.Write(VgcConstants.RegP0, (byte)sprite);
    _bus.Write(VgcConstants.RegP1, (byte)row);
    for (int i = 0; i < 8; i++)
        _bus.Write((ushort)(VgcConstants.RegP2 + i), (byte)data[i]!.GetValue<int>());
    _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);
    return Ok();
}
```

Follow the same pattern for all sprite commands. Add them to the `ProcessRequest` switch.

**Step 2: Commit**

```bash
git add e6502.Avalonia/Ipc/
git commit -m "feat: TCP server with multicolor sprite commands"
```

---

## Task 11: Update MCP Tools for New Sprite Commands

**Files:**
- Modify: `e6502.MCP/EmulatorTools.cs`

**Step 1: Add new MCP tools for sprites**

Add `[McpServerTool]` methods that call the new TCP sprite commands:

```csharp
[McpServerToolType]
public static class EmulatorTools
{
    // ... existing tools unchanged ...

    [McpServerTool, Description("Define a row of pixels for a sprite (16 pixels as 8 nibble-packed bytes)")]
    public static async Task<string> SpriteDefineRow(EmulatorClient client,
        [Description("Sprite index 0-15")] int sprite,
        [Description("Row index 0-15")] int row,
        [Description("8 bytes of pixel data (high nibble=left, low nibble=right)")] int[] data)
    {
        var req = new JsonObject
        {
            ["command"] = "sprite_define_row",
            ["sprite"] = sprite,
            ["row"] = row,
            ["data"] = new JsonArray(data.Select(d => JsonValue.Create(d)).ToArray())
        };
        var resp = await client.SendAsync(req);
        return resp?["ok"]?.GetValue<bool>() == true ? "Row defined" : "Failed";
    }

    // ... similar for sprite_enable, sprite_disable, sprite_position,
    //     sprite_clear, sprite_priority, sprite_flip, sprite_copy ...
}
```

**Step 2: Commit**

```bash
git add e6502.MCP/EmulatorTools.cs
git commit -m "feat: MCP tools for multicolor sprite commands"
```

---

## Task 12: Initialize Vector Table at Boot

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`

**Step 1: Write vector table addresses into RAM during construction**

After loading ROM, write the address vector table at `$0200`:

```csharp
// In CompositeBusDevice constructor, after ROM load:

// Initialize vector table at $0200
void WriteWord(int addr, int value)
{
    _ram[addr]     = (byte)(value & 0xFF);
    _ram[addr + 1] = (byte)((value >> 8) & 0xFF);
}

WriteWord(VgcConstants.VectorTableBase + 0x00, VgcConstants.VgcBase);      // VGC_BASE
WriteWord(VgcConstants.VectorTableBase + 0x02, VgcConstants.RegCmd);       // CMD_REG
WriteWord(VgcConstants.VectorTableBase + 0x04, VgcConstants.CharRamBase);  // SCREEN_RAM
WriteWord(VgcConstants.VectorTableBase + 0x06, VgcConstants.ColorRamBase); // COLOR_RAM
WriteWord(VgcConstants.VectorTableBase + 0x08, VgcConstants.VscBase);      // VSC_BASE
WriteWord(VgcConstants.VectorTableBase + 0x0A, VgcConstants.FioBase);      // FIO_BASE
```

The jump table entries at `$0220+` are deferred — they require actual 6502 machine code routines, which is a separate EhBASIC handler task.

**Step 2: Write a test**

```csharp
[TestMethod]
public void VectorTable_Initialized_AtBoot()
{
    var bus = new CompositeBusDevice();
    int vgcBase = bus.Read(0x0200) | (bus.Read(0x0201) << 8);
    Assert.AreEqual(VgcConstants.VgcBase, vgcBase);

    int cmdReg = bus.Read(0x0202) | (bus.Read(0x0203) << 8);
    Assert.AreEqual(VgcConstants.RegCmd, cmdReg);
}
```

**Step 3: Run tests, verify pass, commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/
git commit -m "feat: initialize vector table at boot"
```

---

## Task 13: Delete e6502.TUI and Clean Up

**Files:**
- Delete: `e6502.TUI/` (entire directory)
- Modify: `e6502.sln` (remove TUI project)
- Modify: `e6502UnitTests/e6502UnitTests.csproj` (remove TUI reference, update ehbasic.bin path)
- Modify or delete: old TUI test files that reference `e6502.TUI` types

**Step 1: Remove TUI project from solution**

Run: `dotnet sln e6502.sln remove e6502.TUI/e6502.TUI.csproj`

**Step 2: Delete TUI directory**

Run: `rm -rf e6502.TUI`

**Step 3: Update test project**

Remove the TUI project reference from `e6502UnitTests/e6502UnitTests.csproj`. Update the ehbasic.bin path to point to the Avalonia project:

```xml
<None Update="..\e6502.Avalonia\Resources\ehbasic.bin">
    <Link>Resources\ehbasic.bin</Link>
    <CopyToOutputDirectory>Always</CopyToOutputDirectory>
</None>
```

**Step 4: Update or remove old test files**

The following test files reference `e6502.TUI` types and need to be either:
- **Deleted** (if superseded by new Avalonia tests): `VgcTests.cs`, `CompositeBusTests.cs`, `IntegrationTests.cs`, `SpriteRendererTests.cs`, `ScreenEditorTests.cs`, `BlockGraphicsTests.cs`, `VgcCharOutTests.cs`, `VgcConstantsTests.cs`
- **Updated** to reference `e6502.Avalonia` types

The safest approach: delete the old TUI-specific test files and ensure the new Avalonia test files (`AvaloniaVgcTests.cs`, `AvaloniaCompositeBusTests.cs`, etc.) cover all the same ground. The CPU tests (`e6502Tests.cs`) don't reference TUI and stay untouched.

**Step 5: Verify everything builds and tests pass**

Run: `dotnet build`
Expected: BUILD SUCCEEDED

Run: `dotnet test`
Expected: ALL PASS

**Step 6: Commit**

```bash
git add -A
git commit -m "chore: remove e6502.TUI, update tests for Avalonia"
```

---

## Task 14: Final Integration Testing

**Step 1: Run the full test suite**

Run: `dotnet test --verbosity normal`
Expected: ALL PASS (CPU tests + new Avalonia hardware/rendering tests)

**Step 2: Manual smoke test**

Run: `dotnet run --project e6502.Avalonia`

Verify:
- [ ] Window opens with black background
- [ ] EhBASIC prompt appears in CP437 font
- [ ] Keyboard input works (type C for cold start)
- [ ] BASIC commands work (PRINT "HELLO")
- [ ] Graphics mode works (set MODE 1, draw commands)
- [ ] Window scales with nearest-neighbor when resized
- [ ] Aspect ratio is maintained

**Step 3: Test MCP integration**

Start the emulator, then in another terminal:
Run: `dotnet run --project e6502.MCP`

Verify MCP tools can read screen, type text, and use new sprite commands.

**Step 4: Commit any final fixes**

```bash
git add -A
git commit -m "fix: integration test fixes"
```

---

## Summary of Commits

1. `feat: create e6502.Avalonia project scaffold`
2. `feat: add VgcConstants with new memory map for Avalonia`
3. `feat: VGC with multicolor sprites and command interface`
4. `feat: composite bus, VSC, and file I/O for Avalonia`
5. `feat: CP437 bitmap font and Avalonia color palette`
6. `feat: multicolor sprite renderer with collision detection`
7. `feat: EmulatorCanvas with 6-pass pixel rendering pipeline`
8. `feat: keyboard input handling for Avalonia`
9. `feat: wire up MainWindow with CPU thread, timers, and canvas`
10. `feat: TCP server with multicolor sprite commands`
11. `feat: MCP tools for multicolor sprite commands`
12. `feat: initialize vector table at boot`
13. `chore: remove e6502.TUI, update tests for Avalonia`
14. `fix: integration test fixes`
