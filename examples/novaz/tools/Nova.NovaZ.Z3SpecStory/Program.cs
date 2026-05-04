using System.Text;

const int StorySize = 0x1000;
const int InitialPc = 0x0040;
const int ObjectTable = 0x0C00;
const int Object1PropTable = 0x0C70;
const int Object2PropTable = 0x0C90;
const int Object3PropTable = 0x0CB0;
const int Globals = 0x0CD0;
const int Abbreviations = 0x0DC0;
const int Dictionary = 0x0E80;
const int TextBuffer = 0x0EA0;
const int ParseBuffer = 0x0EF0;
const int ScratchTable = 0x0F20;
const int PrintAddrText = 0x0F50;
const int PrintPaddrText = 0x0F60;
const int AbbrevStringText = 0x0F70;
const int AbbrevUseText = 0x0F80;
const int EscapeText = 0x0F90;
const int StaticMemory = 0x1000;

string outputPath = ParseOutput(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story);
WriteObjectTable(story);
WriteGlobals(story);
WriteDictionary(story);
WriteBuffers(story);
WriteBE16(story, Abbreviations, AbbrevStringText / 2);
WritePackedText(story, PrintAddrText, "addr");
WritePackedText(story, PrintPaddrText, "paddr");
WritePackedText(story, AbbrevStringText, "abbr");
WritePackedZChars(story, AbbrevUseText, [1, 0, 5]);
WritePackedZChars(story, EscapeText, [5, 6, 2, 0, 5, 5]);

var code = new ZCode(InitialPc);
EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (InitialPc + codeBytes.Length >= ObjectTable)
    throw new InvalidOperationException($"Z3 spec program overlaps data tables: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, InitialPc, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes)");

static string ParseOutput(string[] args)
{
    string output = "build/z3-spec.z3";
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
    Console.Error.WriteLine("Nova.NovaZ.Z3SpecStory");
    Console.Error.WriteLine("  --output <story.z3>");
}

static void WriteHeader(byte[] story)
{
    story[0x00] = 3;
    story[0x01] = 0;
    WriteBE16(story, 0x02, 1);
    WriteBE16(story, 0x04, InitialPc);
    WriteBE16(story, 0x06, InitialPc);
    WriteBE16(story, 0x08, Dictionary);
    WriteBE16(story, 0x0A, ObjectTable);
    WriteBE16(story, 0x0C, Globals);
    WriteBE16(story, 0x0E, StaticMemory);
    WriteBE16(story, 0x10, 0);
    Encoding.ASCII.GetBytes("Z3SPEC").CopyTo(story, 0x12);
    WriteBE16(story, 0x18, Abbreviations);
    WriteBE16(story, 0x1A, StorySize / 2);
}

static void WriteObjectTable(byte[] story)
{
    WriteBE16(story, ObjectTable + ((6 - 1) * 2), 0x6666);
    WriteBE16(story, ObjectTable + ((7 - 1) * 2), 0x7777);

    WriteObjectEntry(story, 1, attrs: 0, parent: 0, sibling: 0, child: 2, Object1PropTable);
    WriteObjectEntry(story, 2, attrs: 0x04000000, parent: 1, sibling: 3, child: 0, Object2PropTable);
    WriteObjectEntry(story, 3, attrs: 0, parent: 1, sibling: 0, child: 0, Object3PropTable);

    WritePropertyTable(story, Object1PropTable, "spec lab");
    WritePropertyTable(story, Object2PropTable, "red key", (10, [0x12, 0x34]), (7, [0x44]));
    WritePropertyTable(story, Object3PropTable, "blue key", (5, [0xAB, 0xCD]));
}

static void WriteGlobals(byte[] story)
{
    WriteBE16(story, Globals, 1);
}

static void WriteDictionary(byte[] story)
{
    story[Dictionary] = 1;
    story[Dictionary + 1] = (byte)',';
    story[Dictionary + 2] = 4;
    WriteBE16(story, Dictionary + 3, 4);
    WriteDictionaryEntry(story, DictionaryEntryAddress(0), "take");
    WriteDictionaryEntry(story, DictionaryEntryAddress(1), "red");
    WriteDictionaryEntry(story, DictionaryEntryAddress(2), ",");
    WriteDictionaryEntry(story, DictionaryEntryAddress(3), "key");
}

static void WriteBuffers(byte[] story)
{
    story[TextBuffer] = 78;
    story[ParseBuffer] = 8;
}

static void WriteObjectEntry(byte[] story, int objectNumber, int attrs, int parent, int sibling, int child, int propTable)
{
    int offset = ObjectTable + 62 + ((objectNumber - 1) * 9);
    story[offset] = (byte)((attrs >> 24) & 0xFF);
    story[offset + 1] = (byte)((attrs >> 16) & 0xFF);
    story[offset + 2] = (byte)((attrs >> 8) & 0xFF);
    story[offset + 3] = (byte)(attrs & 0xFF);
    story[offset + 4] = (byte)parent;
    story[offset + 5] = (byte)sibling;
    story[offset + 6] = (byte)child;
    story[offset + 7] = High(propTable);
    story[offset + 8] = Low(propTable);
}

static void WritePropertyTable(byte[] story, int offset, string name, params (int Number, byte[] Data)[] properties)
{
    byte[] packedName = ZString.Pack(name);
    story[offset++] = (byte)(packedName.Length / 2);
    Array.Copy(packedName, 0, story, offset, packedName.Length);
    offset += packedName.Length;

    foreach ((int number, byte[] data) in properties.OrderByDescending(p => p.Number))
    {
        if (number is < 1 or > 31)
            throw new InvalidOperationException($"Z3 property number out of range: {number}");
        if (data.Length is < 1 or > 8)
            throw new InvalidOperationException($"Z3 property length out of range: {data.Length}");
        story[offset++] = (byte)(((data.Length - 1) << 5) | number);
        Array.Copy(data, 0, story, offset, data.Length);
        offset += data.Length;
    }

    story[offset] = 0;
}

static void WriteDictionaryEntry(byte[] story, int offset, string text)
{
    byte[] encoded = ZString.PackDictionaryEntry(text);
    Array.Copy(encoded, 0, story, offset, encoded.Length);
}

static void WritePackedText(byte[] story, int offset, string text)
{
    byte[] encoded = ZString.Pack(text);
    Array.Copy(encoded, 0, story, offset, encoded.Length);
}

static void WritePackedZChars(byte[] story, int offset, IReadOnlyList<int> zchars)
{
    byte[] encoded = ZString.PackZChars(zchars);
    Array.Copy(encoded, 0, story, offset, encoded.Length);
}

static void EmitSpecProgram(ZCode z)
{
    z.Print("z3 spec start");
    z.NewLine();

    z.TwoOpStore(20, Operand.Small(40), Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 42, "add");

    z.TwoOpStore(21, Operand.Small(100), Operand.Small(58), 0x11);
    z.AssertVarEquals(0x11, 42, "sub");

    z.TwoOpStore(22, Operand.Small(6), Operand.Small(7), 0x11);
    z.AssertVarEquals(0x11, 42, "mul");

    z.TwoOpStore(23, Operand.Small(127), Operand.Small(3), 0x11);
    z.AssertVarEquals(0x11, 42, "div");

    z.TwoOpStore(24, Operand.Small(128), Operand.Small(43), 0x11);
    z.AssertVarEquals(0x11, 42, "mod");

    z.TwoOpStore(8, Operand.Large(0x4000), Operand.Small(0x20), 0x11);
    z.AssertVarEquals(0x11, 0x4020, "or");

    z.TwoOpStore(9, Operand.Large(0x4021), Operand.Small(0x00FF), 0x11);
    z.AssertVarEquals(0x11, 0x21, "and");

    z.OneOpStore(15, Operand.Large(0xFF00), 0x11);
    z.AssertVarEquals(0x11, 0x00FF, "not");

    z.OneOpBranch(0, Operand.Small(0), "jz-ok");
    z.Fail("jz");
    z.Label("jz-ok");

    z.OneOpBranch(0, Operand.Small(1), "jz-false-ok", branchIf: false);
    z.Fail("jz-false");
    z.Label("jz-false-ok");

    z.TwoOpBranch(2, Operand.Large(-1), Operand.Small(1), "jl-signed-ok");
    z.Fail("jl-signed");
    z.Label("jl-signed-ok");

    z.TwoOpBranch(3, Operand.Small(1), Operand.Large(-1), "jg-signed-ok");
    z.Fail("jg-signed");
    z.Label("jg-signed-ok");

    z.TwoOpBranch(7, Operand.Large(0x0F0F), Operand.Small(0x000F), "test-bitmap-ok");
    z.Fail("test-bitmap");
    z.Label("test-bitmap-ok");

    z.TwoOpBranch(7, Operand.Large(0x0F00), Operand.Small(0x000F), "test-bitmap-false-ok", branchIf: false);
    z.Fail("test-bitmap-false");
    z.Label("test-bitmap-false-ok");

    z.TwoOpVarBranch(1, "je4-ok", Operand.Small(7), Operand.Small(1), Operand.Small(7), Operand.Small(9));
    z.Fail("je4");
    z.Label("je4-ok");

    z.Store(0x11, 41);
    z.OneOp(5, Operand.Small(0x11));
    z.AssertVarEquals(0x11, 42, "inc");

    z.OneOp(6, Operand.Small(0x11));
    z.AssertVarEquals(0x11, 41, "dec");

    z.OneOpStore(14, Operand.Small(0x11), 0x12);
    z.AssertVarEquals(0x12, 41, "load-var");

    z.TwoOp(13, Operand.Small(0x11), Operand.Small(41));
    z.TwoOpBranch(5, Operand.Small(0x11), Operand.Small(41), "inc-chk-ok");
    z.Fail("inc-chk");
    z.Label("inc-chk-ok");
    z.AssertVarEquals(0x11, 42, "inc-chk-value");

    z.TwoOp(13, Operand.Small(0x11), Operand.Small(42));
    z.TwoOpBranch(4, Operand.Small(0x11), Operand.Small(42), "dec-chk-ok");
    z.Fail("dec-chk");
    z.Label("dec-chk-ok");
    z.AssertVarEquals(0x11, 41, "dec-chk-value");

    z.VarOp(1, Operand.Large(ScratchTable), Operand.Small(0), Operand.Large(0x1234));
    z.TwoOpStore(15, Operand.Large(ScratchTable), Operand.Small(0), 0x11);
    z.AssertVarEquals(0x11, 0x1234, "loadw");

    z.VarOp(2, Operand.Large(ScratchTable), Operand.Small(2), Operand.Small(0x56));
    z.TwoOpStore(16, Operand.Large(ScratchTable), Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 0x56, "loadb");

    z.VarOp(8, Operand.Large(0x0BAD));
    z.VarOp(9, Operand.Small(0x11));
    z.AssertVarEquals(0x11, 0x0BAD, "stack");

    z.VarOp(8, Operand.Small(1));
    z.VarOp(8, Operand.Small(2));
    z.ZeroOp(9);
    z.VarOp(9, Operand.Small(0x11));
    z.AssertVarEquals(0x11, 1, "pop");

    z.VarOpStore(7, 0x11, Operand.Large(unchecked((short)-7)));
    z.VarOpStore(7, 0x11, Operand.Small(10));
    z.AssertVarGreaterThan(0x11, 0, "random-low");
    z.AssertVarLessThan(0x11, 11, "random-high");

    z.Call1S("routine_return_42", 0x11);
    z.AssertVarEquals(0x11, 42, "call-ret");

    z.Call1S("routine_rtrue", 0x11);
    z.AssertVarEquals(0x11, 1, "rtrue");

    z.Call1S("routine_rfalse", 0x11);
    z.AssertVarEquals(0x11, 0, "rfalse");

    z.Call1S("routine_ret_popped", 0x11);
    z.AssertVarEquals(0x11, 66, "ret-popped");

    z.Call1S("routine_print_ret", 0x11);
    z.AssertVarEquals(0x11, 1, "print-ret");

    z.CallVs("routine_arg_defaults", 0x11, Operand.Small(7));
    z.AssertVarEquals(0x11, 9, "call-vs-args");

    z.OneOp(7, Operand.Large(PrintAddrText));
    z.OneOp(13, Operand.Large(PrintPaddrText / 2));
    z.VarOp(5, Operand.Small('A'));
    z.VarOp(6, Operand.Large(-12));
    z.NewLine();
    z.OneOp(7, Operand.Large(AbbrevUseText));
    z.OneOp(7, Operand.Large(EscapeText));
    z.NewLine();

    z.VarOp(19, Operand.Small(3), Operand.Large(ScratchTable));
    z.Print("abc");
    z.VarOp(19, Operand.Large(unchecked((short)-3)));
    z.TwoOpStore(15, Operand.Large(ScratchTable), Operand.Small(0), 0x11);
    z.AssertVarEquals(0x11, 3, "stream3-count");
    z.TwoOpStore(16, Operand.Large(ScratchTable), Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 'a', "stream3-byte");

    z.OneOpStore(3, Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 1, "get-parent");

    z.OneOpStoreBranch(2, Operand.Small(1), 0x11, "get_child_branch_ok");
    z.Fail("get-child-branch");
    z.Label("get_child_branch_ok");
    z.AssertVarEquals(0x11, 2, "get-child");

    z.OneOpStoreBranch(1, Operand.Small(2), 0x11, "get_sibling_branch_ok");
    z.Fail("get-sibling-branch");
    z.Label("get_sibling_branch_ok");
    z.AssertVarEquals(0x11, 3, "get-sibling");

    z.OneOpStoreBranch(1, Operand.Small(3), 0x11, "get_sibling_empty_ok", branchIf: false);
    z.Fail("get-sibling-empty");
    z.Label("get_sibling_empty_ok");
    z.AssertVarEquals(0x11, 0, "get-sibling-empty-value");

    z.TwoOpBranch(6, Operand.Small(2), Operand.Small(1), "jin_ok");
    z.Fail("jin");
    z.Label("jin_ok");

    z.TwoOpBranch(10, Operand.Small(2), Operand.Small(5), "test_attr_initial_ok");
    z.Fail("test-attr-initial");
    z.Label("test_attr_initial_ok");

    z.TwoOp(12, Operand.Small(2), Operand.Small(5));
    z.TwoOpBranch(10, Operand.Small(2), Operand.Small(5), "test_attr_clear_ok", branchIf: false);
    z.Fail("clear-attr");
    z.Label("test_attr_clear_ok");

    z.TwoOp(11, Operand.Small(2), Operand.Small(5));
    z.TwoOpBranch(10, Operand.Small(2), Operand.Small(5), "test_attr_set_ok");
    z.Fail("set-attr");
    z.Label("test_attr_set_ok");

    z.TwoOpStore(17, Operand.Small(2), Operand.Small(10), 0x11);
    z.AssertVarEquals(0x11, 0x1234, "get-prop-word");

    z.TwoOpStore(17, Operand.Small(2), Operand.Small(7), 0x11);
    z.AssertVarEquals(0x11, 0x44, "get-prop-byte");

    z.TwoOpStore(17, Operand.Small(2), Operand.Small(6), 0x11);
    z.AssertVarEquals(0x11, 0x6666, "get-prop-default");

    z.TwoOpStore(18, Operand.Small(2), Operand.Small(10), 0x11);
    z.AssertVarEquals(0x11, Object2Prop10DataAddress(), "get-prop-addr");

    z.OneOpStore(4, Operand.Var(0x11), 0x12);
    z.AssertVarEquals(0x12, 2, "get-prop-len");

    z.TwoOpStore(19, Operand.Small(2), Operand.Small(0), 0x11);
    z.AssertVarEquals(0x11, 10, "get-next-prop-first");

    z.TwoOpStore(19, Operand.Small(2), Operand.Small(10), 0x11);
    z.AssertVarEquals(0x11, 7, "get-next-prop-second");

    z.TwoOpStore(19, Operand.Small(2), Operand.Small(7), 0x11);
    z.AssertVarEquals(0x11, 0, "get-next-prop-end");

    z.VarOp(3, Operand.Small(2), Operand.Small(10), Operand.Large(0x5678));
    z.TwoOpStore(17, Operand.Small(2), Operand.Small(10), 0x11);
    z.AssertVarEquals(0x11, 0x5678, "put-prop");

    z.OneOp(10, Operand.Small(2));
    z.NewLine();

    z.OneOp(9, Operand.Small(2));
    z.OneOpStore(3, Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 0, "remove-parent");
    z.OneOpStoreBranch(2, Operand.Small(1), 0x11, "remove_child_branch_ok");
    z.Fail("remove-child-branch");
    z.Label("remove_child_branch_ok");
    z.AssertVarEquals(0x11, 3, "remove-child-link");

    z.TwoOp(14, Operand.Small(2), Operand.Small(3));
    z.OneOpStore(3, Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 3, "insert-parent");
    z.OneOpStoreBranch(2, Operand.Small(3), 0x11, "insert_child_branch_ok");
    z.Fail("insert-child-branch");
    z.Label("insert_child_branch_ok");
    z.AssertVarEquals(0x11, 2, "insert-child-link");

    z.TwoOp(14, Operand.Small(2), Operand.Small(1));
    z.OneOpStore(3, Operand.Small(2), 0x11);
    z.AssertVarEquals(0x11, 1, "reinsert-parent");
    z.OneOpStoreBranch(1, Operand.Small(2), 0x11, "reinsert_sibling_branch_ok");
    z.Fail("reinsert-sibling-branch");
    z.Label("reinsert_sibling_branch_ok");
    z.AssertVarEquals(0x11, 3, "reinsert-sibling");

    z.Verify("verify_ok");
    z.Fail("verify");
    z.Label("verify_ok");

    z.Jump("after_routines");

    z.AlignWord();
    z.Label("routine_return_42");
    z.Byte(0);
    z.OneOp(11, Operand.Small(42));

    z.AlignWord();
    z.Label("routine_rtrue");
    z.Byte(0);
    z.ZeroOp(0);

    z.AlignWord();
    z.Label("routine_rfalse");
    z.Byte(0);
    z.ZeroOp(1);

    z.AlignWord();
    z.Label("routine_ret_popped");
    z.Byte(0);
    z.VarOp(8, Operand.Small(66));
    z.ZeroOp(8);

    z.AlignWord();
    z.Label("routine_print_ret");
    z.Byte(0);
    z.PrintRet("pret");

    z.AlignWord();
    z.Label("routine_arg_defaults");
    z.Byte(2);
    z.Word(1);
    z.Word(2);
    z.TwoOpStore(20, Operand.Var(1), Operand.Var(2), 1);
    z.OneOp(11, Operand.Var(1));

    z.Label("after_routines");
    z.Store(0x11, 0);
    z.Store(0x12, 0);
    z.Print("z3 spec ok");
    z.NewLine();

    z.Label("prompt");
    z.Print(">");
    z.VarOp(4, Operand.Large(TextBuffer), Operand.Large(ParseBuffer));
    z.AssertByteEquals(ParseBuffer, 1, 4, "parse-count");
    z.AssertWordEquals(ParseBuffer, 1, DictionaryEntryAddress(0), "parse-take-addr");
    z.AssertByteEquals(ParseBuffer, 4, 4, "parse-take-len");
    z.AssertByteEquals(ParseBuffer, 5, 1, "parse-take-pos");
    z.AssertWordEquals(ParseBuffer, 3, DictionaryEntryAddress(1), "parse-red-addr");
    z.AssertByteEquals(ParseBuffer, 8, 3, "parse-red-len");
    z.AssertByteEquals(ParseBuffer, 9, 6, "parse-red-pos");
    z.AssertWordEquals(ParseBuffer, 5, DictionaryEntryAddress(2), "parse-comma-addr");
    z.AssertByteEquals(ParseBuffer, 12, 1, "parse-comma-len");
    z.AssertByteEquals(ParseBuffer, 13, 9, "parse-comma-pos");
    z.AssertWordEquals(ParseBuffer, 7, DictionaryEntryAddress(3), "parse-key-addr");
    z.AssertByteEquals(ParseBuffer, 16, 3, "parse-key-len");
    z.AssertByteEquals(ParseBuffer, 17, 10, "parse-key-pos");
    z.Print("z3 input ok");
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
static int Object2Prop10DataAddress() => Object2PropTable + 1 + ZString.Pack("red key").Length + 1;
static int DictionaryEntryAddress(int index) => Dictionary + 5 + (index * 4);

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
    private readonly List<byte> _bytes = [];
    private readonly Dictionary<string, int> _labels = new(StringComparer.Ordinal);
    private readonly List<BranchPatch> _branchPatches = [];
    private readonly List<JumpPatch> _jumpPatches = [];
    private readonly List<PackedAddressPatch> _packedAddressPatches = [];
    private int Pc => _origin + _bytes.Count;

    public ZCode(int origin)
    {
        _origin = origin;
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

    public void PrintRet(string text)
    {
        Emit(0xB3);
        EmitRange(ZString.Pack(text));
    }

    public void NewLine() => Emit(0xBB);

    public void Verify(string label)
    {
        Emit(0xBD);
        Branch(label, branchIf: true);
    }

    public void ZeroOp(int op) => Emit(0xB0 | (op & 0x0F));

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

    public void OneOpBranch(int op, Operand operand, string label, bool branchIf = true)
    {
        OneOp(op, operand);
        Branch(label, branchIf);
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

    public void TwoOpVarBranch(int op, string label, params Operand[] operands)
    {
        Emit(0xC0 | (op & 0x1F));
        Emit(TypeByte(operands));
        foreach (var operand in operands)
            EmitOperand(operand);
        Branch(label, branchIf: true);
    }

    public void Store(byte variable, int value) =>
        TwoOp(13, Operand.Small(variable), Operand.Large(value));

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

    public void Call1S(string routineLabel, byte store)
    {
        Emit(0x88);
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _packedAddressPatches.Add(new PackedAddressPatch(patchOffset, routineLabel));
        Emit(store);
    }

    public void CallVs(string routineLabel, byte store, params Operand[] args)
    {
        Operand[] operands = [Operand.Large(0), .. args];
        Emit(0xE0);
        Emit(TypeByte(operands));
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _packedAddressPatches.Add(new PackedAddressPatch(patchOffset, routineLabel));
        foreach (var arg in args)
            EmitOperand(arg);
        Emit(store);
    }

    public void Jump(string label)
    {
        Emit(0x8C);
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _jumpPatches.Add(new JumpPatch(patchOffset, label));
    }

    public void AlignWord()
    {
        if ((Pc & 1) != 0)
            Emit(0xB4);
    }

    public void Byte(int value) => Emit(value);

    public void Word(int value) => EmitWord(value);

    public void AssertVarEquals(byte variable, int expected, string name)
    {
        string ok = $"ok_{name}_{_branchPatches.Count}";
        TwoOpBranch(1, Operand.Var(variable), Operand.Large(expected), ok);
        Fail(name);
        Label(ok);
    }

    public void AssertVarGreaterThan(byte variable, int expected, string name)
    {
        string ok = $"ok_{name}_{_branchPatches.Count}";
        TwoOpBranch(3, Operand.Var(variable), Operand.Large(expected), ok);
        Fail(name);
        Label(ok);
    }

    public void AssertVarLessThan(byte variable, int expected, string name)
    {
        string ok = $"ok_{name}_{_branchPatches.Count}";
        TwoOpBranch(2, Operand.Var(variable), Operand.Large(expected), ok);
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
        Print($"z3 spec fail {name}");
        NewLine();
        Emit(0xBA);
    }

    public byte[] ToArray()
    {
        foreach (var patch in _packedAddressPatches)
        {
            int target = ResolveLabel(patch.Label);
            if ((target & 1) != 0)
                throw new InvalidOperationException($"Routine label is not word-aligned: {patch.Label} at ${target:X4}");
            WriteWordAt(patch.Offset, target / 2);
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

    public void TwoOp(int op, Operand a, Operand b)
    {
        Emit(0xC0 | (op & 0x1F));
        Emit(TypeByte([a, b]));
        EmitOperand(a);
        EmitOperand(b);
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
            throw new ArgumentOutOfRangeException(nameof(operands), "Z3 variable-form instructions support up to four operands.");

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

    public static byte[] PackDictionaryEntry(string text) => Pack(text, fixedWords: 2);

    public static byte[] PackZChars(IReadOnlyList<int> zchars)
    {
        if (zchars.Any(z => z is < 0 or > 31))
            throw new InvalidOperationException("Packed Z-characters must be in the 0..31 range.");

        var padded = zchars.ToList();
        while (padded.Count % 3 != 0)
            padded.Add(5);

        return PackWords(padded);
    }

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
            throw new InvalidOperationException($"Dictionary entry is too long for Z3 encoding: {text}");

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
