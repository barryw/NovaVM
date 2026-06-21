using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// Regression guard for the Arty Z7-20 PS Ethernet (Zynq GEM0 + Realtek RTL8211F +
// lwIP) RX fix. The data path took an enormous amount to bring up; these assert the
// two load-bearing fixes stay in the source so they can't be silently reverted.
// (End-to-end HW proof lives in boards/arty_z7/net_smoke.sh; this is the CI guard.)
// Root cause recap: (1) the BSP's get_Realtek_phy_speed read the RTL8211E PHYSR (reg
// 0x11) which fails on the F -> "Phy setup error" -> init_emacps aborts; (2) NET_CFG
// bit2 = NVLANDISC ("receive only VLAN frames") was set -> the MAC discarded every
// non-VLAN frame (all DHCP/ARP) -> RX read zero while TX worked.
[TestClass]
public class ArtyEthernetRegressionTests
{
    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }

    private static string ArtyPath(params string[] parts)
    {
        string[] prefix = { "e6502.FPGA", "boards", "arty_z7" };
        string[] all = new string[prefix.Length + parts.Length];
        prefix.CopyTo(all, 0);
        parts.CopyTo(all, prefix.Length);
        return RepoPath(all);
    }

    // net.c must clear NET_CFG bit2 (NVLANDISC) -- without this the GEM discards all
    // non-VLAN frames and RX is dead (TX unaffected). NET_CFG is at GEM offset 0x4.
    [TestMethod]
    public void NetC_ClearsNvlandiscBit()
    {
        string src = File.ReadAllText(ArtyPath("ps_fio", "src", "net.c"));
        StringAssert.Contains(src, "NVLANDISC",
            "net.c must document/clear the NVLANDISC bit (the RX-killer).");
        string compact = src.Replace(" ", "").Replace("\t", "");
        StringAssert.Contains(compact, "&~0x00000004",
            "net.c must clear NET_CFG bit2 (NVLANDISC = 0x4) via read-modify-write.");
        StringAssert.Contains(compact, "XEmacPs_WriteReg(base,0x4,",
            "the NVLANDISC clear must target NET_CFG at GEM offset 0x4.");
    }

    // build_ps_fio.py must patch the lwIP physpeed driver to read the RTL8211F's
    // PHYSR1 (extension page 0xa43, register 0x1a) for link/speed, instead of the
    // RTL8211E PHYSR (reg 0x11) which fails on the F and aborts GEM init.
    [TestMethod]
    public void BuildScript_PatchesRtl8211fSpeedRead()
    {
        string py = File.ReadAllText(ArtyPath("vitis", "build_ps_fio.py"));
        StringAssert.Contains(py, "patch_physpeed",
            "build_ps_fio.py must patch the lwIP physpeed driver for the RTL8211F.");
        StringAssert.Contains(py, "0xa43",
            "physpeed patch must select the RTL8211F PHYSR1 page (0xa43).");
        StringAssert.Contains(py, "0x1a",
            "physpeed patch must read the RTL8211F PHYSR1 register (0x1a).");
    }
}
