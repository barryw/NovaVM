import AppKit
import SwiftUI

enum NovaPalette {
    static let colors: [(r: UInt8, g: UInt8, b: UInt8)] = [
        (0,   0,   0),      // 0  Black
        (255, 255, 255),     // 1  White
        (136, 0,   0),       // 2  Red
        (170, 255, 238),     // 3  Cyan
        (204, 68,  204),     // 4  Purple
        (0,   204, 85),      // 5  Green
        (0,   0,   170),     // 6  Blue
        (238, 238, 119),     // 7  Yellow
        (221, 136, 85),      // 8  Orange
        (102, 68,  0),       // 9  Brown
        (255, 119, 119),     // 10 Light Red
        (51,  51,  51),      // 11 Dark Grey
        (119, 119, 119),     // 12 Med Grey
        (170, 255, 102),     // 13 Light Green
        (0,   136, 255),     // 14 Light Blue
        (187, 187, 187),     // 15 Light Grey
    ]

    static let names: [String] = [
        "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue", "Yellow",
        "Orange", "Brown", "Light Red", "Dark Grey", "Med Grey", "Light Green",
        "Light Blue", "Light Grey"
    ]

    static func nsColor(_ index: Int) -> NSColor {
        let c = colors[index & 0x0F]
        return NSColor(red: CGFloat(c.r) / 255, green: CGFloat(c.g) / 255,
                       blue: CGFloat(c.b) / 255, alpha: 1)
    }

    static func swiftUIColor(_ index: Int) -> Color {
        let c = colors[index & 0x0F]
        return Color(red: Double(c.r) / 255, green: Double(c.g) / 255,
                     blue: Double(c.b) / 255)
    }
}
