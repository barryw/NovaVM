namespace e6502.Avalonia.Hardware;

public enum MmlEventType
{
    NoteOn,         // Param1=MIDI note, Param2=duration in ticks
    Rest,           // Param2=duration in ticks
    SetTempo,       // Param1=BPM
    SetInstrument,  // Param1=instrument id
    SetOctave,      // Param1=octave (0-7)
    OctaveUp,
    OctaveDown,
    SetDefaultLen,  // Param1=length denominator (4=quarter)
    ArpStart,       // Param1=count, ArpNotes has MIDI notes, Param2=duration
    SetPulseWidth,  // Param1=pulse width (0-4095)
    PwmSweep,       // Param1=direction (-1, 0, +1)
    SetFilterCutoff,// Param1=cutoff (0-2047), Param2=resonance (0-15)
    FilterMode,     // Param1=mode (0=off, 1=LP, 2=BP, 4=HP, combos ok)
    FilterSweep,    // Param1=direction (-1, 0, +1)
    SetVibrato,     // Param1=depth (0=off)
    Portamento,     // flags next note as portamento target
    LoopStart,
    LoopEnd,        // Param1=repeat count
}

public readonly record struct MmlEvent(MmlEventType Type, int Param1 = 0, int Param2 = 0, int[]? ArpNotes = null);

public sealed class SidInstrument
{
    public byte Waveform;  // 0x10=tri, 0x20=saw, 0x40=pulse, 0x80=noise
    public byte Attack;    // 0-15
    public byte Decay;     // 0-15
    public byte Sustain;   // 0-15
    public byte Release;   // 0-15
    public ushort PulseWidth = 2048;

    public byte Ad => (byte)((Attack << 4) | Decay);
    public byte Sr => (byte)((Sustain << 4) | Release);
}
