using Avalonia;
using Avalonia.Markup.Xaml;

namespace e6502.Avalonia;

public partial class App : Application
{
    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }
}
