using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace e6502UnitTests;

[TestClass]
public class MidiEngineRoutingTests
{
    private static MidiEngine.ChannelInfo[] MakeAnalysis(params (int ch, int notes, int program)[] channels)
    {
        var analysis = new MidiEngine.ChannelInfo[16];
        for (int i = 0; i < 16; i++)
            analysis[i] = new MidiEngine.ChannelInfo { Channel = i };
        foreach (var (ch, notes, program) in channels)
        {
            analysis[ch].NoteCount = notes;
            analysis[ch].GmProgram = program;
        }
        return analysis;
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_VoiceGetsCorrectChannel()
    {
        var analysis = MakeAnalysis((0, 100, 0), (1, 80, 40), (9, 50, 0));
        var rawChannels = new[] { 0, 1, 9 };
        var map = new Dictionary<int, int> { [6] = 0, [7] = 1, [0] = 9 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false, (_, _) => { }, map);

        Assert.AreEqual(0, result.VoiceToChannel[6]);
        Assert.AreEqual(1, result.VoiceToChannel[7]);
        Assert.AreEqual(9, result.VoiceToChannel[0]);
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_WtsVoiceGetsGmProgram()
    {
        var analysis = MakeAnalysis((0, 100, 42));
        var rawChannels = new[] { 0 };
        var map = new Dictionary<int, int> { [6] = 0 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false, (_, _) => { }, map);

        Assert.AreEqual(42, result.InstrumentSlots[6]);
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_SidVoiceGetsBucket()
    {
        var analysis = MakeAnalysis((0, 100, 0));
        var rawChannels = new[] { 0 };
        var map = new Dictionary<int, int> { [0] = 0 };
        int definedSlot = -1;

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (slot, _) => { definedSlot = slot; }, map);

        Assert.AreEqual(0, result.VoiceToChannel[0]);
        Assert.IsTrue(result.InstrumentSlots[0] > 0);
        Assert.IsTrue(definedSlot > 0);
    }

    [TestMethod]
    public void RouteVoices_NoExplicitMapping_AutoRoutesAsDefault()
    {
        var analysis = MakeAnalysis((0, 100, 0), (1, 80, 40));
        var rawChannels = new[] { 0, 1 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false, (_, _) => { }, null);

        Assert.AreEqual(0, result.VoiceToChannel[6]);
        Assert.AreEqual(1, result.VoiceToChannel[7]);
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_DrumOnWtsGetsBankEncoding()
    {
        var analysis = MakeAnalysis((9, 100, 0));
        var rawChannels = new[] { 9 };
        var map = new Dictionary<int, int> { [6] = 9 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false, (_, _) => { }, map);

        Assert.AreEqual(32768, result.InstrumentSlots[6]);
    }
}
