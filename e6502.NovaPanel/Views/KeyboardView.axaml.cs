using System;
using System.Collections.Generic;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Layout;
using Avalonia.VisualTree;
using e6502.NovaPanel.Input;

namespace e6502.NovaPanel.Views;

public partial class KeyboardView : UserControl
{
    // Single code path for activation (shared by click handlers and ActivateForTest).
    private KeyModifiers _latched = KeyModifiers.None;

    // Key → button map: seam for the next task (physical-key highlight).
    // Populated during build; ActivateForTest also uses it.
    private readonly Dictionary<Key, Control> _keyToControl = new();

    public event Action<string>? KeyActivated;

    public KeyboardView()
    {
        Focusable = true;
        InitializeComponent();
        BuildKeyboard();
    }

    // -----------------------------------------------------------------------
    // Focus the view as soon as it is part of the visual tree.
    // -----------------------------------------------------------------------
    protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
    {
        base.OnAttachedToVisualTree(e);
        Focus();
    }

    // -----------------------------------------------------------------------
    // Physical keyboard: key-down handling
    // -----------------------------------------------------------------------
    protected override void OnKeyDown(KeyEventArgs e)
    {
        KeyModifiers effective = e.KeyModifiers | _latched;
        string? token = KeyDispatch.Resolve(e.Key, effective);
        if (token is not null)
        {
            KeyActivated?.Invoke(token);
            e.Handled = true;
            if (_keyToControl.TryGetValue(e.Key, out var ctl) && !ctl.Classes.Contains("active"))
                ctl.Classes.Add("active");
        }
        // Unresolved keys bubble (don't call base to avoid double-dispatch; just don't set Handled).
    }

    // -----------------------------------------------------------------------
    // Physical keyboard: key-up handling — clear highlight
    // -----------------------------------------------------------------------
    protected override void OnKeyUp(KeyEventArgs e)
    {
        if (_keyToControl.TryGetValue(e.Key, out var ctl))
            ctl.Classes.Remove("active");
        base.OnKeyUp(e);
    }

    // -----------------------------------------------------------------------
    // Test helper: returns true while the physical key is highlighted.
    // -----------------------------------------------------------------------
    public bool IsKeyActive(Key key) =>
        _keyToControl.TryGetValue(key, out var c) && c.Classes.Contains("active");

    // -----------------------------------------------------------------------
    // Public test-hook: mirrors exactly what a click does.
    // -----------------------------------------------------------------------
    public void ActivateForTest(Key key)
    {
        if (!_keyToControl.TryGetValue(key, out var ctrl))
            return;

        // Resolve the KeyCap from the control's Tag.
        if (ctrl.Tag is not KeyCap cap)
            return;

        OnCapActivated(cap);
    }

    // -----------------------------------------------------------------------
    // Single activation path
    // -----------------------------------------------------------------------
    private void OnCapActivated(KeyCap cap)
    {
        if (cap.Kind == KeyKind.Modifier)
        {
            KeyModifiers flag = ModifierFlagFor(cap.Key);
            _latched ^= flag;

            // Keep the ToggleButton visual in sync when driven by ActivateForTest.
            if (_keyToControl.TryGetValue(cap.Key, out var ctrl) && ctrl is ToggleButton tb)
                tb.IsChecked = _latched.HasFlag(flag);

            return;
        }

        string? token = KeyDispatch.Resolve(cap.Key, _latched);
        if (token is not null)
            KeyActivated?.Invoke(token);
    }

    // -----------------------------------------------------------------------
    // Build the key grid from KeyboardLayout.Rows
    // -----------------------------------------------------------------------
    private void BuildKeyboard()
    {
        foreach (var row in KeyboardLayout.Rows)
        {
            var rowGrid = new Grid { Height = 38 };

            // One star-sized column per cap — proportional width.
            foreach (var cap in row)
                rowGrid.ColumnDefinitions.Add(new ColumnDefinition(cap.WidthUnits, GridUnitType.Star));

            int col = 0;
            foreach (var cap in row)
            {
                Control ctrl = BuildCapControl(cap);
                Grid.SetColumn(ctrl, col++);
                rowGrid.Children.Add(ctrl);
            }

            RootRows.Children.Add(rowGrid);
        }
    }

    private Control BuildCapControl(KeyCap cap)
    {
        Control ctrl;

        if (cap.Kind == KeyKind.Modifier)
        {
            var tb = new ToggleButton
            {
                Content = cap.Label,
                Tag = cap,
            };
            tb.Classes.Add("mod-key");
            tb.Click += (_, _) => OnCapActivated(cap);
            ctrl = tb;
        }
        else
        {
            var btn = new Button
            {
                Content = cap.Label,
                Tag = cap,
            };
            btn.Classes.Add("key");
            btn.Click += (_, _) => OnCapActivated(cap);
            ctrl = btn;
        }

        // Register all keys; last one wins if there are duplicates (e.g. both Shifts).
        // For the highlight seam we register every key so either Shift lights up.
        _keyToControl[cap.Key] = ctrl;

        return ctrl;
    }

    // -----------------------------------------------------------------------
    // Modifier flag mapping
    // -----------------------------------------------------------------------
    private static KeyModifiers ModifierFlagFor(Key key) => key switch
    {
        Key.LeftShift  or Key.RightShift => KeyModifiers.Shift,
        Key.LeftCtrl   or Key.RightCtrl  => KeyModifiers.Control,
        Key.LeftAlt    or Key.RightAlt   => KeyModifiers.Alt,
        _                                => KeyModifiers.None,
    };
}
