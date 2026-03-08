using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class WavetableSynthTests
{
    private static SampleBank MakeSineBank()
    {
        int len = 512;
        var data = new float[len];
        for (int i = 0; i < len; i++)
            data[i] = MathF.Sin(2f * MathF.PI * i / len);

        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Sine",
            MidiProgram = 0,
            Regions = { new SampleRegion
            {
                KeyRangeLo = 0, KeyRangeHi = 127,
                VelocityRangeLo = 0, VelocityRangeHi = 127,
                SampleData = data,
                SampleRate = 44100,
                RootKey = 60,
                LoopStart = 0, LoopEnd = len - 1, LoopEnabled = true,
                AttackTime = 0f, DecayTime = 0f,
                SustainLevel = 1f, ReleaseTime = 0.01f
            }}
        });
        return bank;
    }

    [TestMethod]
    public void NoteOn_ProducesNonSilentOutput()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());
        synth.NoteOn(0, 60, 127, 0);
        short[] samples = synth.RenderSamples(1024);
        Assert.AreEqual(2048, samples.Length);
        Assert.IsTrue(samples.Any(s => Math.Abs(s) > 100));
    }

    [TestMethod]
    public void NoteOff_EventuallyProducesSilence()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.ReverbLevel = 0;
        synth.ChorusLevel = 0;
        synth.LoadBank(MakeSineBank());
        synth.NoteOn(0, 60, 127, 0);
        synth.RenderSamples(512);
        synth.NoteOff(0);
        short[] tail = synth.RenderSamples(2048);
        bool silent = tail.Skip(tail.Length - 200).All(s => Math.Abs(s) < 50);
        Assert.IsTrue(silent);
    }

    [TestMethod]
    public void Volume_ScalesOutput()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.SetVolume(0, 255);
        synth.NoteOn(0, 60, 127, 0);
        short[] loud = synth.RenderSamples(512);
        synth.NoteOff(0);
        synth.RenderSamples(4096);

        synth.SetVolume(0, 64);
        synth.NoteOn(0, 60, 127, 0);
        short[] quiet = synth.RenderSamples(512);

        double peakLoud = loud.Max(s => Math.Abs((int)s));
        double peakQuiet = quiet.Max(s => Math.Abs((int)s));
        Assert.IsTrue(peakQuiet < peakLoud * 0.5);
    }

    [TestMethod]
    public void Panning_AffectsStereoBalance()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());
        synth.SetPanning(0, 0);  // hard left
        synth.NoteOn(0, 60, 127, 0);
        short[] panned = synth.RenderSamples(512);

        double leftPeak = 0, rightPeak = 0;
        for (int i = 0; i < panned.Length; i += 2)
        {
            leftPeak = Math.Max(leftPeak, Math.Abs(panned[i]));
            rightPeak = Math.Max(rightPeak, Math.Abs(panned[i + 1]));
        }
        Assert.IsTrue(leftPeak > rightPeak * 3);
    }

    [TestMethod]
    public void NoBank_NoteOn_ProducesSilence()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.NoteOn(0, 60, 127, 0);
        short[] samples = synth.RenderSamples(512);
        Assert.IsTrue(samples.All(s => s == 0));
    }

    [TestMethod]
    public void ActiveVoiceMask_ReflectsPlayingVoices()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());
        Assert.AreEqual(0, synth.ActiveVoiceMask);
        synth.NoteOn(0, 60, 127, 0);
        synth.NoteOn(3, 72, 100, 0);
        Assert.AreEqual(0b00001001, synth.ActiveVoiceMask);
    }

    [TestMethod]
    public void PitchBend_ChangesFrequency()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.NoteOn(0, 60, 127, 0);
        short[] normal = synth.RenderSamples(4096);
        synth.NoteOff(0);
        synth.RenderSamples(4096);

        synth.SetPitchBend(0, 0xC000);
        synth.NoteOn(0, 60, 127, 0);
        short[] bent = synth.RenderSamples(4096);

        int crossNormal = CountZeroCrossings(normal);
        int crossBent = CountZeroCrossings(bent);
        Assert.IsTrue(crossBent > crossNormal);
    }

    [TestMethod]
    public void Synth_WithAudioDisabled_StillRendersStereo()
    {
        var synth = new WavetableSynth(enableAudio: false);
        short[] samples = synth.RenderSamples(100);
        Assert.AreEqual(200, samples.Length);
    }

    private static int CountZeroCrossings(short[] stereo)
    {
        int count = 0;
        for (int i = 2; i < stereo.Length; i += 2)
            if ((stereo[i - 2] >= 0) != (stereo[i] >= 0))
                count++;
        return count;
    }
}
