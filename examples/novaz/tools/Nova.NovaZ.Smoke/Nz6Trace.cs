using System.Globalization;
using KDS.e6502;

// Env-gated NZ6 dispatch trace (M2 window-model research; see
// docs/plans/2026-06-10-zork-zero-window-ops.md).
//
// When NOVAZ_SMOKE_TRACE_NZ6=<path> is set, every instruction step is sampled
// and a line is appended to <path> whenever the CPU lands on:
//   - nz6_entry ($2002, resolved from build/novaz6.sym): the NOVAZ6 segment
//     dispatch entry. A holds the dispatch id (NZ6_OP_* for VAR ids $00-$07,
//     NZ6_EXT_BASE($20)+opnum for EXT ops). Operands were pre-decoded by the
//     ROM into zvm_operand_lo/hi + zvm_operand_count before the jump.
//   - the three ROM-side layout-relevant VAR handlers that do NOT route
//     through the segment: zvm_erase_line, zvm_set_text_style,
//     zvm_buffer_mode (resolved from build/runtime.sym). Same operand ABI.
//
// Line format:
//   <seq> <id> <opname> count=<n> ops=<op0>,<op1>,<op2>,<op3>   (ops hex)
// ROM-side hooks log id "--". Turn markers ("--- turn: <command>") group ops
// by input. When the env var is unset this class is a single null check per
// instruction and the run is untouched.
static class Nz6Trace
{
    static readonly string[] VarOpNames =
    [
        "reset", "split_window", "set_window", "erase_window",
        "set_cursor", "get_cursor", "set_colour", "pull",
        "select", "set_text_style", "cr_newline"
    ];

    // EXT opnum -> name; indices mirror nz6_ext_table in src/zvm6.s.
    static readonly string[] ExtOpNames =
    [
        "save", "restore", "log_shift", "art_shift", "set_font",
        "draw_picture", "picture_data", "erase_picture", "set_margins",
        "save_undo", "restore_undo", "print_unicode", "check_unicode",
        "set_true_colour", "ext_14", "ext_15",
        "move_window", "window_size", "window_style", "get_wind_prop",
        "scroll_window", "pop_stack", "read_mouse", "mouse_window",
        "push_stack", "put_wind_prop", "print_form", "make_menu",
        "picture_table", "buffer_screen"
    ];

    const int ExtBase = 0x20;

    static StreamWriter? _writer;
    static long _seq;
    static int _entryPc = -1;
    static int _eraseLinePc = -1;
    static int _setTextStylePc = -1;
    static int _bufferModePc = -1;
    static int _outputStreamPc = -1;
    static int _logShiftPc = -1;
    static int _artShiftPc = -1;
    static int _scanTablePc = -1;
    static int _copyTablePc = -1;
    static int _printTablePc = -1;
    static int _printAddrPc = -1;
    static int _printPaddrPc = -1;
    static int _operandLo;
    static int _operandHi;
    static int _operandCount;

    public static bool Enabled => _writer is not null;

    public static void Init()
    {
        string? path = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_NZ6");
        if (string.IsNullOrWhiteSpace(path))
            return;

        _entryPc = ReadSymbol("novaz6.sym", "nz6_entry")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires nz6_entry in build/novaz6.sym.");
        _eraseLinePc = ReadSymbol("runtime.sym", "zvm_erase_line")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_erase_line in build/runtime.sym.");
        _setTextStylePc = ReadSymbol("runtime.sym", "zvm_set_text_style")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_set_text_style in build/runtime.sym.");
        _bufferModePc = ReadSymbol("runtime.sym", "zvm_buffer_mode")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_buffer_mode in build/runtime.sym.");
        _outputStreamPc = ReadSymbol("runtime.sym", "zvm_output_stream")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_output_stream in build/runtime.sym.");
        _logShiftPc = ReadSymbol("runtime.sym", "zvm_log_shift")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_log_shift in build/runtime.sym.");
        _artShiftPc = ReadSymbol("runtime.sym", "zvm_art_shift")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_art_shift in build/runtime.sym.");
        _scanTablePc = ReadSymbol("runtime.sym", "zvm_scan_table")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_scan_table in build/runtime.sym.");
        _copyTablePc = ReadSymbol("runtime.sym", "zvm_copy_table")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_copy_table in build/runtime.sym.");
        _printTablePc = ReadSymbol("runtime.sym", "zvm_print_table")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_print_table in build/runtime.sym.");
        _printAddrPc = ReadSymbol("runtime.sym", "zvm_print_addr")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_print_addr in build/runtime.sym.");
        _printPaddrPc = ReadSymbol("runtime.sym", "zvm_print_paddr")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_print_paddr in build/runtime.sym.");
        _operandLo = ReadSymbol("runtime.sym", "zvm_operand_lo")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_lo in build/runtime.sym.");
        _operandHi = ReadSymbol("runtime.sym", "zvm_operand_hi")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_hi in build/runtime.sym.");
        _operandCount = ReadSymbol("runtime.sym", "zvm_operand_count")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_count in build/runtime.sym.");

        string? dir = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        _writer = new StreamWriter(path, append: false) { AutoFlush = true };
        _writer.WriteLine("# NZ6 dispatch trace (NOVAZ_SMOKE_TRACE_NZ6)");
        _writer.WriteLine("# regenerate: make -C examples/novaz capture-z6-trace PROJECT=zork-zero");
        _writer.WriteLine($"# nz6_entry=${_entryPc:X4} zvm_erase_line=${_eraseLinePc:X4} zvm_set_text_style=${_setTextStylePc:X4} zvm_buffer_mode=${_bufferModePc:X4} zvm_output_stream=${_outputStreamPc:X4} zvm_log_shift=${_logShiftPc:X4} zvm_art_shift=${_artShiftPc:X4} zvm_scan_table=${_scanTablePc:X4} zvm_copy_table=${_copyTablePc:X4} zvm_print_table=${_printTablePc:X4} zvm_print_addr=${_printAddrPc:X4} zvm_print_paddr=${_printPaddrPc:X4}");
        _writer.WriteLine("# <seq> <id> <opname> count=<n> ops=<op0>,<op1>,<op2>,<op3>  (ops hex; id '--' = ROM-side hook)");
    }

    public static void Sample(Cpu cpu, IBusDevice bus)
    {
        if (_writer is null)
            return;

        int pc = cpu.Pc;
        if (pc == _entryPc)
            LogOp(cpu.GetState().A, bus);
        else if (pc == _eraseLinePc)
            LogRomOp("erase_line", bus);
        else if (pc == _setTextStylePc)
            LogRomOp("set_text_style", bus);
        else if (pc == _bufferModePc)
            LogRomOp("buffer_mode", bus);
        else if (pc == _outputStreamPc)
            LogRomOp("output_stream", bus);
        else if (pc == _logShiftPc)
            LogRomOp("log_shift", bus);
        else if (pc == _artShiftPc)
            LogRomOp("art_shift", bus);
        else if (pc == _scanTablePc)
            LogRomOp("scan_table", bus);
        else if (pc == _copyTablePc)
            LogRomOp("copy_table", bus);
        else if (pc == _printTablePc)
            LogRomOp("print_table", bus);
        else if (pc == _printAddrPc)
            LogRomOp("print_addr", bus);
        else if (pc == _printPaddrPc)
            LogRomOp("print_paddr", bus);
    }

    public static void Marker(string text) => _writer?.WriteLine(text);

    public static void Close()
    {
        _writer?.Dispose();
        _writer = null;
    }

    static void LogOp(byte id, IBusDevice bus)
    {
        string name = id < ExtBase
            ? (id < VarOpNames.Length ? VarOpNames[id] : $"var_{id:X2}?")
            : (id - ExtBase < ExtOpNames.Length ? ExtOpNames[id - ExtBase] : $"ext_{id - ExtBase}?");
        WriteLine($"{id:X2}", name, bus);
    }

    static void LogRomOp(string name, IBusDevice bus) => WriteLine("--", name, bus);

    static void WriteLine(string id, string name, IBusDevice bus)
    {
        int count = bus.Read((ushort)_operandCount);
        int op0 = ReadOperand(bus, 0);
        int op1 = ReadOperand(bus, 1);
        int op2 = ReadOperand(bus, 2);
        int op3 = ReadOperand(bus, 3);
        _writer!.WriteLine($"{_seq++} {id} {name} count={count} ops={op0:X4},{op1:X4},{op2:X4},{op3:X4}");
    }

    static int ReadOperand(IBusDevice bus, int index) =>
        (bus.Read((ushort)(_operandHi + index)) << 8) | bus.Read((ushort)(_operandLo + index));

    // Same ld65 -Ln format TryReadRuntimeSymbol parses, but parametrized by
    // symbol file so the segment's build/novaz6.sym can be read too.
    static int? ReadSymbol(string fileName, string symbolName)
    {
        string[] candidates =
        [
            Path.Combine(Environment.CurrentDirectory, "build", fileName),
            Path.Combine(Environment.CurrentDirectory, "examples", "novaz", "build", fileName)
        ];

        foreach (string path in candidates)
        {
            if (!File.Exists(path))
                continue;

            foreach (string line in File.ReadLines(path))
            {
                string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length < 3 || !parts[2].TrimStart('.').Equals(symbolName, StringComparison.Ordinal))
                    continue;
                if (int.TryParse(parts[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out int address))
                    return address;
            }
        }

        return null;
    }
}
