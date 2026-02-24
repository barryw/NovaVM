using System;
using System.Collections.Generic;
using System.Linq;

namespace e6502.Avalonia.Help;

public sealed class HelpIndex
{
    private readonly List<HelpTopic> _topics;
    private readonly Dictionary<string, HelpTopic> _byTitle;

    public int TopicCount => _topics.Count;

    public HelpIndex(IEnumerable<HelpTopic> topics)
    {
        _topics = topics.ToList();
        _byTitle = new Dictionary<string, HelpTopic>(StringComparer.OrdinalIgnoreCase);
        foreach (var t in _topics)
            _byTitle.TryAdd(t.Title, t);
    }

    public HelpTopic? FindByTitle(string title)
    {
        return _byTitle.GetValueOrDefault(title);
    }

    public List<HelpTopic> Search(string query, HelpTopicType? filterType = null)
    {
        var candidates = filterType.HasValue
            ? _topics.Where(t => t.Type == filterType.Value)
            : _topics;

        if (string.IsNullOrWhiteSpace(query))
            return candidates.OrderBy(t => t.Title).ToList();

        var terms = query.ToLowerInvariant().Split(' ', StringSplitOptions.RemoveEmptyEntries);
        var scored = new List<(HelpTopic topic, int score)>();

        foreach (var topic in candidates)
        {
            var score = ScoreTopic(topic, terms);
            if (score > 0)
                scored.Add((topic, score));
        }

        return scored.OrderByDescending(s => s.score)
            .ThenBy(s => s.topic.Title)
            .Select(s => s.topic)
            .Take(20)
            .ToList();
    }

    public Dictionary<string, List<HelpTopic>> GetByCategory()
    {
        return _topics
            .GroupBy(t => t.Category)
            .ToDictionary(g => g.Key, g => g.OrderBy(t => t.Title).ToList());
    }

    private static int ScoreTopic(HelpTopic topic, string[] terms)
    {
        var totalScore = 0;
        var titleLower = topic.Title.ToLowerInvariant();

        foreach (var term in terms)
        {
            var termScore = 0;

            if (titleLower == term)
                termScore += 100;
            else if (titleLower.StartsWith(term, StringComparison.Ordinal))
                termScore += 80;
            else if (titleLower.Contains(term, StringComparison.Ordinal))
                termScore += 60;

            if (topic.Keywords.Any(k => k.Contains(term, StringComparison.OrdinalIgnoreCase)))
                termScore += 60;

            if (topic.Syntax?.Contains(term, StringComparison.OrdinalIgnoreCase) == true)
                termScore += 40;

            var bodyCount = CountOccurrences(topic.SearchableText, term);
            if (bodyCount > 0)
                termScore += 10 + Math.Min(bodyCount * 2, 20);

            totalScore += termScore;
        }

        return totalScore;
    }

    private static int CountOccurrences(string text, string term)
    {
        var count = 0;
        var index = 0;
        while ((index = text.IndexOf(term, index, StringComparison.OrdinalIgnoreCase)) >= 0)
        {
            count++;
            index += term.Length;
        }
        return count;
    }
}
