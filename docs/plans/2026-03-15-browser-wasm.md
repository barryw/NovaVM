# Browser WASM Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Run the Nova VM emulator in a web browser via Avalonia.Browser + .NET WASM, with SID audio, demo disk auto-load, and interactive BASIC coding.

**Architecture:** Create `e6502.Browser` project targeting `browser-wasm` with `Avalonia.Browser`. Link shared hardware/rendering source files from `e6502.Avalonia`. Replace `OpenAlRenderer` with a Web Audio API implementation (same class name for drop-in compatibility). Create `BrowserBusDevice` as a browser-adapted version of `CompositeBusDevice` that loads ROMs and the demo disk from embedded resources instead of filesystem.

**Tech Stack:** .NET 10, Avalonia 11.3.12 (Browser backend), Web Audio API (ScriptProcessorNode), JS interop via `[JSImport]`/`[JSExport]`

---

### Task 1: Install WASM workload + scaffold project

**Files:**
- Create: `e6502.Browser/e6502.Browser.csproj`
- Modify: `e6502.sln` (add project)

**Step 1:** Install `wasm-tools` workload
```bash
dotnet workload install wasm-tools
```

**Step 2:** Create project directory and `.csproj`:
```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net10.0</TargetFramework>
    <RuntimeIdentifier>browser-wasm</RuntimeIdentifier>
    <OutputType>Exe</OutputType>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>
    <SkipRomBuild>true</SkipRomBuild>
  </PropertyGroup>

  <ItemGroup>
    <PackageReference Include="Avalonia" Version="11.3.12" />
    <PackageReference Include="Avalonia.Browser" Version="11.3.12" />
    <PackageReference Include="Avalonia.Themes.Fluent" Version="11.3.12" />
    <PackageReference Include="Melanchall.DryWetMidi" Version="8.0.3" />
  </ItemGroup>

  <ItemGroup>
    <ProjectReference Include="..\e6502\e6502.csproj" />
    <ProjectReference Include="..\e6502.Storage\e6502.Storage.csproj" />
  </ItemGroup>

  <!-- Link shared source files from e6502.Avalonia -->
  <!-- Hardware (excluding CompositeBusDevice, OpenAlRenderer, CompilerController, MidiAutoSoundfont) -->
  <ItemGroup>
    <Compile Include="..\e6502.Avalonia\Hardware\VgcConstants.cs" Link="Hardware\VgcConstants.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualGraphicsController.cs" Link="Hardware\VirtualGraphicsController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\SidChip.cs" Link="Hardware\SidChip.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\SidFileParser.cs" Link="Hardware\SidFileParser.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\SidRelocator.cs" Link="Hardware\SidRelocator.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\SidPlayer.cs" Link="Hardware\SidPlayer.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MusicEngine.cs" Link="Hardware\MusicEngine.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MmlParser.cs" Link="Hardware\MmlParser.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MmlTypes.cs" Link="Hardware\MmlTypes.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MidiPlayback.cs" Link="Hardware\MidiPlayback.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MidPlayParser.cs" Link="Hardware\MidPlayParser.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\MidiEngine.cs" Link="Hardware\MidiEngine.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\WavetableSynth.cs" Link="Hardware\WavetableSynth.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\Sf2Loader.cs" Link="Hardware\Sf2Loader.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\SampleBank.cs" Link="Hardware\SampleBank.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\ReverbEffect.cs" Link="Hardware\ReverbEffect.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\ChorusEffect.cs" Link="Hardware\ChorusEffect.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualExpansionMemoryController.cs" Link="Hardware\VirtualExpansionMemoryController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualTimerController.cs" Link="Hardware\VirtualTimerController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualDmaController.cs" Link="Hardware\VirtualDmaController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualBlitterController.cs" Link="Hardware\VirtualBlitterController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\VirtualNetworkController.cs" Link="Hardware\VirtualNetworkController.cs" />
    <Compile Include="..\e6502.Avalonia\Hardware\FileIoController.cs" Link="Hardware\FileIoController.cs" />
  </ItemGroup>

  <!-- Rendering -->
  <ItemGroup>
    <Compile Include="..\e6502.Avalonia\Rendering\EmulatorCanvas.cs" Link="Rendering\EmulatorCanvas.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\BitmapFont.cs" Link="Rendering\BitmapFont.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\ColorPalette.cs" Link="Rendering\ColorPalette.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\SpriteRenderer.cs" Link="Rendering\SpriteRenderer.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\SpriteRenderState.cs" Link="Rendering\SpriteRenderState.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\BlockGraphics.cs" Link="Rendering\BlockGraphics.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\TileRenderState.cs" Link="Rendering\TileRenderState.cs" />
    <Compile Include="..\e6502.Avalonia\Rendering\TileRenderer.cs" Link="Rendering\TileRenderer.cs" />
  </ItemGroup>

  <!-- Input -->
  <ItemGroup>
    <Compile Include="..\e6502.Avalonia\Input\ScreenEditor.cs" Link="Input\ScreenEditor.cs" />
    <Compile Include="..\e6502.Avalonia\Input\FontKeymap.cs" Link="Input\FontKeymap.cs" />
  </ItemGroup>

  <!-- Misc -->
  <ItemGroup>
    <Compile Include="..\e6502.Avalonia\UiTransitionGate.cs" Link="UiTransitionGate.cs" />
  </ItemGroup>

  <!-- Embedded resources -->
  <ItemGroup>
    <EmbeddedResource Include="..\e6502.Avalonia\Resources\ehbasic.bin" LogicalName="ehbasic.bin" />
    <EmbeddedResource Include="..\e6502.Avalonia\Resources\extension.bin" LogicalName="extension.bin" />
    <EmbeddedResource Include="..\e6502.Avalonia\Resources\cp437.bin" LogicalName="cp437.bin" />
    <EmbeddedResource Include="..\e6502.Avalonia\Resources\petscii_upper.bin" LogicalName="petscii_upper.bin" />
    <EmbeddedResource Include="..\e6502.Avalonia\Resources\petscii_lower.bin" LogicalName="petscii_lower.bin" />
    <EmbeddedResource Include="..\docs\programs\demo.ndi" LogicalName="demo.ndi" />
  </ItemGroup>
</Project>
```

**Step 3:** Add project to solution
```bash
dotnet sln e6502.sln add e6502.Browser/e6502.Browser.csproj
```

**Step 4:** Verify it's added
```bash
dotnet sln e6502.sln list
```

---

### Task 2: Add Stream support to NdiImage + NdiFloppyDevice

**Files:**
- Modify: `e6502.Storage/NdiImage.cs`
- Modify: `e6502.Storage/NdiFloppyDevice.cs`

Change `NdiImage._stream` from `FileStream` to `Stream` and add `OpenFromBytes(byte[])` factory.
Add `MountFromBytes(byte[])` to `NdiFloppyDevice`.

---

### Task 3: Create browser-compatible OpenAlRenderer (Web Audio API)

**Files:**
- Create: `e6502.Browser/Hardware/OpenAlRenderer.cs`
- Create: `e6502.Browser/Hardware/MidiAutoSoundfont.cs`
- Create: `e6502.Browser/wwwroot/main.js`

The browser's `OpenAlRenderer` has the same class name and constructor as the desktop version but uses Web Audio API via JS interop. `MidiAutoSoundfont` is a no-op stub (no filesystem for soundfonts).

---

### Task 4: Create BrowserBusDevice

**Files:**
- Create: `e6502.Browser/Hardware/BrowserBusDevice.cs`

Adapted from `CompositeBusDevice` — loads ROMs from embedded resources, mounts demo.ndi as FD0 from embedded resource, omits NCC ROM/compiler, omits help system. Same Read/Write/AdvanceCycles memory map routing.

---

### Task 5: Create Avalonia.Browser app shell

**Files:**
- Create: `e6502.Browser/Program.cs`
- Create: `e6502.Browser/App.axaml`
- Create: `e6502.Browser/App.axaml.cs`
- Create: `e6502.Browser/BrowserMainView.cs`
- Create: `e6502.Browser/wwwroot/index.html`

Avalonia.Browser entry point with `ISingleViewApplicationLifetime`. `BrowserMainView` is the simplified main control: just the EmulatorCanvas with keyboard handling. No help panel, no editors, no dock, no TCP server.

---

### Task 6: Build and verify

```bash
dotnet build e6502.Browser
```

Fix any compilation errors from linked files.

---

### Task 7: Test locally

```bash
dotnet run --project e6502.Browser
```

Opens browser, verify:
- EhBASIC boots and shows Ready prompt
- Keyboard input works
- SID audio plays
- Demo disk loads via LOAD "FD0:AUTOBOOT"
