namespace e6502.NovaPanel.Services;

public sealed record DriveInfo(string Slot, bool Mounted, string CurrentPath, string ConfiguredPath);
public sealed record RuntimeInfo(string Name, string Rom, string ExtensionRom, bool Autoboot, bool IsActive);
public sealed record WifiInfo(bool Configured, bool Connected, string Ssid, int Rssi, string LocalIp);
public sealed record SdInfo(string CardType, bool Mounted);
public sealed record AudioInfo(bool Ok, string Status);

public sealed record NovaStatus(
    bool Ok,
    string BootPhase,
    bool SdMounted,
    bool FpgaBridgeAvailable,
    SdInfo Sd,
    WifiInfo Wifi,
    AudioInfo Audio,
    IReadOnlyList<DriveInfo> Drives,
    IReadOnlyList<RuntimeInfo> Runtimes,
    string ActiveRuntime)
{
    public static NovaStatus Empty { get; } = new(
        false, "", false, false,
        new SdInfo("", false), new WifiInfo(false, false, "", 0, ""),
        new AudioInfo(false, ""), Array.Empty<DriveInfo>(), Array.Empty<RuntimeInfo>(), "");
}
