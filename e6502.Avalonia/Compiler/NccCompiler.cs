namespace e6502.Avalonia.Compiler;

/// <summary>
/// Top-level facade: lex → parse → type-check → code-gen.
/// </summary>
public sealed class NccCompiler
{
    /// <summary>Base address for compiled code. Defaults to $0280 (BASIC program area).</summary>
    public ushort BaseAddress { get; init; } = 0x0280;

    public CompileResult Compile(string source)
    {
        // Lex
        var lexer  = new Lexer(source);
        var tokens = lexer.Tokenize();

        // Parse
        var parser  = new Parser(tokens);
        var program = parser.ParseProgram();
        if (parser.Errors.Count > 0)
            return new CompileResult(false, Array.Empty<byte>(), new(), parser.Errors.ToList());

        // Type-check
        var checker = new TypeChecker();
        checker.Check(program);
        if (checker.Errors.Count > 0)
            return new CompileResult(false, Array.Empty<byte>(), new(), checker.Errors.ToList());

        // Code-gen
        var gen = new CodeGen(checker, BaseAddress);
        return gen.Generate(program);
    }
}
