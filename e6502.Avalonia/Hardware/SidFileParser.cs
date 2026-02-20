namespace e6502.Avalonia.Hardware;

public static class SidFileParser
{
    public static SidFileInfo Parse(byte[] data)
    {
        if (data.Length < 124)
            return SidFileInfo.Invalid;

        string magic = $"{(char)data[0]}{(char)data[1]}{(char)data[2]}{(char)data[3]}";
        if (magic != "PSID" && magic != "RSID")
            return SidFileInfo.Invalid;

        int version = ReadBE16(data, 4);
        int dataOffset = ReadBE16(data, 6);
        int loadAddress = ReadBE16(data, 8);
        int initAddress = ReadBE16(data, 10);
        int playAddress = ReadBE16(data, 12);
        int songs = ReadBE16(data, 14);
        int startSong = ReadBE16(data, 16);
        uint speed = (uint)(data[18] << 24 | data[19] << 16 | data[20] << 8 | data[21]);

        string title = ReadString(data, 22, 32);
        string author = ReadString(data, 54, 32);
        string copyright = ReadString(data, 86, 32);

        if (dataOffset > data.Length)
            return SidFileInfo.Invalid;

        byte[] payload = new byte[data.Length - dataOffset];
        Array.Copy(data, dataOffset, payload, 0, payload.Length);

        // If loadAddress is 0, first 2 bytes of payload are the load address (little-endian)
        if (loadAddress == 0 && payload.Length >= 2)
        {
            loadAddress = payload[0] | (payload[1] << 8);
            payload = payload[2..];
        }

        return new SidFileInfo
        {
            IsValid = true,
            Magic = magic,
            Version = version,
            LoadAddress = (ushort)loadAddress,
            InitAddress = (ushort)initAddress,
            PlayAddress = (ushort)playAddress,
            Songs = songs,
            StartSong = startSong,
            Speed = speed,
            UsesCiaTiming = (speed & 1) != 0,
            Title = title,
            Author = author,
            Copyright = copyright,
            Payload = payload
        };
    }

    private static int ReadBE16(byte[] data, int offset) =>
        (data[offset] << 8) | data[offset + 1];

    private static string ReadString(byte[] data, int offset, int maxLen)
    {
        int end = offset;
        while (end < offset + maxLen && end < data.Length && data[end] != 0) end++;
        return System.Text.Encoding.ASCII.GetString(data, offset, end - offset);
    }
}

public record struct SidFileInfo
{
    public bool IsValid;
    public string Magic;
    public int Version;
    public ushort LoadAddress;
    public ushort InitAddress;
    public ushort PlayAddress;
    public int Songs;
    public int StartSong;
    public uint Speed;
    public bool UsesCiaTiming;
    public string Title;
    public string Author;
    public string Copyright;
    public byte[] Payload;

    public static readonly SidFileInfo Invalid = new() { IsValid = false, Payload = [], Magic = "", Title = "", Author = "", Copyright = "" };
}
