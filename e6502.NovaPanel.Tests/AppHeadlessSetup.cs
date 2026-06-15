using Avalonia;
using Avalonia.Headless;
using e6502.NovaPanel;

[assembly: AvaloniaTestApplication(typeof(e6502.NovaPanel.Tests.TestAppBuilder))]

namespace e6502.NovaPanel.Tests;

public static class TestAppBuilder
{
    public static AppBuilder BuildAvaloniaApp() =>
        AppBuilder.Configure<App>().UseHeadless(new AvaloniaHeadlessPlatformOptions());
}
