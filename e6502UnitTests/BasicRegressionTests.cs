using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Integration-level regressions exercising EhBASIC ROM + Avalonia bus.
/// Each test entered verbatim lines the same way the integration harness
/// does, then asserts the printed screen contains expected output.
/// </summary>
[TestClass]
public class BasicRegressionTests
{
    [TestMethod]
    public void ArrayIndexedAssignAndPrintReadsBack()
    {
        string screen = RunProgram(new[]
        {
            "10 DIM A(10)",
            "20 FOR I=0 TO 10:A(I)=I*I:NEXT",
            "30 PRINT A(5)",
            "RUN"
        });

        Assert.IsTrue(screen.Contains(" 25"),
            $"A(5) should be 25 but screen shows:\n{screen}");
    }

    [TestMethod]
    public void MathCoprocessorFunctionsAreAvailableFromBasic()
    {
        string screen = RunProgram(new[]
        {
            "10 IF MSIN(64)=127 THEN 20",
            "15 PRINT \"BAD MSIN\":END",
            "20 IF MCOS(0)=127 THEN 30",
            "25 PRINT \"BAD MCOS\":END",
            "30 IF MSIN(192)=-128 THEN 40",
            "35 PRINT \"BAD SNEG\":END",
            "40 IF MDIST(3,4)=5 THEN 50",
            "45 PRINT \"BAD DIST\":END",
            "50 IF MDIST(-10,4)=12 THEN 60",
            "55 PRINT \"BAD DNEG\":END",
            "60 IF MMULFX($0180,$0180)=576 THEN 70",
            "65 PRINT \"BAD MUL\":END",
            "70 IF MMULFX(-256,512)=-512 THEN 80",
            "75 PRINT \"BAD MNEG\":END",
            "80 IF MDIV(-100,7)=-14 THEN 90",
            "85 PRINT \"BAD MDIV\":END",
            "90 IF MREM(-100,7)=-2 THEN 100",
            "95 PRINT \"BAD MREM\":END",
            "100 IF MATAN2(100,0)=64 THEN 110",
            "105 PRINT \"BAD ATAN\":END",
            "110 IF MDOTFX($0180,$0100,$0200,-256)=512 THEN 120",
            "115 PRINT \"BAD DOT\":END",
            "120 IF MLEN2(3,4)=25 THEN 130",
            "125 PRINT \"BAD LEN\":END",
            "130 IF MSCALX($0180,$0100,$0080)=192 THEN 140",
            "135 PRINT \"BAD SCX\":END",
            "140 IF MSCALY($0180,$0100,$0080)=128 THEN 150",
            "145 PRINT \"BAD SCY\":END",
            "150 IF MMUL16L(300,4)=1200 THEN 160",
            "155 PRINT \"BAD M16L\":END",
            "160 IF MMUL16H(-100,7)=-1 THEN 170",
            "165 PRINT \"BAD M16H\":END",
            "170 IF MDOTS16L($0180,$0100,$0200,-256)=0 THEN 180",
            "175 PRINT \"BAD DSL\":END",
            "180 IF MDOTS16H($0180,$0100,$0200,-256)=2 THEN 190",
            "185 PRINT \"BAD DSH\":END",
            "190 IF MCROSSL($0180,$0100,$0200,-256)=32768 THEN 200",
            "195 PRINT \"BAD CRL\":END",
            "200 IF MCROSSH($0180,$0100,$0200,-256)=-4 THEN 210",
            "205 PRINT \"BAD CRH\":END",
            "210 A=MRND:IF A<0 THEN 225",
            "215 IF A>255 THEN 225",
            "220 PRINT \"MATH OK\":END",
            "225 PRINT \"BAD MRND\"",
            "RUN"
        });
        int runIndex = screen.LastIndexOf("RUN", StringComparison.Ordinal);
        string output = runIndex >= 0 ? screen[runIndex..] : screen;

        Assert.IsFalse(output.Contains("BAD", StringComparison.Ordinal),
            $"Math coprocessor BASIC functions should return expected values.\n{screen}");
        Assert.IsTrue(output.Contains("MATH OK", StringComparison.Ordinal),
            $"Math coprocessor BASIC program did not finish cleanly.\n{screen}");
    }

    [TestMethod]
    public void RelocatedExtensionRomPrimitivesExecuteThroughBasicDispatch()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();

        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 XBANK 0",
            "20 XPOKE 4660,90",
            "30 VPOKE 2,0,77",
            "40 POKE 5000,0",
            "50 BITSET 5000,3",
            "60 BITTGL 5000,1",
            "70 BITCLR 5000,2",
            "80 POKE 5001,XPEEK(4660)",
            "90 POKE 5002,VPEEK(2,0)",
            "100 POKE 5003,PLAYING",
            "110 POKE 5004,MNOTE(1)",
            "120 PRINT \"EXT OK\"",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Relocated extension ROM primitives should parse and run.\n{screen}");
        Assert.IsFalse(screen.Contains("Function Call Error", StringComparison.Ordinal),
            $"Relocated extension ROM primitives should preserve their argument handling.\n{screen}");
        Assert.IsTrue(screen.Contains("EXT OK", StringComparison.Ordinal),
            $"Relocated extension ROM primitive program did not finish cleanly.\n{screen}");
        Assert.AreEqual(0, bus.Read(5000),
            "BITSET/BITTGL/BITCLR should leave the target byte cleared.");
        Assert.AreEqual(90, bus.Read(5001),
            "XPEEK should read the byte written through XPOKE.");
        Assert.AreEqual(77, bus.Read(5002),
            "VPEEK should read the byte written through VPOKE.");
        Assert.AreEqual(0, bus.Read(5003),
            "PLAYING should return zero when no music is active.");
        Assert.AreEqual(0, bus.Read(5004),
            "MNOTE should return zero for a silent voice.");
    }

    [TestMethod]
    public void ClsClearsActiveTextWindowOnly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            $"VPOKE 1,{39 * VgcConstants.ScreenCols},84",
            $"VPOKE 1,{40 * VgcConstants.ScreenCols},87",
            $"POKE {VgcConstants.TextWindowLeft},0",
            $"POKE {VgcConstants.TextWindowTop},40",
            $"POKE {VgcConstants.TextWindowWidth},80",
            $"POKE {VgcConstants.TextWindowHeight},10",
            "CLS",
        ]);

        Assert.AreEqual((byte)'T', bus.Vgc.GetScreenChar(0, 39),
            "CLS in a split-screen text window must not clear rows above the active window.");
        Assert.AreNotEqual((byte)'W', bus.Vgc.GetScreenChar(0, 40),
            "CLS should clear content inside the active text window before BASIC prints Ready.");
        Assert.IsTrue(bus.Vgc.GetCursorY() >= 40,
            $"CLS should leave the cursor inside the active text window, got row {bus.Vgc.GetCursorY()}.");
    }

    [TestMethod]
    public void SpriteCollisionFunctionsAndEventSyntaxAreAvailableFromBasic()
    {
        string screen = RunProgram(new[]
        {
            "10 ON SPRITE COLLISION GOSUB 100",
            "20 PRINT \"EVENT OK\"",
            "30 END",
            "100 PRINT \"IRQ\"",
            "110 RETIRQ",
            "RUN"
        });

        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"ON SPRITE COLLISION GOSUB should tokenize and run.\n{screen}");
        Assert.IsTrue(screen.Contains("EVENT OK", StringComparison.Ordinal),
            $"Sprite collision event setup program did not finish cleanly.\n{screen}");

        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        bus.Vgc.SetCollisionRegisters(0x0203, 0x0404);

        EnterProgramLines(cpu, bus, editor,
        [
            "PRINT SPRCOLL",
            "PRINT SPRBG",
        ]);

        string functionScreen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(functionScreen.Contains(" 515", StringComparison.Ordinal),
            $"SPRCOLL should return the full 16-bit sprite collision mask.\n{functionScreen}");
        Assert.IsTrue(functionScreen.Contains(" 1028", StringComparison.Ordinal),
            $"SPRBG should return the full 16-bit sprite-background collision mask.\n{functionScreen}");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColSt),
            "SPRCOLL should clear the sprite collision low register.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColStHi),
            "SPRCOLL should clear the sprite collision high register.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColBg),
            "SPRBG should clear the sprite-background collision low register.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColBgHi),
            "SPRBG should clear the sprite-background collision high register.");
    }

    [TestMethod]
    public void SpriteCollisionIrqDispatchesToBasicHandler()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 H=0:C=0",
            "20 ON SPRITE COLLISION GOSUB 100",
            "30 PRINT \"WAIT\"",
            "40 IF H=0 THEN 40",
            "50 PRINT \"DONE\";C",
            "60 END",
            "100 C=SPRCOLL",
            "110 H=1",
            "120 RETIRQ",
        ]);
        QueueLine(editor, "CLS");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        QueueLine(editor, "RUN");
        RunUntil(cpu, bus, 50_000_000,
            () => SnapshotScreen(bus.Vgc).Contains("WAIT", StringComparison.Ordinal),
            "BASIC program to arm sprite collision IRQ");

        Assert.AreEqual(
            VgcConstants.IrqSpriteCollision,
            (byte)(bus.Vgc.Read(VgcConstants.RegIrqEnable) & VgcConstants.IrqSpriteCollision),
            "ON SPRITE COLLISION GOSUB should enable the VGC sprite-collision IRQ source.");

        bus.Vgc.SetCollisionRegisters(0x0103, 0x0000);
        Assert.IsTrue(bus.VgcIrqPending, "Collision should raise the enabled VGC IRQ source.");
        cpu.IrqWaiting = true;
        RunUntil(cpu, bus, 50_000_000,
            () => SnapshotScreen(bus.Vgc).Contains("DONE 259", StringComparison.Ordinal),
            "sprite collision IRQ handler to run");

        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColSt),
            "SPRCOLL in the IRQ handler should clear the collision low latch.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColStHi),
            "SPRCOLL in the IRQ handler should clear the collision high latch.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegIrqStatus),
            "SPRCOLL in the IRQ handler should acknowledge the VGC collision IRQ source.");
    }

    [TestMethod]
    public void RamIrqTrampolineAcknowledgesSpriteCollisionBeforeReturning()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        const ushort irqVector = 0x020D;
        byte[] expected =
        [
            0x48,                               // PHA
            0xA5, 0xDF,                         // LDA IrqBase
            0x4A,                               // LSR
            0x05, 0xDF,                         // ORA IrqBase
            0x85, 0xDF,                         // STA IrqBase
            0xA9, VgcConstants.IrqSpriteCollision,
            0x8D, 0xF1, 0xA0,                   // STA VGC_IRQ_STATUS
            0x68,                               // PLA
            0x40,                               // RTI
        ];

        for (int offset = 0; offset < expected.Length; offset++)
        {
            Assert.AreEqual(expected[offset], bus.ReadRam((ushort)(irqVector + offset)),
                $"BASIC RAM IRQ trampoline byte mismatch at ${irqVector + offset:X4}; sprite collision IRQs must be acknowledged before RTI to avoid an IRQ storm.");
        }
    }

    // SPRITE keywords route through lib_call(GRAPHICS) after the Phase 2 refactor
    // (sprite.s is no longer compiled into the BASIC ROM). This test drives the
    // BASIC marshalling end-to-end and asserts the observable VGC sprite state,
    // proving SPRITE n,x,y / n,ON / n,OFF, SPRITESHAPE, SPRITESET (byte+word
    // field), and SPRITEDATA all marshal their args into the module correctly.
    [TestMethod]
    public void SpriteCommandKeywordsRouteThroughLibCall()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 SPRITE 3,200,120",       // GFN_SPR_POS: idx,x(s16),y
            "20 SPRITE 3,ON",            // GFN_SPR_ENABLE
            "30 SPRITE 5,ON",            // enable then disable to prove DISABLE
            "40 SPRITE 5,OFF",           // GFN_SPR_DISABLE
            "50 SPRITESHAPE 6,9",        // GFN_SPR_SHAPE
            "60 SPRITESET 7,6,2",        // GFN_SPR_SETREG: field 6 = priority (byte)
            "70 SPRITESET 8,0,300",      // GFN_SPR_SETREG16: field 0 = X (16-bit word)
            "80 SPRITEDATA 4,1,1,2,3,4,5,6,7,8",  // GFN_SPR_ROW: 8 bytes into row 1
        ]);

        QueueLine(editor, "RUN");
        RunUntilEditorIdle(cpu, bus, editor, 80_000_000);

        var s3 = bus.Vgc.GetSpriteState(3);
        Assert.AreEqual(200, s3.x, "SPRITE 3,200,120 must set sprite 3 X to 200.");
        Assert.AreEqual(120, s3.y, "SPRITE 3,200,120 must set sprite 3 Y to 120.");
        Assert.IsTrue(s3.enabled, "SPRITE 3,ON must enable sprite 3.");

        Assert.IsFalse(bus.Vgc.GetSpriteState(5).enabled,
            "SPRITE 5,OFF must disable sprite 5 after SPRITE 5,ON.");

        Assert.AreEqual(9, bus.Vgc.GetSpriteShapeIndex(6),
            "SPRITESHAPE 6,9 must set sprite 6 shape-slot index to 9.");

        Assert.AreEqual(2, bus.Vgc.GetSpriteState(7).priority,
            "SPRITESET 7,6,2 (byte field 6 = priority) must set sprite 7 priority to 2.");

        Assert.AreEqual(300, bus.Vgc.GetSpriteState(8).x,
            "SPRITESET 8,0,300 (word field 0 = X) must set sprite 8 X to 300.");

        var shape4 = bus.Vgc.GetSpriteShape(4);
        const int bytesPerRow = 8;
        for (int i = 0; i < bytesPerRow; i++)
        {
            Assert.AreEqual(i + 1, shape4[bytesPerRow /*row 1*/ + i],
                $"SPRITEDATA 4,1,... must write byte {i + 1} into row-1 byte {i}.");
        }
    }

    // SPRITEX/SPRITEY/COLLISION/BUMPED are reporters: the converted handlers issue
    // the reporter GFN and read the value back out of the LIB_RESULT mailbox. This
    // test proves the reporter path returns the right BASIC value for each.
    [TestMethod]
    public void SpriteReporterFunctionsReadLibResult()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Seed observable sprite + collision state, then read it back via BASIC.
        bus.Vgc.SetSpritePosition(2, 300, 175);
        bus.Vgc.SetCollisionRegisters(0x0007, 0x0005);  // low bytes: SPR=7, BG=5

        EnterProgramLines(cpu, bus, editor,
        [
            "PRINT SPRITEX(2)",   // GFN_SPR_GETX -> 16-bit X = 300
            "PRINT SPRITEY(2)",   // GFN_SPR_GETY -> Y byte = 175
            "PRINT COLLISION(0)", // GFN_SPR_COLL_STATUS -> low byte = 7
            "PRINT BUMPED(0)",    // GFN_SPR_BG_STATUS  -> low byte = 5
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains(" 300", StringComparison.Ordinal),
            $"SPRITEX(2) should read back sprite 2 X=300 from LIB_RESULT.\n{screen}");
        Assert.IsTrue(screen.Contains(" 175", StringComparison.Ordinal),
            $"SPRITEY(2) should read back sprite 2 Y=175 from LIB_RESULT.\n{screen}");
        Assert.IsTrue(screen.Contains(" 7", StringComparison.Ordinal),
            $"COLLISION(0) should read back the sprite collision status byte 7.\n{screen}");
        Assert.IsTrue(screen.Contains(" 5", StringComparison.Ordinal),
            $"BUMPED(0) should read back the sprite-background collision status byte 5.\n{screen}");
    }

    // SPRITE COLLISION ON/CLEAR route through GFN_SPR_COLL_IRQON/CLEAR. ON must
    // enable the VGC sprite-collision IRQ source (and the module clears stale
    // collisions first); CLEAR must zero the latched mask and ack the IRQ source.
    [TestMethod]
    public void SpriteCollisionOnAndClearRouteThroughLibCall()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        QueueLine(editor, "SPRITE COLLISION ON");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        Assert.AreEqual(
            VgcConstants.IrqSpriteCollision,
            (byte)(bus.Vgc.Read(VgcConstants.RegIrqEnable) & VgcConstants.IrqSpriteCollision),
            "SPRITE COLLISION ON must enable the VGC sprite-collision IRQ source.");

        // Latch a collision, then prove SPRITE COLLISION CLEAR zeroes + acks it.
        bus.Vgc.SetCollisionRegisters(0x0102, 0x0000);
        QueueLine(editor, "SPRITE COLLISION CLEAR");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColSt),
            "SPRITE COLLISION CLEAR must clear the collision low latch.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegColStHi),
            "SPRITE COLLISION CLEAR must clear the collision high latch.");
        Assert.AreEqual(0, bus.Vgc.Read(VgcConstants.RegIrqStatus),
            "SPRITE COLLISION CLEAR must acknowledge the pending collision IRQ source.");

        QueueLine(editor, "SPRITE COLLISION OFF");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        Assert.AreEqual(0,
            (byte)(bus.Vgc.Read(VgcConstants.RegIrqEnable) & VgcConstants.IrqSpriteCollision),
            "SPRITE COLLISION OFF must clear the sprite-collision IRQ enable bit.");
    }

    [TestMethod]
    public void Sid1PokeLandsInSidChip()
    {
        // Known structural limitation: BASIC PEEK($D400-$D43F) cannot return
        // SID register values because the EhBASIC ROM has live instructions
        // at those addresses (e.g., LAB_1F45 at $D3FF, LAB_1F48 at $D402).
        // Any mux that routes SID reads to the SID chip clobbers opcode
        // fetches and silently breaks array operations (see
        // ArrayIndexedAssignAndPrintReadsBack).
        //
        // So this test asserts POKE delivers the value to the SID chip
        // shadow directly, without going through BASIC PEEK.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (char ch in "POKE $D400,$37")
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        Assert.AreEqual(0x37, bus.Sid.Read(0xD400),
            "POKE $D400,$37 should set SID1 voice-1 freq-lo register to $37");
    }

    [TestMethod]
    public void MissingAutoboot_DoesNotLeaveFioErrorLatched()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        Assert.AreNotEqual(0x03, bus.Read(0xB9A1),
            "Missing optional AUTOBOOT should not leave FIO_STATUS in error state.");
        Assert.AreEqual(0x00, bus.Read(0xB9A2),
            "Missing optional AUTOBOOT should clear FIO_ERRCODE before Ready.");
    }

    [TestMethod]
    public void RunProgram_HidesCursorWhileProgramIsExecuting()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 PRINT \"RUNNING\"",
            "30 GOTO 30",
        ]);

        QueueLine(editor, "RUN");
        RunUntil(cpu, bus, 50_000_000,
            () => SnapshotScreen(bus.Vgc).StartsWith("RUNNING", StringComparison.Ordinal) &&
                  !bus.Vgc.IsCursorEnabled,
            "BASIC program to be running with the cursor hidden");
    }

    [TestMethod]
    public void InputStatement_ShowsCursorOnlyWhileWaitingForInput()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 INPUT A$",
            "20 PRINT \"AFTER\"",
            "30 END",
        ]);

        QueueLine(editor, "RUN");
        RunUntil(cpu, bus, 50_000_000,
            () => !editor.HasQueuedInput &&
                  SnapshotScreen(bus.Vgc).Contains("?", StringComparison.Ordinal) &&
                  bus.Vgc.IsCursorEnabled,
            "BASIC INPUT to wait with the cursor visible");

        QueueLine(editor, "OK");
        RunUntilEditorIdle(cpu, bus, editor, 50_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("AFTER", StringComparison.Ordinal),
            $"INPUT program should resume after receiving text.\n{screen}");
    }

    [TestMethod]
    public void GetStatement_LeavesCursorHiddenWhilePolling()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 PRINT \"WAIT\"",
            "30 GET A$",
            "40 IF A$=\"\" THEN 30",
            "50 PRINT \"GOT\"",
        ]);

        QueueLine(editor, "RUN");
        RunUntil(cpu, bus, 50_000_000,
            () => !editor.HasQueuedInput &&
                  SnapshotScreen(bus.Vgc).StartsWith("WAIT", StringComparison.Ordinal),
            "BASIC GET polling loop to start");
        RunSteps(cpu, bus, 100_000);

        Assert.IsFalse(bus.Vgc.IsCursorEnabled,
            "BASIC GET is nonblocking polling, so the cursor should remain hidden.");

        editor.QueueInput((byte)'X');
        RunUntilEditorIdle(cpu, bus, editor, 50_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("GOT", StringComparison.Ordinal),
            $"GET program should consume queued input and continue.\n{screen}");
    }

    [TestMethod]
    public void XramStashFetchRoundtripRestoresRam()
    {
        string screen = RunProgram(new[]
        {
            "10 XALLOC 1",
            "20 POKE $2000,$42",
            "30 STASH $2000,0,1",
            "40 POKE $2000,$FF",
            "50 FETCH $2000,0,1",
            "60 PRINT PEEK($2000)",
            "RUN"
        });

        Assert.IsTrue(screen.Contains(" 66"),
            $"After STASH/FETCH roundtrip, $2000 should hold $42=66 but got:\n{screen}");
    }

    [TestMethod]
    public void XramXfreeReusesReleasedAllocationPages()
    {
        string screen = RunProgram(new[]
        {
            "10 XRESET",
            "20 XALLOC 300",
            "30 A=PEEK($BA05)+256*PEEK($BA06)",
            "40 XALLOC 300",
            "50 B=PEEK($BA05)+256*PEEK($BA06)",
            "60 XFREE 0,512",
            "70 XALLOC 300",
            "80 C=PEEK($BA05)+256*PEEK($BA06)",
            "90 IF A=0 AND B=2 AND C=0 THEN PRINT CHR$(79);CHR$(75):END",
            "100 PRINT CHR$(66);CHR$(65);CHR$(68);A;B;C",
            "RUN"
        });

        Assert.IsTrue(screen.Contains("OK", StringComparison.Ordinal),
            $"XFREE should make the first two allocation pages reusable.\n{screen}");
    }

    [TestMethod]
    public void XramStash256ByteRoundtripRestoresAllBytes()
    {
        // Mirrors tests/integration/xram.6502 stash-fetch-roundtrip. STASH and
        // FETCH are now routed through the shared DMA-backed XRAM runtime, so
        // this catches regressions in the BASIC / extension ROM / DMA path.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in new[]
        {
            "10 XALLOC 1",
            "20 FOR I=0 TO 255:POKE $2000+I,I:NEXT",
            "30 STASH $2000,0,256",
            "40 FOR I=0 TO 255:POKE $2000+I,0:NEXT",
            "50 FETCH $2000,0,256",
            "RUN",
        })
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }

        for (int i = 0; i < 256; i++)
        {
            byte got = bus.Read((ushort)(0x2000 + i));
            Assert.AreEqual((byte)i, got,
                $"After STASH/FETCH 256-byte roundtrip, $2000+{i} should be {i:X2} but is {got:X2}");
        }
    }

    [TestMethod]
    public void ReverseFlashCommands_PrintExpectedAvaloniaTextAttributes()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in new[]
        {
            "10 CLS",
            "20 COLOR 2,4",
            "30 LOCATE 0,0",
            "40 REVERSE:PRINT \"R\";",
            "50 REVERSEOFF:PRINT \"N\";",
            "60 REVERSE 1,6:PRINT \"E\";",
            "70 REVERSEOFF:FLASH:PRINT \"F\";",
            "80 FLASHOFF:PRINT \"S\";",
            "90 END",
            "RUN",
        })
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Reverse/flash program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("RNEFS", StringComparison.Ordinal),
            $"Expected styled output at the top-left of the Avalonia text layer.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'R', color: 0x24, attr: 0x00); // default reverse swaps fg/bg
        AssertTextCell(bus.Vgc, 1, 'N', color: 0x42, attr: 0x00); // normal fg/bg
        AssertTextCell(bus.Vgc, 2, 'E', color: 0x61, attr: 0x00); // explicit reverse fg/bg
        AssertTextCell(bus.Vgc, 3, 'F', color: 0x42, attr: VgcConstants.TextAttrFlash);
        AssertTextCell(bus.Vgc, 4, 'S', color: 0x42, attr: 0x00);

        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF/FLASHOFF should leave subsequent BASIC output normal.");
    }

    [TestMethod]
    public void TextReverseCommand_RendersMockInfocomStatusLineInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 COLOR 15,0",
            "30 LOCATE 0,0",
            "40 REVERSE",
            "50 FOR I=1 TO 80:PRINT \" \";:NEXT",
            "60 LOCATE 0,0",
            "70 PRINT \"WEST OF HOUSE\";",
            "80 LOCATE 55,0",
            "90 PRINT \"SCORE: 0 MOVES: 1\";",
            "100 REVERSEOFF",
            "110 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Mock Infocom status line program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("WEST OF HOUSE", StringComparison.Ordinal),
            $"Expected status location at the top-left of the Avalonia text layer.\n{screen}");
        Assert.IsTrue(screen.Split('\n')[0].Contains("SCORE: 0 MOVES: 1", StringComparison.Ordinal),
            $"Expected score/moves field on status row 0.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'W', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 12, 'E', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 13, ' ', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 55, 'S', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 79, ' ', color: 0xF0, attr: 0x00);
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF should leave later output normal.");
    }

    [TestMethod]
    public void TextFlashCommand_ExecutesInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 COLOR 2,4",
            "30 LOCATE 0,0",
            "40 FLASH:PRINT \"F\";",
            "50 FLASHOFF:PRINT \"S\";",
            "60 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Text flash program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("FS", StringComparison.Ordinal),
            $"Expected text flash output at the top-left of the Avalonia text layer.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'F', color: 0x42, attr: VgcConstants.TextAttrFlash);
        AssertTextCell(bus.Vgc, 1, 'S', color: 0x42, attr: 0x00);
    }

    [TestMethod]
    public void GtextReverseCommand_ExecutesInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Vgc.SetFont(SinglePixelAFont());
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 GCLS",
            "20 GCOLOR 5",
            "30 REVERSE 2,6",
            "40 GTEXT 0,0,0,1,\"A\"",
            "50 REVERSEOFF",
            "60 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"GTEXT reverse program should run without syntax errors.\n{screen}");

        Assert.AreEqual(2, bus.Vgc.GetGfxPixelColor(0, 0),
            "GTEXT reverse should draw set glyph pixels with the explicit reverse foreground.");
        Assert.AreEqual(6, bus.Vgc.GetGfxPixelColor(1, 0),
            "GTEXT reverse should fill unset glyph pixels with the explicit reverse background.");
        Assert.AreEqual(6, bus.Vgc.GetGfxPixelColor(7, 7),
            "GTEXT reverse should fill the whole glyph cell background.");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(8, 0),
            "GTEXT reverse should not draw outside the glyph cell.");
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF should leave later text/graphics output normal.");
    }

    [TestMethod]
    public void GraphicsPrimitivesDrawThroughLibCall()
    {
        // PLOT / LINE / RECT / FILL / GCOLOR now route through the GRAPHICS module
        // via lib_call instead of a baked-in vgc.s copy. Confirm they still mutate
        // the real VGC gfx plane with the right colour.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 GCLS",
            "20 GCOLOR 3",
            "30 PLOT 5,5",
            "40 GCOLOR 7",
            "50 LINE 0,10,20,10",
            "60 GCOLOR 9",
            "70 FILL 30,30,33,33",
            "80 GCOLOR 4",
            "90 RECT 40,40,44,44",
            "100 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Graphics primitive program should run without syntax errors.\n{screen}");

        Assert.AreEqual(3, bus.Vgc.GetGfxPixelColor(5, 5),
            "PLOT should set the pixel to the active GCOLOR through the module.");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(6, 5),
            "PLOT must not touch the neighbour pixel.");

        Assert.AreEqual(7, bus.Vgc.GetGfxPixelColor(0, 10),
            "LINE should draw its left endpoint.");
        Assert.AreEqual(7, bus.Vgc.GetGfxPixelColor(10, 10),
            "LINE should draw its midpoint.");
        Assert.AreEqual(7, bus.Vgc.GetGfxPixelColor(20, 10),
            "LINE should draw its right endpoint.");

        Assert.AreEqual(9, bus.Vgc.GetGfxPixelColor(31, 31),
            "FILL should flood the rectangle interior.");
        Assert.AreEqual(9, bus.Vgc.GetGfxPixelColor(33, 33),
            "FILL should reach the rectangle corner.");

        Assert.AreEqual(4, bus.Vgc.GetGfxPixelColor(40, 40),
            "RECT should draw its top-left corner.");
        Assert.AreEqual(4, bus.Vgc.GetGfxPixelColor(44, 44),
            "RECT should draw its bottom-right corner.");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(42, 42),
            "RECT should leave its interior unfilled.");
    }

    [TestMethod]
    public void TextControlKeywordsRouteThroughLibCall()
    {
        // COLOR / FONT / MODE / LOCATE now route through the GRAPHICS module.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // COLOR sets fg/bg/border registers directly (readable after the prompt).
        EnterProgramLines(cpu, bus, editor, ["COLOR 5,2,1"]);
        Assert.AreEqual(5, bus.Read((ushort)VgcConstants.RegFgCol),
            "COLOR should set the foreground colour through the module.");
        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.RegBgCol),
            "COLOR should set the background colour through the module.");
        Assert.AreEqual(1, bus.Read((ushort)VgcConstants.RegBorder),
            "COLOR should set the border colour through the module.");

        // LOCATE positions the text cursor; the BASIC prompt resets the cursor
        // after a direct line, so prove LOCATE by where a PRINT lands in a program.
        EnterProgramLines(cpu, bus, editor,
        [
            "10 LOCATE 10,12:PRINT \"Z\"",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"COLOR/LOCATE should run without syntax errors.\n{screen}");
        Assert.AreEqual((byte)'Z', bus.Vgc.GetScreenChar(10, 12),
            "LOCATE should position the cursor so the next PRINT lands at col 10, row 12.");
    }

    private static string RunProgram(string[] lines)
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in lines)
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        }

        return SnapshotScreen(bus.Vgc);
    }

    private static void EnterProgramLines(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string[] lines)
    {
        foreach (string line in lines)
        {
            QueueLine(editor, line);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        RunUntil(cpu, bus, maxSteps,
            () => SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal),
            $"screen to contain '{marker}'");
    }

    private static void RunUntil(Cpu cpu, CompositeBusDevice bus, int maxSteps, Func<bool> predicate, string description)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && predicate())
                return;
        }
        Assert.Fail($"Timed out waiting for {description}.\n{SnapshotScreen(bus.Vgc)}");
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

    private static void RunUntilEditorIdle(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, int maxSteps)
    {
        bool queueDrained = false;
        bool sawCursorOff = false;
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if (!editor.HasQueuedInput) queueDrained = true;
            if (queueDrained && !bus.Vgc.IsCursorEnabled) sawCursorOff = true;
            if (queueDrained && sawCursorOff && bus.Vgc.IsCursorEnabled) return;
        }
        Assert.Fail($"Timed out waiting for BASIC idle. Cursor={bus.Vgc.IsCursorEnabled} Queued={editor.HasQueuedInput}\n{SnapshotScreen(bus.Vgc)}");
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

    private static void AssertTextCell(VirtualGraphicsController vgc, int col, char ch, byte color, byte attr)
    {
        Assert.AreEqual((byte)ch, vgc.GetScreenChar(col, 0), $"Unexpected char at text cell {col},0.");
        Assert.AreEqual(color, vgc.GetScreenColor(col, 0), $"Unexpected color attr at text cell {col},0.");
        Assert.AreEqual(attr, vgc.GetScreenTextAttr(col, 0), $"Unexpected text attr at text cell {col},0.");
    }

    private static BitmapFont SinglePixelAFont()
    {
        var fontData = new byte[BitmapFont.FontDataSize];
        fontData[(byte)'A' * BitmapFont.GlyphHeight] = 0x80;
        return new BitmapFont(fontData);
    }
}
