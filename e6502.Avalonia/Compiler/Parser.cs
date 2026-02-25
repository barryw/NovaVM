namespace e6502.Avalonia.Compiler;

public class Parser
{
    private readonly List<Token> _tokens;
    private int _pos;
    private readonly List<string> _errors = new();

    public IReadOnlyList<string> Errors => _errors;

    public Parser(List<Token> tokens)
    {
        _tokens = tokens;
        _pos = 0;
    }

    /// <summary>Convenience: parse from source string.</summary>
    public static Parser FromSource(string source) => new(new Lexer(source).Tokenize());

    // ── Helpers ──────────────────────────────────────────────────────────

    private Token Current => _tokens[_pos];

    private Token Peek(int offset = 1) =>
        _tokens[_pos + offset < _tokens.Count ? _pos + offset : _tokens.Count - 1];

    private Token Advance() => _tokens[_pos++];

    private bool Check(TokenType type) => Current.Type == type;

    private bool Match(TokenType type)
    {
        if (!Check(type)) return false;
        Advance();
        return true;
    }

    private Token Expect(TokenType type)
    {
        if (Check(type)) return Advance();
        Error($"expected {type}, got {Current.Type}");
        return Current;
    }

    private void Error(string msg) => _errors.Add($"line {Current.Line}: {msg}");

    private static bool IsTypeKeyword(TokenType t) =>
        t is TokenType.Byte or TokenType.Int or TokenType.Uint or TokenType.Bool
            or TokenType.Fixed or TokenType.Ufixed or TokenType.Void or TokenType.Struct;

    private static bool IsAssignmentOp(TokenType t) =>
        t is TokenType.Assign or TokenType.PlusAssign or TokenType.MinusAssign
            or TokenType.StarAssign or TokenType.SlashAssign or TokenType.PercentAssign
            or TokenType.AmpAssign or TokenType.PipeAssign or TokenType.CaretAssign
            or TokenType.LShiftAssign or TokenType.RShiftAssign;

    // ── Precedence table ─────────────────────────────────────────────────

    private static int GetBinaryPrecedence(TokenType t) => t switch
    {
        TokenType.PipePipe => 1,                              // logical OR
        TokenType.AmpAmp => 2,                                // logical AND
        TokenType.Pipe => 3,                                  // bitwise OR
        TokenType.Caret => 4,                                 // bitwise XOR
        TokenType.Amp => 5,                                   // bitwise AND
        TokenType.Eq or TokenType.NotEq => 6,                 // equality
        TokenType.Lt or TokenType.Gt
            or TokenType.LtEq or TokenType.GtEq => 7,        // comparison
        TokenType.LShift or TokenType.RShift => 8,            // shift
        TokenType.Plus or TokenType.Minus => 9,               // additive
        TokenType.Star or TokenType.Slash
            or TokenType.Percent => 10,                       // multiplicative
        _ => -1,
    };

    // ── Public entry point ───────────────────────────────────────────────

    public Expr ParseExpression() => ParseAssignment();

    // ── Assignment (right-associative) ───────────────────────────────────

    public Expr ParseAssignment()
    {
        var expr = ParseBinaryExpr(1);

        if (IsAssignmentOp(Current.Type))
        {
            var op = Advance();
            var value = ParseAssignment(); // right-associative
            return new AssignExpr(expr, op.Type, value, expr.Line, expr.Col);
        }

        return expr;
    }

    // ── Binary (precedence climbing) ─────────────────────────────────────

    private Expr ParseBinaryExpr(int minPrecedence)
    {
        var left = ParseUnary();

        while (true)
        {
            int prec = GetBinaryPrecedence(Current.Type);
            if (prec < minPrecedence) break;

            var op = Advance();
            var right = ParseBinaryExpr(prec + 1); // left-associative: prec + 1
            left = new BinaryExpr(left, op.Type, right, left.Line, left.Col);
        }

        return left;
    }

    // ── Unary (prefix) ──────────────────────────────────────────────────

    private Expr ParseUnary()
    {
        var t = Current;

        // sizeof
        if (t.Type == TokenType.Sizeof)
            return ParseSizeof();

        // cast: (type)expr — only if we see '(' followed by a type keyword
        if (t.Type == TokenType.LParen && IsTypeKeyword(Peek().Type))
            return ParseCast();

        // prefix unary: - ! ~ ++ -- &(address-of) *(deref)
        if (t.Type is TokenType.Minus or TokenType.Bang or TokenType.Tilde
            or TokenType.PlusPlus or TokenType.MinusMinus
            or TokenType.Amp or TokenType.Star)
        {
            var op = Advance();
            var operand = ParseUnary();
            return new UnaryExpr(op.Type, operand, true, op.Line, op.Column);
        }

        return ParsePostfix(ParsePrimary());
    }

    // ── Postfix ─────────────────────────────────────────────────────────

    private Expr ParsePostfix(Expr left)
    {
        while (true)
        {
            switch (Current.Type)
            {
                case TokenType.LParen: // function call
                    Advance(); // skip (
                    var args = ParseArgList();
                    Expect(TokenType.RParen);
                    left = new CallExpr(left, args, left.Line, left.Col);
                    break;

                case TokenType.LBracket: // array index
                    Advance(); // skip [
                    var index = ParseExpression();
                    Expect(TokenType.RBracket);
                    left = new IndexExpr(left, index, left.Line, left.Col);
                    break;

                case TokenType.Dot: // member access
                    Advance(); // skip .
                    var dotMember = Expect(TokenType.Identifier);
                    left = new MemberExpr(left, dotMember.Value, false, left.Line, left.Col);
                    break;

                case TokenType.Arrow: // pointer member access
                    Advance(); // skip ->
                    var arrowMember = Expect(TokenType.Identifier);
                    left = new MemberExpr(left, arrowMember.Value, true, left.Line, left.Col);
                    break;

                case TokenType.PlusPlus: // postfix ++
                    var ppToken = Advance();
                    left = new UnaryExpr(ppToken.Type, left, false, left.Line, left.Col);
                    break;

                case TokenType.MinusMinus: // postfix --
                    var mmToken = Advance();
                    left = new UnaryExpr(mmToken.Type, left, false, left.Line, left.Col);
                    break;

                default:
                    return left;
            }
        }
    }

    // ── Primary ─────────────────────────────────────────────────────────

    private Expr ParsePrimary()
    {
        var t = Current;

        switch (t.Type)
        {
            case TokenType.IntLiteral:
                Advance();
                return new IntLiteralExpr(long.Parse(t.Value), t.Line, t.Column);

            case TokenType.FixedLiteral:
                Advance();
                return new FixedLiteralExpr(t.Value, t.Line, t.Column);

            case TokenType.HexLiteral:
                Advance();
                return new HexLiteralExpr(t.Value, t.Line, t.Column);

            case TokenType.BinLiteral:
                Advance();
                return new BinLiteralExpr(t.Value, t.Line, t.Column);

            case TokenType.BoolLiteral:
                Advance();
                return new BoolLiteralExpr(t.Value == "true", t.Line, t.Column);

            case TokenType.StringLiteral:
                Advance();
                return new StringLiteralExpr(t.Value, t.Line, t.Column);

            case TokenType.CharLiteral:
                Advance();
                return new CharLiteralExpr(t.Value.Length > 0 ? t.Value[0] : '\0', t.Line, t.Column);

            case TokenType.Identifier:
                Advance();
                return new IdentifierExpr(t.Value, t.Line, t.Column);

            case TokenType.LParen:
                Advance(); // skip (
                var expr = ParseExpression();
                Expect(TokenType.RParen);
                return expr;

            default:
                Error($"unexpected token {t.Type}");
                Advance(); // skip to avoid infinite loop
                return new IntLiteralExpr(0, t.Line, t.Column);
        }
    }

    // ── Cast: (type)expr ────────────────────────────────────────────────

    private Expr ParseCast()
    {
        var lparen = Advance(); // skip (
        var typeRef = ParseTypeRef();
        Expect(TokenType.RParen);
        var operand = ParseUnary();
        return new CastExpr(typeRef, operand, lparen.Line, lparen.Column);
    }

    // ── sizeof(type) ────────────────────────────────────────────────────

    private Expr ParseSizeof()
    {
        var kw = Advance(); // skip sizeof
        Expect(TokenType.LParen);
        var typeRef = ParseTypeRef();
        Expect(TokenType.RParen);
        return new SizeofExpr(typeRef, kw.Line, kw.Column);
    }

    // ── Argument list ───────────────────────────────────────────────────

    private List<Expr> ParseArgList()
    {
        var args = new List<Expr>();
        if (Check(TokenType.RParen)) return args;

        args.Add(ParseAssignment());
        while (Match(TokenType.Comma))
            args.Add(ParseAssignment());

        return args;
    }

    // ── Type reference ──────────────────────────────────────────────────

    public TypeRef ParseTypeRef()
    {
        var t = Current;
        if (IsTypeKeyword(t.Type))
        {
            Advance();
            bool isPointer = Match(TokenType.Star);
            return new TypeRef(t.Value, isPointer, t.Line, t.Column);
        }

        // Could be a user-defined type name (identifier)
        if (t.Type == TokenType.Identifier)
        {
            Advance();
            bool isPointer = Match(TokenType.Star);
            return new TypeRef(t.Value, isPointer, t.Line, t.Column);
        }

        Error($"expected type name, got {t.Type}");
        return new TypeRef("error", false, t.Line, t.Column);
    }

    // ── Statement parsing ────────────────────────────────────────────────

    public Stmt ParseStatement()
    {
        var t = Current;

        return t.Type switch
        {
            TokenType.LBrace => ParseBlock(),
            TokenType.If => ParseIf(),
            TokenType.While => ParseWhile(),
            TokenType.Do => ParseDoWhile(),
            TokenType.For => ParseFor(),
            TokenType.Switch => ParseSwitch(),
            TokenType.Return => ParseReturn(),
            TokenType.Break => ParseBreak(),
            TokenType.Continue => ParseContinue(),
            TokenType.Asm => ParseAsm(),
            _ when IsTypeKeyword(t.Type) && Peek().Type == TokenType.Identifier => ParseVarDecl(),
            _ => ParseExprStmt(),
        };
    }

    // ── Block ────────────────────────────────────────────────────────────

    public BlockStmt ParseBlock()
    {
        var lbrace = Expect(TokenType.LBrace);
        var stmts = new List<Stmt>();

        while (!Check(TokenType.RBrace) && !Check(TokenType.Eof))
            stmts.Add(ParseStatement());

        Expect(TokenType.RBrace);
        return new BlockStmt(stmts, lbrace.Line, lbrace.Column);
    }

    // ── If ───────────────────────────────────────────────────────────────

    private Stmt ParseIf()
    {
        var kw = Advance(); // skip 'if'
        Expect(TokenType.LParen);
        var condition = ParseExpression();
        Expect(TokenType.RParen);
        var then = ParseStatement();
        Stmt? elseStmt = null;
        if (Match(TokenType.Else))
            elseStmt = ParseStatement();
        return new IfStmt(condition, then, elseStmt, kw.Line, kw.Column);
    }

    // ── While ────────────────────────────────────────────────────────────

    private Stmt ParseWhile()
    {
        var kw = Advance(); // skip 'while'
        Expect(TokenType.LParen);
        var condition = ParseExpression();
        Expect(TokenType.RParen);
        var body = ParseStatement();
        return new WhileStmt(condition, body, kw.Line, kw.Column);
    }

    // ── Do-While ─────────────────────────────────────────────────────────

    private Stmt ParseDoWhile()
    {
        var kw = Advance(); // skip 'do'
        var body = ParseStatement();
        Expect(TokenType.While);
        Expect(TokenType.LParen);
        var condition = ParseExpression();
        Expect(TokenType.RParen);
        Expect(TokenType.Semicolon);
        return new DoWhileStmt(body, condition, kw.Line, kw.Column);
    }

    // ── For ──────────────────────────────────────────────────────────────

    private Stmt ParseFor()
    {
        var kw = Advance(); // skip 'for'
        Expect(TokenType.LParen);

        // init
        Stmt? init = null;
        if (!Check(TokenType.Semicolon))
        {
            if (IsTypeKeyword(Current.Type) && Peek().Type == TokenType.Identifier)
                init = ParseVarDecl(); // VarDecl already consumes ';'
            else
            {
                var expr = ParseExpression();
                Expect(TokenType.Semicolon);
                init = new ExprStmt(expr, expr.Line, expr.Col);
            }
        }
        else
        {
            Advance(); // skip ';'
        }

        // condition
        Expr? condition = null;
        if (!Check(TokenType.Semicolon))
            condition = ParseExpression();
        Expect(TokenType.Semicolon);

        // increment
        Expr? increment = null;
        if (!Check(TokenType.RParen))
            increment = ParseExpression();
        Expect(TokenType.RParen);

        var body = ParseStatement();
        return new ForStmt(init, condition, increment, body, kw.Line, kw.Column);
    }

    // ── Switch ───────────────────────────────────────────────────────────

    private Stmt ParseSwitch()
    {
        var kw = Advance(); // skip 'switch'
        Expect(TokenType.LParen);
        var value = ParseExpression();
        Expect(TokenType.RParen);
        Expect(TokenType.LBrace);

        var cases = new List<CaseClause>();

        while (!Check(TokenType.RBrace) && !Check(TokenType.Eof))
        {
            Expr? caseValue = null;
            var caseTok = Current;

            if (Match(TokenType.Case))
            {
                caseValue = ParseExpression();
                Expect(TokenType.Colon);
            }
            else if (Match(TokenType.Default))
            {
                Expect(TokenType.Colon);
            }
            else
            {
                Error($"expected 'case' or 'default', got {Current.Type}");
                Advance();
                continue;
            }

            var body = new List<Stmt>();
            while (!Check(TokenType.Case) && !Check(TokenType.Default)
                   && !Check(TokenType.RBrace) && !Check(TokenType.Eof))
            {
                body.Add(ParseStatement());
            }

            cases.Add(new CaseClause(caseValue, body, caseTok.Line, caseTok.Column));
        }

        Expect(TokenType.RBrace);
        return new SwitchStmt(value, cases, kw.Line, kw.Column);
    }

    // ── Return ───────────────────────────────────────────────────────────

    private Stmt ParseReturn()
    {
        var kw = Advance(); // skip 'return'
        Expr? value = null;
        if (!Check(TokenType.Semicolon))
            value = ParseExpression();
        Expect(TokenType.Semicolon);
        return new ReturnStmt(value, kw.Line, kw.Column);
    }

    // ── Break / Continue ─────────────────────────────────────────────────

    private Stmt ParseBreak()
    {
        var kw = Advance(); // skip 'break'
        Expect(TokenType.Semicolon);
        return new BreakStmt(kw.Line, kw.Column);
    }

    private Stmt ParseContinue()
    {
        var kw = Advance(); // skip 'continue'
        Expect(TokenType.Semicolon);
        return new ContinueStmt(kw.Line, kw.Column);
    }

    // ── Asm ──────────────────────────────────────────────────────────────

    private Stmt ParseAsm()
    {
        var kw = Advance(); // skip 'asm'
        Expect(TokenType.LBrace);

        // Collect raw tokens between { and } as assembly text
        var parts = new List<string>();
        while (!Check(TokenType.RBrace) && !Check(TokenType.Eof))
        {
            parts.Add(Current.Value);
            Advance();
        }

        Expect(TokenType.RBrace);
        Expect(TokenType.Semicolon);
        return new AsmStmt(string.Join(" ", parts), kw.Line, kw.Column);
    }

    // ── Variable declaration ─────────────────────────────────────────────

    private Stmt ParseVarDecl()
    {
        var typeRef = ParseTypeRef();
        var name = Expect(TokenType.Identifier);

        // Array declaration: type name[size];
        if (Match(TokenType.LBracket))
        {
            var size = ParseExpression();
            Expect(TokenType.RBracket);
            Expr? init = null;
            if (Match(TokenType.Assign))
                init = ParseExpression();
            Expect(TokenType.Semicolon);
            return new ArrayDeclStmt(typeRef, name.Value, size, init, typeRef.Line, typeRef.Col);
        }

        // Scalar declaration: type name; or type name = expr;
        Expr? initializer = null;
        if (Match(TokenType.Assign))
            initializer = ParseExpression();
        Expect(TokenType.Semicolon);
        return new VarDeclStmt(typeRef, name.Value, initializer, typeRef.Line, typeRef.Col);
    }

    // ── Expression statement ─────────────────────────────────────────────

    private Stmt ParseExprStmt()
    {
        var expr = ParseExpression();
        Expect(TokenType.Semicolon);
        return new ExprStmt(expr, expr.Line, expr.Col);
    }

    // ── Declaration parsing ───────────────────────────────────────────────

    /// <summary>Parse an entire NCC program from the token stream.</summary>
    public NccProgram ParseProgram()
    {
        var decls = new List<Decl>();
        while (!Check(TokenType.Eof))
            decls.Add(ParseDeclaration());
        return new NccProgram(decls, 1, 1);
    }

    /// <summary>Parse a single top-level declaration.</summary>
    private Decl ParseDeclaration()
    {
        var t = Current;

        // Preprocessor directives
        if (t.Type == TokenType.Include) return ParseInclude();
        if (t.Type == TokenType.Define) return ParseDefine();
        if (t.Type == TokenType.Asset) return ParseAsset();
        if (t.Type == TokenType.Pragma) return ParsePragma();

        // struct / enum
        if (t.Type == TokenType.Struct) return ParseStruct();
        if (t.Type == TokenType.Enum) return ParseEnum();

        // const qualifier
        bool isConst = false;
        if (t.Type == TokenType.Const)
        {
            isConst = true;
            Advance();
        }

        // __resident / __interrupt qualifiers
        bool isResident = false;
        bool isInterrupt = false;
        if (Current.Type == TokenType.Resident)
        {
            isResident = true;
            Advance();
        }
        else if (Current.Type == TokenType.Interrupt)
        {
            isInterrupt = true;
            Advance();
        }

        // Type + name, then decide: function, array, or variable
        var typeRef = ParseTypeRef();
        var name = Expect(TokenType.Identifier);

        if (Check(TokenType.LParen))
            return ParseFunction(typeRef, name.Value, isResident, isInterrupt);

        if (Check(TokenType.LBracket))
            return ParseGlobalArray(typeRef, name.Value, isConst);

        return ParseGlobalVar(typeRef, name.Value, isConst);
    }

    // ── Function declaration ──────────────────────────────────────────────

    private FuncDecl ParseFunction(TypeRef returnType, string name, bool isResident, bool isInterrupt)
    {
        Expect(TokenType.LParen);
        var parms = new List<ParamDecl>();

        if (!Check(TokenType.RParen))
        {
            parms.Add(ParseParamDecl());
            while (Match(TokenType.Comma))
                parms.Add(ParseParamDecl());
        }

        Expect(TokenType.RParen);
        var body = ParseBlock();
        return new FuncDecl(returnType, name, parms, body, isResident, isInterrupt,
            returnType.Line, returnType.Col);
    }

    private ParamDecl ParseParamDecl()
    {
        var typeRef = ParseTypeRef();
        var name = Expect(TokenType.Identifier);
        return new ParamDecl(typeRef, name.Value, typeRef.Line, typeRef.Col);
    }

    // ── Struct declaration ────────────────────────────────────────────────

    private StructDecl ParseStruct()
    {
        var kw = Advance(); // skip 'struct'
        var name = Expect(TokenType.Identifier);
        Expect(TokenType.LBrace);

        var fields = new List<FieldDecl>();
        while (!Check(TokenType.RBrace) && !Check(TokenType.Eof))
        {
            var fieldType = ParseTypeRef();
            // Parse one or more field names with same type (e.g. "fixed x, y;")
            var fieldName = Expect(TokenType.Identifier);
            Expr? arraySize = null;
            if (Match(TokenType.LBracket))
            {
                arraySize = ParseExpression();
                Expect(TokenType.RBracket);
            }
            fields.Add(new FieldDecl(fieldType, fieldName.Value, arraySize, fieldType.Line, fieldType.Col));

            while (Match(TokenType.Comma))
            {
                var extraName = Expect(TokenType.Identifier);
                Expr? extraArraySize = null;
                if (Match(TokenType.LBracket))
                {
                    extraArraySize = ParseExpression();
                    Expect(TokenType.RBracket);
                }
                fields.Add(new FieldDecl(fieldType, extraName.Value, extraArraySize, fieldType.Line, fieldType.Col));
            }

            Expect(TokenType.Semicolon);
        }

        Expect(TokenType.RBrace);
        Match(TokenType.Semicolon); // optional trailing semicolon
        return new StructDecl(name.Value, fields, kw.Line, kw.Column);
    }

    // ── Enum declaration ──────────────────────────────────────────────────

    private EnumDecl ParseEnum()
    {
        var kw = Advance(); // skip 'enum'
        var name = Expect(TokenType.Identifier);
        Expect(TokenType.LBrace);

        var values = new List<EnumValue>();
        if (!Check(TokenType.RBrace))
        {
            values.Add(ParseEnumValue());
            while (Match(TokenType.Comma))
            {
                if (Check(TokenType.RBrace)) break; // trailing comma
                values.Add(ParseEnumValue());
            }
        }

        Expect(TokenType.RBrace);
        Match(TokenType.Semicolon); // optional trailing semicolon
        return new EnumDecl(name.Value, values, kw.Line, kw.Column);
    }

    private EnumValue ParseEnumValue()
    {
        var name = Expect(TokenType.Identifier);
        Expr? value = null;
        if (Match(TokenType.Assign))
            value = ParseExpression();
        return new EnumValue(name.Value, value, name.Line, name.Column);
    }

    // ── Global variable ───────────────────────────────────────────────────

    private GlobalVarDecl ParseGlobalVar(TypeRef type, string name, bool isConst)
    {
        Expr? init = null;
        if (Match(TokenType.Assign))
            init = ParseExpression();
        Expect(TokenType.Semicolon);
        return new GlobalVarDecl(type, name, init, isConst, type.Line, type.Col);
    }

    // ── Global array ──────────────────────────────────────────────────────

    private GlobalArrayDecl ParseGlobalArray(TypeRef elementType, string name, bool isConst)
    {
        Expect(TokenType.LBracket);

        // Size can be empty for initialized arrays: byte arr[] = { 1, 2, 3 };
        Expr size;
        if (Check(TokenType.RBracket))
            size = new IntLiteralExpr(0, Current.Line, Current.Column);
        else
            size = ParseExpression();
        Expect(TokenType.RBracket);

        List<Expr>? initializer = null;
        if (Match(TokenType.Assign))
        {
            Expect(TokenType.LBrace);
            initializer = new List<Expr>();
            if (!Check(TokenType.RBrace))
            {
                initializer.Add(ParseExpression());
                while (Match(TokenType.Comma))
                {
                    if (Check(TokenType.RBrace)) break; // trailing comma
                    initializer.Add(ParseExpression());
                }
            }
            Expect(TokenType.RBrace);
        }

        Expect(TokenType.Semicolon);
        return new GlobalArrayDecl(elementType, name, size, initializer, isConst,
            elementType.Line, elementType.Col);
    }

    // ── #include directive ────────────────────────────────────────────────

    private IncludeDirective ParseInclude()
    {
        var kw = Advance(); // skip Include token

        // Two forms: "file.c" or <file.h>
        if (Check(TokenType.StringLiteral))
        {
            var path = Advance();
            return new IncludeDirective(path.Value, kw.Line, kw.Column);
        }

        // Angle-bracket form: <name.h> — lexer produces Lt, identifiers/dots, Gt
        if (Check(TokenType.Lt))
        {
            Advance(); // skip <
            var parts = new System.Text.StringBuilder();
            while (!Check(TokenType.Gt) && !Check(TokenType.Eof))
            {
                parts.Append(Current.Value);
                Advance();
            }
            Expect(TokenType.Gt);
            return new IncludeDirective(parts.ToString(), kw.Line, kw.Column);
        }

        Error("expected string literal or <path> after #include");
        return new IncludeDirective("", kw.Line, kw.Column);
    }

    // ── #define directive ─────────────────────────────────────────────────

    private DefineDirective ParseDefine()
    {
        var kw = Advance(); // skip Define token
        var name = Expect(TokenType.Identifier);

        // Value is the next token (int literal, hex literal, identifier, string, etc.)
        string value = "";
        if (!Check(TokenType.Eof) && !Check(TokenType.Include) && !Check(TokenType.Define)
            && !Check(TokenType.Asset) && !Check(TokenType.Pragma))
        {
            value = Current.Value;
            Advance();
        }

        return new DefineDirective(name.Value, value, kw.Line, kw.Column);
    }

    // ── #asset directive ──────────────────────────────────────────────────

    private AssetDirective ParseAsset()
    {
        var kw = Advance(); // skip Asset token
        var assetType = Expect(TokenType.Identifier);
        var filePath = Expect(TokenType.StringLiteral);
        return new AssetDirective(assetType.Value, filePath.Value, kw.Line, kw.Column);
    }

    // ── #pragma directive ─────────────────────────────────────────────────

    private PragmaDirective ParsePragma()
    {
        var kw = Advance(); // skip Pragma token
        var name = Expect(TokenType.Identifier);

        // Value is the next token
        string value = "";
        if (!Check(TokenType.Eof) && !Check(TokenType.Include) && !Check(TokenType.Define)
            && !Check(TokenType.Asset) && !Check(TokenType.Pragma))
        {
            value = Current.Value;
            Advance();
        }

        return new PragmaDirective(name.Value, value, kw.Line, kw.Column);
    }
}
