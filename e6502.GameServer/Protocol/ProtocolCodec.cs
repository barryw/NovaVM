using System.Text;

namespace e6502.GameServer.Protocol;

public sealed class ProtocolReader
{
    private readonly ReadOnlyMemory<byte> _data;
    private int _offset;

    public ProtocolReader(ReadOnlyMemory<byte> data)
    {
        _data = data;
    }

    public int Remaining => _data.Length - _offset;

    public bool TryReadByte(out byte value)
    {
        if (Remaining < 1)
        {
            value = 0;
            return false;
        }

        value = _data.Span[_offset++];
        return true;
    }

    public bool TryReadUInt16(out ushort value)
    {
        if (Remaining < 2)
        {
            value = 0;
            return false;
        }

        var span = _data.Span;
        value = (ushort)(span[_offset] | (span[_offset + 1] << 8));
        _offset += 2;
        return true;
    }

    public bool TryReadString(out string value)
    {
        value = string.Empty;
        if (!TryReadByte(out byte length) || Remaining < length)
            return false;

        value = Encoding.UTF8.GetString(_data.Span.Slice(_offset, length));
        _offset += length;
        return true;
    }

    public ReadOnlyMemory<byte> ReadRemaining()
    {
        var remaining = _data.Slice(_offset);
        _offset = _data.Length;
        return remaining;
    }
}

public sealed class ProtocolWriter
{
    private readonly byte[] _buffer = new byte[NovaGameProtocol.MaxMessagePayload];
    private int _length;

    public int Length => _length;

    public void WriteByte(byte value)
    {
        EnsureCapacity(1);
        _buffer[_length++] = value;
    }

    public void WriteUInt16(ushort value)
    {
        EnsureCapacity(2);
        _buffer[_length++] = (byte)(value & 0xFF);
        _buffer[_length++] = (byte)(value >> 8);
    }

    public void WriteBytes(ReadOnlySpan<byte> value)
    {
        EnsureCapacity(value.Length);
        value.CopyTo(_buffer.AsSpan(_length));
        _length += value.Length;
    }

    public void WriteString(string value)
    {
        int byteCount = Encoding.UTF8.GetByteCount(value);
        if (byteCount > byte.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(value), "String is too long for protocol encoding.");

        EnsureCapacity(1 + byteCount);
        _buffer[_length++] = (byte)byteCount;
        Encoding.UTF8.GetBytes(value, _buffer.AsSpan(_length, byteCount));
        _length += byteCount;
    }

    public byte[] ToArray() => _buffer.AsSpan(0, _length).ToArray();

    private void EnsureCapacity(int byteCount)
    {
        if (_length + byteCount > _buffer.Length)
            throw new InvalidOperationException("Message payload exceeds one Nova NIC frame.");
    }
}

public static class ProtocolCodec
{
    public static byte[] Encode(ProtocolMessage message)
    {
        if (message.Payload.Length > NovaGameProtocol.MaxMessagePayload)
            throw new ArgumentOutOfRangeException(nameof(message), "Payload is too large for one Nova NIC frame.");

        var frame = new byte[NovaGameProtocol.EnvelopeSize + message.Payload.Length];
        frame[0] = message.Version;
        frame[1] = (byte)message.Kind;
        frame[2] = message.Sequence;
        frame[3] = (byte)message.Flags;
        message.Payload.CopyTo(frame, NovaGameProtocol.EnvelopeSize);
        return frame;
    }

    public static ProtocolMessage Decode(ReadOnlySpan<byte> frame)
    {
        if (frame.Length < NovaGameProtocol.EnvelopeSize)
            throw new ProtocolException("Frame is shorter than the fixed envelope.");

        var payload = frame[NovaGameProtocol.EnvelopeSize..].ToArray();
        return new ProtocolMessage(
            frame[0],
            (MessageKind)frame[1],
            frame[2],
            (MessageFlags)frame[3],
            payload);
    }
}

public static class ProtocolMessages
{
    public static ProtocolMessage Error(byte sequence, ProtocolErrorCode code, string text)
    {
        var writer = new ProtocolWriter();
        writer.WriteByte((byte)code);
        writer.WriteString(text);
        return ProtocolMessage.Create(
            MessageKind.Error,
            sequence,
            MessageFlags.Response | MessageFlags.Error,
            writer.ToArray());
    }
}

public sealed class ProtocolException : Exception
{
    public ProtocolException(string message) : base(message)
    {
    }
}
