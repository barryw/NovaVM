using System.Text;
using System.Text.RegularExpressions;

namespace e6502.Avalonia.Help;

public enum HelpTopicType
{
    Guide,
    Command,
    Function,
    Ncc,
    Program
}

public sealed partial class HelpTopic
{
    public required string Title { get; init; }
    public required HelpTopicType Type { get; init; }
    public required string Category { get; init; }
    public required IReadOnlyList<string> Keywords { get; init; }
    public required string? Syntax { get; init; }
    public required IReadOnlyList<string> SeeAlso { get; init; }
    public required string Body { get; init; }
    public required string SourcePath { get; init; }
    public required string SearchableText { get; init; }

    public static HelpTopic Parse(string markdown, string sourcePath)
    {
        var (frontmatter, body) = SplitFrontmatter(markdown);

        var title = ExtractField(frontmatter, "title") ?? Path.GetFileNameWithoutExtension(sourcePath);
        var typeStr = ExtractField(frontmatter, "type") ?? "guide";
        var category = ExtractField(frontmatter, "category") ?? "General";
        var syntax = ExtractField(frontmatter, "syntax");
        var keywords = ExtractList(frontmatter, "keywords");
        var seeAlso = ExtractList(frontmatter, "see_also");

        var type = typeStr.ToLowerInvariant() switch
        {
            "command" => HelpTopicType.Command,
            "function" => HelpTopicType.Function,
            "ncc" => HelpTopicType.Ncc,
            "program" => HelpTopicType.Program,
            _ => HelpTopicType.Guide
        };

        var searchable = BuildSearchableText(title, keywords, syntax, body);

        return new HelpTopic
        {
            Title = title,
            Type = type,
            Category = category,
            Keywords = keywords,
            Syntax = syntax,
            SeeAlso = seeAlso,
            Body = body,
            SourcePath = sourcePath,
            SearchableText = searchable
        };
    }

    private static (string frontmatter, string body) SplitFrontmatter(string markdown)
    {
        var trimmed = markdown.TrimStart();
        if (!trimmed.StartsWith("---"))
            return ("", markdown);

        var endIndex = trimmed.IndexOf("---", 3, StringComparison.Ordinal);
        if (endIndex < 0)
            return ("", markdown);

        var frontmatter = trimmed[3..endIndex].Trim();
        var body = trimmed[(endIndex + 3)..].Trim();
        return (frontmatter, body);
    }

    private static string? ExtractField(string frontmatter, string key)
    {
        var pattern = $@"^{key}\s*:\s*""?([^""\n]+)""?\s*$";
        var match = Regex.Match(frontmatter, pattern, RegexOptions.Multiline | RegexOptions.IgnoreCase);
        return match.Success ? match.Groups[1].Value.Trim().Trim('"') : null;
    }

    private static List<string> ExtractList(string frontmatter, string key)
    {
        var pattern = $@"^{key}\s*:\s*\[([^\]]*)\]";
        var match = Regex.Match(frontmatter, pattern, RegexOptions.Multiline | RegexOptions.IgnoreCase);
        if (!match.Success) return [];

        return match.Groups[1].Value
            .Split(',')
            .Select(s => s.Trim().Trim('"', '\'', ' '))
            .Where(s => s.Length > 0)
            .ToList();
    }

    private static string BuildSearchableText(string title, IEnumerable<string> keywords, string? syntax, string body)
    {
        var sb = new StringBuilder();
        sb.Append(title).Append(' ');
        foreach (var kw in keywords)
            sb.Append(kw).Append(' ');
        if (syntax != null)
            sb.Append(syntax).Append(' ');
        var plain = MarkdownStrip().Replace(body, " ");
        sb.Append(plain);
        return sb.ToString().ToLowerInvariant();
    }

    [GeneratedRegex(@"[#*`\[\]|_>{}\-]")]
    private static partial Regex MarkdownStrip();
}
