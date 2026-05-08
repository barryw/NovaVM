import Foundation

enum GfxFormat {
    static func encode(pixels: [UInt8]) -> Data {
        Data(pixels)
    }

    static func decode(data: Data, width: Int, height: Int) -> [UInt8]? {
        guard data.count == width * height else { return nil }
        return Array(data)
    }
}
