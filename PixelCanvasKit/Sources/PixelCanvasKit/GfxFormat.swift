import Foundation

/// Raw, palette-neutral byte codec: one byte per pixel (the 4-bit index in the
/// low nibble). The simplest container — no header, no embedded palette. Used
/// for `.gfx` blobs and as a building block for richer ``DocumentCodec``s.
public enum GfxFormat {
    public static func encode(pixels: [UInt8]) -> Data {
        Data(pixels)
    }

    public static func decode(data: Data, width: Int, height: Int) -> [UInt8]? {
        guard data.count == width * height else { return nil }
        return Array(data)
    }
}
