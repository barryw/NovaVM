namespace e6502.NovaPanel.Layout;

public static class ResponsiveController
{
    // Width threshold to reveal the side rail.
    public const double WideMinWidth = 1180;
    // Additional height threshold (on top of Wide) to reveal the bottom region.
    public const double FullMinHeight = 620;

    // Compact: width < WideMinWidth.
    // Wide:    width >= WideMinWidth AND height < FullMinHeight.
    // Full:    width >= WideMinWidth AND height >= FullMinHeight.
    public static SizeClass Classify(double width, double height)
    {
        if (width < WideMinWidth)
            return SizeClass.Compact;
        if (height < FullMinHeight)
            return SizeClass.Wide;
        return SizeClass.Full;
    }
}
