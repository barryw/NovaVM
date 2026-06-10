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

(string outputPath, int version, bool mainReturns) = ParseArgs(args);
Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath)) ?? ".");

var story = new byte[StorySize];
WriteHeader(story, version);
WriteDictionary(story);
WriteBuffers(story);
WriteScratchTables(story);
WritePackedText(story, PackedStrings, "ok");

var code = new ZCode(CodeBase, packedAddressScale: 4, routinesByteOffset: RoutinesByteOffset);
if (mainReturns)
    EmitMainReturnsProgram(code);
else
    EmitSpecProgram(code);
byte[] codeBytes = code.ToArray();
if (CodeBase + codeBytes.Length >= PackedStrings)
    throw new InvalidOperationException($"Z6 spec program overlaps packed strings: {codeBytes.Length} bytes.");
Array.Copy(codeBytes, 0, story, CodeBase, codeBytes.Length);

WriteBE16(story, 0x1C, Checksum(story));
File.WriteAllBytes(outputPath, story);
Console.WriteLine($"Wrote {outputPath} ({story.Length} bytes, code {codeBytes.Length} bytes, version {version})");

static (string Output, int Version, bool MainReturns) ParseArgs(string[] args)
{
    string output = "build/z6-spec.z6";
    int version = 6;
    bool mainReturns = false;
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

    return (output, version, mainReturns);
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova.NovaZ.Z6SpecStory");
    Console.Error.WriteLine("  --output <story.z6>");
    Console.Error.WriteLine("  --version <n>   header version byte only (default 6; 7 for rejection tests)");
    Console.Error.WriteLine("  --main-returns  main routine is a bare rtrue (frame-0 return quit test)");
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
static void EmitMainReturnsProgram(ZCode z)
{
    z.Byte(0);       // locals count
    z.ZeroOp(0);     // rtrue
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
    // picture_data N table ?(label) (EXT:6): no pictures in M1. PicTable is
    // baked with $BEEF words so the write-asserts below prove real writes.
    // N>0 first: queries a specific picture — writes NOTHING, branches false.
    z.ExtOpBranch(6, "picn_taken", branchIf: true, Operand.Small(6), Operand.Large(PicTable));
    z.Jump("picn_ok");
    z.Label("picn_taken");
    z.Fail("picn-branch");
    z.Label("picn_ok");
    z.AssertWordEquals(PicTable, 0, 0xBEEF, "picn-nowrite0");
    z.AssertWordEquals(PicTable, 1, 0xBEEF, "picn-nowrite1");
    // N=0: writes word 0 = picture count (0) and word 1 = release (0), and
    // still branches false — the branch means "any pictures available".
    z.ExtOpBranch(6, "pic0_taken", branchIf: true, Operand.Small(0), Operand.Large(PicTable));
    z.Jump("pic0_ok");
    z.Label("pic0_taken");
    z.Fail("pic-branch");
    z.Label("pic0_ok");
    z.AssertWordEquals(PicTable, 0, 0, "piccount");
    z.AssertWordEquals(PicTable, 1, 0, "picrel");
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
