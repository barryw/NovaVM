using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// Proves the resident paged-library loader <c>lib_call</c> (runtime/asm/libcall.s, T5)
    /// against the real <see cref="Cpu"/> over the behavioral <see cref="LibLoaderBus"/>:
    ///   - HIT path: module already resident → dispatch, no page-in.
    ///   - MISS path: program $BA76 page-in (XRAM→bank-1 ext_rom), validate the "NL" header,
    ///     cache LIB_RESIDENT, flip ROMSWAP to EXT, JSR $C000, restore the home bank.
    ///   - Resident cache: a second call to the same module does not re-page.
    ///   - Error paths: bad magic / wrong id / bad version / bad fn / unknown module set
    ///     LIB_STATUS and restore the home bank without a stale EXT mapping.
    /// The loader is built as a relocatable RAM blob (ORG $9C00) because the swap stubs must
    /// run from RAM while bank 1 is mapped over $C000-$FFFF. Mirrors the CPU-driving harness in
    /// PageInChecksumStubTests: fake the JSR by pushing (Sentinel-1), step ExecuteNext() to the
    /// sentinel. The blob + module image are deterministic committed artifacts resolved from the
    /// repo root, the same way ModuleFormatTests resolves testmod.bin.
    /// </summary>
    [TestClass]
    public class LibCallTests
    {
        // Mailbox + entry constants mirror runtime/asm/libabi.inc.
        private const ushort MOD_ID = 0x0300, FN_ID = 0x0301, STATUS = 0x0302,
                             ARG0 = 0x0303, ARG1 = 0x0307, RESULT = 0x0313,
                             HOME_BANK = 0x0317, RESIDENT = 0x0318;
        private const ushort LibCallEntry = 0x9C00;   // blob load address == first emitted byte
        private const int    ShelfBase = 0x060000;    // XRAM shelf slot 0
        private const byte   RsBasic = 0x02;
        private const ushort Sentinel = 0xFFF9;       // lib_call RTS returns here; loop stops

        private static (LibLoaderBus bus, ushort entry) Setup()
        {
            var bus = new LibLoaderBus();
            bus.LoadRam(LibCallEntry, File.ReadAllBytes(RepoPath("software", "tests", "asm", "libcall.bin")));
            bus.StageShelfModule(0, File.ReadAllBytes(RepoPath("software", "tests", "asm", "testmod.bin")), 0x7F);
            bus.PokeRam(HOME_BANK, RsBasic);   // runtime's home ROMSWAP value
            bus.PokeRam(RESIDENT, 0x00);       // nothing resident yet
            return (bus, LibCallEntry);
        }

        private static byte[] CallLib(LibLoaderBus bus, ushort entry,
                                      byte mod, byte fn, int arg0 = 0, int arg1 = 0)
        {
            bus.PokeRam(MOD_ID, mod);
            bus.PokeRam(FN_ID, fn);
            Write32(bus, ARG0, arg0);
            Write32(bus, ARG1, arg1);

            var cpu = new Cpu(bus, E6502Type.Cmos);

            // Fake a JSR to lib_call: push (Sentinel-1) so the loader's final RTS lands on Sentinel.
            bus.PokeRam(0x01FF, (byte)((Sentinel - 1) >> 8));   // PCH
            bus.PokeRam(0x01FE, (byte)((Sentinel - 1) & 0xFF)); // PCL
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, entry,
                                          s.Nf, s.Vf, s.Df, s.If, s.Zf, s.Cf));

            for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "lib_call did not return via RTS to the sentinel");

            return Read32(bus, RESULT);
        }

        [TestMethod]
        public void Miss_FromHostStore_LoadsThenDispatches()
        {
            // True demand-load miss through LibLoaderBus: empty shelf (no StageShelfModule),
            // host store provides the image. Exercises SetModuleStore + DoLoadModule (the FIO
            // mirror) plus the loader's miss → page-in → validate → dispatch tail.
            var bus = new LibLoaderBus();
            bus.LoadRam(LibCallEntry, File.ReadAllBytes(RepoPath("software", "tests", "asm", "libcall.bin")));
            bus.PokeRam(HOME_BANK, RsBasic);
            bus.PokeRam(RESIDENT, 0x00);
            // shelf_tag[] all empty (default RAM=0); shelf_lru[] identity so shelf_touch resolves.
            for (int i = 0; i < 4; i++) bus.PokeRam((ushort)(0x041C + i), (byte)i);
            // host store has the TEST module under id $7F; NOT staged into any slot.
            bus.SetModuleStore(0x7F, File.ReadAllBytes(RepoPath("software", "tests", "asm", "testmod.bin")));

            var r = CallLib(bus, LibCallEntry, 0x7F, fn: 1, arg0: 1000, arg1: 337);

            CollectionAssert.AreEqual(BitConverter.GetBytes(1337), r, "module must dispatch after demand-load");
            Assert.AreEqual(1, bus.PageInCount, "miss must page in exactly once");
            Assert.AreEqual(0x7F, bus.PeekRam(RESIDENT), "loaded module must be cached resident");
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
            // demand-loaded id must be recorded in some shelf_tag slot ($0418-$041B).
            bool tagged = false;
            for (int i = 0; i < 4; i++) if (bus.PeekRam((ushort)(0x0418 + i)) == 0x7F) tagged = true;
            Assert.IsTrue(tagged, "demand-loaded id must be recorded in shelf_tag[]");
        }

        [TestMethod]
        public void Echo_Hit_ReturnsArg0_NoPageIn()
        {
            var (bus, entry) = Setup();
            bus.PokeRam(RESIDENT, 0x7F);          // pretend TEST is already mapped
            bus.LoadExtFromXram(ShelfBase);       // module image already resident in ext_rom

            var r = CallLib(bus, entry, 0x7F, fn: 0, arg0: 0x12345678);

            CollectionAssert.AreEqual(BitConverter.GetBytes(0x12345678), r);
            Assert.AreEqual(0, bus.PageInCount, "HIT path must not page in");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
        }

        [TestMethod]
        public void Add_Miss_PagesIn_ThenDispatches()
        {
            var (bus, entry) = Setup();           // RESIDENT=0, ext_rom empty

            var r = CallLib(bus, entry, 0x7F, fn: 1, arg0: 1000, arg1: 337);

            CollectionAssert.AreEqual(BitConverter.GetBytes(1337), r);
            Assert.AreEqual(1, bus.PageInCount, "MISS path must page in exactly once");
            Assert.AreEqual(0x7F, bus.PeekRam(RESIDENT), "resident module must be cached");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
        }

        [TestMethod]
        public void SecondCall_SameModule_NoSecondPageIn()
        {
            var (bus, entry) = Setup();

            CallLib(bus, entry, 0x7F, fn: 0, arg0: 5);
            CallLib(bus, entry, 0x7F, fn: 1, arg0: 2, arg1: 3);

            Assert.AreEqual(1, bus.PageInCount, "resident after first call → no second page-in");
        }

        [TestMethod]
        public void Sum_Bytes_SmallBuffer()
        {
            var (bus, entry) = Setup();
            byte[] buf = { 1, 2, 3, 4, 5, 10, 20 };       // sum = 45
            const ushort ptr = 0x0500;
            for (int i = 0; i < buf.Length; i++) bus.PokeRam((ushort)(ptr + i), buf[i]);

            // ARG0 is BYTES(ptr16, len16): low word = ptr, high word = len.
            var r = CallLib(bus, entry, 0x7F, fn: 2, arg0: ptr | (buf.Length << 16));

            CollectionAssert.AreEqual(BitConverter.GetBytes(45), r);
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
        }

        [TestMethod]
        public void Sum_Bytes_Over256()
        {
            var (bus, entry) = Setup();
            const int len = 300;                          // > 256 exercises the high length byte
            const ushort ptr = 0x0500;
            for (int i = 0; i < len; i++) bus.PokeRam((ushort)(ptr + i), 1);  // all 1s → sum = 300

            var r = CallLib(bus, entry, 0x7F, fn: 2, arg0: ptr | (len << 16));

            CollectionAssert.AreEqual(BitConverter.GetBytes(len), r);
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
        }

        [TestMethod]
        public void Sum_Bytes_ZeroLen()
        {
            var (bus, entry) = Setup();
            const ushort ptr = 0x0500;
            bus.PokeRam(ptr, 0xFF);                        // present but must NOT be summed

            var r = CallLib(bus, entry, 0x7F, fn: 2, arg0: ptr | (0 << 16));

            CollectionAssert.AreEqual(BitConverter.GetBytes(0), r);
            Assert.AreEqual(0x00, bus.PeekRam(STATUS), "status must be OK");
        }

        [TestMethod]
        public void BadMagic_SetsStatus_NoDispatch()
        {
            var (bus, entry) = Setup();
            var bad = (byte[])File.ReadAllBytes(RepoPath("software", "tests", "asm", "testmod.bin")).Clone();
            bad[3] = 0x00;                        // corrupt 'N'
            bus.LoadXram(ShelfBase, bad);

            CallLib(bus, entry, 0x7F, fn: 0, arg0: 9);

            Assert.AreEqual(0x80, bus.PeekRam(STATUS), "LERR_BAD_MAGIC");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
        }

        [TestMethod]
        public void WrongModuleId_SetsStatus()
        {
            var (bus, entry) = Setup();
            var bad = (byte[])File.ReadAllBytes(RepoPath("software", "tests", "asm", "testmod.bin")).Clone();
            bad[5] = 0x02;                        // header claims SOUND, we ask for TEST
            bus.LoadXram(ShelfBase, bad);

            CallLib(bus, entry, 0x7F, fn: 0, arg0: 9);

            Assert.AreEqual(0x81, bus.PeekRam(STATUS), "LERR_BAD_MODULE");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
        }

        [TestMethod]
        public void BadVersion_SetsStatus()
        {
            var (bus, entry) = Setup();
            var bad = (byte[])File.ReadAllBytes(RepoPath("software", "tests", "asm", "testmod.bin")).Clone();
            bad[6] = 0x02;                        // MOD_VERSION ($C006) claims ABI v2; loader expects v1
            bus.LoadXram(ShelfBase, bad);

            CallLib(bus, entry, 0x7F, fn: 0, arg0: 9);

            Assert.AreEqual(0x82, bus.PeekRam(STATUS), "LERR_BAD_VER");   // $82, not $83
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
        }

        [TestMethod]
        public void BadFnId_SetsStatus()
        {
            var (bus, entry) = Setup();

            CallLib(bus, entry, 0x7F, fn: 9, arg0: 0);   // only fns 0,1 exist

            Assert.AreEqual(0x83, bus.PeekRam(STATUS), "LERR_NO_FN (module's own bad_fn)");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
        }

        [TestMethod]
        public void UnknownModule_SetsStatus()
        {
            var (bus, entry) = Setup();

            CallLib(bus, entry, 0x55, fn: 0);            // not in modtab

            Assert.AreEqual(0x81, bus.PeekRam(STATUS), "LERR_BAD_MODULE");
            Assert.AreEqual(0, bus.PageInCount, "unknown module must not page in");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank never changed");   // EXT never mapped
        }

        private static void Write32(LibLoaderBus bus, ushort addr, int v)
        {
            for (int i = 0; i < 4; i++) bus.PokeRam((ushort)(addr + i), (byte)(v >> (8 * i)));
        }

        private static byte[] Read32(LibLoaderBus bus, ushort addr)
        {
            var b = new byte[4];
            for (int i = 0; i < 4; i++) b[i] = bus.PeekRam((ushort)(addr + i));
            return b;
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
