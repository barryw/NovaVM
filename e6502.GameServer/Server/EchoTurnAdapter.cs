using e6502.GameServer.Protocol;

namespace e6502.GameServer.Server;

public sealed class EchoTurnAdapter : IGameAdapter
{
    public GameDescriptor Descriptor { get; } = new(
        Id: 1,
        Code: "echo",
        DisplayName: "Echo Turn",
        ProtocolMajor: 1,
        ProtocolMinor: 0,
        Capabilities: 0,
        MinPlayers: 1,
        MaxPlayers: 8);

    public GameActionResult HandleAction(
        GameTable table,
        IGameClientSession actor,
        byte actionType,
        ReadOnlyMemory<byte> actionPayload)
    {
        var writer = new ProtocolWriter();
        writer.WriteString(actor.Handle ?? string.Empty);
        writer.WriteByte(actionType);
        writer.WriteBytes(actionPayload.Span);
        return GameActionResult.Accept(writer.ToArray());
    }

    public GameStateResult BuildState(GameTable table, IGameClientSession requester)
    {
        var writer = new ProtocolWriter();
        writer.WriteByte((byte)table.State);
        writer.WriteByte((byte)table.Players.Count);
        writer.WriteByte((byte)table.Observers.Count);
        return GameStateResult.Accept(writer.ToArray());
    }
}
