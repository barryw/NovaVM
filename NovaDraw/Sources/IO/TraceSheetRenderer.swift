import AppKit

enum TraceSheetRenderMode: String, CaseIterable, Sendable {
    case traceGuide
    case pixelated
    case smooth

    var label: String {
        switch self {
        case .traceGuide: "Trace"
        case .pixelated: "Pixelated"
        case .smooth: "Smooth"
        }
    }
}

struct TraceSheetImportSettings: Equatable, Sendable {
    static let maxCanvasWidth = 320
    static let maxCanvasHeight = 200

    var columns: Int
    var rows: Int
    var cellWidth: Int
    var cellHeight: Int
    var sourceInset: Int
    var backdropImageOpacity: Double
    var drawingOpacity: Double
    var renderMode: TraceSheetRenderMode = .traceGuide

    var outputWidth: Int {
        columns * cellWidth
    }

    var outputHeight: Int {
        rows * cellHeight
    }

    static func recommended(sourcePixelWidth: Int, sourcePixelHeight: Int) -> TraceSheetImportSettings {
        let grid = recommendedGrid(sourcePixelWidth: sourcePixelWidth, sourcePixelHeight: sourcePixelHeight)
        return TraceSheetImportSettings(
            columns: grid.columns,
            rows: grid.rows,
            cellWidth: 20,
            cellHeight: 20,
            sourceInset: estimatedInset(
                sourcePixelWidth: sourcePixelWidth,
                sourcePixelHeight: sourcePixelHeight,
                columns: grid.columns,
                rows: grid.rows
            ),
            backdropImageOpacity: 0.55,
            drawingOpacity: 0.9,
            renderMode: .traceGuide
        )
    }

    static func recommendedGrid(sourcePixelWidth: Int, sourcePixelHeight: Int) -> (columns: Int, rows: Int) {
        guard sourcePixelWidth > 0, sourcePixelHeight > 0 else {
            return (1, 1)
        }

        var best = (columns: 1, rows: 1, score: Double.greatestFiniteMagnitude)
        for rows in 1...8 {
            for columns in 1...12 {
                let sourceCellWidth = Double(sourcePixelWidth) / Double(columns)
                let sourceCellHeight = Double(sourcePixelHeight) / Double(rows)
                let squareError = abs((sourceCellWidth / sourceCellHeight) - 1.0)
                let chessSheetBias = abs(Double(columns * rows) - 12.0) * 0.02
                let score = squareError + chessSheetBias
                if score < best.score {
                    best = (columns, rows, score)
                }
            }
        }

        return (best.columns, best.rows)
    }

    static func estimatedInset(sourcePixelWidth: Int, sourcePixelHeight: Int, columns: Int, rows: Int) -> Int {
        guard columns > 0, rows > 0 else { return 0 }
        let sourceCellWidth = Double(sourcePixelWidth) / Double(columns)
        let sourceCellHeight = Double(sourcePixelHeight) / Double(rows)
        let estimate = Int((min(sourceCellWidth, sourceCellHeight) / 45.0).rounded())
        return max(0, min(24, estimate))
    }

    func validationMessage(sourcePixelWidth: Int, sourcePixelHeight: Int) -> String? {
        guard columns > 0, rows > 0, cellWidth > 0, cellHeight > 0 else {
            return "Grid and cell sizes must be greater than zero."
        }

        guard outputWidth <= Self.maxCanvasWidth, outputHeight <= Self.maxCanvasHeight else {
            return "Output must fit within \(Self.maxCanvasWidth)x\(Self.maxCanvasHeight)."
        }

        let sourceCellWidth = Double(sourcePixelWidth) / Double(columns)
        let sourceCellHeight = Double(sourcePixelHeight) / Double(rows)
        guard sourceCellWidth > Double(sourceInset * 2),
              sourceCellHeight > Double(sourceInset * 2) else {
            return "Source inset is larger than the source cell."
        }

        return nil
    }
}

enum TraceSheetRenderer {
    static func sourcePixelSize(for image: NSImage) -> (width: Int, height: Int)? {
        if let cgImage = cgImage(from: image) {
            return (cgImage.width, cgImage.height)
        }

        if let rep = image.representations.first {
            return (rep.pixelsWide, rep.pixelsHigh)
        }

        return nil
    }

    static func renderBackdrop(
        image: NSImage,
        sourcePixelWidth: Int,
        sourcePixelHeight: Int,
        settings: TraceSheetImportSettings
    ) -> NSImage? {
        guard settings.validationMessage(sourcePixelWidth: sourcePixelWidth, sourcePixelHeight: sourcePixelHeight) == nil,
              let cgImage = cgImage(from: image) else {
            return nil
        }

        let normalizedImage = NSImage(
            cgImage: cgImage,
            size: NSSize(width: sourcePixelWidth, height: sourcePixelHeight)
        )
        let interpolation: NSImageInterpolation = settings.renderMode == .pixelated ? .none : .high
        guard let bitmapRep = renderSheetBitmap(
            normalizedImage: normalizedImage,
            sourcePixelWidth: sourcePixelWidth,
            sourcePixelHeight: sourcePixelHeight,
            settings: settings,
            interpolation: interpolation
        ) else {
            return nil
        }

        if settings.renderMode == .traceGuide {
            applyTraceGuide(to: bitmapRep, settings: settings)
        }

        let outputImage = NSImage(size: NSSize(width: settings.outputWidth, height: settings.outputHeight))
        outputImage.addRepresentation(bitmapRep)
        return outputImage
    }

    private static func renderSheetBitmap(
        normalizedImage: NSImage,
        sourcePixelWidth: Int,
        sourcePixelHeight: Int,
        settings: TraceSheetImportSettings,
        interpolation: NSImageInterpolation
    ) -> NSBitmapImageRep? {
        let outputSize = NSSize(width: settings.outputWidth, height: settings.outputHeight)
        guard let bitmapRep = NSBitmapImageRep(
            bitmapDataPlanes: nil,
            pixelsWide: settings.outputWidth,
            pixelsHigh: settings.outputHeight,
            bitsPerSample: 8,
            samplesPerPixel: 4,
            hasAlpha: true,
            isPlanar: false,
            colorSpaceName: .deviceRGB,
            bytesPerRow: settings.outputWidth * 4,
            bitsPerPixel: 32
        ),
        let graphicsContext = NSGraphicsContext(bitmapImageRep: bitmapRep) else {
            return nil
        }
        bitmapRep.size = outputSize

        let sourceCellWidth = CGFloat(sourcePixelWidth) / CGFloat(settings.columns)
        let sourceCellHeight = CGFloat(sourcePixelHeight) / CGFloat(settings.rows)
        let inset = CGFloat(settings.sourceInset)

        NSGraphicsContext.saveGraphicsState()
        NSGraphicsContext.current = graphicsContext
        NSColor.clear.setFill()
        NSRect(origin: .zero, size: outputSize).fill()
        graphicsContext.imageInterpolation = interpolation

        for row in 0..<settings.rows {
            for column in 0..<settings.columns {
                let sourceRect = NSRect(
                    x: CGFloat(column) * sourceCellWidth + inset,
                    y: CGFloat(sourcePixelHeight) - CGFloat(row + 1) * sourceCellHeight + inset,
                    width: sourceCellWidth - inset * 2,
                    height: sourceCellHeight - inset * 2
                )
                let destinationRect = NSRect(
                    x: CGFloat(column * settings.cellWidth),
                    y: CGFloat(settings.outputHeight - (row + 1) * settings.cellHeight),
                    width: CGFloat(settings.cellWidth),
                    height: CGFloat(settings.cellHeight)
                )
                normalizedImage.draw(in: destinationRect, from: sourceRect, operation: .copy, fraction: 1.0)
            }
        }

        NSGraphicsContext.restoreGraphicsState()
        return bitmapRep
    }

    private static func applyTraceGuide(to bitmapRep: NSBitmapImageRep, settings: TraceSheetImportSettings) {
        guard let data = bitmapRep.bitmapData else { return }

        for row in 0..<settings.rows {
            for column in 0..<settings.columns {
                let originX = column * settings.cellWidth
                let originY = row * settings.cellHeight
                let background = borderLuma(
                    data: data,
                    bytesPerRow: bitmapRep.bytesPerRow,
                    originX: originX,
                    originY: originY,
                    width: settings.cellWidth,
                    height: settings.cellHeight
                )

                for y in 0..<settings.cellHeight {
                    for x in 0..<settings.cellWidth {
                        let offset = (originY + y) * bitmapRep.bytesPerRow + (originX + x) * 4
                        let luma = luma(
                            red: data[offset],
                            green: data[offset + 1],
                            blue: data[offset + 2]
                        )
                        let delta = luma - background
                        let magnitude = abs(delta)

                        if magnitude < 0.055 {
                            data[offset + 3] = 0
                            continue
                        }

                        let strong = magnitude >= 0.16
                        let value: UInt8
                        if delta > 0 {
                            value = strong ? 245 : 190
                        } else {
                            value = strong ? 8 : 60
                        }
                        let alpha = UInt8(min(255, max(145, Int((magnitude * 720).rounded()))))
                        data[offset] = value
                        data[offset + 1] = value
                        data[offset + 2] = value
                        data[offset + 3] = alpha
                    }
                }
            }
        }
    }

    private static func borderLuma(
        data: UnsafeMutablePointer<UInt8>,
        bytesPerRow: Int,
        originX: Int,
        originY: Int,
        width: Int,
        height: Int
    ) -> Double {
        var total = 0.0
        var count = 0

        for y in 0..<height {
            for x in 0..<width where x == 0 || y == 0 || x == width - 1 || y == height - 1 {
                let offset = (originY + y) * bytesPerRow + (originX + x) * 4
                total += luma(red: data[offset], green: data[offset + 1], blue: data[offset + 2])
                count += 1
            }
        }

        return count > 0 ? total / Double(count) : 0
    }

    private static func luma(red: UInt8, green: UInt8, blue: UInt8) -> Double {
        (0.2126 * Double(red) + 0.7152 * Double(green) + 0.0722 * Double(blue)) / 255.0
    }

    private static func cgImage(from image: NSImage) -> CGImage? {
        var proposedRect = NSRect(origin: .zero, size: image.size)
        return image.cgImage(forProposedRect: &proposedRect, context: nil, hints: nil)
    }
}
