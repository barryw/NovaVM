using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class NdiImageTests
{
    private string TempPath() => Path.Combine(Path.GetTempPath(), Path.GetRandomFileName() + ".ndi");

    [TestMethod]
    public void Create_And_Open_RoundTrip()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST DISK", 800);

            using var img = NdiImage.Open(path);
            Assert.AreEqual("TEST DISK", img.Header.VolumeLabel);
            Assert.AreEqual(3200, img.Header.TotalSectors);
            Assert.AreEqual(3149, img.FreeSectors);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_ReadFile_RoundTrip()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            byte[] data = [1, 2, 3, 4, 5, 6, 7, 8];
            img.WriteFile("hello.bas", NdiFileType.Bas, 0xFFFF, data);

            byte[] result = img.ReadFile("hello.bas", 0xFFFF);
            CollectionAssert.AreEqual(data, result);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_InSubdirectory()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            int dirIndex = img.MakeDirectory("subdir", 0xFFFF);
            byte[] data = [10, 20, 30];
            img.WriteFile("file.bin", NdiFileType.Bin, (ushort)dirIndex, data);

            // File not visible in root
            var rootEntries = img.ListDirectory(0xFFFF);
            Assert.AreEqual(1, rootEntries.Length); // only the directory
            Assert.AreEqual("subdir", rootEntries[0].Filename);

            // File visible in subdir
            var subEntries = img.ListDirectory((ushort)dirIndex);
            Assert.AreEqual(1, subEntries.Length);
            Assert.AreEqual("file.bin", subEntries[0].Filename);

            byte[] result = img.ReadFile("file.bin", (ushort)dirIndex);
            CollectionAssert.AreEqual(data, result);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void DeleteFile_FreesSpace()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            int freeBefore = img.FreeSectors;
            byte[] data = new byte[512]; // 2 sectors
            img.WriteFile("big.bin", NdiFileType.Bin, 0xFFFF, data);
            int freeAfterWrite = img.FreeSectors;
            Assert.IsTrue(freeAfterWrite < freeBefore);

            img.DeleteFile("big.bin", 0xFFFF);
            Assert.AreEqual(freeBefore, img.FreeSectors);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void ListDirectory_ReturnsEntries()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            img.WriteFile("a.bas", NdiFileType.Bas, 0xFFFF, [1, 2, 3]);
            img.WriteFile("b.sid", NdiFileType.Sid, 0xFFFF, [4, 5, 6]);
            img.MakeDirectory("mydir", 0xFFFF);

            var entries = img.ListDirectory(0xFFFF);
            Assert.AreEqual(3, entries.Length);

            var names = entries.Select(e => e.Filename).ToArray();
            CollectionAssert.Contains(names, "a.bas");
            CollectionAssert.Contains(names, "b.sid");
            CollectionAssert.Contains(names, "mydir");
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void LargeFile_SpansMultipleSectors()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            byte[] data = new byte[1000];
            for (int i = 0; i < data.Length; i++)
                data[i] = (byte)(i & 0xFF);

            img.WriteFile("large.bin", NdiFileType.Bin, 0xFFFF, data);
            byte[] result = img.ReadFile("large.bin", 0xFFFF);

            Assert.AreEqual(1000, result.Length);
            CollectionAssert.AreEqual(data, result);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void RemoveDirectory_EmptyDirectory_Succeeds()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            img.MakeDirectory("emptydir", 0xFFFF);
            img.RemoveDirectory("emptydir", 0xFFFF);

            var entries = img.ListDirectory(0xFFFF);
            Assert.AreEqual(0, entries.Length);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void RemoveDirectory_NonEmpty_Throws()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);

            int dirIndex = img.MakeDirectory("populated", 0xFFFF);
            img.WriteFile("child.bas", NdiFileType.Bas, (ushort)dirIndex, [1, 2, 3]);

            Assert.ThrowsException<InvalidOperationException>(
                () => img.RemoveDirectory("populated", 0xFFFF));
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void Persistence_SurvivedClose_Reopen()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "PERSIST", 800);

            using (var img = NdiImage.Open(path))
            {
                img.WriteFile("data.bin", NdiFileType.Bin, 0xFFFF, [9, 8, 7, 6]);
            }

            using var img2 = NdiImage.Open(path);
            byte[] result = img2.ReadFile("data.bin", 0xFFFF);
            CollectionAssert.AreEqual(new byte[] { 9, 8, 7, 6 }, result);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_Over64KB_RoundTrips()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "BIG", 800);
            using var img = NdiImage.Open(path);
            var bigData = new byte[100_000];
            new Random(42).NextBytes(bigData);
            img.WriteFile("HUGE", NdiFileType.Bin, 0xFFFF, bigData);

            byte[] read = img.ReadFile("HUGE", 0xFFFF);
            CollectionAssert.AreEqual(bigData, read);

            var entry = img.ListDirectory(0xFFFF).First(e => e.Filename == "HUGE");
            Assert.AreEqual(100_000, entry.SizeBytes);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_DiskFull_Throws()
    {
        string path = TempPath();
        try
        {
            // 170KB disk has ~630 free data sectors
            NdiImage.CreateFormatted(path, "TINY", 170);
            using var img = NdiImage.Open(path);

            // Fill the disk with large files (each up to 256 sectors)
            // to avoid exhausting the directory (192 entries) before the BAM
            int fileNum = 0;
            while (img.FreeSectors > 0)
            {
                int sectorsTaken = Math.Min(img.FreeSectors, 256);
                int writeBytes = sectorsTaken * 256;
                img.WriteFile($"F{fileNum++:D3}", NdiFileType.Bin, 0xFFFF, new byte[writeBytes]);
            }

            // BAM is exhausted — any write must fail
            Assert.ThrowsException<InvalidOperationException>(() =>
                img.WriteFile("EXTRA", NdiFileType.Bas, 0xFFFF, new byte[1]));
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void ReadFile_DirectoryEntry_Throws()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);
            img.MakeDirectory("MYDIR", 0xFFFF);
            Assert.ThrowsException<InvalidOperationException>(() =>
                img.ReadFile("MYDIR", 0xFFFF));
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void ReadFile_NotFound_Throws()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);
            Assert.ThrowsException<FileNotFoundException>(() =>
                img.ReadFile("NOPE", 0xFFFF));
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void DeleteFile_ThenRewrite()
    {
        string path = TempPath();
        try
        {
            NdiImage.CreateFormatted(path, "TEST", 800);
            using var img = NdiImage.Open(path);
            img.WriteFile("TEMP", NdiFileType.Bas, 0xFFFF, new byte[] { 1, 2, 3 });
            int freeAfterWrite = img.FreeSectors;
            img.DeleteFile("TEMP", 0xFFFF);
            int freeAfterDelete = img.FreeSectors;
            Assert.IsTrue(freeAfterDelete > freeAfterWrite);
            // Write again — should reuse freed space
            img.WriteFile("TEMP2", NdiFileType.Bas, 0xFFFF, new byte[] { 4, 5, 6 });
            Assert.AreEqual(freeAfterWrite, img.FreeSectors);
        }
        finally { File.Delete(path); }
    }
}
