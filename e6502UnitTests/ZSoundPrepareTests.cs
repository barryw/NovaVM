using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Covers the Z-machine sampled-sound "prepare" semantics that let NovaZ load a
/// story's soundfont at boot: effect 1 (prepare) must load SOUND.PAK from the
/// mounted image without playing, matching the NovaHost hardware path. effect 2
/// (start) then plays.
/// </summary>
[TestClass]
public sealed class ZSoundPrepareTests
{
    // Minimal valid SOUND.PAK ("NZSP"): version 1, one sample (number 3),
    // 8000 Hz, 4 PCM bytes.
    private static byte[] MakeSoundPack()
    {
        return
        [
            (byte)'N', (byte)'Z', (byte)'S', (byte)'P', 0x01, 0x01,
            0x03,             // sound number 3
            0x40, 0x1F,       // rate = 8000 Hz
            0x00, 0x00, 0x00, 0x00, // pcm offset 0
            0x04, 0x00, 0x00, 0x00, // pcm length 4
            10, 20, 30, 40,   // pcm
        ];
    }

    private static void TriggerZSound(FileIoController fio, int number, int effect)
    {
        fio.Write((ushort)VgcConstants.FioSrcL, (byte)number);
        fio.Write((ushort)VgcConstants.FioSrcH, (byte)effect);
        fio.Write((ushort)VgcConstants.FioEndL, 0xFF); // level: loudest
        fio.Write((ushort)VgcConstants.FioEndH, 0x00); // repeats: once
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdZSound);
    }

    [TestMethod]
    public void Prepare_LoadsSoundPack_WithoutPlaying_ThenStartPlays()
    {
        string root = Path.Combine(Path.GetTempPath(), $"e6502-zsnd-{Guid.NewGuid():N}");
        string hd0 = Path.Combine(root, "hd0");
        string hd1 = Path.Combine(root, "hd1");
        string disks = Path.Combine(root, "disks");
        Directory.CreateDirectory(hd0);
        Directory.CreateDirectory(hd1);
        Directory.CreateDirectory(disks);
        DeviceManager? deviceManager = null;

        try
        {
            string imagePath = Path.Combine(disks, "fd0.ndi");
            NdiImage.CreateFormatted(imagePath, "TEST", 800);
            using (var image = NdiImage.Open(imagePath))
                image.WriteFile("SOUND.PAK", NdiFileType.Bin, 0xFFFF, MakeSoundPack());

            deviceManager = new DeviceManager(hd0, hd1, disks);
            deviceManager.AutoMount();
            deviceManager.DefaultDevice = deviceManager.SelectBootDevice();

            var memory = new byte[65536];
            var zsound = new ZSoundController(enableSound: false);
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                hd0,
                deviceManager: deviceManager,
                zsound: zsound);

            Assert.IsFalse(zsound.PackLoaded, "pack must not be loaded before any sound_effect");

            // effect 1 = prepare: loads the pack, does not play.
            TriggerZSound(fio, number: 0, effect: 1);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.IsTrue(zsound.PackLoaded, "prepare must load the soundfont");
            Assert.AreEqual(0, zsound.PlayCount, "prepare must not play");

            // effect 2 = start: plays sample 3.
            TriggerZSound(fio, number: 3, effect: 2);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
            Assert.AreEqual(1, zsound.PlayCount, "start must play once");
            Assert.AreEqual(3, zsound.LastPlayed);
        }
        finally
        {
            try { deviceManager?.GetDevice("FD0").Unmount(); } catch { /* best effort */ }
            Directory.Delete(root, recursive: true);
        }
    }
}
