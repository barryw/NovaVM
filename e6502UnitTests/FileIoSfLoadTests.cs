using System;
using System.IO;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class FileIoSfLoadTests
{
    private static (FileIoController Fio, byte[] Memory) MakeController(
        string saveDir, WavetableSynth? wts = null)
    {
        var memory = new byte[65536];
        var fio = new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            saveDir,
            wts: wts);
        return (fio, memory);
    }

    private static string ReadNullPaddedString(byte[] memory, int address, int maxLen)
    {
        int len = 0;
        while (len < maxLen && memory[address + len] != 0)
            len++;
        return System.Text.Encoding.ASCII.GetString(memory, address, len);
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
            var (fio, memory) = MakeController(dir, wts);

            SetFilename(fio, "test");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNone, fio.Read((ushort)VgcConstants.FioErrCode));
            Assert.AreEqual(1, wts.InstrumentCount);
            Assert.AreEqual("test", ReadNullPaddedString(memory,
                VgcConstants.MusicMetaSoundfont, VgcConstants.MusicMetaSoundfontLen));
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
            var (fio, _) = MakeController(dir, wts);

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
            var (fio, _) = MakeController(dir, wts: null);

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
        // Use a temp directory to avoid deleting the user's real soundfonts
        string tempDir = Path.Combine(Path.GetTempPath(), "e6502-test-sfload-bus-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(tempDir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "test.sf2"), sf2);

            // Create a standalone controller with the temp directory instead of using
            // CompositeBusDevice (which would use the real ~/e6502-programs path)
            var wts = new WavetableSynth();
            var memory = new byte[65536];
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                tempDir,
                wts: wts);

            string name = "test";
            fio.Write((ushort)VgcConstants.FioNameLen, (byte)name.Length);
            for (int i = 0; i < name.Length; i++)
                fio.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(1, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(tempDir, true);
        }
    }
}
