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
            // validate input is valid packed BCD 
            if (oper > 0x99)
                throw new InvalidOperationException($"Invalid BCD number: {oper:X2}");
            if ((oper & 0x0f) > 0x09)
                throw new InvalidOperationException($"Invalid BCD number: {oper:X2}");

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
