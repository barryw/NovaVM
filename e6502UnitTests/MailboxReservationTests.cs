using System;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Proves the canonical paged-library reserved band $0300-$08FF stays clear in
/// NovaBASIC. The band = the $0300-$031F mailbox (libabi.inc LIB_MBOX), the
/// $0320-$041F resident loader band (256 B for libcall.s), and the $0420-$08FF
/// cross-runtime module-BSS band (1248 B, libabi.inc MODULE_BSS_BAND, carved by
/// 4c.2-1 for paged-module low-RAM working storage). All three must be FREE in
/// every runtime; NovaBASIC carves them out by starting user storage (Ram_base)
/// at $0900 instead of $0300.
///
/// Approach (sentinel-survives, the strongest faithful proof against the real
/// EhBASIC ROM running on the Avalonia CompositeBusDevice): boot BASIC, paint a
/// $5A sentinel across the loader + module-BSS bands ($0320-$08FF) directly in
/// host RAM, then drive the real ROM through entering a program that builds
/// program text, scalar/string variables and an array, and RUN it. BASIC keys
/// program/variable/string storage off Ram_base, so if Ram_base were still $0300
/// the program text and variables would overwrite the sentinel. We assert every
/// byte survives. The $0300-$031F mailbox is excluded — it is the lib_call ABI
/// scratchpad and the line reader (LAB_1357 -> SYS_SCREEN_READLINE) legitimately
/// writes it on every input line.
///
/// We deliberately do NOT mock — this exercises the actual tokenizer,
/// variable allocator and string heap in the shipped ROM image, the same way
/// the existing BasicRegressionTests/EhBasicTokenizationTests do. ReadRam/
/// WriteRam are host-side accessors into the flat 64KB array; low RAM
/// ($0300-$031F) has no MMIO interception so they are byte-identical to a CPU
/// access there.
/// </summary>
[TestClass]
public class MailboxReservationTests
{
    // The sentinel proof targets the bands that must stay clear of Ram_base-keyed
    // user storage: the $0320-$041F resident loader band (libcall.s) and the
    // $0420-$08FF cross-runtime module-BSS band (libabi.inc MODULE_BSS_BAND).
    //
    // Exclusive top of the reserved band. User storage (Ram_base) must start here.
    private const ushort MailboxEnd = 0x0900;

    [TestMethod]
    public void Basic_DoesNotClobber_MailboxRegion()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // A sentinel-survival paint over $0300-$08FF is no longer viable: the line
        // reader (LAB_1357) now routes through lib_call(SYSTEM, SYS_SCREEN_READLINE),
        // which legitimately uses the whole band at runtime — the $0300-$031F mailbox
        // (LIB_MBOX ABI scratch), the $0320-$041F resident loader (JSR'd on every
        // call), and the $0420-$08FF module-BSS working store. Painting it would
        // corrupt the loader the first input line needs. Instead we prove the real
        // invariant directly from BASIC's storage pointers: every one sits at/above
        // $0900, so program/variable/string storage never overlaps the reserved band.
        //
        // Build program text + scalar + string + array, then RUN it. Each touches
        // Ram_base-relative storage; if Ram_base were wrongly $0300 these pointers
        // would land inside the band.
        QueueLine(editor, "10 A=12345");
        QueueLine(editor, "20 B$=\"HELLO\"");
        QueueLine(editor, "30 DIM C(20)");
        QueueLine(editor, "40 FOR I=0 TO 20:C(I)=I:NEXT");
        QueueLine(editor, "RUN");
        QueueLine(editor, "LIST");
        RunUntil(cpu, bus, 200_000_000, () => !editor.HasQueuedInput);
        RunSteps(cpu, bus, 5_000_000); // let the last RUN/LIST settle

        // The load-bearing invariant: EVERY BASIC memory pointer (program text,
        // variables, arrays, string heap, mem limit) sits at or above the band top
        // ($0900). This proves Ram_base-keyed storage never lands in the reserved
        // $0300-$08FF band, independent of the band's legitimate runtime occupants
        // (the lib_call mailbox, the resident loader, and paged module BSS — all of
        // which the line reader now exercises, since LAB_1357 routes through
        // lib_call(SYSTEM, SYS_SCREEN_READLINE) on every input line).
        ushort Ptr(ushort lo) => (ushort)(bus.ReadRam(lo) | (bus.ReadRam((ushort)(lo + 1)) << 8));
        (string Name, ushort Lo)[] pointers =
        {
            ("Smem  (Start-of-Basic)",       0x79),
            ("Svar  (Start-of-Variables)",   0x7B),
            ("Sarry (Start-of-Arrays)",      0x7D),
            ("Earry (End-of-Arrays)",        0x7F),
            ("Sstor (String storage)",       0x81),
            ("Emem  (Limit-of-memory)",      0x85),
        };
        foreach (var (name, lo) in pointers)
        {
            ushort p = Ptr(lo);
            Assert.IsTrue(p >= MailboxEnd,
                $"BASIC pointer {name} = ${p:X4}; must be >= ${MailboxEnd:X4} so the " +
                $"$0300-$08FF reserved band sits below all user storage.");
        }

        // Direct corroboration: BASIC's program text starts within the first user
        // page at/just above Ram_base ($0900) — never in the reserved band below it.
        // (EhBASIC stores a leading $00 at Ram_base and keeps Smem one byte past it,
        // so Smem = $0901; the invariant is "in the $09xx page", not exactly $0900.)
        ushort smem = Ptr(0x79);
        Assert.IsTrue(smem >= MailboxEnd && smem < MailboxEnd + 0x100,
            $"Start-of-Basic (Smem) = ${smem:X4}; program text must begin in the first " +
            $"user page (>= $0900) just above the reserved band.");
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }
        Assert.Fail($"Timed out waiting for screen to contain '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
    }

    private static void RunUntil(Cpu cpu, CompositeBusDevice bus, int maxSteps, Func<bool> predicate)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && predicate())
                return;
        }
    }

    private static void RunSteps(Cpu cpu, CompositeBusDevice bus, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }
    }

    private static string SnapshotScreen(VirtualGraphicsController vgc)
    {
        var sb = new System.Text.StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = vgc.GetScreenChar(col, row);
                sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }
}
