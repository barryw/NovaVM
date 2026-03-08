using System;
using System.IO;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class FileIoSfLoadTests
{
    private static FileIoController MakeController(string saveDir, WavetableSynth? wts = null)
    {
        var memory = new byte[65536];
        return new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            saveDir,
            wts: wts);
    }

    private static void SetFilename(FileIoController fio, string filename)
    {
        fio.Write((ushort)VgcConstants.FioNameLen, (byte)filename.Length);
        for (int i = 0; i < filename.Length; i++)
            fio.Write((ushort)(VgcConstants.FioName + i), (byte)filename[i]);
    }

    [TestMethod]
    public void SfLoad_ValidFile_LoadsSoundfont()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-sfload-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "test.sf2"), sf2);

            var wts = new WavetableSynth();
            var fio = MakeController(dir, wts);

            SetFilename(fio, "test");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNone, fio.Read((ushort)VgcConstants.FioErrCode));
            Assert.AreEqual(1, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void SfLoad_FileNotFound_SetsError()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-sfload-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(dir);

        try
        {
            var wts = new WavetableSynth();
            var fio = MakeController(dir, wts);

            SetFilename(fio, "nonexistent");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNotFound, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void SfLoad_NoWts_SetsError()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-sfload-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(dir);

        try
        {
            var fio = MakeController(dir, wts: null);

            SetFilename(fio, "test");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrIo, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void SfLoad_ViaCompositeBus_LoadsSoundfont()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        string dir = bus.Fio.SaveDirectory;
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "test.sf2"), sf2);

            string name = "test";
            bus.Write((ushort)VgcConstants.FioNameLen, (byte)name.Length);
            for (int i = 0; i < name.Length; i++)
                bus.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);

            bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, bus.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsInstrumentCount));
            Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsSoundfontStatus));
        }
        finally
        {
            if (Directory.Exists(sfDir))
                Directory.Delete(sfDir, true);
        }
    }
}
