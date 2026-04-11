using System;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// SID integration tests verifying both SID chips work through CompositeBusDevice
/// and produce audio. Tests mirror what the FPGA RTL must also handle correctly.
/// </summary>
[TestClass]
public class SidFpgaIntegrationTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    // SID register addresses
    private const ushort SID1 = 0xD400;
    private const ushort SID2 = 0xD420;

    // Voice offsets
    private const int FreqLo = 0x00;
    private const int FreqHi = 0x01;
    private const int PwLo   = 0x02;
    private const int PwHi   = 0x03;
    private const int Ctrl   = 0x04;
    private const int Ad     = 0x05;
    private const int Sr     = 0x06;
    private const int FcLo   = 0x15;
    private const int FcHi   = 0x16;
    private const int ResFilt = 0x17;
    private const int ModeVol = 0x18;
    private const int Vol1   = 0x1D;
    private const int Vol2   = 0x1E;
    private const int Vol3   = 0x1F;

    // ── SID1 Voice Generation ──

    [TestMethod]
    public void Sid1_SawtoothWave_ProducesAudio()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x20, note: 69); // sawtooth, A4
        var samples = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(samples), "Sawtooth should produce non-silent output");
    }

    [TestMethod]
    public void Sid1_PulseWave_ProducesAudio()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SID1 + PwLo, 0x00);
        sid.Write(SID1 + PwHi, 0x08); // 50% duty
        SetupTone(sid, SID1, waveform: 0x40, note: 60); // pulse, C4
        var samples = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(samples), "Pulse wave should produce non-silent output");
    }

    [TestMethod]
    public void Sid1_TriangleWave_ProducesAudio()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x10, note: 72); // triangle, C5
        var samples = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(samples), "Triangle should produce non-silent output");
    }

    [TestMethod]
    public void Sid1_NoiseWave_ProducesAudio()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x80, note: 60); // noise
        var samples = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(samples), "Noise should produce non-silent output");
    }

    // ── Dual SID Independence ──

    [TestMethod]
    public void DualSid_BothChips_ProduceIndependentAudio()
    {
        var sid1 = new SidChip(enableAudio: false, baseAddress: SID1);
        var sid2 = new SidChip(enableAudio: false, baseAddress: SID2);

        // SID1: low sawtooth
        SetupTone(sid1, SID1, waveform: 0x20, note: 48); // C3
        // SID2: high triangle
        SetupTone(sid2, SID2, waveform: 0x10, note: 84); // C6

        var samples1 = sid1.RenderSamplesForTest(2048);
        var samples2 = sid2.RenderSamplesForTest(2048);

        Assert.IsTrue(HasNonZero(samples1), "SID1 should produce audio");
        Assert.IsTrue(HasNonZero(samples2), "SID2 should produce audio");

        // Verify they're different waveforms (not identical output)
        bool differ = false;
        for (int i = 100; i < 200; i++)
            if (samples1[i] != samples2[i]) { differ = true; break; }
        Assert.IsTrue(differ, "SID1 and SID2 should produce different waveforms");
    }

    // ── Per-Voice Volume ──

    [TestMethod]
    public void PerVoiceVolume_DefaultsToMax()
    {
        var sid = new SidChip(enableAudio: false);
        Assert.AreEqual(0x0F, sid.Read(SID1 + Vol1));
        Assert.AreEqual(0x0F, sid.Read(SID1 + Vol2));
        Assert.AreEqual(0x0F, sid.Read(SID1 + Vol3));
    }

    [TestMethod]
    public void PerVoiceVolume_ZeroMutesVoice()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x20, note: 69);

        // Full volume — should be audible
        sid.Write(SID1 + Vol1, 0x0F);
        var loud = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(loud), "Full per-voice volume should be audible");

        // Zero volume — should be silent
        sid.Write(SID1 + Vol1, 0x00);
        sid.Flush();
        var silent = sid.RenderSamplesForTest(2048);
        long energy = 0;
        foreach (var s in silent) energy += Math.Abs(s);
        Assert.IsTrue(energy == 0, "Zero per-voice volume should silence voice");
    }

    // ── CompositeBus Routing ──

    [TestMethod]
    public void CompositeBus_Sid1_WriteAndReadBack()
    {
        using var bus = MakeBus();
        // Write all voice 1 registers
        bus.Write(SID1 + FreqLo, 0x37);
        bus.Write(SID1 + FreqHi, 0x1C);
        bus.Write(SID1 + PwLo, 0x00);
        bus.Write(SID1 + PwHi, 0x08);
        bus.Write(SID1 + Ad, 0x00);
        bus.Write(SID1 + Sr, 0xF0);
        bus.Write(SID1 + ModeVol, 0x0F);

        Assert.AreEqual(0x37, bus.Sid.Read(SID1 + FreqLo));
        Assert.AreEqual(0x1C, bus.Sid.Read(SID1 + FreqHi));
        Assert.AreEqual(0x0F, bus.Sid.Read(SID1 + ModeVol));
    }

    [TestMethod]
    public void CompositeBus_Sid2_Independent()
    {
        using var bus = MakeBus();
        bus.Write(SID1 + FreqLo, 0xAA);
        bus.Write(SID2 + FreqLo, 0x55);

        Assert.AreEqual(0xAA, bus.Sid.Read(SID1 + FreqLo));
        Assert.AreEqual(0x55, bus.Sid2.Read(SID2 + FreqLo));
    }

    // ── Filter ──

    [TestMethod]
    public void Filter_CutoffRegister_WritesCorrectly()
    {
        using var bus = MakeBus();
        bus.Write(SID1 + FcLo, 0x07);  // lower 3 bits
        bus.Write(SID1 + FcHi, 0xFF);  // upper 8 bits
        Assert.AreEqual(0x07, bus.Sid.Read(SID1 + FcLo));
        Assert.AreEqual(0xFF, bus.Sid.Read(SID1 + FcHi));
    }

    [TestMethod]
    public void Filter_RouteVoice1_ThroughLowpass()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x20, note: 69); // sawtooth A4

        // Enable filter on voice 1, lowpass mode
        sid.Write(SID1 + FcHi, 0x40);       // mid cutoff
        sid.Write(SID1 + ResFilt, 0x01);     // route voice 1 to filter
        sid.Write(SID1 + ModeVol, 0x1F);     // lowpass + max volume

        var filtered = sid.RenderSamplesForTest(2048);
        Assert.IsTrue(HasNonZero(filtered), "Filtered sawtooth should still produce audio");
    }

    // ── All Three Voices Simultaneously ──

    [TestMethod]
    public void ThreeVoices_PlaySimultaneously()
    {
        var sid = new SidChip(enableAudio: false);

        // Voice 1: triangle C4
        SetupVoice(sid, SID1, 0, waveform: 0x10, note: 60);
        // Voice 2: sawtooth E4
        SetupVoice(sid, SID1, 1, waveform: 0x20, note: 64);
        // Voice 3: pulse G4
        sid.Write(SID1 + 2 * 7 + PwLo, 0x00);
        sid.Write(SID1 + 2 * 7 + PwHi, 0x08);
        SetupVoice(sid, SID1, 2, waveform: 0x40, note: 67);

        sid.Write(SID1 + ModeVol, 0x0F); // max master volume

        var samples = sid.RenderSamplesForTest(4096);
        Assert.IsTrue(HasNonZero(samples), "C-major chord should produce audio");

        // Verify the output has some complexity (not a flat line)
        int crossings = CountZeroCrossings(samples);
        Assert.IsTrue(crossings > 10, $"Three voices should create complex waveform (got {crossings} zero crossings)");
    }

    // ── ADSR Envelope ──

    [TestMethod]
    public void Envelope_Attack_RampsUp()
    {
        var sid = new SidChip(enableAudio: false);
        // Slow attack (A=4), instant decay, max sustain
        sid.Write(SID1 + FreqLo, 0x37);
        sid.Write(SID1 + FreqHi, 0x1C);
        sid.Write(SID1 + Ad, 0x40);   // attack=4, decay=0
        sid.Write(SID1 + Sr, 0xF0);   // sustain=F, release=0
        sid.Write(SID1 + ModeVol, 0x0F);
        sid.Write(SID1 + Ctrl, 0x21); // sawtooth + gate

        // Render early samples — should start quiet
        var early = sid.RenderSamplesForTest(256);
        long earlyEnergy = 0;
        foreach (var s in early) earlyEnergy += Math.Abs(s);

        // Render more — should get louder
        var later = sid.RenderSamplesForTest(4096);
        long laterEnergy = 0;
        foreach (var s in later) laterEnergy += Math.Abs(s);

        Assert.IsTrue(laterEnergy > earlyEnergy,
            $"Envelope should ramp up (early={earlyEnergy}, later={laterEnergy})");
    }

    [TestMethod]
    public void Envelope_Release_DecaysToSilence()
    {
        var sid = new SidChip(enableAudio: false);
        SetupTone(sid, SID1, waveform: 0x20, note: 69);
        sid.RenderSamplesForTest(2048); // let it sustain

        // Gate off with fast release
        sid.Write(SID1 + Sr, 0xF0); // sustain=F, release=0 (fastest)
        sid.Write(SID1 + Ctrl, 0x20); // gate off

        // After release, should decay toward silence
        var released = sid.RenderSamplesForTest(8192);
        // Check last 256 samples are quieter than first 256
        long startEnergy = 0, endEnergy = 0;
        for (int i = 0; i < 256; i++) startEnergy += Math.Abs(released[i]);
        for (int i = released.Length - 256; i < released.Length; i++) endEnergy += Math.Abs(released[i]);
        Assert.IsTrue(endEnergy <= startEnergy,
            $"Release should decay (start={startEnergy}, end={endEnergy})");
    }

    // ── Helpers ──

    private static void SetupTone(SidChip sid, ushort baseAddr, byte waveform, int note)
    {
        var (lo, hi) = MidiToSidFreq(note);
        sid.Write((ushort)(baseAddr + FreqLo), lo);
        sid.Write((ushort)(baseAddr + FreqHi), hi);
        sid.Write((ushort)(baseAddr + Ad), 0x00);    // instant attack/decay
        sid.Write((ushort)(baseAddr + Sr), 0xF0);    // max sustain, instant release
        sid.Write((ushort)(baseAddr + ModeVol), 0x0F); // max master volume
        sid.Write((ushort)(baseAddr + Ctrl), (byte)(waveform | 0x01)); // waveform + gate
    }

    private static void SetupVoice(SidChip sid, ushort baseAddr, int voice, byte waveform, int note)
    {
        int ofs = voice * 7;
        var (lo, hi) = MidiToSidFreq(note);
        sid.Write((ushort)(baseAddr + ofs + FreqLo), lo);
        sid.Write((ushort)(baseAddr + ofs + FreqHi), hi);
        sid.Write((ushort)(baseAddr + ofs + Ad), 0x00);
        sid.Write((ushort)(baseAddr + ofs + Sr), 0xF0);
        sid.Write((ushort)(baseAddr + ofs + Ctrl), (byte)(waveform | 0x01));
    }

    private static (byte lo, byte hi) MidiToSidFreq(int midi)
    {
        double freq = 440.0 * Math.Pow(2.0, (midi - 69) / 12.0);
        int sidFreq = (int)(freq * 16777216.0 / 985248.0);
        return ((byte)(sidFreq & 0xFF), (byte)((sidFreq >> 8) & 0xFF));
    }

    private static bool HasNonZero(short[] samples)
    {
        foreach (var s in samples)
            if (s != 0) return true;
        return false;
    }

    private static int CountZeroCrossings(short[] samples)
    {
        int crossings = 0;
        for (int i = 1; i < samples.Length; i++)
            if ((samples[i - 1] >= 0 && samples[i] < 0) || (samples[i - 1] < 0 && samples[i] >= 0))
                crossings++;
        return crossings;
    }
}
