using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Proves the canonical paged-library reserved band $0300-$08FF stays clear in
/// NovaLogo — the Logo sibling of <see cref="MailboxReservationTests"/> for
/// NovaBASIC. The band = the $0300-$031F mailbox (libabi.inc LIB_MBOX), the
/// $0320-$041F resident loader band (256 B for libcall.s), and the $0420-$08FF
/// cross-runtime module-BSS band (1248 B, libabi.inc MODULE_BSS_BAND, carved by
/// 4c.2-1 for paged-module low-RAM working storage). All three must be FREE in
/// every runtime.
///
/// NovaLogo carves the hole in novalogo.cfg by moving the BSS region START up
/// to $0900 (keeping its END fixed at $8000) and pushing HEAP_START to $09E0,
/// a uniform +$04E0 shift that preserves the proven-safe BSS↔heap overlap.
/// Before that carve, the very first BSS object — proc_body_buf (2048 bytes in
/// procedures.s) — straddles the band, so any procedure definition writes
/// through it.
///
/// Approach (sentinel-survives, the strongest faithful proof against the real
/// shipped novalogo.bin running on the Avalonia CompositeBusDevice): cold-start
/// Logo and define a procedure through the editor — which now lives in the SYSTEM
/// module and runs via lib_call(SYSTEM, SYS_FN_EDIT), so it legitimately drives the
/// $0300 mailbox and the $0420-$08FF module-BSS band during definition (that band
/// is FOR paged modules; it is not a runtime clobber). THEN paint a $5A sentinel
/// across the whole reserved band and drive the real ROM through FOUNDATION-ONLY
/// work — no further module/lib_call:
///   * invoke the procedure  -> re-tokenizer + proc_body_buf (Logo BSS, $0900+)
///   * build a list          -> the list/heap machinery
///   * heap allocation        -> the bump allocator ($09E0+)
/// We then assert every band byte $0300-$08FF still equals $5A. (Foundation paths
/// touch the band only if the novalogo.cfg BSS/heap carve regressed.)
///
/// We deliberately do NOT mock — this exercises the actual tokenizer,
/// procedure editor and heap in the shipped ROM image, the same way
/// NovaLogoHarnessTests does. ReadRam/WriteRam are host-side accessors into the
/// flat 64KB array; low RAM ($0300-$031F) has no MMIO interception so they are
/// byte-identical to a CPU access there.
/// </summary>
[TestClass]
public class NovaLogoMailboxReservationTests
{
    private const ushort MailboxStart = 0x0300;
    // Reserved band = $0300-$031F mailbox (LIB_MBOX) + $0320-$041F resident
    // loader band (libcall.s) + $0420-$08FF cross-runtime module-BSS band
    // (libabi.inc MODULE_BSS_BAND). Exclusive end = $0900.
    private const ushort MailboxEnd = 0x0900;
    private const byte Sentinel = 0x5A;
    // $0318 = LIB_RESIDENT (libabi.inc LIB_MBOX+24): a runtime-managed mailbox cell,
    // NOT scratch. cold_start seeds it $FF and ensure_ext_resident (4c.0c) rewrites it
    // when re-paging the host ext (e.g. the TO-editor path below). That deliberate
    // mailbox write is the loader contract, not a BSS/heap overlap, so it is excluded
    // from the sentinel sweep. Every other band byte must still survive byte-for-byte.
    private const ushort LibResident = 0x0318;

    [TestMethod]
    public void Logo_DoesNotClobber_MailboxRegion()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        // Wait for the Logo prompt ("?") after cold start.
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Define the procedure via the editor FIRST, then PRINT a marker so we know
        // the editor exited and proc_build_record ran. The editor now lives in the
        // SYSTEM module and runs via lib_call(SYSTEM, SYS_FN_EDIT), so DURING the
        // definition it legitimately drives the $0300 mailbox AND its working store
        // in the $0420-$08FF module-BSS band — that is the band's PURPOSE for a
        // paged module, not a runtime clobber (the same reason this test avoids
        // turtle/graphics commands, which also lib_call). So we paint the sentinel
        // AFTER the definition and then exercise only FOUNDATION paths.
        //
        // Body is a single long line of REPEAT loops so the editor writes the typed
        // body well past proc_body_buf offset $90 — the offset that, before the
        // novalogo.cfg carve, would have straddled $0300-$031F.
        QueueText(editor, "TO BOX :N\r");
        QueueText(editor,
            "REPEAT :N [MAKE \"A 1 MAKE \"B 2 MAKE \"A :A + 9 MAKE \"B :B + 8 " +
            "MAKE \"A :A + :B MAKE \"B :B + :A MAKE \"A 3 MAKE \"B 4 " +
            "MAKE \"A :A + 7 MAKE \"B :B + 6 MAKE \"C :A]\r");
        QueueText(editor, "\x13\x11");          // ^S save, ^Q quit editor
        QueueText(editor, "PRINT \"DEFINED\r"); // editor exited + proc record built
        RunUntilScreenContains(cpu, bus, "DEFINED", 200_000_000);

        // Paint the band sentinel NOW: the editor (a paged module) is done, and
        // everything below is FOUNDATION-ONLY — invoking the proc (drives the
        // re-tokenizer + proc_body_buf, both in Logo BSS at $0900+), building a list
        // and hammering the heap bump allocator ($09E0+). None of these foundation
        // paths may dip into $0300-$08FF if the novalogo.cfg carve holds.
        for (ushort a = MailboxStart; a < MailboxEnd; a++)
            bus.WriteRam(a, Sentinel);

        QueueText(editor, "BOX 1\r");           // invoke -> re-tokenize/proc_body_buf path
        QueueText(editor, "MAKE \"L [1 2 3 4]\r"); // build a list (heap)
        QueueText(editor, "SHOW :L\r");
        QueueText(editor, "REPEAT 8 [MAKE \"L FPUT 0 :L]\r"); // hammer the heap
        QueueText(editor, "PRINT \"MBOX_DONE\r");

        RunUntilScreenContains(cpu, bus, "MBOX_DONE", 200_000_000);
        RunSteps(cpu, bus, 2_000_000); // let the last line settle

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"Logo program fell through to the unknown-word path; the test program is broken.\n{screen}");
        Assert.IsFalse(screen.Contains("OUT OF MEMORY", StringComparison.OrdinalIgnoreCase),
            $"Logo program exhausted the heap; the test program is broken.\n{screen}");

        // The sentinel must survive byte-for-byte across the whole band, except for
        // LIB_RESIDENT ($0318) which the loader/ensure_ext_resident legitimately manages.
        for (ushort a = MailboxStart; a < MailboxEnd; a++)
        {
            if (a == LibResident) continue;
            Assert.AreEqual(Sentinel, bus.ReadRam(a),
                $"NovaLogo clobbered reserved band byte ${a:X4} (expected $5A, got " +
                $"${bus.ReadRam(a):X2}). BSS must start at $0900 (HEAP_START=$09E0) so " +
                $"$0300-$08FF stays clear.");
        }
    }

    private static void QueueText(ScreenEditor editor, string text)
    {
        foreach (char ch in text)
            editor.QueueInput((byte)ch);
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
        var sb = new StringBuilder();
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
