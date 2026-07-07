import Foundation

// NSPR ("Nova SPRite bank", .nsp) — a metasprite animation sheet.
//
// One file holds many characters; each character is a metasprite (a fixed
// layout of 16x16 hardware-sprite `parts`) with named `animations`. Animation
// is a per-frame shape-index swap per part, matching the on-device runtime
// (msprite.s parts + anim.s frame tables). A plain 16x16 sprite is a 1-part
// metasprite; `kind` reserves room for vsprites, so the format never needs a
// v1->v2 break. This is a byte-for-byte port of e6502.Storage/NsprBank.cs — the
// two MUST stay in sync.
//
// Shapes are packed 16x16 4bpp, 128 bytes each, low-nibble = left pixel — the
// same packing as `SpriteFormat.encode`, so use SpriteFormat to convert between
// a bank's shape bytes and editable [UInt8] pixels.
//
// Layout (little-endian, counts are u8):
//   Header(16): "NSPR" · version · kind · flags(bit0=palette) · shapeCount ·
//               charCount · cellW(16) · cellH(16) · reserved[5]
//   [palette 48B if flag]  16 x {R,G,B}
//   shapes: shapeCount x 128B
//   characters x charCount { name[8] · partCount · parts{dx,dy,flags} ·
//     animCount · animations{ name[8] · frameCount · ticks · flags ·
//       frames x (partCount x shapeIndex) } }

enum NsprKind: UInt8, Sendable {
    case hwMetasprite = 0
    case vsprite = 1
}

struct NsprAnimFlags: OptionSet, Sendable {
    let rawValue: UInt8
    static let loop = NsprAnimFlags(rawValue: 0x01)
    static let pingPong = NsprAnimFlags(rawValue: 0x02)
}

struct NsprPart: Sendable, Equatable {
    var dx: UInt8 = 0
    var dy: UInt8 = 0
    var flags: UInt8 = 0
}

struct NsprAnimation: Sendable, Equatable {
    var name: String = ""
    var ticks: UInt8 = 6
    var flags: NsprAnimFlags = .loop
    /// Each frame is one shape index per part (count == owning character's part count).
    var frames: [[UInt8]] = []
}

struct NsprCharacter: Sendable, Equatable {
    var name: String = ""
    var parts: [NsprPart] = []
    var animations: [NsprAnimation] = []
}

struct NsprBank: Sendable, Equatable {
    static let shapeBytes = 128       // 16x16, 4bpp, 2px/byte
    static let cellW: UInt8 = 16
    static let cellH: UInt8 = 16
    static let nameLen = 8
    static let paletteBytes = 48      // 16 x RGB
    static let magic: [UInt8] = Array("NSPR".utf8)

    var version: UInt8 = 1
    var kind: NsprKind = .hwMetasprite
    /// 16xRGB (48 bytes) if the intended palette is embedded, else nil.
    var palette: [UInt8]? = nil
    /// Shared pool of 128-byte shapes; animations reference these by index.
    var shapes: [[UInt8]] = []
    var characters: [NsprCharacter] = []

    enum NsprError: Error, Equatable {
        case badMagic
        case truncated
        case invalid(String)
    }

    // MARK: Encode

    func encoded() throws -> Data {
        try validate()
        var d = Data()
        d.append(contentsOf: Self.magic)
        d.append(version)
        d.append(kind.rawValue)
        d.append(palette != nil ? 0x01 : 0x00)
        d.append(UInt8(shapes.count))
        d.append(UInt8(characters.count))
        d.append(Self.cellW)
        d.append(Self.cellH)
        d.append(contentsOf: [UInt8](repeating: 0, count: 5))   // reserved
        if let pal = palette { d.append(contentsOf: pal) }
        for shape in shapes { d.append(contentsOf: shape) }

        for ch in characters {
            appendName(&d, ch.name)
            d.append(UInt8(ch.parts.count))
            for p in ch.parts { d.append(p.dx); d.append(p.dy); d.append(p.flags) }
            d.append(UInt8(ch.animations.count))
            for a in ch.animations {
                appendName(&d, a.name)
                d.append(UInt8(a.frames.count))
                d.append(a.ticks)
                d.append(a.flags.rawValue)
                for frame in a.frames { d.append(contentsOf: frame) }
            }
        }
        return d
    }

    // MARK: Decode

    static func decode(_ data: Data) throws -> NsprBank {
        var r = Reader(data)
        let header = try r.take(16)
        guard Array(header.prefix(4)) == magic else { throw NsprError.badMagic }
        var bank = NsprBank()
        bank.version = header[4]
        bank.kind = NsprKind(rawValue: header[5]) ?? .hwMetasprite
        let hasPalette = (header[6] & 0x01) != 0
        let shapeCount = Int(header[7])
        let charCount = Int(header[8])
        // header[9]/[10] = cellW/cellH (fixed 16x16 for v1); reserved ignored.

        if hasPalette { bank.palette = Array(try r.take(paletteBytes)) }
        for _ in 0..<shapeCount { bank.shapes.append(Array(try r.take(shapeBytes))) }

        for _ in 0..<charCount {
            var ch = NsprCharacter(name: try r.takeName())
            let partCount = Int(try r.takeByte())
            for _ in 0..<partCount {
                let p = try r.take(3)
                ch.parts.append(NsprPart(dx: p[0], dy: p[1], flags: p[2]))
            }
            let animCount = Int(try r.takeByte())
            for _ in 0..<animCount {
                var a = NsprAnimation(name: try r.takeName())
                let frameCount = Int(try r.takeByte())
                a.ticks = try r.takeByte()
                a.flags = NsprAnimFlags(rawValue: try r.takeByte())
                for _ in 0..<frameCount { a.frames.append(Array(try r.take(partCount))) }
                ch.animations.append(a)
            }
            bank.characters.append(ch)
        }
        return bank
    }

    // MARK: Validate

    func validate() throws {
        if shapes.count > 255 { throw NsprError.invalid("shape count > 255") }
        if characters.count > 255 { throw NsprError.invalid("character count > 255") }
        if let pal = palette, pal.count != Self.paletteBytes {
            throw NsprError.invalid("palette must be \(Self.paletteBytes) bytes")
        }
        for sh in shapes where sh.count != Self.shapeBytes {
            throw NsprError.invalid("shape must be \(Self.shapeBytes) bytes")
        }
        for ch in characters {
            if ch.parts.count > 255 || ch.animations.count > 255 {
                throw NsprError.invalid("part/anim count > 255")
            }
            for a in ch.animations {
                if a.frames.count > 255 { throw NsprError.invalid("frame count > 255") }
                for frame in a.frames {
                    if frame.count != ch.parts.count {
                        throw NsprError.invalid("frame must hold one shape index per part")
                    }
                    for idx in frame where Int(idx) >= shapes.count {
                        throw NsprError.invalid("frame references shape \(idx) (only \(shapes.count) shapes)")
                    }
                }
            }
        }
    }

    // MARK: Helpers

    private func appendName(_ d: inout Data, _ name: String) {
        var buf = [UInt8](repeating: 0, count: Self.nameLen)
        for (i, b) in name.utf8.prefix(Self.nameLen).enumerated() { buf[i] = b }
        d.append(contentsOf: buf)
    }

    private struct Reader {
        let data: Data
        var pos: Int = 0
        init(_ d: Data) { data = d }

        mutating func take(_ n: Int) throws -> [UInt8] {
            guard pos + n <= data.count else { throw NsprError.truncated }
            let start = data.startIndex + pos
            let slice = Array(data[start..<(start + n)])
            pos += n
            return slice
        }

        mutating func takeByte() throws -> UInt8 { try take(1)[0] }

        mutating func takeName() throws -> String {
            let buf = try take(nameLen)
            let len = buf.firstIndex(of: 0) ?? buf.count
            return String(decoding: buf[0..<len], as: UTF8.self)
        }
    }
}
