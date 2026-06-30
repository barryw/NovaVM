import Testing
import Foundation
@testable import PixelCanvasKit

@Suite struct GfxFormatTests {
    @Test func roundTrip() {
        var pixels = [UInt8](repeating: 0, count: 320 * 200)
        pixels[0] = 5; pixels[63999] = 15
        let data = GfxFormat.encode(pixels: pixels)
        #expect(data.count == 64000)
        let decoded = GfxFormat.decode(data: data, width: 320, height: 200)!
        #expect(decoded[0] == 5)
        #expect(decoded[63999] == 15)
    }

    @Test func wrongSizeReturnsNil() {
        let data = Data([1, 2, 3])
        #expect(GfxFormat.decode(data: data, width: 320, height: 200) == nil)
    }
}
