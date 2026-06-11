import Testing
import Foundation
@testable import NovaDraw

@Suite struct NvgFormatTests {
    @Test func roundTrip() {
        var pixels = [UInt8](repeating: 0, count: 320 * 200)
        for x in 10..<15 { pixels[x] = 7 }
        pixels[100 * 320 + 50] = 3

        let data = NvgFormat.encode(pixels: pixels, width: 320, height: 200)
        #expect(data[0] == 0x4E)
        #expect(data[1] == 0x56)
        #expect(data[2] == 0x47)
        #expect(data[3] == 0x32)
        #expect(data[8] == 0x02)
        #expect(data[10] == 64)
        #expect(data[11] == 0)

        let decoded = NvgFormat.decode(data: data)!
        #expect(decoded.width == 320)
        #expect(decoded.height == 200)
        #expect(decoded.pixels.count == 320 * 200)
        for x in 10..<15 { #expect(decoded.pixels[x] == 7) }
        #expect(decoded.pixels[100 * 320 + 50] == 3)
        #expect(decoded.pixels[0] == 0)
    }

    @Test func allBlackIsMinimal() {
        let pixels = [UInt8](repeating: 0, count: 320 * 200)
        let data = NvgFormat.encode(pixels: pixels, width: 320, height: 200)
        #expect(data.count == 64 + (320 * 200 / 2))
    }

    @Test func invalidMagicReturnsNil() {
        let data = Data([0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00])
        #expect(NvgFormat.decode(data: data) == nil)
    }

    @Test func truncatedDataReturnsNil() {
        #expect(NvgFormat.decode(data: Data([0x4E, 0x56])) == nil)
    }
}
