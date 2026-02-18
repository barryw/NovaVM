using Avalonia.Controls;
using Avalonia.Threading;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Ipc;
using e6502.Avalonia.Rendering;
using KDS.e6502;

namespace e6502.Avalonia;

public partial class MainWindow : Window
{
    private readonly CompositeBusDevice _bus;
    private readonly Cpu _cpu;
    private readonly ScreenEditor _editor;
    private readonly EmulatorCanvas _canvas;
    private bool _running = true;

    public MainWindow()
    {
        InitializeComponent();

        _bus = new CompositeBusDevice();
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);

        var fontPath = Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin");
        var font = BitmapFont.LoadFromFile(fontPath);

        _canvas = new EmulatorCanvas(_bus.Vgc, font, _editor);
        Content = _canvas;

        // TCP server for MCP
        int tcpPort = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int ep) ? ep : 6502;
        var tcpServer = new EmulatorTcpServer(_bus, _editor, _cpu, tcpPort);
        tcpServer.Start();

        // CPU thread
        var cpuThread = new Thread(() =>
        {
            while (_running) _cpu.ExecuteNext();
        }) { IsBackground = true, Name = "CpuThread" };
        cpuThread.Start();

        // 30fps render timer
        DispatcherTimer.Run(() =>
        {
            _bus.Vgc.IncrementFrameCounter();
            _canvas.RequestRedraw();
            return true;
        }, TimeSpan.FromMilliseconds(33));

        // Cursor blink timer
        DispatcherTimer.Run(() =>
        {
            _canvas.ToggleCursor();
            return true;
        }, TimeSpan.FromMilliseconds(500));

        Closing += (_, _) => _running = false;
        Opened += (_, _) => _canvas.Focus();
    }
}
