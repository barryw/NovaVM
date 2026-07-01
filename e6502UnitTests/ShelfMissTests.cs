using System;
using System.Collections.Generic;
using System.IO;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class ShelfMissTests
{
    private static string RepoPath(params string[] p) =>
        Path.Combine(Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..")), Path.Combine(p));

    // Mailbox cells + loader entry mirror runtime/asm/libabi.inc (see CompositeLoaderTests).
    private const ushort STATUS = 0x0302, HOME_BANK = 0x0317, RESIDENT = 0x0318;
    private const ushort LIB_CALL = 0x0320;       // resident loader entry (libabi.inc LIB_LOADER_BAND)
    private const byte   MODULE_ID_GRAPHICS = 0x01;
    private const byte   MODULE_ID_FILES = 0x04;  // not pre-staged -> must demand-load
    private const byte   FILE_RNG = 0x0D;         // libfiles.inc: () -> RESULT = 32 host RNG bits
    private const byte   GFN_GCLS = 0x00;
    private const byte   LERR_OK = 0x00;
    private const byte   LERR_BAD_MODULE = 0x81;  // libabi.inc LERR_BAD_MODULE
    private const ushort Sentinel = 0xFFF9;       // loader's final RTS lands here; loop stops

    // Drive the resident lib_call loader for (mod, fn). Fakes a JSR to $0320 by pushing
    // (Sentinel-1) so the loader's final RTS lands on Sentinel; runs until it returns.
    private static void CallLib(CompositeBusDevice bus, byte mod, byte fn)
    {
        bus.WriteRam(0x0300, mod);     // LIB_MOD_ID
        bus.WriteRam(0x0301, fn);      // LIB_FN_ID
        bus.WriteRam(STATUS, 0xFF);    // poison so a real OK write is observable

        var cpu = new Cpu(bus, E6502Type.Cmos);
        bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
        bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
        var s = cpu.GetState();
        cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, LIB_CALL,
                                      s.Nf, s.Vf, s.Df, s.If, s.Zf, s.Cf));

        for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
            cpu.ExecuteNext();
        Assert.AreEqual(Sentinel, cpu.Pc, "lib_call did not return via RTS to the sentinel");
    }

    [TestMethod]
    public void Fio_LoadModule_StreamsStoreImageIntoSlot()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        byte[] gfx = File.ReadAllBytes(RepoPath("software", "modules", "graphics", "graphics.bin"));
        bus.SetShelfModuleStore(new Dictionary<byte, byte[]> { [1] = gfx });

        // request LOAD_MODULE(id=1, slot=2). FIO owns its register bank, so params and the
        // trigger must go through the routed bus write (Write), not the raw RAM hook (WriteRam).
        bus.Write(0xB9A4, 1);     // OFF_SRC_LO = id   (FioSrcL)
        bus.Write(0xB9A6, 2);     // OFF_END_LO = slot (FioEndL)
        bus.Write(0xB9A0, 0x2C);  // CMD_LOAD_MODULE -> triggers synchronously

        // host (emulator) executes synchronously on the cmd write; verify XRAM slot 2 == image
        for (int i = 0; i < gfx.Length; i++)
            Assert.AreEqual(gfx[i], bus.ReadXram(0x068000 + i), $"slot-2 XRAM mismatch at {i}");
        Assert.AreEqual(2, bus.Read(0xB9A1), "FIO STATUS must be OK(2)");
    }

    [TestMethod]
    public void Fio_LoadModule_UnknownId_SetsError()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        bus.SetShelfModuleStore(new Dictionary<byte, byte[]>());   // empty store
        bus.Write(0xB9A4, 0x42);
        bus.Write(0xB9A6, 0);
        bus.Write(0xB9A0, 0x2C);
        Assert.AreEqual(3, bus.Read(0xB9A1), "unknown id must set FIO STATUS ERR(3)");
    }

    [TestMethod]
    public void LibCall_Miss_LoadsFromHostThenDispatches()
    {
        using var bus = new CompositeBusDevice(enableSound:false, bootRom:CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
        // empty shelf (no StageShelfModule); host store has graphics
        bus.SetShelfModuleStore(new Dictionary<byte,byte[]>{ [1]=File.ReadAllBytes(RepoPath("software", "modules","graphics","graphics.bin")) });
        bus.WriteRam(RESIDENT, 0x00);
        bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);
        // ensure shelf_tag all empty + lru identity
        for (int i=0;i<4;i++){ bus.WriteRam((ushort)(0x0418+i),0); bus.WriteRam((ushort)(0x041C+i),(byte)i); }

        CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

        Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "miss must load-on-demand then dispatch OK");
        Assert.AreEqual(MODULE_ID_GRAPHICS, bus.ReadRam(RESIDENT));
        Assert.AreEqual(1, bus.PageInCount);
        // graphics landed in some slot tag
        bool tagged=false; for(int i=0;i<4;i++) if(bus.ReadRam((ushort)(0x0418+i))==1) tagged=true;
        Assert.IsTrue(tagged, "loaded module id must be recorded in shelf_tag");
    }

    [TestMethod]
    public void LibCall_Miss_EvictsLruBackWhenFull()
    {
        using var bus = new CompositeBusDevice(enableSound:false, bootRom:CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
        bus.SetShelfModuleStore(new Dictionary<byte,byte[]>{ [1]=File.ReadAllBytes(RepoPath("software", "modules","graphics","graphics.bin")) });
        // fill all 4 slots with dummy ids, lru order [0,1,2,3] => LRU back = slot 3
        for (int i=0;i<4;i++){ bus.WriteRam((ushort)(0x0418+i),(byte)(0x10+i)); bus.WriteRam((ushort)(0x041C+i),(byte)i); }
        bus.WriteRam(RESIDENT, 0x00);
        bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

        CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

        Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS));
        Assert.AreEqual(1, bus.ReadRam(0x041B), "victim should be slot 3 (LRU back): tag[3] now =1");
        Assert.AreEqual(MODULE_ID_GRAPHICS, bus.ReadRam(RESIDENT));
    }

    // Production demand-load wiring: StageConfiguredModules populates the FIO module
    // store so a lib_call to a NON-pre-staged module (>4 modules, 4 shelf slots) still
    // demand-loads. The store is built by scanning the Resources dir for module images
    // (magic "NL" at +3, id at +5), keyed by module id; non-module .bins are ignored.
    [TestMethod]
    public void BuildModuleStore_IndexesModuleImagesById_IgnoringNonModules()
    {
        string tmp = Path.Combine(Path.GetTempPath(), "novamodstore_" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(tmp);
        try
        {
            File.Copy(RepoPath("software", "modules", "graphics", "graphics.bin"), Path.Combine(tmp, "graphics.bin"));
            File.Copy(RepoPath("software", "modules", "files", "files.bin"), Path.Combine(tmp, "files.bin"));
            File.WriteAllBytes(Path.Combine(tmp, "ehbasic.bin"), new byte[16384]); // non-module: no "NL" magic

            var store = CompositeBusDevice.BuildModuleStore(tmp);

            Assert.IsTrue(store.ContainsKey(0x01), "graphics (id 1) must be indexed by its header id");
            Assert.IsTrue(store.ContainsKey(0x04), "files (id 4) must be indexed by its header id");
            Assert.IsFalse(store.ContainsKey(0x00), "a non-module .bin (no NL magic) must be ignored");
            Assert.AreEqual(16384, store[0x04].Length, "stored image must be the full 16K module");
            Assert.AreEqual((byte)'N', store[0x04][3], "image must carry the NL module magic at +3");
        }
        finally { Directory.Delete(tmp, true); }
    }

    // End-to-end: the PRODUCTION ctor (StageConfiguredModules) populates the demand-load
    // store from Resources — no manual SetShelfModuleStore. A lib_call to FILES (id 4,
    // never pre-staged) must miss, stream files.bin from that store, dispatch, and RUN the
    // wrapper: FILE_RNG -> fio_rng -> the FIO RNG -> LERR_OK. First end-to-end execution of
    // a new module's marshalling (everything else was structural-only).
    [TestMethod]
    public void ProductionStore_DemandLoadsAndRunsUnstagedModule_FileRng()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
        // empty shelf + identity LRU; the store comes from the ctor (NOT set here)
        for (int i = 0; i < 4; i++) { bus.WriteRam((ushort)(0x0418 + i), 0); bus.WriteRam((ushort)(0x041C + i), (byte)i); }
        bus.WriteRam(RESIDENT, 0x00);
        bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

        CallLib(bus, MODULE_ID_FILES, FILE_RNG);

        Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS),
            "FILES must demand-load from the production store and FILE_RNG must run to OK");
        bool tagged = false;
        for (int i = 0; i < 4; i++) if (bus.ReadRam((ushort)(0x0418 + i)) == MODULE_ID_FILES) tagged = true;
        Assert.IsTrue(tagged, "demand-loaded FILES id must be recorded in a shelf_tag slot");
    }

    [TestMethod]
    public void LibCall_Miss_HostError_ReturnsBadModuleAndLeavesSlotEmpty()
    {
        using var bus = new CompositeBusDevice(enableSound:false, bootRom:CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
        bus.SetShelfModuleStore(new Dictionary<byte,byte[]>());   // empty -> host returns ERR
        for (int i=0;i<4;i++){ bus.WriteRam((ushort)(0x0418+i),0); bus.WriteRam((ushort)(0x041C+i),(byte)i); }
        bus.WriteRam(RESIDENT, 0x00);
        bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

        CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

        Assert.AreEqual(LERR_BAD_MODULE, bus.ReadRam(STATUS));
        for(int i=0;i<4;i++) Assert.AreEqual(0, bus.ReadRam((ushort)(0x0418+i)), "failed load must leave slots empty");
    }
}
