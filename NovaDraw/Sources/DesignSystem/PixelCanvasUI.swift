//
//  PixelCanvasUI.swift
//  WHI shared pixel-editor token layer — Design System v1 (WAL-27 / WAL-29)
//
//  VENDORED, KEEP IN SYNC across NovaDraw / FamiForge / Miggy Draw (RetroDraw).
//  The ONLY line that differs between editors is `accent` (the per-product line
//  color). Everything else — neutrals, type, spacing, radius — is identical.
//  When PixelCanvasKit (WAL-20) lands, this file folds into the shared framework.
//
//  Source of truth: WHI Design System v1 (WAL-23/WAL-18) and the WAL-25 audit.
//  Per-product accent (WAL-29):
//    • NovaDraw  → cyan   #53e0d8
//    • FamiForge → amber  #f5b700
//    • Miggy Draw→ orange #ff8c00
//  Amiga line-red #e63030 is BANNED as a UI accent (fails WCAG AA on dark).
//

import SwiftUI

/// Shared design tokens for the WHI pixel editors.
enum PixelCanvasUI {

    // MARK: - Per-product accent (the one brand variable)

    /// Selection ring, active tool, focus outline, hover, active mode.
    /// Never the *only* signal (always pair with shape/position) and never body text.
    static let accent = hex(0x53e0d8) // NovaDraw — Nova cyan

    // MARK: - Surface / neutral tokens (shared, dark UI)

    enum Colors {
        static let bg          = hex(0x0d1117) // window background
        static let surface     = hex(0x161b22) // panels (left/right/status)
        static let surface2    = hex(0x1c222b) // raised controls, canvas chrome
        static let border      = hex(0x2a2f37) // dividers, swatch borders
        static let text        = hex(0xf3f0e6) // primary (warm off-white)
        static let textMuted   = hex(0xb9b4a6) // labels, secondary
        static let textFaint   = hex(0x7e7a70) // hints, disabled
        static let canvasField = hex(0x0d1117) // drawing area behind pixels
    }

    /// The per-product accent, exposed under Colors for call-site symmetry.
    static var accentColor: Color { accent }

    // MARK: - Typography
    //
    // Spec calls for Inter (UI) + JetBrains Mono (numeric/status). Until those
    // faces are bundled we resolve to the system UI font and the guaranteed
    // monospaced *design* (which preserves column alignment for coordinates and
    // hex), so the layout is correct on every machine and the build never
    // depends on a font file being present.

    enum Typography {
        /// Monospaced face for coordinates, hex, tile/color indices, status strip.
        static func mono(_ size: CGFloat, weight: Font.Weight = .regular) -> Font {
            .system(size: size, weight: weight, design: .monospaced)
        }

        /// UI face for labels and controls.
        static func ui(_ size: CGFloat, weight: Font.Weight = .regular) -> Font {
            .system(size: size, weight: weight)
        }

        // v1 status-strip size.
        static let statusSize: CGFloat = 11
    }

    // MARK: - Spacing (4-pt scale) & radius

    enum Spacing {
        static let xs: CGFloat = 4
        static let sm: CGFloat = 8
        static let md: CGFloat = 12
        static let lg: CGFloat = 16
        static let xl: CGFloat = 24
    }

    enum Radius {
        static let sm: CGFloat = 4
        static let md: CGFloat = 8
        static let lg: CGFloat = 10
    }

    // MARK: - Hex helper (local, to avoid colliding with per-app Color extensions)

    static func hex(_ value: UInt32) -> Color {
        Color(
            .sRGB,
            red: Double((value >> 16) & 0xFF) / 255.0,
            green: Double((value >> 8) & 0xFF) / 255.0,
            blue: Double(value & 0xFF) / 255.0,
            opacity: 1.0
        )
    }
}

// MARK: - Shared bottom status strip
//
// Consistent JetBrains-Mono (monospaced) strip across all three editors:
//   X,Y · tool · color index · zoom% · W×H
// Every field is optional so each editor shows what it has without a redesign.

struct PixelStatusStrip: View {
    var cursor: (x: Int, y: Int)?
    var tool: String?
    var colorIndex: Int?
    var zoomPercent: Int?
    var dimensions: (w: Int, h: Int)?

    var body: some View {
        HStack(spacing: PixelCanvasUI.Spacing.md) {
            field(icon: "scope", cursor.map { "\($0.x), \($0.y)" } ?? "—")

            if let tool, !tool.isEmpty {
                separator
                Text(tool)
            }

            if let colorIndex {
                separator
                Text("idx \(colorIndex)")
            }

            Spacer(minLength: PixelCanvasUI.Spacing.sm)

            if let zoomPercent {
                Text("\(zoomPercent)%")
            }

            if let dimensions {
                separator
                Text("\(dimensions.w)×\(dimensions.h)")
            }
        }
        .font(PixelCanvasUI.Typography.mono(PixelCanvasUI.Typography.statusSize))
        .foregroundStyle(PixelCanvasUI.Colors.textMuted)
        .lineLimit(1)
        .padding(.horizontal, PixelCanvasUI.Spacing.md)
        .padding(.vertical, PixelCanvasUI.Spacing.xs)
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(PixelCanvasUI.Colors.surface)
        .overlay(alignment: .top) {
            Rectangle()
                .fill(PixelCanvasUI.Colors.border)
                .frame(height: 1)
        }
    }

    private var separator: some View {
        Rectangle()
            .fill(PixelCanvasUI.Colors.border)
            .frame(width: 1, height: 11)
    }

    private func field(icon: String, _ value: String) -> some View {
        HStack(spacing: PixelCanvasUI.Spacing.xs) {
            Image(systemName: icon)
                .foregroundStyle(PixelCanvasUI.Colors.textFaint)
            Text(value)
        }
    }
}
