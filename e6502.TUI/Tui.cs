using e6502.TUI.Hardware;
using e6502.TUI.Ipc;
using e6502.TUI.Rendering;
using KDS.e6502;
using Terminal.Gui;
using Attribute = Terminal.Gui.Attribute;

var bus = new CompositeBusDevice();
var cpu = new Cpu(bus);
cpu.Boot();

// ── Shared screen editor (used by DisplayView, TCP server, and VGC) ─
var editor = new ScreenEditor(bus.Vgc);

// ── TCP server for MCP / external control ────────────────────────────
int tcpPort = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int ep) ? ep : 6502;
using var tcpServer = new EmulatorTcpServer(bus, editor, cpu, tcpPort);
tcpServer.Start();

// ── Turbo Pascal / Borland IDE color scheme ──────────────────────────
var borlandBlue = new Color(0, 0, 170);
var borlandCyan = new Color(0, 170, 170);

var windowScheme = new ColorScheme
{
    Normal = new Attribute(Color.White, borlandBlue),
    Focus = new Attribute(Color.White, borlandBlue),
    HotNormal = new Attribute(Color.BrightYellow, borlandBlue),
    HotFocus = new Attribute(Color.BrightYellow, borlandBlue),
};

var menuScheme = new ColorScheme
{
    Normal = new Attribute(Color.Black, borlandCyan),
    Focus = new Attribute(Color.White, borlandBlue),
    HotNormal = new Attribute(Color.Red, borlandCyan),
    HotFocus = new Attribute(Color.BrightYellow, borlandBlue),
};

// ── Menu bar ─────────────────────────────────────────────────────────
var menu = new MenuBar
{
    Menus = new MenuBarItem[]
    {
        new MenuBarItem("_Emulator", new MenuItem[]
        {
            new MenuItem("_Cold Start", "", () => { bus.Vgc.Reset(); cpu.Boot(0xC000); }),
            new MenuItem("_Warm Start", "", () => cpu.Boot(0x0000)),
            null!, // separator
            new MenuItem("_Quit", "", () => Application.RequestStop()),
        }),
    },
    ColorScheme = menuScheme,
};

var display = new DisplayView(bus.Vgc, editor)
{
    X = 0,
    Y = Pos.Bottom(menu),
    Width = Dim.Fill(),
    Height = Dim.Fill(),
};

var window = new Window
{
    Title = "Enhanced 6502 BASIC",
    // 80x25 display + 2 for border + 1 for menu bar
    Width = VgcConstants.ScreenCols + 2,
    Height = VgcConstants.ScreenRows + 2 + 1,
    X = Pos.Center(),
    Y = Pos.Center(),
    ColorScheme = windowScheme,
    BorderStyle = LineStyle.Double,
};

window.Add(menu, display);

// ── CPU thread ───────────────────────────────────────────────────────
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
    bus.Vgc.IncrementFrameCounter();
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
