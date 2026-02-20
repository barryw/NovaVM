using KDS.e6502.Extensions;

namespace KDS.e6502.OpCodes
{
    internal class OpCodeRecord
    {
        public byte OpCode { get; }
        internal string Instruction { get; }
        public AddressModes AddressMode { get; }
        public ushort Bytes { get; private set; }
        public int Cycles { get; private set; }
        public bool CheckPageBoundary { get; private set; }
        internal bool IsValid { get; }

        public OpCodeRecord()
        {
            Bytes = 1;
            CheckPageBoundary = false;
            IsValid = false;
            Instruction = string.Empty;
        }

        public OpCodeRecord(byte opcode, string instruction, AddressModes addressmode, ushort bytes, int cycles,
                               bool checkPageBoundary)
        {
            OpCode = opcode;
            Instruction = instruction;
            AddressMode = addressmode;
            Bytes = bytes;
            Cycles = cycles;
            CheckPageBoundary = checkPageBoundary;
            IsValid = true;
        }

        public string Dasm()
        {
            if (!IsValid)
                return "???";

            if (AddressMode == AddressModes.Accumulator)
            {
                return $"{Instruction} A";
            }
            
            return "???";
        }

        public string Dasm(int oper)
        {
            if (!IsValid)
                return "???";

            var dasm = Instruction;
            switch (AddressMode)
            {
                case AddressModes.Accumulator:
                    dasm += " A";
                    break;

                // Absolute mode prints the address with no parenthesis
                case AddressModes.Absolute:
                    dasm += " $" + oper.Hex4();
                    break;
                case AddressModes.AbsoluteX:
                    dasm += " $" + oper.Hex4() + ", X";
                    break;
                case AddressModes.AbsoluteY:
                    dasm += " $" + oper.Hex4() + ", Y";
                    break;

                // No parenthesis for relative branches
                case AddressModes.Relative:
                    dasm += " $" + oper.Hex2();
                    break;

                // Zero page is also direct addressing so no parenthesis
                case AddressModes.ZeroPage:
                    dasm += " $" + oper.Hex2();
                    break;
                case AddressModes.ZeroPage0:
                    dasm += " ($" + oper.Hex2() + ")";
                    break;
                case AddressModes.ZeroPageX:
                    dasm += " $" + oper.Hex2() + ",X";
                    break;
                case AddressModes.ZeroPageY:
                    dasm += " $" + oper.Hex2() + ",Y";
                    break;

                // # sign indicates immediate
                case AddressModes.Immediate:
                    dasm += " #$" + oper.Hex2();
                    break;

                // parenthesis indicate an indirect addressing into memory
                case AddressModes.Indirect:
                    dasm += " ($" + oper.Hex4() + ")";
                    break;
                case AddressModes.XIndirect:
                    dasm += " ($" + oper.Hex2() + ",X)";
                    break;
                case AddressModes.IndirectY:
                    dasm += " ($" + oper.Hex2() + "),Y";
                    break;

                case AddressModes.Implied: // do nothing
                    break;

                case AddressModes.BranchExt:
                    // assumption is that in this mode the oper passed in is a word and not a byte
                    byte zp = (byte)(oper >> 8);
                    byte br = (byte)(oper & 0xff);
                    dasm += " $" + zp.ToString("X2") + ", $" + br.ToString("X2");
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            
            return dasm;
        }

        // this is so something useful is displayed in the watch window
        public override string ToString()
        {
            return !IsValid ? "???" : $"{OpCode:X2} {Instruction} {AddressMode.ToString()}";
        }
    }
}
