using Avalonia.Media;

namespace e6502.Avalonia.Help;

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
    public static readonly Color BadgeProgram = Color.Parse("#F59E0B");
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
