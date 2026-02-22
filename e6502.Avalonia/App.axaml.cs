using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Controls.Presenters;
using Avalonia.Controls.Templates;
using Avalonia.Markup.Xaml;
using Dock.Avalonia.Themes.Fluent;
using e6502.Avalonia.DevTools;

namespace e6502.Avalonia;

public partial class App : Application
{
    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
        Styles.Add(new DockFluentTheme());
    }

    public override void OnFrameworkInitializationCompleted()
    {
        DataTemplates.Add(new FuncDataTemplate<EmulatorDocument>((doc, _) =>
        {
            if (doc?.Canvas is not { } canvas)
                return new TextBlock { Text = "No canvas" };
            DetachFromVisualParent(canvas);
            return canvas;
        }));
        DataTemplates.Add(new FuncDataTemplate<SpriteEditorTool>((tool, _) =>
        {
            var view = tool?.GetOrCreateView();
            if (view?.Parent is not null)
                DetachFromVisualParent(view);
            return view;
        }, supportsRecycling: false));

        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            desktop.MainWindow = new MainWindow();

        base.OnFrameworkInitializationCompleted();
    }

    internal static void DetachFromVisualParent(Control control)
    {
        if (control.Parent is ContentPresenter cp)
            cp.Content = null;
        else if (control.Parent is ContentControl cc)
            cc.Content = null;
        else if (control.Parent is Decorator decorator)
            decorator.Child = null;
        else if (control.Parent is Panel panel)
            panel.Children.Remove(control);
    }
}
