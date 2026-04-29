using KDS.e6502;

namespace e6502.Avalonia.Hardware;

public static class VgcVramPort
{
    public static void WriteVramByte(this IBusDevice bus, byte plane, int address, byte value)
    {
        Select(bus, plane, address, autoincrement: false);
        bus.Write(VgcConstants.VramData, value);
    }

    public static byte ReadVramByte(this IBusDevice bus, byte plane, int address)
    {
        byte ctrl = bus.Read(VgcConstants.VramCtrl);
        Select(bus, plane, address, autoincrement: false);
        _ = bus.Read(VgcConstants.VramData); // post the BRAM-backed read
        byte value = bus.Read(VgcConstants.VramData);
        bus.Write(VgcConstants.VramCtrl, ctrl);
        return value;
    }

    public static void WriteVramBlock(this IBusDevice bus, byte plane, int address, ReadOnlySpan<byte> data)
    {
        Select(bus, plane, address, autoincrement: true);
        foreach (byte value in data)
            bus.Write(VgcConstants.VramData, value);
    }

    public static void ReadVramBlock(this IBusDevice bus, byte plane, int address, Span<byte> data)
    {
        Select(bus, plane, address, autoincrement: true);
        _ = bus.Read(VgcConstants.VramData); // first read returns the previous latch
        for (int i = 0; i < data.Length; i++)
            data[i] = bus.Read(VgcConstants.VramData);
    }

    public static void WriteTextCell(this IBusDevice bus, int col, int row, byte ch, byte color)
    {
        int offset = row * VgcConstants.ScreenCols + col;
        bus.WriteVramByte(VgcConstants.VramPlaneChar, offset, ch);
        bus.WriteVramByte(VgcConstants.VramPlaneColor, offset, color);
    }

    public static void WriteTextColor(this IBusDevice bus, int col, int row, byte color)
    {
        int offset = row * VgcConstants.ScreenCols + col;
        bus.WriteVramByte(VgcConstants.VramPlaneColor, offset, color);
    }

    private static void Select(IBusDevice bus, byte plane, int address, bool autoincrement)
    {
        bus.Write(VgcConstants.VramPlane, plane);
        bus.Write(VgcConstants.VramAddrL, (byte)(address & 0xFF));
        bus.Write(VgcConstants.VramAddrH, (byte)((address >> 8) & 0xFF));
        bus.Write(VgcConstants.VramCtrl, autoincrement ? VgcConstants.VramCtrlAutoInc : (byte)0);
    }
}
