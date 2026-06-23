using System.Text;

const int StorySize = 0x1000;
const int InitialPc = 0x0040;
const int Dictionary = 0x0E00;
const int ObjectTable = 0x0E40;
const int Globals = 0x0EC0;
const int StaticMemory = 0x1000;

string outputPath = ParseOutput(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story);

var code = new ZCode();
EmitStyleProgram(code);
byte[] codeBytes = code.ToArray();
if (InitialPc + codeBytes.Length >= Dictionary)
    throw new InvalidOperationException($"Z4 style fixture overlaps data tables: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, InitialPc, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes)");

static string ParseOutput(string[] args)
{
    string output = "build/z4-styles.z4";
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
    Console.Error.WriteLine("Nova.NovaZ.Z4StyleStory");
    Console.Error.WriteLine("  --output <story.z4>");
}

static void WriteHeader(byte[] story)
{
    story[0x00] = 4;
    story[0x01] = 0;
    WriteBE16(story, 0x02, 1);
    WriteBE16(story, 0x04, InitialPc);
    WriteBE16(story, 0x06, InitialPc);
    WriteBE16(story, 0x08, Dictionary);
    WriteBE16(story, 0x0A, ObjectTable);
    WriteBE16(story, 0x0C, Globals);
    WriteBE16(story, 0x0E, StaticMemory);
    WriteBE16(story, 0x10, 0);
    Encoding.ASCII.GetBytes("STYL01").CopyTo(story, 0x12);
    WriteBE16(story, 0x18, 0);
    WriteBE16(story, 0x1A, StorySize / 4);
}

static void EmitStyleProgram(ZCode z)
{
    z.Print("z4 style fixture");
    z.NewLine();
    z.Print("normal text uses color 12");
    z.NewLine();
    z.SetTextStyle(2);
    z.Print("bold uses the bold attr");
    z.NewLine();
    z.Print("bold phrase survives reset");
    z.SetTextStyle(0);
    z.NewLine();
    z.Print("normal text again");
    z.NewLine();
    z.SetTextStyle(1);
    z.Print("reverse uses the reverse attr");
    z.NewLine();
    z.SetTextStyle(3);
    z.Print("bold reverse uses both attrs");
    z.NewLine();
    z.SetTextStyle(0);
    z.SetTextStyle(4);
    z.Print("italic request is ignored");
    z.NewLine();
    z.SetTextStyle(0);
    z.SetTextStyle(8);
    z.Print("fixed request is ignored");
    z.NewLine();
    // Styles are cumulative until style 0 resets them: bold then reverse,
    // applied in separate calls, must combine to bold-reverse (attr $06).
    z.SetTextStyle(0);
    z.SetTextStyle(2);
    z.SetTextStyle(1);
    z.Print("stacked styles use both attrs");
    z.NewLine();
    z.SetTextStyle(0);
    z.Print("style fixture done");
    z.NewLine();
    z.ReadChar();   // hold the styled screen until a key press (Arty HDMI demo)
    z.Quit();
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

sealed class ZCode
{
    private readonly List<byte> _bytes = [];

    public void Print(string text)
    {
        Emit(0xB2);
        EmitRange(ZString.Pack(text));
    }

    public void NewLine() => Emit(0xBB);

    public void SetTextStyle(int style)
    {
        Emit(0xF1);
        Emit(0x7F);
        Emit(style);
    }

    public void Quit() => Emit(0xBA);

    // read_char 1 -> (sp): wait for a keypress. Holds the styled screen so the
    // Arty HDMI demo persists; the headless smoke test matches the stable screen.
    public void ReadChar()
    {
        Emit(0xF6);   // read_char (VAR form, opcode 246)
        Emit(0x7F);   // operand types: one small constant, rest omitted
        Emit(0x01);   // operand: 1 (read from the keyboard)
        Emit(0x00);   // store result -> stack
    }

    public byte[] ToArray() => [.. _bytes];

    private void EmitRange(IEnumerable<byte> bytes)
    {
        foreach (byte b in bytes)
            Emit(b);
    }

    private void Emit(int value) => _bytes.Add((byte)(value & 0xFF));
}

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
                throw new InvalidOperationException($"Character cannot be encoded in ZSCII escape form: U+{(int)ch:X4}");
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
