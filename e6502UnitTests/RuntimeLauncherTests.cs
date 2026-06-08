using System;
using System.Text;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// The runtime launcher is the AUTOBOOT.bin that swaps BASIC for a named /roms
/// runtime. These lock the critical byte structure; the end-to-end behavior is
/// proven by the boot/swap integration test.
/// </summary>
[TestClass]
public class RuntimeLauncherTests
{
    private static bool Contains(byte[] hay, params byte[] needle)
    {
        for (int i = 0; i + needle.Length <= hay.Length; i++)
        {
            bool ok = true;
            for (int j = 0; j < needle.Length; j++)
                if (hay[i + j] != needle[j]) { ok = false; break; }
            if (ok) return true;
        }
        return false;
    }

    [TestMethod]
    public void Build_HasLoadHeader_Name_AndColdBoot()
    {
        byte[] bin = RuntimeLauncher.Build("STUB");

        // 2-byte load-address header = $7200 (RAM, < $C000)
        Assert.AreEqual(0x00, bin[0]);
        Assert.AreEqual(0x72, bin[1]);

        // embedded runtime name at the tail
        Assert.AreEqual("STUB", Encoding.ASCII.GetString(bin, bin.Length - 4, 4));

        // issues FIO_CMD_LOADRUNTIME: LDA #$28 ; STA $B9A0
        Assert.IsTrue(Contains(bin, 0xA9, 0x28, 0x8D, 0xA0, 0xB9), "must issue LOADRUNTIME");

        // cold-boots the new runtime: JMP ($FFFC)
        Assert.IsTrue(Contains(bin, 0x6C, 0xFC, 0xFF), "must cold-boot via reset vector");

        // name-copy reads from $7225 = LoadAddress + CodeLength
        Assert.IsTrue(Contains(bin, 0xBD, 0x25, 0x72), "must read name from $7225");
    }

    [TestMethod]
    public void Build_EncodesNameLength()
    {
        byte[] bin = RuntimeLauncher.Build("NOVAFORTH"); // length 9
        Assert.IsTrue(Contains(bin, 0xE0, 0x09), "CPX #namelen");
        Assert.AreEqual("NOVAFORTH", Encoding.ASCII.GetString(bin, bin.Length - 9, 9));
    }

    [TestMethod]
    public void Build_RejectsEmptyName()
        => Assert.ThrowsException<ArgumentException>(() => RuntimeLauncher.Build(""));
}
