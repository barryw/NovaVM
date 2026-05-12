namespace e6502.GameServer.Protocol;

public static class NovaGameProtocol
{
    public const byte ServerProtocolMajor = 1;
    public const byte ServerProtocolMinor = 0;
    public const int DefaultPort = 6503;

    public const int MaxFramePayload = 256;
    public const int EnvelopeSize = 4;
    public const int MaxMessagePayload = MaxFramePayload - EnvelopeSize;
}

public enum MessageKind : byte
{
    Hello = 0x01,
    Welcome = 0x02,
    Error = 0x03,
    Ping = 0x04,
    Pong = 0x05,

    LobbyChat = 0x10,
    UserListRequest = 0x11,
    UserListItem = 0x12,
    UserJoined = 0x13,
    UserLeft = 0x14,
    TableChat = 0x15,

    GameListRequest = 0x20,
    GameListItem = 0x21,

    TableListRequest = 0x30,
    TableListItem = 0x31,
    TableCreate = 0x32,
    TableJoin = 0x33,
    TableLeave = 0x34,
    TableObserve = 0x35,
    TableCreated = 0x36,
    TableJoined = 0x37,
    TableLeft = 0x38,

    TableEvent = 0x40,
    PlayerAction = 0x41,
    StateRequest = 0x42,
    StateChunk = 0x43,

    AiListRequest = 0x50,
    AiListItem = 0x51,
    AiAddToTable = 0x52,
    OpponentListRequest = 0x53,
    OpponentListItem = 0x54
}

[Flags]
public enum MessageFlags : byte
{
    None = 0x00,
    Response = 0x01,
    Error = 0x02,
    More = 0x04,
    Event = 0x08
}

public enum ProtocolErrorCode : byte
{
    Unknown = 0x00,
    BadMessage = 0x01,
    UnsupportedProtocol = 0x02,
    HandleInUse = 0x03,
    NotRegistered = 0x04,
    UnknownGame = 0x05,
    UnknownTable = 0x06,
    BadAction = 0x07,
    TableFull = 0x08,
    UnsupportedMessage = 0x09,
    InvalidHandle = 0x0A,
    NotSeated = 0x0B,
    NotParticipant = 0x0C,
    UnknownAiProvider = 0x0D,
    Timeout = 0x0E
}

public enum OpponentKind : byte
{
    Human = 0,
    Ai = 1
}

public sealed record ProtocolMessage(
    byte Version,
    MessageKind Kind,
    byte Sequence,
    MessageFlags Flags,
    byte[] Payload)
{
    public static ProtocolMessage Create(
        MessageKind kind,
        byte sequence,
        MessageFlags flags = MessageFlags.None,
        ReadOnlySpan<byte> payload = default)
    {
        if (payload.Length > NovaGameProtocol.MaxMessagePayload)
            throw new ArgumentOutOfRangeException(nameof(payload), "Payload is too large for one Nova NIC frame.");

        return new ProtocolMessage(
            NovaGameProtocol.ServerProtocolMajor,
            kind,
            sequence,
            flags,
            payload.ToArray());
    }
}
