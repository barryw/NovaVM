import Foundation

public enum NovaDrawDrawing {
    public static func sanitizeColor(_ color: Int) -> UInt8 {
        UInt8(max(0, min(15, color)))
    }

    public static func drawLine(project: inout NovaDrawProject,
                                imageIndex: Int,
                                x0: Int,
                                y0: Int,
                                x1: Int,
                                y1: Int,
                                color: UInt8) throws {
        var x0 = x0
        var y0 = y0
        let dx = abs(x1 - x0)
        let dy = -abs(y1 - y0)
        let sx = x0 < x1 ? 1 : -1
        let sy = y0 < y1 ? 1 : -1
        var err = dx + dy

        while true {
            try project.setPixel(imageIndex: imageIndex, x: x0, y: y0, color: color)
            if x0 == x1 && y0 == y1 { break }
            let e2 = 2 * err
            if e2 >= dy {
                err += dy
                x0 += sx
            }
            if e2 <= dx {
                err += dx
                y0 += sy
            }
        }
    }

    public static func drawRect(project: inout NovaDrawProject,
                                imageIndex: Int,
                                x: Int,
                                y: Int,
                                width: Int,
                                height: Int,
                                color: UInt8,
                                filled: Bool) throws {
        guard width > 0, height > 0 else { throw NovaDrawProjectError.invalidArgument("width/height") }
        let minX = x
        let minY = y
        let maxX = x + width - 1
        let maxY = y + height - 1

        if filled {
            for py in minY...maxY {
                for px in minX...maxX {
                    try project.setPixel(imageIndex: imageIndex, x: px, y: py, color: color)
                }
            }
            return
        }

        for px in minX...maxX {
            try project.setPixel(imageIndex: imageIndex, x: px, y: minY, color: color)
            try project.setPixel(imageIndex: imageIndex, x: px, y: maxY, color: color)
        }
        for py in minY...maxY {
            try project.setPixel(imageIndex: imageIndex, x: minX, y: py, color: color)
            try project.setPixel(imageIndex: imageIndex, x: maxX, y: py, color: color)
        }
    }

    public static func clearRegion(project: inout NovaDrawProject,
                                   imageIndex: Int,
                                   x: Int,
                                   y: Int,
                                   width: Int,
                                   height: Int) throws {
        guard width > 0, height > 0 else { throw NovaDrawProjectError.invalidArgument("width/height") }
        for py in y..<(y + height) {
            for px in x..<(x + width) {
                try project.clearPixel(imageIndex: imageIndex, x: px, y: py)
            }
        }
    }

    public static func drawCircle(project: inout NovaDrawProject,
                                  imageIndex: Int,
                                  cx: Int,
                                  cy: Int,
                                  radius: Int,
                                  color: UInt8,
                                  filled: Bool) throws {
        guard radius >= 0 else { throw NovaDrawProjectError.invalidArgument("radius") }
        if radius == 0 {
            try project.setPixel(imageIndex: imageIndex, x: cx, y: cy, color: color)
            return
        }

        var x = radius
        var y = 0
        var d = 1 - radius
        while x >= y {
            if filled {
                for px in (cx - x)...(cx + x) {
                    try project.setPixel(imageIndex: imageIndex, x: px, y: cy + y, color: color)
                    try project.setPixel(imageIndex: imageIndex, x: px, y: cy - y, color: color)
                }
                for px in (cx - y)...(cx + y) {
                    try project.setPixel(imageIndex: imageIndex, x: px, y: cy + x, color: color)
                    try project.setPixel(imageIndex: imageIndex, x: px, y: cy - x, color: color)
                }
            } else {
                for point in [
                    (cx + x, cy + y), (cx - x, cy + y),
                    (cx + x, cy - y), (cx - x, cy - y),
                    (cx + y, cy + x), (cx - y, cy + x),
                    (cx + y, cy - x), (cx - y, cy - x),
                ] {
                    try project.setPixel(imageIndex: imageIndex, x: point.0, y: point.1, color: color)
                }
            }

            y += 1
            if d <= 0 {
                d += 2 * y + 1
            } else {
                x -= 1
                d += 2 * (y - x) + 1
            }
        }
    }

    public static func floodFill(project: inout NovaDrawProject,
                                 imageIndex: Int,
                                 x: Int,
                                 y: Int,
                                 color: UInt8) throws -> Int {
        try project.validateImageIndex(imageIndex)
        guard let startIndex = project.pixelIndex(x: x, y: y) else { return 0 }
        let targetColor = project.images[imageIndex].pixels[startIndex] & 0x0F
        let targetPainted = project.images[imageIndex].paintedPixels[startIndex] != 0
        guard targetColor != (color & 0x0F) || !targetPainted else { return 0 }

        var changed = 0
        var stack = [(x, y)]
        var seen = Set<Int>()
        while let (cx, cy) = stack.popLast() {
            guard let index = project.pixelIndex(x: cx, y: cy), seen.insert(index).inserted else { continue }
            guard project.images[imageIndex].pixels[index] == targetColor,
                  (project.images[imageIndex].paintedPixels[index] != 0) == targetPainted else {
                continue
            }

            try project.setPixel(imageIndex: imageIndex, x: cx, y: cy, color: color)
            changed += 1
            stack.append((cx + 1, cy))
            stack.append((cx - 1, cy))
            stack.append((cx, cy + 1))
            stack.append((cx, cy - 1))
        }
        return changed
    }

    public static func replaceColor(project: inout NovaDrawProject,
                                    imageIndex: Int,
                                    from: UInt8,
                                    to: UInt8,
                                    paintedOnly: Bool) throws -> Int {
        try project.validateImageIndex(imageIndex)
        let from = from & 0x0F
        let to = to & 0x0F
        var changed = 0
        for index in project.images[imageIndex].pixels.indices {
            if paintedOnly && project.images[imageIndex].paintedPixels[index] == 0 {
                continue
            }
            guard project.images[imageIndex].pixels[index] == from else { continue }
            project.images[imageIndex].pixels[index] = to
            if !paintedOnly {
                project.images[imageIndex].paintedPixels[index] = 255
            }
            changed += 1
        }
        return changed
    }

    public static func rows(from project: NovaDrawProject,
                            imageIndex: Int,
                            x: Int,
                            y: Int,
                            width: Int,
                            height: Int) throws -> [String] {
        guard project.images.indices.contains(imageIndex) else {
            throw NovaDrawProjectError.invalidImageIndex(imageIndex)
        }
        guard width > 0, height > 0 else {
            throw NovaDrawProjectError.invalidArgument("width/height")
        }

        let digits = Array("0123456789ABCDEF")
        var lines: [String] = []
        for py in y..<(y + height) {
            var line = ""
            for px in x..<(x + width) {
                guard let index = project.pixelIndex(x: px, y: py) else {
                    line.append(" ")
                    continue
                }
                if project.images[imageIndex].paintedPixels[index] == 0 {
                    line.append(".")
                } else {
                    line.append(digits[Int(project.images[imageIndex].pixels[index] & 0x0F)])
                }
            }
            lines.append(line)
        }
        return lines
    }
}
