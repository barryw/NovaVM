using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Common;

namespace e6502UnitTests;

[TestClass]
public class FileIoControllerTests
{
    private static FileIoController MakeController(string saveDir)
    {
        var memory = new byte[65536];
        return new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            saveDir);
    }

    private static void SetFilename(FileIoController fio, string filename)
    {
        fio.Write((ushort)VgcConstants.FioNameLen, (byte)filename.Length);
        for (int i = 0; i < filename.Length; i++)
            fio.Write((ushort)(VgcConstants.FioName + i), (byte)filename[i]);
    }

    private static string ReadFilename(FileIoController fio)
    {
        int len = fio.Read((ushort)VgcConstants.FioNameLen);
        var sb = new StringBuilder(len);
        for (int i = 0; i < len; i++)
            sb.Append((char)fio.Read((ushort)(VgcConstants.FioName + i)));
        return sb.ToString();
    }

    private static int ReadSize(FileIoController fio)
    {
        int lo = fio.Read((ushort)VgcConstants.FioSizeL);
        int hi = fio.Read((ushort)VgcConstants.FioSizeH);
        return lo | (hi << 8);
    }

    private static FileIoController MakeControllerWithGraphics(
        string saveDir,
        byte[] screen,
        byte[] color,
        byte[] gfx,
        byte[] sprite)
    {
        var memory = new byte[65536];
        return new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            saveDir,
            vgcRead: (space, offset) => space switch
            {
                VgcConstants.MemSpaceScreen when (uint)offset < screen.Length => screen[offset],
                VgcConstants.MemSpaceColor when (uint)offset < color.Length => color[offset],
                VgcConstants.MemSpaceGfx when (uint)offset < gfx.Length => gfx[offset],
                VgcConstants.MemSpaceSprite when (uint)offset < sprite.Length => sprite[offset],
                _ => 0
            },
            vgcWrite: (space, offset, value) =>
            {
                switch (space)
                {
                    case VgcConstants.MemSpaceScreen when (uint)offset < screen.Length:
                        screen[offset] = value;
                        break;
                    case VgcConstants.MemSpaceColor when (uint)offset < color.Length:
                        color[offset] = value;
                        break;
                    case VgcConstants.MemSpaceGfx when (uint)offset < gfx.Length:
                        gfx[offset] = value;
                        break;
                    case VgcConstants.MemSpaceSprite when (uint)offset < sprite.Length:
                        sprite[offset] = value;
                        break;
                }
            },
            vgcSpaceLength: space => space switch
            {
                VgcConstants.MemSpaceScreen => screen.Length,
                VgcConstants.MemSpaceColor => color.Length,
                VgcConstants.MemSpaceGfx => gfx.Length,
                VgcConstants.MemSpaceSprite => sprite.Length,
                _ => 0
            });
    }

    private static FileIoController MakeControllerWithXram(string saveDir, byte[] xram)
    {
        var memory = new byte[65536];

        return new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            saveDir,
            xramRead: address => xram[address],
            xramWrite: (address, value) =>
            {
                if ((uint)address >= xram.Length)
                    return false;

                xram[address] = value;
                return true;
            },
            xramCapacity: () => xram.Length,
            xramRefreshStats: () => { });
    }

    [TestMethod]
    public void ClearErr_ClearsLatchedFileIoError()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            var fio = MakeController(dir);
            SetFilename(fio, "does-not-exist");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDelete);

            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNotFound, fio.Read((ushort)VgcConstants.FioErrCode));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdClearErr);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNone, fio.Read((ushort)VgcConstants.FioErrCode));
            Assert.AreEqual(0, fio.Read((ushort)VgcConstants.FioCmd));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void Delete_RemovesExistingFile()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            string path = Path.Combine(dir, "delete-me.bas");
            File.WriteAllBytes(path, [0x00, 0x04, 0xAA, 0xBB]);

            var fio = MakeController(dir);
            SetFilename(fio, "delete-me");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDelete);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNone, fio.Read((ushort)VgcConstants.FioErrCode));
            Assert.IsFalse(File.Exists(path));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void Delete_MissingFile_ReturnsNotFound()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            var fio = MakeController(dir);
            SetFilename(fio, "does-not-exist");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDelete);

            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNotFound, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void DirOpenAndDirRead_EnumerateBasFiles()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            File.WriteAllBytes(Path.Combine(dir, "beta.bas"), [0x00, 0x04, 1, 2, 3]);
            File.WriteAllBytes(Path.Combine(dir, "alpha.bas"), [0x00, 0x04, 7]);

            var fio = MakeController(dir);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("alpha", ReadFilename(fio));
            Assert.AreEqual(1, ReadSize(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("beta", ReadFilename(fio));
            Assert.AreEqual(3, ReadSize(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void GSave_WritesGraphicsDataToGfxFile()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            var screen = new byte[2000];
            var color = new byte[2000];
            var gfx = new byte[64000];
            var sprite = new byte[2048];
            for (int i = 0; i < 32; i++) sprite[0x40 + i] = (byte)(0xA0 + i);

            var fio = MakeControllerWithGraphics(dir, screen, color, gfx, sprite);
            SetFilename(fio, "spr0");
            fio.Write((ushort)VgcConstants.FioGSpace, VgcConstants.MemSpaceSprite);
            fio.Write((ushort)VgcConstants.FioGAddrL, 0x40);
            fio.Write((ushort)VgcConstants.FioGAddrH, 0x00);
            fio.Write((ushort)VgcConstants.FioGLenL, 32);
            fio.Write((ushort)VgcConstants.FioGLenH, 0x00);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdGSave);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(32, ReadSize(fio));

            string path = Path.Combine(dir, "spr0.gfx");
            Assert.IsTrue(File.Exists(path));
            byte[] data = File.ReadAllBytes(path);
            Assert.AreEqual(32, data.Length);
            for (int i = 0; i < 32; i++)
                Assert.AreEqual((byte)(0xA0 + i), data[i]);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void FioCmdSidStop_SetsOk()
    {
        var bus = new CompositeBusDevice();
        bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSidStop);
        Assert.AreEqual(VgcConstants.FioStatusOk, bus.Read((ushort)VgcConstants.FioStatus));
    }

    [TestMethod]
    public void FioCmdSidPlay_NoFile_SetsNotFoundError()
    {
        var bus = new CompositeBusDevice();
        string name = "nonexistent";
        bus.Write((ushort)VgcConstants.FioNameLen, (byte)name.Length);
        for (int i = 0; i < name.Length; i++)
            bus.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);
        bus.Write((ushort)VgcConstants.FioSrcL, 0x01);
        bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSidPlay);
        Assert.AreEqual(VgcConstants.FioStatusError, bus.Read((ushort)VgcConstants.FioStatus));
    }

    [TestMethod]
    public void ProgramLoaded_FiresOnLoad()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            // Create a minimal .bas file (2-byte load address + 1 byte data)
            File.WriteAllBytes(Path.Combine(dir, "hello.bas"), [0x00, 0x04, 0x42]);

            var fio = MakeController(dir);
            string? loadedName = null;
            fio.ProgramLoaded += name => loadedName = name;

            SetFilename(fio, "hello");
            fio.Write((ushort)VgcConstants.FioSrcL, 0x00);
            fio.Write((ushort)VgcConstants.FioSrcH, 0x04);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("hello", loadedName);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void ProgramSaved_FiresOnSave()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");

        try
        {
            var memory = new byte[65536];
            memory[0x0400] = 0x42;
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                dir);

            string? savedName = null;
            bool? wasNewDoc = null;
            fio.ProgramSaved += (name, isNew) => { savedName = name; wasNewDoc = isNew; };

            SetFilename(fio, "test-prog");
            fio.Write((ushort)VgcConstants.FioSrcL, 0x00);
            fio.Write((ushort)VgcConstants.FioSrcH, 0x04);
            fio.Write((ushort)VgcConstants.FioEndL, 0x01);
            fio.Write((ushort)VgcConstants.FioEndH, 0x04);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSave);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("test-prog", savedName);
            Assert.IsTrue(wasNewDoc);

            // Companion .md should exist
            Assert.IsTrue(File.Exists(Path.Combine(dir, "test-prog.md")));
        }
        finally
        {
            if (Directory.Exists(dir))
                Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void Save_DoesNotOverwriteExistingMd()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            // Pre-create .md with custom content
            string mdPath = Path.Combine(dir, "existing.md");
            File.WriteAllText(mdPath, "# Custom Help");

            var memory = new byte[65536];
            memory[0x0400] = 0x42;
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                dir);

            bool? wasNewDoc = null;
            fio.ProgramSaved += (_, isNew) => wasNewDoc = isNew;

            SetFilename(fio, "existing");
            fio.Write((ushort)VgcConstants.FioSrcL, 0x00);
            fio.Write((ushort)VgcConstants.FioSrcH, 0x04);
            fio.Write((ushort)VgcConstants.FioEndL, 0x01);
            fio.Write((ushort)VgcConstants.FioEndH, 0x04);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSave);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.IsFalse(wasNewDoc);
            Assert.AreEqual("# Custom Help", File.ReadAllText(mdPath));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void Delete_RemovesCompanionMd()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            string basPath = Path.Combine(dir, "game.bas");
            string mdPath = Path.Combine(dir, "game.md");
            File.WriteAllBytes(basPath, [0x00, 0x04, 0xAA]);
            File.WriteAllText(mdPath, "# Game Help");

            var fio = MakeController(dir);
            SetFilename(fio, "game");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDelete);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.IsFalse(File.Exists(basPath));
            Assert.IsFalse(File.Exists(mdPath));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void GLoad_ReadsGraphicsDataFromGfxFile()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            byte[] fileData = [1, 2, 3, 4, 5, 6, 7, 8];
            File.WriteAllBytes(Path.Combine(dir, "chars.gfx"), fileData);

            var screen = new byte[2000];
            var color = new byte[2000];
            var gfx = new byte[64000];
            var sprite = new byte[2048];

            var fio = MakeControllerWithGraphics(dir, screen, color, gfx, sprite);
            SetFilename(fio, "chars");
            fio.Write((ushort)VgcConstants.FioGSpace, VgcConstants.MemSpaceScreen);
            fio.Write((ushort)VgcConstants.FioGAddrL, 0x10);
            fio.Write((ushort)VgcConstants.FioGAddrH, 0x00);
            fio.Write((ushort)VgcConstants.FioGLenL, 0x00); // 0 = full file
            fio.Write((ushort)VgcConstants.FioGLenH, 0x00);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdGLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(fileData.Length, ReadSize(fio));
            for (int i = 0; i < fileData.Length; i++)
                Assert.AreEqual(fileData[i], screen[0x10 + i]);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void XSave_WritesXramDataToDefaultXramFile()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            var xram = new byte[1024];
            for (int i = 0; i < 16; i++)
                xram[0x40 + i] = (byte)(0x80 + i);

            var fio = MakeControllerWithXram(dir, xram);
            SetFilename(fio, "snapshot");
            fio.Write((ushort)VgcConstants.FioGSpace, 0x00);
            fio.Write((ushort)VgcConstants.FioGAddrL, 0x40);
            fio.Write((ushort)VgcConstants.FioGAddrH, 0x00);
            fio.Write((ushort)VgcConstants.FioGLenL, 16);
            fio.Write((ushort)VgcConstants.FioGLenH, 0x00);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdXSave);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(16, ReadSize(fio));

            byte[] data = File.ReadAllBytes(Path.Combine(dir, "snapshot.xram"));
            Assert.AreEqual(16, data.Length);
            for (int i = 0; i < 16; i++)
                Assert.AreEqual((byte)(0x80 + i), data[i]);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void XLoad_HonorsExplicitExtensionAndOptionalLength()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-fio-{Guid.NewGuid():N}");
        Directory.CreateDirectory(dir);

        try
        {
            File.WriteAllBytes(Path.Combine(dir, "story.z3"), [0x11, 0x22, 0x33, 0x44, 0x55]);
            var xram = new byte[1024];

            var fio = MakeControllerWithXram(dir, xram);
            SetFilename(fio, "story.z3");
            fio.Write((ushort)VgcConstants.FioGSpace, 0x00);
            fio.Write((ushort)VgcConstants.FioGAddrL, 0x20);
            fio.Write((ushort)VgcConstants.FioGAddrH, 0x00);
            fio.Write((ushort)VgcConstants.FioGLenL, 3);
            fio.Write((ushort)VgcConstants.FioGLenH, 0x00);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdXLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(3, ReadSize(fio));
            Assert.AreEqual((byte)0x11, xram[0x20]);
            Assert.AreEqual((byte)0x22, xram[0x21]);
            Assert.AreEqual((byte)0x33, xram[0x22]);
            Assert.AreEqual((byte)0x00, xram[0x23]);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void GlobMatch_StarMatchesAnything()
    {
        Assert.IsTrue(FileIoController.GlobMatch("*", "anything"));
        Assert.IsTrue(FileIoController.GlobMatch("*", ""));
        Assert.IsTrue(FileIoController.GlobMatch("*", "BACH-FUGUE"));
    }

    [TestMethod]
    public void GlobMatch_StarPrefix()
    {
        Assert.IsTrue(FileIoController.GlobMatch("BACH*", "BACH-FUGUE"));
        Assert.IsTrue(FileIoController.GlobMatch("BACH*", "BACH"));
        Assert.IsFalse(FileIoController.GlobMatch("BACH*", "MOZART"));
    }

    [TestMethod]
    public void GlobMatch_StarSuffix()
    {
        Assert.IsTrue(FileIoController.GlobMatch("*FUGUE", "BACH-FUGUE"));
        Assert.IsFalse(FileIoController.GlobMatch("*FUGUE", "BACH-SONATA"));
    }

    [TestMethod]
    public void GlobMatch_QuestionMark()
    {
        Assert.IsTrue(FileIoController.GlobMatch("A?C", "ABC"));
        Assert.IsFalse(FileIoController.GlobMatch("A?C", "ABBC"));
        Assert.IsFalse(FileIoController.GlobMatch("A?C", "AC"));
    }

    [TestMethod]
    public void GlobMatch_CaseInsensitive()
    {
        Assert.IsTrue(FileIoController.GlobMatch("bach*", "BACH-FUGUE"));
        Assert.IsTrue(FileIoController.GlobMatch("BACH*", "bach-fugue"));
    }

    [TestMethod]
    public void GlobMatch_MiddleStar()
    {
        Assert.IsTrue(FileIoController.GlobMatch("BACH*FUGUE", "BACH-CHROMATIC-FUGUE"));
        Assert.IsFalse(FileIoController.GlobMatch("BACH*FUGUE", "BACH-SONATA"));
    }

    [TestMethod]
    public void ParseFilterPattern_SimpleWildcard()
    {
        var result = FileIoController.ParseFilterPattern("*.mid");
        Assert.IsNull(result.DevicePrefix);
        Assert.IsNull(result.DirectoryPath);
        Assert.AreEqual("*", result.NamePattern);
        Assert.AreEqual(".mid", result.ExtFilter);
    }

    [TestMethod]
    public void ParseFilterPattern_WithDevice()
    {
        var result = FileIoController.ParseFilterPattern("FD0:*.sid");
        Assert.AreEqual("FD0", result.DevicePrefix);
        Assert.IsNull(result.DirectoryPath);
        Assert.AreEqual("*", result.NamePattern);
        Assert.AreEqual(".sid", result.ExtFilter);
    }

    [TestMethod]
    public void ParseFilterPattern_WithDeviceAndPath()
    {
        var result = FileIoController.ParseFilterPattern("FD0:songs/*.sid");
        Assert.AreEqual("FD0", result.DevicePrefix);
        Assert.AreEqual("songs", result.DirectoryPath);
        Assert.AreEqual("*", result.NamePattern);
        Assert.AreEqual(".sid", result.ExtFilter);
    }

    [TestMethod]
    public void ParseFilterPattern_NoExtension()
    {
        var result = FileIoController.ParseFilterPattern("BACH*");
        Assert.IsNull(result.DevicePrefix);
        Assert.IsNull(result.DirectoryPath);
        Assert.AreEqual("BACH*", result.NamePattern);
        Assert.IsNull(result.ExtFilter);
    }

    [TestMethod]
    public void ParseFilterPattern_NameAndExt()
    {
        var result = FileIoController.ParseFilterPattern("BACH*.mid");
        Assert.IsNull(result.DevicePrefix);
        Assert.IsNull(result.DirectoryPath);
        Assert.AreEqual("BACH*", result.NamePattern);
        Assert.AreEqual(".mid", result.ExtFilter);
    }

    [TestMethod]
    public void ParseFilterPattern_DevicePathAndPattern()
    {
        var result = FileIoController.ParseFilterPattern("HD:music/bach/*.mid");
        Assert.AreEqual("HD", result.DevicePrefix);
        Assert.AreEqual("music/bach", result.DirectoryPath);
        Assert.AreEqual("*", result.NamePattern);
        Assert.AreEqual(".mid", result.ExtFilter);
    }

    private static (FileIoController Fio, byte[] Memory, string TempDir) MakeControllerWithDevice()
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "fio_test_" + Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(tempDir);
        var memory = new byte[65536];
        var dm = new e6502.Storage.DeviceManager(tempDir, tempDir, tempDir);
        dm.DefaultDevice = "HD0";
        var fio = new FileIoController(
            address => memory[address],
            (address, data) => memory[address] = data,
            tempDir,
            deviceManager: dm);
        return (fio, memory, tempDir);
    }

    [TestMethod]
    public void DirOpen_FilteredByExtension_OnlyReturnsMid()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            File.WriteAllBytes(Path.Combine(tempDir, "song1.mid"), new byte[] { 0x4D, 0x54, 0x68, 0x64 });
            File.WriteAllBytes(Path.Combine(tempDir, "song2.mid"), new byte[] { 0x4D, 0x54, 0x68, 0x64 });
            File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
            File.WriteAllBytes(Path.Combine(tempDir, "tune.sid"), new byte[124]);

            SetFilename(fio, "*.mid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("song1", ReadFilename(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("song2", ReadFilename(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_FilteredByNameGlob()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            File.WriteAllBytes(Path.Combine(tempDir, "bach-fugue.mid"), new byte[4]);
            File.WriteAllBytes(Path.Combine(tempDir, "bach-sonata.mid"), new byte[4]);
            File.WriteAllBytes(Path.Combine(tempDir, "mozart-sonata.mid"), new byte[4]);

            SetFilename(fio, "bach*.mid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("bach-fugue", ReadFilename(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("bach-sonata", ReadFilename(fio));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_UnfilteredBackwardCompatible()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
            File.WriteAllBytes(Path.Combine(tempDir, "tune.sid"), new byte[124]);

            fio.Write((ushort)VgcConstants.FioNameLen, 0);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            var name = ReadFilename(fio);
            Assert.IsTrue(name.Length > 0);
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_NoMatches_ReturnsEndOfDir()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
            SetFilename(fio, "*.mid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_SidMetadata_PopulatesBuffer()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            var sid = new byte[124];
            sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44; // PSID
            sid[4] = 0x00; sid[5] = 0x02; // version 2
            sid[6] = 0x00; sid[7] = 0x7C; // data offset 124
            sid[8] = 0x10; sid[9] = 0x00; // load $1000 (BE)
            sid[10] = 0x10; sid[11] = 0x00; // init $1000
            sid[12] = 0x10; sid[13] = 0x03; // play $1003
            sid[14] = 0x00; sid[15] = 0x03; // 3 songs
            sid[16] = 0x00; sid[17] = 0x01; // start song
            var title = Encoding.ASCII.GetBytes("Test Song");
            Array.Copy(title, 0, sid, 22, title.Length);
            var author = Encoding.ASCII.GetBytes("Test Author");
            Array.Copy(author, 0, sid, 54, author.Length);
            File.WriteAllBytes(Path.Combine(tempDir, "mysong.sid"), sid);

            SetFilename(fio, "*.sid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(1, memory[VgcConstants.MetaType]);
            var titleBytes = new byte[9];
            Array.Copy(memory, VgcConstants.MetaTitle, titleBytes, 0, 9);
            Assert.AreEqual("Test Song", Encoding.ASCII.GetString(titleBytes));
            Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
            Assert.AreEqual(0x10, memory[VgcConstants.MetaLoadH]);
            Assert.AreEqual(3, memory[VgcConstants.MetaSongs]);
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_BinMetadata_PopulatesLoadAddress()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            File.WriteAllBytes(Path.Combine(tempDir, "prog.bin"), new byte[] { 0x00, 0x90, 0xEA, 0xEA });
            SetFilename(fio, "*.bin");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(2, memory[VgcConstants.MetaType]);
            Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
            Assert.AreEqual(0x90, memory[VgcConstants.MetaLoadH]);
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirRead_Filtered_PopulatesMetadataEachEntry()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            var sid1 = new byte[124];
            sid1[0] = 0x50; sid1[1] = 0x53; sid1[2] = 0x49; sid1[3] = 0x44;
            sid1[4] = 0x00; sid1[5] = 0x02; sid1[6] = 0x00; sid1[7] = 0x7C;
            var t1 = Encoding.ASCII.GetBytes("First Song");
            Array.Copy(t1, 0, sid1, 22, t1.Length);
            var sid2 = new byte[124];
            Array.Copy(sid1, sid2, 124);
            Array.Clear(sid2, 22, 32);
            var t2 = Encoding.ASCII.GetBytes("Second Song");
            Array.Copy(t2, 0, sid2, 22, t2.Length);
            File.WriteAllBytes(Path.Combine(tempDir, "aaa.sid"), sid1);
            File.WriteAllBytes(Path.Combine(tempDir, "bbb.sid"), sid2);

            SetFilename(fio, "*.sid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("First Song", Encoding.ASCII.GetString(memory, VgcConstants.MetaTitle, 10));

            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("Second Song", Encoding.ASCII.GetString(memory, VgcConstants.MetaTitle, 11));
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirRead_Unfiltered_DoesNotPopulateMetadata()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            var sid = new byte[124];
            sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44;
            sid[4] = 0x00; sid[5] = 0x02; sid[6] = 0x00; sid[7] = 0x7C;
            var t = Encoding.ASCII.GetBytes("Should Not Appear");
            Array.Copy(t, 0, sid, 22, t.Length);
            File.WriteAllBytes(Path.Combine(tempDir, "test.sid"), sid);

            fio.Write((ushort)VgcConstants.FioNameLen, 0);
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(0, memory[VgcConstants.MetaType]);
            Assert.AreEqual(0, memory[VgcConstants.MetaTitle]);
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void FilteredEnumeration_EndToEnd_MixedFileTypes()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            // Create a mix of files
            var sid = new byte[124];
            sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44; // PSID
            sid[4] = 0x00; sid[5] = 0x02;
            sid[6] = 0x00; sid[7] = 0x7C;
            var title = Encoding.ASCII.GetBytes("Cool SID Tune");
            Array.Copy(title, 0, sid, 22, title.Length);
            File.WriteAllBytes(Path.Combine(tempDir, "cool.sid"), sid);
            File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
            File.WriteAllBytes(Path.Combine(tempDir, "app.bin"), new byte[] { 0x00, 0x80, 0xEA });

            // Filter *.sid — should only get the SID file
            SetFilename(fio, "*.sid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("cool", ReadFilename(fio));
            Assert.AreEqual(1, memory[VgcConstants.MetaType]); // SID

            // Advance — should be end
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));

            // Filter *.bin — should only get the BIN file
            SetFilename(fio, "*.bin");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual("app", ReadFilename(fio));
            Assert.AreEqual(2, memory[VgcConstants.MetaType]); // BIN
            Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
            Assert.AreEqual(0x80, memory[VgcConstants.MetaLoadH]);

            // Filter * (no extension) — should get all 3 files
            SetFilename(fio, "*");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            int count = 1;
            while (true)
            {
                fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
                if (fio.Read((ushort)VgcConstants.FioStatus) != VgcConstants.FioStatusOk)
                    break;
                count++;
            }
            Assert.AreEqual(3, count);
        }
        finally { Directory.Delete(tempDir, true); }
    }

    [TestMethod]
    public void DirOpen_MidiMetadata_PopulatesBuffer()
    {
        var (fio, memory, tempDir) = MakeControllerWithDevice();
        try
        {
            var trackChunk = new TrackChunk(
                new SequenceTrackNameEvent("Test MIDI Song"),
                new CopyrightNoticeEvent("Test Composer"),
                new NoteOnEvent(
                    (SevenBitNumber)60,
                    (SevenBitNumber)100),
                new NoteOffEvent(
                    (SevenBitNumber)60,
                    (SevenBitNumber)0) { DeltaTime = 480 }
            );
            var midi = new MidiFile(trackChunk);
            midi.Write(Path.Combine(tempDir, "test.mid"));

            SetFilename(fio, "*.mid");
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(3, memory[VgcConstants.MetaType]); // MID
            Assert.AreEqual("Test MIDI Song", Encoding.ASCII.GetString(memory, VgcConstants.MetaTitle, 14));
            Assert.AreEqual("Test Composer", Encoding.ASCII.GetString(memory, VgcConstants.MetaAuthor, 13));
            Assert.IsTrue(memory[VgcConstants.MetaSongs] >= 1, $"Expected >= 1 tracks, got {memory[VgcConstants.MetaSongs]}");
            int dur = memory[VgcConstants.MetaDurL] | (memory[VgcConstants.MetaDurH] << 8);
            Assert.IsTrue(dur > 0, $"Duration should be > 0, got {dur}");
        }
        finally { Directory.Delete(tempDir, true); }
    }
}
