using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Threading;
using Dock.Avalonia.Controls;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.DevTools;
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
    private bool _devMode;
    private DockControl? _dockControl;
    private DevDockFactory? _dockFactory;
    private long _lastMoveResizeTick;
    private PixelPoint _lastWindowPos;
    private Size _lastWindowSize;

    public MainWindow()
    {
        InitializeComponent();

        _bus = new CompositeBusDevice(enableSound: true);
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);

        var fontPath = Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin");
        var font = BitmapFont.LoadFromFile(fontPath);

        _canvas = new EmulatorCanvas(_bus.Vgc, font, _editor);
        Content = _canvas;

        // Debugger service
        var debugger = new DebuggerService(_cpu, _bus);

        // TCP server for MCP
        int tcpPort = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int ep) ? ep : 6502;
        var tcpServer = new EmulatorTcpServer(_bus, _editor, _cpu, debugger, tcpPort);
        tcpServer.Start();

        // CPU thread
        var cpuThread = new Thread(() =>
        {
            int timerAccum = 0;
            while (_running)
            {
                debugger.CheckBreakpointAndWait();
                int cycles = _cpu.ClocksForNext();
                _cpu.ExecuteNext();
                timerAccum += cycles;
                if (timerAccum >= 100)
                {
                    _bus.Timer.Tick();
                    if (_bus.Timer.IrqPending)
                        _cpu.IrqWaiting = true;
                    if (_bus.Nic.IrqPending)
                        _cpu.IrqWaiting = true;
                    timerAccum -= 100;
                }
            }
        }) { IsBackground = true, Name = "CpuThread" };
        cpuThread.Start();

        // 60 Hz render timer (also drives BASIC VSYNC frame counter)
        DispatcherTimer.Run(() =>
        {
            _bus.Vgc.IncrementFrameCounter();
            if (_bus.Vgc.IsRasterIrqEnabled)
                _cpu.IrqWaiting = true;
            _bus.Music.Tick();

            // Detect window move/resize — skip expensive redraw during drag
            var pos = Position;
            var size = ClientSize;
            if (pos != _lastWindowPos || size != _lastWindowSize)
            {
                _lastWindowPos = pos;
                _lastWindowSize = size;
                _lastMoveResizeTick = Environment.TickCount64;
                UiTransitionGate.PauseFor(300);
            }

            if (!UiTransitionGate.IsPaused && Environment.TickCount64 - _lastMoveResizeTick > 200)
                _canvas.RequestRedraw();

            return true;
        }, TimeSpan.FromMilliseconds(1000.0 / 60.0));

        // Cursor blink timer
        DispatcherTimer.Run(() =>
        {
            if (!UiTransitionGate.IsPaused)
                _canvas.ToggleCursor();
            return true;
        }, TimeSpan.FromMilliseconds(500));

        // Pause rendering during maximize/restore/minimize transitions
        PropertyChanged += (_, e) =>
        {
            if (e.Property == WindowStateProperty)
            {
                _lastMoveResizeTick = Environment.TickCount64;
                UiTransitionGate.PauseFor(450);
            }
        };

        Closing += (_, _) =>
        {
            _running = false;
            _bus.Dispose();
        };
        Opened += (_, _) =>
        {
            _canvas.Focus();
            if (OperatingSystem.IsMacOS())
                SetMacAspectRatio();
        };
    }

    protected override void OnKeyDown(KeyEventArgs e)
    {
        var mod = OperatingSystem.IsMacOS() ? KeyModifiers.Meta : KeyModifiers.Control;
        if (e.KeyModifiers.HasFlag(mod))
        {
            switch (e.Key)
            {
                case Key.F:
                    WindowState = WindowState == WindowState.FullScreen
                        ? WindowState.Normal
                        : WindowState.FullScreen;
                    e.Handled = true;
                    return;
                case Key.D:
                    ToggleDevMode();
                    e.Handled = true;
                    return;
            }
        }
        if (e.Key == Key.F11)
        {
            WindowState = WindowState == WindowState.FullScreen
                ? WindowState.Normal
                : WindowState.FullScreen;
            e.Handled = true;
            return;
        }

        base.OnKeyDown(e);
    }

    private void ToggleDevMode()
    {
        if (_devMode)
            ExitDevMode();
        else
            EnterDevMode();
    }

    private void EnterDevMode()
    {
        _devMode = true;
        UiTransitionGate.PauseFor(450);

        if (OperatingSystem.IsMacOS())
            ClearMacAspectRatio();

        App.DetachFromVisualParent(_canvas);
        Content = null;

        if (_dockControl is null)
        {
            _dockFactory = new DevDockFactory(_canvas, _bus.Vgc);
            var layout = _dockFactory.CreateLayout();
            _dockFactory.InitLayout(layout);

            _dockControl = new DockControl
            {
                Layout = layout
            };
        }

        Content = _dockControl;
        Width = 1400;
        Height = 850;
    }

    private void ExitDevMode()
    {
        _devMode = false;
        UiTransitionGate.PauseFor(450);
        App.DetachFromVisualParent(_canvas);
        Content = null;
        Content = _canvas;
        Width = 640;
        Height = 400;
        _canvas.Focus();
        if (OperatingSystem.IsMacOS())
            SetMacAspectRatio();
    }

    private void ClearMacAspectRatio()
    {
        var handle = TryGetPlatformHandle();
        if (handle is null) return;

        // setAspectRatio: with {0,0} doesn't properly clear the constraint.
        // Apple docs: resizeIncrements and aspectRatio are mutually exclusive —
        // setting one clears the other. So we set resizeIncrements to {1,1}
        // (the default "no constraint" value) to fully remove the aspect ratio.
        var nsWindow = handle.Handle;
        var sel = sel_registerName("setResizeIncrements:");
        objc_msgSend_NSSize(nsWindow, sel, new NSSize { width = 1, height = 1 });
    }

    private void SetMacAspectRatio()
    {
        var handle = TryGetPlatformHandle();
        if (handle is null) return;

        var nsWindow = handle.Handle;
        var aspectSel = sel_registerName("setAspectRatio:");
        objc_msgSend_NSSize(nsWindow, aspectSel,
            new NSSize { width = EmulatorCanvas.NativeWidth, height = EmulatorCanvas.NativeHeight });
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct NSSize { public double width; public double height; }

    [DllImport("/usr/lib/libobjc.dylib", EntryPoint = "objc_msgSend")]
    private static extern void objc_msgSend_NSSize(IntPtr receiver, IntPtr selector, NSSize size);

    [DllImport("/usr/lib/libobjc.dylib")]
    private static extern IntPtr sel_registerName(string name);
}
