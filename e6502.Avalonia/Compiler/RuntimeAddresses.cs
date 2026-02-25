namespace e6502.Avalonia.Compiler;

/// <summary>
/// Fixed entry-point addresses for NCC runtime library routines in the NCC ROM.
/// The compiler generates JSR calls to these addresses.
/// All routines live in the NCC ROM at $C000-$FFFF.
/// </summary>
public static class RuntimeAddresses
{
    // ── Jump table at start of ROM ──────────────────────────────────────────
    // Each entry is a JMP to the actual routine. 3 bytes per entry.
    // Using a jump table lets us move routine implementations without
    // changing compiled code.

    public const ushort RomBase        = 0xC000;

    // Jump table entries (each JMP = 3 bytes)
    public const ushort MulByte        = 0xC000;  // byte multiply: A*X → A (lo) X (hi)
    public const ushort MulInt         = 0xC003;  // 16-bit multiply: ZP_TMP0 * ZP_TMP1 → ZP_TMP0
    public const ushort DivByte        = 0xC006;  // byte divide: A/X → A (quotient) X (remainder)
    public const ushort DivInt         = 0xC009;  // 16-bit divide
    public const ushort ModByte        = 0xC00C;  // byte modulo: A%X → A
    public const ushort Print          = 0xC00F;  // print null-terminated string: ZP_PTR0 → screen
    public const ushort PrintChar      = 0xC012;  // print char in A
    public const ushort GetKey         = 0xC015;  // non-blocking read: result in A (0=no key)
    public const ushort WaitKey        = 0xC018;  // blocking read: waits, result in A
    public const ushort VSync          = 0xC01B;  // wait for next vertical blank
    public const ushort MemCpy         = 0xC01E;  // copy ZP_TMP2 bytes from ZP_PTR0 to ZP_TMP0
    public const ushort MemSet         = 0xC021;  // fill ZP_TMP1 bytes at ZP_PTR0 with A
    public const ushort GfxCmd         = 0xC024;  // execute VGC graphics command (params pre-loaded)
    public const ushort SpriteCmd      = 0xC027;  // execute VGC sprite command
    public const ushort PlaySound      = 0xC02A;  // play SFX: note=A, dur=X, inst=Y
    public const ushort MusicPlay      = 0xC02D;  // start music playback
    public const ushort MusicStop      = 0xC030;  // stop music
    public const ushort SetVolume      = 0xC033;  // set master volume (A=0-15)
    public const ushort FixedMul       = 0xC036;  // 8.8 fixed multiply: ZP_TMP0 * ZP_TMP1 → ZP_TMP0
    public const ushort FixedDiv       = 0xC039;  // 8.8 fixed divide: ZP_TMP0 / ZP_TMP1 → ZP_TMP0
    public const ushort EditorEntry    = 0xC03C;  // editor main loop (Phase 8)

    // Zero-page addresses used by runtime (must match CodeGen)
    public const byte ZP_TMP0 = 0xF0;
    public const byte ZP_TMP1 = 0xF2;
    public const byte ZP_TMP2 = 0xF4;
    public const byte ZP_PTR0 = 0xF6;
    public const byte ZP_FP   = 0xF8;
}
