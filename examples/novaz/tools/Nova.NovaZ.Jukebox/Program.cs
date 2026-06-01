using System.Text;

// Nova.NovaZ.Jukebox — generates a tiny V3 Z-story that auditions a game's
// sampled sounds. On boot it plays the first sound and prints its number; each
// RETURN advances to the next number in the supplied list, wrapping around. It
// is packaged with a game's real SOUND.PAK to make a mount-and-run "Infocom
// sound museum" .ndi, so every sound can be heard without a walkthrough.

const int StorySize = 0x1000;
const int InitialPc = 0x0040;
const int ObjectTable = 0x0C00;
const int Object1PropTable = 0x0C70;
const int Globals = 0x0CD0;
const int Dictionary = 0x0E80;
const int TextBuffer = 0x0EA0;
const int ParseBuffer = 0x0EF0;
const int NumbersTable = 0x0F20;
const int StaticMemory = 0x1000;

(string outputPath, int[] numbers, string title) = ParseArgs(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story, title);
WriteObjectTable(story, title);
WriteGlobals(story);
WriteDictionary(story);
WriteBuffers(story);

if (numbers.Length is 0 or > 250)
    throw new InvalidOperationException($"Need 1..250 sound numbers, got {numbers.Length}.");
for (int i = 0; i < numbers.Length; i++)
    story[NumbersTable + i] = (byte)numbers[i];

var code = new ZCode(InitialPc);
EmitJukebox(code, numbers.Length);
byte[] codeBytes = code.ToArray();
if (InitialPc + codeBytes.Length >= ObjectTable)
    throw new InvalidOperationException($"Jukebox program overlaps data tables: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, InitialPc, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, {numbers.Length} sounds: {string.Join(",", numbers)})");

static (string Output, int[] Numbers, string Title) ParseArgs(string[] args)
{
    string output = "build/jukebox.z3";
    int[] numbers = [3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18];
    string title = "Sound Jukebox";
    for (int i = 0; i < args.Length; i++)
    {
        string? value = i + 1 < args.Length ? args[i + 1] : null;
        switch (args[i])
        {
            case "--output" when value is not null:
                output = value;
                i++;
                break;
            case "--numbers" when value is not null:
                numbers = value.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                    .Select(int.Parse).ToArray();
                i++;
                break;
            case "--title" when value is not null:
                title = value;
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

    return (output, numbers, title);
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova.NovaZ.Jukebox");
    Console.Error.WriteLine("  --output <jukebox.z3>");
    Console.Error.WriteLine("  --numbers 3,4,6,7,...   sound_effect numbers to cycle (default 3..18)");
    Console.Error.WriteLine("  --title <text>          status-line / banner title");
}

static void EmitJukebox(ZCode z, int count)
{
    z.Print("Nova Infocom Sound Jukebox");
    z.NewLine();
    z.Print("Press RETURN to play each sampled sound.");
    z.NewLine();
    z.NewLine();
    z.Store(0x10, 0);                                              // index = 0

    z.Label("loop");
    z.TwoOpStore(16, Operand.Large(NumbersTable), Operand.Var(0x10), 0x11); // loadb table,index -> num
    z.Print("Sound #");
    z.VarOp(6, Operand.Var(0x11));                                 // print_num num
    z.NewLine();
    z.VarOp(21, Operand.Var(0x11), Operand.Small(2), Operand.Small(8)); // sound_effect num, start, vol 8
    z.Print(">");                                                 // bare prompt so the wait is detectable
    z.VarOp(4, Operand.Large(TextBuffer), Operand.Large(ParseBuffer)); // sread (wait for RETURN; ignored)
    z.OneOp(5, Operand.Small(0x10));                              // inc index
    z.TwoOpBranch(1, Operand.Var(0x10), Operand.Small(count), "reset"); // je index,count ?reset
    z.Jump("loop");
    z.Label("reset");
    z.Store(0x10, 0);
    z.Jump("loop");
}

static void WriteHeader(byte[] story, string title)
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
    Encoding.ASCII.GetBytes("JUKEBX").CopyTo(story, 0x12);
    WriteBE16(story, 0x18, 0);
    WriteBE16(story, 0x1A, StorySize / 2);
}

static void WriteObjectTable(byte[] story, string title)
{
    // Object 1 is the "room" shown on the V3 status line (global 0 = 1).
    int offset = ObjectTable + 62;                  // after the 31 property defaults
    story[offset + 4] = 0;                          // parent
    story[offset + 5] = 0;                          // sibling
    story[offset + 6] = 0;                          // child
    story[offset + 7] = High(Object1PropTable);
    story[offset + 8] = Low(Object1PropTable);

    byte[] packedName = ZString.Pack(title);
    story[Object1PropTable] = (byte)(packedName.Length / 2);
    Array.Copy(packedName, 0, story, Object1PropTable + 1, packedName.Length);
    story[Object1PropTable + 1 + packedName.Length] = 0; // property terminator
}

static void WriteGlobals(byte[] story) => WriteBE16(story, Globals, 1); // global 0 = object 1

static void WriteDictionary(byte[] story)
{
    story[Dictionary] = 0;          // no word separators
    story[Dictionary + 1] = 4;      // entry length
    WriteBE16(story, Dictionary + 2, 0); // zero entries (input is ignored)
}

static void WriteBuffers(byte[] story)
{
    story[TextBuffer] = 78;
    story[ParseBuffer] = 8;
}

static void WriteBE16(byte[] data, int offset, int value)
{
    data[offset] = High(value);
    data[offset + 1] = Low(value);
}

static byte High(int value) => (byte)((value >> 8) & 0xFF);
static byte Low(int value) => (byte)(value & 0xFF);

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
    private int Pc => _origin + _bytes.Count;

    public ZCode(int origin) => _origin = origin;

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

    public void OneOp(int op, Operand operand)
    {
        Emit(0x80 | (((int)operand.Kind & 0x03) << 4) | (op & 0x0F));
        EmitOperand(operand);
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

    public void Store(byte variable, int value) =>
        TwoOp(13, Operand.Small(variable), Operand.Large(value));

    public void VarOp(int op, params Operand[] operands)
    {
        Emit(0xE0 | (op & 0x1F));
        Emit(TypeByte(operands));
        foreach (var operand in operands)
            EmitOperand(operand);
    }

    public void Jump(string label)
    {
        Emit(0x8C);
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _jumpPatches.Add(new JumpPatch(patchOffset, label));
    }

    public byte[] ToArray()
    {
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

static class ZString
{
    private const string A0 = "abcdefghijklmnopqrstuvwxyz";
    private const string A1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    private const string A2 = " \r0123456789.,!?_#'\"/\\-:()";

    public static byte[] Pack(string text)
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
                throw new InvalidOperationException($"Character cannot be encoded: U+{(int)ch:X4}");
            zchars.Add(5);
            zchars.Add(6);
            zchars.Add((ch >> 5) & 0x1F);
            zchars.Add(ch & 0x1F);
        }

        while (zchars.Count % 3 != 0)
            zchars.Add(5);

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
