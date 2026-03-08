using System.Text;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Parses SF2 soundfont files and produces a <see cref="SampleBank"/>.
/// </summary>
public static class Sf2Loader
{
    // Generator operator IDs
    private const ushort GenKeyRange = 43;
    private const ushort GenVelRange = 44;
    private const ushort GenSampleID = 53;
    private const ushort GenSampleModes = 54;
    private const ushort GenOverridingRootKey = 58;
    private const ushort GenAttackVolEnv = 34;
    private const ushort GenHoldVolEnv = 35;
    private const ushort GenDecayVolEnv = 36;
    private const ushort GenSustainVolEnv = 37;
    private const ushort GenReleaseVolEnv = 38;
    private const ushort GenKeynumToVolEnvHold = 39;
    private const ushort GenKeynumToVolEnvDecay = 40;
    private const ushort GenInstrument = 41;

    public static SampleBank Load(Stream stream)
    {
        using var reader = new BinaryReader(stream, Encoding.ASCII, leaveOpen: true);

        // RIFF header
        var riffId = ReadFourCC(reader);
        if (riffId != "RIFF")
            throw new InvalidDataException("Not a RIFF file.");

        var riffSize = reader.ReadUInt32();
        var formType = ReadFourCC(reader);
        if (formType != "sfbk")
            throw new InvalidDataException("Not an SF2 soundfont file.");

        // Parse top-level LIST chunks
        float[]? smplPool = null;
        SampleHeader[]? shdrs = null;
        PresetHeader[]? phdrs = null;
        BagEntry[]? pbags = null;
        GenEntry[]? pgens = null;
        InstHeader[]? insts = null;
        BagEntry[]? ibags = null;
        GenEntry[]? igens = null;

        long riffEnd = stream.Position + riffSize - 4; // -4 for formType already read
        while (stream.Position < riffEnd)
        {
            var chunkId = ReadFourCC(reader);
            var chunkSize = reader.ReadUInt32();
            var chunkStart = stream.Position;

            if (chunkId == "LIST")
            {
                var listType = ReadFourCC(reader);
                var listEnd = chunkStart + chunkSize;

                if (listType == "sdta")
                {
                    while (stream.Position < listEnd)
                    {
                        var subId = ReadFourCC(reader);
                        var subSize = reader.ReadUInt32();
                        if (subId == "smpl")
                        {
                            var sampleCount = (int)(subSize / 2);
                            smplPool = new float[sampleCount];
                            for (int i = 0; i < sampleCount; i++)
                                smplPool[i] = reader.ReadInt16() / 32768f;
                        }
                        else
                        {
                            stream.Position += subSize;
                        }
                        // Pad to even boundary
                        if (subSize % 2 != 0 && stream.Position < listEnd)
                            stream.Position++;
                    }
                }
                else if (listType == "pdta")
                {
                    while (stream.Position < listEnd)
                    {
                        var subId = ReadFourCC(reader);
                        var subSize = reader.ReadUInt32();
                        var subStart = stream.Position;

                        switch (subId)
                        {
                            case "phdr":
                                phdrs = ParsePresetHeaders(reader, subSize);
                                break;
                            case "pbag":
                                pbags = ParseBags(reader, subSize);
                                break;
                            case "pgen":
                                pgens = ParseGens(reader, subSize);
                                break;
                            case "inst":
                                insts = ParseInstHeaders(reader, subSize);
                                break;
                            case "ibag":
                                ibags = ParseBags(reader, subSize);
                                break;
                            case "igen":
                                igens = ParseGens(reader, subSize);
                                break;
                            case "shdr":
                                shdrs = ParseSampleHeaders(reader, subSize);
                                break;
                            default:
                                stream.Position = subStart + subSize;
                                break;
                        }

                        // Ensure we're at the end of this sub-chunk
                        stream.Position = subStart + subSize;
                        if (subSize % 2 != 0 && stream.Position < listEnd)
                            stream.Position++;
                    }
                }
                else
                {
                    stream.Position = chunkStart + chunkSize;
                }
            }
            else
            {
                stream.Position = chunkStart + chunkSize;
            }

            // Pad to even boundary
            if (chunkSize % 2 != 0 && stream.Position < riffEnd)
                stream.Position++;
        }

        if (smplPool == null || shdrs == null || phdrs == null || pbags == null ||
            pgens == null || insts == null || ibags == null || igens == null)
            throw new InvalidDataException("SF2 file is missing required chunks.");

        return BuildSampleBank(smplPool, shdrs, phdrs, pbags, pgens, insts, ibags, igens);
    }

    private static SampleBank BuildSampleBank(
        float[] smplPool, SampleHeader[] shdrs,
        PresetHeader[] phdrs, BagEntry[] pbags, GenEntry[] pgens,
        InstHeader[] insts, BagEntry[] ibags, GenEntry[] igens)
    {
        var bank = new SampleBank();

        // Last entry in each array is a terminal record
        int presetCount = phdrs.Length - 1;

        for (int p = 0; p < presetCount; p++)
        {
            var preset = phdrs[p];
            var instrument = new SampleInstrument
            {
                Name = preset.Name,
                MidiBank = preset.Bank,
                MidiProgram = preset.Preset
            };

            // Walk preset zones (bags) to find instrument references
            int pbagStart = preset.BagNdx;
            int pbagEnd = phdrs[p + 1].BagNdx;

            // Extract preset-level global zone (first zone without an instrument generator)
            Dictionary<ushort, ushort>? presetGlobalGens = null;
            int firstPresetZone = pbagStart;
            if (pbagStart < pbagEnd)
            {
                int gStart = pbags[pbagStart].GenNdx;
                int gEnd = pbags[pbagStart + 1].GenNdx;
                bool hasInstGen = false;
                for (int g = gStart; g < gEnd; g++)
                {
                    if (pgens[g].Oper == GenInstrument) { hasInstGen = true; break; }
                }
                if (!hasInstGen)
                {
                    presetGlobalGens = new Dictionary<ushort, ushort>();
                    for (int g = gStart; g < gEnd; g++)
                        presetGlobalGens[pgens[g].Oper] = pgens[g].Amount;
                    firstPresetZone = pbagStart + 1;
                }
            }

            for (int pb = firstPresetZone; pb < pbagEnd; pb++)
            {
                int pgenStart = pbags[pb].GenNdx;
                int pgenEnd = pbags[pb + 1].GenNdx;

                // Extract preset zone key/velocity ranges (restricts which notes use this zone)
                int pzKeyLo = 0, pzKeyHi = 127;
                int pzVelLo = 0, pzVelHi = 127;

                // Collect preset-zone generators (additive offsets)
                // Merge preset global + preset zone generators
                var presetOffsets = new Dictionary<ushort, short>();
                if (presetGlobalGens != null)
                {
                    foreach (var (oper, amount) in presetGlobalGens)
                    {
                        if (oper == GenInstrument) continue;
                        if (oper == GenKeyRange)
                        {
                            pzKeyLo = amount & 0xFF;
                            pzKeyHi = (amount >> 8) & 0xFF;
                        }
                        else if (oper == GenVelRange)
                        {
                            pzVelLo = amount & 0xFF;
                            pzVelHi = (amount >> 8) & 0xFF;
                        }
                        else
                        {
                            presetOffsets[oper] = (short)amount;
                        }
                    }
                }

                // Find instrument generator and collect zone-specific offsets
                int instIndex = -1;
                for (int pg = pgenStart; pg < pgenEnd; pg++)
                {
                    if (pgens[pg].Oper == GenInstrument)
                    {
                        instIndex = (int)pgens[pg].Amount;
                    }
                    else if (pgens[pg].Oper == GenKeyRange)
                    {
                        // Zone-level keyRange overrides global
                        pzKeyLo = pgens[pg].Amount & 0xFF;
                        pzKeyHi = (pgens[pg].Amount >> 8) & 0xFF;
                    }
                    else if (pgens[pg].Oper == GenVelRange)
                    {
                        pzVelLo = pgens[pg].Amount & 0xFF;
                        pzVelHi = (pgens[pg].Amount >> 8) & 0xFF;
                    }
                    else
                    {
                        // Preset zone generators override preset global
                        presetOffsets[pgens[pg].Oper] = (short)pgens[pg].Amount;
                    }
                }

                if (instIndex < 0 || instIndex >= insts.Length - 1)
                    continue;

                // Walk instrument zones
                int ibagStart = insts[instIndex].BagNdx;
                int ibagEnd = insts[instIndex + 1].BagNdx;

                // Extract instrument-level global zone (first zone without a sampleID generator)
                Dictionary<ushort, ushort>? instGlobalGens = null;
                int firstNonGlobal = ibagStart;
                if (ibagStart < ibagEnd)
                {
                    int gStart = ibags[ibagStart].GenNdx;
                    int gEnd = ibags[ibagStart + 1].GenNdx;
                    bool hasSampleId = false;
                    for (int g = gStart; g < gEnd; g++)
                    {
                        if (igens[g].Oper == GenSampleID) { hasSampleId = true; break; }
                    }
                    if (!hasSampleId)
                    {
                        instGlobalGens = new Dictionary<ushort, ushort>();
                        for (int g = gStart; g < gEnd; g++)
                            instGlobalGens[igens[g].Oper] = igens[g].Amount;
                        firstNonGlobal = ibagStart + 1;
                    }
                }

                for (int ib = firstNonGlobal; ib < ibagEnd; ib++)
                {
                    int igenStart = ibags[ib].GenNdx;
                    int igenEnd = ibags[ib + 1].GenNdx;

                    var region = BuildRegion(igens, igenStart, igenEnd, shdrs, smplPool,
                        instGlobalGens, presetOffsets);
                    if (region == null) continue;

                    // Intersect region ranges with preset zone ranges
                    region.KeyRangeLo = Math.Max(region.KeyRangeLo, pzKeyLo);
                    region.KeyRangeHi = Math.Min(region.KeyRangeHi, pzKeyHi);
                    region.VelocityRangeLo = Math.Max(region.VelocityRangeLo, pzVelLo);
                    region.VelocityRangeHi = Math.Min(region.VelocityRangeHi, pzVelHi);

                    // Only keep if ranges are still valid
                    if (region.KeyRangeLo <= region.KeyRangeHi &&
                        region.VelocityRangeLo <= region.VelocityRangeHi)
                    {
                        instrument.Regions.Add(region);
                    }
                }
            }

            if (instrument.Regions.Count > 0)
                bank.Instruments.Add(instrument);
        }

        return bank;
    }

    private static void ApplyGenerator(ushort oper, ushort amount,
        ref int keyLo, ref int keyHi, ref int velLo, ref int velHi,
        ref int sampleId, ref int sampleModes, ref int rootKeyOverride,
        ref short attackTC, ref short holdTC, ref short decayTC, ref short sustainCB, ref short releaseTC,
        ref short keynumToHoldTC, ref short keynumToDecayTC)
    {
        switch (oper)
        {
            case GenKeyRange:
                keyLo = amount & 0xFF;
                keyHi = (amount >> 8) & 0xFF;
                break;
            case GenVelRange:
                velLo = amount & 0xFF;
                velHi = (amount >> 8) & 0xFF;
                break;
            case GenSampleID:
                sampleId = amount;
                break;
            case GenSampleModes:
                sampleModes = amount;
                break;
            case GenOverridingRootKey:
                rootKeyOverride = amount;
                break;
            case GenAttackVolEnv:
                attackTC = (short)amount;
                break;
            case GenHoldVolEnv:
                holdTC = (short)amount;
                break;
            case GenDecayVolEnv:
                decayTC = (short)amount;
                break;
            case GenSustainVolEnv:
                sustainCB = (short)amount;
                break;
            case GenReleaseVolEnv:
                releaseTC = (short)amount;
                break;
            case GenKeynumToVolEnvHold:
                keynumToHoldTC = (short)amount;
                break;
            case GenKeynumToVolEnvDecay:
                keynumToDecayTC = (short)amount;
                break;
        }
    }

    private static SampleRegion? BuildRegion(
        GenEntry[] igens, int genStart, int genEnd,
        SampleHeader[] shdrs, float[] smplPool,
        Dictionary<ushort, ushort>? instGlobalGens = null,
        Dictionary<ushort, short>? presetOffsets = null)
    {
        int keyLo = 0, keyHi = 127;
        int velLo = 0, velHi = 127;
        int sampleId = -1;
        int sampleModes = 0;
        int rootKeyOverride = -1;
        short attackTC = -12000;
        short holdTC = -12000;
        short decayTC = -12000;
        short sustainCB = 0;
        short releaseTC = -12000;
        short keynumToHoldTC = 0;
        short keynumToDecayTC = 0;

        // Apply instrument-level global zone defaults first
        if (instGlobalGens != null)
        {
            foreach (var (oper, amount) in instGlobalGens)
            {
                if (oper == GenKeyRange || oper == GenVelRange || oper == GenSampleID)
                    continue;
                ApplyGenerator(oper, amount, ref keyLo, ref keyHi, ref velLo, ref velHi,
                    ref sampleId, ref sampleModes, ref rootKeyOverride,
                    ref attackTC, ref holdTC, ref decayTC, ref sustainCB, ref releaseTC,
                    ref keynumToHoldTC, ref keynumToDecayTC);
            }
        }

        // Local zone generators override instrument globals
        for (int g = genStart; g < genEnd; g++)
        {
            ApplyGenerator(igens[g].Oper, igens[g].Amount,
                ref keyLo, ref keyHi, ref velLo, ref velHi,
                ref sampleId, ref sampleModes, ref rootKeyOverride,
                ref attackTC, ref holdTC, ref decayTC, ref sustainCB, ref releaseTC,
                ref keynumToHoldTC, ref keynumToDecayTC);
        }

        // Apply preset-level offsets (additive for envelope/modulator generators)
        if (presetOffsets != null)
        {
            foreach (var (oper, offset) in presetOffsets)
            {
                switch (oper)
                {
                    case GenAttackVolEnv: attackTC = ClampTC(attackTC + offset); break;
                    case GenHoldVolEnv: holdTC = ClampTC(holdTC + offset); break;
                    case GenDecayVolEnv: decayTC = ClampTC(decayTC + offset); break;
                    case GenSustainVolEnv: sustainCB = ClampTC(sustainCB + offset); break;
                    case GenReleaseVolEnv: releaseTC = ClampTC(releaseTC + offset); break;
                    case GenKeynumToVolEnvHold: keynumToHoldTC = ClampTC(keynumToHoldTC + offset); break;
                    case GenKeynumToVolEnvDecay: keynumToDecayTC = ClampTC(keynumToDecayTC + offset); break;
                    case GenSampleModes: sampleModes = (ushort)offset; break;
                }
            }
        }

        // Zone without sampleID is a global zone — skip
        if (sampleId < 0 || sampleId >= shdrs.Length - 1)
            return null;

        var shdr = shdrs[sampleId];

        int rootKey = rootKeyOverride >= 0 ? rootKeyOverride : shdr.OriginalPitch;
        int loopStart = (int)(shdr.LoopStart - shdr.Start);
        int loopEnd = (int)(shdr.LoopEnd - shdr.Start);
        int sampleLen = (int)(shdr.End - shdr.Start);

        // Extract sample slice from pool
        float[] data;
        if (shdr.Start < smplPool.Length)
        {
            int available = Math.Min(sampleLen, smplPool.Length - (int)shdr.Start);
            data = new float[available];
            Array.Copy(smplPool, (int)shdr.Start, data, 0, available);
        }
        else
        {
            data = Array.Empty<float>();
        }

        return new SampleRegion
        {
            KeyRangeLo = keyLo,
            KeyRangeHi = keyHi,
            VelocityRangeLo = velLo,
            VelocityRangeHi = velHi,
            SampleData = data,
            SampleRate = (int)shdr.SampleRate,
            RootKey = rootKey,
            LoopStart = Math.Max(0, loopStart),
            LoopEnd = Math.Max(0, loopEnd),
            LoopEnabled = (sampleModes & 1) != 0,
            AttackTime = TimecentsToSeconds(attackTC),
            HoldTime = TimecentsToSeconds(holdTC),
            KeynumToHoldTC = keynumToHoldTC,
            DecayTime = TimecentsToSeconds(decayTC),
            KeynumToDecayTC = keynumToDecayTC,
            SustainLevel = CentibelsToLevel(sustainCB),
            ReleaseTime = TimecentsToSeconds(releaseTC)
        };
    }

    private static short ClampTC(int value) =>
        (short)Math.Clamp(value, short.MinValue, short.MaxValue);

    private static float TimecentsToSeconds(short timecents)
    {
        double seconds = Math.Pow(2, timecents / 1200.0);
        return Math.Clamp((float)seconds, 0.001f, 100f);
    }

    private static float CentibelsToLevel(short centibels)
    {
        if (centibels <= 0) return 1.0f;
        double level = Math.Pow(10, -centibels / 200.0);
        return Math.Clamp((float)level, 0f, 1f);
    }

    #region Chunk Parsers

    private static string ReadFourCC(BinaryReader reader)
    {
        var bytes = reader.ReadBytes(4);
        if (bytes.Length < 4)
            throw new InvalidDataException("Unexpected end of SF2 file.");
        return Encoding.ASCII.GetString(bytes);
    }

    private static PresetHeader[] ParsePresetHeaders(BinaryReader reader, uint chunkSize)
    {
        int count = (int)(chunkSize / 38);
        var result = new PresetHeader[count];
        for (int i = 0; i < count; i++)
        {
            var nameBytes = reader.ReadBytes(20);
            var name = Encoding.ASCII.GetString(nameBytes).TrimEnd('\0');
            var preset = reader.ReadUInt16();
            var bank = reader.ReadUInt16();
            var bagNdx = reader.ReadUInt16();
            reader.ReadBytes(12); // library, genre, morphology
            result[i] = new PresetHeader(name, preset, bank, bagNdx);
        }
        return result;
    }

    private static InstHeader[] ParseInstHeaders(BinaryReader reader, uint chunkSize)
    {
        int count = (int)(chunkSize / 22);
        var result = new InstHeader[count];
        for (int i = 0; i < count; i++)
        {
            var nameBytes = reader.ReadBytes(20);
            var name = Encoding.ASCII.GetString(nameBytes).TrimEnd('\0');
            var bagNdx = reader.ReadUInt16();
            result[i] = new InstHeader(name, bagNdx);
        }
        return result;
    }

    private static BagEntry[] ParseBags(BinaryReader reader, uint chunkSize)
    {
        int count = (int)(chunkSize / 4);
        var result = new BagEntry[count];
        for (int i = 0; i < count; i++)
        {
            var genNdx = reader.ReadUInt16();
            var modNdx = reader.ReadUInt16();
            result[i] = new BagEntry(genNdx, modNdx);
        }
        return result;
    }

    private static GenEntry[] ParseGens(BinaryReader reader, uint chunkSize)
    {
        int count = (int)(chunkSize / 4);
        var result = new GenEntry[count];
        for (int i = 0; i < count; i++)
        {
            var oper = reader.ReadUInt16();
            var amount = reader.ReadUInt16();
            result[i] = new GenEntry(oper, amount);
        }
        return result;
    }

    private static SampleHeader[] ParseSampleHeaders(BinaryReader reader, uint chunkSize)
    {
        int count = (int)(chunkSize / 46);
        var result = new SampleHeader[count];
        for (int i = 0; i < count; i++)
        {
            var nameBytes = reader.ReadBytes(20);
            var name = Encoding.ASCII.GetString(nameBytes).TrimEnd('\0');
            var start = reader.ReadUInt32();
            var end = reader.ReadUInt32();
            var loopStart = reader.ReadUInt32();
            var loopEnd = reader.ReadUInt32();
            var sampleRate = reader.ReadUInt32();
            var originalPitch = reader.ReadByte();
            var pitchCorrection = reader.ReadSByte();
            var sampleLink = reader.ReadUInt16();
            var sampleType = reader.ReadUInt16();
            result[i] = new SampleHeader(name, start, end, loopStart, loopEnd,
                sampleRate, originalPitch, pitchCorrection, sampleLink, sampleType);
        }
        return result;
    }

    #endregion

    #region Internal Records

    private record PresetHeader(string Name, ushort Preset, ushort Bank, ushort BagNdx);
    private record InstHeader(string Name, ushort BagNdx);
    private record BagEntry(ushort GenNdx, ushort ModNdx);
    private record GenEntry(ushort Oper, ushort Amount);
    private record SampleHeader(string Name, uint Start, uint End, uint LoopStart,
        uint LoopEnd, uint SampleRate, byte OriginalPitch, sbyte PitchCorrection,
        ushort SampleLink, ushort SampleType);

    #endregion
}
