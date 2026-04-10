namespace e6502.Avalonia.Hardware;

/// <summary>
/// Browser stub — no filesystem access for soundfont loading.
/// WTS voices will be silent in the browser version.
/// </summary>
public static class MidiAutoSoundfont
{
    public static void TryLoad(FileIoController fio, WavetableSynth wts)
    {
        // No-op in browser — soundfonts require filesystem access
    }
}
