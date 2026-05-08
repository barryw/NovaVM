import Foundation

enum SpriteFormat {
    struct Decoded: Sendable {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var data = Data(count: 128)
        for y in 0..<16 {
            for x in stride(from: 0, to: 16, by: 2) {
                let left = (y < height && x < width) ? pixels[y * width + x] & 0x0F : 0
                let right = (y < height && x + 1 < width) ? pixels[y * width + x + 1] & 0x0F : 0
                data[y * 8 + x / 2] = left | (right << 4)
            }
        }
        return data
    }

    static func encodeSheet(pixels: [UInt8], width: Int, height: Int) -> Data {
        var data = Data()
        let cols = width / 16
        let rows = height / 16
        for row in 0..<rows {
            for col in 0..<cols {
                var sprite = [UInt8](repeating: 0, count: 16 * 16)
                for y in 0..<16 {
                    for x in 0..<16 {
                        let srcX = col * 16 + x
                        let srcY = row * 16 + y
                        if srcX < width && srcY < height {
                            sprite[y * 16 + x] = pixels[srcY * width + srcX]
                        }
                    }
                }
                data.append(encode(pixels: sprite, width: 16, height: 16))
            }
        }
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 128 else { return nil }
        let spriteCount = data.count / 128
        let cols = spriteCount == 1 ? 1 : Int(ceil(sqrt(Double(spriteCount))))
        let rows = spriteCount == 1 ? 1 : (spriteCount + cols - 1) / cols
        let width = cols * 16
        let height = rows * 16
        var pixels = [UInt8](repeating: 0, count: width * height)

        for s in 0..<spriteCount {
            let col = s % cols
            let row = s / cols
            let base = s * 128
            for y in 0..<16 {
                for x in stride(from: 0, to: 16, by: 2) {
                    let b = data[base + y * 8 + x / 2]
                    let dstX = col * 16 + x
                    let dstY = row * 16 + y
                    pixels[dstY * width + dstX] = b & 0x0F
                    pixels[dstY * width + dstX + 1] = (b >> 4) & 0x0F
                }
            }
        }
        return Decoded(width: width, height: height, pixels: pixels)
    }
}
