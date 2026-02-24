using System.Linq;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Threading;
using Dock.Avalonia.Controls;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.DevTools;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Help;
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
    private HelpPanel? _helpPanel;
    private HelpIndex? _helpIndex;
    private bool _helpVisible;

    public MainWindow()
    {
        InitializeComponent();

        int targetCpuHz = RuntimeOptions.GetIntFromEnvironment("NOVA_CPU_HZ", VgcConstants.DefaultCpuHz);
        bool turboMode = RuntimeOptions.GetFlagFromEnvironment("NOVA_TURBO");
        bool timingLog = RuntimeOptions.GetFlagFromEnvironment("NOVA_TIMING_LOG");

        _bus = new CompositeBusDevice(
            enableSound: true,
            cpuHz: targetCpuHz,
            frameRateHz: VgcConstants.FrameRateHz);
        _cpu = new Cpu(_bus);
        _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);
        InitializeHelp();

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
            var scheduler = new RealTimeCycleScheduler(
                targetCpuHz,
                maxBacklogCycles: Math.Max(targetCpuHz / 5, Math.Max(1, targetCpuHz / _bus.FrameRateHz) * 2));
            const int turboChunkCycles = 200_000;

            long telemetryStartTicks = Stopwatch.GetTimestamp();
            long telemetryCycles = 0;
            long telemetryFrames = 0;
            long lastFrameTotal = _bus.TotalFrames;
            double peakPendingCycles = 0;

            while (_running)
            {
                debugger.CheckBreakpointAndWait();

                int cycleBudget = turboMode ? turboChunkCycles : scheduler.TakeCycleBudget();
                if (cycleBudget <= 0)
                {
                    Thread.Yield();
                    continue;
                }

                int chunkCyclesExecuted = 0;
                while (_running && cycleBudget > 0)
                {
                    debugger.CheckBreakpointAndWait();

                    int cycles = _cpu.ClocksForNext();
                    _cpu.ExecuteNext();
                    _bus.AdvanceCycles(cycles);
                    chunkCyclesExecuted += cycles;

                    if (_bus.Timer.IrqPending || _bus.Nic.IrqPending || _bus.ConsumeRasterIrqPending())
                        _cpu.IrqWaiting = true;

                    cycleBudget -= cycles;
                }

                if (!timingLog)
                    continue;

                telemetryCycles += chunkCyclesExecuted;
                long frameNow = _bus.TotalFrames;
                telemetryFrames += frameNow - lastFrameTotal;
                lastFrameTotal = frameNow;
                if (!turboMode && scheduler.PendingCycles > peakPendingCycles)
                    peakPendingCycles = scheduler.PendingCycles;

                long nowTicks = Stopwatch.GetTimestamp();
                long elapsedTicks = nowTicks - telemetryStartTicks;
                if (elapsedTicks < Stopwatch.Frequency)
                    continue;

                double elapsedSec = (double)elapsedTicks / Stopwatch.Frequency;
                double effectiveMhz = telemetryCycles / elapsedSec / 1_000_000.0;
                double effectiveFps = telemetryFrames / elapsedSec;
                double backlogPct = turboMode || scheduler.MaxBacklogCycles == 0
                    ? 0
                    : peakPendingCycles * 100.0 / scheduler.MaxBacklogCycles;

                string line =
                    $"[VM CLOCK] mode={(turboMode ? "turbo" : "realtime")} targetHz={targetCpuHz} effMHz={effectiveMhz:F2} fps={effectiveFps:F2} backlog={backlogPct:F1}%";
                Debug.WriteLine(line);
                Console.WriteLine(line);

                telemetryStartTicks = nowTicks;
                telemetryCycles = 0;
                telemetryFrames = 0;
                peakPendingCycles = 0;
            }
        }) { IsBackground = true, Name = "CpuThread" };
        cpuThread.Start();

        // 60 Hz render timer (visual refresh only)
        DispatcherTimer.Run(() =>
        {
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
        if (e.Key == Key.F1)
        {
            ToggleHelpPanel();
            e.Handled = true;
            return;
        }

        if (e.Key == Key.Escape && _helpVisible)
        {
            HideHelpPanel();
            e.Handled = true;
            return;
        }

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

    private void InitializeHelp()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null) return;
        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        _helpIndex = new HelpIndex(topics);
    }

    private static string? FindHelpDirectory()
    {
        var dir = AppDomain.CurrentDomain.BaseDirectory;
        for (var i = 0; i < 6; i++)
        {
            var candidate = Path.Combine(dir, "help");
            if (Directory.Exists(candidate)) return candidate;
            candidate = Path.Combine(dir, "docs", "help");
            if (Directory.Exists(candidate)) return candidate;
            var parent = Directory.GetParent(dir)?.FullName;
            if (parent == null) break;
            dir = parent;
        }
        return null;
    }

    public void ShowHelpPanel(string? searchTerm = null)
    {
        if (_helpIndex == null) return;

        if (_helpPanel == null)
        {
            _helpPanel = new HelpPanel(_helpIndex);
            _helpPanel.CloseRequested += HideHelpPanel;
            _helpPanel.TryThisRequested += OnTryThisRequested;
        }

        if (!_helpVisible)
        {
            var currentContent = Content as Control;
            if (currentContent == null) return;

            Content = null;

            var grid = new Grid();
            grid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 280, MaxWidth = 500 });
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star));

            Grid.SetColumn(_helpPanel, 0);
            grid.Children.Add(_helpPanel);

            var splitter = new GridSplitter
            {
                Width = 4,
                Background = new SolidColorBrush(HelpStyles.CodeBorder)
            };
            Grid.SetColumn(splitter, 1);
            grid.Children.Add(splitter);

            App.DetachFromVisualParent(currentContent);
            Grid.SetColumn(currentContent, 2);
            grid.Children.Add(currentContent);

            Content = grid;
            _helpVisible = true;

            Width += 354;
        }

        if (searchTerm != null)
            _helpPanel.NavigateToTopic(searchTerm);
        else
            _helpPanel.ShowLastViewedOrBrowse();
    }

    public void HideHelpPanel()
    {
        if (!_helpVisible || Content is not Grid grid) return;

        var emulatorContent = grid.Children
            .OfType<Control>()
            .FirstOrDefault(c => Grid.GetColumn(c) == 2);

        if (emulatorContent != null)
        {
            grid.Children.Remove(emulatorContent);
            Content = emulatorContent;
        }

        _helpVisible = false;
        Width -= 354;
    }

    private void ToggleHelpPanel()
    {
        if (_helpVisible)
            HideHelpPanel();
        else
            ShowHelpPanel();
    }

    private async void OnTryThisRequested(string code)
    {
        // Check if a BASIC program is in memory by comparing program start/end pointers
        // These are at zero page addresses $2B/$2C (start) and $2D/$2E (end)
        var progStart = (ushort)(_bus.Read(0x002B) | (_bus.Read(0x002C) << 8));
        var progEnd = (ushort)(_bus.Read(0x002D) | (_bus.Read(0x002E) << 8));

        if (progEnd > progStart)
        {
            var dialog = new Window
            {
                Title = "Replace Program?",
                Width = 340,
                Height = 140,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                CanResize = false
            };

            var result = false;
            var stack = new StackPanel { Spacing = 12, Margin = new Thickness(20) };
            stack.Children.Add(new TextBlock
            {
                Text = "This will replace your current program. Continue?",
                TextWrapping = TextWrapping.Wrap,
                FontSize = 13
            });
            var btnPanel = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 8, HorizontalAlignment = HorizontalAlignment.Right };
            var cancelBtn = new Button { Content = "Cancel", Padding = new Thickness(16, 6) };
            cancelBtn.Click += (_, _) => dialog.Close();
            var okBtn = new Button { Content = "Replace & Enter", Padding = new Thickness(16, 6) };
            okBtn.Click += (_, _) => { result = true; dialog.Close(); };
            btnPanel.Children.Add(cancelBtn);
            btnPanel.Children.Add(okBtn);
            stack.Children.Add(btnPanel);
            dialog.Content = stack;

            await dialog.ShowDialog(this);
            if (!result) return;
        }

        EnterCodeIntoBasic(code);
    }

    private void EnterCodeIntoBasic(string code)
    {
        // Type NEW + Enter to clear any existing program
        QueueString("NEW");
        QueueByte(0x0D);

        DispatcherTimer.RunOnce(() =>
        {
            var lines = code.Split('\n', StringSplitOptions.RemoveEmptyEntries);
            for (var i = 0; i < lines.Length; i++)
            {
                var capturedLine = lines[i].TrimEnd();
                var delay = TimeSpan.FromMilliseconds(50 * (i + 1));
                DispatcherTimer.RunOnce(() =>
                {
                    var upper = ConvertToUpperOutsideQuotes(capturedLine);
                    QueueString(upper);
                    QueueByte(0x0D);
                }, delay);
            }
        }, TimeSpan.FromMilliseconds(100));
    }

    private void QueueString(string text)
    {
        foreach (var c in text)
            QueueByte((byte)c);
    }

    private void QueueByte(byte b)
    {
        _editor.QueueInput(b);
    }

    private static string ConvertToUpperOutsideQuotes(string text)
    {
        var sb = new System.Text.StringBuilder(text.Length);
        var inQuotes = false;
        foreach (var c in text)
        {
            if (c == '"') inQuotes = !inQuotes;
            sb.Append(!inQuotes && c >= 'a' && c <= 'z' ? (char)(c - 32) : c);
        }
        return sb.ToString();
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
