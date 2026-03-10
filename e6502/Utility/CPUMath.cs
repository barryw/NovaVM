namespace KDS.e6502.Utility
{
    internal static class CpuMath
    {
        public static byte BcdToHex(int result)
        {
            return result switch
            {
                > 0xff => throw new InvalidOperationException($"Invalid BCD to hex number: {result}"),
                <= 9 => (byte)result,
                _ => (byte)(((result / 10) << 4) + (result % 10))
            };
        }

        public static int HexToBcd(byte oper)
        {
            // Real 6502 doesn't validate BCD digits — invalid values produce
            // undefined but non-fatal results.  Tolerate them so SID routines
            // (and anything else) that abuse decimal mode don't crash.
            return ((oper >> 4) * 10) + (oper & 0x0f);
        }
        public static int SignExtend(int num)
        {
            if (num < 0x80)
                return num;
            return (0xff << 8 | num) & 0xffff;
        }
    }
}
