using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using KDS.e6502;
using Terminal.Gui;

var bus = new CompositeBusDevice();
var cpu = new Cpu(bus);
cpu.Boot();

var display = new DisplayView(bus.Vgc)
{
    X = 0,
    Y = 0,
    Width = Dim.Fill(),
    Height = Dim.Fill(),
};

var window = new Window
{
    Title = "Enhanced 6502 BASIC",
    // 80x25 display + 2 for window border
    Width = VgcConstants.ScreenCols + 2,
    Height = VgcConstants.ScreenRows + 2,
    X = Pos.Center(),
    Y = Pos.Center(),
};

window.Add(display);

// CPU runs on background thread
bool running = true;
var cpuThread = new Thread(() =>
{
    while (running)
        cpu.ExecuteNext();
})
{
    IsBackground = true,
    Name = "CpuThread",
};
cpuThread.Start();

Application.Init();

Application.AddTimeout(TimeSpan.FromMilliseconds(33), () =>
{
    display.SetNeedsDraw();
    return true;
});

Application.AddTimeout(TimeSpan.FromMilliseconds(500), () =>
{
    display.ToggleCursor();
    return true;
});

Application.Run(window);
running = false;
Application.Shutdown();
