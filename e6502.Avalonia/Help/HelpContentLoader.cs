using System.Collections.Generic;
using System.IO;

namespace e6502.Avalonia.Help;

public static class HelpContentLoader
{
    public static List<HelpTopic> LoadFromDirectory(string helpDirectory)
    {
        var topics = new List<HelpTopic>();
        var mdFiles = Directory.GetFiles(helpDirectory, "*.md", SearchOption.AllDirectories);

        foreach (var file in mdFiles)
        {
            var content = File.ReadAllText(file);
            var relativePath = Path.GetRelativePath(helpDirectory, file)
                .Replace('\\', '/');
            topics.Add(HelpTopic.Parse(content, relativePath));
        }

        return topics;
    }
}
