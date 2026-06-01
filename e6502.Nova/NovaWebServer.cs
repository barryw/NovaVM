using System.Diagnostics;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;

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
    private readonly HttpClient _statusHttp = new() { Timeout = TimeSpan.FromSeconds(4) };
    private readonly HttpClient _commandHttp = new() { Timeout = TimeSpan.FromSeconds(30) };
    private readonly HttpClient _uploadHttp = new() { Timeout = TimeSpan.FromMinutes(5) };
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

    public LocalNovaWebServer(string boardHost, string bind, int port)
    {
        string normalizedHost = boardHost.StartsWith("http://", StringComparison.OrdinalIgnoreCase) ||
            boardHost.StartsWith("https://", StringComparison.OrdinalIgnoreCase)
            ? boardHost.TrimEnd('/')
            : $"http://{boardHost.TrimEnd('/')}";
        BoardBase = normalizedHost;

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
        try
        {
            string method = context.Request.HttpMethod.ToUpperInvariant();
            string path = context.Request.Url?.AbsolutePath ?? "/";

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
                await ProxyUploadAsync(context, SdUrl(sdPath), token);
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
                await ProxyBoardAsync(context, HttpMethod.Delete, SdUrl(sdPath), null, token);
                return;
            }

            if (method == "POST" && path.StartsWith("/api/drives/", StringComparison.Ordinal))
            {
                string suffix = path["/api/drives/".Length..];
                string[] parts = suffix.Split('/', StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length == 2 && IsDriveSlot(parts[0]) && parts[1] is "mount" or "unmount")
                {
                    byte[] body = await ReadBodyAsync(context.Request, token);
                    await ProxyBoardAsync(context, HttpMethod.Post, $"/drives/{parts[0]}/{parts[1]}", body, token);
                    return;
                }
            }

            if (method == "POST" && path == "/api/vm-reset")
            {
                await ProxyBoardAsync(context, HttpMethod.Post, "/vm-reset", Array.Empty<byte>(), token);
                return;
            }

            if (method == "POST" && path == "/api/reboot")
            {
                await ProxyBoardAsync(context, HttpMethod.Post, "/reboot", Array.Empty<byte>(), token);
                return;
            }

            if (method == "POST" && path == "/api/audio-stop")
            {
                await ProxyBoardAsync(context, HttpMethod.Post, "/audio-stop", Array.Empty<byte>(), token);
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
            context.Response.Close();
        }
    }

    private async Task<JsonObject> BuildStatusAsync(CancellationToken token)
    {
        var root = new JsonObject
        {
            ["target"] = BoardBase,
            ["health"] = await GetBoardJsonAsync("/health", token),
            ["sdStatus"] = await GetBoardJsonAsync("/sd-status", token),
            ["wifi"] = await GetBoardJsonAsync("/wifi", token),
            ["audio"] = await GetBoardJsonAsync("/audio-status", token),
            ["drives"] = await GetBoardJsonAsync("/drives", token),
            ["bootConfig"] = await GetBoardJsonAsync("/sd/config/boot.json", token)
        };
        return root;
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
        await AddFilesAsync(disks, "Floppy", "disks/floppy", ".ndi", token);
        await AddFilesAsync(disks, "Hard", "disks/hard", ".ndi", token);
        return disks;
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
        JsonNode? node = await GetBoardJsonAsync($"/sd/{CleanRelativePath(dir)}/", token);
        if (node is not JsonArray rows)
            return;

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

        JsonObject? existingVm = config["vm"] as JsonObject;
        JsonObject vm = existingVm ?? new JsonObject();
        vm["defaultRuntime"] = name;
        if (existingVm is null)
            config["vm"] = vm;
        await WriteBootConfigAsync(config, token);
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

        JsonObject config = await ReadBootConfigAsync(token);
        JsonObject? existingLanguages = config["languages"] as JsonObject;
        JsonObject languages = existingLanguages ?? new JsonObject();
        var entry = new JsonObject { ["rom"] = EnsureAbsolutePath(rom) };
        if (!string.IsNullOrWhiteSpace(extensionRom))
            entry["extensionRom"] = EnsureAbsolutePath(extensionRom);
        languages[name] = entry;
        if (existingLanguages is null)
            config["languages"] = languages;

        if (makeActive)
        {
            JsonObject? existingVm = config["vm"] as JsonObject;
            JsonObject vm = existingVm ?? new JsonObject();
            vm["defaultRuntime"] = name;
            if (existingVm is null)
                config["vm"] = vm;
        }

        await WriteBootConfigAsync(config, token);
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

        JsonObject? languages = config["languages"] as JsonObject;
        languages?.Remove(name);
        await WriteBootConfigAsync(config, token);
        InvalidateStatusCache();
        await SendJsonAsync(context.Response, new JsonObject { ["ok"] = true, ["removedRuntime"] = name }, token);
    }

    private async Task<JsonObject> ReadBootConfigAsync(CancellationToken token)
    {
        JsonNode? config = await GetBoardJsonAsync("/sd/config/boot.json", token);
        if (config is JsonObject obj && obj["error"] is null)
            return obj.DeepClone().AsObject();

        return JsonNode.Parse("""
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
    }

    private async Task WriteBootConfigAsync(JsonObject config, CancellationToken token)
    {
        string json = config.ToJsonString(new JsonSerializerOptions { WriteIndented = true });
        using var content = new StringContent(json, Encoding.UTF8, "application/json");
        using HttpResponseMessage response = await _commandHttp.PutAsync(BoardUrl("/sd/config/boot.json"), content, token);
        string body = await response.Content.ReadAsStringAsync(token);
        if (!response.IsSuccessStatusCode)
            throw new InvalidOperationException(string.IsNullOrWhiteSpace(body)
                ? $"boot config write failed: {(int)response.StatusCode} {response.ReasonPhrase}"
                : body);
    }

    private async Task<JsonNode?> GetBoardJsonAsync(string path, CancellationToken token)
    {
        try
        {
            using HttpResponseMessage response = await _statusHttp.GetAsync(BoardUrl(path), token);
            string body = await response.Content.ReadAsStringAsync(token);
            if (!response.IsSuccessStatusCode)
                return ErrorNode($"GET {path}: {(int)response.StatusCode} {response.ReasonPhrase}");
            if (string.IsNullOrWhiteSpace(body))
                return new JsonObject();
            return JsonNode.Parse(body);
        }
        catch (Exception ex)
        {
            return ErrorNode(ex.Message);
        }
    }

    private async Task ProxyUploadAsync(HttpListenerContext context, string boardPath, CancellationToken token)
    {
        using var content = new StreamContent(context.Request.InputStream);
        content.Headers.ContentType = new System.Net.Http.Headers.MediaTypeHeaderValue("application/octet-stream");
        using HttpResponseMessage response = await _uploadHttp.PutAsync(BoardUrl(boardPath), content, token);
        if (response.IsSuccessStatusCode)
        {
            InvalidateStatusCache();
            InvalidateInventoryCache();
        }
        await SendBoardResponseAsync(context.Response, response, token);
    }

    private async Task ProxyBoardAsync(HttpListenerContext context, HttpMethod method, string boardPath,
                                       byte[]? body, CancellationToken token)
    {
        using var request = new HttpRequestMessage(method, BoardUrl(boardPath));
        if (body is not null)
        {
            request.Content = new ByteArrayContent(body);
            if (body.Length > 0)
                request.Content.Headers.ContentType = new System.Net.Http.Headers.MediaTypeHeaderValue("application/json");
        }

        using HttpResponseMessage response = await _commandHttp.SendAsync(request, token);
        if (response.IsSuccessStatusCode)
        {
            InvalidateStatusCache();
            if (method == HttpMethod.Delete || boardPath.StartsWith("/sd/", StringComparison.Ordinal))
                InvalidateInventoryCache();
        }
        await SendBoardResponseAsync(context.Response, response, token);
    }

    private static async Task SendBoardResponseAsync(HttpListenerResponse target, HttpResponseMessage source,
                                                     CancellationToken token)
    {
        string text = await source.Content.ReadAsStringAsync(token);
        target.StatusCode = (int)source.StatusCode;
        target.ContentType = source.Content.Headers.ContentType?.ToString() ?? "application/json";
        await WriteTextAsync(target, text, token);
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

    private string BoardUrl(string path)
    {
        string normalized = path.StartsWith('/') ? path : "/" + path;
        return BoardBase + normalized;
    }

    private static string SdUrl(string path) => "/sd/" + CleanRelativePath(path);

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

    private static JsonObject ErrorNode(string message) => new()
    {
        ["ok"] = false,
        ["error"] = message
    };

    private static bool IsDriveSlot(string slot) =>
        slot is "fd0" or "fd1" or "fd2" or "fd3" or "hd0" or "hd1";

    private static string? Query(HttpListenerRequest request, string name) =>
        request.QueryString[name];

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
.row{
  display:grid;
  grid-template-columns:1fr auto;
  gap:8px;
  align-items:center;
  padding:8px 0;
  border-bottom:1px solid var(--line);
}
.row:last-child{border-bottom:0}
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
      <section class="panel">
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
          <div id="diskList"></div>
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
            <input id="runtimeName" placeholder="runtime name">
            <label class="check"><input type="checkbox" id="runtimeMakeActive"> Make active</label>
            <input class="wide" type="file" id="runtimeRom" accept=".bin,.rom">
            <input class="wide" type="file" id="runtimeExt" accept=".bin,.rom">
            <button class="primary wide" id="deployRuntimeBtn">Deploy Runtime</button>
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
  const active = !bad(audio) && !!(audio.playing || audio.active || audio.kind && audio.kind !== "none");
  lamp("stAudio", active ? "ok" : "", active ? (audio.kind || "playing") : "idle");
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
    row.innerHTML = `
      <div>
        <button class="name ${selected === d.path ? "selected" : ""}" data-select="${esc(d.path)}">${esc(d.name)}</button>
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
  const name = $("runtimeName").value.trim();
  const rom = $("runtimeRom").files[0];
  const ext = $("runtimeExt").files[0];
  if (!name) return toast("Runtime name required.", true);
  if (!rom) return toast("ROM file required.", true);
  const romPath = "/roms/" + safeName(rom.name);
  let extPath = "";
  toast("Uploading " + rom.name + "...");
  await uploadTo(romPath, rom);
  if (ext){
    extPath = "/roms/" + safeName(ext.name);
    toast("Uploading " + ext.name + "...");
    await uploadTo(extPath, ext);
  }
  await api("/api/runtime/add", {
    method:"POST",
    headers:{"Content-Type":"application/json"},
    body:JSON.stringify({name,rom:romPath,extensionRom:extPath || null,makeActive:$("runtimeMakeActive").checked})
  });
  toast("Runtime deployed: " + name);
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
setInterval(() => refresh(false).catch(() => {}), 15000);
</script>
</body>
</html>
""";
}
