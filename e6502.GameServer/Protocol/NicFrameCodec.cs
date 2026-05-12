namespace e6502.GameServer.Protocol;

public static class NicFrameCodec
{
    public static async ValueTask<byte[]?> ReadFrameAsync(Stream stream, CancellationToken cancellationToken = default)
    {
        var header = new byte[1];
        int read = await stream.ReadAsync(header.AsMemory(0, 1), cancellationToken);
        if (read == 0)
            return null;

        int length = header[0] == 0 ? 256 : header[0];
        var payload = new byte[length];
        await ReadExactlyAsync(stream, payload, cancellationToken);
        return payload;
    }

    public static async ValueTask WriteFrameAsync(Stream stream, ReadOnlyMemory<byte> frame, CancellationToken cancellationToken = default)
    {
        if (frame.Length is <= 0 or > NovaGameProtocol.MaxFramePayload)
            throw new ArgumentOutOfRangeException(nameof(frame), "Nova NIC frames must be 1-256 bytes.");

        byte length = frame.Length == 256 ? (byte)0 : (byte)frame.Length;
        await stream.WriteAsync(new[] { length }, cancellationToken);
        await stream.WriteAsync(frame, cancellationToken);
        await stream.FlushAsync(cancellationToken);
    }

    private static async ValueTask ReadExactlyAsync(Stream stream, Memory<byte> buffer, CancellationToken cancellationToken)
    {
        int offset = 0;
        while (offset < buffer.Length)
        {
            int read = await stream.ReadAsync(buffer[offset..], cancellationToken);
            if (read == 0)
                throw new EndOfStreamException("Connection closed in the middle of a Nova NIC frame.");

            offset += read;
        }
    }
}
