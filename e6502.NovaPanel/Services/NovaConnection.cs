using System.Text.Json.Nodes;
using e6502.NovaHost;

namespace e6502.NovaPanel.Services;

public enum ConnectionState { Disconnected, Connecting, Connected, Error }

public sealed class NovaConnection : IAsyncDisposable
{
    private readonly Func<CancellationToken, Task<JsonObject>> _statusSource;
    private readonly NovaHostManagementClient? _mgmt;
    private CancellationTokenSource? _loopCts;
    private Task? _loopTask;
    private readonly object _startLock = new();

    public ConnectionState State { get; private set; } = ConnectionState.Disconnected;
    public NovaStatus LastStatus { get; private set; } = NovaStatus.Empty;
    public string? LastError { get; private set; }

    public event Action<ConnectionState>? StateChanged;
    public event Action<NovaStatus>? StatusUpdated;

    // Test ctor: inject the status source directly.
    public NovaConnection(Func<CancellationToken, Task<JsonObject>> statusSource)
        => _statusSource = statusSource;

    // Production ctor: build the management client from host; status source = GetStatusAsync.
    public NovaConnection(string host, int mgmtPort = 6504)
    {
        _mgmt = new NovaHostManagementClient(host, mgmtPort);
        _statusSource = ct => _mgmt.GetStatusAsync(ct);
    }

    // One poll iteration — directly callable (this is what tests drive).
    public async Task PollOnceAsync(CancellationToken ct = default)
    {
        SetState(ConnectionState.Connecting);
        try
        {
            JsonObject raw = await _statusSource(ct);
            LastStatus = NovaStatusParser.Parse(raw);
            LastError = null;
            SetState(ConnectionState.Connected);
            StatusUpdated?.Invoke(LastStatus);
        }
        catch (OperationCanceledException) { throw; }
        catch (Exception ex)
        {
            LastError = ex.Message;
            SetState(ConnectionState.Error);
        }
    }

    // Background poll loop with backoff. NOT unit-tested.
    public void Start(TimeSpan interval)
    {
        lock (_startLock)
        {
            if (_loopTask is { IsCompleted: false })
                return;

            _loopCts = new CancellationTokenSource();
            CancellationToken ct = _loopCts.Token;
            _loopTask = Task.Run(async () =>
            {
                TimeSpan currentInterval = interval;
                while (!ct.IsCancellationRequested)
                {
                    try
                    {
                        await PollOnceAsync(ct);
                        currentInterval = interval;
                    }
                    catch (OperationCanceledException)
                    {
                        break;
                    }

                    TimeSpan delay = State == ConnectionState.Error
                        ? TimeSpan.FromSeconds(Math.Min(currentInterval.TotalSeconds * 2, 5))
                        : currentInterval;

                    try { await Task.Delay(delay, ct); }
                    catch (OperationCanceledException) { break; }
                }
            }, ct);
        }
    }

    public async Task StopAsync()
    {
        CancellationTokenSource? cts;
        Task? loop;
        lock (_startLock)
        {
            cts = _loopCts;
            loop = _loopTask;
            _loopCts = null;
            _loopTask = null;
        }

        if (cts is not null)
        {
            await cts.CancelAsync();
            cts.Dispose();
        }

        if (loop is not null)
        {
            try { await loop; }
            catch (OperationCanceledException) { }
        }
    }

    private void SetState(ConnectionState next)
    {
        if (State == next) return;
        State = next;
        StateChanged?.Invoke(next);
    }

    // Action passthroughs — thin, never throw to caller; return (ok, error).
    public Task<(bool ok, string? error)> MountAsync(string slot, string? path, CancellationToken ct = default)
        => Guard(c => c.MountDriveAsync(slot, path, ct), ct);

    public Task<(bool ok, string? error)> UnmountAsync(string slot, CancellationToken ct = default)
        => Guard(c => c.UnmountDriveAsync(slot, ct), ct);

    public Task<(bool ok, string? error)> RuntimeSetAsync(string name, CancellationToken ct = default)
        => Guard(c => c.RuntimeSetAsync(name, ct), ct);

    public Task<(bool ok, string? error)> RuntimeAddAsync(string name, string rom, string? ext, bool makeActive, CancellationToken ct = default)
        => Guard(c => c.RuntimeAddAsync(name, rom, ext, makeActive, ct), ct);

    public Task<(bool ok, string? error)> RuntimeRemoveAsync(string name, CancellationToken ct = default)
        => Guard(c => c.RuntimeRemoveAsync(name, ct), ct);

    public Task<(bool ok, string? error)> VmResetAsync(CancellationToken ct = default)
        => Guard(c => c.VmResetAsync(ct), ct);

    public Task<(bool ok, string? error)> HostRebootAsync(CancellationToken ct = default)
        => Guard(c => c.HostRebootAsync(ct), ct);

    public Task<(bool ok, string? error)> AudioStopAsync(CancellationToken ct = default)
        => Guard(c => c.AudioStopAsync(ct), ct);

    public Task<(bool ok, string? error)> DeleteAsync(string path, CancellationToken ct = default)
        => Guard(c => c.DeletePathAsync(path, ct), ct);

    public async Task<JsonArray> ListDirectoryAsync(string path, CancellationToken ct = default)
    {
        if (_mgmt is null) return new JsonArray();
        try { return await _mgmt.ListDirectoryAsync(path, ct); }
        catch { return new JsonArray(); }
    }

    private async Task<(bool ok, string? error)> Guard(Func<NovaHostManagementClient, Task> call, CancellationToken ct)
    {
        if (_mgmt is null) return (false, "not connected");
        try { await call(_mgmt); return (true, null); }
        catch (Exception ex) { LastError = ex.Message; return (false, ex.Message); }
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync();
        _mgmt?.Dispose();
    }
}
