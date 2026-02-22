using Dock.Model.Mvvm.Controls;
using e6502.Avalonia.Rendering;

namespace e6502.Avalonia.DevTools;

public class EmulatorDocument : Document
{
    public EmulatorCanvas? Canvas { get; set; }

    public EmulatorDocument()
    {
        Title = "Emulator";
        CanClose = false;
        CanFloat = false;
    }
}
