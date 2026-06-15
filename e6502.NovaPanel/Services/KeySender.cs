using System.Threading.Channels;

namespace e6502.NovaPanel.Services;

public interface IKeySink
{
    Task SendKeyAsync(string token, CancellationToken ct);
}

/// <summary>
/// Serializes key tokens through a single background consumer so the UI thread
/// never blocks and tokens arrive at the sink in enqueue order.
/// </summary>
public sealed class KeySender : IAsyncDisposable
{
    private readonly record struct WorkItem(string? Token, TaskCompletionSource? Signal);

    private readonly IKeySink _sink;
    private readonly Channel<WorkItem> _channel;
    private readonly CancellationTokenSource _cts;
    private readonly Task _consumer;

    public KeySender(IKeySink sink)
    {
        _sink = sink;
        _channel = Channel.CreateUnbounded<WorkItem>(new UnboundedChannelOptions
        {
            SingleReader = true
        });
        _cts = new CancellationTokenSource();
        _consumer = Task.Run(ConsumeAsync);
    }

    /// <summary>
    /// Enqueues a key token for delivery. Never blocks.
    /// </summary>
    public void Send(string token) =>
        _channel.Writer.TryWrite(new WorkItem(token, null));

    /// <summary>
    /// Returns a task that completes once all previously enqueued tokens have
    /// been delivered (or attempted) by the sink.
    /// </summary>
    public Task FlushAsync()
    {
        var tcs = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
        _channel.Writer.TryWrite(new WorkItem(null, tcs));
        return tcs.Task;
    }

    public async ValueTask DisposeAsync()
    {
        _channel.Writer.TryComplete();
        await _cts.CancelAsync();
        try { await _consumer; } catch (OperationCanceledException) { }
        _cts.Dispose();
    }

    private async Task ConsumeAsync()
    {
        var ct = _cts.Token;
        await foreach (var item in _channel.Reader.ReadAllAsync(ct))
        {
            if (item.Signal is not null)
            {
                // Sentinel: signal flush completion.
                item.Signal.TrySetResult();
                continue;
            }

            // Normal token — swallow sink exceptions so ordering is preserved.
            try
            {
                await _sink.SendKeyAsync(item.Token!, ct);
            }
            catch (OperationCanceledException) when (ct.IsCancellationRequested)
            {
                throw;
            }
            catch
            {
                // Swallow: a failed send must not kill the consumer.
            }
        }
    }
}
