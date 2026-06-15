namespace e6502.NovaPanel.Layout;

public enum SizeClass
{
    Compact, // keyboard-only; panels collapsed (reachable via overlays)
    Wide,    // + side rail (status/drives/runtime)
    Full     // + bottom region (disk images/library/modules)
}
