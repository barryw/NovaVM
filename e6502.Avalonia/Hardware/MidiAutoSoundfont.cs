namespace e6502.Avalonia.Hardware;

/// <summary>
/// Auto-loads a default soundfont when WTS has no instruments.
/// </summary>
public static class MidiAutoSoundfont
{
    private const string DefaultSoundfont = "GeneralUser_GS";

    public static void TryLoad(FileIoController fio, WavetableSynth wts)
    {
        if (wts.InstrumentCount > 0) return;

        string sfPath = Path.Combine(fio.SaveDirectory, "soundfonts", DefaultSoundfont + ".sf2");
        if (!File.Exists(sfPath)) return;

        try
        {
            using var fs = File.OpenRead(sfPath);
            var bank = Sf2Loader.Load(fs);
            wts.LoadBank(bank);
        }
        catch
        {
            // Silently fail — fall back to SID-only
        }
    }
}
