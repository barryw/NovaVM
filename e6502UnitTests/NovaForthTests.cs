using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NovaForthTests
{
    private const ushort ReservedStart = 0x0300;
    private const ushort ReservedEnd = 0x0900;
    private const byte Sentinel = 0x5A;
    private const byte CtrlK = 0x0B;
    private const byte KeyEsc = 0x1B;
    private const byte SyntaxKeyword = 0x03;
    private const byte SyntaxNumber = 0x07;
    private const byte SyntaxComment = 0x0C;

    [TestMethod]
    public void BootShowsBannerAndPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("Nova Forth v0.1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("BYTES FREE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("OK>", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ArithmeticAndStackWordsWork()
    {
        string screen = RunForthLines("1 2 + .", "5 DUP * .", "1 2 3 ROT .S");

        Assert.IsTrue(screen.Contains("3 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("25 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<3> 2 3 1", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ColonDefinitionsExecute()
    {
        string screen = RunForthLines(": SQUARE DUP * ;", "5 SQUARE .");

        Assert.IsTrue(screen.Contains("25 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ConstantsVariablesHexAndSignedNumbersWork()
    {
        string screen = RunForthLines(
            "42 CONSTANT ANSWER",
            "ANSWER .",
            "VARIABLE X",
            "7 X !",
            "X @ .",
            "$10 -1 + .");

        Assert.IsTrue(screen.Contains("42 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("7 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("15 ", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void BaseAndDictionaryAllocationWordsWork()
    {
        string screen = RunForthLines(
            "HEX FF DECIMAL .",
            "HERE 4660 , @ HEX . DECIMAL",
            "HERE 65 C, C@ .",
            "1 CELLS . 7 CHAR+ . BASE @ .");

        Assert.IsTrue(screen.Contains("255 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("1234 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("65 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("8 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("10 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ControlFlowWordsCompileBranches()
    {
        string screen = RunForthLines(
            ": CHOOSE IF 100 11 + ELSE 200 22 + THEN ;",
            "0 CHOOSE .",
            "1 CHOOSE .",
            ": COUNTDOWN BEGIN DUP . 1- DUP 0= UNTIL DROP ;",
            "3 COUNTDOWN",
            ": DOWNW BEGIN DUP WHILE DUP . 1- REPEAT DROP ;",
            "3 DOWNW");

        Assert.IsTrue(screen.Contains("222 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("111 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("3 2 1", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ExecutionCommentsAndLiteralWordsWork()
    {
        string screen = RunForthLines(
            "5 ' DUP EXECUTE + .",
            ": ADDSELF ['] DUP EXECUTE + ;",
            "6 ADDSELF .",
            "3 ( ignored tokens ) 4 + . \\ 999 .",
            ": FIVE [ 2 3 + ] LITERAL ;",
            "FIVE .");

        Assert.IsTrue(screen.Contains("10 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("12 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("7 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("5 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ReturnStackWordsAreBalancedInsideDefinitions()
    {
        string screen = RunForthLines(": RTEST >R R@ R> + ;", "5 RTEST .");

        Assert.IsTrue(screen.Contains("10 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void SourceInputAndLookupPrimitivesWork()
    {
        string screen = RunForthLines(
            "SOURCE SWAP DROP .",
            ">IN @ .",
            "PARSE-NAME ABC SWAP DROP .",
            "BL WORD DUP DUP FIND SWAP DROP . DROP",
            "BL WORD ; DUP FIND SWAP DROP . DROP");

        Assert.IsTrue(screen.Contains("18 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("6 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("3 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("1 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void EvaluateInterpretsTemporarySourceAndRestoresCallerInput()
    {
        string screen = RunForthLines(
            "HERE 50 C, 32 C, 51 C, 32 C, 43 C, 32 C, 46 C, 7 EVALUATE",
            "1 1 + .");

        Assert.IsTrue(screen.Contains("5 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void AcceptReadsRawInputIntoCallerBuffer()
    {
        string screen = RunForthLineWithRawInput("HERE 8 ACCEPT .", "OK");

        Assert.IsTrue(screen.Contains("2 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ComparisonAndBitPrimitivesWork()
    {
        string screen = RunForthLines(
            "1 2 < . 2 1 > . 1 1 = . 1 2 = .",
            "6 3 AND . 6 3 OR . 6 3 XOR . 0 INVERT .",
            "$FFFF 1 U< . 1 $FFFF U< .");

        Assert.IsTrue(screen.Contains("-1 -1 -1 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 7 5 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 -1", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ShiftAndDivisionPrimitivesWork()
    {
        string screen = RunForthLines(
            "17 5 /MOD SWAP . .",
            "1 4 LSHIFT .",
            "$8000 15 RSHIFT .",
            "1 16 LSHIFT .");

        Assert.IsTrue(screen.Contains("2 3", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("16 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("1 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void FillAndMovePrimitivesWork()
    {
        string screen = RunForthLines(
            "HERE 4 ALLOT DUP 4 90 FILL DUP C@ . DUP 3 + C@ . DROP",
            "HERE 65 C, 66 C, 67 C, 68 C, DUP DUP CHAR+ 3 MOVE DUP C@ . DUP CHAR+ C@ . DUP 2 + C@ . DUP 3 + C@ . DROP");

        Assert.IsTrue(screen.Contains("90 90", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("65 65 66 67", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void DoubleStackAndPairMemoryPrimitivesWork()
    {
        string screen = RunForthLines(
            "1 2 3 4 2OVER .S 2DROP 2DROP 2DROP",
            "1 2 3 4 2SWAP .S 2DROP 2DROP",
            "HERE DUP >R 123 456 ROT 2! R> 2@ SWAP . .");

        Assert.IsTrue(screen.Contains("<6> 1 2 3 4 1 2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<4> 3 4 1 2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("123 456", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void CountPlusStoreAlignAndUnsignedDisplayPrimitivesWork()
    {
        string screen = RunForthLines(
            "VARIABLE Y 10 Y ! 5 Y +! Y @ .",
            "HERE 3 C, 65 C, 66 C, 67 C, COUNT TYPE",
            "HERE ALIGNED HERE = .",
            "$FFFF U.",
            "-1 S>D . . 1 S>D . .");

        Assert.IsTrue(screen.Contains("15 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("ABC", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("65535 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 1", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void DoubleArithmeticPrimitivesWork()
    {
        string screen = RunForthLines(
            "300 7 UM* SWAP . .",
            "$FFFF 2 UM* SWAP U. U.",
            "300 -7 M* SWAP . .",
            "-8 3 /MOD SWAP . .",
            "1000 0 7 UM/MOD SWAP . .",
            "0 1 2 UM/MOD SWAP U. U.",
            "-7 S>D 3 SM/REM SWAP . .",
            "-7 S>D 3 FM/MOD SWAP . .",
            "7 -2 3 */MOD SWAP . .",
            "7 -2 3 */ .");

        Assert.IsTrue(screen.Contains("2100 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("65534 1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-2100 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-2 -2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("6 142", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 32768", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 -2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 -3", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-2 -4", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-4 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("DIVIDE BY ZERO", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void PicturedNumericPrimitivesWork()
    {
        string screen = RunForthLines(
            "12345 0 <# #S #> TYPE",
            "42 0 <# #S -1 SIGN #> TYPE",
            "42 0 <# #S 33 HOLD #> TYPE",
            "HERE DUP >R 49 C, 50 C, 51 C, 88 C, 0 0 R> 4 >NUMBER . DROP SWAP . .");

        Assert.IsTrue(screen.Contains("12345", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-42", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("!42", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("1 123 0", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void PicturedNumericConvertsAcrossBases()
    {
        string screen = RunForthLines(
            "2 0 2 BASE ! <# #S #> TYPE SPACE DECIMAL",
            "36 0 36 BASE ! <# #S #> TYPE SPACE DECIMAL",
            ": S=",
            " >R SWAP R@ = IF",
            " R> ?DUP IF",
            " 0 DO",
            " OVER C@ OVER C@ - IF 2DROP 0 UNLOOP EXIT THEN",
            " SWAP CHAR+ SWAP CHAR+",
            " LOOP",
            " THEN",
            " 2DROP -1",
            " ELSE",
            " R> DROP 2DROP 0",
            " THEN ;",
            "36 CONSTANT MAX-BASE",
            ": COUNT-BITS 0 0 INVERT BEGIN DUP WHILE >R 1+ R> 2* REPEAT DROP ;",
            "COUNT-BITS 2* CONSTANT #BITS-UD",
            ": GP5",
            " BASE @ -1",
            " MAX-BASE 1+ 2 DO",
            " I BASE !",
            " I 0 <# #S #> S\" 10\" S= AND",
            " LOOP",
            " SWAP BASE ! ;",
            "HEX",
            "GP5 DECIMAL .");

        Assert.IsTrue(screen.Contains("10 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void IncludedPicturedNumericLoopPreservesStack()
    {
        string path = Hd0Path("gp5-include.4th");
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllLines(path, new[]
        {
            ": S=",
            " >R SWAP R@ = IF",
            " R> ?DUP IF",
            " 0 DO",
            " OVER C@ OVER C@ - IF 2DROP 0 UNLOOP EXIT THEN",
            " SWAP CHAR+ SWAP CHAR+",
            " LOOP",
            " THEN",
            " 2DROP -1",
            " ELSE",
            " R> DROP 2DROP 0",
            " THEN ;",
            "36 CONSTANT MAX-BASE",
            ": COUNT-BITS",
            " 0 0 INVERT BEGIN DUP WHILE >R 1+ R> 2* REPEAT DROP ;",
            "COUNT-BITS 2* CONSTANT #BITS-UD",
            ": GP5",
            " BASE @ -1",
            " MAX-BASE 1+ 2 DO",
            " I BASE !",
            " I 0 <# #S #> S\" 10\" S= AND",
            " LOOP",
            " SWAP BASE ! ;"
        });

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
            QueueLine(editor, "INCLUDE gp5-include.4th");
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);
            QueueLine(editor, "HEX GP5 DECIMAL . S\" GP5-DONE\" TYPE");
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);

            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsTrue(screen.Contains("-1 GP5-DONE", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void EditCreatesAndSavesForthSourceFile()
    {
        string path = Hd0Path("forth-edit-test.4th");
        File.Delete(path);

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
            QueueLine(editor, "EDIT forth-edit-test.4th");
            RunUntilScreenContains(cpu, bus, "forth-edit-test.4th", 80_000_000);
            QueueLine(editor, ": EDITED 777 ;");
            QueueSave(editor);
            QueueAltX(editor);
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);

            Assert.IsTrue(File.Exists(path), "EDIT should create missing .4th files when the user saves.");
            StringAssert.Contains(File.ReadAllText(path), ": EDITED 777 ;");

            QueueLine(editor, "INCLUDE forth-edit-test.4th");
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);
            QueueLine(editor, "EDITED . S\" EDIT-DONE\" TYPE");
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);

            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsTrue(screen.Contains("777 EDIT-DONE", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("EDIT FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void EditUsesForthSyntaxHighlighting()
    {
        string path = Hd0Path("forth-highlight-test.4th");
        File.WriteAllText(path, ": HILITE DUP 123 ; \\ comment\n");

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
            QueueLine(editor, "EDIT forth-highlight-test.4th");
            RunUntilScreenContains(cpu, bus, "forth-highlight-test.4th", 80_000_000);
            RunSteps(cpu, bus, 5_000_000);

            Assert.AreEqual((byte)':', bus.Vgc.GetScreenChar(0, 1), SnapshotScreen(bus.Vgc));
            Assert.AreEqual(SyntaxKeyword, bus.Vgc.GetScreenColor(0, 1),
                "The Forth profile should color definition punctuation so definitions are visually scannable.");
            Assert.AreEqual(SyntaxKeyword, bus.Vgc.GetScreenColor(9, 1),
                "The Forth profile should color known core words from its token table.");
            Assert.AreEqual(SyntaxNumber, bus.Vgc.GetScreenColor(13, 1),
                "The Forth profile should color numeric literals.");
            Assert.AreEqual(SyntaxKeyword, bus.Vgc.GetScreenColor(17, 1),
                "The Forth profile should color semicolon definition terminators.");
            Assert.AreEqual(SyntaxComment, bus.Vgc.GetScreenColor(19, 1),
                "The Forth profile should color backslash comments through end-of-line.");

            QueueAltX(editor);
            RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void EditUsesForthAutoIndent()
    {
        string path = Hd0Path("forth-indent-test.4th");
        File.Delete(path);

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
            QueueLine(editor, "EDIT forth-indent-test.4th");
            RunUntilScreenContains(cpu, bus, "forth-indent-test.4th", 80_000_000);
            QueueLine(editor, ": INDENT");
            QueueLine(editor, "DUP");
            RunSteps(cpu, bus, 5_000_000);

            Assert.AreEqual((byte)'D', bus.Vgc.GetScreenChar(2, 2),
                "After a Forth definition opener, auto-indent should insert two spaces on the next line.");
            QueueSave(editor);
            QueueAltX(editor);
            RunUntilForthIdle(cpu, bus, editor, 120_000_000);

            StringAssert.Contains(File.ReadAllText(path), ": INDENT\n  DUP\n");
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void StringLiteralAndAbortQuotePrimitivesWork()
    {
        string screen = RunForthLines(
            "S\" HI\" TYPE",
            ": SAY S\" OK\" TYPE ;",
            "SAY",
            ": CHECK ABORT\" BAD\" ;",
            "0 CHECK 123 .",
            "-1 CHECK",
            "7 .");

        Assert.IsTrue(screen.Contains("HI", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("OK", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("123 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("BAD", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("7 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void LoopPrimitivesWork()
    {
        string screen = RunForthLines(
            ": TDO 5 0 DO I . LOOP ;",
            "TDO",
            ": TPLUS 10 0 DO I . 3 +LOOP ;",
            "TPLUS",
            ": TJ 2 0 DO 2 0 DO J . I . LOOP LOOP ;",
            "TJ",
            ": TLEAVE 5 0 DO I . I 2 = IF LEAVE THEN LOOP 9 . ;",
            "TLEAVE",
            ": TUN 5 0 DO I . UNLOOP EXIT LOOP 9 . ;",
            "TUN");

        Assert.IsTrue(screen.Contains("0 1 2 3 4", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 3 6 9", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 0 0 1 1 0 1 1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 1 2 9", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void DefiningWordPrimitivesWork()
    {
        string screen = RunForthLines(
            ": FACT DUP 2 < IF DROP 1 ELSE DUP 1- RECURSE * THEN ;",
            "5 FACT .",
            "CREATE CELL 99 ,",
            "' CELL >BODY @ .",
            ": BUILDER CREATE , DOES> @ 1+ ;",
            "41 BUILDER ANSWER",
            "ANSWER .");

        Assert.IsTrue(screen.Contains("120 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("99 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("42 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void RedefinitionSearchesPreviousDefinitionWhileCompiling()
    {
        string screen = RunForthLines(
            ": GDX 123 ;",
            ": GDX GDX 234 ;",
            "GDX .S");

        Assert.IsTrue(screen.Contains("<2> 123 234", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void HayesHarnessAcceptsRedefinitionSearchRule()
    {
        string screen = RunForthLines(
            "INCLUDE forth/test/hayes-core/ttester-nova.4th",
            "T{ : GDX 123 ; : GDX GDX 234 ; -> }T",
            "T{ GDX -> 123 234 }T",
            "HREPORT");

        Assert.IsTrue(screen.Contains("0 HAYES-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCORRECT RESULT", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("WRONG NUMBER OF RESULTS", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void EnvironmentQueryReportsForth2012Claims()
    {
        string screen = RunForthLines(
            "S\" core\" ENVIRONMENT? SWAP . .",
            "S\" CORE-EXT\" ENVIRONMENT? SWAP . .",
            "S\" FILE\" ENVIRONMENT? SWAP . .",
            "S\" FLOORED\" ENVIRONMENT? SWAP . .",
            "S\" ADDRESS-UNIT-BITS\" ENVIRONMENT? SWAP . .",
            "S\" /COUNTED-STRING\" ENVIRONMENT? SWAP . .",
            "S\" MAX-N\" ENVIRONMENT? SWAP . .",
            "S\" STACK-CELLS\" ENVIRONMENT? SWAP . .",
            "S\" NOPE\" ENVIRONMENT? .");

        Assert.IsTrue(screen.Contains("-1 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("8 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("255 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("32767 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("64 -1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void CoreExtensionBootLibraryWordsWork()
    {
        string screen = RunForthLines(
            "TRUE . FALSE . 1 2 <> . 1 1 <> . 0 0<> . 5 0> . -1 0> .",
            "20 10 30 WITHIN . 10 10 30 WITHIN . 30 10 30 WITHIN .",
            "1 2 NIP . 1 2 TUCK .S 2DROP DROP",
            ": RPAIR 1 2 2>R 2R@ 2R> ; RPAIR .S 2DROP 2DROP",
            "4 BUFFER: B1 65 B1 C! B1 C@ .");

        Assert.IsTrue(screen.Contains("-1 0 -1 0 0 -1 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("-1 -1 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 <3> 2 1 2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<4> 1 2 1 2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("65 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void AdditionalCoreExtensionWordsWork()
    {
        string screen = RunForthLines(
            "10 VALUE LIMIT LIMIT . 20 TO LIMIT LIMIT . : SETLIMIT 30 TO LIMIT ; SETLIMIT LIMIT .",
            "1 2 3 0 PICK .S 2DROP 2DROP",
            "1 2 3 2 PICK .S 2DROP 2DROP",
            "1 2 3 1 ROLL .S 2DROP DROP",
            "1 2 3 2 ROLL .S 2DROP DROP",
            "CHAR , PARSE AB, SWAP DROP .",
            ": RT 321 5 .R ; RT 8 .",
            ": RNEG -42 5 .R ; RNEG 8 .",
            ": DPAREN .( BUILDOK) 99 ; DPAREN .",
            "HERE 3 ALLOT DUP 3 88 FILL DUP 3 ERASE DUP C@ . DUP 2 + C@ . DROP",
            "HERE 65 C, 66 C, 0 0 <# ROT 2 HOLDS #> TYPE",
            "PAD 77 OVER C! C@ .");

        Assert.IsTrue(screen.Contains("10 20 30", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<4> 1 2 3 3", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<4> 1 2 3 1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<3> 1 3 2", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<3> 2 3 1", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("3 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("  3218", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("  -428", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("BUILDOK", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("99 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("0 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("AB", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("77 ", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void IncludeLoadsForthSourceFromStorage()
    {
        string path = Hd0Path("FORTH-TEST.4TH");
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, ": FROMFILE 77 ;\n");

        try
        {
            string screen = RunForthLines(
                "INCLUDE FORTH-TEST.4TH",
                "FROMFILE .");

            Assert.IsTrue(screen.Contains("77 ", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void IncludeLoadsForthSourceLargerThanOldRamStagingBuffer()
    {
        string path = Hd0Path("bigforth.4th");
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        var source = new StringBuilder();
        for (int i = 0; i < 180; i++)
            source.Append("\\ filler line ").Append(i.ToString("D3")).Append(" 0123456789 0123456789 0123456789 0123456789\n");
        source.Append(": BIGINCLUDE 4321 ;\n");
        Assert.IsTrue(Encoding.ASCII.GetByteCount(source.ToString()) > 8192);
        File.WriteAllText(path, source.ToString(), Encoding.ASCII);

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
            QueueLine(editor, "INCLUDE bigforth.4th");
            RunSteps(cpu, bus, 120_000_000);
            QueueLine(editor, "BIGINCLUDE .");
            RunUntilScreenContains(cpu, bus, "4321 ", 80_000_000);

            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsTrue(screen.Contains("4321 ", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void SaveForthWritesSourceThatCanBeIncluded()
    {
        string path = Hd0Path("SAVED.4TH");
        File.Delete(path);

        try
        {
            string screen = RunForthLines(
                "S\" : SAVED 5 ;\" S\" SAVED.4TH\" SAVE-FORTH",
                "INCLUDE SAVED.4TH",
                "SAVED .");

            Assert.IsTrue(screen.Contains("5 ", StringComparison.Ordinal), screen);
            Assert.AreEqual(": SAVED 5 ;", File.ReadAllText(path));
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void FileAccessWordsUseSharedFilesModule()
    {
        string path = Hd0Path("fstream.txt");
        File.Delete(path);

        try
        {
            string screen = RunForthLines(
                "VARIABLE FID VARIABLE N",
                "S\" fstream.txt\" R/W CREATE-FILE SWAP FID ! .",
                "S\" Alpha\" FID @ WRITE-LINE .",
                "0 0 FID @ REPOSITION-FILE .",
                "HERE 16 FID @ READ-LINE ROT N ! DROP DROP HERE N @ TYPE",
                "FID @ CLOSE-FILE .",
                "S\" fstream.txt\" DELETE-FILE .");

            Assert.IsTrue(screen.Contains("Alpha", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
            Assert.IsFalse(File.Exists(path), "DELETE-FILE should remove the file created by CREATE-FILE.");
        }
        finally
        {
            File.Delete(path);
        }
    }

    [TestMethod]
    public void NovaFilesModuleLibraryWrapsFileDomainNdk()
    {
        string[] files =
        [
            Hd0Path("nraw.bin"),
            Hd0Path("npage.bin"),
            Hd0Path("ngfx.bin"),
            Hd0Path("nhandle.txt"),
            Hd0Path("nrenamed.txt")
        ];
        string dir = Hd0Path("ndkdir");

        foreach (string file in files)
            File.Delete(file);
        if (Directory.Exists(dir))
            Directory.Delete(dir, recursive: true);

        try
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

            string[] lines =
            [
                "INCLUDE forth/lib/nova/files.4th",
                ": ?IOR IF ABORT THEN ;",
                ": ?EQ = 0= IF ABORT THEN ;",
                "CREATE FB 64 ALLOT VARIABLE FID",
                ": FPREP 82 FB C! 65 FB 1 + C! 87 FB 2 + C! 33 FB 3 + C! ;",
                "FPREP",
                "S\" nraw.bin\" FB FB 4 + FILES-SAVE ?IOR",
                "0 FB 8 + C! 0 FB 9 + C! 0 FB 10 + C! 0 FB 11 + C!",
                "S\" nraw.bin\" FB 8 + FILES-LOAD ?IOR",
                "FB 8 + C@ 82 ?EQ FB 11 + C@ 33 ?EQ",
                "S\" nraw.bin\" FILES-DIR-OPEN ?IOR FILES-DIR-READ ?IOR",
                "FILES-NAME DROP C@ 110 ?EQ",
                "S\" ndkdir\" FILES-MKDIR ?IOR",
                "S\" ndkdir\" FILES-CD ?IOR",
                "FILES-PWD ?IOR 2DROP",
                "S\" ..\" FILES-CD ?IOR",
                "S\" ndkdir\" FILES-RMDIR ?IOR",
                "S\" ngfx.bin\" 1 0 1 FILES-GSAVE ?IOR",
                "S\" ngfx.bin\" 1 1 1 FILES-GLOAD ?IOR",
                "FIO-CMD-CLEARERR FILES-RUN ?IOR",
                "FILES-RNG ?IOR 2DROP",
                "S\" npage.bin\" FB FB 4 + FILES-SAVE ?IOR",
                "0 FB 12 + C! 0 FB 13 + C!",
                "S\" npage.bin\" 1 0 PAGER-TARGET-RAM FB 12 + 0 2 FILES-PAGE ?IOR",
                "FB 12 + C@ 65 ?EQ FB 13 + C@ 87 ?EQ",
                "S\" nhandle.txt\" R/W FILES-FCREATE ?IOR FID !",
                "S\" H!\" FID @ FILES-FWRITE ?IOR 2 ?EQ",
                "FID @ FILES-FTELL ?IOR DROP 2 ?EQ",
                "FID @ FILES-FSIZE ?IOR DROP 2 ?EQ",
                "4 0 FID @ FILES-FRESIZE ?IOR",
                "FID @ FILES-FSIZE ?IOR DROP 4 ?EQ",
                "0 0 FID @ FILES-FSEEK ?IOR",
                "0 FB 20 + C! 0 FB 21 + C!",
                "FB 20 + 2 FID @ FILES-FREAD ?IOR 2 ?EQ",
                "FB 20 + C@ 72 ?EQ FB 21 + C@ 33 ?EQ",
                "FID @ FILES-FFLUSH ?IOR",
                "FID @ FILES-FCLOSE ?IOR",
                "S\" nhandle.txt\" R/O FILES-FOPEN ?IOR FID !",
                "FID @ FILES-FCLOSE ?IOR",
                "S\" nhandle.txt\" FILES-FSTATUS ?IOR DROP",
                "S\" nhandle.txt\" S\" nrenamed.txt\" FILES-FRENAME ?IOR",
                "S\" nrenamed.txt\" FILES-FDELETE ?IOR",
                "S\" nraw.bin\" FILES-DELETE ?IOR",
                "S\" npage.bin\" FILES-FDELETE ?IOR",
                "S\" ngfx.bin\" FILES-FDELETE ?IOR",
                "S\" NDK-FILES-DONE\" TYPE"
            ];

            foreach (string line in lines)
            {
                QueueLine(editor, line);
                RunUntilForthIdle(cpu, bus, editor, 160_000_000);
            }

            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsTrue(screen.Contains("NDK-FILES-DONE", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("ABORTED", StringComparison.Ordinal), screen);
        }
        finally
        {
            foreach (string file in files)
                File.Delete(file);
            if (Directory.Exists(dir))
                Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void BootAutoloadsCoreAndAutoexecLibrariesWhenPresent()
    {
        string libDir = Hd0Path("forth", "lib");
        Directory.CreateDirectory(libDir);
        string corePath = Path.Combine(libDir, "core.4th");
        string coreExtPath = Path.Combine(libDir, "core-ext.4th");
        string autoexecPath = Hd0Path("forth", "autoexec.4th");
        string? oldCore = File.Exists(corePath) ? File.ReadAllText(corePath) : null;
        string? oldCoreExt = File.Exists(coreExtPath) ? File.ReadAllText(coreExtPath) : null;
        string? oldAutoexec = File.Exists(autoexecPath) ? File.ReadAllText(autoexecPath) : null;
        File.WriteAllText(corePath, ": AUTOCORE 123 ;\n");
        File.WriteAllText(coreExtPath, ": AUTOEXT 456 ;\n");
        File.WriteAllText(autoexecPath, "INCLUDE forth/lib/core-ext.4th\n");

        try
        {
            string screen = RunForthLines("AUTOCORE . AUTOEXT .");

            Assert.IsTrue(screen.Contains("123 ", StringComparison.Ordinal), screen);
            Assert.IsTrue(screen.Contains("456 ", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
            Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        }
        finally
        {
            RestoreFile(corePath, oldCore);
            RestoreFile(coreExtPath, oldCoreExt);
            RestoreFile(autoexecPath, oldAutoexec);
        }
    }

    [TestMethod]
    public void VgcLibraryDrawsCircle()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/vgc.4th");
        RunSteps(cpu, bus, 16_000_000);
        QueueLine(editor, ": VGC-FILL-CHECK HERE 4 ALLOT DUP 4 90 FILL DUP C@ 90 = SWAP 3 + C@ 90 = AND 0= IF ABORT THEN ;");
        RunSteps(cpu, bus, 8_000_000);
        QueueLine(editor, "VGC-FILL-CHECK");
        RunSteps(cpu, bus, 8_000_000);
        QueueLine(editor, "EGA-PALETTE NOVA-PALETTE GRAPHICS GCLS 5 GCOLOR 100 100 20 CIRCLE");
        RunSteps(cpu, bus, 8_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);

        Assert.AreEqual(1, bus.Read((ushort)VgcConstants.RegMode),
            "GRAPHICS should switch the VGC into graphics-over-text mode.");
        Assert.AreEqual(VgcConstants.PaletteModeC64, bus.Read((ushort)VgcConstants.RegPaletteMode),
            "NOVA-PALETTE should select the Nova/C64 fixed palette.");
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.RegCmd),
            "VGC-CMD! should wait until the VGC command register is idle.");
        Assert.AreEqual(5, GfxPixel(bus, 120, 100),
            "CIRCLE must draw the right cardinal point with GCOLOR 5.");
        Assert.AreEqual(5, GfxPixel(bus, 80, 100),
            "CIRCLE must draw the left cardinal point with GCOLOR 5.");
        Assert.AreEqual(5, GfxPixel(bus, 100, 80),
            "CIRCLE must draw the top cardinal point with GCOLOR 5.");
        Assert.AreEqual(5, GfxPixel(bus, 100, 120),
            "CIRCLE must draw the bottom cardinal point with GCOLOR 5.");
        Assert.AreEqual(0, GfxPixel(bus, 100, 100),
            "CIRCLE should draw an outline, not fill the centre.");
    }

    [TestMethod]
    public void HayesCoreSuitePasses()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/test/hayes-core/ttester-nova.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-01-basic.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-02-arithmetic.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-03-division.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-04-memory-dictionary.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-05-flow-defining.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-06-evaluate-pictured-move.4th");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, "INCLUDE forth/test/hayes-core/core-07-search-finish.4th");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "HREPORT");

        RunUntilScreenContains(cpu, bus, "HAYES-DONE", 240_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 HAYES-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void Forth2012CoreSuitePasses()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/test/forth2012/nova-core.4th");
        QueueLine(editor, "NovaForth accepts standard input");

        RunUntilScreenContains(cpu, bus, "F2012-CORE-DONE", 2_000_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 F2012-CORE-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("WRONG NUMBER OF RESULTS", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCORRECT RESULT", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void Forth2012CoreExtSuitePasses()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/test/forth2012/nova-core-ext.4th");

        RunUntilScreenContains(cpu, bus, "F2012-CORE-EXT-DONE", 2_000_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 F2012-CORE-EXT-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("WRONG NUMBER OF RESULTS", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCORRECT RESULT", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void Forth2012FileAccessSuitePasses()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/test/forth2012/nova-file.4th");

        RunUntilScreenContains(cpu, bus, "F2012-FILE-DONE", 2_000_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 F2012-FILE-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("WRONG NUMBER OF RESULTS", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCORRECT RESULT", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void Forth2012StringSuitePasses()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/test/forth2012/nova-string.4th");

        RunUntilScreenContains(cpu, bus, "F2012-STRING-DONE", 2_000_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 F2012-STRING-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("WRONG NUMBER OF RESULTS", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCORRECT RESULT", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("RETURN STACK", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("SOURCE LINE TOO LONG", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void StringLibraryCompareBlankAndReplacementWordsWork()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/lib/string.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "INCLUDE forth/test/forth2012/src/tester.fr");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "INCLUDE forth/test/forth2012/src/utilities.fth");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, ": S13 S\" aaaaa      a\" ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "PAD 25 CHAR a FILL PAD 5 + 6 BLANK PAD 12 S13 COMPARE .S");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "DROP PAD 5 + C@ . PAD 10 + C@ . PAD 11 + C@ .");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "S13 DROP 5 + C@ . S13 DROP 10 + C@ . S13 DROP 11 + C@ .");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "S\" wxyz\" S\" mac1\" REPLACES .S");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "S\" %mac1%\" PAD 20 SUBSTITUTE .S");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, "2DROP DROP");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, ": MAC2 S\" mac2\" ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, ": SUBST PAD 20 SUBSTITUTE ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "S\" defg%mac1%hi\" SUBST . TYPE");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, "S\" 12\" MAC2 REPLACES");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "S\" abc %mac2% def%mac1%gh\" PAD 20 SUBSTITUTE . TYPE");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, ": REPL PARSE-NAME REPLACES ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "$\" \" REPL MAC3 .S");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "$\" abc%mac3%def%mac1%gh\" PAD 20 SUBSTITUTE . TYPE");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, "CREATE SUBBUF 48 CHARS ALLOT");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, ": $CHECK 2SWAP OVER SUBBUF <> >R COMPARE R> OR ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, ": $CHECKN ROT >R $CHECK R> ;");
        RunUntilForthIdle(cpu, bus, editor, 40_000_000);
        QueueLine(editor, "$\" abc%mac3%def%mac1%gh\" SUBBUF 20 SUBSTITUTE $\" abcdefwxyzgh\" $CHECKN .S");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("<1> 0", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("32 32 97", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<0>", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<3>", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 abcdefwxyzgh", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("1 defgwxyzhi", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 abc 12 defwxyzgh", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("<2> 0 2", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void NovaNdkSystemLibraryCallsSharedModule()
    {
        string screen = RunForthLines(
            "INCLUDE forth/lib/nova/system.4th",
            "TIMER . DROP RNG8 . DROP S\" NDK-SYSTEM-DONE\" TYPE");

        Assert.IsTrue(screen.Contains("0 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("NDK-SYSTEM-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void NovaNdkGraphicsLibraryPlotsThroughSharedModule()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/graphics.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "1 GFX-MODE . 0 GFX-GCLS . 5 GFX-GCOLOR . 12 9 GFX-PLOT . S\" NDK-GFX-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("0 0 0 0 NDK-GFX-DONE", StringComparison.Ordinal), screen);
        Assert.AreEqual(5, GfxPixel(bus, 12, 9));
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void NovaNdkRemainingModuleLibrariesSmoke()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/sound.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "SND-INIT . SND-STATUS . . S\" NDK-SOUND-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/memory.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "0 0 90 MEM-WRITE8 . 0 0 MEM-READ8 . . S\" NDK-MEMORY-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        Assert.AreEqual(90, bus.ReadXram(0),
            "MEM-WRITE8 should route through MODULE-MEMORY and write byte 90 to XRAM address 0.");

        QueueLine(editor, "INCLUDE forth/lib/nova/net.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "NET-NGS-INIT . NET-NGS-STATUS . . S\" NDK-NET-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/editor.4th");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);
        QueueLine(editor, "EDITOR-FN-EDIT . EDITOR-PROFILE-FORTH . S\" NDK-EDITOR-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NDK-SOUND-DONE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("NDK-MEMORY-DONE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("NDK-NET-DONE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("NDK-EDITOR-DONE", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void NovaForthCanDefineSquareWordUsingTurtleNdkLibrary()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        QueueLine(editor, "INCLUDE forth/lib/nova/turtle.4th");
        RunUntilForthIdle(cpu, bus, editor, 160_000_000);
        QueueLine(editor, ": SQUARE 4 0 DO 50 FD 90 RT LOOP ;");
        RunUntilForthIdle(cpu, bus, editor, 80_000_000);
        QueueLine(editor, "CS 5 SETPC SQUARE ST S\" NDK-TURTLE-SQUARE-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 360_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NDK-TURTLE-SQUARE-DONE", StringComparison.Ordinal), screen);
        Assert.AreEqual(40, bus.Read((ushort)VgcConstants.RegCursorY),
            "CS should park the Forth prompt at the first row of the turtle split text band.");
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.RegTextTopRow),
            "CS should leave the physical text row mapping stable for the split text band.");
        Assert.AreEqual(40, bus.Read((ushort)VgcConstants.RegTextScrollStart),
            "CS should constrain prompt scrolling to the bottom turtle text band.");
        Assert.AreEqual(10, bus.Read((ushort)VgcConstants.RegTextScrollRows),
            "CS should reserve the bottom ten text rows for Forth while turtle graphics are visible.");
        Assert.AreEqual(160, ReadRamWord(bus, 0x9F01),
            "The square word should return the turtle to the center X coordinate.");
        Assert.AreEqual(100, ReadRamWord(bus, 0x9F04),
            "The square word should return the turtle to the center Y coordinate.");
        Assert.AreEqual(0, ReadRamWord(bus, 0x9F06),
            "Four right turns should restore heading 0.");
        Assert.AreEqual(1, bus.ReadRam(0x9F09),
            "The Forth turtle demo should leave the turtle visible like Logo.");
        Assert.AreEqual(5, bus.ReadRam(0x9F0A),
            "SETPC 5 should store the raw pen color byte, not a fixed-point value.");
        Assert.AreEqual(1, bus.ReadRam(0x9F11),
            "CS should leave the turtle module in split graphics mode.");
        Assert.AreEqual(5, GfxPixel(bus, 160, 75), "Left side of the square should be color 5.");
        Assert.AreEqual(5, GfxPixel(bus, 185, 50), "Top side of the square should be color 5.");
        Assert.AreEqual(5, GfxPixel(bus, 210, 75), "Right side of the square should be color 5.");
        Assert.AreEqual(5, GfxPixel(bus, 185, 100), "Bottom side of the square should be color 5.");
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);

        QueueLine(editor, "TS S\" NDK-TURTLE-TEXT-DONE\" TYPE");
        RunUntilForthIdle(cpu, bus, editor, 120_000_000);

        screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NDK-TURTLE-TEXT-DONE", StringComparison.Ordinal), screen);
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.RegTextTopRow),
            "TS should restore full-screen text row mapping.");
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.RegTextScrollStart),
            "TS should restore full-screen text scrolling from row 0.");
        Assert.AreEqual(50, bus.Read((ushort)VgcConstants.RegTextScrollRows),
            "TS should return to the default full-screen text scroll window.");
        Assert.IsFalse(screen.Contains("UNKNOWN WORD", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal), screen);
        Assert.IsFalse(screen.Contains("INCLUDE FAILED", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void EscapedStringLiteralDecodesCoreExtEscapes()
    {
        string screen = RunForthLines(
            "INCLUDE forth/lib/core-ext.4th",
            ": SSQ3 S\\\" \\a\\b\\e\\f\\l\\m\\q\\r\\t\\v\\x0F0\\x1Fa\\xaBx\\z\\\"\\\\\" ;",
            "SSQ3 SWAP DROP .",
            "SSQ3 DROP C@ . SSQ3 DROP 1 + C@ . SSQ3 DROP 2 + C@ . SSQ3 DROP 3 + C@ .",
            "SSQ3 DROP 4 + C@ . SSQ3 DROP 5 + C@ . SSQ3 DROP 6 + C@ . SSQ3 DROP 7 + C@ .",
            "SSQ3 DROP 8 + C@ . SSQ3 DROP 9 + C@ . SSQ3 DROP 10 + C@ . SSQ3 DROP 11 + C@ .",
            "SSQ3 DROP 12 + C@ . SSQ3 DROP 13 + C@ . SSQ3 DROP 14 + C@ . SSQ3 DROP 15 + C@ .",
            "SSQ3 DROP 16 + C@ . SSQ3 DROP 17 + C@ . SSQ3 DROP 18 + C@ . SSQ3 DROP 19 + C@ .");

        Assert.IsTrue(screen.Contains("20 ", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("7 8 27 12", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("10 13 10 34", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("13 9 11 15", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("48 31 97 171", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("120 0 34 92", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void EscapedStringLiteralSurvivesNestedEvaluate()
    {
        string screen = RunForthLines(
            "INCLUDE forth/lib/core-ext.4th",
            """: SSQ7  S\" 111 : SSQ8 S\\\" 222\" EVALUATE ; SSQ8 333" EVALUATE ;""",
            """SSQ7 . . . S" SSQ7-DONE" TYPE""",
            """: SSQ9  S\" 11 : SSQ10 S\\\" \\x32\\x32\" EVALUATE ; SSQ10 33" EVALUATE ;""",
            """SSQ9 . . . S" SSQ9-DONE" TYPE""");

        Assert.IsTrue(screen.Contains("333 222 111 SSQ7-DONE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("33 22 11 SSQ9-DONE", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void SaveRestoreInputReplaysStringSourcePosition()
    {
        string screen = RunForthLines(
            "VARIABLE SI_INC 0 SI_INC !",
            ": SI1 SI_INC @ >IN +! 15 SI_INC ! ;",
            ": S$ S\" SAVE-INPUT SI1 RESTORE-INPUT 12345\" ;",
            "S$ EVALUATE SI_INC @ .S");

        Assert.IsTrue(screen.Contains("<3> 0 2345 15", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void UnknownWordAbortsLineAndReturnsToPrompt()
    {
        string screen = RunForthLines("NOPE", "1 1 + .");

        Assert.IsTrue(screen.Contains("UNKNOWN WORD: NOPE", StringComparison.Ordinal), screen);
        Assert.IsTrue(screen.Contains("2 ", StringComparison.Ordinal), screen);
    }

    [TestMethod]
    public void ForthDoesNotClobberReservedLoaderBands()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);

        for (ushort a = ReservedStart; a < ReservedEnd; a++)
            bus.WriteRam(a, Sentinel);

        QueueLine(editor, ": SQUARE DUP * ;");
        QueueLine(editor, "9 SQUARE .");
        QueueLine(editor, "VARIABLE X");
        QueueLine(editor, "123 X ! X @ .");
        RunUntilScreenContains(cpu, bus, "123 ", 80_000_000);

        for (ushort a = ReservedStart; a < ReservedEnd; a++)
        {
            Assert.AreEqual(Sentinel, bus.ReadRam(a),
                $"NovaForth clobbered reserved band byte ${a:X4}; runtime BSS/dictionary must stay at $0900+.");
        }
    }

    private static string RunForthLines(params string[] lines)
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
        foreach (string line in lines)
        {
            QueueLine(editor, line);
            RunSteps(cpu, bus, 4_000_000);
        }

        return SnapshotScreen(bus.Vgc);
    }

    private static string RunForthLineWithRawInput(string line, string rawInput)
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Forth);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "OK>", 10_000_000);
        QueueLine(editor, line);
        foreach (char ch in rawInput)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
        RunSteps(cpu, bus, 8_000_000);

        return SnapshotScreen(bus.Vgc);
    }

    private static string Hd0Path(params string[] parts)
    {
        string? root = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
        Assert.IsFalse(string.IsNullOrWhiteSpace(root), "AssemblySetup must set NOVA_STORAGE_ROOT.");
        return Path.Combine([root!, "hd0", .. parts]);
    }

    private static void RestoreFile(string path, string? contents)
    {
        if (contents is null)
            File.Delete(path);
        else
            File.WriteAllText(path, contents);
    }

    private static byte GfxPixel(CompositeBusDevice bus, int x, int y)
    {
        int offset = y * VgcConstants.GfxWidth + x;
        Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, offset, out byte value),
            $"Could not read VGC gfx pixel ({x},{y}).");
        return value;
    }

    private static ushort ReadRamWord(CompositeBusDevice bus, ushort address) =>
        (ushort)(bus.ReadRam(address) | (bus.ReadRam((ushort)(address + 1)) << 8));

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
    }

    private static void QueueSave(ScreenEditor editor)
    {
        editor.QueueInput(CtrlK);
        editor.QueueInput((byte)'s');
    }

    private static void QueueAltX(ScreenEditor editor)
    {
        editor.QueueInput(KeyEsc);
        editor.QueueInput((byte)'x');
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, long maxSteps)
    {
        for (long i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }

        Assert.Fail($"Timed out waiting for screen to contain '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
    }

    private static void RunUntilForthIdle(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, int maxSteps)
    {
        bool sawCursorOffAfterInput = false;
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if (!editor.HasQueuedInput && !bus.Vgc.IsCursorEnabled)
                sawCursorOffAfterInput = true;
            if (sawCursorOffAfterInput && !editor.HasQueuedInput && bus.Vgc.IsCursorEnabled)
                return;
            if ((i & 0x3FF) == 0)
            {
                string screen = SnapshotScreen(bus.Vgc);
                if (screen.Contains("STACK UNDERFLOW", StringComparison.Ordinal) ||
                    screen.Contains("RETURN STACK", StringComparison.Ordinal) ||
                    screen.Contains("UNKNOWN WORD", StringComparison.Ordinal) ||
                    screen.Contains("INCLUDE FAILED", StringComparison.Ordinal))
                    Assert.Fail(screen);
            }
        }

        CpuState state = cpu.GetState();
        byte sp = state.Sp;
        string stackBytes =
            $"{bus.ReadRam((ushort)(0x0100 | ((sp + 1) & 0xFF))):X2} " +
            $"{bus.ReadRam((ushort)(0x0100 | ((sp + 2) & 0xFF))):X2} " +
            $"{bus.ReadRam((ushort)(0x0100 | ((sp + 3) & 0xFF))):X2} " +
            $"{bus.ReadRam((ushort)(0x0100 | ((sp + 4) & 0xFF))):X2}";
        Assert.Fail(
            $"Timed out waiting for Forth prompt. PC=${cpu.Pc:X4} state={bus.ReadRam(0x0037):X2} abort={bus.ReadRam(0x0038):X2} " +
            $"dsp={bus.ReadRam(0x0039):X2} rsp={bus.ReadRam(0x003A):X2} ip=${(bus.ReadRam(0x0041) | (bus.ReadRam(0x0042) << 8)):X4} " +
            $"sp=${sp:X2} stack={stackBytes}\n" +
            SnapshotScreen(bus.Vgc));
    }

    private static void RunSteps(Cpu cpu, CompositeBusDevice bus, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }
    }

    private static string SnapshotScreen(VirtualGraphicsController vgc)
    {
        var sb = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = vgc.GetScreenChar(col, row);
                sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }
}
