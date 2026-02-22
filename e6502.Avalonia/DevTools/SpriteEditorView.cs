using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Threading;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;

using StorageFilePickerSaveOptions = global::Avalonia.Platform.Storage.FilePickerSaveOptions;
using StorageFilePickerOpenOptions = global::Avalonia.Platform.Storage.FilePickerOpenOptions;

namespace e6502.Avalonia.DevTools;

public class SpriteEditorView : UserControl
{
    private enum DrawTool { Draw, Erase, Fill }

    private const int PixelSize = 18;
    private const int GridSize = 16;
    private const int PreviewScale = 4;
    private const int SwatchSize = 24;
    private const int SpriteCount = 16;
    private const int ThumbDisplaySize = 24;
    private const int AnimPreviewSize = 64;

    private readonly VirtualGraphicsController _vgc;
    private readonly Border[] _swatches = new Border[16];
    private readonly Canvas _pixelGrid;

    // Pixel grid rendered by DrawingContext.
    private readonly PixelGridSurface _gridSurface;

    // Cached palette brushes for DrawingContext rendering
    private static readonly IBrush[] PaletteBrushes = BuildPaletteBrushes();

    private static IBrush[] BuildPaletteBrushes()
    {
        var brushes = new IBrush[16];
        for (int i = 0; i < 16; i++)
            brushes[i] = new SolidColorBrush(ColorPalette.Get(i));
        return brushes;
    }

    // Resize detection — skip poll work during window resize/move
    private long _lastArrangeTick;

    // Custom controls replacing WriteableBitmaps
    private readonly ThumbnailStrip _thumbnailStrip;
    private readonly SpritePreview _spritePreview;
    private readonly AnimationPreview _animPreview;

    // Properties
    private CheckBox _chkEnabled = null!;
    private NumericUpDown _nudX = null!;
    private NumericUpDown _nudY = null!;
    private ComboBox _cboPriority = null!;
    private CheckBox _chkFlipH = null!;
    private CheckBox _chkFlipV = null!;
    private bool _suppressPropEvents;

    // Copy/Clear
    private ComboBox _cboCopyTarget = null!;

    // Save/Load
    private TextBlock _gloadHint = null!;

    // Animation
    private ComboBox _cboAnimFrom = null!;
    private ComboBox _cboAnimTo = null!;
    private ComboBox _cboAnimSpeed = null!;
    private Button _btnAnimToggle = null!;
    private DispatcherTimer? _animTimer;
    private int _animCurrentFrame;
    private bool _animPlaying;

    private readonly byte[] _lastKnownShape = new byte[VgcConstants.SpriteShapeSize];

    private IDisposable? _pollTimer;
    private IDisposable? _detachDebounce;
    private int _selectedSprite;
    private byte _selectedColor = 1; // white
    private DrawTool _activeTool = DrawTool.Draw;
    private Button[] _toolButtons = null!;
    private bool _isDrawing;
    private bool _isErasing;

    public SpriteEditorView(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
        Background = new SolidColorBrush(Color.FromRgb(30, 30, 30));

        _gridSurface = new PixelGridSurface
        {
            Width = GridSize * PixelSize,
            Height = GridSize * PixelSize
        };

        _thumbnailStrip = new ThumbnailStrip(vgc);
        _thumbnailStrip.SpriteSelected += SelectSprite;

        _spritePreview = new SpritePreview
        {
            Width = GridSize * PreviewScale,
            Height = GridSize * PreviewScale
        };

        _animPreview = new AnimationPreview
        {
            Width = AnimPreviewSize,
            Height = AnimPreviewSize
        };

        _pixelGrid = BuildPixelGrid();
        Content = BuildLayout();

        RefreshAllThumbnails();
        RefreshGridFromVgc();
        UpdatePreview();
        UpdateThumbBorderHighlights();
        UpdateSwatchHighlights();
        RefreshPropertiesFromVgc();
        UpdateGloadHint();
    }

    protected override Size ArrangeOverride(Size finalSize)
    {
        _lastArrangeTick = Environment.TickCount64;
        return base.ArrangeOverride(finalSize);
    }

    protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnAttachedToVisualTree(e);
        _detachDebounce?.Dispose();
        _detachDebounce = null;
        if (_pollTimer is null)
            StartPollTimer();
    }

    protected override void OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnDetachedFromVisualTree(e);
        _detachDebounce = DispatcherTimer.RunOnce(() =>
        {
            if (Parent is null)
            {
                _pollTimer?.Dispose();
                _pollTimer = null;
                StopAnimation();
            }
            _detachDebounce = null;
        }, TimeSpan.FromMilliseconds(200));
    }

    // -------------------------------------------------------------------------
    // Poll timer — detects external changes every 250ms
    // -------------------------------------------------------------------------

    private void StartPollTimer()
    {
        var pollBuffer = new byte[VgcConstants.SpriteShapeSize];
        var thumbPollBuffer = new byte[VgcConstants.SpriteShapeSize];
        _pollTimer = DispatcherTimer.Run(() =>
        {
            if (!IsVisible) return true;
            if (UiTransitionGate.IsPaused) return true;

            // Skip poll work during window resize/move to avoid beachball
            if (Environment.TickCount64 - _lastArrangeTick < 300) return true;

            // Check selected sprite shape -> refresh grid + preview
            _vgc.CopySpriteShape(_selectedSprite, pollBuffer);
            if (!pollBuffer.AsSpan().SequenceEqual(_lastKnownShape))
            {
                RefreshGridFromVgc();
                UpdatePreview();
            }

            // Check all 16 sprite shapes -> refresh changed thumbnails
            for (int i = 0; i < SpriteCount; i++)
            {
                _vgc.CopySpriteShape(i, thumbPollBuffer);
                if (!thumbPollBuffer.AsSpan().SequenceEqual(_thumbnailStrip.GetShapeCache(i)))
                    RefreshThumbnail(i);
            }

            // Refresh properties (position, flags, etc. may change externally)
            RefreshPropertiesFromVgc();

            // Refresh thumbnail border highlights (enabled state may change)
            UpdateThumbBorderHighlights();

            return true;
        }, TimeSpan.FromMilliseconds(250));
    }

    // -------------------------------------------------------------------------
    // Layout
    // -------------------------------------------------------------------------

    private Control BuildLayout()
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Vertical,
            Spacing = 6,
            Margin = new Thickness(8)
        };

        panel.Children.Add(new TextBlock
        {
            Text = "Sprite",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 0, 0, 2)
        });
        panel.Children.Add(_thumbnailStrip);

        panel.Children.Add(new TextBlock
        {
            Text = "Properties",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 4, 0, 2)
        });
        panel.Children.Add(BuildPropertiesPanel());

        panel.Children.Add(new TextBlock
        {
            Text = "Editor",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 4, 0, 2)
        });
        panel.Children.Add(_pixelGrid);

        panel.Children.Add(new TextBlock
        {
            Text = "Color",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 4, 0, 2)
        });
        panel.Children.Add(BuildColorPalette());

        panel.Children.Add(new TextBlock
        {
            Text = "Tools",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 4, 0, 2)
        });
        panel.Children.Add(BuildDrawingTools());

        panel.Children.Add(new TextBlock
        {
            Text = "Preview",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 4, 0, 2)
        });
        panel.Children.Add(_spritePreview);

        panel.Children.Add(BuildCopyClearTools());
        panel.Children.Add(BuildSaveLoadPanel());
        panel.Children.Add(BuildAnimationPanel());

        return new ScrollViewer
        {
            Content = panel,
            VerticalScrollBarVisibility = ScrollBarVisibility.Visible
        };
    }

    // -------------------------------------------------------------------------
    // Properties panel
    // -------------------------------------------------------------------------

    private Control BuildPropertiesPanel()
    {
        var outer = new StackPanel { Orientation = Orientation.Vertical, Spacing = 4 };

        // Row 1: Enabled, X, Y, Priority
        var row1 = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 8 };
        _chkEnabled = StyledCheckBox("Enabled");
        _chkEnabled.VerticalAlignment = VerticalAlignment.Center;
        _chkEnabled.IsCheckedChanged += OnEnabledChanged;

        _nudX = StyledNumericUpDown(-16, 320);
        _nudX.ValueChanged += OnPositionChanged;
        _nudY = StyledNumericUpDown(-16, 200);
        _nudY.ValueChanged += OnPositionChanged;

        _cboPriority = new ComboBox { Width = 110 };
        _cboPriority.Items.Add("Behind (0)");
        _cboPriority.Items.Add("Between (1)");
        _cboPriority.Items.Add("Front (2)");
        _cboPriority.SelectedIndex = 2;
        _cboPriority.SelectionChanged += OnPriorityChanged;

        row1.Children.Add(_chkEnabled);
        row1.Children.Add(new TextBlock { Text = "X:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        row1.Children.Add(_nudX);
        row1.Children.Add(new TextBlock { Text = "Y:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        row1.Children.Add(_nudY);
        row1.Children.Add(_cboPriority);

        // Row 2: Flip H, Flip V
        var row2 = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 12 };
        _chkFlipH = StyledCheckBox("Flip H");
        _chkFlipH.IsCheckedChanged += OnFlipChanged;
        _chkFlipV = StyledCheckBox("Flip V");
        _chkFlipV.IsCheckedChanged += OnFlipChanged;
        row2.Children.Add(_chkFlipH);
        row2.Children.Add(_chkFlipV);

        outer.Children.Add(row1);
        outer.Children.Add(row2);
        return outer;
    }

    private void OnEnabledChanged(object? sender, global::Avalonia.Interactivity.RoutedEventArgs e)
    {
        if (_suppressPropEvents) return;
        _vgc.SetSpriteEnabled(_selectedSprite, _chkEnabled.IsChecked == true);
        UpdateThumbBorderHighlights();
    }

    private void OnPositionChanged(object? sender, NumericUpDownValueChangedEventArgs e)
    {
        if (_suppressPropEvents) return;
        _vgc.SetSpritePosition(_selectedSprite, (int)(_nudX.Value ?? 0), (int)(_nudY.Value ?? 0));
    }

    private void OnPriorityChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (_suppressPropEvents) return;
        _vgc.SetSpritePriority(_selectedSprite, (byte)(_cboPriority.SelectedIndex >= 0 ? _cboPriority.SelectedIndex : 2));
    }

    private void OnFlipChanged(object? sender, global::Avalonia.Interactivity.RoutedEventArgs e)
    {
        if (_suppressPropEvents) return;
        byte flags = 0;
        if (_chkFlipH.IsChecked == true) flags |= 0x01;
        if (_chkFlipV.IsChecked == true) flags |= 0x02;
        _vgc.SetSpriteFlags(_selectedSprite, flags);
        UpdatePreview();
    }

    private void RefreshPropertiesFromVgc()
    {
        var state = _vgc.GetSpriteState(_selectedSprite);
        bool enabled = state.enabled;
        decimal x = state.x;
        decimal y = state.y;
        int pri = Math.Clamp((int)state.priority, 0, 2);
        bool flipH = (state.flags & 0x01) != 0;
        bool flipV = (state.flags & 0x02) != 0;

        // Only set properties when values actually changed to avoid layout invalidation
        if (_chkEnabled.IsChecked != enabled || _nudX.Value != x || _nudY.Value != y
            || _cboPriority.SelectedIndex != pri || _chkFlipH.IsChecked != flipH || _chkFlipV.IsChecked != flipV)
        {
            _suppressPropEvents = true;
            if (_chkEnabled.IsChecked != enabled) _chkEnabled.IsChecked = enabled;
            if (_nudX.Value != x) _nudX.Value = x;
            if (_nudY.Value != y) _nudY.Value = y;
            if (_cboPriority.SelectedIndex != pri) _cboPriority.SelectedIndex = pri;
            if (_chkFlipH.IsChecked != flipH) _chkFlipH.IsChecked = flipH;
            if (_chkFlipV.IsChecked != flipV) _chkFlipV.IsChecked = flipV;
            _suppressPropEvents = false;
        }
    }

    // -------------------------------------------------------------------------
    // Pixel grid (editor)
    // -------------------------------------------------------------------------

    private Canvas BuildPixelGrid()
    {
        var canvas = new Canvas
        {
            Width = GridSize * PixelSize,
            Height = GridSize * PixelSize,
            Background = Brushes.Black
        };

        canvas.Children.Add(_gridSurface);

        canvas.PointerPressed += OnGridPointerPressed;
        canvas.PointerMoved += OnGridPointerMoved;
        canvas.PointerReleased += OnGridPointerReleased;

        return canvas;
    }

    // -------------------------------------------------------------------------
    // Color palette
    // -------------------------------------------------------------------------

    private Control BuildColorPalette()
    {
        var wrap = new WrapPanel { Orientation = Orientation.Horizontal };
        for (int i = 0; i < 16; i++)
        {
            int index = i;
            var swatch = new Border
            {
                Width = SwatchSize,
                Height = SwatchSize,
                Margin = new Thickness(1),
                Background = new SolidColorBrush(ColorPalette.Get(i)),
                BorderThickness = new Thickness(2),
                BorderBrush = Brushes.Transparent,
                Cursor = new Cursor(StandardCursorType.Hand)
            };
            swatch.PointerPressed += (_, _) => SelectColor(index);
            _swatches[i] = swatch;
            wrap.Children.Add(swatch);
        }
        return wrap;
    }

    // -------------------------------------------------------------------------
    // Drawing tools
    // -------------------------------------------------------------------------

    private Control BuildDrawingTools()
    {
        var outer = new StackPanel { Orientation = Orientation.Vertical, Spacing = 4 };

        // Row 1: Draw/Erase/Fill tool modes
        var row1 = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 2 };
        var btnDraw = MakeToolButton("Draw", DrawTool.Draw);
        var btnErase = MakeToolButton("Erase", DrawTool.Erase);
        var btnFill = MakeToolButton("Fill", DrawTool.Fill);
        _toolButtons = [btnDraw, btnErase, btnFill];
        UpdateToolButtonHighlights();
        row1.Children.Add(btnDraw);
        row1.Children.Add(btnErase);
        row1.Children.Add(btnFill);

        // Row 2: Shift + Mirror/Rotate
        var row2 = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 2 };

        Button MakeBtn(string label, Action action)
        {
            var b = StyledButton(label);
            b.Click += (_, _) => action();
            return b;
        }

        row2.Children.Add(MakeBtn("<", () => ShiftPixels(-1, 0)));
        row2.Children.Add(MakeBtn(">", () => ShiftPixels(1, 0)));
        row2.Children.Add(MakeBtn("^", () => ShiftPixels(0, -1)));
        row2.Children.Add(MakeBtn("v", () => ShiftPixels(0, 1)));
        row2.Children.Add(MakeBtn("MirH", () => MirrorPixels(horizontal: true)));
        row2.Children.Add(MakeBtn("MirV", () => MirrorPixels(horizontal: false)));
        row2.Children.Add(MakeBtn("Rot90", () => RotatePixels90CW()));

        outer.Children.Add(row1);
        outer.Children.Add(row2);
        return outer;
    }

    private Button MakeToolButton(string label, DrawTool tool)
    {
        var btn = StyledButton(label);
        btn.BorderThickness = new Thickness(2);
        btn.Click += (_, _) =>
        {
            _activeTool = tool;
            UpdateToolButtonHighlights();
        };
        return btn;
    }

    private void UpdateToolButtonHighlights()
    {
        for (int i = 0; i < _toolButtons.Length; i++)
        {
            _toolButtons[i].BorderBrush = (DrawTool)i == _activeTool
                ? Brushes.Yellow
                : Brushes.Transparent;
        }
    }

    // -------------------------------------------------------------------------
    // Copy & Clear tools
    // -------------------------------------------------------------------------

    private Control BuildCopyClearTools()
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 6,
            Margin = new Thickness(0, 6, 0, 0)
        };

        panel.Children.Add(new TextBlock { Text = "Copy to:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        _cboCopyTarget = new ComboBox { Width = 60 };
        for (int i = 0; i < SpriteCount; i++)
            _cboCopyTarget.Items.Add(i.ToString());
        _cboCopyTarget.SelectedIndex = 0;
        panel.Children.Add(_cboCopyTarget);

        var copyBtn = StyledButton("Copy");
        copyBtn.Click += (_, _) =>
        {
            int target = _cboCopyTarget.SelectedIndex;
            if (target >= 0 && target < SpriteCount)
            {
                _vgc.CopySpriteSlot(_selectedSprite, target);
                RefreshThumbnail(target);
                if (target == _selectedSprite)
                {
                    RefreshGridFromVgc();
                    UpdatePreview();
                }
            }
        };
        panel.Children.Add(copyBtn);

        var clearBtn = StyledButton("Clear");
        clearBtn.Click += (_, _) =>
        {
            _vgc.ClearSpriteShape(_selectedSprite);
            RefreshGridFromVgc();
            UpdatePreview();
            RefreshThumbnail(_selectedSprite);
        };
        panel.Children.Add(clearBtn);

        return panel;
    }

    // -------------------------------------------------------------------------
    // Save/Load panel
    // -------------------------------------------------------------------------

    private Control BuildSaveLoadPanel()
    {
        var outer = new StackPanel { Orientation = Orientation.Vertical, Spacing = 4, Margin = new Thickness(0, 6, 0, 0) };

        var btnRow = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 6 };

        var saveBtn = StyledButton("Save Sprite");
        saveBtn.Click += async (_, _) =>
        {
            try { await SaveSpriteAsync(false); }
            catch (Exception ex) { await ShowErrorAsync("Save failed", ex); }
        };

        var saveAllBtn = StyledButton("Save All");
        saveAllBtn.Click += async (_, _) =>
        {
            try { await SaveSpriteAsync(true); }
            catch (Exception ex) { await ShowErrorAsync("Save failed", ex); }
        };

        var loadBtn = StyledButton("Load");
        loadBtn.Click += async (_, _) =>
        {
            try { await LoadSpriteAsync(); }
            catch (Exception ex) { await ShowErrorAsync("Load failed", ex); }
        };

        btnRow.Children.Add(saveBtn);
        btnRow.Children.Add(saveAllBtn);
        btnRow.Children.Add(loadBtn);

        _gloadHint = new TextBlock
        {
            FontFamily = new FontFamily("Courier New, monospace"),
            FontSize = 11,
            Foreground = new SolidColorBrush(Color.FromRgb(128, 128, 128)),
            Margin = new Thickness(0, 2, 0, 0)
        };

        outer.Children.Add(btnRow);
        outer.Children.Add(_gloadHint);
        return outer;
    }

    private void UpdateGloadHint()
    {
        int offset = _selectedSprite * VgcConstants.SpriteShapeSize;
        _gloadHint.Text = $"GLOAD \"name\",3,{offset},{VgcConstants.SpriteShapeSize}";
    }

    // -------------------------------------------------------------------------
    // Animation panel
    // -------------------------------------------------------------------------

    private Control BuildAnimationPanel()
    {
        var outer = new StackPanel
        {
            Orientation = Orientation.Vertical,
            Spacing = 4,
            Margin = new Thickness(0, 6, 0, 0)
        };

        outer.Children.Add(new TextBlock
        {
            Text = "Animation",
            FontWeight = FontWeight.Bold,
            Foreground = Brushes.White,
            Margin = new Thickness(0, 0, 0, 2)
        });

        var row = new StackPanel { Orientation = Orientation.Horizontal, Spacing = 6 };

        row.Children.Add(new TextBlock { Text = "From:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        _cboAnimFrom = new ComboBox { Width = 55 };
        for (int i = 0; i < SpriteCount; i++) _cboAnimFrom.Items.Add(i.ToString());
        _cboAnimFrom.SelectedIndex = 0;
        row.Children.Add(_cboAnimFrom);

        row.Children.Add(new TextBlock { Text = "To:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        _cboAnimTo = new ComboBox { Width = 55 };
        for (int i = 0; i < SpriteCount; i++) _cboAnimTo.Items.Add(i.ToString());
        _cboAnimTo.SelectedIndex = 3;
        row.Children.Add(_cboAnimTo);

        row.Children.Add(new TextBlock { Text = "Speed:", Foreground = Brushes.Gray, VerticalAlignment = VerticalAlignment.Center });
        _cboAnimSpeed = new ComboBox { Width = 75 };
        _cboAnimSpeed.Items.Add("50ms");
        _cboAnimSpeed.Items.Add("100ms");
        _cboAnimSpeed.Items.Add("200ms");
        _cboAnimSpeed.Items.Add("333ms");
        _cboAnimSpeed.Items.Add("500ms");
        _cboAnimSpeed.SelectedIndex = 2;
        row.Children.Add(_cboAnimSpeed);

        _btnAnimToggle = StyledButton("Play");
        _btnAnimToggle.Click += (_, _) =>
        {
            if (_animPlaying) StopAnimation();
            else StartAnimation();
        };
        row.Children.Add(_btnAnimToggle);

        outer.Children.Add(row);
        outer.Children.Add(_animPreview);
        return outer;
    }

    private void StartAnimation()
    {
        int from = _cboAnimFrom.SelectedIndex;
        int to = _cboAnimTo.SelectedIndex;
        if (from < 0 || to < 0) return;

        int[] speeds = [50, 100, 200, 333, 500];
        int speedIdx = _cboAnimSpeed.SelectedIndex;
        if (speedIdx < 0) speedIdx = 2;
        int ms = speeds[speedIdx];

        _animCurrentFrame = from;
        _animPlaying = true;
        _btnAnimToggle.Content = "Stop";

        int step = from <= to ? 1 : -1;

        _animTimer = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(ms) };
        _animTimer.Tick += (_, _) =>
        {
            if (UiTransitionGate.IsPaused)
                return;
            RenderAnimFrame(_animCurrentFrame);
            _animCurrentFrame += step;
            if (step > 0 && _animCurrentFrame > to) _animCurrentFrame = from;
            else if (step < 0 && _animCurrentFrame < to) _animCurrentFrame = from;
        };
        RenderAnimFrame(_animCurrentFrame);
        _animTimer.Start();
    }

    private void StopAnimation()
    {
        _animTimer?.Stop();
        _animTimer = null;
        _animPlaying = false;
        if (_btnAnimToggle != null)
            _btnAnimToggle.Content = "Play";
    }

    private void RenderAnimFrame(int spriteIndex)
    {
        if ((uint)spriteIndex >= SpriteCount) return;
        var shape = new byte[VgcConstants.SpriteShapeSize];
        _vgc.CopySpriteShape(spriteIndex, shape);
        _animPreview.Update(shape);
        _animPreview.InvalidateVisual();
    }

    // -------------------------------------------------------------------------
    // Selection
    // -------------------------------------------------------------------------

    private void SelectSprite(int index)
    {
        _selectedSprite = index;
        UpdateThumbBorderHighlights();
        RefreshGridFromVgc();
        UpdatePreview();
        RefreshPropertiesFromVgc();
        UpdateGloadHint();
    }

    private void SelectColor(int index)
    {
        _selectedColor = (byte)index;
        UpdateSwatchHighlights();
    }

    // -------------------------------------------------------------------------
    // Grid refresh & pixel editing
    // -------------------------------------------------------------------------

    private void RefreshGridFromVgc()
    {
        _vgc.CopySpriteShape(_selectedSprite, _lastKnownShape);
        RenderGridBitmap();
    }

    private void RenderGridBitmap()
    {
        _gridSurface.Update(_lastKnownShape);
        _gridSurface.InvalidateVisual();
    }

    private void SetPixel(int x, int y, byte color)
    {
        if ((uint)x >= GridSize || (uint)y >= GridSize) return;

        _vgc.SetSpritePixel(_selectedSprite, x, y, color);

        // Update local cache to match
        int localIdx = y * VgcConstants.SpriteBytesPerRow + x / 2;
        int absAddr = _selectedSprite * VgcConstants.SpriteShapeSize + localIdx;
        _vgc.TryReadMemorySpace(VgcConstants.MemSpaceSprite, absAddr, out byte updated);
        _lastKnownShape[localIdx] = updated;

        // Re-render grid bitmap (fast — just 288x288 pixel writes)
        RenderGridBitmap();
        UpdatePreview();
        RefreshThumbnail(_selectedSprite);
    }

    private void OnGridPointerPressed(object? sender, PointerPressedEventArgs e)
    {
        var point = e.GetCurrentPoint(_pixelGrid);
        int x = (int)(point.Position.X / PixelSize);
        int y = (int)(point.Position.Y / PixelSize);
        if ((uint)x >= GridSize || (uint)y >= GridSize) return;

        if (_activeTool == DrawTool.Fill)
        {
            FloodFill(x, y, _selectedColor);
            e.Handled = true;
            return;
        }

        _isErasing = _activeTool == DrawTool.Erase || point.Properties.IsRightButtonPressed;
        _isDrawing = true;
        SetPixel(x, y, _isErasing ? (byte)0 : _selectedColor);
        e.Handled = true;
    }

    private void OnGridPointerMoved(object? sender, PointerEventArgs e)
    {
        if (!_isDrawing || _activeTool == DrawTool.Fill) return;
        var point = e.GetCurrentPoint(_pixelGrid);
        int x = (int)(point.Position.X / PixelSize);
        int y = (int)(point.Position.Y / PixelSize);
        SetPixel(x, y, _isErasing ? (byte)0 : _selectedColor);
        e.Handled = true;
    }

    private void OnGridPointerReleased(object? sender, PointerReleasedEventArgs e)
    {
        _isDrawing = false;
        e.Handled = true;
    }

    // -------------------------------------------------------------------------
    // Preview (flip-aware)
    // -------------------------------------------------------------------------

    private void UpdatePreview()
    {
        var shape = new byte[VgcConstants.SpriteShapeSize];
        _vgc.CopySpriteShape(_selectedSprite, shape);

        var state = _vgc.GetSpriteState(_selectedSprite);
        bool xFlip = (state.flags & 0x01) != 0;
        bool yFlip = (state.flags & 0x02) != 0;

        _spritePreview.Update(shape, xFlip, yFlip);
        _spritePreview.InvalidateVisual();
    }

    // -------------------------------------------------------------------------
    // Thumbnails
    // -------------------------------------------------------------------------

    private void RefreshThumbnail(int index)
    {
        var shape = new byte[VgcConstants.SpriteShapeSize];
        _vgc.CopySpriteShape(index, shape);
        _thumbnailStrip.Update(index, shape);
        _thumbnailStrip.InvalidateVisual();
    }

    private void RefreshAllThumbnails()
    {
        for (int i = 0; i < SpriteCount; i++)
        {
            var shape = new byte[VgcConstants.SpriteShapeSize];
            _vgc.CopySpriteShape(i, shape);
            _thumbnailStrip.Update(i, shape);
        }
        _thumbnailStrip.InvalidateVisual();
    }

    // -------------------------------------------------------------------------
    // Highlight updates
    // -------------------------------------------------------------------------

    private void UpdateThumbBorderHighlights()
    {
        var enabledFlags = new bool[SpriteCount];
        for (int i = 0; i < SpriteCount; i++)
            enabledFlags[i] = _vgc.GetSpriteState(i).enabled;

        _thumbnailStrip.SetSelection(_selectedSprite, enabledFlags);
        _thumbnailStrip.InvalidateVisual();
    }

    private void UpdateSwatchHighlights()
    {
        for (int i = 0; i < 16; i++)
        {
            _swatches[i].BorderBrush = i == _selectedColor
                ? Brushes.Yellow
                : Brushes.Transparent;
        }
    }

    // -------------------------------------------------------------------------
    // Save / Load
    // -------------------------------------------------------------------------

    private async Task SaveSpriteAsync(bool saveAll)
    {
        var topLevel = TopLevel.GetTopLevel(this);
        if (topLevel is null) return;

        string suggested = saveAll ? "sprites.gfx" : $"sprite{_selectedSprite}.gfx";
        var file = await topLevel.StorageProvider.SaveFilePickerAsync(
            new StorageFilePickerSaveOptions
            {
                Title = saveAll ? "Save All Sprites" : "Save Sprite",
                DefaultExtension = "gfx",
                SuggestedFileName = suggested
            });

        if (file is null) return;

        byte[] data;
        if (saveAll)
        {
            data = new byte[SpriteCount * VgcConstants.SpriteShapeSize];
            for (int i = 0; i < SpriteCount; i++)
                _vgc.CopySpriteShape(i, data.AsSpan(i * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize));
        }
        else
        {
            data = new byte[VgcConstants.SpriteShapeSize];
            _vgc.CopySpriteShape(_selectedSprite, data);
        }

        await using var stream = await file.OpenWriteAsync();
        stream.SetLength(0);
        await stream.WriteAsync(data);
    }

    private async Task LoadSpriteAsync()
    {
        var topLevel = TopLevel.GetTopLevel(this);
        if (topLevel is null) return;

        var files = await topLevel.StorageProvider.OpenFilePickerAsync(
            new StorageFilePickerOpenOptions
            {
                Title = "Load Sprite",
                AllowMultiple = false
            });

        if (files.Count == 0) return;

        await using var stream = await files[0].OpenReadAsync();
        using var ms = new MemoryStream();
        await stream.CopyToAsync(ms);
        var data = ms.ToArray();

        int spritesInFile = Math.Max(1, data.Length / VgcConstants.SpriteShapeSize);

        for (int s = 0; s < spritesInFile; s++)
        {
            int targetSprite = _selectedSprite + s;
            if (targetSprite >= SpriteCount) break;

            int fileOffset = s * VgcConstants.SpriteShapeSize;
            int spriteBase = targetSprite * VgcConstants.SpriteShapeSize;
            int len = Math.Min(VgcConstants.SpriteShapeSize, data.Length - fileOffset);

            for (int i = 0; i < len; i++)
                _vgc.TryWriteMemorySpace(VgcConstants.MemSpaceSprite, spriteBase + i, data[fileOffset + i]);

            RefreshThumbnail(targetSprite);
        }

        RefreshGridFromVgc();
        UpdatePreview();
    }

    private async Task ShowErrorAsync(string title, Exception ex)
    {
        var topLevel = TopLevel.GetTopLevel(this);
        if (topLevel is Window window)
        {
            var dialog = new Window
            {
                Title = title,
                Width = 400,
                Height = 150,
                Content = new TextBlock
                {
                    Text = ex.Message,
                    TextWrapping = TextWrapping.Wrap,
                    Margin = new Thickness(16)
                }
            };
            await dialog.ShowDialog(window);
        }
    }

    // -------------------------------------------------------------------------
    // Flood fill (iterative, 4-connected)
    // -------------------------------------------------------------------------

    private void FloodFill(int startX, int startY, byte fillColor)
    {
        var shape = new byte[VgcConstants.SpriteShapeSize];
        _vgc.CopySpriteShape(_selectedSprite, shape);

        byte targetColor = GetNibble(shape, startX, startY);
        if (targetColor == fillColor) return;

        var stack = new Stack<(int x, int y)>();
        stack.Push((startX, startY));

        while (stack.Count > 0)
        {
            var (x, y) = stack.Pop();
            if ((uint)x >= GridSize || (uint)y >= GridSize) continue;
            if (GetNibble(shape, x, y) != targetColor) continue;

            SetNibble(shape, x, y, fillColor);
            _vgc.SetSpritePixel(_selectedSprite, x, y, fillColor);

            stack.Push((x - 1, y));
            stack.Push((x + 1, y));
            stack.Push((x, y - 1));
            stack.Push((x, y + 1));
        }

        RefreshGridFromVgc();
        UpdatePreview();
        RefreshThumbnail(_selectedSprite);
    }

    // -------------------------------------------------------------------------
    // Bulk pixel operations (shift, mirror, rotate)
    // -------------------------------------------------------------------------

    private byte[,] ReadSpriteGrid()
    {
        var shape = new byte[VgcConstants.SpriteShapeSize];
        _vgc.CopySpriteShape(_selectedSprite, shape);
        var grid = new byte[GridSize, GridSize];
        for (int y = 0; y < GridSize; y++)
            for (int x = 0; x < GridSize; x++)
                grid[x, y] = GetNibble(shape, x, y);
        return grid;
    }

    private void WriteSpriteGrid(byte[,] grid)
    {
        for (int y = 0; y < GridSize; y++)
            for (int x = 0; x < GridSize; x++)
                _vgc.SetSpritePixel(_selectedSprite, x, y, grid[x, y]);
        RefreshGridFromVgc();
        UpdatePreview();
        RefreshThumbnail(_selectedSprite);
    }

    private void ShiftPixels(int dx, int dy)
    {
        var old = ReadSpriteGrid();
        var result = new byte[GridSize, GridSize];
        for (int y = 0; y < GridSize; y++)
            for (int x = 0; x < GridSize; x++)
            {
                int sx = x - dx, sy = y - dy;
                result[x, y] = (uint)sx < GridSize && (uint)sy < GridSize ? old[sx, sy] : (byte)0;
            }
        WriteSpriteGrid(result);
    }

    private void MirrorPixels(bool horizontal)
    {
        var old = ReadSpriteGrid();
        var result = new byte[GridSize, GridSize];
        for (int y = 0; y < GridSize; y++)
            for (int x = 0; x < GridSize; x++)
                result[x, y] = horizontal ? old[15 - x, y] : old[x, 15 - y];
        WriteSpriteGrid(result);
    }

    private void RotatePixels90CW()
    {
        var old = ReadSpriteGrid();
        var result = new byte[GridSize, GridSize];
        for (int y = 0; y < GridSize; y++)
            for (int x = 0; x < GridSize; x++)
                result[x, y] = old[y, 15 - x];
        WriteSpriteGrid(result);
    }

    // -------------------------------------------------------------------------
    // Control styling
    // -------------------------------------------------------------------------

    private static readonly IBrush ControlBg = new SolidColorBrush(Color.FromRgb(55, 55, 55));
    private static readonly IBrush ControlBorder = new SolidColorBrush(Color.FromRgb(100, 100, 100));

    private static Button StyledButton(string label) => new()
    {
        Content = label,
        Margin = new Thickness(1),
        Foreground = Brushes.White,
        Background = ControlBg,
        BorderBrush = ControlBorder,
        BorderThickness = new Thickness(1)
    };

    private static CheckBox StyledCheckBox(string label) => new()
    {
        Content = label,
        Foreground = Brushes.White
    };

    private static NumericUpDown StyledNumericUpDown(decimal min, decimal max) => new()
    {
        Minimum = min,
        Maximum = max,
        Value = 0,
        Width = 120,
        FormatString = "0",
        Foreground = Brushes.White,
        Background = ControlBg,
        BorderBrush = ControlBorder,
        BorderThickness = new Thickness(1)
    };

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    private static byte GetNibble(ReadOnlySpan<byte> shape, int x, int y)
    {
        int byteIdx = y * VgcConstants.SpriteBytesPerRow + x / 2;
        byte b = shape[byteIdx];
        return (byte)(x % 2 == 0 ? (b >> 4) & 0x0F : b & 0x0F);
    }

    private static void SetNibble(Span<byte> shape, int x, int y, byte color)
    {
        int byteIdx = y * VgcConstants.SpriteBytesPerRow + x / 2;
        if (x % 2 == 0)
            shape[byteIdx] = (byte)((shape[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
        else
            shape[byteIdx] = (byte)((shape[byteIdx] & 0xF0) | (color & 0x0F));
    }

    // =========================================================================
    // Inner controls — DrawingContext-based rendering (no WriteableBitmaps)
    // =========================================================================

    /// <summary>
    /// Renders the 16x16 sprite edit grid using DrawingContext.
    /// </summary>
    private sealed class PixelGridSurface : Control
    {
        private readonly byte[] _shape = new byte[VgcConstants.SpriteShapeSize];
        private static readonly IBrush GridLineBrush = new SolidColorBrush(Color.FromRgb(0x28, 0x28, 0x28));
        private const int PixelSize = 18;

        public void Update(ReadOnlySpan<byte> shape)
        {
            shape.CopyTo(_shape);
        }

        public override void Render(DrawingContext dc)
        {
            if (UiTransitionGate.IsPaused) return;
            for (int gy = 0; gy < 16; gy++)
            {
                for (int gx = 0; gx < 16; gx++)
                {
                    double x = gx * PixelSize;
                    double y = gy * PixelSize;
                    byte color = GetNibble(_shape, gx, gy);
                    dc.DrawRectangle(PaletteBrushes[color], null, new Rect(x, y, PixelSize, PixelSize));
                    dc.DrawRectangle(GridLineBrush, null, new Rect(x, y, PixelSize, 1));
                    dc.DrawRectangle(GridLineBrush, null, new Rect(x, y, 1, PixelSize));
                }
            }
        }
    }

    /// <summary>
    /// Renders 16 sprite thumbnails in a 4x4 grid using DrawingContext.
    /// Replaces 16 WriteableBitmaps + 16 Border + 16 Image controls.
    /// </summary>
    private sealed class ThumbnailStrip : Control
    {
        private const int Columns = 4;
        private const int Rows = 4;
        private const int ThumbPixels = 16;
        private const int CellSize = 24;
        private const int BorderWidth = 2;
        private const int CellStride = CellSize + BorderWidth * 2 + 2; // +2 for margin

        private readonly byte[][] _shapeCache = new byte[16][];
        private readonly bool[] _enabledFlags = new bool[16];
        private int _selectedIndex;
        private readonly VirtualGraphicsController _vgc;

        private static readonly IBrush DarkGrayBrush = new SolidColorBrush(Color.FromRgb(34, 34, 34));
        private static readonly IBrush TransparentPixelBrush = new SolidColorBrush(Color.FromRgb(0x22, 0x22, 0x22));
        private static readonly IPen YellowPen = new Pen(Brushes.Yellow, 2);
        private static readonly IPen GreenPen = new Pen(Brushes.Green, 2);

        public event Action<int>? SpriteSelected;

        public ThumbnailStrip(VirtualGraphicsController vgc)
        {
            _vgc = vgc;
            for (int i = 0; i < 16; i++)
                _shapeCache[i] = new byte[VgcConstants.SpriteShapeSize];

            Width = Columns * CellStride;
            Height = Rows * CellStride;
            Cursor = new Cursor(StandardCursorType.Hand);
        }

        public byte[] GetShapeCache(int index) => _shapeCache[index];

        public void Update(int index, byte[] shape)
        {
            Buffer.BlockCopy(shape, 0, _shapeCache[index], 0, shape.Length);
        }

        public void SetSelection(int selectedIndex, bool[] enabledFlags)
        {
            _selectedIndex = selectedIndex;
            Array.Copy(enabledFlags, _enabledFlags, 16);
        }

        protected override void OnPointerPressed(PointerPressedEventArgs e)
        {
            var pos = e.GetPosition(this);
            int col = (int)(pos.X / CellStride);
            int row = (int)(pos.Y / CellStride);
            if ((uint)col < Columns && (uint)row < Rows)
            {
                int index = row * Columns + col;
                SpriteSelected?.Invoke(index);
            }
            e.Handled = true;
        }

        public override void Render(DrawingContext dc)
        {
            if (UiTransitionGate.IsPaused) return;

            double pixelW = (double)CellSize / ThumbPixels;
            double pixelH = (double)CellSize / ThumbPixels;

            for (int i = 0; i < 16; i++)
            {
                int col = i % Columns;
                int row = i / Columns;
                double cellX = col * CellStride + BorderWidth + 1; // +1 for margin
                double cellY = row * CellStride + BorderWidth + 1;

                // Background
                dc.DrawRectangle(DarkGrayBrush, null, new Rect(cellX, cellY, CellSize, CellSize));

                // Sprite pixels
                var shape = _shapeCache[i];
                for (int sy = 0; sy < ThumbPixels; sy++)
                {
                    for (int sx = 0; sx < ThumbPixels; sx++)
                    {
                        byte color = GetNibble(shape, sx, sy);
                        var brush = color == 0 ? TransparentPixelBrush : PaletteBrushes[color];
                        dc.DrawRectangle(brush, null,
                            new Rect(cellX + sx * pixelW, cellY + sy * pixelH, pixelW, pixelH));
                    }
                }

                // Border highlight
                IPen? pen = null;
                if (i == _selectedIndex)
                    pen = YellowPen;
                else if (_enabledFlags[i])
                    pen = GreenPen;

                if (pen != null)
                {
                    dc.DrawRectangle(null, pen,
                        new Rect(cellX - BorderWidth, cellY - BorderWidth,
                            CellSize + BorderWidth * 2, CellSize + BorderWidth * 2));
                }
            }
        }
    }

    /// <summary>
    /// Renders a single sprite preview with flip support using DrawingContext.
    /// Replaces 1 WriteableBitmap + 1 Image.
    /// </summary>
    private sealed class SpritePreview : Control
    {
        private byte[] _shape = new byte[VgcConstants.SpriteShapeSize];
        private bool _flipH, _flipV;

        public void Update(byte[] shape, bool flipH, bool flipV)
        {
            Buffer.BlockCopy(shape, 0, _shape, 0, shape.Length);
            _flipH = flipH;
            _flipV = flipV;
        }

        public override void Render(DrawingContext dc)
        {
            if (UiTransitionGate.IsPaused) return;

            double w = Bounds.Width;
            double h = Bounds.Height;
            double pixelW = w / 16;
            double pixelH = h / 16;

            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    int srcX = _flipH ? 15 - x : x;
                    int srcY = _flipV ? 15 - y : y;
                    byte color = GetNibble(_shape, srcX, srcY);
                    dc.DrawRectangle(PaletteBrushes[color], null,
                        new Rect(x * pixelW, y * pixelH, pixelW, pixelH));
                }
            }
        }
    }

    /// <summary>
    /// Renders animation preview frames using DrawingContext.
    /// Replaces 1 WriteableBitmap + 1 Image.
    /// </summary>
    private sealed class AnimationPreview : Control
    {
        private byte[] _shape = new byte[VgcConstants.SpriteShapeSize];
        private static readonly IBrush TransparentPixelBrush = new SolidColorBrush(Color.FromRgb(0x22, 0x22, 0x22));

        public void Update(byte[] shape)
        {
            Buffer.BlockCopy(shape, 0, _shape, 0, shape.Length);
        }

        public override void Render(DrawingContext dc)
        {
            if (UiTransitionGate.IsPaused) return;

            double w = Bounds.Width;
            double h = Bounds.Height;
            double pixelW = w / 16;
            double pixelH = h / 16;

            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    byte color = GetNibble(_shape, x, y);
                    var brush = color == 0 ? TransparentPixelBrush : PaletteBrushes[color];
                    dc.DrawRectangle(brush, null,
                        new Rect(x * pixelW, y * pixelH, pixelW, pixelH));
                }
            }
        }
    }
}
