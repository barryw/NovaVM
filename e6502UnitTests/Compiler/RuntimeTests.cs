using System;
using KDS.e6502;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

/// <summary>
/// Tests for NccRomBuilder (Phase 7 runtime library).
///
/// Unit tests verify the ROM structure.
/// End-to-end tests compile NCC programs that call runtime routines and
/// verify the results by inspecting memory after execution.
///
/// Note: end-to-end tests run in a flat BusDevice (no VGC hardware),
/// so only pure-computation routines (MulByte/DivByte/ModByte/MulInt)
/// are exercised.  I/O and graphics routines are structural-only tested.
/// </summary>
[TestClass]
public class RuntimeTests
{
    // ── Unit tests: ROM structure ────────────────────────────────────────────

    [TestMethod]
    public void RomSize_Is16384Bytes()
    {
        byte[] rom = new NccRomBuilder().Build();
        Assert.AreEqual(16384, rom.Length);
    }

    [TestMethod]
    public void ResetVector_PointsInsideRom()
    {
        byte[] rom = new NccRomBuilder().Build();
        // Reset vector at offset 0x3FFC / 0x3FFD
        ushort resetLo = rom[0x3FFC];
        ushort resetHi = rom[0x3FFD];
        ushort resetAddr = (ushort)(resetLo | (resetHi << 8));
        Assert.IsTrue(resetAddr >= 0xC000 && resetAddr <= 0xFFFF,
            $"Reset vector ${resetAddr:X4} is outside ROM range");
    }

    [TestMethod]
    public void NmiVector_PointsInsideRom()
    {
        byte[] rom = new NccRomBuilder().Build();
        ushort addr = (ushort)(rom[0x3FFA] | (rom[0x3FFB] << 8));
        Assert.IsTrue(addr >= 0xC000 && addr <= 0xFFFF,
            $"NMI vector ${addr:X4} is outside ROM range");
    }

    [TestMethod]
    public void IrqVector_PointsInsideRom()
    {
        byte[] rom = new NccRomBuilder().Build();
        ushort addr = (ushort)(rom[0x3FFE] | (rom[0x3FFF] << 8));
        Assert.IsTrue(addr >= 0xC000 && addr <= 0xFFFF,
            $"IRQ vector ${addr:X4} is outside ROM range");
    }

    [TestMethod]
    public void JumpTable_Has22JmpInstructions()
    {
        byte[] rom = new NccRomBuilder().Build();
        // First 22×3 = 66 bytes should all be JMP (opcode 0x4C) instructions.
        for (int i = 0; i < 22; i++)
        {
            int offset = i * 3;
            Assert.AreEqual(0x4C, rom[offset],
                $"Jump table entry {i} at offset {offset} is not JMP (0x4C), got 0x{rom[offset]:X2}");
        }
    }

    [TestMethod]
    public void JumpTable_AllTargetsInsideRom()
    {
        byte[] rom = new NccRomBuilder().Build();
        for (int i = 0; i < 22; i++)
        {
            int offset = i * 3;
            ushort target = (ushort)(rom[offset + 1] | (rom[offset + 2] << 8));
            Assert.IsTrue(target >= 0xC000 && target <= 0xFFFF,
                $"Jump table entry {i} target ${target:X4} is outside ROM range");
        }
    }

    [TestMethod]
    public void MulByteEntry_JumpsToValidAddress()
    {
        byte[] rom = new NccRomBuilder().Build();
        // MulByte is jump table entry 0 at offset 0 in the ROM ($C000).
        Assert.AreEqual(0x4C, rom[0], "First byte should be JMP opcode");
        ushort target = (ushort)(rom[1] | (rom[2] << 8));
        Assert.IsTrue(target > 0xC03E, $"MulByte impl should be after the jump table, got ${target:X4}");
    }

    [TestMethod]
    public void DivByteEntry_JumpsToValidAddress()
    {
        byte[] rom = new NccRomBuilder().Build();
        // DivByte is jump table entry 2 (offset 6).
        ushort target = (ushort)(rom[7] | (rom[8] << 8));
        Assert.IsTrue(target > 0xC03E, $"DivByte impl should be after the jump table, got ${target:X4}");
    }

    [TestMethod]
    public void ModByteEntry_JumpsToValidAddress()
    {
        byte[] rom = new NccRomBuilder().Build();
        // ModByte is jump table entry 4 (offset 12).
        ushort target = (ushort)(rom[13] | (rom[14] << 8));
        Assert.IsTrue(target > 0xC03E, $"ModByte impl should be after the jump table, got ${target:X4}");
    }

    [TestMethod]
    public void JumpTable_EntryAddressesMatchRuntimeAddresses()
    {
        // The jump table entries must be at the addresses declared in RuntimeAddresses,
        // since the compiler emits JSR calls to those fixed addresses.
        byte[] rom = new NccRomBuilder().Build();

        // Verify opcode at each declared RuntimeAddress offset
        var entries = new (string Name, ushort Addr)[]
        {
            (nameof(RuntimeAddresses.MulByte),   RuntimeAddresses.MulByte),
            (nameof(RuntimeAddresses.MulInt),    RuntimeAddresses.MulInt),
            (nameof(RuntimeAddresses.DivByte),   RuntimeAddresses.DivByte),
            (nameof(RuntimeAddresses.DivInt),    RuntimeAddresses.DivInt),
            (nameof(RuntimeAddresses.ModByte),   RuntimeAddresses.ModByte),
            (nameof(RuntimeAddresses.Print),     RuntimeAddresses.Print),
            (nameof(RuntimeAddresses.PrintChar), RuntimeAddresses.PrintChar),
            (nameof(RuntimeAddresses.GetKey),    RuntimeAddresses.GetKey),
            (nameof(RuntimeAddresses.WaitKey),   RuntimeAddresses.WaitKey),
            (nameof(RuntimeAddresses.VSync),     RuntimeAddresses.VSync),
            (nameof(RuntimeAddresses.MemCpy),    RuntimeAddresses.MemCpy),
            (nameof(RuntimeAddresses.MemSet),    RuntimeAddresses.MemSet),
            (nameof(RuntimeAddresses.GfxCmd),    RuntimeAddresses.GfxCmd),
            (nameof(RuntimeAddresses.SpriteCmd), RuntimeAddresses.SpriteCmd),
            (nameof(RuntimeAddresses.PlaySound), RuntimeAddresses.PlaySound),
            (nameof(RuntimeAddresses.MusicPlay), RuntimeAddresses.MusicPlay),
            (nameof(RuntimeAddresses.MusicStop), RuntimeAddresses.MusicStop),
            (nameof(RuntimeAddresses.SetVolume), RuntimeAddresses.SetVolume),
            (nameof(RuntimeAddresses.FixedMul),  RuntimeAddresses.FixedMul),
            (nameof(RuntimeAddresses.FixedDiv),  RuntimeAddresses.FixedDiv),
            (nameof(RuntimeAddresses.EditorEntry), RuntimeAddresses.EditorEntry),
        };

        foreach (var (name, addr) in entries)
        {
            int offset = addr - RuntimeAddresses.RomBase;
            Assert.AreEqual(0x4C, rom[offset],
                $"{name} at ${addr:X4} (rom offset {offset}) should be JMP (0x4C), got 0x{rom[offset]:X2}");
        }
    }

    // ── MulByte direct execution tests ───────────────────────────────────────
    //
    // The harness emits: LDA #mul; LDX #mcd; JSR $C000; STA $01; STX $02; BRK
    // We read results back from address $01 (A) and $02 (X) after execution.

    // Note: MulByte leaves result_lo in ZP_TMP1+1 ($F3) and result_hi in ZP_TMP1 ($F2).
    // We verify via ZP memory reads rather than A/X register capture, since
    // the harness STA/STX relies on correct return address handling.

    [TestMethod]
    public void MulByte_6x7_Returns42()
    {
        var bus = RunByteRoutine(RuntimeAddresses.MulByte, 6, 7);
        Assert.AreEqual(42, bus.Read(0x0300), $"6*7 lo should be 42. ZP state: zp_hi={bus.Read(RuntimeAddresses.ZP_TMP1)}, zp_lo={bus.Read((ushort)(RuntimeAddresses.ZP_TMP1+1))}");
        Assert.AreEqual(0,  bus.Read(0x0301), $"6*7 hi should be 0");
    }

    [TestMethod]
    public void MulByte_15x17_Returns255()
    {
        // 15 * 17 = 255 (fits in 1 byte)
        var bus = RunByteRoutine(RuntimeAddresses.MulByte, 15, 17);
        Assert.AreEqual(255, bus.Read(0x0300), $"15*17=255, got {bus.Read(0x0300)}. zp_lo={bus.Read((ushort)(RuntimeAddresses.ZP_TMP1+1))}");
        Assert.AreEqual(0,   bus.Read(0x0301));
    }

    [TestMethod]
    public void MulByte_16x16_Returns256()
    {
        // 16 * 16 = 256 → A=0 (lo), X=1 (hi)
        var bus = RunByteRoutine(RuntimeAddresses.MulByte, 16, 16);
        Assert.AreEqual(0, bus.Read(0x0300), $"16*16 lo should be 0. zp_lo={bus.Read((ushort)(RuntimeAddresses.ZP_TMP1+1))}");
        Assert.AreEqual(1, bus.Read(0x0301), $"16*16 hi should be 1. zp_hi={bus.Read(RuntimeAddresses.ZP_TMP1)}");
    }

    [TestMethod]
    public void MulByte_0xAny_Returns0()
    {
        var bus = RunByteRoutine(RuntimeAddresses.MulByte, 0, 99);
        Assert.AreEqual(0, bus.Read(0x0300));
        Assert.AreEqual(0, bus.Read(0x0301));
    }

    // ── DivByte direct execution tests ────────────────────────────────────────
    // DivByte returns: A=quotient (from LDA ZP_TMP0), X=remainder (from TAX).
    // ZP_TMP0 ($F0) contains the quotient after the routine.

    [TestMethod]
    public void DivByte_84div2_Returns42()
    {
        var bus = RunByteRoutine(RuntimeAddresses.DivByte, 84, 2);
        Assert.AreEqual(42, bus.Read(0x0300), $"84/2 quotient. ZP_TMP0={bus.Read(RuntimeAddresses.ZP_TMP0)}");
        Assert.AreEqual(0,  bus.Read(0x0301), $"84/2 remainder");
    }

    [TestMethod]
    public void DivByte_47div10_Returns4rem7()
    {
        var bus = RunByteRoutine(RuntimeAddresses.DivByte, 47, 10);
        Assert.AreEqual(4, bus.Read(0x0300), $"47/10 quotient. ZP_TMP0={bus.Read(RuntimeAddresses.ZP_TMP0)}");
        Assert.AreEqual(7, bus.Read(0x0301), $"47/10 remainder");
    }

    [TestMethod]
    public void DivByte_255div1_Returns255()
    {
        var bus = RunByteRoutine(RuntimeAddresses.DivByte, 255, 1);
        Assert.AreEqual(255, bus.Read(0x0300));
        Assert.AreEqual(0,   bus.Read(0x0301));
    }

    // ── ModByte direct execution tests ────────────────────────────────────────

    [TestMethod]
    public void ModByte_47mod10_Returns7()
    {
        var bus = RunByteRoutine(RuntimeAddresses.ModByte, 47, 10);
        Assert.AreEqual(7, bus.Read(0x0300), $"47%10. A stored at $0300 = {bus.Read(0x0300)}");
    }

    [TestMethod]
    public void ModByte_100mod7_Returns2()
    {
        // 100 / 7 = 14 remainder 2
        var bus = RunByteRoutine(RuntimeAddresses.ModByte, 100, 7);
        Assert.AreEqual(2, bus.Read(0x0300), $"100%7={bus.Read(0x0300)}");
    }

    // ── MulInt direct execution tests ─────────────────────────────────────────

    [TestMethod]
    public void MulInt_100x10_Returns1000()
    {
        var bus = RunZpRoutine(RuntimeAddresses.MulInt, zp0lo: 100, zp0hi: 0, zp1lo: 10, zp1hi: 0);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(1000, result, $"100*10 should be 1000, got {result}");
    }

    [TestMethod]
    public void MulInt_300x200_Returns60000()
    {
        // 300 = 0x012C, 200 = 0x00C8, result = 60000 = 0xEA60
        var bus = RunZpRoutine(RuntimeAddresses.MulInt, zp0lo: 0x2C, zp0hi: 0x01, zp1lo: 0xC8, zp1hi: 0x00);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(60000, result, $"300*200 should be 60000, got {result}");
    }

    // ── DivInt direct execution tests ─────────────────────────────────────────

    [TestMethod]
    public void DivInt_1000div10_Returns100()
    {
        // 1000 = 0x03E8
        var bus = RunZpRoutine(RuntimeAddresses.DivInt, zp0lo: 0xE8, zp0hi: 0x03, zp1lo: 10, zp1hi: 0);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(100, result, $"1000/10 should be 100, got {result}");
    }

    // ── Fixed-point execution tests ────────────────────────────────────────────

    [TestMethod]
    public void FixedMul_2x3_Is6()
    {
        // 2.0 * 3.0 = 6.0 → 0x0200 * 0x0300 = 0x0600
        var bus = RunZpRoutine(RuntimeAddresses.FixedMul, zp0lo: 0x00, zp0hi: 0x02, zp1lo: 0x00, zp1hi: 0x03);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(0x0600, result, $"2.0 * 3.0 should be 0x0600 (6.0), got 0x{result:X4}");
    }

    [TestMethod]
    public void FixedMul_1_5x2_Is3()
    {
        // 1.5 * 2.0 = 3.0 → 0x0180 * 0x0200 = 0x0300
        var bus = RunZpRoutine(RuntimeAddresses.FixedMul, zp0lo: 0x80, zp0hi: 0x01, zp1lo: 0x00, zp1hi: 0x02);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(0x0300, result, $"1.5 * 2.0 should be 0x0300 (3.0), got 0x{result:X4}");
    }

    [TestMethod]
    public void FixedDiv_6div2_Is3()
    {
        // 6.0 / 2.0 = 3.0 → 0x0600 / 0x0200 = 0x0300
        var bus = RunZpRoutine(RuntimeAddresses.FixedDiv, zp0lo: 0x00, zp0hi: 0x06, zp1lo: 0x00, zp1hi: 0x02);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(0x0300, result, $"6.0 / 2.0 should be 0x0300 (3.0), got 0x{result:X4}");
    }

    [TestMethod]
    public void FixedDiv_1div4_Is0_25()
    {
        // 1.0 / 4.0 = 0.25 → 0x0100 / 0x0400 = 0x0040
        var bus = RunZpRoutine(RuntimeAddresses.FixedDiv, zp0lo: 0x00, zp0hi: 0x01, zp1lo: 0x00, zp1hi: 0x04);
        int result = bus.Read(RuntimeAddresses.ZP_TMP0) |
                     (bus.Read((ushort)(RuntimeAddresses.ZP_TMP0 + 1)) << 8);
        Assert.AreEqual(0x0040, result, $"1.0 / 4.0 should be 0x0040 (0.25), got 0x{result:X4}");
    }

    // ── End-to-end NCC compiler tests ────────────────────────────────────────
    //
    // These compile NCC programs and run them with the ROM installed at $C000.

    private const ushort LoadAddr   = 0x0280;
    private const int    MaxCycles  = 200_000;

    private static (Cpu cpu, BusDevice bus, CompileResult result)
        CompileAndRunWithRom(string source, int maxCycles = MaxCycles)
    {
        var compiler = new NccCompiler { BaseAddress = LoadAddr };
        var result   = compiler.Compile(source);
        Assert.IsTrue(result.Success, "Compile failed:\n" + string.Join("\n", result.Errors));

        var bus = new BusDevice(new byte[65536]);

        // Install the NCC ROM at $C000.
        byte[] rom = new NccRomBuilder().Build();
        for (int i = 0; i < rom.Length; i++)
            bus.Write((ushort)(0xC000 + i), rom[i]);

        // Load compiled code at LoadAddr.
        byte[] code = result.Code;
        for (int i = 0; i < code.Length; i++)
            bus.Write((ushort)(LoadAddr + i), code[i]);

        // Reset vector → LoadAddr.
        bus.Write(0xFFFC, (byte)(LoadAddr & 0xFF));
        bus.Write(0xFFFD, (byte)(LoadAddr >> 8));

        // BRK vector → $FF00 (halt).
        bus.Write(0xFFFE, 0x00);
        bus.Write(0xFFFF, 0xFF);
        bus.Write(0xFF00, 0x00);  // BRK

        var cpu = new Cpu(bus, E6502Type.Nmos);
        cpu.Boot();

        for (int i = 0; i < maxCycles; i++)
        {
            cpu.ExecuteNext();
            if (cpu.Pc == 0xFF00) break;
        }

        return (cpu, bus, result);
    }

    [TestMethod]
    public void E2E_ByteMultiply_6x7_Is42()
    {
        const string src = "byte x; void main() { x = 6 * 7; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(42, bus.Read(result.Symbols["x"].Address),
            "6 * 7 should be 42");
    }

    [TestMethod]
    public void E2E_ByteDivide_84div2_Is42()
    {
        const string src = "byte x; void main() { x = 84 / 2; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(42, bus.Read(result.Symbols["x"].Address),
            "84 / 2 should be 42");
    }

    [TestMethod]
    public void E2E_ByteModulo_47mod10_Is7()
    {
        const string src = "byte x; void main() { x = 47 % 10; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(7, bus.Read(result.Symbols["x"].Address),
            "47 % 10 should be 7");
    }

    [TestMethod]
    public void E2E_IntMultiply_100x10_Is1000()
    {
        const string src = "int x; void main() { x = 100 * 10; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        ushort xAddr = result.Symbols["x"].Address;
        int value = bus.Read(xAddr) | (bus.Read((ushort)(xAddr + 1)) << 8);
        Assert.AreEqual(1000, value, $"100 * 10 should be 1000, got {value}");
    }

    [TestMethod]
    public void E2E_ByteMultiplyChained()
    {
        // (2 * 3) * 7 = 42
        const string src = "byte x; byte t; void main() { t = 2 * 3; x = t * 7; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(42, bus.Read(result.Symbols["x"].Address));
    }

    [TestMethod]
    public void E2E_ByteDivideRemainder()
    {
        // 100 / 3 = 33 (remainder 1)
        const string src = "byte x; void main() { x = 100 / 3; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(33, bus.Read(result.Symbols["x"].Address));
    }

    [TestMethod]
    public void E2E_ByteModuloZeroRemainder()
    {
        // 42 % 7 = 0
        const string src = "byte x; void main() { x = 42 % 7; }";
        var (_, bus, result) = CompileAndRunWithRom(src);
        Assert.AreEqual(0, bus.Read(result.Symbols["x"].Address));
    }

    // ── ROM routine execution harnesses ─────────────────────────────────────
    //
    // RunByteRoutine: emits LDX #$FF; TXS; LDA #a; LDX #x; JSR entry; STA $0300; STX $0301; BRK
    // Results: A stored at address $0300, X stored at $0301.
    //
    // RunZpRoutine: sets ZP_TMP0/+1 = zp0 and ZP_TMP1/+1 = zp1 via direct
    // writes, then JSR entry; BRK.  Results remain in ZP_TMP0.

    private static BusDevice RunByteRoutine(ushort entryAddr, byte aIn, byte xIn)
    {
        var bus = new BusDevice(new byte[65536]);
        InstallRom(bus);

        // Mini-program at $0200:
        //   LDX #$FF       (2 bytes) — initialize stack pointer
        //   TXS            (1 byte)
        //   LDA #aIn       (2 bytes)
        //   LDX #xIn       (2 bytes)
        //   JSR entryAddr  (3 bytes)
        //   STA $0300      (3 bytes) — save A result to absolute address
        //   STX $0301      (3 bytes) — save X result to absolute address
        //   BRK            (1 byte)
        int p = 0x0200;
        bus.Write((ushort)(p++), 0xA2); bus.Write((ushort)(p++), 0xFF);       // LDX #$FF
        bus.Write((ushort)(p++), 0x9A);                                        // TXS
        bus.Write((ushort)(p++), 0xA9); bus.Write((ushort)(p++), aIn);        // LDA #aIn
        bus.Write((ushort)(p++), 0xA2); bus.Write((ushort)(p++), xIn);        // LDX #xIn
        bus.Write((ushort)(p++), 0x20);                                        // JSR
        bus.Write((ushort)(p++), (byte)(entryAddr & 0xFF));
        bus.Write((ushort)(p++), (byte)(entryAddr >> 8));
        bus.Write((ushort)(p++), 0x8D); bus.Write((ushort)(p++), 0x00); bus.Write((ushort)(p++), 0x03); // STA $0300
        bus.Write((ushort)(p++), 0x8E); bus.Write((ushort)(p++), 0x01); bus.Write((ushort)(p++), 0x03); // STX $0301
        bus.Write((ushort)(p++), 0x00);                                        // BRK

        RunCpu(bus, 0x0200);
        return bus;
    }

    private static BusDevice RunZpRoutine(ushort entryAddr,
        byte zp0lo, byte zp0hi, byte zp1lo, byte zp1hi)
    {
        var bus = new BusDevice(new byte[65536]);
        InstallRom(bus);

        // Pre-load ZP inputs via direct writes.
        bus.Write(RuntimeAddresses.ZP_TMP0, zp0lo);
        bus.Write((ushort)(RuntimeAddresses.ZP_TMP0 + 1), zp0hi);
        bus.Write(RuntimeAddresses.ZP_TMP1, zp1lo);
        bus.Write((ushort)(RuntimeAddresses.ZP_TMP1 + 1), zp1hi);

        // Mini-program: LDX #$FF; TXS; JSR entry; BRK
        int p = 0x0200;
        bus.Write((ushort)(p++), 0xA2); bus.Write((ushort)(p++), 0xFF);  // LDX #$FF
        bus.Write((ushort)(p++), 0x9A);                                    // TXS
        bus.Write((ushort)(p++), 0x20);                                    // JSR
        bus.Write((ushort)(p++), (byte)(entryAddr & 0xFF));
        bus.Write((ushort)(p++), (byte)(entryAddr >> 8));
        bus.Write((ushort)(p++), 0x00);                                    // BRK

        RunCpu(bus, 0x0200);
        return bus;
    }

    private static void InstallRom(BusDevice bus)
    {
        byte[] rom = new NccRomBuilder().Build();
        for (int i = 0; i < rom.Length; i++)
            bus.Write((ushort)(0xC000 + i), rom[i]);
    }

    private static void RunCpu(BusDevice bus, ushort entry)
    {
        // Reset vector → entry
        bus.Write(0xFFFC, (byte)(entry & 0xFF));
        bus.Write(0xFFFD, (byte)(entry >> 8));
        // BRK/IRQ vector → $FF00 (halt)
        bus.Write(0xFFFE, 0x00);
        bus.Write(0xFFFF, 0xFF);
        bus.Write(0xFF00, 0x00);  // BRK at halt address

        var cpu = new Cpu(bus, E6502Type.Nmos);
        cpu.Boot();

        for (int i = 0; i < 100_000; i++)
        {
            cpu.ExecuteNext();
            if (cpu.Pc == 0xFF00) break;
        }
    }
}
