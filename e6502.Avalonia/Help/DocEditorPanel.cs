using System;
using System.Collections.Generic;
using System.IO;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Threading;

namespace e6502.Avalonia.Help;

public sealed class DocEditorPanel : UserControl
{
    private readonly string _filePath;
    private readonly TextBox _editor;
    private readonly StackPanel _previewArea;
    private readonly ScrollViewer _previewScroll;
    private readonly MarkdownRenderer _renderer = new();
    private DispatcherTimer? _debounceTimer;

    public event Action? CloseRequested;
    public event Action? Saved;

    public DocEditorPanel(string filePath, string initialContent)
    {
        _filePath = filePath;

        MinWidth = 300;
        MaxWidth = 550;
        ClipToBounds = true;
        Background = new SolidColorBrush(HelpStyles.Background);

        var root = new DockPanel();

        // Header
        var header = CreateHeader(Path.GetFileNameWithoutExtension(filePath));
        DockPanel.SetDock(header, global::Avalonia.Controls.Dock.Top);
        root.Children.Add(header);

        // Split: top = editor, bottom = preview
        var splitGrid = new Grid();
        splitGrid.RowDefinitions.Add(new RowDefinition(GridLength.Star));
        splitGrid.RowDefinitions.Add(new RowDefinition(GridLength.Auto));
        splitGrid.RowDefinitions.Add(new RowDefinition(GridLength.Star));

        // Editor TextBox
        _editor = new TextBox
        {
            AcceptsReturn = true,
            AcceptsTab = true,
            TextWrapping = TextWrapping.Wrap,
            FontFamily = new FontFamily(HelpStyles.MonoFontFamily),
            FontSize = HelpStyles.FontSizeCode,
            Text = initialContent,
            Padding = new Thickness(8),
            Margin = new Thickness(HelpStyles.PanelPadding, 8, HelpStyles.PanelPadding, 0)
        };
        _editor.TextChanged += OnTextChanged;
        var editorScroll = new ScrollViewer
        {
            Content = _editor,
            HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled
        };
        Grid.SetRow(editorScroll, 0);
        splitGrid.Children.Add(editorScroll);

        // Separator
        var separator = new Border
        {
            Height = 1,
            Background = new SolidColorBrush(HelpStyles.CodeBorder),
            Margin = new Thickness(HelpStyles.PanelPadding, 4)
        };
        Grid.SetRow(separator, 1);
        splitGrid.Children.Add(separator);

        // Preview
        _previewArea = new StackPanel
        {
            Spacing = HelpStyles.SectionSpacing,
            Margin = new Thickness(HelpStyles.PanelPadding, 8, HelpStyles.PanelPadding + 2, HelpStyles.PanelPadding)
        };
        _previewScroll = new ScrollViewer
        {
            Content = _previewArea,
            HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled
        };
        Grid.SetRow(_previewScroll, 2);
        splitGrid.Children.Add(_previewScroll);

        root.Children.Add(splitGrid);
        Content = root;

        // Initial preview render
        UpdatePreview(initialContent);
    }

    private Border CreateHeader(string programName)
    {
        var panel = new DockPanel { Margin = new Thickness(HelpStyles.PanelPadding, 12, 8, 0) };

        var title = new TextBlock
        {
            Text = $"Doc: {programName}",
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

        var saveBtn = new Button
        {
            Content = "Save",
            FontSize = HelpStyles.FontSizeSmall,
            FontWeight = FontWeight.SemiBold,
            Background = new SolidColorBrush(HelpStyles.AccentBlue),
            Foreground = Brushes.White,
            Padding = new Thickness(12, 4),
            CornerRadius = new CornerRadius(4),
            HorizontalAlignment = HorizontalAlignment.Right,
            VerticalAlignment = VerticalAlignment.Center,
            Margin = new Thickness(0, 0, 4, 0),
            Cursor = new Cursor(StandardCursorType.Hand)
        };
        saveBtn.Click += (_, _) => Save();
        DockPanel.SetDock(saveBtn, global::Avalonia.Controls.Dock.Right);
        panel.Children.Add(saveBtn);

        return new Border { Child = panel };
    }

    private void OnTextChanged(object? sender, TextChangedEventArgs e)
    {
        _debounceTimer?.Stop();
        _debounceTimer = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(300) };
        _debounceTimer.Tick += (_, _) =>
        {
            _debounceTimer.Stop();
            UpdatePreview(_editor.Text ?? "");
        };
        _debounceTimer.Start();
    }

    private void UpdatePreview(string markdown)
    {
        _previewArea.Children.Clear();

        // Parse and render using the existing markdown renderer
        var controls = _renderer.RenderBody(markdown);
        foreach (var control in controls)
            _previewArea.Children.Add(control);
    }

    public void Save()
    {
        try
        {
            File.WriteAllText(_filePath, _editor.Text ?? "");
            Saved?.Invoke();
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Failed to save doc: {ex.Message}");
        }
    }
}
