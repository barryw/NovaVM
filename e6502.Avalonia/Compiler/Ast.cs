namespace e6502.Avalonia.Compiler;

// ── Base types ────────────────────────────────────────────────────────

public abstract record AstNode(int Line, int Col);
public abstract record Expr(int Line, int Col) : AstNode(Line, Col);
public abstract record Stmt(int Line, int Col) : AstNode(Line, Col);
public abstract record Decl(int Line, int Col) : AstNode(Line, Col);

// ── Expression nodes ──────────────────────────────────────────────────

// Literals
public record IntLiteralExpr(long Value, int Line, int Col) : Expr(Line, Col);
public record FixedLiteralExpr(string Value, int Line, int Col) : Expr(Line, Col);
public record HexLiteralExpr(string Value, int Line, int Col) : Expr(Line, Col);
public record BinLiteralExpr(string Value, int Line, int Col) : Expr(Line, Col);
public record BoolLiteralExpr(bool Value, int Line, int Col) : Expr(Line, Col);
public record StringLiteralExpr(string Value, int Line, int Col) : Expr(Line, Col);
public record CharLiteralExpr(char Value, int Line, int Col) : Expr(Line, Col);

// Identifier
public record IdentifierExpr(string Name, int Line, int Col) : Expr(Line, Col);

// Operations
public record BinaryExpr(Expr Left, TokenType Op, Expr Right, int Line, int Col) : Expr(Line, Col);
public record UnaryExpr(TokenType Op, Expr Operand, bool Prefix, int Line, int Col) : Expr(Line, Col);
public record AssignExpr(Expr Target, TokenType Op, Expr Value, int Line, int Col) : Expr(Line, Col);

// Access
public record CallExpr(Expr Callee, List<Expr> Args, int Line, int Col) : Expr(Line, Col);
public record IndexExpr(Expr Array, Expr Index, int Line, int Col) : Expr(Line, Col);
public record MemberExpr(Expr Object, string Member, bool IsArrow, int Line, int Col) : Expr(Line, Col);

// Other
public record CastExpr(TypeRef TargetType, Expr Operand, int Line, int Col) : Expr(Line, Col);
public record SizeofExpr(TypeRef TargetType, int Line, int Col) : Expr(Line, Col);

// ── Statement nodes ───────────────────────────────────────────────────

public record ExprStmt(Expr Expression, int Line, int Col) : Stmt(Line, Col);
public record BlockStmt(List<Stmt> Statements, int Line, int Col) : Stmt(Line, Col);
public record VarDeclStmt(TypeRef Type, string Name, Expr? Initializer, int Line, int Col) : Stmt(Line, Col);
public record ArrayDeclStmt(TypeRef ElementType, string Name, Expr Size, Expr? Initializer, int Line, int Col) : Stmt(Line, Col);
public record IfStmt(Expr Condition, Stmt Then, Stmt? Else, int Line, int Col) : Stmt(Line, Col);
public record WhileStmt(Expr Condition, Stmt Body, int Line, int Col) : Stmt(Line, Col);
public record DoWhileStmt(Stmt Body, Expr Condition, int Line, int Col) : Stmt(Line, Col);
public record ForStmt(Stmt? Init, Expr? Condition, Expr? Increment, Stmt Body, int Line, int Col) : Stmt(Line, Col);
public record SwitchStmt(Expr Value, List<CaseClause> Cases, int Line, int Col) : Stmt(Line, Col);
public record CaseClause(Expr? Value, List<Stmt> Body, int Line, int Col);
public record ReturnStmt(Expr? Value, int Line, int Col) : Stmt(Line, Col);
public record BreakStmt(int Line, int Col) : Stmt(Line, Col);
public record ContinueStmt(int Line, int Col) : Stmt(Line, Col);
public record AsmStmt(string Assembly, int Line, int Col) : Stmt(Line, Col);

// ── Declaration nodes ─────────────────────────────────────────────────

public record FuncDecl(TypeRef ReturnType, string Name, List<ParamDecl> Params, BlockStmt Body,
    bool IsResident, bool IsInterrupt, int Line, int Col) : Decl(Line, Col);
public record ParamDecl(TypeRef Type, string Name, int Line, int Col);
public record StructDecl(string Name, List<FieldDecl> Fields, int Line, int Col) : Decl(Line, Col);
public record FieldDecl(TypeRef Type, string Name, Expr? ArraySize, int Line, int Col);
public record EnumDecl(string Name, List<EnumValue> Values, int Line, int Col) : Decl(Line, Col);
public record EnumValue(string Name, Expr? Value, int Line, int Col);
public record GlobalVarDecl(TypeRef Type, string Name, Expr? Initializer, bool IsConst, int Line, int Col) : Decl(Line, Col);
public record GlobalArrayDecl(TypeRef ElementType, string Name, Expr Size, List<Expr>? Initializer, bool IsConst, int Line, int Col) : Decl(Line, Col);
public record IncludeDirective(string Path, int Line, int Col) : Decl(Line, Col);
public record DefineDirective(string Name, string Value, int Line, int Col) : Decl(Line, Col);
public record AssetDirective(string AssetType, string FilePath, int Line, int Col) : Decl(Line, Col);
public record PragmaDirective(string Name, string Value, int Line, int Col) : Decl(Line, Col);

// ── Type reference ────────────────────────────────────────────────────

public record TypeRef(string Name, bool IsPointer, int Line, int Col);

// ── Program (root) ────────────────────────────────────────────────────

public record NccProgram(List<Decl> Declarations, int Line, int Col) : AstNode(Line, Col);
