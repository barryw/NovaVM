using e6502.GameServer.Protocol;

namespace e6502.GameServer.Server;

public sealed class ChessGameAdapter : IGameAdapter
{
    public const ushort GameId = 2;
    public const byte ProtocolMajor = 1;
    public const byte ProtocolMinor = 0;

    public const byte ActionMove = 1;
    public const byte ActionResign = 2;
    public const byte ActionOfferDraw = 3;

    private const int MaxMoveHistory = 48;

    public GameDescriptor Descriptor { get; } = new(
        Id: GameId,
        Code: "chess",
        DisplayName: "Chess",
        ProtocolMajor: ProtocolMajor,
        ProtocolMinor: ProtocolMinor,
        Capabilities: 0x0001,
        MinPlayers: 2,
        MaxPlayers: 2);

    public GameActionResult HandleAction(
        GameTable table,
        IGameClientSession actor,
        byte actionType,
        ReadOnlyMemory<byte> actionPayload)
    {
        if (actionType != ActionMove)
            return GameActionResult.Reject(ProtocolErrorCode.BadAction, "unsupported chess action");

        if (actionPayload.Length < 2)
            return GameActionResult.Reject(ProtocolErrorCode.BadAction, "bad chess move");

        byte from = actionPayload.Span[0];
        byte to = actionPayload.Span[1];
        if (!IsValidSquare(from) || !IsValidSquare((byte)(to & 0x7F)))
            return GameActionResult.Reject(ProtocolErrorCode.BadAction, "invalid chess square");

        var state = GetState(table);
        byte? actorSide = GetSideForPlayer(table, actor);
        if (actorSide is null || actorSide.Value != state.SideToMove)
            return GameActionResult.Reject(ProtocolErrorCode.BadAction, "not your turn");

        byte side = state.SideToMove;
        state.Ply++;
        state.SideToMove = (byte)(state.SideToMove == 0 ? 1 : 0);
        state.LastFrom = from;
        state.LastTo = to;
        state.Moves.Add(new ChessMoveRecord(state.Ply, side, from, to));
        if (state.Moves.Count > MaxMoveHistory)
            state.Moves.RemoveAt(0);

        var writer = new ProtocolWriter();
        writer.WriteString(actor.Handle ?? string.Empty);
        writer.WriteUInt16(state.Ply);
        writer.WriteByte(side);
        writer.WriteByte(from);
        writer.WriteByte(to);
        return GameActionResult.Accept(writer.ToArray());
    }

    public GameStateResult BuildState(GameTable table, IGameClientSession requester)
    {
        var state = GetState(table);
        var writer = new ProtocolWriter();
        writer.WriteByte(1);
        writer.WriteUInt16(state.Ply);
        writer.WriteByte(state.SideToMove);
        writer.WriteByte(state.LastFrom);
        writer.WriteByte(state.LastTo);
        writer.WriteByte((byte)state.Moves.Count);
        foreach (var move in state.Moves)
        {
            writer.WriteUInt16(move.Ply);
            writer.WriteByte(move.Side);
            writer.WriteByte(move.From);
            writer.WriteByte(move.To);
        }

        return GameStateResult.Accept(writer.ToArray());
    }

    public static ChessTableState GetState(GameTable table)
    {
        if (table.AdapterState is ChessTableState state)
            return state;

        state = new ChessTableState();
        table.AdapterState = state;
        return state;
    }

    public static byte? GetSideForPlayer(GameTable table, IGameClientSession session)
    {
        for (int i = 0; i < table.Players.Count; i++)
        {
            if (ReferenceEquals(table.Players[i], session))
                return i == 0 ? (byte)1 : (byte)0;
        }

        return null;
    }

    private static bool IsValidSquare(byte square) => (square & 0x88) == 0;

    public sealed class ChessTableState
    {
        public ushort Ply { get; set; }
        public byte SideToMove { get; set; } = 1;
        public byte LastFrom { get; set; } = 0xFF;
        public byte LastTo { get; set; } = 0xFF;
        public List<ChessMoveRecord> Moves { get; } = [];
    }

    public sealed record ChessMoveRecord(ushort Ply, byte Side, byte From, byte To);
}
