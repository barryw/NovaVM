import Foundation

enum NvgFormat {
    struct Decoded: Sendable {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var spans: [(addr: UInt16, data: [UInt8])] = []
        for y in 0..<height {
            var x = 0
            let row = y * width
            while x < width {
                if pixels[row + x] == 0 { x += 1; continue }
                let start = x
                var run: [UInt8] = []
                while x < width && pixels[row + x] != 0 && run.count < 255 {
                    run.append(pixels[row + x])
                    x += 1
                }
                spans.append((UInt16(row + start), run))
            }
        }

        var data = Data()
        data.append(contentsOf: [0x4E, 0x56, 0x47, 0x31]) // "NVG1"
        withUnsafeBytes(of: UInt16(width).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt16(height).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt32(spans.count).littleEndian) { data.append(contentsOf: $0) }
        for span in spans {
            withUnsafeBytes(of: span.addr.littleEndian) { data.append(contentsOf: $0) }
            data.append(UInt8(span.data.count))
            data.append(contentsOf: span.data)
        }
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 12,
              data[0] == 0x4E, data[1] == 0x56, data[2] == 0x47, data[3] == 0x31
        else { return nil }

        let width = Int(data[4]) | (Int(data[5]) << 8)
        let height = Int(data[6]) | (Int(data[7]) << 8)
        let spanCount = Int(data[8]) | (Int(data[9]) << 8) | (Int(data[10]) << 16) | (Int(data[11]) << 24)

        var pixels = [UInt8](repeating: 0, count: width * height)
        var offset = 12
        for _ in 0..<spanCount {
            guard offset + 3 <= data.count else { return nil }
            let addr = Int(data[offset]) | (Int(data[offset + 1]) << 8)
            let len = Int(data[offset + 2])
            offset += 3
            guard offset + len <= data.count else { return nil }
            for i in 0..<len {
                let idx = addr + i
                if idx < pixels.count { pixels[idx] = data[offset + i] }
            }
            offset += len
        }
        return Decoded(width: width, height: height, pixels: pixels)
    }
}
