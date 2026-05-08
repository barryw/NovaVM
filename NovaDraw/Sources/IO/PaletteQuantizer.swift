import AppKit

enum PaletteQuantizer {
    static func quantize(image: NSImage, width: Int, height: Int) -> [UInt8] {
        guard let bitmapRep = NSBitmapImageRep(
            bitmapDataPlanes: nil, pixelsWide: width, pixelsHigh: height,
            bitsPerSample: 8, samplesPerPixel: 4, hasAlpha: true,
            isPlanar: false, colorSpaceName: .deviceRGB,
            bytesPerRow: width * 4, bitsPerPixel: 32
        ) else { return [UInt8](repeating: 0, count: width * height) }

        NSGraphicsContext.saveGraphicsState()
        let gfxCtx = NSGraphicsContext(bitmapImageRep: bitmapRep)!
        NSGraphicsContext.current = gfxCtx
        let cg = gfxCtx.cgContext
        cg.translateBy(x: 0, y: CGFloat(height))
        cg.scaleBy(x: 1, y: -1)
        image.draw(in: NSRect(origin: .zero, size: NSSize(width: width, height: height)),
                   from: .zero, operation: .copy, fraction: 1.0)
        NSGraphicsContext.restoreGraphicsState()

        var result = [UInt8](repeating: 0, count: width * height)
        guard let data = bitmapRep.bitmapData else { return result }

        for i in 0..<(width * height) {
            let r = Double(data[i * 4]) / 255.0
            let g = Double(data[i * 4 + 1]) / 255.0
            let b = Double(data[i * 4 + 2]) / 255.0
            result[i] = nearestPaletteIndex(r: r, g: g, b: b)
        }
        return result
    }

    private static let grayIndices: [Int] = [0, 11, 12, 15, 1]

    static func nearestPaletteIndex(r: Double, g: Double, b: Double) -> UInt8 {
        let lum = 0.2126 * r + 0.7152 * g + 0.0722 * b
        let (h, s, v) = rgbToHsv(r: r, g: g, b: b)

        if v < 0.045 || lum < 0.022 { return 0 }

        if s < 0.18 {
            let best = grayIndices.min { a, b in
                let la = paletteChannelLuma(a)
                let lb = paletteChannelLuma(b)
                return abs(la - lum) < abs(lb - lum)
            }!
            return UInt8(best)
        }

        let deg = h * 360.0
        if deg < 20 || deg >= 335 { return v >= 0.50 ? 10 : 2 }
        if deg < 44 { return v >= 0.45 ? 8 : 9 }
        if deg < 72 { return 7 }
        if deg < 145 { return v >= 0.50 ? 13 : 5 }
        if deg < 195 { return v >= 0.55 ? 3 : 14 }
        if deg < 230 { return v >= 0.45 ? 14 : 6 }
        if deg < 270 { return v >= 0.62 ? 14 : 6 }
        if deg < 320 { return v >= 0.35 ? 4 : 6 }
        return v >= 0.50 ? 10 : 2
    }

    private static func paletteChannelLuma(_ index: Int) -> Double {
        let c = NovaPalette.colors[index]
        return 0.2126 * Double(c.r) / 255 + 0.7152 * Double(c.g) / 255 + 0.0722 * Double(c.b) / 255
    }

    private static func rgbToHsv(r: Double, g: Double, b: Double) -> (h: Double, s: Double, v: Double) {
        let maxC = max(r, g, b)
        let minC = min(r, g, b)
        let delta = maxC - minC
        let v = maxC
        let s = maxC == 0 ? 0 : delta / maxC
        var h: Double = 0
        if delta > 0 {
            if maxC == r { h = ((g - b) / delta).truncatingRemainder(dividingBy: 6) }
            else if maxC == g { h = (b - r) / delta + 2 }
            else { h = (r - g) / delta + 4 }
            h /= 6
            if h < 0 { h += 1 }
        }
        return (h, s, v)
    }
}
