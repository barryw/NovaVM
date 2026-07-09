using System;

namespace e6502UnitTests;

/// <summary>
/// Sets a process environment variable for the lifetime of a <c>using</c> scope and
/// restores its previous value on <see cref="Dispose"/>.
///
/// Tests configure <c>CompositeBusDevice</c> through process-global env vars
/// (NOVA_NO_AUTOMOUNT, NOVA_STORAGE_ROOT, NOAUTO, ...). A test that sets one of these
/// and forgets to restore it silently corrupts EVERY later test in the same process —
/// e.g. leaking NOVA_NO_AUTOMOUNT=1 stops the Forth/NDK tests from automounting their
/// language disk and they fail with "INCLUDE FAILED". Always scope such changes with
/// this helper instead of a bare Environment.SetEnvironmentVariable:
///
///     using (new EnvScope("NOVA_NO_AUTOMOUNT", "1"))
///         bus = new CompositeBusDevice(...);   // env restored when the scope exits
///
/// The AssemblySetup cleanup guard fails the whole run if this discipline slips, so a
/// leak can never silently poison the suite again.
/// </summary>
public sealed class EnvScope : IDisposable
{
    private readonly string _name;
    private readonly string? _previous;

    public EnvScope(string name, string? value)
    {
        _name = name;
        _previous = Environment.GetEnvironmentVariable(name);
        Environment.SetEnvironmentVariable(name, value);
    }

    public void Dispose() => Environment.SetEnvironmentVariable(_name, _previous);
}
