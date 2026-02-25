namespace e6502.Avalonia.Compiler;

// ── Symbol table ──────────────────────────────────────────────────────────────

public enum SymbolKind { Variable, Parameter, Function, Struct, Enum, EnumValue, Const }

public record Symbol(string Name, NccType Type, SymbolKind Kind);

// ── TypeChecker ───────────────────────────────────────────────────────────────

public class TypeChecker
{
    private readonly List<string> _errors   = new();
    private readonly List<string> _warnings = new();

    // Scope stack: innermost scope is last.
    private readonly List<Dictionary<string, Symbol>> _scopes = new();

    // Struct and enum type registry (populated in pass 1).
    private readonly Dictionary<string, StructType> _structs = new();
    private readonly Dictionary<string, EnumType>   _enums   = new();

    // Return type of the function currently being checked (null at global scope).
    private NccType? _currentReturn;

    public IReadOnlyList<string> Errors   => _errors;
    public IReadOnlyList<string> Warnings => _warnings;

    // ── Entry point ──────────────────────────────────────────────────────────

    public void Check(NccProgram program)
    {
        PushScope();                   // global scope

        // Pass 1 — register all top-level names so forward references work.
        foreach (var decl in program.Declarations)
            RegisterTopLevel(decl);

        // Pass 2 — type-check bodies and initializers.
        foreach (var decl in program.Declarations)
            CheckDecl(decl);

        PopScope();
    }

    // ── Pass 1 ───────────────────────────────────────────────────────────────

    private void RegisterTopLevel(Decl decl)
    {
        switch (decl)
        {
            case StructDecl sd:
                RegisterStruct(sd);
                break;

            case EnumDecl ed:
                RegisterEnum(ed);
                break;

            case FuncDecl fd:
            {
                var paramTypes = fd.Params.Select(p => ResolveTypeRef(p.Type, p.Line)).ToList();
                var retType    = ResolveTypeRef(fd.ReturnType, fd.Line);
                var funcType   = new FuncType(retType, paramTypes);
                DeclareSymbol(fd.Name, funcType, SymbolKind.Function, fd.Line);
                break;
            }

            case GlobalVarDecl gv:
            {
                var t = ResolveTypeRef(gv.Type, gv.Line);
                var kind = gv.IsConst ? SymbolKind.Const : SymbolKind.Variable;
                DeclareSymbol(gv.Name, t, kind, gv.Line);
                break;
            }

            case GlobalArrayDecl ga:
            {
                // Size must be a compile-time constant integer; use 0 as fallback.
                int sz = EvalConstIntExpr(ga.Size);
                var elemType = ResolveTypeRef(ga.ElementType, ga.Line);
                var arrType  = new ArrayType(elemType, sz);
                var kind = ga.IsConst ? SymbolKind.Const : SymbolKind.Variable;
                DeclareSymbol(ga.Name, arrType, kind, ga.Line);
                break;
            }

            // Directives — nothing to register.
        }
    }

    private void RegisterStruct(StructDecl sd)
    {
        if (_structs.ContainsKey(sd.Name))
        {
            Error(sd.Line, $"duplicate struct '{sd.Name}'");
            return;
        }

        int offset = 0;
        var fields = new List<StructField>();
        foreach (var f in sd.Fields)
        {
            var ft = ResolveTypeRef(f.Type, f.Line);
            if (f.ArraySize is not null)
            {
                int sz = EvalConstIntExpr(f.ArraySize);
                ft = new ArrayType(ft, sz);
            }
            fields.Add(new StructField(f.Name, ft, offset));
            offset += ft.SizeInBytes;
        }

        var st = new StructType(sd.Name, fields);
        _structs[sd.Name] = st;
        DeclareSymbol(sd.Name, st, SymbolKind.Struct, sd.Line);
    }

    private void RegisterEnum(EnumDecl ed)
    {
        if (_enums.ContainsKey(ed.Name))
        {
            Error(ed.Line, $"duplicate enum '{ed.Name}'");
            return;
        }

        // Build enum members with auto-incrementing values.
        var members = new List<EnumMember>();
        int nextVal = 0;
        foreach (var ev in ed.Values)
        {
            if (ev.Value is not null)
                nextVal = (int)EvalConstIntExpr(ev.Value);
            members.Add(new EnumMember(ev.Name, nextVal));
            nextVal++;
        }

        var et = new EnumType(ed.Name, members);
        _enums[ed.Name] = et;
        DeclareSymbol(ed.Name, et, SymbolKind.Enum, ed.Line);

        // Register each enum value as a constant of the enum's type.
        foreach (var ev in ed.Values)
            DeclareSymbol(ev.Name, et, SymbolKind.EnumValue, ev.Line);
    }

    // ── Pass 2 — declaration checking ────────────────────────────────────────

    private void CheckDecl(Decl decl)
    {
        switch (decl)
        {
            case FuncDecl fd:
                CheckFunc(fd);
                break;

            case GlobalVarDecl gv when gv.Initializer is not null:
            {
                var varType  = ResolveTypeRef(gv.Type, gv.Line);
                var initType = ResolveExprType(gv.Initializer);
                if (initType is not null)
                    CheckAssignable(varType, initType, gv.Line);
                break;
            }

            case GlobalArrayDecl ga when ga.Initializer is not null:
            {
                var elemType = ResolveTypeRef(ga.ElementType, ga.Line);
                foreach (var init in ga.Initializer)
                {
                    var t = ResolveExprType(init);
                    if (t is not null)
                        CheckAssignable(elemType, t, ga.Line);
                }
                break;
            }

            // Structs, enums, directives — nothing to check in pass 2.
        }
    }

    private void CheckFunc(FuncDecl fd)
    {
        var savedReturn = _currentReturn;
        _currentReturn  = ResolveTypeRef(fd.ReturnType, fd.Line);

        PushScope();

        foreach (var p in fd.Params)
        {
            var pt = ResolveTypeRef(p.Type, p.Line);
            DeclareSymbol(p.Name, pt, SymbolKind.Parameter, p.Line);
        }

        CheckBlock(fd.Body);

        PopScope();
        _currentReturn = savedReturn;
    }

    // ── Statement checking ────────────────────────────────────────────────────

    private void CheckStmt(Stmt stmt)
    {
        switch (stmt)
        {
            case BlockStmt bs:
                PushScope();
                CheckBlock(bs);
                PopScope();
                break;

            case VarDeclStmt vd:
            {
                var t = ResolveTypeRef(vd.Type, vd.Line);
                if (vd.Initializer is not null)
                {
                    var it = ResolveExprType(vd.Initializer);
                    if (it is not null)
                        CheckAssignable(t, it, vd.Line);
                }
                DeclareSymbol(vd.Name, t, SymbolKind.Variable, vd.Line);
                break;
            }

            case ArrayDeclStmt ad:
            {
                var elemType = ResolveTypeRef(ad.ElementType, ad.Line);
                var sizeType = ResolveExprType(ad.Size);
                if (sizeType is not null && !sizeType.IsIntegral)
                    Error(ad.Line, $"array size must be integral, got '{sizeType.Name}'");

                int sz = EvalConstIntExpr(ad.Size);
                var arrType = new ArrayType(elemType, sz);
                DeclareSymbol(ad.Name, arrType, SymbolKind.Variable, ad.Line);

                if (ad.Initializer is not null)
                {
                    var it = ResolveExprType(ad.Initializer);
                    if (it is not null)
                        CheckAssignable(elemType, it, ad.Line);
                }
                break;
            }

            case ExprStmt es:
                ResolveExprType(es.Expression);
                break;

            case IfStmt ifs:
            {
                var ct = ResolveExprType(ifs.Condition);
                if (ct is not null)
                    CheckCondition(ct, ifs.Line);
                CheckStmt(ifs.Then);
                if (ifs.Else is not null)
                    CheckStmt(ifs.Else);
                break;
            }

            case WhileStmt ws:
            {
                var ct = ResolveExprType(ws.Condition);
                if (ct is not null)
                    CheckCondition(ct, ws.Line);
                CheckStmt(ws.Body);
                break;
            }

            case DoWhileStmt dws:
            {
                CheckStmt(dws.Body);
                var ct = ResolveExprType(dws.Condition);
                if (ct is not null)
                    CheckCondition(ct, dws.Line);
                break;
            }

            case ForStmt fs:
            {
                // For-init may introduce a local variable, so push a scope.
                PushScope();
                if (fs.Init is not null)
                    CheckStmt(fs.Init);
                if (fs.Condition is not null)
                {
                    var ct = ResolveExprType(fs.Condition);
                    if (ct is not null)
                        CheckCondition(ct, fs.Line);
                }
                if (fs.Increment is not null)
                    ResolveExprType(fs.Increment);
                CheckStmt(fs.Body);
                PopScope();
                break;
            }

            case SwitchStmt sw:
            {
                var vt = ResolveExprType(sw.Value);
                if (vt is not null && !vt.IsIntegral && vt is not EnumType)
                    Error(sw.Line, $"switch value must be integral or enum, got '{vt.Name}'");

                foreach (var c in sw.Cases)
                {
                    if (c.Value is not null)
                    {
                        var cv = ResolveExprType(c.Value);
                        if (cv is not null && vt is not null && !TypesCompatible(vt, cv))
                            Error(c.Line, $"case value type '{cv.Name}' incompatible with switch type '{vt.Name}'");
                    }
                    foreach (var s in c.Body)
                        CheckStmt(s);
                }
                break;
            }

            case ReturnStmt rs:
            {
                if (_currentReturn is null)
                {
                    Error(rs.Line, "return statement outside function");
                    break;
                }
                if (rs.Value is null)
                {
                    if (_currentReturn is not VoidType)
                        Error(rs.Line, $"function returning '{_currentReturn.Name}' must return a value");
                }
                else
                {
                    if (_currentReturn is VoidType)
                    {
                        Error(rs.Line, "void function cannot return a value");
                    }
                    else
                    {
                        var vt = ResolveExprType(rs.Value);
                        if (vt is not null)
                            CheckAssignable(_currentReturn, vt, rs.Line, "return type");
                    }
                }
                break;
            }

            case BreakStmt:
            case ContinueStmt:
            case AsmStmt:
                break;   // nothing to check

            default:
                break;
        }
    }

    // BlockStmt body iteration without an extra scope push (caller manages scope).
    private void CheckBlock(BlockStmt block)
    {
        foreach (var s in block.Statements)
            CheckStmt(s);
    }

    // ── Expression type resolution ────────────────────────────────────────────

    public NccType? ResolveExprType(Expr expr)
    {
        switch (expr)
        {
            case IntLiteralExpr il:
                return il.Value is >= 0 and <= 255 ? NccType.Byte : NccType.Int;

            case FixedLiteralExpr:
                return NccType.Fixed;

            case HexLiteralExpr hl:
            {
                // Strip "0x" or "0X" prefix and parse.
                var hex = hl.Value.StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                    ? hl.Value[2..]
                    : hl.Value;
                if (long.TryParse(hex, System.Globalization.NumberStyles.HexNumber, null, out long v))
                    return v <= 0xFF ? NccType.Byte : NccType.Uint;
                return NccType.Uint;
            }

            case BinLiteralExpr bl:
            {
                // Strip "0b" or "0B" prefix.
                var bin = bl.Value.StartsWith("0b", StringComparison.OrdinalIgnoreCase)
                    ? bl.Value[2..]
                    : bl.Value;
                return bin.Length <= 8 ? NccType.Byte : NccType.Uint;
            }

            case BoolLiteralExpr:
                return NccType.Bool;

            case StringLiteralExpr:
                return new PointerType(NccType.Byte);

            case CharLiteralExpr:
                return NccType.Byte;

            case IdentifierExpr id:
            {
                var sym = LookupSymbol(id.Name);
                if (sym is null)
                {
                    Error(id.Line, $"undeclared identifier '{id.Name}'");
                    return null;
                }
                // Functions used as values yield their FuncType.
                return sym.Type;
            }

            case BinaryExpr be:
                return ResolveBinaryType(be);

            case UnaryExpr ue:
                return ResolveUnaryType(ue);

            case AssignExpr ae:
                return ResolveAssignType(ae);

            case CallExpr ce:
                return ResolveCallType(ce);

            case IndexExpr ix:
                return ResolveIndexType(ix);

            case MemberExpr me:
                return ResolveMemberType(me);

            case CastExpr cast:
            {
                // Check operand, validate cast (any numeric-to-numeric allowed).
                var srcType = ResolveExprType(cast.Operand);
                var dstType = ResolveTypeRef(cast.TargetType, cast.Line);
                if (srcType is not null && !IsCastable(srcType, dstType))
                    Error(cast.Line, $"cannot cast '{srcType.Name}' to '{dstType.Name}'");
                return dstType;
            }

            case SizeofExpr:
                return NccType.Uint;

            default:
                return null;
        }
    }

    // ── Binary expressions ────────────────────────────────────────────────────

    private NccType? ResolveBinaryType(BinaryExpr be)
    {
        var lt = ResolveExprType(be.Left);
        var rt = ResolveExprType(be.Right);
        if (lt is null || rt is null) return null;

        switch (be.Op)
        {
            // Arithmetic
            case TokenType.Plus or TokenType.Minus or TokenType.Star
                or TokenType.Slash or TokenType.Percent:
                if (!lt.IsNumeric)
                {
                    Error(be.Line, $"arithmetic operator on non-numeric type '{lt.Name}'");
                    return null;
                }
                if (!rt.IsNumeric)
                {
                    Error(be.Line, $"arithmetic operator on non-numeric type '{rt.Name}'");
                    return null;
                }
                return ArithmeticResultType(lt, rt);

            // Bitwise
            case TokenType.Amp or TokenType.Pipe or TokenType.Caret
                or TokenType.LShift or TokenType.RShift:
                if (!lt.IsIntegral)
                {
                    Error(be.Line, $"bitwise operator requires integral operands, got '{lt.Name}'");
                    return null;
                }
                if (!rt.IsIntegral)
                {
                    Error(be.Line, $"bitwise operator requires integral operands, got '{rt.Name}'");
                    return null;
                }
                return ArithmeticResultType(lt, rt);

            // Comparison
            case TokenType.Eq or TokenType.NotEq or TokenType.Lt
                or TokenType.Gt or TokenType.LtEq or TokenType.GtEq:
                if (!TypesCompatible(lt, rt))
                    Error(be.Line, $"comparison between incompatible types '{lt.Name}' and '{rt.Name}'");
                return NccType.Bool;

            // Logical
            case TokenType.AmpAmp or TokenType.PipePipe:
                if (lt is not BoolType)
                    Error(be.Line, $"logical operator requires bool operands, got '{lt.Name}'");
                if (rt is not BoolType)
                    Error(be.Line, $"logical operator requires bool operands, got '{rt.Name}'");
                return NccType.Bool;

            default:
                return null;
        }
    }

    // ── Unary expressions ─────────────────────────────────────────────────────

    private NccType? ResolveUnaryType(UnaryExpr ue)
    {
        var ot = ResolveExprType(ue.Operand);
        if (ot is null) return null;

        switch (ue.Op)
        {
            case TokenType.Minus:
                if (!ot.IsNumeric)
                {
                    Error(ue.Line, $"unary minus on non-numeric type '{ot.Name}'");
                    return null;
                }
                return ot;

            case TokenType.Tilde:
                if (!ot.IsIntegral)
                {
                    Error(ue.Line, $"bitwise NOT on non-integral type '{ot.Name}'");
                    return null;
                }
                return ot;

            case TokenType.Bang:
                if (ot is not BoolType)
                    Error(ue.Line, $"logical NOT on non-bool type '{ot.Name}'");
                return NccType.Bool;

            case TokenType.PlusPlus or TokenType.MinusMinus:
                if (!ot.IsIntegral && ot is not PointerType)
                    Error(ue.Line, $"++ / -- on non-integral type '{ot.Name}'");
                return ot;

            case TokenType.Star:
                // Pointer dereference.
                if (ot is not PointerType pt)
                {
                    Error(ue.Line, $"dereference of non-pointer type '{ot.Name}'");
                    return null;
                }
                return pt.BaseType;

            case TokenType.Amp:
                // Address-of: result is pointer to operand type.
                return new PointerType(ot);

            default:
                return ot;
        }
    }

    // ── Assignment expressions ────────────────────────────────────────────────

    private NccType? ResolveAssignType(AssignExpr ae)
    {
        var targetType = ResolveExprType(ae.Target);
        var valueType  = ResolveExprType(ae.Value);
        if (targetType is null || valueType is null) return targetType;

        // Compound assignments check numeric compatibility.
        if (ae.Op != TokenType.Assign)
        {
            bool isArithmetic = ae.Op is TokenType.PlusAssign or TokenType.MinusAssign
                or TokenType.StarAssign or TokenType.SlashAssign or TokenType.PercentAssign;
            bool isBitwise = ae.Op is TokenType.AmpAssign or TokenType.PipeAssign
                or TokenType.CaretAssign or TokenType.LShiftAssign or TokenType.RShiftAssign;

            if (isArithmetic && !targetType.IsNumeric)
                Error(ae.Line, $"arithmetic compound assignment on non-numeric type '{targetType.Name}'");
            if (isBitwise && !targetType.IsIntegral)
                Error(ae.Line, $"bitwise compound assignment on non-integral type '{targetType.Name}'");
        }

        CheckAssignable(targetType, valueType, ae.Line);
        return targetType;
    }

    // ── Call expressions ──────────────────────────────────────────────────────

    private NccType? ResolveCallType(CallExpr ce)
    {
        var calleeType = ResolveExprType(ce.Callee);
        if (calleeType is null) return null;

        if (calleeType is not FuncType ft)
        {
            Error(ce.Line, $"callee is not a function (type '{calleeType.Name}')");
            return null;
        }

        if (ce.Args.Count != ft.ParamTypes.Count)
        {
            Error(ce.Line,
                $"function expects {ft.ParamTypes.Count} argument(s), got {ce.Args.Count}");
        }

        int checkCount = Math.Min(ce.Args.Count, ft.ParamTypes.Count);
        for (int i = 0; i < checkCount; i++)
        {
            var argType = ResolveExprType(ce.Args[i]);
            if (argType is not null && !NccType.CanImplicitlyConvert(argType, ft.ParamTypes[i]))
            {
                Error(ce.Args[i].Line,
                    $"argument {i + 1}: cannot convert '{argType.Name}' to '{ft.ParamTypes[i].Name}'");
            }
        }

        return ft.ReturnType;
    }

    // ── Index expressions ─────────────────────────────────────────────────────

    private NccType? ResolveIndexType(IndexExpr ix)
    {
        var arrType = ResolveExprType(ix.Array);
        var idxType = ResolveExprType(ix.Index);

        if (arrType is null) return null;

        if (idxType is not null && !idxType.IsIntegral)
            Error(ix.Line, $"array index must be integral, got '{idxType.Name}'");

        switch (arrType)
        {
            case ArrayType   at: return at.ElementType;
            case PointerType pt: return pt.BaseType;
            default:
                Error(ix.Line, $"cannot index non-array type '{arrType.Name}'");
                return null;
        }
    }

    // ── Member expressions ────────────────────────────────────────────────────

    private NccType? ResolveMemberType(MemberExpr me)
    {
        var objType = ResolveExprType(me.Object);
        if (objType is null) return null;

        StructType? st;

        if (me.IsArrow)
        {
            // Arrow operator: object must be pointer to struct.
            if (objType is not PointerType pt || pt.BaseType is not StructType pst)
            {
                Error(me.Line,
                    $"'->' requires pointer to struct, got '{objType.Name}'");
                return null;
            }
            st = pst;
        }
        else
        {
            if (objType is not StructType directSt)
            {
                Error(me.Line,
                    $"'.' requires struct type, got '{objType.Name}'");
                return null;
            }
            st = directSt;
        }

        var field = st.Fields.FirstOrDefault(f => f.Name == me.Member);
        if (field is null)
        {
            Error(me.Line,
                $"struct '{st.Name}' has no member '{me.Member}'");
            return null;
        }
        return field.Type;
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    /// <summary>Wider numeric type for arithmetic — byte+byte=byte, anything+fixed=fixed etc.</summary>
    private static NccType ArithmeticResultType(NccType a, NccType b)
    {
        // If either is fixed-point, result is fixed-point.
        if (a is UfixedType || b is UfixedType) return NccType.Ufixed;
        if (a is FixedType  || b is FixedType)  return NccType.Fixed;
        // Integral: promote to wider.
        if (a is UintType   || b is UintType)   return NccType.Uint;
        if (a is IntType    || b is IntType)     return NccType.Int;
        return NccType.Byte;
    }

    /// <summary>True when two types are compatible for comparison or switch matching.</summary>
    private static bool TypesCompatible(NccType a, NccType b)
    {
        if (a == b) return true;
        if (a.IsNumeric && b.IsNumeric) return true;
        if (a is EnumType ea && b is EnumType eb)
            return ea.Name == eb.Name;
        if (a is EnumType && b.IsIntegral) return true;
        if (b is EnumType && a.IsIntegral) return true;
        if (a is PointerType && b is PointerType) return true;
        return false;
    }

    /// <summary>Valid cast: any numeric-to-numeric, pointer-to-pointer, numeric-to-pointer, pointer-to-numeric.</summary>
    private static bool IsCastable(NccType from, NccType to)
    {
        if (from == to) return true;
        if (from.IsNumeric && to.IsNumeric) return true;
        if (from is BoolType && to.IsIntegral) return true;
        if (from.IsIntegral && to is BoolType) return true;
        if (from is PointerType && to is PointerType) return true;
        if (from is PointerType && to.IsIntegral) return true;
        if (from.IsIntegral && to is PointerType) return true;
        if (from is EnumType && to.IsIntegral) return true;
        if (from.IsIntegral && to is EnumType) return true;
        return false;
    }

    /// <summary>Check that a value of type 'from' can be placed in a location of type 'to'.
    /// Emits error or narrowing warning.</summary>
    private void CheckAssignable(NccType to, NccType from, int line, string context = "assignment")
    {
        if (NccType.CanImplicitlyConvert(from, to)) return;

        // Narrowing: int/uint → byte is a warning.
        if (from is IntType or UintType && to is ByteType)
        {
            _warnings.Add($"line {line}: implicit narrowing from '{from.Name}' to 'byte' in {context}");
            return;
        }

        // Pointer types are mutually assignable (unsafe cast, warn).
        if (from is PointerType && to is PointerType)
        {
            _warnings.Add($"line {line}: implicit pointer conversion in {context}");
            return;
        }

        Error(line, $"cannot assign '{from.Name}' to '{to.Name}' in {context}");
    }

    /// <summary>Condition must be bool or numeric (numeric is truthy).</summary>
    private void CheckCondition(NccType t, int line)
    {
        if (t is BoolType || t.IsNumeric || t is PointerType) return;
        Error(line, $"condition must be bool or numeric, got '{t.Name}'");
    }

    // ── TypeRef resolution ───────────────────────────────────────────────────

    private NccType ResolveTypeRef(TypeRef tr, int line)
    {
        NccType? base_ = NccType.ResolveType(tr.Name);

        if (base_ is null)
        {
            if (_structs.TryGetValue(tr.Name, out var st))
                base_ = st;
            else if (_enums.TryGetValue(tr.Name, out var et))
                base_ = et;
            else
            {
                Error(line, $"unknown type '{tr.Name}'");
                base_ = NccType.Byte;   // error recovery
            }
        }

        return tr.IsPointer ? new PointerType(base_) : base_;
    }

    // ── Scope helpers ─────────────────────────────────────────────────────────

    private void PushScope() => _scopes.Add(new Dictionary<string, Symbol>());

    private void PopScope() => _scopes.RemoveAt(_scopes.Count - 1);

    private Symbol? LookupSymbol(string name)
    {
        for (int i = _scopes.Count - 1; i >= 0; i--)
            if (_scopes[i].TryGetValue(name, out var sym))
                return sym;
        return null;
    }

    private void DeclareSymbol(string name, NccType type, SymbolKind kind, int line)
    {
        var scope = _scopes[^1];
        if (scope.ContainsKey(name))
        {
            Error(line, $"'{name}' is already declared in this scope");
            return;
        }
        scope[name] = new Symbol(name, type, kind);
    }

    // ── Constant integer folding (best-effort for array sizes) ───────────────

    private int EvalConstIntExpr(Expr expr)
    {
        return expr switch
        {
            IntLiteralExpr il => (int)il.Value,
            HexLiteralExpr hl => EvalHex(hl.Value),
            BinLiteralExpr bl => EvalBin(bl.Value),
            IdentifierExpr id when LookupSymbol(id.Name) is { Kind: SymbolKind.Const, Type: ByteType or IntType or UintType }
                => 0,   // unknown at check time — use 0
            _ => 0,
        };

        static int EvalHex(string v)
        {
            var s = v.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? v[2..] : v;
            return int.TryParse(s, System.Globalization.NumberStyles.HexNumber, null, out int r) ? r : 0;
        }

        static int EvalBin(string v)
        {
            var s = v.StartsWith("0b", StringComparison.OrdinalIgnoreCase) ? v[2..] : v;
            try { return Convert.ToInt32(s, 2); } catch { return 0; }
        }
    }

    // ── Error helpers ─────────────────────────────────────────────────────────

    private bool Error(int line, string message)
    {
        _errors.Add($"line {line}: {message}");
        return true;
    }
}
