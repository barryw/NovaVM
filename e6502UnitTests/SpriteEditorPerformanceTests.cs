using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Headless;
using Avalonia.Media.Imaging;
using e6502.Avalonia.DevTools;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Regression tests for sprite editor performance fixes.
/// These verify structural choices that prevent beachball during window resize/maximize.
/// </summary>
[TestClass]
public class SpriteEditorPerformanceTests
{
    private static bool _platformReady;

    [ClassInitialize]
    public static void InitAvalonia(TestContext _)
    {
        if (_platformReady) return;
        AppBuilder.Configure<Application>()
            .UseHeadless(new AvaloniaHeadlessPlatformOptions())
            .SetupWithoutStarting();
        _platformReady = true;
    }

    private static SpriteEditorView CreateView() => new(new VirtualGraphicsController());

    /// <summary>
    /// The pixel grid must use a single custom control (PixelGridSurface), not 256 Border controls.
    /// 256 controls cause expensive layout/compositor work during resize.
    /// </summary>
    [TestMethod]
    public void PixelGrid_UsesSingleControl_Not256Borders()
    {
        var view = CreateView();

        // Content -> ScrollViewer -> StackPanel -> find the Canvas (pixel grid)
        var scrollViewer = (ScrollViewer)view.Content!;
        var stackPanel = (StackPanel)scrollViewer.Content!;

        Canvas? pixelGrid = null;
        foreach (var child in stackPanel.Children)
        {
            if (child is Canvas c)
            {
                pixelGrid = c;
                break;
            }
        }

        Assert.IsNotNull(pixelGrid, "Pixel grid Canvas not found");
        Assert.AreEqual(1, pixelGrid.Children.Count,
            "Pixel grid should have exactly 1 child (PixelGridSurface), not 256 Borders");
        Assert.AreEqual("PixelGridSurface", pixelGrid.Children[0].GetType().Name,
            "Pixel grid child should be a PixelGridSurface using DrawingContext");
    }

    /// <summary>
    /// ScrollViewer must use Visible (not Auto) scrollbar visibility.
    /// Auto causes a double-measure pass over the entire control tree during resize.
    /// </summary>
    [TestMethod]
    public void ScrollViewer_UsesVisibleScrollbar_NotAuto()
    {
        var view = CreateView();

        var scrollViewer = (ScrollViewer)view.Content!;
        Assert.AreEqual(ScrollBarVisibility.Visible, scrollViewer.VerticalScrollBarVisibility,
            "ScrollViewer must use Visible (not Auto) to avoid double-measure during resize");
    }

    /// <summary>
    /// The pixel grid Canvas should have fixed dimensions matching GridSize * PixelSize.
    /// This prevents the Canvas from participating in dynamic layout during resize.
    /// </summary>
    [TestMethod]
    public void PixelGrid_HasFixedDimensions()
    {
        var view = CreateView();

        var scrollViewer = (ScrollViewer)view.Content!;
        var stackPanel = (StackPanel)scrollViewer.Content!;

        Canvas? pixelGrid = null;
        foreach (var child in stackPanel.Children)
        {
            if (child is Canvas c) { pixelGrid = c; break; }
        }

        Assert.IsNotNull(pixelGrid);

        // GridSize=16, PixelSize=18 -> 288x288
        Assert.AreEqual(288, pixelGrid.Width, "Pixel grid width should be fixed at 288");
        Assert.AreEqual(288, pixelGrid.Height, "Pixel grid height should be fixed at 288");
    }

    /// <summary>
    /// Thumbnails must be rendered by a single ThumbnailStrip control using DrawingContext,
    /// not 16 individual WriteableBitmaps that each create a CALayer GPU surface.
    /// </summary>
    [TestMethod]
    public void Thumbnails_UseSingleControlNotIndividualBitmaps()
    {
        var view = CreateView();

        var scrollViewer = (ScrollViewer)view.Content!;
        var stackPanel = (StackPanel)scrollViewer.Content!;

        // The ThumbnailStrip should be a direct child of the StackPanel (no WrapPanel wrapper)
        Control? thumbnailControl = null;
        int wrapPanelCount = 0;
        foreach (var child in stackPanel.Children)
        {
            // Skip color palette WrapPanel — look for a non-WrapPanel, non-standard control
            if (child is WrapPanel)
            {
                wrapPanelCount++;
                continue;
            }
            // The thumbnail strip is a custom Control (not a standard Avalonia type)
            if (child.GetType().Name == "ThumbnailStrip")
            {
                thumbnailControl = child;
                break;
            }
        }

        Assert.IsNotNull(thumbnailControl,
            "ThumbnailStrip control not found — thumbnails should be a single custom Control, not 16 Borders");

        // Verify it has no child controls (it renders everything via DrawingContext)
        if (thumbnailControl is Panel p)
            Assert.AreEqual(0, p.Children.Count, "ThumbnailStrip should have no child controls");
    }
}
