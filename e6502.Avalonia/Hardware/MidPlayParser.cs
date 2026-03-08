using System.Collections.Generic;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Parses MIDPLAY filename strings with optional voice=channel mapping.
/// Format: "filename" or "filename,voice=channel,voice=channel,..."
/// Voice numbers are 1-based (1-14), converted to 0-based internally.
/// </summary>
public static class MidPlayParser
{
    public static (string Filename, Dictionary<int, int>? Mapping) Parse(string input)
    {
        int commaIdx = input.IndexOf(',');
        if (commaIdx < 0)
            return (input, null);

        string filename = input[..commaIdx];
        string rest = input[(commaIdx + 1)..];

        var mapping = new Dictionary<int, int>();
        foreach (var pair in rest.Split(','))
        {
            var parts = pair.Split('=');
            if (parts.Length != 2) continue;
            if (!int.TryParse(parts[0].Trim(), out int voice)) continue;
            if (!int.TryParse(parts[1].Trim(), out int channel)) continue;
            if (voice < 1 || voice > 14) continue;
            if (channel < 0 || channel > 15) continue;
            mapping[voice - 1] = channel; // convert to 0-based
        }

        return (filename, mapping.Count > 0 ? mapping : null);
    }
}
