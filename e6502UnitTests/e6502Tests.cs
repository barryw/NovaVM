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
