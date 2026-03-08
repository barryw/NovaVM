using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Builds minimal valid SF2 byte arrays for testing.
/// </summary>
internal static class Sf2Builder
{
    public static byte[] Build(string presetName, int midiBank, int midiProgram,
        int rootKey, int keyLo, int keyHi, short[]? sampleData = null)
    {
        sampleData ??= new short[] { 0, 1000, 2000, 3000, 4000, 3000, 2000, 1000 };

        // Add 46 trailing zero samples as SF2 requires
        var fullSamples = new short[sampleData.Length + 46];
        Array.Copy(sampleData, fullSamples, sampleData.Length);

        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms, Encoding.ASCII);

        // We'll write the RIFF container at the end; for now, build sub-chunks

        // --- sdta LIST ---
        var sdta = BuildSdta(fullSamples);

        // --- INFO LIST (minimal) ---
        var info = BuildInfo();

        // --- pdta LIST ---
        var pdta = BuildPdta(presetName, midiBank, midiProgram, rootKey, keyLo, keyHi,
            sampleData.Length, fullSamples.Length);

        // --- Write RIFF header ---
        w.Write(Encoding.ASCII.GetBytes("RIFF"));
        uint riffSize = 4 + (uint)info.Length + (uint)sdta.Length + (uint)pdta.Length;
        w.Write(riffSize);
        w.Write(Encoding.ASCII.GetBytes("sfbk"));
        w.Write(info);
        w.Write(sdta);
        w.Write(pdta);

        return ms.ToArray();
    }

    private static byte[] BuildInfo()
    {
        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms, Encoding.ASCII);

        // ifil sub-chunk (required by spec, version 2.01)
        var ifilData = new byte[4];
        ifilData[0] = 2; ifilData[1] = 0; // major = 2
        ifilData[2] = 1; ifilData[3] = 0; // minor = 1

        w.Write(Encoding.ASCII.GetBytes("LIST"));
        uint listSize = 4 + 8 + (uint)ifilData.Length;
        w.Write(listSize);
        w.Write(Encoding.ASCII.GetBytes("INFO"));
        w.Write(Encoding.ASCII.GetBytes("ifil"));
        w.Write((uint)ifilData.Length);
        w.Write(ifilData);

        return ms.ToArray();
    }

    private static byte[] BuildSdta(short[] samples)
    {
        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms, Encoding.ASCII);

        uint smplSize = (uint)(samples.Length * 2);

        w.Write(Encoding.ASCII.GetBytes("LIST"));
        uint listSize = 4 + 8 + smplSize;
        w.Write(listSize);
        w.Write(Encoding.ASCII.GetBytes("sdta"));
        w.Write(Encoding.ASCII.GetBytes("smpl"));
        w.Write(smplSize);
        foreach (var s in samples)
            w.Write(s);

        return ms.ToArray();
    }

    private static byte[] BuildPdta(string presetName, int midiBank, int midiProgram,
        int rootKey, int keyLo, int keyHi, int sampleDataLen, int fullSampleLen)
    {
        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms, Encoding.ASCII);

        // phdr: 1 preset + 1 terminal = 2 records, 38 bytes each
        var phdr = new byte[2 * 38];
        WriteName(phdr, 0, presetName);
        WriteU16(phdr, 20, (ushort)midiProgram);
        WriteU16(phdr, 22, (ushort)midiBank);
        WriteU16(phdr, 24, 0); // presetBagNdx = 0
        // Terminal preset
        WriteName(phdr, 38, "EOP");
        WriteU16(phdr, 38 + 20, 0);
        WriteU16(phdr, 38 + 22, 0);
        WriteU16(phdr, 38 + 24, 1); // presetBagNdx = 1

        // pbag: 1 zone + 1 terminal = 2 records, 4 bytes each
        var pbag = new byte[2 * 4];
        WriteU16(pbag, 0, 0); // genNdx = 0
        WriteU16(pbag, 2, 0); // modNdx = 0
        WriteU16(pbag, 4, 1); // terminal genNdx = 1
        WriteU16(pbag, 6, 0);

        // pgen: 1 generator (instrument=0) + 1 terminal = 2 records, 4 bytes each
        var pgen = new byte[2 * 4];
        WriteU16(pgen, 0, 41); // genOper = instrument
        WriteU16(pgen, 2, 0);  // instrument index 0
        // Terminal
        WriteU16(pgen, 4, 0);
        WriteU16(pgen, 6, 0);

        // pmod: terminal only (10 bytes)
        var pmod = new byte[10];

        // inst: 1 instrument + 1 terminal = 2 records, 22 bytes each
        var inst = new byte[2 * 22];
        WriteName(inst, 0, presetName);
        WriteU16(inst, 20, 0); // instBagNdx = 0
        WriteName(inst, 22, "EOI");
        WriteU16(inst, 42, 1); // terminal instBagNdx = 1

        // ibag: 1 zone + 1 terminal = 2 records, 4 bytes each
        var ibag = new byte[2 * 4];
        WriteU16(ibag, 0, 0); // genNdx = 0
        WriteU16(ibag, 2, 0);
        WriteU16(ibag, 4, 3); // terminal genNdx = 3 (we have 3 generators: keyRange, overridingRootKey, sampleID)
        WriteU16(ibag, 6, 0);

        // imod: terminal only (10 bytes)
        var imod = new byte[10];

        // igen: keyRange + overridingRootKey + sampleID + terminal = 4 records, 4 bytes each
        var igen = new byte[4 * 4];
        // keyRange
        WriteU16(igen, 0, 43); // genOper = keyRange
        igen[4] = (byte)keyLo;   // Oops, amount is at offset 2
        igen[5] = (byte)keyHi;
        // Fix: amount bytes are at offset 2,3 within each 4-byte record
        // Let me redo this properly
        WriteU16(igen, 0, 43);
        igen[2] = (byte)keyLo;
        igen[3] = (byte)keyHi;
        // overridingRootKey
        WriteU16(igen, 4, 58);
        WriteU16(igen, 6, (ushort)rootKey);
        // sampleID
        WriteU16(igen, 8, 53);
        WriteU16(igen, 10, 0); // sample index 0
        // Terminal
        WriteU16(igen, 12, 0);
        WriteU16(igen, 14, 0);

        // shdr: 1 sample + 1 terminal = 2 records, 46 bytes each
        var shdr = new byte[2 * 46];
        WriteName(shdr, 0, "Sample1");
        WriteU32(shdr, 20, 0); // start
        WriteU32(shdr, 24, (uint)sampleDataLen); // end
        WriteU32(shdr, 28, 0); // loopStart
        WriteU32(shdr, 32, (uint)sampleDataLen); // loopEnd
        WriteU32(shdr, 36, 44100); // sampleRate
        shdr[40] = (byte)rootKey; // originalPitch
        shdr[41] = 0; // pitchCorrection
        WriteU16(shdr, 42, 0); // sampleLink
        WriteU16(shdr, 44, 1); // sampleType = monoSample
        // Terminal sample header (all zeros is fine)
        WriteName(shdr, 46, "EOS");

        // Build pdta LIST
        w.Write(Encoding.ASCII.GetBytes("LIST"));
        uint pdtaSize = 4
            + ChunkSize(phdr) + ChunkSize(pbag) + ChunkSize(pmod) + ChunkSize(pgen)
            + ChunkSize(inst) + ChunkSize(ibag) + ChunkSize(imod) + ChunkSize(igen)
            + ChunkSize(shdr);
        w.Write(pdtaSize);
        w.Write(Encoding.ASCII.GetBytes("pdta"));

        WriteChunk(w, "phdr", phdr);
        WriteChunk(w, "pbag", pbag);
        WriteChunk(w, "pmod", pmod);
        WriteChunk(w, "pgen", pgen);
        WriteChunk(w, "inst", inst);
        WriteChunk(w, "ibag", ibag);
        WriteChunk(w, "imod", imod);
        WriteChunk(w, "igen", igen);
        WriteChunk(w, "shdr", shdr);

        return ms.ToArray();
    }

    private static uint ChunkSize(byte[] data) => 8 + (uint)data.Length;

    private static void WriteChunk(BinaryWriter w, string id, byte[] data)
    {
        w.Write(Encoding.ASCII.GetBytes(id));
        w.Write((uint)data.Length);
        w.Write(data);
    }

    private static void WriteName(byte[] buf, int offset, string name)
    {
        var bytes = Encoding.ASCII.GetBytes(name);
        int len = Math.Min(bytes.Length, 20);
        Array.Copy(bytes, 0, buf, offset, len);
    }

    private static void WriteU16(byte[] buf, int offset, ushort value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)(value >> 8);
    }

    private static void WriteU32(byte[] buf, int offset, uint value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)((value >> 8) & 0xFF);
        buf[offset + 2] = (byte)((value >> 16) & 0xFF);
        buf[offset + 3] = (byte)((value >> 24) & 0xFF);
    }
}

[TestClass]
public class Sf2LoaderTests
{
    [TestMethod]
    public void Load_MinimalSf2_ReturnsOneInstrument()
    {
        var data = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
        using var stream = new MemoryStream(data);

        var bank = Sf2Loader.Load(stream);

        Assert.AreEqual(1, bank.Instruments.Count);
        Assert.AreEqual("TestPiano", bank.Instruments[0].Name);
    }

    [TestMethod]
    public void Load_SetsCorrectMidiProgram()
    {
        var data = Sf2Builder.Build("Strings", 0, 48, 60, 0, 127);
        using var stream = new MemoryStream(data);

        var bank = Sf2Loader.Load(stream);

        Assert.AreEqual(48, bank.Instruments[0].MidiProgram);
        Assert.AreEqual(0, bank.Instruments[0].MidiBank);
    }

    [TestMethod]
    public void Load_SetsRootKeyAndKeyRange()
    {
        var data = Sf2Builder.Build("Bass", 0, 32, 36, 21, 60);
        using var stream = new MemoryStream(data);

        var bank = Sf2Loader.Load(stream);

        var region = bank.Instruments[0].Regions[0];
        Assert.AreEqual(36, region.RootKey);
        Assert.AreEqual(21, region.KeyRangeLo);
        Assert.AreEqual(60, region.KeyRangeHi);
    }

    [TestMethod]
    public void Load_ConvertsSampleDataToFloat()
    {
        var samples = new short[] { 0, 16384, 32767, -32768, -16384 };
        var data = Sf2Builder.Build("Test", 0, 0, 60, 0, 127, samples);
        using var stream = new MemoryStream(data);

        var bank = Sf2Loader.Load(stream);

        var regionData = bank.Instruments[0].Regions[0].SampleData;
        Assert.AreEqual(samples.Length, regionData.Length);
        Assert.AreEqual(0f, regionData[0], 0.001f);
        Assert.AreEqual(16384f / 32768f, regionData[1], 0.001f);
        Assert.AreEqual(32767f / 32768f, regionData[2], 0.001f);
        Assert.AreEqual(-1f, regionData[3], 0.001f);
        Assert.AreEqual(-16384f / 32768f, regionData[4], 0.001f);
    }

    [TestMethod]
    public void Load_InvalidData_ThrowsInvalidDataException()
    {
        var garbage = new byte[] { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        using var stream = new MemoryStream(garbage);

        Assert.ThrowsException<InvalidDataException>(() => Sf2Loader.Load(stream));
    }
}
