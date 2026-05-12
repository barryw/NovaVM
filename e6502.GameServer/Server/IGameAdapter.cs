namespace e6502.GameServer.Server;

public interface IGameAdapter
{
    GameDescriptor Descriptor { get; }

    GameActionResult HandleAction(
        GameTable table,
        IGameClientSession actor,
        byte actionType,
        ReadOnlyMemory<byte> actionPayload);

    GameStateResult BuildState(GameTable table, IGameClientSession requester);
}
