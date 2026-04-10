using System.Runtime.InteropServices.JavaScript;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Threading;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using e6502.Browser.Rendering;
using KDS.e6502;

namespace e6502.Browser;

/// <summary>
/// Simplified main view for the browser.
/// Rendering is handled by DirectCanvas (byte[] -> JS -> HTML5 Canvas),
/// completely bypassing Avalonia's SkiaSharp/CanvasKit pipeline.
/// This Avalonia UserControl exists only for keyboard focus and input routing.
/// </summary>
public partial class BrowserMainView : UserControl
{
    private readonly CompositeBusDevice _bus;
    private readonly Cpu _cpu;
    private readonly ScreenEditor _editor;
    private readonly BitmapFont _font;
    private bool _quoteMode;

    public BrowserMainView()
    {
        _bus = new CompositeBusDevice(
            cpuHz: VgcConstants.DefaultCpuHz,
            frameRateHz: VgcConstants.FrameRateHz);
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);

        // Load fonts from embedded resources
        var fontSlots = new List<byte[]>();
        fontSlots.Add(LoadEmbeddedResource("cp437.bin"));

        byte[]? petUpper = TryLoadEmbeddedResource("petscii_upper.bin");
        if (petUpper != null) fontSlots.Add(petUpper);

        byte[]? petLower = TryLoadEmbeddedResource("petscii_lower.bin");
        if (petLower != null) fontSlots.Add(petLower);

        _font = new BitmapFont(fontSlots.ToArray());
        if (_font.SlotCount > 1)
            _font.SetKeymap(1, FontKeymap.PetsciiUpper);
        if (_font.SlotCount > 2)
            _font.SetKeymap(2, FontKeymap.PetsciiLower);

        _bus.Vgc.SetFont(_font);

        // Initialize DirectCanvas with everything — it owns CPU + rendering
        // JS requestAnimationFrame calls DirectCanvas.Tick() which runs CPU + renders
        const int browserCpuHz = 2_000_000;
        DirectCanvas.Initialize(_bus.Vgc, _font, _cpu, _bus, browserCpuHz);

        // Minimal Avalonia content — invisible, exists only for keyboard focus
        Focusable = true;
        Width = 1;
        Height = 1;
        Opacity = 0;

        // Signal JS that the .NET runtime is ready for audio + canvas interop
        try { NotifyRuntimeReady(); } catch { /* JS not loaded yet */ }
    }

    [JSImport("globalThis.novaAudio.runtimeLoaded")]
    private static partial void NotifyRuntimeReady();

    protected override void OnKeyDown(KeyEventArgs e)
    {
        // Resume audio on first keypress (browser requires user gesture)
        OpenAlRenderer.ResumeWebAudio();

        // Check for font keymap graphic character input
        if (e.Key >= Key.A && e.Key <= Key.Z)
        {
            int fontIndex = _bus.Vgc.GetFontIndex();
            var keymap = _font.GetKeymap(fontIndex);
            KeyMod mod = KeyMod.None;
            if (e.KeyModifiers.HasFlag(KeyModifiers.Shift))
                mod = KeyMod.Shift;
            else if (e.KeyModifiers.HasFlag(KeyModifiers.Control) || e.KeyModifiers.HasFlag(KeyModifiers.Meta))
                mod = KeyMod.Ctrl;

            if (mod != KeyMod.None && keymap.TryMap(mod, (char)('A' + (e.Key - Key.A)), out byte ch))
            {
                _editor.QueueInput(ch);
                e.Handled = true;
                base.OnKeyDown(e);
                return;
            }
        }

        if ((e.KeyModifiers.HasFlag(KeyModifiers.Control) || e.KeyModifiers.HasFlag(KeyModifiers.Meta)) && e.Key == Key.V
            || (e.KeyModifiers.HasFlag(KeyModifiers.Shift) && e.Key == Key.Insert))
        {
            _ = PasteClipboardAsync();
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        switch (e.Key)
        {
            case Key.Left:
                _editor.QueueInput(28);
                _editor.CursorLeft();
                e.Handled = true;
                break;
            case Key.Right:
                _editor.QueueInput(29);
                _editor.CursorRight();
                e.Handled = true;
                break;
            case Key.Up:
                _editor.QueueInput(30);
                _editor.CursorUp();
                e.Handled = true;
                break;
            case Key.Down:
                _editor.QueueInput(31);
                _editor.CursorDown();
                e.Handled = true;
                break;
            case Key.Enter:
                _ = QueueEnterAsync();
                e.Handled = true;
                break;
            case Key.Back:
                _editor.QueueInput(0x08);
                e.Handled = true;
                break;
            case Key.Escape:
                _editor.QueueInput(0x03);
                e.Handled = true;
                break;
        }
        base.OnKeyDown(e);
    }

    protected override void OnTextInput(TextInputEventArgs e)
    {
        if (string.IsNullOrEmpty(e.Text))
        {
            base.OnTextInput(e);
            return;
        }

        bool handled = false;
        foreach (char ch in e.Text)
        {
            if (ch < 0x20 || ch > 0x7E)
                continue;
            QueuePrintableChar((byte)ch);
            handled = true;
        }

        if (handled)
            e.Handled = true;

        base.OnTextInput(e);
    }

    private async Task QueueEnterAsync()
    {
        await Dispatcher.UIThread.InvokeAsync(() => { }, DispatcherPriority.Background);
        await Task.Delay(4);
        await Dispatcher.UIThread.InvokeAsync(() => { }, DispatcherPriority.Background);
        await WaitForInputIdleAsync(maxWaitMs: 250);
        _quoteMode = false;
        _editor.QueueInput(0x0D);
    }

    private async Task PasteClipboardAsync()
    {
        var clipboard = TopLevel.GetTopLevel(this)?.Clipboard;
        if (clipboard is null)
            return;

        string? text = await global::Avalonia.Input.Platform.ClipboardExtensions.TryGetTextAsync(clipboard);
        if (string.IsNullOrEmpty(text))
            return;

        await WaitForInputIdleAsync(maxWaitMs: 3000);

        for (int i = 0; i < text.Length; i++)
        {
            char ch = text[i];

            if (ch == '\r' || ch == '\n')
            {
                if (ch == '\n' && i > 0 && text[i - 1] == '\r')
                    continue;
                _quoteMode = false;
                _editor.QueueInput(0x0D);
                await WaitForInputIdleAsync(maxWaitMs: 8000);
                continue;
            }

            if (ch == '\t')
                ch = ' ';

            if (ch >= 0x20 && ch <= 0x7E)
            {
                QueuePrintableChar((byte)ch);
            }
        }
    }

    private async Task WaitForInputIdleAsync(int maxWaitMs)
    {
        int waited = 0;
        while ((_editor.HasQueuedInput || !_bus.Vgc.IsCursorEnabled) && waited < maxWaitMs)
        {
            await Task.Delay(1);
            waited++;
        }
    }

    private void QueuePrintableChar(byte ch)
    {
        if (ch == '"')
            _quoteMode = !_quoteMode;
        if (!_quoteMode && ch >= 0x61 && ch <= 0x7A)
            ch -= 0x20;
        _editor.QueueInput(ch);
    }

    private static byte[] LoadEmbeddedResource(string name)
    {
        var asm = typeof(BrowserMainView).Assembly;
        using var stream = asm.GetManifestResourceStream(name)
            ?? throw new FileNotFoundException($"Embedded resource '{name}' not found.");
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }

    private static byte[]? TryLoadEmbeddedResource(string name)
    {
        var asm = typeof(BrowserMainView).Assembly;
        using var stream = asm.GetManifestResourceStream(name);
        if (stream == null) return null;
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }
}
