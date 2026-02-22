using Dock.Model.Controls;
using Dock.Model.Core;
using Dock.Model.Mvvm;
using Dock.Model.Mvvm.Controls;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;

namespace e6502.Avalonia.DevTools;

public class DevDockFactory : Factory
{
    private readonly EmulatorCanvas _canvas;
    private readonly VirtualGraphicsController _vgc;

    public DevDockFactory(EmulatorCanvas canvas, VirtualGraphicsController vgc)
    {
        _canvas = canvas;
        _vgc = vgc;
    }

    public override IRootDock CreateLayout()
    {
        var emulatorDoc = new EmulatorDocument
        {
            Id = "Emulator",
            Title = "Emulator",
            Canvas = _canvas,
            CanClose = false,
            CanFloat = false
        };

        var spriteEditor = new SpriteEditorTool
        {
            Id = "SpriteEditor",
            Title = "Sprite Editor",
            Vgc = _vgc,
            CanClose = false,
            CanFloat = true
        };

        var documentDock = new DocumentDock
        {
            Id = "DocumentsPane",
            Title = "Documents",
            Proportion = 0.6,
            VisibleDockables = CreateList<IDockable>(emulatorDoc),
            ActiveDockable = emulatorDoc,
            CanCreateDocument = false
        };

        var toolDock = new ToolDock
        {
            Id = "ToolsPane",
            Title = "Tools",
            Proportion = 0.4,
            VisibleDockables = CreateList<IDockable>(spriteEditor),
            ActiveDockable = spriteEditor,
            Alignment = Alignment.Right
        };

        var mainLayout = new ProportionalDock
        {
            Id = "MainLayout",
            Orientation = Orientation.Horizontal,
            VisibleDockables = CreateList<IDockable>(
                documentDock,
                new ProportionalDockSplitter(),
                toolDock
            )
        };

        var rootDock = CreateRootDock();
        rootDock.Id = "Root";
        rootDock.Title = "Root";
        rootDock.ActiveDockable = mainLayout;
        rootDock.DefaultDockable = mainLayout;
        rootDock.VisibleDockables = CreateList<IDockable>(mainLayout);
        rootDock.IsFocusableRoot = true;

        return rootDock;
    }
}
