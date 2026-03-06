using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class NdiDirectoryTests
{
    // 48 sectors * 256 bytes = 12288 bytes = 192 entries max
    private const int DirSectorCount = 48;

    [TestMethod]
    public void AddFile_RoundTrip()
    {
        var dir = new NdiDirectory(DirSectorCount);
        int idx = dir.AddEntry("myfile.bas", NdiFileType.Bas, 0xFFFF, startSector: 51, sizeBytes: 1024, sectorCount: 4);

        var entry = dir.GetEntry(idx);

        Assert.AreEqual(idx, entry.Index);
        Assert.AreEqual("myfile.bas", entry.Filename);
        Assert.AreEqual(NdiFileType.Bas, entry.FileType);
        Assert.AreEqual((ushort)0xFFFF, entry.ParentIndex);
        Assert.AreEqual((ushort)51, entry.StartSector);
        Assert.AreEqual((ushort)1024, entry.SizeBytes);
        Assert.AreEqual((ushort)4, entry.SectorCount);
        Assert.IsTrue(entry.IsActive);
        Assert.IsFalse(entry.IsDirectory);
    }

    [TestMethod]
    public void AddDirectory_IsDirectory()
    {
        var dir = new NdiDirectory(DirSectorCount);
        int idx = dir.AddDirectory("subdir", parentIndex: 0xFFFF);

        var entry = dir.GetEntry(idx);

        Assert.IsTrue(entry.IsActive);
        Assert.IsTrue(entry.IsDirectory);
        Assert.AreEqual("subdir", entry.Filename);
        Assert.AreEqual(NdiFileType.Bas, entry.FileType); // default type
        Assert.AreEqual((ushort)0xFFFF, entry.ParentIndex);
    }

    [TestMethod]
    public void ListEntries_InParent()
    {
        var dir = new NdiDirectory(DirSectorCount);

        // Add two files to root
        dir.AddEntry("root1.bas", NdiFileType.Bas, 0xFFFF, 51, 512, 2);
        dir.AddEntry("root2.sid", NdiFileType.Sid, 0xFFFF, 53, 2048, 8);

        // Add a subdirectory and a file inside it
        int subIdx = dir.AddDirectory("sub", 0xFFFF);
        dir.AddEntry("child.mid", NdiFileType.Mid, (ushort)subIdx, 61, 100, 1);

        var rootEntries = dir.ListEntries(0xFFFF);
        var subEntries = dir.ListEntries((ushort)subIdx);

        Assert.AreEqual(3, rootEntries.Length); // root1, root2, sub
        Assert.AreEqual(1, subEntries.Length);
        Assert.AreEqual("child.mid", subEntries[0].Filename);
    }

    [TestMethod]
    public void RemoveEntry_MarksInactive()
    {
        var dir = new NdiDirectory(DirSectorCount);
        int idx = dir.AddEntry("toremove.bin", NdiFileType.Bin, 0xFFFF, 51, 256, 1);

        dir.RemoveEntry(idx);

        var listing = dir.ListEntries(0xFFFF);
        Assert.AreEqual(0, listing.Length);

        // GetEntry on a removed slot should reflect inactive state
        var entry = dir.GetEntry(idx);
        Assert.IsFalse(entry.IsActive);
    }

    [TestMethod]
    public void FindEntry_ByNameAndParent()
    {
        var dir = new NdiDirectory(DirSectorCount);
        dir.AddEntry("HELLO.BAS", NdiFileType.Bas, 0xFFFF, 51, 100, 1);

        // Case-insensitive match
        int found = dir.FindEntry("hello.bas", 0xFFFF);
        Assert.AreNotEqual(-1, found);

        // Wrong parent
        int notFound = dir.FindEntry("hello.bas", 5);
        Assert.AreEqual(-1, notFound);

        // Non-existent name
        int missing = dir.FindEntry("ghost.bas", 0xFFFF);
        Assert.AreEqual(-1, missing);
    }

    [TestMethod]
    public void RoundTrip_Bytes()
    {
        var dir = new NdiDirectory(DirSectorCount);
        dir.AddEntry("file1.bas", NdiFileType.Bas, 0xFFFF, 51, 512, 2);
        dir.AddEntry("file2.gfx", NdiFileType.Gfx, 0xFFFF, 53, 4096, 16);
        int subIdx = dir.AddDirectory("mydir", 0xFFFF);
        dir.AddEntry("inner.mid", NdiFileType.Mid, (ushort)subIdx, 69, 200, 1);

        byte[] bytes = dir.ToBytes();
        var restored = NdiDirectory.FromBytes(bytes, DirSectorCount);

        var rootEntries = restored.ListEntries(0xFFFF);
        var subEntries = restored.ListEntries((ushort)subIdx);

        Assert.AreEqual(3, rootEntries.Length);
        Assert.AreEqual(1, subEntries.Length);

        var f1 = rootEntries.First(e => e.Filename == "file1.bas");
        Assert.AreEqual(NdiFileType.Bas, f1.FileType);
        Assert.AreEqual((ushort)512, f1.SizeBytes);
        Assert.AreEqual((ushort)2, f1.SectorCount);

        var f2 = rootEntries.First(e => e.Filename == "file2.gfx");
        Assert.AreEqual(NdiFileType.Gfx, f2.FileType);
        Assert.AreEqual((ushort)4096, f2.SizeBytes);

        Assert.AreEqual("inner.mid", subEntries[0].Filename);
        Assert.AreEqual(NdiFileType.Mid, subEntries[0].FileType);
    }

    [TestMethod]
    public void Filename_TruncatedTo32Chars()
    {
        var dir = new NdiDirectory(DirSectorCount);
        string longName = new string('X', 50);
        int idx = dir.AddEntry(longName, NdiFileType.Bas, 0xFFFF, 51, 100, 1);

        var entry = dir.GetEntry(idx);
        Assert.AreEqual(32, entry.Filename.Length);
    }

    [TestMethod]
    public void ToBytes_SizeMatchesSectors()
    {
        var dir = new NdiDirectory(DirSectorCount);
        byte[] bytes = dir.ToBytes();
        Assert.AreEqual(DirSectorCount * 256, bytes.Length);
    }
}
