namespace KDS.e6502.OpCodes
{
    internal class OpCodeTable
    {
        public OpCodeRecord[] OpCodes { get; private set; }

        public OpCodeTable()
        {
            OpCodes = new OpCodeRecord[0xff + 1];
            for (var ii = 0; ii <= 0xff; ii++)
            {
                OpCodes[ii] = new OpCodeRecord();
            }
            CreateTable();
        }

        private void CreateTable()
        {
            var oplist = new OpCodeReader();

            foreach (var line in oplist)
            {
                if (line.Length <= 40) continue;
                var address = line[..14].Trim();
                var instruction = line.Substring(14, 14).Trim();
                var opcode = line.Substring(28, 6).Trim();
                var bytes = line.Substring(34, 6).Trim();
                var cycles = line[40..].Trim();

                var recCheckPageBoundary = (cycles.Length == 2);
                cycles = cycles[..1];
                    
                if (!int.TryParse(opcode, System.Globalization.NumberStyles.AllowHexSpecifier, null, out var recOpcode))
                {
                    throw new InvalidDataException("Line + [" + line + "] (opc) has invalid data");
                }

                string recInstr;
                try
                {
                    var idx = instruction.IndexOf(" ", StringComparison.Ordinal);
                    recInstr = idx > -1 ? instruction.Substring(0, instruction.IndexOf(" ", StringComparison.Ordinal)) : instruction.TrimEnd();
                }
                catch
                {
                    throw new InvalidDataException("Line + [" + line + "] (assembler) has invalid data");
                }

                AddressModes recMode;
                switch (address)
                {
                    case "accumulator":
                        recMode = AddressModes.Accumulator;
                        break;
                    case "absolute":
                        recMode = AddressModes.Absolute;
                        break;
                    case "absolute,X":
                        recMode = AddressModes.AbsoluteX;
                        break;
                    case "absolute,Y":
                        recMode = AddressModes.AbsoluteY;
                        break;
                    case "immediate":
                        recMode = AddressModes.Immediate;
                        break;
                    case "implied":
                        recMode = AddressModes.Implied;
                        break;
                    case "indirect":
                        recMode = AddressModes.Indirect;
                        break;
                    case "(indirect,X)":
                        recMode = AddressModes.XIndirect;
                        break;
                    case "(indirect),Y":
                        recMode = AddressModes.IndirectY;
                        break;
                    case "relative":
                        recMode = AddressModes.Relative;
                        break;
                    case "zeropage":
                        recMode = AddressModes.ZeroPage;
                        break;
                    case "zeropage,X":
                        recMode = AddressModes.ZeroPageX;
                        break;
                    case "zeropage,Y":
                        recMode = AddressModes.ZeroPageY;
                        break;
                    case "zeropage,0":
                        recMode = AddressModes.ZeroPage0;
                        break;
                    case "branchext":
                        recMode = AddressModes.BranchExt;
                        break;
                    default:
                        throw new InvalidDataException("Line + [" + line + "] (addressing) has invalid data");

                }
                if (!ushort.TryParse(bytes, out ushort recBytes))
                {
                    throw new InvalidDataException("Line + [" + line + "] (bytes) has invalid data");
                }
                if (!int.TryParse(cycles, out int recCycles))
                {
                    throw new InvalidDataException("Line + [" + line + "] (cycles) has invalid data");
                }

                OpCodes[recOpcode] = new OpCodeRecord((byte)recOpcode, recInstr, recMode, recBytes, recCycles,
                    recCheckPageBoundary);
            }
        }
    }
}
