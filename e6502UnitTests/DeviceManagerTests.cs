using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace e6502UnitTests;

[TestClass]
public class DeviceManagerTests
{
    private static string MakeTempDir()
    {
        string path = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        Directory.CreateDirectory(path);
        return path;
    }

    // -------------------------------------------------------------------------
    // 1. ParsePrefix_ExplicitDevice
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ParsePrefix_ExplicitDevice()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            var (device, path) = dm.ResolveFilename("HD0:MYFILE");

            Assert.AreEqual("HD0", device.Prefix);
            Assert.AreEqual("MYFILE", path);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 2. ParsePrefix_NoDevice_DefaultsToHD0
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ParsePrefix_NoDevice_DefaultsToHD0()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            var (device, path) = dm.ResolveFilename("MYFILE");

            Assert.AreEqual("HD0", device.Prefix);
            Assert.AreEqual("MYFILE", path);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 3. ParsePrefix_WithSubdirectory
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ParsePrefix_WithSubdirectory()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            var (device, path) = dm.ResolveFilename("FD0:GAMES/DEMO");

            Assert.AreEqual("FD0", device.Prefix);
            Assert.AreEqual("GAMES/DEMO", path);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 4. GetDevice_InvalidPrefix_Throws
    // -------------------------------------------------------------------------

    [TestMethod]
    public void GetDevice_InvalidPrefix_Throws()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            Assert.ThrowsException<IOException>(() => dm.GetDevice("XX9"));
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 5. AutoMount_MountsExistingNdi
    // -------------------------------------------------------------------------

    [TestMethod]
    public void AutoMount_MountsExistingNdi()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        DeviceManager? dm = null;
        try
        {
            // Create a formatted fd0.ndi in the disks directory
            string ndiPath = Path.Combine(disks, "fd0.ndi");
            NdiImage.CreateFormatted(ndiPath, "TEST", 800);

            dm = new DeviceManager(hd0, hd1, disks);
            Assert.IsFalse(dm.GetDevice("FD0").IsMounted, "FD0 should not be mounted before AutoMount");

            dm.AutoMount();

            Assert.IsTrue(dm.GetDevice("FD0").IsMounted, "FD0 should be mounted after AutoMount");
            Assert.IsFalse(dm.GetDevice("FD1").IsMounted, "FD1 should not be mounted (no fd1.ndi)");
        }
        finally
        {
            // Unmount to release file handles before deleting temp dirs on Windows.
            try
            {
                if (dm?.GetDevice("FD0") is NdiFloppyDevice fd0 && fd0.IsMounted)
                    fd0.Unmount();
            }
            catch { }

            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            try { Directory.Delete(disks, recursive: true); } catch { }
        }
    }

    // -------------------------------------------------------------------------
    // 6. FindAutoboot_ReturnsFirstMatch
    // -------------------------------------------------------------------------

    [TestMethod]
    public void FindAutoboot_ReturnsFirstMatch()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            // Put AUTOBOOT.bas in HD0 dir
            File.WriteAllBytes(Path.Combine(hd0, "AUTOBOOT.bas"), [0x01, 0x02]);

            var dm = new DeviceManager(hd0, hd1, disks);
            var result = dm.FindAutoboot();

            Assert.IsNotNull(result, "FindAutoboot should return a match");
            Assert.AreEqual("HD0", result!.Value.Device.Prefix);
            Assert.AreEqual("AUTOBOOT", result.Value.Filename);
            Assert.AreEqual(".bas", result.Value.Extension);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    [TestMethod]
    public void FindAutoboot_PrefersInsertedFloppyOverHardDrive()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        DeviceManager? dm = null;
        try
        {
            File.WriteAllBytes(Path.Combine(hd0, "AUTOBOOT.bas"), [0x01, 0x02]);

            string fd0Path = Path.Combine(disks, "fd0.ndi");
            NdiImage.CreateFormatted(fd0Path, "BOOTFD", 800);
            using (var img = NdiImage.Open(fd0Path))
                img.WriteFile("AUTOBOOT.bin", NdiFileType.Bin, 0xFFFF, [0x00, 0x72, 0x60]);

            dm = new DeviceManager(hd0, hd1, disks);
            dm.AutoMount();

            var result = dm.FindAutoboot();

            Assert.IsNotNull(result, "FindAutoboot should return a match");
            Assert.AreEqual("FD0", result!.Value.Device.Prefix);
            Assert.AreEqual("AUTOBOOT", result.Value.Filename);
            Assert.AreEqual(".bin", result.Value.Extension);
        }
        finally
        {
            try
            {
                if (dm?.GetDevice("FD0") is NdiFloppyDevice fd0 && fd0.IsMounted)
                    fd0.Unmount();
            }
            catch { }

            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            try { Directory.Delete(disks, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void SelectBootDevice_PrefersInsertedFloppyWhenNoAutobootExists()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        DeviceManager? dm = null;
        try
        {
            string fd0Path = Path.Combine(disks, "fd0.ndi");
            NdiImage.CreateFormatted(fd0Path, "BOOTFD", 800);

            dm = new DeviceManager(hd0, hd1, disks);
            dm.AutoMount();

            Assert.AreEqual("FD0", dm.SelectBootDevice());
        }
        finally
        {
            try
            {
                if (dm?.GetDevice("FD0") is NdiFloppyDevice fd0 && fd0.IsMounted)
                    fd0.Unmount();
            }
            catch { }

            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            try { Directory.Delete(disks, recursive: true); } catch { }
        }
    }

    // -------------------------------------------------------------------------
    // 7. DefaultDevice can be changed and affects ResolveFilename
    // -------------------------------------------------------------------------

    [TestMethod]
    public void DefaultDevice_Change_AffectsResolveFilename()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            dm.DefaultDevice = "HD1";

            var (device, path) = dm.ResolveFilename("SOMEFILE");
            Assert.AreEqual("HD1", device.Prefix);
            Assert.AreEqual("SOMEFILE", path);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 8. MountDevice and UnmountDevice
    // -------------------------------------------------------------------------

    [TestMethod]
    public void MountDevice_And_UnmountDevice()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            string ndiPath = Path.Combine(disks, "myimage.ndi");
            NdiImage.CreateFormatted(ndiPath, "MYIMAGE", 800);

            var dm = new DeviceManager(hd0, hd1, disks);
            Assert.IsFalse(dm.GetDevice("FD1").IsMounted);

            dm.MountDevice("FD1", "myimage");
            Assert.IsTrue(dm.GetDevice("FD1").IsMounted);

            dm.UnmountDevice("FD1");
            Assert.IsFalse(dm.GetDevice("FD1").IsMounted);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            try { Directory.Delete(disks, recursive: true); } catch { }
        }
    }

    // -------------------------------------------------------------------------
    // 9. FormatDevice creates and mounts a new image
    // -------------------------------------------------------------------------

    [TestMethod]
    public void FormatDevice_CreatesAndMounts()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            Assert.IsFalse(dm.GetDevice("FD2").IsMounted);

            dm.FormatDevice("FD2", "NEWDISK", 800);

            Assert.IsTrue(dm.GetDevice("FD2").IsMounted, "FD2 should be mounted after FormatDevice");
            Assert.IsTrue(File.Exists(Path.Combine(disks, "fd2.ndi")), "fd2.ndi should exist in disksDir");
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            try { Directory.Delete(disks, recursive: true); } catch { }
        }
    }

    // -------------------------------------------------------------------------
    // 10. GetDevice is case-insensitive
    // -------------------------------------------------------------------------

    [TestMethod]
    public void GetDevice_CaseInsensitive()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            var dev = dm.GetDevice("hd0");
            Assert.AreEqual("HD0", dev.Prefix);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }

    // -------------------------------------------------------------------------
    // 11. FindAutoboot with no autoboot files returns null
    // -------------------------------------------------------------------------

    [TestMethod]
    public void FindAutoboot_NoFiles_ReturnsNull()
    {
        string hd0 = MakeTempDir();
        string hd1 = MakeTempDir();
        string disks = MakeTempDir();
        try
        {
            var dm = new DeviceManager(hd0, hd1, disks);
            var result = dm.FindAutoboot();
            Assert.IsNull(result);
        }
        finally
        {
            Directory.Delete(hd0, recursive: true);
            Directory.Delete(hd1, recursive: true);
            Directory.Delete(disks, recursive: true);
        }
    }
}
