#!/usr/bin/env swift
import AppKit

let palette: [(UInt8, UInt8, UInt8)] = [
    (0,0,0), (255,255,255), (136,0,0), (170,255,238),
    (204,68,204), (0,204,85), (0,0,170), (238,238,119),
    (221,136,85), (102,68,0), (255,119,119), (51,51,51),
    (119,119,119), (170,255,102), (0,136,255), (187,187,187),
]

func makeIcon(size: Int) -> NSImage {
    let img = NSImage(size: NSSize(width: size, height: size))
    img.lockFocus()
    let ctx = NSGraphicsContext.current!.cgContext

    // Background — dark grey rounded rect
    let bg = NSColor(red: 0.15, green: 0.15, blue: 0.17, alpha: 1)
    bg.setFill()
    let path = NSBezierPath(roundedRect: NSRect(x: 0, y: 0, width: size, height: size),
                            xRadius: CGFloat(size) / 5, yRadius: CGFloat(size) / 5)
    path.fill()

    // Draw a 4x4 pixel grid of Nova colors in the center
    let gridSize = 4
    let margin = CGFloat(size) * 0.18
    let cellSize = (CGFloat(size) - margin * 2) / CGFloat(gridSize)
    let gap = cellSize * 0.08
    let colorOrder = [6, 14, 3, 1, 2, 5, 7, 4, 8, 10, 13, 15, 9, 11, 12, 0]

    for row in 0..<gridSize {
        for col in 0..<gridSize {
            let i = colorOrder[row * gridSize + col]
            let c = palette[i]
            let color = NSColor(red: CGFloat(c.0)/255, green: CGFloat(c.1)/255, blue: CGFloat(c.2)/255, alpha: 1)
            color.setFill()
            let x = margin + CGFloat(col) * cellSize + gap
            let y = margin + CGFloat(gridSize - 1 - row) * cellSize + gap
            let rect = NSRect(x: x, y: y, width: cellSize - gap * 2, height: cellSize - gap * 2)
            let cell = NSBezierPath(roundedRect: rect, xRadius: gap * 2, yRadius: gap * 2)
            cell.fill()
        }
    }

    // Grid lines overlay
    NSColor(white: 1.0, alpha: 0.15).setStroke()
    for i in 1..<gridSize {
        let p = margin + CGFloat(i) * cellSize
        let line = NSBezierPath()
        line.lineWidth = max(1, CGFloat(size) / 256)
        line.move(to: NSPoint(x: p, y: margin))
        line.line(to: NSPoint(x: p, y: CGFloat(size) - margin))
        line.stroke()
        let hline = NSBezierPath()
        hline.lineWidth = max(1, CGFloat(size) / 256)
        hline.move(to: NSPoint(x: margin, y: p))
        hline.line(to: NSPoint(x: CGFloat(size) - margin, y: p))
        hline.stroke()
    }

    // Pencil accent — small diagonal line in bottom-right
    let pencilColor = NSColor(red: 0, green: 0.53, blue: 1.0, alpha: 0.9)
    pencilColor.setStroke()
    let pl = NSBezierPath()
    pl.lineWidth = max(2, CGFloat(size) / 100)
    pl.lineCapStyle = .round
    let px = CGFloat(size) * 0.72
    let py = CGFloat(size) * 0.18
    pl.move(to: NSPoint(x: px, y: py))
    pl.line(to: NSPoint(x: px + CGFloat(size) * 0.12, y: py + CGFloat(size) * 0.12))
    pl.stroke()

    img.unlockFocus()
    return img
}

let sizes = [16, 32, 64, 128, 256, 512, 1024]
let iconDir = "AppIcon.iconset"

try? FileManager.default.createDirectory(atPath: iconDir, withIntermediateDirectories: true)

for s in sizes {
    let img = makeIcon(size: s)
    guard let tiff = img.tiffRepresentation,
          let rep = NSBitmapImageRep(data: tiff),
          let png = rep.representation(using: .png, properties: [:]) else { continue }

    let name1x = "icon_\(s)x\(s).png"
    try png.write(to: URL(fileURLWithPath: "\(iconDir)/\(name1x)"))

    if s <= 512 {
        let img2x = makeIcon(size: s * 2)
        guard let tiff2 = img2x.tiffRepresentation,
              let rep2 = NSBitmapImageRep(data: tiff2),
              let png2 = rep2.representation(using: .png, properties: [:]) else { continue }
        let name2x = "icon_\(s)x\(s)@2x.png"
        try png2.write(to: URL(fileURLWithPath: "\(iconDir)/\(name2x)"))
    }
}

let proc = Process()
proc.executableURL = URL(fileURLWithPath: "/usr/bin/iconutil")
proc.arguments = ["-c", "icns", iconDir, "-o", "Resources/AppIcon.icns"]
try? FileManager.default.createDirectory(atPath: "Resources", withIntermediateDirectories: true)
try proc.run()
proc.waitUntilExit()

try? FileManager.default.removeItem(atPath: iconDir)
print("Generated Resources/AppIcon.icns")
