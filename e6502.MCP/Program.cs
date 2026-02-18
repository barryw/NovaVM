using e6502.MCP;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using ModelContextProtocol.Server;

var builder = Host.CreateApplicationBuilder(args);

builder.Logging.AddConsole(options =>
{
    options.LogToStandardErrorThreshold = LogLevel.Trace;
});

string host = Environment.GetEnvironmentVariable("EMULATOR_HOST") ?? "127.0.0.1";
int port = int.TryParse(Environment.GetEnvironmentVariable("EMULATOR_PORT"), out int p) ? p : 6502;

builder.Services.AddSingleton(new EmulatorClient(host, port));

builder.Services
    .AddMcpServer()
    .WithStdioServerTransport()
    .WithToolsFromAssembly();

await builder.Build().RunAsync();
