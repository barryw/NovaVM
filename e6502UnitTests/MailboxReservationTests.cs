using System;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Proves the canonical paged-library mailbox region $0300-$031F (libabi.inc
/// LIB_MBOX) stays clear in NovaBASIC. The mailbox must be FREE in every
/// runtime; NovaBASIC carves it out by starting user storage (Ram_base) at
/// $0320 instead of $0300.
///
/// Approach (sentinel-survives, the strongest faithful proof against the real
/// EhBASIC ROM running on the Avalonia CompositeBusDevice): boot BASIC, paint a
/// $5A sentinel across $0300-$031F directly in host RAM, then drive the real
/// ROM through entering a program that builds program text, scalar/string
/// variables and an array, and RUN it. BASIC keys program/variable/string
/// storage off Ram_base, so if Ram_base were still $0300 the program text and
/// variables would overwrite the sentinel. We assert every byte survives.
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
    private const ushort MailboxStart = 0x0300;
    private const ushort MailboxEnd = 0x0320; // exclusive (LIB_MBOX_END)
    private const byte Sentinel = 0x5A;

    [TestMethod]
    public void Basic_DoesNotClobber_MailboxRegion()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Paint the mailbox sentinel AFTER BASIC has cold-started (cold start
        // clears low RAM), so any later corruption is BASIC's doing.
        for (ushort a = MailboxStart; a < MailboxEnd; a++)
            bus.WriteRam(a, Sentinel);

        // Build program text + scalar + string + array, then RUN it. Each of
        // these touches Ram_base-relative storage. We queue every line up front
        // and step a fixed, generous budget rather than waiting on editor-idle:
        // when Ram_base is wrongly $0300 the sentinel write corrupts BASIC's
        // own program area and idle-detection can hang, so a bounded step loop
        // keeps the failure expressing as a clean sentinel-mismatch assertion.
        QueueLine(editor, "10 A=12345");
        QueueLine(editor, "20 B$=\"HELLO\"");
        QueueLine(editor, "30 DIM C(20)");
        QueueLine(editor, "40 FOR I=0 TO 20:C(I)=I:NEXT");
        QueueLine(editor, "RUN");
        QueueLine(editor, "LIST");
        RunUntil(cpu, bus, 200_000_000, () => !editor.HasQueuedInput);
        RunSteps(cpu, bus, 5_000_000); // let the last RUN/LIST settle

        // Start-of-variables pointer must be at or above the mailbox top.
        ushort svar = (ushort)(bus.ReadRam(0x7B) | (bus.ReadRam(0x7C) << 8));
        Assert.IsTrue(svar >= MailboxEnd,
            $"Start-of-variables (Svarl/Svarh) = ${svar:X4}; must be >= $0320 so " +
            $"the $0300-$031F mailbox sits below user storage.");

        // The sentinel must survive byte-for-byte.
        for (ushort a = MailboxStart; a < MailboxEnd; a++)
            Assert.AreEqual(Sentinel, bus.ReadRam(a),
                $"BASIC clobbered mailbox byte ${a:X4} (expected $5A, got " +
                $"${bus.ReadRam(a):X2}). Ram_base must be $0320 so $0300-$031F stays clear.");
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
