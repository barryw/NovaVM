using System.Net;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;

int port = ParsePort(args);
var state = new GameServerState(aiProviders: [new StockfishAiProvider()]);
var server = new TcpGameServer(state, IPAddress.Any, port);

using var cts = new CancellationTokenSource();
Console.CancelKeyPress += (_, e) =>
{
    e.Cancel = true;
    cts.Cancel();
};

Console.WriteLine($"Nova game server listening on 0.0.0.0:{port}");
Console.WriteLine("Press Ctrl-C to stop.");
await server.RunAsync(cts.Token);

static int ParsePort(string[] args)
{
    for (int i = 0; i < args.Length; i++)
    {
        if (args[i] == "--port" && i + 1 < args.Length && int.TryParse(args[i + 1], out int namedPort))
            return namedPort;
        if (int.TryParse(args[i], out int positionalPort))
            return positionalPort;
    }

    string? envPort = Environment.GetEnvironmentVariable("NOVA_GAME_PORT");
    return int.TryParse(envPort, out int parsed) ? parsed : NovaGameProtocol.DefaultPort;
}
