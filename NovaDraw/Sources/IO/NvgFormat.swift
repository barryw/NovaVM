import Foundation

enum NvgFormat {
    struct Decoded: Sendable {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    private static let c64Palette: [(UInt8, UInt8, UInt8)] = [
        (0, 0, 0), (255, 255, 255), (136, 0, 0), (170, 255, 238),
        (204, 68, 204), (0, 204, 85), (0, 0, 170), (238, 238, 119),
        (221, 136, 85), (102, 68, 0), (255, 119, 119), (51, 51, 51),
        (119, 119, 119), (170, 255, 102), (0, 136, 255), (187, 187, 187),
    ]

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var packed: [UInt8] = []
        packed.reserveCapacity(((width + 1) / 2) * height)
        for y in 0..<height {
            let row = y * width
            var x = 0
            while x < width {
                let left = pixels[row + x] & 0x0F
                let right = x + 1 < width ? pixels[row + x + 1] & 0x0F : 0
                packed.append((left << 4) | right)
                x += 2
            }
        }

        var data = Data()
        data.append(contentsOf: [0x4E, 0x56, 0x47, 0x32]) // "NVG2"
        withUnsafeBytes(of: UInt16(width).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt16(height).littleEndian) { data.append(contentsOf: $0) }
        data.append(0x02) // embedded RGB palette
        data.append(0xFF) // no transparent color key
        withUnsafeBytes(of: UInt16(64).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt32(packed.count).littleEndian) { data.append(contentsOf: $0) }
        for color in c64Palette {
            data.append(color.0)
            data.append(color.1)
            data.append(color.2)
        }
        data.append(contentsOf: packed)
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 16,
              data[0] == 0x4E, data[1] == 0x56, data[2] == 0x47, data[3] == 0x32
        else { return nil }

        let width = Int(data[4]) | (Int(data[5]) << 8)
        let height = Int(data[6]) | (Int(data[7]) << 8)
        let payloadOffset = Int(data[10]) | (Int(data[11]) << 8)
        let payloadLength = Int(data[12]) | (Int(data[13]) << 8) | (Int(data[14]) << 16) | (Int(data[15]) << 24)
        guard width > 0, height > 0, payloadOffset >= 16, payloadOffset + payloadLength <= data.count else {
            return nil
        }

        var pixels = [UInt8](repeating: 0, count: width * height)
        var offset = payloadOffset
        for y in 0..<height {
            let row = y * width
            var x = 0
            while x < width {
                guard offset < payloadOffset + payloadLength else { return nil }
                let byte = data[offset]
                offset += 1
                pixels[row + x] = byte >> 4
                if x + 1 < width {
                    pixels[row + x + 1] = byte & 0x0F
                }
                x += 2
            }
        }
        return Decoded(width: width, height: height, pixels: pixels)
    }
}
