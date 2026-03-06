using System;
using System.IO;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.Storage;

namespace e6502UnitTests;

[TestClass]
public class HostDirectoryDeviceTests
{
    private static string MakeTempDir()
        => Path.Combine(Path.GetTempPath(), $"e6502-hdd-{Guid.NewGuid():N}");

    [TestMethod]
    public void SaveAndLoad_RoundTrip()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");

            byte[] data = [0x01, 0x02, 0x03, 0xFF];
            device.Save("HELLO", data, ".bas");

            byte[] loaded = device.Load("HELLO", ".bas");
            CollectionAssert.AreEqual(data, loaded);
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Subdirectory_CdAndList()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");

            // Create a subdir and verify it appears in root listing
            device.MakeDirectory("GAMES");
            StorageDirEntry[] rootEntries = device.ListDirectory(null);
            Assert.IsTrue(rootEntries.Any(e => e.Filename == "GAMES" && e.IsDirectory),
                "GAMES subdir should appear in root listing");

            // CD into subdir, save a file, list it
            device.CurrentDirectory = "GAMES";
            byte[] data = [0xDE, 0xAD, 0xBE, 0xEF];
            device.Save("TUNE", data, ".sid");

            StorageDirEntry[] gameEntries = device.ListDirectory(null);
            Assert.AreEqual(1, gameEntries.Length, "Should see one file in GAMES");
            Assert.AreEqual("TUNE", gameEntries[0].Filename);
            Assert.IsFalse(gameEntries[0].IsDirectory);
            Assert.AreEqual(NdiFileType.Sid, gameEntries[0].FileType);

            // CD back to root, listing should still show GAMES (subdir) not TUNE
            device.CurrentDirectory = "/";
            StorageDirEntry[] backAtRoot = device.ListDirectory(null);
            Assert.IsTrue(backAtRoot.Any(e => e.Filename == "GAMES" && e.IsDirectory));
            Assert.IsFalse(backAtRoot.Any(e => e.Filename == "TUNE"));
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Delete_RemovesFile()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");

            byte[] data = [0x42];
            device.Save("TEMP", data, ".bin");
            Assert.IsTrue(device.FileExists("TEMP", ".bin"));

            device.Delete("TEMP", ".bin");
            Assert.IsFalse(device.FileExists("TEMP", ".bin"));

            Assert.ThrowsException<FileNotFoundException>(() => device.Load("TEMP", ".bin"));
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Load_NotFound_Throws()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");

            Assert.ThrowsException<FileNotFoundException>(() => device.Load("MISSING", ".bas"));
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void ListDirectory_SubdirsFirstThenFilesSortedByName()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");

            device.Save("ZEBRA", [1], ".bas");
            device.Save("ALPHA", [2], ".bas");
            device.MakeDirectory("BDIR");

            StorageDirEntry[] entries = device.ListDirectory(null);
            // BDIR (directory) should come before ALPHA and ZEBRA (files)
            Assert.AreEqual("BDIR", entries[0].Filename);
            Assert.AreEqual("ALPHA", entries[1].Filename);
            Assert.AreEqual("ZEBRA", entries[2].Filename);
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Format_Throws_NotSupported()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            Assert.ThrowsException<NotSupportedException>(() => device.Format("LABEL", 1440));
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Mount_Throws_NotSupported()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            Assert.ThrowsException<NotSupportedException>(() => device.Mount("some.ndi"));
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void Unmount_Throws_NotSupported()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            Assert.ThrowsException<NotSupportedException>(() => device.Unmount());
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void IsMounted_AlwaysTrue()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            Assert.IsTrue(device.IsMounted);
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void CurrentDirectory_DefaultsToRoot()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            Assert.AreEqual("/", device.CurrentDirectory);
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }

    [TestMethod]
    public void RemoveDirectory_DeletesEmpty()
    {
        string dir = MakeTempDir();
        try
        {
            Directory.CreateDirectory(dir);
            var device = new HostDirectoryDevice(dir, "FD0");
            device.MakeDirectory("EMPTY");
            device.RemoveDirectory("EMPTY");
            Assert.AreEqual(0, device.ListDirectory(null).Length);
        }
        finally
        {
            Directory.Delete(dir, recursive: true);
        }
    }
}
