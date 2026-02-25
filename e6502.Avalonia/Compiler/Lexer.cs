namespace e6502.Avalonia.Compiler;

public class Lexer
{
    private readonly string _source;
    private int _pos;
    private int _line = 1;
    private int _col = 1;

    private static readonly Dictionary<string, TokenType> Keywords = new()
    {
        // Type keywords
        ["byte"] = TokenType.Byte,
        ["int"] = TokenType.Int,
        ["uint"] = TokenType.Uint,
        ["bool"] = TokenType.Bool,
        ["fixed"] = TokenType.Fixed,
        ["ufixed"] = TokenType.Ufixed,
        ["void"] = TokenType.Void,
        ["struct"] = TokenType.Struct,
        ["enum"] = TokenType.Enum,
        ["const"] = TokenType.Const,

        // Control flow
        ["if"] = TokenType.If,
        ["else"] = TokenType.Else,
        ["while"] = TokenType.While,
        ["do"] = TokenType.Do,
        ["for"] = TokenType.For,
        ["switch"] = TokenType.Switch,
        ["case"] = TokenType.Case,
        ["default"] = TokenType.Default,
        ["break"] = TokenType.Break,
        ["continue"] = TokenType.Continue,
        ["return"] = TokenType.Return,

        // Other
        ["asm"] = TokenType.Asm,
        ["sizeof"] = TokenType.Sizeof,
        ["true"] = TokenType.BoolLiteral,
        ["false"] = TokenType.BoolLiteral,
        ["__resident"] = TokenType.Resident,
        ["__interrupt"] = TokenType.Interrupt,
    };

    public Lexer(string source)
    {
        _source = source;
    }

    public List<Token> Tokenize()
    {
        var tokens = new List<Token>();

        while (_pos < _source.Length)
        {
            char c = _source[_pos];

            // Whitespace
            if (c == ' ' || c == '\t' || c == '\r')
            {
                Advance();
                continue;
            }

            // Newline
            if (c == '\n')
            {
                Advance();
                _line++;
                _col = 1;
                continue;
            }

            // Comments
            if (c == '/' && _pos + 1 < _source.Length)
            {
                if (_source[_pos + 1] == '/')
                {
                    SkipLineComment();
                    continue;
                }
                if (_source[_pos + 1] == '*')
                {
                    SkipBlockComment();
                    continue;
                }
            }

            // Numbers
            if (char.IsDigit(c))
            {
                tokens.Add(ReadNumber());
                continue;
            }

            // Strings
            if (c == '"')
            {
                tokens.Add(ReadString());
                continue;
            }

            // Characters
            if (c == '\'')
            {
                tokens.Add(ReadChar());
                continue;
            }

            // Identifiers / keywords
            if (c == '_' || char.IsLetter(c))
            {
                tokens.Add(ReadIdentifier());
                continue;
            }

            // Preprocessor
            if (c == '#')
            {
                tokens.Add(ReadPreprocessor());
                continue;
            }

            // Operators and delimiters
            var opToken = ReadOperatorOrDelimiter();
            if (opToken != null)
            {
                tokens.Add(opToken.Value);
                continue;
            }

            // Unknown character
            tokens.Add(new Token(TokenType.Error, c.ToString(), _line, _col));
            Advance();
        }

        tokens.Add(new Token(TokenType.Eof, "", _line, _col));
        return tokens;
    }

    private char Peek() => _pos < _source.Length ? _source[_pos] : '\0';
    private char PeekAt(int offset) => _pos + offset < _source.Length ? _source[_pos + offset] : '\0';

    private void Advance()
    {
        _pos++;
        _col++;
    }

    private void SkipLineComment()
    {
        // Skip the //
        Advance();
        Advance();
        while (_pos < _source.Length && _source[_pos] != '\n')
            Advance();
    }

    private void SkipBlockComment()
    {
        // Skip the /*
        Advance();
        Advance();
        while (_pos < _source.Length)
        {
            if (_source[_pos] == '*' && PeekAt(1) == '/')
            {
                Advance();
                Advance();
                return;
            }
            if (_source[_pos] == '\n')
            {
                _pos++;
                _line++;
                _col = 1;
            }
            else
            {
                Advance();
            }
        }
    }

    private Token ReadNumber()
    {
        int startCol = _col;

        // Check for 0x, 0b prefixes
        if (_source[_pos] == '0' && _pos + 1 < _source.Length)
        {
            char next = _source[_pos + 1];
            if (next == 'x' || next == 'X')
            {
                Advance(); // 0
                Advance(); // x
                int hexStart = _pos;
                while (_pos < _source.Length && IsHexDigit(_source[_pos]))
                    Advance();
                return new Token(TokenType.HexLiteral, _source[hexStart.._pos], _line, startCol);
            }
            if (next == 'b' || next == 'B')
            {
                Advance(); // 0
                Advance(); // b
                int binStart = _pos;
                while (_pos < _source.Length && (_source[_pos] == '0' || _source[_pos] == '1'))
                    Advance();
                return new Token(TokenType.BinLiteral, _source[binStart.._pos], _line, startCol);
            }
        }

        int numStart = _pos;
        bool hasDot = false;
        while (_pos < _source.Length && (char.IsDigit(_source[_pos]) || _source[_pos] == '.'))
        {
            if (_source[_pos] == '.')
            {
                if (hasDot) break;
                hasDot = true;
            }
            Advance();
        }

        string value = _source[numStart.._pos];
        var type = hasDot ? TokenType.FixedLiteral : TokenType.IntLiteral;
        return new Token(type, value, _line, startCol);
    }

    private Token ReadString()
    {
        int startCol = _col;
        Advance(); // skip opening "

        var sb = new System.Text.StringBuilder();
        while (_pos < _source.Length && _source[_pos] != '"')
        {
            if (_source[_pos] == '\\' && _pos + 1 < _source.Length)
            {
                Advance(); // skip backslash
                sb.Append(DecodeEscape(_source[_pos]));
                Advance();
            }
            else
            {
                sb.Append(_source[_pos]);
                Advance();
            }
        }
        if (_pos < _source.Length) Advance(); // skip closing "

        return new Token(TokenType.StringLiteral, sb.ToString(), _line, startCol);
    }

    private Token ReadChar()
    {
        int startCol = _col;
        Advance(); // skip opening '

        string value;
        if (_pos < _source.Length && _source[_pos] == '\\' && _pos + 1 < _source.Length)
        {
            Advance(); // skip backslash
            value = DecodeEscape(_source[_pos]).ToString();
            Advance();
        }
        else if (_pos < _source.Length)
        {
            value = _source[_pos].ToString();
            Advance();
        }
        else
        {
            value = "";
        }

        if (_pos < _source.Length && _source[_pos] == '\'')
            Advance(); // skip closing '

        return new Token(TokenType.CharLiteral, value, _line, startCol);
    }

    private static char DecodeEscape(char c) => c switch
    {
        'n' => '\n',
        't' => '\t',
        '0' => '\0',
        '\\' => '\\',
        '"' => '"',
        '\'' => '\'',
        _ => c,
    };

    private Token ReadIdentifier()
    {
        int startCol = _col;
        int start = _pos;

        while (_pos < _source.Length && (char.IsLetterOrDigit(_source[_pos]) || _source[_pos] == '_'))
            Advance();

        string word = _source[start.._pos];

        if (Keywords.TryGetValue(word, out var kwType))
            return new Token(kwType, word, _line, startCol);

        return new Token(TokenType.Identifier, word, _line, startCol);
    }

    private Token ReadPreprocessor()
    {
        int startCol = _col;
        Advance(); // skip #

        // Check if followed by a letter (directive keyword)
        if (_pos < _source.Length && char.IsLetter(_source[_pos]))
        {
            int start = _pos;
            while (_pos < _source.Length && char.IsLetter(_source[_pos]))
                Advance();
            string directive = _source[start.._pos];

            return directive switch
            {
                "include" => new Token(TokenType.Include, "#include", _line, startCol),
                "define" => new Token(TokenType.Define, "#define", _line, startCol),
                "asset" => new Token(TokenType.Asset, "#asset", _line, startCol),
                "pragma" => new Token(TokenType.Pragma, "#pragma", _line, startCol),
                _ => new Token(TokenType.Hash, "#", _line, startCol),
            };
        }

        return new Token(TokenType.Hash, "#", _line, startCol);
    }

    private Token? ReadOperatorOrDelimiter()
    {
        char c = _source[_pos];
        int startCol = _col;
        char next = PeekAt(1);
        char next2 = PeekAt(2);

        // Three-character operators
        if (c == '<' && next == '<' && next2 == '=')
        {
            Advance(); Advance(); Advance();
            return new Token(TokenType.LShiftAssign, "<<=", _line, startCol);
        }
        if (c == '>' && next == '>' && next2 == '=')
        {
            Advance(); Advance(); Advance();
            return new Token(TokenType.RShiftAssign, ">>=", _line, startCol);
        }

        // Two-character operators
        if (c == '=' && next == '=') { Advance(); Advance(); return new Token(TokenType.Eq, "==", _line, startCol); }
        if (c == '!' && next == '=') { Advance(); Advance(); return new Token(TokenType.NotEq, "!=", _line, startCol); }
        if (c == '<' && next == '=') { Advance(); Advance(); return new Token(TokenType.LtEq, "<=", _line, startCol); }
        if (c == '>' && next == '=') { Advance(); Advance(); return new Token(TokenType.GtEq, ">=", _line, startCol); }
        if (c == '&' && next == '&') { Advance(); Advance(); return new Token(TokenType.AmpAmp, "&&", _line, startCol); }
        if (c == '|' && next == '|') { Advance(); Advance(); return new Token(TokenType.PipePipe, "||", _line, startCol); }
        if (c == '<' && next == '<') { Advance(); Advance(); return new Token(TokenType.LShift, "<<", _line, startCol); }
        if (c == '>' && next == '>') { Advance(); Advance(); return new Token(TokenType.RShift, ">>", _line, startCol); }
        if (c == '+' && next == '=') { Advance(); Advance(); return new Token(TokenType.PlusAssign, "+=", _line, startCol); }
        if (c == '-' && next == '=') { Advance(); Advance(); return new Token(TokenType.MinusAssign, "-=", _line, startCol); }
        if (c == '*' && next == '=') { Advance(); Advance(); return new Token(TokenType.StarAssign, "*=", _line, startCol); }
        if (c == '/' && next == '=') { Advance(); Advance(); return new Token(TokenType.SlashAssign, "/=", _line, startCol); }
        if (c == '%' && next == '=') { Advance(); Advance(); return new Token(TokenType.PercentAssign, "%=", _line, startCol); }
        if (c == '&' && next == '=') { Advance(); Advance(); return new Token(TokenType.AmpAssign, "&=", _line, startCol); }
        if (c == '|' && next == '=') { Advance(); Advance(); return new Token(TokenType.PipeAssign, "|=", _line, startCol); }
        if (c == '^' && next == '=') { Advance(); Advance(); return new Token(TokenType.CaretAssign, "^=", _line, startCol); }
        if (c == '+' && next == '+') { Advance(); Advance(); return new Token(TokenType.PlusPlus, "++", _line, startCol); }
        if (c == '-' && next == '-') { Advance(); Advance(); return new Token(TokenType.MinusMinus, "--", _line, startCol); }
        if (c == '-' && next == '>') { Advance(); Advance(); return new Token(TokenType.Arrow, "->", _line, startCol); }

        // Single-character operators
        TokenType? singleType = c switch
        {
            '+' => TokenType.Plus,
            '-' => TokenType.Minus,
            '*' => TokenType.Star,
            '/' => TokenType.Slash,
            '%' => TokenType.Percent,
            '&' => TokenType.Amp,
            '|' => TokenType.Pipe,
            '^' => TokenType.Caret,
            '~' => TokenType.Tilde,
            '!' => TokenType.Bang,
            '=' => TokenType.Assign,
            '<' => TokenType.Lt,
            '>' => TokenType.Gt,
            '.' => TokenType.Dot,
            '(' => TokenType.LParen,
            ')' => TokenType.RParen,
            '{' => TokenType.LBrace,
            '}' => TokenType.RBrace,
            '[' => TokenType.LBracket,
            ']' => TokenType.RBracket,
            ';' => TokenType.Semicolon,
            ',' => TokenType.Comma,
            ':' => TokenType.Colon,
            _ => null,
        };

        if (singleType != null)
        {
            Advance();
            return new Token(singleType.Value, c.ToString(), _line, startCol);
        }

        return null;
    }

    private static bool IsHexDigit(char c) =>
        char.IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
