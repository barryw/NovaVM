import Testing
import Foundation
@testable import NovaDraw

@Suite struct VSpriteFormatTests {
    @Test func roundTrip() {
        let pixels: [UInt8] = [1, 2, 3, 4, 5, 6]
        let data = VSpriteFormat.encode(pixels: pixels, width: 3, height: 2)
        #expect(data.count == 10)

        let decoded = VSpriteFormat.decode(data: data)!
        #expect(decoded.width == 3)
        #expect(decoded.height == 2)
        #expect(decoded.pixels == [1, 2, 3, 4, 5, 6])
    }

    @Test func headerEncoding() {
        let data = VSpriteFormat.encode(pixels: [0], width: 256, height: 100)
        #expect(data[0] == 0x00)
        #expect(data[1] == 0x01)  // 256 little-endian
        #expect(data[2] == 0x64)
        #expect(data[3] == 0x00)  // 100 little-endian
    }

    @Test func wrongSizeReturnsNil() {
        var data = Data()
        withUnsafeBytes(of: UInt16(3).littleEndian) { data.append(contentsOf: $0) }
        withUnsafeBytes(of: UInt16(2).littleEndian) { data.append(contentsOf: $0) }
        data.append(contentsOf: [1, 2, 3])  // only 3 pixels, need 6
        #expect(VSpriteFormat.decode(data: data) == nil)
    }

    @Test func tooSmallReturnsNil() {
        #expect(VSpriteFormat.decode(data: Data([0x01])) == nil)
    }
}
