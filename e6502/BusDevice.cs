namespace KDS.e6502
{
    /// <summary>
    /// Example Bus Device for loading a 
    /// </summary>
    public sealed class BusDevice : IBusDevice
    {
        private readonly byte[] _ram = new byte[0x10000];

        public BusDevice(byte[] program, ushort loadingAddress = 0)
        {
            Load(program, loadingAddress);
        }

        public void Load(byte[] program)
        {
            Load(program, 0);
        }

        private void Load(byte[] program, int loadingAddress)
        {
            program.CopyTo(_ram, loadingAddress);
        }

        public byte Read(ushort address)
        {
            return _ram[address];
        }

        public void Write(ushort address, byte data)
        {
            _ram[address] = data;
        }
    }
}
