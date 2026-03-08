namespace e6502.Avalonia.Editor;

public interface ISyntaxHighlighter
{
    byte[] HighlightLine(string line);
    void Reset();
}
