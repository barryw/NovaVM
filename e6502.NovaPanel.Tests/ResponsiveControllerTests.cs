using e6502.NovaPanel.Layout;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class ResponsiveControllerTests
{
    [Theory]
    [InlineData(900, 360, SizeClass.Compact)]
    [InlineData(1179, 700, SizeClass.Compact)]   // just below wide width -> compact even if tall
    [InlineData(1180, 400, SizeClass.Wide)]      // at wide width, short -> wide
    [InlineData(1180, 619, SizeClass.Wide)]      // just below full height -> wide
    [InlineData(1180, 620, SizeClass.Full)]      // at full height -> full
    [InlineData(1600, 900, SizeClass.Full)]
    public void Classifies_by_breakpoints(double w, double h, SizeClass expected)
        => Assert.Equal(expected, ResponsiveController.Classify(w, h));
}
