using e6502.GameServer.Protocol;

namespace e6502.GameServer.Server;

public interface IGameClientSession
{
    ushort SessionId { get; }
    string? Handle { get; set; }

    ValueTask SendAsync(ProtocolMessage message, CancellationToken cancellationToken);
}

public sealed record GameDescriptor(
    ushort Id,
    string Code,
    string DisplayName,
    byte ProtocolMajor,
    byte ProtocolMinor,
    ushort Capabilities,
    byte MinPlayers,
    byte MaxPlayers);

public sealed record AiDescriptor(
    ushort Id,
    ushort GameId,
    string Code,
    string DisplayName,
    byte GameProtocolMajor,
    byte GameProtocolMinor,
    ushort Capabilities);

public sealed record OpponentDescriptor(
    OpponentKind Kind,
    ushort Id,
    ushort GameId,
    string Code,
    string DisplayName,
    byte GameProtocolMajor,
    byte GameProtocolMinor,
    ushort Capabilities);

public enum TableState : byte
{
    Waiting = 0,
    Playing = 1,
    Finished = 2
}

public enum TableJoinRole : byte
{
    Player = 0,
    Observer = 1
}

public sealed class GameTable
{
    private readonly List<IGameClientSession> _players = [];
    private readonly List<IGameClientSession> _observers = [];

    public GameTable(ushort id, GameDescriptor game, string name)
    {
        Id = id;
        Game = game;
        Name = name;
    }

    public ushort Id { get; }
    public GameDescriptor Game { get; }
    public string Name { get; }
    public TableState State { get; private set; } = TableState.Waiting;
    public object? AdapterState { get; set; }
    public IReadOnlyList<IGameClientSession> Players => _players;
    public IReadOnlyList<IGameClientSession> Observers => _observers;
    public bool IsEmpty => _players.Count == 0 && _observers.Count == 0;

    public bool HasParticipant(IGameClientSession session) =>
        _players.Contains(session) || _observers.Contains(session);

    public bool HasPlayer(IGameClientSession session) => _players.Contains(session);

    public bool TryAddPlayer(IGameClientSession session)
    {
        if (_players.Contains(session))
            return true;
        if (_players.Count >= Game.MaxPlayers)
            return false;

        _observers.Remove(session);
        _players.Add(session);
        if (_players.Count >= Game.MinPlayers)
            State = TableState.Playing;
        return true;
    }

    public void AddObserver(IGameClientSession session)
    {
        if (!_observers.Contains(session) && !_players.Contains(session))
            _observers.Add(session);
    }

    public void Remove(IGameClientSession session)
    {
        _players.Remove(session);
        _observers.Remove(session);
        if (State == TableState.Playing && _players.Count < Game.MinPlayers)
            State = TableState.Waiting;
    }

    public IReadOnlyList<IGameClientSession> SnapshotRecipients() =>
        _players.Concat(_observers).ToArray();
}

public sealed record GameActionResult(
    bool Accepted,
    ProtocolErrorCode ErrorCode,
    string? ErrorText,
    byte[] EventPayload)
{
    public static GameActionResult Accept(ReadOnlySpan<byte> eventPayload = default) =>
        new(true, ProtocolErrorCode.Unknown, null, eventPayload.ToArray());

    public static GameActionResult Reject(ProtocolErrorCode code, string errorText) =>
        new(false, code, errorText, []);
}

public sealed record GameStateResult(
    bool Accepted,
    ProtocolErrorCode ErrorCode,
    string? ErrorText,
    byte[] StatePayload)
{
    public static GameStateResult Accept(ReadOnlySpan<byte> statePayload = default) =>
        new(true, ProtocolErrorCode.Unknown, null, statePayload.ToArray());

    public static GameStateResult Reject(ProtocolErrorCode code, string errorText) =>
        new(false, code, errorText, []);
}
