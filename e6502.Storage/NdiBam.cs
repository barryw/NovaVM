namespace e6502.Storage;

/// <summary>
/// Block Allocation Map for a Nova Disk Image.
/// Each bit represents one data sector: 0 = free, 1 = used.
/// </summary>
public sealed class NdiBam
{
    private readonly byte[] _bits;
    private readonly int _totalSectors;
    private int _freeCount;

    public int FreeCount => _freeCount;

    public NdiBam(int totalSectors)
    {
        _totalSectors = totalSectors;
        _bits = new byte[(totalSectors + 7) >> 3];
        _freeCount = totalSectors;
    }

    private NdiBam(byte[] bits, int totalSectors)
    {
        _bits = bits;
        _totalSectors = totalSectors;
        _freeCount = 0;
        for (int i = 0; i < totalSectors; i++)
            if (!IsAllocated(i))
                _freeCount++;
    }

    public bool IsAllocated(int sector) =>
        (_bits[sector >> 3] & (1 << (sector & 7))) != 0;

    /// <summary>
    /// Finds the first contiguous run of <paramref name="count"/> free sectors
    /// and marks them allocated. Returns the start sector, or -1 if no run exists.
    /// </summary>
    public int AllocateContiguous(int count)
    {
        int runStart = -1;
        int runLen = 0;

        for (int i = 0; i < _totalSectors; i++)
        {
            if (!IsAllocated(i))
            {
                if (runStart < 0)
                    runStart = i;
                runLen++;
                if (runLen == count)
                {
                    // Mark all sectors in the run as used
                    for (int j = runStart; j < runStart + count; j++)
                        _bits[j >> 3] |= (byte)(1 << (j & 7));
                    _freeCount -= count;
                    return runStart;
                }
            }
            else
            {
                runStart = -1;
                runLen = 0;
            }
        }

        return -1;
    }

    /// <summary>
    /// Marks <paramref name="count"/> sectors beginning at <paramref name="start"/> as free.
    /// </summary>
    public void Free(int start, int count)
    {
        for (int i = start; i < start + count; i++)
        {
            if (IsAllocated(i))
            {
                _bits[i >> 3] &= (byte)~(1 << (i & 7));
                _freeCount++;
            }
        }
    }

    /// <summary>Serializes the bitmap to a byte array.</summary>
    public byte[] ToBytes()
    {
        var copy = new byte[_bits.Length];
        Array.Copy(_bits, copy, _bits.Length);
        return copy;
    }

    /// <summary>
    /// Deserializes a bitmap from a byte array produced by <see cref="ToBytes"/>.
    /// </summary>
    public static NdiBam FromBytes(byte[] data, int totalSectors)
    {
        var bits = new byte[(totalSectors + 7) >> 3];
        Array.Copy(data, bits, Math.Min(data.Length, bits.Length));
        return new NdiBam(bits, totalSectors);
    }
}
