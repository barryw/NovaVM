using System.Text;

// Synthetic Version 6 story for the NovaZ M1 milestone (text-only V6 boot).
//
// V6 packed addressing (Z-Machine Standard 1.1):
//   routine byte address = 4P + 8 * R_O   (R_O = word at header $28)
//   string  byte address = 4P + 8 * S_O   (S_O = word at header $2A)
// The offsets are deliberately nonzero AND different from each other so that
// any interpreter that ignores them -- or mixes them up -- cannot pass.
//
// Header $06 in V6 is the packed address of the MAIN ROUTINE (locals-count
// byte first), called with no arguments; it is not a raw initial PC.

const int StorySize = 0x1000;

const int RoutinesOffsetWords = 0x0040;            // header $28 -> byte offset 0x200
const int StringsOffsetWords = 0x0080;             // header $2A -> byte offset 0x400
const int RoutinesByteOffset = RoutinesOffsetWords * 8;
const int StringsByteOffset = StringsOffsetWords * 8;

const int Globals = 0x0040;                        // 240 globals x 2 = $0040-$021F
const int ObjectTable = 0x0220;
const int TextBuffer = 0x0400;
const int ParseBuffer = 0x0450;
const int Dictionary = 0x0480;
const int PicTable = 0x0500;                       // scratch: picture_data table
const int UserStack = 0x0520;                      // scratch: user-stack table (Task 8)
const int StaticMemory = 0x0600;
const int CodeBase = 0x0600;                       // routines region; packed (0x600-0x200)/4
const int PackedStrings = 0x0900;                  // strings region;  packed (0x900-0x400)/4

(string outputPath, int version) = ParseArgs(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story, version);
WriteDictionary(story);
WriteBuffers(story);
WritePackedText(story, PackedStrings, "ok");

var code = new ZCode(CodeBase, packedAddressScale: 4, routinesByteOffset: RoutinesByteOffset);
EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (CodeBase + codeBytes.Length >= PackedStrings)
    throw new InvalidOperationException($"Z6 spec program overlaps packed strings: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, CodeBase, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes, version {version})");

static (string Output, int Version) ParseArgs(string[] args)
{
    string output = "build/z6-spec.z6";
    int version = 6;
    for (int i = 0; i < args.Length; i++)
    {
        string? value = i + 1 < args.Length ? args[i + 1] : null;
        switch (args[i])
        {
            case "--output" when value is not null:
                output = value;
                i++;
                break;
            case "--version" when value is not null:
                if (!int.TryParse(value, out version) || version is < 1 or > 8)
                    throw new ArgumentException($"--version must be 1-8: {value}");
                i++;
                break;
            case "-h" or "--help":
                PrintUsage();
                Environment.Exit(0);
                break;
            default:
                Console.Error.WriteLine($"Unknown or incomplete argument: {args[i]}");
                PrintUsage();
                Environment.Exit(1);
                break;
        }
    }

    return (output, version);
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova.NovaZ.Z6SpecStory");
    Console.Error.WriteLine("  --output <story.z6>");
    Console.Error.WriteLine("  --version <n>   header version byte only (default 6; 7 for rejection tests)");
}

static void WriteHeader(byte[] story, int version)
{
    story[0x00] = (byte)version;
    story[0x01] = 0;
    WriteBE16(story, 0x02, 1);                              // release
    WriteBE16(story, 0x04, StaticMemory);                   // base of high memory
    WriteBE16(story, 0x06, (CodeBase - RoutinesByteOffset) / 4); // packed main routine
    WriteBE16(story, 0x08, Dictionary);
    WriteBE16(story, 0x0A, ObjectTable);
    WriteBE16(story, 0x0C, Globals);
    WriteBE16(story, 0x0E, StaticMemory);
    WriteBE16(story, 0x10, 0);                              // flags2
    Encoding.ASCII.GetBytes("Z6SPEC").CopyTo(story, 0x12);  // serial
    WriteBE16(story, 0x18, 0);                              // abbreviations
    WriteBE16(story, 0x1A, StorySize / 8);                  // file length (v6+: /8)
    story[0x1E] = 6;
    story[0x1F] = (byte)'N';
    WriteBE16(story, 0x28, RoutinesOffsetWords);            // R_O
    WriteBE16(story, 0x2A, StringsOffsetWords);             // S_O
}

static void WriteDictionary(byte[] story)
{
    story[Dictionary] = 1;
    story[Dictionary + 1] = (byte)',';
    story[Dictionary + 2] = 6;
    WriteBE16(story, Dictionary + 3, 1);
    WriteDictionaryEntry(story, Dictionary + 5, "look");
}

static void WriteBuffers(byte[] story)
{
    story[TextBuffer] = 78;
    story[TextBuffer + 1] = 0;
    story[ParseBuffer] = 8;
}

static void WriteDictionaryEntry(byte[] story, int offset, string text)
{
    byte[] encoded = ZString.PackDictionaryEntry(text, fixedWords: 3);
    Array.Copy(encoded, 0, story, offset, encoded.Length);
}

static void WritePackedText(byte[] story, int offset, string text)
{
    byte[] encoded = ZString.Pack(text);
    Array.Copy(encoded, 0, story, offset, encoded.Length);
}

static void EmitSpecProgram(ZCode z)
{
    // Main routine: header $06 points here (packed). Locals-count byte first.
    z.Byte(0);

    // M1 core: literal print, packed string via S_O, routine call via R_O.
    z.Print("z6 m1 ");
    z.OneOp(13, Operand.Large((PackedStrings - StringsByteOffset) / 4)); // print_paddr "ok"
    z.Call1S("routine_return_42", 0x11);
    z.AssertVarEquals(0x11, 42, "v6-call");
    z.NewLine();

    // --- Task 6: V6 dispatch routing into the NOVAZ6 segment (M1 stubs) ---
    // erase_window -1 (VAR:13): routed to the segment; the M1 stub ignores
    // it, so the "z6 m1 ok" text already on screen must survive to the end.
    z.VarOp(13, Operand.Large(0xFFFF));
    // set_window 0 (VAR:11): routed stub, no-op.
    z.VarOp(11, Operand.Small(0));
    // get_wind_prop 1,4 (EXT:19, store): the stub must consume the store
    // byte and store 0 — a mis-consumed store byte derails the instruction
    // stream, which is what this assert really pins.
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(4));
    z.AssertVarEquals(0x12, 0, "windprop-stub");
    // picture_data 0, table (EXT:6, branch): stub decodes the branch bytes
    // and applies "false". If the branch is wrongly taken we hit the fail
    // block; fall-through is the good path.
    z.ExtOpBranch(6, "fail_pic", branchIf: true, Operand.Small(0), Operand.Large(PicTable));
    z.Jump("pic_ok");
    z.Label("fail_pic");
    z.Fail("pic-branch");
    z.Label("pic_ok");
    // buffer_screen 0 (EXT:29, store): stub stores 0.
    z.ExtOpStore(29, 0x12, Operand.Small(0));
    z.AssertVarEquals(0x12, 0, "bufscreen-stub");
    // push_stack 5, stack (EXT:24, branch): the M1 stub always branches
    // FALSE ("stack full"), so taken = failure, fall-through = stub path.
    // NOTE(Task 8): this entire block gets rewritten — a real push onto a non-full
    // stack succeeds and branches TRUE, so this expectation inverts then.
    z.ExtOpBranch(24, "fail_us", branchIf: true, Operand.Small(5), Operand.Large(UserStack));
    z.Jump("us_ok");
    z.Label("fail_us");
    z.Fail("us-stub");
    z.Label("us_ok");

    z.Label("prompt");
    z.Print(">");
    z.VarOpStore(4, 0x11, Operand.Large(TextBuffer), Operand.Large(ParseBuffer)); // read
    z.Print("z6 input ok");
    z.NewLine();
    z.Jump("prompt");

    z.Align(4);
    z.Label("routine_return_42");
    z.Byte(0);
    z.OneOp(11, Operand.Small(42));
}

static void WriteBE16(byte[] data, int offset, int value)
{
    data[offset] = (byte)((value >> 8) & 0xFF);
    data[offset + 1] = (byte)(value & 0xFF);
}

static int Checksum(byte[] story)
{
    int sum = 0;
    for (int i = 0x40; i < story.Length; i++)
        sum = (sum + story[i]) & 0xFFFF;
    return sum;
}

enum OperandKind
{
    Large = 0,
    Small = 1,
    Variable = 2
}

readonly record struct Operand(OperandKind Kind, int Value)
{
    public static Operand Large(int value) => new(OperandKind.Large, value & 0xFFFF);
    public static Operand Small(int value) => new(OperandKind.Small, value & 0xFF);
    public static Operand Var(int variable) => new(OperandKind.Variable, variable & 0xFF);
}

sealed class ZCode
{
    private readonly int _origin;
    private readonly int _packedAddressScale;
    private readonly int _routinesByteOffset;
    private readonly List<byte> _bytes = [];
    private readonly Dictionary<string, int> _labels = new(StringComparer.Ordinal);
    private readonly List<BranchPatch> _branchPatches = [];
    private readonly List<JumpPatch> _jumpPatches = [];
    private readonly List<PackedAddressPatch> _packedAddressPatches = [];
    private int Pc => _origin + _bytes.Count;

    public ZCode(int origin, int packedAddressScale, int routinesByteOffset)
    {
        _origin = origin;
        _packedAddressScale = packedAddressScale;
        _routinesByteOffset = routinesByteOffset;
    }

    public void Label(string name)
    {
        if (!_labels.TryAdd(name, Pc))
            throw new InvalidOperationException($"Duplicate label: {name}");
    }

    public void Print(string text)
    {
        Emit(0xB2);
        EmitRange(ZString.Pack(text));
    }

    public void NewLine() => Emit(0xBB);

    public void ZeroOp(int op) => Emit(0xB0 | (op & 0x0F));

    public void ZeroOpStore(int op, byte store)
    {
        ZeroOp(op);
        Emit(store);
    }

    public void ZeroOpBranch(int op, string label, bool branchIf = true)
    {
        ZeroOp(op);
        Branch(label, branchIf);
    }

    public void OneOp(int op, Operand operand)
    {
        Emit(0x80 | (((int)operand.Kind & 0x03) << 4) | (op & 0x0F));
        EmitOperand(operand);
    }

    public void OneOpStore(int op, Operand operand, byte store)
    {
        OneOp(op, operand);
        Emit(store);
    }

    public void OneOpStoreBranch(int op, Operand operand, byte store, string label, bool branchIf = true)
    {
        OneOpStore(op, operand, store);
        Branch(label, branchIf);
    }

    public void TwoOp(int op, Operand a, Operand b)
    {
        Emit(0xC0 | (op & 0x1F));
        Emit(TypeByte([a, b]));
        EmitOperand(a);
        EmitOperand(b);
    }

    public void TwoOpStore(int op, Operand a, Operand b, byte store)
    {
        TwoOp(op, a, b);
        Emit(store);
    }

    public void TwoOpBranch(int op, Operand a, Operand b, string label, bool branchIf = true)
    {
        TwoOp(op, a, b);
        Branch(label, branchIf);
    }

    public void VarOp(int op, params Operand[] operands)
    {
        Emit(0xE0 | (op & 0x1F));
        Emit(TypeByte(operands));
        foreach (var operand in operands)
            EmitOperand(operand);
    }

    public void VarOpStore(int op, byte store, params Operand[] operands)
    {
        VarOp(op, operands);
        Emit(store);
    }

    // store variable, value (2OP:13).
    public void Store(byte variable, int value) =>
        TwoOp(13, Operand.Small(variable), Operand.Large(value));

    public void VarOpStoreBranch(int op, byte store, string label, params Operand[] operands)
    {
        VarOpStore(op, store, operands);
        Branch(label, branchIf: true);
    }

    public void VarOpBranch(int op, string label, params Operand[] operands)
    {
        VarOp(op, operands);
        Branch(label, branchIf: true);
    }

    public void ExtOp(int op, params Operand[] operands)
    {
        Emit(0xBE);
        Emit(op & 0xFF);
        Emit(TypeByte(operands));
        foreach (var operand in operands)
            EmitOperand(operand);
    }

    public void ExtOpStore(int op, byte store, params Operand[] operands)
    {
        ExtOp(op, operands);
        Emit(store);
    }

    public void ExtOpBranch(int op, string label, bool branchIf = true, params Operand[] operands)
    {
        ExtOp(op, operands);
        Branch(label, branchIf);
    }

    public void Call1S(string routineLabel, byte store)
    {
        Emit(0x88);
        EmitPackedAddressPatch(routineLabel);
        Emit(store);
    }

    public void Call1N(string routineLabel)
    {
        Emit(0x8F);
        EmitPackedAddressPatch(routineLabel);
    }

    public void Call2S(string routineLabel, byte store, Operand arg)
    {
        Emit(0xC0 | 25);
        Emit(TypeByte([Operand.Large(0), arg]));
        EmitPackedAddressPatch(routineLabel);
        EmitOperand(arg);
        Emit(store);
    }

    public void CallVs(string routineLabel, byte store, params Operand[] args)
    {
        Operand[] operands = [Operand.Large(0), .. args];
        Emit(0xE0);
        Emit(TypeByte(operands));
        EmitPackedAddressPatch(routineLabel);
        foreach (var arg in args)
            EmitOperand(arg);
        Emit(store);
    }

    public void CallVn(string routineLabel, params Operand[] args)
    {
        Operand[] operands = [Operand.Large(0), .. args];
        Emit(0xE0 | 25);
        Emit(TypeByte(operands));
        EmitPackedAddressPatch(routineLabel);
        foreach (var arg in args)
            EmitOperand(arg);
    }

    public void Jump(string label)
    {
        Emit(0x8C);
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _jumpPatches.Add(new JumpPatch(patchOffset, label));
    }

    public void Align(int alignment)
    {
        while ((Pc % alignment) != 0)
            Emit(0xB4);
    }

    public void Byte(int value) => Emit(value);

    public void AssertVarEquals(byte variable, int expected, string name)
    {
        string ok = $"ok_{name}_{_branchPatches.Count}";
        TwoOpBranch(1, Operand.Var(variable), Operand.Large(expected), ok);
        Fail(name);
        Label(ok);
    }

    public void AssertByteEquals(int baseAddr, int offset, int expected, string name)
    {
        TwoOpStore(16, Operand.Large(baseAddr), Operand.Small(offset), 0x11);
        AssertVarEquals(0x11, expected, name);
    }

    public void AssertWordEquals(int baseAddr, int wordIndex, int expected, string name)
    {
        TwoOpStore(15, Operand.Large(baseAddr), Operand.Small(wordIndex), 0x11);
        AssertVarEquals(0x11, expected, name);
    }

    public void Fail(string name)
    {
        Print($"z6 spec fail {name}");
        NewLine();
        Emit(0xBA);
    }

    public byte[] ToArray()
    {
        foreach (var patch in _packedAddressPatches)
        {
            int target = ResolveLabel(patch.Label);
            int relative = target - _routinesByteOffset;
            if (relative < 0 || (relative % _packedAddressScale) != 0)
                throw new InvalidOperationException(
                    $"Routine label is not {_packedAddressScale}-byte aligned past the routines offset: {patch.Label} at ${target:X4}");
            WriteWordAt(patch.Offset, relative / _packedAddressScale);
        }

        foreach (var patch in _jumpPatches)
        {
            int after = _origin + patch.Offset + 2;
            int target = ResolveLabel(patch.Label);
            WriteWordAt(patch.Offset, target - after + 2);
        }

        foreach (var patch in _branchPatches)
        {
            int after = _origin + patch.Offset + 2;
            int target = ResolveLabel(patch.Label);
            int offset = target - after + 2;
            int encoded = offset & 0x3FFF;
            _bytes[patch.Offset] = (byte)((patch.BranchIf ? 0x80 : 0x00) | ((encoded >> 8) & 0x3F));
            _bytes[patch.Offset + 1] = (byte)(encoded & 0xFF);
        }

        return [.. _bytes];
    }

    private void Branch(string label, bool branchIf)
    {
        int patchOffset = _bytes.Count;
        Emit(0);
        Emit(0);
        _branchPatches.Add(new BranchPatch(patchOffset, label, branchIf));
    }

    private int ResolveLabel(string label) =>
        _labels.TryGetValue(label, out int address)
            ? address
            : throw new InvalidOperationException($"Unknown label: {label}");

    private void EmitPackedAddressPatch(string routineLabel)
    {
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _packedAddressPatches.Add(new PackedAddressPatch(patchOffset, routineLabel));
    }

    private void EmitOperand(Operand operand)
    {
        switch (operand.Kind)
        {
            case OperandKind.Large:
                EmitWord(operand.Value);
                break;
            case OperandKind.Small:
            case OperandKind.Variable:
                Emit(operand.Value);
                break;
            default:
                throw new ArgumentOutOfRangeException(nameof(operand));
        }
    }

    private static byte TypeByte(IReadOnlyList<Operand> operands)
    {
        if (operands.Count > 4)
            throw new ArgumentOutOfRangeException(nameof(operands), "Z6 spec story uses at most four operands per instruction.");

        int value = 0;
        for (int i = 0; i < 4; i++)
        {
            int kind = i < operands.Count ? (int)operands[i].Kind : 3;
            value |= kind << (6 - (i * 2));
        }

        return (byte)value;
    }

    private void EmitWord(int value)
    {
        Emit((value >> 8) & 0xFF);
        Emit(value & 0xFF);
    }

    private void WriteWordAt(int offset, int value)
    {
        _bytes[offset] = (byte)((value >> 8) & 0xFF);
        _bytes[offset + 1] = (byte)(value & 0xFF);
    }

    private void EmitRange(IEnumerable<byte> bytes)
    {
        foreach (byte b in bytes)
            Emit(b);
    }

    private void Emit(int value) => _bytes.Add((byte)(value & 0xFF));
}

readonly record struct BranchPatch(int Offset, string Label, bool BranchIf);
readonly record struct JumpPatch(int Offset, string Label);
readonly record struct PackedAddressPatch(int Offset, string Label);

static class ZString
{
    private const string A0 = "abcdefghijklmnopqrstuvwxyz";
    private const string A1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    private const string A2 = " \r0123456789.,!?_#'\"/\\-:()";

    public static byte[] Pack(string text) => Pack(text, fixedWords: null);

    public static byte[] PackDictionaryEntry(string text, int fixedWords) => Pack(text, fixedWords);

    private static byte[] Pack(string text, int? fixedWords)
    {
        var zchars = new List<int>();
        foreach (char ch in text)
        {
            if (ch == ' ')
            {
                zchars.Add(0);
                continue;
            }

            int a0 = A0.IndexOf(ch);
            if (a0 >= 0)
            {
                zchars.Add(a0 + 6);
                continue;
            }

            int a1 = A1.IndexOf(ch);
            if (a1 >= 0)
            {
                zchars.Add(4);
                zchars.Add(a1 + 6);
                continue;
            }

            int a2 = A2.IndexOf(ch);
            if (a2 >= 0)
            {
                zchars.Add(5);
                zchars.Add(a2 + 6);
                continue;
            }

            if (ch > 0x3FF)
                throw new InvalidOperationException($"Character cannot be encoded in ZSCII escape form: U+{(int)ch:X4}");
            zchars.Add(5);
            zchars.Add(6);
            zchars.Add((ch >> 5) & 0x1F);
            zchars.Add(ch & 0x1F);
        }

        if (fixedWords is { } words && zchars.Count > words * 3)
            throw new InvalidOperationException($"Dictionary entry is too long for fixed encoding: {text}");

        while (zchars.Count % 3 != 0)
            zchars.Add(5);

        if (fixedWords is { } fixedWordCount)
        {
            while (zchars.Count < fixedWordCount * 3)
                zchars.Add(5);
        }

        return PackWords(zchars);
    }

    private static byte[] PackWords(IReadOnlyList<int> zchars)
    {
        var bytes = new byte[(zchars.Count / 3) * 2];
        for (int i = 0, j = 0; i < zchars.Count; i += 3, j += 2)
        {
            int word = (zchars[i] << 10) | (zchars[i + 1] << 5) | zchars[i + 2];
            if (i + 3 >= zchars.Count)
                word |= 0x8000;
            bytes[j] = (byte)((word >> 8) & 0xFF);
            bytes[j + 1] = (byte)(word & 0xFF);
        }

        return bytes;
    }
}
