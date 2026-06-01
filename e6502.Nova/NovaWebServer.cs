using System.Diagnostics;
using System.Collections.Concurrent;
using System.IO.Compression;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using e6502.Storage;

static class NovaWebServer
{
    public static int Run(string boardHost, string bind, int port, bool openBrowser)
    {
        using var cts = new CancellationTokenSource();
        Console.CancelKeyPress += (_, ev) =>
        {
            ev.Cancel = true;
            cts.Cancel();
        };

        var server = new LocalNovaWebServer(boardHost, bind, port);
        try
        {
            server.Start();
            Console.WriteLine($"Nova control center: {server.Url}");
            Console.WriteLine($"Target NovaHost:     {server.BoardBase}");
            Console.WriteLine("Press Ctrl+C to stop.");

            if (openBrowser)
                TryOpenBrowser(server.Url);

            server.RunAsync(cts.Token).GetAwaiter().GetResult();
            return 0;
        }
        catch (HttpListenerException ex)
        {
            Console.Error.WriteLine($"webserver: {ex.Message}");
            return 1;
        }
        catch (OperationCanceledException)
        {
            return 0;
        }
    }

    private static void TryOpenBrowser(string url)
    {
        try
        {
            if (OperatingSystem.IsMacOS())
                Process.Start("open", url);
            else if (OperatingSystem.IsWindows())
                Process.Start(new ProcessStartInfo(url) { UseShellExecute = true });
            else
                Process.Start("xdg-open", url);
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"open browser: {ex.Message}");
        }
    }
}

sealed class LocalNovaWebServer
{
    private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.Web)
    {
        WriteIndented = false
    };

    private readonly HttpListener _listener = new();
    private readonly NovaHostManagementClient _management;
    private readonly ConcurrentDictionary<Guid, WebSocket> _eventClients = new();
    private readonly ConcurrentDictionary<string, DiskBootInfo> _diskBootInfoCache = new(StringComparer.Ordinal);
    private readonly SemaphoreSlim _statusGate = new(1, 1);
    private readonly SemaphoreSlim _inventoryGate = new(1, 1);
    private readonly string _prefix;
    private JsonObject? _statusCache;
    private JsonObject? _inventoryCache;
    private DateTimeOffset _statusCacheAt = DateTimeOffset.MinValue;
    private DateTimeOffset _inventoryCacheAt = DateTimeOffset.MinValue;
    private bool _started;
    private static readonly TimeSpan StatusFreshFor = TimeSpan.FromSeconds(8);
    private static readonly TimeSpan InventoryFreshFor = TimeSpan.FromSeconds(30);
    private const int RemoteMetadataChunkBytes = 16 * 1024;
    private const int MaxNdiDirectoryBytes = 128 * 1024;

    public LocalNovaWebServer(string boardHost, string bind, int port)
    {
        string normalizedHost = boardHost.StartsWith("http://", StringComparison.OrdinalIgnoreCase) ||
            boardHost.StartsWith("https://", StringComparison.OrdinalIgnoreCase)
            ? boardHost.TrimEnd('/')
            : $"http://{boardHost.TrimEnd('/')}";
        BoardBase = normalizedHost;
        _management = new NovaHostManagementClient(normalizedHost);

        string listenerHost = string.IsNullOrWhiteSpace(bind) ? "127.0.0.1" : bind;
        _prefix = $"http://{listenerHost}:{port}/";
        _listener.Prefixes.Add(_prefix);
        Url = listenerHost is "*" or "+"
            ? $"http://localhost:{port}/"
            : _prefix;
    }

    public string Url { get; }
    public string BoardBase { get; }

    public void Start()
    {
        if (_started)
            return;
        _listener.Start();
        _started = true;
    }

    public async Task RunAsync(CancellationToken token)
    {
        Start();
        using var stopRegistration = token.Register(() =>
        {
            try { _listener.Stop(); } catch { }
        });

        while (!token.IsCancellationRequested && _listener.IsListening)
        {
            HttpListenerContext context;
            try
            {
                context = await _listener.GetContextAsync();
            }
            catch (HttpListenerException) when (token.IsCancellationRequested)
            {
                break;
            }
            catch (ObjectDisposedException) when (token.IsCancellationRequested)
            {
                break;
            }

            _ = Task.Run(() => HandleAsync(context, token), CancellationToken.None);
        }
    }

    private async Task HandleAsync(HttpListenerContext context, CancellationToken token)
    {
        bool closeResponse = true;
        try
        {
            string method = context.Request.HttpMethod.ToUpperInvariant();
            string path = context.Request.Url?.AbsolutePath ?? "/";

            if (method == "GET" && path == "/events")
            {
                if (!context.Request.IsWebSocketRequest)
                {
                    await SendErrorAsync(context.Response, 400, "websocket required", token);
                    return;
                }
                closeResponse = false;
                await HandleEventsAsync(context, token);
                return;
            }

            if (method == "GET" && path == "/")
            {
                await SendHtmlAsync(context.Response, IndexHtml, token);
                return;
            }

            if (method == "GET" && path == "/api/status")
            {
                await SendJsonAsync(context.Response, await GetStatusSnapshotAsync(token), token);
                return;
            }

            if (method == "GET" && path == "/api/inventory")
            {
                await SendJsonAsync(context.Response, await GetInventorySnapshotAsync(token), token);
                return;
            }

            if (method == "PUT" && path == "/api/sd")
            {
                string? sdPath = Query(context.Request, "path");
                if (string.IsNullOrWhiteSpace(sdPath))
                {
                    await SendErrorAsync(context.Response, 400, "path is required", token);
                    return;
                }
                await ProxyUploadAsync(context, EnsureAbsolutePath(sdPath), token);
                return;
            }

            if (method == "DELETE" && path == "/api/sd")
            {
                string? sdPath = Query(context.Request, "path");
                if (string.IsNullOrWhiteSpace(sdPath))
                {
                    await SendErrorAsync(context.Response, 400, "path is required", token);
                    return;
                }
                await SendOkAsync(context.Response,
                    await _management.DeletePathAsync(EnsureAbsolutePath(sdPath), token),
                    token);
                InvalidateStatusCache();
                InvalidateInventoryCache();
                return;
            }

            if (method == "POST" && path.StartsWith("/api/drives/", StringComparison.Ordinal))
            {
                string suffix = path["/api/drives/".Length..];
                string[] parts = suffix.Split('/', StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length == 2 && IsDriveSlot(parts[0]) && parts[1] is "mount" or "unmount")
                {
                    if (parts[1] == "mount")
                    {
                        JsonObject payload = await ReadJsonObjectAsync(context.Request, token);
                        string? mountPath = payload["path"]?.GetValue<string>();
                        await SendOkAsync(context.Response,
                            await _management.MountDriveAsync(parts[0], mountPath, token),
                            token);
                    }
                    else
                    {
                        await SendOkAsync(context.Response,
                            await _management.UnmountDriveAsync(parts[0], token),
                            token);
                    }
                    InvalidateStatusCache();
                    return;
                }
            }

            if (method == "POST" && path == "/api/vm-reset")
            {
                await SendOkAsync(context.Response, await _management.VmResetAsync(token), token);
                InvalidateStatusCache();
                return;
            }

            if (method == "POST" && path == "/api/reboot")
            {
                await SendOkAsync(context.Response, await _management.HostRebootAsync(token), token);
                InvalidateStatusCache();
                return;
            }

            if (method == "POST" && path == "/api/audio-stop")
            {
                await SendOkAsync(context.Response, await _management.AudioStopAsync(token), token);
                InvalidateStatusCache();
                return;
            }

            if (method == "POST" && path == "/api/runtime/set")
            {
                await RuntimeSetAsync(context, token);
                return;
            }

            if (method == "POST" && path == "/api/runtime/add")
            {
                await RuntimeAddAsync(context, token);
                return;
            }

            if (method == "PUT" && path == "/api/runtime/package")
            {
                await RuntimePackageAsync(context, token);
                return;
            }

            if (method == "POST" && path == "/api/runtime/remove")
            {
                await RuntimeRemoveAsync(context, token);
                return;
            }

            await SendErrorAsync(context.Response, 404, "not found", token);
        }
        catch (Exception ex)
        {
            await SendErrorAsync(context.Response, 500, ex.Message, token);
        }
        finally
        {
            if (closeResponse)
                context.Response.Close();
        }
    }

    private async Task<JsonObject> BuildStatusAsync(CancellationToken token)
    {
        try
        {
            JsonObject root = await _management.GetStatusAsync(token);
            root["target"] = $"{BoardBase} via tcp+cbor";
            return root;
        }
        catch (Exception ex)
        {
            return new JsonObject
            {
                ["target"] = $"{BoardBase} via tcp+cbor",
                ["management"] = _management.ConnectionState,
                ["health"] = ErrorObject(ex.Message),
                ["sdStatus"] = ErrorObject(ex.Message),
                ["wifi"] = ErrorObject(ex.Message),
                ["audio"] = ErrorObject(ex.Message),
                ["drives"] = new JsonArray(),
                ["bootConfig"] = ErrorObject(ex.Message)
            };
        }
    }

    private async Task<JsonObject> BuildInventoryAsync(CancellationToken token)
    {
        var root = new JsonObject
        {
            ["disks"] = await BuildDiskListAsync(token),
            ["library"] = await BuildLibraryAsync(token)
        };
        return root;
    }

    private async Task<JsonObject> GetStatusSnapshotAsync(CancellationToken token)
    {
        DateTimeOffset now = DateTimeOffset.UtcNow;
        if (_statusCache is not null && now - _statusCacheAt < StatusFreshFor)
            return CloneStatus(cached: true, refreshing: false);

        if (!await _statusGate.WaitAsync(0, token))
        {
            if (_statusCache is not null)
                return CloneStatus(cached: true, refreshing: true);

            await _statusGate.WaitAsync(token);
        }

        try
        {
            now = DateTimeOffset.UtcNow;
            if (_statusCache is not null && now - _statusCacheAt < StatusFreshFor)
                return CloneStatus(cached: true, refreshing: false);

            _statusCache = await BuildStatusAsync(token);
            _statusCacheAt = DateTimeOffset.UtcNow;
            return CloneStatus(cached: false, refreshing: false);
        }
        finally
        {
            _statusGate.Release();
        }
    }

    private JsonObject CloneStatus(bool cached, bool refreshing)
    {
        JsonObject clone = _statusCache?.DeepClone().AsObject() ?? new JsonObject();
        clone["cached"] = cached;
        clone["refreshing"] = refreshing;
        clone["cacheAgeMs"] = _statusCacheAt == DateTimeOffset.MinValue
            ? 0
            : (long)Math.Max(0, (DateTimeOffset.UtcNow - _statusCacheAt).TotalMilliseconds);
        return clone;
    }

    private async Task<JsonObject> GetInventorySnapshotAsync(CancellationToken token)
    {
        DateTimeOffset now = DateTimeOffset.UtcNow;
        if (_inventoryCache is not null && now - _inventoryCacheAt < InventoryFreshFor)
            return CloneInventory(cached: true, refreshing: false);

        if (!await _inventoryGate.WaitAsync(0, token))
        {
            if (_inventoryCache is not null)
                return CloneInventory(cached: true, refreshing: true);

            await _inventoryGate.WaitAsync(token);
        }

        try
        {
            now = DateTimeOffset.UtcNow;
            if (_inventoryCache is not null && now - _inventoryCacheAt < InventoryFreshFor)
                return CloneInventory(cached: true, refreshing: false);

            _inventoryCache = await BuildInventoryAsync(token);
            _inventoryCacheAt = DateTimeOffset.UtcNow;
            return CloneInventory(cached: false, refreshing: false);
        }
        finally
        {
            _inventoryGate.Release();
        }
    }

    private JsonObject CloneInventory(bool cached, bool refreshing)
    {
        JsonObject clone = _inventoryCache?.DeepClone().AsObject() ?? new JsonObject();
        clone["cached"] = cached;
        clone["refreshing"] = refreshing;
        clone["cacheAgeMs"] = _inventoryCacheAt == DateTimeOffset.MinValue
            ? 0
            : (long)Math.Max(0, (DateTimeOffset.UtcNow - _inventoryCacheAt).TotalMilliseconds);
        return clone;
    }

    private async Task<JsonArray> BuildDiskListAsync(CancellationToken token)
    {
        var disks = new JsonArray();
        await AddDiskFilesAsync(disks, "Floppy", "disks/floppy", token);
        await AddDiskFilesAsync(disks, "Hard", "disks/hard", token);
        return disks;
    }

    private async Task AddDiskFilesAsync(JsonArray files, string group, string dir,
                                         CancellationToken token)
    {
        JsonArray rows = await _management.ListDirectoryAsync(EnsureAbsolutePath(dir), token);

        foreach (JsonNode? row in rows)
        {
            if (row?["dir"]?.GetValue<bool>() == true)
                continue;
            string name = row?["name"]?.GetValue<string>() ?? "";
            if (string.IsNullOrWhiteSpace(name))
                continue;
            if (!name.EndsWith(".ndi", StringComparison.OrdinalIgnoreCase))
                continue;

            long size = row?["size"]?.GetValue<long>() ?? 0;
            string path = "/" + CleanRelativePath($"{dir}/{name}");
            string bootCacheKey = path + "\n" + size;
            if (!_diskBootInfoCache.TryGetValue(bootCacheKey, out DiskBootInfo? boot))
            {
                boot = await ReadNdiBootInfoAsync(path, size, token);
                if (boot.Status != "unknown")
                    _diskBootInfoCache[bootCacheKey] = boot;
            }
            JsonNode item = new JsonObject
            {
                ["group"] = group,
                ["name"] = name,
                ["size"] = size,
                ["path"] = path,
                ["bootable"] = boot.Status == "bootable",
                ["bootStatus"] = boot.Status,
                ["bootFile"] = boot.FileName,
                ["bootError"] = boot.Error
            };
            files.Add(item);
        }
    }

    private async Task<DiskBootInfo> ReadNdiBootInfoAsync(string path, long size,
                                                          CancellationToken token)
    {
        try
        {
            if (size < NdiHeader.Size)
                return DiskBootInfo.Unknown("NDI image is too small");

            byte[] headerBytes = await ReadRemoteRangeAsync(path, 0, NdiHeader.Size, token);
            NdiHeader header = NdiHeader.FromBytes(headerBytes);

            long directoryOffset = checked((long)header.DirectoryStartSector * header.SectorSize);
            int directoryBytes = checked((int)(header.DirectorySectorCount * header.SectorSize));
            if (directoryBytes <= 0 || directoryBytes > MaxNdiDirectoryBytes)
                return DiskBootInfo.Unknown("NDI directory is outside supported bounds");
            if (directoryOffset < 0 || directoryOffset > uint.MaxValue)
                return DiskBootInfo.Unknown("NDI directory offset is too large");

            byte[] directoryBytesRaw = await ReadRemoteRangeAsync(path, (uint)directoryOffset,
                directoryBytes, token);
            NdiDirectory directory = NdiDirectory.FromBytes(directoryBytesRaw,
                checked((int)header.DirectorySectorCount));

            foreach (string file in new[] { "AUTOBOOT.bas", "AUTOBOOT.bin" })
            {
                NdiDirEntry? entry = directory.ListEntries(0xFFFF).FirstOrDefault(e =>
                    !e.IsDirectory &&
                    string.Equals(e.Filename, file, StringComparison.OrdinalIgnoreCase));
                if (entry is not null)
                    return DiskBootInfo.Bootable(entry.Filename);
            }

            return DiskBootInfo.NotBootable();
        }
        catch (Exception ex) when (ex is IOException or InvalidDataException or OverflowException or
            NovaHostCommandException or EndOfStreamException)
        {
            return DiskBootInfo.Unknown(ex.Message);
        }
    }

    private async Task<byte[]> ReadRemoteRangeAsync(string path, uint offset, int length,
                                                    CancellationToken token)
    {
        byte[] result = new byte[length];
        int written = 0;
        while (written < length)
        {
            uint chunkOffset = checked(offset + (uint)written);
            uint want = (uint)Math.Min(RemoteMetadataChunkBytes, length - written);
            NovaFileChunk chunk = await _management.ReadFileChunkAsync(path, chunkOffset, want, token);
            if (chunk.Data.Length == 0)
                throw new EndOfStreamException("remote file ended before NDI metadata was complete");
            chunk.Data.AsSpan().CopyTo(result.AsSpan(written));
            written += chunk.Data.Length;
        }
        return result;
    }

    private async Task<JsonObject> BuildLibraryAsync(CancellationToken token)
    {
        return new JsonObject
        {
            ["roms"] = await ListFilesAsync("roms", null, token),
            ["soundfonts"] = await ListFilesAsync("soundfonts", null, token),
            ["music"] = await ListFilesAsync("music", null, token),
            ["boot"] = await ListFilesAsync("assets/boot", null, token),
            ["sid"] = await ListFilesAsync("assets/sid", null, token)
        };
    }

    private async Task<JsonArray> ListFilesAsync(string dir, string? extension, CancellationToken token)
    {
        var files = new JsonArray();
        await AddFilesAsync(files, dir, dir, extension, token);
        return files;
    }

    private async Task AddFilesAsync(JsonArray files, string group, string dir, string? extension, CancellationToken token)
    {
        JsonArray rows = await _management.ListDirectoryAsync(EnsureAbsolutePath(dir), token);

        foreach (JsonNode? row in rows)
        {
            if (row?["dir"]?.GetValue<bool>() == true)
                continue;
            string name = row?["name"]?.GetValue<string>() ?? "";
            if (string.IsNullOrWhiteSpace(name))
                continue;
            if (extension is not null && !name.EndsWith(extension, StringComparison.OrdinalIgnoreCase))
                continue;

            JsonNode item = new JsonObject
            {
                ["group"] = group,
                ["name"] = name,
                ["size"] = row?["size"]?.GetValue<long>() ?? 0,
                ["path"] = "/" + CleanRelativePath($"{dir}/{name}")
            };
            files.Add(item);
        }
    }

    private async Task RuntimeSetAsync(HttpListenerContext context, CancellationToken token)
    {
        JsonObject payload = await ReadJsonObjectAsync(context.Request, token);
        string name = payload["name"]?.GetValue<string>()?.Trim() ?? "";
        if (name.Length == 0)
        {
            await SendErrorAsync(context.Response, 400, "runtime name is required", token);
            return;
        }

        JsonObject config = await ReadBootConfigAsync(token);
        JsonObject? languages = config["languages"] as JsonObject;
        if (languages is null || !languages.ContainsKey(name))
        {
            await SendErrorAsync(context.Response, 400, "runtime is not configured", token);
            return;
        }

        JsonObject entry = languages[name]?.AsObject() ?? new JsonObject();
        string rom = entry["rom"]?.GetValue<string>() ?? "";
        string? extensionRom = entry["extensionRom"]?.GetValue<string>();
        if (rom.Length == 0)
        {
            await SendErrorAsync(context.Response, 400, "runtime has no rom configured", token);
            return;
        }

        await _management.RuntimeAddAsync(name, rom, extensionRom, makeActive: true, token);
        InvalidateStatusCache();
        await SendJsonAsync(context.Response, new JsonObject { ["ok"] = true, ["activeRuntime"] = name }, token);
    }

    private async Task RuntimeAddAsync(HttpListenerContext context, CancellationToken token)
    {
        JsonObject payload = await ReadJsonObjectAsync(context.Request, token);
        string name = payload["name"]?.GetValue<string>()?.Trim() ?? "";
        string rom = payload["rom"]?.GetValue<string>()?.Trim() ?? "";
        string? extensionRom = payload["extensionRom"]?.GetValue<string>()?.Trim();
        bool makeActive = payload["makeActive"]?.GetValue<bool>() == true;
        if (name.Length == 0 || rom.Length == 0)
        {
            await SendErrorAsync(context.Response, 400, "runtime name and rom are required", token);
            return;
        }

        await _management.RuntimeAddAsync(name, EnsureAbsolutePath(rom),
            string.IsNullOrWhiteSpace(extensionRom) ? null : EnsureAbsolutePath(extensionRom),
            makeActive, token);
        InvalidateStatusCache();
        await SendJsonAsync(context.Response, new JsonObject { ["ok"] = true, ["runtime"] = name }, token);
    }

    private async Task RuntimeRemoveAsync(HttpListenerContext context, CancellationToken token)
    {
        JsonObject payload = await ReadJsonObjectAsync(context.Request, token);
        string name = payload["name"]?.GetValue<string>()?.Trim() ?? "";
        if (name.Length == 0)
        {
            await SendErrorAsync(context.Response, 400, "runtime name is required", token);
            return;
        }

        JsonObject config = await ReadBootConfigAsync(token);
        string active = config["vm"]?["defaultRuntime"]?.GetValue<string>() ?? "";
        if (name == active)
        {
            await SendErrorAsync(context.Response, 400, "active runtime cannot be removed", token);
            return;
        }

        await _management.RuntimeRemoveAsync(name, token);
        InvalidateStatusCache();
        await SendJsonAsync(context.Response, new JsonObject { ["ok"] = true, ["removedRuntime"] = name }, token);
    }

    private async Task RuntimePackageAsync(HttpListenerContext context, CancellationToken token)
    {
        string fileName = Query(context.Request, "filename") ?? "runtime.zip";
        string? nameOverride = Query(context.Request, "name");
        bool makeActive = string.Equals(Query(context.Request, "makeActive"), "true",
            StringComparison.OrdinalIgnoreCase);

        byte[] packageBytes = await ReadBodyAsync(context.Request, token);
        RuntimePackage package = ReadRuntimePackage(fileName, packageBytes, nameOverride);
        string runtimeDir = "/roms/runtimes/" + SafeRemoteName(package.Name);
        string romPath = runtimeDir + "/" + SafeRemoteName(package.Rom.FileName);
        string? extensionPath = package.ExtensionRom is null
            ? null
            : runtimeDir + "/" + SafeRemoteName(package.ExtensionRom.FileName);

        await UploadBytesAsync(romPath, package.Rom.Bytes, token);
        if (package.ExtensionRom is not null && extensionPath is not null)
            await UploadBytesAsync(extensionPath, package.ExtensionRom.Bytes, token);

        await _management.RuntimeAddAsync(package.Name, romPath, extensionPath, makeActive, token);
        InvalidateStatusCache();
        InvalidateInventoryCache();
        await SendJsonAsync(context.Response, new JsonObject
        {
            ["ok"] = true,
            ["runtime"] = package.Name,
            ["rom"] = romPath,
            ["extensionRom"] = extensionPath,
            ["active"] = makeActive
        }, token);
    }

    private async Task<JsonObject> ReadBootConfigAsync(CancellationToken token)
    {
        try
        {
            string text = await _management.ReadTextFileAsync("/config/boot.json", token);
            if (JsonNode.Parse(text) is JsonObject obj)
                return MergeBootConfigDefaults(obj);
        }
        catch
        {
        }

        return MergeBootConfigDefaults(null);
    }

    private static JsonObject DefaultBootConfig() => JsonNode.Parse("""
        {
          "vm": {
            "defaultRuntime": "novabasic"
          },
          "languages": {
            "novabasic": {
              "rom": "/roms/novabasic.bin",
              "extensionRom": "/roms/extension.bin",
              "autoboot": true
            }
          },
          "mounts": {},
          "network": {}
        }
        """)!.AsObject();

    private static JsonObject MergeBootConfigDefaults(JsonObject? config)
    {
        JsonObject merged = DefaultBootConfig();
        if (config is null)
            return merged;

        if (config["mounts"] is JsonObject mounts)
            merged["mounts"] = mounts.DeepClone();
        if (config["network"] is JsonObject network)
            merged["network"] = network.DeepClone();

        JsonObject mergedLanguages = merged["languages"]!.AsObject();
        if (config["languages"] is JsonObject languages)
        {
            foreach ((string name, JsonNode? entry) in languages)
            {
                if (entry is not null)
                    mergedLanguages[name] = entry.DeepClone();
            }
        }

        JsonObject mergedVm = merged["vm"]!.AsObject();
        if (config["vm"] is JsonObject vm)
        {
            foreach ((string key, JsonNode? value) in vm)
            {
                if (value is not null)
                    mergedVm[key] = value.DeepClone();
            }
        }

        string active = mergedVm["defaultRuntime"]?.GetValue<string>() ?? "";
        if (active.Length == 0 || !mergedLanguages.ContainsKey(active))
            mergedVm["defaultRuntime"] = mergedLanguages.ContainsKey("novabasic")
                ? "novabasic"
                : mergedLanguages.First().Key;
        return merged;
    }

    private async Task ProxyUploadAsync(HttpListenerContext context, string boardPath, CancellationToken token)
    {
        long length = context.Request.ContentLength64;
        await BroadcastEventAsync("upload", new JsonObject
        {
            ["path"] = boardPath,
            ["state"] = "started",
            ["size"] = length
        }, token);
        var progress = new Progress<long>(written =>
        {
            _ = BroadcastEventAsync("upload", new JsonObject
            {
                ["path"] = boardPath,
                ["state"] = "progress",
                ["written"] = written,
                ["size"] = length
            }, CancellationToken.None);
        });
        await _management.WriteFileAsync(boardPath, context.Request.InputStream,
            length, progress, token);
        InvalidateStatusCache();
        InvalidateInventoryCache();
        await BroadcastEventAsync("upload", new JsonObject
        {
            ["path"] = boardPath,
            ["state"] = "complete",
            ["size"] = length
        }, token);
        await SendJsonAsync(context.Response, new JsonObject
        {
            ["ok"] = true,
            ["path"] = boardPath,
            ["size"] = length
        }, token);
    }

    private async Task UploadBytesAsync(string path, byte[] bytes, CancellationToken token)
    {
        using var stream = new MemoryStream(bytes);
        await _management.WriteFileAsync(path, stream, bytes.Length, null, token);
    }

    private static async Task<JsonObject> ReadJsonObjectAsync(HttpListenerRequest request, CancellationToken token)
    {
        byte[] data = await ReadBodyAsync(request, token);
        if (data.Length == 0)
            return new JsonObject();
        return JsonNode.Parse(data) as JsonObject ?? new JsonObject();
    }

    private static async Task<byte[]> ReadBodyAsync(HttpListenerRequest request, CancellationToken token)
    {
        using var ms = new MemoryStream();
        await request.InputStream.CopyToAsync(ms, token);
        return ms.ToArray();
    }

    private void InvalidateStatusCache()
    {
        _statusCacheAt = DateTimeOffset.MinValue;
    }

    private void InvalidateInventoryCache()
    {
        _inventoryCacheAt = DateTimeOffset.MinValue;
        _diskBootInfoCache.Clear();
    }

    private sealed record DiskBootInfo(string Status, string FileName, string Error)
    {
        public static DiskBootInfo Bootable(string fileName) =>
            new("bootable", fileName, "");

        public static DiskBootInfo NotBootable() =>
            new("not-bootable", "", "");

        public static DiskBootInfo Unknown(string error) =>
            new("unknown", "", error);
    }

    private async Task HandleEventsAsync(HttpListenerContext context, CancellationToken token)
    {
        if (!context.Request.IsWebSocketRequest)
        {
            await SendErrorAsync(context.Response, 400, "websocket required", token);
            return;
        }

        HttpListenerWebSocketContext wsContext = await context.AcceptWebSocketAsync(null);
        WebSocket socket = wsContext.WebSocket;
        Guid id = Guid.NewGuid();
        _eventClients[id] = socket;
        await SendSocketEventAsync(socket, "connected", new JsonObject
        {
            ["target"] = BoardBase,
            ["management"] = _management.ConnectionState
        }, token);

        byte[] buffer = new byte[256];
        try
        {
            while (!token.IsCancellationRequested &&
                   socket.State is WebSocketState.Open or WebSocketState.CloseReceived)
            {
                WebSocketReceiveResult result = await socket.ReceiveAsync(buffer, token);
                if (result.MessageType == WebSocketMessageType.Close)
                    break;
            }
        }
        catch
        {
        }
        finally
        {
            _eventClients.TryRemove(id, out _);
            try
            {
                if (socket.State == WebSocketState.Open)
                    await socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "bye", CancellationToken.None);
            }
            catch
            {
            }
            socket.Dispose();
        }
    }

    private async Task BroadcastEventAsync(string type, JsonObject data, CancellationToken token)
    {
        foreach ((Guid id, WebSocket socket) in _eventClients)
        {
            if (socket.State != WebSocketState.Open)
            {
                _eventClients.TryRemove(id, out _);
                continue;
            }
            try
            {
                await SendSocketEventAsync(socket, type, data.DeepClone().AsObject(), token);
            }
            catch
            {
                _eventClients.TryRemove(id, out _);
            }
        }
    }

    private static async Task SendSocketEventAsync(WebSocket socket, string type,
                                                   JsonObject data,
                                                   CancellationToken token)
    {
        var payload = new JsonObject
        {
            ["type"] = type,
            ["at"] = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            ["data"] = data
        };
        byte[] bytes = Encoding.UTF8.GetBytes(payload.ToJsonString(JsonOptions));
        await socket.SendAsync(bytes, WebSocketMessageType.Text, true, token);
    }

    private static async Task SendOkAsync(HttpListenerResponse response,
                                          JsonObject data,
                                          CancellationToken token)
    {
        JsonObject body = data.DeepClone().AsObject();
        body["ok"] = true;
        await SendJsonAsync(response, body, token);
    }

    private static async Task SendHtmlAsync(HttpListenerResponse response, string html, CancellationToken token)
    {
        response.StatusCode = 200;
        response.ContentType = "text/html; charset=utf-8";
        response.Headers["Cache-Control"] = "no-store";
        await WriteTextAsync(response, html, token);
    }

    private static async Task SendJsonAsync(HttpListenerResponse response, JsonNode? node, CancellationToken token)
    {
        response.StatusCode = response.StatusCode == 200 ? 200 : response.StatusCode;
        response.ContentType = "application/json; charset=utf-8";
        response.Headers["Cache-Control"] = "no-store";
        await WriteTextAsync(response, (node ?? new JsonObject()).ToJsonString(JsonOptions), token);
    }

    private static async Task SendErrorAsync(HttpListenerResponse response, int status, string message,
                                             CancellationToken token)
    {
        response.StatusCode = status;
        await SendJsonAsync(response, new JsonObject { ["ok"] = false, ["error"] = message }, token);
    }

    private static async Task WriteTextAsync(HttpListenerResponse response, string text, CancellationToken token)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(text);
        response.ContentLength64 = bytes.Length;
        await response.OutputStream.WriteAsync(bytes, token);
    }

    private static string CleanRelativePath(string path)
    {
        string value = path.Trim().Replace('\\', '/');
        while (value.StartsWith('/'))
            value = value[1..];
        return value;
    }

    private static string EnsureAbsolutePath(string path)
    {
        string clean = CleanRelativePath(path);
        return "/" + clean;
    }

    private static string SafeRemoteName(string name)
    {
        string value = Path.GetFileName(name.Trim());
        if (value.Length == 0)
            value = "runtime";
        var sb = new StringBuilder(value.Length);
        foreach (char ch in value)
            sb.Append(char.IsLetterOrDigit(ch) || ch is '.' or '_' or '-' ? ch : '_');
        return sb.ToString();
    }

    private static RuntimePackage ReadRuntimePackage(string fileName, byte[] data,
                                                     string? nameOverride)
    {
        string lower = fileName.ToLowerInvariant();
        if (!lower.EndsWith(".zip", StringComparison.Ordinal) &&
            !lower.EndsWith(".nvr", StringComparison.Ordinal) &&
            !lower.EndsWith(".nrp", StringComparison.Ordinal))
        {
            if (!IsRomFile(fileName))
                throw new InvalidOperationException("runtime package must be a .zip/.nvr or a ROM file");
            string name = CleanRuntimeName(nameOverride, Path.GetFileNameWithoutExtension(fileName));
            return new RuntimePackage(name, new RuntimePackageFile(fileName, data), null);
        }

        using var ms = new MemoryStream(data);
        using var zip = new ZipArchive(ms, ZipArchiveMode.Read);
        JsonObject? manifest = ReadRuntimeManifest(zip);
        List<RuntimePackageFile> roms = ReadRuntimePackageRoms(zip);
        if (roms.Count == 0)
            throw new InvalidOperationException("runtime package contains no .bin/.rom files");

        string packageBase = Path.GetFileNameWithoutExtension(fileName);
        string nameFromManifest = manifest?["name"]?.GetValue<string>() ?? "";
        string runtimeName = CleanRuntimeName(nameOverride, nameFromManifest.Length > 0 ? nameFromManifest : packageBase);

        RuntimePackageFile rom;
        RuntimePackageFile? extensionRom = null;
        string manifestRom = manifest?["rom"]?.GetValue<string>() ?? "";
        string manifestExt = manifest?["extensionRom"]?.GetValue<string>() ??
            manifest?["extension"]?.GetValue<string>() ?? "";
        if (manifestRom.Length > 0)
        {
            rom = FindPackageFile(roms, manifestRom);
            if (manifestExt.Length > 0)
                extensionRom = FindPackageFile(roms, manifestExt);
        }
        else
        {
            (rom, extensionRom) = InferPackageRoms(roms);
        }

        return new RuntimePackage(runtimeName, rom, extensionRom);
    }

    private static JsonObject? ReadRuntimeManifest(ZipArchive zip)
    {
        foreach (string manifestName in new[] { "nova-runtime.json", "runtime.json", "package.json" })
        {
            ZipArchiveEntry? entry = zip.Entries.FirstOrDefault(e =>
                string.Equals(Path.GetFileName(e.FullName), manifestName, StringComparison.OrdinalIgnoreCase));
            if (entry is null)
                continue;
            using Stream stream = entry.Open();
            return JsonNode.Parse(stream) as JsonObject;
        }
        return null;
    }

    private static List<RuntimePackageFile> ReadRuntimePackageRoms(ZipArchive zip)
    {
        var roms = new List<RuntimePackageFile>();
        foreach (ZipArchiveEntry entry in zip.Entries)
        {
            if (entry.FullName.EndsWith("/", StringComparison.Ordinal))
                continue;
            if (!IsRomFile(entry.Name))
                continue;
            using Stream stream = entry.Open();
            using var ms = new MemoryStream();
            stream.CopyTo(ms);
            roms.Add(new RuntimePackageFile(entry.Name, ms.ToArray()));
        }
        return roms;
    }

    private static RuntimePackageFile FindPackageFile(List<RuntimePackageFile> files, string requested)
    {
        RuntimePackageFile? file = files.FirstOrDefault(f =>
            string.Equals(f.FileName, requested, StringComparison.OrdinalIgnoreCase)) ??
            files.FirstOrDefault(f =>
                string.Equals(Path.GetFileName(f.FileName), Path.GetFileName(requested),
                    StringComparison.OrdinalIgnoreCase));
        return file ?? throw new InvalidOperationException($"runtime package manifest references missing ROM: {requested}");
    }

    private static (RuntimePackageFile Rom, RuntimePackageFile? ExtensionRom) InferPackageRoms(List<RuntimePackageFile> roms)
    {
        if (roms.Count == 1)
            return (roms[0], null);
        if (roms.Count != 2)
            throw new InvalidOperationException("runtime package needs nova-runtime.json when it contains more than two ROMs");

        RuntimePackageFile? extension = roms.FirstOrDefault(r => LooksLikeExtensionRom(r.FileName));
        if (extension is null)
            throw new InvalidOperationException("two-ROM runtime packages need nova-runtime.json or an extension ROM filename containing 'ext' or 'extension'");
        RuntimePackageFile primary = roms.First(r => !ReferenceEquals(r, extension));
        return (primary, extension);
    }

    private static bool IsRomFile(string fileName)
    {
        string ext = Path.GetExtension(fileName);
        return ext.Equals(".bin", StringComparison.OrdinalIgnoreCase) ||
            ext.Equals(".rom", StringComparison.OrdinalIgnoreCase);
    }

    private static bool LooksLikeExtensionRom(string fileName)
    {
        string lower = Path.GetFileNameWithoutExtension(fileName).ToLowerInvariant();
        return lower.Contains("extension", StringComparison.Ordinal) ||
            lower.Contains("_ext", StringComparison.Ordinal) ||
            lower.EndsWith("ext", StringComparison.Ordinal);
    }

    private static string CleanRuntimeName(string? preferred, string fallback)
    {
        string raw = string.IsNullOrWhiteSpace(preferred) ? fallback : preferred.Trim();
        var sb = new StringBuilder(raw.Length);
        foreach (char ch in raw.ToLowerInvariant())
            sb.Append(char.IsLetterOrDigit(ch) || ch is '_' or '-' ? ch : '_');
        string value = sb.ToString().Trim('_');
        return value.Length == 0 ? "runtime" : value;
    }

    private static JsonObject ErrorObject(string message) => new()
    {
        ["ok"] = false,
        ["error"] = message
    };

    private static bool IsDriveSlot(string slot) =>
        slot is "fd0" or "fd1" or "fd2" or "fd3" or "hd0" or "hd1";

    private static string? Query(HttpListenerRequest request, string name) =>
        request.QueryString[name];

    private sealed record RuntimePackage(string Name, RuntimePackageFile Rom,
                                         RuntimePackageFile? ExtensionRom);

    private sealed record RuntimePackageFile(string FileName, byte[] Bytes);

    private const string IndexHtml = """
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Nova Control Center</title>
<style>
:root{
  --bench:#0d100d;
  --deck:#151a15;
  --deck2:#1c231c;
  --slot:#202820;
  --inset:#0b0f0c;
  --line:rgba(223,238,214,.14);
  --line2:rgba(223,238,214,.28);
  --ink:#edf6e9;
  --muted:#a7b5a0;
  --dim:#6f7d68;
  --nova:#65e49a;
  --cyan:#6fc8ff;
  --amber:#dfbd62;
  --red:#ec735f;
  --ok:#5ce18a;
}
*{box-sizing:border-box}
html,body{min-height:100%}
body{
  margin:0;
  color:var(--ink);
  background:
    linear-gradient(180deg,rgba(101,228,154,.06),transparent 260px),
    radial-gradient(circle at 18% 0,rgba(111,200,255,.06),transparent 260px),
    var(--bench);
  font:14px/1.45 Inter,ui-sans-serif,system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;
}
button,input,select{font:inherit}
button{
  min-height:36px;
  border:1px solid var(--line2);
  color:var(--ink);
  background:#242c23;
  border-radius:7px;
  padding:8px 11px;
  cursor:pointer;
}
button:hover{border-color:rgba(101,228,154,.58);background:#2b362b}
button:disabled{cursor:not-allowed;color:var(--dim);border-color:var(--line);background:#171c17}
.primary{background:#173a25;border-color:rgba(101,228,154,.48)}
.primary:hover{background:#1c472e}
.danger:hover{border-color:rgba(236,115,95,.72);background:#39211e}
input,select{
  min-width:0;
  color:var(--ink);
  background:var(--inset);
  border:1px solid var(--line);
  border-radius:7px;
  padding:8px;
}
.page{max-width:1320px;margin:0 auto;padding:22px}
.top{
  display:grid;
  grid-template-columns:auto 1fr auto;
  gap:18px;
  align-items:center;
  padding-bottom:18px;
  border-bottom:1px solid var(--line);
}
.brand{display:flex;align-items:center;gap:13px}
.mark{
  width:50px;height:50px;border:1px solid rgba(101,228,154,.46);
  border-radius:8px;background:#101610;padding:7px;
  display:grid;grid-template-columns:repeat(5,1fr);gap:4px;
  box-shadow:inset 0 0 0 1px rgba(255,255,255,.03),0 0 26px rgba(101,228,154,.08);
}
.mark i{display:block;border-radius:2px;background:var(--nova)}
.mark i:nth-child(2n){background:var(--cyan)}
.mark i:nth-child(3n){background:var(--amber)}
.mark i:nth-child(5n){background:#f4f0cc}
h1{font-size:19px;letter-spacing:0;margin:0}
.sub{color:var(--muted);font-size:12px;margin-top:2px}
.status-grid{display:grid;grid-template-columns:repeat(6,minmax(88px,1fr));gap:8px}
.lamp{
  min-height:50px;
  border:1px solid var(--line);
  border-radius:8px;
  background:rgba(21,26,21,.78);
  padding:8px 9px;
}
.lamp b{display:flex;align-items:center;gap:7px;font-size:11px;color:var(--muted);font-weight:700;text-transform:uppercase}
.lamp span{display:block;margin-top:4px;font-size:13px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.dot{width:8px;height:8px;border-radius:50%;background:var(--dim);box-shadow:0 0 0 3px rgba(111,125,104,.13)}
.ok .dot{background:var(--ok);box-shadow:0 0 0 3px rgba(92,225,138,.13)}
.warn .dot{background:var(--amber);box-shadow:0 0 0 3px rgba(223,189,98,.14)}
.bad .dot{background:var(--red);box-shadow:0 0 0 3px rgba(236,115,95,.13)}
.actions{display:flex;gap:8px;justify-content:flex-end;flex-wrap:wrap}
.main{display:grid;grid-template-columns:1.35fr .95fr;gap:16px;margin-top:16px}
.side{display:grid;gap:16px;align-content:start}
.panel{
  border:1px solid var(--line);
  border-radius:8px;
  background:rgba(21,26,21,.9);
  overflow:hidden;
}
.panel-head{
  min-height:48px;
  display:flex;align-items:center;justify-content:space-between;gap:10px;
  padding:11px 13px;
  border-bottom:1px solid var(--line);
  background:rgba(28,35,28,.62);
}
.panel-head h2{font-size:13px;text-transform:uppercase;letter-spacing:.08em;margin:0;color:#dcead6}
.panel-body{padding:13px}
.slot-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}
.slot{
  border:1px solid var(--line);
  border-radius:8px;
  background:var(--slot);
  padding:11px;
  min-height:128px;
}
.slot-top{display:flex;align-items:center;justify-content:space-between;gap:8px}
.slot-name{font-size:18px;font-weight:850;letter-spacing:.02em}
.badge{border:1px solid var(--line);border-radius:999px;padding:3px 8px;color:var(--muted);font-size:12px}
.badge.mounted,.badge.active{color:#061009;background:var(--ok);border-color:transparent}
.path{
  margin:11px 0 10px;
  min-height:38px;
  color:var(--muted);
  font:12px/1.35 ui-monospace,SFMono-Regular,Menlo,Consolas,monospace;
  word-break:break-all;
}
.slot-actions,.row-actions{display:flex;gap:7px;flex-wrap:wrap}
.upload,.runtime-controls,.library-controls{
  display:grid;
  gap:8px;
  margin-bottom:12px;
}
.upload{grid-template-columns:1fr auto auto}
.runtime-controls{grid-template-columns:1fr auto}
.library-controls{grid-template-columns:auto 1fr auto}
.runtime-deploy{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:12px}
.runtime-deploy .wide{grid-column:1 / -1}
.check{display:flex;align-items:center;gap:7px;color:var(--muted);font-size:12px}
.check input{width:16px;height:16px;min-width:16px}
.mount-existing{
  display:grid;
  grid-template-columns:auto 1fr;
  gap:8px;
  align-items:center;
  margin-bottom:8px;
}
.disk-panel .panel-body{
  display:grid;
  gap:10px;
}
.disk-panel .upload,.disk-panel .mount-existing{margin-bottom:0}
.scroll-list{
  max-height:clamp(168px,26vh,260px);
  overflow:auto;
  padding-right:4px;
  border-top:1px solid var(--line);
}
.row{
  display:grid;
  grid-template-columns:1fr auto;
  gap:8px;
  align-items:center;
  padding:8px 0;
  border-bottom:1px solid var(--line);
}
.row:last-child{border-bottom:0}
.disk-list .row{
  grid-template-columns:minmax(0,1fr) auto;
  min-height:44px;
  padding:7px 0;
}
.disk-list .row>div:first-child{min-width:0}
.disk-title{display:flex;align-items:center;gap:7px;min-width:0}
.disk-list .name,.disk-list .meta{
  display:block;
  max-width:100%;
  overflow:hidden;
  text-overflow:ellipsis;
  white-space:nowrap;
}
.disk-title .name{min-width:0}
.boot-chip{
  flex:0 0 auto;
  border:1px solid var(--line);
  border-radius:999px;
  padding:2px 7px;
  font-size:11px;
  line-height:1.35;
  color:var(--dim);
  background:rgba(14,18,14,.7);
}
.boot-chip.bootable{color:#061009;background:var(--ok);border-color:transparent}
.boot-chip.unknown{color:var(--amber);border-color:rgba(223,189,98,.35)}
.disk-list .row-actions{flex-wrap:nowrap}
.disk-list .row-actions button{min-height:30px;padding:5px 8px}
.name{
  color:var(--ink);
  background:transparent;
  border:0;
  min-height:0;
  padding:0;
  text-align:left;
  font-weight:700;
}
.name.selected{color:var(--nova)}
.meta{color:var(--dim);font-size:12px}
.toast{margin-top:12px;min-height:22px;color:var(--muted);font-size:12px}
.toast.error{color:var(--red)}
@media (max-width:980px){
  .top{grid-template-columns:1fr}
  .actions{justify-content:flex-start}
  .status-grid{grid-template-columns:repeat(2,minmax(0,1fr))}
  .main{grid-template-columns:1fr}
  .slot-grid{grid-template-columns:1fr}
  .runtime-deploy{grid-template-columns:1fr}
  .upload,.library-controls{grid-template-columns:1fr}
}
</style>
</head>
<body>
<div class="page">
  <header class="top">
    <div class="brand">
      <div class="mark" aria-hidden="true"><i></i><i></i><i></i><i></i><i></i><i></i><i></i><i></i><i></i><i></i></div>
      <div>
        <h1>Nova Control Center</h1>
        <div class="sub" id="targetLine">local console</div>
      </div>
    </div>
    <section class="status-grid" aria-label="System status">
      <div class="lamp" id="stBoot"><b><i class="dot"></i>Boot</b><span>...</span></div>
      <div class="lamp" id="stSd"><b><i class="dot"></i>SD</b><span>...</span></div>
      <div class="lamp" id="stBridge"><b><i class="dot"></i>Bridge</b><span>...</span></div>
      <div class="lamp" id="stWifi"><b><i class="dot"></i>WiFi</b><span>...</span></div>
      <div class="lamp" id="stHttp"><b><i class="dot"></i>HTTP</b><span>...</span></div>
      <div class="lamp" id="stAudio"><b><i class="dot"></i>Audio</b><span>...</span></div>
    </section>
    <div class="actions">
      <button id="refreshBtn">Refresh</button>
      <button id="vmResetBtn">VM Reset</button>
      <button class="danger" id="rebootBtn">Host Reboot</button>
    </div>
  </header>

  <main class="main">
    <section class="panel">
      <div class="panel-head">
        <h2>Drive Bay</h2>
        <span class="meta" id="selectedPath">No image selected</span>
      </div>
      <div class="panel-body">
        <div class="slot-grid" id="slotGrid"></div>
      </div>
    </section>

    <div class="side">
      <section class="panel disk-panel">
        <div class="panel-head">
          <h2>Disk Images</h2>
          <span class="meta" id="diskCount">...</span>
        </div>
        <div class="panel-body">
          <div class="upload">
            <input type="file" id="diskFile" accept=".ndi">
            <select id="diskTarget" aria-label="Disk target">
              <option value="disks/floppy">Floppy</option>
              <option value="disks/hard">Hard disk</option>
            </select>
            <button class="primary" id="diskUploadBtn">Upload</button>
          </div>
          <div class="mount-existing">
            <span class="meta">Mount target</span>
            <select id="mountTarget" aria-label="Mount target">
              <option value="fd0">FD0</option>
              <option value="fd1">FD1</option>
              <option value="fd2">FD2</option>
              <option value="fd3">FD3</option>
              <option value="hd0">HD0</option>
              <option value="hd1">HD1</option>
            </select>
          </div>
          <div class="scroll-list disk-list" id="diskList"></div>
          <div class="toast" id="toast"></div>
        </div>
      </section>

      <section class="panel">
        <div class="panel-head">
          <h2>Runtime</h2>
          <span class="meta" id="runtimeStatus">...</span>
        </div>
        <div class="panel-body">
          <div class="runtime-controls">
            <select id="runtimeSelect" aria-label="Active runtime"></select>
            <button class="primary" id="setRuntimeBtn">Set Active</button>
          </div>
          <div class="runtime-deploy">
            <input class="wide" type="file" id="runtimePackage" accept=".zip,.nvr,.nrp,.bin,.rom">
            <input id="runtimeName" placeholder="runtime name override">
            <label class="check"><input type="checkbox" id="runtimeMakeActive"> Make active</label>
            <button class="primary wide" id="deployRuntimeBtn">Install Runtime Package</button>
          </div>
          <div id="runtimeList"></div>
        </div>
      </section>

      <section class="panel">
        <div class="panel-head">
          <h2>Library</h2>
          <span class="meta" id="libraryCount">...</span>
        </div>
        <div class="panel-body">
          <div class="library-controls">
            <select id="libraryKind" aria-label="Library kind">
              <option value="roms">ROMs</option>
              <option value="soundfonts">SoundFonts</option>
              <option value="music">Music</option>
              <option value="boot">Boot assets</option>
              <option value="sid">SID assets</option>
            </select>
            <input type="file" id="libraryFile">
            <button class="primary" id="libraryUploadBtn">Upload</button>
          </div>
          <div id="libraryList"></div>
        </div>
      </section>
    </div>
  </main>
</div>
<script>
const slots = ["fd0","fd1","fd2","fd3","hd0","hd1"];
const libraryDirs = {roms:"roms",soundfonts:"soundfonts",music:"music",boot:"assets/boot",sid:"assets/sid"};
let state = {};
let selected = "";

const $ = id => document.getElementById(id);
const esc = value => String(value ?? "").replace(/[&<>"']/g, ch => ({"&":"&amp;","<":"&lt;",">":"&gt;","\"":"&quot;","'":"&#39;"}[ch]));
const cleanPath = p => String(p || "").replace(/^\/+/,"");
const safeName = name => String(name || "file").replace(/[^A-Za-z0-9._-]/g, "_");

function toast(text, bad=false){
  const el = $("toast");
  el.textContent = text || "";
  el.className = bad ? "toast error" : "toast";
}

function lamp(id, state, text){
  const el = $(id);
  el.className = "lamp " + state;
  el.querySelector("span").textContent = text;
}

function fmtBytes(n){
  n = Number(n || 0);
  if (n >= 1024*1024) return (n/1024/1024).toFixed(1) + " MB";
  if (n >= 1024) return (n/1024).toFixed(1) + " KB";
  return n + " B";
}

function bad(node){ return node && node.ok === false && node.error; }

async function api(path, opts){
  const res = await fetch(path, opts || {});
  const text = await res.text();
  let body = null;
  try{ body = text ? JSON.parse(text) : null; }catch{ body = text; }
  if (!res.ok){
    const msg = body && body.error ? body.error : (text || `${res.status} ${res.statusText}`);
    throw new Error(msg);
  }
  return body;
}

function connectEvents(){
  const proto = location.protocol === "https:" ? "wss:" : "ws:";
  const ws = new WebSocket(`${proto}//${location.host}/events`);
  ws.onmessage = ev => {
    try{
      const msg = JSON.parse(ev.data);
      if (msg.type === "upload" && msg.data){
        const d = msg.data;
        if (d.state === "progress" && d.size > 0) toast(`Uploading ${Math.floor((d.written / d.size) * 100)}%`);
        if (d.state === "complete") toast("Upload complete");
      }
    }catch{}
  };
  ws.onclose = () => setTimeout(connectEvents, 2000);
}

async function uploadTo(path, file){
  await api("/api/sd?path=" + encodeURIComponent(path), {method:"PUT", body:file});
}

async function removePath(path){
  await api("/api/sd?path=" + encodeURIComponent(path), {method:"DELETE"});
}

function renderStatus(){
  const h = state.health || {};
  const sd = state.sdStatus || {};
  const wifi = state.wifi || {};
  const audio = state.audio || {};
  $("targetLine").textContent = state.target || "local console";
  if (bad(h)){
    lamp("stBoot","bad","offline");
    lamp("stBridge","bad","offline");
    lamp("stHttp","bad","offline");
  } else {
    const ready = h.bootPhase === "ready";
    lamp("stBoot", ready ? "ok" : "warn", h.bootPhase || "unknown");
    lamp("stBridge", h.fpgaBridgeAvailable ? "ok" : "warn", h.fpgaBridgeAvailable ? "available" : "busy");
    const httpOk = h.http && h.http.taskRunning && h.http.taskLoopSeen;
    lamp("stHttp", httpOk ? "ok" : "warn", httpOk ? "serving" : "degraded");
  }
  lamp("stSd", !bad(sd) && sd.mounted ? "ok" : "bad", !bad(sd) && sd.mounted ? (sd.cardType || "mounted") : "missing");
  lamp("stWifi", !bad(wifi) && wifi.connected ? "ok" : (!bad(wifi) && wifi.configured ? "warn" : "bad"), !bad(wifi) && wifi.connected ? (wifi.localIp || "connected") : (!bad(wifi) && wifi.configured ? "configured" : "not set"));
  const midiActive = !bad(audio) && audio.midiPlaying === true;
  const sidActive = !bad(audio) && audio.sidPlaying === true;
  const legacyActive = !bad(audio) && !!(audio.playing || audio.active || audio.kind && audio.kind !== "none");
  const active = midiActive || sidActive || legacyActive;
  const audioLabel = midiActive
    ? "MIDI"
    : sidActive
      ? "SID"
      : (audio.kind && audio.kind !== "none" ? audio.kind : "playing");
  lamp("stAudio", active ? "ok" : "", active ? audioLabel : "idle");
}

function selectDisk(path){
  selected = path || "";
  $("selectedPath").textContent = selected || "No image selected";
  renderDisks();
  renderDrives();
}

function renderDrives(){
  const grid = $("slotGrid");
  grid.innerHTML = "";
  const rows = Array.isArray(state.drives) ? state.drives : [];
  const bySlot = Object.fromEntries(rows.map(d => [d.slot, d]));
  for (const slot of slots){
    const d = bySlot[slot] || {slot, mounted:false, currentPath:"", configuredPath:""};
    const imagePath = d.configuredPath || "";
    const shown = d.mounted ? (imagePath || "mounted image") : imagePath;
    const cwd = d.mounted && d.currentPath && d.currentPath !== "/"
      ? `<div class="meta">cwd ${esc(d.currentPath)}</div>`
      : "";
    const el = document.createElement("article");
    el.className = "slot";
    el.innerHTML = `
      <div class="slot-top">
        <div class="slot-name">${slot.toUpperCase()}</div>
        <div class="badge ${d.mounted ? "mounted" : ""}">${d.mounted ? "mounted" : "empty"}</div>
      </div>
      <div class="path">${esc(shown || "no image")}${cwd}</div>
      <div class="slot-actions">
        <button class="primary" data-mount="${slot}" ${selected ? "" : "disabled"}>Mount</button>
        <button data-remount="${slot}" ${d.configuredPath ? "" : "disabled"}>Recall</button>
        <button class="danger" data-unmount="${slot}" ${d.mounted ? "" : "disabled"}>Unmount</button>
      </div>`;
    grid.appendChild(el);
  }
}

function renderDisks(){
  const list = $("diskList");
  const rows = Array.isArray(state.disks) ? state.disks : [];
  list.innerHTML = "";
  $("diskCount").textContent = rows.length + " image" + (rows.length === 1 ? "" : "s");
  for (const d of rows){
    const row = document.createElement("div");
    row.className = "row";
    const bootState = d.bootStatus || (d.bootable ? "bootable" : "not-bootable");
    const bootLabel = bootState === "bootable" ? "boot" : (bootState === "unknown" ? "?" : "data");
    const bootTitle = bootState === "bootable"
      ? "Bootable: " + (d.bootFile || "AUTOBOOT")
      : (bootState === "unknown" ? "Boot check failed: " + (d.bootError || "unknown") : "No root AUTOBOOT.bas or AUTOBOOT.bin");
    row.innerHTML = `
      <div>
        <div class="disk-title">
          <button class="name ${selected === d.path ? "selected" : ""}" data-select="${esc(d.path)}">${esc(d.name)}</button>
          <span class="boot-chip ${esc(bootState)}" title="${esc(bootTitle)}">${esc(bootLabel)}</span>
        </div>
        <div class="meta">${esc(d.group)} / ${fmtBytes(d.size)}</div>
      </div>
      <div class="row-actions">
        <button class="primary" data-mount-existing="${esc(d.path)}">Mount</button>
        <button class="danger" data-delete="${esc(d.path)}">Delete</button>
      </div>`;
    list.appendChild(row);
  }
  if (!rows.length) list.innerHTML = `<div class="meta">No disk images found.</div>`;
}

function renderRuntime(){
  const cfg = bad(state.bootConfig) ? null : state.bootConfig;
  const languages = cfg && cfg.languages ? cfg.languages : {};
  const names = Object.keys(languages).sort();
  const active = cfg && cfg.vm ? cfg.vm.defaultRuntime || "" : "";
  $("runtimeStatus").textContent = active ? "active: " + active : "not set";
  const select = $("runtimeSelect");
  select.innerHTML = "";
  for (const name of names){
    const option = document.createElement("option");
    option.value = name;
    option.textContent = name;
    option.selected = name === active;
    select.appendChild(option);
  }
  $("setRuntimeBtn").disabled = names.length === 0;
  const list = $("runtimeList");
  list.innerHTML = "";
  for (const name of names){
    const entry = languages[name] || {};
    const row = document.createElement("div");
    row.className = "row";
    row.innerHTML = `
      <div>
        <div class="name ${name === active ? "selected" : ""}">${esc(name)}</div>
        <div class="meta">${esc(entry.rom || "(no rom)")}${entry.extensionRom ? " / " + esc(entry.extensionRom) : ""}</div>
      </div>
      <div class="row-actions">
        <span class="badge ${name === active ? "active" : ""}">${name === active ? "active" : "ready"}</span>
        <button class="danger" data-remove-runtime="${esc(name)}" ${name === active ? "disabled" : ""}>Remove</button>
      </div>`;
    list.appendChild(row);
  }
  if (!names.length) list.innerHTML = `<div class="meta">No runtimes configured.</div>`;
}

function renderLibrary(){
  const kind = $("libraryKind").value;
  const rows = state.library && Array.isArray(state.library[kind]) ? state.library[kind] : [];
  const list = $("libraryList");
  list.innerHTML = "";
  $("libraryCount").textContent = rows.length + " file" + (rows.length === 1 ? "" : "s");
  for (const item of rows){
    const row = document.createElement("div");
    row.className = "row";
    row.innerHTML = `
      <div>
        <div class="name">${esc(item.name)}</div>
        <div class="meta">${esc(item.path)} / ${fmtBytes(item.size)}</div>
      </div>
      <div class="row-actions">
        <button class="danger" data-delete="${esc(item.path)}">Delete</button>
      </div>`;
    list.appendChild(row);
  }
  if (!rows.length) list.innerHTML = `<div class="meta">No files.</div>`;
}

async function refresh(loadInventory=false){
  toast("");
  state = {...state, ...(await api("/api/status"))};
  if (loadInventory || !Array.isArray(state.disks) || !state.library){
    const inventory = await api("/api/inventory");
    state = {...state, ...inventory};
  }
  renderStatus();
  renderDrives();
  renderDisks();
  renderRuntime();
  renderLibrary();
}

async function mount(slot, path){
  toast(`Mounting ${slot.toUpperCase()}...`);
  const opts = path ? {method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({path})} : {method:"POST"};
  await api(`/api/drives/${slot}/mount`, opts);
  toast(`${slot.toUpperCase()} mounted`);
  await refresh(false);
}

async function unmount(slot){
  toast(`Unmounting ${slot.toUpperCase()}...`);
  await api(`/api/drives/${slot}/unmount`, {method:"POST"});
  toast(`${slot.toUpperCase()} unmounted`);
  await refresh(false);
}

async function uploadDisk(){
  const file = $("diskFile").files[0];
  if (!file) return toast("Choose an NDI image.", true);
  if (!file.name.toLowerCase().endsWith(".ndi")) return toast("NDI images only.", true);
  const path = "/" + cleanPath($("diskTarget").value + "/" + safeName(file.name));
  toast("Uploading " + file.name + "...");
  await uploadTo(path, file);
  selectDisk(path);
  toast("Uploaded " + file.name);
  await refresh(true);
}

async function setRuntime(){
  const name = $("runtimeSelect").value;
  if (!name) return toast("Select a runtime.", true);
  await api("/api/runtime/set", {method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({name})});
  toast("Active runtime set to " + name + ". Host reboot to apply.");
  await refresh(false);
}

async function deployRuntime(){
  const file = $("runtimePackage").files[0];
  const name = $("runtimeName").value.trim();
  if (!file) return toast("Choose a runtime package.", true);
  const params = new URLSearchParams({filename:file.name,makeActive:$("runtimeMakeActive").checked ? "true" : "false"});
  if (name) params.set("name", name);
  toast("Installing " + file.name + "...");
  const result = await api("/api/runtime/package?" + params.toString(), {method:"PUT", body:file});
  toast("Runtime installed: " + result.runtime + ". Host reboot to apply.");
  await refresh(true);
}

async function uploadLibrary(){
  const file = $("libraryFile").files[0];
  if (!file) return toast("Choose a file.", true);
  const dir = libraryDirs[$("libraryKind").value];
  const path = "/" + cleanPath(dir + "/" + safeName(file.name));
  toast("Uploading " + file.name + "...");
  await uploadTo(path, file);
  toast("Uploaded " + file.name);
  await refresh(true);
}

async function deleteRemote(path){
  if (!confirm("Delete " + path + "?")) return;
  await removePath(path);
  if (selected === path) selectDisk("");
  toast("Deleted " + path);
  await refresh(true);
}

async function removeRuntime(name){
  if (!confirm("Remove runtime " + name + "?")) return;
  await api("/api/runtime/remove", {method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({name})});
  toast("Removed runtime " + name);
  await refresh(false);
}

async function vmReset(){
  if (!confirm("Reset the VM?")) return;
  await api("/api/vm-reset", {method:"POST"});
  toast("VM reset requested.");
  await refresh(false);
}

async function rebootHost(){
  if (!confirm("Reboot NovaHost?")) return;
  await api("/api/reboot", {method:"POST"});
  toast("NovaHost reboot requested.");
}

document.addEventListener("click", async ev => {
  const t = ev.target;
  try{
    if (t.dataset.select) selectDisk(t.dataset.select);
    if (t.dataset.mount) await mount(t.dataset.mount, selected);
    if (t.dataset.mountExisting) await mount($("mountTarget").value, t.dataset.mountExisting);
    if (t.dataset.remount) await mount(t.dataset.remount, "");
    if (t.dataset.unmount) await unmount(t.dataset.unmount);
    if (t.dataset.delete) await deleteRemote(t.dataset.delete);
    if (t.dataset.removeRuntime) await removeRuntime(t.dataset.removeRuntime);
  }catch(e){ toast(e.message, true); }
});
$("refreshBtn").addEventListener("click", () => refresh(true).catch(e => toast(e.message, true)));
$("vmResetBtn").addEventListener("click", () => vmReset().catch(e => toast(e.message, true)));
$("rebootBtn").addEventListener("click", () => rebootHost().catch(e => toast(e.message, true)));
$("diskUploadBtn").addEventListener("click", () => uploadDisk().catch(e => toast(e.message, true)));
$("setRuntimeBtn").addEventListener("click", () => setRuntime().catch(e => toast(e.message, true)));
$("deployRuntimeBtn").addEventListener("click", () => deployRuntime().catch(e => toast(e.message, true)));
$("libraryUploadBtn").addEventListener("click", () => uploadLibrary().catch(e => toast(e.message, true)));
$("libraryKind").addEventListener("change", renderLibrary);
refresh(true).catch(e => toast(e.message, true));
connectEvents();
setInterval(() => refresh(false).catch(() => {}), 15000);
</script>
</body>
</html>
""";
}
