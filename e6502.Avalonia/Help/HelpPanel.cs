using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;

namespace e6502.Avalonia.Help;

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

        MinWidth = 280;
        MaxWidth = 500;
        ClipToBounds = true;
        Background = new SolidColorBrush(HelpStyles.Background);

        var root = new DockPanel();

        // Header
        var header = CreateHeader();
        DockPanel.SetDock(header, global::Avalonia.Controls.Dock.Top);
        root.Children.Add(header);

        // Search bar
        _searchBox = CreateSearchBox();
        var searchContainer = new Border
        {
            Padding = new Thickness(HelpStyles.PanelPadding, 8),
            Child = _searchBox
        };
        DockPanel.SetDock(searchContainer, global::Avalonia.Controls.Dock.Top);
        root.Children.Add(searchContainer);

        // Filter tabs
        _filterBar = CreateFilterBar();
        var filterContainer = new Border
        {
            Padding = new Thickness(HelpStyles.PanelPadding, 0, HelpStyles.PanelPadding, 8),
            Child = _filterBar
        };
        DockPanel.SetDock(filterContainer, global::Avalonia.Controls.Dock.Top);
        root.Children.Add(filterContainer);

        // Content area — padding on the StackPanel, not the ScrollViewer,
        // so the vertical scrollbar doesn't eat into content width.
        _contentArea = new StackPanel
        {
            Spacing = HelpStyles.SectionSpacing,
            Margin = new Thickness(HelpStyles.PanelPadding, HelpStyles.PanelPadding, HelpStyles.PanelPadding + 2, HelpStyles.PanelPadding)
        };
        _contentScroll = new ScrollViewer
        {
            Content = _contentArea,
            HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled
        };
        root.Children.Add(_contentScroll);

        Content = root;
        ShowBrowseView();
    }

    public void NavigateToTopic(string titleOrKeyword)
    {
        var topic = _index.FindByTitle(titleOrKeyword);
        if (topic != null)
            ShowTopic(topic);
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
        DockPanel.SetDock(title, global::Avalonia.Controls.Dock.Left);
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
        DockPanel.SetDock(closeBtn, global::Avalonia.Controls.Dock.Right);
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
                _currentTopic = null;
            RunSearch(box.Text ?? "");
        };
        return box;
    }

    private StackPanel CreateFilterBar()
    {
        var panel = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 6 };

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
            _contentArea.Children.Add(CreateSearchResult(topic));
    }

    private Control CreateSearchResult(HelpTopic topic)
    {
        var panel = new StackPanel { Spacing = 2 };

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

        var preview = topic.Body.Split('\n')
            .FirstOrDefault(l => !string.IsNullOrWhiteSpace(l) && !l.TrimStart().StartsWith('#'))
            ?? "";
        if (preview.Length > 80) preview = preview[..77] + "...";

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
                _contentArea.Children.Add(CreateSearchResult(topic));
        }
    }
}
