using e6502.GameServer.Protocol;

namespace e6502.GameServer.Server;

public sealed class GameServerState
{
    private const int MaxHandleLength = 16;
    private const int MaxTableNameLength = 32;
    private const int MaxChatLength = 120;

    private readonly object _gate = new();
    private readonly Dictionary<ushort, IGameClientSession> _sessions = [];
    private readonly Dictionary<string, IGameClientSession> _handles = new(StringComparer.OrdinalIgnoreCase);
    private readonly Dictionary<ushort, IGameAdapter> _adapters = [];
    private readonly Dictionary<ushort, IAiProvider> _aiProviders = [];
    private readonly Dictionary<ushort, GameTable> _tables = [];
    private ushort _nextSessionId = 1;
    private ushort _nextTableId = 1;

    public GameServerState(
        IEnumerable<IGameAdapter>? adapters = null,
        IEnumerable<IAiProvider>? aiProviders = null)
    {
        foreach (var adapter in adapters ?? [new EchoTurnAdapter(), new ChessGameAdapter()])
            _adapters.Add(adapter.Descriptor.Id, adapter);

        foreach (var aiProvider in aiProviders ?? [])
            _aiProviders.Add(aiProvider.Descriptor.Id, aiProvider);
    }

    public ushort AllocateSessionId()
    {
        lock (_gate)
            return _nextSessionId++;
    }

    public void RegisterConnection(IGameClientSession session)
    {
        lock (_gate)
            _sessions[session.SessionId] = session;
    }

    public async ValueTask RemoveConnectionAsync(IGameClientSession session, CancellationToken cancellationToken)
    {
        List<IGameClientSession> recipients;
        string? handle;

        lock (_gate)
        {
            _sessions.Remove(session.SessionId);
            handle = session.Handle;
            if (!string.IsNullOrEmpty(handle))
                _handles.Remove(handle);

            foreach (var table in _tables.Values.ToList())
            {
                table.Remove(session);
                RemoveAiPlayersIfNoHumanPlayersLocked(table);
                if (table.IsEmpty)
                    _tables.Remove(table.Id);
            }

            recipients = _handles.Values.ToList();
            session.Handle = null;
        }

        if (!string.IsNullOrEmpty(handle))
        {
            var writer = new ProtocolWriter();
            writer.WriteString(handle);
            await BroadcastAsync(recipients, ProtocolMessage.Create(
                MessageKind.UserLeft,
                0,
                MessageFlags.Event,
                writer.ToArray()), cancellationToken);
        }
    }

    public async ValueTask HandleMessageAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (message.Version != NovaGameProtocol.ServerProtocolMajor)
        {
            await session.SendAsync(ProtocolMessages.Error(
                message.Sequence,
                ProtocolErrorCode.UnsupportedProtocol,
                "unsupported server protocol"), cancellationToken);
            return;
        }

        switch (message.Kind)
        {
            case MessageKind.Hello:
                await HandleHelloAsync(session, message, cancellationToken);
                break;
            case MessageKind.Ping:
                await session.SendAsync(ProtocolMessage.Create(
                    MessageKind.Pong,
                    message.Sequence,
                    MessageFlags.Response,
                    message.Payload), cancellationToken);
                break;
            case MessageKind.LobbyChat:
                await HandleLobbyChatAsync(session, message, cancellationToken);
                break;
            case MessageKind.TableChat:
                await HandleTableChatAsync(session, message, cancellationToken);
                break;
            case MessageKind.UserListRequest:
                await HandleUserListRequestAsync(session, message.Sequence, cancellationToken);
                break;
            case MessageKind.GameListRequest:
                await HandleGameListRequestAsync(session, message.Sequence, cancellationToken);
                break;
            case MessageKind.TableListRequest:
                await HandleTableListRequestAsync(session, message, cancellationToken);
                break;
            case MessageKind.TableCreate:
                await HandleTableCreateAsync(session, message, cancellationToken);
                break;
            case MessageKind.TableJoin:
            case MessageKind.TableObserve:
                await HandleTableJoinAsync(session, message, cancellationToken);
                break;
            case MessageKind.TableLeave:
                await HandleTableLeaveAsync(session, message, cancellationToken);
                break;
            case MessageKind.PlayerAction:
                await HandlePlayerActionAsync(session, message, cancellationToken);
                break;
            case MessageKind.StateRequest:
                await HandleStateRequestAsync(session, message, cancellationToken);
                break;
            case MessageKind.AiListRequest:
                await HandleAiListRequestAsync(session, message, cancellationToken);
                break;
            case MessageKind.AiAddToTable:
                await HandleAiAddToTableAsync(session, message, cancellationToken);
                break;
            case MessageKind.OpponentListRequest:
                await HandleOpponentListRequestAsync(session, message, cancellationToken);
                break;
            default:
                await session.SendAsync(ProtocolMessages.Error(
                    message.Sequence,
                    ProtocolErrorCode.UnsupportedMessage,
                    "unsupported message"), cancellationToken);
                break;
        }
    }

    private async ValueTask HandleHelloAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadByte(out _) ||
            !reader.TryReadUInt16(out _) ||
            !reader.TryReadString(out string requestedHandle))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad hello", cancellationToken);
            return;
        }

        if (!IsValidHandle(requestedHandle))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.InvalidHandle, "invalid handle", cancellationToken);
            return;
        }

        List<IGameClientSession> joinedRecipients = [];
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (session.Handle is not null)
            {
                errorCode = ProtocolErrorCode.BadMessage;
                errorText = "already registered";
            }
            else if (_handles.ContainsKey(requestedHandle))
            {
                errorCode = ProtocolErrorCode.HandleInUse;
                errorText = "handle in use";
            }
            else
            {
                session.Handle = requestedHandle;
                _handles.Add(requestedHandle, session);
                joinedRecipients = _handles.Values.Where(s => !ReferenceEquals(s, session)).ToList();
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        var welcome = new ProtocolWriter();
        welcome.WriteUInt16(session.SessionId);
        welcome.WriteString(requestedHandle);
        welcome.WriteString("Nova Game Server");
        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.Welcome,
            message.Sequence,
            MessageFlags.Response,
            welcome.ToArray()), cancellationToken);

        if (joinedRecipients.Count > 0)
        {
            var joined = new ProtocolWriter();
            joined.WriteString(requestedHandle);
            await BroadcastAsync(joinedRecipients, ProtocolMessage.Create(
                MessageKind.UserJoined,
                0,
                MessageFlags.Event,
                joined.ToArray()), cancellationToken);
        }

        return;
    }

    private async ValueTask HandleLobbyChatAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadString(out string text))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad chat", cancellationToken);
            return;
        }
        if (!IsValidChatText(text))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "invalid chat", cancellationToken);
            return;
        }

        var writer = new ProtocolWriter();
        writer.WriteString(session.Handle!);
        writer.WriteString(text);
        List<IGameClientSession> recipients;
        lock (_gate)
            recipients = _handles.Values.ToList();

        await BroadcastAsync(recipients, ProtocolMessage.Create(
            MessageKind.LobbyChat,
            message.Sequence,
            MessageFlags.Event,
            writer.ToArray()), cancellationToken);
    }

    private async ValueTask HandleTableChatAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId) || !reader.TryReadString(out string text))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad table chat", cancellationToken);
            return;
        }
        if (!IsValidChatText(text))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "invalid chat", cancellationToken);
            return;
        }

        List<IGameClientSession> recipients = [];
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out var table))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if (!table.HasParticipant(session))
            {
                errorCode = ProtocolErrorCode.NotParticipant;
                errorText = "not at table";
            }
            else
            {
                recipients = table.SnapshotRecipients().ToList();
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteString(session.Handle!);
        writer.WriteString(text);
        await BroadcastAsync(recipients, ProtocolMessage.Create(
            MessageKind.TableChat,
            message.Sequence,
            MessageFlags.Event,
            writer.ToArray()), cancellationToken);
    }

    private async ValueTask HandleUserListRequestAsync(IGameClientSession session, byte sequence, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, sequence, cancellationToken))
            return;

        List<string> handles;
        lock (_gate)
            handles = _handles.Keys.Order(StringComparer.OrdinalIgnoreCase).ToList();

        if (handles.Count == 0)
        {
            await session.SendAsync(ProtocolMessage.Create(MessageKind.UserListItem, sequence, MessageFlags.Response), cancellationToken);
            return;
        }

        for (int i = 0; i < handles.Count; i++)
        {
            var writer = new ProtocolWriter();
            writer.WriteString(handles[i]);
            var flags = MessageFlags.Response | (i + 1 < handles.Count ? MessageFlags.More : MessageFlags.None);
            await session.SendAsync(ProtocolMessage.Create(MessageKind.UserListItem, sequence, flags, writer.ToArray()), cancellationToken);
        }
    }

    private async ValueTask HandleGameListRequestAsync(IGameClientSession session, byte sequence, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, sequence, cancellationToken))
            return;

        List<GameDescriptor> games;
        lock (_gate)
            games = _adapters.Values.Select(a => a.Descriptor).OrderBy(g => g.Id).ToList();

        for (int i = 0; i < games.Count; i++)
        {
            var payload = WriteGameDescriptor(games[i]);
            var flags = MessageFlags.Response | (i + 1 < games.Count ? MessageFlags.More : MessageFlags.None);
            await session.SendAsync(ProtocolMessage.Create(MessageKind.GameListItem, sequence, flags, payload), cancellationToken);
        }
    }

    private async ValueTask HandleTableListRequestAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        ushort? gameId = null;
        if (message.Payload.Length >= 2)
        {
            var reader = new ProtocolReader(message.Payload);
            if (reader.TryReadUInt16(out ushort parsedGameId))
                gameId = parsedGameId;
        }

        List<GameTable> tables;
        lock (_gate)
            tables = _tables.Values
                .Where(t => gameId is null || t.Game.Id == gameId.Value)
                .OrderBy(t => t.Id)
                .ToList();

        if (tables.Count == 0)
        {
            await session.SendAsync(ProtocolMessage.Create(MessageKind.TableListItem, message.Sequence, MessageFlags.Response), cancellationToken);
            return;
        }

        for (int i = 0; i < tables.Count; i++)
        {
            var flags = MessageFlags.Response | (i + 1 < tables.Count ? MessageFlags.More : MessageFlags.None);
            await session.SendAsync(ProtocolMessage.Create(
                MessageKind.TableListItem,
                message.Sequence,
                flags,
                WriteTable(tables[i])), cancellationToken);
        }
    }

    private async ValueTask HandleTableCreateAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort gameId) ||
            !reader.TryReadByte(out byte gameProtoMajor) ||
            !reader.TryReadByte(out _) ||
            !reader.TryReadString(out string tableName))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad table create", cancellationToken);
            return;
        }

        GameTable? table = null;
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_adapters.TryGetValue(gameId, out var adapter))
            {
                errorCode = ProtocolErrorCode.UnknownGame;
                errorText = "unknown game";
            }
            else if (gameProtoMajor != adapter.Descriptor.ProtocolMajor)
            {
                errorCode = ProtocolErrorCode.UnsupportedProtocol;
                errorText = "unsupported game protocol";
            }
            else
            {
                var safeName = SanitizeTableName(tableName, session.Handle!);
                table = new GameTable(_nextTableId++, adapter.Descriptor, safeName);
                table.TryAddPlayer(session);
                TryAddDefaultAiPlayerLocked(table);
                _tables.Add(table.Id, table);
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.TableCreated,
            message.Sequence,
            MessageFlags.Response,
            WriteTable(table!)), cancellationToken);
    }

    private async ValueTask HandleTableJoinAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad table join", cancellationToken);
            return;
        }

        byte roleByte = message.Kind == MessageKind.TableObserve ? (byte)TableJoinRole.Observer : (byte)TableJoinRole.Player;
        if (message.Kind == MessageKind.TableJoin && reader.TryReadByte(out byte parsedRole))
            roleByte = parsedRole;

        GameTable? table = null;
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out table!))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if ((TableJoinRole)roleByte == TableJoinRole.Observer)
            {
                table.AddObserver(session);
            }
            else if (!table.TryAddPlayer(session))
            {
                errorCode = ProtocolErrorCode.TableFull;
                errorText = "table full";
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.TableJoined,
            message.Sequence,
            MessageFlags.Response,
            WriteTable(table!)), cancellationToken);

        await RunAiTurnAsync(table!.Id, cancellationToken);
    }

    private async ValueTask HandleTableLeaveAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad table leave", cancellationToken);
            return;
        }

        List<IGameClientSession> recipients = [];
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out var table))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if (!table.HasParticipant(session))
            {
                errorCode = ProtocolErrorCode.NotParticipant;
                errorText = "not at table";
            }
            else
            {
                table.Remove(session);
                RemoveAiPlayersIfNoHumanPlayersLocked(table);
                recipients = table.SnapshotRecipients().ToList();
                if (table.IsEmpty)
                    _tables.Remove(tableId);
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteString(session.Handle!);
        var payload = writer.ToArray();

        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.TableLeft,
            message.Sequence,
            MessageFlags.Response,
            payload), cancellationToken);

        if (recipients.Count > 0)
        {
            await BroadcastAsync(recipients, ProtocolMessage.Create(
                MessageKind.TableLeft,
                0,
                MessageFlags.Event,
                payload), cancellationToken);
        }
    }

    private async ValueTask HandlePlayerActionAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId) || !reader.TryReadByte(out byte actionType))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad action", cancellationToken);
            return;
        }

        ReadOnlyMemory<byte> actionPayload = reader.ReadRemaining();
        GameActionResult? result = null;
        List<IGameClientSession> recipients = [];
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out var table))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if (!table.HasPlayer(session))
            {
                errorCode = ProtocolErrorCode.NotSeated;
                errorText = "not a player at table";
            }
            else
            {
                var adapter = _adapters[table.Game.Id];
                result = adapter.HandleAction(table, session, actionType, actionPayload);
                recipients = table.SnapshotRecipients()
                    .Where(recipient => !ReferenceEquals(recipient, session))
                    .ToList();
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        if (!result!.Accepted)
        {
            await SendErrorAsync(session, message.Sequence, result.ErrorCode, result.ErrorText ?? "bad action", cancellationToken);
            return;
        }

        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteByte(actionType);
        writer.WriteBytes(result.EventPayload);
        await BroadcastAsync(recipients, ProtocolMessage.Create(
            MessageKind.TableEvent,
            message.Sequence,
            MessageFlags.Event,
            writer.ToArray()), cancellationToken);

        await RunAiTurnAsync(tableId, cancellationToken);
    }

    private async ValueTask HandleStateRequestAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad state request", cancellationToken);
            return;
        }

        GameStateResult? result = null;
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out var table))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if (!table.HasParticipant(session))
            {
                errorCode = ProtocolErrorCode.NotParticipant;
                errorText = "not at table";
            }
            else
            {
                var adapter = _adapters[table.Game.Id];
                result = adapter.BuildState(table, session);
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        if (!result!.Accepted)
        {
            await SendErrorAsync(session, message.Sequence, result.ErrorCode, result.ErrorText ?? "state unavailable", cancellationToken);
            return;
        }

        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteBytes(result.StatePayload);
        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.StateChunk,
            message.Sequence,
            MessageFlags.Response,
            writer.ToArray()), cancellationToken);
    }

    private async ValueTask HandleAiListRequestAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        ushort? gameId = null;
        if (message.Payload.Length >= 2)
        {
            var reader = new ProtocolReader(message.Payload);
            if (reader.TryReadUInt16(out ushort parsedGameId) && parsedGameId != 0)
                gameId = parsedGameId;
        }

        List<AiDescriptor> providers;
        lock (_gate)
            providers = _aiProviders.Values
                .Select(provider => provider.Descriptor)
                .Where(provider => gameId is null || provider.GameId == gameId.Value || provider.GameId == 0)
                .OrderBy(provider => provider.Id)
                .ToList();

        if (providers.Count == 0)
        {
            await session.SendAsync(ProtocolMessage.Create(MessageKind.AiListItem, message.Sequence, MessageFlags.Response), cancellationToken);
            return;
        }

        for (int i = 0; i < providers.Count; i++)
        {
            var flags = MessageFlags.Response | (i + 1 < providers.Count ? MessageFlags.More : MessageFlags.None);
            await session.SendAsync(ProtocolMessage.Create(
                MessageKind.AiListItem,
                message.Sequence,
                flags,
                WriteAiDescriptor(providers[i])), cancellationToken);
        }
    }

    private async ValueTask HandleAiAddToTableAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        var reader = new ProtocolReader(message.Payload);
        if (!reader.TryReadUInt16(out ushort tableId) || !reader.TryReadUInt16(out ushort providerId))
        {
            await SendErrorAsync(session, message.Sequence, ProtocolErrorCode.BadMessage, "bad ai add request", cancellationToken);
            return;
        }

        GameTable? table = null;
        ProtocolErrorCode? errorCode = null;
        string? errorText = null;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out table!))
            {
                errorCode = ProtocolErrorCode.UnknownTable;
                errorText = "unknown table";
            }
            else if (!table.HasPlayer(session))
            {
                errorCode = ProtocolErrorCode.NotSeated;
                errorText = "not a player at table";
            }
            else if (!_aiProviders.TryGetValue(providerId, out var provider) ||
                !ProviderSupportsTable(provider.Descriptor, table))
            {
                errorCode = ProtocolErrorCode.UnknownAiProvider;
                errorText = "unknown ai provider";
            }
            else if (provider.Descriptor.GameId != 0 &&
                provider.Descriptor.GameProtocolMajor != table.Game.ProtocolMajor)
            {
                errorCode = ProtocolErrorCode.UnsupportedProtocol;
                errorText = "unsupported ai game protocol";
            }
            else
            {
                var aiSession = new AiClientSession(_nextSessionId++, provider)
                {
                    Handle = AllocateAiHandleLocked(provider.Descriptor)
                };

                if (!table.TryAddPlayer(aiSession))
                {
                    errorCode = ProtocolErrorCode.TableFull;
                    errorText = "table full";
                }
                else
                {
                    _sessions.Add(aiSession.SessionId, aiSession);
                    _handles.Add(aiSession.Handle!, aiSession);
                }
            }
        }

        if (errorCode is not null)
        {
            await SendErrorAsync(session, message.Sequence, errorCode.Value, errorText!, cancellationToken);
            return;
        }

        await session.SendAsync(ProtocolMessage.Create(
            MessageKind.TableJoined,
            message.Sequence,
            MessageFlags.Response,
            WriteTable(table!)), cancellationToken);

        await RunAiTurnAsync(tableId, cancellationToken);
    }

    private async ValueTask HandleOpponentListRequestAsync(IGameClientSession session, ProtocolMessage message, CancellationToken cancellationToken)
    {
        if (!await RequireRegisteredAsync(session, message.Sequence, cancellationToken))
            return;

        ushort? gameId = null;
        if (message.Payload.Length >= 2)
        {
            var reader = new ProtocolReader(message.Payload);
            if (reader.TryReadUInt16(out ushort parsedGameId) && parsedGameId != 0)
                gameId = parsedGameId;
        }

        List<OpponentDescriptor> opponents;
        lock (_gate)
        {
            opponents = _handles.Values
                .Where(candidate => !ReferenceEquals(candidate, session))
                .Where(candidate => candidate is not AiClientSession)
                .Select(candidate => new OpponentDescriptor(
                    OpponentKind.Human,
                    candidate.SessionId,
                    0,
                    candidate.Handle!,
                    candidate.Handle!,
                    0,
                    0,
                    0))
                .Concat(_aiProviders.Values
                    .Select(provider => provider.Descriptor)
                    .Where(provider => gameId is null || provider.GameId == gameId.Value || provider.GameId == 0)
                    .Select(provider => new OpponentDescriptor(
                        OpponentKind.Ai,
                        provider.Id,
                        provider.GameId,
                        provider.Code,
                        provider.DisplayName,
                        provider.GameProtocolMajor,
                        provider.GameProtocolMinor,
                        provider.Capabilities)))
                .OrderBy(opponent => opponent.Kind)
                .ThenBy(opponent => opponent.DisplayName, StringComparer.OrdinalIgnoreCase)
                .ToList();
        }

        if (opponents.Count == 0)
        {
            await session.SendAsync(ProtocolMessage.Create(MessageKind.OpponentListItem, message.Sequence, MessageFlags.Response), cancellationToken);
            return;
        }

        for (int i = 0; i < opponents.Count; i++)
        {
            var flags = MessageFlags.Response | (i + 1 < opponents.Count ? MessageFlags.More : MessageFlags.None);
            await session.SendAsync(ProtocolMessage.Create(
                MessageKind.OpponentListItem,
                message.Sequence,
                flags,
                WriteOpponentDescriptor(opponents[i])), cancellationToken);
        }
    }

    private async ValueTask RunAiTurnAsync(ushort tableId, CancellationToken cancellationToken)
    {
        GameTable? table;
        AiClientSession? aiSession;
        IAiProvider? provider;
        ushort chessPly = 0;

        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out table))
                return;

            aiSession = FindAiPlayerForTurn(table);
            if (aiSession is null)
                return;

            provider = aiSession.Provider;
            if (table.Game.Id == ChessGameAdapter.GameId)
                chessPly = ChessGameAdapter.GetState(table).Ply;
        }

        GameActionRequest? action = await provider.BuildActionAsync(table, aiSession, cancellationToken);
        if (action is null)
            return;

        GameActionResult? result;
        List<IGameClientSession> recipients;
        lock (_gate)
        {
            if (!_tables.TryGetValue(tableId, out table) || !table.HasPlayer(aiSession))
                return;

            if (table.Game.Id == ChessGameAdapter.GameId &&
                ChessGameAdapter.GetState(table).Ply != chessPly)
                return;

            var adapter = _adapters[table.Game.Id];
            result = adapter.HandleAction(table, aiSession, action.ActionType, action.Payload);
            if (!result.Accepted)
                return;

            recipients = table.SnapshotRecipients().ToList();
        }

        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteByte(action.ActionType);
        writer.WriteBytes(result.EventPayload);
        await BroadcastAsync(recipients, ProtocolMessage.Create(
            MessageKind.TableEvent,
            0,
            MessageFlags.Event,
            writer.ToArray()), cancellationToken);
    }

    private static bool ProviderSupportsTable(AiDescriptor provider, GameTable table) =>
        provider.GameId == 0 || provider.GameId == table.Game.Id;

    private void TryAddDefaultAiPlayerLocked(GameTable table)
    {
        if (table.Game.Id != ChessGameAdapter.GameId)
            return;
        if (table.Players.Any(player => player is AiClientSession))
            return;

        var provider = _aiProviders.Values
            .Select(aiProvider => aiProvider.Descriptor)
            .Where(descriptor => ProviderSupportsTable(descriptor, table))
            .OrderBy(descriptor => descriptor.DisplayName, StringComparer.OrdinalIgnoreCase)
            .FirstOrDefault();
        if (provider is null)
            return;
        if (!_aiProviders.TryGetValue(provider.Id, out var aiProvider))
            return;

        var aiSession = new AiClientSession(_nextSessionId++, aiProvider)
        {
            Handle = AllocateAiHandleLocked(provider)
        };
        if (!table.TryAddPlayer(aiSession))
            return;

        _sessions.Add(aiSession.SessionId, aiSession);
        _handles.Add(aiSession.Handle!, aiSession);
    }

    private AiClientSession? FindAiPlayerForTurn(GameTable table)
    {
        foreach (var player in table.Players)
        {
            if (player is not AiClientSession aiSession)
                continue;

            if (!ProviderSupportsTable(aiSession.Provider.Descriptor, table))
                continue;

            if (table.Game.Id == ChessGameAdapter.GameId)
            {
                var state = ChessGameAdapter.GetState(table);
                byte? side = ChessGameAdapter.GetSideForPlayer(table, aiSession);
                if (side is null || side.Value != state.SideToMove)
                    continue;
            }

            return aiSession;
        }

        return null;
    }

    private void RemoveAiPlayersIfNoHumanPlayersLocked(GameTable table)
    {
        if (table.Players.Any(player => player is not AiClientSession))
            return;

        foreach (var aiSession in table.Players.OfType<AiClientSession>().ToList())
        {
            table.Remove(aiSession);
            _sessions.Remove(aiSession.SessionId);
            if (!string.IsNullOrEmpty(aiSession.Handle))
                _handles.Remove(aiSession.Handle);
            aiSession.Handle = null;
        }
    }

    private async ValueTask<bool> RequireRegisteredAsync(IGameClientSession session, byte sequence, CancellationToken cancellationToken)
    {
        if (!string.IsNullOrEmpty(session.Handle))
            return true;

        await SendErrorAsync(session, sequence, ProtocolErrorCode.NotRegistered, "send hello first", cancellationToken);
        return false;
    }

    private static async ValueTask SendErrorAsync(
        IGameClientSession session,
        byte sequence,
        ProtocolErrorCode code,
        string text,
        CancellationToken cancellationToken)
    {
        await session.SendAsync(ProtocolMessages.Error(sequence, code, text), cancellationToken);
    }

    private static async ValueTask BroadcastAsync(
        IReadOnlyList<IGameClientSession> recipients,
        ProtocolMessage message,
        CancellationToken cancellationToken)
    {
        foreach (var recipient in recipients)
            await recipient.SendAsync(message, cancellationToken);
    }

    private static byte[] WriteGameDescriptor(GameDescriptor game)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(game.Id);
        writer.WriteByte(game.ProtocolMajor);
        writer.WriteByte(game.ProtocolMinor);
        writer.WriteUInt16(game.Capabilities);
        writer.WriteByte(game.MinPlayers);
        writer.WriteByte(game.MaxPlayers);
        writer.WriteString(game.Code);
        writer.WriteString(game.DisplayName);
        return writer.ToArray();
    }

    private static byte[] WriteTable(GameTable table)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(table.Id);
        writer.WriteUInt16(table.Game.Id);
        writer.WriteByte((byte)table.State);
        writer.WriteByte((byte)table.Players.Count);
        writer.WriteByte(table.Game.MaxPlayers);
        writer.WriteByte((byte)Math.Min(byte.MaxValue, table.Observers.Count));
        writer.WriteString(table.Name);
        writer.WriteString($"{table.Game.Code} {table.Players.Count}/{table.Game.MaxPlayers}");
        return writer.ToArray();
    }

    private static byte[] WriteAiDescriptor(AiDescriptor provider)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(provider.Id);
        writer.WriteUInt16(provider.GameId);
        writer.WriteByte(provider.GameProtocolMajor);
        writer.WriteByte(provider.GameProtocolMinor);
        writer.WriteUInt16(provider.Capabilities);
        writer.WriteString(provider.Code);
        writer.WriteString(provider.DisplayName);
        return writer.ToArray();
    }

    private static byte[] WriteOpponentDescriptor(OpponentDescriptor opponent)
    {
        var writer = new ProtocolWriter();
        writer.WriteByte((byte)opponent.Kind);
        writer.WriteUInt16(opponent.Id);
        writer.WriteUInt16(opponent.GameId);
        writer.WriteByte(opponent.GameProtocolMajor);
        writer.WriteByte(opponent.GameProtocolMinor);
        writer.WriteUInt16(opponent.Capabilities);
        writer.WriteString(opponent.Code);
        writer.WriteString(opponent.DisplayName);
        return writer.ToArray();
    }

    private static bool IsValidChatText(string text) =>
        text.Length is > 0 and <= MaxChatLength;

    private static bool IsValidHandle(string handle)
    {
        if (handle.Length is < 1 or > MaxHandleLength)
            return false;

        foreach (char ch in handle)
        {
            bool ok = ch is >= 'a' and <= 'z'
                or >= 'A' and <= 'Z'
                or >= '0' and <= '9'
                or '_' or '-';
            if (!ok)
                return false;
        }

        return true;
    }

    private static string SanitizeTableName(string requestedName, string ownerHandle)
    {
        string trimmed = requestedName.Trim();
        if (trimmed.Length == 0)
            return $"{ownerHandle}'s table";
        if (trimmed.Length <= MaxTableNameLength)
            return trimmed;
        return trimmed[..MaxTableNameLength];
    }

    private string AllocateAiHandleLocked(AiDescriptor provider)
    {
        string fragment = SanitizeHandleFragment(provider.Code);
        string baseHandle = $"ai-{fragment}";
        if (baseHandle.Length > MaxHandleLength)
            baseHandle = baseHandle[..MaxHandleLength];

        if (!_handles.ContainsKey(baseHandle))
            return baseHandle;

        for (int suffix = 2; suffix < 1000; suffix++)
        {
            string suffixText = suffix.ToString();
            int prefixLength = MaxHandleLength - suffixText.Length;
            string candidate = baseHandle.Length > prefixLength
                ? baseHandle[..prefixLength] + suffixText
                : baseHandle + suffixText;
            if (!_handles.ContainsKey(candidate))
                return candidate;
        }

        string fallback = $"ai{_nextSessionId}";
        return fallback.Length <= MaxHandleLength
            ? fallback
            : fallback[..MaxHandleLength];
    }

    private static string SanitizeHandleFragment(string value)
    {
        Span<char> chars = stackalloc char[Math.Min(MaxHandleLength, Math.Max(1, value.Length))];
        int length = 0;
        foreach (char ch in value)
        {
            if (length >= chars.Length)
                break;

            if (ch is >= 'a' and <= 'z'
                or >= 'A' and <= 'Z'
                or >= '0' and <= '9'
                or '_' or '-')
            {
                chars[length++] = char.ToLowerInvariant(ch);
            }
        }

        return length == 0 ? "ai" : new string(chars[..length]);
    }

    private sealed class AiClientSession : IGameClientSession
    {
        public AiClientSession(ushort sessionId, IAiProvider provider)
        {
            SessionId = sessionId;
            Provider = provider;
        }

        public ushort SessionId { get; }
        public IAiProvider Provider { get; }
        public string? Handle { get; set; }

        public ValueTask SendAsync(ProtocolMessage message, CancellationToken cancellationToken) =>
            ValueTask.CompletedTask;
    }
}
