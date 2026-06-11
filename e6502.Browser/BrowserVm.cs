using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using e6502.Browser.Rendering;
using KDS.e6502;

namespace e6502.Browser;

public static class BrowserVm
{
    private const int BrowserCpuHz = VgcConstants.DefaultCpuHz;
    private static bool _initialized;

    private static CompositeBusDevice? _bus;
    private static Cpu? _cpu;
    private static ScreenEditor? _editor;
    private static BitmapFont? _font;

    public static void Initialize()
    {
        if (_initialized)
            return;

        _bus = new CompositeBusDevice(
            cpuHz: BrowserCpuHz,
            frameRateHz: VgcConstants.FrameRateHz);
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);
        BrowserInput.Initialize(_editor);

        var fontSlots = new List<byte[]>
        {
            LoadEmbeddedResource("cp437.bin")
        };

        byte[]? petUpper = TryLoadEmbeddedResource("petscii_upper.bin");
        if (petUpper != null)
            fontSlots.Add(petUpper);

        byte[]? petLower = TryLoadEmbeddedResource("petscii_lower.bin");
        if (petLower != null)
            fontSlots.Add(petLower);

        _font = new BitmapFont(fontSlots.ToArray());
        if (_font.SlotCount > 1)
            _font.SetKeymap(1, FontKeymap.PetsciiUpper);
        if (_font.SlotCount > 2)
            _font.SetKeymap(2, FontKeymap.PetsciiLower);

        _bus.Vgc.SetFont(_font);
        DirectCanvas.Initialize(_bus.Vgc, _font, _cpu, _bus, BrowserCpuHz);

        _initialized = true;
    }

    private static byte[] LoadEmbeddedResource(string name)
    {
        var asm = typeof(BrowserVm).Assembly;
        using var stream = asm.GetManifestResourceStream(name)
            ?? throw new FileNotFoundException($"Embedded resource '{name}' not found.");
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }

    private static byte[]? TryLoadEmbeddedResource(string name)
    {
        var asm = typeof(BrowserVm).Assembly;
        using var stream = asm.GetManifestResourceStream(name);
        if (stream == null)
            return null;
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }
}
