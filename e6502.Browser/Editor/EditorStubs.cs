// Minimal stubs for editor types referenced by linked EmulatorCanvas.cs.
// No editors are used in the browser version — these exist only for compilation.

using Avalonia.Input;

namespace e6502.Avalonia.Editor;

public enum EditorMode { Normal, Running, Debug }

public abstract class ScreenTextEditor
{
    public virtual bool IsActive => false;
    public virtual EditorMode Mode => EditorMode.Normal;
    public virtual bool HandleKeyDown(Key key, KeyModifiers modifiers) => false;
    public virtual void HandleTextInput(string text) { }
}

public class NccEditor : ScreenTextEditor { }
public class BasicEditor : ScreenTextEditor
{
    public void ToggleActivation() { }
}
