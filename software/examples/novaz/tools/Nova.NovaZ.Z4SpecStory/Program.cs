// =====================================================================
//  NovaZ — Program.cs (Nova.NovaZ.Z4SpecStory)
//
//  Generates a Version 4 compliance story for NovaZ, exercising V4-specific
//  behaviour that game transcripts do not reliably cover: the Flags1
//  interpreter-capability bits (bold/italic/fixed/timed-input) and timed
//  keyboard input (read_char with a time + interrupt routine).
//
//  Copyright (C) 2026 Barry Walker
//  SPDX-License-Identifier: MIT
// =====================================================================

using System.Text;

// Keep this story V4-only. The interpreter writes the Flags1 bits at boot, so
// the story reads its own header byte $01 and asserts the advertised bits.

const int StorySize = 0x2000;
const int InitialPc = 0x0040;
const int ObjectTable = 0x1000;
const int Globals = 0x1200;
const int Dictionary = 0x1500;
const int StaticMemory = 0x1500;

string outputPath = ParseOutput(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story);
WriteDictionary(story);

var code = new ZCode(InitialPc);
EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (InitialPc + codeBytes.Length >= ObjectTable)
    throw new InvalidOperationException($"Z4 spec program overlaps data tables: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, InitialPc, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes)");

static string ParseOutput(string[] args)
{
    string output = "build/z4-spec.z4";
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
                Console.Error.WriteLine("Nova.NovaZ.Z4SpecStory");
                Console.Error.WriteLine("  --output <story.z4>");
                Environment.Exit(0);
                break;
            default:
                Console.Error.WriteLine($"Unknown or incomplete argument: {args[i]}");
                Environment.Exit(1);
                break;
        }
    }

    return output;
}

static void WriteHeader(byte[] story)
{
    story[0x00] = 4;            // version
    story[0x01] = 0;            // Flags1 - interpreter fills this in at boot
    WriteBE16(story, 0x02, 1);  // release
    WriteBE16(story, 0x04, InitialPc); // high memory mark (code lives here)
    WriteBE16(story, 0x06, InitialPc);
    WriteBE16(story, 0x08, Dictionary);
    WriteBE16(story, 0x0A, ObjectTable);
    WriteBE16(story, 0x0C, Globals);
    WriteBE16(story, 0x0E, StaticMemory);
    WriteBE16(story, 0x10, 0);
    Encoding.ASCII.GetBytes("Z4SPEC").CopyTo(story, 0x12);
    WriteBE16(story, 0x18, 0);              // abbreviations table (unused)
    WriteBE16(story, 0x1A, StorySize / 4);  // file length / 4 for V4
}

static void WriteDictionary(byte[] story)
{
    story[Dictionary] = 0;          // no input-code separators
    story[Dictionary + 1] = 6;      // entry length
    WriteBE16(story, Dictionary + 2, 0); // entry count
}

static void EmitSpecProgram(ZCode z)
{
    z.Print("z4 spec start");
    z.NewLine();

    // --- Flags1 interpreter capability bits (header byte $01) ---
    // Variable 0x10 = first global, used as a scratch holding Flags1.
    z.TwoOpStore(16, Operand.Large(0), Operand.Small(0x01), 0x10); // loadb 0,1 -> G0

    AssertBitSet(z, 0x04, "bold");     // bit 2: boldface available
    AssertBitClear(z, 0x08, "italic"); // bit 3: italic NOT available (no glyphs)
    AssertBitSet(z, 0x10, "fixed");    // bit 4: fixed-pitch available
    AssertBitSet(z, 0x80, "timed");    // bit 7: timed input available

    z.Print("z4 caps ok");
    z.NewLine();

    // --- Timed keyboard input (read_char with a time + interrupt routine) ---
    // No key is ever pressed. The interrupt routine fires every tenth of a
    // second; it counts its calls in global 0x13 and returns true on the third,
    // which aborts read_char and yields a result of 0. This proves both that
    // the routine is invoked and that a true return cancels input.
    z.Store(0x13, 0);                          // counter = 0
    z.ReadCharTimed(0x14, time: 1, "timer_routine"); // result -> G(0x14)
    z.AssertVarEquals(0x14, 0, "timed-abort"); // input was cancelled
    z.AssertVarEquals(0x13, 3, "timed-count"); // routine fired exactly 3x
    z.Print("z4 timed ok");
    z.NewLine();

    z.Print("z4 spec ok");
    z.NewLine();
    z.Quit();

    // Interrupt routine: count calls, abort on the third.
    z.Align4();
    z.Label("timer_routine");
    z.Byte(0);                                       // 0 locals
    z.OneOp(5, Operand.Small(0x13));                 // inc G(0x13)
    z.TwoOpBranch(1, Operand.Var(0x13), Operand.Small(3), "timer_abort"); // je counter,3
    z.RFalse();                                      // continue input
    z.Label("timer_abort");
    z.RTrue();                                       // cancel input
}

// Assert that (Flags1 & mask) == mask, i.e. the capability bit is advertised.
static void AssertBitSet(ZCode z, int mask, string name)
{
    string ok = $"ok_set_{name}";
    z.TwoOpStore(9, Operand.Var(0x10), Operand.Small(mask), 0x11); // and G0,mask -> G1
    z.TwoOpBranch(1, Operand.Var(0x11), Operand.Small(mask), ok);  // je G1,mask ?ok
    z.Fail(name);
    z.Label(ok);
}

// Assert that (Flags1 & mask) == 0, i.e. the capability bit is not advertised.
static void AssertBitClear(ZCode z, int mask, string name)
{
    string ok = $"ok_clear_{name}";
    z.TwoOpStore(9, Operand.Var(0x10), Operand.Small(mask), 0x11); // and G0,mask -> G1
    z.OneOpBranch(0, Operand.Var(0x11), ok);                        // jz G1 ?ok
    z.Fail(name);
    z.Label(ok);
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
    private readonly List<byte> _bytes = [];
    private readonly Dictionary<string, int> _labels = new(StringComparer.Ordinal);
    private readonly List<BranchPatch> _branchPatches = [];
    private readonly List<PackedAddressPatch> _packedAddressPatches = [];
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

    public void Quit() => Emit(0xBA);

    public void RTrue() => Emit(0xB0);

    public void RFalse() => Emit(0xB1);

    public void Byte(int value) => Emit(value);

    public void Align4()
    {
        while ((Pc & 3) != 0)
            Emit(0xB4); // nop padding ahead of a routine (never executed inline)
    }

    public void Store(byte variable, int value) =>
        TwoOp(13, Operand.Small(variable), Operand.Large(value));

    // read_char 1 time routine -> (result). The routine is a packed address.
    public void ReadCharTimed(byte store, int time, string routineLabel)
    {
        Operand[] operands = [Operand.Small(1), Operand.Small(time), Operand.Large(0)];
        Emit(0xE0 | 22); // VAR:22 read_char
        Emit(TypeByte(operands));
        Emit(1);
        Emit(time);
        int patchOffset = _bytes.Count;
        EmitWord(0);
        _packedAddressPatches.Add(new PackedAddressPatch(patchOffset, routineLabel));
        Emit(store);
    }

    // Assert variable == expected, else print a fail marker and quit.
    public void AssertVarEquals(byte variable, int expected, string name)
    {
        string ok = $"ok_eq_{name}";
        TwoOpBranch(1, Operand.Var(variable), Operand.Large(expected), ok);
        Fail(name);
        Label(ok);
    }

    public void OneOp(int op, Operand operand)
    {
        Emit(0x80 | (((int)operand.Kind & 0x03) << 4) | (op & 0x0F));
        EmitOperand(operand);
    }

    public void OneOpBranch(int op, Operand operand, string label, bool branchIf = true)
    {
        OneOp(op, operand);
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

    public void Fail(string name)
    {
        Print($"z4 spec fail {name}");
        NewLine();
        Emit(0xBA);
    }

    public byte[] ToArray()
    {
        foreach (var patch in _packedAddressPatches)
        {
            int target = ResolveLabel(patch.Label);
            if ((target & 3) != 0)
                throw new InvalidOperationException($"V4 routine not 4-aligned: {patch.Label} at ${target:X4}");
            WriteWordAt(patch.Offset, target / 4);
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
readonly record struct PackedAddressPatch(int Offset, string Label);

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
