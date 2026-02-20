using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MusicEngineTests
{
    private const ushort SidBase = 0xD400;

    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    // -------------------------------------------------------------------------
    // Instrument tests
    // -------------------------------------------------------------------------

    [TestMethod]
    public void DefineInstrument_StoresInTable()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.DefineInstrument(3, 0x20, 2, 4, 8, 3);
        SidInstrument inst = engine.GetInstrument(3);

        Assert.AreEqual(0x20, inst.Waveform);
        Assert.AreEqual(2,    inst.Attack);
        Assert.AreEqual(4,    inst.Decay);
        Assert.AreEqual(8,    inst.Sustain);
        Assert.AreEqual(3,    inst.Release);
    }

    [TestMethod]
    public void DefaultInstrument_IsPulseBeep()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        SidInstrument inst = engine.GetInstrument(0);

        Assert.AreEqual(0x40, inst.Waveform); // pulse
        Assert.AreEqual(0,    inst.Attack);
        Assert.AreEqual(9,    inst.Decay);
        Assert.AreEqual(0,    inst.Sustain);
        Assert.AreEqual(6,    inst.Release);
    }

    // -------------------------------------------------------------------------
    // SFX tests
    // -------------------------------------------------------------------------

    [TestMethod]
    public void PlaySound_WritesGateAndWaveformToSid()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Instrument 0 is pulse (0x40), gate should be set (bit 0)
        engine.PlaySound(69, 30); // A4, 30 frames

        // Control register for voice 0 = SidBase + 4
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0x41, ctrl, "Expected pulse waveform (0x40) | gate (0x01)");
    }

    [TestMethod]
    public void PlaySound_FrequencyCorrectForA440()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(69, 10); // MIDI 69 = A4 = 440 Hz

        byte lo = bus.Sid.Read(SidBase + 0);
        byte hi = bus.Sid.Read(SidBase + 1);
        int  freq = lo | (hi << 8);

        // Expected: 440 * 16777216 / 985248 ≈ 7482 (PAL)
        // Allow ±10 for rounding
        Assert.IsTrue(Math.Abs(freq - 7482) <= 10, $"Expected ~7482, got {freq}");
    }

    [TestMethod]
    public void PlaySound_GatesOffAfterDuration()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(69, 2); // 2 frames

        engine.Tick(); // frame 1 — gate still on
        byte ctrlAfterTick1 = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(1, ctrlAfterTick1 & 0x01, "Gate should still be on after 1 tick");

        engine.Tick(); // frame 2 — gate off
        byte ctrlAfterTick2 = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0, ctrlAfterTick2 & 0x01, "Gate should be off after duration expires");
    }

    [TestMethod]
    public void IsPlaying_TrueWhileSfxActive()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(60, 5);
        Assert.IsTrue(engine.IsPlaying, "IsPlaying should be true immediately after PlaySound");

        // Tick until done
        for (int i = 0; i < 5; i++)
            engine.Tick();

        Assert.IsFalse(engine.IsPlaying, "IsPlaying should be false after duration expires");
    }

    [TestMethod]
    public void SfxCompleted_SetWhenSfxEnds()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(60, 1);
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted should not be set before expiry");

        engine.Tick(); // expires here

        Assert.IsTrue(engine.SfxCompleted, "SfxCompleted should be true after SFX expires");
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted is one-shot — should clear on read");
    }

    // -------------------------------------------------------------------------
    // Volume
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SetVolume_WritesMasterVolume()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetVolume(12);

        byte vol = bus.Sid.Read(SidBase + 0x18);
        Assert.AreEqual(12, vol & 0x0F, "Volume bits 0-3 should be 12");
    }

    [TestMethod]
    public void SetVolume_PreservesFilterMode()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Pre-set filter mode bits (bits 4-7) to 0x50
        bus.Sid.Write(SidBase + 0x18, 0x50);

        engine.SetVolume(9);

        byte vol = bus.Sid.Read(SidBase + 0x18);
        Assert.AreEqual(0x50, vol & 0xF0, "Filter mode bits 4-7 should be preserved");
        Assert.AreEqual(9,    vol & 0x0F, "Volume bits 0-3 should be updated");
    }

    // -------------------------------------------------------------------------
    // Music basic
    // -------------------------------------------------------------------------

    [TestMethod]
    public void MusicPlay_WritesFirstNoteToSid()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // C4 = MIDI 60, quarter note
        engine.SetSequence(0, "C4");
        engine.MusicPlay();

        // Tick once to trigger the first event (WaitTicks starts at 0, accumulates ticks)
        engine.Tick();

        // Control register voice 0 should have gate on
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(1, ctrl & 0x01, "Gate should be on after first note event");
    }

    [TestMethod]
    public void MusicStop_GatesOffMusicVoices()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "C1"); // whole note
        engine.MusicPlay();
        engine.Tick(); // play the note

        engine.MusicStop();

        // Gate should be off on voice 0
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0, ctrl & 0x01, "Gate should be cleared after MusicStop");
    }

    // -------------------------------------------------------------------------
    // Voice allocation
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SfxUseFreeVoice_WhenMusicOnOtherVoices()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Put music on voices 0 and 1 only
        engine.SetSequence(0, "C1");
        engine.SetSequence(1, "E1");
        engine.MusicPlay();
        engine.Tick();

        // Voice 2 has no sequence — SFX should use it
        engine.PlaySound(69, 5);

        // Voice 2 control register should have gate on
        byte ctrl2 = bus.Sid.Read(SidBase + 14 + 4); // voice 2 ctrl = base + 14 + 4
        Assert.AreEqual(1, ctrl2 & 0x01, "SFX should use free voice 2");
    }

    [TestMethod]
    public void SfxStealsLowestPriorityVoice_WhenAllBusy()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // All 3 voices have sequences
        engine.SetSequence(0, "C1");
        engine.SetSequence(1, "E1");
        engine.SetSequence(2, "G1");
        engine.MusicPlay();
        engine.Tick();

        // Default steal priority is {2,1,0} — voice 2 stolen first
        engine.PlaySound(60, 5);

        // SFX should write to voice 2
        byte ctrl2 = bus.Sid.Read(SidBase + 14 + 4);
        Assert.AreEqual(1, ctrl2 & 0x01, "SFX should steal voice 2 (lowest priority)");
    }

    // -------------------------------------------------------------------------
    // Arpeggio
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Arpeggio_CyclesNotes()
    {
        // {CEG}1 — arpeggio over C4(60), E4(64), G4(67), whole note duration
        // ArpStart event: ArpIndex=0, plays ArpNotes[0] immediately via WriteVoice
        // Each TickArp call increments ArpIndex first, then writes that note's freq
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "{CEG}1");
        engine.MusicPlay();

        // Tick 1: TickArp increments ArpIndex 0→1, writes E4 freq. Then event fires.
        // Wait — TickArp runs at the top of TickVoice, before events are processed.
        // On tick 1: ArpNotes is null (no ArpStart yet), so TickArp does nothing.
        // Then the while-loop fires (WaitTicks==0): ProcessEvent sets ArpNotes, ArpIndex=0, plays C4.
        // WaitTicks = 384 (whole note). Consume happens: WaitTicks -= min(TickAccum, WaitTicks).
        engine.Tick(); // ArpStart fires, C4 playing

        byte lo1 = bus.Sid.Read(SidBase + 0);
        byte hi1 = bus.Sid.Read(SidBase + 1);
        int  freq1 = lo1 | (hi1 << 8);

        // C4 = MIDI 60
        int sidC4 = (int)(261.63 * 16777216.0 / 985248.0); // ~4452

        // Tick 2: TickArp fires first (ArpNotes != null), increments ArpIndex 0→1, writes E4
        engine.Tick();

        byte lo2 = bus.Sid.Read(SidBase + 0);
        byte hi2 = bus.Sid.Read(SidBase + 1);
        int  freq2 = lo2 | (hi2 << 8);

        // E4 = MIDI 64
        int sidE4 = (int)(329.63 * 16777216.0 / 985248.0); // ~5608

        // freq1 should be near C4, freq2 near E4
        Assert.IsTrue(Math.Abs(freq1 - sidC4) <= 20, $"Tick1 freq should be ~C4({sidC4}), got {freq1}");
        Assert.IsTrue(Math.Abs(freq2 - sidE4) <= 20, $"Tick2 freq should be ~E4({sidE4}), got {freq2}");
        Assert.AreNotEqual(freq1, freq2, "Arpeggio should change frequency each tick");
    }

    // -------------------------------------------------------------------------
    // Loop (MML [...] expansion)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Loop_RepeatsNote()
    {
        // [C16]2 — parser expands to C16 C16 (two NoteOn events, 24 ticks each)
        // At 240 BPM: TicksPerFrame = 6.4, so 24 ticks ≈ 3.75 frames per note
        // After 4 frames the first note is done; after ~8 frames both notes done.
        // We verify the sequence runs to completion (IsMusicPlaying becomes false).
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "[C16]2");
        engine.SetTempo(240);
        engine.MusicPlay();

        // Run up to 20 ticks; music should stop well before that
        bool stoppedEarly = false;
        for (int i = 0; i < 20; i++)
        {
            engine.Tick();
            if (!engine.IsMusicPlaying)
            {
                stoppedEarly = true;
                break;
            }
        }

        Assert.IsTrue(stoppedEarly, "Music should complete after two 16th notes at 240 BPM");

        // Also confirm the gate was set at some point (note played) by checking that
        // the sequence produced 2 NoteOn events via the parser
        var events = MmlParser.Parse("[C16]2");
        int noteOnCount = events.Count(e => e.Type == MmlEventType.NoteOn);
        Assert.AreEqual(2, noteOnCount, "Parser should expand [C16]2 into exactly 2 NoteOn events");
    }

    // -------------------------------------------------------------------------
    // Music loop mode
    // -------------------------------------------------------------------------

    [TestMethod]
    public void MusicLoop_RestartsWhenDone()
    {
        // Short sequence (single 16th note at 240 BPM ≈ 4 frames).
        // SetLoop(true) should restart all voices when Done.
        // After restarting, IsMusicPlaying stays true and the note plays again.
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "C16");
        engine.SetTempo(240); // 6.4 ticks/frame; 24 ticks = ~3.75 frames
        engine.SetLoop(true);
        engine.MusicPlay();

        // Tick 10 times — enough for the sequence to complete and loop at least once.
        // If loop works, IsMusicPlaying should still be true after completion.
        for (int i = 0; i < 10; i++)
            engine.Tick();

        Assert.IsTrue(engine.IsMusicPlaying, "Music should still be playing after looping");

        // Verify gate is on (note restarted and re-triggered)
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(1, ctrl & 0x01, "Gate should be on after loop restarts the sequence");
    }

    // -------------------------------------------------------------------------
    // PWM sweep
    // -------------------------------------------------------------------------

    [TestMethod]
    public void PwmSweep_IncrementsEachTick()
    {
        // "@P2048 @PS+ C1" — set pulse width to 2048, start sweep up (+1 dir = +32/frame)
        // On the tick where the events fire, SetPulseWidth and PwmSweep are processed.
        // TickPwm runs at the TOP of each TickVoice, BEFORE events are processed.
        // So frame 1: TickPwm(dir=0, no-op), events fire setting PW=2048 and dir=+1.
        // Frame 2: TickPwm(dir=+1) → PW = 2048+32 = 2080.
        // Frame 3: TickPwm → PW = 2112. Etc.
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "@P2048 @PS+ C1");
        engine.MusicPlay();

        engine.Tick(); // events processed: PW=2048, PwmDir=+1, C4 gate on

        // Read PW after tick 1 — events just set it to 2048, no sweep yet
        byte pwLo1 = bus.Sid.Read(SidBase + 2);
        byte pwHi1 = bus.Sid.Read(SidBase + 3);
        int  pw1   = pwLo1 | ((pwHi1 & 0x0F) << 8);
        Assert.AreEqual(2048, pw1, "Pulse width should be 2048 after first tick (sweep not yet applied)");

        engine.Tick(); // TickPwm: PW = 2048 + 32 = 2080

        byte pwLo2 = bus.Sid.Read(SidBase + 2);
        byte pwHi2 = bus.Sid.Read(SidBase + 3);
        int  pw2   = pwLo2 | ((pwHi2 & 0x0F) << 8);
        Assert.AreEqual(2080, pw2, "Pulse width should be 2080 after second tick (one sweep step)");

        engine.Tick(); // TickPwm: PW = 2080 + 32 = 2112

        byte pwLo3 = bus.Sid.Read(SidBase + 2);
        byte pwHi3 = bus.Sid.Read(SidBase + 3);
        int  pw3   = pwLo3 | ((pwHi3 & 0x0F) << 8);
        Assert.AreEqual(2112, pw3, "Pulse width should be 2112 after third tick (two sweep steps)");
    }

    // -------------------------------------------------------------------------
    // Tempo
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SetTempo_ChangesBpm()
    {
        // A 16th note = 24 ticks. At 240 BPM: TicksPerFrame=6.4, done in ~4 frames.
        // At 60 BPM: TicksPerFrame=1.6, done in ~15 frames.
        // We count how many ticks until IsMusicPlaying becomes false at each tempo.
        int CountTicksToFinish(int bpm)
        {
            var bus    = MakeBus();
            var engine = new MusicEngine(bus);
            engine.SetSequence(0, "C16");
            engine.SetTempo(bpm);
            engine.MusicPlay();
            for (int i = 1; i <= 50; i++)
            {
                engine.Tick();
                if (!engine.IsMusicPlaying) return i;
            }
            return 50; // didn't finish
        }

        int fastFrames = CountTicksToFinish(240);
        int slowFrames = CountTicksToFinish(60);

        Assert.IsTrue(fastFrames < slowFrames,
            $"240 BPM should finish sooner ({fastFrames} frames) than 60 BPM ({slowFrames} frames)");

        // At 240 BPM (6.4 ticks/frame), 24 ticks should finish within 5 frames
        // (4 frames to drain WaitTicks, then 1 more for TickMusic to detect done and stop)
        Assert.IsTrue(fastFrames <= 5, $"At 240 BPM a 16th note should finish within 5 frames, took {fastFrames}");

        // At 60 BPM (1.6 ticks/frame), 24 ticks should take ~15 frames
        Assert.IsTrue(slowFrames >= 14, $"At 60 BPM a 16th note should take at least 14 frames, took {slowFrames}");
    }

    // -------------------------------------------------------------------------
    // SFX voice stealing with music
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SfxDoesNotDisruptOtherMusicVoices()
    {
        // Music on voices 0 and 1 (whole notes), SFX steals voice 2.
        // After SFX is placed on voice 2, voices 0 and 1 should still have gate on.
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "C1");
        engine.SetSequence(1, "E1");
        engine.MusicPlay();
        engine.Tick(); // voice 0 and 1 get their notes

        byte ctrl0Before = bus.Sid.Read(SidBase + 4);
        byte ctrl1Before = bus.Sid.Read(SidBase + 7 + 4);
        Assert.AreEqual(1, ctrl0Before & 0x01, "Voice 0 gate should be on before SFX");
        Assert.AreEqual(1, ctrl1Before & 0x01, "Voice 1 gate should be on before SFX");

        // SFX: voice 2 has no sequence → AllocateSfxVoice picks voice 2
        engine.PlaySound(69, 5);

        // Voices 0 and 1 gates should be unaffected
        byte ctrl0After = bus.Sid.Read(SidBase + 4);
        byte ctrl1After = bus.Sid.Read(SidBase + 7 + 4);
        Assert.AreEqual(1, ctrl0After & 0x01, "Voice 0 gate should remain on after SFX steals voice 2");
        Assert.AreEqual(1, ctrl1After & 0x01, "Voice 1 gate should remain on after SFX steals voice 2");

        // Tick music — voice 2 is skipped (stolen), voices 0+1 continue
        engine.Tick();
        byte ctrl0Tick = bus.Sid.Read(SidBase + 4);
        byte ctrl1Tick = bus.Sid.Read(SidBase + 7 + 4);
        Assert.AreEqual(1, ctrl0Tick & 0x01, "Voice 0 gate should stay on during SFX playback");
        Assert.AreEqual(1, ctrl1Tick & 0x01, "Voice 1 gate should stay on during SFX playback");
    }

    // -------------------------------------------------------------------------
    // SfxCompleted during music
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SfxCompleted_StillWorksWhileMusicPlaying()
    {
        // Music on voice 0, SFX on voice 2 (free). SfxCompleted should fire normally.
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "C1");
        engine.MusicPlay();
        engine.Tick();

        engine.PlaySound(60, 1); // 1 frame SFX on voice 2
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted should not be set before expiry");

        engine.Tick(); // SFX expires: _sfxFrames 1→0, gate off, _sfxCompleted = true

        Assert.IsTrue(engine.SfxCompleted, "SfxCompleted should be true after SFX expires during music");
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted is one-shot — should clear on read");

        // Music should still be playing
        Assert.IsTrue(engine.IsMusicPlaying, "Music should still be playing after SFX completes");
    }
}
