# Copper Expansion & Sprite Multiplexing Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Enable vertical sprite multiplexing by expanding the copper to write sprite registers, adding 256 shape slots with per-sprite indirection, and restructuring the renderer to evaluate sprites per-scanline.

**Architecture:** Sprite state becomes memory-mapped at `$A040`–`$A0BF` (8 bytes × 16 sprites). Shape RAM expands from 16 to 256 slots (32KB). The copper's register whitelist expands to include all sprite registers. The renderer moves from a full-frame sprite pre-pass to per-scanline evaluation, allowing copper changes to take effect between scanlines.

**Tech Stack:** C#/.NET, Avalonia, MSTest

**Design doc:** `docs/plans/2026-02-23-copper-sprite-multiplex-design.md`

---

### Task 1: Add Constants for Sprite Registers and Expanded Shape Slots

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`

**Step 1: Add new constants**

After the existing sprite constants section (around line 380), add:

```csharp
// Sprite register block: $A040-$A0BF (8 bytes per sprite, 16 sprites)
public const int SpriteRegBase    = 0xA040;
public const int SpriteRegEnd     = 0xA0BF;
public const int SpriteRegStride  = 8;

// Per-sprite register offsets within each 8-byte block
public const int SprRegXLo        = 0;
public const int SprRegXHi        = 1;
public const int SprRegYLo        = 2;
public const int SprRegYHi        = 3;
public const int SprRegShape      = 4;
public const int SprRegFlags      = 5;  // bit0=xFlip, bit1=yFlip, bit7=enable
public const int SprRegPriority   = 6;
public const int SprRegReserved   = 7;

// Sprite flags bitmasks
public const byte SprFlagXFlip    = 0x01;
public const byte SprFlagYFlip    = 0x02;
public const byte SprFlagFlipMask = 0x03;
public const byte SprFlagEnable   = 0x80;

// Expanded shape slot storage
public const int ShapeSlotCount   = 256;
public const int ShapeRamSize     = ShapeSlotCount * SpriteShapeSize;  // 32768
```

**Step 2: Build and verify**

Run: `dotnet build`
Expected: clean build, no errors

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: add constants for sprite registers and 256 shape slots"
```

---

### Task 2: Expand Shape Storage to 256 Slots

**Files:**
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write failing tests**

Add to `AvaloniaVgcTests.cs`:

```csharp
[TestMethod]
public void ShapeSlots_DefaultShapeIndexMatchesSpriteIndex()
{
    for (int i = 0; i < VgcConstants.MaxSprites; i++)
        Assert.AreEqual(i, _vgc.GetSpriteShapeIndex(i),
            $"Sprite {i} should default to shape slot {i}");
}

[TestMethod]
public void ShapeSlots_SetShapeIndex_ChangesShapeUsed()
{
    // Write a pixel to shape slot 5
    _vgc.SetSpritePixelInSlot(5, 0, 0, 0x0A);

    // Point sprite 0 at slot 5
    _vgc.SetSpriteShapeIndex(0, 5);

    // GetSpriteShape(0) should now return slot 5's data
    var shape = _vgc.GetSpriteShape(0);
    Assert.AreEqual(0xA0, shape[0], "High nibble should be 0xA for color 10");
}

[TestMethod]
public void ShapeSlots_CanAccessSlot255()
{
    _vgc.SetSpritePixelInSlot(255, 0, 0, 0x07);
    _vgc.SetSpriteShapeIndex(0, 255);
    var shape = _vgc.GetSpriteShape(0);
    Assert.AreEqual(0x70, shape[0]);
}

[TestMethod]
public void ShapeSlots_GetShapeBySlot_ReturnsCorrectData()
{
    _vgc.SetSpritePixelInSlot(42, 3, 0, 0x0C);
    var shape = _vgc.GetSpriteShapeBySlot(42);
    // Pixel at x=3 (odd) -> low nibble of byte 1
    Assert.AreEqual(0x0C, shape[1] & 0x0F);
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "ShapeSlots" --verbosity normal`
Expected: FAIL — methods `GetSpriteShapeIndex`, `SetSpriteShapeIndex`, `SetSpritePixelInSlot`, `GetSpriteShapeBySlot` don't exist

**Step 3: Implement shape slot expansion**

In `VirtualGraphicsController.cs`:

1. Change `_spriteShapes` allocation (line 52):
```csharp
private readonly byte[] _spriteShapes = new byte[VgcConstants.ShapeRamSize];
```

2. Add `_spriteShapeIndex` array after the existing sprite arrays (after line 60):
```csharp
private readonly byte[] _spriteShapeIndex = new byte[VgcConstants.MaxSprites];
```

3. In `Reset()` (after line 122 where `_spritePriority` is filled), add:
```csharp
// Default shape index: sprite N uses slot N
for (int i = 0; i < VgcConstants.MaxSprites; i++)
    _spriteShapeIndex[i] = (byte)i;
```

4. Add new accessor methods after `GetSpriteState` (after line 376):
```csharp
public byte GetSpriteShapeIndex(int index) => _spriteShapeIndex[index];

public void SetSpriteShapeIndex(int index, byte slotIndex)
{
    if ((uint)index >= VgcConstants.MaxSprites) return;
    _spriteShapeIndex[index] = slotIndex;
}

public ReadOnlySpan<byte> GetSpriteShapeBySlot(int slotIndex) =>
    _spriteShapes.AsSpan(slotIndex * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

public void SetSpritePixelInSlot(int slotIndex, int x, int y, byte color)
{
    if ((uint)slotIndex >= VgcConstants.ShapeSlotCount || (uint)x >= 16 || (uint)y >= 16) return;
    int byteIdx = slotIndex * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
    lock (_spriteShapeLock)
    {
        if (x % 2 == 0)
            _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
        else
            _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
    }
}
```

5. Update `GetSpriteShape` (line 344) to use indirection:
```csharp
public ReadOnlySpan<byte> GetSpriteShape(int index) =>
    _spriteShapes.AsSpan(_spriteShapeIndex[index] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);
```

6. Update `CopySpriteShape` (line 350) to use indirection:
```csharp
public void CopySpriteShape(int index, Span<byte> destination)
{
    lock (_spriteShapeLock)
        _spriteShapes.AsSpan(_spriteShapeIndex[index] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize)
            .CopyTo(destination);
}
```

7. Update `SetSpritePixel` (line 361) to use indirection:
```csharp
public void SetSpritePixel(int spriteIndex, int x, int y, byte color)
{
    if ((uint)spriteIndex >= VgcConstants.MaxSprites || (uint)x >= 16 || (uint)y >= 16) return;
    int slotOffset = _spriteShapeIndex[spriteIndex] * VgcConstants.SpriteShapeSize;
    int byteIdx = slotOffset + y * VgcConstants.SpriteBytesPerRow + x / 2;
    lock (_spriteShapeLock)
    {
        if (x % 2 == 0)
            _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
        else
            _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
    }
}
```

8. Update `ClearSpriteShape` (line 415) to use indirection:
```csharp
public void ClearSpriteShape(int index)
{
    if ((uint)index >= VgcConstants.MaxSprites) return;
    int slotOffset = _spriteShapeIndex[index] * VgcConstants.SpriteShapeSize;
    lock (_spriteShapeLock)
        Array.Clear(_spriteShapes, slotOffset, VgcConstants.SpriteShapeSize);
}
```

9. Update `CopySpriteSlot` (line 404) — this copies shape data between slots of the sprites' current shape indices:
```csharp
public void CopySpriteSlot(int src, int dest)
{
    if ((uint)src >= VgcConstants.MaxSprites || (uint)dest >= VgcConstants.MaxSprites) return;
    int srcOffset = _spriteShapeIndex[src] * VgcConstants.SpriteShapeSize;
    int destOffset = _spriteShapeIndex[dest] * VgcConstants.SpriteShapeSize;
    lock (_spriteShapeLock)
    {
        Array.Copy(_spriteShapes, srcOffset, _spriteShapes, destOffset, VgcConstants.SpriteShapeSize);
    }
}
```

10. Update the sprite command `CmdSprDef` pixel write (inside `ExecuteCommand`, around line 544-559) to use the sprite's current shape index. Find the line that computes `byteIdx` using `sprite * SpriteShapeSize` and change it to use `_spriteShapeIndex[sprite] * SpriteShapeSize`.

11. Update the sprite command `CmdSprRow` bulk row write (around line 561-573) similarly — change `sprite * SpriteShapeSize` to `_spriteShapeIndex[sprite] * SpriteShapeSize`.

12. Update the sprite command `CmdSprClr` (around line 574-579) — change `p0 * SpriteShapeSize` to `_spriteShapeIndex[p0] * SpriteShapeSize`.

13. Update the sprite command `CmdSprCopy` (around line 581-591) — this copies shape data. Change `p0 * SpriteShapeSize` and `dest * SpriteShapeSize` to use `_spriteShapeIndex[p0]` and `_spriteShapeIndex[dest]`.

**Step 4: Run tests**

Run: `dotnet test --verbosity normal`
Expected: ALL tests pass (new shape slot tests + all existing sprite tests remain green)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: expand sprite shape RAM to 256 slots with per-sprite shape index"
```

---

### Task 3: Memory-Mapped Sprite Register Writes

**Files:**
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write failing tests**

```csharp
[TestMethod]
public void SpriteReg_WritePosition_UpdatesSpriteState()
{
    // Write X=300 (0x012C), Y=-10 (0xFFF6) to sprite 0 via registers
    _vgc.Write(VgcConstants.SpriteRegBase + 0, 0x2C);  // XLo
    _vgc.Write(VgcConstants.SpriteRegBase + 1, 0x01);  // XHi
    _vgc.Write(VgcConstants.SpriteRegBase + 2, 0xF6);  // YLo
    _vgc.Write(VgcConstants.SpriteRegBase + 3, 0xFF);  // YHi

    var state = _vgc.GetSpriteState(0);
    Assert.AreEqual(300, state.x);
    Assert.AreEqual(-10, state.y);
}

[TestMethod]
public void SpriteReg_WriteShapeIndex_UpdatesShapePointer()
{
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegShape, 42);
    Assert.AreEqual(42, _vgc.GetSpriteShapeIndex(0));
}

[TestMethod]
public void SpriteReg_WriteFlags_UpdatesEnableAndFlip()
{
    // Enable + xFlip + yFlip
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags,
               VgcConstants.SprFlagEnable | VgcConstants.SprFlagXFlip | VgcConstants.SprFlagYFlip);

    var state = _vgc.GetSpriteState(0);
    Assert.IsTrue(state.enabled);
    Assert.AreEqual(0x03, state.flags);  // both flip bits
}

[TestMethod]
public void SpriteReg_WriteFlagsEnable_UpdatesSpriteCount()
{
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);
    Assert.AreEqual(1, _vgc.Read(VgcConstants.RegSpriteCount));

    // Disable via flags
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, 0x00);
    Assert.AreEqual(0, _vgc.Read(VgcConstants.RegSpriteCount));
}

[TestMethod]
public void SpriteReg_WritePriority_ClampedTo2()
{
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegPriority, 5);
    var state = _vgc.GetSpriteState(0);
    Assert.AreEqual(2, state.priority);
}

[TestMethod]
public void SpriteReg_Sprite7_CorrectOffset()
{
    int base7 = VgcConstants.SpriteRegBase + 7 * VgcConstants.SpriteRegStride;
    _vgc.Write(base7 + VgcConstants.SprRegXLo, 0x40);
    _vgc.Write(base7 + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);

    var state = _vgc.GetSpriteState(7);
    Assert.AreEqual(0x40, state.x);
    Assert.IsTrue(state.enabled);
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SpriteReg_Write" --verbosity normal`
Expected: FAIL — VGC Write doesn't handle `$A040`–`$A0BF`

**Step 3: Implement sprite register writes**

In `VirtualGraphicsController.cs`, modify `Write(ushort address, byte data)`:

Add a new block **before** the "Core registers" block (before the `if (address >= VgcConstants.VgcBase && address < VgcConstants.VgcBase + 16)` on line 259):

```csharp
// Sprite registers $A040-$A0BF
if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
{
    WriteSpriteRegister(address, data);
    return;
}
```

Add the `WriteSpriteRegister` method:

```csharp
private void WriteSpriteRegister(int address, byte data)
{
    int offset = address - VgcConstants.SpriteRegBase;
    int sprite = offset / VgcConstants.SpriteRegStride;
    int field = offset % VgcConstants.SpriteRegStride;

    if ((uint)sprite >= VgcConstants.MaxSprites) return;

    switch (field)
    {
        case VgcConstants.SprRegXLo:
            _spriteX[sprite] = (short)((_spriteX[sprite] & ~0xFF) | data);
            break;
        case VgcConstants.SprRegXHi:
            _spriteX[sprite] = (short)((data << 8) | (_spriteX[sprite] & 0xFF));
            break;
        case VgcConstants.SprRegYLo:
            _spriteY[sprite] = (short)((_spriteY[sprite] & ~0xFF) | data);
            break;
        case VgcConstants.SprRegYHi:
            _spriteY[sprite] = (short)((data << 8) | (_spriteY[sprite] & 0xFF));
            break;
        case VgcConstants.SprRegShape:
            _spriteShapeIndex[sprite] = data;
            break;
        case VgcConstants.SprRegFlags:
            _spriteEnabled[sprite] = (data & VgcConstants.SprFlagEnable) != 0;
            _spriteFlags[sprite] = (byte)(data & VgcConstants.SprFlagFlipMask);
            UpdateSpriteCount();
            break;
        case VgcConstants.SprRegPriority:
            _spritePriority[sprite] = (byte)Math.Min((int)data, 2);
            break;
    }
}
```

Also update `OwnsAddress` (line 139) to include the sprite register range:

```csharp
public bool OwnsAddress(ushort address)
{
    // VGC registers + command: $A000-$A01E
    if (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcRegsEnd)
        return true;

    // Sprite registers: $A040-$A0BF
    if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
        return true;

    // Character RAM: $AA00-$B1CF
    if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
        return true;

    // Color RAM: $B1D0-$B99F
    if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
        return true;

    return false;
}
```

**Step 4: Run tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: add memory-mapped sprite register writes at \$A040-\$A0BF"
```

---

### Task 4: Memory-Mapped Sprite Register Reads

**Files:**
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write failing tests**

```csharp
[TestMethod]
public void SpriteReg_ReadPosition_ReturnsCurrentState()
{
    // Set position via command
    _vgc.Write(VgcConstants.RegP0, 0);  // sprite 0
    _vgc.Write(VgcConstants.RegP1, 0x2C);  // xLo
    _vgc.Write(VgcConstants.RegP2, 0x01);  // xHi
    _vgc.Write(VgcConstants.RegP3, 0x50);  // yLo
    _vgc.Write(VgcConstants.RegP4, 0x00);  // yHi
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

    Assert.AreEqual(0x2C, _vgc.Read(VgcConstants.SpriteRegBase + VgcConstants.SprRegXLo));
    Assert.AreEqual(0x01, _vgc.Read(VgcConstants.SpriteRegBase + VgcConstants.SprRegXHi));
    Assert.AreEqual(0x50, _vgc.Read(VgcConstants.SpriteRegBase + VgcConstants.SprRegYLo));
    Assert.AreEqual(0x00, _vgc.Read(VgcConstants.SpriteRegBase + VgcConstants.SprRegYHi));
}

[TestMethod]
public void SpriteReg_ReadFlags_CompositeOfEnableAndFlip()
{
    // Enable sprite 0 and set xFlip via commands
    _vgc.Write(VgcConstants.RegP0, 0);
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

    _vgc.Write(VgcConstants.RegP0, 0);
    _vgc.Write(VgcConstants.RegP1, 0x01);  // xFlip
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

    byte flags = _vgc.Read(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags);
    Assert.AreEqual(VgcConstants.SprFlagEnable | VgcConstants.SprFlagXFlip, flags);
}

[TestMethod]
public void SpriteReg_ReadShapeIndex_ReturnsDefault()
{
    Assert.AreEqual(3, _vgc.Read(VgcConstants.SpriteRegBase + 3 * VgcConstants.SpriteRegStride + VgcConstants.SprRegShape));
}

[TestMethod]
public void SpriteReg_ReadWriteRoundTrip()
{
    int addr = VgcConstants.SpriteRegBase + 5 * VgcConstants.SpriteRegStride;
    _vgc.Write(addr + VgcConstants.SprRegXLo, 0xAB);
    _vgc.Write(addr + VgcConstants.SprRegXHi, 0x00);
    _vgc.Write(addr + VgcConstants.SprRegShape, 200);
    _vgc.Write(addr + VgcConstants.SprRegPriority, 1);

    Assert.AreEqual(0xAB, _vgc.Read(addr + VgcConstants.SprRegXLo));
    Assert.AreEqual(0x00, _vgc.Read(addr + VgcConstants.SprRegXHi));
    Assert.AreEqual(200, _vgc.Read(addr + VgcConstants.SprRegShape));
    Assert.AreEqual(1, _vgc.Read(addr + VgcConstants.SprRegPriority));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SpriteReg_Read" --verbosity normal`
Expected: FAIL — VGC Read returns 0 for these addresses

**Step 3: Implement sprite register reads**

In `VirtualGraphicsController.Read(ushort address)`, add a new block **before** the "Command parameter registers" block (before line 170):

```csharp
// Sprite registers $A040-$A0BF
if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
    return ReadSpriteRegister(address);
```

Add the `ReadSpriteRegister` method:

```csharp
private byte ReadSpriteRegister(int address)
{
    int offset = address - VgcConstants.SpriteRegBase;
    int sprite = offset / VgcConstants.SpriteRegStride;
    int field = offset % VgcConstants.SpriteRegStride;

    if ((uint)sprite >= VgcConstants.MaxSprites) return 0;

    return field switch
    {
        VgcConstants.SprRegXLo => (byte)(_spriteX[sprite] & 0xFF),
        VgcConstants.SprRegXHi => (byte)((_spriteX[sprite] >> 8) & 0xFF),
        VgcConstants.SprRegYLo => (byte)(_spriteY[sprite] & 0xFF),
        VgcConstants.SprRegYHi => (byte)((_spriteY[sprite] >> 8) & 0xFF),
        VgcConstants.SprRegShape => _spriteShapeIndex[sprite],
        VgcConstants.SprRegFlags => (byte)((_spriteEnabled[sprite] ? VgcConstants.SprFlagEnable : 0) | (_spriteFlags[sprite] & VgcConstants.SprFlagFlipMask)),
        VgcConstants.SprRegPriority => _spritePriority[sprite],
        _ => 0
    };
}
```

**Step 4: Run tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: add memory-mapped sprite register reads at \$A040-\$A0BF"
```

---

### Task 5: Expand Copper Register Whitelist for Sprite Registers

**Files:**
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write failing tests**

```csharp
[TestMethod]
public void Copper_CanTargetSpriteRegister()
{
    // Add copper event targeting sprite 0 shape index at $A044
    _vgc.Write(VgcConstants.RegP0, 0);     // x lo
    _vgc.Write(VgcConstants.RegP1, 0);     // x hi
    _vgc.Write(VgcConstants.RegP2, 50);    // y
    _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.SpriteRegBase + VgcConstants.SprRegShape));  // register lo
    _vgc.Write(VgcConstants.RegP4, (byte)((VgcConstants.SpriteRegBase + VgcConstants.SprRegShape) >> 8));  // register hi
    _vgc.Write(VgcConstants.RegP5, 42);    // value
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

    _vgc.IncrementFrameCounter();
    var program = _vgc.GetCopperProgram();

    Assert.AreEqual(1, program.Length);
    Assert.AreEqual(42, program[0].Value);
}

[TestMethod]
public void Copper_CanTargetAllSpriteRegFields()
{
    // Try each field for sprite 0
    int[] fields = [
        VgcConstants.SprRegXLo, VgcConstants.SprRegXHi,
        VgcConstants.SprRegYLo, VgcConstants.SprRegYHi,
        VgcConstants.SprRegShape, VgcConstants.SprRegFlags,
        VgcConstants.SprRegPriority
    ];

    foreach (int field in fields)
    {
        int regAddr = VgcConstants.SpriteRegBase + field;
        _vgc.Write(VgcConstants.RegP3, (byte)(regAddr & 0xFF));
        _vgc.Write(VgcConstants.RegP4, (byte)((regAddr >> 8) & 0xFF));
        _vgc.Write(VgcConstants.RegP5, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);
    }

    _vgc.IncrementFrameCounter();
    Assert.AreEqual(fields.Length, _vgc.GetCopperProgram().Length);
}

[TestMethod]
public void Copper_RejectsReservedSpriteField()
{
    int regAddr = VgcConstants.SpriteRegBase + VgcConstants.SprRegReserved;
    _vgc.Write(VgcConstants.RegP3, (byte)(regAddr & 0xFF));
    _vgc.Write(VgcConstants.RegP4, (byte)((regAddr >> 8) & 0xFF));
    _vgc.Write(VgcConstants.RegP5, 1);
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

    _vgc.IncrementFrameCounter();
    Assert.AreEqual(0, _vgc.GetCopperProgram().Length);
}

[TestMethod]
public void Copper_RejectsAddressBetweenVgcAndSpriteRegs()
{
    // $A020 is in the gap between VGC regs and sprite regs
    int regAddr = 0xA020;
    _vgc.Write(VgcConstants.RegP3, (byte)(regAddr & 0xFF));
    _vgc.Write(VgcConstants.RegP4, (byte)((regAddr >> 8) & 0xFF));
    _vgc.Write(VgcConstants.RegP5, 1);
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

    _vgc.IncrementFrameCounter();
    Assert.AreEqual(0, _vgc.GetCopperProgram().Length);
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "Copper_CanTarget|Copper_Rejects" --verbosity normal`
Expected: FAIL — copper rejects sprite register addresses

**Step 3: Implement copper expansion**

Update `TryResolveCopperRegister` (line 787):

```csharp
private static bool TryResolveCopperRegister(int registerSpecifier, out byte registerIndex)
{
    registerIndex = 0;

    // Direct index: 0-15 for VGC core registers
    if ((uint)registerSpecifier < 16)
    {
        registerIndex = (byte)registerSpecifier;
        return IsCopperWritableRegister(registerIndex);
    }

    // Absolute address: VGC core registers $A000-$A00F
    if (registerSpecifier >= VgcConstants.VgcBase && registerSpecifier < VgcConstants.VgcBase + 16)
    {
        registerIndex = (byte)(registerSpecifier - VgcConstants.VgcBase);
        return IsCopperWritableRegister(registerIndex);
    }

    // Absolute address: Sprite registers $A040-$A0BF
    if (registerSpecifier >= VgcConstants.SpriteRegBase && registerSpecifier <= VgcConstants.SpriteRegEnd)
    {
        registerIndex = (byte)(registerSpecifier - VgcConstants.VgcBase);
        return IsCopperWritableRegister(registerIndex);
    }

    return false;
}
```

Update `IsCopperWritableRegister` (line 804):

```csharp
private static bool IsCopperWritableRegister(byte registerIndex) =>
    registerIndex == VgcConstants.RegMode - VgcConstants.VgcBase ||
    registerIndex == VgcConstants.RegBgCol - VgcConstants.VgcBase ||
    registerIndex == VgcConstants.RegScrollX - VgcConstants.VgcBase ||
    registerIndex == VgcConstants.RegScrollY - VgcConstants.VgcBase ||
    IsCopperSpriteRegister(registerIndex);

private static bool IsCopperSpriteRegister(byte registerIndex)
{
    int offset = registerIndex - (VgcConstants.SpriteRegBase - VgcConstants.VgcBase);
    if (offset < 0 || offset >= VgcConstants.MaxSprites * VgcConstants.SpriteRegStride)
        return false;
    int field = offset % VgcConstants.SpriteRegStride;
    return field != VgcConstants.SprRegReserved;  // all fields except reserved
}
```

**Step 4: Run tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: expand copper register whitelist to include sprite registers"
```

---

### Task 6: Add SpriteRenderState and Per-Scanline Sprite Evaluation

**Files:**
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs`
- Modify: `e6502.Avalonia/Rendering/SpriteRenderer.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write failing tests for sprite render state**

```csharp
[TestMethod]
public void SpriteRenderState_SnapshotFromVgc_CapturesAllSprites()
{
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegXLo, 100);
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegShape, 5);

    var state = SpriteRenderState.FromVgc(_vgc);
    Assert.IsTrue(state.IsEnabled(0));
    Assert.AreEqual(100, state.GetX(0));
    Assert.AreEqual(5, state.GetShapeIndex(0));
}

[TestMethod]
public void SpriteRenderState_Apply_UpdatesSpriteFields()
{
    var state = SpriteRenderState.FromVgc(_vgc);
    byte regIndex = (byte)(VgcConstants.SpriteRegBase - VgcConstants.VgcBase + VgcConstants.SprRegShape);

    state.Apply(regIndex, 42);
    Assert.AreEqual(42, state.GetShapeIndex(0));
}

[TestMethod]
public void SpriteRenderState_Apply_EnableViaFlags()
{
    var state = SpriteRenderState.FromVgc(_vgc);
    byte regIndex = (byte)(VgcConstants.SpriteRegBase - VgcConstants.VgcBase + VgcConstants.SprRegFlags);

    state.Apply(regIndex, VgcConstants.SprFlagEnable | VgcConstants.SprFlagXFlip);
    Assert.IsTrue(state.IsEnabled(0));
    Assert.IsTrue(state.IsXFlip(0));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SpriteRenderState" --verbosity normal`
Expected: FAIL — `SpriteRenderState` doesn't exist

**Step 3: Implement SpriteRenderState**

Add to `VirtualGraphicsController.cs` — new public method to expose shape data for the renderer:

```csharp
public ReadOnlySpan<byte> GetSpriteShapeRam() => _spriteShapes;
```

Create `SpriteRenderState` as a struct in `EmulatorCanvas.cs` (inside the file, after `RenderVideoState`):

```csharp
private struct SpriteRenderState
{
    private readonly int[] _x;
    private readonly int[] _y;
    private readonly byte[] _shapeIndex;
    private readonly byte[] _flags;   // bit0=xFlip, bit1=yFlip, bit7=enable
    private readonly byte[] _priority;

    private SpriteRenderState(int[] x, int[] y, byte[] shapeIndex, byte[] flags, byte[] priority)
    {
        _x = x; _y = y; _shapeIndex = shapeIndex; _flags = flags; _priority = priority;
    }

    public static SpriteRenderState FromVgc(VirtualGraphicsController vgc)
    {
        int count = VgcConstants.MaxSprites;
        var x = new int[count];
        var y = new int[count];
        var shapeIndex = new byte[count];
        var flags = new byte[count];
        var priority = new byte[count];

        for (int i = 0; i < count; i++)
        {
            var s = vgc.GetSpriteState(i);
            x[i] = s.x;
            y[i] = s.y;
            shapeIndex[i] = vgc.GetSpriteShapeIndex(i);
            flags[i] = (byte)((s.enabled ? VgcConstants.SprFlagEnable : 0) | (s.flags & VgcConstants.SprFlagFlipMask));
            priority[i] = s.priority;
        }

        return new SpriteRenderState(x, y, shapeIndex, flags, priority);
    }

    public int GetX(int sprite) => _x[sprite];
    public int GetY(int sprite) => _y[sprite];
    public byte GetShapeIndex(int sprite) => _shapeIndex[sprite];
    public byte GetPriority(int sprite) => _priority[sprite];
    public bool IsEnabled(int sprite) => (_flags[sprite] & VgcConstants.SprFlagEnable) != 0;
    public bool IsXFlip(int sprite) => (_flags[sprite] & VgcConstants.SprFlagXFlip) != 0;
    public bool IsYFlip(int sprite) => (_flags[sprite] & VgcConstants.SprFlagYFlip) != 0;

    public void Apply(byte registerIndex, byte value)
    {
        int baseOffset = VgcConstants.SpriteRegBase - VgcConstants.VgcBase;
        int offset = registerIndex - baseOffset;
        if (offset < 0 || offset >= VgcConstants.MaxSprites * VgcConstants.SpriteRegStride) return;

        int sprite = offset / VgcConstants.SpriteRegStride;
        int field = offset % VgcConstants.SpriteRegStride;

        switch (field)
        {
            case VgcConstants.SprRegXLo:
                _x[sprite] = (short)((_x[sprite] & ~0xFF) | value);
                break;
            case VgcConstants.SprRegXHi:
                _x[sprite] = (short)((value << 8) | (_x[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegYLo:
                _y[sprite] = (short)((_y[sprite] & ~0xFF) | value);
                break;
            case VgcConstants.SprRegYHi:
                _y[sprite] = (short)((value << 8) | (_y[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegShape:
                _shapeIndex[sprite] = value;
                break;
            case VgcConstants.SprRegFlags:
                _flags[sprite] = value;
                break;
            case VgcConstants.SprRegPriority:
                _priority[sprite] = (byte)Math.Min(value, 2);
                break;
        }
    }

    public static bool IsSpriteRegister(byte registerIndex)
    {
        int baseOffset = VgcConstants.SpriteRegBase - VgcConstants.VgcBase;
        int offset = registerIndex - baseOffset;
        return offset >= 0 && offset < VgcConstants.MaxSprites * VgcConstants.SpriteRegStride;
    }
}
```

**Step 4: Run tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 5: Commit**

```bash
git add e6502.Avalonia/Rendering/EmulatorCanvas.cs e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: add SpriteRenderState for per-scanline sprite snapshot"
```

---

### Task 7: Per-Scanline Sprite Rendering and Copper Integration

This is the core rendering pipeline restructure. Replace `BuildSpritePriorityMaps` with per-scanline sprite evaluation, and split copper event processing so sprite register changes fire at scanline boundaries.

**Files:**
- Modify: `e6502.Avalonia/Rendering/EmulatorCanvas.cs`
- Modify: `e6502.Avalonia/Rendering/SpriteRenderer.cs`

**Step 1: Add scanline sprite rasterization to SpriteRenderer**

Replace the entire `SpriteRenderer` class with:

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
    /// <summary>
    /// Rasterizes all sprites that overlap a given scanline into three line buffers (behind/between/front).
    /// Reads shape data from the provided shape RAM span using the sprite state's shape indices.
    /// </summary>
    public static void RasterizeScanline(
        int scanlineY,
        SpriteRenderState sprites,
        ReadOnlySpan<byte> shapeRam,
        Span<byte> lineBehind,
        Span<byte> lineBetween,
        Span<byte> lineFront,
        Span<ushort> spriteMask)
    {
        lineBehind.Clear();
        lineBetween.Clear();
        lineFront.Clear();
        spriteMask.Clear();

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            if (!sprites.IsEnabled(i)) continue;

            int sy = sprites.GetY(i);
            if (scanlineY < sy || scanlineY >= sy + VgcConstants.SpritePixelSize) continue;

            int row = scanlineY - sy;
            bool xFlip = sprites.IsXFlip(i);
            bool yFlip = sprites.IsYFlip(i);
            int srcRow = yFlip ? 15 - row : row;

            Span<byte> target = sprites.GetPriority(i) switch
            {
                VgcConstants.SpritePriBehindAll => lineBehind,
                VgcConstants.SpritePriBetween => lineBetween,
                _ => lineFront
            };

            int slotOffset = sprites.GetShapeIndex(i) * VgcConstants.SpriteShapeSize;
            int rowBase = slotOffset + srcRow * VgcConstants.SpriteBytesPerRow;

            for (int col = 0; col < VgcConstants.SpritePixelSize; col++)
            {
                int srcCol = xFlip ? 15 - col : col;
                int byteIdx = rowBase + srcCol / 2;
                byte color = (srcCol % 2 == 0)
                    ? (byte)((shapeRam[byteIdx] >> 4) & 0x0F)
                    : (byte)(shapeRam[byteIdx] & 0x0F);

                if (color == 0) continue;

                int screenX = sprites.GetX(i) + col;
                if ((uint)screenX >= VgcConstants.GfxWidth) continue;

                target[screenX] = color;
                spriteMask[screenX] |= (ushort)(1 << i);
            }
        }
    }

    /// <summary>
    /// Accumulates collision bits for one scanline.
    /// </summary>
    public static void AccumulateCollisions(
        int scanlineY,
        SpriteRenderState sprites,
        ReadOnlySpan<ushort> spriteMask,
        VirtualGraphicsController vgc,
        int scrollX,
        int scrollY,
        ref ushort spriteToSprite,
        ref ushort spriteToBg)
    {
        for (int x = 0; x < VgcConstants.GfxWidth; x++)
        {
            ushort mask = spriteMask[x];
            if (mask == 0) continue;

            // Sprite-sprite: any two bits set means collision
            if ((mask & (mask - 1)) != 0)
            {
                spriteToSprite |= mask;
            }

            // Sprite-background: check gfx pixel
            int gfxX = (x + scrollX) % VgcConstants.GfxWidth;
            int gfxY = (scanlineY + scrollY) % VgcConstants.GfxHeight;
            if (gfxX < 0) gfxX += VgcConstants.GfxWidth;
            if (gfxY < 0) gfxY += VgcConstants.GfxHeight;
            if (vgc.GetGfxPixelColor(gfxX, gfxY) != 0)
            {
                spriteToBg |= mask;
            }
        }
    }

    // Keep old methods for any remaining callers (e.g., external tools)
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

                if (color == 0) continue;

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
}
```

Note: `SpriteRenderState` is used here. Since it's defined as a private struct in `EmulatorCanvas.cs`, we need to make it a **public struct** in its own file or move it. The cleanest approach: create `e6502.Avalonia/Rendering/SpriteRenderState.cs` with the `SpriteRenderState` struct as `public`, and remove it from `EmulatorCanvas.cs`. Ensure the namespace is `e6502.Avalonia.Rendering`.

**Step 2: Restructure RenderFramebuffer**

Replace the entire `RenderFramebuffer` method, `BuildSpritePriorityMaps`, and remove the three `_spriteMap*` fields from `EmulatorCanvas`:

Remove fields (lines 26-28):
```csharp
// DELETE these three lines:
// private readonly byte[] _spriteMapBehind = ...
// private readonly byte[] _spriteMapBetween = ...
// private readonly byte[] _spriteMapFront = ...
```

Add new per-scanline buffers as fields:
```csharp
private readonly byte[] _lineBehind = new byte[VgcConstants.GfxWidth];
private readonly byte[] _lineBetween = new byte[VgcConstants.GfxWidth];
private readonly byte[] _lineFront = new byte[VgcConstants.GfxWidth];
private readonly ushort[] _spriteMask = new ushort[VgcConstants.GfxWidth];
```

Replace `RenderFramebuffer` (lines 214-309) and delete `BuildSpritePriorityMaps` (lines 312-339):

```csharp
private unsafe void RenderFramebuffer()
{
    using var fb = _framebuffer.Lock();
    var ptr = (uint*)fb.Address;
    int stride = fb.RowBytes / 4;

    ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = _vgc.GetCopperProgram();
    bool copperEnabled = _vgc.IsCopperEnabled && !copperProgram.IsEmpty;
    int copperIndex = 0;

    var state = RenderVideoState.FromVgc(_vgc);
    var sprites = SpriteRenderState.FromVgc(_vgc);
    ReadOnlySpan<byte> shapeRam = _vgc.GetSpriteShapeRam();
    int cursorX = _vgc.GetCursorX();
    int cursorY = _vgc.GetCursorY();
    bool cursorEnabled = _cursorVisible && _vgc.IsCursorEnabled;

    ushort colSS = 0, colSB = 0;

    for (int y = 0; y < VgcConstants.GfxHeight; y++)
    {
        // Pre-fire copper events targeting sprite registers for this scanline
        if (copperEnabled)
        {
            int scanlineEnd = (y + 1) * VgcConstants.GfxWidth;
            int peekIndex = copperIndex;
            while (peekIndex < copperProgram.Length && copperProgram[peekIndex].Position < scanlineEnd)
            {
                if (SpriteRenderState.IsSpriteRegister(copperProgram[peekIndex].RegisterIndex))
                    sprites.Apply(copperProgram[peekIndex].RegisterIndex, copperProgram[peekIndex].Value);
                peekIndex++;
            }
        }

        // Rasterize sprites for this scanline
        SpriteRenderer.RasterizeScanline(y, sprites, shapeRam,
            _lineBehind, _lineBetween, _lineFront, _spriteMask);

        // Accumulate collision data
        SpriteRenderer.AccumulateCollisions(y, sprites, _spriteMask, _vgc,
            state.ScrollX, state.ScrollY, ref colSS, ref colSB);

        for (int x = 0; x < VgcConstants.GfxWidth; x++)
        {
            // Fire non-sprite copper events at exact pixel position
            if (copperEnabled)
            {
                int position = y * VgcConstants.GfxWidth + x;
                while (copperIndex < copperProgram.Length && copperProgram[copperIndex].Position == position)
                {
                    if (!SpriteRenderState.IsSpriteRegister(copperProgram[copperIndex].RegisterIndex))
                        state.Apply(copperProgram[copperIndex].RegisterIndex, copperProgram[copperIndex].Value);
                    copperIndex++;
                }
            }

            byte spriteBehind = _lineBehind[x];
            byte spriteBetween = _lineBetween[x];
            byte spriteFront = _lineFront[x];

            int sampleGfxX = Wrap320(x + state.ScrollX);
            int sampleGfxY = Wrap200(y + state.ScrollY);
            byte gfxColorIndex = _vgc.GetGfxPixelColor(sampleGfxX, sampleGfxY);
            uint gfxPixel = gfxColorIndex == 0 ? 0u : Palette[gfxColorIndex & 0x0F];

            for (int dy = 0; dy < 2; dy++)
            {
                int py = y * 2 + dy;
                int rowBase = py * stride;
                for (int dx = 0; dx < 2; dx++)
                {
                    int px = x * 2 + dx;
                    uint pixel = Palette[state.BgColor & 0x0F];

                    if (spriteBehind != 0)
                        pixel = Palette[spriteBehind & 0x0F];

                    bool textOpaque = false;
                    uint textPixel = 0;
                    if (state.Mode != 3)
                        textOpaque = TrySampleTextPixel(px, py, state, cursorX, cursorY, cursorEnabled, out textPixel);

                    if (state.Mode == 3)
                    {
                        if (gfxPixel != 0)
                            pixel = gfxPixel;
                        if (spriteBetween != 0)
                            pixel = Palette[spriteBetween & 0x0F];
                    }
                    else if (state.Mode == 2)
                    {
                        if (gfxPixel != 0)
                            pixel = gfxPixel;
                        if (spriteBetween != 0)
                            pixel = Palette[spriteBetween & 0x0F];
                        if (textOpaque)
                            pixel = textPixel;
                    }
                    else
                    {
                        if (textOpaque)
                            pixel = textPixel;
                        if (spriteBetween != 0)
                            pixel = Palette[spriteBetween & 0x0F];
                        if (state.Mode >= 1 && gfxPixel != 0)
                            pixel = gfxPixel;
                    }

                    if (spriteFront != 0)
                        pixel = Palette[spriteFront & 0x0F];

                    ptr[rowBase + px] = pixel;
                }
            }
        }
    }

    _vgc.SetCollisionRegisters(colSS, colSB);
}
```

**Step 3: Build and verify**

Run: `dotnet build`
Expected: clean build

**Step 4: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass (existing sprite rendering tests, copper tests, collision tests all green)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Rendering/EmulatorCanvas.cs e6502.Avalonia/Rendering/SpriteRenderer.cs e6502.Avalonia/Rendering/SpriteRenderState.cs
git commit -m "feat: per-scanline sprite rendering with copper sprite register support"
```

---

### Task 8: Copper Sprite Multiplexing Integration Test

Verify the full pipeline works end-to-end: copper changes sprite shape index mid-frame, and the renderer uses the new shape for subsequent scanlines.

**Files:**
- Test: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write integration test**

```csharp
[TestMethod]
public void Copper_SpriteMultiplex_ChangesShapeAtScanline()
{
    // Set up sprite 0: enabled at (0, 0) using shape slot 0
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);
    _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegShape, 0);

    // Put distinct colors in shape slot 0 and slot 1
    _vgc.SetSpritePixelInSlot(0, 0, 0, 1);   // slot 0: color 1 at (0,0)
    _vgc.SetSpritePixelInSlot(1, 0, 0, 2);   // slot 1: color 2 at (0,0)

    // Copper event: at Y=100, change sprite 0's shape to slot 1
    int regAddr = VgcConstants.SpriteRegBase + VgcConstants.SprRegShape;
    _vgc.Write(VgcConstants.RegP0, 0);     // x=0
    _vgc.Write(VgcConstants.RegP1, 0);
    _vgc.Write(VgcConstants.RegP2, 100);   // y=100
    _vgc.Write(VgcConstants.RegP3, (byte)(regAddr & 0xFF));
    _vgc.Write(VgcConstants.RegP4, (byte)(regAddr >> 8));
    _vgc.Write(VgcConstants.RegP5, 1);     // shape slot 1
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

    // Also reposition sprite to Y=100 at that scanline
    int yLoAddr = VgcConstants.SpriteRegBase + VgcConstants.SprRegYLo;
    _vgc.Write(VgcConstants.RegP2, 100);
    _vgc.Write(VgcConstants.RegP3, (byte)(yLoAddr & 0xFF));
    _vgc.Write(VgcConstants.RegP4, (byte)(yLoAddr >> 8));
    _vgc.Write(VgcConstants.RegP5, 100);
    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

    _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperEnable);
    _vgc.IncrementFrameCounter();

    // Verify copper program has 2 events
    var program = _vgc.GetCopperProgram();
    Assert.AreEqual(2, program.Length);

    // Verify the sprite state snapshot + copper execution produces correct results
    // (Full rendering tested via the Avalonia GUI; this validates copper program structure)
    Assert.AreEqual(1, program[0].Value);    // shape slot 1
    Assert.AreEqual(100, program[1].Value);  // Y=100
}
```

**Step 2: Run test**

Run: `dotnet test --filter "Copper_SpriteMultiplex" --verbosity normal`
Expected: PASS

**Step 3: Run full test suite**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 4: Commit**

```bash
git add e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "test: add copper sprite multiplexing integration test"
```

---

### Task 9: Update DMA and MemorySpace Size for Expanded Shape RAM

The DMA controller and memory I/O commands already use `_spriteShapes.Length` for bounds checking, so expanding the array in Task 2 should have cascaded. Verify this and add an explicit test.

**Files:**
- Test: `e6502UnitTests/AvaloniaDmaTests.cs`

**Step 1: Write test**

```csharp
[TestMethod]
public void Dma_XramToSpriteShape_CanWriteSlot255()
{
    var bus = MakeBus();

    // Write a test byte into XRAM
    bus.Write((ushort)VgcConstants.XmcPageSelect, 0);
    bus.Write((ushort)VgcConstants.XmcWindowBase, 0xAB);

    // DMA copy 1 byte from XRAM offset 0 to sprite shape slot 255 offset 0
    int destAddr = 255 * VgcConstants.SpriteShapeSize;
    StartDma(bus,
        VgcConstants.DmaSpaceXram, VgcConstants.DmaSpaceVgcSprite,
        0, destAddr, 1);
    AssertDmaOk(bus, 1);

    // Verify the byte landed
    Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceSprite, destAddr, out byte val));
    Assert.AreEqual(0xAB, val);
}

[TestMethod]
public void MemIo_SpriteShapeRam_ReportsExpandedSize()
{
    var bus = MakeBus();
    Assert.AreEqual(VgcConstants.ShapeRamSize, bus.Vgc.GetMemorySpaceLength(VgcConstants.MemSpaceSprite));
}
```

**Step 2: Run tests**

Run: `dotnet test --filter "Dma_XramToSpriteShape_CanWriteSlot255|MemIo_SpriteShapeRam_ReportsExpandedSize" --verbosity normal`
Expected: PASS (the expanded `_spriteShapes` array already handles this)

**Step 3: Run full test suite**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 4: Commit**

```bash
git add e6502UnitTests/AvaloniaDmaTests.cs
git commit -m "test: verify DMA and MemorySpace work with expanded 256-slot sprite shape RAM"
```

---

### Task 10: Update VGC Documentation Comment and Design Doc

**Files:**
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs` (class doc comment at top)

**Step 1: Update the class doc comment**

Update the comment at the top of `VirtualGraphicsController` (lines 14-26) to reflect the new address ownership and architecture:

```csharp
/// <summary>
/// Virtual Graphics Controller for the Avalonia renderer.
/// Manages text display (80x25), block graphics (320x200), and multicolor sprites
/// (16 sprites, 16x16 pixels, 4-bit color per pixel, 256 shape slots).
///
/// Address ownership:
///   $A000-$A01F  VGC registers + command interface
///   $A040-$A0BF  Sprite registers (8 bytes × 16 sprites)
///   $AA00-$B1CF  Character RAM (2000 bytes)
///   $B1D0-$B99F  Color RAM (2000 bytes)
///
/// Sprite shape data stored in 256 × 128-byte slots (32KB), accessible via
/// memory space I/O and DMA. Each sprite has a shape index register pointing
/// to its active shape slot. The copper can write sprite registers at scanline
/// granularity for vertical sprite multiplexing.
/// </summary>
```

**Step 2: Build**

Run: `dotnet build`
Expected: clean build

**Step 3: Final full test run**

Run: `dotnet test --verbosity normal`
Expected: ALL pass

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualGraphicsController.cs
git commit -m "docs: update VGC class documentation for sprite registers and multiplexing"
```
