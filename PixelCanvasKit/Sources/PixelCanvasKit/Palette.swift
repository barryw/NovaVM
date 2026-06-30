import Foundation
#if canImport(AppKit)
import AppKit
#endif
#if canImport(SwiftUI)
import SwiftUI
#endif

/// A single RGB entry in a ``Palette``.
public struct PaletteColor: Equatable, Sendable {
    public let r: UInt8
    public let g: UInt8
    public let b: UInt8

    public init(_ r: UInt8, _ g: UInt8, _ b: UInt8) {
        self.r = r
        self.g = g
        self.b = b
    }
}

/// An injected, named colour table that maps 4-bit (or wider) pixel indices to
/// RGB. This is the seam that replaces NovaDraw's hard-coded `NovaPalette`:
/// every consumer supplies the palette its target hardware uses (C64-ish 16,
/// NES, Amiga, …) instead of the engine baking one in.
///
/// Index handling wraps modulo ``count`` so a stored 4-bit index is always
/// valid; for a 16-entry palette this matches the historical `index & 0x0F`.
public struct Palette: Sendable {
    public let name: String
    public let colors: [PaletteColor]
    public let entryNames: [String]

    public init(name: String, colors: [PaletteColor], entryNames: [String] = []) {
        self.name = name
        self.colors = colors
        self.entryNames = entryNames
    }

    public var count: Int { colors.count }

    /// The colour for `index`, wrapped into range. Empty palettes return black.
    public func color(at index: Int) -> PaletteColor {
        guard count > 0 else { return PaletteColor(0, 0, 0) }
        let wrapped = ((index % count) + count) % count
        return colors[wrapped]
    }

    /// The human-readable name for `index`, or a generated fallback.
    public func name(at index: Int) -> String {
        guard count > 0 else { return "—" }
        let wrapped = ((index % count) + count) % count
        return wrapped < entryNames.count ? entryNames[wrapped] : "Color \(wrapped)"
    }

    /// Nearest palette index to an RGB triple in `0...1`, using a luma-weighted
    /// squared-distance metric. A deterministic, palette-agnostic default;
    /// consumers needing target-specific behaviour (e.g. NovaDraw's hue
    /// heuristic) can match against `colors` themselves.
    public func nearestIndex(r: Double, g: Double, b: Double) -> UInt8 {
        guard count > 0 else { return 0 }
        var bestIndex = 0
        var bestDistance = Double.greatestFiniteMagnitude
        for (i, color) in colors.enumerated() {
            let dr = r - Double(color.r) / 255
            let dg = g - Double(color.g) / 255
            let db = b - Double(color.b) / 255
            // Weight by luma coefficients so perceived brightness dominates.
            let distance = 0.2126 * dr * dr + 0.7152 * dg * dg + 0.0722 * db * db
            if distance < bestDistance {
                bestDistance = distance
                bestIndex = i
            }
        }
        return UInt8(bestIndex & 0xFF)
    }

    #if canImport(AppKit)
    /// AppKit colour for `index` (opaque).
    public func nsColor(_ index: Int) -> NSColor {
        let c = color(at: index)
        return NSColor(red: CGFloat(c.r) / 255, green: CGFloat(c.g) / 255,
                       blue: CGFloat(c.b) / 255, alpha: 1)
    }
    #endif

    #if canImport(SwiftUI)
    /// SwiftUI colour for `index`.
    public func swiftUIColor(_ index: Int) -> Color {
        let c = color(at: index)
        return Color(red: Double(c.r) / 255, green: Double(c.g) / 255,
                     blue: Double(c.b) / 255)
    }
    #endif
}

public extension Palette {
    /// The 16-colour C64-derived palette NovaDraw ships with — previously the
    /// hard-coded `NovaPalette`. Provided as a convenience/default; it is data,
    /// not a baked-in assumption of the engine.
    static let nova = Palette(
        name: "Nova 16",
        colors: [
            PaletteColor(0,   0,   0),    // 0  Black
            PaletteColor(255, 255, 255),  // 1  White
            PaletteColor(136, 0,   0),    // 2  Red
            PaletteColor(170, 255, 238),  // 3  Cyan
            PaletteColor(204, 68,  204),  // 4  Purple
            PaletteColor(0,   204, 85),   // 5  Green
            PaletteColor(0,   0,   170),  // 6  Blue
            PaletteColor(238, 238, 119),  // 7  Yellow
            PaletteColor(221, 136, 85),   // 8  Orange
            PaletteColor(102, 68,  0),    // 9  Brown
            PaletteColor(255, 119, 119),  // 10 Light Red
            PaletteColor(51,  51,  51),   // 11 Dark Grey
            PaletteColor(119, 119, 119),  // 12 Med Grey
            PaletteColor(170, 255, 102),  // 13 Light Green
            PaletteColor(0,   136, 255),  // 14 Light Blue
            PaletteColor(187, 187, 187),  // 15 Light Grey
        ],
        entryNames: [
            "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue", "Yellow",
            "Orange", "Brown", "Light Red", "Dark Grey", "Med Grey", "Light Green",
            "Light Blue", "Light Grey",
        ]
    )
}
