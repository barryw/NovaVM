import Foundation
import Testing
@testable import NovaDraw

@Suite struct NsprFormatTests {
    private func sampleBank() -> NsprBank {
        var bank = NsprBank()
        bank.palette = (0..<NsprBank.paletteBytes).map { UInt8($0) }
        for i in 0..<5 {
            bank.shapes.append([UInt8](repeating: UInt8(0x10 + i), count: NsprBank.shapeBytes))
        }

        // A plain 1-part (16x16) sprite with a 3-frame looping idle.
        var hero = NsprCharacter(name: "HERO")
        hero.parts = [NsprPart(dx: 0, dy: 0, flags: 0)]
        var idle = NsprAnimation(name: "IDLE", ticks: 8, flags: .loop)
        idle.frames = [[0], [1], [2]]
        hero.animations = [idle]
        bank.characters.append(hero)

        // A 2x2 (4-part, 32x32) metasprite with a 2-frame ping-pong walk.
        var boss = NsprCharacter(name: "BOSS")
        boss.parts = [
            NsprPart(dx: 0, dy: 0, flags: 0),
            NsprPart(dx: 16, dy: 0, flags: 0),
            NsprPart(dx: 0, dy: 16, flags: 0),
            NsprPart(dx: 16, dy: 16, flags: 1),
        ]
        var walk = NsprAnimation(name: "WALK", ticks: 4, flags: [.loop, .pingPong])
        walk.frames = [[0, 1, 2, 3], [4, 3, 2, 1]]
        boss.animations = [walk]
        bank.characters.append(boss)
        return bank
    }

    @Test func roundTrips() throws {
        let a = sampleBank()
        let b = try NsprBank.decode(try a.encoded())
        #expect(a == b)
    }

    @Test func headerByteLayout() throws {
        let d = try sampleBank().encoded()
        #expect(Array(d.prefix(4)) == Array("NSPR".utf8))
        #expect(d[4] == 1)          // version
        #expect(d[5] == 0)          // kind = hw metasprite
        #expect(d[6] & 0x01 == 1)   // palette present
        #expect(d[7] == 5)          // shape count
        #expect(d[8] == 2)          // character count
        #expect(d[9] == 16)         // cell w
        #expect(d[10] == 16)        // cell h
    }

    @Test func rejectsBadMagic() throws {
        var d = try sampleBank().encoded()
        d[0] = UInt8(ascii: "X")
        #expect(throws: NsprBank.NsprError.badMagic) { _ = try NsprBank.decode(d) }
    }

    @Test func validatesFramePartCount() {
        var bank = NsprBank()
        bank.shapes = [[UInt8](repeating: 0, count: NsprBank.shapeBytes)]
        var ch = NsprCharacter(name: "X")
        ch.parts = [NsprPart(), NsprPart()]   // 2 parts
        var an = NsprAnimation(name: "A")
        an.frames = [[0]]                      // only 1 shape index for 2 parts
        ch.animations = [an]
        bank.characters = [ch]
        #expect(throws: NsprBank.NsprError.self) { _ = try bank.encoded() }
    }
}
