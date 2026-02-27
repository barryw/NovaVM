using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
}
