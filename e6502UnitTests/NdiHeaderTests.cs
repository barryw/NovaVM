using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace e6502UnitTests;

[TestClass]
public class NdiHeaderTests
{
    [TestMethod]
    public void RoundTrip_DefaultHeader()
    {
        var original = NdiHeader.Create("TEST DISK", 800);
        var bytes = original.ToBytes();
        var restored = NdiHeader.FromBytes(bytes);

        Assert.AreEqual(original.FormatVersion, restored.FormatVersion);
        Assert.AreEqual(original.SectorSize, restored.SectorSize);
        Assert.AreEqual(original.TotalSectors, restored.TotalSectors);
        Assert.AreEqual(original.VolumeLabel, restored.VolumeLabel);
        Assert.AreEqual(original.DirectoryStartSector, restored.DirectoryStartSector);
        Assert.AreEqual(original.DirectorySectorCount, restored.DirectorySectorCount);
        Assert.AreEqual(original.DataStartSector, restored.DataStartSector);
        Assert.AreEqual(original.FreeSectorCount, restored.FreeSectorCount);
    }

    [TestMethod]
    public void Create_800KB_HasCorrectSectorCount()
    {
        var header = NdiHeader.Create("MY DISK", 800);
        Assert.AreEqual(3200u, header.TotalSectors);
    }

    [TestMethod]
    public void Create_170KB_MinimumSize()
    {
        var header = NdiHeader.Create("TINY", 170);
        // 170 * 1024 / 256 = 680 sectors
        Assert.AreEqual(680u, header.TotalSectors);
    }

    [TestMethod]
    public void Create_64MB_UsesLongSectorFields()
    {
        var header = NdiHeader.Create("HARD DISK", 65536);

        Assert.AreEqual(262144u, header.TotalSectors);
        Assert.AreEqual(129u, header.DirectoryStartSector);
        Assert.AreEqual(48u, header.DirectorySectorCount);
        Assert.AreEqual(177u, header.DataStartSector);
        Assert.AreEqual(261967u, header.FreeSectorCount);
    }

    [TestMethod]
    public void InvalidMagic_Throws()
    {
        var bytes = new byte[256];
        bytes[0] = (byte)'B';
        bytes[1] = (byte)'A';
        bytes[2] = (byte)'D';
        bytes[3] = 0x1A;
        Assert.ThrowsException<InvalidDataException>(() => NdiHeader.FromBytes(bytes));
    }

    [TestMethod]
    public void VolumeLabel_TruncatedTo32()
    {
        var longLabel = new string('A', 50);
        var header = NdiHeader.Create(longLabel, 800);
        Assert.AreEqual(32, header.VolumeLabel.Length);
        Assert.AreEqual(new string('A', 32), header.VolumeLabel);
    }

    [TestMethod]
    public void ToBytes_Is256Bytes()
    {
        var header = NdiHeader.Create("DISK", 800);
        var bytes = header.ToBytes();
        Assert.AreEqual(256, bytes.Length);
    }

    [TestMethod]
    public void Create_800KB_SectorLayout()
    {
        var header = NdiHeader.Create("TEST", 800);
        // 3200 sectors, BAM = ceil(3200/2048) = 2 sectors
        // Directory starts at sector 1 + 2 = 3
        // Directory = 48 sectors
        // Data starts at 3 + 48 = 51
        // Free = 3200 - 51 = 3149
        Assert.AreEqual(3u, header.DirectoryStartSector);
        Assert.AreEqual(48u, header.DirectorySectorCount);
        Assert.AreEqual(51u, header.DataStartSector);
        Assert.AreEqual(3149u, header.FreeSectorCount);
    }

    [TestMethod]
    public void FreeSectorCount_CanBeUpdated()
    {
        var header = NdiHeader.Create("DISK", 800);
        header.FreeSectorCount = 100;
        Assert.AreEqual(100u, header.FreeSectorCount);
    }

    [TestMethod]
    public void FromBytes_TooShort_Throws()
    {
        var bytes = new byte[100]; // less than 256
        Assert.ThrowsException<InvalidDataException>(() => NdiHeader.FromBytes(bytes));
    }
}
