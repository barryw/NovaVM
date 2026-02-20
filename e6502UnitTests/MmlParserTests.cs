using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MmlParserTests
{
    [TestMethod]
    public void Parse_SingleNote_DefaultQuarter()
    {
        var events = MmlParser.Parse("C");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(60, events[0].Param1);  // C4 = MIDI 60
        Assert.AreEqual(96, events[0].Param2);  // quarter note = 96 ticks
    }

    [TestMethod]
    public void Parse_NoteWithSharp()
    {
        var events = MmlParser.Parse("C#");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(61, events[0].Param1);  // C#4 = MIDI 61
    }

    [TestMethod]
    public void Parse_NoteWithSharpPlus()
    {
        var events = MmlParser.Parse("C+");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(61, events[0].Param1);
    }

    [TestMethod]
    public void Parse_NoteWithFlat()
    {
        // B- in octave 4: B=11, flat -> 10, MIDI = 5*12+10 = 70
        var events = MmlParser.Parse("B-");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(70, events[0].Param1);
    }

    [TestMethod]
    public void Parse_NoteWithExplicitLength()
    {
        var events = MmlParser.Parse("C8");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(48, events[0].Param2);  // eighth = 48 ticks
    }

    [TestMethod]
    public void Parse_NoteWhole()
    {
        var events = MmlParser.Parse("C1");
        Assert.AreEqual(384, events[0].Param2);
    }

    [TestMethod]
    public void Parse_NoteHalf()
    {
        var events = MmlParser.Parse("C2");
        Assert.AreEqual(192, events[0].Param2);
    }

    [TestMethod]
    public void Parse_NoteSixteenth()
    {
        var events = MmlParser.Parse("C16");
        Assert.AreEqual(24, events[0].Param2);
    }

    [TestMethod]
    public void Parse_DottedNote()
    {
        var events = MmlParser.Parse("C4.");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(144, events[0].Param2);  // 96 * 1.5 = 144 ticks
    }

    [TestMethod]
    public void Parse_DottedEighth()
    {
        var events = MmlParser.Parse("C8.");
        Assert.AreEqual(72, events[0].Param2);  // 48 * 1.5 = 72
    }

    [TestMethod]
    public void Parse_Rest()
    {
        var events = MmlParser.Parse("R4");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.Rest, events[0].Type);
        Assert.AreEqual(96, events[0].Param2);
    }

    [TestMethod]
    public void Parse_RestDefaultDuration()
    {
        var events = MmlParser.Parse("R");
        Assert.AreEqual(MmlEventType.Rest, events[0].Type);
        Assert.AreEqual(96, events[0].Param2);  // default L4
    }

    [TestMethod]
    public void Parse_MultipleNotes()
    {
        var events = MmlParser.Parse("C D E");
        Assert.AreEqual(3, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[2].Type);
        Assert.AreEqual(60, events[0].Param1); // C4
        Assert.AreEqual(62, events[1].Param1); // D4
        Assert.AreEqual(64, events[2].Param1); // E4
    }

    [TestMethod]
    public void Parse_OctaveCommand()
    {
        var events = MmlParser.Parse("O5 C");
        Assert.AreEqual(2, events.Count);
        Assert.AreEqual(MmlEventType.SetOctave, events[0].Type);
        Assert.AreEqual(5, events[0].Param1);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
        Assert.AreEqual(72, events[1].Param1);  // C5 = MIDI 72
    }

    [TestMethod]
    public void Parse_OctaveUpDown()
    {
        var events = MmlParser.Parse("C > C < C");
        // Events: NoteOn(C4), OctaveUp, NoteOn(C5), OctaveDown, NoteOn(C4)
        Assert.AreEqual(5, events.Count);
        Assert.AreEqual(60, events[0].Param1);
        Assert.AreEqual(MmlEventType.OctaveUp, events[1].Type);
        Assert.AreEqual(72, events[2].Param1);
        Assert.AreEqual(MmlEventType.OctaveDown, events[3].Type);
        Assert.AreEqual(60, events[4].Param1);
    }

    [TestMethod]
    public void Parse_DefaultLength()
    {
        var events = MmlParser.Parse("L8 C D");
        Assert.AreEqual(3, events.Count);
        Assert.AreEqual(MmlEventType.SetDefaultLen, events[0].Type);
        Assert.AreEqual(8, events[0].Param1);
        Assert.AreEqual(48, events[1].Param2);
        Assert.AreEqual(48, events[2].Param2);
    }

    [TestMethod]
    public void Parse_Tempo()
    {
        var events = MmlParser.Parse("T120 C");
        Assert.AreEqual(2, events.Count);
        Assert.AreEqual(MmlEventType.SetTempo, events[0].Type);
        Assert.AreEqual(120, events[0].Param1);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
    }

    [TestMethod]
    public void Parse_InstrumentSwitch()
    {
        var events = MmlParser.Parse("I3 C");
        Assert.AreEqual(2, events.Count);
        Assert.AreEqual(MmlEventType.SetInstrument, events[0].Type);
        Assert.AreEqual(3, events[0].Param1);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
    }

    [TestMethod]
    public void Parse_BarLine_Ignored()
    {
        var events = MmlParser.Parse("C D | E F");
        Assert.AreEqual(4, events.Count);
        foreach (var e in events)
            Assert.AreEqual(MmlEventType.NoteOn, e.Type);
    }

    [TestMethod]
    public void Parse_Tie()
    {
        // C4 (96) tied to C8 (48) = 144 ticks, single NoteOn
        var events = MmlParser.Parse("C4&C8");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(60, events[0].Param1);
        Assert.AreEqual(144, events[0].Param2);
    }

    [TestMethod]
    public void Parse_TieSimple()
    {
        // Tie without repeated note letter
        var events = MmlParser.Parse("C4&8");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(144, events[0].Param2);
    }

    [TestMethod]
    public void Parse_MultipleTies()
    {
        var events = MmlParser.Parse("C4&4&4");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(288, events[0].Param2);  // 96+96+96
    }

    [TestMethod]
    public void Parse_Arpeggio()
    {
        var events = MmlParser.Parse("{CEG}4");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.ArpStart, events[0].Type);
        Assert.AreEqual(3, events[0].Param1);
        Assert.AreEqual(96, events[0].Param2);
        CollectionAssert.AreEqual(new[] { 60, 64, 67 }, events[0].ArpNotes);
    }

    [TestMethod]
    public void Parse_ArpeggioWithSharp()
    {
        var events = MmlParser.Parse("{C#EG}4");
        Assert.AreEqual(1, events.Count);
        Assert.IsNotNull(events[0].ArpNotes);
        Assert.AreEqual(61, events[0].ArpNotes![0]);  // C#4
    }

    [TestMethod]
    public void Parse_Loop()
    {
        var events = MmlParser.Parse("[C D]2");
        // Expect: LoopStart(2), NoteOn(C), NoteOn(D), NoteOn(C), NoteOn(D), LoopEnd(2)
        Assert.AreEqual(6, events.Count);
        Assert.AreEqual(MmlEventType.LoopStart, events[0].Type);
        Assert.AreEqual(2, events[0].Param1);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[2].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[3].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[4].Type);
        Assert.AreEqual(MmlEventType.LoopEnd, events[5].Type);
        Assert.AreEqual(2, events[5].Param1);
    }

    [TestMethod]
    public void Parse_LoopThreeTimes()
    {
        var events = MmlParser.Parse("[C]3");
        Assert.AreEqual(MmlEventType.LoopStart, events[0].Type);
        Assert.AreEqual(3, events[0].Param1);
        Assert.AreEqual(3, events.Count(e => e.Type == MmlEventType.NoteOn));
        Assert.AreEqual(MmlEventType.LoopEnd, events[^1].Type);
    }

    [TestMethod]
    public void Parse_PulseWidth()
    {
        var events = MmlParser.Parse("@P2048");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.SetPulseWidth, events[0].Type);
        Assert.AreEqual(2048, events[0].Param1);
    }

    [TestMethod]
    public void Parse_PwmSweepUp()
    {
        var events = MmlParser.Parse("@PS+");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.PwmSweep, events[0].Type);
        Assert.AreEqual(1, events[0].Param1);
    }

    [TestMethod]
    public void Parse_PwmSweepDown()
    {
        var events = MmlParser.Parse("@PS-");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.PwmSweep, events[0].Type);
        Assert.AreEqual(-1, events[0].Param1);
    }

    [TestMethod]
    public void Parse_PwmSweepStop()
    {
        var events = MmlParser.Parse("@PS0");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.PwmSweep, events[0].Type);
        Assert.AreEqual(0, events[0].Param1);
    }

    [TestMethod]
    public void Parse_Vibrato()
    {
        var events = MmlParser.Parse("~6 C");
        Assert.AreEqual(2, events.Count);
        Assert.AreEqual(MmlEventType.SetVibrato, events[0].Type);
        Assert.AreEqual(6, events[0].Param1);
        Assert.AreEqual(MmlEventType.NoteOn, events[1].Type);
    }

    [TestMethod]
    public void Parse_VibratoOff()
    {
        var events = MmlParser.Parse("~0");
        Assert.AreEqual(MmlEventType.SetVibrato, events[0].Type);
        Assert.AreEqual(0, events[0].Param1);
    }

    [TestMethod]
    public void Parse_Portamento()
    {
        var events = MmlParser.Parse("C /E");
        // NoteOn(C), Portamento, NoteOn(E)
        Assert.AreEqual(3, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(60, events[0].Param1);
        Assert.AreEqual(MmlEventType.Portamento, events[1].Type);
        Assert.AreEqual(MmlEventType.NoteOn, events[2].Type);
        Assert.AreEqual(64, events[2].Param1);
    }

    [TestMethod]
    public void Parse_FilterCutoff()
    {
        var events = MmlParser.Parse("@F1024,8");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.SetFilterCutoff, events[0].Type);
        Assert.AreEqual(1024, events[0].Param1);
        Assert.AreEqual(8, events[0].Param2);
    }

    [TestMethod]
    public void Parse_FilterModeLowpass()
    {
        var events = MmlParser.Parse("@FL");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.FilterMode, events[0].Type);
        Assert.AreEqual(1, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterModeBandpass()
    {
        var events = MmlParser.Parse("@FB");
        Assert.AreEqual(MmlEventType.FilterMode, events[0].Type);
        Assert.AreEqual(2, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterModeHighpass()
    {
        var events = MmlParser.Parse("@FH");
        Assert.AreEqual(MmlEventType.FilterMode, events[0].Type);
        Assert.AreEqual(4, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterModeOff()
    {
        var events = MmlParser.Parse("@FO");
        Assert.AreEqual(MmlEventType.FilterMode, events[0].Type);
        Assert.AreEqual(0, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterSweepUp()
    {
        var events = MmlParser.Parse("@FS+");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.FilterSweep, events[0].Type);
        Assert.AreEqual(1, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterSweepDown()
    {
        var events = MmlParser.Parse("@FS-");
        Assert.AreEqual(MmlEventType.FilterSweep, events[0].Type);
        Assert.AreEqual(-1, events[0].Param1);
    }

    [TestMethod]
    public void Parse_FilterSweepStop()
    {
        var events = MmlParser.Parse("@FS0");
        Assert.AreEqual(MmlEventType.FilterSweep, events[0].Type);
        Assert.AreEqual(0, events[0].Param1);
    }

    [TestMethod]
    public void Parse_CaseInsensitive()
    {
        var upper = MmlParser.Parse("C4");
        var lower = MmlParser.Parse("c4");
        Assert.AreEqual(upper.Count, lower.Count);
        Assert.AreEqual(upper[0].Param1, lower[0].Param1);
        Assert.AreEqual(upper[0].Param2, lower[0].Param2);
    }

    [TestMethod]
    public void Parse_UnknownChars_Skipped()
    {
        // Should not throw, should produce 1 note event
        var events = MmlParser.Parse("?C?");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
    }

    [TestMethod]
    public void Parse_Empty()
    {
        var events = MmlParser.Parse("");
        Assert.AreEqual(0, events.Count);
    }

    [TestMethod]
    public void Parse_AllNotes_Octave4()
    {
        // Verify all diatonic MIDI values in octave 4
        // C=60 D=62 E=64 F=65 G=67 A=69 B=71
        var events = MmlParser.Parse("C D E F G A B");
        Assert.AreEqual(7, events.Count);
        Assert.AreEqual(60, events[0].Param1); // C
        Assert.AreEqual(62, events[1].Param1); // D
        Assert.AreEqual(64, events[2].Param1); // E
        Assert.AreEqual(65, events[3].Param1); // F
        Assert.AreEqual(67, events[4].Param1); // G
        Assert.AreEqual(69, events[5].Param1); // A
        Assert.AreEqual(71, events[6].Param1); // B
    }

    [TestMethod]
    public void Parse_OctaveClampHigh()
    {
        var events = MmlParser.Parse("O7 > C");
        // OctaveUp from 7 should stay at 7
        var note = events.First(e => e.Type == MmlEventType.NoteOn);
        Assert.AreEqual((7 + 1) * 12, note.Param1); // C7 = MIDI 96
    }

    [TestMethod]
    public void Parse_OctaveClampLow()
    {
        var events = MmlParser.Parse("O1 < C");
        var note = events.First(e => e.Type == MmlEventType.NoteOn);
        Assert.AreEqual((1 + 1) * 12, note.Param1); // C1 = MIDI 24
    }

    [TestMethod]
    public void Parse_ComplexSequence()
    {
        // Realistic MML: set tempo, instrument, play scale
        var events = MmlParser.Parse("T140 I2 L8 C D E F G");
        Assert.AreEqual(8, events.Count);
        Assert.AreEqual(MmlEventType.SetTempo, events[0].Type);
        Assert.AreEqual(140, events[0].Param1);
        Assert.AreEqual(MmlEventType.SetInstrument, events[1].Type);
        Assert.AreEqual(MmlEventType.SetDefaultLen, events[2].Type);
        for (int i = 3; i < 8; i++)
        {
            Assert.AreEqual(MmlEventType.NoteOn, events[i].Type);
            Assert.AreEqual(48, events[i].Param2);
        }
    }
}
