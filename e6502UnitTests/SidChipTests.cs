using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidChipTests
{
    [TestMethod]
    public void Write_PublishesRegisterEvent()
    {
        var sid = new SidChip(enableAudio: false);
        ushort eventAddress = 0;
        byte eventValue = 0;

        sid.RegisterWritten = (address, value) =>
        {
            eventAddress = address;
            eventValue = value;
        };

        sid.Write(0xD404, 0x21);

        Assert.AreEqual((ushort)0xD404, eventAddress);
        Assert.AreEqual((byte)0x21, eventValue);
    }
}
