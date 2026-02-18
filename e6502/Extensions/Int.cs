namespace KDS.e6502.Extensions;

public static class Int
{
    public static string Hex4(this int value)
    {
        return value.ToString("X4");
    }
    
    public static string Hex2(this int value)
    {
        return value.ToString("X2");
    }
}