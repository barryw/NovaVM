namespace e6502.Avalonia.Hardware;

public sealed class SampleRegion
{
    public int KeyRangeLo { get; set; }
    public int KeyRangeHi { get; set; } = 127;
    public int VelocityRangeLo { get; set; }
    public int VelocityRangeHi { get; set; } = 127;
    public float[] SampleData { get; set; } = Array.Empty<float>();
    public int SampleRate { get; set; } = 44100;
    public int RootKey { get; set; } = 60;
    public int LoopStart { get; set; }
    public int LoopEnd { get; set; }
    public bool LoopEnabled { get; set; }
    public float AttackTime { get; set; } = 0.01f;
    public float DecayTime { get; set; } = 0.1f;
    public float SustainLevel { get; set; } = 0.7f;
    public float ReleaseTime { get; set; } = 0.3f;

    public bool Matches(int note, int velocity) =>
        note >= KeyRangeLo && note <= KeyRangeHi &&
        velocity >= VelocityRangeLo && velocity <= VelocityRangeHi;
}

public sealed class SampleInstrument
{
    public string Name { get; set; } = "";
    public int MidiBank { get; set; }
    public int MidiProgram { get; set; }
    public List<SampleRegion> Regions { get; set; } = new();

    public SampleRegion? FindRegion(int note, int velocity)
    {
        foreach (var r in Regions)
            if (r.Matches(note, velocity))
                return r;
        return null;
    }
}

public sealed class SampleBank
{
    public List<SampleInstrument> Instruments { get; } = new();
}
