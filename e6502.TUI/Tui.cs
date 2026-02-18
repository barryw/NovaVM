using e6502.TUI;
using KDS.e6502;
using Terminal.Gui;

Application.Run<E6502Tui>();
Application.Shutdown();

internal class E6502Tui : Window
{
    private readonly Cpu _cpu;
    private ConsoleView _display;
    private readonly Thread _cpuThread;

    public E6502Tui()
    {
        SetupDisplay();

        _cpu = new Cpu(new TuiBasicBusDevice(_display));
        _cpu.Boot();

        _cpuThread = new Thread(RunBasic);
        _cpuThread.Start();
    }

    private void RunBasic()
    {
        do
        {
            _cpu.ExecuteNext();
        } while (_cpuThread.IsAlive);
    }

    private void SetupDisplay()
    {
        Title = "Enhanced 6502 BASIC";
        Width = 80;
        Height = 25;

        var menu = new MenuBar
        {
            Menus =
            [
                new MenuBarItem("_File",
                [
                    new MenuItem("_Quit", "", () =>
                    {
                        Application.RequestStop();
                    }),
                ]),
            ]
        };

        _display = new ConsoleView(0, 1, 80, 26);

        Add(menu, _display);
    }
}
