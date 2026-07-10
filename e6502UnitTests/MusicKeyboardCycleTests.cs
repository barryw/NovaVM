using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Reproduces the hardware defect: after several browse->keyboard->browse
/// round-trips the music app falls out to BASIC ("Ready") while the song keeps
/// playing. The board's debugger is disabled, so we run the identical music.bin
/// in the emulator (full CPU-state visibility) with a real disk mounted so the
/// keyboard reads a LIVE song's $BA50 status/note/elapsed block every frame,
/// exactly as on hardware. The crash detector is CPU state: the app sets SP=$FF
/// at start and its code lives in [$7200,$9F00); a crash sends PC out of range
/// or leaks the stack.
/// </summary>
[TestClass]
public class MusicKeyboardCycleTests
{
    private const ushort AppEntry = 0x7200;
    private const ushort AppLo = 0x7200, AppHi = 0x9F00;
    private const byte KeyEnter = 0x0D, KeyEsc = 0x1B, KeyRight = 0x1D, KeyDown = 0x1F;
    private const ushort MusicStatus = 0xBA50;

    private sealed record Harness(CompositeBusDevice Bus, Cpu Cpu, ScreenEditor Editor);

    private static string RepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir, "software", "assembly", "apps", "music", "music.bin")))
                return dir;
            dir = Path.GetDirectoryName(dir);
        }
        throw new InvalidOperationException("Could not locate repo root (apps/music/music.bin).");
    }

    private static Harness Boot(bool mountDisk)
    {
        // NOVA_NO_AUTOMOUNT is read once by the CompositeBusDevice ctor. Scope it to
        // construction only — leaking it process-wide breaks later tests that rely on
        // automount (e.g. Forth INCLUDE). The AssemblySetup guard enforces this.
        CompositeBusDevice bus;
        using (new EnvScope("NOVA_NO_AUTOMOUNT", "1"))
            bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        if (mountDisk)
        {
            string ndi = Path.Combine(RepoRoot(), "docs", "programs", "demo.ndi");
            Assert.IsTrue(File.Exists(ndi), $"Missing test disk: {ndi}");
            // Mount a private copy so parallel/sequential tests don't fight over the
            // shared repo image (the bus holds it open for the test's lifetime).
            string tmp = Path.Combine(Path.GetTempPath(), $"showcase-{Guid.NewGuid():N}.ndi");
            File.Copy(ndi, tmp, overwrite: true);
            bus.DeviceManager.GetDevice("FD0").Mount(tmp);
            bus.DeviceManager.DefaultDevice = "FD0";
        }
        string path = Path.Combine(RepoRoot(), "software", "assembly", "apps", "music", "music.bin");
        Assert.IsTrue(File.Exists(path), $"Build the music app first: {path}");
        byte[] prg = File.ReadAllBytes(path);
        ushort load = (ushort)(prg[0] | (prg[1] << 8));
        for (int i = 2; i < prg.Length; i++)
            bus.WriteRam((ushort)(load + i - 2), prg[i]);
        var cpu = new Cpu(bus, E6502Type.Cmos);
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        cpu.Boot(AppEntry);
        var h = new Harness(bus, cpu, editor);
        RunFrames(h, 300);   // let the browser render
        return h;
    }

    /// <summary>Advance the app N video frames, ticking the music engines once per
    /// frame so a live song's notes/elapsed progress deterministically (the bus's
    /// own ticks are wall-clock gated and won't fire in a fast test).</summary>
    private static void RunFrames(Harness h, int frames)
    {
        for (int f = 0; f < frames; f++)
        {
            long start = h.Bus.TotalFrames;
            int guard = 2_000_000;
            while (h.Bus.TotalFrames == start && guard-- > 0)
            {
                int cyc = h.Cpu.ClocksForNext();
                h.Cpu.ExecuteNext();
                h.Bus.AdvanceCycles(cyc);
            }
            h.Bus.Music.Tick();
            h.Bus.MidiPlayback.Tick();
        }
    }

    private static void Press(Harness h, byte key, int frames = 220)
    {
        h.Editor.QueueInput(key);
        RunFrames(h, frames);
    }

    private static string Screen(Harness h)
    {
        var sb = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = h.Bus.Vgc.GetScreenChar(col, row);
                sb.Append(ch >= 32 && ch < 127 ? (char)ch : ' ');
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }

    private static void WriteMetaString(Harness h, int address, int length, string text)
    {
        byte[] bytes = Encoding.ASCII.GetBytes(text);
        for (int i = 0; i < length; i++)
            h.Bus.WriteRam((ushort)(address + i), i < bytes.Length ? bytes[i] : (byte)0);
    }

    private static void CycleAndAssert(Harness h, int nRow, string label)
    {
        // Select row nRow (down nRow times from the top).
        for (int i = 0; i < nRow; i++) Press(h, KeyDown);

        var log = new StringBuilder($"[{label}] boot SP=${h.Cpu.GetState().Sp:X2}\n");
        for (int cycle = 1; cycle <= 8; cycle++)
        {
            Press(h, KeyEnter);                 // enter keyboard -> app issues FioCmd (song plays)
            var kb = h.Cpu.GetState();
            byte status = h.Bus.Read(MusicStatus);
            string kbScreen = Screen(h);
            bool kbdShown = kbScreen.Contains("PRESS ESC") || kbScreen.Contains("ESC/Q");

            Press(h, KeyEsc);                   // exit -> browser
            var br = h.Cpu.GetState();
            string brScreen = Screen(h);
            bool browserBack = brScreen.Contains("FEATURED");

            log.Append($"  cycle {cycle}: enter PC=${kb.Pc:X4} SP=${kb.Sp:X2} status=${status:X2} kbd={kbdShown} | esc PC=${br.Pc:X4} SP=${br.Sp:X2} browser={browserBack}\n");

            Assert.IsTrue(br.Pc >= AppLo && br.Pc < AppHi,
                $"{label} cycle {cycle}: PC ${br.Pc:X4} left app code — crashed out of the app.\n{log}\nScreen:\n{brScreen}");
            Assert.IsTrue(br.Sp >= 0xC0,
                $"{label} cycle {cycle}: SP ${br.Sp:X2} below $C0 — stack leaking.\n{log}");
            Assert.IsTrue(kbdShown,
                $"{label} cycle {cycle}: keyboard did not appear after RETURN.\n{log}\nScreen:\n{kbScreen}");
            Assert.IsTrue(browserBack,
                $"{label} cycle {cycle}: ESC did not return to the browser (keyboard stuck).\n{log}\nScreen:\n{brScreen}");
        }
    }

    private static int Elapsed(Harness h) => h.Bus.Read(0xBA5F) | (h.Bus.Read(0xBA60) << 8);

    [TestMethod]
    public void Music_MidActuallyPlaysAndTimeAdvances()
    {
        // Regression: a double file extension (app sent name+".mid", host appends
        // ".mid" again) meant FIO never found the file, so nothing played, the
        // $BA50 status stayed 0, and elapsed/total/metadata never populated — the
        // "shows a keyboard but does not play, time does not increase" defect.
        var h = Boot(mountDisk: true);
        Press(h, KeyEnter);                       // row 0 (FEATURED) = champagne-supernova.mid
        byte status = h.Bus.Read(MusicStatus);
        int total = h.Bus.Read(0xBA61) | (h.Bus.Read(0xBA62) << 8);
        int el0 = Elapsed(h);
        Assert.AreNotEqual(0, status, "MID play did not start (status still 0).");
        Assert.IsTrue(total > 0, $"MID total duration not published (total={total}).");
        RunFrames(h, 120);
        Assert.IsTrue(Elapsed(h) > el0, $"Elapsed time did not advance ({el0} -> {Elapsed(h)}).");

        // Title must be the song, not an instrument track name ("Drums").
        string screen = Screen(h);
        Assert.IsFalse(screen.Contains("DRUM", StringComparison.OrdinalIgnoreCase),
            $"Title shows an instrument track name, not the song.\n{screen}");
        Assert.IsTrue(screen.Contains("Champagne Supernova", StringComparison.OrdinalIgnoreCase),
            $"Title should be the prettified song name.\n{screen}");
    }

    [TestMethod]
    public void Music_SidActuallyPlays()
    {
        var h = Boot(mountDisk: true);
        for (int i = 0; i < 3; i++) Press(h, KeyDown);   // row 3 (FEATURED) = commando.sid
        Press(h, KeyEnter);
        Assert.AreNotEqual(0, h.Bus.Read(MusicStatus), "SID play did not start (status still 0).");
    }

    [TestMethod]
    public void Music_RefreshesEveryAsynchronousMetadataField()
    {
        // Hardware publishes fields over several frames. Changing only author or
        // soundfont must still redraw, and clearing a field must erase stale text.
        var h = Boot(mountDisk: false);
        Press(h, KeyEnter, frames: 120);
        h.Bus.WriteRam((ushort)VgcConstants.MetaType, 3);
        WriteMetaString(h, VgcConstants.MetaTitle, VgcConstants.MetaTitleLen, "ASYNC TITLE");
        WriteMetaString(h, VgcConstants.MetaAuthor, VgcConstants.MetaAuthorLen, "FIRST AUTHOR");
        WriteMetaString(h, VgcConstants.MusicMetaSoundfont,
            VgcConstants.MusicMetaSoundfontLen, "OLD BANK");
        RunFrames(h, 4);

        string first = Screen(h);
        StringAssert.Contains(first, "FIRST AUTHOR");
        StringAssert.Contains(first, "FONT: OLD BANK");

        WriteMetaString(h, VgcConstants.MetaAuthor, VgcConstants.MetaAuthorLen, "SECOND AUTHOR");
        WriteMetaString(h, VgcConstants.MusicMetaSoundfont,
            VgcConstants.MusicMetaSoundfontLen, "");
        RunFrames(h, 4);

        string second = Screen(h);
        StringAssert.Contains(second, "SECOND AUTHOR");
        Assert.IsFalse(second.Contains("FIRST AUTHOR", StringComparison.Ordinal), second);
        Assert.IsFalse(second.Contains("FONT:", StringComparison.Ordinal), second);
    }

    [TestMethod]
    public void Music_UsesSharedArmedVgcCommandWait()
    {
        // FPGA busy asserts after the command write; the shared helper deliberately
        // arms for 32 reads, while the removed one-read loop could return too early.
        string root = RepoRoot();
        string music = File.ReadAllText(Path.Combine(root,
            "software", "assembly", "apps", "music", "music.s"));
        string notes = File.ReadAllText(Path.Combine(root,
            "software", "assembly", "apps", "music", "music_notes.inc"));
        StringAssert.Contains(music, ".import vgc_gtext, vgc_vsync, vgc_wait_cmd");
        StringAssert.Contains(notes, "jsr vgc_wait_cmd");
        Assert.IsFalse((music + notes).Contains("mus_cmdwait", StringComparison.Ordinal),
            "Do not reintroduce the unarmed local VGC wait.");
    }

    [TestMethod]
    public void Music_BootsToBrowser()
    {
        var h = Boot(mountDisk: false);
        string s = Screen(h);
        var st = h.Cpu.GetState();
        Assert.IsTrue(st.Pc >= AppLo && st.Pc < AppHi, $"App PC ${st.Pc:X4} outside code range.\n{s}");
        Assert.IsTrue(s.Contains("FEATURED") || s.Contains("COMMANDO"), $"Browser text not found.\n{s}");
    }

    [TestMethod]
    public void Music_SurvivesVisualizeOnlyCycles()
    {
        // No disk: FIO errors, keyboard visualizes nothing. Documents that the
        // pure cycle logic is sound.
        var h = Boot(mountDisk: false);
        CycleAndAssert(h, 3, "no-song");
    }

    [TestMethod]
    public void Music_SurvivesMidKeyboardCycles()
    {
        // Row 0 (FEATURED) = champagne-supernova.mid — plays through MusicEngine,
        // so the keyboard reads a live $BA50 block each frame like on hardware.
        var h = Boot(mountDisk: true);
        CycleAndAssert(h, 0, "mid");
    }

    [TestMethod]
    public void Music_SurvivesSidKeyboardCycles()
    {
        // Row 3 (FEATURED) = commando.sid — the case the user watched crash.
        var h = Boot(mountDisk: true);
        CycleAndAssert(h, 3, "sid");
    }

    [TestMethod]
    public void Music_AllCuratedTracksStartAndReturnCleanly()
    {
        // Every embedded table entry must resolve to a packaged file and start.
        // This also exercises scrolling, category changes, mixed MID/SID handoff,
        // ESC cleanup, and full redraws across the complete showcase disk.
        var h = Boot(mountDisk: true);
        var log = new StringBuilder("sweep:\n");
        int[] categoryCounts = [14, 3, 15, 19, 10];
        for (int category = 0; category < categoryCounts.Length; category++)
        {
            for (int row = 0; row < categoryCounts[category]; row++)
            {
                Press(h, KeyEnter, frames: 100); // stay inside the 150-frame auto-exit grace
                var kb = h.Cpu.GetState();
                byte status = h.Bus.Read(MusicStatus);
                bool kbdShown = Screen(h).Contains("PRESS ESC") || Screen(h).Contains("ESC/Q");

                Press(h, KeyEsc, frames: 140);   // back to browser after input arms
                var af = h.Cpu.GetState();
                byte stoppedStatus = (byte)(h.Bus.Read(MusicStatus) & 0x0E);
                bool browserBack = Screen(h).Contains("FEATURED");
                log.Append($"  category {category} row {row}: play PC=${kb.Pc:X4} SP=${kb.Sp:X2} status=${status:X2} kbd={kbdShown} | back PC=${af.Pc:X4} SP=${af.Sp:X2} browser={browserBack}\n");

                Assert.AreNotEqual(0, status,
                    $"category {category} row {row}: packaged track did not start.\n{log}");
                Assert.IsTrue(af.Pc >= AppLo && af.Pc < AppHi,
                    $"category {category} row {row}: PC ${af.Pc:X4} left app code — crashed.\n{log}\nScreen:\n{Screen(h)}");
                Assert.IsTrue(af.Sp >= 0xC0,
                    $"category {category} row {row}: SP ${af.Sp:X2} below $C0 — stack leak.\n{log}");
                Assert.IsTrue(kbdShown,
                    $"category {category} row {row}: keyboard didn't appear.\n{log}");
                Assert.IsTrue(browserBack,
                    $"category {category} row {row}: ESC didn't return to browser.\n{log}");
                Assert.AreEqual(0, stoppedStatus,
                    $"category {category} row {row}: ESC returned but hosted audio kept playing.\n{log}");

                if (row + 1 < categoryCounts[category])
                    Press(h, KeyDown);
            }

            if (category + 1 < categoryCounts.Length)
                Press(h, KeyRight);
        }
    }
}
