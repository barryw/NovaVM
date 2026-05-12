using System.Diagnostics;

namespace e6502.GameServer.Server;

public sealed class StockfishAiProvider : IAiProvider
{
    public const ushort ProviderId = 1;

    private readonly string _stockfishPath;
    private readonly int _moveTimeMs;

    public StockfishAiProvider(string? stockfishPath = null, int? moveTimeMs = null)
    {
        _stockfishPath = stockfishPath
            ?? Environment.GetEnvironmentVariable("STOCKFISH_PATH")
            ?? "stockfish";

        _moveTimeMs = moveTimeMs
            ?? ParseMoveTime(Environment.GetEnvironmentVariable("STOCKFISH_MOVETIME_MS"))
            ?? 500;
    }

    public AiDescriptor Descriptor { get; } = new(
        Id: ProviderId,
        GameId: ChessGameAdapter.GameId,
        Code: "stockfish",
        DisplayName: "Stockfish",
        GameProtocolMajor: ChessGameAdapter.ProtocolMajor,
        GameProtocolMinor: ChessGameAdapter.ProtocolMinor,
        Capabilities: 0x0001);

    public async ValueTask<GameActionRequest?> BuildActionAsync(
        GameTable table,
        IGameClientSession aiSession,
        CancellationToken cancellationToken)
    {
        if (table.Game.Id != ChessGameAdapter.GameId)
            return null;

        var state = ChessGameAdapter.GetState(table);
        byte? aiSide = ChessGameAdapter.GetSideForPlayer(table, aiSession);
        if (aiSide is null || aiSide.Value != state.SideToMove)
            return null;

        string[] moves = state.Moves.Select(move => ToUciMove(move.From, move.To)).ToArray();
        string? bestMove = await GetBestMoveAsync(moves, cancellationToken);
        if (bestMove is null || !TryParseUciMove(bestMove, out byte from, out byte to))
            return null;

        return new GameActionRequest(ChessGameAdapter.ActionMove, [from, to]);
    }

    private async Task<string?> GetBestMoveAsync(IReadOnlyList<string> moves, CancellationToken cancellationToken)
    {
        using var process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = _stockfishPath,
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            }
        };

        try
        {
            process.Start();
        }
        catch
        {
            return null;
        }

        try
        {
            await SendAsync(process, "uci", cancellationToken);
            if (!await ReadUntilAsync(process, line => line == "uciok", cancellationToken))
                return null;

            await SendAsync(process, "isready", cancellationToken);
            if (!await ReadUntilAsync(process, line => line == "readyok", cancellationToken))
                return null;

            await SendAsync(process, "ucinewgame", cancellationToken);
            string position = moves.Count == 0
                ? "position startpos"
                : $"position startpos moves {string.Join(' ', moves)}";
            await SendAsync(process, position, cancellationToken);
            await SendAsync(process, $"go movetime {_moveTimeMs}", cancellationToken);

            while (!cancellationToken.IsCancellationRequested)
            {
                string? line = await process.StandardOutput.ReadLineAsync(cancellationToken);
                if (line is null)
                    return null;

                if (!line.StartsWith("bestmove ", StringComparison.Ordinal))
                    continue;

                string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                return parts.Length >= 2 ? parts[1] : null;
            }

            return null;
        }
        finally
        {
            TryQuit(process);
        }
    }

    private static async Task SendAsync(Process process, string command, CancellationToken cancellationToken)
    {
        await process.StandardInput.WriteLineAsync(command.AsMemory(), cancellationToken);
        await process.StandardInput.FlushAsync(cancellationToken);
    }

    private static async Task<bool> ReadUntilAsync(
        Process process,
        Func<string, bool> predicate,
        CancellationToken cancellationToken)
    {
        while (!cancellationToken.IsCancellationRequested)
        {
            string? line = await process.StandardOutput.ReadLineAsync(cancellationToken);
            if (line is null)
                return false;
            if (predicate(line))
                return true;
        }

        return false;
    }

    private static void TryQuit(Process process)
    {
        try
        {
            if (!process.HasExited)
            {
                process.StandardInput.WriteLine("quit");
                process.WaitForExit(500);
            }
        }
        catch
        {
        }

        try
        {
            if (!process.HasExited)
                process.Kill(entireProcessTree: true);
        }
        catch
        {
        }
    }

    private static string ToUciMove(byte from, byte to)
    {
        byte cleanTo = (byte)(to & 0x7F);
        Span<char> chars = stackalloc char[5];
        WriteSquare(from, chars);
        WriteSquare(cleanTo, chars[2..]);
        if ((to & 0x80) != 0)
        {
            chars[4] = 'n';
            return new string(chars);
        }

        return new string(chars[..4]);
    }

    private static void WriteSquare(byte square, Span<char> output)
    {
        output[0] = (char)('a' + (square & 0x07));
        output[1] = (char)('8' - (square >> 4));
    }

    private static bool TryParseUciMove(string move, out byte from, out byte to)
    {
        from = 0;
        to = 0;
        if (move.Length < 4)
            return false;

        if (!TryParseSquare(move.AsSpan(0, 2), out from) ||
            !TryParseSquare(move.AsSpan(2, 2), out to))
            return false;

        if (move.Length >= 5 && move[4] == 'n')
            to |= 0x80;

        return true;
    }

    private static bool TryParseSquare(ReadOnlySpan<char> text, out byte square)
    {
        square = 0;
        if (text.Length != 2)
            return false;

        char fileChar = char.ToLowerInvariant(text[0]);
        char rankChar = text[1];
        if (fileChar is < 'a' or > 'h' || rankChar is < '1' or > '8')
            return false;

        int file = fileChar - 'a';
        int row = '8' - rankChar;
        square = (byte)((row << 4) | file);
        return true;
    }

    private static int? ParseMoveTime(string? value)
    {
        if (!int.TryParse(value, out int parsed))
            return null;
        return Math.Clamp(parsed, 50, 30_000);
    }
}
