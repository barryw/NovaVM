using System.Linq;
using e6502.Avalonia.Help;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
    public void Parse_ProgramType()
    {
        var md = """
            ---
            title: Reversi
            type: program
            category: Programs
            keywords: [reversi, board game]
            ---

            How to play Reversi.
            """;
        var topic = HelpTopic.Parse(md, "programs/reversi.md");
        Assert.AreEqual(HelpTopicType.Program, topic.Type);
        Assert.AreEqual("Programs", topic.Category);
        Assert.AreEqual("Reversi", topic.Title);
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
