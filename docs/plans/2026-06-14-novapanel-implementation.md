# NovaPanel Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build `e6502.NovaPanel`, a standalone Avalonia app that remote-controls Nova hardware with full web-admin parity plus an on-screen keyboard (click or type-with-highlight).

**Architecture:** Extract the existing NovaHost management client into a new shared library `e6502.NovaHost`; add a newline-JSON debug client there too. The Avalonia app (code-behind, no MVVM framework, matching `e6502.Avalonia`) consumes both. Testable logic (key dispatch, responsive breakpoints, status parsing, wire framing) lives in plain classes covered by an xUnit + Avalonia.Headless test project. UI is assembled as UserControls.

**Tech Stack:** .NET 10, Avalonia 12.0.2 (Fluent theme), xUnit + Avalonia.Headless.XUnit for tests, System.Text.Json, TcpClient.

**Design reference:** `docs/plans/2026-06-14-novapanel-design.md`

**Conventions:**
- Work in worktree `.worktrees/novapanel`, branch `feature/novapanel`.
- TDD: write the failing test, watch it fail, implement, watch it pass, commit.
- Commit after every green task. Conventional Commits. Co-author trailer:
  `Co-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>`
- Build/test from the worktree root.
- Verified firmware key vocabulary (authoritative — do not add keys outside this set):
  `ENTER BACKSPACE TAB ESC SPACE LEFT RIGHT UP DOWN HOME END PGUP PGDN CTRL-HOME CTRL-END DELETE CTRL-C SCREEN-HOME`, plus `ALT-<letter>`, `CTRL-<letter>`, any printable ASCII 0x20–0x7E. **No function keys.**

---

## Phase 0 — Shared library `e6502.NovaHost`

### Task 0.1: Create the library project

**Files:**
- Create: `e6502.NovaHost/e6502.NovaHost.csproj`

**Step 1: Create the csproj**

```xml
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <TargetFramework>net10.0</TargetFramework>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
    <RootNamespace>e6502.NovaHost</RootNamespace>
  </PropertyGroup>

  <ItemGroup>
    <ProjectReference Include="..\e6502.Storage\e6502.Storage.csproj" />
  </ItemGroup>

</Project>
```

**Step 2: Add project to solution**

Run: `dotnet sln e6502.sln add e6502.NovaHost/e6502.NovaHost.csproj`
Expected: "Project ... added to the solution."

**Step 3: Commit**

```bash
git add e6502.NovaHost/e6502.NovaHost.csproj e6502.sln
git commit -m "build(novahost): scaffold e6502.NovaHost shared library"
```

### Task 0.2: Move the management client into the library (namespaced + public)

**Files:**
- Move: `e6502.Nova/NovaHostManagementClient.cs` → `e6502.NovaHost/NovaHostManagementClient.cs`
- Modify: `e6502.Nova/e6502.Nova.csproj` (add ProjectReference to e6502.NovaHost)
- Modify: callers in `e6502.Nova` that reference the moved types (add `using e6502.NovaHost;`)

**Step 1: Move the file with git**

Run: `git mv e6502.Nova/NovaHostManagementClient.cs e6502.NovaHost/NovaHostManagementClient.cs`

**Step 2: Namespace + visibility**

Wrap the file's types in `namespace e6502.NovaHost;` (file-scoped, after the `using`s). Change:
- `sealed class NovaHostManagementClient` → `public sealed class NovaHostManagementClient`
- The records/exception that callers consume → `public`:
  `NovaHostCommandException`, `NovaTransferProgress`, `NovaFileChunk`
- Keep `static class CborLite` and the private `enum Command` as-is (internal/private — impl detail).

**Step 3: Reference the library from e6502.Nova**

In `e6502.Nova/e6502.Nova.csproj`, add inside the existing ProjectReference ItemGroup:

```xml
<ProjectReference Include="..\e6502.NovaHost\e6502.NovaHost.csproj" />
```

**Step 4: Add usings to callers**

Run: `grep -rl "NovaHostManagementClient\|NovaFileChunk\|NovaTransferProgress\|NovaHostCommandException" e6502.Nova` — for each file returned, add `using e6502.NovaHost;` at the top (typically `Program.cs`, `NovaWebServer.cs`).

**Step 5: Build the CLI to verify extraction**

Run: `dotnet build e6502.Nova/e6502.Nova.csproj -c Debug`
Expected: `Build succeeded. 0 Error(s)`. If a type is reported inaccessible, make it `public` in the moved file and rebuild. (`CborLite` should NOT need to be public; if it does, a caller is using it directly — make it `public` and note it.)

**Step 6: Commit**

```bash
git add -A
git commit -m "refactor(novahost): extract management client into shared library"
```

### Task 0.3: Add the debug (keyboard) client — framing test first

**Files:**
- Create: `e6502.NovaHost/NovaDebugClient.cs`
- Create test project (next task) — but write this implementation to satisfy the test in Task 0.4.

Defer implementation until the test exists. Proceed to 0.4.

### Task 0.4: Test project + NovaDebugClient framing (TDD)

**Files:**
- Create: `e6502.NovaPanel.Tests/e6502.NovaPanel.Tests.csproj`
- Create: `e6502.NovaPanel.Tests/FakeTcpServer.cs`
- Create: `e6502.NovaPanel.Tests/NovaDebugClientTests.cs`
- Create: `e6502.NovaHost/NovaDebugClient.cs`

**Step 1: Create the test project**

```xml
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <TargetFramework>net10.0</TargetFramework>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
    <IsPackable>false</IsPackable>
  </PropertyGroup>

  <ItemGroup>
    <PackageReference Include="Microsoft.NET.Test.Sdk" Version="17.11.1" />
    <PackageReference Include="xunit" Version="2.9.2" />
    <PackageReference Include="xunit.runner.visualstudio" Version="2.8.2" />
    <PackageReference Include="Avalonia.Headless.XUnit" Version="12.0.2" />
  </ItemGroup>

  <ItemGroup>
    <ProjectReference Include="..\e6502.NovaHost\e6502.NovaHost.csproj" />
    <ProjectReference Include="..\e6502.NovaPanel\e6502.NovaPanel.csproj" />
  </ItemGroup>

</Project>
```

Note: the `e6502.NovaPanel` reference will not resolve until Phase 1. To keep this task self-contained, TEMPORARILY omit the `e6502.NovaPanel` ProjectReference line; add it back in Task 1.x. Add project to sln:

Run: `dotnet sln e6502.sln add e6502.NovaPanel.Tests/e6502.NovaPanel.Tests.csproj`

**Step 2: Write FakeTcpServer helper**

```csharp
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace e6502.NovaPanel.Tests;

/// Minimal loopback server: accepts one client, records each received line,
/// and replies with a fixed line per request.
public sealed class FakeTcpServer : IAsyncDisposable
{
    private readonly TcpListener _listener;
    private readonly string _reply;
    public List<string> Received { get; } = new();
    public int Port => ((IPEndPoint)_listener.LocalEndpoint).Port;

    public FakeTcpServer(string reply = "{\"ok\":true}")
    {
        _reply = reply;
        _listener = new TcpListener(IPAddress.Loopback, 0);
        _listener.Start();
        _ = AcceptLoop();
    }

    private async Task AcceptLoop()
    {
        try
        {
            using TcpClient client = await _listener.AcceptTcpClientAsync();
            using NetworkStream s = client.GetStream();
            using var reader = new StreamReader(s, new UTF8Encoding(false));
            using var writer = new StreamWriter(s, new UTF8Encoding(false)) { AutoFlush = true, NewLine = "\n" };
            string? line;
            while ((line = await reader.ReadLineAsync()) is not null)
            {
                Received.Add(line);
                await writer.WriteLineAsync(_reply);
            }
        }
        catch { /* server torn down */ }
    }

    public ValueTask DisposeAsync()
    {
        _listener.Stop();
        return ValueTask.CompletedTask;
    }
}
```

**Step 3: Write the failing tests**

```csharp
using e6502.NovaHost;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class NovaDebugClientTests
{
    [Fact]
    public async Task SendKey_writes_exact_send_key_frame()
    {
        await using var server = new FakeTcpServer();
        using var client = new NovaDebugClient("127.0.0.1", server.Port);

        await client.SendKeyAsync("ENTER");

        Assert.Single(server.Received);
        Assert.Equal("{\"command\":\"send_key\",\"key\":\"ENTER\"}", server.Received[0]);
    }

    [Fact]
    public async Task TypeText_writes_exact_type_text_frame()
    {
        await using var server = new FakeTcpServer();
        using var client = new NovaDebugClient("127.0.0.1", server.Port);

        await client.TypeTextAsync("hi");

        Assert.Equal("{\"command\":\"type_text\",\"text\":\"hi\"}", server.Received[0]);
    }
}
```

**Step 4: Run — expect failure**

Run: `dotnet test e6502.NovaPanel.Tests/e6502.NovaPanel.Tests.csproj`
Expected: FAIL — `NovaDebugClient` does not exist.

**Step 5: Implement NovaDebugClient** (mirrors `e6502.MCP/EmulatorClient.cs`)

```csharp
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace e6502.NovaHost;

/// Newline-delimited JSON client for the NovaHost debug port (6503 remote, 6502 local).
/// Used for keyboard input: type_text and send_key.
public sealed class NovaDebugClient : IDisposable
{
    private static readonly UTF8Encoding Utf8NoBom = new(false);
    private readonly string _host;
    private readonly int _port;
    private readonly SemaphoreSlim _sem = new(1, 1);
    private TcpClient? _tcp;
    private StreamReader? _reader;
    private StreamWriter? _writer;

    public NovaDebugClient(string host, int port = 6503)
    {
        _host = host;
        _port = port;
    }

    public bool IsConnected => _tcp is { Connected: true };

    private async Task EnsureConnectedAsync(CancellationToken ct)
    {
        if (_tcp is { Connected: true }) return;
        _tcp?.Dispose();
        _tcp = new TcpClient();
        await _tcp.ConnectAsync(_host, _port, ct);
        NetworkStream stream = _tcp.GetStream();
        _reader = new StreamReader(stream, Utf8NoBom, false);
        _writer = new StreamWriter(stream, Utf8NoBom) { AutoFlush = true, NewLine = "\n" };
    }

    public Task SendKeyAsync(string key, CancellationToken ct = default) =>
        SendAsync(new JsonObject { ["command"] = "send_key", ["key"] = key }, ct);

    public Task TypeTextAsync(string text, CancellationToken ct = default) =>
        SendAsync(new JsonObject { ["command"] = "type_text", ["text"] = text }, ct);

    private async Task SendAsync(JsonObject request, CancellationToken ct)
    {
        await _sem.WaitAsync(ct);
        try
        {
            await EnsureConnectedAsync(ct);
            await _writer!.WriteLineAsync(request.ToJsonString(JsonOpts));
            await _reader!.ReadLineAsync(ct); // drain the {"ok":true} reply
        }
        finally
        {
            _sem.Release();
        }
    }

    private static readonly JsonSerializerOptions JsonOpts = new();

    public void Dispose()
    {
        _tcp?.Dispose();
        _sem.Dispose();
    }
}
```

Note: `JsonObject.ToJsonString()` emits compact JSON with the keys in insertion order, producing exactly `{"command":"send_key","key":"ENTER"}`. If the assertion fails on key escaping, inspect the actual string and adjust the test to the real output (do not loosen the wire format).

**Step 6: Run — expect pass**

Run: `dotnet test e6502.NovaPanel.Tests/e6502.NovaPanel.Tests.csproj`
Expected: PASS (2 tests).

**Step 7: Commit**

```bash
git add -A
git commit -m "feat(novahost): add NovaDebugClient with framing tests"
```

### Task 0.5: Full solution build gate

**Step 1:** Run: `dotnet build e6502.sln -c Debug`
Expected: `0 Error(s)` (warnings unchanged from baseline).

**Step 2:** Commit only if changes were needed (e.g. a caller that needed a `using`).

---

## Phase 1 — Avalonia app scaffold `e6502.NovaPanel`

### Task 1.1: Create the Avalonia project

**Files:**
- Create: `e6502.NovaPanel/e6502.NovaPanel.csproj`
- Create: `e6502.NovaPanel/Program.cs`
- Create: `e6502.NovaPanel/App.axaml` + `App.axaml.cs`
- Create: `e6502.NovaPanel/MainWindow.axaml` + `MainWindow.axaml.cs`
- Create: `e6502.NovaPanel/app.manifest` (optional, copy pattern from e6502.Avalonia if present)

**Step 1: csproj** (match e6502.Avalonia versions; no AOT for v1 to keep iteration fast)

```xml
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>WinExe</OutputType>
    <TargetFramework>net10.0</TargetFramework>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
    <BuiltInComInteropSupport>true</BuiltInComInteropSupport>
    <AvaloniaUseCompiledBindingsByDefault>true</AvaloniaUseCompiledBindingsByDefault>
    <ApplicationManifest>app.manifest</ApplicationManifest>
  </PropertyGroup>

  <ItemGroup>
    <PackageReference Include="Avalonia" Version="12.0.2" />
    <PackageReference Include="Avalonia.Desktop" Version="12.0.2" />
    <PackageReference Include="Avalonia.Themes.Fluent" Version="12.0.2" />
  </ItemGroup>

  <ItemGroup>
    <ProjectReference Include="..\e6502.NovaHost\e6502.NovaHost.csproj" />
    <ProjectReference Include="..\e6502.Storage\e6502.Storage.csproj" />
  </ItemGroup>

</Project>
```

If `e6502.Avalonia/app.manifest` exists, copy it; otherwise drop the `<ApplicationManifest>` line.

**Step 2: Program.cs**

```csharp
using Avalonia;

namespace e6502.NovaPanel;

internal static class Program
{
    [STAThread]
    public static void Main(string[] args) => BuildAvaloniaApp()
        .StartWithClassicDesktopLifetime(args);

    public static AppBuilder BuildAvaloniaApp() => AppBuilder.Configure<App>()
        .UsePlatformDetect()
        .LogToTrace();
}
```

**Step 3: App.axaml**

```xml
<Application xmlns="https://github.com/avaloniaui"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
             x:Class="e6502.NovaPanel.App"
             RequestedThemeVariant="Dark">
  <Application.Styles>
    <FluentTheme />
  </Application.Styles>
</Application>
```

**Step 4: App.axaml.cs**

```csharp
using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;

namespace e6502.NovaPanel;

public partial class App : Application
{
    public override void Initialize() => AvaloniaXamlLoader.Load(this);

    public override void OnFrameworkInitializationCompleted()
    {
        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            desktop.MainWindow = new MainWindow();
        base.OnFrameworkInitializationCompleted();
    }
}
```

**Step 5: MainWindow.axaml** (placeholder; real layout in Phase 7)

```xml
<Window xmlns="https://github.com/avaloniaui"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        x:Class="e6502.NovaPanel.MainWindow"
        Title="NovaPanel" Width="900" Height="360"
        Background="#101014">
  <TextBlock Text="NovaPanel" Foreground="White"
             HorizontalAlignment="Center" VerticalAlignment="Center"/>
</Window>
```

**Step 6: MainWindow.axaml.cs**

```csharp
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace e6502.NovaPanel;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void InitializeComponent() => AvaloniaXamlLoader.Load(this);
}
```

**Step 7: Add to sln + build**

Run:
```bash
dotnet sln e6502.sln add e6502.NovaPanel/e6502.NovaPanel.csproj
dotnet build e6502.NovaPanel/e6502.NovaPanel.csproj -c Debug
```
Expected: `Build succeeded`.

**Step 8: Re-link the test project** — add back the `e6502.NovaPanel` ProjectReference removed in Task 0.4, then `dotnet build e6502.NovaPanel.Tests`.

**Step 9: Commit**

```bash
git add -A
git commit -m "feat(novapanel): scaffold Avalonia app shell"
```

### Task 1.2: Headless smoke test (app boots)

**Files:**
- Create: `e6502.NovaPanel.Tests/AppHeadlessSetup.cs`
- Create: `e6502.NovaPanel.Tests/SmokeTests.cs`

**Step 1: Avalonia headless test bootstrap**

```csharp
using Avalonia;
using Avalonia.Headless;
using e6502.NovaPanel;

[assembly: AvaloniaTestApplication(typeof(e6502.NovaPanel.Tests.TestAppBuilder))]

namespace e6502.NovaPanel.Tests;

public static class TestAppBuilder
{
    public static AppBuilder BuildAvaloniaApp() =>
        AppBuilder.Configure<App>().UseHeadless(new AvaloniaHeadlessPlatformOptions());
}
```

**Step 2: Failing smoke test**

```csharp
using Avalonia.Headless.XUnit;
using Avalonia.Controls;
using e6502.NovaPanel;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class SmokeTests
{
    [AvaloniaFact]
    public void Window_constructs()
    {
        var w = new MainWindow();
        w.Show();
        Assert.Equal("NovaPanel", w.Title);
    }
}
```

**Step 3: Run** — `dotnet test e6502.NovaPanel.Tests` — expect PASS.

**Step 4: Commit**

```bash
git add -A
git commit -m "test(novapanel): headless app boot smoke test"
```

---

## Phase 2 — Key dispatch (pure logic, TDD)

### Task 2.1: KeyToken resolver

**Files:**
- Create: `e6502.NovaPanel/Input/KeyDispatch.cs`
- Create: `e6502.NovaPanel.Tests/KeyDispatchTests.cs`

A pure static mapper from Avalonia `Key` + `KeyModifiers` to either a protocol token string (for `send_key`) or null (dead key). Shift resolves the printable ASCII locally.

**Step 1: Failing tests**

```csharp
using Avalonia.Input;
using e6502.NovaPanel.Input;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class KeyDispatchTests
{
    [Theory]
    [InlineData(Key.Enter, KeyModifiers.None, "ENTER")]
    [InlineData(Key.Back, KeyModifiers.None, "BACKSPACE")]
    [InlineData(Key.Tab, KeyModifiers.None, "TAB")]
    [InlineData(Key.Escape, KeyModifiers.None, "ESC")]
    [InlineData(Key.Space, KeyModifiers.None, "SPACE")]
    [InlineData(Key.Left, KeyModifiers.None, "LEFT")]
    [InlineData(Key.Up, KeyModifiers.None, "UP")]
    [InlineData(Key.PageUp, KeyModifiers.None, "PGUP")]
    [InlineData(Key.Delete, KeyModifiers.None, "DELETE")]
    [InlineData(Key.Home, KeyModifiers.Control, "CTRL-HOME")]
    [InlineData(Key.End, KeyModifiers.Control, "CTRL-END")]
    [InlineData(Key.A, KeyModifiers.None, "a")]
    [InlineData(Key.A, KeyModifiers.Shift, "A")]
    [InlineData(Key.D1, KeyModifiers.None, "1")]
    [InlineData(Key.D1, KeyModifiers.Shift, "!")]
    [InlineData(Key.A, KeyModifiers.Control, "CTRL-A")]
    [InlineData(Key.A, KeyModifiers.Alt, "ALT-A")]
    [InlineData(Key.C, KeyModifiers.Control, "CTRL-C")]
    public void Resolves_expected_token(Key key, KeyModifiers mods, string expected)
        => Assert.Equal(expected, KeyDispatch.Resolve(key, mods));

    [Theory]
    [InlineData(Key.F1)]
    [InlineData(Key.F12)]
    [InlineData(Key.LeftShift)]
    [InlineData(Key.Insert)]
    public void Dead_keys_return_null(Key key)
        => Assert.Null(KeyDispatch.Resolve(key, KeyModifiers.None));
}
```

**Step 2: Run — expect FAIL** (`KeyDispatch` missing).

**Step 3: Implement** `KeyDispatch.Resolve`:
- Named-key table (Enter/Back/Tab/Escape/Space/arrows/Home/End/PageUp/PageDown/Delete) → tokens.
- Ctrl+Home → `CTRL-HOME`, Ctrl+End → `CTRL-END`.
- Letters A–Z: base lowercase char; if Control → `CTRL-<UPPER>`; if Alt → `ALT-<UPPER>`; else Shift→uppercase, no-shift→lowercase.
- Digits/punctuation via two maps (unshifted, shifted) covering the US-ANSI rows in the design.
- Everything else (F-keys, modifiers themselves, Insert, etc.) → null.

Implementer note: Control/Alt take precedence over Shift for letters. Punctuation with Control/Alt is undefined by the protocol → return null (don't emit garbage).

**Step 4: Run — expect PASS.**

**Step 5: Commit**

```bash
git add -A
git commit -m "feat(novapanel): key dispatch resolver with full coverage"
```

### Task 2.2: Layout model (rows of keys)

**Files:**
- Create: `e6502.NovaPanel/Input/KeyboardLayout.cs`
- Create: `e6502.NovaPanel.Tests/KeyboardLayoutTests.cs`

A data-only description: list of rows, each a list of `KeyCap { Label, ShiftLabel?, Key, WidthUnits, IsModifier }`. No keycap maps to a dead protocol token.

**Step 1: Failing test** — assert every non-modifier KeyCap resolves to non-null via `KeyDispatch.Resolve(cap.Key, None)` (proves no dead keys in the layout), and that the layout contains Enter, Space, and arrow keys.

**Step 2–4:** Run (fail) → implement the ANSI layout from the design (F-row omitted) → run (pass).

**Step 5: Commit** `feat(novapanel): keyboard layout model`.

---

## Phase 3 — Keyboard sender service

### Task 3.1: KeySender (queue + debug client)

**Files:**
- Create: `e6502.NovaPanel/Services/KeySender.cs`
- Create: `e6502.NovaPanel.Tests/KeySenderTests.cs`

Wraps a `NovaDebugClient`. Exposes `Send(string token)` that enqueues and drains on a background task so UI thread never blocks; preserves order. For tests, inject an interface `IKeySink { Task SendKeyAsync(string); Task TypeTextAsync(string); }` implemented by a thin adapter over `NovaDebugClient`, and a fake in tests.

**Step 1: Failing test** — feed `Send("a")`, `Send("ENTER")`; assert the fake sink received `["a","ENTER"]` in order after a flush/await.

**Step 2–4:** Run (fail) → implement (Channel<string> + single consumer) → run (pass).

**Step 5: Commit** `feat(novapanel): ordered key sender service`.

---

## Phase 4 — KeyboardView (headless UI tests)

### Task 4.1: KeyboardView control + click dispatch

**Files:**
- Create: `e6502.NovaPanel/Views/KeyboardView.axaml` + `.axaml.cs`
- Create: `e6502.NovaPanel.Tests/KeyboardViewTests.cs`

`KeyboardView` builds a `Grid` of buttons from `KeyboardLayout`. Exposes an event `KeyActivated(string token)` (or a callback delegate) raised on click and on physical key. The view holds modifier latch state (Shift/Ctrl/Alt toggle buttons).

**Step 1: Failing headless test** — construct view, find the button whose cap.Key == Key.Enter, invoke its command/click, assert `KeyActivated` fired with `"ENTER"`.

```csharp
[AvaloniaFact]
public void Clicking_enter_raises_ENTER()
{
    var view = new KeyboardView();
    string? got = null;
    view.KeyActivated += t => got = t;
    view.ActivateForTest(Avalonia.Input.Key.Enter); // helper that simulates a keycap click
    Assert.Equal("ENTER", got);
}
```

**Step 2–4:** Run (fail) → implement view + `ActivateForTest` → run (pass).

**Step 5: Commit** `feat(novapanel): keyboard view with click dispatch`.

### Task 4.2: Physical key capture + highlight

**Files:**
- Modify: `e6502.NovaPanel/Views/KeyboardView.axaml.cs`
- Modify: `e6502.NovaPanel.Tests/KeyboardViewTests.cs`

Handle `OnKeyDown`/`OnKeyUp`: resolve via `KeyDispatch`, raise `KeyActivated`, set a `.active` pseudo-class on the matching keycap button (clear on key up / after a short timer). Mark `e.Handled = true` for resolved keys.

**Step 1: Failing headless test** — use Avalonia headless key injection on a window hosting the view; assert `KeyActivated` fires with the right token and the matching button has the active pseudo-class.

```csharp
[AvaloniaFact]
public void Physical_A_with_shift_raises_uppercase_and_highlights()
{
    var window = new Window { Content = new KeyboardView() };
    window.Show();
    var view = (KeyboardView)window.Content!;
    string? got = null;
    view.KeyActivated += t => got = t;
    window.KeyPressQwerty(PhysicalKey.A, RawInputModifiers.Shift); // Avalonia.Headless helper
    Assert.Equal("A", got);
    Assert.True(view.IsKeyActive(Key.A));
}
```

(If `KeyPressQwerty` signature differs in 12.0.2, use the available `window.KeyPress(...)` headless extension; confirm the exact API at implementation time.)

**Step 2–4:** Run (fail) → implement capture + highlight + `IsKeyActive` test helper → run (pass).

**Step 5: Commit** `feat(novapanel): physical key capture and highlight`.

---

## Phase 5 — Responsive layout (pure logic, TDD)

### Task 5.1: SizeClass + ResponsiveController

**Files:**
- Create: `e6502.NovaPanel/Layout/SizeClass.cs` (enum: Compact, Wide, Full)
- Create: `e6502.NovaPanel/Layout/ResponsiveController.cs`
- Create: `e6502.NovaPanel.Tests/ResponsiveControllerTests.cs`

`ResponsiveController.Classify(double width, double height)` → SizeClass. Thresholds as named constants (e.g. `WideMinWidth=1180`, `FullMinHeight=620`). Wide requires width ≥ WideMinWidth; Full requires Wide AND height ≥ FullMinHeight.

**Step 1: Failing tests** at each boundary (just below / at / above each threshold).

**Step 2–4:** Run (fail) → implement → run (pass).

**Step 5: Commit** `feat(novapanel): responsive size-class controller`.

---

## Phase 6 — Connection + status service

### Task 6.1: Status model parsing (TDD)

**Files:**
- Create: `e6502.NovaPanel/Services/NovaStatus.cs` (POCOs: drives, runtimes, wifi, sd, health, audio)
- Create: `e6502.NovaPanel/Services/NovaStatusParser.cs`
- Create: `e6502.NovaPanel.Tests/NovaStatusParserTests.cs`
- Create: `e6502.NovaPanel.Tests/Fixtures/status-sample.json`

Parse the `GetStatus` `JsonObject` shape (see design doc field list) into typed POCOs the panels bind to. Drives, `bootConfig.languages`→runtimes (+`defaultRuntime`→active), wifi, sdStatus, health.

**Step 1:** Save a representative `status-sample.json` (from the design doc's documented shape).

**Step 2: Failing tests** — parse the fixture, assert: 6 drives parsed, fd0 mounted+path, active runtime name, wifi ssid, sd cardType, bootPhase.

**Step 3–4:** Run (fail) → implement parser (defensive: missing fields → defaults, never throw) → run (pass).

**Step 5: Commit** `feat(novapanel): typed status parser`.

### Task 6.2: NovaConnection service (poll loop + state)

**Files:**
- Create: `e6502.NovaPanel/Services/NovaConnection.cs`
- Create: `e6502.NovaPanel.Tests/NovaConnectionTests.cs`

Owns a `NovaHostManagementClient` (6504) + `NovaDebugClient`/`KeySender` (6503). Exposes:
- `ConnectionState` enum (Disconnected/Connecting/Connected/Error) + `event Action StateChanged`.
- `event Action<NovaStatus> StatusUpdated`.
- `Start(host)` / `Stop()`; internal poll on a timer (~2s) calling `GetStatusAsync`, parsing, raising events; on exception → Error state + backoff reconnect (1→5s).
- Pass-through action methods: Mount/Unmount/RuntimeSet/Add/Remove/VmReset/HostReboot/AudioStop/Delete/ListDirectory/WriteFile, each marshalling errors into a returned `(bool ok, string? error)`.

For testability, keep the poll tick callable directly (`Task PollOnceAsync()`); the timer just calls it. Don't unit-test the live socket here (covered by integration), but unit-test the state machine: inject a delegate `Func<Task<JsonObject>>` status source; assert state transitions Disconnected→Connecting→Connected on success and →Error on throw.

**Step 1: Failing tests** for the state machine with a fake status source.

**Step 2–4:** Run (fail) → implement → run (pass).

**Step 5: Commit** `feat(novapanel): connection service with poll loop and state machine`.

---

## Phase 7 — Panels (UserControls)

Each panel is a UserControl bound to `NovaStatus` / connection. These are largely XAML + code-behind; test the non-trivial formatting logic where present, smoke-test rendering otherwise. One task per panel; each ends with a headless construct-and-bind smoke test + commit.

### Task 7.1: ConnBar
Host textbox, Connect/Disconnect, status chips (bootPhase, SD, FPGA bridge, WiFi, audio), VM Reset, Host Reboot (confirm dialog). Wire to `NovaConnection`. Smoke: construct, set a `NovaStatus`, assert chip text reflects bootPhase. Commit.

### Task 7.2: StatusPanel
Expanded lamps from health/sd/wifi/audio. Smoke + commit.

### Task 7.3: DrivesPanel
6 rows from `status.Drives`; Mount (disk picker)/Recall/Unmount call `NovaConnection`. Smoke: bind drives, assert 6 rows + mounted badge. Commit.

### Task 7.4: DiskImagesPanel
`ListDirectory("disks/floppy")`+`disks/hard`; rows name/size/bootStatus; Mount→slot picker; Delete (confirm); Upload `.ndi` via `WriteFileAsync` + progress bar (run off UI thread). Smoke + commit.

### Task 7.5: RuntimePanel
List from parsed runtimes; active highlighted; Set Active/Add/Remove (Remove disabled when active). Smoke + commit.

### Task 7.6: LibraryPanel
Kind selector (roms/soundfonts/music/boot/sid) → ListDirectory; rows name/size; Delete; Upload. Smoke + commit.

### Task 7.7: ModulesPanel
`ListDirectory("lib")` → `NovaModule.Parse` (e6502.Storage) → rows name/id/version/fnCount/staged/idMismatch; Functions button → searchable modal; toggle staged; Delete. Test: parse a known `.nmod` fixture → assert fields. Commit.

---

## Phase 8 — Assemble MainWindow + responsive wiring

### Task 8.1: Compose layout

**Files:**
- Modify: `e6502.NovaPanel/MainWindow.axaml` + `.axaml.cs`

Outer `Grid`: left rail column (status/drives/runtime), center (ConnBar + KeyboardView), bottom row (DiskImages/Library/Modules). Subscribe to `Bounds` changes → `ResponsiveController.Classify` → set rail column width and bottom row height to 0/Auto and toggle panel `IsVisible`. Compact: panels hidden, reachable via `≡` chips → overlay (`Popup`/`Flyout`).

**Step 1:** Compute the KeyboardView desired size after first layout; set `Window.MinWidth`/`MinHeight` so the window can never shrink below keyboard + ConnBar. Set initial `Width`/`Height` to the Compact size.

**Step 2: Headless test** — show window at a small size → assert rail not visible (Compact); resize wide → assert rail visible (Wide); resize tall+wide → assert bottom row visible (Full).

**Step 3:** Run → pass.

**Step 4: Commit** `feat(novapanel): responsive main window layout`.

### Task 8.2: Min-size == keyboard guard test

**Step 1: Headless test** — assert `window.MinWidth >= keyboardView.Bounds.Width` after layout and that setting Width below MinWidth clamps. Commit.

---

## Phase 9 — Settings persistence

### Task 9.1: Settings load/save (TDD)

**Files:**
- Create: `e6502.NovaPanel/Services/PanelSettings.cs`
- Create: `e6502.NovaPanel.Tests/PanelSettingsTests.cs`

JSON at `~/.e6502/novapanel.json`: `{ host, x, y, width, height }`. Inject the directory path for tests (write to temp dir, round-trip).

**Step 1: Failing test** — save then load returns same values; missing file → defaults.

**Step 2–4:** Run (fail) → implement → run (pass).

**Step 5:** Wire into MainWindow (load host + bounds on open, save on close). Commit `feat(novapanel): persist host and window bounds`.

---

## Phase 10 — Final gate

### Task 10.1: Full build + test + manual launch

**Step 1:** `dotnet build e6502.sln -c Debug` → 0 errors.
**Step 2:** `dotnet test e6502.NovaPanel.Tests` → all green.
**Step 3:** `dotnet test e6502.sln` → confirm no regression in existing suites (CLI extraction).
**Step 4:** Manual: `dotnet run --project e6502.NovaPanel`, point at the live board host, verify: window opens keyboard-sized, typing highlights + reaches Nova, resize reveals panels, mount/unmount/runtime/status all work next to a QuickTime capture window.
**Step 5:** Update root `README.md`/`CLAUDE.md` solution table with the two new projects. Commit `docs: list NovaPanel + NovaHost projects`.

### Task 10.2: Finish the branch

Use superpowers:finishing-a-development-branch to choose merge/PR/cleanup.

---

## Risk notes for the implementer

- **Avalonia headless API in 12.0.2:** the exact key-injection extension method names (`KeyPressQwerty`, `KeyTextInput`, `KeyPress`) may differ. Confirm against the installed `Avalonia.Headless` assembly before writing Task 4.2; adjust the test, never the production key handling.
- **CborLite visibility:** Task 0.2 keeps it internal. If the e6502.Nova build fails because a caller uses it directly, make it `public` and note it — don't duplicate it.
- **xUnit vs repo MSTest:** the existing suite is MSTest; this new project is xUnit purely because Avalonia.Headless integrates cleanly there. They coexist in the solution.
- **No function keys:** never wire F1–F12 to tokens — the firmware rejects them. The layout-has-no-dead-keys test (Task 2.2) enforces this.
