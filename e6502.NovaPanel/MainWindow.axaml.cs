using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace e6502.NovaPanel;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void InitializeComponent() => AvaloniaXamlLoader.Load(this);
}
