namespace e6502.Avalonia.Compiler;

/// <summary>
/// Builds the NCC ROM image (16KB, $C000-$FFFF) programmatically.
///
/// Layout:
///   $C000-$C03E  Jump table (22 entries x 3 bytes each)
///   $C03F+       Runtime routine implementations (sequential)
///   $FFF9        Reset / IRQ / NMI handlers (near end)
///   $FFFA-$FFFF  6502 interrupt vectors
///
/// Calling conventions for runtime routines:
///   - MulByte:   A=multiplier, X=multiplicand  →  A=result_lo, X=result_hi
///   - DivByte:   A=dividend,   X=divisor        →  A=quotient,  X=remainder
///   - ModByte:   A=dividend,   X=divisor        →  A=remainder
///   - MulInt:    ZP_TMP0=left(16), ZP_TMP1=right(16) → ZP_TMP0=result(16)
///   - DivInt:    ZP_TMP0=dividend(16), ZP_TMP1=divisor(16) → ZP_TMP0=quotient(16)
///   - PlaySound: A=note, X=duration, Y=instrument (→ FioSrcL/H/EndL + FioCmdSound)
/// </summary>
public sealed class NccRomBuilder
{
    private const int RomSize = 16384;   // 16KB
    private const ushort RomBase = 0xC000;

    // Number of jump table slots (must match RuntimeAddresses entries)
    private const int JumpTableCount = 22;
    private const int JumpTableBytes = JumpTableCount * 3;  // 66 bytes

    // Hardware register constants (mirrors of VgcConstants, avoiding dependency)
    private const ushort RegStatus  = 0xA008;   // frame counter
    private const ushort RegCharOut = 0xA00E;   // character output
    private const ushort RegCharIn  = 0xA00F;   // character input
    private const ushort RegCmd     = 0xA010;   // VGC command
    private const byte   CmdGcls    = 0x07;     // graphics clear

    private const ushort FioCmd     = 0xB9A0;
    private const ushort FioSrcL    = 0xB9A4;   // note (for PlaySound)
    private const ushort FioSrcH    = 0xB9A5;   // duration
    private const ushort FioEndL    = 0xB9A6;   // instrument
    private const byte   FioCmdSound     = 0x0B;
    private const byte   FioCmdMusicPlay = 0x0E;
    private const byte   FioCmdMusicStop = 0x0F;
    private const byte   FioCmdVolume    = 0x0C;

    // Zero-page temporaries (match RuntimeAddresses and CodeGen)
    private const byte ZP_TMP0 = 0xF0;
    private const byte ZP_TMP1 = 0xF2;
    private const byte ZP_TMP2 = 0xF4;
    private const byte ZP_PTR0 = 0xF6;

    private readonly byte[] _rom = new byte[RomSize];
    private int _pos;  // write offset within _rom (0 = $C000)

    private ushort Addr => (ushort)(RomBase + _pos);

    // ── Emission helpers ─────────────────────────────────────────────────────

    private void Emit(byte b) => _rom[_pos++] = b;

    private void EmitWord(ushort w)
    {
        _rom[_pos++] = (byte)(w & 0xFF);
        _rom[_pos++] = (byte)(w >> 8);
    }

    private void OpJMP(ushort addr) { Emit(0x4C); EmitWord(addr); }
    private void OpJSR(ushort addr) { Emit(0x20); EmitWord(addr); }
    private void OpRTS()  => Emit(0x60);
    private void OpRTI()  => Emit(0x40);
    private void OpBRK()  => Emit(0x00);
    private void OpNOP()  => Emit(0xEA);
    private void OpSEI()  => Emit(0x78);
    private void OpCLD()  => Emit(0xD8);
    private void OpTXS()  => Emit(0x9A);
    private void OpTAX()  => Emit(0xAA);
    private void OpTXA()  => Emit(0x8A);
    private void OpTYA()  => Emit(0x98);
    private void OpPHA()  => Emit(0x48);
    private void OpPLA()  => Emit(0x68);
    private void OpCLC()  => Emit(0x18);
    private void OpSEC()  => Emit(0x38);
    private void OpDEX()  => Emit(0xCA);
    private void OpINX()  => Emit(0xE8);
    private void OpINY()  => Emit(0xC8);
    private void OpDEY()  => Emit(0x88);
    private void OpLSR_A() => Emit(0x4A);
    private void OpASL_A() => Emit(0x0A);
    private void OpROL_A() => Emit(0x2A);
    private void OpROR_A() => Emit(0x6A);

    private void OpLDA_Imm(byte v)   { Emit(0xA9); Emit(v); }
    private void OpLDA_ZP(byte zp)   { Emit(0xA5); Emit(zp); }
    private void OpLDA_Abs(ushort a) { Emit(0xAD); EmitWord(a); }
    private void OpLDA_IndY(byte zp) { Emit(0xB1); Emit(zp); }

    private void OpLDX_Imm(byte v)   { Emit(0xA2); Emit(v); }
    private void OpLDX_ZP(byte zp)   { Emit(0xA6); Emit(zp); }

    private void OpLDY_Imm(byte v)   { Emit(0xA0); Emit(v); }

    private void OpSTA_ZP(byte zp)   { Emit(0x85); Emit(zp); }
    private void OpSTA_Abs(ushort a) { Emit(0x8D); EmitWord(a); }
    private void OpSTA_IndY(byte zp) { Emit(0x91); Emit(zp); }

    private void OpSTX_Abs(ushort a) { Emit(0x8E); EmitWord(a); }
    private void OpSTY_Abs(ushort a) { Emit(0x8C); EmitWord(a); }

    private void OpSTX_ZP(byte zp)   { Emit(0x86); Emit(zp); }
    private void OpSTY_ZP(byte zp)   { Emit(0x84); Emit(zp); }

    private void OpADC_Imm(byte v)   { Emit(0x69); Emit(v); }
    private void OpADC_ZP(byte zp)   { Emit(0x65); Emit(zp); }

    private void OpSBC_ZP(byte zp)   { Emit(0xE5); Emit(zp); }

    private void OpCMP_ZP(byte zp)   { Emit(0xC5); Emit(zp); }
    private void OpCMP_Abs(ushort a) { Emit(0xCD); EmitWord(a); }
    private void OpCPY_ZP(byte zp)   { Emit(0xC4); Emit(zp); }

    private void OpINC_ZP(byte zp)   { Emit(0xE6); Emit(zp); }
    private void OpLSR_ZP(byte zp)   { Emit(0x46); Emit(zp); }
    private void OpASL_ZP(byte zp)   { Emit(0x06); Emit(zp); }
    private void OpROL_ZP(byte zp)   { Emit(0x26); Emit(zp); }
    private void OpROR_ZP(byte zp)   { Emit(0x66); Emit(zp); }

    private void OpEOR_ZP(byte zp)   { Emit(0x45); Emit(zp); }
    private void OpORA_ZP(byte zp)   { Emit(0x05); Emit(zp); }
    private void OpAND_Imm(byte v)   { Emit(0x29); Emit(v); }

    // Branch: emit opcode + placeholder offset byte; returns offset of the placeholder.
    private int OpBEQ() { Emit(0xF0); int i = _pos; Emit(0x00); return i; }
    private int OpBNE() { Emit(0xD0); int i = _pos; Emit(0x00); return i; }
    private int OpBCC() { Emit(0x90); int i = _pos; Emit(0x00); return i; }
    private int OpBCS() { Emit(0xB0); int i = _pos; Emit(0x00); return i; }
    private int OpBPL() { Emit(0x10); int i = _pos; Emit(0x00); return i; }

    // Patch a forward branch: fills in the relative offset from branch byte to CurrentAddr.
    private void PatchBranch(int offsetIdx)
    {
        // Branch offset is relative to the byte after the offset byte itself.
        int from = (int)(RomBase + offsetIdx + 1);
        int to   = (int)Addr;
        int rel  = to - from;
        if (rel < -128 || rel > 127)
            throw new InvalidOperationException(
                $"NccRomBuilder: branch out of range ({rel}) from ${from:X4} to ${to:X4}");
        _rom[offsetIdx] = (byte)(sbyte)rel;
    }

    // Patch absolute address into jump-table slot (3-byte JMP).
    private void PatchJMP(int slotOffset, ushort targetAddr)
    {
        _rom[slotOffset]     = 0x4C;  // JMP
        _rom[slotOffset + 1] = (byte)(targetAddr & 0xFF);
        _rom[slotOffset + 2] = (byte)(targetAddr >> 8);
    }

    // ── Public entry point ────────────────────────────────────────────────────

    public byte[] Build()
    {
        Array.Fill(_rom, (byte)0xEA);  // NOP fill (safe for stray execution)
        _pos = 0;

        // Step 1: Skip past the jump table (fill with NOPs for now; patched later).
        _pos = JumpTableBytes;  // 66 bytes

        // Step 2: Emit routine implementations, recording their addresses.
        var addrs = new ushort[JumpTableCount];

        addrs[0]  = Addr; EmitMulByte();
        addrs[1]  = Addr; EmitMulInt();
        addrs[2]  = Addr; EmitDivByte();
        addrs[3]  = Addr; EmitDivInt();
        addrs[4]  = Addr; EmitModByte();
        addrs[5]  = Addr; EmitPrint();
        addrs[6]  = Addr; EmitPrintChar();
        addrs[7]  = Addr; EmitGetKey();
        addrs[8]  = Addr; EmitWaitKey();
        addrs[9]  = Addr; EmitVSync();
        addrs[10] = Addr; EmitMemCpy();
        addrs[11] = Addr; EmitMemSet();
        addrs[12] = Addr; EmitGfxCmd();
        addrs[13] = Addr; EmitSpriteCmd();
        addrs[14] = Addr; EmitPlaySound();
        addrs[15] = Addr; EmitMusicPlay();
        addrs[16] = Addr; EmitMusicStop();
        addrs[17] = Addr; EmitSetVolume();
        addrs[18] = Addr; EmitFixedMul();
        addrs[19] = Addr; EmitFixedDiv();
        addrs[20] = Addr; EmitEditorEntry();

        // Emit handlers after routines, before vectors.
        ushort resetAddr = Addr; EmitResetHandler(addrs[20]);
        ushort irqAddr   = Addr; EmitIrqHandler();
        ushort nmiAddr   = Addr; EmitNmiHandler();

        // Step 3: Patch jump table. addrs[21] = self-loop for any unimplemented slot.
        ushort selfLoop = Addr;  // current position = unimplemented stub
        OpNOP(); OpRTS();        // safe no-op stub
        addrs[21] = selfLoop;

        _pos = 0;
        for (int i = 0; i < JumpTableCount; i++)
        {
            ushort target = i < addrs.Length ? addrs[i] : selfLoop;
            PatchJMP(_pos, target);
            _pos += 3;
        }

        // Step 4: Set vectors at $FFFA (offset 0x3FFA).
        SetWord(0x3FFA, nmiAddr);
        SetWord(0x3FFC, resetAddr);
        SetWord(0x3FFE, irqAddr);

        return (byte[])_rom.Clone();
    }

    private void SetWord(int offset, ushort addr)
    {
        _rom[offset]     = (byte)(addr & 0xFF);
        _rom[offset + 1] = (byte)(addr >> 8);
    }

    // ── MulByte: A × X → A=lo, X=hi ──────────────────────────────────────────
    //
    // Algorithm: classic 8×8→16 bit multiply using 8 shift-and-add iterations.
    //
    // ZP usage:
    //   ZP_TMP0   = multiplier (shifts right each iteration)
    //   ZP_TMP0+1 = accumulator hi byte of result (temp)
    //   ZP_TMP0+2 = result lo byte (shifts right via ROR)
    //   ZP_TMP1   = multiplicand (constant)
    //
    // After each iteration we rotate the result right, shifting the partial
    // product bit into the accumulator and the accumulator into the result lo.
    // This is the standard 6502 "right-shift multiply" pattern.

    private void EmitMulByte()
    {
        // Input: A = multiplier, X = multiplicand
        // Save inputs
        OpSTA_ZP(ZP_TMP0);           // ZP_TMP0   = multiplier
        OpSTX_ZP((byte)(ZP_TMP0 + 1)); // ZP_TMP0+1 = multiplicand
        OpLDA_Imm(0);
        OpSTA_ZP(ZP_TMP1);           // ZP_TMP1   = result_hi = 0
        OpSTA_ZP((byte)(ZP_TMP1 + 1)); // ZP_TMP1+1 = result_lo = 0
        OpLDX_Imm(8);                // 8 iterations

        // loop:
        ushort loopAddr = Addr;
        OpLSR_ZP(ZP_TMP0);           // shift multiplier right; LSB → carry
        int skipAdd = OpBCC();        // BCC no_add
        // Add multiplicand to result_hi
        OpCLC();
        OpLDA_ZP(ZP_TMP1);           // result_hi
        OpADC_ZP((byte)(ZP_TMP0 + 1)); // + multiplicand
        OpSTA_ZP(ZP_TMP1);
        // no_add:
        PatchBranch(skipAdd);
        // Shift 16-bit result right: result_hi → carry → result_lo via ROR
        OpLDA_ZP(ZP_TMP1);
        OpROR_A();
        OpSTA_ZP(ZP_TMP1);
        OpLDA_ZP((byte)(ZP_TMP1 + 1));
        OpROR_A();
        OpSTA_ZP((byte)(ZP_TMP1 + 1));
        OpDEX();
        // BNE loop
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;

        // Result: result_hi in ZP_TMP1, result_lo in ZP_TMP1+1
        // Return: A = lo, X = hi
        OpLDA_ZP((byte)(ZP_TMP1 + 1));  // A = lo
        OpLDX_ZP(ZP_TMP1);              // X = hi
        OpRTS();
    }

    // ── MulInt: ZP_TMP0(16) × ZP_TMP1(16) → ZP_TMP0(16) ─────────────────────
    //
    // 16×16→16 multiply (lower 16 bits of 32-bit product).
    // Uses repeated 16-bit shift-and-add, 16 iterations.
    //
    // ZP layout during execution:
    //   ZP_TMP0/+1  = multiplicand (will shift left)
    //   ZP_TMP1/+1  = multiplier   (will shift right)
    //   ZP_TMP2/+1  = result accumulator (starts 0)
    //   ZP_TMP2+2/+3 = scratch for multiplicand hi word

    private void EmitMulInt()
    {
        // We want: result = 0; for 16 bits: if (multiplier & 1) result += multiplicand; multiplicand <<= 1; multiplier >>= 1;
        // ZP_TMP0/+1 = multiplicand (input), ZP_TMP1/+1 = multiplier (input)
        // Use ZP_TMP2/+1 as result accumulator.

        // result = 0
        OpLDA_Imm(0);
        OpSTA_ZP(ZP_TMP2);
        OpSTA_ZP((byte)(ZP_TMP2 + 1));
        // loop count = 16
        OpLDX_Imm(16);

        ushort loopAddr = Addr;
        // Check bit 0 of multiplier
        OpLDA_ZP(ZP_TMP1);
        OpAND_Imm(0x01);
        int skipAdd = OpBEQ();  // BEQ no_add
        // result += multiplicand (16-bit)
        OpCLC();
        OpLDA_ZP(ZP_TMP2);
        OpADC_ZP(ZP_TMP0);
        OpSTA_ZP(ZP_TMP2);
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpADC_ZP((byte)(ZP_TMP0 + 1));
        OpSTA_ZP((byte)(ZP_TMP2 + 1));
        PatchBranch(skipAdd);   // no_add:
        // multiplicand <<= 1 (16-bit ASL)
        OpASL_ZP(ZP_TMP0);
        OpROL_ZP((byte)(ZP_TMP0 + 1));
        // multiplier >>= 1 (16-bit LSR)
        OpLSR_ZP((byte)(ZP_TMP1 + 1));
        // ROR ZP_TMP1 (carry from LSR above flows into bit 7)
        Emit(0x66); Emit(ZP_TMP1);  // ROR zp
        OpDEX();
        // BNE loop
        Emit(0xD0);
        int loopRel2 = _pos; Emit(0x00);
        int from2 = (int)(RomBase + loopRel2 + 1);
        int rel2 = (int)loopAddr - from2;
        _rom[loopRel2] = (byte)(sbyte)rel2;

        // Copy result to ZP_TMP0
        OpLDA_ZP(ZP_TMP2);
        OpSTA_ZP(ZP_TMP0);
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpSTA_ZP((byte)(ZP_TMP0 + 1));
        OpRTS();
    }

    // ── DivByte: A / X → A=quotient, X=remainder ──────────────────────────────
    //
    // Classic restoring division algorithm, 8 iterations.
    //
    // ZP layout:
    //   ZP_TMP0   = dividend (shifts left each iteration, becomes quotient)
    //   ZP_TMP0+1 = divisor (constant)
    //   A         = remainder accumulator (grows from MSB of dividend)

    private void EmitDivByte()
    {
        // Guard: divisor == 0 → return 0 quotient, 0 remainder
        // (we keep it simple; callers should not divide by zero)
        OpSTA_ZP(ZP_TMP0);           // ZP_TMP0   = dividend
        OpSTX_ZP((byte)(ZP_TMP0 + 1)); // ZP_TMP0+1 = divisor
        OpLDA_Imm(0);                // remainder = 0
        OpLDX_Imm(8);                // 8 iterations

        ushort loopAddr = Addr;
        // Shift dividend left, MSB → carry → rotate into remainder
        OpASL_ZP(ZP_TMP0);           // shift dividend left; MSB → carry
        OpROL_A();                   // carry → remainder LSB; remainder MSB → carry
        // Is remainder >= divisor?
        OpCMP_ZP((byte)(ZP_TMP0 + 1));
        int noSub = OpBCC();          // BCC no_sub
        // remainder -= divisor
        OpSBC_ZP((byte)(ZP_TMP0 + 1));
        // Set quotient bit: INC ZP_TMP0 sets bit 0 (already shifted so bit 0 = 0)
        OpINC_ZP(ZP_TMP0);
        PatchBranch(noSub);           // no_sub:
        OpDEX();
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;

        // A = remainder, ZP_TMP0 = quotient
        OpTAX();                     // X = remainder
        OpLDA_ZP(ZP_TMP0);           // A = quotient
        OpRTS();
    }

    // ── DivInt: ZP_TMP0(16) / ZP_TMP1(16) → ZP_TMP0(16) quotient ─────────────
    //
    // 16-bit restoring division. ZP_TMP0 = dividend → quotient.
    // ZP_TMP1 = divisor (unchanged).
    // ZP_TMP2/+1 = remainder accumulator.

    private void EmitDivInt()
    {
        // remainder = 0
        OpLDA_Imm(0);
        OpSTA_ZP(ZP_TMP2);
        OpSTA_ZP((byte)(ZP_TMP2 + 1));
        OpLDX_Imm(16);   // 16 iterations

        ushort loopAddr = Addr;
        // Shift dividend (ZP_TMP0) left into remainder (ZP_TMP2)
        // ASL ZP_TMP0 lo; ROL ZP_TMP0 hi; ROL ZP_TMP2 lo; ROL ZP_TMP2 hi
        OpASL_ZP(ZP_TMP0);
        OpROL_ZP((byte)(ZP_TMP0 + 1));
        OpROL_ZP(ZP_TMP2);
        OpROL_ZP((byte)(ZP_TMP2 + 1));

        // Compare remainder with divisor (16-bit): check if rem >= div
        // Subtract and see; restore if negative.
        // 16-bit: rem_hi - div_hi, then if hi equal compare lo
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpCMP_ZP((byte)(ZP_TMP1 + 1));
        int hiLt = OpBCC();  // rem_hi < div_hi → no subtract
        int hiGt = OpBNE();  // rem_hi > div_hi → subtract
        // hi bytes equal → check lo
        OpLDA_ZP(ZP_TMP2);
        OpCMP_ZP(ZP_TMP1);
        int loLt = OpBCC();  // lo < → no subtract
        PatchBranch(hiGt);   // hiGt lands here (rem_hi > div_hi or lo >=)
        // remainder -= divisor (16-bit)
        OpSEC();
        OpLDA_ZP(ZP_TMP2);
        OpSBC_ZP(ZP_TMP1);
        OpSTA_ZP(ZP_TMP2);
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpSBC_ZP((byte)(ZP_TMP1 + 1));
        OpSTA_ZP((byte)(ZP_TMP2 + 1));
        // Set quotient bit (INC the already-shifted dividend lo — bit 0 is now 0)
        OpINC_ZP(ZP_TMP0);
        int skipPatch = OpBPL();  // skip the two branches below
        PatchBranch(hiLt);        // no_sub from hi < path
        PatchBranch(loLt);        // no_sub from lo < path
        PatchBranch(skipPatch);   // fall through

        OpDEX();
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;

        // ZP_TMP0 already contains the quotient.
        OpRTS();
    }

    // ── ModByte: A % X → A ────────────────────────────────────────────────────

    private void EmitModByte()
    {
        // Call DivByte; remainder is in X; move to A.
        // Since this is a JMP-table stub, we can't directly fall-through to DivByte
        // (addresses are determined at build time). We inline the divide logic
        // by re-calling the actual DivByte implementation address. But we don't
        // know the DivByte implementation address here; we only have the jump-table
        // address (0xC006) which is stable.
        OpJSR(RuntimeAddresses.DivByte);  // A=quotient, X=remainder
        OpTXA();                           // remainder → A
        OpRTS();
    }

    // ── Print: print null-terminated string at ZP_PTR0 ($F6/$F7) ─────────────

    private void EmitPrint()
    {
        OpLDY_Imm(0);
        ushort loopAddr = Addr;
        OpLDA_IndY(ZP_PTR0);         // load byte from string
        int done = OpBEQ();           // null terminator → done
        OpSTA_Abs(RegCharOut);        // write to screen
        OpINY();
        // BNE loop (loop if Y != 0; handles up to 255 chars)
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;
        PatchBranch(done);            // done:
        OpRTS();
    }

    // ── PrintChar: print char in A ────────────────────────────────────────────

    private void EmitPrintChar()
    {
        OpSTA_Abs(RegCharOut);
        OpRTS();
    }

    // ── GetKey: non-blocking key read → A ────────────────────────────────────

    private void EmitGetKey()
    {
        OpLDA_Abs(RegCharIn);
        OpRTS();
    }

    // ── WaitKey: blocking key read → A ───────────────────────────────────────

    private void EmitWaitKey()
    {
        ushort loopAddr = Addr;
        OpLDA_Abs(RegCharIn);
        int notZero = OpBNE();  // non-zero key → done (BNE forward)
        // BEQ loop (back-branch)
        Emit(0xF0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;
        PatchBranch(notZero);  // notZero:
        OpRTS();
    }

    // ── VSync: wait for frame counter to change ───────────────────────────────

    private void EmitVSync()
    {
        // Load current frame counter snapshot
        OpLDA_Abs(RegStatus);
        // loop: compare with current counter; loop if equal
        ushort loopAddr = Addr;
        OpCMP_Abs(RegStatus);
        // BEQ loop (back-branch)
        Emit(0xF0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;
        OpRTS();
    }

    // ── MemCpy: copy ZP_TMP2 bytes from ZP_PTR0 to ZP_TMP0 ───────────────────
    //
    // ZP_PTR0 ($F6/$F7) = source pointer
    // ZP_TMP0 ($F0/$F1) = dest pointer
    // ZP_TMP2 ($F4)     = byte count (max 255)

    private void EmitMemCpy()
    {
        OpLDY_Imm(0);
        ushort loopAddr = Addr;
        // Check if Y == count
        OpCPY_ZP(ZP_TMP2);
        int done = OpBEQ();            // BEQ done
        OpLDA_IndY(ZP_PTR0);           // load from source
        OpSTA_IndY(ZP_TMP0);           // store to dest
        OpINY();
        // BNE loop (handles up to 255 bytes; loop assumes count <= 255)
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;
        PatchBranch(done);             // done:
        OpRTS();
    }

    // ── MemSet: fill ZP_TMP1 bytes at ZP_PTR0 with A ─────────────────────────
    //
    // A         = fill byte
    // ZP_PTR0   = destination pointer
    // ZP_TMP1   = byte count

    private void EmitMemSet()
    {
        OpSTA_ZP(ZP_TMP2);            // save fill value
        OpLDY_Imm(0);
        ushort loopAddr = Addr;
        OpCPY_ZP(ZP_TMP1);
        int done = OpBEQ();
        OpLDA_ZP(ZP_TMP2);            // reload fill value
        OpSTA_IndY(ZP_PTR0);
        OpINY();
        Emit(0xD0);
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;
        PatchBranch(done);
        OpRTS();
    }

    // ── GfxCmd: write A to VGC command register ($A010) ──────────────────────
    // Parameters must be pre-loaded into $A011-$A01E by the caller.

    private void EmitGfxCmd()
    {
        OpSTA_Abs(RegCmd);
        OpRTS();
    }

    // ── SpriteCmd: same interface as GfxCmd ──────────────────────────────────

    private void EmitSpriteCmd()
    {
        OpSTA_Abs(RegCmd);
        OpRTS();
    }

    // ── PlaySound: A=note, X=duration, Y=instrument ───────────────────────────
    //
    // Maps to FileIoController DoSound: FioSrcL=note, FioSrcH=duration, FioEndL=instrument.

    private void EmitPlaySound()
    {
        OpSTA_Abs(FioSrcL);          // note → $B9A4
        OpSTX_Abs(FioSrcH);          // duration → $B9A5
        OpSTY_Abs(FioEndL);          // instrument → $B9A6
        OpLDA_Imm(FioCmdSound);      // command = 0x0B
        OpSTA_Abs(FioCmd);           // trigger
        OpRTS();
    }

    // ── MusicPlay ─────────────────────────────────────────────────────────────

    private void EmitMusicPlay()
    {
        OpLDA_Imm(FioCmdMusicPlay);
        OpSTA_Abs(FioCmd);
        OpRTS();
    }

    // ── MusicStop ─────────────────────────────────────────────────────────────

    private void EmitMusicStop()
    {
        OpLDA_Imm(FioCmdMusicStop);
        OpSTA_Abs(FioCmd);
        OpRTS();
    }

    // ── SetVolume: A=0-15 → FioSrcL + FioCmdVolume ───────────────────────────

    private void EmitSetVolume()
    {
        OpSTA_Abs(FioSrcL);          // level → $B9A4
        OpLDA_Imm(FioCmdVolume);
        OpSTA_Abs(FioCmd);
        OpRTS();
    }

    // ── FixedMul: 8.8 × 8.8 → 8.8 (ZP_TMP0 × ZP_TMP1 → ZP_TMP0) ────────────
    //
    // 8.8 fixed-point: treat ZP_TMP0 and ZP_TMP1 as 16-bit integers.
    // Result = (ZP_TMP0 * ZP_TMP1) >> 8 (keep middle 16 bits of 32-bit product).
    //
    // We compute this as a 16×16 multiply into a 32-bit accumulator, then
    // extract bits [23:8] as the result.
    //
    // Scratch ZP for fixed-point routines ($FA-$FF, after ZP_FP at $F8/$F9).
    private const byte ZP_ACC0 = 0xFA;  // 4-6 bytes scratch: $FA-$FF

    // ── FixedMul: 8.8 × 8.8 → 8.8 (ZP_TMP0 * ZP_TMP1 → ZP_TMP0) ────────────
    //
    // Standard right-shifting accumulator multiply (16×16→32):
    //   acc[4] = 0;  multiplicand = TMP0 (unchanged);  multiplier = TMP1
    //   for 16 bits:
    //     LSR multiplier (bit 0 → carry)
    //     if carry: acc[2..3] += multiplicand
    //     ROR acc[3..0]   (carries addition overflow into upper bits)
    //   result = acc[1]:acc[2]  (bits 8..23 of 32-bit product)

    private void EmitFixedMul()
    {
        // Zero 32-bit accumulator at ZP_ACC0..+3
        OpLDA_Imm(0);
        OpSTA_ZP(ZP_ACC0);
        OpSTA_ZP((byte)(ZP_ACC0 + 1));
        OpSTA_ZP((byte)(ZP_ACC0 + 2));
        OpSTA_ZP((byte)(ZP_ACC0 + 3));

        OpLDX_Imm(16);

        ushort loopAddr = Addr;

        // Shift multiplier right; bit 0 → carry
        OpLSR_ZP((byte)(ZP_TMP1 + 1));
        OpROR_ZP(ZP_TMP1);
        int skipAdd = OpBCC();  // if bit was 0, skip add

        // acc[2..3] += multiplicand (TMP0)
        OpCLC();
        OpLDA_ZP((byte)(ZP_ACC0 + 2));
        OpADC_ZP(ZP_TMP0);
        OpSTA_ZP((byte)(ZP_ACC0 + 2));
        OpLDA_ZP((byte)(ZP_ACC0 + 3));
        OpADC_ZP((byte)(ZP_TMP0 + 1));
        OpSTA_ZP((byte)(ZP_ACC0 + 3));

        PatchBranch(skipAdd);  // no_add:

        // ROR entire 32-bit accumulator (carry from add propagates)
        OpROR_ZP((byte)(ZP_ACC0 + 3));
        OpROR_ZP((byte)(ZP_ACC0 + 2));
        OpROR_ZP((byte)(ZP_ACC0 + 1));
        OpROR_ZP(ZP_ACC0);

        OpDEX();
        Emit(0xD0);  // BNE
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;

        // Extract bits [23:8]: byte 1 = lo, byte 2 = hi
        OpLDA_ZP((byte)(ZP_ACC0 + 1));
        OpSTA_ZP(ZP_TMP0);
        OpLDA_ZP((byte)(ZP_ACC0 + 2));
        OpSTA_ZP((byte)(ZP_TMP0 + 1));
        OpRTS();
    }

    // ── FixedDiv: 8.8 / 8.8 → 8.8 (ZP_TMP0 / ZP_TMP1 → ZP_TMP0) ────────────
    //
    // (a / b) in 8.8 = (a << 8) / b, producing a 16-bit quotient.
    // We form a 24-bit shifted dividend, then do 24 iterations of restoring
    // division against the 16-bit divisor, accumulating a 24-bit quotient.
    // The lower 16 bits of the quotient are the 8.8 result.
    //
    // ZP layout:
    //   ZP_TMP0/+1   = dividend (input), then result (output)
    //   ZP_TMP1/+1   = divisor (input, unchanged)
    //   ZP_ACC0..+2   = 24-bit shifted dividend (3 bytes, $FA-$FC)
    //   ZP_ACC0+3..+5 = 24-bit quotient (3 bytes, $FD-$FF)
    //   ZP_TMP2/+1   = 16-bit remainder

    private void EmitFixedDiv()
    {
        // Build 24-bit shifted dividend (a << 8) in ZP_ACC0..+2
        OpLDA_Imm(0);
        OpSTA_ZP(ZP_ACC0);                        // low byte = 0
        OpLDA_ZP(ZP_TMP0);
        OpSTA_ZP((byte)(ZP_ACC0 + 1));            // mid = original lo
        OpLDA_ZP((byte)(ZP_TMP0 + 1));
        OpSTA_ZP((byte)(ZP_ACC0 + 2));            // high = original hi

        // Zero 24-bit quotient at $FD/$FE/$FF
        OpLDA_Imm(0);
        OpSTA_ZP((byte)(ZP_ACC0 + 3));
        OpSTA_ZP((byte)(ZP_ACC0 + 4));
        OpSTA_ZP((byte)(ZP_ACC0 + 5));

        // Zero 16-bit remainder
        OpSTA_ZP(ZP_TMP2);
        OpSTA_ZP((byte)(ZP_TMP2 + 1));

        OpLDX_Imm(24);  // 24 iterations

        ushort loopAddr = Addr;

        // Shift quotient left (3 bytes)
        OpASL_ZP((byte)(ZP_ACC0 + 3));
        OpROL_ZP((byte)(ZP_ACC0 + 4));
        OpROL_ZP((byte)(ZP_ACC0 + 5));

        // Shift 24-bit dividend left; MSB → carry → into remainder
        OpASL_ZP(ZP_ACC0);
        OpROL_ZP((byte)(ZP_ACC0 + 1));
        OpROL_ZP((byte)(ZP_ACC0 + 2));
        OpROL_ZP(ZP_TMP2);
        OpROL_ZP((byte)(ZP_TMP2 + 1));

        // Compare remainder with divisor (16-bit)
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpCMP_ZP((byte)(ZP_TMP1 + 1));
        int hiLt = OpBCC();   // rem_hi < div_hi → no_sub
        int hiGt = OpBNE();   // rem_hi > div_hi → do_sub
        // hi equal → compare lo
        OpLDA_ZP(ZP_TMP2);
        OpCMP_ZP(ZP_TMP1);
        int loLt = OpBCC();   // rem_lo < div_lo → no_sub

        PatchBranch(hiGt);    // do_sub:
        // remainder -= divisor
        OpSEC();
        OpLDA_ZP(ZP_TMP2);
        OpSBC_ZP(ZP_TMP1);
        OpSTA_ZP(ZP_TMP2);
        OpLDA_ZP((byte)(ZP_TMP2 + 1));
        OpSBC_ZP((byte)(ZP_TMP1 + 1));
        OpSTA_ZP((byte)(ZP_TMP2 + 1));
        // Set quotient bit (bit 0 was 0 from ASL)
        OpINC_ZP((byte)(ZP_ACC0 + 3));

        PatchBranch(hiLt);    // no_sub:
        PatchBranch(loLt);

        OpDEX();
        Emit(0xD0);  // BNE
        int loopRel = _pos; Emit(0x00);
        int from = (int)(RomBase + loopRel + 1);
        int rel = (int)loopAddr - from;
        _rom[loopRel] = (byte)(sbyte)rel;

        // Extract lower 16 bits of 24-bit quotient into ZP_TMP0
        OpLDA_ZP((byte)(ZP_ACC0 + 3));
        OpSTA_ZP(ZP_TMP0);
        OpLDA_ZP((byte)(ZP_ACC0 + 4));
        OpSTA_ZP((byte)(ZP_TMP0 + 1));
        OpRTS();
    }

    // ── EditorEntry: Phase 8 stub ─────────────────────────────────────────────

    private void EmitEditorEntry(ushort editorAddr = 0)
    {
        // Stub: loop forever (or just RTS for testing).
        // Will be replaced in Phase 8.
        OpRTS();
    }

    // ── Reset handler ────────────────────────────────────────────────────────

    private void EmitResetHandler(ushort editorEntryAddr)
    {
        OpSEI();                      // disable interrupts
        OpCLD();                      // clear decimal mode
        OpLDX_Imm(0xFF);
        OpTXS();                      // reset stack pointer
        // Clear graphics screen
        OpLDA_Imm(CmdGcls);
        OpSTA_Abs(RegCmd);
        // Set background color to black (0) and foreground to white (1)
        OpLDA_Imm(0);
        OpSTA_Abs(0xA001);            // RegBgCol
        OpLDA_Imm(1);
        OpSTA_Abs(0xA002);            // RegFgCol
        // Jump to editor entry
        OpJMP(editorEntryAddr);
    }

    // ── IRQ handler ──────────────────────────────────────────────────────────

    private void EmitIrqHandler()
    {
        OpRTI();
    }

    // ── NMI handler ──────────────────────────────────────────────────────────

    private void EmitNmiHandler()
    {
        OpRTI();
    }

}
