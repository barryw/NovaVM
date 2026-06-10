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

const int StorySize = 0x1800;

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
const int CursorArray = 0x0540;                    // scratch: get_cursor result words
const int MouseTable = 0x0560;                     // scratch: read_mouse result words (Task 9)
const int StaticMemory = 0x0600;
const int CodeBase = 0x0600;                       // routines region; packed (0x600-0x200)/4
const int PackedStrings = 0x1600;                  // strings region;  packed (0x1600-0x400)/4

(string outputPath, int version, bool mainReturns, string? fixture, string? picturesPakPath) = ParseArgs(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story, version);
WriteDictionary(story);
WriteBuffers(story);
WriteScratchTables(story);
WriteObjectTable(story);
WritePackedText(story, PackedStrings, "ok");

var code = new ZCode(CodeBase, packedAddressScale: 4, routinesByteOffset: RoutinesByteOffset);
if (mainReturns)
    EmitMainReturnsProgram(code);
else if (fixture == "colours")
    EmitColoursProgram(code);
else
    EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (CodeBase + codeBytes.Length >= PackedStrings)
    throw new InvalidOperationException($"Z6 spec program overlaps packed strings: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, CodeBase, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
if (picturesPakPath is not null)
{
    byte[] pak = BuildSyntheticPicturesPak();
    File.WriteAllBytes(picturesPakPath, pak);
    Console.WriteLine($"Wrote {picturesPakPath} ({pak.Length} bytes)");
}
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes, version {version})");

static (string Output, int Version, bool MainReturns, string? Fixture, string? PicturesPak) ParseArgs(string[] args)
{
    string output = "build/z6-spec.z6";
    int version = 6;
    bool mainReturns = false;
    string? fixture = null;
    string? picturesPak = null;
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
            case "--main-returns":
                mainReturns = true;
                break;
            case "--pictures-pak" when value is not null:
                picturesPak = value;
                i++;
                break;
            case "--fixture" when value is not null:
                if (value != "colours")
                    throw new ArgumentException($"Unknown fixture: {value}");
                fixture = value;
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

    return (output, version, mainReturns, fixture, picturesPak);
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova.NovaZ.Z6SpecStory");
    Console.Error.WriteLine("  --output <story.z6>");
    Console.Error.WriteLine("  --version <n>   header version byte only (default 6; 7 for rejection tests)");
    Console.Error.WriteLine("  --main-returns  main routine is a bare rtrue (frame-0 return quit test)");
    Console.Error.WriteLine("  --fixture colours  V6 colour/style fixture (EGA mapping asserts)");
    Console.Error.WriteLine("  --pictures-pak <pics.pak>  also emit the synthetic 2-picture PICS.PAK");
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

static void WriteObjectTable(byte[] story)
{
    // v4+ object table: 63 default-property words (zeros), then 14-byte
    // entries: attrs[6], parent, sibling, child, propTableAddr. Five objects:
    // object 1 parents the chain 2 -> 3 -> 4 -> 5. Removing a MIDDLE child
    // (object 4) exercises zobject_remove's previous-sibling scan path — the
    // path the v4+ get_entry scratch clobber corrupted (Zork Zero wedge).
    int entries = ObjectTable + 126;
    int propsBlob = entries + 5 * 14;           // shared empty prop table
    story[propsBlob] = 0;                       // short-name length 0
    WriteBE16(story, propsBlob + 1, 0);         // prop terminator
    void Obj(int n, int parent, int sibling, int child)
    {
        int e = entries + (n - 1) * 14;
        WriteBE16(story, e + 6, parent);
        WriteBE16(story, e + 8, sibling);
        WriteBE16(story, e + 10, child);
        WriteBE16(story, e + 12, propsBlob);
    }
    Obj(1, 0, 0, 2);
    Obj(2, 1, 3, 0);
    Obj(3, 1, 4, 0);
    Obj(4, 1, 5, 0);
    Obj(5, 1, 0, 0);
}

static void WriteScratchTables(byte[] story)
{
    // User stack (Task 8): word 0 = 4 free slots, value slots zeroed.
    WriteBE16(story, UserStack, 4);

    // picture_data / read_mouse tables (Task 9): poisoned with $BEEF so the
    // zero-asserts prove the handlers actually WROTE, instead of passing on
    // zero-initialized memory.
    WriteBE16(story, PicTable, 0xBEEF);
    WriteBE16(story, PicTable + 2, 0xBEEF);
    for (int i = 0; i < 4; i++)
        WriteBE16(story, MouseTable + i * 2, 0xBEEF);
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

// --main-returns: the smallest possible V6 story — the main routine is a
// bare rtrue. Returning from frame 0 must halt the machine cleanly with
// ZVM_STOP_QUIT (the smoke target asserts the stop reason directly).
// Synthetic PICS.PAK for the spec image (format mirrors BlorbPictures):
// "NZPK" v1, count=3, release=7. Pic 3 = 8x8 px solid magenta ($5), pic 7 =
// 6x10 px solid cyan ($3) — ceil-divide by 4 gives 2x2 and 2(w)x3(h) cells.
// Pic 9 = 4x4 px with transparency (flags $11: bit 0 + transparent index 1):
// each row E,1,1,E — yellow edges around a 2px transparent middle, the
// nibble-keyed blit fixture. Offsets are absolute; two pixels per byte.
static byte[] BuildSyntheticPicturesPak()
{
    const int headerSize = 9, entrySize = 15;
    int dataStart = headerSize + 3 * entrySize;
    int len3 = 4 * 8;      // 8 px = 4 bytes/row, 8 rows
    int len7 = 3 * 10;     // 6 px = 3 bytes/row, 10 rows
    int len9 = 2 * 4;      // 4 px = 2 bytes/row, 4 rows
    var pak = new byte[dataStart + len3 + len7 + len9];
    pak[0] = (byte)'N'; pak[1] = (byte)'Z'; pak[2] = (byte)'P'; pak[3] = (byte)'K';
    pak[4] = 1;
    pak[5] = 3; pak[6] = 0;          // count
    pak[7] = 7; pak[8] = 0;          // release
    WriteEntry(pak, headerSize, z: 3, w: 8, h: 8, offset: dataStart, len: len3);
    WriteEntry(pak, headerSize + entrySize, z: 7, w: 6, h: 10, offset: dataStart + len3, len: len7);
    WriteEntry(pak, headerSize + 2 * entrySize, z: 9, w: 4, h: 4, offset: dataStart + len3 + len7, len: len9, flags: 0x11);
    for (int i = 0; i < len3; i++) pak[dataStart + i] = 0x55;
    for (int i = 0; i < len7; i++) pak[dataStart + len3 + i] = 0x33;
    for (int r = 0; r < 4; r++)
    {
        pak[dataStart + len3 + len7 + r * 2] = 0xE1;     // E 1
        pak[dataStart + len3 + len7 + r * 2 + 1] = 0x1E; // 1 E
    }
    return pak;

    static void WriteEntry(byte[] pak, int at, int z, int w, int h, int offset, int len, byte flags = 0)
    {
        pak[at] = (byte)z; pak[at + 1] = (byte)(z >> 8);
        pak[at + 2] = (byte)w; pak[at + 3] = (byte)(w >> 8);
        pak[at + 4] = (byte)h; pak[at + 5] = (byte)(h >> 8);
        pak[at + 6] = flags;
        pak[at + 7] = (byte)offset; pak[at + 8] = (byte)(offset >> 8);
        pak[at + 9] = (byte)(offset >> 16); pak[at + 10] = (byte)(offset >> 24);
        pak[at + 11] = (byte)len; pak[at + 12] = (byte)(len >> 8);
        pak[at + 13] = (byte)(len >> 16); pak[at + 14] = (byte)(len >> 24);
    }
}

static void EmitMainReturnsProgram(ZCode z)
{
    z.Byte(0);       // locals count
    z.ZeroOp(0);     // rtrue
}

// V6 colour fixture (M3 Task 2): set_colour drives the live text colour
// through the segment's Z->EGA mapping, set_text_style derives bold/reverse
// from the current colour pair, and the V6 boot default is EGA white-on-black
// ($0F). Asserted by test-z6-colours via --expect-text-color (colour RAM
// bytes, palette-mode independent).
static void EmitColoursProgram(ZCode z)
{
    z.Byte(0); // locals count

    z.Print("egadef");                                        // boot default 0F
    z.NewLine();
    z.TwoOp(27, Operand.Small(4), Operand.Small(6));          // green on blue
    z.Print("egagrnblu");                                     // -> 12
    z.NewLine();
    z.TwoOp(27, Operand.Small(9), Operand.Small(2));          // white on black
    z.Print("egawhite");                                      // -> 0F
    z.NewLine();
    z.TwoOp(27, Operand.Small(2), Operand.Small(9));          // Zork Zero's pair
    z.Print("egaparch");                                      // -> F0
    z.NewLine();
    z.VarOp(17, Operand.Small(1));                            // reverse
    z.Print("egarev");                                        // -> 0F
    z.NewLine();
    z.VarOp(17, Operand.Small(0));
    z.Print("egaroman");                                      // -> F0
    z.NewLine();
    z.VarOp(17, Operand.Small(2));                            // bold: fg|8
    z.Print("egabold");                                       // -> F8
    z.NewLine();
    z.VarOp(17, Operand.Small(0));
    z.TwoOp(27, Operand.Small(0), Operand.Small(0));          // 0,0 = keep
    z.Print("egakeep");                                       // -> F0
    z.NewLine();
    z.TwoOpVar(27, Operand.Small(3), Operand.Large(0xFFFF));  // red fg; bg -1 =
    z.Print("egaredkeep");                                    // pixel under cursor
    // (no art here -> 0) -> 04
    z.NewLine();
    z.TwoOp(27, Operand.Small(1), Operand.Small(1));          // 1,1 = defaults
    z.Print("egadflt");                                       // -> 0F
    z.NewLine();
    z.Print("colour fixture done");
    z.NewLine();
    z.ZeroOp(10); // quit
}

static void EmitSpecProgram(ZCode z)
{
    // Main routine: header $06 points here (packed). Locals-count byte first.
    z.Byte(0);

    // --- Task 7: erase_window -1 FIRST — it resets the 8-window table to
    // defaults and clears the whole screen, so everything printed below must
    // survive to the end (and lands at deterministic cells for --expect-at).
    z.VarOp(13, Operand.Large(0xFFFF));

    // M1 core: routine call via R_O (silent on success). The literal/packed
    // print markers moved into the inset playfield at the end of the program
    // (the M2 banner now owns cell 0,0).
    z.Call1S("routine_return_42", 0x11);
    z.AssertVarEquals(0x11, 42, "v6-call");

    // --- Task 6: V6 dispatch routing into the NOVAZ6 segment ---
    // set_window 0 (VAR:11): already current — must be a clean no-op.
    z.VarOp(11, Operand.Small(0));
    // get_wind_prop 1,4 (EXT:19, store): after reset, window 1's y-cursor
    // (prop 4) defaults to 1 unit. Also pins store-byte consumption — a
    // mis-consumed store byte derails the instruction stream.
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(4));
    z.AssertVarEquals(0x12, 1, "windprop-default");
    // picture_data N table ?(label) (EXT:6) against the synthetic PICS.PAK
    // on the spec image (pic 3 = 8x8 px, pic 7 = 6x10 px, release 7; see
    // BuildSyntheticPicturesPak). Dimensions come back in CELLS — pixel dims
    // divided by 4 and rounded UP, so layout reservations never overflow.
    z.ExtOpBranch(6, "pic3_known", branchIf: true, Operand.Small(3), Operand.Large(PicTable));
    z.Fail("pic3-branch");
    z.Label("pic3_known");
    z.AssertWordEquals(PicTable, 0, 2, "pic3-h");
    z.AssertWordEquals(PicTable, 1, 2, "pic3-w");
    z.ExtOpBranch(6, "pic7_known", branchIf: true, Operand.Small(7), Operand.Large(PicTable));
    z.Fail("pic7-branch");
    z.Label("pic7_known");
    z.AssertWordEquals(PicTable, 0, 3, "pic7-h"); // ceil(10/4)
    z.AssertWordEquals(PicTable, 1, 2, "pic7-w"); // ceil(6/4)
    // Unknown picture: writes NOTHING (the $BEEF poison survives), branch
    // false. Re-poison first — pic3/pic7 above overwrote the table.
    z.VarOp(1, Operand.Large(PicTable), Operand.Small(0), Operand.Large(0xBEEF));
    z.VarOp(1, Operand.Large(PicTable), Operand.Small(1), Operand.Large(0xBEEF));
    z.ExtOpBranch(6, "picn_taken", branchIf: true, Operand.Small(99), Operand.Large(PicTable));
    z.Jump("picn_ok");
    z.Label("picn_taken");
    z.Fail("picn-branch");
    z.Label("picn_ok");
    z.AssertWordEquals(PicTable, 0, 0xBEEF, "picn-nowrite0");
    z.AssertWordEquals(PicTable, 1, 0xBEEF, "picn-nowrite1");
    // N=0: word 0 = picture count, word 1 = release; branch TRUE because
    // pictures exist on this image.
    z.ExtOpBranch(6, "pic0_taken", branchIf: true, Operand.Small(0), Operand.Large(PicTable));
    z.Fail("pic0-branch");
    z.Label("pic0_taken");
    z.AssertWordEquals(PicTable, 0, 3, "piccount");
    z.AssertWordEquals(PicTable, 1, 7, "picrel");
    // Flags1 bit 1 (pictures available) must be set once the pak loaded.
    z.TwoOpStore(16, Operand.Large(0), Operand.Small(1), 0x11); // loadb hdr $01
    z.TwoOpStore(9, Operand.Var(0x11), Operand.Small(2), 0x11); // and #2
    z.AssertVarEquals(0x11, 2, "flags1-pics");

    // --- CR interrupt (window props 8/9): each newline printed to the
    // window decrements the countdown; reaching 0 fires the prop-8 routine
    // exactly once, AFTER the newline (the Frotz r393 ordering Zork Zero
    // was tuned against). $FFFF decrements harmlessly forever.
    z.Store(0x60, 0);                                  // fired counter
    z.ExtOp(25, Operand.Small(0), Operand.Small(9), Operand.Small(3));
    z.ExtOpWithPackedRoutine(25, Operand.Small(0), Operand.Small(8), "cr_routine");
    z.NewLine();
    z.NewLine();
    z.AssertVarEquals(0x60, 0, "cr-early");            // 2 of 3: not yet
    z.NewLine();
    z.AssertVarEquals(0x60, 1, "cr-fired");            // exactly once
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(9));
    z.AssertVarEquals(0x12, 0, "cr-count0");
    z.ExtOp(25, Operand.Small(0), Operand.Small(9), Operand.Large(0xFFFF));
    z.NewLine();
    z.AssertVarEquals(0x60, 1, "cr-never");
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(9));
    z.AssertVarEquals(0x12, 0xFFFE, "cr-ffff-dec");
    // buffer_screen 0 (EXT:29, store): stub stores 0.
    z.ExtOpStore(29, 0x12, Operand.Small(0));
    z.AssertVarEquals(0x12, 0, "bufscreen-stub");
    // --- Task 8: user stacks (push_stack EXT:24, pop_stack EXT:21) ---
    // UserStack is baked into the image as [4,0,0,0,0]: word 0 holds the
    // number of FREE slots. push_stack writes the value at word[free] and
    // decrements word 0, so the table fills DOWNWARD: the first push on a
    // fresh 4-slot stack lands at word index 4.
    z.ExtOpBranch(24, "us_push1_ok", branchIf: true, Operand.Small(7), Operand.Large(UserStack));
    z.Fail("us-push1");
    z.Label("us_push1_ok");
    z.ExtOpBranch(24, "us_push2_ok", branchIf: true, Operand.Small(9), Operand.Large(UserStack));
    z.Fail("us-push2");
    z.Label("us_push2_ok");
    z.AssertWordEquals(UserStack, 0, 2, "us-free");        // 4 - 2 pushed
    z.AssertWordEquals(UserStack, 4, 7, "us-val1");        // first push at word[4]
    z.AssertWordEquals(UserStack, 3, 9, "us-val2");        // second at word[3]
    // pop_stack 1: discards one entry by adding 1 to the free count. NO
    // store, NO branch — reaching the next assert proves stream integrity.
    z.ExtOp(21, Operand.Small(1), Operand.Large(UserStack));
    z.AssertWordEquals(UserStack, 0, 3, "us-pop");
    // Exhaust the stack: three more pushes succeed (free 3 -> 0)...
    z.ExtOpBranch(24, "us_push3_ok", branchIf: true, Operand.Small(1), Operand.Large(UserStack));
    z.Fail("us-push3");
    z.Label("us_push3_ok");
    z.ExtOpBranch(24, "us_push4_ok", branchIf: true, Operand.Small(2), Operand.Large(UserStack));
    z.Fail("us-push4");
    z.Label("us_push4_ok");
    z.ExtOpBranch(24, "us_push5_ok", branchIf: true, Operand.Small(3), Operand.Large(UserStack));
    z.Fail("us-push5");
    z.Label("us_push5_ok");
    // ...and a push on the now-FULL stack writes nothing and branches FALSE.
    z.ExtOpBranch(24, "us_full_taken", branchIf: true, Operand.Small(4), Operand.Large(UserStack));
    z.Jump("us_full_ok");
    z.Label("us_full_taken");
    z.Fail("us-full");
    z.Label("us_full_ok");
    z.AssertWordEquals(UserStack, 0, 0, "us-full-free");   // count untouched
    z.AssertWordEquals(UserStack, 1, 3, "us-full-nowrite"); // last value intact
    // --- pull (VAR:9) in V6 is a STORE op (Task 10: Zork Zero derails on it
    // at the very first prompt). With a nonzero user-stack operand it is the
    // exact reverse of push_stack: word 0 (the free count) increments and the
    // value comes back from word[new count]. The stack here is full from the
    // pushes above: [free=0, 3, 2, 1, 7].
    z.VarOpStore(9, 0x13, Operand.Large(UserStack));
    z.AssertVarEquals(0x13, 3, "pull-user1");
    z.AssertWordEquals(UserStack, 0, 1, "pull-user1-free");
    z.VarOpStore(9, 0x13, Operand.Large(UserStack));
    z.AssertVarEquals(0x13, 2, "pull-user2");
    z.AssertWordEquals(UserStack, 0, 2, "pull-user2-free");
    // With the operand absent (types $FF) V6 pull pops the GAME stack — and
    // still consumes the store byte (this is exactly Zork Zero's `E9 FF 00`).
    z.VarOp(8, Operand.Large(0x1234));                     // push 0x1234
    z.VarOpStore(9, 0x13);                                 // pull -> g3
    z.AssertVarEquals(0x13, 0x1234, "pull-game");
    // An operand that is present but ZERO also means the game stack.
    z.VarOp(8, Operand.Large(0x4321));
    z.VarOpStore(9, 0x13, Operand.Small(0));
    z.AssertVarEquals(0x13, 0x4321, "pull-game0");
    // Refill the user stack to FULL (free=0) so the noop-stream assert below
    // keeps checking what it always checked.
    z.ExtOpBranch(24, "pull_refill1", branchIf: true, Operand.Small(2), Operand.Large(UserStack));
    z.Fail("pull-refill1");
    z.Label("pull_refill1");
    z.ExtOpBranch(24, "pull_refill2", branchIf: true, Operand.Small(3), Operand.Large(UserStack));
    z.Fail("pull-refill2");
    z.Label("pull_refill2");
    // The Zork Zero "peek" idiom (routine $EADC there): pull with a VARIABLE
    // operand holding the stack address, value parked in a variable, pushed
    // through the game stack, then push_stack'd back from sp. The user stack
    // must come back bit-identical. Stack here: [free=0, 3, 2, 1, 7].
    z.Store(0x14, UserStack);                              // g4 := stack addr
    z.VarOpStore(9, 0x15, Operand.Var(0x14));              // pull [g4] -> g5
    z.AssertVarEquals(0x15, 3, "peek-val");
    z.VarOp(8, Operand.Var(0x15));                         // push g5
    z.VarOpStore(9, 0x00);                                 // pull -> sp
    z.ExtOpBranch(24, "peek_ok", branchIf: true, Operand.Var(0), Operand.Var(0x14));
    z.Fail("peek-push");
    z.Label("peek_ok");
    z.AssertWordEquals(UserStack, 0, 0, "peek-free");      // count restored
    z.AssertWordEquals(UserStack, 1, 3, "peek-top");       // top intact

    // --- Task 9: honest no-capability graphics/mouse/menu stubs ---
    // read_mouse table (EXT:22): no mouse — y, x, buttons, menu word all 0.
    // MouseTable is baked with $BEEF words to prove the writes happen.
    z.ExtOp(22, Operand.Large(MouseTable));
    z.AssertWordEquals(MouseTable, 0, 0, "mouse0");
    z.AssertWordEquals(MouseTable, 1, 0, "mouse1");
    z.AssertWordEquals(MouseTable, 2, 0, "mouse2");
    z.AssertWordEquals(MouseTable, 3, 0, "mouse3");
    // make_menu N table ?(label) (EXT:27): menus unsupported — branch FALSE.
    z.ExtOpBranch(27, "menu_taken", branchIf: true, Operand.Small(1), Operand.Large(PicTable));
    z.Jump("menu_ok");
    z.Label("menu_taken");
    z.Fail("menu-branch");
    z.Label("menu_ok");
    // Visual/deferred no-ops (no store, no branch): draw_picture,
    // erase_picture, picture_table, scroll_window, mouse_window. Executing
    // them in sequence and reaching the next assert proves each consumed
    // exactly its operands and left the instruction stream intact.
    // (print_form EXT:26 is deliberately NOT exercised until M2 gives it
    // semantics — its operand is a formatted table.)
    z.ExtOp(5, Operand.Small(1));                       // draw_picture 1
    z.ExtOp(7, Operand.Small(1));                       // erase_picture 1
    z.ExtOp(28, Operand.Large(PicTable));               // picture_table
    z.ExtOp(20, Operand.Small(0), Operand.Small(8));    // scroll_window 0,8
    z.ExtOp(23, Operand.Small(1));                      // mouse_window 1
    z.AssertWordEquals(UserStack, 0, 0, "noop-stream"); // stream still aligned

    // --- Task 7: minimal 8-window model ---
    // Window property round-trip: put_wind_prop 1,4,21 then read it back.
    z.ExtOp(25, Operand.Small(1), Operand.Small(4), Operand.Small(21));
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(4));
    z.AssertVarEquals(0x12, 21, "windprop-rt");

    // move_window 2, y=41, x=81 -> props 0/1.
    z.ExtOp(16, Operand.Small(2), Operand.Small(41), Operand.Small(81));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(0));
    z.AssertVarEquals(0x12, 41, "movewin-y");
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(1));
    z.AssertVarEquals(0x12, 81, "movewin-x");

    // window_size 2, y=80, x=120 -> props 2/3.
    z.ExtOp(17, Operand.Small(2), Operand.Small(80), Operand.Small(120));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(2));
    z.AssertVarEquals(0x12, 80, "winsize-y");
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(3));
    z.AssertVarEquals(0x12, 120, "winsize-x");

    // window_style 2 against prop 14: set, or, and-not, xor.
    z.ExtOp(18, Operand.Small(2), Operand.Small(0b1010), Operand.Small(0));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(14));
    z.AssertVarEquals(0x12, 0b1010, "winstyle-set");
    z.ExtOp(18, Operand.Small(2), Operand.Small(0b0001), Operand.Small(1));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(14));
    z.AssertVarEquals(0x12, 0b1011, "winstyle-or");
    z.ExtOp(18, Operand.Small(2), Operand.Small(0b0010), Operand.Small(2));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(14));
    z.AssertVarEquals(0x12, 0b1001, "winstyle-clear");
    z.ExtOp(18, Operand.Small(2), Operand.Small(0b1111), Operand.Small(3));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(14));
    z.AssertVarEquals(0x12, 0b0110, "winstyle-xor");

    // set_margins left=8 right=12 window=2 -> props 6/7.
    z.ExtOp(8, Operand.Small(8), Operand.Small(12), Operand.Small(2));
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(6));
    z.AssertVarEquals(0x12, 8, "margins-l");
    z.ExtOpStore(19, 0x12, Operand.Small(2), Operand.Small(7));
    z.AssertVarEquals(0x12, 12, "margins-r");

    // set_colour fg=4 bg=5 window=3 -> prop 11 records (bg<<8)|fg; M1 does
    // not touch the live VGC palette.
    z.TwoOpVar(27, Operand.Small(4), Operand.Small(5), Operand.Small(3));
    z.ExtOpStore(19, 0x12, Operand.Small(3), Operand.Small(11));
    z.AssertVarEquals(0x12, (5 << 8) | 4, "colour-rec");

    // Units ARE cells: the header advertises an 80x50-unit screen with a
    // 1x1 font (zstory.s), so every coordinate below is a 1-based text
    // cell — exactly what Zork Zero computes and sends (capture finding 1).
    // V6 split_window is a pure table op (Zork Zero never calls it): window
    // 1 y-size = 40, window 0 origin y = 41, y-size = 10.
    z.VarOp(10, Operand.Small(40));
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(2));
    z.AssertVarEquals(0x12, 40, "split-w1size");
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(0));
    z.AssertVarEquals(0x12, 41, "split-w0y");
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(2));
    z.AssertVarEquals(0x12, 10, "split-w0size");

    // Zork Zero's real layout, verbatim (capture boot seqs 15-18): window 0
    // = 45x70 playfield inset at (6,6), window 1 = top 5 rows full width.
    // These REPLACE the split geometry above — windows are vtext regions now.
    z.ExtOp(16, Operand.Small(0), Operand.Small(6), Operand.Small(6));   // move_window 0,6,6
    z.ExtOp(17, Operand.Small(0), Operand.Small(45), Operand.Small(70)); // window_size 0,45,70
    z.ExtOp(16, Operand.Small(1), Operand.Small(1), Operand.Small(1));   // move_window 1,1,1
    z.ExtOp(17, Operand.Small(1), Operand.Small(5), Operand.Small(80));  // window_size 1,5,80
    z.VarOp(11, Operand.Small(0));                          // set_window 0
    z.VarOp(15, Operand.Small(1), Operand.Small(1));        // set_cursor 1,1
    z.Print("INSET");                                       // -> abs cell (col 5, row 5)

    // Banner separation: window 1 renders at the real screen top-left.
    z.VarOp(11, Operand.Small(1));                          // set_window 1
    // set_cursor 0,1 — Zork Zero's verbatim pre-read banner op: row/col 0
    // clamps to 1 (the stored prop must read back as 1).
    z.VarOp(15, Operand.Small(0), Operand.Small(1));
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(4));
    z.AssertVarEquals(0x12, 1, "setcursor-y0-clamp");
    z.VarOp(15, Operand.Small(1), Operand.Small(0));        // col 0 clamps too
    z.ExtOpStore(19, 0x12, Operand.Small(1), Operand.Small(5));
    z.AssertVarEquals(0x12, 1, "setcursor-x0-clamp");
    z.Print("BANNER");                                      // -> abs cell (0,0)

    // Back to window 0: printing continues at the inset cursor (right after
    // INSET, same row), NOT at the banner cursor -> "INSETW0" at (5,5).
    z.VarOp(11, Operand.Small(0));                          // set_window 0
    z.Print("W0");
    z.NewLine();

    // Cursor readback: set_cursor with no printing in between must read
    // back exactly (cells, relative to the window; 13,17 is inside 45x70).
    z.VarOp(15, Operand.Small(13), Operand.Small(17));      // set_cursor 13,17
    z.VarOp(16, Operand.Large(CursorArray));                // get_cursor array
    z.AssertWordEquals(CursorArray, 0, 13, "getcursor-y");
    z.AssertWordEquals(CursorArray, 1, 17, "getcursor-x");

    // set_cursor on a NON-current window only updates the table.
    z.VarOp(15, Operand.Small(29), Operand.Small(33), Operand.Small(4));
    z.ExtOpStore(19, 0x12, Operand.Small(4), Operand.Small(4));
    z.AssertVarEquals(0x12, 29, "setcursor-other-y");
    z.ExtOpStore(19, 0x12, Operand.Small(4), Operand.Small(5));
    z.AssertVarEquals(0x12, 33, "setcursor-other-x");

    // M1 print markers (literal + packed string via S_O), now landing inside
    // the inset playfield: set_cursor 3,1 -> abs cell (col 5, row 7).
    z.VarOp(15, Operand.Small(3), Operand.Small(1));
    z.Print("z6 m1 ");
    z.OneOp(13, Operand.Large((PackedStrings - StringsByteOffset) / 4)); // print_paddr "ok"
    z.NewLine();

    // --- M2 Task 3: in-window scrolling kills stale text ---
    // Sentinel OUTSIDE window 0's rect: banner cell abs (4,4) — the last
    // banner row, directly above the playfield — must survive the playfield
    // scrolling below, rect-clipped.
    z.VarOp(11, Operand.Small(1));                          // set_window 1
    z.VarOp(15, Operand.Small(5), Operand.Small(5));        // set_cursor 5,5 -> abs (4,4)
    z.Print("SENT");
    z.VarOp(11, Operand.Small(0));                          // set_window 0; cursor rel (3,0)

    // Fill rows 3..43 with distinctive lines, then a LONG line on the bottom
    // row (rel 44). Each following newline scrolls the 45-row window once:
    //   scroll 1: INSETW0 (rel 0) scrolls off; the vacated bottom row must
    //             come back BLANK — the short "Z2" printed there leaves no
    //             tail of LONGTAILXYZ (which scrolled through that exact row).
    //   scroll 2: "ZLAST ok" on the fresh bottom row (the packed-string "ok"
    //             via S_O keeps its on-screen proof here — the boot marker
    //             scrolls off).
    //   scroll 3: leaves the bottom row bare for the prompt (the harness's
    //             ready detection needs '>' alone on the cursor row). Final
    //             rows: rel 0 = F03, rel 41 = LONG, rel 42 = Z2, rel 43 =
    //             "ZLAST ok", rel 44 (abs row 49) = the bare prompt.
    for (int row = 3; row <= 43; row++)
    {
        z.Print($"F{row:00}");
        z.NewLine();
    }
    z.Print("LONGTAILXYZ");                                 // rel row 44 (bottom)
    z.NewLine();                                            // scroll #1
    z.Print("Z2");                                          // on the freshly blanked bottom row
    z.NewLine();                                            // scroll #2
    z.Print("ZLAST ");
    z.OneOp(13, Operand.Large((PackedStrings - StringsByteOffset) / 4)); // print_paddr "ok"
    z.NewLine();                                            // scroll #3

    // --- M2 Task 4: scroll_window (EXT:20) + interrupt-prop round-trip ---
    // Newline-interrupt props 8/9 must round-trip EXACTLY (Zork Zero arms
    // routine $384B / countdown -1 on every refresh and reads prop 9 back —
    // capture finding 5). They are stored, never fired.
    z.ExtOp(25, Operand.Small(0), Operand.Small(9), Operand.Large(0xFFFF));
    z.ExtOp(25, Operand.Small(0), Operand.Small(8), Operand.Large(0x384B));
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(9));
    z.AssertVarEquals(0x12, 0xFFFF, "intprop9-rt");
    z.ExtOpStore(19, 0x12, Operand.Small(0), Operand.Small(8));
    z.AssertVarEquals(0x12, 0x384B, "intprop8-rt");

    // scroll_window on a NON-current window: build a 5x5 window 3 in the
    // free right margin (abs col 75, rows 9-13), fill all five rows, return
    // to window 0, then scroll window 3 by one cell while window 0 is live.
    // AA scrolls off, EE lands on row 12, the vacated bottom row (abs 13)
    // comes back blank. Amount 0 must be a clean no-op.
    z.ExtOp(16, Operand.Small(3), Operand.Small(10), Operand.Small(76)); // move_window 3,10,76
    z.ExtOp(17, Operand.Small(3), Operand.Small(5), Operand.Small(5));   // window_size 3,5,5
    z.VarOp(11, Operand.Small(3));                          // set_window 3
    z.VarOp(15, Operand.Small(1), Operand.Small(1));
    z.Print("AA");
    z.VarOp(15, Operand.Small(2), Operand.Small(1));
    z.Print("BB");
    z.VarOp(15, Operand.Small(3), Operand.Small(1));
    z.Print("CC");
    z.VarOp(15, Operand.Small(4), Operand.Small(1));
    z.Print("DD");
    z.VarOp(15, Operand.Small(5), Operand.Small(1));
    z.Print("EE");
    z.VarOp(11, Operand.Small(0));                          // back to window 0
    z.ExtOp(20, Operand.Small(3), Operand.Small(1));        // scroll_window 3,1 (non-current)
    z.ExtOp(20, Operand.Small(3), Operand.Small(0));        // amount 0: no-op

    // scroll_window on the CURRENT window: the game's page-reset (capture
    // finding 4). Amount is in CELLS (units = cells). Content moves up 3
    // rows, 3 blank rows open at the bottom, and the cursor does NOT move
    // (it sits at rel 44,0 from scroll #3 above — Zork Zero always follows
    // with set_cursor): the prompt below must still land on the bare bottom
    // row. Final window 0 rows: rel 0 = F06, rel 38 = LONG, rel 39 = Z2,
    // rel 40 = "ZLAST ok", rel 41-44 blank (41 was blank already; 42-44
    // vacated and blanked by the scroll).
    z.ExtOp(20, Operand.Small(0), Operand.Small(3));        // scroll_window 0,3

    // Object-tree integrity across remove_obj of a MIDDLE child (the
    // zobject_remove previous-sibling scan path). Baked tree: 1 parents
    // 2 -> 3 -> 4 -> 5. remove_obj(4) must rewrite sibling(3) := 5 and leave
    // every other link alone; the historic v4+ scratch clobber wrote object
    // 2*(3-1)=4's sibling instead, leaving 3 -> 4 stale (circular-walk wedge).
    z.OneOp(9, Operand.Small(4));                              // remove_obj 4
    z.OneOpStoreBranch(1, Operand.Small(3), 0x12, "objrm-sib3", branchIf: true);
    z.Label("objrm-sib3");
    z.AssertVarEquals(0x12, 5, "objrm-prev");                  // sibling(3) == 5
    z.OneOpStoreBranch(1, Operand.Small(2), 0x12, "objrm-sib2", branchIf: true);
    z.Label("objrm-sib2");
    z.AssertVarEquals(0x12, 3, "objrm-sib2v");                 // sibling(2) == 3
    z.OneOpStoreBranch(2, Operand.Small(1), 0x12, "objrm-ch1", branchIf: true);
    z.Label("objrm-ch1");
    z.AssertVarEquals(0x12, 2, "objrm-child");                 // child(1) == 2
    z.OneOpStore(3, Operand.Small(4), 0x12);                   // get_parent 4
    z.AssertVarEquals(0x12, 0, "objrm-parent");                // detached
    // insert_obj round-trip: 4 back under 1 as new first child.
    z.TwoOp(14, Operand.Small(4), Operand.Small(1));           // insert_obj 4,1
    z.OneOpStoreBranch(2, Operand.Small(1), 0x12, "objins-ch", branchIf: true);
    z.Label("objins-ch");
    z.AssertVarEquals(0x12, 4, "objins-child");                // child(1) == 4
    z.OneOpStoreBranch(1, Operand.Small(4), 0x12, "objins-sib", branchIf: true);
    z.Label("objins-sib");
    z.AssertVarEquals(0x12, 2, "objins-sib4");                 // sibling(4) == 2

    // --- draw_picture / erase_picture blits. This block runs at the TAIL:
    // newline scrolls now move the gfx layer with the text (MCGA semantics),
    // so art drawn early would scroll away with the program's output. The
    // px targets are unchanged; coords are window-0-relative to the final
    // inset origin (6,6): rel = abs_cell - 4.
    // Pic 3 at cells (20,10) -> px x 40-47, y 80-87, solid magenta ($5).
    z.ExtOp(5, Operand.Small(3), Operand.Small(16), Operand.Small(6));
    // Pic 9 (4x4, E 1 1 E rows, transparent index 1) over its top-left
    // corner: opaque yellow edges land, the transparent middle keeps the
    // magenta underneath — the nibble-keyed unpack fixture.
    z.ExtOp(5, Operand.Small(9), Operand.Small(16), Operand.Small(6));
    // Pic 7 (6x10 cyan) at cells (30,40) -> px x 160-165, y 120-129, then
    // erased: the rect refills with the window background (black).
    z.ExtOp(5, Operand.Small(7), Operand.Small(26), Operand.Small(36));
    z.ExtOp(7, Operand.Small(7), Operand.Small(26), Operand.Small(36));
    // Clipping: pic 3 at x cell 79 -> px 316-323 clips at 320 without
    // derailing; fully off-screen draw (y cell 59) is a clean no-op.
    z.ExtOp(5, Operand.Small(3), Operand.Small(1), Operand.Small(75));
    z.ExtOp(5, Operand.Small(3), Operand.Small(56), Operand.Small(1));

    // MCGA-faithful compositing: drawing a picture overwrites the text
    // pixels under it, so the blit blanks the covered cells (space, bg 0 =
    // the global background -> the art shows through in mode 2). This runs
    // LAST: earlier program output scrolls the full-screen window. The
    // markers live in the left gutter (col 1, rows 30/36) outside the final
    // 45x70 inset, which later scrolling never touches. PP's cell must go
    // blank under the draw; the uncovered control QQ must survive (it
    // proves window-4 printing landed at all; window 2 is unusable here —
    // the earlier set_margins fixture left it with margins 8+12).
    z.ExtOp(16, Operand.Small(4), Operand.Small(31), Operand.Small(2)); // move_window 4 -> (31,2)
    z.ExtOp(17, Operand.Small(4), Operand.Small(10), Operand.Small(4)); // window_size 4: 10x4
    z.VarOp(11, Operand.Small(4));                                      // set_window 4
    z.VarOp(15, Operand.Small(1), Operand.Small(1));                    // cursor home
    z.Print("PP");
    z.VarOp(15, Operand.Small(7), Operand.Small(1));                    // cursor row 7 -> abs row 36
    z.Print("QQ");
    // draw from window 4 itself (coords are window-relative; window 0 is
    // the inset here): rel (1,1) = window 4's origin = cell (1,30)
    z.ExtOp(5, Operand.Small(3), Operand.Small(1), Operand.Small(1));
    // Art scrolls WITH the text (the MCGA framebuffer is one surface):
    // scroll_window 4 by one cell row moves QQ up to row 35 AND shifts the
    // picture's gfx rows up 4px — its old bottom rows go empty.
    z.ExtOp(20, Operand.Small(4), Operand.Small(1));
    z.VarOp(11, Operand.Small(0));                                      // back to window 0


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

    // CR-interrupt routine: bump the fired counter (global var $60), no
    // printing (a printing CR routine would re-enter the newline path).
    z.Align(4);
    z.Label("cr_routine");
    z.Byte(0);
    z.OneOp(5, Operand.Small(0x60));   // inc
    z.ZeroOp(0);                       // rtrue
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

    // 2OP opcode in VAR form (up to 4 operands), e.g. V6 "set_colour fg bg window".
    public void TwoOpVar(int op, params Operand[] operands)
    {
        Emit(0xC0 | (op & 0x1F));
        Emit(TypeByte(operands));
        foreach (var operand in operands)
            EmitOperand(operand);
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

    /// <summary>EXT op whose LAST operand is the packed address of a routine.</summary>
    public void ExtOpWithPackedRoutine(int op, Operand a, Operand b, string routineLabel)
    {
        Emit(0xBE);
        Emit(op & 0xFF);
        Emit(TypeByte([a, b, Operand.Large(0)]));
        EmitOperand(a);
        EmitOperand(b);
        EmitPackedAddressPatch(routineLabel);
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
