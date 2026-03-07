using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class NdiFloppyDeviceTests
{
    private static string TempNdi() =>
        Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");

    private static void CreateDisk(string path)
        => NdiImage.CreateFormatted(path, "TEST", 800);

    // -------------------------------------------------------------------------
    // 1. Mount/Save/Load round trip
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Mount_SaveLoad_RoundTrip()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);

            var dev = new NdiFloppyDevice("F");
            Assert.IsFalse(dev.IsMounted);

            dev.Mount(path);
            Assert.IsTrue(dev.IsMounted);

            byte[] data = [1, 2, 3, 4, 5];
            dev.Save("HELLO", data, ".bas");

            byte[] loaded = dev.Load("HELLO", ".bas");
            CollectionAssert.AreEqual(data, loaded);

            dev.Unmount();
            Assert.IsFalse(dev.IsMounted);
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 2. Subdirectory: cd, save, list, cd back
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Subdirectory_CdAndAccess()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);

            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            // Create subdir and navigate into it
            dev.MakeDirectory("MUSIC");
            dev.CurrentDirectory = "MUSIC";
            Assert.AreEqual("MUSIC", dev.CurrentDirectory);

            // Save a file in the subdir
            byte[] song = [0xDE, 0xAD, 0xBE, 0xEF];
            dev.Save("TUNE", song, ".bin");

            // List the subdir — should contain the file
            var entries = dev.ListDirectory(null);
            Assert.AreEqual(1, entries.Length);
            Assert.AreEqual("TUNE", entries[0].Filename);
            Assert.IsFalse(entries[0].IsDirectory);

            // Navigate back to root
            dev.CurrentDirectory = "/";
            Assert.AreEqual("/", dev.CurrentDirectory);

            // Root listing should contain only the MUSIC directory, not TUNE
            var rootEntries = dev.ListDirectory(null);
            Assert.AreEqual(1, rootEntries.Length);
            Assert.AreEqual("MUSIC", rootEntries[0].Filename);
            Assert.IsTrue(rootEntries[0].IsDirectory);

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 3. Operations on unmounted device throw IOException
    // -------------------------------------------------------------------------

    [TestMethod]
    public void NotMounted_Throws()
    {
        var dev = new NdiFloppyDevice("F");

        Assert.ThrowsException<IOException>(() => dev.Load("X", ".bas"));
        Assert.ThrowsException<IOException>(() => dev.Save("X", [1], ".bas"));
        Assert.ThrowsException<IOException>(() => dev.Delete("X", ".bas"));
        Assert.ThrowsException<IOException>(() => dev.ListDirectory(null));
        Assert.ThrowsException<IOException>(() => dev.MakeDirectory("X"));
        Assert.ThrowsException<IOException>(() => dev.RemoveDirectory("X"));
        Assert.ThrowsException<IOException>(() => dev.FileExists("X", ".bas"));
    }

    // -------------------------------------------------------------------------
    // 4. Format throws NotSupportedException
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Format_ThrowsNotSupported()
    {
        var dev = new NdiFloppyDevice("F");
        Assert.ThrowsException<NotSupportedException>(() => dev.Format("X", 800));
    }

    // -------------------------------------------------------------------------
    // 5. FileExists works correctly
    // -------------------------------------------------------------------------

    [TestMethod]
    public void FileExists_ReturnsCorrectResults()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);
            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            Assert.IsFalse(dev.FileExists("MISSING", ".bas"));

            dev.Save("PRESENT", [42], ".bas");
            Assert.IsTrue(dev.FileExists("PRESENT", ".bas"));

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 6. Delete removes the file
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Delete_RemovesFile()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);
            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            dev.Save("TODEL", [9, 8, 7], ".bin");
            Assert.IsTrue(dev.FileExists("TODEL", ".bin"));

            dev.Delete("TODEL", ".bin");
            Assert.IsFalse(dev.FileExists("TODEL", ".bin"));

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 7. Save/Load with extension disambiguates same base name
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Save_Load_WithExtension_Disambiguates()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);
            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            dev.Save("TUNE", new byte[] { 1, 2, 3 }, ".bas");
            dev.Save("TUNE", new byte[] { 4, 5, 6, 7 }, ".sid");

            var basData = dev.Load("TUNE", ".bas");
            var sidData = dev.Load("TUNE", ".sid");

            Assert.AreEqual(3, basData.Length);
            Assert.AreEqual(4, sidData.Length);
            Assert.AreEqual(1, basData[0]);
            Assert.AreEqual(4, sidData[0]);

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 8. Multi-level directory navigation
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CurrentDirectory_MultiLevel()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);
            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            // Create nested directories: MUSIC/BACH
            dev.MakeDirectory("MUSIC");
            dev.CurrentDirectory = "MUSIC";
            dev.MakeDirectory("BACH");
            dev.CurrentDirectory = "/";

            // Navigate to nested directory
            dev.CurrentDirectory = "MUSIC/BACH";
            Assert.AreEqual("MUSIC/BACH", dev.CurrentDirectory);

            // Save file in nested dir
            dev.Save("FUGUE", new byte[] { 10, 20, 30 }, ".bas");

            // Go back to root, verify file is not there
            dev.CurrentDirectory = "/";
            Assert.IsFalse(dev.FileExists("FUGUE", ".bas"));

            // Navigate back, verify file IS there
            dev.CurrentDirectory = "MUSIC/BACH";
            Assert.IsTrue(dev.FileExists("FUGUE", ".bas"));

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }

    // -------------------------------------------------------------------------
    // 9. Setting CurrentDirectory to non-existent path throws
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CurrentDirectory_NonExistent_Throws()
    {
        string path = TempNdi();
        try
        {
            CreateDisk(path);
            var dev = new NdiFloppyDevice("F");
            dev.Mount(path);

            Assert.ThrowsException<DirectoryNotFoundException>(() =>
                dev.CurrentDirectory = "NOPE");

            dev.Unmount();
        }
        finally { File.Delete(path); }
    }
}
