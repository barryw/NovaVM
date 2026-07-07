import Foundation
import Testing
@testable import NovaDraw

// Cross-implementation conformance: NovaDraw's NsprFormat must agree byte-for-byte
// with the shared golden fixture testdata/nspr/sample.nsp (authored by the C#
// codec). See docs/sprite-editor/NSPR.md for the spec + canonical sample.
//
// The fixture lives at the repo root (outside this Swift package), so it's
// resolved from this file's path. If that's brittle in your build, wire it as a
// NovaDrawTests package resource in Package.swift and load it from Bundle.module
// instead — the assertions below don't change.
@Suite struct NsprConformanceTests {
    private func fixtureData() throws -> Data {
        var root = URL(fileURLWithPath: #filePath)
        // .../NovaVM/NovaDraw/Tests/NsprConformanceTests.swift -> up 3 to NovaVM
        for _ in 0..<3 { root.deleteLastPathComponent() }
        let url = root.appendingPathComponent("testdata/nspr/sample.nsp")
        return try Data(contentsOf: url)
    }

    /// The canonical conformance sample (docs/sprite-editor/NSPR.md), mirroring
    /// e6502UnitTests/NsprConformanceTests.cs `CanonicalBank()`.
    private func canonicalBank() -> NsprBank {
        var bank = NsprBank()
        bank.version = 1
        bank.kind = .hwMetasprite
        bank.palette = (0..<NsprBank.paletteBytes).map { UInt8($0) }
        for i in 0..<5 {
            bank.shapes.append([UInt8](repeating: UInt8(0x10 + i), count: NsprBank.shapeBytes))
        }

        var hero = NsprCharacter(name: "HERO")
        hero.parts = [NsprPart(dx: 0, dy: 0, flags: 0)]
        var idle = NsprAnimation(name: "IDLE", ticks: 8, flags: .loop)
        idle.frames = [[0], [1], [2]]
        hero.animations = [idle]
        bank.characters.append(hero)

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

    @Test func goldenFixtureDecodesToCanonicalBank() throws {
        let bank = try NsprBank.decode(try fixtureData())
        #expect(bank.shapes.count == 5)
        #expect(bank.characters.count == 2)
        #expect(bank.characters[0].name == "HERO")
        #expect(bank.characters[0].parts.count == 1)
        #expect(bank.characters[0].animations[0].name == "IDLE")
        #expect(bank.characters[0].animations[0].frames.count == 3)
        #expect(bank.characters[1].name == "BOSS")
        #expect(bank.characters[1].parts.count == 4)
        #expect(bank.characters[1].parts[1].dx == 16)
        let walk = bank.characters[1].animations[0]
        #expect(walk.flags == [.loop, .pingPong])
        #expect(walk.frames[1] == [4, 3, 2, 1])
    }

    @Test func canonicalBankReencodesByteIdenticalToFixture() throws {
        let fixture = try fixtureData()
        #expect(try canonicalBank().encoded() == fixture)
    }
}
