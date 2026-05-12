namespace e6502.GameServer.Server;

public interface IAiProvider
{
    AiDescriptor Descriptor { get; }

    ValueTask<GameActionRequest?> BuildActionAsync(
        GameTable table,
        IGameClientSession aiSession,
        CancellationToken cancellationToken);
}

public sealed record GameActionRequest(byte ActionType, byte[] Payload);
