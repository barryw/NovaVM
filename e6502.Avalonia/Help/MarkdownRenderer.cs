using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;

namespace e6502.Avalonia.Help;

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

            if (string.IsNullOrWhiteSpace(trimmed)) { i++; continue; }

            if (trimmed.StartsWith(":::"))
            {
                var (control, newIndex) = ParseFencedDiv(lines, i);
                if (control != null) controls.Add(control);
                i = newIndex;
                continue;
            }

            if (trimmed.StartsWith('#'))
            {
                var level = trimmed.TakeWhile(c => c == '#').Count();
                var text = trimmed[level..].Trim();
                controls.Add(CreateHeading(text, level));
                i++;
                continue;
            }

            if (trimmed.StartsWith("```"))
            {
                var (code, lang, newIndex) = ParseCodeBlock(lines, i);
                controls.Add(CreateCodeBlock(code, lang));
                i = newIndex;
                continue;
            }

            if (trimmed.Contains('|') && trimmed.StartsWith('|'))
            {
                var (table, newIndex) = ParseTable(lines, i);
                if (table != null) controls.Add(table);
                i = newIndex;
                continue;
            }

            if (trimmed.StartsWith("- ") || trimmed.StartsWith("* "))
            {
                var (list, newIndex) = ParseList(lines, i);
                controls.Add(list);
                i = newIndex;
                continue;
            }

            // Paragraph
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
        if (i < lines.Length) i++;
        return (sb.ToString(), language, i);
    }

    private (Control? control, int nextIndex) ParseFencedDiv(string[] lines, int startIndex)
    {
        var firstLine = lines[startIndex].TrimStart();
        var divType = firstLine[3..].Trim().ToLowerInvariant();

        if (string.IsNullOrEmpty(divType))
            return (null, startIndex + 1);

        var contentLines = new List<string>();
        var i = startIndex + 1;
        while (i < lines.Length && !lines[i].TrimStart().StartsWith(":::"))
        {
            contentLines.Add(lines[i]);
            i++;
        }
        if (i < lines.Length) i++;

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
        var code = content;
        if (content.Contains("```"))
        {
            var codeLines = content.Split('\n');
            var codeContent = new List<string>();
            var inCode = false;
            foreach (var line in codeLines)
            {
                if (line.TrimStart().StartsWith("```")) { inCode = !inCode; continue; }
                if (inCode) codeContent.Add(line);
            }
            code = string.Join("\n", codeContent);
        }

        var stack = new StackPanel { Spacing = 8 };
        stack.Children.Add(new TextBlock
        {
            Text = code,
            FontFamily = new FontFamily(HelpStyles.MonoFontFamily),
            FontSize = HelpStyles.FontSizeCode,
            Foreground = new SolidColorBrush(HelpStyles.TextPrimary),
            TextWrapping = TextWrapping.NoWrap
        });

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
            Cursor = new global::Avalonia.Input.Cursor(global::Avalonia.Input.StandardCursorType.Hand)
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
            if (line.Contains("---")) { i++; continue; }
            var cells = line.Split('|', StringSplitOptions.None)
                .Skip(1).SkipLast(1)
                .Select(c => c.Trim())
                .ToArray();
            rows.Add(cells);
            i++;
        }

        if (rows.Count == 0) return (null, i);

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
            if (!trimmed.StartsWith("- ") && !trimmed.StartsWith("* ")) break;

            var text = trimmed[2..].Trim();
            var item = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 6 };
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
                Cursor = new global::Avalonia.Input.Cursor(global::Avalonia.Input.StandardCursorType.Hand)
            };
            var capturedLink = link;
            pill.Click += (_, _) => LinkClicked?.Invoke(capturedLink);
            wrap.Children.Add(pill);
        }
        stack.Children.Add(wrap);
        return stack;
    }
}
