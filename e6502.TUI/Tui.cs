using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using KDS.e6502;
using Terminal.Gui;

Application.Init();

var bus     = new CompositeBusDevice();
var cpu     = new Cpu(bus);
cpu.Boot();

var display = new DisplayView(bus.Vgc);

var window = new Window
{
    Title  = "Enhanced 6502 BASIC",
    X      = 0,
    Y      = 1,
    Width  = Dim.Fill(),
    Height = Dim.Fill(),
};

var menu = new MenuBar
{
    Menus =
    [
        new MenuBarItem("_File",
        [
            new MenuItem("_Quit", "", () => Application.RequestStop()),
        ]),
    ]
};

display.X = 0;
display.Y = 0;

window.Add(display);

Application.Top!.Add(menu, window);

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

// 30 fps refresh
Application.AddTimeout(TimeSpan.FromMilliseconds(33), () =>
{
    display.SetNeedsDraw();
    return true;
});

// 500 ms cursor blink
Application.AddTimeout(TimeSpan.FromMilliseconds(500), () =>
{
    display.ToggleCursor();
    return true;
});

Application.Run();
running = false;
Application.Shutdown();
