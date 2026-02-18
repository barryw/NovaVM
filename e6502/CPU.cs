using KDS.e6502.OpCodes;
using KDS.e6502.Utility;

namespace KDS.e6502
{
    public enum E6502Type
    {
        Cmos,
        Nmos
    };

    public sealed class Cpu
    {
        // Main Register
        private byte A { get; set; }

        // Index Registers
        private byte X { get; set; }
        private byte Y { get; set; }

        // Program Counter
        public ushort Pc { get; private set; }

        // Stack Pointer
        // Memory location is hard coded to 0x01xx
        // Stack is descending (decrement on push, increment on pop)
        // 6502 is an empty stack so SP points to where next value is stored
        private byte Sp { get; set; }

        // Status Registers (in order bit 7 to 0)
        private bool Nf { get; set; }    // negative flag (N)

        private bool Vf { get; set; }    // overflow flag (V)
        // bit 5 is unused
        // bit 4 is the break flag however it is not a physical flag in the CPU
        private bool Df { get; set; }    // binary coded decimal flag (D)
        private bool If { get; set; }    // interrupt flag (I)
        private bool Zf { get; set; }    // zero flag (Z)
        private bool Cf { get; set; }    // carry flag (C)

        // Flag for hardware interrupt (IRQ)
        public bool IrqWaiting { get; set; }
        // Flag for non maskable interrupt (NMI)
        public bool NmiWaiting { get; set; }

        // List of op codes and their attributes
        private readonly OpCodeTable _opCodeTable;

        // The current opcode
        private OpCodeRecord _currentOp;

        private readonly E6502Type _cpuType;

        private bool _prefetched;
        private int _prefetchedOperand;

        public IBusDevice SystemBus { get; }

        public Cpu(IBusDevice bus, E6502Type cpuType = E6502Type.Nmos)
        {
            _opCodeTable = new OpCodeTable();
            _currentOp = new OpCodeRecord();

            // Set these on instantiation so they are known values when using this object in testing.
            // Real programs should explicitly load these values before using them.
            A = 0;
            X = 0;
            Y = 0;
            Sp = 0;
            Pc = 0;
            Nf = false;
            Vf = false;
            Df = false;
            If = true;
            Zf = false;
            Cf = false;
            NmiWaiting = false;
            IrqWaiting = false;
            _cpuType = cpuType;
            SystemBus = bus;
        }

        public void Boot()
        {
            // On reset the addresses 0xfffc and 0xfffd are read and PC is loaded with this value.
            // It is expected that the initial program loaded will have these values set to something.
            // Most 6502 systems contain ROM in the upper region (around 0xe000-0xffff)
            Boot(ReadWord(0xfffc));
        }

        public void Boot(ushort pc)
        {
            Pc = pc;
            If = true;
            NmiWaiting = false;
            IrqWaiting = false;
        }

        /// <summary>
        /// Without executing the instruction determine how many clocks the next instruction will take.
        /// </summary>
        /// <returns>how many clock cycles for the next instruction</returns>
        public int ClocksForNext()
        {
            var clocks = 0;

            if (ProcessInterrupts())
            {
                clocks = 6;
            }

            _currentOp = _opCodeTable.OpCodes[SystemBus.Read(Pc)];
            _prefetchedOperand = GetOperand(_currentOp.AddressMode, out bool crossBoundary);

            clocks += _currentOp.Cycles + ClocksForCmos() + ClocksForBranching();
            if (crossBoundary) clocks++;

            _prefetched = true;
            return clocks;
        }

        private int ClocksForCmos()
        {
            var clocks = 0;
            if (_cpuType != E6502Type.Cmos) return clocks;
            switch (_currentOp.OpCode)
            {
                // CMOS fixes a bug in this op code which results in an extra clock cycle
                case 0x6c:
                    clocks++;
                    break;

                // extra clock cycle on CMOS in decimal mode
                case 0x7d:
                case 0xfd:
                    if (Df) clocks++;
                    break;

                // On 65C02 (abs,X) takes one less clock cycle (but still add back 1 if page boundary crossed)
                case 0x1e:
                case 0x3e:
                case 0x5e:
                case 0x7e:
                    clocks--;
                    break;

            }
            return clocks;
        }
        private int ClocksForBranching()
        {
            // Account for extra cycles if a branch is taken
            var clocks = _currentOp.OpCode switch
            {
                // BCC - branch on carry clear
                0x90 => PrefetchBranch(!Cf),
                // BCS - branch on carry set
                0xb0 => PrefetchBranch(Cf),
                // BEQ - branch on zero
                0xf0 => PrefetchBranch(Zf),
                // BMI - branch on negative
                0x30 => PrefetchBranch(Nf),
                // BNE - branch on non zero
                0xd0 => PrefetchBranch(!Zf),
                // BPL - branch on non negative
                0x10 => PrefetchBranch(!Nf),
                // BRA - unconditional branch to immediate address
                // NOTE: In OpcodeList.txt the number of clock cycles is one less than the documentation.
                // This is because CheckBranch() adds one when a branch is taken, which in this case is always.
                0x80 => PrefetchBranch(true),
                // BVC - branch on overflow clear
                0x50 => PrefetchBranch(!Vf),
                // BVS - branch on overflow set
                0x70 => PrefetchBranch(Vf),
                _ => 0
            };
            return clocks;
        }
        private int PrefetchBranch(bool flag)
        {
            if (!flag) return 0;
            // extra cycle if branch destination is a different page than
            // the next instruction
            return (Pc & 0xff00) != ((Pc + _prefetchedOperand) & 0xff00) ? 2 : 1;
        }

        // returns # of clock cycles needed to execute the instruction
        public void ExecuteNext()
        {
            if (!_prefetched) ProcessInterrupts();
            ExecuteInstruction();
            _prefetched = false;
        }

        private bool ProcessInterrupts()
        {
            // Check for non maskable interrupt (has higher priority over IRQ)
            if (NmiWaiting)
            {
                DoIrq(0xfffa);
                NmiWaiting = false;
                return true;
            }
            // Check for hardware interrupt, if enabled
            if (If) return false;
            if (!IrqWaiting) return false;
            DoIrq(0xfffe);
            IrqWaiting = false;
            return true;
        }

        private void ExecuteInstruction()
        {
            int result;
            int oper;
            if (_prefetched)
            {
                oper = _prefetchedOperand;
            }
            else
            {
                _currentOp = _opCodeTable.OpCodes[SystemBus.Read(Pc)];
                oper = GetOperand(_currentOp.AddressMode, out _);
            }

            switch (_currentOp.OpCode)
            {
                // ADC - add memory to accumulator with carry
                // A+M+C -> A,C (NZCV)
                case 0x61:
                case 0x65:
                case 0x69:
                case 0x6d:
                case 0x71:
                case 0x72:
                case 0x75:
                case 0x79:
                case 0x7d:

                    if (Df)
                    {
                        result = CpuMath.HexToBcd(A) + CpuMath.HexToBcd((byte)oper);
                        if (Cf) result++;

                        Cf = (result > 99);

                        if (result > 99)
                        {
                            result -= 100;
                        }
                        Zf = (result == 0);

                        // convert decimal result to hex BCD result
                        A = CpuMath.BcdToHex(result);

                        // Unlike ZF and CF, the NF flag represents the MSB after conversion
                        // to BCD.
                        Nf = (A > 0x7f);
                    }
                    else
                    {
                        Adc((byte)oper);
                    }
                    Pc += _currentOp.Bytes;
                    break;

                // AND - and memory with accumulator
                // A AND M -> A (NZ)
                case 0x21:
                case 0x25:
                case 0x29:
                case 0x2d:
                case 0x31:
                case 0x32:
                case 0x35:
                case 0x39:
                case 0x3d:
                    result = A & oper;

                    Nf = ((result & 0x80) == 0x80);
                    Zf = ((result & 0xff) == 0x00);

                    A = (byte)result;
                    Pc += _currentOp.Bytes;
                    break;

                // ASL - shift left one bit (NZC)
                // C <- (76543210) <- 0

                case 0x06:
                case 0x16:
                case 0x0a:
                case 0x0e:
                case 0x1e:

                    // shift bit 7 into carry
                    Cf = (oper >= 0x80);

                    // shift operand
                    result = oper << 1;

                    Nf = ((result & 0x80) == 0x80);
                    Zf = ((result & 0xff) == 0x00);

                    SaveOperand(_currentOp.AddressMode, result);
                    Pc += _currentOp.Bytes;

                    break;

                // BBRx - test bit in memory (no flags)
                // Test the zero page location and branch of the specified bit is clear
                // These instructions are only available on Rockwell and WDC 65C02 chips.
                // Number of clock cycles is the same regardless if the branch is taken.
                case 0x0f:
                case 0x1f:
                case 0x2f:
                case 0x3f:
                case 0x4f:
                case 0x5f:
                case 0x6f:
                case 0x7f:

                    // upper nibble specifies the bit to check
                    var checkBit = (byte)(_currentOp.OpCode >> 4);
                    byte checkValue = 0x01;
                    for (int ii = 0; ii < checkBit; ii++)
                    {
                        checkValue = (byte)(checkValue << 1);
                    }

                    // if the specified bit is 0 then branch
                    byte offset = SystemBus.Read((ushort)(Pc + 2));
                    Pc += _currentOp.Bytes;

                    if ((oper & checkValue) == 0x00)
                        Pc += offset;

                    break;

                // BBSx - test bit in memory (no flags)
                // Test the zero page location and branch of the specified bit is set
                // These instructions are only available on Rockwell and WDC 65C02 chips.
                // Number of clock cycles is the same regardless if the branch is taken.
                case 0x8f:
                case 0x9f:
                case 0xaf:
                case 0xbf:
                case 0xcf:
                case 0xdf:
                case 0xef:
                case 0xff:

                    // upper nibble specifies the bit to check (but ignore bit 7)
                    checkBit = (byte)((_currentOp.OpCode & 0x70) >> 4);
                    checkValue = 0x01;
                    for (int ii = 0; ii < checkBit; ii++)
                    {
                        checkValue = (byte)(checkValue << 1);
                    }

                    // if the specified bit is 1 then branch
                    offset = SystemBus.Read((ushort)(Pc + 2));
                    Pc += _currentOp.Bytes;

                    if ((oper & checkValue) == checkValue)
                        Pc += offset;

                    break;

                // BCC - branch on carry clear
                case 0x90:
                    Pc += _currentOp.Bytes;
                    CheckBranch(!Cf, oper);
                    break;

                // BCS - branch on carry set
                case 0xb0:
                    Pc += _currentOp.Bytes;
                    CheckBranch(Cf, oper);
                    break;

                // BEQ - branch on zero
                case 0xf0:
                    Pc += _currentOp.Bytes;
                    CheckBranch(Zf, oper);
                    break;

                // BIT - test bits in memory with accumulator (NZV)
                // bits 7 and 6 of oper are transferred to bits 7 and 6 of conditional register (N and V)
                // the zero flag is set to the result of oper AND accumulator
                case 0x24:
                case 0x2c:
                // added by 65C02
                case 0x34:
                case 0x3c:
                case 0x89:
                    result = A & oper;

                    // The WDC programming manual for 65C02 indicates NV are unaffected in immediate mode.
                    // The extended op code test program reflects this.
                    if (_currentOp.AddressMode != AddressModes.Immediate)
                    {
                        Nf = ((oper & 0x80) == 0x80);
                        Vf = ((oper & 0x40) == 0x40);
                    }

                    Zf = ((result & 0xff) == 0x00);

                    Pc += _currentOp.Bytes;
                    break;

                // BMI - branch on negative
                case 0x30:
                    Pc += _currentOp.Bytes;
                    CheckBranch(Nf, oper);
                    break;

                // BNE - branch on non zero
                case 0xd0:
                    Pc += _currentOp.Bytes;
                    CheckBranch(!Zf, oper);
                    break;

                // BPL - branch on non negative
                case 0x10:
                    Pc += _currentOp.Bytes;
                    CheckBranch(!Nf, oper);
                    break;

                // BRA - unconditional branch to immediate address
                // NOTE: In OpcodeList.txt the number of clock cycles is one less than the documentation.
                // This is because CheckBranch() adds one when a branch is taken, which in this case is always.
                case 0x80:
                    Pc += _currentOp.Bytes;
                    CheckBranch(true, oper);
                    break;

                // BRK - force break (I)
                case 0x00:

                    // This is a software interrupt (IRQ).  These events happen in a specific order.

                    // Processor adds two to the current PC
                    Pc += 2;

                    // Call IRQ routine
                    DoIrq(0xfffe, true);

                    // Whether or not the decimal flag is cleared depends on the type of 6502 CPU.
                    // The CMOS 65C02 clears this flag but the NMOS 6502 does not.
                    if (_cpuType == E6502Type.Cmos)
                        Df = false;

                    break;
                // BVC - branch on overflow clear
                case 0x50:
                    Pc += _currentOp.Bytes;
                    CheckBranch(!Vf, oper);
                    break;

                // BVS - branch on overflow set
                case 0x70:
                    Pc += _currentOp.Bytes;
                    CheckBranch(Vf, oper);
                    break;

                // CLC - clear carry flag
                case 0x18:
                    Cf = false;
                    Pc += _currentOp.Bytes;
                    break;

                // CLD - clear decimal mode
                case 0xd8:
                    Df = false;
                    Pc += _currentOp.Bytes;
                    break;

                // CLI - clear interrupt disable bit
                case 0x58:
                    If = false;
                    Pc += _currentOp.Bytes;
                    break;

                // CLV - clear overflow flag
                case 0xb8:
                    Vf = false;
                    Pc += _currentOp.Bytes;
                    break;

                // CMP - compare memory with accumulator (NZC)
                // CMP, CPX and CPY are unsigned comparisons
                case 0xc5:
                case 0xc9:
                case 0xc1:
                case 0xcd:
                case 0xd1:
                case 0xd2:
                case 0xd5:
                case 0xd9:
                case 0xdd:

                    byte temp = (byte)(A - oper);

                    Cf = A >= (byte)oper;
                    Zf = A == (byte)oper;
                    Nf = ((temp & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;

                // CPX - compare memory and X (NZC)
                case 0xe0:
                case 0xe4:
                case 0xec:
                    temp = (byte)(X - oper);

                    Cf = X >= (byte)oper;
                    Zf = X == (byte)oper;
                    Nf = ((temp & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;

                // CPY - compare memory and Y (NZC)
                case 0xc0:
                case 0xc4:
                case 0xcc:
                    temp = (byte)(Y - oper);

                    Cf = Y >= (byte)oper;
                    Zf = Y == (byte)oper;
                    Nf = ((temp & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;

                // DEC - decrement memory by 1 (NZ)
                case 0xc6:
                case 0xce:
                case 0xd6:
                case 0xde:
                // added by 65C02
                case 0x3a:
                    result = oper - 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    SaveOperand(_currentOp.AddressMode, result);

                    Pc += _currentOp.Bytes;
                    break;

                // DEX - decrement X by one (NZ)
                case 0xca:
                    result = X - 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    X = (byte)result;
                    Pc += _currentOp.Bytes;
                    break;

                // DEY - decrement Y by one (NZ)
                case 0x88:
                    result = Y - 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    Y = (byte)result;
                    Pc += _currentOp.Bytes;
                    break;

                // EOR - XOR memory with accumulator (NZ)
                case 0x41:
                case 0x45:
                case 0x49:
                case 0x4d:
                case 0x51:
                case 0x52:
                case 0x55:
                case 0x59:
                case 0x5d:
                    result = A ^ (byte)oper;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    A = (byte)result;

                    Pc += _currentOp.Bytes;
                    break;

                // INC - increment memory by 1 (NZ)
                case 0xe6:
                case 0xee:
                case 0xf6:
                case 0xfe:
                // added by 65C02
                case 0x1a:
                    result = oper + 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    SaveOperand(_currentOp.AddressMode, result);

                    Pc += _currentOp.Bytes;
                    break;

                // INX - increment X by one (NZ)
                case 0xe8:
                    result = X + 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    X = (byte)result;
                    Pc += _currentOp.Bytes;
                    break;

                // INY - increment Y by one (NZ)
                case 0xc8:
                    result = Y + 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    Y = (byte)result;
                    Pc += _currentOp.Bytes;
                    break;

                // JMP - jump to new location (two byte immediate)
                case 0x4c:
                case 0x6c:
                // added for 65C02
                case 0x7c:

                    if (_currentOp.AddressMode == AddressModes.Absolute)
                    {
                        Pc = GetImmWord();
                    }
                    else if (_currentOp.AddressMode == AddressModes.Indirect)
                    {
                        Pc = ReadWord(GetImmWord());
                    }
                    else if (_currentOp.AddressMode == AddressModes.AbsoluteX)
                    {
                        Pc = ReadWord((ushort)(GetImmWord() + X));
                    }
                    else
                    {
                        throw new InvalidOperationException("This address mode is invalid with the JMP instruction");
                    }

                    break;

                // JSR - jump to new location and save return address
                case 0x20:
                    // documentation says push PC+2 even though this is a 3 byte instruction
                    // When pulled via RTS 1 is added to the result
                    Push((ushort)(Pc + 2));
                    Pc = GetImmWord();
                    break;

                // LDA - load accumulator with memory (NZ)
                case 0xa1:
                case 0xa5:
                case 0xa9:
                case 0xad:
                case 0xb1:
                case 0xb2:
                case 0xb5:
                case 0xb9:
                case 0xbd:
                    A = (byte)oper;

                    Zf = ((A & 0xff) == 0x00);
                    Nf = ((A & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;

                // LDX - load index X with memory (NZ)
                case 0xa2:
                case 0xa6:
                case 0xae:
                case 0xb6:
                case 0xbe:
                    X = (byte)oper;

                    Zf = ((X & 0xff) == 0x00);
                    Nf = ((X & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;

                // LDY - load index Y with memory (NZ)
                case 0xa0:
                case 0xa4:
                case 0xac:
                case 0xb4:
                case 0xbc:
                    Y = (byte)oper;

                    Zf = ((Y & 0xff) == 0x00);
                    Nf = ((Y & 0x80) == 0x80);

                    Pc += _currentOp.Bytes;
                    break;


                // LSR - shift right one bit (NZC)
                // 0 -> (76543210) -> C
                case 0x46:
                case 0x4a:
                case 0x4e:
                case 0x56:
                case 0x5e:

                    // shift bit 0 into carry
                    Cf = ((oper & 0x01) == 0x01);

                    // shift operand
                    result = oper >> 1;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    SaveOperand(_currentOp.AddressMode, result);

                    Pc += _currentOp.Bytes;
                    break;

                // NOP - no operation
                case 0xea:
                    Pc += _currentOp.Bytes;
                    break;

                // ORA - OR memory with accumulator (NZ)
                case 0x01:
                case 0x05:
                case 0x09:
                case 0x0d:
                case 0x11:
                case 0x12:
                case 0x15:
                case 0x19:
                case 0x1d:
                    result = A | (byte)oper;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);

                    A = (byte)result;

                    Pc += _currentOp.Bytes;
                    break;

                // PHA - push accumulator on stack
                case 0x48:
                    Push(A);
                    Pc += _currentOp.Bytes;
                    break;

                // PHP - push processor status on stack
                case 0x08:
                    int sr = 0x00;

                    if (Nf) sr |= 0x80;
                    if (Vf) sr |= 0x40;
                    sr |= 0x20; // bit 5 is always 1
                    sr |= 0x10; // bit 4 is always 1 for PHP
                    if (Df) sr |= 0x08;
                    if (If) sr |= 0x04;
                    if (Zf) sr |= 0x02;
                    if (Cf) sr |= 0x01;

                    Push((byte)sr);
                    Pc += _currentOp.Bytes;
                    break;

                // PHX - push X on stack
                case 0xda:
                    Push(X);
                    Pc += _currentOp.Bytes;
                    break;

                // PHY - push Y on stack
                case 0x5a:
                    Push(Y);
                    Pc += _currentOp.Bytes;
                    break;

                // PLA - pull accumulator from stack (NZ)
                case 0x68:
                    A = PopByte();
                    Nf = (A & 0x80) == 0x80;
                    Zf = (A & 0xff) == 0x00;
                    Pc += _currentOp.Bytes;
                    break;

                // PLP - pull status from stack
                case 0x28:
                    sr = PopByte();

                    Nf = (sr & 0x80) == 0x80;
                    Vf = (sr & 0x40) == 0x40;
                    Df = (sr & 0x08) == 0x08;
                    If = (sr & 0x04) == 0x04;
                    Zf = (sr & 0x02) == 0x02;
                    Cf = (sr & 0x01) == 0x01;
                    Pc += _currentOp.Bytes;
                    break;

                // PLX - pull X from stack (NZ)
                case 0xfa:
                    X = PopByte();
                    Nf = (X & 0x80) == 0x80;
                    Zf = (X & 0xff) == 0x00;
                    Pc += _currentOp.Bytes;
                    break;

                // PLY - pull Y from stack (NZ)
                case 0x7a:
                    Y = PopByte();
                    Nf = (Y & 0x80) == 0x80;
                    Zf = (Y & 0xff) == 0x00;
                    Pc += _currentOp.Bytes;
                    break;

                // RMBx - clear bit in memory (no flags)
                // Clear the zero page location of the specified bit
                // These instructions are only available on Rockwell and WDC 65C02 chips.
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                case 0x47:
                case 0x57:
                case 0x67:
                case 0x77:

                    // upper nibble specifies the bit to check
                    checkBit = (byte)(_currentOp.OpCode >> 4);
                    checkValue = 0x01;
                    for (int ii = 0; ii < checkBit; ii++)
                    {
                        checkValue = (byte)(checkValue << 1);
                    }
                    checkValue = (byte)~checkValue;
                    SaveOperand(_currentOp.AddressMode, oper & checkValue);
                    Pc += _currentOp.Bytes;
                    break;

                // SMBx - set bit in memory (no flags)
                // Set the zero page location of the specified bit
                // These instructions are only available on Rockwell and WDC 65C02 chips.
                case 0x87:
                case 0x97:
                case 0xa7:
                case 0xb7:
                case 0xc7:
                case 0xd7:
                case 0xe7:
                case 0xf7:

                    // upper nibble specifies the bit to check (but ignore bit 7)
                    checkBit = (byte)((_currentOp.OpCode & 0x70) >> 4);
                    checkValue = 0x01;
                    for (int ii = 0; ii < checkBit; ii++)
                    {
                        checkValue = (byte)(checkValue << 1);
                    }
                    SaveOperand(_currentOp.AddressMode, oper | checkValue);
                    Pc += _currentOp.Bytes;
                    break;

                // ROL - rotate left one bit (NZC)
                // C <- 76543210 <- C
                case 0x26:
                case 0x2a:
                case 0x2e:
                case 0x36:
                case 0x3e:

                    // preserve existing cf value
                    var oldCf = Cf;

                    // shift bit 7 into carry flag
                    Cf = (oper >= 0x80);

                    // shift operand
                    result = oper << 1;

                    // old carry flag goes to bit zero
                    if (oldCf) result |= 0x01;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);
                    SaveOperand(_currentOp.AddressMode, result);

                    Pc += _currentOp.Bytes;
                    break;

                // ROR - rotate right one bit (NZC)
                // C -> 76543210 -> C
                case 0x66:
                case 0x6a:
                case 0x6e:
                case 0x76:
                case 0x7e:

                    // preserve existing cf value
                    oldCf = Cf;

                    // shift bit 0 into carry flag
                    Cf = (oper & 0x01) == 0x01;

                    // shift operand
                    result = oper >> 1;

                    // old carry flag goes to bit 7
                    if (oldCf) result |= 0x80;

                    Zf = ((result & 0xff) == 0x00);
                    Nf = ((result & 0x80) == 0x80);
                    SaveOperand(_currentOp.AddressMode, result);

                    Pc += _currentOp.Bytes;
                    break;

                // RTI - return from interrupt
                case 0x40:
                    // pull SR
                    sr = PopByte();

                    Nf = (sr & 0x80) == 0x80;
                    Vf = (sr & 0x40) == 0x40;
                    Df = (sr & 0x08) == 0x08;
                    If = (sr & 0x04) == 0x04;
                    Zf = (sr & 0x02) == 0x02;
                    Cf = (sr & 0x01) == 0x01;

                    // pull PC
                    Pc = PopWord();

                    break;

                // RTS - return from subroutine
                case 0x60:
                    Pc = (ushort)(PopWord() + 1);
                    break;

                // SBC - subtract memory from accumulator with borrow (NZCV)
                // A-M-C -> A (NZCV)
                case 0xe1:
                case 0xe5:
                case 0xe9:
                case 0xed:
                case 0xf1:
                case 0xf2:
                case 0xf5:
                case 0xf9:
                case 0xfd:

                    if (Df)
                    {
                        result = CpuMath.HexToBcd(A) - CpuMath.HexToBcd((byte)oper);
                        if (!Cf) result--;

                        Cf = (result >= 0);

                        // BCD numbers wrap around when subtraction is negative
                        if (result < 0)
                            result += 100;
                        Zf = (result == 0);

                        A = CpuMath.BcdToHex(result);

                        // Unlike ZF and CF, the NF flag represents the MSB after conversion
                        // to BCD.
                        Nf = (A > 0x7f);
                    }
                    else
                    {
                        Adc((byte)~oper);
                    }
                    Pc += _currentOp.Bytes;

                    break;

                // SEC - set carry flag
                case 0x38:
                    Cf = true;
                    Pc += _currentOp.Bytes;
                    break;

                // SED - set decimal mode
                case 0xf8:
                    Df = true;
                    Pc += _currentOp.Bytes;
                    break;

                // SEI - set interrupt disable bit
                case 0x78:
                    If = true;
                    Pc += _currentOp.Bytes;
                    break;

                // STA - store accumulator in memory
                case 0x81:
                case 0x85:
                case 0x8d:
                case 0x91:
                case 0x92:
                case 0x95:
                case 0x99:
                case 0x9d:
                    SaveOperand(_currentOp.AddressMode, A);
                    Pc += _currentOp.Bytes;
                    break;

                // STX - store X in memory
                case 0x86:
                case 0x8e:
                case 0x96:
                    SaveOperand(_currentOp.AddressMode, X);
                    Pc += _currentOp.Bytes;
                    break;

                // STY - store Y in memory
                case 0x84:
                case 0x8c:
                case 0x94:
                    SaveOperand(_currentOp.AddressMode, Y);
                    Pc += _currentOp.Bytes;
                    break;

                // STZ - Store zero
                case 0x64:
                case 0x74:
                case 0x9c:
                case 0x9e:
                    SaveOperand(_currentOp.AddressMode, 0);
                    Pc += _currentOp.Bytes;
                    break;

                // TAX - transfer accumulator to X (NZ)
                case 0xaa:
                    X = A;
                    Zf = ((X & 0xff) == 0x00);
                    Nf = ((X & 0x80) == 0x80);
                    Pc += _currentOp.Bytes;
                    break;

                // TAY - transfer accumulator to Y (NZ)
                case 0xa8:
                    Y = A;
                    Zf = ((Y & 0xff) == 0x00);
                    Nf = ((Y & 0x80) == 0x80);
                    Pc += _currentOp.Bytes;
                    break;

                // TRB - test and reset bits (Z)
                // Perform bitwise AND between accumulator and contents of memory
                case 0x14:
                case 0x1c:
                    SaveOperand(_currentOp.AddressMode, ~A & oper);
                    Zf = (A & oper) == 0x00;
                    Pc += _currentOp.Bytes;
                    break;

                // TSB - test and set bits (Z)
                // Perform bitwise AND between accumulator and contents of memory
                case 0x04:
                case 0x0c:
                    SaveOperand(_currentOp.AddressMode, A | oper);
                    Zf = (A & oper) == 0x00;
                    Pc += _currentOp.Bytes;
                    break;

                // TSX - transfer SP to X (NZ)
                case 0xba:
                    X = Sp;
                    Zf = ((X & 0xff) == 0x00);
                    Nf = ((X & 0x80) == 0x80);
                    Pc += _currentOp.Bytes;
                    break;

                // TXA - transfer X to A (NZ)
                case 0x8a:
                    A = X;
                    Zf = ((A & 0xff) == 0x00);
                    Nf = ((A & 0x80) == 0x80);
                    Pc += _currentOp.Bytes;
                    break;

                // TXS - transfer X to SP (no flags -- some online docs are incorrect)
                case 0x9a:
                    Sp = X;
                    Pc += _currentOp.Bytes;
                    break;

                // TYA - transfer Y to A (NZ)
                case 0x98:
                    A = Y;
                    Zf = ((A & 0xff) == 0x00);
                    Nf = ((A & 0x80) == 0x80);
                    Pc += _currentOp.Bytes;
                    break;

                // The original 6502 has undocumented and erratic behavior if
                // undocumented op codes are invoked.  The 65C02 on the other hand
                // are guaranteed to be NOPs although they vary in number of bytes
                // and cycle counts.  These NOPs are listed in the OpcodeList.txt file
                // so the proper number of clock cycles are used.
                //
                // Instructions STP (0xdb) and WAI (0xcb) will reach this case.
                // For now these are treated as a NOP.
                default:
                    Pc += _currentOp.Bytes;
                    break;
            }
        }

        private int GetOperand(AddressModes mode, out bool crossBoundary)
        {
            int oper = 0;
            crossBoundary = false;
            switch (mode)
            {
                // Accumulator mode uses the value in the accumulator
                case AddressModes.Accumulator:
                    oper = A;
                    break;

                // Retrieves the byte at the specified memory location
                case AddressModes.Absolute:
                    oper = SystemBus.Read(GetImmWord());
                    break;

                // Indexed absolute retrieves the byte at the specified memory location
                case AddressModes.AbsoluteX:

                    ushort imm = GetImmWord();
                    ushort result = (ushort)(imm + X);
                    oper = SystemBus.Read(result);
                    if (_currentOp.CheckPageBoundary)
                    {
                        crossBoundary = ((imm & 0xff00) != (result & 0xff00));
                    }
                    break;
                case AddressModes.AbsoluteY:
                    imm = GetImmWord();
                    result = (ushort)(imm + Y);
                    oper = SystemBus.Read(result);
                    if (_currentOp.CheckPageBoundary)
                    {
                        crossBoundary = ((imm & 0xff00) != (result & 0xff00));
                    }
                    break;

                // Immediate mode uses the next byte in the instruction directly.
                case AddressModes.Immediate:
                    oper = GetImmByte();
                    break;

                // Implied or Implicit are single byte instructions that do not use
                // the next bytes for the operand.
                case AddressModes.Implied:
                    break;

                // Indirect mode uses the absolute address to get another address.
                // The immediate word is a memory location from which to retrieve
                // the 16 bit operand.
                case AddressModes.Indirect:
                    oper = ReadWord(GetImmWord());
                    break;

                // The indexed indirect modes uses the immediate byte rather than the
                // immediate word to get the memory location from which to retrieve
                // the 16 bit operand.  This is a combination of ZeroPage indexed and Indirect.
                case AddressModes.XIndirect:

                    /*
                     * 1) fetch immediate byte
                     * 2) add X to the byte
                     * 3) obtain word from this zero page address
                     * 4) return the byte located at the address specified by the word
                     */

                    oper = SystemBus.Read(ReadWord((byte)(GetImmByte() + X)));
                    break;

                // The Indirect Indexed works a bit differently than above.
                // The Y register is added *after* the dereferencing instead of before.
                case AddressModes.IndirectY:

                    /*
                        1) Fetch the address (word) at the immediate zero page location
                        2) Add Y to obtain the final target address
                        3)Load the byte at this address
                    */

                    ushort addr = ReadWord(GetImmByte());
                    oper = SystemBus.Read((ushort)(addr + Y));
                    if (_currentOp.CheckPageBoundary)
                    {
                        crossBoundary = ((oper & 0xff00) != (addr & 0xff00));
                    }
                    break;


                // Relative is used for branching, the immediate value is a
                // signed 8 bit value and used to offset the current PC.
                case AddressModes.Relative:
                    oper = CpuMath.SignExtend(GetImmByte());
                    break;

                // Zero Page mode is a fast way of accessing the first 256 bytes of memory.
                // Best programming practice is to place your variables in 0x00-0xff.
                // Retrieve the byte at the indicated memory location.
                case AddressModes.ZeroPage:
                    oper = SystemBus.Read(GetImmByte());
                    break;
                case AddressModes.ZeroPageX:
                    oper = SystemBus.Read((ushort)((GetImmByte() + X) & 0xff));
                    break;
                case AddressModes.ZeroPageY:
                    oper = SystemBus.Read((ushort)((GetImmByte() + Y) & 0xff));
                    break;

                // this mode is from the 65C02 extended set
                // works like ZeroPageY when Y=0
                case AddressModes.ZeroPage0:
                    oper = SystemBus.Read(ReadWord((ushort)(GetImmByte() & 0xff)));
                    break;

                // for this mode do the same thing as ZeroPage
                case AddressModes.BranchExt:
                    oper = SystemBus.Read(GetImmByte());
                    break;
            }
            return oper;
        }

        private void SaveOperand(AddressModes mode, int data)
        {
            switch (mode)
            {
                // Accumulator mode uses the value in the accumulator
                case AddressModes.Accumulator:
                    A = (byte)data;
                    break;

                // Absolute mode retrieves the byte at the indicated memory location
                case AddressModes.Absolute:
                    SystemBus.Write(GetImmWord(), (byte)data);
                    break;
                case AddressModes.AbsoluteX:
                    SystemBus.Write((ushort)(GetImmWord() + X), (byte)data);
                    break;
                case AddressModes.AbsoluteY:
                    SystemBus.Write((ushort)(GetImmWord() + Y), (byte)data);
                    break;

                // Immediate mode uses the next byte in the instruction directly.
                case AddressModes.Immediate:
                    throw new InvalidOperationException("Address mode " + mode + " is not valid for this operation");

                // Implied or Implicit are single byte instructions that do not use
                // the next bytes for the operand.
                case AddressModes.Implied:
                    throw new InvalidOperationException("Address mode " + mode + " is not valid for this operation");

                // Indirect mode uses the absolute address to get another address.
                // The immediate word is a memory location from which to retrieve
                // the 16 bit operand.
                case AddressModes.Indirect:
                    throw new InvalidOperationException("Address mode " + mode + " is not valid for this operation");

                // The indexed indirect modes uses the immediate byte rather than the
                // immediate word to get the memory location from which to retrieve
                // the 16 bit operand.  This is a combination of ZeroPage indexed and Indirect.
                case AddressModes.XIndirect:
                    SystemBus.Write(ReadWord((byte)(GetImmByte() + X)), (byte)data);
                    break;

                // The Indirect Indexed works a bit differently than above.
                // The Y register is added *after* the dereferencing instead of before.
                case AddressModes.IndirectY:
                    SystemBus.Write((ushort)(ReadWord(GetImmByte()) + Y), (byte)data);
                    break;

                // Relative is used for branching, the immediate value is a
                // signed 8 bit value and used to offset the current PC.
                case AddressModes.Relative:
                    throw new InvalidOperationException("Address mode " + mode.ToString() + " is not valid for this operation");

                // Zero Page mode is a fast way of accessing the first 256 bytes of memory.
                // Best programming practice is to place your variables in 0x00-0xff.
                // Retrieve the byte at the indicated memory location.
                case AddressModes.ZeroPage:
                    SystemBus.Write(GetImmByte(), (byte)data);
                    break;
                case AddressModes.ZeroPageX:
                    SystemBus.Write((ushort)((GetImmByte() + X) & 0xff), (byte)data);
                    break;
                case AddressModes.ZeroPageY:
                    SystemBus.Write((ushort)((GetImmByte() + Y) & 0xff), (byte)data);
                    break;
                case AddressModes.ZeroPage0:
                    SystemBus.Write(ReadWord((ushort)((GetImmByte()) & 0xff)), (byte)data);
                    break;

                // for this mode do the same thing as ZeroPage
                case AddressModes.BranchExt:
                    SystemBus.Write(GetImmByte(), (byte)data);
                    break;
            }
        }

        private ushort GetImmWord()
        {
            return ReadWord((ushort)(Pc + 1));
        }

        private byte GetImmByte()
        {
            return SystemBus.Read((ushort)(Pc + 1));
        }

        private ushort ReadWord(ushort address)
        {
            return (ushort)(SystemBus.Read((ushort)(address + 1)) << 8 | SystemBus.Read(address) & 0xffff);
        }

        private void Push(byte data)
        {
            SystemBus.Write((ushort)(0x0100 | Sp), data);
            Sp--;
        }

        private void Push(ushort data)
        {
            // HI byte is in a higher address, LO byte is in the lower address
            SystemBus.Write((ushort)(0x0100 | Sp), (byte)(data >> 8));
            SystemBus.Write((ushort)(0x0100 | (Sp - 1)), (byte)(data & 0xff));
            Sp -= 2;
        }

        private byte PopByte()
        {
            Sp++;
            return SystemBus.Read((ushort)(0x0100 | Sp));
        }

        private ushort PopWord()
        {
            // HI byte is in a higher address, LO byte is in the lower address
            Sp += 2;
            ushort idx = (ushort)(0x0100 | Sp);
            return (ushort)((SystemBus.Read(idx) << 8 | SystemBus.Read((ushort)(idx - 1))) & 0xffff);
        }

        private void Adc(byte oper)
        {
            var answer = (ushort)(A + oper);
            if (Cf) answer++;

            Cf = (answer > 0xff);
            Zf = ((answer & 0xff) == 0x00);
            Nf = (answer & 0x80) == 0x80;

            //ushort temp = (ushort)(~(A ^ oper) & (A ^ answer) & 0x80);
            Vf = (~(A ^ oper) & (A ^ answer) & 0x80) != 0x00;

            A = (byte)answer;
        }

        private void DoIrq(ushort vector, bool isBrk = false)
        {
            // Push the MSB of the PC
            Push((byte)(Pc >> 8));

            // Push the LSB of the PC
            Push((byte)(Pc & 0xff));

            // Push the status register
            var sr = 0x00;
            if (Nf) sr |= 0x80;
            if (Vf) sr |= 0x40;

            sr |= 0x20;             // bit 5 is unused and always 1

            if (isBrk)
                sr |= 0x10;         // software interrupt (BRK) pushes B flag as 1
                                    // hardware interrupt pushes B flag as 0
            if (Df) sr |= 0x08;
            if (If) sr |= 0x04;
            if (Zf) sr |= 0x02;
            if (Cf) sr |= 0x01;

            Push((byte)sr);

            // set interrupt disable flag
            If = true;

            // On 65C02, IRQ, NMI, and RESET also clear the D flag (but not on BRK) after pushing the status register.
            if (_cpuType == E6502Type.Cmos && !isBrk)
                Df = false;

            // load program counter with the interrupt vector
            Pc = ReadWord(vector);
        }

        private void CheckBranch(bool flag, int oper)
        {
            if (flag)
            {
                Pc += (ushort)oper;
            }
        }
    }
}
