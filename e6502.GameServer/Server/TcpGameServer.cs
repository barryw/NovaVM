using System.Net;
using System.Net.Sockets;
using e6502.GameServer.Protocol;

namespace e6502.GameServer.Server;

public sealed class TcpGameServer
{
    private static readonly TimeSpan DefaultUnauthenticatedIdleTimeout = TimeSpan.FromMinutes(10);
    private static readonly TimeSpan DefaultRegisteredClientIdleTimeout = TimeSpan.FromHours(12);

    private readonly TcpListener _listener;
    private readonly GameServerState _state;
    private readonly TimeSpan _unauthenticatedIdleTimeout;
    private readonly TimeSpan _registeredClientIdleTimeout;

    public TcpGameServer(
        GameServerState state,
        IPAddress address,
        int port,
        TimeSpan? clientIdleTimeout = null,
        TimeSpan? registeredClientIdleTimeout = null)
    {
        _state = state;
        _listener = new TcpListener(address, port);
        _unauthenticatedIdleTimeout = clientIdleTimeout ?? DefaultUnauthenticatedIdleTimeout;
        _registeredClientIdleTimeout = registeredClientIdleTimeout
            ?? clientIdleTimeout
            ?? DefaultRegisteredClientIdleTimeout;
    }

    public int BoundPort => _listener.LocalEndpoint is IPEndPoint endpoint ? endpoint.Port : 0;

    public async Task RunAsync(CancellationToken cancellationToken)
    {
        _listener.Start();
        try
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                var client = await _listener.AcceptTcpClientAsync(cancellationToken);
                Console.WriteLine($"Client connected from {client.Client.RemoteEndPoint}");
                _ = Task.Run(() => RunClientAsync(client, cancellationToken), CancellationToken.None);
            }
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
        }
        finally
        {
            _listener.Stop();
        }
    }

    private async Task RunClientAsync(TcpClient client, CancellationToken serverCancellationToken)
    {
        await using var session = new TcpGameClientSession(_state.AllocateSessionId(), client);
        _state.RegisterConnection(session);

        try
        {
            using var linkedCts = CancellationTokenSource.CreateLinkedTokenSource(serverCancellationToken);
            var stream = client.GetStream();
            while (!linkedCts.IsCancellationRequested)
            {
                byte[]? frame;
                using (var readCts = CancellationTokenSource.CreateLinkedTokenSource(linkedCts.Token))
                {
                    readCts.CancelAfter(GetIdleTimeout(session));
                    try
                    {
                        frame = await NicFrameCodec.ReadFrameAsync(stream, readCts.Token);
                    }
                    catch (OperationCanceledException) when (!linkedCts.IsCancellationRequested)
                    {
                        await SendIdleTimeoutAsync(session);
                        break;
                    }
                }

                if (frame is null)
                    break;

                ProtocolMessage message;
                try
                {
                    message = ProtocolCodec.Decode(frame);
                }
                catch (ProtocolException ex)
                {
                    Console.WriteLine($"Session {session.SessionId}: bad frame: {ex.Message}");
                    await session.SendAsync(ProtocolMessages.Error(0, ProtocolErrorCode.BadMessage, ex.Message), linkedCts.Token);
                    continue;
                }

                Console.WriteLine(
                    $"Session {session.SessionId}: {message.Kind} seq={message.Sequence} bytes={frame.Length}");
                await _state.HandleMessageAsync(session, message, linkedCts.Token);
            }
        }
        catch (IOException)
        {
            Console.WriteLine($"Session {session.SessionId}: IO error");
        }
        catch (SocketException)
        {
            Console.WriteLine($"Session {session.SessionId}: socket error");
        }
        catch (OperationCanceledException) when (serverCancellationToken.IsCancellationRequested)
        {
        }
        finally
        {
            Console.WriteLine($"Session {session.SessionId}: disconnected");
            await _state.RemoveConnectionAsync(session, CancellationToken.None);
        }
    }

    private TimeSpan GetIdleTimeout(TcpGameClientSession session) =>
        string.IsNullOrEmpty(session.Handle)
            ? _unauthenticatedIdleTimeout
            : _registeredClientIdleTimeout;

    private static async ValueTask SendIdleTimeoutAsync(IGameClientSession session)
    {
        try
        {
            await session.SendAsync(ProtocolMessages.Error(
                0,
                ProtocolErrorCode.Timeout,
                "idle timeout"), CancellationToken.None);
        }
        catch (IOException)
        {
        }
        catch (SocketException)
        {
        }
    }
}

internal sealed class TcpGameClientSession : IGameClientSession, IAsyncDisposable
{
    private readonly TcpClient _client;
    private readonly SemaphoreSlim _sendLock = new(1, 1);

    public TcpGameClientSession(ushort sessionId, TcpClient client)
    {
        SessionId = sessionId;
        _client = client;
    }

    public ushort SessionId { get; }
    public string? Handle { get; set; }

    public async ValueTask SendAsync(ProtocolMessage message, CancellationToken cancellationToken)
    {
        byte[] frame = ProtocolCodec.Encode(message);
        await _sendLock.WaitAsync(cancellationToken);
        try
        {
            Console.WriteLine(
                $"Session {SessionId}: -> {message.Kind} seq={message.Sequence} bytes={frame.Length}");
            await NicFrameCodec.WriteFrameAsync(_client.GetStream(), frame, cancellationToken);
        }
        finally
        {
            _sendLock.Release();
        }
    }

    public ValueTask DisposeAsync()
    {
        _sendLock.Dispose();
        _client.Dispose();
        return ValueTask.CompletedTask;
    }
}
