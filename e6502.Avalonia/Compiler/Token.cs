namespace e6502.Avalonia.Compiler;

public enum TokenType
{
    // Literals
    IntLiteral, FixedLiteral, StringLiteral, CharLiteral,
    HexLiteral, BinLiteral, BoolLiteral,

    // Identifiers
    Identifier,

    // Type keywords
    Byte, Int, Uint, Bool, Fixed, Ufixed, Void, Struct, Enum, Const,

    // Control flow keywords
    If, Else, While, Do, For, Switch, Case, Default, Break, Continue, Return,

    // Other keywords
    Asm, Resident, Interrupt, Sizeof, Asset, Pragma, Include, Define,

    // Operators
    Plus, Minus, Star, Slash, Percent,          // + - * / %
    Amp, Pipe, Caret, Tilde, Bang,              // & | ^ ~ !
    AmpAmp, PipePipe,                           // && ||
    LShift, RShift,                             // << >>
    Eq, NotEq, Lt, Gt, LtEq, GtEq,            // == != < > <= >=
    Assign, PlusAssign, MinusAssign,            // = += -=
    StarAssign, SlashAssign, PercentAssign,     // *= /= %=
    AmpAssign, PipeAssign, CaretAssign,         // &= |= ^=
    LShiftAssign, RShiftAssign,                 // <<= >>=
    PlusPlus, MinusMinus,                       // ++ --
    Arrow, Dot,                                 // -> .

    // Delimiters
    LParen, RParen, LBrace, RBrace,            // ( ) { }
    LBracket, RBracket,                         // [ ]
    Semicolon, Comma, Colon, Hash,              // ; , : #

    // Special
    Eof, Error,
}

public readonly record struct Token(TokenType Type, string Value, int Line, int Column);
