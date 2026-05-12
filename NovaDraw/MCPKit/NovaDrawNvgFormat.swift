import Foundation

public enum NovaDrawNvgFormat {
    public static func encode(pixels: [UInt8], paintedPixels: [UInt8], width: Int, height: Int) -> Data {
        var spans: [(addr: UInt16, data: [UInt8])] = []
        for y in 0..<height {
            var x = 0
            let row = y * width
            while x < width {
                if paintedPixels[row + x] == 0 || pixels[row + x] == 0 {
                    x += 1
                    continue
                }
                let start = x
                var run: [UInt8] = []
                while x < width,
                      paintedPixels[row + x] != 0,
                      pixels[row + x] != 0,
                      run.count < 255 {
                    run.append(pixels[row + x])
                    x += 1
                }
                spans.append((UInt16(row + start), run))
            }
        }

        var data = Data()
        data.append(contentsOf: [0x4E, 0x56, 0x47, 0x31])
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
}
