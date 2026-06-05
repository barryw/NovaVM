using System;
using KDS.e6502;

namespace e6502UnitTests
{
    /// <summary>
    /// Behavioral test bus for the paged-library loader (lib_call, T5). Models the three
    /// hardware things the loader touches against the real <see cref="Cpu"/>:
    ///   1. ROMSWAP ($A03F) — a WRITE flips what is mapped at $C000-$FFFF between the runtime
    ///      bank (RAM) and the bank-1 ext_rom overlay. On real HW a READ of $A03F returns RAM
    ///      (NOT the bank), and the loader never reads it — modeled faithfully here.
    ///   2. Page-in ($BA76) — writing 1 to PGD_CMD copies WORDS words from XRAM[SRC] into the
    ///      ext_rom image (byte order is identity). Synchronous model: PGD_STATUS always reads
    ///      DONE/ok.
    ///   3. A fake 512 KB XRAM shelf, loadable by tests.
    /// Mirrors the contract proven on HW (page_dma, Verilator Phase 2) and the $A03F overlay
    /// contract from RomSwapBus in PageInChecksumStubTests.
    /// </summary>
    internal sealed class LibLoaderBus : IBusDevice
    {
        private readonly byte[] _ram  = new byte[0x10000];
        private readonly byte[] _ext  = new byte[0x4000];        // bank-1 ext_rom image ($C000-$FFFF)
        private readonly byte[] _xram = new byte[512 * 1024];    // fake SDRAM shelf
        private byte _romswap = 0x02;                            // ROMSWAP_BASIC
        public int PageInCount { get; private set; }

        private const ushort REG_ROMSWAP = 0xA03F;
        private const byte    RS_EXT      = 0x04;
        private const byte    PGD_DONE    = 0x02;
        private const ushort  VGC_CMD = 0xA010;                  // command/busy register
        private const ushort  PGD_CMD = 0xBA76, PGD_STATUS = 0xBA77,
                              PGD_SRCL = 0xBA78, PGD_SRCM = 0xBA79, PGD_SRCH = 0xBA7A,
                              PGD_WORDSL = 0xBA7B, PGD_WORDSH = 0xBA7C;

        public byte Read(ushort a)
        {
            if (a == PGD_STATUS) return PGD_DONE;                 // synchronous model: always done/ok
            if (a == VGC_CMD) return 0;                          // VGC executes synchronously: never busy
            if (a >= 0xC000 && _romswap == RS_EXT) return _ext[a - 0xC000];
            return _ram[a];                                      // incl. $A03F (not readable on HW)
        }

        public void Write(ushort a, byte d)
        {
            if (a == REG_ROMSWAP) { _romswap = d; return; }      // write flips the overlay
            _ram[a] = d;                                         // SRC/WORDS land in RAM for readback
            if (a == PGD_CMD && (d & 1) != 0) DoPageIn();
        }

        private void DoPageIn()
        {
            int src = _ram[PGD_SRCL] | (_ram[PGD_SRCM] << 8) | (_ram[PGD_SRCH] << 16);
            int words = _ram[PGD_WORDSL] | (_ram[PGD_WORDSH] << 8);
            for (int i = 0; i < words; i++)                      // byte order = identity (design)
            {
                _ext[2 * i]     = _xram[src + 2 * i];
                _ext[2 * i + 1] = _xram[src + 2 * i + 1];
            }
            PageInCount++;
        }

        // Dynamic module shelf (libabi.inc): N=4 cache slots at SHELF_BASE + i*$4000;
        // the directory (slot->id tags + LRU order) lives in the resident loader band.
        private const int    ShelfBaseAddr = 0x060000, ShelfSlotBytes = 0x4000, ShelfN = 4;
        private const ushort ShelfTag = 0x0418, ShelfLru = 0x041C;   // SHELF_N bytes each

        // Mirror the firmware/CompositeBusDevice boot path: stage a module image into XRAM
        // shelf slot and seed the directory so the resident loader's scan finds it.
        // Seeds shelf_lru to identity [0,1,2,3] on the slot-0 stage.
        public void StageShelfModule(int slot, byte[] image, byte id)
        {
            LoadXram(ShelfBaseAddr + slot * ShelfSlotBytes, image);
            _ram[ShelfTag + slot] = id;
            if (slot == 0)
                for (int i = 0; i < ShelfN; i++) _ram[ShelfLru + i] = (byte)i;
        }

        public void LoadRam(ushort at, byte[] b)  => Array.Copy(b, 0, _ram,  at, b.Length);
        public void LoadXram(int at, byte[] b)    => Array.Copy(b, 0, _xram, at, b.Length);
        public void LoadExtFromXram(int src)      => Array.Copy(_xram, src, _ext, 0, _ext.Length); // HIT-path helper
        public byte PeekRam(ushort a)             => _ram[a];
        public void PokeRam(ushort a, byte v)     => _ram[a] = v;
        public byte CurrentBank                   => _romswap;
    }
}
