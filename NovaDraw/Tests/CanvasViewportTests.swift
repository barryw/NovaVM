import CoreGraphics
import Testing
@testable import NovaDraw

@MainActor
@Suite struct CanvasViewportTests {
    @Test func supportsHighZoomForSmallTraceSheets() {
        let doc = NovaDocument(width: 120, height: 40)

        CanvasViewport.applyCenteredZoom(
            document: doc,
            size: CGSize(width: 1200, height: 800),
            zoom: 256
        )

        #expect(doc.zoom == 256)
    }

    @Test func fitZoomDoesNotBecomeMinimumZoom() {
        let doc = NovaDocument(width: 120, height: 40)
        CanvasViewport.fitToView(document: doc, size: CGSize(width: 1200, height: 800))
        #expect(doc.zoom == 10)

        CanvasViewport.applyCenteredZoom(
            document: doc,
            size: CGSize(width: 1200, height: 800),
            zoom: 4
        )

        #expect(doc.zoom == 4)
    }

    @Test func centeredZoomKeepsCanvasCentered() {
        let doc = NovaDocument(width: 20, height: 20)

        CanvasViewport.applyCenteredZoom(
            document: doc,
            size: CGSize(width: 1000, height: 800),
            zoom: 16
        )

        #expect(doc.panOffset == CGPoint(x: 340, y: 240))

        CanvasViewport.applyCenteredZoom(
            document: doc,
            size: CGSize(width: 1000, height: 800),
            zoom: 32
        )

        #expect(doc.panOffset == CGPoint(x: 180, y: 80))
    }

    @Test func zoomStopsReachSpriteEditingLevels() {
        #expect(CanvasViewport.nextZoomStop(after: 16) == 32)
        #expect(CanvasViewport.nextZoomStop(after: 128) == 256)
        #expect(CanvasViewport.previousZoomStop(before: 32) == 16)
    }

    @Test func panMovesCanvasByDragDelta() {
        let doc = NovaDocument(width: 20, height: 20)
        doc.panOffset = CGPoint(x: 180, y: 80)

        CanvasViewport.pan(document: doc, by: CGPoint(x: 12.5, y: -8))

        #expect(doc.panOffset == CGPoint(x: 192.5, y: 72))
    }
}
