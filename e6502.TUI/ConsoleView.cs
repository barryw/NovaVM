using System.Text;
using Terminal.Gui;

namespace e6502.TUI;

public class ConsoleView : View
{
    private const string _cursor = "\u2588";
    private byte[] _contents = new byte[0xfa0]; // 2000 bytes for screen memory
    private int _cursorX;
    private int _cursorY;

    public ConsoleView(int x, int y, int width, int height)
    {
        X = x;
        Y = y;
        Width = width;
        Height = height;

        _cursorX = 0;
        _cursorY = 0;

        Encoding.UTF8.GetBytes(_cursor).CopyTo(_contents, ScreenLocationToBytePosition());
        ContentsToString();
    }

    public void AppendCharacter(char character)
    {
        _contents[ScreenLocationToBytePosition()] = System.Text.Encoding.UTF8.GetBytes(character.ToString())[0];
        _cursorX++;
        if (_cursorX == 80)
        {
            _cursorX = 0;
            _cursorY++;
            if (_cursorY == 25)
                ScrollScreen();
        }

        ContentsToString();
    }

    private int ScreenLocationToBytePosition()
    {
        return (_cursorY * 80) + _cursorX;
    }

    /// <summary>
    /// Convert screen RAM to a string and set the Text property
    /// </summary>
    private void ContentsToString()
    {
        Text = BitConverter.ToString(_contents);
    }

    /// <summary>
    /// Scroll the screen up by 80 characters
    /// </summary>
    private void ScrollScreen()
    {

    }
}
