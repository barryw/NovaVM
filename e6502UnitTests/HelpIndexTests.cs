using System.Linq;
using e6502.Avalonia.Help;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
        Assert.IsTrue(groups["Graphics"].Count >= 2);
    }

    [TestMethod]
    public void TopicCount_IsCorrect()
    {
        Assert.AreEqual(5, _index.TopicCount);
    }
}
