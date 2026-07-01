using System.Buffers.Binary;

/// <summary>
/// Decodes the sound resources of a Blorb file into raw signed 8-bit mono PCM
/// and packs them into the NovaZ SOUND.PAK format. The Blorb RIdx resource
/// number is the Z-machine sound_effect number.
///
/// SOUND.PAK layout:
///   "NZSP", version(1), count(1)
///   index[count]: number(1) rate_hz(2 LE) pcm_offset(4 LE) pcm_len(4 LE)
///   pcm: concatenated signed 8-bit mono samples (pcm_offset is relative to
///        the start of this section)
/// </summary>
internal static class BlorbSounds
{
    private const int IndexEntrySize = 11;

    internal sealed record Sample(int Number, int RateHz, byte[] Pcm);

    public static byte[] BuildPack(string blorbPath)
    {
        byte[] blorb = File.ReadAllBytes(blorbPath);
        var samples = DecodeSounds(blorb);
        if (samples.Count == 0)
            throw new InvalidDataException($"No sound resources found in Blorb: {blorbPath}");
        if (samples.Count > 255)
            throw new InvalidDataException($"Too many sound resources ({samples.Count}); max 255.");

        int headerSize = 6 + samples.Count * IndexEntrySize;
        int pcmTotal = samples.Sum(s => s.Pcm.Length);
        var pack = new byte[headerSize + pcmTotal];

        pack[0] = (byte)'N'; pack[1] = (byte)'Z'; pack[2] = (byte)'S'; pack[3] = (byte)'P';
        pack[4] = 1;                       // version
        pack[5] = (byte)samples.Count;

        int idx = 6;
        int pcmOffset = 0;
        foreach (var s in samples.OrderBy(s => s.Number))
        {
            pack[idx] = (byte)s.Number;
            BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(idx + 1), (ushort)s.RateHz);
            BinaryPrimitives.WriteUInt32LittleEndian(pack.AsSpan(idx + 3), (uint)pcmOffset);
            BinaryPrimitives.WriteUInt32LittleEndian(pack.AsSpan(idx + 7), (uint)s.Pcm.Length);
            Array.Copy(s.Pcm, 0, pack, headerSize + pcmOffset, s.Pcm.Length);
            pcmOffset += s.Pcm.Length;
            idx += IndexEntrySize;
        }

        return pack;
    }

    // --- WTS soundfont (NSFB) output for hardware -----------------------------
    //
    // On the FPGA the audio synthesizer is the WTS chip, fed soundfont samples
    // (in SDRAM) + note events by NovaHost. We piggyback on that proven path by
    // emitting the Z-sounds as a one-instrument NSFB soundfont: each sound number
    // is one region whose key range and RootKey both equal the number, so a
    // note-on at note==number plays the sample one-shot at its native rate
    // (pitchRatio = 2^((note-RootKey)/12) = 1). NovaHost loads this and triggers
    // a note when the 6502 issues FIO_CMD_ZSOUND. Format mirrors the canonical
    // writer in e6502.Tools/NovaSoundBankCommand.cs (magic "NSFB", PCM8, 34-byte
    // header, 40-byte instrument + region records, frame-indexed sample offsets).
    private const uint NsfbMagic        = 0x4246534E; // "NSFB" little-endian
    private const ushort NsfbVersionPcm8 = 1;
    private const int NsfbHeaderSizeV1  = 34;
    private const int NsfbInstrumentRecordSize = 40;
    private const int NsfbRegionRecordSize = 40;
    private const ushort NsfbBakedSampleRate = 32000; // mixer hint; per-region rate drives pitch

    public static byte[] BuildSoundfont(string blorbPath)
    {
        byte[] blorb = File.ReadAllBytes(blorbPath);
        var samples = DecodeSounds(blorb).OrderBy(s => s.Number).ToList();
        if (samples.Count == 0)
            throw new InvalidDataException($"No sound resources found in Blorb: {blorbPath}");
        foreach (var s in samples)
            if (s.Number is < 0 or > 127)
                throw new InvalidDataException($"Sound number {s.Number} is out of MIDI key range (0..127).");

        int pcmTotal = samples.Sum(s => s.Pcm.Length);
        int instrumentTableOffset = NsfbHeaderSizeV1;
        int regionTableOffset = instrumentTableOffset + NsfbInstrumentRecordSize; // one instrument
        int sampleDataOffset = regionTableOffset + samples.Count * NsfbRegionRecordSize;

        using var ms = new MemoryStream(sampleDataOffset + pcmTotal);
        using var w = new BinaryWriter(ms);

        // Header (little-endian, matches NovaSoundBankCommand V1).
        w.Write(NsfbMagic);
        w.Write(NsfbVersionPcm8);
        w.Write((ushort)NsfbHeaderSizeV1);
        w.Write((ushort)NsfbInstrumentRecordSize);
        w.Write((ushort)NsfbRegionRecordSize);
        w.Write((ushort)1);                 // instrument count
        w.Write((ushort)samples.Count);     // region count
        w.Write(NsfbBakedSampleRate);
        w.Write((uint)instrumentTableOffset);
        w.Write((uint)regionTableOffset);
        w.Write((uint)sampleDataOffset);
        w.Write((uint)pcmTotal);

        // One instrument (bank 0, program 0) covering all regions.
        w.Write((byte)0);                   // bank
        w.Write((byte)0);                   // program
        w.Write((ushort)0);                 // first region
        w.Write((ushort)samples.Count);     // region count
        WriteFixedAscii(w, "ZSOUND", 32);
        w.Write((ushort)0);                 // reserved

        // One region per sound: key range and RootKey == sound number, no loop,
        // instant full-volume envelope so it plays as a fixed one-shot recording.
        int frameOffset = 0;
        foreach (var s in samples)
        {
            byte note = (byte)s.Number;
            w.Write(note);                  // key lo
            w.Write(note);                  // key hi
            w.Write((byte)0);               // vel lo
            w.Write((byte)127);             // vel hi
            w.Write(note);                  // root key
            w.Write((byte)0);               // flags (bit0 = loop) -> one-shot
            w.Write((ushort)Math.Clamp(s.RateHz, 1, ushort.MaxValue)); // sample rate
            w.Write((short)0);              // tuning cents
            w.Write((uint)frameOffset);     // sample start (frames; PCM8 => bytes)
            w.Write((uint)s.Pcm.Length);    // sample length
            w.Write((uint)frameOffset);     // loop start (unused)
            w.Write((uint)(frameOffset + s.Pcm.Length)); // loop end (unused)
            w.Write((ushort)0);             // attack frames (instant)
            w.Write((ushort)0);             // decay frames
            w.Write((ushort)0xFFFF);        // sustain level (full)
            w.Write((ushort)0);             // release frames
            w.Write((uint)0);               // reserved
            w.Write((ushort)0);             // reserved
            frameOffset += s.Pcm.Length;
        }

        // Concatenated signed 8-bit PCM (same bytes as SOUND.PAK's pcm section).
        foreach (var s in samples)
            w.Write(s.Pcm);

        w.Flush();
        return ms.ToArray();
    }

    public static string SoundfontSummary(byte[] nsfb)
    {
        int regionCount = nsfb[12] | (nsfb[13] << 8);
        return $"{regionCount} sounds as 1 WTS instrument (NSFB PCM8)";
    }

    private static void WriteFixedAscii(BinaryWriter w, string text, int length)
    {
        var bytes = new byte[length];
        int n = Math.Min(text.Length, length - 1);
        for (int i = 0; i < n; i++)
            bytes[i] = (byte)(text[i] <= 0x7F ? text[i] : '?');
        w.Write(bytes);
    }

    /// <summary>Human-readable summary for the packer's console output.</summary>
    public static string Summary(byte[] pack)
    {
        int count = pack[5];
        var nums = new List<int>();
        for (int i = 0; i < count; i++)
            nums.Add(pack[6 + i * IndexEntrySize]);
        int rate0 = count > 0 ? BinaryPrimitives.ReadUInt16LittleEndian(pack.AsSpan(7)) : 0;
        return $"{count} sounds, numbers [{string.Join(",", nums)}], first rate {rate0} Hz";
    }

    private static List<Sample> DecodeSounds(byte[] blorb)
    {
        if (blorb.Length < 12 ||
            Tag(blorb, 0) != "FORM" || Tag(blorb, 8) != "IFRS")
            throw new InvalidDataException("Not a Blorb file (expected FORM/IFRS).");

        int ridx = FindChunk(blorb, "RIdx") ?? throw new InvalidDataException("Blorb has no RIdx chunk.");
        int count = ReadBE32(blorb, ridx + 8);
        int entry = ridx + 12;

        var samples = new List<Sample>();
        for (int i = 0; i < count; i++, entry += 12)
        {
            if (Tag(blorb, entry) != "Snd ")
                continue;
            int number = ReadBE32(blorb, entry + 4);
            int start = ReadBE32(blorb, entry + 8);
            var sample = DecodeAiffChunk(blorb, start, number);
            if (sample is not null)
                samples.Add(sample);
        }

        return samples;
    }

    // A Snd resource points at a chunk; Infocom uses FORM/AIFF.
    private static Sample? DecodeAiffChunk(byte[] data, int chunkStart, int number)
    {
        string tag = Tag(data, chunkStart);
        if (tag != "FORM")
            return null; // OGGV/MOD etc. — Infocom uses AIFF only.
        if (Tag(data, chunkStart + 8) != "AIFF")
            return null;

        int formSize = ReadBE32(data, chunkStart + 4);
        int end = chunkStart + 8 + formSize;
        int p = chunkStart + 12;

        int channels = 1, sampleSize = 8, rate = 8000;
        int ssndStart = -1, ssndLen = 0;

        while (p + 8 <= end)
        {
            string t = Tag(data, p);
            int sz = ReadBE32(data, p + 4);
            int body = p + 8;
            if (t == "COMM")
            {
                channels = ReadBE16(data, body);
                sampleSize = ReadBE16(data, body + 6);
                rate = (int)Math.Round(ReadExtended80(data, body + 8));
            }
            else if (t == "SSND")
            {
                int ssndOffset = ReadBE32(data, body);     // offset into sample data
                ssndStart = body + 8 + ssndOffset;          // skip offset(4)+blockSize(4)
                ssndLen = sz - 8 - ssndOffset;
            }
            p = body + sz + (sz & 1); // chunks are padded to even
        }

        if (ssndStart < 0)
            return null;

        byte[] pcm = ToSigned8Mono(data, ssndStart, ssndLen, channels, sampleSize);
        return new Sample(number, rate <= 0 ? 8000 : rate, pcm);
    }

    private static byte[] ToSigned8Mono(byte[] data, int start, int len, int channels, int sampleSize)
    {
        int bytesPerSample = sampleSize / 8;
        if (bytesPerSample < 1) bytesPerSample = 1;
        int frameBytes = bytesPerSample * channels;
        int frames = len / frameBytes;
        var pcm = new byte[frames];

        for (int f = 0; f < frames; f++)
        {
            int src = start + f * frameBytes;
            // AIFF samples are signed, big-endian, MSB first. Take the top byte
            // of the first (left) channel — that downconverts 16-bit to 8-bit
            // and stereo to mono in one step.
            pcm[f] = data[src];
        }

        return pcm;
    }

    // 80-bit IEEE 754 extended (AIFF sample rate).
    private static double ReadExtended80(byte[] b, int o)
    {
        int exp = ((b[o] & 0x7F) << 8) | b[o + 1];
        ulong mant = 0;
        for (int i = 0; i < 8; i++)
            mant = (mant << 8) | b[o + 2 + i];
        if (exp == 0 && mant == 0)
            return 0;
        double value = (double)mant / (double)(1UL << 63) * Math.Pow(2, exp - 16383);
        return (b[o] & 0x80) != 0 ? -value : value;
    }

    private static int? FindChunk(byte[] data, string tag)
    {
        int p = 12; // skip FORM + size + IFRS
        while (p + 8 <= data.Length)
        {
            int sz = ReadBE32(data, p + 4);
            if (Tag(data, p) == tag)
                return p;
            p += 8 + sz + (sz & 1);
        }
        return null;
    }

    private static string Tag(byte[] d, int o) => System.Text.Encoding.ASCII.GetString(d, o, 4);
    private static int ReadBE16(byte[] d, int o) => (d[o] << 8) | d[o + 1];
    private static int ReadBE32(byte[] d, int o) => (d[o] << 24) | (d[o + 1] << 16) | (d[o + 2] << 8) | d[o + 3];
}
