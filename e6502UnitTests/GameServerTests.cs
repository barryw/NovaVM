using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class GameServerTests
{
    [TestMethod]
    public void ProtocolCodec_RoundTripsEnvelope()
    {
        var payload = new byte[] { 1, 2, 3 };
        var message = ProtocolMessage.Create(MessageKind.Ping, 7, MessageFlags.Response, payload);

        byte[] encoded = ProtocolCodec.Encode(message);
        var decoded = ProtocolCodec.Decode(encoded);

        Assert.AreEqual(NovaGameProtocol.ServerProtocolMajor, decoded.Version);
        Assert.AreEqual(MessageKind.Ping, decoded.Kind);
        Assert.AreEqual(7, decoded.Sequence);
        Assert.AreEqual(MessageFlags.Response, decoded.Flags);
        CollectionAssert.AreEqual(payload, decoded.Payload);
    }

    [TestMethod]
    public async Task NicFrameCodec_RoundTripsLengthPrefixedFrame()
    {
        var stream = new MemoryStream();
        byte[] frame = ProtocolCodec.Encode(ProtocolMessage.Create(
            MessageKind.Ping,
            9,
            payload: new byte[] { 0xAA, 0xBB }));

        await NicFrameCodec.WriteFrameAsync(stream, frame);

        byte[] raw = stream.ToArray();
        Assert.AreEqual(frame.Length, raw[0]);

        stream.Position = 0;
        byte[]? decoded = await NicFrameCodec.ReadFrameAsync(stream);
        CollectionAssert.AreEqual(frame, decoded);
    }

    [TestMethod]
    public async Task TcpGameServer_UsesRegisteredIdleTimeoutAfterHello()
    {
        var state = new GameServerState();
        var server = new TcpGameServer(
            state,
            IPAddress.Loopback,
            0,
            clientIdleTimeout: TimeSpan.FromMilliseconds(100),
            registeredClientIdleTimeout: TimeSpan.FromSeconds(1));
        using var cts = new CancellationTokenSource();
        Task serverTask = server.RunAsync(cts.Token);

        try
        {
            int port = await WaitForServerPortAsync(server);
            using var client = new TcpClient();
            await client.ConnectAsync(IPAddress.Loopback, port);
            NetworkStream stream = client.GetStream();

            await NicFrameCodec.WriteFrameAsync(stream, ProtocolCodec.Encode(Hello(1, "alice")));
            ProtocolMessage welcome = await ReadProtocolMessageAsync(stream);
            Assert.AreEqual(MessageKind.Welcome, welcome.Kind);

            await Task.Delay(250);

            await NicFrameCodec.WriteFrameAsync(stream, ProtocolCodec.Encode(ProtocolMessage.Create(MessageKind.Ping, 2)));
            ProtocolMessage pong = await ReadProtocolMessageAsync(stream);

            Assert.AreEqual(MessageKind.Pong, pong.Kind);
            Assert.AreEqual(2, pong.Sequence);
        }
        finally
        {
            cts.Cancel();
            await serverTask;
        }
    }

    [TestMethod]
    public async Task GameServer_RegistersUniqueHandles()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var duplicate = RegisterSession(state);

        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(duplicate, Hello(2, "ALICE"), CancellationToken.None);

        Assert.AreEqual("alice", alice.Handle);
        Assert.IsNull(duplicate.Handle);
        Assert.AreEqual(MessageKind.Welcome, alice.Sent.Single().Kind);
        Assert.AreEqual(MessageKind.Error, duplicate.Sent.Single().Kind);

        var error = new ProtocolReader(duplicate.Sent.Single().Payload);
        Assert.IsTrue(error.TryReadByte(out byte code));
        Assert.AreEqual((byte)ProtocolErrorCode.HandleInUse, code);
    }

    [TestMethod]
    public async Task GameServer_ListsBuiltInGames()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        alice.Sent.Clear();

        await state.HandleMessageAsync(alice, ProtocolMessage.Create(MessageKind.GameListRequest, 3), CancellationToken.None);

        Assert.IsTrue(alice.Sent.Count >= 2);
        Assert.IsTrue(alice.Sent.All(message => message.Kind == MessageKind.GameListItem));

        ProtocolMessage echoGame = alice.Sent.Single(message => ReadGameCode(message) == "echo");
        ProtocolMessage chessGame = alice.Sent.Single(message => ReadGameCode(message) == "chess");

        var reader = new ProtocolReader(echoGame.Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort gameId));
        Assert.IsTrue(reader.TryReadByte(out byte gameMajor));
        Assert.IsTrue(reader.TryReadByte(out byte gameMinor));
        Assert.IsTrue(reader.TryReadUInt16(out _));
        Assert.IsTrue(reader.TryReadByte(out byte minPlayers));
        Assert.IsTrue(reader.TryReadByte(out byte maxPlayers));
        Assert.IsTrue(reader.TryReadString(out string code));
        Assert.IsTrue(reader.TryReadString(out string displayName));

        Assert.AreEqual((ushort)1, gameId);
        Assert.AreEqual((byte)1, gameMajor);
        Assert.AreEqual((byte)0, gameMinor);
        Assert.AreEqual((byte)1, minPlayers);
        Assert.AreEqual((byte)8, maxPlayers);
        Assert.AreEqual("echo", code);
        Assert.AreEqual("Echo Turn", displayName);

        reader = new ProtocolReader(chessGame.Payload);
        Assert.IsTrue(reader.TryReadUInt16(out gameId));
        Assert.IsTrue(reader.TryReadByte(out gameMajor));
        Assert.IsTrue(reader.TryReadByte(out gameMinor));
        Assert.IsTrue(reader.TryReadUInt16(out _));
        Assert.IsTrue(reader.TryReadByte(out minPlayers));
        Assert.IsTrue(reader.TryReadByte(out maxPlayers));
        Assert.IsTrue(reader.TryReadString(out code));
        Assert.IsTrue(reader.TryReadString(out displayName));

        Assert.AreEqual(ChessGameAdapter.GameId, gameId);
        Assert.AreEqual(ChessGameAdapter.ProtocolMajor, gameMajor);
        Assert.AreEqual(ChessGameAdapter.ProtocolMinor, gameMinor);
        Assert.AreEqual((byte)2, minPlayers);
        Assert.AreEqual((byte)2, maxPlayers);
        Assert.AreEqual("chess", code);
        Assert.AreEqual("Chess", displayName);
    }

    [TestMethod]
    public async Task GameServer_CreatesTableAndBroadcastsGameAction()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var bob = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(bob, Hello(2, "bob"), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateEchoTable(3), CancellationToken.None);
        Assert.AreEqual(MessageKind.TableCreated, alice.Sent.Single().Kind);
        ushort tableId = ReadTableId(alice.Sent.Single());

        await state.HandleMessageAsync(bob, JoinTable(4, tableId, TableJoinRole.Player), CancellationToken.None);
        Assert.AreEqual(MessageKind.TableJoined, bob.Sent.Single().Kind);

        alice.Sent.Clear();
        bob.Sent.Clear();
        await state.HandleMessageAsync(alice, PlayerAction(5, tableId, actionType: 2, new byte[] { 0xAA }), CancellationToken.None);

        Assert.AreEqual(0, alice.Sent.Count);
        Assert.AreEqual(MessageKind.TableEvent, bob.Sent.Single().Kind);

        var reader = new ProtocolReader(bob.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort eventTableId));
        Assert.IsTrue(reader.TryReadByte(out byte eventActionType));
        Assert.IsTrue(reader.TryReadString(out string actor));
        Assert.IsTrue(reader.TryReadByte(out byte adapterActionType));
        ReadOnlyMemory<byte> actionPayload = reader.ReadRemaining();

        Assert.AreEqual(tableId, eventTableId);
        Assert.AreEqual((byte)2, eventActionType);
        Assert.AreEqual("alice", actor);
        Assert.AreEqual((byte)2, adapterActionType);
        CollectionAssert.AreEqual(new byte[] { 0xAA }, actionPayload.ToArray());
    }

    [TestMethod]
    public async Task GameServer_TableChatBroadcastsToParticipants()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var bob = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(bob, Hello(2, "bob"), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateEchoTable(3), CancellationToken.None);
        ushort tableId = ReadTableId(alice.Sent.Single());
        await state.HandleMessageAsync(bob, JoinTable(4, tableId, TableJoinRole.Observer), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, TableChat(5, tableId, "hello table"), CancellationToken.None);

        Assert.AreEqual(MessageKind.TableChat, alice.Sent.Single().Kind);
        Assert.AreEqual(MessageKind.TableChat, bob.Sent.Single().Kind);

        var reader = new ProtocolReader(bob.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort eventTableId));
        Assert.IsTrue(reader.TryReadString(out string handle));
        Assert.IsTrue(reader.TryReadString(out string text));
        Assert.AreEqual(tableId, eventTableId);
        Assert.AreEqual("alice", handle);
        Assert.AreEqual("hello table", text);
    }

    [TestMethod]
    public async Task GameServer_StateRequestReturnsAdapterStateForObservers()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var observer = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(observer, Hello(2, "observer"), CancellationToken.None);
        alice.Sent.Clear();
        observer.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateEchoTable(3), CancellationToken.None);
        ushort tableId = ReadTableId(alice.Sent.Single());
        await state.HandleMessageAsync(observer, JoinTable(4, tableId, TableJoinRole.Observer), CancellationToken.None);
        observer.Sent.Clear();

        await state.HandleMessageAsync(observer, StateRequest(5, tableId), CancellationToken.None);

        Assert.AreEqual(MessageKind.StateChunk, observer.Sent.Single().Kind);
        var reader = new ProtocolReader(observer.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort stateTableId));
        Assert.IsTrue(reader.TryReadByte(out byte tableState));
        Assert.IsTrue(reader.TryReadByte(out byte playerCount));
        Assert.IsTrue(reader.TryReadByte(out byte observerCount));

        Assert.AreEqual(tableId, stateTableId);
        Assert.AreEqual((byte)TableState.Playing, tableState);
        Assert.AreEqual((byte)1, playerCount);
        Assert.AreEqual((byte)1, observerCount);
    }

    [TestMethod]
    public async Task GameServer_TableLeaveNotifiesRemainingParticipants()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var bob = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(bob, Hello(2, "bob"), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateEchoTable(3), CancellationToken.None);
        ushort tableId = ReadTableId(alice.Sent.Single());
        await state.HandleMessageAsync(bob, JoinTable(4, tableId, TableJoinRole.Player), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, TableLeave(5, tableId), CancellationToken.None);

        Assert.AreEqual(MessageKind.TableLeft, alice.Sent.Single().Kind);
        Assert.IsTrue((alice.Sent.Single().Flags & MessageFlags.Response) != 0);
        Assert.AreEqual(MessageKind.TableLeft, bob.Sent.Single().Kind);
        Assert.IsTrue((bob.Sent.Single().Flags & MessageFlags.Event) != 0);

        var reader = new ProtocolReader(bob.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort eventTableId));
        Assert.IsTrue(reader.TryReadString(out string handle));
        Assert.AreEqual(tableId, eventTableId);
        Assert.AreEqual("alice", handle);
    }

    [TestMethod]
    public async Task GameServer_ListsAiProviders()
    {
        var state = new GameServerState(aiProviders: [new FakeAiProvider()]);
        var alice = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        alice.Sent.Clear();

        await state.HandleMessageAsync(alice, AiListRequest(2, gameId: 1), CancellationToken.None);

        Assert.AreEqual(MessageKind.AiListItem, alice.Sent.Single().Kind);
        var reader = new ProtocolReader(alice.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort providerId));
        Assert.IsTrue(reader.TryReadUInt16(out ushort gameId));
        Assert.IsTrue(reader.TryReadByte(out byte gameMajor));
        Assert.IsTrue(reader.TryReadByte(out byte gameMinor));
        Assert.IsTrue(reader.TryReadUInt16(out ushort capabilities));
        Assert.IsTrue(reader.TryReadString(out string code));
        Assert.IsTrue(reader.TryReadString(out string displayName));

        Assert.AreEqual((ushort)42, providerId);
        Assert.AreEqual((ushort)1, gameId);
        Assert.AreEqual((byte)1, gameMajor);
        Assert.AreEqual((byte)0, gameMinor);
        Assert.AreEqual((ushort)0x0001, capabilities);
        Assert.AreEqual("test-ai", code);
        Assert.AreEqual("Test AI", displayName);
    }

    [TestMethod]
    public async Task GameServer_ListsTypedOpponents()
    {
        var state = new GameServerState(aiProviders: [new FakeAiProvider()]);
        var alice = RegisterSession(state);
        var bob = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(bob, Hello(2, "bob"), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, OpponentListRequest(3, gameId: 1), CancellationToken.None);

        Assert.AreEqual(2, alice.Sent.Count);
        Assert.IsTrue(alice.Sent.All(message => message.Kind == MessageKind.OpponentListItem));

        ProtocolMessage human = alice.Sent.Single(message => ReadOpponentKind(message) == OpponentKind.Human);
        ProtocolMessage ai = alice.Sent.Single(message => ReadOpponentKind(message) == OpponentKind.Ai);

        var reader = new ProtocolReader(human.Payload);
        Assert.IsTrue(reader.TryReadByte(out byte kind));
        Assert.IsTrue(reader.TryReadUInt16(out ushort id));
        Assert.IsTrue(reader.TryReadUInt16(out ushort gameId));
        Assert.IsTrue(reader.TryReadByte(out byte gameMajor));
        Assert.IsTrue(reader.TryReadByte(out byte gameMinor));
        Assert.IsTrue(reader.TryReadUInt16(out ushort capabilities));
        Assert.IsTrue(reader.TryReadString(out string code));
        Assert.IsTrue(reader.TryReadString(out string displayName));

        Assert.AreEqual((byte)OpponentKind.Human, kind);
        Assert.AreEqual(bob.SessionId, id);
        Assert.AreEqual((ushort)0, gameId);
        Assert.AreEqual((byte)0, gameMajor);
        Assert.AreEqual((byte)0, gameMinor);
        Assert.AreEqual((ushort)0, capabilities);
        Assert.AreEqual("bob", code);
        Assert.AreEqual("bob", displayName);

        reader = new ProtocolReader(ai.Payload);
        Assert.IsTrue(reader.TryReadByte(out kind));
        Assert.IsTrue(reader.TryReadUInt16(out id));
        Assert.IsTrue(reader.TryReadUInt16(out gameId));
        Assert.IsTrue(reader.TryReadByte(out gameMajor));
        Assert.IsTrue(reader.TryReadByte(out gameMinor));
        Assert.IsTrue(reader.TryReadUInt16(out capabilities));
        Assert.IsTrue(reader.TryReadString(out code));
        Assert.IsTrue(reader.TryReadString(out displayName));

        Assert.AreEqual((byte)OpponentKind.Ai, kind);
        Assert.AreEqual((ushort)42, id);
        Assert.AreEqual((ushort)1, gameId);
        Assert.AreEqual((byte)1, gameMajor);
        Assert.AreEqual((byte)0, gameMinor);
        Assert.AreEqual((ushort)0x0001, capabilities);
        Assert.AreEqual("test-ai", code);
        Assert.AreEqual("Test AI", displayName);
    }

    [TestMethod]
    public async Task GameServer_ChessAdapterBroadcastsMovesAndState()
    {
        var state = new GameServerState();
        var alice = RegisterSession(state);
        var bob = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        await state.HandleMessageAsync(bob, Hello(2, "bob"), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateChessTable(3), CancellationToken.None);
        ushort tableId = ReadTableId(alice.Sent.Single());
        await state.HandleMessageAsync(bob, JoinTable(4, tableId, TableJoinRole.Player), CancellationToken.None);
        alice.Sent.Clear();
        bob.Sent.Clear();

        await state.HandleMessageAsync(alice, PlayerAction(
            5,
            tableId,
            ChessGameAdapter.ActionMove,
            new byte[] { 0x64, 0x44 }), CancellationToken.None);

        Assert.AreEqual(0, alice.Sent.Count);
        Assert.AreEqual(MessageKind.TableEvent, bob.Sent.Single().Kind);

        var reader = new ProtocolReader(bob.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort eventTableId));
        Assert.IsTrue(reader.TryReadByte(out byte actionType));
        Assert.IsTrue(reader.TryReadString(out string actor));
        Assert.IsTrue(reader.TryReadUInt16(out ushort ply));
        Assert.IsTrue(reader.TryReadByte(out byte side));
        Assert.IsTrue(reader.TryReadByte(out byte from));
        Assert.IsTrue(reader.TryReadByte(out byte to));

        Assert.AreEqual(tableId, eventTableId);
        Assert.AreEqual(ChessGameAdapter.ActionMove, actionType);
        Assert.AreEqual("alice", actor);
        Assert.AreEqual((ushort)1, ply);
        Assert.AreEqual((byte)1, side);
        Assert.AreEqual((byte)0x64, from);
        Assert.AreEqual((byte)0x44, to);

        bob.Sent.Clear();
        await state.HandleMessageAsync(bob, StateRequest(6, tableId), CancellationToken.None);

        Assert.AreEqual(MessageKind.StateChunk, bob.Sent.Single().Kind);
        reader = new ProtocolReader(bob.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort stateTableId));
        Assert.IsTrue(reader.TryReadByte(out byte stateVersion));
        Assert.IsTrue(reader.TryReadUInt16(out ushort statePly));
        Assert.IsTrue(reader.TryReadByte(out byte sideToMove));
        Assert.IsTrue(reader.TryReadByte(out byte lastFrom));
        Assert.IsTrue(reader.TryReadByte(out byte lastTo));
        Assert.IsTrue(reader.TryReadByte(out byte moveCount));

        Assert.AreEqual(tableId, stateTableId);
        Assert.AreEqual((byte)1, stateVersion);
        Assert.AreEqual((ushort)1, statePly);
        Assert.AreEqual((byte)0, sideToMove);
        Assert.AreEqual((byte)0x64, lastFrom);
        Assert.AreEqual((byte)0x44, lastTo);
        Assert.AreEqual((byte)1, moveCount);
    }

    [TestMethod]
    public async Task GameServer_AutoSeatsChessProviderAndMovesAfterHuman()
    {
        var state = new GameServerState(aiProviders: [new FakeChessAiProvider()]);
        var alice = RegisterSession(state);
        await state.HandleMessageAsync(alice, Hello(1, "alice"), CancellationToken.None);
        alice.Sent.Clear();

        await state.HandleMessageAsync(alice, CreateChessTable(2), CancellationToken.None);
        ushort tableId = ReadTableId(alice.Sent.Single());
        alice.Sent.Clear();

        await state.HandleMessageAsync(alice, PlayerAction(
            4,
            tableId,
            ChessGameAdapter.ActionMove,
            new byte[] { 0x64, 0x44 }), CancellationToken.None);

        Assert.AreEqual(1, alice.Sent.Count(message => message.Kind == MessageKind.TableEvent));
        ProtocolMessage aiMove = alice.Sent.Last(message => message.Kind == MessageKind.TableEvent);

        var reader = new ProtocolReader(aiMove.Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort eventTableId));
        Assert.IsTrue(reader.TryReadByte(out byte actionType));
        Assert.IsTrue(reader.TryReadString(out string actor));
        Assert.IsTrue(reader.TryReadUInt16(out ushort ply));
        Assert.IsTrue(reader.TryReadByte(out byte side));
        Assert.IsTrue(reader.TryReadByte(out byte from));
        Assert.IsTrue(reader.TryReadByte(out byte to));

        Assert.AreEqual(tableId, eventTableId);
        Assert.AreEqual(ChessGameAdapter.ActionMove, actionType);
        Assert.AreEqual("ai-testchess", actor);
        Assert.AreEqual((ushort)2, ply);
        Assert.AreEqual((byte)0, side);
        Assert.AreEqual((byte)0x14, from);
        Assert.AreEqual((byte)0x34, to);

        alice.Sent.Clear();
        await state.HandleMessageAsync(alice, TableLeave(5, tableId), CancellationToken.None);
        alice.Sent.Clear();

        await state.HandleMessageAsync(alice, ProtocolMessage.Create(MessageKind.UserListRequest, 6), CancellationToken.None);

        Assert.AreEqual(1, alice.Sent.Count);
        reader = new ProtocolReader(alice.Sent.Single().Payload);
        Assert.IsTrue(reader.TryReadString(out string remainingHandle));
        Assert.AreEqual("alice", remainingHandle);
    }

    private static FakeSession RegisterSession(GameServerState state)
    {
        var session = new FakeSession(state.AllocateSessionId());
        state.RegisterConnection(session);
        return session;
    }

    private static async Task<int> WaitForServerPortAsync(TcpGameServer server)
    {
        var deadline = DateTime.UtcNow + TimeSpan.FromSeconds(2);
        while (DateTime.UtcNow < deadline)
        {
            if (server.BoundPort != 0)
                return server.BoundPort;

            await Task.Delay(10);
        }

        Assert.Fail("Game server did not start listening.");
        return 0;
    }

    private static async Task<ProtocolMessage> ReadProtocolMessageAsync(NetworkStream stream)
    {
        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(2));
        byte[]? frame = await NicFrameCodec.ReadFrameAsync(stream, cts.Token);
        Assert.IsNotNull(frame);
        return ProtocolCodec.Decode(frame);
    }

    private static ProtocolMessage Hello(byte sequence, string handle)
    {
        var writer = new ProtocolWriter();
        writer.WriteByte(NovaGameProtocol.ServerProtocolMinor);
        writer.WriteUInt16(0);
        writer.WriteString(handle);
        writer.WriteString("test-client");
        writer.WriteByte(1);
        writer.WriteByte(0);
        return ProtocolMessage.Create(MessageKind.Hello, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage CreateEchoTable(byte sequence)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(1);
        writer.WriteByte(1);
        writer.WriteByte(0);
        writer.WriteString("Test table");
        return ProtocolMessage.Create(MessageKind.TableCreate, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage CreateChessTable(byte sequence)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(ChessGameAdapter.GameId);
        writer.WriteByte(ChessGameAdapter.ProtocolMajor);
        writer.WriteByte(ChessGameAdapter.ProtocolMinor);
        writer.WriteString("Chess table");
        return ProtocolMessage.Create(MessageKind.TableCreate, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage JoinTable(byte sequence, ushort tableId, TableJoinRole role)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteByte((byte)role);
        return ProtocolMessage.Create(MessageKind.TableJoin, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage PlayerAction(byte sequence, ushort tableId, byte actionType, ReadOnlySpan<byte> payload)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteByte(actionType);
        writer.WriteBytes(payload);
        return ProtocolMessage.Create(MessageKind.PlayerAction, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage TableChat(byte sequence, ushort tableId, string text)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteString(text);
        return ProtocolMessage.Create(MessageKind.TableChat, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage TableLeave(byte sequence, ushort tableId)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        return ProtocolMessage.Create(MessageKind.TableLeave, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage StateRequest(byte sequence, ushort tableId)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        return ProtocolMessage.Create(MessageKind.StateRequest, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage AiListRequest(byte sequence, ushort gameId)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(gameId);
        return ProtocolMessage.Create(MessageKind.AiListRequest, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage OpponentListRequest(byte sequence, ushort gameId)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(gameId);
        return ProtocolMessage.Create(MessageKind.OpponentListRequest, sequence, payload: writer.ToArray());
    }

    private static ProtocolMessage AiAddToTable(byte sequence, ushort tableId, ushort providerId)
    {
        var writer = new ProtocolWriter();
        writer.WriteUInt16(tableId);
        writer.WriteUInt16(providerId);
        return ProtocolMessage.Create(MessageKind.AiAddToTable, sequence, payload: writer.ToArray());
    }

    private static ushort ReadTableId(ProtocolMessage message)
    {
        var reader = new ProtocolReader(message.Payload);
        Assert.IsTrue(reader.TryReadUInt16(out ushort tableId));
        return tableId;
    }

    private static string ReadGameCode(ProtocolMessage message)
    {
        var reader = new ProtocolReader(message.Payload);
        Assert.IsTrue(reader.TryReadUInt16(out _));
        Assert.IsTrue(reader.TryReadByte(out _));
        Assert.IsTrue(reader.TryReadByte(out _));
        Assert.IsTrue(reader.TryReadUInt16(out _));
        Assert.IsTrue(reader.TryReadByte(out _));
        Assert.IsTrue(reader.TryReadByte(out _));
        Assert.IsTrue(reader.TryReadString(out string code));
        return code;
    }

    private static OpponentKind ReadOpponentKind(ProtocolMessage message)
    {
        var reader = new ProtocolReader(message.Payload);
        Assert.IsTrue(reader.TryReadByte(out byte kind));
        return (OpponentKind)kind;
    }

    private sealed class FakeSession : IGameClientSession
    {
        public FakeSession(ushort sessionId)
        {
            SessionId = sessionId;
        }

        public ushort SessionId { get; }
        public string? Handle { get; set; }
        public List<ProtocolMessage> Sent { get; } = [];

        public ValueTask SendAsync(ProtocolMessage message, CancellationToken cancellationToken)
        {
            Sent.Add(message);
            return ValueTask.CompletedTask;
        }
    }

    private sealed class FakeAiProvider : IAiProvider
    {
        public AiDescriptor Descriptor { get; } = new(
            Id: 42,
            GameId: 1,
            Code: "test-ai",
            DisplayName: "Test AI",
            GameProtocolMajor: 1,
            GameProtocolMinor: 0,
            Capabilities: 0x0001);

        public ValueTask<GameActionRequest?> BuildActionAsync(
            GameTable table,
            IGameClientSession aiSession,
            CancellationToken cancellationToken) =>
            ValueTask.FromResult<GameActionRequest?>(null);
    }

    private sealed class FakeChessAiProvider : IAiProvider
    {
        public const ushort ProviderId = 43;

        public AiDescriptor Descriptor { get; } = new(
            Id: ProviderId,
            GameId: ChessGameAdapter.GameId,
            Code: "testchess",
            DisplayName: "Test Chess AI",
            GameProtocolMajor: ChessGameAdapter.ProtocolMajor,
            GameProtocolMinor: ChessGameAdapter.ProtocolMinor,
            Capabilities: 0x0001);

        public ValueTask<GameActionRequest?> BuildActionAsync(
            GameTable table,
            IGameClientSession aiSession,
            CancellationToken cancellationToken) =>
            ValueTask.FromResult<GameActionRequest?>(new GameActionRequest(
                ChessGameAdapter.ActionMove,
                [0x14, 0x34]));
    }
}
