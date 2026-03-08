using System;
using System.IO;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MidiAutoSoundfontTests
{
    private static FileIoController MakeFio(string dir, WavetableSynth wts)
    {
        var memory = new byte[65536];
        return new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            dir,
            wts: wts);
    }

    [TestMethod]
    public void TryLoad_LoadsWhenSfExists()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "GeneralUser_GS.sf2"), sf2);

            var wts = new WavetableSynth();
            Assert.AreEqual(0, wts.InstrumentCount);

            var fio = MakeFio(dir, wts);
            MidiAutoSoundfont.TryLoad(fio, wts);

            Assert.IsTrue(wts.InstrumentCount > 0);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void TryLoad_NoOpWhenAlreadyLoaded()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "GeneralUser_GS.sf2"), sf2);

            var wts = new WavetableSynth();
            var fio = MakeFio(dir, wts);

            MidiAutoSoundfont.TryLoad(fio, wts);
            int count1 = wts.InstrumentCount;

            MidiAutoSoundfont.TryLoad(fio, wts);
            Assert.AreEqual(count1, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void TryLoad_NoOpWhenFileNotFound()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(dir);

        try
        {
            var wts = new WavetableSynth();
            var fio = MakeFio(dir, wts);

            MidiAutoSoundfont.TryLoad(fio, wts);
            Assert.AreEqual(0, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }
}
