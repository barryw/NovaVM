using Dock.Model.Mvvm.Controls;
using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.DevTools;

public class SpriteEditorTool : Tool
{
    public VirtualGraphicsController? Vgc { get; set; }

    private SpriteEditorView? _cachedView;

    public SpriteEditorTool()
    {
        Title = "Sprite Editor";
        CanClose = false;
        CanFloat = true;
    }

    public SpriteEditorView? GetOrCreateView()
    {
        if (Vgc is null) return null;
        return _cachedView ??= new SpriteEditorView(Vgc);
    }
}
