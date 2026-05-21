using System.Runtime.Versioning;

[assembly: SupportedOSPlatform("browser")]

namespace e6502.Browser;

internal sealed partial class Program
{
    private static Task Main(string[] args)
    {
        BrowserVm.Initialize();
        return Task.CompletedTask;
    }
}
