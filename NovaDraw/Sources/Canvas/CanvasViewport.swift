import CoreGraphics

@MainActor
enum CanvasViewport {
    static let minimumZoom: CGFloat = 1
    static let maximumZoom: CGFloat = 256
    static let zoomStops: [CGFloat] = [1, 2, 4, 8, 16, 32, 64, 128, 256]

    static func clampedZoom(_ zoom: CGFloat) -> CGFloat {
        guard zoom.isFinite else { return minimumZoom }
        return min(max(zoom, minimumZoom), maximumZoom)
    }

    static func fitZoom(document: NovaDocument, size: CGSize) -> CGFloat {
        guard size.width > 0, size.height > 0 else {
            return clampedZoom(document.zoom)
        }

        let fit = min(size.width / CGFloat(document.width),
                      size.height / CGFloat(document.height))
        return clampedZoom(fit)
    }

    static func fitToView(document: NovaDocument, size: CGSize) {
        document.zoom = fitZoom(document: document, size: size)
        centerCanvas(document: document, size: size)
    }

    static func applyZoom(document: NovaDocument, size: CGSize, zoom: CGFloat, anchor: CGPoint) {
        let newZoom = clampedZoom(zoom)
        let oldZoom = document.zoom

        guard oldZoom > 0,
              oldZoom.isFinite,
              anchor.x.isFinite,
              anchor.y.isFinite
        else {
            applyCenteredZoom(document: document, size: size, zoom: newZoom)
            return
        }

        let canvasX = (anchor.x - document.panOffset.x) / oldZoom
        let canvasY = (anchor.y - document.panOffset.y) / oldZoom

        document.zoom = newZoom
        document.panOffset = CGPoint(
            x: anchor.x - canvasX * newZoom,
            y: anchor.y - canvasY * newZoom
        )
    }

    static func applyCenteredZoom(document: NovaDocument, size: CGSize, zoom: CGFloat) {
        let newZoom = clampedZoom(zoom)

        guard size.width > 0, size.height > 0 else {
            document.zoom = newZoom
            return
        }

        document.zoom = newZoom
        centerCanvas(document: document, size: size)
    }

    static func centerCanvas(document: NovaDocument, size: CGSize) {
        document.panOffset = centeredPanOffset(document: document, size: size)
    }

    static func clampPanOffset(document: NovaDocument, size: CGSize) {
        centerCanvas(document: document, size: size)
    }

    static func pan(document: NovaDocument, by delta: CGPoint) {
        guard delta.x.isFinite, delta.y.isFinite else { return }
        document.panOffset = CGPoint(
            x: document.panOffset.x + delta.x,
            y: document.panOffset.y + delta.y
        )
    }

    static func nextZoomStop(after zoom: CGFloat) -> CGFloat {
        let current = clampedZoom(zoom)
        return zoomStops.first { $0 > current * 1.01 } ?? maximumZoom
    }

    static func previousZoomStop(before zoom: CGFloat) -> CGFloat {
        let current = clampedZoom(zoom)
        return zoomStops.last { $0 < current / 1.01 } ?? minimumZoom
    }

    private static func centeredPanOffset(document: NovaDocument, size: CGSize) -> CGPoint {
        guard size.width > 0, size.height > 0 else {
            return document.panOffset
        }

        let canvasW = CGFloat(document.width) * document.zoom
        let canvasH = CGFloat(document.height) * document.zoom

        return CGPoint(
            x: (size.width - canvasW) / 2,
            y: (size.height - canvasH) / 2
        )
    }
}
