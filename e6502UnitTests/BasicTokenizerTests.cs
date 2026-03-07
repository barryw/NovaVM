using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace e6502UnitTests;

[TestClass]
public class BasicTokenizerTests
{
    private static BasicTokenizer LoadTokenizer()
    {
        // Search upward from test output directory for ehbasic/tokens.json
        string dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
            if (File.Exists(candidate))
                return BasicTokenizer.FromJsonFile(candidate);
            string? parent = Path.GetDirectoryName(dir);
            if (parent is null || parent == dir)
                break;
            dir = parent;
        }
        throw new FileNotFoundException("Could not locate ehbasic/tokens.json");
    }

    [TestMethod]
    public void Tokenize_SimplePrint()
    {
        var tok = LoadTokenizer();
        byte[] result = tok.Tokenize(["10 PRINT \"HELLO\""], 0x0801);

        // Must end with $00 $00
        Assert.IsTrue(result.Length >= 2, "Result too short");
        Assert.AreEqual(0x00, result[result.Length - 2], "Second-to-last byte must be $00");
        Assert.AreEqual(0x00, result[result.Length - 1], "Last byte must be $00");

        // Must be non-trivial (more than just the terminator)
        Assert.IsTrue(result.Length > 2, "Result should contain at least one line");

        // PRINT token is $9F (159)
        bool hasPrintToken = false;
        foreach (byte b in result)
        {
            if (b == 0x9F) { hasPrintToken = true; break; }
        }
        Assert.IsTrue(hasPrintToken, "PRINT token (0x9F) not found in output");
    }

    [TestMethod]
    public void Detokenize_RoundTrip()
    {
        var tok = LoadTokenizer();
        string[] lines =
        [
            "10 PRINT \"HELLO\"",
            "20 GOTO 10"
        ];

        byte[] tokenized = tok.Tokenize(lines, 0x0801);
        string[] restored = tok.Detokenize(tokenized);

        Assert.AreEqual(lines.Length, restored.Length, "Line count mismatch");
        Assert.AreEqual(lines[0], restored[0], "Line 10 mismatch");
        Assert.AreEqual(lines[1], restored[1], "Line 20 mismatch");
    }

    [TestMethod]
    public void Tokenize_StringsNotTokenized()
    {
        var tok = LoadTokenizer();
        // Keywords inside quotes must NOT be tokenized
        byte[] result = tok.Tokenize(["10 PRINT \"PRINT GOTO\""], 0x0801);

        // Find the string content — locate the opening quote byte
        // The bytes after the PRINT token + space + quote should be literal ASCII
        // "PRINT GOTO" in ASCII: P=0x50, R=0x52, I=0x49, N=0x4E, T=0x54, ...
        // Verify 0x50 ('P') appears literally in the output
        // and that there is only ONE PRINT token (0x9F), not two
        int printTokenCount = 0;
        foreach (byte b in result)
        {
            if (b == 0x9F) printTokenCount++;
        }
        Assert.AreEqual(1, printTokenCount, "PRINT inside quotes should not be tokenized");
    }

    [TestMethod]
    public void Tokenize_GosubBeforeGo()
    {
        var tok = LoadTokenizer();
        // GOSUB token is $8D (141), GOTO is $89 (137)
        // "GOSUB 100" must tokenize as GOSUB, not GO + SUB (SUB isn't even a keyword)
        byte[] result = tok.Tokenize(["10 GOSUB 100"], 0x0801);

        bool hasGosub = false;
        foreach (byte b in result)
        {
            if (b == 0x8D) { hasGosub = true; break; }
        }
        Assert.IsTrue(hasGosub, "GOSUB token (0x8D) not found; longest-match failed");

        // Also confirm GOTO token does not appear (GOTO = 0x89)
        bool hasGoto = false;
        foreach (byte b in result)
        {
            if (b == 0x89) { hasGoto = true; break; }
        }
        Assert.IsFalse(hasGoto, "GOTO token should not appear when input is GOSUB");
    }

    [TestMethod]
    public void Tokenize_EmptyProgram()
    {
        var tok = LoadTokenizer();
        byte[] result = tok.Tokenize([], 0x0801);

        Assert.AreEqual(2, result.Length, "Empty program should be exactly [$00][$00]");
        Assert.AreEqual(0x00, result[0]);
        Assert.AreEqual(0x00, result[1]);
    }

    [TestMethod]
    public void Tokenize_RemRestIsLiteral()
    {
        var tok = LoadTokenizer();
        // REM token is $91 (145); everything after it must be literal bytes
        byte[] result = tok.Tokenize(["10 REM THIS IS+A-COMMENT"], 0x0801);

        // Locate the REM token (0x91)
        int remIndex = -1;
        for (int i = 0; i < result.Length; i++)
        {
            if (result[i] == 0x91) { remIndex = i; break; }
        }
        Assert.IsTrue(remIndex >= 0, "REM token (0x91) not found");

        // After REM, rest should be literal ASCII " THIS IS+A-COMMENT"
        // Find $00 terminator after remIndex
        int termIndex = remIndex + 1;
        while (termIndex < result.Length && result[termIndex] != 0x00)
            termIndex++;

        string literal = System.Text.Encoding.ASCII.GetString(result, remIndex + 1, termIndex - remIndex - 1);
        Assert.AreEqual(" THIS IS+A-COMMENT", literal, "Comment body should be literal");
    }

    [TestMethod]
    public void Tokenize_ExtendedToken()
    {
        var tok = LoadTokenizer();
        // From tokens.json: prefix_byte=1, extended id 1 = "DIR"
        // "10 DIR" should emit: ... [prefix_byte=0x01] [0x01] ...
        byte[] result = tok.Tokenize(["10 DIR"], 0x0801);

        // Find prefix byte (0x01) followed by extended id (0x01)
        bool found = false;
        for (int i = 0; i < result.Length - 1; i++)
        {
            if (result[i] == 0x01 && result[i + 1] == 0x01)
            {
                found = true;
                break;
            }
        }
        Assert.IsTrue(found, "Extended token prefix+id (0x01 0x01) not found for DIR");
    }

    [TestMethod]
    public void Tokenize_NoLineNumber_ThrowsFormatException()
    {
        var tok = LoadTokenizer();
        Assert.ThrowsException<FormatException>(() =>
            tok.Tokenize(new[] { "PRINT \"HELLO\"" }, 0x0801));
    }

    [TestMethod]
    public void Tokenize_EmptyLine_ThrowsFormatException()
    {
        var tok = LoadTokenizer();
        Assert.ThrowsException<FormatException>(() =>
            tok.Tokenize(new[] { "" }, 0x0801));
    }
}
