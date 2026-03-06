using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiBamTests
{
    [TestMethod]
    public void NewBam_AllFree()
    {
        var bam = new NdiBam(100);
        Assert.AreEqual(100, bam.FreeCount);
        for (int i = 0; i < 100; i++)
            Assert.IsFalse(bam.IsAllocated(i));
    }

    [TestMethod]
    public void AllocateContiguous_ReturnsConsecutiveSectors()
    {
        var bam = new NdiBam(100);
        int start = bam.AllocateContiguous(10);
        Assert.AreEqual(0, start);
        Assert.AreEqual(90, bam.FreeCount);
        for (int i = 0; i < 10; i++)
            Assert.IsTrue(bam.IsAllocated(i));
        for (int i = 10; i < 100; i++)
            Assert.IsFalse(bam.IsAllocated(i));
    }

    [TestMethod]
    public void Free_ReleaseSectors()
    {
        var bam = new NdiBam(100);
        bam.AllocateContiguous(5); // sectors 0-4
        Assert.AreEqual(95, bam.FreeCount);

        bam.Free(1, 3); // free sectors 1, 2, 3
        Assert.AreEqual(98, bam.FreeCount);
        Assert.IsTrue(bam.IsAllocated(0));
        Assert.IsFalse(bam.IsAllocated(1));
        Assert.IsFalse(bam.IsAllocated(2));
        Assert.IsFalse(bam.IsAllocated(3));
        Assert.IsTrue(bam.IsAllocated(4));
    }

    [TestMethod]
    public void AllocateContiguous_SkipsUsedSectors()
    {
        var bam = new NdiBam(100);
        bam.AllocateContiguous(5); // sectors 0-4
        int start = bam.AllocateContiguous(3);
        Assert.AreEqual(5, start);
        Assert.AreEqual(92, bam.FreeCount);
        for (int i = 5; i < 8; i++)
            Assert.IsTrue(bam.IsAllocated(i));
    }

    [TestMethod]
    public void AllocateContiguous_NotEnoughSpace_ReturnsNegative()
    {
        var bam = new NdiBam(10);
        bam.AllocateContiguous(10); // fill completely
        Assert.AreEqual(0, bam.FreeCount);
        int result = bam.AllocateContiguous(1);
        Assert.AreEqual(-1, result);
    }

    [TestMethod]
    public void AllocateContiguous_FragmentedSpace_FirstFit()
    {
        var bam = new NdiBam(20);
        bam.AllocateContiguous(10); // 0-9 used
        bam.Free(0, 3);             // 0-2 free, 3-9 used, 10-19 free
        // Only contiguous runs: 0-2 (3 sectors), 10-19 (10 sectors)
        // Requesting 5 should skip the hole at 0-2 and land at 10
        int start = bam.AllocateContiguous(5);
        Assert.AreEqual(10, start);
    }

    [TestMethod]
    public void RoundTrip_Bytes()
    {
        var bam = new NdiBam(200);
        bam.AllocateContiguous(50);  // 0-49
        bam.Free(10, 5);             // free 10-14

        byte[] bytes = bam.ToBytes();
        var restored = NdiBam.FromBytes(bytes, 200);

        Assert.AreEqual(bam.FreeCount, restored.FreeCount);
        for (int i = 0; i < 200; i++)
            Assert.AreEqual(bam.IsAllocated(i), restored.IsAllocated(i), $"Mismatch at sector {i}");
    }
}
