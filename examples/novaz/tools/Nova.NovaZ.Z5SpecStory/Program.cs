using System.Text;

const int StorySize = 0x2000;
const int InitialPc = 0x0040;
const int ObjectTable = 0x1200;
const int Object1PropTable = 0x1480;
const int Object2PropTable = 0x14C0;
const int Object3PropTable = 0x1500;
const int Globals = 0x1600;
const int Dictionary = 0x1700;
const int TextBuffer = 0x1780;
const int ParseBuffer = 0x17D0;
const int ScratchTable = 0x1820;
const int PackedText = 0x1900;
const int StaticMemory = 0x1A00;

string outputPath = ParseOutput(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story);
WriteObjectTable(story);
WriteGlobals(story);
WriteDictionary(story);
WriteBuffers(story);
WritePackedText(story, PackedText, "paddr5");

var code = new ZCode(InitialPc, packedAddressScale: 4);
EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (InitialPc + codeBytes.Length >= ObjectTable)
    throw new InvalidOperationException($"Z5 spec program overlaps data tables: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, InitialPc, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes)");

static string ParseOutput(string[] args)
{
    string output = "build/z5-spec.z5";
    for (int i = 0; i < args.Length; i++)
    {
        string? value = i + 1 < args.Length ? args[i + 1] : null;
        switch (args[i])
        {
            case "--output" when value is not null:
                output = value;
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

    return output;
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova.NovaZ.Z5SpecStory");
    Console.Error.WriteLine("  --output <story.z5>");
}

static void WriteHeader(byte[] story)
{
    story[0x00] = 5;
    story[0x01] = 0;
    WriteBE16(story, 0x02, 1);
    WriteBE16(story, 0x04, InitialPc);
    WriteBE16(story, 0x06, InitialPc);
    WriteBE16(story, 0x08, Dictionary);
    WriteBE16(story, 0x0A, ObjectTable);
    WriteBE16(story, 0x0C, Globals);
    WriteBE16(story, 0x0E, StaticMemory);
    WriteBE16(story, 0x10, 0);
    Encoding.ASCII.GetBytes("Z5SPEC").CopyTo(story, 0x12);
    WriteBE16(story, 0x18, 0);
    WriteBE16(story, 0x1A, StorySize / 4);
    story[0x1E] = 6;
    story[0x1F] = (byte)'N';
}

static void WriteObjectTable(byte[] story)
{
    WriteBE16(story, ObjectTable + ((15 - 1) * 2), 0x5151);

    WriteObjectEntry(story, 1, [], parent: 0, sibling: 0, child: 2, Object1PropTable);
    WriteObjectEntry(story, 2, [(40, true)], parent: 1, sibling: 3, child: 0, Object2PropTable);
    WriteObjectEntry(story, 3, [], parent: 1, sibling: 0, child: 0, Object3PropTable);

    WritePropertyTable(story, Object1PropTable, "spec room");
    WritePropertyTable(
        story,
        Object2PropTable,
        "silver key",
        (40, [0x12, 0x34]),
        (33, [0xAA, 0xBB, 0xCC]),
        (20, [0x44]));
    WritePropertyTable(story, Object3PropTable, "brass lamp");
}

static void WriteGlobals(byte[] story)
{
    WriteBE16(story, Globals, 1);
}

static void WriteDictionary(byte[] story)
{
    story[Dictionary] = 1;
    story[Dictionary + 1] = (byte)',';
    story[Dictionary + 2] = 6;
    WriteBE16(story, Dictionary + 3, 4);
    WriteDictionaryEntry(story, DictionaryEntryAddress(0), "take");
    WriteDictionaryEntry(story, DictionaryEntryAddress(1), "silver");
    WriteDictionaryEntry(story, DictionaryEntryAddress(2), ",");
    WriteDictionaryEntry(story, DictionaryEntryAddress(3), "key");
}

static void WriteBuffers(byte[] story)
{
    story[TextBuffer] = 78;
    story[TextBuffer + 1] = 0;
    story[ParseBuffer] = 8;
}

static void WriteObjectEntry(
    byte[] story,
    int objectNumber,
    IReadOnlyList<(int Attribute, bool Set)> attrs,
    int parent,
    int sibling,
    int child,
    int propTable)
{
    int offset = ObjectTable + 126 + ((objectNumber - 1) * 14);
    foreach ((int attr, bool set) in attrs)
    {
        if (!set)
            continue;
        if (attr is < 0 or > 47)
            throw new InvalidOperationException($"Z5 attribute out of range: {attr}");
        story[offset + (attr / 8)] |= (byte)(0x80 >> (attr & 7));
    }

    WriteBE16(story, offset + 6, parent);
    WriteBE16(story, offset + 8, sibling);
    WriteBE16(story, offset + 10, child);
    WriteBE16(story, offset + 12, propTable);
}

static void WritePropertyTable(byte[] story, int offset, string name, params (int Number, byte[] Data)[] properties)
{
    byte[] packedName = ZString.Pack(name);
    story[offset++] = (byte)(packedName.Length / 2);
    Array.Copy(packedName, 0, story, offset, packedName.Length);
    offset += packedName.Length;

    foreach ((int number, byte[] data) in properties.OrderByDescending(p => p.Number))
    {
        if (number is < 1 or > 63)
            throw new InvalidOperationException($"Z5 property number out of range: {number}");
        if (data.Length is < 1 or > 64)
            throw new InvalidOperationException($"Z5 property length out of range: {data.Length}");

        if (data.Length <= 2)
        {
            story[offset++] = (byte)(number | (data.Length == 2 ? 0x40 : 0x00));
        }
        else
        {
            story[offset++] = (byte)(0x80 | number);
            story[offset++] = (byte)(data.Length == 64 ? 0 : data.Length);
        }

        Array.Copy(data, 0, story, offset, data.Length);
        offset += data.Length;
    }

    story[offset] = 0;
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
    z.Print("z5 spec start");
    z.NewLine();

    z.Call1S("routine_return_42", 0x11);
    z.AssertVarEquals(0x11, 42, "call-1s");

    z.Call1S("routine_local_zero", 0x11);
    z.AssertVarEquals(0x11, 42, "v5-local-zero");

    z.Call2S("routine_add_23", 0x11, Operand.Small(19));
    z.AssertVarEquals(0x11, 42, "call-2s");

    z.CallVs2("routine_seventh_arg", 0x11,
        Operand.Small(1),
        Operand.Small(2),
        Operand.Small(3),
        Operand.Small(4),
        Operand.Small(5),
        Operand.Small(6),
        Operand.Small(7));
    z.AssertVarEquals(0x11, 7, "call-vs2-args");

    z.CallVs("routine_check_args", 0x11, Operand.Small(1), Operand.Small(2));
    z.AssertVarEquals(0x11, 42, "check-arg-count");

    z.Call1N("routine_call1n");
    z.CallVn("routine_call_vn", Operand.Small(1));
    z.CallVn2("routine_call_vn2",
        Operand.Small(1),
        Operand.Small(2),
        Operand.Small(3),
        Operand.Small(4),
        Operand.Small(5));

    z.ExtOpStore(9, 0x11);
    z.AssertVarEquals(0x11, 0, "save-undo-unavailable");
    z.ExtOpStore(10, 0x11);
    z.AssertVarEquals(0x11, 0, "restore-undo-unavailable");

    z.ZeroOpStore(9, 0x11);
    z.AssertVarEquals(0x11, 0, "catch-frame");

    z.ZeroOpBranch(15, "piracy_false", branchIf: false);
    z.Fail("piracy");
    z.Label("piracy_false");

    z.OneOp(13, Operand.Large(PackedText / 4));
    z.NewLine();

    z.OneOpStore(3, Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 1, "get-parent-v4");

    z.OneOpStoreBranch(2, Operand.Small(1), 0x11, "get_child_branch_ok");
    z.Fail("get-child-v4");
    z.Label("get_child_branch_ok");
    z.AssertVarEquals(0x11, 2, "get-child-v4-value");

    z.OneOpStoreBranch(1, Operand.Small(2), 0x11, "get_sibling_branch_ok");
    z.Fail("get-sibling-v4");
    z.Label("get_sibling_branch_ok");
    z.AssertVarEquals(0x11, 3, "get-sibling-v4-value");

    z.OneOpStore(3, Operand.Small(0), 0x11);
    z.AssertVarEquals(0x11, 0, "get-parent-zero");

    z.OneOpStoreBranch(1, Operand.Small(0), 0x11, "get_sibling_zero_ok", branchIf: false);
    z.Fail("get-sibling-zero");
    z.Label("get_sibling_zero_ok");
    z.AssertVarEquals(0x11, 0, "get-sibling-zero-value");

    z.OneOpStoreBranch(2, Operand.Small(0), 0x11, "get_child_zero_ok", branchIf: false);
    z.Fail("get-child-zero");
    z.Label("get_child_zero_ok");
    z.AssertVarEquals(0x11, 0, "get-child-zero-value");

    z.TwoOpBranch(6, Operand.Small(2), Operand.Small(1), "jin_ok");
    z.Fail("jin-v4");
    z.Label("jin_ok");

    z.TwoOpBranch(10, Operand.Small(2), Operand.Small(40), "test_attr_ok");
    z.Fail("test-attr-v4");
    z.Label("test_attr_ok");

    z.TwoOpStore(17, Operand.Small(2), Operand.Small(40), 0x11);
    z.AssertVarEquals(0x11, 0x1234, "get-prop-v4");

    z.TwoOpStore(17, Operand.Small(2), Operand.Small(15), 0x11);
    z.AssertVarEquals(0x11, 0x5151, "get-prop-default-v4");

    z.TwoOpStore(18, Operand.Small(2), Operand.Small(40), 0x11);
    z.AssertVarEquals(0x11, Object2Prop40DataAddress(), "get-prop-addr-v4");

    z.OneOpStore(4, Operand.Var(0x11), 0x12);
    z.AssertVarEquals(0x12, 2, "get-prop-len-v4");

    z.TwoOpStore(19, Operand.Small(2), Operand.Small(0), 0x11);
    z.AssertVarEquals(0x11, 40, "next-prop-v4-first");

    z.TwoOpStore(19, Operand.Small(2), Operand.Small(40), 0x11);
    z.AssertVarEquals(0x11, 33, "next-prop-v4-second");

    z.VarOp(3, Operand.Small(2), Operand.Small(40), Operand.Large(0x5678));
    z.TwoOpStore(17, Operand.Small(2), Operand.Small(40), 0x11);
    z.AssertVarEquals(0x11, 0x5678, "put-prop-v4");

    z.TwoOp(14, Operand.Small(2), Operand.Small(1));
    z.OneOpStoreBranch(1, Operand.Small(2), 0x11, "insert-existing-sibling-ok");
    z.Fail("insert-existing-sibling");
    z.Label("insert-existing-sibling-ok");
    z.AssertVarEquals(0x11, 3, "insert-existing-sibling-value");

    z.OneOp(10, Operand.Small(2));
    z.NewLine();

    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(0), Operand.Small('A'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(1), Operand.Small('B'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(2), Operand.Small('C'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(3), Operand.Small('D'));
    z.VarOp(29, Operand.Large(ScratchTable), Operand.Large(ScratchTable + 8), Operand.Small(4));
    z.AssertByteEquals(ScratchTable + 8, 0, 'A', "copy-table-0");
    z.AssertByteEquals(ScratchTable + 8, 3, 'D', "copy-table-3");

    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(0), Operand.Small('A'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(1), Operand.Small('B'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(2), Operand.Small('C'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(3), Operand.Small('D'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(4), Operand.Small('E'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(5), Operand.Small('F'));
    z.VarOp(29, Operand.Large(ScratchTable), Operand.Large(ScratchTable + 2), Operand.Small(4));
    z.AssertByteEquals(ScratchTable, 2, 'A', "copy-table-overlap-pos-2");
    z.AssertByteEquals(ScratchTable, 5, 'D', "copy-table-overlap-pos-5");

    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(0), Operand.Small('A'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(1), Operand.Small('B'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(2), Operand.Small('C'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(3), Operand.Small('D'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(4), Operand.Small('E'));
    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(5), Operand.Small('F'));
    z.VarOp(29, Operand.Large(ScratchTable), Operand.Large(ScratchTable + 2), Operand.Large(unchecked((short)-4)));
    z.AssertByteEquals(ScratchTable, 2, 'A', "copy-table-overlap-neg-2");
    z.AssertByteEquals(ScratchTable, 5, 'B', "copy-table-overlap-neg-5");

    z.VarOp(30, Operand.Large(ScratchTable + 8), Operand.Small(2), Operand.Small(2), Operand.Small(0));
    z.NewLine();

    z.VarOp(1, Operand.Large(ScratchTable + 16), Operand.Small(0), Operand.Large(0x1111));
    z.VarOp(1, Operand.Large(ScratchTable + 16), Operand.Small(1), Operand.Large(0x2222));
    z.VarOpStoreBranch(23, 0x11, "scan_table_ok",
        Operand.Large(0x2222),
        Operand.Large(ScratchTable + 16),
        Operand.Small(2),
        Operand.Small(0x82));
    z.Fail("scan-table");
    z.Label("scan_table_ok");
    z.AssertVarEquals(0x11, ScratchTable + 18, "scan-table-addr");

    z.VarOp(2, Operand.Large(TextBuffer), Operand.Small(0), Operand.Small(20));
    z.VarOp(2, Operand.Large(TextBuffer), Operand.Small(1), Operand.Small(11));
    z.StoreText(TextBuffer + 2, "take silver");
    z.VarOp(27, Operand.Large(TextBuffer), Operand.Large(ParseBuffer));
    z.AssertByteEquals(ParseBuffer, 1, 2, "tokenise-count-v5");
    z.AssertWordEquals(ParseBuffer, 1, DictionaryEntryAddress(0), "tokenise-take-v5");
    z.AssertWordEquals(ParseBuffer, 3, DictionaryEntryAddress(1), "tokenise-silver-v5");

    z.StoreText(ScratchTable + 48, "take");
    z.VarOp(28,
        Operand.Large(ScratchTable + 48),
        Operand.Small(4),
        Operand.Small(0),
        Operand.Large(ScratchTable + 60));
    byte[] encodedTake = ZString.PackDictionaryEntry("take", fixedWords: 3);
    for (int i = 0; i < encodedTake.Length; i++)
        z.AssertByteEquals(ScratchTable + 60, i, encodedTake[i], $"encode-text-take-{i}");

    z.VarOp(19, Operand.Small(3), Operand.Large(ScratchTable + 32));
    z.Print("hide");
    z.VarOp(19, Operand.Large(unchecked((short)-3)));
    z.AssertWordEquals(ScratchTable + 32, 0, 4, "stream3-count-v5");
    z.AssertByteEquals(ScratchTable + 32, 2, 'h', "stream3-char-0-v5");
    z.AssertByteEquals(ScratchTable + 32, 5, 'e', "stream3-char-3-v5");
    z.Print("stream3 ok");
    z.NewLine();

    z.VarOp(10, Operand.Small(2));
    z.VarOp(11, Operand.Small(1));
    z.VarOp(15, Operand.Small(2), Operand.Small(1));
    z.Print("upper5");
    z.VarOp(14, Operand.Small(1));
    z.VarOp(11, Operand.Small(0));
    z.VarOp(13, Operand.Large(unchecked((short)-2)));

    z.Print("z5 spec ok");
    z.NewLine();

    z.Jump("after_routines");

    z.Align(4);
    z.Label("routine_return_42");
    z.Byte(0);
    z.OneOp(11, Operand.Small(42));

    z.Align(4);
    z.Label("routine_local_zero");
    z.Byte(1);
    z.TwoOpBranch(1, Operand.Var(1), Operand.Small(0), "local_zero_ok");
    z.OneOp(11, Operand.Small(99));
    z.Label("local_zero_ok");
    z.OneOp(11, Operand.Small(42));

    z.Align(4);
    z.Label("routine_add_23");
    z.Byte(1);
    z.TwoOpStore(20, Operand.Var(1), Operand.Small(23), 1);
    z.OneOp(11, Operand.Var(1));

    z.Align(4);
    z.Label("routine_seventh_arg");
    z.Byte(7);
    z.OneOp(11, Operand.Var(7));

    z.Align(4);
    z.Label("routine_check_args");
    z.Byte(2);
    z.VarOpBranch(31, "check_args_ok", Operand.Small(2));
    z.OneOp(11, Operand.Small(99));
    z.Label("check_args_ok");
    z.OneOp(11, Operand.Small(42));

    z.Align(4);
    z.Label("routine_call1n");
    z.Byte(0);
    z.Print("call1n ok");
    z.NewLine();
    z.ZeroOp(0);

    z.Align(4);
    z.Label("routine_call_vn");
    z.Byte(1);
    z.Print("callvn ok");
    z.NewLine();
    z.ZeroOp(0);

    z.Align(4);
    z.Label("routine_call_vn2");
    z.Byte(5);
    z.Print("callvn2 ok");
    z.NewLine();
    z.ZeroOp(0);

    z.Label("after_routines");
    z.Label("prompt");
    z.Print(">");
    z.VarOpStore(4, 0x11, Operand.Large(TextBuffer), Operand.Large(ParseBuffer));
    z.AssertVarEquals(0x11, 13, "read-result-v5");
    z.AssertByteEquals(ParseBuffer, 1, 4, "parse-count-v5");
    z.AssertWordEquals(ParseBuffer, 1, DictionaryEntryAddress(0), "parse-take-addr-v5");
    z.AssertByteEquals(ParseBuffer, 4, 4, "parse-take-len-v5");
    z.AssertByteEquals(ParseBuffer, 5, 2, "parse-take-pos-v5");
    z.AssertWordEquals(ParseBuffer, 3, DictionaryEntryAddress(1), "parse-silver-addr-v5");
    z.AssertByteEquals(ParseBuffer, 8, 6, "parse-silver-len-v5");
    z.AssertByteEquals(ParseBuffer, 9, 7, "parse-silver-pos-v5");
    z.AssertWordEquals(ParseBuffer, 5, DictionaryEntryAddress(2), "parse-comma-addr-v5");
    z.AssertByteEquals(ParseBuffer, 12, 1, "parse-comma-len-v5");
    z.AssertByteEquals(ParseBuffer, 13, 13, "parse-comma-pos-v5");
    z.AssertWordEquals(ParseBuffer, 7, DictionaryEntryAddress(3), "parse-key-addr-v5");
    z.AssertByteEquals(ParseBuffer, 16, 3, "parse-key-len-v5");
    z.AssertByteEquals(ParseBuffer, 17, 14, "parse-key-pos-v5");
    z.Print("z5 input ok");
    z.NewLine();
    z.Jump("prompt");
}

static void WriteBE16(byte[] data, int offset, int value)
{
    data[offset] = High(value);
    data[offset + 1] = Low(value);
}

static byte High(int value) => (byte)((value >> 8) & 0xFF);
static byte Low(int value) => (byte)(value & 0xFF);
static int Object2Prop40DataAddress() => Object2PropTable + 1 + ZString.Pack("silver key").Length + 1;
static int DictionaryEntryAddress(int index) => Dictionary + 5 + (index * 6);

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
    private readonly List<byte> _bytes = [];
    private readonly Dictionary<string, int> _labels = new(StringComparer.Ordinal);
    private readonly List<BranchPatch> _branchPatches = [];
    private readonly List<JumpPatch> _jumpPatches = [];
    private readonly List<PackedAddressPatch> _packedAddressPatches = [];
    private int Pc => _origin + _bytes.Count;

    public ZCode(int origin, int packedAddressScale)
    {
        _origin = origin;
        _packedAddressScale = packedAddressScale;
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

    public void ExtOpStore(int op, byte store)
    {
        Emit(0xBE);
        Emit(op & 0xFF);
        Emit(0xFF);
        Emit(store);
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

    public void CallVs2(string routineLabel, byte store, params Operand[] args)
    {
        EmitCallWithOptionalStore(12, routineLabel, args, store);
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

    public void CallVn2(string routineLabel, params Operand[] args)
    {
        EmitCallWithOptionalStore(26, routineLabel, args, store: null);
    }

    private void EmitCallWithOptionalStore(int op, string routineLabel, IReadOnlyList<Operand> args, byte? store)
    {
        Operand[] operands = [Operand.Large(0), .. args];
        Emit(0xE0 | (op & 0x1F));
        (byte first, byte second) = TypeBytes8(operands);
        Emit(first);
        Emit(second);
        EmitPackedAddressPatch(routineLabel);
        foreach (var arg in args)
            EmitOperand(arg);
        if (store is { } storeVar)
            Emit(storeVar);
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

    public void StoreText(int address, string text)
    {
        for (int i = 0; i < text.Length; i++)
            VarOp(2, Operand.Large(address), Operand.Small(i), Operand.Small(text[i]));
    }

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
        Print($"z5 spec fail {name}");
        NewLine();
        Emit(0xBA);
    }

    public byte[] ToArray()
    {
        foreach (var patch in _packedAddressPatches)
        {
            int target = ResolveLabel(patch.Label);
            if ((target % _packedAddressScale) != 0)
                throw new InvalidOperationException($"Routine label is not aligned to {_packedAddressScale}: {patch.Label} at ${target:X4}");
            WriteWordAt(patch.Offset, target / _packedAddressScale);
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
            throw new ArgumentOutOfRangeException(nameof(operands), "Use TypeBytes8 for more than four operands.");

        int value = 0;
        for (int i = 0; i < 4; i++)
        {
            int kind = i < operands.Count ? (int)operands[i].Kind : 3;
            value |= kind << (6 - (i * 2));
        }

        return (byte)value;
    }

    private static (byte First, byte Second) TypeBytes8(IReadOnlyList<Operand> operands)
    {
        if (operands.Count > 8)
            throw new ArgumentOutOfRangeException(nameof(operands), "Z5 double-variable instructions support up to eight operands.");

        int first = 0;
        int second = 0;
        for (int i = 0; i < 8; i++)
        {
            int kind = i < operands.Count ? (int)operands[i].Kind : 3;
            if (i < 4)
                first |= kind << (6 - (i * 2));
            else
                second |= kind << (6 - ((i - 4) * 2));
        }

        return ((byte)first, (byte)second);
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
