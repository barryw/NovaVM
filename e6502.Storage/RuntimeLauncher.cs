using System;
using System.Text;

namespace e6502.Storage;

/// <summary>
/// Generates the tiny 6502 "runtime launcher" that ships as a boot floppy's
/// AUTOBOOT.bin. BASIC's existing .bin autoboot loads it to RAM ($7200) and
/// jumps to it; the launcher then asks the host to load a named runtime from
/// /roms into the $C000 bank (FIO_CMD_LOADRUNTIME) and cold-boots it via
/// JMP ($FFFC). It runs from RAM so it survives the ROM-bank overwrite.
///
/// FIO MMIO ABI mirrors e6502.Avalonia VgcConstants / runtime/asm/fio.inc:
///   $B9A0 FIO_CMD, $B9A1 FIO_STATUS, $B9A2 FIO_ERRCODE, $B9A3 FIO_NAMELEN,
///   $B9B0 FIO_NAME[]; FIO_CMD_LOADRUNTIME = $28.
/// </summary>
public static class RuntimeLauncher
{
    public const ushort LoadAddress = 0x7200;   // RAM, < $C000 (survives the swap)
    private const byte FioCmdLoadRuntime = 0x28;

    /// <summary>
    /// Build the AUTOBOOT.bin bytes for a launcher that boots runtime
    /// <paramref name="runtimeName"/> from /roms. Layout: 2-byte load-address
    /// header + code + the embedded name.
    /// </summary>
    public static byte[] Build(string runtimeName)
    {
        if (string.IsNullOrEmpty(runtimeName))
            throw new ArgumentException("Runtime name must not be empty.", nameof(runtimeName));

        byte[] name = Encoding.ASCII.GetBytes(runtimeName);
        if (name.Length < 1 || name.Length > 63)
            throw new ArgumentException("Runtime name must be 1..63 chars.", nameof(runtimeName));

        byte len = (byte)name.Length;
        // The name is appended immediately after the fixed-length code block.
        ushort nameAddr = (ushort)(LoadAddress + CodeLength);

        byte[] code =
        {
            0xA2, 0x00,                                     // LDX #$00
            0xBD, (byte)(nameAddr & 0xFF), (byte)(nameAddr >> 8), // @cp: LDA name,X
            0x9D, 0xB0, 0xB9,                              // STA $B9B0,X   (FIO_NAME)
            0xE8,                                          // INX
            0xE0, len,                                     // CPX #len
            0xD0, 0xF5,                                    // BNE @cp       (-11)
            0xA9, len,                                     // LDA #len
            0x8D, 0xA3, 0xB9,                             // STA $B9A3      (FIO_NAMELEN)
            0x9C, 0xA1, 0xB9,                             // STZ $B9A1      (FIO_STATUS)
            0x9C, 0xA2, 0xB9,                             // STZ $B9A2      (FIO_ERRCODE)
            0xA9, FioCmdLoadRuntime,                       // LDA #$28
            0x8D, 0xA0, 0xB9,                             // STA $B9A0      (FIO_CMD)
            0xAD, 0xA1, 0xB9,                             // @wait: LDA $B9A1 (FIO_STATUS)
            0xF0, 0xFB,                                    // BEQ @wait     (-5)
            0x6C, 0xFC, 0xFF,                            // JMP ($FFFC)   cold boot
        };

        var bin = new byte[2 + code.Length + name.Length];
        bin[0] = (byte)(LoadAddress & 0xFF);
        bin[1] = (byte)(LoadAddress >> 8);
        Array.Copy(code, 0, bin, 2, code.Length);
        Array.Copy(name, 0, bin, 2 + code.Length, name.Length);
        return bin;
    }

    // Fixed length of the code block above (name address depends on it).
    internal const int CodeLength = 37;
}
