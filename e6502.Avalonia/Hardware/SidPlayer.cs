namespace e6502.Avalonia.Hardware;

public sealed class SidPlayer
{
    private const ushort DataBase  = 0x03D0;
    private const ushort CodeBase  = 0x03D2;
    private const ushort IrqVector = 0xFFFE;

    private readonly CompositeBusDevice _bus;
    private byte _savedIrqLo;
    private byte _savedIrqHi;
    private byte _savedIrqCtrl;

    public SidPlayer(CompositeBusDevice bus) => _bus = bus;

    public void Play(SidFileInfo info, int song = 1)
    {
        if (!info.IsValid) return;

        _savedIrqLo = _bus.Read(IrqVector);
        _savedIrqHi = _bus.Read((ushort)(IrqVector + 1));
        _savedIrqCtrl = _bus.Read((ushort)VgcConstants.RegIrqCtrl);

        for (int i = 0; i < info.Payload.Length; i++)
            _bus.Write((ushort)(info.LoadAddress + i), info.Payload[i]);

        InjectTrampoline(info, song);

        _bus.WriteRam(IrqVector, (byte)(CodeBase & 0xFF));
        _bus.WriteRam((ushort)(IrqVector + 1), (byte)(CodeBase >> 8));

        // Enable VGC raster IRQ — fires at 60 Hz synced to display refresh
        _bus.Write((ushort)VgcConstants.RegIrqCtrl, (byte)(_savedIrqCtrl | 0x01));
    }

    public void Stop()
    {
        // Disable raster IRQ (restore original state)
        _bus.Write((ushort)VgcConstants.RegIrqCtrl, _savedIrqCtrl);

        // Gate off all three SID voices and silence volume
        _bus.Write(0xD404, 0x00);
        _bus.Write(0xD40B, 0x00);
        _bus.Write(0xD412, 0x00);
        _bus.Write(0xD418, 0x00);

        // Flush any buffered audio so silence is immediate
        _bus.Sid.Flush();

        _bus.WriteRam(IrqVector, _savedIrqLo);
        _bus.WriteRam((ushort)(IrqVector + 1), _savedIrqHi);
    }

    private void InjectTrampoline(SidFileInfo info, int song)
    {
        byte initLo = (byte)(info.InitAddress & 0xFF);
        byte initHi = (byte)(info.InitAddress >> 8);
        byte playLo = (byte)(info.PlayAddress & 0xFF);
        byte playHi = (byte)(info.PlayAddress >> 8);

        // Data at $03D0
        _bus.Write(DataBase,                   0x01);           // init_flag = 1
        _bus.Write((ushort)(DataBase + 1), (byte)(song - 1));   // song_num (0-based)

        // Code at $03D2
        // Byte layout (offsets from $03D2):
        //  0: 48        PHA
        //  1: 8A        TXA
        //  2: 48        PHA
        //  3: 98        TYA
        //  4: 48        PHA
        //  5: AD D0 03  LDA $03D0        (check init flag)
        //  8: F0 0B     BEQ +11 → $03E7  (skip init block)
        // 10: AD D1 03  LDA $03D1        (song number)
        // 13: 20 lo hi  JSR init
        // 16: A9 00     LDA #$00
        // 18: 8D D0 03  STA $03D0        (clear init flag)
        // 21: 20 lo hi  JSR play          (offset +21 from $03D2 = $03E7)
        // 24: 68        PLA
        // 25: A8        TAY
        // 26: 68        PLA
        // 27: AA        TAX
        // 28: 68        PLA
        // 29: 40        RTI               (ends at $03EF)
        byte[] code =
        [
            0x48,                   // PHA
            0x8A,                   // TXA
            0x48,                   // PHA
            0x98,                   // TYA
            0x48,                   // PHA
            0xAD, 0xD0, 0x03,       // LDA $03D0
            0xF0, 0x0B,             // BEQ +11  ($03DA + 2 + 11 = $03E7)
            0xAD, 0xD1, 0x03,       // LDA $03D1
            0x20, initLo, initHi,   // JSR init
            0xA9, 0x00,             // LDA #$00
            0x8D, 0xD0, 0x03,       // STA $03D0
            0x20, playLo, playHi,   // JSR play    ← BEQ lands here ($03E7)
            0x68,                   // PLA
            0xA8,                   // TAY
            0x68,                   // PLA
            0xAA,                   // TAX
            0x68,                   // PLA
            0x40                    // RTI
        ];

        for (int i = 0; i < code.Length; i++)
            _bus.Write((ushort)(CodeBase + i), code[i]);
    }
}
