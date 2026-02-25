namespace e6502.Avalonia.Compiler;

// ── Result types ─────────────────────────────────────────────────────────────

public record SymbolInfo(string Name, NccType Type, ushort Address);

public record CompileResult(
    bool Success,
    byte[] Code,
    Dictionary<string, SymbolInfo> Symbols,
    List<string> Errors);

// ── CodeGen ──────────────────────────────────────────────────────────────────

/// <summary>
/// Walks a type-checked NCC AST and emits 6502 machine code.
///
/// Memory layout:
///   baseAddr+0   JMP entry_tramp           (3 bytes)
///   baseAddr+3   Global variable storage   (aligned to size)
///   ...          Entry trampoline: JSR main; BRK
///   ...          Function bodies (in declaration order)
///   ...          String literals (null-terminated)
///
/// Calling convention:
///   - Arguments pushed right-to-left onto the 6502 stack (byte args only).
///   - Byte return value in A.
///   - 16-bit return value in A (low byte) / X (high byte).
///   - Callee saves/restores frame pointer in ZP_FP/ZP_FP+1.
///   - Local variables live on the stack, accessed via frame pointer.
/// </summary>
public sealed class CodeGen
{
    // ── Zero-page temporaries ────────────────────────────────────────────────

    private const byte ZP_TMP0 = 0xF0;   // 2 bytes: general temp (low/high)
    private const byte ZP_TMP1 = 0xF2;   // 2 bytes: second temp
    private const byte ZP_TMP2 = 0xF4;   // 2 bytes: third temp
    private const byte ZP_PTR0 = 0xF6;   // 2 bytes: pointer temp
    private const byte ZP_FP   = 0xF8;   // 2 bytes: frame pointer (lo, hi)

    // ── State ────────────────────────────────────────────────────────────────

    private readonly CodeEmitter _emit;
    private readonly TypeChecker _checker;
    private readonly List<string> _errors = new();

    // Global variable name → absolute address
    private readonly Dictionary<string, ushort> _globals = new();

    // Function name → absolute address (filled during function emission)
    private readonly Dictionary<string, ushort> _functions = new();

    // Function name → list of JSR placeholder indices needing back-patching
    private readonly Dictionary<string, List<int>> _forwardCalls = new();

    // String literal → absolute address
    private readonly Dictionary<string, ushort> _strings = new();

    // String literal reference sites needing back-patching: (str, LDA_imm_idx, LDX_imm_idx)
    private readonly List<(string Str, int LdaIdx, int LdxIdx)> _stringRefs = new();

    // Symbol export table
    private readonly Dictionary<string, SymbolInfo> _symbols = new();

    // Current function's local variable layout: name → stack offset from FP
    // (negative offsets: locals are below the saved FP on the stack)
    private Dictionary<string, (int Offset, NccType Type)> _locals = new();
    private int _localSize;          // total bytes reserved for locals in current function
    private NccType? _returnType;    // return type of current function

    // Break/continue target stacks
    private readonly Stack<ushort> _breakTargets    = new();
    private readonly Stack<ushort> _continueTargets = new();
    // Pending break patches (buffer index of JMP placeholder, keyed by scope depth)
    private readonly Stack<List<int>> _breakPatches    = new();
    private readonly Stack<List<int>> _continuePatches = new();

    // ── Constructor ──────────────────────────────────────────────────────────

    public CodeGen(TypeChecker checker, ushort baseAddress = 0x0280)
    {
        _checker = checker;
        _emit    = new CodeEmitter(baseAddress);
    }

    // ── Entry point ──────────────────────────────────────────────────────────

    public CompileResult Generate(NccProgram program)
    {
        // --- Pass 1: allocate global variables ---
        // Layout:
        //   baseAddr+0  JMP <trampoline>     3 bytes placeholder
        //   baseAddr+3  globals              variable size
        int jmpPlaceholder = _emit.JMP_Placeholder();  // will patch after globals

        AllocateGlobals(program);

        // --- Trampoline: JSR main; BRK ---
        ushort trampolineAddr = _emit.CurrentAddress;
        _emit.Patch(jmpPlaceholder, trampolineAddr);

        // We don't know main's address yet; emit JSR with placeholder.
        _emit.Emit(0x20);
        int mainCallIdx = _emit.EmitPlaceholder();
        _emit.BRK();  // halt after main returns

        // --- Pass 2: emit function bodies ---
        foreach (var decl in program.Declarations)
        {
            if (decl is FuncDecl fd)
                EmitFunction(fd);
        }

        // --- Back-patch forward function calls ---
        foreach (var (name, patches) in _forwardCalls)
        {
            if (_functions.TryGetValue(name, out ushort addr))
            {
                foreach (int idx in patches)
                    _emit.Patch(idx, addr);
            }
            else
            {
                _errors.Add($"Undefined function '{name}'");
            }
        }

        // Back-patch main call
        if (_functions.TryGetValue("main", out ushort mainAddr))
            _emit.Patch(mainCallIdx, mainAddr);
        else
            _errors.Add("Program has no 'main' function");

        // --- Emit string literals ---
        foreach (var (str, _) in _strings.ToList())
            EmitStringLiteral(str);

        // Patch string literal address references now that addresses are known.
        foreach (var (str, ldaIdx, ldxIdx) in _stringRefs)
        {
            if (_strings.TryGetValue(str, out ushort strAddr))
            {
                _emit.PatchByte(ldaIdx, (byte)(strAddr & 0xFF));
                _emit.PatchByte(ldxIdx, (byte)((strAddr >> 8) & 0xFF));
            }
        }

        if (_errors.Count > 0)
            return new CompileResult(false, Array.Empty<byte>(), _symbols, _errors);

        return new CompileResult(true, _emit.ToArray(), _symbols, _errors);
    }

    // ── Global allocation ────────────────────────────────────────────────────

    private void AllocateGlobals(NccProgram program)
    {
        foreach (var decl in program.Declarations)
        {
            switch (decl)
            {
                case GlobalVarDecl gv:
                {
                    var type = ResolveType(gv.Type);
                    ushort addr = _emit.CurrentAddress;
                    _globals[gv.Name] = addr;
                    _symbols[gv.Name] = new SymbolInfo(gv.Name, type, addr);
                    // Reserve bytes (emit zeroes)
                    for (int i = 0; i < type.SizeInBytes; i++)
                        _emit.Emit(0x00);
                    break;
                }

                case GlobalArrayDecl ga:
                {
                    var elemType = ResolveType(ga.ElementType);
                    int size = EvalConstInt(ga.Size);
                    var arrType = new ArrayType(elemType, size);
                    ushort addr = _emit.CurrentAddress;
                    _globals[ga.Name] = addr;
                    _symbols[ga.Name] = new SymbolInfo(ga.Name, arrType, addr);
                    for (int i = 0; i < arrType.SizeInBytes; i++)
                        _emit.Emit(0x00);
                    break;
                }

                // Functions, structs, enums, directives: no global storage
            }
        }
    }

    // ── Function emission ────────────────────────────────────────────────────

    private void EmitFunction(FuncDecl fd)
    {
        ushort funcAddr = _emit.CurrentAddress;
        _functions[fd.Name] = funcAddr;

        // Build local variable layout for this function.
        // Parameters are on the stack above the saved frame pointer.
        // Locals are below the frame pointer.
        _locals    = new Dictionary<string, (int, NccType)>();
        _localSize = 0;
        _returnType = ResolveType(fd.ReturnType);
        _currentFuncIsInterrupt = fd.IsInterrupt;

        // First pass: collect local variable declarations to know how much
        // stack space to reserve in the prologue.
        int localOffset = 0;
        CollectLocals(fd.Body, ref localOffset);
        _localSize = localOffset;

        // Parameters are at positive offsets from FP (above the saved FP/PC).
        // Stack layout on entry to function body (after prologue):
        //   SP+0..  (empty, stack grows downward)
        //   SP+1..  locals[0]  (byte or 2 bytes)
        //   SP+n..  locals[n-1]
        //   FP+0    saved FP low
        //   FP+1    saved FP high
        //   FP+2    return address low  (pushed by JSR)
        //   FP+3    return address high
        //   FP+4    param[last] (rightmost arg pushed first = deepest)
        //   ...
        //
        // For simplicity we use absolute addresses for globals, and for
        // parameters we assign fixed global-like ZP locations (limited to
        // a small number of params).
        // A full frame-pointer local scheme is complex; we implement a
        // simplified version that allocates parameter storage as ZP temporaries.

        // Assign parameter "virtual" addresses in zero-page scratch area.
        // We reserve ZP 0xD0-0xEF for up to 16 byte params.
        // This limits parameter support but covers the tested cases.
        byte zpParamBase = 0xD0;
        var paramAddrs = new Dictionary<string, (byte ZP, NccType Type)>();
        byte zpNext = zpParamBase;
        // Parameters in NCC are pushed right-to-left; we just give each a ZP slot.
        foreach (var p in fd.Params)
        {
            var pt = ResolveType(p.Type);
            paramAddrs[p.Name] = (zpNext, pt);
            zpNext += (byte)pt.SizeInBytes;
        }

        // Emit prologue.
        EmitPrologue(fd, paramAddrs);

        // Emit body.
        EmitBlock(fd.Body);

        // Emit epilogue (reached if no explicit return).
        EmitEpilogue();
    }

    /// <summary>
    /// Scans a block recursively and assigns ZP offsets to all local VarDeclStmt.
    /// We use ZP 0xC0-0xCF for local variables (up to 16 bytes).
    /// </summary>
    private void CollectLocals(BlockStmt block, ref int offset)
    {
        foreach (var stmt in block.Statements)
            CollectLocalsStmt(stmt, ref offset);
    }

    private void CollectLocalsStmt(Stmt stmt, ref int offset)
    {
        switch (stmt)
        {
            case VarDeclStmt vd:
            {
                var type = ResolveType(vd.Type);
                byte zpAddr = (byte)(0xC0 + offset);
                _locals[vd.Name] = (zpAddr, type);
                offset += type.SizeInBytes;
                break;
            }
            case BlockStmt bs:
                CollectLocals(bs, ref offset);
                break;
            case IfStmt ifs:
                CollectLocalsStmt(ifs.Then, ref offset);
                if (ifs.Else is not null) CollectLocalsStmt(ifs.Else, ref offset);
                break;
            case WhileStmt ws:
                CollectLocalsStmt(ws.Body, ref offset);
                break;
            case DoWhileStmt dws:
                CollectLocalsStmt(dws.Body, ref offset);
                break;
            case ForStmt fs:
                if (fs.Init is not null) CollectLocalsStmt(fs.Init, ref offset);
                CollectLocalsStmt(fs.Body, ref offset);
                break;
        }
    }

    private void EmitPrologue(FuncDecl fd,
        Dictionary<string, (byte ZP, NccType Type)> paramAddrs)
    {
        // Interrupt handlers: save all registers before doing anything else.
        if (fd.IsInterrupt)
        {
            _emit.Emit(0x08);  // PHP
            _emit.PHA();
            _emit.TXA();
            _emit.PHA();
            _emit.TYA();
            _emit.PHA();
        }

        // If function has parameters, pull them off the stack into ZP slots.
        // The caller pushes args right-to-left (last arg pushed first),
        // so on the stack (from top = SP+1): param[0], param[1], ...
        // But the return address sits on top of the args (JSR pushed it last).
        // Stack layout after JSR:
        //   SP+1 = ret hi
        //   SP+2 = ret lo
        //   SP+3 = param[0]   (first arg = pushed last by caller)
        //   SP+4 = param[1]
        //   ...
        // We save return address, pull params, restore return address.
        if (fd.Params.Count > 0)
        {
            // Pull return address (high byte first since stack is big-endian hi,lo).
            _emit.PLA();
            _emit.STA_ZP(ZP_TMP0);      // save ret-hi
            _emit.PLA();
            _emit.STA_ZP((byte)(ZP_TMP0 + 1));  // save ret-lo

            // Pull parameters in order (first param = topmost after ret addr).
            foreach (var p in fd.Params)
            {
                var (zpAddr, ptype) = paramAddrs[p.Name];
                if (ptype.SizeInBytes == 1)
                {
                    _emit.PLA();
                    _emit.STA_ZP(zpAddr);
                }
                else // 2 bytes
                {
                    _emit.PLA();
                    _emit.STA_ZP(zpAddr);
                    _emit.PLA();
                    _emit.STA_ZP((byte)(zpAddr + 1));
                }
            }

            // Restore return address.
            _emit.LDA_ZP((byte)(ZP_TMP0 + 1));
            _emit.PHA();
            _emit.LDA_ZP(ZP_TMP0);
            _emit.PHA();
        }

        // Register param ZP addresses as locals so expression gen can find them.
        foreach (var p in fd.Params)
        {
            if (!_locals.ContainsKey(p.Name))
            {
                var type = ResolveType(p.Type);
                // paramAddrs[p.Name].ZP — we need to make params accessible.
                // Re-derive the ZP address.
                byte zpBase = 0xD0;
                byte zpNext = zpBase;
                foreach (var pp in fd.Params)
                {
                    var pt2 = ResolveType(pp.Type);
                    if (pp.Name == p.Name) break;
                    zpNext += (byte)pt2.SizeInBytes;
                }
                _locals[p.Name] = (zpNext, type);
            }
        }
    }

    private bool _currentFuncIsInterrupt;

    private void EmitEpilogue()
    {
        if (_currentFuncIsInterrupt)
        {
            _emit.PLA();
            _emit.TAY();
            _emit.PLA();
            _emit.TAX();
            _emit.PLA();
            _emit.Emit(0x28);  // PLP
            _emit.RTI();
        }
        else
        {
            _emit.RTS();
        }
    }

    // ── Statement emission ───────────────────────────────────────────────────

    private void EmitBlock(BlockStmt block)
    {
        foreach (var stmt in block.Statements)
            EmitStmt(stmt);
    }

    private void EmitStmt(Stmt stmt)
    {
        switch (stmt)
        {
            case BlockStmt bs:
                EmitBlock(bs);
                break;

            case VarDeclStmt vd:
                EmitVarDecl(vd);
                break;

            case ExprStmt es:
                EmitExpr(es.Expression);  // result discarded
                break;

            case IfStmt ifs:
                EmitIf(ifs);
                break;

            case WhileStmt ws:
                EmitWhile(ws);
                break;

            case DoWhileStmt dws:
                EmitDoWhile(dws);
                break;

            case ForStmt fs:
                EmitFor(fs);
                break;

            case ReturnStmt rs:
                EmitReturn(rs);
                break;

            case BreakStmt:
                EmitBreak();
                break;

            case ContinueStmt:
                EmitContinue();
                break;

            case AsmStmt:
                // TODO: inline assembly — skip for now
                break;

            case SwitchStmt sw:
                EmitSwitch(sw);
                break;

            case ArrayDeclStmt:
                // TODO: local array declarations
                break;
        }
    }

    private void EmitVarDecl(VarDeclStmt vd)
    {
        // _locals was pre-populated by CollectLocals.
        if (!_locals.TryGetValue(vd.Name, out var info))
        {
            _errors.Add($"Local '{vd.Name}' not pre-allocated");
            return;
        }

        if (vd.Initializer is not null)
        {
            EmitExpr(vd.Initializer);
            StoreToLocal(vd.Name, info.Type);
        }
    }

    private void EmitIf(IfStmt ifs)
    {
        EmitExprAsBool(ifs.Condition);
        // A == 0 means false
        _emit.CMP_Imm(0);
        int toElse = _emit.BEQ();

        EmitStmt(ifs.Then);

        if (ifs.Else is not null)
        {
            int toEnd = _emit.JMP_Placeholder();
            _emit.PatchBranch(toElse);
            EmitStmt(ifs.Else);
            _emit.Patch(toEnd, _emit.CurrentAddress);
        }
        else
        {
            _emit.PatchBranch(toElse);
        }
    }

    private void EmitWhile(WhileStmt ws)
    {
        ushort loopTop = _emit.CurrentAddress;
        _breakPatches.Push(new List<int>());
        _continuePatches.Push(new List<int>());

        EmitExprAsBool(ws.Condition);
        _emit.CMP_Imm(0);
        int toExit = _emit.BEQ();

        EmitStmt(ws.Body);

        // Continue target = back to loop top
        PatchContinues(loopTop);

        _emit.JMP(loopTop);
        ushort exitAddr = _emit.CurrentAddress;
        _emit.PatchBranch(toExit);

        PatchBreaks(exitAddr);
        _breakPatches.Pop();
        _continuePatches.Pop();
    }

    private void EmitDoWhile(DoWhileStmt dws)
    {
        ushort loopTop = _emit.CurrentAddress;
        _breakPatches.Push(new List<int>());
        _continuePatches.Push(new List<int>());

        EmitStmt(dws.Body);

        ushort condAddr = _emit.CurrentAddress;
        PatchContinues(condAddr);

        EmitExprAsBool(dws.Condition);
        _emit.CMP_Imm(0);
        int toExit = _emit.BEQ();
        _emit.JMP(loopTop);
        _emit.PatchBranch(toExit);

        ushort exitAddr = _emit.CurrentAddress;
        PatchBreaks(exitAddr);
        _breakPatches.Pop();
        _continuePatches.Pop();
    }

    private void EmitFor(ForStmt fs)
    {
        if (fs.Init is not null)
            EmitStmt(fs.Init);

        ushort loopTop = _emit.CurrentAddress;
        _breakPatches.Push(new List<int>());
        _continuePatches.Push(new List<int>());

        int toExit = -1;
        if (fs.Condition is not null)
        {
            EmitExprAsBool(fs.Condition);
            _emit.CMP_Imm(0);
            toExit = _emit.BEQ();
        }

        EmitStmt(fs.Body);

        ushort incrAddr = _emit.CurrentAddress;
        PatchContinues(incrAddr);

        if (fs.Increment is not null)
            EmitExpr(fs.Increment);

        _emit.JMP(loopTop);
        ushort exitAddr = _emit.CurrentAddress;

        if (toExit >= 0)
            _emit.PatchBranch(toExit);

        PatchBreaks(exitAddr);
        _breakPatches.Pop();
        _continuePatches.Pop();
    }

    private void EmitReturn(ReturnStmt rs)
    {
        if (rs.Value is not null)
        {
            EmitExpr(rs.Value);
            // A (and X for 16-bit) already contains the return value.
        }
        EmitEpilogue();
    }

    private void EmitBreak()
    {
        if (_breakPatches.Count == 0)
        {
            _errors.Add("break outside loop");
            return;
        }
        int idx = _emit.JMP_Placeholder();
        _breakPatches.Peek().Add(idx);
    }

    private void EmitContinue()
    {
        if (_continuePatches.Count == 0)
        {
            _errors.Add("continue outside loop");
            return;
        }
        int idx = _emit.JMP_Placeholder();
        _continuePatches.Peek().Add(idx);
    }

    private void PatchBreaks(ushort target)
    {
        foreach (int idx in _breakPatches.Peek())
            _emit.Patch(idx, target);
    }

    private void PatchContinues(ushort target)
    {
        foreach (int idx in _continuePatches.Peek())
            _emit.Patch(idx, target);
    }

    private void EmitSwitch(SwitchStmt sw)
    {
        _breakPatches.Push(new List<int>());

        EmitExpr(sw.Value);
        _emit.STA_ZP(ZP_TMP0);  // save switch value

        var caseJumps = new List<int>();  // JMP placeholder indices for each case body

        // Emit comparisons — each uses BNE to skip a JMP to the case body.
        // This avoids BEQ range limitations since BNE only skips 3 bytes.
        foreach (var c in sw.Cases)
        {
            if (c.Value is null) continue;  // default — handle at end
            EmitExpr(c.Value);
            _emit.CMP_ZP(ZP_TMP0);
            int skipBranch = _emit.BNE();       // skip over JMP if not equal
            int jmpToCase = _emit.JMP_Placeholder();  // JMP to case body (absolute, no range limit)
            _emit.PatchBranch(skipBranch);      // BNE lands here (next comparison)
            caseJumps.Add(jmpToCase);
        }

        // Jump past all cases if nothing matched.
        int pastAll = _emit.JMP_Placeholder();

        // Emit case bodies.
        int caseIdx = 0;
        ushort? defaultAddr = null;
        foreach (var c in sw.Cases)
        {
            if (c.Value is null)
            {
                defaultAddr = _emit.CurrentAddress;
            }
            else
            {
                // Patch the JMP for this case.
                _emit.Patch(caseJumps[caseIdx++], _emit.CurrentAddress);
            }
            foreach (var s in c.Body)
                EmitStmt(s);
        }

        ushort exitAddr = _emit.CurrentAddress;
        _emit.Patch(pastAll, defaultAddr ?? exitAddr);

        PatchBreaks(exitAddr);
        _breakPatches.Pop();
    }

    // ── Expression emission ──────────────────────────────────────────────────

    // Emits code that leaves the result in A (byte) or A/X (16-bit, A=lo X=hi).
    private void EmitExpr(Expr expr)
    {
        switch (expr)
        {
            case IntLiteralExpr il:
                EmitIntLiteral(il.Value);
                break;

            case HexLiteralExpr hl:
            {
                var s = hl.Value.StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                    ? hl.Value[2..] : hl.Value;
                long v = long.Parse(s, System.Globalization.NumberStyles.HexNumber);
                EmitIntLiteral(v);
                break;
            }

            case BinLiteralExpr bl:
            {
                var s = bl.Value.StartsWith("0b", StringComparison.OrdinalIgnoreCase)
                    ? bl.Value[2..] : bl.Value;
                long v = Convert.ToInt64(s, 2);
                EmitIntLiteral(v);
                break;
            }

            case BoolLiteralExpr bll:
                _emit.LDA_Imm(bll.Value ? (byte)1 : (byte)0);
                break;

            case CharLiteralExpr cl:
                _emit.LDA_Imm((byte)cl.Value);
                break;

            case StringLiteralExpr sl:
            {
                // Lazy-allocate; address will be back-patched after string data is emitted.
                if (!_strings.ContainsKey(sl.Value))
                    _strings[sl.Value] = 0;  // address TBD
                // Emit LDA #lo / LDX #hi with trackable placeholder indices.
                _emit.Emit(0xA9);           // LDA immediate opcode
                int ldaIdx = _emit.Length;  // index of the immediate byte
                _emit.Emit(0x00);           // placeholder lo
                _emit.Emit(0xA2);           // LDX immediate opcode
                int ldxIdx = _emit.Length;  // index of the immediate byte
                _emit.Emit(0x00);           // placeholder hi
                _stringRefs.Add((sl.Value, ldaIdx, ldxIdx));
                break;
            }

            case IdentifierExpr id:
                EmitLoadIdent(id.Name);
                break;

            case BinaryExpr be:
                EmitBinary(be);
                break;

            case UnaryExpr ue:
                EmitUnary(ue);
                break;

            case AssignExpr ae:
                EmitAssign(ae);
                break;

            case CallExpr ce:
                EmitCall(ce);
                break;

            case IndexExpr ix:
                EmitIndex(ix);
                break;

            case CastExpr cast:
                EmitExpr(cast.Operand);
                // TODO: actual type conversion for narrowing/widening
                break;

            default:
                _errors.Add($"Unsupported expression: {expr.GetType().Name}");
                _emit.LDA_Imm(0);
                break;
        }
    }

    private void EmitIntLiteral(long value)
    {
        if (value >= 0 && value <= 255)
        {
            _emit.LDA_Imm((byte)value);
            // X = 0 for 16-bit contexts
            _emit.LDX_Imm(0);
        }
        else
        {
            ushort v = (ushort)(value & 0xFFFF);
            _emit.LDA_Imm((byte)(v & 0xFF));
            _emit.LDX_Imm((byte)(v >> 8));
        }
    }

    // Loads a named variable into A (byte) or A/X (16-bit).
    private void EmitLoadIdent(string name)
    {
        if (_globals.TryGetValue(name, out ushort gaddr))
        {
            var sym = _symbols[name];
            if (sym.Type.SizeInBytes == 1)
            {
                _emit.LDA_Abs(gaddr);
            }
            else
            {
                _emit.LDA_Abs(gaddr);
                _emit.LDX_Abs((ushort)(gaddr + 1));
            }
        }
        else if (_locals.TryGetValue(name, out var local))
        {
            byte zpAddr = (byte)local.Offset;
            if (local.Type.SizeInBytes == 1)
            {
                _emit.LDA_ZP(zpAddr);
            }
            else
            {
                _emit.LDA_ZP(zpAddr);
                _emit.LDX_ZP((byte)(zpAddr + 1));
            }
        }
        else
        {
            _errors.Add($"Unknown variable '{name}'");
            _emit.LDA_Imm(0);
        }
    }

    // Stores A (byte) or A/X (16-bit) to a named global or local.
    private void EmitStoreIdent(string name, NccType type)
    {
        if (_globals.TryGetValue(name, out ushort gaddr))
        {
            if (type.SizeInBytes == 1)
            {
                _emit.STA_Abs(gaddr);
            }
            else
            {
                _emit.STA_Abs(gaddr);
                _emit.TXA();
                _emit.STA_Abs((ushort)(gaddr + 1));
                // Restore A to low byte.
                _emit.LDA_Abs(gaddr);
            }
        }
        else if (_locals.TryGetValue(name, out var local))
        {
            byte zpAddr = (byte)local.Offset;
            if (type.SizeInBytes == 1)
            {
                _emit.STA_ZP(zpAddr);
            }
            else
            {
                _emit.STA_ZP(zpAddr);
                _emit.TXA();
                _emit.STA_ZP((byte)(zpAddr + 1));
                _emit.LDA_ZP(zpAddr);
            }
        }
        else
        {
            _errors.Add($"Unknown variable '{name}' for store");
        }
    }

    private void StoreToLocal(string name, NccType type)
    {
        if (!_locals.TryGetValue(name, out var local))
        {
            _errors.Add($"Local '{name}' not found for store");
            return;
        }
        byte zpAddr = (byte)local.Offset;
        if (type.SizeInBytes == 1)
        {
            _emit.STA_ZP(zpAddr);
        }
        else
        {
            _emit.STA_ZP(zpAddr);
            _emit.TXA();
            _emit.STA_ZP((byte)(zpAddr + 1));
            _emit.LDA_ZP(zpAddr);
        }
    }

    // ── Binary expressions ───────────────────────────────────────────────────

    private void EmitBinary(BinaryExpr be)
    {
        bool wide = ExprSizeInBytes(be.Left) > 1 || ExprSizeInBytes(be.Right) > 1;

        switch (be.Op)
        {
            case TokenType.Plus:
                EmitAdd(be, wide);
                break;
            case TokenType.Minus:
                EmitSub(be, wide);
                break;
            case TokenType.Amp:
                EmitBitwise(be, BitwiseOp.And, wide);
                break;
            case TokenType.Pipe:
                EmitBitwise(be, BitwiseOp.Or, wide);
                break;
            case TokenType.Caret:
                EmitBitwise(be, BitwiseOp.Xor, wide);
                break;
            case TokenType.LShift:
                EmitShift(be, left: true);
                break;
            case TokenType.RShift:
                EmitShift(be, left: false);
                break;
            case TokenType.Eq:
                EmitCompare(be, CompareOp.Eq);
                break;
            case TokenType.NotEq:
                EmitCompare(be, CompareOp.Ne);
                break;
            case TokenType.Lt:
                EmitCompare(be, CompareOp.Lt);
                break;
            case TokenType.Gt:
                EmitCompare(be, CompareOp.Gt);
                break;
            case TokenType.LtEq:
                EmitCompare(be, CompareOp.Le);
                break;
            case TokenType.GtEq:
                EmitCompare(be, CompareOp.Ge);
                break;
            case TokenType.AmpAmp:
                EmitLogical(be, and: true);
                break;
            case TokenType.PipePipe:
                EmitLogical(be, and: false);
                break;
            default:
                _errors.Add($"Unsupported binary op: {be.Op}");
                _emit.LDA_Imm(0);
                break;
        }
    }

    private void EmitAdd(BinaryExpr be, bool wide)
    {
        if (!wide)
        {
            // Byte add: left → A, push; right → A; CLC; ADC from stack.
            EmitExpr(be.Left);
            _emit.PHA();
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP0);
            _emit.PLA();
            _emit.CLC();
            _emit.ADC_ZP(ZP_TMP0);
        }
        else
        {
            // 16-bit add: left → A/X, save; right → A/X, add.
            EmitExpr(be.Left);
            _emit.STA_ZP(ZP_TMP0);
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_TMP0 + 1));
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP1);
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_TMP1 + 1));
            _emit.CLC();
            _emit.LDA_ZP(ZP_TMP0);
            _emit.ADC_ZP(ZP_TMP1);
            _emit.STA_ZP(ZP_TMP2);
            _emit.LDA_ZP((byte)(ZP_TMP0 + 1));
            _emit.ADC_ZP((byte)(ZP_TMP1 + 1));
            _emit.TAX();
            _emit.LDA_ZP(ZP_TMP2);
        }
    }

    private void EmitSub(BinaryExpr be, bool wide)
    {
        if (!wide)
        {
            EmitExpr(be.Left);
            _emit.PHA();
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP0);
            _emit.PLA();
            _emit.SEC();
            _emit.SBC_ZP(ZP_TMP0);
        }
        else
        {
            EmitExpr(be.Left);
            _emit.STA_ZP(ZP_TMP0);
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_TMP0 + 1));
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP1);
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_TMP1 + 1));
            _emit.SEC();
            _emit.LDA_ZP(ZP_TMP0);
            _emit.SBC_ZP(ZP_TMP1);
            _emit.STA_ZP(ZP_TMP2);
            _emit.LDA_ZP((byte)(ZP_TMP0 + 1));
            _emit.SBC_ZP((byte)(ZP_TMP1 + 1));
            _emit.TAX();
            _emit.LDA_ZP(ZP_TMP2);
        }
    }

    private enum BitwiseOp { And, Or, Xor }

    private void EmitBitwise(BinaryExpr be, BitwiseOp op, bool wide)
    {
        EmitExpr(be.Left);
        _emit.PHA();
        if (wide) _emit.PHX();

        EmitExpr(be.Right);
        _emit.STA_ZP(ZP_TMP0);
        if (wide)
        {
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_TMP0 + 1));
            _emit.PLX();
        }
        _emit.PLA();

        switch (op)
        {
            case BitwiseOp.And:
                _emit.AND_ZP(ZP_TMP0);
                if (wide) { _emit.PHA(); _emit.TXA(); _emit.AND_ZP((byte)(ZP_TMP0 + 1)); _emit.TAX(); _emit.PLA(); }
                break;
            case BitwiseOp.Or:
                _emit.ORA_ZP(ZP_TMP0);
                if (wide) { _emit.PHA(); _emit.TXA(); _emit.ORA_ZP((byte)(ZP_TMP0 + 1)); _emit.TAX(); _emit.PLA(); }
                break;
            case BitwiseOp.Xor:
                _emit.EOR_ZP(ZP_TMP0);
                if (wide) { _emit.PHA(); _emit.TXA(); _emit.EOR_ZP((byte)(ZP_TMP0 + 1)); _emit.TAX(); _emit.PLA(); }
                break;
        }
    }

    private void EmitShift(BinaryExpr be, bool left)
    {
        // Only constant shift counts supported for now.
        EmitExpr(be.Left);
        int count = 1;
        if (be.Right is IntLiteralExpr il) count = (int)il.Value;
        for (int i = 0; i < count; i++)
        {
            if (left) _emit.ASL_A(); else _emit.LSR_A();
        }
    }

    private enum CompareOp { Eq, Ne, Lt, Gt, Le, Ge }

    private void EmitCompare(BinaryExpr be, CompareOp op)
    {
        bool wide = ExprSizeInBytes(be.Left) > 1 || ExprSizeInBytes(be.Right) > 1;

        if (!wide)
        {
            EmitExpr(be.Left);
            _emit.PHA();
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP0);
            _emit.PLA();
            _emit.CMP_ZP(ZP_TMP0);
        }
        else
        {
            // 16-bit: compare high bytes first, then low bytes if equal.
            EmitExpr(be.Left);
            _emit.STA_ZP(ZP_TMP0);
            _emit.STX_ZP((byte)(ZP_TMP0 + 1));
            EmitExpr(be.Right);
            _emit.STA_ZP(ZP_TMP1);
            _emit.STX_ZP((byte)(ZP_TMP1 + 1));
            // Compare high bytes first.
            _emit.LDA_ZP((byte)(ZP_TMP0 + 1));
            _emit.CMP_ZP((byte)(ZP_TMP1 + 1));
            int highDone = _emit.BNE();  // if high bytes differ, flags already set
            // High bytes equal — compare low bytes.
            _emit.LDA_ZP(ZP_TMP0);
            _emit.CMP_ZP(ZP_TMP1);
            _emit.PatchBranch(highDone);  // BNE lands here; flags from whichever CMP ran last drive result
        }

        // After CMP: Z=1 if equal, C=1 if A>=operand, N=sign
        switch (op)
        {
            case CompareOp.Eq:
                // Z set → equal
                { int notEq = _emit.BNE(); _emit.LDA_Imm(1); int skip = _emit.JMP_Placeholder(); _emit.PatchBranch(notEq); _emit.LDA_Imm(0); _emit.Patch(skip, _emit.CurrentAddress); }
                break;
            case CompareOp.Ne:
                { int isEq = _emit.BEQ(); _emit.LDA_Imm(1); int skip = _emit.JMP_Placeholder(); _emit.PatchBranch(isEq); _emit.LDA_Imm(0); _emit.Patch(skip, _emit.CurrentAddress); }
                break;
            case CompareOp.Lt:
                // C clear → left < right (unsigned)
                { int notLt = _emit.BCS(); _emit.LDA_Imm(1); int skip = _emit.JMP_Placeholder(); _emit.PatchBranch(notLt); _emit.LDA_Imm(0); _emit.Patch(skip, _emit.CurrentAddress); }
                break;
            case CompareOp.Ge:
                // C set → left >= right
                { int notGe = _emit.BCC(); _emit.LDA_Imm(1); int skip = _emit.JMP_Placeholder(); _emit.PatchBranch(notGe); _emit.LDA_Imm(0); _emit.Patch(skip, _emit.CurrentAddress); }
                break;
            case CompareOp.Gt:
                // left > right: right < left → swap and check C clear
                // Re-compare with operands reversed.
                {
                    // We already compared left-right. For Gt: left > right ⟺ !(left <= right) ⟺ !(C=1 || Z=1)
                    // Use: BEQ notGt; BCS isGt; LDA #0; skip; isGt: LDA #1; skip:
                    int isEq2  = _emit.BEQ();
                    int isGt   = _emit.BCS();
                    _emit.LDA_Imm(0);
                    int skip2  = _emit.JMP_Placeholder();
                    _emit.PatchBranch(isEq2);
                    _emit.LDA_Imm(0);
                    int skip3  = _emit.JMP_Placeholder();
                    _emit.PatchBranch(isGt);
                    _emit.LDA_Imm(1);
                    ushort end = _emit.CurrentAddress;
                    _emit.Patch(skip2, end);
                    _emit.Patch(skip3, end);
                }
                break;
            case CompareOp.Le:
                // left <= right: C=1 (left >= right) but if Z=1 also equal.
                // left <= right ⟺ !(left > right) ⟺ C=0 || Z=1
                {
                    int isEq3 = _emit.BEQ();
                    int notLe = _emit.BCS();
                    _emit.LDA_Imm(1);
                    int skip4 = _emit.JMP_Placeholder();
                    _emit.PatchBranch(isEq3);
                    _emit.LDA_Imm(1);
                    int skip5 = _emit.JMP_Placeholder();
                    _emit.PatchBranch(notLe);
                    _emit.LDA_Imm(0);
                    ushort end2 = _emit.CurrentAddress;
                    _emit.Patch(skip4, end2);
                    _emit.Patch(skip5, end2);
                }
                break;
        }
    }

    private void EmitLogical(BinaryExpr be, bool and)
    {
        // Short-circuit evaluation.
        EmitExprAsBool(be.Left);
        _emit.CMP_Imm(0);

        if (and)
        {
            // If left is false, result is false.
            int toFalse = _emit.BEQ();
            EmitExprAsBool(be.Right);
            _emit.CMP_Imm(0);
            int toFalse2 = _emit.BEQ();
            _emit.LDA_Imm(1);
            int toEnd = _emit.JMP_Placeholder();
            _emit.PatchBranch(toFalse);
            _emit.PatchBranch(toFalse2);
            _emit.LDA_Imm(0);
            _emit.Patch(toEnd, _emit.CurrentAddress);
        }
        else
        {
            // If left is true, result is true.
            int toTrue = _emit.BNE();
            EmitExprAsBool(be.Right);
            _emit.CMP_Imm(0);
            int toFalse = _emit.BEQ();
            _emit.PatchBranch(toTrue);
            _emit.LDA_Imm(1);
            int toEnd = _emit.JMP_Placeholder();
            _emit.PatchBranch(toFalse);
            _emit.LDA_Imm(0);
            _emit.Patch(toEnd, _emit.CurrentAddress);
        }
    }

    // ── Unary expressions ────────────────────────────────────────────────────

    private void EmitUnary(UnaryExpr ue)
    {
        switch (ue.Op)
        {
            case TokenType.Minus:
                // Two's complement: EOR #$FF; CLC; ADC #1
                EmitExpr(ue.Operand);
                _emit.EOR_Imm(0xFF);
                _emit.CLC();
                _emit.ADC_Imm(1);
                break;

            case TokenType.Tilde:
                EmitExpr(ue.Operand);
                _emit.EOR_Imm(0xFF);
                break;

            case TokenType.Bang:
                // Logical NOT: 0→1, nonzero→0
                EmitExpr(ue.Operand);
                _emit.CMP_Imm(0);
                int notZero = _emit.BNE();
                _emit.LDA_Imm(1);
                int skip = _emit.JMP_Placeholder();
                _emit.PatchBranch(notZero);
                _emit.LDA_Imm(0);
                _emit.Patch(skip, _emit.CurrentAddress);
                break;

            case TokenType.PlusPlus when ue.Prefix:
                EmitIncDec(ue.Operand, inc: true, post: false);
                break;
            case TokenType.MinusMinus when ue.Prefix:
                EmitIncDec(ue.Operand, inc: false, post: false);
                break;
            case TokenType.PlusPlus:
                EmitIncDec(ue.Operand, inc: true, post: true);
                break;
            case TokenType.MinusMinus:
                EmitIncDec(ue.Operand, inc: false, post: true);
                break;

            case TokenType.Star:
                // Pointer dereference: load pointer, then indirect load.
                EmitExpr(ue.Operand);
                // A=lo, X=hi → store to ZP_PTR0 and use LDA (ZP_PTR0),Y
                _emit.STA_ZP(ZP_PTR0);
                _emit.TXA();
                _emit.STA_ZP((byte)(ZP_PTR0 + 1));
                _emit.LDY_Imm(0);
                _emit.LDA_IndY(ZP_PTR0);
                break;

            case TokenType.Amp:
                // Address-of: only supported for globals.
                if (ue.Operand is IdentifierExpr idExpr &&
                    _globals.TryGetValue(idExpr.Name, out ushort gaddr))
                {
                    _emit.LDA_Imm((byte)(gaddr & 0xFF));
                    _emit.LDX_Imm((byte)(gaddr >> 8));
                }
                else
                {
                    _errors.Add("Address-of only supported for globals");
                    _emit.LDA_Imm(0);
                    _emit.LDX_Imm(0);
                }
                break;

            default:
                EmitExpr(ue.Operand);
                break;
        }
    }

    private void EmitIncDec(Expr target, bool inc, bool post)
    {
        // Load current value.
        EmitExpr(target);

        if (post)
        {
            // Save original value to return.
            _emit.PHA();
        }

        if (inc)
        {
            _emit.CLC();
            _emit.ADC_Imm(1);
        }
        else
        {
            _emit.SEC();
            _emit.SBC_Imm(1);
        }

        // Store back.
        if (target is IdentifierExpr id)
        {
            var type = GetIdentType(id.Name);
            EmitStoreIdent(id.Name, type);
        }

        if (post)
        {
            _emit.PLA();  // restore original value as expression result
        }
    }

    // ── Assignment ───────────────────────────────────────────────────────────

    private void EmitAssign(AssignExpr ae)
    {
        // Compound assignments: expand to binary op.
        if (ae.Op != TokenType.Assign)
        {
            TokenType binOp = ae.Op switch
            {
                TokenType.PlusAssign  => TokenType.Plus,
                TokenType.MinusAssign => TokenType.Minus,
                TokenType.StarAssign  => TokenType.Star,
                TokenType.AmpAssign   => TokenType.Amp,
                TokenType.PipeAssign  => TokenType.Pipe,
                TokenType.CaretAssign => TokenType.Caret,
                _                     => TokenType.Plus,
            };
            // Synthesise: target = target op value
            var synth = new BinaryExpr(ae.Target, binOp, ae.Value, ae.Line, ae.Col);
            EmitBinary(synth);
        }
        else
        {
            EmitExpr(ae.Value);
        }

        // Store result.
        if (ae.Target is IdentifierExpr id)
        {
            var type = GetIdentType(id.Name);
            EmitStoreIdent(id.Name, type);
        }
        else if (ae.Target is IndexExpr ix)
        {
            EmitIndexStore(ix);
        }
        else if (ae.Target is UnaryExpr { Op: TokenType.Star } deref)
        {
            // Store through pointer.
            _emit.STA_ZP(ZP_TMP2);   // save value
            EmitExpr(deref.Operand);
            _emit.STA_ZP(ZP_PTR0);
            _emit.TXA();
            _emit.STA_ZP((byte)(ZP_PTR0 + 1));
            _emit.LDY_Imm(0);
            _emit.LDA_ZP(ZP_TMP2);
            _emit.STA_IndY(ZP_PTR0);
        }
        else
        {
            _errors.Add($"Unsupported assignment target: {ae.Target.GetType().Name}");
        }
    }

    // ── Call ─────────────────────────────────────────────────────────────────

    private void EmitCall(CallExpr ce)
    {
        if (ce.Callee is not IdentifierExpr fnId)
        {
            _errors.Add("Indirect function calls not supported");
            _emit.LDA_Imm(0);
            return;
        }

        // Push arguments right-to-left (last arg first, deepest on stack).
        for (int i = ce.Args.Count - 1; i >= 0; i--)
        {
            var argType = _checker.ResolveExprType(ce.Args[i]) ?? NccType.Byte;
            EmitExpr(ce.Args[i]);
            if (argType.SizeInBytes == 1)
            {
                _emit.PHA();
            }
            else
            {
                // Push high byte first (it's deeper on the stack).
                _emit.PHX();
                _emit.PHA();
            }
        }

        // Emit JSR.
        if (_functions.TryGetValue(fnId.Name, out ushort addr))
        {
            _emit.JSR(addr);
        }
        else
        {
            // Forward reference — emit placeholder.
            _emit.Emit(0x20);
            int idx = _emit.EmitPlaceholder();
            if (!_forwardCalls.ContainsKey(fnId.Name))
                _forwardCalls[fnId.Name] = new List<int>();
            _forwardCalls[fnId.Name].Add(idx);
        }
    }

    // ── Array indexing ───────────────────────────────────────────────────────

    private void EmitIndex(IndexExpr ix)
    {
        if (ix.Array is not IdentifierExpr arrId)
        {
            _errors.Add("Complex array expressions not supported");
            _emit.LDA_Imm(0);
            return;
        }

        ushort baseAddr = 0;
        NccType elemType = NccType.Byte;

        if (_globals.TryGetValue(arrId.Name, out ushort gaddr))
        {
            baseAddr = gaddr;
            if (_symbols.TryGetValue(arrId.Name, out var sym) && sym.Type is ArrayType at)
                elemType = at.ElementType;
        }
        else
        {
            _errors.Add($"Array '{arrId.Name}' not found");
            _emit.LDA_Imm(0);
            return;
        }

        // Load index into X.
        EmitExpr(ix.Index);
        _emit.TAX();

        if (elemType.SizeInBytes == 1)
        {
            _emit.LDA_AbsX(baseAddr);
        }
        else
        {
            // 16-bit elements: index * 2
            _emit.ASL_A();
            _emit.TAX();
            _emit.LDA_AbsX(baseAddr);
            _emit.STA_ZP(ZP_TMP0);
            _emit.INX();
            _emit.LDA_AbsX(baseAddr);
            _emit.TAX();
            _emit.LDA_ZP(ZP_TMP0);
        }
    }

    private void EmitIndexStore(IndexExpr ix)
    {
        // Value is in A. Save it, compute address, store.
        _emit.STA_ZP(ZP_TMP2);

        if (ix.Array is not IdentifierExpr arrId ||
            !_globals.TryGetValue(arrId.Name, out ushort baseAddr))
        {
            _errors.Add("Complex array store not supported");
            return;
        }

        NccType elemType = NccType.Byte;
        if (_symbols.TryGetValue(arrId.Name, out var sym) && sym.Type is ArrayType at)
            elemType = at.ElementType;

        EmitExpr(ix.Index);
        _emit.TAX();
        _emit.LDA_ZP(ZP_TMP2);
        _emit.STA_AbsX(baseAddr);
    }

    // ── Boolean coercion ─────────────────────────────────────────────────────

    // Emits code leaving 0 or 1 in A.
    private void EmitExprAsBool(Expr expr)
    {
        EmitExpr(expr);
        // If already a comparison result (0 or 1), it's fine.
        // If it's a numeric value, treat nonzero as true.
        // We ensure A is 0 or 1 by:
        //   BEQ +3; LDA #1; done: ...  → but we want 0=false, nonzero=true
        // Actually the if/while just does CMP #0 after us, so leave as-is.
    }

    // ── String literal emission ───────────────────────────────────────────────

    private void EmitStringLiteral(string str)
    {
        ushort addr = _emit.CurrentAddress;
        _strings[str] = addr;
        foreach (char c in str)
            _emit.Emit((byte)c);
        _emit.Emit(0);  // null terminator
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    private NccType GetIdentType(string name)
    {
        if (_symbols.TryGetValue(name, out var sym))
            return sym.Type;
        if (_locals.TryGetValue(name, out var local))
            return local.Type;
        return NccType.Byte;
    }

    /// <summary>
    /// Determines the size in bytes of an expression using CodeGen's own
    /// symbol tables. This is used instead of TypeChecker.ResolveExprType
    /// because the TypeChecker's scopes are empty after type-checking.
    /// </summary>
    private int ExprSizeInBytes(Expr expr) => expr switch
    {
        IdentifierExpr id          => GetIdentType(id.Name).SizeInBytes,
        IntLiteralExpr il          => il.Value is >= 0 and <= 255 ? 1 : 2,
        HexLiteralExpr hl          => EvalHex(hl.Value) <= 0xFF ? 1 : 2,
        BinLiteralExpr bl          => EvalBin(bl.Value) <= 0xFF ? 1 : 2,
        CastExpr cast              => ResolveType(cast.TargetType).SizeInBytes,
        BinaryExpr be              => Math.Max(ExprSizeInBytes(be.Left), ExprSizeInBytes(be.Right)),
        AssignExpr ae              => ExprSizeInBytes(ae.Target),
        CallExpr ce when ce.Callee is IdentifierExpr fn
                                   => (_functions.ContainsKey(fn.Name) ? 1 : 1),  // default byte return
        _                          => 1,
    };

    private NccType ResolveType(TypeRef tr)
    {
        var t = NccType.ResolveType(tr.Name);
        if (t is null) return NccType.Byte;
        return tr.IsPointer ? new PointerType(t) : t;
    }

    private int EvalConstInt(Expr expr) => expr switch
    {
        IntLiteralExpr il => (int)il.Value,
        HexLiteralExpr hl => EvalHex(hl.Value),
        BinLiteralExpr bl => EvalBin(bl.Value),
        _ => 0
    };

    private static int EvalHex(string v)
    {
        var s = v.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? v[2..] : v;
        return int.TryParse(s, System.Globalization.NumberStyles.HexNumber, null, out int r) ? r : 0;
    }

    private static int EvalBin(string v)
    {
        var s = v.StartsWith("0b", StringComparison.OrdinalIgnoreCase) ? v[2..] : v;
        try { return Convert.ToInt32(s, 2); } catch { return 0; }
    }
}
