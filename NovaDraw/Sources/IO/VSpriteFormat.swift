import Foundation

enum VSpriteFormat {
    struct Decoded: Sendable {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var data = Data()
        withUnsafeBytes(of: UInt16(width).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt16(height).littleEndian) { data.append(contentsOf: $0) }
        data.append(contentsOf: pixels)
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 4 else { return nil }
        let width = Int(data[0]) | (Int(data[1]) << 8)
        let height = Int(data[2]) | (Int(data[3]) << 8)
        guard data.count == 4 + width * height else { return nil }
        return Decoded(width: width, height: height, pixels: Array(data[4...]))
    }
}
