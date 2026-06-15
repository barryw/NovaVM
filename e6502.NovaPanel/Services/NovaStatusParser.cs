using System.Text.Json.Nodes;

namespace e6502.NovaPanel.Services;

public static class NovaStatusParser
{
    public static NovaStatus Parse(JsonObject? status)
    {
        if (status is null)
            return NovaStatus.Empty;

        var health = status["health"] as JsonObject;
        var sdStatus = status["sdStatus"] as JsonObject;
        var wifi = status["wifi"] as JsonObject;
        var audio = status["audio"] as JsonObject;
        var drivesArr = status["drives"] as JsonArray;
        var bootConfig = status["bootConfig"] as JsonObject;

        bool ok                 = Bool(health, "ok");
        string bootPhase        = Str(health, "bootPhase");
        bool sdMounted          = Bool(health, "sdMounted");
        bool fpgaBridge         = Bool(health, "fpgaBridgeAvailable");

        var sdInfo = new SdInfo(
            Str(sdStatus, "cardType"),
            Bool(sdStatus, "mounted"));

        var wifiInfo = new WifiInfo(
            Bool(wifi, "configured"),
            Bool(wifi, "connected"),
            Str(wifi, "ssid"),
            Int(wifi, "rssi"),
            Str(wifi, "localIp"));

        var audioInfo = new AudioInfo(
            Bool(audio, "ok"),
            Str(audio, "status"));

        var drives = ParseDrives(drivesArr);
        var (runtimes, activeRuntime) = ParseRuntimes(bootConfig);

        return new NovaStatus(ok, bootPhase, sdMounted, fpgaBridge,
            sdInfo, wifiInfo, audioInfo, drives, runtimes, activeRuntime);
    }

    // ── Drives ────────────────────────────────────────────────────────────

    private static IReadOnlyList<DriveInfo> ParseDrives(JsonArray? arr)
    {
        if (arr is null) return Array.Empty<DriveInfo>();
        var result = new List<DriveInfo>(arr.Count);
        foreach (var node in arr)
        {
            var obj = node as JsonObject;
            result.Add(new DriveInfo(
                Str(obj, "slot"),
                Bool(obj, "mounted"),
                Str(obj, "currentPath"),
                Str(obj, "configuredPath")));
        }
        return result;
    }

    // ── Runtimes ──────────────────────────────────────────────────────────

    private static (IReadOnlyList<RuntimeInfo> Runtimes, string ActiveRuntime) ParseRuntimes(JsonObject? bootConfig)
    {
        if (bootConfig is null)
            return (Array.Empty<RuntimeInfo>(), "");

        string defaultRuntime = Str(bootConfig["vm"] as JsonObject, "defaultRuntime");
        var languages = bootConfig["languages"] as JsonObject;

        if (languages is null)
            return (Array.Empty<RuntimeInfo>(), defaultRuntime);

        var result = new List<RuntimeInfo>(languages.Count);
        foreach (var kv in languages)
        {
            string name = kv.Key;
            var lang = kv.Value as JsonObject;
            result.Add(new RuntimeInfo(
                name,
                Str(lang, "rom"),
                Str(lang, "extensionRom"),
                Bool(lang, "autoboot"),
                name == defaultRuntime));
        }
        return (result, defaultRuntime);
    }

    // ── Helpers ───────────────────────────────────────────────────────────

    private static string Str(JsonObject? obj, string key) =>
        obj?[key]?.GetValue<string>() ?? "";

    private static bool Bool(JsonObject? obj, string key)
    {
        try { return obj?[key]?.GetValue<bool>() ?? false; }
        catch { return false; }
    }

    private static int Int(JsonObject? obj, string key)
    {
        try { return obj?[key]?.GetValue<int>() ?? 0; }
        catch { return 0; }
    }
}
