namespace e6502.Avalonia.Compiler;

// ── Base type ──────────────────────────────────────────────────────────────

public abstract record NccType
{
    public abstract int    SizeInBytes { get; }
    public abstract string Name        { get; }

    // ── Singleton primitives ───────────────────────────────────────────────
    public static readonly NccType Byte   = new ByteType();
    public static readonly NccType Int    = new IntType();
    public static readonly NccType Uint   = new UintType();
    public static readonly NccType Bool   = new BoolType();
    public static readonly NccType Fixed  = new FixedType();
    public static readonly NccType Ufixed = new UfixedType();
    public static readonly NccType Void   = new VoidType();

    // ── Helpers ───────────────────────────────────────────────────────────

    public bool IsNumeric  => this is ByteType or IntType or UintType or FixedType or UfixedType;
    public bool IsIntegral => this is ByteType or IntType or UintType;

    // ── Implicit conversion rules ─────────────────────────────────────────

    public static bool CanImplicitlyConvert(NccType from, NccType to)
    {
        if (from == to) return true;

        return (from, to) switch
        {
            (ByteType,  IntType)    => true,
            (ByteType,  UintType)   => true,
            (ByteType,  FixedType)  => true,
            (ByteType,  UfixedType) => true,
            (IntType,   FixedType)  => true,
            (UintType,  UfixedType) => true,
            _                      => false,
        };
    }

    // ── Name-to-type resolution ───────────────────────────────────────────

    public static NccType? ResolveType(string name) => name switch
    {
        "byte"   => Byte,
        "int"    => Int,
        "uint"   => Uint,
        "bool"   => Bool,
        "fixed"  => Fixed,
        "ufixed" => Ufixed,
        "void"   => Void,
        _        => null,
    };
}

// ── Primitive types ────────────────────────────────────────────────────────

public sealed record ByteType   : NccType { public override int SizeInBytes => 1; public override string Name => "byte";   }
public sealed record IntType    : NccType { public override int SizeInBytes => 2; public override string Name => "int";    }
public sealed record UintType   : NccType { public override int SizeInBytes => 2; public override string Name => "uint";   }
public sealed record BoolType   : NccType { public override int SizeInBytes => 1; public override string Name => "bool";   }
public sealed record FixedType  : NccType { public override int SizeInBytes => 2; public override string Name => "fixed";  }
public sealed record UfixedType : NccType { public override int SizeInBytes => 2; public override string Name => "ufixed"; }
public sealed record VoidType   : NccType { public override int SizeInBytes => 0; public override string Name => "void";   }

// ── Composite types ────────────────────────────────────────────────────────

public sealed record PointerType(NccType BaseType) : NccType
{
    public override int    SizeInBytes => 2;
    public override string Name        => $"{BaseType.Name}*";
}

public sealed record ArrayType(NccType ElementType, int Size) : NccType
{
    public override int    SizeInBytes => ElementType.SizeInBytes * Size;
    public override string Name        => $"{ElementType.Name}[{Size}]";
}

public record StructField(string Name, NccType Type, int Offset);

public sealed record StructType(string Name, List<StructField> Fields) : NccType
{
    public override int    SizeInBytes => Fields.Sum(f => f.Type.SizeInBytes);
    public override string Name        { get; } = Name;
}

public sealed record EnumType(string Name, List<EnumMember> Values) : NccType
{
    public override int    SizeInBytes => 1;
    public override string Name        { get; } = Name;
}

public record EnumMember(string Name, int Value);

public sealed record FuncType(NccType ReturnType, List<NccType> ParamTypes) : NccType
{
    public override int    SizeInBytes => 0;
    public override string Name        => $"func({string.Join(", ", ParamTypes.Select(p => p.Name))})->{ReturnType.Name}";
}
