# NCC (Nova C Compiler) Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a C-like compiled language to the Nova VM with integrated editor, debugger, and overlay system, shipped as a swappable 16K ROM.

**Architecture:** The NCC compiler runs as C# host code (virtual hardware), producing native 6502 machine code. The 16K ROM at $C000-$FFFF contains the runtime library, editor, and debugger UI. Source code lives in XRAM. ROM swapping between BASIC and NCC is triggered via a register at $A03F.

**Tech Stack:** C#/.NET 10, MSTest, Avalonia (existing GUI), 6502 assembly (runtime ROM)

---

## Phase 1: ROM Swap Infrastructure

### Task 1.1: Add Compiler Controller Constants

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`

**Step 1: Add constants for the Compiler Controller and ROM swap register**

Add after the Help system registers section:

```csharp
// -------------------------------------------------------------------------
// ROM swap register ($A03F)
// -------------------------------------------------------------------------

public const int RegRomSwap       = 0xA03F;
public const byte RomSwapNcc      = 0x01;   // switch to NCC ROM
public const byte RomSwapBasic    = 0x02;   // switch to BASIC ROM

// -------------------------------------------------------------------------
// Compiler Controller registers ($A031-$A03D)
// -------------------------------------------------------------------------

public const int CmpBase          = 0xA031;
public const int CmpEnd           = 0xA03D;

public const int CmpCmd           = 0xA031;  // write triggers command
public const int CmpStatus        = 0xA032;  // 0=idle, 1=compiling, 2=ok, 3=error
public const int CmpErrCount      = 0xA033;  // number of errors
public const int CmpWarnCount     = 0xA034;  // number of warnings
public const int CmpErrLineL      = 0xA035;  // current error line low
public const int CmpErrLineH      = 0xA036;  // current error line high
public const int CmpCodeSizeL     = 0xA037;  // compiled code size low
public const int CmpCodeSizeH     = 0xA038;  // compiled code size high
public const int CmpErrMsg        = 0xA039;  // read error message byte-by-byte
public const int CmpErrIdx        = 0xA03A;  // current error index (write to select)
public const int CmpSrcAddrL      = 0xA03B;  // source XRAM address low
public const int CmpSrcAddrM      = 0xA03C;  // source XRAM address mid
public const int CmpSrcAddrH      = 0xA03D;  // source XRAM address high (24-bit, full 512K)

public const byte CmpCmdCompile   = 0x01;   // compile source from XRAM
public const byte CmpCmdGetError  = 0x02;   // populate error fields for CmpErrIdx
public const byte CmpCmdGetWarn   = 0x03;   // populate warning fields for CmpErrIdx

public const byte CmpStatusIdle      = 0x00;
public const byte CmpStatusCompiling = 0x01;
public const byte CmpStatusOk        = 0x02;
public const byte CmpStatusError     = 0x03;
```

**Step 2: Run build to verify constants compile**

Run: `dotnet build e6502.Avalonia`
Expected: Build succeeded

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: add Compiler Controller and ROM swap constants"
```

---

### Task 1.2: Implement ROM Swap in CompositeBusDevice

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`

**Step 1: Write test for ROM swap**

Create: `e6502UnitTests/RomSwapTests.cs`

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RomSwapTests
{
    [TestMethod]
    public void InitialRom_IsBasic()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
    }

    [TestMethod]
    public void WriteRomSwapNcc_SetsNccRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Ncc, bus.CurrentRom);
    }

    [TestMethod]
    public void WriteRomSwapBasic_SetsBasicRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
    }

    [TestMethod]
    public void RomSwap_FiresResetRequestedEvent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bool fired = false;
        bus.RomSwapRequested += (_, _) => fired = true;
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        Assert.IsTrue(fired);
    }

    [TestMethod]
    public void RomSwap_ChangesReadableRomContent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte basicByte = bus.Read(0xC000);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        byte nccByte = bus.Read(0xC000);
        // NCC ROM will have different content than BASIC ROM
        // (at minimum the reset vector will differ)
        Assert.AreNotEqual(basicByte, nccByte);
    }

    [TestMethod]
    public void WriteInvalidSwapValue_NoOp()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, 0xFF);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
    }
}
```

**Step 2: Run test to verify it fails**

Run: `dotnet test --filter "FullyQualifiedName~RomSwapTests" --verbosity normal`
Expected: FAIL — `ActiveRom`, `CurrentRom`, `RomSwapRequested` don't exist yet

**Step 3: Add ROM enum and swap support to CompositeBusDevice**

In `CompositeBusDevice.cs`, add:
- `public enum ActiveRom { Basic, Ncc }` inside the class
- `public ActiveRom CurrentRom { get; private set; } = ActiveRom.Basic;`
- `public event EventHandler? RomSwapRequested;`
- Private `byte[] _basicRom` and `byte[] _nccRom` fields
- In constructor: after loading ehbasic.bin, copy the $C000-$FFFF range to `_basicRom`; load `ncc.bin` into `_nccRom` (with fallback to a stub ROM if file doesn't exist yet)
- In `Write()`: intercept `RegRomSwap` before ROM protection check. On valid value, copy the target ROM into `_ram[0xC000..]`, set `CurrentRom`, fire `RomSwapRequested`

**Step 4: Create a stub NCC ROM binary**

Create a minimal 16KB binary that has valid reset vectors pointing to a simple loop. This will be replaced later with the real NCC ROM. Generate it programmatically in a helper or as a resource.

For now, create `e6502.Avalonia/Resources/ncc.bin` — a 16384-byte file with:
- $FFFC/$FFFD = $C0, $00 (reset vector → $C000)
- $C000 = $4C, $00, $C0 (JMP $C000 — infinite loop)
- Rest filled with $EA (NOP)

**Step 5: Add ncc.bin to csproj for copying**

In `e6502.Avalonia.csproj`, add `ncc.bin` to the copy-to-output items alongside `ehbasic.bin`.

**Step 6: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~RomSwapTests" --verbosity normal`
Expected: All PASS

**Step 7: Run all existing tests to verify no regressions**

Run: `dotnet test --verbosity normal`
Expected: All PASS

**Step 8: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/RomSwapTests.cs e6502.Avalonia/Resources/ncc.bin e6502.Avalonia/e6502.Avalonia.csproj
git commit -m "feat: ROM swap infrastructure — BASIC ↔ NCC via $A03F register"
```

---

### Task 1.3: Wire ROM Swap to CPU Reset in MainWindow

**Files:**
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`

**Step 1: Subscribe to RomSwapRequested in MainWindow**

In the CPU thread loop or initialization, subscribe to `_bus.RomSwapRequested`. When fired, call `_cpu.Boot()` to reset the CPU (reads the new reset vector from the swapped ROM).

**Step 2: Test manually**

Run: `dotnet run --project e6502.Avalonia`
Verify the emulator boots into BASIC as before. (ROM swap from BASIC won't work yet — needs the NCC command in the BASIC ROM, which comes later.)

**Step 3: Commit**

```bash
git add e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: wire ROM swap event to CPU reset"
```

---

## Phase 2: Compiler Controller (Virtual Hardware)

### Task 2.1: Implement CompilerController Class

**Files:**
- Create: `e6502.Avalonia/Hardware/CompilerController.cs`
- Create: `e6502UnitTests/CompilerControllerTests.cs`

**Step 1: Write basic tests**

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class CompilerControllerTests
{
    [TestMethod]
    public void OwnsAddress_CmpBase_True()
    {
        var cc = new CompilerController(_ => 0, (_, _) => { });
        Assert.IsTrue(cc.OwnsAddress(VgcConstants.CmpBase));
    }

    [TestMethod]
    public void OwnsAddress_CmpEnd_True()
    {
        var cc = new CompilerController(_ => 0, (_, _) => { });
        Assert.IsTrue(cc.OwnsAddress(VgcConstants.CmpEnd));
    }

    [TestMethod]
    public void OwnsAddress_Outside_False()
    {
        var cc = new CompilerController(_ => 0, (_, _) => { });
        Assert.IsFalse(cc.OwnsAddress(0xA02F));
        Assert.IsFalse(cc.OwnsAddress(VgcConstants.RegRomSwap));
    }

    [TestMethod]
    public void InitialStatus_Idle()
    {
        var cc = new CompilerController(_ => 0, (_, _) => { });
        Assert.AreEqual(VgcConstants.CmpStatusIdle, cc.Read(VgcConstants.CmpStatus));
    }

    [TestMethod]
    public void Compile_EmptySource_Error()
    {
        var xram = new byte[512 * 1024];
        var ram = new byte[65536];
        var cc = new CompilerController(
            addr => xram[addr],
            (addr, data) => ram[addr] = data);

        // Write 0-length source (null terminator at start)
        xram[0] = 0;

        // Set source address to 0
        cc.Write(VgcConstants.CmpSrcAddrL, 0);
        cc.Write(VgcConstants.CmpSrcAddrM, 0);
        cc.Write(VgcConstants.CmpSrcAddrE, 0);

        // Trigger compile
        cc.Write(VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        Assert.AreEqual(VgcConstants.CmpStatusError, cc.Read(VgcConstants.CmpStatus));
        Assert.IsTrue(cc.Read(VgcConstants.CmpErrCount) > 0);
    }
}
```

**Step 2: Run tests — verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~CompilerControllerTests" --verbosity normal`
Expected: FAIL — `CompilerController` doesn't exist yet

**Step 3: Implement CompilerController**

```csharp
namespace e6502.Avalonia.Hardware;

public sealed class CompilerController
{
    private readonly byte[] _regs = new byte[VgcConstants.CmpEnd - VgcConstants.CmpBase + 1];
    private readonly Func<int, byte> _readXram;
    private readonly Action<ushort, byte> _writeCpuRam;
    private readonly List<CompileError> _errors = new();
    private readonly List<CompileError> _warnings = new();
    private int _errMsgPos;

    public CompilerController(Func<int, byte> readXram, Action<ushort, byte> writeCpuRam)
    {
        _readXram = readXram;
        _writeCpuRam = writeCpuRam;
    }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.CmpBase && address <= VgcConstants.CmpEnd;

    public byte Read(ushort address)
    {
        if (address == VgcConstants.CmpErrMsg)
            return ReadNextErrorMsgByte();
        return _regs[address - VgcConstants.CmpBase];
    }

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.CmpBase] = data;
        if (address == VgcConstants.CmpCmd)
            ExecuteCommand(data);
    }

    private void ExecuteCommand(byte cmd)
    {
        switch (cmd)
        {
            case VgcConstants.CmpCmdCompile:
                DoCompile();
                break;
            case VgcConstants.CmpCmdGetError:
                PopulateError(_errors);
                break;
            case VgcConstants.CmpCmdGetWarn:
                PopulateError(_warnings);
                break;
        }
    }

    private void DoCompile()
    {
        SetStatus(VgcConstants.CmpStatusCompiling);
        _errors.Clear();
        _warnings.Clear();

        // Read source address from registers
        int srcAddr = _regs[VgcConstants.CmpSrcAddrL - VgcConstants.CmpBase]
                    | (_regs[VgcConstants.CmpSrcAddrM - VgcConstants.CmpBase] << 8)
                    | (_regs[VgcConstants.CmpSrcAddrE - VgcConstants.CmpBase] << 16);

        // Read source from XRAM until null terminator
        var source = ReadSourceFromXram(srcAddr);

        if (string.IsNullOrWhiteSpace(source))
        {
            _errors.Add(new CompileError(1, "empty source"));
            SetResult(VgcConstants.CmpStatusError, _errors.Count, _warnings.Count, 0);
            return;
        }

        // TODO: Invoke the actual compiler (Phase 3+)
        // For now, just report success with 0 bytes
        _errors.Add(new CompileError(0, "compiler not yet implemented"));
        SetResult(VgcConstants.CmpStatusError, _errors.Count, _warnings.Count, 0);
    }

    private string ReadSourceFromXram(int addr)
    {
        var sb = new System.Text.StringBuilder();
        for (int i = 0; i < 512 * 1024; i++)
        {
            byte b = _readXram(addr + i);
            if (b == 0) break;
            sb.Append((char)b);
        }
        return sb.ToString();
    }

    private void PopulateError(List<CompileError> list)
    {
        int idx = _regs[VgcConstants.CmpErrIdx - VgcConstants.CmpBase];
        if (idx < list.Count)
        {
            var err = list[idx];
            _regs[VgcConstants.CmpErrLineL - VgcConstants.CmpBase] = (byte)(err.Line & 0xFF);
            _regs[VgcConstants.CmpErrLineH - VgcConstants.CmpBase] = (byte)((err.Line >> 8) & 0xFF);
            _errMsgPos = 0;
        }
    }

    private byte ReadNextErrorMsgByte()
    {
        int idx = _regs[VgcConstants.CmpErrIdx - VgcConstants.CmpBase];
        var list = _errors; // TODO: track which list based on last command
        if (idx >= list.Count) return 0;
        var msg = list[idx].Message;
        if (_errMsgPos >= msg.Length) return 0;
        return (byte)msg[_errMsgPos++];
    }

    private void SetStatus(byte status) =>
        _regs[VgcConstants.CmpStatus - VgcConstants.CmpBase] = status;

    private void SetResult(byte status, int errors, int warnings, int codeSize)
    {
        _regs[VgcConstants.CmpStatus - VgcConstants.CmpBase] = status;
        _regs[VgcConstants.CmpErrCount - VgcConstants.CmpBase] = (byte)errors;
        _regs[VgcConstants.CmpWarnCount - VgcConstants.CmpBase] = (byte)warnings;
        _regs[VgcConstants.CmpCodeSizeL - VgcConstants.CmpBase] = (byte)(codeSize & 0xFF);
        _regs[VgcConstants.CmpCodeSizeH - VgcConstants.CmpBase] = (byte)((codeSize >> 8) & 0xFF);
    }

    private record CompileError(int Line, string Message);
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~CompilerControllerTests" --verbosity normal`
Expected: All PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompilerController.cs e6502UnitTests/CompilerControllerTests.cs
git commit -m "feat: CompilerController virtual hardware with register interface"
```

---

### Task 2.2: Wire CompilerController into CompositeBusDevice

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Create: `e6502UnitTests/CompilerBusRoutingTests.cs`

**Step 1: Write routing test**

```csharp
[TestClass]
public class CompilerBusRoutingTests
{
    [TestMethod]
    public void ReadCmpStatus_RoutesToCompilerController()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte status = bus.Read(VgcConstants.CmpStatus);
        Assert.AreEqual(VgcConstants.CmpStatusIdle, status);
    }

    [TestMethod]
    public void WriteCmpCmd_RoutesToCompilerController()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        // Should not throw — just verifies routing
        bus.Write(VgcConstants.CmpCmd, 0xFF);
    }
}
```

**Step 2: Run tests — verify they fail**

**Step 3: Wire into CompositeBusDevice**

Add `private readonly CompilerController _compiler;` field. Construct with XRAM read delegate and CPU RAM write delegate. Add `OwnsAddress` checks in `Read()` and `Write()` — place after help registers, before VGC.

**Step 4: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: All PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/CompilerBusRoutingTests.cs
git commit -m "feat: wire CompilerController into CompositeBusDevice"
```

---

## Phase 3: NCC Lexer

The compiler lives in a new namespace: `e6502.Avalonia.Compiler`. All compiler code is C# running on the host.

### Task 3.1: Define Token Types

**Files:**
- Create: `e6502.Avalonia/Compiler/Token.cs`

**Step 1: Write test for token creation**

Create: `e6502UnitTests/Compiler/TokenTests.cs`

```csharp
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class TokenTests
{
    [TestMethod]
    public void Token_Stores_Type_And_Value()
    {
        var tok = new Token(TokenType.IntLiteral, "42", 1, 5);
        Assert.AreEqual(TokenType.IntLiteral, tok.Type);
        Assert.AreEqual("42", tok.Value);
        Assert.AreEqual(1, tok.Line);
        Assert.AreEqual(5, tok.Column);
    }
}
```

**Step 2: Implement Token and TokenType**

```csharp
namespace e6502.Avalonia.Compiler;

public enum TokenType
{
    // Literals
    IntLiteral, FixedLiteral, StringLiteral, CharLiteral,
    HexLiteral, BinLiteral, BoolLiteral,

    // Identifiers
    Identifier,

    // Type keywords
    Byte, Int, Uint, Bool, Fixed, Ufixed, Void, Struct, Enum, Const,

    // Control flow keywords
    If, Else, While, Do, For, Switch, Case, Default, Break, Continue, Return,

    // Other keywords
    Asm, Resident, Interrupt, Sizeof, Asset, Pragma, Include, Define,

    // Operators
    Plus, Minus, Star, Slash, Percent,          // + - * / %
    Amp, Pipe, Caret, Tilde, Bang,              // & | ^ ~ !
    AmpAmp, PipePipe,                           // && ||
    LShift, RShift,                             // << >>
    Eq, NotEq, Lt, Gt, LtEq, GtEq,            // == != < > <= >=
    Assign, PlusAssign, MinusAssign,            // = += -=
    StarAssign, SlashAssign, PercentAssign,     // *= /= %=
    AmpAssign, PipeAssign, CaretAssign,         // &= |= ^=
    LShiftAssign, RShiftAssign,                 // <<= >>=
    PlusPlus, MinusMinus,                       // ++ --
    Arrow, Dot,                                 // -> .

    // Delimiters
    LParen, RParen, LBrace, RBrace,            // ( ) { }
    LBracket, RBracket,                         // [ ]
    Semicolon, Comma, Colon, Hash,              // ; , : #

    // Special
    Eof, Error,
}

public readonly record struct Token(TokenType Type, string Value, int Line, int Column);
```

**Step 3: Run test**

Run: `dotnet test --filter "FullyQualifiedName~TokenTests" --verbosity normal`
Expected: PASS

**Step 4: Commit**

```bash
git add e6502.Avalonia/Compiler/Token.cs e6502UnitTests/Compiler/TokenTests.cs
git commit -m "feat: NCC token types and Token record"
```

---

### Task 3.2: Implement Lexer

**Files:**
- Create: `e6502.Avalonia/Compiler/Lexer.cs`
- Create: `e6502UnitTests/Compiler/LexerTests.cs`

**Step 1: Write comprehensive lexer tests**

```csharp
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class LexerTests
{
    private static List<Token> Lex(string source) => new Lexer(source).Tokenize();

    [TestMethod]
    public void Empty_ReturnsEof()
    {
        var tokens = Lex("");
        Assert.AreEqual(1, tokens.Count);
        Assert.AreEqual(TokenType.Eof, tokens[0].Type);
    }

    [TestMethod]
    public void IntegerLiteral()
    {
        var tokens = Lex("42");
        Assert.AreEqual(TokenType.IntLiteral, tokens[0].Type);
        Assert.AreEqual("42", tokens[0].Value);
    }

    [TestMethod]
    public void HexLiteral()
    {
        var tokens = Lex("0xA040");
        Assert.AreEqual(TokenType.HexLiteral, tokens[0].Type);
        Assert.AreEqual("0xA040", tokens[0].Value);
    }

    [TestMethod]
    public void BinaryLiteral()
    {
        var tokens = Lex("0b10110");
        Assert.AreEqual(TokenType.BinLiteral, tokens[0].Type);
    }

    [TestMethod]
    public void FixedLiteral()
    {
        var tokens = Lex("3.14");
        Assert.AreEqual(TokenType.FixedLiteral, tokens[0].Type);
        Assert.AreEqual("3.14", tokens[0].Value);
    }

    [TestMethod]
    public void StringLiteral()
    {
        var tokens = Lex("\"hello\"");
        Assert.AreEqual(TokenType.StringLiteral, tokens[0].Type);
        Assert.AreEqual("hello", tokens[0].Value);
    }

    [TestMethod]
    public void CharLiteral()
    {
        var tokens = Lex("'A'");
        Assert.AreEqual(TokenType.CharLiteral, tokens[0].Type);
        Assert.AreEqual("A", tokens[0].Value);
    }

    [TestMethod]
    public void Keywords()
    {
        var tokens = Lex("byte int uint bool fixed ufixed void struct enum const");
        Assert.AreEqual(TokenType.Byte, tokens[0].Type);
        Assert.AreEqual(TokenType.Int, tokens[1].Type);
        Assert.AreEqual(TokenType.Uint, tokens[2].Type);
        Assert.AreEqual(TokenType.Bool, tokens[3].Type);
        Assert.AreEqual(TokenType.Fixed, tokens[4].Type);
        Assert.AreEqual(TokenType.Ufixed, tokens[5].Type);
        Assert.AreEqual(TokenType.Void, tokens[6].Type);
        Assert.AreEqual(TokenType.Struct, tokens[7].Type);
        Assert.AreEqual(TokenType.Enum, tokens[8].Type);
        Assert.AreEqual(TokenType.Const, tokens[9].Type);
    }

    [TestMethod]
    public void ControlFlowKeywords()
    {
        var tokens = Lex("if else while do for switch case default break continue return");
        Assert.AreEqual(TokenType.If, tokens[0].Type);
        Assert.AreEqual(TokenType.Return, tokens[10].Type);
    }

    [TestMethod]
    public void Operators()
    {
        var tokens = Lex("+ - * / % & | ^ ~ ! && || << >> == != < > <= >=");
        Assert.AreEqual(TokenType.Plus, tokens[0].Type);
        Assert.AreEqual(TokenType.GtEq, tokens[^2].Type);
    }

    [TestMethod]
    public void AssignmentOperators()
    {
        var tokens = Lex("= += -= *= /= %= &= |= ^= <<= >>=");
        Assert.AreEqual(TokenType.Assign, tokens[0].Type);
        Assert.AreEqual(TokenType.RShiftAssign, tokens[^2].Type);
    }

    [TestMethod]
    public void Delimiters()
    {
        var tokens = Lex("( ) { } [ ] ; , : #");
        Assert.AreEqual(TokenType.LParen, tokens[0].Type);
        Assert.AreEqual(TokenType.Hash, tokens[^2].Type);
    }

    [TestMethod]
    public void Identifier()
    {
        var tokens = Lex("player_x");
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual("player_x", tokens[0].Value);
    }

    [TestMethod]
    public void LineComment()
    {
        var tokens = Lex("42 // comment\n43");
        Assert.AreEqual(TokenType.IntLiteral, tokens[0].Type);
        Assert.AreEqual("42", tokens[0].Value);
        Assert.AreEqual(TokenType.IntLiteral, tokens[1].Type);
        Assert.AreEqual("43", tokens[1].Value);
    }

    [TestMethod]
    public void BlockComment()
    {
        var tokens = Lex("42 /* block\ncomment */ 43");
        Assert.AreEqual(2, tokens.Count(t => t.Type == TokenType.IntLiteral));
    }

    [TestMethod]
    public void TracksLineNumbers()
    {
        var tokens = Lex("a\nb\nc");
        Assert.AreEqual(1, tokens[0].Line);
        Assert.AreEqual(2, tokens[1].Line);
        Assert.AreEqual(3, tokens[2].Line);
    }

    [TestMethod]
    public void CompleteStatement()
    {
        var tokens = Lex("byte x = 42;");
        Assert.AreEqual(TokenType.Byte, tokens[0].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual(TokenType.Assign, tokens[2].Type);
        Assert.AreEqual(TokenType.IntLiteral, tokens[3].Type);
        Assert.AreEqual(TokenType.Semicolon, tokens[4].Type);
        Assert.AreEqual(TokenType.Eof, tokens[5].Type);
    }

    [TestMethod]
    public void BoolLiterals()
    {
        var tokens = Lex("true false");
        Assert.AreEqual(TokenType.BoolLiteral, tokens[0].Type);
        Assert.AreEqual("true", tokens[0].Value);
        Assert.AreEqual(TokenType.BoolLiteral, tokens[1].Type);
        Assert.AreEqual("false", tokens[1].Value);
    }

    [TestMethod]
    public void IncrementDecrement()
    {
        var tokens = Lex("++ --");
        Assert.AreEqual(TokenType.PlusPlus, tokens[0].Type);
        Assert.AreEqual(TokenType.MinusMinus, tokens[1].Type);
    }

    [TestMethod]
    public void ArrowAndDot()
    {
        var tokens = Lex("p->x p.y");
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual(TokenType.Arrow, tokens[1].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[2].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[3].Type);
        Assert.AreEqual(TokenType.Dot, tokens[4].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[5].Type);
    }

    [TestMethod]
    public void PreprocessorDirectives()
    {
        var tokens = Lex("#include #define #asset #pragma");
        Assert.AreEqual(TokenType.Include, tokens[0].Type);
        Assert.AreEqual(TokenType.Define, tokens[1].Type);
        Assert.AreEqual(TokenType.Asset, tokens[2].Type);
        Assert.AreEqual(TokenType.Pragma, tokens[3].Type);
    }
}
```

**Step 2: Run tests — verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~LexerTests" --verbosity normal`
Expected: FAIL — `Lexer` doesn't exist

**Step 3: Implement Lexer**

The `Lexer` class takes a source string and produces a `List<Token>`. Implementation should handle:
- Whitespace skipping
- `//` line comments and `/* */` block comments
- Number literals (decimal, hex `0x`, binary `0b`, fixed-point with `.`)
- String literals (`"..."`) and char literals (`'.'`)
- Keywords (lookup table from string → TokenType)
- Identifiers (`[a-zA-Z_][a-zA-Z0-9_]*`)
- Multi-character operators (`==`, `!=`, `<=`, `>=`, `&&`, `||`, `<<`, `>>`, `->`, `++`, `--`, `+=`, `-=`, `<<=`, `>>=`, etc.)
- Single-character operators and delimiters
- `#` followed by keyword = preprocessor directive (tokenized as single token: `#include` → `TokenType.Include`)
- Line/column tracking

**Step 4: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~LexerTests" --verbosity normal`
Expected: All PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Compiler/Lexer.cs e6502UnitTests/Compiler/LexerTests.cs
git commit -m "feat: NCC lexer — tokenizes full C-like syntax"
```

---

## Phase 4: NCC Parser & AST

### Task 4.1: Define AST Node Types

**Files:**
- Create: `e6502.Avalonia/Compiler/Ast.cs`

All AST nodes. Key node types:

- **Expressions:** IntLiteral, FixedLiteral, StringLiteral, BoolLiteral, Identifier, BinaryExpr, UnaryExpr, CallExpr, IndexExpr, MemberExpr, CastExpr, SizeofExpr, AssignExpr
- **Statements:** VarDecl, ExprStmt, IfStmt, WhileStmt, DoWhileStmt, ForStmt, SwitchStmt, ReturnStmt, BreakStmt, ContinueStmt, BlockStmt, AsmStmt
- **Declarations:** FuncDecl, StructDecl, EnumDecl, GlobalVarDecl, IncludeDirective, DefineDirective, AssetDirective, PragmaDirective
- **Program:** list of top-level declarations

Each node carries `Line` and `Column` for error reporting.

**Step 1: Implement AST types as C# records**

**Step 2: Commit**

```bash
git add e6502.Avalonia/Compiler/Ast.cs
git commit -m "feat: NCC AST node types"
```

---

### Task 4.2: Implement Expression Parser

**Files:**
- Create: `e6502.Avalonia/Compiler/Parser.cs`
- Create: `e6502UnitTests/Compiler/ParserExprTests.cs`

**Step 1: Write expression parser tests**

Test cases:
- Integer literal → IntLiteralExpr
- Binary ops with precedence: `1 + 2 * 3` → Plus(1, Mul(2, 3))
- Unary: `-x`, `!flag`, `~mask`
- Parenthesized: `(1 + 2) * 3`
- Function call: `spr_move(0, x, y)`
- Array index: `arr[i]`
- Member access: `player.x`, `p->y`
- Assignment: `x = 42`, `x += 1`
- Cast: `(int)x`
- Compound: `a->b[i].c`

**Step 2: Implement expression parsing (Pratt parser / precedence climbing)**

**Step 3: Run tests, verify pass**

**Step 4: Commit**

```bash
git add e6502.Avalonia/Compiler/Parser.cs e6502UnitTests/Compiler/ParserExprTests.cs
git commit -m "feat: NCC expression parser with precedence climbing"
```

---

### Task 4.3: Implement Statement Parser

**Files:**
- Modify: `e6502.Avalonia/Compiler/Parser.cs`
- Create: `e6502UnitTests/Compiler/ParserStmtTests.cs`

Test cases:
- Variable declarations: `byte x = 42;`
- If/else: `if (x > 0) { ... } else { ... }`
- While: `while (running) { ... }`
- Do/while: `do { ... } while (x);`
- For: `for (byte i = 0; i < 10; i++) { ... }`
- Switch/case: `switch (state) { case IDLE: ...; break; }`
- Return: `return 42;`, `return;`
- Break/continue
- Expression statement: `spr_move(0, x, y);`
- Block: `{ stmt; stmt; }`
- Asm block: `asm { LDA #$00; STA $A000; };`

---

### Task 4.4: Implement Declaration Parser

**Files:**
- Modify: `e6502.Avalonia/Compiler/Parser.cs`
- Create: `e6502UnitTests/Compiler/ParserDeclTests.cs`

Test cases:
- Function: `void main() { ... }`
- Function with params: `byte add(byte a, byte b) { return a + b; }`
- Struct: `struct Player { fixed x, y; byte health; };`
- Enum: `enum State { IDLE, WALK, JUMP };`
- Global variable: `byte score;`
- Global array: `byte bullets[16];`
- `#include "file.c"`
- `#define NAME value`
- `#asset sprite "file.gfx"`
- `#pragma overlay_size 8192`
- `__resident` and `__interrupt` modifiers

---

## Phase 5: Type Checker

### Task 5.1: Define Type System

**Files:**
- Create: `e6502.Avalonia/Compiler/Types.cs`

Type representations:
- `ByteType` (8-bit unsigned)
- `IntType` (16-bit signed)
- `UintType` (16-bit unsigned)
- `BoolType` (8-bit)
- `FixedType` (16-bit 8.8 signed)
- `UfixedType` (16-bit 8.8 unsigned)
- `VoidType`
- `PointerType(BaseType)`
- `ArrayType(ElementType, Size)`
- `StructType(Name, Fields[])`
- `EnumType(Name, Values[])`
- `FuncType(ReturnType, ParamTypes[])`

### Task 5.2: Implement Semantic Analysis

**Files:**
- Create: `e6502.Avalonia/Compiler/TypeChecker.cs`
- Create: `e6502UnitTests/Compiler/TypeCheckerTests.cs`

Responsibilities:
- Resolve all identifiers to their declarations
- Type-check expressions (binary ops, assignments, function calls)
- Verify function signatures match call sites
- Check array bounds are constant
- Verify struct member access
- Implicit conversions: byte→int, byte→fixed (zero fractional part)
- Error on: type mismatches, undeclared variables, duplicate declarations, void in expressions
- Build symbol table with scope tracking (global, function-local, block-local)

---

## Phase 6: 6502 Code Generator

### Task 6.1: Code Generation Framework

**Files:**
- Create: `e6502.Avalonia/Compiler/CodeGen.cs`
- Create: `e6502.Avalonia/Compiler/CodeEmitter.cs`

`CodeEmitter` tracks current output address, emits bytes, provides helpers for common 6502 patterns (LDA/STA, 16-bit add/sub, push/pop, JSR/RTS).

`CodeGen` walks the typed AST and emits 6502 instructions.

**Memory layout of compiled output:**

```
$0280  ┌─ JMP main ─────────────┐
$0283  │ Global variables        │
       │ (zero-initialized)      │
       ├─────────────────────────┤
       │ Function code           │
       │ (in order of definition)│
       ├─────────────────────────┤
       │ String/const data       │
       ├─────────────────────────┤
       │ Stack grows down from   │
$9FFF  └─ overlay boundary ──────┘
```

### Task 6.2: Expression Code Generation

- Byte operations: use A register directly
- Int/uint/fixed operations: use A/X pair (low/high) or zero-page temporaries
- Fixed-point multiply: runtime library call
- Comparison → set carry/zero flags → conditional branch
- Function calls: push args to stack, JSR, pop result

### Task 6.3: Statement Code Generation

- If/else → CMP/BEQ/BNE/JMP
- While/for → label, condition, BEQ exit, body, JMP label
- Switch → cascade of CMP/BEQ
- Return → load result, RTS
- Variable assignment → STA to known address (global = absolute, local = stack-relative)

### Task 6.4: Function Code Generation

- Function prologue: adjust stack pointer for locals
- Function epilogue: restore stack, RTS
- `__interrupt`: save A/X/Y/P, body, restore, RTI
- Calling convention: args pushed right-to-left, result in A (byte) or A/X (16-bit)

### Task 6.5: Integration Test — End-to-End Compile and Execute

**Files:**
- Create: `e6502UnitTests/Compiler/EndToEndTests.cs`

Test pattern: compile a small NCC program, load the output into a `BusDevice`, boot the CPU, run to completion (RTS or BRK), check memory for expected values.

```csharp
[TestMethod]
public void SimpleAssignment_CompilesAndRuns()
{
    var source = "byte x; void main() { x = 42; }";
    var compiler = new NccCompiler();
    var result = compiler.Compile(source);
    Assert.IsTrue(result.Success);

    var bus = new BusDevice();
    result.Code.CopyTo(bus.Ram, 0x0280);
    // Set reset vector to entry point
    bus.Ram[0xFFFC] = 0x80;
    bus.Ram[0xFFFD] = 0x02;

    var cpu = new Cpu(bus);
    cpu.Boot();
    // Run until RTS (or cycle limit)
    for (int i = 0; i < 1000 && cpu.Pc != /* halt addr */; i++)
        cpu.ExecuteNext();

    Assert.AreEqual(42, bus.Ram[result.Symbols["x"].Address]);
}
```

---

## Phase 7: Runtime Library (6502 ROM)

### Task 7.1: NCC ROM Bootstrap

**Files:**
- Create: `e6502.Avalonia/Resources/ncc_rom/` directory
- Create: `e6502.Avalonia/Resources/ncc_rom/boot.asm`

Assembly (ca65) source for the NCC ROM. Bootstrap:
- Reset vector → editor init
- IRQ/NMI vectors → handlers in ROM
- Hardware init (clear screen, set colors)
- Jump to editor main loop

### Task 7.2: Runtime Library Routines

Implement in 6502 assembly (or generate from C#):
- Fixed-point multiply/divide (8.8 × 8.8 → 8.8)
- memcpy, memset
- print (string output via RegCharOut)
- get_key, wait_key (poll RegCharIn)
- vgc_vsync (wait for frame counter change)
- Hardware library wrappers: each `nova.h` function maps to setting VGC registers and writing command byte

### Task 7.3: Debug Support Routines

- BRK handler: save CPU state, enter debug mode
- Single-step support: replace next instruction with BRK, save original
- Watch update: read variable values from known addresses, format, display

---

## Phase 8: Editor

### Task 8.1: NCC Screen Editor

**Files:**
- Create: `e6502.Avalonia/Editor/NccEditor.cs`

C# implementation (runs on host, renders via VGC) of the full-screen editor:
- Text buffer backed by XRAM (via XMC)
- Cursor movement, insert/delete, line operations
- Scroll viewport when cursor moves beyond visible area
- Syntax highlighting via the Lexer (tokenize visible lines, apply colors)
- Status bar: filename, line/column, mode

### Task 8.2: File Operations

- F3: File browser (list .c files from ~/e6502-programs)
- Ctrl+S: Save current file to disk
- Ctrl+O: Open file (via file browser)
- New file creation

### Task 8.3: Build Integration

- F9: Trigger compile via CompilerController
- Display errors in a panel below the editor
- Cursor jumps to first error line
- Enter cycles through errors
- F5: Build + Run (hand screen to compiled program)
- Ctrl+Break: Stop running program, return to editor

---

## Phase 9: Debugger

### Task 9.1: Symbol Table Export

The compiler exports a symbol table to XRAM: function names/addresses, variable names/types/addresses, line→address mapping. Format TBD but must be compact.

### Task 9.2: Debug Mode UI

Split-screen: source on left, watch/stack on right. Current line highlighted. Breakpoint markers in gutter.

### Task 9.3: Breakpoint & Stepping Engine

- F2: Toggle breakpoint at current line (insert/remove BRK in compiled code)
- F8: Step over (set temporary breakpoint after current line)
- F7: Step into (set temporary breakpoint at call target)
- F9: Step out (set temporary breakpoint at return address)
- F5: Continue to next breakpoint

---

## Phase 10: Overlay System

### Task 10.1: Asset Packer

**Files:**
- Create: `e6502.Avalonia/Compiler/AssetPacker.cs`

At compile time: gather all `#asset` files, create a `.pak` bundle, store in `~/e6502-programs`. Format: header with asset names/types/offsets, followed by concatenated asset data.

### Task 10.2: Overlay Manager Runtime

Add to the runtime library in ROM:
- `ovl_init`: load .pak from disk to XRAM via FIO
- `spr_load`, `gfx_load`, etc.: DMA from XRAM named blocks to hardware
- Segment tracking: which overlay segments are in the window
- LRU eviction

### Task 10.3: Compiler Overlay Support

- Analyze call graph, partition functions into segments
- Generate trampolines for overlay function calls
- `__resident` annotation support
- `#pragma overlay_size` support
- Compile-time warnings for oversized segments

---

## Phase 11: MCP Integration

### Task 11.1: TCP Commands for NCC

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`

Add commands: `ncc_compile`, `ncc_edit`, `ncc_run`, `ncc_switch_rom`. Follows existing TCP command pattern.

### Task 11.2: MCP Tools

**Files:**
- Modify: `e6502.MCP/EmulatorTools.cs`

Add MCP tools: `CompileNcc`, `SwitchToNcc`, `SwitchToBasic`, `EditNccFile`, `RunNccProgram`.

---

## Execution Order

Phases 1-6 are sequential (each builds on the previous). Within each phase, tasks are sequential.

Phases 7-8 can be developed in parallel once Phase 6 is complete.

Phase 9 depends on Phase 8.

Phase 10 depends on Phase 6.

Phase 11 can be done any time after Phase 2.

**Suggested implementation order for first milestone (compilable "hello world"):**
1. Phase 1 (ROM swap) — foundation
2. Phase 2 (Compiler controller) — hardware interface
3. Phase 3 (Lexer) — first compiler component
4. Phase 4 (Parser) — AST generation
5. Phase 5 (Type checker) — semantic analysis
6. Phase 6 (Code gen) — produce running 6502 code
7. Phase 7 (Runtime) — minimal bootstrap + print
8. Phase 8 (Editor) — write/compile/run cycle

**First demo program target:**

```c
#include <nova.h>

void main() {
  cls();
  print("Hello from NCC!");
  wait_key();
}
```
