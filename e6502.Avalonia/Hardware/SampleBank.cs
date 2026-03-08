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
    public float HoldTime { get; set; }
    public short KeynumToHoldTC { get; set; }
    public float DecayTime { get; set; } = 0.1f;
    public short KeynumToDecayTC { get; set; }
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

    // Program lookup cache: key = (bank << 8) | program, value = index into Instruments
    private Dictionary<int, int>? _programIndex;

    /// <summary>
    /// Find instrument index by MIDI bank and program number.
    /// Returns -1 if not found.
    /// </summary>
    public int FindByProgram(int bank, int program)
    {
        if (_programIndex == null)
            RebuildProgramIndex();
        int key = (bank << 8) | (program & 0x7F);
        return _programIndex!.TryGetValue(key, out int idx) ? idx : -1;
    }

    /// <summary>
    /// Find instrument index by GM program number (bank 0).
    /// Returns -1 if not found.
    /// </summary>
    public int FindByProgram(int program) => FindByProgram(0, program);

    public void RebuildProgramIndex()
    {
        _programIndex = new Dictionary<int, int>();
        for (int i = 0; i < Instruments.Count; i++)
        {
            var inst = Instruments[i];
            int key = (inst.MidiBank << 8) | (inst.MidiProgram & 0x7F);
            _programIndex.TryAdd(key, i); // first match wins
        }
    }
}
