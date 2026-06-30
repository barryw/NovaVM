import CoreGraphics
import Testing
@testable import PixelCanvasKit

@MainActor
@Suite struct CanvasViewportTests {
    @Test func supportsHighZoomForSmallTraceSheets() {
        let doc = PixelDocument(width: 120, height: 40)

        CanvasViewport.applyCenteredZoom(
            document: doc,
            size: CGSize(width: 1200, height: 800),
            zoom: 256
        )

        #expect(doc.zoom == 256)
    }

    @Test func fitZoomDoesNotBecomeMinimumZoom() {
        let doc = PixelDocument(width: 120, height: 40)
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
        let doc = PixelDocument(width: 20, height: 20)

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

    @Test func anchoredZoomKeepsCanvasPointUnderPointer() {
        let doc = PixelDocument(width: 320, height: 200)
        doc.zoom = 4
        doc.panOffset = CGPoint(x: 20, y: 30)
        let anchor = CGPoint(x: 180, y: 90)
        let before = canvasPoint(document: doc, at: anchor)

        CanvasViewport.applyZoom(
            document: doc,
            size: CGSize(width: 1000, height: 800),
            zoom: 8,
            anchor: anchor
        )

        let after = canvasPoint(document: doc, at: anchor)
        #expect(doc.zoom == 8)
        #expect(doc.panOffset == CGPoint(x: -140, y: -30))
        #expect(approximatelyEqual(after.x, before.x))
        #expect(approximatelyEqual(after.y, before.y))
    }

    @Test func anchoredZoomUsesClampedZoomWithoutRecentering() {
        let doc = PixelDocument(width: 320, height: 200)
        doc.zoom = 8
        doc.panOffset = CGPoint(x: 12, y: 18)
        let anchor = CGPoint(x: 52, y: 74)
        let before = canvasPoint(document: doc, at: anchor)

        CanvasViewport.applyZoom(
            document: doc,
            size: CGSize(width: 1000, height: 800),
            zoom: 999,
            anchor: anchor
        )

        let after = canvasPoint(document: doc, at: anchor)
        #expect(doc.zoom == CanvasViewport.maximumZoom)
        #expect(doc.panOffset == CGPoint(x: -1228, y: -1718))
        #expect(approximatelyEqual(after.x, before.x))
        #expect(approximatelyEqual(after.y, before.y))
    }

    @Test func panMovesCanvasByDragDelta() {
        let doc = PixelDocument(width: 20, height: 20)
        doc.panOffset = CGPoint(x: 180, y: 80)

        CanvasViewport.pan(document: doc, by: CGPoint(x: 12.5, y: -8))

        #expect(doc.panOffset == CGPoint(x: 192.5, y: 72))
    }

    private func canvasPoint(document: PixelDocument, at anchor: CGPoint) -> CGPoint {
        CGPoint(
            x: (anchor.x - document.panOffset.x) / document.zoom,
            y: (anchor.y - document.panOffset.y) / document.zoom
        )
    }

    private func approximatelyEqual(_ lhs: CGFloat, _ rhs: CGFloat) -> Bool {
        abs(lhs - rhs) < 0.0001
    }
}
