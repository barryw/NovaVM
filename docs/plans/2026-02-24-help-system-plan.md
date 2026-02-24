# NovaBASIC In-App Help System — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a searchable, modern help panel to the emulator that loads from canonical Markdown documentation, with BASIC `HELP` command integration and "Try this!" example injection.

**Architecture:** Markdown files with YAML frontmatter in `docs/help/` are embedded as resources in the Avalonia assembly. A `HelpIndex` parses and indexes them at startup. A `HelpPanel` UserControl renders topics with styled typography. The BASIC ROM gets a `HELP` token that writes to a VGC register, intercepted by `CompositeBusDevice` to open the panel. Pandoc generates the PDF user guide from the same Markdown source.

**Tech Stack:** C#/.NET 10, Avalonia 11.3, MSTest, 6502 assembly (ca65/ld65), Pandoc + LaTeX

**Design Document:** `docs/plans/2026-02-24-help-system-design.md`

---

## Phase 1: Help Content & Data Model

### Task 1: Create sample help content files

Create the directory structure and a few representative Markdown files to test against.

**Files:**
- Create: `docs/help/guides/sprites.md`
- Create: `docs/help/reference/commands/line.md`
- Create: `docs/help/reference/commands/sprite.md`
- Create: `docs/help/reference/commands/gcolor.md`
- Create: `docs/help/reference/functions/collision.md`

**Step 1: Create directory structure**

```bash
mkdir -p docs/help/guides docs/help/reference/commands docs/help/reference/functions
```

**Step 2: Create guide file — `docs/help/guides/sprites.md`**

```markdown
---
title: Sprites
type: guide
category: Graphics
keywords: [sprite, animation, collision, shape, movement]
see_also: [sprite, spritedata, spriteshape, spriteset, collision]
---

NovaBASIC supports up to 16 hardware sprites, each 16x16 pixels with 4-bit color (16 colors per sprite). Sprites are rendered by the VGC independently of the text and graphics layers, making them ideal for game characters, projectiles, and other moving objects.

## Enabling and Positioning

Each sprite has an index from 0 to 15. To use a sprite, enable it and set its position:

```basic
SPRITE 0,ON
SPRITE 0,100,80
```

The first command enables sprite 0. The second sets its position to x=100, y=80. Coordinates use the graphics resolution (0-319 for x, 0-199 for y).

## Defining Shapes

Sprites use shape slots (0-255) stored in dedicated shape RAM. Define pixel data with `SPRITEDATA`:

::: tryit
```basic
10 REM SIMPLE ARROW SPRITE
20 SPRITEDATA 0,0,"0000000100000000"
30 SPRITEDATA 0,1,"0000001110000000"
40 SPRITEDATA 0,2,"0000011111000000"
50 SPRITEDATA 0,3,"0000000100000000"
60 SPRITEDATA 0,4,"0000000100000000"
70 SPRITE 0,ON
80 SPRITESHAPE 0,0
90 SPRITE 0,160,100
```
:::

Each `SPRITEDATA` call defines one row (0-15) of a shape slot. The hex string encodes 16 pixels, each being a color index 0-F. Color 0 is transparent by default.

## Priority Layers

Sprites render at one of three priority levels:

| Priority | Constant | Rendering Order |
|----------|----------|-----------------|
| 0 | Behind | Behind text and graphics |
| 1 | Between | Between background and text |
| 2 | Front | In front of everything |

```basic
SPRITE 0,PRIORITY,2
```

## Collision Detection

The VGC detects two types of collisions each frame:

- **Sprite-sprite:** Two enabled sprites overlap on non-transparent pixels
- **Sprite-background:** A sprite overlaps a non-zero graphics pixel

Read collisions with the `COLLISION()` and `BUMPED()` functions:

```basic
IF COLLISION(0) THEN PRINT "SPRITE 0 HIT ANOTHER SPRITE"
IF BUMPED(0) THEN PRINT "SPRITE 0 HIT BACKGROUND"
```

::: note
Collision flags are cleared after reading. Read them once per frame and store the result if you need to check multiple times.
:::

## Flipping

Sprites can be flipped horizontally or vertically without modifying shape data:

```basic
SPRITE 0,FLIPX,ON
SPRITE 0,FLIPY,ON
```

::: warning
Flipping applies to the rendered sprite only. The underlying shape data in shape RAM is unchanged.
:::
```

**Step 3: Create command reference files**

`docs/help/reference/commands/line.md`:
```markdown
---
title: LINE
type: command
category: Graphics
keywords: [line, draw, graphics, straight]
syntax: "LINE x1,y1,x2,y2"
see_also: [plot, rect, circle, gcolor]
---

Draws a straight line between two points using the current graphics color.

## Parameters

| Parameter | Description |
|-----------|-------------|
| x1, y1 | Start point (0-319, 0-199) |
| x2, y2 | End point (0-319, 0-199) |

## Examples

::: tryit
```basic
10 MODE 2 : GCOLOR 1
20 LINE 0,0,319,199
30 LINE 319,0,0,199
```
:::

## Notes

- Requires graphics mode (`MODE 2` or `MODE 3`)
- Color is set by the most recent `GCOLOR` call
- Coordinates outside the screen are clipped
```

`docs/help/reference/commands/sprite.md`:
```markdown
---
title: SPRITE
type: command
category: Sprites
keywords: [sprite, enable, position, flip, priority]
syntax: "SPRITE n,ON|OFF | SPRITE n,x,y | SPRITE n,FLIPX|FLIPY,ON|OFF | SPRITE n,PRIORITY,p"
see_also: [spritedata, spriteshape, spriteset, collision, bumped]
---

Controls sprite properties: enable/disable, position, flipping, and priority.

## Syntax Variants

| Form | Description |
|------|-------------|
| `SPRITE n,ON` | Enable sprite n |
| `SPRITE n,OFF` | Disable sprite n |
| `SPRITE n,x,y` | Set sprite position |
| `SPRITE n,FLIPX,ON` | Enable horizontal flip |
| `SPRITE n,FLIPY,ON` | Enable vertical flip |
| `SPRITE n,PRIORITY,p` | Set render priority (0-2) |

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| n | 0-15 | Sprite index |
| x | 0-319 | Horizontal position |
| y | 0-199 | Vertical position |
| p | 0-2 | Priority layer (0=behind, 1=between, 2=front) |

## Examples

::: tryit
```basic
10 SPRITE 0,ON
20 SPRITE 0,160,100
30 SPRITE 0,PRIORITY,2
40 SPRITE 0,FLIPX,ON
```
:::

## Notes

- Sprites must have shape data defined via `SPRITEDATA` or `SPRITESHAPE` to be visible
- Position coordinates use the 320x200 graphics resolution
- See the Sprites guide for a full overview
```

`docs/help/reference/commands/gcolor.md`:
```markdown
---
title: GCOLOR
type: command
category: Graphics
keywords: [color, graphics, draw, palette]
syntax: "GCOLOR c"
see_also: [line, plot, rect, circle, fill, color]
---

Sets the current graphics drawing color for subsequent graphics commands.

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| c | 0-15 | Color index from the 16-color palette |

## Examples

```basic
MODE 2
GCOLOR 1 : LINE 0,0,319,199
GCOLOR 5 : CIRCLE 160,100,50
```

## Notes

- Affects `PLOT`, `LINE`, `RECT`, `CIRCLE`, `FILL`, and `PAINT`
- Does not affect text color (use `COLOR` for that)
- Color 0 is the background color
```

`docs/help/reference/functions/collision.md`:
```markdown
---
title: COLLISION
type: function
category: Sprites
keywords: [collision, sprite, hit, detect]
syntax: "COLLISION(n)"
see_also: [bumped, sprite, spritex, spritey]
---

Returns the sprite-sprite collision status for sprite n.

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| n | 0-15 | Sprite index to check |

## Return Value

Non-zero if sprite n collided with another sprite since the last read; 0 otherwise.

## Examples

```basic
100 C = COLLISION(0)
110 IF C THEN PRINT "HIT!"
```

## Notes

- The collision flag is cleared after reading
- Collision detection happens once per frame during rendering
- Only enabled sprites with non-transparent overlapping pixels register collisions
```

**Step 4: Commit**

```bash
git add docs/help/
git commit -m "feat: add sample help content in canonical Markdown format"
```

---

### Task 2: HelpTopic data model

**Files:**
- Create: `e6502.Avalonia/Help/HelpTopic.cs`
- Create: `e6502UnitTests/HelpTopicTests.cs`

**Step 1: Write failing test**

Create `e6502UnitTests/HelpTopicTests.cs`:

```csharp
using KDS.e6502.Help;

namespace e6502UnitTests;

[TestClass]
public class HelpTopicTests
{
    private const string SampleMarkdown = """
        ---
        title: LINE
        type: command
        category: Graphics
        keywords: [line, draw, graphics]
        syntax: "LINE x1,y1,x2,y2"
        see_also: [plot, rect, circle]
        ---

        Draws a line between two points.

        ## Parameters

        | Parameter | Description |
        |-----------|-------------|
        | x1, y1 | Start point |

        ## Examples

        ```basic
        LINE 0,0,319,199
        ```
        """;

    [TestMethod]
    public void Parse_ExtractsTitle()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.AreEqual("LINE", topic.Title);
    }

    [TestMethod]
    public void Parse_ExtractsType()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.AreEqual(HelpTopicType.Command, topic.Type);
    }

    [TestMethod]
    public void Parse_ExtractsCategory()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.AreEqual("Graphics", topic.Category);
    }

    [TestMethod]
    public void Parse_ExtractsKeywords()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        CollectionAssert.AreEquivalent(
            new[] { "line", "draw", "graphics" }, topic.Keywords.ToArray());
    }

    [TestMethod]
    public void Parse_ExtractsSyntax()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.AreEqual("LINE x1,y1,x2,y2", topic.Syntax);
    }

    [TestMethod]
    public void Parse_ExtractsSeeAlso()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        CollectionAssert.AreEquivalent(
            new[] { "plot", "rect", "circle" }, topic.SeeAlso.ToArray());
    }

    [TestMethod]
    public void Parse_ExtractsBody()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.IsTrue(topic.Body.Contains("Draws a line"));
        Assert.IsTrue(topic.Body.Contains("## Parameters"));
    }

    [TestMethod]
    public void Parse_SetsSourcePath()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.AreEqual("reference/commands/line.md", topic.SourcePath);
    }

    [TestMethod]
    public void Parse_BuildsSearchableText()
    {
        var topic = HelpTopic.Parse(SampleMarkdown, "reference/commands/line.md");
        Assert.IsTrue(topic.SearchableText.Contains("line"));
        Assert.IsTrue(topic.SearchableText.Contains("draws a line"));
        // Should be lowercased
        Assert.AreEqual(topic.SearchableText, topic.SearchableText.ToLowerInvariant());
    }

    [TestMethod]
    public void Parse_GuideType()
    {
        var md = """
            ---
            title: Sprites
            type: guide
            category: Graphics
            keywords: [sprite]
            see_also: [sprite]
            ---

            Guide about sprites.
            """;
        var topic = HelpTopic.Parse(md, "guides/sprites.md");
        Assert.AreEqual(HelpTopicType.Guide, topic.Type);
        Assert.IsNull(topic.Syntax);
    }

    [TestMethod]
    public void Parse_FunctionType()
    {
        var md = """
            ---
            title: COLLISION
            type: function
            category: Sprites
            keywords: [collision]
            syntax: "COLLISION(n)"
            see_also: [bumped]
            ---

            Returns collision status.
            """;
        var topic = HelpTopic.Parse(md, "reference/functions/collision.md");
        Assert.AreEqual(HelpTopicType.Function, topic.Type);
    }
}
```

**Step 2: Run test to verify it fails**

```bash
dotnet test --filter "FullyQualifiedName~HelpTopic" --verbosity normal
```

Expected: FAIL — `HelpTopic` class doesn't exist.

**Step 3: Implement HelpTopic**

Create `e6502.Avalonia/Help/HelpTopic.cs`:

```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace KDS.e6502.Help;

public enum HelpTopicType
{
    Guide,
    Command,
    Function
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
        // Strip markdown formatting for body search
        var plain = MarkdownStrip().Replace(body, " ");
        sb.Append(plain);
        return sb.ToString().ToLowerInvariant();
    }

    [GeneratedRegex(@"[#*`\[\]|_>{}\-]")]
    private static partial Regex MarkdownStrip();
}
```

**Step 4: Run tests**

```bash
dotnet test --filter "FullyQualifiedName~HelpTopic" --verbosity normal
```

Expected: all pass.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Help/HelpTopic.cs e6502UnitTests/HelpTopicTests.cs
git commit -m "feat: add HelpTopic data model with frontmatter parsing"
```

---

### Task 3: HelpIndex — loading and search

**Files:**
- Create: `e6502.Avalonia/Help/HelpIndex.cs`
- Create: `e6502UnitTests/HelpIndexTests.cs`

**Step 1: Write failing tests**

Create `e6502UnitTests/HelpIndexTests.cs`:

```csharp
using KDS.e6502.Help;

namespace e6502UnitTests;

[TestClass]
public class HelpIndexTests
{
    private HelpIndex _index = null!;

    [TestInitialize]
    public void Setup()
    {
        var topics = new[]
        {
            HelpTopic.Parse("""
                ---
                title: LINE
                type: command
                category: Graphics
                keywords: [line, draw]
                syntax: "LINE x1,y1,x2,y2"
                see_also: [plot, rect]
                ---
                Draws a line between two points.
                """, "reference/commands/line.md"),

            HelpTopic.Parse("""
                ---
                title: PLOT
                type: command
                category: Graphics
                keywords: [plot, pixel, point]
                syntax: "PLOT x,y"
                see_also: [line, gcolor]
                ---
                Plots a single pixel at the given coordinates.
                """, "reference/commands/plot.md"),

            HelpTopic.Parse("""
                ---
                title: SPRITE
                type: command
                category: Sprites
                keywords: [sprite, enable, position]
                syntax: "SPRITE n,ON|OFF"
                see_also: [spritedata]
                ---
                Controls sprite properties.
                """, "reference/commands/sprite.md"),

            HelpTopic.Parse("""
                ---
                title: COLLISION
                type: function
                category: Sprites
                keywords: [collision, hit, detect]
                syntax: "COLLISION(n)"
                see_also: [bumped, sprite]
                ---
                Returns sprite collision status.
                """, "reference/functions/collision.md"),

            HelpTopic.Parse("""
                ---
                title: Sprites
                type: guide
                category: Graphics
                keywords: [sprite, animation, collision, shape]
                see_also: [sprite, spritedata]
                ---
                NovaBASIC supports up to 16 hardware sprites.
                """, "guides/sprites.md"),
        };

        _index = new HelpIndex(topics);
    }

    [TestMethod]
    public void Search_ExactTitleMatch_RankedFirst()
    {
        var results = _index.Search("line");
        Assert.AreEqual("LINE", results[0].Title);
    }

    [TestMethod]
    public void Search_PrefixMatch_FindsMultiple()
    {
        var results = _index.Search("spr");
        // Should find SPRITE command, Sprites guide, and COLLISION (has "sprite" in keywords/body)
        Assert.IsTrue(results.Count >= 2);
        var titles = results.Select(r => r.Title).ToList();
        Assert.IsTrue(titles.Contains("SPRITE"));
        Assert.IsTrue(titles.Contains("Sprites"));
    }

    [TestMethod]
    public void Search_BodyMatch_Found()
    {
        var results = _index.Search("pixel");
        var titles = results.Select(r => r.Title).ToList();
        Assert.IsTrue(titles.Contains("PLOT"));
    }

    [TestMethod]
    public void Search_FilterByCommand()
    {
        var results = _index.Search("spr", HelpTopicType.Command);
        Assert.IsTrue(results.All(r => r.Type == HelpTopicType.Command));
        Assert.IsTrue(results.Any(r => r.Title == "SPRITE"));
    }

    [TestMethod]
    public void Search_FilterByGuide()
    {
        var results = _index.Search("sprite", HelpTopicType.Guide);
        Assert.IsTrue(results.All(r => r.Type == HelpTopicType.Guide));
        Assert.AreEqual("Sprites", results[0].Title);
    }

    [TestMethod]
    public void Search_FilterByFunction()
    {
        var results = _index.Search("collision", HelpTopicType.Function);
        Assert.IsTrue(results.All(r => r.Type == HelpTopicType.Function));
        Assert.AreEqual("COLLISION", results[0].Title);
    }

    [TestMethod]
    public void Search_EmptyQuery_ReturnsAll()
    {
        var results = _index.Search("");
        Assert.AreEqual(5, results.Count);
    }

    [TestMethod]
    public void Search_NoMatch_ReturnsEmpty()
    {
        var results = _index.Search("xyznonexistent");
        Assert.AreEqual(0, results.Count);
    }

    [TestMethod]
    public void Search_CaseInsensitive()
    {
        var results = _index.Search("LINE");
        Assert.AreEqual("LINE", results[0].Title);

        var results2 = _index.Search("line");
        Assert.AreEqual("LINE", results2[0].Title);
    }

    [TestMethod]
    public void Search_MultipleTerms()
    {
        var results = _index.Search("sprite collision");
        // COLLISION function should rank high (matches both terms)
        var titles = results.Select(r => r.Title).ToList();
        Assert.IsTrue(titles.Contains("COLLISION"));
    }

    [TestMethod]
    public void FindByTitle_ExactMatch()
    {
        var topic = _index.FindByTitle("LINE");
        Assert.IsNotNull(topic);
        Assert.AreEqual("LINE", topic.Title);
    }

    [TestMethod]
    public void FindByTitle_CaseInsensitive()
    {
        var topic = _index.FindByTitle("line");
        Assert.IsNotNull(topic);
        Assert.AreEqual("LINE", topic.Title);
    }

    [TestMethod]
    public void FindByTitle_NotFound_ReturnsNull()
    {
        var topic = _index.FindByTitle("NONEXISTENT");
        Assert.IsNull(topic);
    }

    [TestMethod]
    public void GetByCategory_ReturnsGrouped()
    {
        var groups = _index.GetByCategory();
        Assert.IsTrue(groups.ContainsKey("Graphics"));
        Assert.IsTrue(groups.ContainsKey("Sprites"));
        Assert.IsTrue(groups["Graphics"].Count >= 2); // LINE, PLOT
    }

    [TestMethod]
    public void TopicCount_IsCorrect()
    {
        Assert.AreEqual(5, _index.TopicCount);
    }
}
```

**Step 2: Run tests to verify they fail**

```bash
dotnet test --filter "FullyQualifiedName~HelpIndex" --verbosity normal
```

Expected: FAIL — `HelpIndex` doesn't exist.

**Step 3: Implement HelpIndex**

Create `e6502.Avalonia/Help/HelpIndex.cs`:

```csharp
using System;
using System.Collections.Generic;
using System.Linq;

namespace KDS.e6502.Help;

public sealed class HelpIndex
{
    private readonly List<HelpTopic> _topics;
    private readonly Dictionary<string, HelpTopic> _byTitle; // lowercased title -> topic

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

            // Title exact match
            if (titleLower == term)
                termScore += 100;
            // Title prefix match
            else if (titleLower.StartsWith(term, StringComparison.Ordinal))
                termScore += 80;
            // Title contains
            else if (titleLower.Contains(term, StringComparison.Ordinal))
                termScore += 60;

            // Keyword match
            if (topic.Keywords.Any(k => k.Contains(term, StringComparison.OrdinalIgnoreCase)))
                termScore += 60;

            // Syntax match
            if (topic.Syntax?.Contains(term, StringComparison.OrdinalIgnoreCase) == true)
                termScore += 40;

            // Body match (lower weight, boost by count)
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
        while ((index = text.IndexOf(term, index, StringComparison.Ordinal)) >= 0)
        {
            count++;
            index += term.Length;
        }
        return count;
    }
}
```

**Step 4: Run tests**

```bash
dotnet test --filter "FullyQualifiedName~HelpIndex" --verbosity normal
```

Expected: all pass.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Help/HelpIndex.cs e6502UnitTests/HelpIndexTests.cs
git commit -m "feat: add HelpIndex with scored search and category filtering"
```

---

### Task 4: HelpContentLoader — load from embedded resources

**Files:**
- Create: `e6502.Avalonia/Help/HelpContentLoader.cs`
- Create: `e6502UnitTests/HelpContentLoaderTests.cs`

**Step 1: Write failing test**

Create `e6502UnitTests/HelpContentLoaderTests.cs`:

```csharp
using KDS.e6502.Help;

namespace e6502UnitTests;

[TestClass]
public class HelpContentLoaderTests
{
    [TestMethod]
    public void LoadFromDirectory_FindsMarkdownFiles()
    {
        // Test loading from the docs/help/ directory on disk
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found — run from repo root");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        Assert.IsTrue(topics.Count > 0, "Should find at least one .md file");
    }

    [TestMethod]
    public void LoadFromDirectory_ParsesTopicsCorrectly()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        // Every topic should have a title
        Assert.IsTrue(topics.All(t => !string.IsNullOrEmpty(t.Title)));
        // Every topic should have a type
        Assert.IsTrue(topics.All(t => Enum.IsDefined(t.Type)));
    }

    [TestMethod]
    public void LoadFromDirectory_SetsCorrectSourcePaths()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        // Source paths should be relative (e.g., "reference/commands/line.md")
        Assert.IsTrue(topics.All(t => !Path.IsPathRooted(t.SourcePath)));
        Assert.IsTrue(topics.All(t => t.SourcePath.EndsWith(".md")));
    }

    private static string? FindHelpDirectory()
    {
        var dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            var candidate = Path.Combine(dir, "docs", "help");
            if (Directory.Exists(candidate))
                return candidate;
            dir = Directory.GetParent(dir)?.FullName;
        }
        return null;
    }
}
```

**Step 2: Run test to verify it fails**

```bash
dotnet test --filter "FullyQualifiedName~HelpContentLoader" --verbosity normal
```

**Step 3: Implement HelpContentLoader**

Create `e6502.Avalonia/Help/HelpContentLoader.cs`:

```csharp
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace KDS.e6502.Help;

public static class HelpContentLoader
{
    /// <summary>
    /// Load help topics from a directory of Markdown files.
    /// Used at runtime when loading from the docs/help/ directory.
    /// </summary>
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
```

**Step 4: Run tests**

```bash
dotnet test --filter "FullyQualifiedName~HelpContentLoader" --verbosity normal
```

Expected: all pass.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Help/HelpContentLoader.cs e6502UnitTests/HelpContentLoaderTests.cs
git commit -m "feat: add HelpContentLoader for loading Markdown files from disk"
```

---

## Phase 2: Help Panel UI

### Task 5: MarkdownRenderer — Markdown to Avalonia controls

This is the largest single component. It converts parsed Markdown into styled Avalonia `Control` trees.

**Files:**
- Create: `e6502.Avalonia/Help/MarkdownRenderer.cs`
- Create: `e6502.Avalonia/Help/HelpStyles.cs`

**Step 1: Create HelpStyles constants**

Create `e6502.Avalonia/Help/HelpStyles.cs`:

```csharp
using Avalonia.Media;

namespace KDS.e6502.Help;

/// <summary>
/// Visual style constants for the help panel.
/// Modern documentation aesthetic with clean typography.
/// </summary>
public static class HelpStyles
{
    // Colors
    public static readonly Color Background = Color.Parse("#FFFFFF");
    public static readonly Color TextPrimary = Color.Parse("#1A1A2E");
    public static readonly Color TextSecondary = Color.Parse("#6B7280");
    public static readonly Color AccentBlue = Color.Parse("#3B82F6");
    public static readonly Color AccentPurple = Color.Parse("#8B5CF6");
    public static readonly Color AccentTeal = Color.Parse("#14B8A6");
    public static readonly Color CodeBackground = Color.Parse("#F3F4F6");
    public static readonly Color CodeBorder = Color.Parse("#E5E7EB");
    public static readonly Color NoteBlue = Color.Parse("#DBEAFE");
    public static readonly Color NoteBorder = Color.Parse("#3B82F6");
    public static readonly Color WarningAmber = Color.Parse("#FEF3C7");
    public static readonly Color WarningBorder = Color.Parse("#F59E0B");
    public static readonly Color TipGreen = Color.Parse("#D1FAE5");
    public static readonly Color TipBorder = Color.Parse("#10B981");
    public static readonly Color TryItBackground = Color.Parse("#EDE9FE");
    public static readonly Color TryItBorder = Color.Parse("#8B5CF6");
    public static readonly Color TryItButton = Color.Parse("#7C3AED");
    public static readonly Color TableHeaderBg = Color.Parse("#F9FAFB");
    public static readonly Color TableBorder = Color.Parse("#E5E7EB");
    public static readonly Color TableAltRow = Color.Parse("#F9FAFB");
    public static readonly Color SearchBg = Color.Parse("#F9FAFB");
    public static readonly Color FilterActive = Color.Parse("#3B82F6");
    public static readonly Color FilterInactive = Color.Parse("#E5E7EB");
    public static readonly Color BadgeGuide = Color.Parse("#8B5CF6");
    public static readonly Color BadgeCommand = Color.Parse("#3B82F6");
    public static readonly Color BadgeFunction = Color.Parse("#14B8A6");
    public static readonly Color SeeAlsoPill = Color.Parse("#E0E7FF");

    // Typography
    public const string BodyFontFamily = "Inter, Segoe UI, -apple-system, sans-serif";
    public const string MonoFontFamily = "JetBrains Mono, Cascadia Code, Menlo, Consolas, monospace";
    public const double FontSizeH1 = 20;
    public const double FontSizeH2 = 16;
    public const double FontSizeH3 = 14;
    public const double FontSizeBody = 13;
    public const double FontSizeCode = 12;
    public const double FontSizeSmall = 11;
    public const double FontSizeBadge = 10;

    // Spacing
    public const double PanelPadding = 16;
    public const double SectionSpacing = 16;
    public const double ParagraphSpacing = 8;
    public const double CodeBlockPadding = 12;
    public const double CalloutPadding = 12;
    public const double CalloutBorderWidth = 3;
    public const double TableCellPadding = 8;
    public const double BadgePaddingH = 8;
    public const double BadgePaddingV = 2;
    public const double CornerRadius = 6;
}
```

**Step 2: Implement MarkdownRenderer**

Create `e6502.Avalonia/Help/MarkdownRenderer.cs`. This is a line-by-line Markdown parser that produces Avalonia controls. No external Markdown library — the subset we need is small enough to handle directly.

```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;

namespace KDS.e6502.Help;

public sealed class MarkdownRenderer
{
    public event Action<string>? TryThisClicked;
    public event Action<string>? LinkClicked;

    public Control Render(HelpTopic topic)
    {
        var stack = new StackPanel { Spacing = HelpStyles.SectionSpacing };

        // Title
        stack.Children.Add(CreateHeading(topic.Title, 1));

        // Syntax badge (for commands/functions)
        if (topic.Syntax != null)
            stack.Children.Add(CreateSyntaxBlock(topic.Syntax));

        // Body
        var bodyControls = RenderMarkdownBody(topic.Body);
        foreach (var control in bodyControls)
            stack.Children.Add(control);

        // See also
        if (topic.SeeAlso.Count > 0)
            stack.Children.Add(CreateSeeAlso(topic.SeeAlso));

        return stack;
    }

    private List<Control> RenderMarkdownBody(string body)
    {
        var controls = new List<Control>();
        var lines = body.Split('\n');
        var i = 0;

        while (i < lines.Length)
        {
            var line = lines[i];
            var trimmed = line.TrimStart();

            // Skip empty lines
            if (string.IsNullOrWhiteSpace(trimmed))
            {
                i++;
                continue;
            }

            // Fenced div (:::)
            if (trimmed.StartsWith(":::"))
            {
                var (control, newIndex) = ParseFencedDiv(lines, i);
                if (control != null)
                    controls.Add(control);
                i = newIndex;
                continue;
            }

            // Heading
            if (trimmed.StartsWith('#'))
            {
                var level = trimmed.TakeWhile(c => c == '#').Count();
                var text = trimmed[level..].Trim();
                controls.Add(CreateHeading(text, level));
                i++;
                continue;
            }

            // Code block
            if (trimmed.StartsWith("```"))
            {
                var (code, lang, newIndex) = ParseCodeBlock(lines, i);
                controls.Add(CreateCodeBlock(code, lang));
                i = newIndex;
                continue;
            }

            // Table
            if (trimmed.Contains('|') && trimmed.StartsWith('|'))
            {
                var (table, newIndex) = ParseTable(lines, i);
                if (table != null)
                    controls.Add(table);
                i = newIndex;
                continue;
            }

            // Unordered list item
            if (trimmed.StartsWith("- ") || trimmed.StartsWith("* "))
            {
                var (list, newIndex) = ParseList(lines, i);
                controls.Add(list);
                i = newIndex;
                continue;
            }

            // Paragraph (collect consecutive non-empty, non-special lines)
            var paraLines = new List<string>();
            while (i < lines.Length && !string.IsNullOrWhiteSpace(lines[i])
                && !lines[i].TrimStart().StartsWith('#')
                && !lines[i].TrimStart().StartsWith("```")
                && !lines[i].TrimStart().StartsWith(":::")
                && !(lines[i].TrimStart().StartsWith('|') && lines[i].Contains('|'))
                && !lines[i].TrimStart().StartsWith("- ")
                && !lines[i].TrimStart().StartsWith("* "))
            {
                paraLines.Add(lines[i].Trim());
                i++;
            }
            if (paraLines.Count > 0)
                controls.Add(CreateParagraph(string.Join(" ", paraLines)));
        }

        return controls;
    }

    private TextBlock CreateHeading(string text, int level)
    {
        var fontSize = level switch
        {
            1 => HelpStyles.FontSizeH1,
            2 => HelpStyles.FontSizeH2,
            3 => HelpStyles.FontSizeH3,
            _ => HelpStyles.FontSizeBody
        };

        return new TextBlock
        {
            Text = text,
            FontSize = fontSize,
            FontWeight = FontWeight.Bold,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            TextWrapping = TextWrapping.Wrap,
            Margin = new Thickness(0, level == 1 ? 0 : 8, 0, 4)
        };
    }

    private Border CreateSyntaxBlock(string syntax)
    {
        return new Border
        {
            Background = new SolidColorBrush(HelpStyles.CodeBackground),
            BorderBrush = new SolidColorBrush(HelpStyles.CodeBorder),
            BorderThickness = new Thickness(1),
            CornerRadius = new CornerRadius(HelpStyles.CornerRadius),
            Padding = new Thickness(HelpStyles.CodeBlockPadding),
            Child = new TextBlock
            {
                Text = syntax,
                FontFamily = new FontFamily(HelpStyles.MonoFontFamily),
                FontSize = HelpStyles.FontSizeCode + 1,
                FontWeight = FontWeight.SemiBold,
                Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
                TextWrapping = TextWrapping.Wrap
            }
        };
    }

    private TextBlock CreateParagraph(string text)
    {
        // TODO: handle inline code (`code`) and bold/italic via InlineCollection
        // For now, plain text
        return new TextBlock
        {
            Text = text,
            FontSize = HelpStyles.FontSizeBody,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            TextWrapping = TextWrapping.Wrap,
            LineHeight = 20
        };
    }

    private Border CreateCodeBlock(string code, string? language)
    {
        return new Border
        {
            Background = new SolidColorBrush(HelpStyles.CodeBackground),
            BorderBrush = new SolidColorBrush(HelpStyles.CodeBorder),
            BorderThickness = new Thickness(1),
            CornerRadius = new CornerRadius(HelpStyles.CornerRadius),
            Padding = new Thickness(HelpStyles.CodeBlockPadding),
            Child = new TextBlock
            {
                Text = code,
                FontFamily = new FontFamily(HelpStyles.MonoFontFamily),
                FontSize = HelpStyles.FontSizeCode,
                Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
                TextWrapping = TextWrapping.NoWrap
            }
        };
    }

    private (string code, string? language, int nextIndex) ParseCodeBlock(string[] lines, int startIndex)
    {
        var firstLine = lines[startIndex].TrimStart();
        var language = firstLine.Length > 3 ? firstLine[3..].Trim() : null;
        if (string.IsNullOrEmpty(language)) language = null;

        var sb = new StringBuilder();
        var i = startIndex + 1;
        while (i < lines.Length && !lines[i].TrimStart().StartsWith("```"))
        {
            if (sb.Length > 0) sb.Append('\n');
            sb.Append(lines[i]);
            i++;
        }
        if (i < lines.Length) i++; // skip closing ```
        return (sb.ToString(), language, i);
    }

    private (Control? control, int nextIndex) ParseFencedDiv(string[] lines, int startIndex)
    {
        var firstLine = lines[startIndex].TrimStart();
        var divType = firstLine[3..].Trim().ToLowerInvariant();

        if (string.IsNullOrEmpty(divType))
        {
            // This is the closing :::
            return (null, startIndex + 1);
        }

        // Collect content until closing :::
        var contentLines = new List<string>();
        var i = startIndex + 1;
        while (i < lines.Length && !lines[i].TrimStart().StartsWith(":::"))
        {
            contentLines.Add(lines[i]);
            i++;
        }
        if (i < lines.Length) i++; // skip closing :::

        var content = string.Join("\n", contentLines).Trim();

        return divType switch
        {
            "note" => (CreateCallout(content, "Note", HelpStyles.NoteBlue, HelpStyles.NoteBorder), i),
            "warning" => (CreateCallout(content, "Warning", HelpStyles.WarningAmber, HelpStyles.WarningBorder), i),
            "tip" => (CreateCallout(content, "Tip", HelpStyles.TipGreen, HelpStyles.TipBorder), i),
            "tryit" => (CreateTryItBlock(content), i),
            _ => (null, i)
        };
    }

    private Border CreateCallout(string content, string label, Color bgColor, Color borderColor)
    {
        var stack = new StackPanel { Spacing = 4 };
        stack.Children.Add(new TextBlock
        {
            Text = label,
            FontSize = HelpStyles.FontSizeSmall,
            FontWeight = FontWeight.Bold,
            Foreground = new SolidColorBrush(borderColor)
        });
        stack.Children.Add(new TextBlock
        {
            Text = content,
            FontSize = HelpStyles.FontSizeBody,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            TextWrapping = TextWrapping.Wrap,
            LineHeight = 20
        });

        return new Border
        {
            Background = new SolidColorBrush(bgColor),
            BorderBrush = new SolidColorBrush(borderColor),
            BorderThickness = new Thickness(HelpStyles.CalloutBorderWidth, 0, 0, 0),
            CornerRadius = new CornerRadius(0, HelpStyles.CornerRadius, HelpStyles.CornerRadius, 0),
            Padding = new Thickness(HelpStyles.CalloutPadding),
            Child = stack
        };
    }

    private Border CreateTryItBlock(string content)
    {
        // Extract code from the content (strip ```basic ... ```)
        var code = content;
        if (content.Contains("```"))
        {
            var codeLines = content.Split('\n');
            var codeContent = new List<string>();
            var inCode = false;
            foreach (var line in codeLines)
            {
                if (line.TrimStart().StartsWith("```"))
                {
                    inCode = !inCode;
                    continue;
                }
                if (inCode)
                    codeContent.Add(line);
            }
            code = string.Join("\n", codeContent);
        }

        var stack = new StackPanel { Spacing = 8 };

        // Code display
        stack.Children.Add(new TextBlock
        {
            Text = code,
            FontFamily = new FontFamily(HelpStyles.MonoFontFamily),
            FontSize = HelpStyles.FontSizeCode,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            TextWrapping = TextWrapping.NoWrap
        });

        // Try this! button
        var button = new Button
        {
            Content = "Try this!",
            FontSize = HelpStyles.FontSizeSmall,
            FontWeight = FontWeight.SemiBold,
            Background = new SolidColorBrush(HelpStyles.TryItButton),
            Foreground = Brushes.White,
            Padding = new Thickness(12, 6),
            CornerRadius = new CornerRadius(4),
            HorizontalAlignment = HorizontalAlignment.Left,
            Cursor = new Avalonia.Input.Cursor(Avalonia.Input.StandardCursorType.Hand)
        };
        var capturedCode = code;
        button.Click += (_, _) => TryThisClicked?.Invoke(capturedCode);
        stack.Children.Add(button);

        return new Border
        {
            Background = new SolidColorBrush(HelpStyles.TryItBackground),
            BorderBrush = new SolidColorBrush(HelpStyles.TryItBorder),
            BorderThickness = new Thickness(1),
            CornerRadius = new CornerRadius(HelpStyles.CornerRadius),
            Padding = new Thickness(HelpStyles.CodeBlockPadding),
            Child = stack
        };
    }

    private (Control? table, int nextIndex) ParseTable(string[] lines, int startIndex)
    {
        var rows = new List<string[]>();
        var i = startIndex;

        while (i < lines.Length && lines[i].TrimStart().StartsWith('|'))
        {
            var line = lines[i].Trim();
            // Skip separator row (|---|---|)
            if (line.Contains("---"))
            {
                i++;
                continue;
            }
            var cells = line.Split('|', StringSplitOptions.None)
                .Skip(1) // skip empty before first |
                .SkipLast(1) // skip empty after last |
                .Select(c => c.Trim())
                .ToArray();
            rows.Add(cells);
            i++;
        }

        if (rows.Count == 0)
            return (null, i);

        var grid = new Grid();
        var colCount = rows.Max(r => r.Length);

        for (var c = 0; c < colCount; c++)
            grid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));

        for (var r = 0; r < rows.Count; r++)
        {
            grid.RowDefinitions.Add(new RowDefinition(GridLength.Auto));
            for (var c = 0; c < rows[r].Length; c++)
            {
                var isHeader = r == 0;
                var isAltRow = r > 0 && r % 2 == 0;

                var cell = new Border
                {
                    Background = new SolidColorBrush(
                        isHeader ? HelpStyles.TableHeaderBg :
                        isAltRow ? HelpStyles.TableAltRow : Colors.Transparent),
                    BorderBrush = new SolidColorBrush(HelpStyles.TableBorder),
                    BorderThickness = new Thickness(0, 0, 0, 1),
                    Padding = new Thickness(HelpStyles.TableCellPadding),
                    Child = new TextBlock
                    {
                        Text = rows[r][c],
                        FontSize = HelpStyles.FontSizeBody,
                        FontWeight = isHeader ? FontWeight.SemiBold : FontWeight.Normal,
                        Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
                        TextWrapping = TextWrapping.Wrap
                    }
                };

                Grid.SetRow(cell, r);
                Grid.SetColumn(cell, c);
                grid.Children.Add(cell);
            }
        }

        return (new Border
        {
            BorderBrush = new SolidColorBrush(HelpStyles.TableBorder),
            BorderThickness = new Thickness(1),
            CornerRadius = new CornerRadius(HelpStyles.CornerRadius),
            ClipToBounds = true,
            Child = grid
        }, i);
    }

    private (StackPanel list, int nextIndex) ParseList(string[] lines, int startIndex)
    {
        var stack = new StackPanel { Spacing = 4 };
        var i = startIndex;

        while (i < lines.Length)
        {
            var trimmed = lines[i].TrimStart();
            if (!trimmed.StartsWith("- ") && !trimmed.StartsWith("* "))
                break;

            var text = trimmed[2..].Trim();
            var item = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 6
            };
            item.Children.Add(new TextBlock
            {
                Text = "\u2022",
                FontSize = HelpStyles.FontSizeBody,
                Foreground = new SolidColorBrush(HelpStyles.TextSecondary),
                VerticalAlignment = VerticalAlignment.Top
            });
            item.Children.Add(new TextBlock
            {
                Text = text,
                FontSize = HelpStyles.FontSizeBody,
                Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
                TextWrapping = TextWrapping.Wrap
            });
            stack.Children.Add(item);
            i++;
        }

        return (stack, i);
    }

    private Control CreateSeeAlso(IReadOnlyList<string> seeAlso)
    {
        var stack = new StackPanel { Spacing = 8 };
        stack.Children.Add(new TextBlock
        {
            Text = "See also",
            FontSize = HelpStyles.FontSizeSmall,
            FontWeight = FontWeight.SemiBold,
            Foreground = new SolidColorBrush(HelpStyles.TextSecondary),
            Margin = new Thickness(0, 8, 0, 0)
        });

        var wrap = new WrapPanel { Orientation = Orientation.Horizontal };
        foreach (var link in seeAlso)
        {
            var pill = new Button
            {
                Content = link.ToUpperInvariant(),
                FontSize = HelpStyles.FontSizeBadge,
                Background = new SolidColorBrush(HelpStyles.SeeAlsoPill),
                Foreground = new SolidColorBrush(HelpStyles.AccentBlue),
                BorderThickness = new Thickness(0),
                Padding = new Thickness(HelpStyles.BadgePaddingH, HelpStyles.BadgePaddingV),
                CornerRadius = new CornerRadius(10),
                Margin = new Thickness(0, 0, 4, 4),
                Cursor = new Avalonia.Input.Cursor(Avalonia.Input.StandardCursorType.Hand)
            };
            var capturedLink = link;
            pill.Click += (_, _) => LinkClicked?.Invoke(capturedLink);
            wrap.Children.Add(pill);
        }
        stack.Children.Add(wrap);
        return stack;
    }
}
```

**Step 3: Commit** (visual testing will be done when integrated into the panel)

```bash
git add e6502.Avalonia/Help/MarkdownRenderer.cs e6502.Avalonia/Help/HelpStyles.cs
git commit -m "feat: add MarkdownRenderer and HelpStyles for help panel"
```

---

### Task 6: HelpPanel UserControl

The main help panel view with search, category filters, browse/topic states.

**Files:**
- Create: `e6502.Avalonia/Help/HelpPanel.cs`

**Step 1: Implement HelpPanel**

Create `e6502.Avalonia/Help/HelpPanel.cs`:

```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Threading;

namespace KDS.e6502.Help;

public sealed class HelpPanel : UserControl
{
    private readonly HelpIndex _index;
    private readonly MarkdownRenderer _renderer;

    private readonly TextBox _searchBox;
    private readonly StackPanel _filterBar;
    private readonly ScrollViewer _contentScroll;
    private readonly StackPanel _contentArea;

    private HelpTopicType? _activeFilter;
    private HelpTopic? _currentTopic;
    private HelpTopic? _lastViewedTopic;
    private readonly List<Button> _filterButtons = [];

    public event Action? CloseRequested;
    public event Action<string>? TryThisRequested;

    public HelpPanel(HelpIndex index)
    {
        _index = index;
        _renderer = new MarkdownRenderer();
        _renderer.TryThisClicked += code => TryThisRequested?.Invoke(code);
        _renderer.LinkClicked += NavigateToTopic;

        Width = 350;
        MinWidth = 280;
        MaxWidth = 500;
        Background = new SolidColorBrush(HelpStyles.Background);

        // Build the UI
        var root = new DockPanel();

        // Header
        var header = CreateHeader();
        DockPanel.SetDock(header, Dock.Top);
        root.Children.Add(header);

        // Search bar
        _searchBox = CreateSearchBox();
        var searchContainer = new Border
        {
            Padding = new Thickness(HelpStyles.PanelPadding, 8),
            Child = _searchBox
        };
        DockPanel.SetDock(searchContainer, Dock.Top);
        root.Children.Add(searchContainer);

        // Filter tabs
        _filterBar = CreateFilterBar();
        var filterContainer = new Border
        {
            Padding = new Thickness(HelpStyles.PanelPadding, 0, HelpStyles.PanelPadding, 8),
            Child = _filterBar
        };
        DockPanel.SetDock(filterContainer, Dock.Top);
        root.Children.Add(filterContainer);

        // Content area
        _contentArea = new StackPanel { Spacing = HelpStyles.SectionSpacing };
        _contentScroll = new ScrollViewer
        {
            Content = _contentArea,
            Padding = new Thickness(HelpStyles.PanelPadding),
            HorizontalScrollBarVisibility = Avalonia.Controls.Primitives.ScrollBarVisibility.Disabled
        };
        root.Children.Add(_contentScroll);

        Content = root;

        // Initial state: browse view
        ShowBrowseView();
    }

    public void NavigateToTopic(string titleOrKeyword)
    {
        var topic = _index.FindByTitle(titleOrKeyword);
        if (topic != null)
        {
            ShowTopic(topic);
        }
        else
        {
            _searchBox.Text = titleOrKeyword;
            RunSearch(titleOrKeyword);
        }
    }

    public void ShowLastViewedOrBrowse()
    {
        if (_lastViewedTopic != null)
            ShowTopic(_lastViewedTopic);
        else
            ShowBrowseView();
    }

    private Border CreateHeader()
    {
        var panel = new DockPanel { Margin = new Thickness(HelpStyles.PanelPadding, 12, 8, 0) };

        var title = new TextBlock
        {
            Text = "Help",
            FontSize = HelpStyles.FontSizeH1,
            FontWeight = FontWeight.Bold,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            VerticalAlignment = VerticalAlignment.Center
        };
        DockPanel.SetDock(title, Dock.Left);
        panel.Children.Add(title);

        var closeBtn = new Button
        {
            Content = "\u2715",
            FontSize = 14,
            Background = Brushes.Transparent,
            Foreground = new SolidColorBrush(HelpStyles.TextSecondary),
            BorderThickness = new Thickness(0),
            Padding = new Thickness(8, 4),
            HorizontalAlignment = HorizontalAlignment.Right,
            VerticalAlignment = VerticalAlignment.Center,
            Cursor = new Cursor(StandardCursorType.Hand)
        };
        closeBtn.Click += (_, _) => CloseRequested?.Invoke();
        DockPanel.SetDock(closeBtn, Dock.Right);
        panel.Children.Add(closeBtn);

        return new Border { Child = panel };
    }

    private TextBox CreateSearchBox()
    {
        var box = new TextBox
        {
            Watermark = "Search docs...",
            FontSize = HelpStyles.FontSizeBody,
            Padding = new Thickness(10, 8),
            CornerRadius = new CornerRadius(HelpStyles.CornerRadius)
        };
        box.TextChanged += (_, _) =>
        {
            if (_currentTopic != null && !string.IsNullOrEmpty(box.Text))
            {
                // Switch back to search mode when typing
                _currentTopic = null;
            }
            RunSearch(box.Text ?? "");
        };
        return box;
    }

    private StackPanel CreateFilterBar()
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 6
        };

        var filters = new (string label, HelpTopicType? type)[]
        {
            ("All", null),
            ("Guides", HelpTopicType.Guide),
            ("Commands", HelpTopicType.Command),
            ("Functions", HelpTopicType.Function),
        };

        foreach (var (label, type) in filters)
        {
            var btn = new Button
            {
                Content = label,
                FontSize = HelpStyles.FontSizeSmall,
                Padding = new Thickness(10, 4),
                CornerRadius = new CornerRadius(12),
                BorderThickness = new Thickness(1),
                Cursor = new Cursor(StandardCursorType.Hand),
                Tag = type
            };
            btn.Click += (_, _) =>
            {
                _activeFilter = type;
                UpdateFilterHighlights();
                if (_currentTopic == null)
                    RunSearch(_searchBox.Text ?? "");
            };
            _filterButtons.Add(btn);
            panel.Children.Add(btn);
        }

        _activeFilter = null;
        UpdateFilterHighlights();
        return panel;
    }

    private void UpdateFilterHighlights()
    {
        foreach (var btn in _filterButtons)
        {
            var isActive = Equals(btn.Tag, _activeFilter) ||
                           (btn.Tag == null && _activeFilter == null);
            btn.Background = new SolidColorBrush(
                isActive ? HelpStyles.FilterActive : HelpStyles.FilterInactive);
            btn.Foreground = new SolidColorBrush(
                isActive ? Colors.White : HelpStyles.TextPrimary);
            btn.BorderBrush = new SolidColorBrush(
                isActive ? HelpStyles.FilterActive : HelpStyles.FilterInactive);
        }
    }

    private void RunSearch(string query)
    {
        _contentArea.Children.Clear();

        if (string.IsNullOrWhiteSpace(query) && _activeFilter == null)
        {
            ShowBrowseView();
            return;
        }

        var results = _index.Search(query, _activeFilter);

        if (results.Count == 0)
        {
            _contentArea.Children.Add(new TextBlock
            {
                Text = "No results found.",
                FontSize = HelpStyles.FontSizeBody,
                Foreground = new SolidColorBrush(HelpStyles.TextSecondary),
                Margin = new Thickness(0, 20)
            });
            return;
        }

        foreach (var topic in results)
        {
            _contentArea.Children.Add(CreateSearchResult(topic));
        }
    }

    private Control CreateSearchResult(HelpTopic topic)
    {
        var panel = new StackPanel { Spacing = 2 };

        // Title row with badge
        var titleRow = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 6 };
        titleRow.Children.Add(CreateTypeBadge(topic.Type));
        titleRow.Children.Add(new TextBlock
        {
            Text = topic.Title,
            FontSize = HelpStyles.FontSizeBody,
            FontWeight = FontWeight.SemiBold,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary)
        });
        panel.Children.Add(titleRow);

        // Preview (first line of body)
        var preview = topic.Body.Split('\n')
            .FirstOrDefault(l => !string.IsNullOrWhiteSpace(l) && !l.TrimStart().StartsWith('#'))
            ?? "";
        if (preview.Length > 80)
            preview = preview[..77] + "...";

        panel.Children.Add(new TextBlock
        {
            Text = preview,
            FontSize = HelpStyles.FontSizeSmall,
            Foreground = new SolidColorBrush(HelpStyles.TextSecondary),
            TextWrapping = TextWrapping.NoWrap,
            TextTrimming = TextTrimming.CharacterEllipsis
        });

        var border = new Border
        {
            Child = panel,
            Padding = new Thickness(10, 8),
            CornerRadius = new CornerRadius(4),
            Margin = new Thickness(0, 0, 0, 2),
            Cursor = new Cursor(StandardCursorType.Hand)
        };

        border.PointerEntered += (_, _) =>
            border.Background = new SolidColorBrush(HelpStyles.SearchBg);
        border.PointerExited += (_, _) =>
            border.Background = Brushes.Transparent;
        border.PointerPressed += (_, _) => ShowTopic(topic);

        return border;
    }

    private Border CreateTypeBadge(HelpTopicType type)
    {
        var (label, color) = type switch
        {
            HelpTopicType.Guide => ("Guide", HelpStyles.BadgeGuide),
            HelpTopicType.Command => ("Cmd", HelpStyles.BadgeCommand),
            HelpTopicType.Function => ("Fn", HelpStyles.BadgeFunction),
            _ => ("?", HelpStyles.TextSecondary)
        };

        return new Border
        {
            Background = new SolidColorBrush(color),
            CornerRadius = new CornerRadius(8),
            Padding = new Thickness(HelpStyles.BadgePaddingH, HelpStyles.BadgePaddingV),
            Child = new TextBlock
            {
                Text = label,
                FontSize = HelpStyles.FontSizeBadge,
                FontWeight = FontWeight.SemiBold,
                Foreground = Brushes.White
            }
        };
    }

    private void ShowTopic(HelpTopic topic)
    {
        _currentTopic = topic;
        _lastViewedTopic = topic;
        _contentArea.Children.Clear();
        _contentScroll.Offset = new Vector(0, 0);

        // Back button
        var backBtn = new Button
        {
            Content = "\u2190 Back",
            FontSize = HelpStyles.FontSizeSmall,
            Background = Brushes.Transparent,
            Foreground = new SolidColorBrush(HelpStyles.AccentBlue),
            BorderThickness = new Thickness(0),
            Padding = new Thickness(0, 4),
            Cursor = new Cursor(StandardCursorType.Hand),
            Margin = new Thickness(0, 0, 0, 8)
        };
        backBtn.Click += (_, _) =>
        {
            _currentTopic = null;
            RunSearch(_searchBox.Text ?? "");
        };
        _contentArea.Children.Add(backBtn);

        // Rendered topic
        var rendered = _renderer.Render(topic);
        _contentArea.Children.Add(rendered);
    }

    private void ShowBrowseView()
    {
        _contentArea.Children.Clear();

        var groups = _index.GetByCategory();
        foreach (var (category, topics) in groups.OrderBy(g => g.Key))
        {
            _contentArea.Children.Add(new TextBlock
            {
                Text = category,
                FontSize = HelpStyles.FontSizeH3,
                FontWeight = FontWeight.Bold,
                Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
                Margin = new Thickness(0, 8, 0, 4)
            });

            foreach (var topic in topics)
            {
                _contentArea.Children.Add(CreateSearchResult(topic));
            }
        }
    }
}
```

**Step 2: Commit**

```bash
git add e6502.Avalonia/Help/HelpPanel.cs
git commit -m "feat: add HelpPanel UserControl with search, browse, and topic views"
```

---

### Task 7: MainWindow integration — panel management, F1, layout

**Files:**
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`

**Step 1: Add help register constants to VgcConstants.cs**

At the end of the register definitions (near line 452, after `RegIrqCtrl`), add:

```csharp
// Help system registers ($A020-$A030)
public const ushort RegHelp = 0xA020;          // Help command: 1=open, 2=search
public const ushort HelpSearchBuffer = 0xA021; // 16-byte buffer for search term ($A021-$A030)
public const int HelpSearchBufferLen = 16;
```

**Step 2: Add help panel fields and methods to MainWindow.axaml.cs**

Add fields (near line 23, with other fields):

```csharp
private HelpPanel? _helpPanel;
private HelpIndex? _helpIndex;
private Grid? _rootGrid;
private GridSplitter? _helpSplitter;
private bool _helpVisible;
```

Add initialization in the constructor or `OnOpened` (after `_bus` is created, where the emulator is set up):

```csharp
private void InitializeHelp()
{
    // Find docs/help/ directory relative to the executable
    var helpDir = FindHelpDirectory();
    if (helpDir == null) return;

    var topics = HelpContentLoader.LoadFromDirectory(helpDir);
    _helpIndex = new HelpIndex(topics);
}

private static string? FindHelpDirectory()
{
    // Walk up from the executable to find docs/help/
    var dir = AppDomain.CurrentDomain.BaseDirectory;
    for (var i = 0; i < 6; i++)
    {
        var candidate = Path.Combine(dir, "docs", "help");
        if (Directory.Exists(candidate)) return candidate;
        var parent = Directory.GetParent(dir)?.FullName;
        if (parent == null) break;
        dir = parent;
    }
    return null;
}
```

Add help panel toggle methods:

```csharp
private void ShowHelpPanel(string? searchTerm = null)
{
    if (_helpIndex == null) return;

    if (_helpPanel == null)
    {
        _helpPanel = new HelpPanel(_helpIndex);
        _helpPanel.CloseRequested += HideHelpPanel;
        _helpPanel.TryThisRequested += OnTryThisRequested;
    }

    if (!_helpVisible)
    {
        // Wrap current content in a Grid with: HelpPanel | Splitter | Content
        var currentContent = Content as Control;
        if (currentContent == null) return;

        Content = null;

        _rootGrid = new Grid();
        _rootGrid.ColumnDefinitions.Add(new ColumnDefinition(350, GridUnitType.Pixel) { MinWidth = 280, MaxWidth = 500 });
        _rootGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
        _rootGrid.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Star));

        Grid.SetColumn(_helpPanel, 0);
        _rootGrid.Children.Add(_helpPanel);

        _helpSplitter = new GridSplitter
        {
            Width = 4,
            Background = new SolidColorBrush(HelpStyles.CodeBorder)
        };
        Grid.SetColumn(_helpSplitter, 1);
        _rootGrid.Children.Add(_helpSplitter);

        if (currentContent.Parent is Panel p)
            p.Children.Remove(currentContent);
        else
            App.DetachFromVisualParent(currentContent);

        Grid.SetColumn(currentContent, 2);
        _rootGrid.Children.Add(currentContent);

        Content = _rootGrid;
        _helpVisible = true;

        // Expand window width to accommodate panel
        Width += 354;
    }

    if (searchTerm != null)
        _helpPanel.NavigateToTopic(searchTerm);
    else
        _helpPanel.ShowLastViewedOrBrowse();
}

private void HideHelpPanel()
{
    if (!_helpVisible || _rootGrid == null) return;

    // Extract the emulator content (column 2)
    var emulatorContent = _rootGrid.Children
        .OfType<Control>()
        .FirstOrDefault(c => Grid.GetColumn(c) == 2);

    if (emulatorContent != null)
    {
        _rootGrid.Children.Remove(emulatorContent);
        Content = emulatorContent;
    }

    _helpVisible = false;
    Width -= 354;
}

private void ToggleHelpPanel()
{
    if (_helpVisible)
        HideHelpPanel();
    else
        ShowHelpPanel();
}
```

**Step 3: Wire F1 key in `OnKeyDown`**

In the `OnKeyDown` method (around line 191), add before the existing key checks:

```csharp
if (e.Key == Key.F1)
{
    ToggleHelpPanel();
    e.Handled = true;
    return;
}
```

**Step 4: Wire Escape to close help panel**

In `OnKeyDown`, add:

```csharp
if (e.Key == Key.Escape && _helpVisible)
{
    HideHelpPanel();
    e.Handled = true;
    return;
}
```

**Step 5: Wire the "Try this!" handler**

```csharp
private async void OnTryThisRequested(string code)
{
    // Check if a BASIC program is in memory
    // BASIC program start pointer is at $0280 (Smeml) and end at $0282 (Svarl)
    var progStart = (ushort)(_bus.Read(0x002B) | (_bus.Read(0x002C) << 8));
    var progEnd = (ushort)(_bus.Read(0x002D) | (_bus.Read(0x002E) << 8));

    if (progEnd > progStart)
    {
        // There's a program in memory — confirm
        var dialog = new Window
        {
            Title = "Replace Program?",
            Width = 340,
            Height = 140,
            WindowStartupLocation = WindowStartupLocation.CenterOwner,
            CanResize = false
        };

        var result = false;
        var stack = new StackPanel
        {
            Spacing = 12,
            Margin = new Thickness(20)
        };
        stack.Children.Add(new TextBlock
        {
            Text = "This will replace your current program. Continue?",
            TextWrapping = TextWrapping.Wrap,
            FontSize = 13
        });
        var btnPanel = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 8, HorizontalAlignment = HorizontalAlignment.Right };
        var cancelBtn = new Button { Content = "Cancel", Padding = new Thickness(16, 6) };
        cancelBtn.Click += (_, _) => dialog.Close();
        var okBtn = new Button { Content = "Replace & Enter", Padding = new Thickness(16, 6) };
        okBtn.Click += (_, _) => { result = true; dialog.Close(); };
        btnPanel.Children.Add(cancelBtn);
        btnPanel.Children.Add(okBtn);
        stack.Children.Add(btnPanel);
        dialog.Content = stack;

        await dialog.ShowDialog(this);
        if (!result) return;
    }

    // Enter the code
    EnterCodeIntoBasic(code);
}

private void EnterCodeIntoBasic(string code)
{
    var editor = _bus.ScreenEditor;

    // Type NEW + Enter
    foreach (var c in "NEW")
        editor.QueueInput((byte)c);
    editor.QueueInput(0x0D); // Enter

    // Small delay then enter each line
    DispatcherTimer.RunOnce(() =>
    {
        var lines = code.Split('\n', StringSplitOptions.RemoveEmptyEntries);
        var delay = TimeSpan.FromMilliseconds(50);

        for (var i = 0; i < lines.Length; i++)
        {
            var line = lines[i].TrimEnd();
            var capturedLine = line;
            DispatcherTimer.RunOnce(() =>
            {
                // Uppercase outside quotes (same as MCP EnterBasicLine)
                var upper = ConvertToUpperOutsideQuotes(capturedLine);
                foreach (var c in upper)
                    editor.QueueInput((byte)c);
                editor.QueueInput(0x0D);
            }, delay * (i + 1));
        }
    }, TimeSpan.FromMilliseconds(100));
}

private static string ConvertToUpperOutsideQuotes(string text)
{
    var sb = new System.Text.StringBuilder(text.Length);
    var inQuotes = false;
    foreach (var c in text)
    {
        if (c == '"') inQuotes = !inQuotes;
        sb.Append(!inQuotes && c >= 'a' && c <= 'z' ? (char)(c - 32) : c);
    }
    return sb.ToString();
}
```

**Step 6: Run all tests to verify no regressions**

```bash
dotnet test --verbosity normal
```

**Step 7: Commit**

```bash
git add e6502.Avalonia/MainWindow.axaml.cs e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: integrate help panel into MainWindow with F1 toggle and Try This"
```

---

## Phase 3: BASIC ROM Integration

### Task 8: Add HELP token to EhBASIC ROM

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add XTK_HELP constant**

Near line 514, after `XTK_BITTGL = $45`, add:

```asm
XTK_HELP           = $46              ; HELP [keyword]
```

**Step 2: Bump XTK_COUNT**

At line 8409, change:

```asm
XTK_COUNT = 70
```

**Step 3: Add to keyword string table**

In `TAB_XTKSTR` (around line 8425), add the word pointer:

```asm
      .word @s_help
```

At the end of the string definitions (after `@s_bittgl`), add:

```asm
@s_help:   .byte "HELP",0
```

**Step 4: Add to dispatch table**

In `TAB_XTKCMD` (around line 1909), after the `LAB_BITTGL-1` entry, add:

```asm
      .word LAB_HELP-1         ; XTK_HELP ($46)
```

**Step 5: Write the HELP handler**

Add the handler near the other extended token handlers. The `HELP` command:
- With no argument: writes `$01` to `$A020` (RegHelp)
- With argument: writes the keyword string to `$A021-$A030`, then writes `$02` to `$A020`

```asm
;; ========================================
;; HELP [keyword]
;; Opens the help panel, optionally searching for a keyword
;; ========================================
LAB_HELP
      JSR   LAB_GBYT          ; peek at next byte
      BEQ   @no_arg           ; end of statement? open general help

      ; There's an argument — evaluate as string expression
      JSR   LAB_EVEX          ; evaluate expression
      JSR   LAB_EVST          ; pop string: A=length, ut1_pl/ph=pointer

      ; Copy string to help buffer ($A021-$A030), max 16 chars
      TAY                     ; Y = length
      CPY   #$10
      BCC   @len_ok
      LDY   #$10              ; clamp to 16
@len_ok
      STY   @help_len         ; save length
      DEY
@copy_loop
      BMI   @copy_done
      LDA   (ut1_pl),Y
      STA   $A021,Y
      DEY
      BPL   @copy_loop

@copy_done
      ; Zero-fill remainder of buffer
      LDY   @help_len
@zero_loop
      CPY   #$10
      BCS   @trigger_search
      LDA   #$00
      STA   $A021,Y
      INY
      BNE   @zero_loop

@trigger_search
      LDA   #$02              ; command: search
      STA   $A020             ; RegHelp
      RTS

@no_arg
      LDA   #$01              ; command: open
      STA   $A020             ; RegHelp
      RTS

@help_len
      .byte $00
```

**Step 6: Assemble and copy ROM**

```bash
cd ehbasic && make clean && make && cp basic.bin ../e6502.Avalonia/Resources/ehbasic.bin && cd ..
```

**Step 7: Commit**

```bash
git add ehbasic/
git commit -m "feat: add HELP extended token to EhBASIC ROM"
```

---

### Task 9: CompositeBusDevice — intercept help register

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`

**Step 1: Add help request event and interception**

Add a field/event to `CompositeBusDevice`:

```csharp
public event Action<string?>? HelpRequested;
```

In the `Write` method, add an interception **before** the VGC check (before line 144 where `_vgc.OwnsAddress` is checked):

```csharp
// Help system register
if (address == VgcConstants.RegHelp)
{
    if (data == 0x01)
    {
        HelpRequested?.Invoke(null);
    }
    else if (data == 0x02)
    {
        // Read search string from buffer
        var sb = new System.Text.StringBuilder();
        for (var i = 0; i < VgcConstants.HelpSearchBufferLen; i++)
        {
            var ch = _ram[VgcConstants.HelpSearchBuffer + i];
            if (ch == 0) break;
            sb.Append((char)ch);
        }
        HelpRequested?.Invoke(sb.ToString());
    }
    return;
}
```

**Step 2: Wire event in MainWindow**

In `MainWindow.axaml.cs`, where `_bus` is created/initialized, add:

```csharp
_bus.HelpRequested += searchTerm =>
{
    Dispatcher.UIThread.Post(() => ShowHelpPanel(searchTerm));
};
```

**Step 3: Run tests**

```bash
dotnet test --verbosity normal
```

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: intercept help register writes and wire to help panel"
```

---

## Phase 4: Build Pipeline

### Task 10: PDF generation with Pandoc

**Files:**
- Create: `docs/help/build-pdf.sh`
- Create: `docs/help/latex-template.tex`
- Create: `docs/help/nova-filter.lua`
- Create: `docs/help/book.yaml`

**Step 1: Create the book manifest**

Create `docs/help/book.yaml` matching the chapter structure from the design doc. This defines the order chapters appear in the PDF.

**Step 2: Create the Pandoc Lua filter**

Create `docs/help/nova-filter.lua`:

```lua
-- nova-filter.lua
-- Pandoc Lua filter for NovaBASIC user guide
-- Maps fenced divs to LaTeX tcolorbox environments

function Div(el)
  if el.classes:includes("note") then
    return pandoc.RawBlock("latex",
      "\\begin{notebox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{notebox}")
  elseif el.classes:includes("warning") then
    return pandoc.RawBlock("latex",
      "\\begin{warningbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{warningbox}")
  elseif el.classes:includes("tip") then
    return pandoc.RawBlock("latex",
      "\\begin{tipbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{tipbox}")
  elseif el.classes:includes("tryit") then
    return pandoc.RawBlock("latex",
      "\\begin{tryitbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{tryitbox}")
  end
end

function CodeBlock(el)
  if el.classes:includes("basic") then
    return pandoc.RawBlock("latex",
      "\\begin{lstlisting}[language=NovaBASIC]\n" ..
      el.text ..
      "\n\\end{lstlisting}")
  end
end
```

**Step 3: Create the LaTeX template**

Extract the preamble, styling, and environment definitions from the existing `docs/manual/novabasic_user_guide.tex` into `docs/help/latex-template.tex` as a Pandoc template. This preserves all current visual styling.

**Step 4: Create the build script**

Create `docs/help/build-pdf.sh`:

```bash
#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
HELP_DIR="$SCRIPT_DIR"
OUTPUT_DIR="$REPO_ROOT/docs/manual"

# Check required tools
for tool in pandoc latexmk pdflatex; do
    if ! command -v "$tool" &>/dev/null; then
        echo "ERROR: $tool is required but not installed."
        echo "Install with: brew install $tool (macOS) or see documentation."
        exit 1
    fi
done

# Read book.yaml and assemble chapters
# (uses a Python helper for YAML parsing — Python is available on all dev machines)
python3 "$SCRIPT_DIR/assemble-book.py" \
    --manifest "$HELP_DIR/book.yaml" \
    --help-dir "$HELP_DIR" \
    --output "$OUTPUT_DIR/assembled.md"

# Run Pandoc
pandoc "$OUTPUT_DIR/assembled.md" \
    --template="$HELP_DIR/latex-template.tex" \
    --lua-filter="$HELP_DIR/nova-filter.lua" \
    --pdf-engine=latexmk \
    --pdf-engine-opt="-pdf" \
    -o "$OUTPUT_DIR/novabasic_user_guide.pdf"

# Clean up intermediate
rm -f "$OUTPUT_DIR/assembled.md"

echo "PDF generated: $OUTPUT_DIR/novabasic_user_guide.pdf"
```

```bash
chmod +x docs/help/build-pdf.sh
```

**Step 5: Create the assembly script**

Create `docs/help/assemble-book.py`:

```python
#!/usr/bin/env python3
"""Assemble Markdown help files into a single document for Pandoc."""

import argparse
import os
import glob
import yaml

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--help-dir", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    with open(args.manifest) as f:
        book = yaml.safe_load(f)

    parts = []
    parts.append(f"---\ntitle: \"{book.get('title', 'NovaBASIC User Guide')}\"\n---\n")

    # Chapters
    for chapter in book.get("chapters", []):
        parts.append(f"\n# {chapter['title']}\n")
        for src in chapter.get("sources", []):
            path = os.path.join(args.help_dir, src)
            parts.append(read_body(path))

    # Appendices
    for appendix in book.get("appendices", []):
        parts.append(f"\n# {appendix['title']}\n")
        if "auto_generate" in appendix:
            kind = appendix["auto_generate"]
            pattern = os.path.join(args.help_dir, "reference", kind, "*.md")
            files = sorted(glob.glob(pattern))
            for fpath in files:
                parts.append(read_body(fpath))
                parts.append("\n---\n")
        else:
            for src in appendix.get("sources", []):
                path = os.path.join(args.help_dir, src)
                parts.append(read_body(path))

    with open(args.output, "w") as f:
        f.write("\n".join(parts))

def read_body(path):
    """Read a Markdown file, stripping YAML frontmatter."""
    with open(path) as f:
        content = f.read()
    if content.startswith("---"):
        end = content.index("---", 3)
        return content[end + 3:].strip()
    return content.strip()

if __name__ == "__main__":
    main()
```

**Step 6: Commit**

```bash
git add docs/help/build-pdf.sh docs/help/nova-filter.lua docs/help/book.yaml docs/help/assemble-book.py
git commit -m "feat: add Pandoc PDF generation pipeline for help content"
```

---

### Task 11: Unified MSBuild — single command builds everything

**Files:**
- Modify: `e6502.Avalonia/e6502.Avalonia.csproj`

**Step 1: Update csproj with unified build targets**

Replace the existing `BuildEhBasic` target and add new targets. The existing target (lines 27-31) becomes part of a larger chain:

```xml
<!-- Validate required tools -->
<Target Name="ValidateTools" BeforeTargets="AssembleRom">
  <Exec Command="command -v ca65 || (echo 'ERROR: ca65 not found. Install cc65: brew install cc65' &amp;&amp; exit 1)"
        StandardOutputImportance="low" />
  <Exec Command="command -v ld65 || (echo 'ERROR: ld65 not found. Install cc65: brew install cc65' &amp;&amp; exit 1)"
        StandardOutputImportance="low" />
  <Exec Command="command -v pandoc || (echo 'ERROR: pandoc not found. Install: brew install pandoc' &amp;&amp; exit 1)"
        StandardOutputImportance="low" />
  <Exec Command="command -v latexmk || (echo 'ERROR: latexmk not found. Install MacTeX or texlive' &amp;&amp; exit 1)"
        StandardOutputImportance="low" />
</Target>

<!-- Assemble EhBASIC ROM -->
<Target Name="AssembleRom" BeforeTargets="PreBuildEvent">
  <Exec Command="make -C ../ehbasic" />
  <Copy SourceFiles="../ehbasic/basic.bin" DestinationFiles="Resources/ehbasic.bin" />
</Target>

<!-- Generate PDF user guide -->
<Target Name="GeneratePdf" BeforeTargets="PreBuildEvent" DependsOnTargets="AssembleRom">
  <Exec Command="bash ../docs/help/build-pdf.sh" />
</Target>
```

Add the help content as items that get copied to the output directory so `HelpContentLoader` can find them:

```xml
<!-- Help content files -->
<ItemGroup>
  <Content Include="../docs/help/**/*.md" Link="help/%(RecursiveDir)%(Filename)%(Extension)">
    <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
  </Content>
  <Content Include="../docs/help/book.yaml" Link="help/book.yaml">
    <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
  </Content>
</ItemGroup>
```

**Step 2: Update HelpContentLoader to find help files in the output directory**

Update `FindHelpDirectory` in MainWindow to also check the output directory:

```csharp
private static string? FindHelpDirectory()
{
    // Check next to the executable first (deployed/output directory)
    var exeDir = AppDomain.CurrentDomain.BaseDirectory;
    var candidate = Path.Combine(exeDir, "help");
    if (Directory.Exists(candidate)) return candidate;

    // Walk up from executable to find docs/help/ (development)
    var dir = exeDir;
    for (var i = 0; i < 6; i++)
    {
        candidate = Path.Combine(dir, "docs", "help");
        if (Directory.Exists(candidate)) return candidate;
        var parent = Directory.GetParent(dir)?.FullName;
        if (parent == null) break;
        dir = parent;
    }
    return null;
}
```

**Step 3: Verify build**

```bash
dotnet build
```

**Step 4: Run all tests**

```bash
dotnet test --verbosity normal
```

**Step 5: Commit**

```bash
git add e6502.Avalonia/e6502.Avalonia.csproj e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: unified build — ROM assembly, PDF generation, help content in one command"
```

---

## Phase 5: Content Migration

### Task 12: Convert LaTeX documentation to Markdown

This is the largest content task. Convert all existing LaTeX chapters to Markdown with frontmatter.

**Files:**
- Create: `tools/tex2md.py` (conversion script)
- Create: All Markdown files in `docs/help/guides/` and `docs/help/reference/`

**Step 1: Write the conversion script**

Create `tools/tex2md.py` that:
1. Reads each `.tex` file from `docs/manual/chapters/`
2. Strips LaTeX commands and converts to Markdown
3. Converts `\begin{lstlisting}` to ` ```basic ` code fences
4. Converts `\begin{notebox}` etc. to `::: note` fenced divs
5. Converts `\begin{longtable}` to Markdown tables
6. Splits the command reference appendix into individual command files with frontmatter
7. Splits the function reference into individual function files with frontmatter
8. Outputs guide files with appropriate frontmatter

This script handles the mechanical conversion; hand-editing follows.

**Step 2: Run conversion**

```bash
python3 tools/tex2md.py
```

**Step 3: Hand-edit for quality**

Review each generated file:
- Verify frontmatter metadata is correct (title, type, category, keywords, see_also)
- Fix any conversion artifacts
- Ensure code examples are properly fenced
- Verify fenced divs are correctly formatted
- Add `syntax` field to all command/function reference files

**Step 4: Verify PDF generation**

```bash
bash docs/help/build-pdf.sh
```

Compare generated PDF against the current one to verify no content loss.

**Step 5: Verify help content loads**

```bash
dotnet build && dotnet run --project e6502.Avalonia
```

Press F1, verify all topics appear and are searchable.

**Step 6: Commit**

```bash
git add docs/help/ tools/tex2md.py
git commit -m "feat: migrate LaTeX documentation to canonical Markdown format"
```

---

### Task 13: Remove old LaTeX source files

Only after verifying the new pipeline produces equivalent output.

**Files:**
- Delete: `docs/manual/chapters/*.tex`
- Delete: `docs/manual/novabasic_user_guide.tex`
- Delete: `docs/manual/Makefile`
- Keep: `docs/manual/` directory for generated PDF output

**Step 1: Verify new pipeline**

```bash
dotnet build
# Visually compare docs/manual/novabasic_user_guide.pdf with the old version
```

**Step 2: Remove old files**

```bash
git rm docs/manual/chapters/*.tex docs/manual/novabasic_user_guide.tex docs/manual/Makefile
```

**Step 3: Commit**

```bash
git commit -m "chore: remove old LaTeX source files, replaced by canonical Markdown"
```

---

## Testing Checklist

After all tasks are complete, verify end-to-end:

1. **Build:** `dotnet build` succeeds (ROM + PDF + help content)
2. **Tests:** `dotnet test` all pass
3. **F1 toggle:** Press F1, help panel appears on left. Press F1 again, it closes.
4. **Search:** Type "sprite" in search box, verify SPRITE command, Sprites guide, and related results appear.
5. **Category filters:** Click "Commands", verify only commands shown. Click "All" to reset.
6. **Topic view:** Click a search result, verify topic renders with headings, tables, code blocks, callout boxes.
7. **See also:** Click a "see also" pill, verify navigation to linked topic.
8. **Try this!:** Click "Try this!" on an example with no program in memory — code should be entered. With a program in memory — confirmation dialog should appear.
9. **HELP command:** In BASIC, type `HELP` — panel opens. Type `HELP LINE` — panel opens to LINE topic.
10. **Escape:** Press Escape while help panel is visible — panel closes.
11. **Dev mode coexistence:** Open help panel (F1), then enter dev mode (Ctrl+D) — layout should be Help | Emulator | Sprite Editor.
12. **PDF quality:** Open generated PDF, compare styling against previous version — tcolorbox environments, code listings, colors, typography should match.
