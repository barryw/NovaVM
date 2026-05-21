using e6502.Avalonia.Hardware;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Tools;

public static class NovaSoundBankCommand
{
    private const uint Magic = 0x4246534E; // NSFB, little-endian on disk
    private const ushort VersionPcm8 = 1;
    private const ushort VersionPcm16 = 2;
    private const int HeaderSizeV1 = 34;
    private const int HeaderSizeV2 = 38;
    private const ushort SampleFormatPcm8 = 1;
    private const ushort SampleFormatPcm16 = 2;
    private const int InstrumentRecordSize = 40;
    private const int RegionRecordSize = 40;
    private const int HardwareSampleRate = 48000;
    private const int BakedSampleRate = 32000;
    private const int DefaultMaxSampleBytes = 0xF7E000;
    private const int ReportInstrumentCount = 8;

    public static int Run(string[] args)
    {
        string? sf2Path = null;
        string? midiPath = null;
        string? outputPath = null;
        int maxSampleBytes = DefaultMaxSampleBytes;
        bool micro = false;
        bool full = false;
        bool pcm16 = false;
        int microRegionsPerInstrument = 3;
        int maxRegionSamples = 2048;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--sf2" when i + 1 < args.Length:
                    sf2Path = args[++i];
                    break;
                case "--midi" when i + 1 < args.Length:
                    midiPath = args[++i];
                    break;
                case "-o" or "--output" when i + 1 < args.Length:
                    outputPath = args[++i];
                    break;
                case "--max-sample-bytes" when i + 1 < args.Length:
                    maxSampleBytes = ParsePositiveInt(args[++i], "--max-sample-bytes");
                    break;
                case "--micro":
                    micro = true;
                    break;
                case "--full":
                    full = true;
                    break;
                case "--pcm16":
                    pcm16 = true;
                    break;
                case "--micro-regions" when i + 1 < args.Length:
                    microRegionsPerInstrument = ParsePositiveInt(args[++i], "--micro-regions");
                    break;
                case "--max-region-samples" when i + 1 < args.Length:
                    maxRegionSamples = ParsePositiveInt(args[++i], "--max-region-samples");
                    break;
                case "-h" or "--help":
                    PrintUsage();
                    return 0;
                default:
                    Console.Error.WriteLine($"Unknown or incomplete option: {args[i]}");
                    PrintUsage();
                    return 1;
            }
        }

        if (sf2Path is null || outputPath is null || (!full && midiPath is null))
        {
            PrintUsage();
            return 1;
        }

        try
        {
            using var sf2 = File.OpenRead(sf2Path);
            var bank = Sf2Loader.Load(sf2);
            var needed = full
                ? CollectAllRegions(bank)
                : CollectNeededRegions(MidiFile.Read(midiPath!));
            if (needed.Count == 0)
                throw new InvalidOperationException(full
                    ? "SoundFont does not contain playable instruments."
                    : "MIDI file does not contain playable note-on events.");

            var built = micro
                ? BuildMicroBank(bank, needed, maxSampleBytes, microRegionsPerInstrument, maxRegionSamples, pcm16)
                : BuildBank(bank, needed, maxSampleBytes, pcm16);
            WriteBank(outputPath, built);

            Console.WriteLine(
                $"Wrote {outputPath}: instruments={built.Instruments.Count}, " +
                $"regions={built.Regions.Count}, samples={built.SampleFrameCount} frames, " +
                $"sampleBytes={built.SampleBytes.Count}, format={(pcm16 ? "pcm16" : "pcm8")}");
            PrintReport(built);
            return 0;
        }
        catch (Exception ex) when (ex is IOException or InvalidDataException or InvalidOperationException or ArgumentException)
        {
            Console.Error.WriteLine($"sf2bank failed: {ex.Message}");
            return 1;
        }
    }

    private static Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>> CollectNeededRegions(MidiFile midi)
    {
        var currentProgram = new int[16];
        var result = new Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>>();

        var events = midi.GetTrackChunks()
            .SelectMany(t => t.GetTimedEvents())
            .OrderBy(e => e.Time)
            .ToList();

        foreach (var timed in events)
        {
            switch (timed.Event)
            {
                case ProgramChangeEvent pc:
                    currentProgram[pc.Channel] = pc.ProgramNumber & 0x7F;
                    break;
                case NoteOnEvent noteOn when noteOn.Velocity > 0:
                    int bank = noteOn.Channel == 9 ? 128 : 0;
                    int program = noteOn.Channel == 9 ? 0 : currentProgram[noteOn.Channel];
                    var key = (bank, program);
                    if (!result.TryGetValue(key, out var notes))
                    {
                        notes = new List<(int Note, int Velocity)>();
                        result.Add(key, notes);
                    }
                    notes.Add((noteOn.NoteNumber, noteOn.Velocity));
                    break;
            }
        }

        return result;
    }

    private static Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>> CollectAllRegions(SampleBank bank)
    {
        var result = new Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>>();
        foreach (var instrument in bank.Instruments)
        {
            if (instrument.Regions.Count == 0)
                continue;

            var notes = new List<(int Note, int Velocity)>();
            foreach (var region in instrument.Regions)
            {
                if (region.SampleData.Length == 0)
                    continue;

                int keyLo = Math.Clamp(region.KeyRangeLo, 0, 127);
                int keyHi = Math.Clamp(region.KeyRangeHi, 0, 127);
                int velLo = Math.Clamp(region.VelocityRangeLo, 0, 127);
                int velHi = Math.Clamp(region.VelocityRangeHi, 0, 127);
                if (keyLo > keyHi || velLo > velHi)
                    continue;

                notes.Add((keyLo, Math.Max(1, velLo)));
                notes.Add((keyHi, Math.Max(1, velHi)));
            }

            if (notes.Count > 0)
                result[(instrument.MidiBank, instrument.MidiProgram)] = notes;
        }

        return result;
    }

    private static BuiltBank BuildBank(
        SampleBank source,
        Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>> needed,
        int maxSampleBytes,
        bool pcm16)
    {
        var built = new BuiltBank(pcm16);

        foreach (var ((bank, program), notes) in needed.OrderBy(k => k.Key.Bank).ThenBy(k => k.Key.Program))
        {
            int sourceIndex = source.FindByProgram(bank, program);
            if (sourceIndex < 0 && bank == 128)
                sourceIndex = source.FindByProgram(128, 0);
            if (sourceIndex < 0)
                sourceIndex = source.FindByProgram(0, program);
            if (sourceIndex < 0)
                throw new InvalidOperationException($"Soundfont has no instrument for bank {bank}, program {program}.");

            var sourceInstrument = source.Instruments[sourceIndex];
            int firstRegion = built.Regions.Count;
            var report = new InstrumentBuildReport(
                (byte)Math.Clamp(bank, 0, 255),
                (byte)Math.Clamp(program, 0, 127),
                sourceInstrument.Name,
                notes.Select(n => n.Note).Distinct().Count(),
                notes.Count);

            foreach (var region in sourceInstrument.Regions)
            {
                if (region.SampleData.Length == 0 ||
                    !notes.Any(n => region.Matches(n.Note, n.Velocity)))
                    continue;

                AddRegion(built, region, maxSampleBytes, report);
            }

            int regionCount = built.Regions.Count - firstRegion;
            if (regionCount == 0)
                throw new InvalidOperationException(
                    $"Soundfont instrument {sourceInstrument.Name} has no matching sample regions for bank {bank}, program {program}.");

            built.Instruments.Add(new BankInstrument(
                (byte)Math.Clamp(bank, 0, 255),
                (byte)Math.Clamp(program, 0, 127),
                (ushort)firstRegion,
                (ushort)regionCount,
                sourceInstrument.Name));
            built.Reports.Add(report);
        }

        return built;
    }

    private static BuiltBank BuildMicroBank(
        SampleBank source,
        Dictionary<(int Bank, int Program), List<(int Note, int Velocity)>> needed,
        int maxSampleBytes,
        int regionsPerInstrument,
        int maxRegionSamples,
        bool pcm16)
    {
        var built = new BuiltBank(pcm16);

        foreach (var ((bank, program), notes) in needed.OrderBy(k => k.Key.Bank).ThenBy(k => k.Key.Program))
        {
            int sourceIndex = source.FindByProgram(bank, program);
            if (sourceIndex < 0 && bank == 128)
                sourceIndex = source.FindByProgram(128, 0);
            if (sourceIndex < 0)
                sourceIndex = source.FindByProgram(0, program);
            if (sourceIndex < 0)
                throw new InvalidOperationException($"Soundfont has no instrument for bank {bank}, program {program}.");

            var sourceInstrument = source.Instruments[sourceIndex];
            var played = notes
                .GroupBy(n => n.Note)
                .Select(g => (Note: g.Key, Velocity: Math.Max(1, (int)Math.Round(g.Average(n => n.Velocity)))))
                .OrderBy(n => n.Note)
                .ToList();
            if (played.Count == 0)
                continue;

            int firstRegion = built.Regions.Count;
            var report = new InstrumentBuildReport(
                (byte)Math.Clamp(bank, 0, 255),
                (byte)Math.Clamp(program, 0, 127),
                sourceInstrument.Name,
                played.Count,
                notes.Count);
            int regionCount = Math.Clamp(regionsPerInstrument, 1, played.Count);
            for (int r = 0; r < regionCount; r++)
            {
                int start = r * played.Count / regionCount;
                int end = ((r + 1) * played.Count / regionCount) - 1;
                int mid = (start + end) / 2;
                var representative = played[mid];
                var sourceRegion = FindBestRegion(sourceInstrument, representative.Note, representative.Velocity, played);
                if (sourceRegion is null)
                    continue;

                int keyLo = played[start].Note;
                int keyHi = played[end].Note;
                AddMicroRegion(built, sourceRegion, keyLo, keyHi,
                    maxRegionSamples, maxSampleBytes, report);
            }

            int added = built.Regions.Count - firstRegion;
            if (added == 0)
                throw new InvalidOperationException(
                    $"Soundfont instrument {sourceInstrument.Name} has no usable micro sample regions for bank {bank}, program {program}.");

            built.Instruments.Add(new BankInstrument(
                (byte)Math.Clamp(bank, 0, 255),
                (byte)Math.Clamp(program, 0, 127),
                (ushort)firstRegion,
                (ushort)added,
                sourceInstrument.Name));
            built.Reports.Add(report);
        }

        return built;
    }

    private static SampleRegion? FindBestRegion(
        SampleInstrument instrument,
        int note,
        int velocity,
        IReadOnlyList<(int Note, int Velocity)> played)
    {
        var region = instrument.Regions.FirstOrDefault(r => r.SampleData.Length > 0 && r.Matches(note, velocity));
        if (region is not null)
            return region;

        region = instrument.Regions.FirstOrDefault(r => r.SampleData.Length > 0 && r.Matches(note, 100));
        if (region is not null)
            return region;

        foreach (var n in played)
        {
            region = instrument.Regions.FirstOrDefault(r => r.SampleData.Length > 0 && r.Matches(n.Note, n.Velocity));
            if (region is not null)
                return region;
        }

        return instrument.Regions.FirstOrDefault(r => r.SampleData.Length > 0);
    }

    private static void AddRegion(
        BuiltBank built,
        SampleRegion source,
        int maxSampleBytes,
        InstrumentBuildReport report)
    {
        if (source.SampleData.Length == 0)
            return;

        int sampleLength = source.SampleData.Length;
        bool loop = source.LoopEnabled &&
            source.LoopEnd > source.LoopStart &&
            source.LoopStart >= 0 &&
            source.LoopEnd <= sampleLength;
        int storedLength = loop ? source.LoopEnd : sampleLength;
        var sample = AddSampleBlob(
            built,
            new[] { new SampleSpan(source.SampleData, 0, storedLength) },
            maxSampleBytes);
        int sampleStart = sample.StartFrame;
        int loopStart = loop ? sampleStart + source.LoopStart : sampleStart;
        int loopEnd = loop ? sampleStart + source.LoopEnd : sampleStart + sampleLength;
        report.AddRegion(
            built.BytesPerSample,
            sampleLength,
            storedLength,
            sample.Reused);

        built.Regions.Add(new BankRegion(
            (byte)Math.Clamp(source.KeyRangeLo, 0, 127),
            (byte)Math.Clamp(source.KeyRangeHi, 0, 127),
            (byte)Math.Clamp(source.VelocityRangeLo, 0, 127),
            (byte)Math.Clamp(source.VelocityRangeHi, 0, 127),
            (byte)Math.Clamp(source.RootKey, 0, 127),
            loop ? (byte)1 : (byte)0,
            (ushort)Math.Clamp(source.SampleRate, 1, ushort.MaxValue),
            (short)Math.Clamp((int)Math.Round(source.TuningCents), short.MinValue, short.MaxValue),
            (uint)sampleStart,
            (uint)storedLength,
            (uint)loopStart,
            (uint)loopEnd,
            SecondsToFrames(source.AttackTime),
            SecondsToFrames(source.DecayTime),
            (ushort)Math.Clamp((int)Math.Round(source.SustainLevel * 65535), 0, ushort.MaxValue),
            SecondsToFrames(source.ReleaseTime)));
    }

    private static void AddMicroRegion(
        BuiltBank built,
        SampleRegion source,
        int keyLo,
        int keyHi,
        int maxRegionSamples,
        int maxSampleBytes,
        InstrumentBuildReport report)
    {
        if (source.SampleData.Length == 0)
            return;

        int attackSamples = 0;
        int loopSamples = 0;
        int copySamples;
        bool loop = false;

        if (source.LoopEnabled &&
            source.LoopEnd > source.LoopStart + 32 &&
            source.LoopStart >= 0 &&
            source.LoopEnd <= source.SampleData.Length)
        {
            attackSamples = Math.Min(source.LoopStart, Math.Max(0, maxRegionSamples / 4));
            loopSamples = Math.Min(source.LoopEnd - source.LoopStart, maxRegionSamples - attackSamples);
            loop = loopSamples >= 32;
            if (!loop)
            {
                attackSamples = 0;
                loopSamples = 0;
            }
        }

        if (loop)
        {
            copySamples = attackSamples + loopSamples;
        }
        else
        {
            copySamples = Math.Min(source.SampleData.Length, maxRegionSamples);
        }

        if (copySamples <= 0)
            return;

        SampleBlobRef sample;
        if (loop)
        {
            sample = AddSampleBlob(
                built,
                new[]
                {
                    new SampleSpan(source.SampleData, 0, attackSamples),
                    new SampleSpan(source.SampleData, source.LoopStart, loopSamples)
                },
                maxSampleBytes);
        }
        else
        {
            sample = AddSampleBlob(
                built,
                new[] { new SampleSpan(source.SampleData, 0, copySamples) },
                maxSampleBytes);
        }

        int sampleStart = sample.StartFrame;
        int loopStart = loop ? sampleStart + attackSamples : sampleStart;
        int loopEnd = loop ? sampleStart + copySamples : sampleStart + copySamples;
        report.AddRegion(
            built.BytesPerSample,
            source.SampleData.Length,
            copySamples,
            sample.Reused);

        built.Regions.Add(new BankRegion(
            (byte)Math.Clamp(keyLo, 0, 127),
            (byte)Math.Clamp(keyHi, 0, 127),
            0,
            127,
            (byte)Math.Clamp(source.RootKey, 0, 127),
            loop ? (byte)1 : (byte)0,
            (ushort)Math.Clamp(source.SampleRate, 1, ushort.MaxValue),
            (short)Math.Clamp((int)Math.Round(source.TuningCents), short.MinValue, short.MaxValue),
            (uint)sampleStart,
            (uint)copySamples,
            (uint)loopStart,
            (uint)loopEnd,
            SecondsToFrames(source.AttackTime),
            SecondsToFrames(source.DecayTime),
            (ushort)Math.Clamp((int)Math.Round(source.SustainLevel * 65535), 0, ushort.MaxValue),
            SecondsToFrames(source.ReleaseTime)));
    }

    private static SampleBlobRef AddSampleBlob(
        BuiltBank built,
        IReadOnlyList<SampleSpan> spans,
        int maxSampleBytes)
    {
        byte[] bytes = EncodePcm(built.SampleFormat, spans);
        int frames = bytes.Length / built.BytesPerSample;
        var key = new SampleBlobKey(bytes.Length, HashBytes(bytes));
        if (built.SampleBlobs.TryGetValue(key, out var matches))
        {
            foreach (var match in matches)
            {
                if (BytesEqual(built.SampleBytes, match.ByteStart, bytes))
                    return new SampleBlobRef(match.StartFrame, frames, true);
            }
        }

        if (built.SampleBytes.Count + bytes.Length > maxSampleBytes)
            throw new InvalidOperationException(
                $"sample data exceeds {maxSampleBytes} bytes; raise --max-sample-bytes or reduce the MIDI subset");

        int byteStart = built.SampleBytes.Count;
        int startFrame = built.SampleFrameCount;
        built.SampleBytes.AddRange(bytes);

        var entry = new SampleBlobEntry(byteStart, startFrame, bytes.Length);
        if (matches is null)
        {
            matches = new List<SampleBlobEntry>();
            built.SampleBlobs.Add(key, matches);
        }
        matches.Add(entry);
        return new SampleBlobRef(startFrame, frames, false);
    }

    private static byte[] EncodePcm(ushort sampleFormat, IReadOnlyList<SampleSpan> spans)
    {
        int frameCount = spans.Sum(s => s.Count);
        int bytesPerSample = sampleFormat == SampleFormatPcm16 ? 2 : 1;
        byte[] bytes = new byte[frameCount * bytesPerSample];
        int offset = 0;

        foreach (var span in spans)
        {
            for (int i = 0; i < span.Count; i++)
            {
                float sample = Math.Clamp(span.Data[span.Start + i], -1f, 1f);
                if (sampleFormat == SampleFormatPcm16)
                {
                    short pcm = (short)Math.Clamp((int)MathF.Round(sample * 32767f), short.MinValue, short.MaxValue);
                    bytes[offset++] = (byte)(pcm & 0xFF);
                    bytes[offset++] = (byte)(((ushort)pcm >> 8) & 0xFF);
                }
                else
                {
                    sbyte pcm = (sbyte)Math.Clamp((int)MathF.Round(sample * 127f), -128, 127);
                    bytes[offset++] = unchecked((byte)pcm);
                }
            }
        }

        return bytes;
    }

    private static ulong HashBytes(byte[] bytes)
    {
        const ulong offset = 14695981039346656037UL;
        const ulong prime = 1099511628211UL;
        ulong hash = offset;
        foreach (byte b in bytes)
        {
            hash ^= b;
            hash *= prime;
        }
        return hash;
    }

    private static bool BytesEqual(List<byte> existing, int start, byte[] bytes)
    {
        if (start < 0 || start + bytes.Length > existing.Count)
            return false;

        for (int i = 0; i < bytes.Length; i++)
        {
            if (existing[start + i] != bytes[i])
                return false;
        }

        return true;
    }

    private static ushort SecondsToFrames(float seconds) =>
        (ushort)Math.Clamp((int)Math.Round(seconds * HardwareSampleRate), 0, ushort.MaxValue);

    private static void WriteBank(string path, BuiltBank bank)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(path)) ?? ".");
        using var fs = File.Create(path);
        using var writer = new BinaryWriter(fs);

        ushort version = bank.SampleFormat == SampleFormatPcm16 ? VersionPcm16 : VersionPcm8;
        ushort headerSize = bank.SampleFormat == SampleFormatPcm16
            ? (ushort)HeaderSizeV2
            : (ushort)HeaderSizeV1;
        uint instrumentTableOffset = headerSize;
        uint regionTableOffset = instrumentTableOffset + (uint)(bank.Instruments.Count * InstrumentRecordSize);
        uint sampleDataOffset = regionTableOffset + (uint)(bank.Regions.Count * RegionRecordSize);

        writer.Write(Magic);
        writer.Write(version);
        writer.Write(headerSize);
        writer.Write((ushort)InstrumentRecordSize);
        writer.Write((ushort)RegionRecordSize);
        writer.Write((ushort)bank.Instruments.Count);
        writer.Write((ushort)bank.Regions.Count);
        writer.Write((ushort)BakedSampleRate);
        writer.Write(instrumentTableOffset);
        writer.Write(regionTableOffset);
        writer.Write(sampleDataOffset);
        writer.Write((uint)bank.SampleBytes.Count);
        if (version >= VersionPcm16)
        {
            writer.Write(bank.SampleFormat);
            writer.Write((ushort)0);
        }

        foreach (var inst in bank.Instruments)
        {
            writer.Write(inst.Bank);
            writer.Write(inst.Program);
            writer.Write(inst.FirstRegion);
            writer.Write(inst.RegionCount);
            WriteFixedAscii(writer, inst.Name, 32);
            writer.Write((ushort)0);
        }

        foreach (var region in bank.Regions)
        {
            writer.Write(region.KeyLo);
            writer.Write(region.KeyHi);
            writer.Write(region.VelLo);
            writer.Write(region.VelHi);
            writer.Write(region.RootKey);
            writer.Write(region.Flags);
            writer.Write(region.SampleRate);
            writer.Write(region.TuningCents);
            writer.Write(region.SampleStart);
            writer.Write(region.SampleLength);
            writer.Write(region.LoopStart);
            writer.Write(region.LoopEnd);
            writer.Write(region.AttackFrames);
            writer.Write(region.DecayFrames);
            writer.Write(region.SustainLevel);
            writer.Write(region.ReleaseFrames);
            writer.Write((uint)0);
            writer.Write((ushort)0);
        }

        writer.Write(bank.SampleBytes.ToArray());
    }

    private static void WriteFixedAscii(BinaryWriter writer, string text, int length)
    {
        Span<byte> bytes = stackalloc byte[length];
        int n = Math.Min(text.Length, length - 1);
        for (int i = 0; i < n; i++)
            bytes[i] = (byte)(text[i] <= 0x7F ? text[i] : '?');
        writer.Write(bytes);
    }

    private static int ParsePositiveInt(string value, string option)
    {
        if (!int.TryParse(value, out int parsed) || parsed <= 0)
            throw new ArgumentException($"{option} expects a positive integer.");
        return parsed;
    }

    private static void PrintUsage()
    {
        Console.Error.WriteLine("Usage: sf2bank --sf2 soundfont.sf2 --output bank.nsfb [--full] [--midi song.mid] [--pcm16] [--micro] [--micro-regions N] [--max-region-samples N] [--max-sample-bytes N]");
    }

    private static void PrintReport(BuiltBank bank)
    {
        long selected = bank.Reports.Sum(r => r.SelectedBytes);
        long trimSaved = bank.Reports.Sum(r => r.TrimSavedBytes);
        long dedupeSaved = bank.Reports.Sum(r => r.DedupedBytes);
        int headerBytes = bank.SampleFormat == SampleFormatPcm16
            ? HeaderSizeV2
            : HeaderSizeV1;
        long metadataBytes =
            headerBytes +
            (long)bank.Instruments.Count * InstrumentRecordSize +
            (long)bank.Regions.Count * RegionRecordSize;

        Console.WriteLine(
            $"Report: selectedSampleBytes={selected}, trimSaved={trimSaved}, " +
            $"dedupeSaved={dedupeSaved}, storedSampleBytes={bank.SampleBytes.Count}, " +
            $"metadataBytes={metadataBytes}");
        Console.WriteLine("Top instruments by stored sample bytes:");

        foreach (var report in bank.Reports
                     .OrderByDescending(r => r.StoredBytes)
                     .ThenBy(r => r.Bank)
                     .ThenBy(r => r.Program)
                     .Take(ReportInstrumentCount))
        {
            Console.WriteLine(
                $"  bank={report.Bank} program={report.Program} " +
                $"regions={report.RegionCount} notes={report.NoteCount} " +
                $"noteOns={report.NoteOnCount} stored={report.StoredBytes} " +
                $"trimSaved={report.TrimSavedBytes} dedupeSaved={report.DedupedBytes} " +
                $"name=\"{report.Name}\"");
        }
    }

    private sealed class BuiltBank
    {
        public BuiltBank(bool pcm16)
        {
            SampleFormat = pcm16 ? SampleFormatPcm16 : SampleFormatPcm8;
        }

        public List<BankInstrument> Instruments { get; } = new();
        public List<BankRegion> Regions { get; } = new();
        public List<byte> SampleBytes { get; } = new();
        public Dictionary<SampleBlobKey, List<SampleBlobEntry>> SampleBlobs { get; } = new();
        public List<InstrumentBuildReport> Reports { get; } = new();
        public ushort SampleFormat { get; }
        public int BytesPerSample => SampleFormat == SampleFormatPcm16 ? 2 : 1;
        public int SampleFrameCount => SampleBytes.Count / BytesPerSample;
    }

    private sealed record BankInstrument(byte Bank, byte Program, ushort FirstRegion, ushort RegionCount, string Name);

    private sealed record BankRegion(
        byte KeyLo,
        byte KeyHi,
        byte VelLo,
        byte VelHi,
        byte RootKey,
        byte Flags,
        ushort SampleRate,
        short TuningCents,
        uint SampleStart,
        uint SampleLength,
        uint LoopStart,
        uint LoopEnd,
        ushort AttackFrames,
        ushort DecayFrames,
        ushort SustainLevel,
        ushort ReleaseFrames);

    private sealed class InstrumentBuildReport(
        byte bank,
        byte program,
        string name,
        int noteCount,
        int noteOnCount)
    {
        public byte Bank { get; } = bank;
        public byte Program { get; } = program;
        public string Name { get; } = name;
        public int NoteCount { get; } = noteCount;
        public int NoteOnCount { get; } = noteOnCount;
        public int RegionCount { get; private set; }
        public long SelectedBytes { get; private set; }
        public long StoredBytes { get; private set; }
        public long TrimSavedBytes { get; private set; }
        public long DedupedBytes { get; private set; }

        public void AddRegion(int bytesPerSample, int selectedFrames, int storedFrames, bool reused)
        {
            long selectedBytes = (long)selectedFrames * bytesPerSample;
            long storedBytes = (long)storedFrames * bytesPerSample;
            RegionCount++;
            SelectedBytes += selectedBytes;
            TrimSavedBytes += Math.Max(0, selectedBytes - storedBytes);
            if (reused)
                DedupedBytes += storedBytes;
            else
                StoredBytes += storedBytes;
        }
    }

    private readonly record struct SampleSpan(float[] Data, int Start, int Count);
    private readonly record struct SampleBlobKey(int ByteLength, ulong Hash);
    private readonly record struct SampleBlobEntry(int ByteStart, int StartFrame, int ByteLength);
    private readonly record struct SampleBlobRef(int StartFrame, int FrameCount, bool Reused);
}
