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
using e6502.Avalonia.Editor;
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
    private DocEditorPanel? _docEditor;
    private bool _docEditorVisible;
    private DebuggerService _debugger = null!;
    private NccEditor? _nccEditor;

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
        _bus.RomSwapRequested += (_, _) => _cpu.Boot();

        _editor = new ScreenEditor(_bus.Vgc);
        _bus.Vgc.SetScreenEditor(_editor);
        _bus.HelpRequested += searchTerm =>
        {
            Dispatcher.UIThread.Post(() => ShowHelpPanel(searchTerm));
        };
        _bus.Fio.ProgramSaved += (name, isNewDoc) =>
        {
            if (isNewDoc)
            {
                Dispatcher.UIThread.Post(() =>
                {
                    string mdPath = Path.Combine(_bus.Fio.SaveDirectory, name + ".md");
                    ShowDocEditor(mdPath);
                });
            }
        };
        _bus.NccEditorRequested += () =>
        {
            Dispatcher.UIThread.Post(() =>
            {
                if (_nccEditor is { IsActive: false })
                    _nccEditor.Activate();
            });
        };
        InitializeHelp();

        var resDir = Path.Combine(AppContext.BaseDirectory, "Resources");
        var fontSlots = new List<byte[]>();
        fontSlots.Add(File.ReadAllBytes(Path.Combine(resDir, "cp437.bin")));

        var petUpper = Path.Combine(resDir, "petscii_upper.bin");
        if (File.Exists(petUpper))
            fontSlots.Add(File.ReadAllBytes(petUpper));

        var petLower = Path.Combine(resDir, "petscii_lower.bin");
        if (File.Exists(petLower))
            fontSlots.Add(File.ReadAllBytes(petLower));

        var font = new BitmapFont(fontSlots.ToArray());
        if (font.SlotCount > 1)
            font.SetKeymap(1, FontKeymap.PetsciiUpper);
        if (font.SlotCount > 2)
            font.SetKeymap(2, FontKeymap.PetsciiLower);

        _canvas = new EmulatorCanvas(_bus.Vgc, font, _editor);
        Content = _canvas;

        // Debugger service
        _debugger = new DebuggerService(_cpu, _bus);

        // NCC editor
        _nccEditor = new NccEditor(_bus, _debugger, _cpu);
        _canvas.NccEditor = _nccEditor;

        // TCP server for MCP
        int tcpPort = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int ep) ? ep : 6502;
        var tcpServer = new EmulatorTcpServer(_bus, _editor, _cpu, _debugger, tcpPort);
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
                _debugger.CheckBreakpointAndWait();

                int cycleBudget = turboMode ? turboChunkCycles : scheduler.TakeCycleBudget();
                if (cycleBudget <= 0)
                {
                    Thread.Yield();
                    continue;
                }

                int chunkCyclesExecuted = 0;
                while (_running && cycleBudget > 0)
                {
                    _debugger.CheckBreakpointAndWait();

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

            // NCC editor: detect program halt (debugger paused while editor was running)
            if (_nccEditor is { IsActive: true })
            {
                if (_nccEditor.IsRunning && _debugger.IsPaused)
                    _nccEditor.StopRunning();
                else if (_nccEditor.Mode == EditorMode.Debug)
                    _nccEditor.CheckDebugBreak();
            }

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
        // Handle NCC activation confirmation (Y/N prompt on BASIC screen)
        if (_awaitingNccConfirm)
        {
            HandleNccConfirmKey(e.Key);
            e.Handled = true;
            return;
        }

        if (e.Key == Key.F1 && e.KeyModifiers.HasFlag(KeyModifiers.Shift))
        {
            // Shift+F1: open doc editor for current program
            if (_bus.CurrentProgramName != null)
            {
                string mdPath = Path.Combine(_bus.Fio.SaveDirectory, _bus.CurrentProgramName + ".md");
                if (!File.Exists(mdPath))
                    File.WriteAllText(mdPath, $"---\ntitle: {_bus.CurrentProgramName}\ntype: program\ncategory: Programs\nkeywords: [{_bus.CurrentProgramName}]\n---\n\n");
                _bus.LoadProgramHelp(_bus.CurrentProgramName);
                ShowDocEditor(mdPath);
            }
            e.Handled = true;
            return;
        }

        if (e.Key == Key.F1)
        {
            if (_nccEditor is { IsActive: true })
            {
                ShowHelpPanelNcc();
            }
            else if (_bus.CurrentProgramName != null && IsProgramRunning())
            {
                // Program is running — show its help
                if (_bus.CurrentProgramHelp != null)
                    ShowProgramHelpPanel(_bus.CurrentProgramHelp);
                else
                    ShowNoProgramHelpPanel(_bus.CurrentProgramName);
            }
            else
            {
                ToggleHelpPanel();
            }
            e.Handled = true;
            return;
        }

        if (e.Key == Key.Escape && (_helpVisible || _docEditorVisible))
        {
            if (_docEditorVisible) HideDocEditor();
            if (_helpVisible) HideHelpPanel();
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
                case Key.N:
                    ToggleNccEditor();
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

    private bool _awaitingNccConfirm;

    private void ToggleNccEditor()
    {
        if (_nccEditor == null) return;
        if (_nccEditor.IsActive)
        {
            // Delegate to NccEditor's Ctrl+Q exit handler (Y/N prompt)
            _nccEditor.HandleKeyDown(Key.Q, KeyModifiers.Control);
        }
        else
        {
            // Show confirmation on BASIC screen
            WriteToScreen(0, 24, "THIS WILL CLEAR THE BASIC PROGRAM.  ARE YOU SURE? (Y/N) ", 1);
            _awaitingNccConfirm = true;
        }
    }

    private void HandleNccConfirmKey(Key key)
    {
        _awaitingNccConfirm = false;
        // Clear the prompt line
        WriteToScreen(0, 24, new string(' ', 80), 1);
        if (key == Key.Y && _nccEditor is { IsActive: false })
            _nccEditor.Activate();
    }

    private void WriteToScreen(int col, int row, string text, byte fg)
    {
        for (int i = 0; i < text.Length && col + i < VgcConstants.ScreenCols; i++)
        {
            int offset = row * VgcConstants.ScreenCols + col + i;
            _bus.Write((ushort)(VgcConstants.CharRamBase + offset), (byte)text[i]);
            _bus.Write((ushort)(VgcConstants.ColorRamBase + offset), fg);
        }
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

            if (OperatingSystem.IsMacOS())
                ClearMacAspectRatio();

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

        _helpPanel?.RestoreFullView();

        if (_docEditorVisible && _docEditor != null)
        {
            // Rebuild with just canvas + doc editor
            grid.Children.Clear();
            Content = null;

            var newGrid = new Grid();
            newGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star));
            newGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
            newGrid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 300, MaxWidth = 550 });

            App.DetachFromVisualParent(_canvas);
            Grid.SetColumn(_canvas, 0);
            newGrid.Children.Add(_canvas);

            var splitter = new GridSplitter
            {
                Width = 4,
                Background = new SolidColorBrush(HelpStyles.CodeBorder)
            };
            Grid.SetColumn(splitter, 1);
            newGrid.Children.Add(splitter);

            App.DetachFromVisualParent(_docEditor);
            Grid.SetColumn(_docEditor, 2);
            newGrid.Children.Add(_docEditor);

            Content = newGrid;
        }
        else
        {
            var emulatorContent = grid.Children
                .OfType<Control>()
                .FirstOrDefault(c => Grid.GetColumn(c) == 2);

            if (emulatorContent != null)
            {
                grid.Children.Clear();
                Content = emulatorContent;
            }
        }

        _helpVisible = false;
        Width -= 354;

        if (!_docEditorVisible && OperatingSystem.IsMacOS())
            SetMacAspectRatio();
    }

    private void ToggleHelpPanel()
    {
        if (_helpVisible)
            HideHelpPanel();
        else
            ShowHelpPanel();
    }

    private void ShowProgramHelpPanel(HelpTopic topic)
    {
        if (_helpIndex == null) return;
        ShowHelpPanel();
        _helpPanel?.ShowProgramHelp(topic);
    }

    private void ShowNoProgramHelpPanel(string name)
    {
        if (_helpIndex == null) return;
        ShowHelpPanel();
        _helpPanel?.ShowNoProgramHelp(name);
    }

    public void ShowDocEditor(string mdPath)
    {
        if (_docEditorVisible) HideDocEditor();

        string content = File.Exists(mdPath) ? File.ReadAllText(mdPath) : "";
        _docEditor = new DocEditorPanel(mdPath, content);
        _docEditor.CloseRequested += HideDocEditor;
        _docEditor.Saved += () =>
        {
            // Refresh program help after save
            if (_bus.CurrentProgramName != null)
                _bus.LoadProgramHelp(_bus.CurrentProgramName);
        };

        // Get the current content (might be a grid with help panel, or just canvas)
        var currentContent = Content as Control;
        if (currentContent == null) return;

        if (OperatingSystem.IsMacOS())
            ClearMacAspectRatio();

        Content = null;

        var grid = new Grid();

        if (currentContent is Grid existingGrid && _helpVisible)
        {
            // Help panel already showing — reuse the existing grid and add doc editor on right
            foreach (var child in existingGrid.Children.OfType<Control>().ToList())
                existingGrid.Children.Remove(child);

            // Rebuild: help panel | splitter | canvas | splitter | doc editor
            grid.ColumnDefinitions.Add(existingGrid.ColumnDefinitions[0]); // help
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto)); // splitter
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star)); // canvas
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto)); // splitter
            grid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 300, MaxWidth = 550 });

            var helpPanel = _helpPanel;
            if (helpPanel != null)
            {
                App.DetachFromVisualParent(helpPanel);
                Grid.SetColumn(helpPanel, 0);
                grid.Children.Add(helpPanel);
            }

            var leftSplitter = new GridSplitter
            {
                Width = 4,
                Background = new SolidColorBrush(HelpStyles.CodeBorder)
            };
            Grid.SetColumn(leftSplitter, 1);
            grid.Children.Add(leftSplitter);

            // Find the canvas (was at column 2 in existing grid)
            var canvas = existingGrid.Children.Count > 0
                ? existingGrid.Children.OfType<Control>().FirstOrDefault()
                : _canvas;
            // Actually need to find canvas properly
            foreach (var child in new List<Control> { _helpPanel!, _canvas }.Where(c => c != null))
                App.DetachFromVisualParent(child);

            Grid.SetColumn(_canvas, 2);
            grid.Children.Add(_canvas);

            var rightSplitter = new GridSplitter
            {
                Width = 4,
                Background = new SolidColorBrush(HelpStyles.CodeBorder)
            };
            Grid.SetColumn(rightSplitter, 3);
            grid.Children.Add(rightSplitter);

            Grid.SetColumn(_docEditor, 4);
            grid.Children.Add(_docEditor);
        }
        else
        {
            // No help panel — just canvas + doc editor on right
            App.DetachFromVisualParent(currentContent);

            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star));
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
            grid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 300, MaxWidth = 550 });

            Grid.SetColumn(currentContent, 0);
            grid.Children.Add(currentContent);

            var splitter = new GridSplitter
            {
                Width = 4,
                Background = new SolidColorBrush(HelpStyles.CodeBorder)
            };
            Grid.SetColumn(splitter, 1);
            grid.Children.Add(splitter);

            Grid.SetColumn(_docEditor, 2);
            grid.Children.Add(_docEditor);
        }

        Content = grid;
        _docEditorVisible = true;
        Width += 354;
    }

    public void HideDocEditor()
    {
        if (!_docEditorVisible) return;

        // Save before closing
        _docEditor?.Save();

        if (Content is Grid grid)
        {
            if (_helpVisible && _helpPanel != null)
            {
                // Rebuild with just help panel + canvas
                grid.Children.Clear();
                Content = null;

                var newGrid = new Grid();
                newGrid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 280, MaxWidth = 500 });
                newGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
                newGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star));

                App.DetachFromVisualParent(_helpPanel);
                Grid.SetColumn(_helpPanel, 0);
                newGrid.Children.Add(_helpPanel);

                var splitter = new GridSplitter
                {
                    Width = 4,
                    Background = new SolidColorBrush(HelpStyles.CodeBorder)
                };
                Grid.SetColumn(splitter, 1);
                newGrid.Children.Add(splitter);

                App.DetachFromVisualParent(_canvas);
                Grid.SetColumn(_canvas, 2);
                newGrid.Children.Add(_canvas);

                Content = newGrid;
            }
            else
            {
                // Just restore canvas
                grid.Children.Clear();
                App.DetachFromVisualParent(_canvas);
                Content = _canvas;
            }
        }

        _docEditorVisible = false;
        _docEditor = null;
        Width -= 354;

        if (!_helpVisible && OperatingSystem.IsMacOS())
            SetMacAspectRatio();
    }

    private bool IsProgramRunning()
    {
        // EhBASIC zero page: Clineh ($88) = current line high byte.
        // In direct/immediate mode, Clineh = $FF.
        // When running a program, it holds the actual line number high byte.
        return _bus.Read(0x0088) != 0xFF;
    }

    private void ShowHelpPanelNcc()
    {
        ShowHelpPanel();
        _helpPanel?.SetActiveFilter(HelpTopicType.Ncc);
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
