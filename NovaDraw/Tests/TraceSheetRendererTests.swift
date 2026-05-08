import AppKit
import Testing
@testable import NovaDraw

@MainActor
@Suite struct TraceSheetRendererTests {
    @Test func recommendsChessSheetLayout() {
        let settings = TraceSheetImportSettings.recommended(
            sourcePixelWidth: 2172,
            sourcePixelHeight: 724
        )

        #expect(settings.columns == 6)
        #expect(settings.rows == 2)
        #expect(settings.cellWidth == 20)
        #expect(settings.cellHeight == 20)
        #expect(settings.sourceInset == 8)
        #expect(settings.outputWidth == 120)
        #expect(settings.outputHeight == 40)
    }

    @Test func renderBackdropProducesTargetSizedImage() throws {
        let source = makeImage(width: 60, height: 20)
        let settings = TraceSheetImportSettings(
            columns: 3,
            rows: 1,
            cellWidth: 20,
            cellHeight: 20,
            sourceInset: 0,
            backdropImageOpacity: 0.55,
            drawingOpacity: 0.9
        )

        let rendered = try #require(TraceSheetRenderer.renderBackdrop(
            image: source,
            sourcePixelWidth: 60,
            sourcePixelHeight: 20,
            settings: settings
        ))
        let size = try #require(TraceSheetRenderer.sourcePixelSize(for: rendered))

        #expect(size.width == 60)
        #expect(size.height == 20)
    }

    @Test func invalidSettingsDoNotRender() {
        let source = makeImage(width: 20, height: 20)
        let settings = TraceSheetImportSettings(
            columns: 1,
            rows: 1,
            cellWidth: 20,
            cellHeight: 20,
            sourceInset: 10,
            backdropImageOpacity: 0.55,
            drawingOpacity: 0.9
        )

        #expect(TraceSheetRenderer.renderBackdrop(
            image: source,
            sourcePixelWidth: 20,
            sourcePixelHeight: 20,
            settings: settings
        ) == nil)
    }

    @Test func traceGuideDropsBackgroundAndKeepsShape() throws {
        let source = makeImage(width: 20, height: 20) {
            NSColor(calibratedWhite: 0.5, alpha: 1).setFill()
            NSRect(x: 0, y: 0, width: 20, height: 20).fill()
            NSColor.black.setFill()
            NSRect(x: 6, y: 6, width: 8, height: 8).fill()
        }
        let settings = TraceSheetImportSettings(
            columns: 1,
            rows: 1,
            cellWidth: 20,
            cellHeight: 20,
            sourceInset: 0,
            backdropImageOpacity: 0.55,
            drawingOpacity: 0.9,
            renderMode: .traceGuide
        )

        let rendered = try #require(TraceSheetRenderer.renderBackdrop(
            image: source,
            sourcePixelWidth: 20,
            sourcePixelHeight: 20,
            settings: settings
        ))
        let bitmap = try #require(rendered.representations.compactMap { $0 as? NSBitmapImageRep }.first)
        let data = try #require(bitmap.bitmapData)
        let cornerOffset = 0
        let centerOffset = 10 * bitmap.bytesPerRow + 10 * 4

        #expect(data[cornerOffset + 3] == 0)
        #expect(data[centerOffset] < 80)
        #expect(data[centerOffset + 3] > 140)
    }

    private func makeImage(width: Int, height: Int) -> NSImage {
        makeImage(width: width, height: height) {
            NSColor.white.setFill()
            NSRect(x: 0, y: 0, width: width, height: height).fill()
        }
    }

    private func makeImage(width: Int, height: Int, draw: () -> Void) -> NSImage {
        let rep = NSBitmapImageRep(
            bitmapDataPlanes: nil,
            pixelsWide: width,
            pixelsHigh: height,
            bitsPerSample: 8,
            samplesPerPixel: 4,
            hasAlpha: true,
            isPlanar: false,
            colorSpaceName: .deviceRGB,
            bytesPerRow: width * 4,
            bitsPerPixel: 32
        )!

        NSGraphicsContext.saveGraphicsState()
        NSGraphicsContext.current = NSGraphicsContext(bitmapImageRep: rep)
        draw()
        NSGraphicsContext.restoreGraphicsState()

        let image = NSImage(size: NSSize(width: width, height: height))
        image.addRepresentation(rep)
        return image
    }
}
