import AppKit
import SwiftUI
import PixelCanvasKit

/// NovaDraw's concrete palette. The colour *data* now lives in PixelCanvasKit
/// as the injectable `Palette.nova` value — this enum is a thin app-side shim
/// that preserves NovaDraw's existing call sites (`NovaPalette.swiftUIColor(_:)`,
/// `.colors`, `.names`) while delegating to the package. The engine no longer
/// hard-codes a palette; NovaDraw injects this one.
enum NovaPalette {
    /// The palette NovaDraw renders with.
    static let palette: Palette = .nova

    static let colors: [(r: UInt8, g: UInt8, b: UInt8)] =
        palette.colors.map { (r: $0.r, g: $0.g, b: $0.b) }

    static let names: [String] = palette.entryNames

    static func nsColor(_ index: Int) -> NSColor {
        palette.nsColor(index)
    }

    static func swiftUIColor(_ index: Int) -> Color {
        palette.swiftUIColor(index)
    }
}
