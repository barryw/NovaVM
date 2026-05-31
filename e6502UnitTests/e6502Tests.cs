using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    [TestClass]
    public class E6502Tests
    {

        private const string ResourcePath = @"Resources/";

        /*
         *  Load and run test program found here:
         *  https://codegolf.stackexchange.com/questions/12844/emulate-a-mos-6502-cpu?rq=1
         *  If the program gets to PC=$45C0 then all tests passed.
         */

        [TestMethod]
        public void RunAllSuiteTest()
        {
            var bus = new BusDevice(File.ReadAllBytes($"{ResourcePath}AllSuiteA.bin"), 0x4000);
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);

            ushort prevPc;
            do
            {
                prevPc = cpu.Pc;
                cpu.ExecuteNext();
            } while (prevPc != cpu.Pc);
            Assert.AreEqual(0x45c0, cpu.Pc, $"Test program failed at ${cpu.Pc:X4}");
            Assert.AreEqual(0xff, cpu.SystemBus.Read(0x0210), "Test value failed");
        }

        [TestMethod]
        public void RunAllSuiteTestByTick()
        {
            var bus = new BusDevice(File.ReadAllBytes($"{ResourcePath}AllSuiteA.bin"), 0x4000);
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);

            ushort prevPc;
            do
            {
                prevPc = cpu.Pc;
                cpu.ClocksForNext();
                cpu.ExecuteNext();

            } while (prevPc != cpu.Pc);
            Assert.AreEqual(0x45c0, cpu.Pc, $"Test program failed at ${cpu.Pc:X4}");
            Assert.AreEqual(0xff, cpu.SystemBus.Read(0x0210), "Test value failed");
        }

        [TestMethod]
        public void RunExtTestProgram()
        {
            /*
             *  This loads a test program that exercises all the extended instructions of the 65C02 (CMOS).
             *  If the program gets to PC=24a8 then all tests passed.
             */

            var bus = new BusDevice(File.ReadAllBytes($"{ResourcePath}65C02_extended_opcodes_test.bin"));
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);

            ushort prevPc;

            do
            {
                prevPc = cpu.Pc;
                cpu.ClocksForNext();
                cpu.ExecuteNext();

            } while (prevPc != cpu.Pc);
            Assert.AreEqual(0x24a8, cpu.Pc, $"Test program failed at ${cpu.Pc:X4}");
        }

        // A read-counting bus to prove that store instructions never read their
        // destination. Real 6502 STA/STX/STY/STZ perform no read of the target;
        // the cycle-prefetch path must not either, or it corrupts read-sensitive
        // I/O (e.g. the VGC VRAM port, which auto-increments on every read).
        private sealed class ReadCountingBus : IBusDevice
        {
            private readonly byte[] _ram = new byte[0x10000];
            public readonly int[] Reads = new int[0x10000];
            public ReadCountingBus(byte[] program, ushort load) => program.CopyTo(_ram, load);
            public byte Read(ushort address) { Reads[address]++; return _ram[address]; }
            public void Write(ushort address, byte data) => _ram[address] = data;
        }

        [TestMethod]
        public void StoreDoesNotReadItsTarget()
        {
            byte[] prog =
            {
                0xA9, 0x42,        // LDA #$42
                0x8D, 0x00, 0x50,  // STA $5000   (store: must NOT read $5000)
                0xAD, 0x01, 0x50,  // LDA $5001   (load: must read $5001)
                0x4C, 0x08, 0x04,  // JMP $0408   (self loop, not executed)
            };
            var bus = new ReadCountingBus(prog, 0x0400);
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);
            for (int i = 0; i < 3; i++) { cpu.ClocksForNext(); cpu.ExecuteNext(); }
            Assert.AreEqual(0, bus.Reads[0x5000], "STA must not read its destination address.");
            Assert.IsTrue(bus.Reads[0x5001] >= 1, "LDA must read its source address.");
            Assert.AreEqual(0x42, cpu.SystemBus.Read(0x5000), "STA must still write its value.");
        }

        [TestMethod]
        public void CmosStzStoresZeroForAllAddressingModes()
        {
            var program = new byte[]
            {
                0xA9, 0xA0,             // LDA #$A0
                0x8D, 0x00, 0x02,       // STA $0200
                0x9C, 0x01, 0x02,       // STZ $0201
                0xA2, 0x02,             // LDX #$02
                0xA9, 0x55,             // LDA #$55
                0x9E, 0x00, 0x02,       // STZ $0200,X
                0xA9, 0x77,             // LDA #$77
                0x64, 0x23,             // STZ $23
                0xA9, 0x99,             // LDA #$99
                0x74, 0x22,             // STZ $22,X
                0xEA,                   // NOP
            };
            var bus = new BusDevice(program, 0x0400);
            foreach (ushort address in new ushort[] { 0x0200, 0x0201, 0x0202, 0x0023, 0x0024 })
                bus.Write(address, 0xFF);

            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);
            for (int i = 0; i < 10; i++)
                cpu.ExecuteNext();

            Assert.AreEqual(0xA0, bus.Read(0x0200), "STA baseline proves the program wrote to RAM.");
            Assert.AreEqual(0x00, bus.Read(0x0201), "STZ absolute must assert the store path and write zero.");
            Assert.AreEqual(0x00, bus.Read(0x0202), "STZ absolute,X must assert the store path and write zero.");
            Assert.AreEqual(0x00, bus.Read(0x0023), "STZ zero-page must write zero.");
            Assert.AreEqual(0x00, bus.Read(0x0024), "STZ zero-page,X must write zero.");
        }

        [TestMethod]
        public void RunFuncTestProgram()
        {
            /*
             *  This loads a test program that exercises all the standard instructions of the 6502.
             *  If the program gets to PC=$3399 then all tests passed.
             */

            var bus = new BusDevice(File.ReadAllBytes($"{ResourcePath}6502_functional_test.bin"));
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(0x0400);

            ushort prevPc;
            do
            {
                prevPc = cpu.Pc;
                cpu.ClocksForNext();
                cpu.ExecuteNext();

            } while (prevPc != cpu.Pc);
            Assert.AreEqual(0x3399, cpu.Pc, $"Test program failed at ${cpu.Pc:X4}");
        }

        [TestMethod]
        public void RunInterruptTest()
        {
            /*
             *  This loads a test program that tests interrupt handling in the 6502.
             *  If the program gets to PC=$06ec then all tests passed.
             *  
             *  Unlike the other test binaries, this one required NMOS mode
             */

            var bus = new BusDevice(File.ReadAllBytes($"{ResourcePath}6502_interrupt_test.bin"), 0x0400);
            var cpu = new Cpu(bus);
            cpu.Boot(0x0400);

            ushort prevPc;
            do
            {
                prevPc = cpu.Pc;
                cpu.ClocksForNext();
                cpu.ExecuteNext();

                // Add interrupts where expected in the test.
                switch (prevPc)
                {
                    // IRQ tests
                    case 0x0434:
                    case 0x0464:
                    case 0x04a3:
                    case 0x04de:
                        cpu.IrqWaiting = true;
                        break;

                    // NMI tests
                    case 0x05c8:
                    case 0x05f8:
                    case 0x0637:
                    case 0x0672:
                        cpu.NmiWaiting = true;
                        break;

                    // IRQ and NMI waiting tests
                    case 0x06a0:
                    case 0x06db:
                        cpu.IrqWaiting = true;
                        cpu.NmiWaiting = true;
                        break;
                }

            } while (prevPc != cpu.Pc);
            Assert.AreEqual(0x06ec, cpu.Pc, $"Test program failed at ${cpu.Pc:X4}");
        }
    }
}
