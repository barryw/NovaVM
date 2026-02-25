namespace e6502.Avalonia.Compiler;

/// <summary>
/// Low-level byte emitter. Tracks a base address and emits 6502 machine code
/// into an internal buffer. All addresses are absolute unless noted.
/// </summary>
public sealed class CodeEmitter
{
    private readonly List<byte> _code = new();
    private readonly ushort _baseAddress;

    public CodeEmitter(ushort baseAddress = 0x0280)
    {
        _baseAddress = baseAddress;
    }

    public ushort CurrentAddress => (ushort)(_baseAddress + _code.Count);
    public int Length => _code.Count;
    public byte[] ToArray() => _code.ToArray();

    // ── Core emission ────────────────────────────────────────────────────────

    public void Emit(byte b) => _code.Add(b);

    public void EmitWord(ushort word)
    {
        _code.Add((byte)(word & 0xFF));
        _code.Add((byte)(word >> 8));
    }

    /// <summary>Emit two placeholder bytes; returns the buffer index for later patching.</summary>
    public int EmitPlaceholder()
    {
        int idx = _code.Count;
        _code.Add(0x00);
        _code.Add(0x00);
        return idx;
    }

    /// <summary>Patch a previously emitted word at the given buffer index.</summary>
    public void Patch(int index, ushort address)
    {
        _code[index]     = (byte)(address & 0xFF);
        _code[index + 1] = (byte)(address >> 8);
    }

    /// <summary>Patch a single byte at the given buffer index.</summary>
    public void PatchByte(int index, byte value) => _code[index] = value;

    // ── Load / Store ─────────────────────────────────────────────────────────

    public void LDA_Imm(byte val)       { Emit(0xA9); Emit(val); }
    public void LDA_Abs(ushort addr)    { Emit(0xAD); EmitWord(addr); }
    public void LDA_ZP(byte addr)       { Emit(0xA5); Emit(addr); }
    public void LDA_AbsX(ushort addr)   { Emit(0xBD); EmitWord(addr); }
    public void LDA_AbsY(ushort addr)   { Emit(0xB9); EmitWord(addr); }
    public void LDA_ZPX(byte addr)      { Emit(0xB5); Emit(addr); }
    public void LDA_IndY(byte zpAddr)   { Emit(0xB1); Emit(zpAddr); }

    public void LDX_Imm(byte val)       { Emit(0xA2); Emit(val); }
    public void LDX_Abs(ushort addr)    { Emit(0xAE); EmitWord(addr); }
    public void LDX_ZP(byte addr)       { Emit(0xA6); Emit(addr); }

    public void LDY_Imm(byte val)       { Emit(0xA0); Emit(val); }
    public void LDY_Abs(ushort addr)    { Emit(0xAC); EmitWord(addr); }

    public void STA_Abs(ushort addr)    { Emit(0x8D); EmitWord(addr); }
    public void STA_ZP(byte addr)       { Emit(0x85); Emit(addr); }
    public void STA_AbsX(ushort addr)   { Emit(0x9D); EmitWord(addr); }
    public void STA_AbsY(ushort addr)   { Emit(0x99); EmitWord(addr); }
    public void STA_ZPX(byte addr)      { Emit(0x95); Emit(addr); }
    public void STA_IndY(byte zpAddr)   { Emit(0x91); Emit(zpAddr); }

    public void STX_Abs(ushort addr)    { Emit(0x8E); EmitWord(addr); }
    public void STX_ZP(byte addr)       { Emit(0x86); Emit(addr); }

    public void STY_Abs(ushort addr)    { Emit(0x8C); EmitWord(addr); }
    public void STY_ZP(byte addr)       { Emit(0x84); Emit(addr); }

    // ── Stack ────────────────────────────────────────────────────────────────

    public void PHA()   => Emit(0x48);
    public void PLA()   => Emit(0x68);
    // PHX/PLX are 65C02; use TXA/PHA + PLA/TAX for NMOS compatibility.
    public void PHX()   { TXA(); PHA(); }
    public void PLX()   { PLA(); TAX(); }
    public void PHY()   { TYA(); PHA(); }
    public void PLY()   { PLA(); TAY(); }

    // ── Arithmetic ───────────────────────────────────────────────────────────

    public void ADC_Imm(byte val)       { Emit(0x69); Emit(val); }
    public void ADC_Abs(ushort addr)    { Emit(0x6D); EmitWord(addr); }
    public void ADC_ZP(byte addr)       { Emit(0x65); Emit(addr); }
    public void SBC_Imm(byte val)       { Emit(0xE9); Emit(val); }
    public void SBC_Abs(ushort addr)    { Emit(0xED); EmitWord(addr); }
    public void SBC_ZP(byte addr)       { Emit(0xE5); Emit(addr); }

    public void CLC()   => Emit(0x18);
    public void SEC()   => Emit(0x38);
    public void CLD()   => Emit(0xD8);
    public void SED()   => Emit(0xF8);

    // ── Logic ────────────────────────────────────────────────────────────────

    public void AND_Imm(byte val)       { Emit(0x29); Emit(val); }
    public void AND_Abs(ushort addr)    { Emit(0x2D); EmitWord(addr); }
    public void AND_ZP(byte addr)       { Emit(0x25); Emit(addr); }
    public void ORA_Imm(byte val)       { Emit(0x09); Emit(val); }
    public void ORA_Abs(ushort addr)    { Emit(0x0D); EmitWord(addr); }
    public void ORA_ZP(byte addr)       { Emit(0x05); Emit(addr); }
    public void EOR_Imm(byte val)       { Emit(0x49); Emit(val); }
    public void EOR_Abs(ushort addr)    { Emit(0x4D); EmitWord(addr); }
    public void EOR_ZP(byte addr)       { Emit(0x45); Emit(addr); }

    // ── Shifts ───────────────────────────────────────────────────────────────

    public void ASL_A()     => Emit(0x0A);
    public void LSR_A()     => Emit(0x4A);
    public void ROL_A()     => Emit(0x2A);
    public void ROR_A()     => Emit(0x6A);

    // ── Compare ──────────────────────────────────────────────────────────────

    public void CMP_Imm(byte val)       { Emit(0xC9); Emit(val); }
    public void CMP_Abs(ushort addr)    { Emit(0xCD); EmitWord(addr); }
    public void CMP_ZP(byte addr)       { Emit(0xC5); Emit(addr); }
    public void CPX_Imm(byte val)       { Emit(0xE0); Emit(val); }
    public void CPX_Abs(ushort addr)    { Emit(0xEC); EmitWord(addr); }
    public void CPY_Imm(byte val)       { Emit(0xC0); Emit(val); }
    public void CPY_Abs(ushort addr)    { Emit(0xCC); EmitWord(addr); }

    // ── Branches ─────────────────────────────────────────────────────────────
    // Each emits the opcode + a placeholder offset byte.
    // Returns the buffer index of the offset byte for later patching.

    public int BEQ()  { Emit(0xF0); int i = _code.Count; Emit(0x00); return i; }
    public int BNE()  { Emit(0xD0); int i = _code.Count; Emit(0x00); return i; }
    public int BCC()  { Emit(0x90); int i = _code.Count; Emit(0x00); return i; }
    public int BCS()  { Emit(0xB0); int i = _code.Count; Emit(0x00); return i; }
    public int BMI()  { Emit(0x30); int i = _code.Count; Emit(0x00); return i; }
    public int BPL()  { Emit(0x10); int i = _code.Count; Emit(0x00); return i; }

    /// <summary>
    /// Patches a branch offset byte at <paramref name="index"/> to jump to
    /// <see cref="CurrentAddress"/>. The branch instruction is at index-1.
    /// Throws if out of range (-128..+127).
    /// </summary>
    public void PatchBranch(int index)
    {
        // The branch offset is relative to the address of the instruction
        // *after* the branch (i.e. index + 1 in the buffer = base + index + 1).
        int from = (int)_baseAddress + index + 1;
        int to   = (int)CurrentAddress;
        int rel  = to - from;
        if (rel < -128 || rel > 127)
            throw new InvalidOperationException(
                $"Branch out of range: offset {rel} from ${from:X4} to ${to:X4}");
        _code[index] = (byte)(sbyte)rel;
    }

    // ── Jump / Call ──────────────────────────────────────────────────────────

    public void JMP(ushort addr)    { Emit(0x4C); EmitWord(addr); }

    /// <summary>Emit JMP with placeholder address; returns the buffer index of the address bytes.</summary>
    public int JMP_Placeholder()    { Emit(0x4C); return EmitPlaceholder(); }

    public void JSR(ushort addr)    { Emit(0x20); EmitWord(addr); }
    public void RTS()               => Emit(0x60);
    public void BRK()               => Emit(0x00);

    // ── Transfer ─────────────────────────────────────────────────────────────

    public void TAX()   => Emit(0xAA);
    public void TAY()   => Emit(0xA8);
    public void TXA()   => Emit(0x8A);
    public void TYA()   => Emit(0x98);
    public void TSX()   => Emit(0xBA);
    public void TXS()   => Emit(0x9A);

    // ── Inc / Dec ────────────────────────────────────────────────────────────

    public void INC_Abs(ushort addr)    { Emit(0xEE); EmitWord(addr); }
    public void INC_ZP(byte addr)       { Emit(0xE6); Emit(addr); }
    public void DEC_Abs(ushort addr)    { Emit(0xCE); EmitWord(addr); }
    public void DEC_ZP(byte addr)       { Emit(0xC6); Emit(addr); }
    public void INX()   => Emit(0xE8);
    public void DEX()   => Emit(0xCA);
    public void INY()   => Emit(0xC8);
    public void DEY()   => Emit(0x88);

    // ── Misc ─────────────────────────────────────────────────────────────────

    public void NOP()   => Emit(0xEA);
    public void SEI()   => Emit(0x78);
    public void CLI_()  => Emit(0x58);  // trailing _ to avoid keyword conflict
    public void RTI()   => Emit(0x40);
}
