namespace KDS.e6502;

public static class RuntimeOptions
{
    public static int GetIntFromEnvironment(string name, int fallback, int min = 1, int max = int.MaxValue)
    {
        string? value = Environment.GetEnvironmentVariable(name);
        if (!int.TryParse(value, out int parsed))
            return fallback;
        if (parsed < min || parsed > max)
            return fallback;
        return parsed;
    }

    public static bool GetFlagFromEnvironment(string name, bool fallback = false)
    {
        string? value = Environment.GetEnvironmentVariable(name);
        if (value is null)
            return fallback;

        return value.Trim().ToLowerInvariant() switch
        {
            "1" or "true" or "yes" or "on" => true,
            "0" or "false" or "no" or "off" => false,
            _ => fallback
        };
    }
}
