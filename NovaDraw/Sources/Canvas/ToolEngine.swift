import Foundation

@Observable
@MainActor
final class ToolEngine {
    let document: NovaDocument

    private var dragStart: (x: Int, y: Int)?
    private var dragShift = false
    private var previewPixels: NovaPixelSnapshot?

    // MARK: - Selection drag state

    private enum SelectDragMode {
        case none, creating, moving
    }

    private struct FloatingSelection {
        let width: Int
        let height: Int
        let pixels: NovaPixelSnapshot
        let basePixels: NovaPixelSnapshot
        let originalPixels: NovaPixelSnapshot
        let originalSelection: (x: Int, y: Int, w: Int, h: Int)?
        let imageIndex: Int
    }

    private var selectDragMode: SelectDragMode = .none
    private var moveOffset: (dx: Int, dy: Int) = (0, 0)
    private var floatingSelection: FloatingSelection?

    init(document: NovaDocument) {
        self.document = document
    }

    func mouseDown(x: Int, y: Int, shift: Bool) {
        discardFloatingSelectionIfImageChanged()
        if document.currentTool != .select {
            commitFloatingSelection(deselect: true)
        }

        dragStart = (x, y)
        dragShift = shift

        switch document.currentTool {
        case .pencil:
            document.pushUndo()
            document.setPixel(x, y, document.foregroundColor)
        case .eraser:
            document.pushUndo()
            document.clearPixel(x, y)
        case .fill:
            document.pushUndo()
            floodFill(x: x, y: y, newColor: document.foregroundColor)
        case .eyedropper:
            document.foregroundColor = document.getPixel(x, y)
            document.selectTool(document.previousTool)
        case .line, .rect, .circle:
            previewPixels = document.makePixelSnapshot()
            document.pushUndo()
        case .select:
            if let sel = normalizedSelection(document.selection),
               x >= sel.x && x < sel.x + sel.w && y >= sel.y && y < sel.y + sel.h {
                document.selection = sel
                selectDragMode = .moving
                moveOffset = (dx: x - sel.x, dy: y - sel.y)
            } else {
                commitFloatingSelection(deselect: true)
                selectDragMode = .creating
                document.selection = (x, y, 1, 1)
            }
        }
    }

    func mouseDragged(x: Int, y: Int, shift: Bool) {
        guard let start = dragStart else { return }

        switch document.currentTool {
        case .pencil:
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
            dragStart = (x, y)
        case .eraser:
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: 0, paints: false)
            dragStart = (x, y)
        case .line:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
        case .rect:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            drawRect(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor, filled: shift)
        case .circle:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            let r = Int(sqrt(Double((x - start.x) * (x - start.x) + (y - start.y) * (y - start.y))).rounded())
            drawCircle(cx: start.x, cy: start.y, r: r, color: document.foregroundColor, filled: shift)
        case .select:
            switch selectDragMode {
            case .creating:
                document.selection = (start.x, start.y, x - start.x, y - start.y)
            case .moving:
                guard let sel = normalizedSelection(document.selection) else { break }
                beginFloatingSelectionIfNeeded()
                let movedSelection = (x - moveOffset.dx, y - moveOffset.dy, sel.w, sel.h)
                if floatingSelection != nil {
                    renderFloatingSelection(at: movedSelection)
                } else {
                    document.selection = movedSelection
                }
            case .none:
                break
            }
        default:
            break
        }
    }

    func mouseUp(x: Int, y: Int) {
        guard let start = dragStart else { return }

        switch document.currentTool {
        case .line:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
        case .rect:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            drawRect(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor, filled: dragShift)
        case .circle:
            if let base = previewPixels { document.loadPixelSnapshotRaw(base) }
            let r = Int(sqrt(Double((x - start.x) * (x - start.x) + (y - start.y) * (y - start.y))).rounded())
            drawCircle(cx: start.x, cy: start.y, r: r, color: document.foregroundColor, filled: dragShift)
        case .select:
            switch selectDragMode {
            case .creating:
                let rawX = start.x
                let rawY = start.y
                let rawW = x - start.x
                let rawH = y - start.y
                let nx = rawW >= 0 ? rawX : rawX + rawW
                let ny = rawH >= 0 ? rawY : rawY + rawH
                let nw = max(1, abs(rawW))
                let nh = max(1, abs(rawH))
                document.selection = (nx, ny, nw, nh)
            case .moving:
                break
            case .none:
                break
            }
            selectDragMode = .none
        default:
            break
        }

        dragStart = nil
        previewPixels = nil
    }

    // MARK: - Right-button erase

    func eraseMouseDown(x: Int, y: Int) {
        dragStart = (x, y)
        dragShift = false
        previewPixels = nil
        document.pushUndo()
        document.clearPixel(x, y)
    }

    func eraseMouseDragged(x: Int, y: Int) {
        guard let start = dragStart else {
            eraseMouseDown(x: x, y: y)
            return
        }
        drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: 0, paints: false)
        dragStart = (x, y)
    }

    func eraseMouseUp() {
        dragStart = nil
        previewPixels = nil
    }

    // MARK: - Undo / Redo

    func undo() {
        floatingSelection = nil
        selectDragMode = .none
        document.clearSelection()
        document.undo()
    }

    func redo() {
        floatingSelection = nil
        selectDragMode = .none
        document.clearSelection()
        document.redo()
    }

    // MARK: - Selection operations

    func copySelection() {
        discardFloatingSelectionIfImageChanged()
        guard let sel = normalizedSelection(document.selection) else { return }
        let snapshot = capturePixels(in: sel)
        document.clipboard = (sel.w, sel.h, snapshot.pixels, snapshot.paintedPixels)
    }

    func cutSelection() {
        guard document.selection != nil else { return }
        copySelection()
        deleteSelection()
    }

    func pasteClipboard(at target: (x: Int, y: Int)? = nil) {
        discardFloatingSelectionIfImageChanged()
        guard let clip = document.clipboard else { return }
        let px = document.selection?.x ?? target?.x ?? max(0, (document.width - clip.w) / 2)
        let py = document.selection?.y ?? target?.y ?? max(0, (document.height - clip.h) / 2)

        commitFloatingSelection(deselect: false)
        document.selectTool(.select)
        document.pushUndo()
        let snapshot = NovaPixelSnapshot(pixels: clip.pixels, paintedPixels: clip.paintedPixels)
        floatingSelection = FloatingSelection(
            width: clip.w,
            height: clip.h,
            pixels: snapshot,
            basePixels: document.makePixelSnapshot(),
            originalPixels: document.makePixelSnapshot(),
            originalSelection: nil,
            imageIndex: document.selectedImageIndex
        )
        renderFloatingSelection(at: (px, py, clip.w, clip.h))
    }

    func deselectSelection() {
        commitFloatingSelection(deselect: true)
        document.clearSelection()
    }

    func deleteSelection() {
        discardFloatingSelectionIfImageChanged()
        guard let sel = normalizedSelection(document.selection) else { return }
        commitFloatingSelection(deselect: false)
        document.pushUndo()
        clearPixels(in: sel)
        document.selection = nil
    }

    func nudgeSelection(dx: Int, dy: Int) {
        discardFloatingSelectionIfImageChanged()
        guard dx != 0 || dy != 0,
              let sel = normalizedSelection(document.selection) else { return }

        document.selection = sel
        beginFloatingSelectionIfNeeded()
        renderFloatingSelection(at: (sel.x + dx, sel.y + dy, sel.w, sel.h))
    }

    func commitSelection() {
        commitFloatingSelection(deselect: true)
        document.clearSelection()
    }

    func cancelSelectionAction() {
        discardFloatingSelectionIfImageChanged()
        guard let floatingSelection else {
            document.clearSelection()
            return
        }

        document.loadPixelSnapshotRaw(floatingSelection.originalPixels, markDirty: true)
        document.selection = floatingSelection.originalSelection
        self.floatingSelection = nil
        selectDragMode = .none
    }

    private func beginFloatingSelectionIfNeeded() {
        discardFloatingSelectionIfImageChanged()
        guard floatingSelection == nil,
              let sel = normalizedSelection(document.selection) else { return }

        let originalPixels = document.makePixelSnapshot()
        let pixels = capturePixels(in: sel)
        document.pushUndo()
        clearPixels(in: sel)
        floatingSelection = FloatingSelection(
            width: sel.w,
            height: sel.h,
            pixels: pixels,
            basePixels: document.makePixelSnapshot(),
            originalPixels: originalPixels,
            originalSelection: sel,
            imageIndex: document.selectedImageIndex
        )
        renderFloatingSelection(at: sel)
    }

    private func renderFloatingSelection(at sel: (x: Int, y: Int, w: Int, h: Int)) {
        guard let floatingSelection else { return }
        document.loadPixelSnapshotRaw(floatingSelection.basePixels)
        document.selection = (sel.x, sel.y, floatingSelection.width, floatingSelection.height)
        stampPixels(
            floatingSelection.pixels,
            atX: sel.x,
            y: sel.y,
            width: floatingSelection.width,
            height: floatingSelection.height
        )
    }

    private func commitFloatingSelection(deselect: Bool) {
        discardFloatingSelectionIfImageChanged()
        floatingSelection = nil
        if deselect {
            document.clearSelection()
        }
    }

    private func discardFloatingSelectionIfImageChanged() {
        if let floatingSelection, floatingSelection.imageIndex != document.selectedImageIndex {
            self.floatingSelection = nil
        }
    }

    private func normalizedSelection(_ selection: (x: Int, y: Int, w: Int, h: Int)?) -> (x: Int, y: Int, w: Int, h: Int)? {
        guard let selection else { return nil }
        let x = selection.w >= 0 ? selection.x : selection.x + selection.w
        let y = selection.h >= 0 ? selection.y : selection.y + selection.h
        let w = abs(selection.w)
        let h = abs(selection.h)
        guard w > 0, h > 0 else { return nil }
        return (x, y, w, h)
    }

    private func capturePixels(in sel: (x: Int, y: Int, w: Int, h: Int)) -> NovaPixelSnapshot {
        var pixels = [UInt8](repeating: 0, count: sel.w * sel.h)
        var paintedPixels = [UInt8](repeating: 0, count: sel.w * sel.h)
        for y in 0..<sel.h {
            for x in 0..<sel.w {
                let sourceX = sel.x + x
                let sourceY = sel.y + y
                let index = y * sel.w + x
                pixels[index] = document.getPixel(sourceX, sourceY)
                paintedPixels[index] = document.isPixelPainted(sourceX, sourceY) ? 255 : 0
            }
        }
        return NovaPixelSnapshot(pixels: pixels, paintedPixels: paintedPixels)
    }

    private func stampPixels(_ snapshot: NovaPixelSnapshot, atX px: Int, y py: Int, width: Int, height: Int) {
        for y in 0..<height {
            for x in 0..<width {
                let index = y * width + x
                if snapshot.paintedPixels[index] == 0 {
                    document.clearPixel(px + x, py + y)
                } else {
                    document.setPixel(px + x, py + y, snapshot.pixels[index])
                }
            }
        }
    }

    private func clearPixels(in sel: (x: Int, y: Int, w: Int, h: Int)) {
        for y in 0..<sel.h {
            for x in 0..<sel.w {
                document.clearPixel(sel.x + x, sel.y + y)
            }
        }
    }

    // MARK: - Bresenham line

    func drawLine(x0: Int, y0: Int, x1: Int, y1: Int, color: UInt8, paints: Bool = true) {
        var x0 = x0, y0 = y0
        let dx = abs(x1 - x0)
        let dy = -abs(y1 - y0)
        let sx = x0 < x1 ? 1 : -1
        let sy = y0 < y1 ? 1 : -1
        var err = dx + dy
        while true {
            if paints {
                document.setPixel(x0, y0, color)
            } else {
                document.clearPixel(x0, y0)
            }
            if x0 == x1 && y0 == y1 { break }
            let e2 = 2 * err
            if e2 >= dy { err += dy; x0 += sx }
            if e2 <= dx { err += dx; y0 += sy }
        }
    }

    // MARK: - Rectangle

    func drawRect(x0: Int, y0: Int, x1: Int, y1: Int, color: UInt8, filled: Bool) {
        let minX = min(x0, x1), maxX = max(x0, x1)
        let minY = min(y0, y1), maxY = max(y0, y1)
        if filled {
            for y in minY...maxY {
                for x in minX...maxX {
                    document.setPixel(x, y, color)
                }
            }
        } else {
            for x in minX...maxX {
                document.setPixel(x, minY, color)
                document.setPixel(x, maxY, color)
            }
            for y in minY...maxY {
                document.setPixel(minX, y, color)
                document.setPixel(maxX, y, color)
            }
        }
    }

    // MARK: - Midpoint circle

    func drawCircle(cx: Int, cy: Int, r: Int, color: UInt8, filled: Bool) {
        if r == 0 { document.setPixel(cx, cy, color); return }
        var x = r, y = 0, d = 1 - r
        while x >= y {
            if filled {
                for fx in (cx - x)...(cx + x) {
                    document.setPixel(fx, cy + y, color)
                    document.setPixel(fx, cy - y, color)
                }
                for fx in (cx - y)...(cx + y) {
                    document.setPixel(fx, cy + x, color)
                    document.setPixel(fx, cy - x, color)
                }
            } else {
                for (px, py) in [
                    (cx + x, cy + y), (cx - x, cy + y),
                    (cx + x, cy - y), (cx - x, cy - y),
                    (cx + y, cy + x), (cx - y, cy + x),
                    (cx + y, cy - x), (cx - y, cy - x),
                ] {
                    document.setPixel(px, py, color)
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

    // MARK: - Flood fill (4-connected)

    func floodFill(x: Int, y: Int, newColor: UInt8) {
        let target = document.getPixel(x, y)
        let targetPainted = document.isPixelPainted(x, y)
        if target == newColor && targetPainted { return }

        var stack = [(x, y)]
        while let (cx, cy) = stack.popLast() {
            guard cx >= 0, cx < document.width, cy >= 0, cy < document.height,
                  document.getPixel(cx, cy) == target,
                  document.isPixelPainted(cx, cy) == targetPainted else { continue }
            document.setPixel(cx, cy, newColor)
            stack.append((cx + 1, cy))
            stack.append((cx - 1, cy))
            stack.append((cx, cy + 1))
            stack.append((cx, cy - 1))
        }
    }
}
