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
        "select", "set_text_style", "cr_newline", "pre_newline_scroll",
        "restore_display"
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
    static int _drawPicturePc = -1;
    static int _pictureRectSetupPc = -1;
    static int _pictureClipRectPc = -1;
    static int _drawStartUnpackPc = -1;
    static int _drawRts2Pc = -1;
    static int _pagerLoadPc = -1;
    static int _scrollCompositePc = -1;
    static int _scrollRowsCompositePc = -1;
    static int _vtextBltFillPlanePc = -1;
    static int _vtextBltCopyUpPlanePc = -1;
    static int _vtextPutCharPc = -1;
    static int _vtextPutRunPc = -1;
    static int _vtextClearRegionPc = -1;
    static int _vtextExposeGfxSpacesRegionPc = -1;
    static int _vtextClearLinePc = -1;
    static int _vtextScrollUpPc = -1;
    static int _operandLo;
    static int _operandHi;
    static int _operandCount;
    static int _vtextLeft = -1;
    static int _vtextTop = -1;
    static int _vtextWidth = -1;
    static int _vtextHeight = -1;
    static int _vtextCurX = -1;
    static int _vtextCurY = -1;
    static int _vtextChar = -1;
    static bool _tracePictures;
    static int _winCurrent = -1;
    static int _picW = -1;
    static int _picH = -1;
    static int _picFlags = -1;
    static int _picOff = -1;
    static int _picLen = -1;
    static int _bltCellX = -1;
    static int _bltCellY = -1;
    static int _bltPx = -1;
    static int _bltPy = -1;
    static int _bltDst = -1;
    static int _bltWBytes = -1;
    static int _bltWClip = -1;
    static int _bltHClip = -1;

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
        _drawPicturePc = ReadSymbol("novaz6.sym", "nz6_ext_draw_picture") ?? -1;
        _pictureRectSetupPc = ReadSymbol("novaz6.sym", "nz6_pic_rect_setup") ?? -1;
        _pictureClipRectPc = ReadSymbol("novaz6.sym", "nz6_pic_clip_rect") ?? -1;
        _drawStartUnpackPc = ReadSymbol("novaz6.sym", "@start_unpack") ?? -1;
        _drawRts2Pc = ReadSymbol("novaz6.sym", "@rts2") ?? -1;
        _pagerLoadPc = ReadSymbol("runtime.sym", "pager_load_file_page") ?? -1;
        _scrollCompositePc = ReadSymbol("novaz6.sym", "nz6_scroll_live_composite") ?? -1;
        _scrollRowsCompositePc = ReadSymbol("novaz6.sym", "nz6_scroll_live_rows_composite") ?? -1;
        _vtextBltFillPlanePc = ReadSymbol("runtime.sym", "vtext_blt_fill_plane") ?? -1;
        _vtextBltCopyUpPlanePc = ReadSymbol("runtime.sym", "vtext_blt_copy_up_plane") ?? -1;
        _vtextPutCharPc = ReadSymbol("runtime.sym", "vtext_put_char") ?? -1;
        _vtextPutRunPc = ReadSymbol("runtime.sym", "vtext_put_run") ?? -1;
        _vtextClearRegionPc = ReadSymbol("runtime.sym", "vtext_clear_region") ?? -1;
        _vtextExposeGfxSpacesRegionPc = ReadSymbol("runtime.sym", "vtext_expose_gfx_spaces_region") ?? -1;
        _vtextClearLinePc = ReadSymbol("runtime.sym", "vtext_clear_line") ?? -1;
        _vtextScrollUpPc = ReadSymbol("runtime.sym", "vtext_scroll_up") ?? -1;
        _operandLo = ReadSymbol("runtime.sym", "zvm_operand_lo")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_lo in build/runtime.sym.");
        _operandHi = ReadSymbol("runtime.sym", "zvm_operand_hi")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_hi in build/runtime.sym.");
        _operandCount = ReadSymbol("runtime.sym", "zvm_operand_count")
            ?? throw new InvalidOperationException("NOVAZ_SMOKE_TRACE_NZ6 requires zvm_operand_count in build/runtime.sym.");
        _vtextLeft = ReadSymbol("runtime.sym", "VTEXT_LEFT") ?? -1;
        _vtextTop = ReadSymbol("runtime.sym", "VTEXT_TOP") ?? -1;
        _vtextWidth = ReadSymbol("runtime.sym", "VTEXT_WIDTH") ?? -1;
        _vtextHeight = ReadSymbol("runtime.sym", "VTEXT_HEIGHT") ?? -1;
        _vtextCurX = ReadSymbol("runtime.sym", "VTEXT_CURX") ?? -1;
        _vtextCurY = ReadSymbol("runtime.sym", "VTEXT_CURY") ?? -1;
        _vtextChar = ReadSymbol("runtime.sym", "VTEXT_CHAR") ?? -1;
        _tracePictures = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_NZ6_PICS") == "1";
        if (_tracePictures)
            InitPictureTraceSymbols();

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
        else if (_tracePictures && pc == _drawPicturePc)
            LogPictureTrace("draw-entry", bus);
        else if (_tracePictures && pc == _pictureRectSetupPc)
            LogPictureTrace("draw-found", bus);
        else if (_tracePictures && pc == _pictureClipRectPc)
            LogPictureTrace("draw-rect", bus);
        else if (_tracePictures && pc == _pagerLoadPc)
            LogPagerTrace(bus);
        else if (_tracePictures && pc == _drawStartUnpackPc)
            LogPictureTrace("draw-start-unpack", bus);
        else if (_tracePictures && pc == _drawRts2Pc)
            LogPictureTrace("draw-rts2", bus);
        else if (pc == _scrollCompositePc)
            LogScrollTrace("scroll-hook", bus);
        else if (pc == _scrollRowsCompositePc)
            LogScrollTrace("scroll-rows", bus);
        else if (pc == _vtextBltFillPlanePc)
            LogVtextBlitTrace("vtext-fill", cpu, bus);
        else if (pc == _vtextBltCopyUpPlanePc)
            LogVtextBlitTrace("vtext-copy-up", cpu, bus);
        else if (pc == _vtextPutCharPc)
            LogVtextPutCharTrace(cpu, bus);
        else if (pc == _vtextPutRunPc)
            LogVtextPutRunTrace(cpu, bus);
        else if (pc == _vtextClearRegionPc)
            LogVtextRegionTrace("vtext-clear", bus);
        else if (pc == _vtextExposeGfxSpacesRegionPc)
            LogVtextRegionTrace("vtext-expose-spaces", bus);
        else if (pc == _vtextClearLinePc)
            LogVtextRegionTrace("vtext-clear-line", bus);
        else if (pc == _vtextScrollUpPc)
            LogVtextRegionTrace("vtext-scroll-up", bus);
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

    static void LogScrollTrace(string stage, IBusDevice bus)
    {
        if (_vtextLeft < 0 || _vtextTop < 0 || _vtextWidth < 0 || _vtextHeight < 0 ||
            _vtextCurX < 0 || _vtextCurY < 0)
            return;

        _writer!.WriteLine(
            $"{_seq++} ** {stage} vtext={bus.Read((ushort)_vtextLeft)},{bus.Read((ushort)_vtextTop)}+" +
            $"{bus.Read((ushort)_vtextCurX)},{bus.Read((ushort)_vtextCurY)} " +
            $"size={bus.Read((ushort)_vtextWidth)}x{bus.Read((ushort)_vtextHeight)}");
    }

    static void LogVtextBlitTrace(string stage, Cpu cpu, IBusDevice bus)
    {
        if (_vtextLeft < 0 || _vtextTop < 0 || _vtextWidth < 0 || _vtextHeight < 0 ||
            _vtextCurX < 0 || _vtextCurY < 0)
            return;

        CpuState state = cpu.GetState();
        _writer!.WriteLine(
            $"{_seq++} ** {stage} A={state.A:X2} X={state.X:X2} " +
            $"vtext={bus.Read((ushort)_vtextLeft)},{bus.Read((ushort)_vtextTop)}+" +
            $"{bus.Read((ushort)_vtextCurX)},{bus.Read((ushort)_vtextCurY)} " +
            $"size={bus.Read((ushort)_vtextWidth)}x{bus.Read((ushort)_vtextHeight)}");
    }

    static void LogVtextRegionTrace(string stage, IBusDevice bus)
    {
        if (_vtextLeft < 0 || _vtextTop < 0 || _vtextWidth < 0 || _vtextHeight < 0 ||
            _vtextCurX < 0 || _vtextCurY < 0)
            return;

        _writer!.WriteLine(
            $"{_seq++} ** {stage} vtext={bus.Read((ushort)_vtextLeft)},{bus.Read((ushort)_vtextTop)}+" +
            $"{bus.Read((ushort)_vtextCurX)},{bus.Read((ushort)_vtextCurY)} " +
            $"size={bus.Read((ushort)_vtextWidth)}x{bus.Read((ushort)_vtextHeight)}");
    }

    static void LogVtextPutCharTrace(Cpu cpu, IBusDevice bus)
    {
        if (_vtextChar < 0)
            return;
        CpuState state = cpu.GetState();
        int ch = bus.Read((ushort)_vtextChar);
        _writer!.WriteLine(
            $"{_seq++} ** vtext-put-char A={state.A:X2} char={FormatByte(ch)} " +
            $"vtext={bus.Read((ushort)_vtextLeft)},{bus.Read((ushort)_vtextTop)}+" +
            $"{bus.Read((ushort)_vtextCurX)},{bus.Read((ushort)_vtextCurY)} " +
            $"size={bus.Read((ushort)_vtextWidth)}x{bus.Read((ushort)_vtextHeight)}");
    }

    static void LogVtextPutRunTrace(Cpu cpu, IBusDevice bus)
    {
        CpuState state = cpu.GetState();
        int ptr = state.A | (state.Y << 8);
        int length = state.X;
        string sample = ReadAsciiSample(bus, ptr, length, 12);
        _writer!.WriteLine(
            $"{_seq++} ** vtext-put-run len={length} sample=\"{sample}\" " +
            $"vtext={bus.Read((ushort)_vtextLeft)},{bus.Read((ushort)_vtextTop)}+" +
            $"{bus.Read((ushort)_vtextCurX)},{bus.Read((ushort)_vtextCurY)} " +
            $"size={bus.Read((ushort)_vtextWidth)}x{bus.Read((ushort)_vtextHeight)}");
    }

    static string ReadAsciiSample(IBusDevice bus, int address, int length, int maxLength)
    {
        int count = Math.Min(length, maxLength);
        Span<char> chars = stackalloc char[count];
        for (int i = 0; i < count; i++)
        {
            int value = bus.Read((ushort)(address + i));
            chars[i] = value is >= 0x20 and <= 0x7e ? (char)value : '.';
        }
        return new string(chars).Replace("\\", "\\\\").Replace("\"", "\\\"");
    }

    static string FormatByte(int value)
    {
        char c = value is >= 0x20 and <= 0x7e ? (char)value : '.';
        return $"${value:X2}('{c}')";
    }

    static void InitPictureTraceSymbols()
    {
        _winCurrent = ReadSymbol("novaz6.sym", "nz6_win_current") ?? -1;
        _picW = ReadSymbol("novaz6.sym", "nz6_pic_w_lo") ?? -1;
        _picH = ReadSymbol("novaz6.sym", "nz6_pic_h_lo") ?? -1;
        _picFlags = ReadSymbol("novaz6.sym", "nz6_pic_flags") ?? -1;
        _picOff = ReadSymbol("novaz6.sym", "nz6_pic_off_l") ?? -1;
        _picLen = ReadSymbol("novaz6.sym", "nz6_pic_len_lo") ?? -1;
        _bltCellX = ReadSymbol("novaz6.sym", "nz6_blt_cellx") ?? -1;
        _bltCellY = ReadSymbol("novaz6.sym", "nz6_blt_celly") ?? -1;
        _bltPx = ReadSymbol("novaz6.sym", "nz6_blt_px_lo") ?? -1;
        _bltPy = ReadSymbol("novaz6.sym", "nz6_blt_py") ?? -1;
        _bltDst = ReadSymbol("novaz6.sym", "nz6_blt_dst_lo") ?? -1;
        _bltWBytes = ReadSymbol("novaz6.sym", "nz6_blt_wbytes") ?? -1;
        _bltWClip = ReadSymbol("novaz6.sym", "nz6_blt_wclip") ?? -1;
        _bltHClip = ReadSymbol("novaz6.sym", "nz6_blt_hclip") ?? -1;
    }

    static void LogPictureTrace(string stage, IBusDevice bus)
    {
        int count = bus.Read((ushort)_operandCount);
        int op0 = ReadOperand(bus, 0);
        int op1 = ReadOperand(bus, 1);
        int op2 = ReadOperand(bus, 2);
        int win = ReadByte(bus, _winCurrent);
        int picW = ReadWordLe(bus, _picW);
        int picH = ReadWordLe(bus, _picH);
        int flags = ReadByte(bus, _picFlags);
        int offset = Read24Le(bus, _picOff);
        int length = ReadWordLe(bus, _picLen);
        int px = ReadWordLe(bus, _bltPx);
        int py = ReadByte(bus, _bltPy);
        int cellX = ReadByte(bus, _bltCellX);
        int cellY = ReadByte(bus, _bltCellY);
        int dst = ReadWordLe(bus, _bltDst);
        int rowBytes = ReadByte(bus, _bltWBytes);
        int wClip = ReadWordLe(bus, _bltWClip);
        int hClip = ReadByte(bus, _bltHClip);
        _writer!.WriteLine(
            $"{_seq++} ## {stage} win={win:X2} count={count} ops={op0:X4},{op1:X4},{op2:X4} " +
            $"pic={picW}x{picH} flags={flags:X2} off={offset:X6} len={length:X4} " +
            $"px={px} py={py} cell={cellX},{cellY} dst={dst:X4} rowBytes={rowBytes} clip={wClip}x{hClip}");
    }

    static void LogPagerTrace(IBusDevice bus)
    {
        int nameLen = bus.Read(0x0028);
        int namePtr = bus.Read(0x0029) | (bus.Read(0x002A) << 8);
        int file = bus.Read(0x002B) | (bus.Read(0x002C) << 8) | (bus.Read(0x002D) << 16);
        int addr = bus.Read(0x0020) | (bus.Read(0x0021) << 8) | (bus.Read(0x0022) << 16);
        int len = bus.Read(0x0026) | (bus.Read(0x0027) << 8);
        int target = bus.Read(0x002E);
        int op0 = ReadOperand(bus, 0);
        int op1 = ReadOperand(bus, 1);
        int op2 = ReadOperand(bus, 2);
        _writer!.WriteLine(
            $"{_seq++} ## pager-enter ops={op0:X4},{op1:X4},{op2:X4} " +
            $"nameLen={nameLen} namePtr=${namePtr:X4} file=${file:X6} dest=${addr:X6} len=${len:X4} target=${target:X2}");
    }

    static int ReadByte(IBusDevice bus, int address) =>
        address >= 0 ? bus.Read((ushort)address) : 0;

    static int ReadWordLe(IBusDevice bus, int address) =>
        address >= 0 ? bus.Read((ushort)address) | (bus.Read((ushort)(address + 1)) << 8) : 0;

    static int Read24Le(IBusDevice bus, int address) =>
        address >= 0
            ? bus.Read((ushort)address) | (bus.Read((ushort)(address + 1)) << 8) | (bus.Read((ushort)(address + 2)) << 16)
            : 0;

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
