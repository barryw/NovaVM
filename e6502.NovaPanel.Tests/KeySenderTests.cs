using e6502.NovaPanel.Services;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class KeySenderTests
{
    private sealed class RecordingSink : IKeySink
    {
        public List<string> Tokens { get; } = new();
        public Task SendKeyAsync(string token, CancellationToken ct)
        {
            Tokens.Add(token);
            return Task.CompletedTask;
        }
    }

    [Fact]
    public async Task Sends_tokens_in_order()
    {
        var sink = new RecordingSink();
        await using var sender = new KeySender(sink);

        sender.Send("a");
        sender.Send("b");
        sender.Send("ENTER");
        await sender.FlushAsync();

        Assert.Equal(new[] { "a", "b", "ENTER" }, sink.Tokens);
    }

    [Fact]
    public async Task Failing_send_does_not_break_ordering()
    {
        var sink = new ThrowOnceSink();
        await using var sender = new KeySender(sink);

        sender.Send("x");   // throws inside sink
        sender.Send("y");
        await sender.FlushAsync();

        Assert.Equal(new[] { "y" }, sink.Delivered); // x threw, y still delivered after
    }

    private sealed class ThrowOnceSink : IKeySink
    {
        private bool _thrown;
        public List<string> Delivered { get; } = new();
        public Task SendKeyAsync(string token, CancellationToken ct)
        {
            if (!_thrown) { _thrown = true; throw new InvalidOperationException("boom"); }
            Delivered.Add(token);
            return Task.CompletedTask;
        }
    }
}
