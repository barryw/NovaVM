import Testing
import Foundation
@testable import NovaDraw

@Suite struct SpriteFormatTests {
    @Test func singleSprite() {
        var pixels = [UInt8](repeating: 0, count: 16 * 16)
        pixels[0] = 1
        pixels[1] = 2
        pixels[16] = 5

        let data = SpriteFormat.encode(pixels: pixels, width: 16, height: 16)
        #expect(data.count == 128)
        #expect(data[0] == 0x21)  // low=1, high=2
        #expect(data[8] == 0x05)  // row 1, pixel 0 = 5

        let decoded = SpriteFormat.decode(data: data)!
        #expect(decoded.width == 16)
        #expect(decoded.height == 16)
        #expect(decoded.pixels[0] == 1)
        #expect(decoded.pixels[1] == 2)
        #expect(decoded.pixels[16] == 5)
    }

    @Test func spriteSheet() {
        var pixels = [UInt8](repeating: 0, count: 32 * 32)
        pixels[0] = 1
        pixels[16] = 2
        pixels[32 * 16] = 3
        pixels[32 * 16 + 16] = 4

        let data = SpriteFormat.encodeSheet(pixels: pixels, width: 32, height: 32)
        #expect(data.count == 128 * 4)
    }

    @Test func tooSmallReturnsNil() {
        #expect(SpriteFormat.decode(data: Data([0x00])) == nil)
    }

    @Test func nibblePacking() {
        var pixels = [UInt8](repeating: 0, count: 16 * 16)
        pixels[0] = 0x0A  // left pixel
        pixels[1] = 0x0B  // right pixel
        let data = SpriteFormat.encode(pixels: pixels, width: 16, height: 16)
        #expect(data[0] == 0xBA)  // low nibble=A, high nibble=B
    }
}
