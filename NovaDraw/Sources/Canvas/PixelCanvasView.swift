import SwiftUI
import AppKit

struct PixelCanvasView: View {
    let document: NovaDocument
    @Binding var cursorPosition: (x: Int, y: Int)?
    let toolEngine: ToolEngine

    @State private var viewSize: CGSize = .zero
    @State private var hoverLocation: CGPoint?
    @State private var activeBackdropDragLocation: CGPoint?
    @State private var activeCanvasPanLocation: CGPoint?

    var body: some View {
        Canvas(rendersAsynchronously: false) { context, size in
            if viewSize != size { DispatchQueue.main.async { viewSize = size } }
            context.fill(Path(CGRect(origin: .zero, size: size)), with: .color(.init(white: 0.2)))

            let zoom = document.zoom
            let ox = document.panOffset.x
            let oy = document.panOffset.y
            let w = document.width, h = document.height
            let canvasRect = CGRect(x: ox, y: oy, width: CGFloat(w) * zoom, height: CGFloat(h) * zoom)

            drawBackdrop(context: &context, canvasRect: canvasRect)
            drawPixels(context: &context, canvasRect: canvasRect, w: w, h: h)

            if document.gridOn {
                drawGrid(context: &context, zoom: zoom, ox: ox, oy: oy, w: w, h: h)
            }

            if let sel = document.selection {
                drawSelection(context: &context, sel: sel, zoom: zoom, ox: ox, oy: oy)
            }
        }
        .overlay {
            CanvasMouseEventView(
                onLeftMouse: handleLeftMouse,
                onRightMouse: handleRightMouse,
                onMiddleMouse: handleMiddleMouse,
                onHover: handleHover,
                onScroll: handleScroll,
                onMagnify: handleMagnify,
                onCut: toolEngine.cutSelection,
                onCopy: toolEngine.copySelection,
                onPaste: handlePaste,
                onDelete: toolEngine.deleteSelection,
                onDeselect: toolEngine.deselectSelection,
                onCancelSelection: toolEngine.cancelSelectionAction,
                onCommitSelection: toolEngine.commitSelection,
                onNudgeSelection: toolEngine.nudgeSelection,
                canCutOrCopy: { document.selection != nil },
                canPaste: { document.clipboard != nil }
            )
            .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
        .focusable()
    }

    // MARK: - Coordinate Conversion

    private func pixelCoord(from point: CGPoint) -> (x: Int, y: Int)? {
        let canvasPoint = canvasCoord(from: point)
        let x = Int(floor(canvasPoint.x))
        let y = Int(floor(canvasPoint.y))
        guard x >= 0, x < document.width, y >= 0, y < document.height else { return nil }
        return (x, y)
    }

    private func canvasCoord(from point: CGPoint) -> CGPoint {
        CGPoint(
            x: (point.x - document.panOffset.x) / document.zoom,
            y: (point.y - document.panOffset.y) / document.zoom
        )
    }

    private func handleLeftMouse(phase: MousePhase, location: CGPoint, modifiers: NSEvent.ModifierFlags) {
        hoverLocation = location
        cursorPosition = pixelCoord(from: location)

        if shouldAdjustFloatingBackdrop(modifiers: modifiers) || activeBackdropDragLocation != nil {
            handleBackdropDrag(phase: phase, location: location)
            return
        }

        guard let coord = pixelCoord(from: location) else {
            if phase == .up {
                toolEngine.mouseUp(x: 0, y: 0)
            }
            return
        }

        switch phase {
        case .down:
            toolEngine.mouseDown(x: coord.x, y: coord.y, shift: modifiers.contains(.shift))
        case .dragged:
            toolEngine.mouseDragged(x: coord.x, y: coord.y, shift: modifiers.contains(.shift))
        case .up:
            toolEngine.mouseUp(x: coord.x, y: coord.y)
        }
    }

    private func handlePaste() {
        let target = hoverLocation.flatMap(pixelCoord(from:))
        toolEngine.pasteClipboard(at: target)
    }

    private func handleRightMouse(phase: MousePhase, location: CGPoint, modifiers: NSEvent.ModifierFlags) {
        if document.currentTool == .select {
            if phase == .down {
                toolEngine.deselectSelection()
            }
            return
        }

        guard let coord = pixelCoord(from: location) else {
            if phase == .up {
                toolEngine.eraseMouseUp()
            }
            return
        }

        switch phase {
        case .down:
            toolEngine.eraseMouseDown(x: coord.x, y: coord.y)
        case .dragged:
            toolEngine.eraseMouseDragged(x: coord.x, y: coord.y)
        case .up:
            toolEngine.eraseMouseUp()
        }
    }

    private func handleMiddleMouse(phase: MousePhase, location: CGPoint, modifiers: NSEvent.ModifierFlags) {
        _ = modifiers
        switch phase {
        case .down:
            activeCanvasPanLocation = location
        case .dragged:
            guard let previous = activeCanvasPanLocation else {
                activeCanvasPanLocation = location
                return
            }
            CanvasViewport.pan(
                document: document,
                by: CGPoint(x: location.x - previous.x, y: location.y - previous.y)
            )
            activeCanvasPanLocation = location
            handleHover(location)
        case .up:
            activeCanvasPanLocation = nil
            handleHover(location)
        }
    }

    private func handleHover(_ location: CGPoint?) {
        guard let location else {
            cursorPosition = nil
            return
        }
        hoverLocation = location
        cursorPosition = pixelCoord(from: location)
    }

    private func handleScroll(
        location: CGPoint,
        deltaY: CGFloat,
        modifiers: NSEvent.ModifierFlags,
        hasPreciseScrollingDeltas: Bool
    ) {
        guard deltaY != 0 else { return }
        if shouldAdjustFloatingBackdrop(modifiers: modifiers) {
            let factor = NovaDocument.backdropScrollScaleFactor(
                deltaY: deltaY,
                hasPreciseScrollingDeltas: hasPreciseScrollingDeltas
            )
            document.scaleBackdrop(by: factor, around: canvasCoord(from: location))
            return
        }

        let factor: CGFloat = deltaY > 0 ? 1.25 : 0.8
        CanvasViewport.applyZoom(
            document: document,
            size: viewSize,
            zoom: document.zoom * factor,
            anchor: location
        )
    }

    private func handleMagnify(location: CGPoint, magnification: CGFloat, modifiers: NSEvent.ModifierFlags) {
        if shouldAdjustFloatingBackdrop(modifiers: modifiers) {
            document.scaleBackdrop(by: 1 + magnification, around: canvasCoord(from: location))
            return
        }

        CanvasViewport.applyZoom(
            document: document,
            size: viewSize,
            zoom: document.zoom * (1 + magnification),
            anchor: location
        )
    }

    private func shouldAdjustFloatingBackdrop(modifiers: NSEvent.ModifierFlags) -> Bool {
        document.backdropPlacementMode == .floating
            && document.backdropImage != nil
            && modifiers.intersection([.command, .control, .option, .shift]).isEmpty == false
    }

    private func handleBackdropDrag(phase: MousePhase, location: CGPoint) {
        let canvasPoint = canvasCoord(from: location)
        switch phase {
        case .down:
            activeBackdropDragLocation = canvasPoint
        case .dragged:
            guard let previous = activeBackdropDragLocation else {
                activeBackdropDragLocation = canvasPoint
                return
            }
            document.moveBackdrop(by: CGPoint(
                x: canvasPoint.x - previous.x,
                y: canvasPoint.y - previous.y
            ))
            activeBackdropDragLocation = canvasPoint
        case .up:
            activeBackdropDragLocation = nil
        }
    }

    // MARK: - Rendering

    private func drawBackdrop(context: inout GraphicsContext, canvasRect: CGRect) {
        guard document.backdropImageOpacity > 0, let backdrop = document.backdropImage else { return }
        var ctx = context
        ctx.opacity = document.backdropImageOpacity
        ctx.clip(to: Path(canvasRect))
        let image = Image(nsImage: backdrop)
            .interpolation(document.backdropPlacementMode == .floating ? .high : .none)
        let resolved = ctx.resolve(image)
        ctx.draw(resolved, in: backdropRect(for: backdrop, canvasRect: canvasRect))
    }

    private func backdropRect(for image: NSImage, canvasRect: CGRect) -> CGRect {
        guard document.backdropPlacementMode == .floating else {
            return canvasRect
        }

        let pixelSize = backdropPixelSize(for: image)
        return CGRect(
            x: canvasRect.minX + document.backdropOffset.x * document.zoom,
            y: canvasRect.minY + document.backdropOffset.y * document.zoom,
            width: pixelSize.width * document.backdropScale * document.zoom,
            height: pixelSize.height * document.backdropScale * document.zoom
        )
    }

    private func backdropPixelSize(for image: NSImage) -> CGSize {
        if let bitmapRep = image.representations.compactMap({ $0 as? NSBitmapImageRep }).first {
            return CGSize(width: bitmapRep.pixelsWide, height: bitmapRep.pixelsHigh)
        }

        var proposedRect = CGRect(origin: .zero, size: image.size)
        if let cgImage = image.cgImage(forProposedRect: &proposedRect, context: nil, hints: nil) {
            return CGSize(width: cgImage.width, height: cgImage.height)
        }

        return image.size
    }

    private func drawPixels(context: inout GraphicsContext, canvasRect: CGRect, w: Int, h: Int) {
        guard let cgImage = buildCGImage(w: w, h: h) else { return }
        var ctx = context
        ctx.opacity = document.backdropOpacity
        let scaledImage = Image(decorative: cgImage, scale: 1).interpolation(.none)
        let resolved = ctx.resolve(scaledImage)
        ctx.draw(resolved, in: canvasRect)
    }

    private func buildCGImage(w: Int, h: Int) -> CGImage? {
        var rgba = [UInt8](repeating: 0, count: w * h * 4)
        for i in 0..<(w * h) {
            let c = NovaPalette.colors[Int(document.pixels[i]) & 0x0F]
            rgba[i * 4] = c.r
            rgba[i * 4 + 1] = c.g
            rgba[i * 4 + 2] = c.b
            rgba[i * 4 + 3] = document.isPixelPainted(at: i) ? 255 : 0
        }
        let colorSpace = CGColorSpaceCreateDeviceRGB()
        guard let provider = CGDataProvider(data: Data(rgba) as CFData) else { return nil }
        return CGImage(width: w, height: h, bitsPerComponent: 8,
                       bitsPerPixel: 32, bytesPerRow: w * 4,
                       space: colorSpace,
                       bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue),
                       provider: provider, decode: nil,
                       shouldInterpolate: false, intent: .defaultIntent)
    }

    private func drawGrid(context: inout GraphicsContext, zoom: CGFloat, ox: CGFloat, oy: CGFloat, w: Int, h: Int) {
        let canvasW = CGFloat(w) * zoom
        let canvasH = CGFloat(h) * zoom

        if zoom >= 4 {
            var path = Path()
            for x in 0...w {
                let px = ox + CGFloat(x) * zoom
                path.move(to: CGPoint(x: px, y: oy))
                path.addLine(to: CGPoint(x: px, y: oy + canvasH))
            }
            for y in 0...h {
                let py = oy + CGFloat(y) * zoom
                path.move(to: CGPoint(x: ox, y: py))
                path.addLine(to: CGPoint(x: ox + canvasW, y: py))
            }
            context.stroke(path, with: .color(.white.opacity(0.15)), lineWidth: 0.5)
        }

        let major = document.gridMajor
        var majorPath = Path()
        for x in stride(from: 0, through: w, by: major) {
            let px = ox + CGFloat(x) * zoom
            majorPath.move(to: CGPoint(x: px, y: oy))
            majorPath.addLine(to: CGPoint(x: px, y: oy + canvasH))
        }
        for y in stride(from: 0, through: h, by: major) {
            let py = oy + CGFloat(y) * zoom
            majorPath.move(to: CGPoint(x: ox, y: py))
            majorPath.addLine(to: CGPoint(x: ox + canvasW, y: py))
        }
        context.stroke(majorPath, with: .color(.white.opacity(0.3)), lineWidth: 1)
    }

    private func drawSelection(context: inout GraphicsContext, sel: (x: Int, y: Int, w: Int, h: Int),
                                zoom: CGFloat, ox: CGFloat, oy: CGFloat) {
        let selRect = CGRect(
            x: ox + CGFloat(sel.x) * zoom,
            y: oy + CGFloat(sel.y) * zoom,
            width: CGFloat(sel.w) * zoom,
            height: CGFloat(sel.h) * zoom
        )
        let dash = StrokeStyle(lineWidth: 1, dash: [4, 4])
        context.stroke(Path(selRect), with: .color(.white), style: dash)
    }
}

private enum MousePhase {
    case down
    case dragged
    case up
}

private struct CanvasMouseEventView: NSViewRepresentable {
    let onLeftMouse: (MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void
    let onRightMouse: (MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void
    let onMiddleMouse: (MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void
    let onHover: (CGPoint?) -> Void
    let onScroll: (CGPoint, CGFloat, NSEvent.ModifierFlags, Bool) -> Void
    let onMagnify: (CGPoint, CGFloat, NSEvent.ModifierFlags) -> Void
    let onCut: () -> Void
    let onCopy: () -> Void
    let onPaste: () -> Void
    let onDelete: () -> Void
    let onDeselect: () -> Void
    let onCancelSelection: () -> Void
    let onCommitSelection: () -> Void
    let onNudgeSelection: (Int, Int) -> Void
    let canCutOrCopy: () -> Bool
    let canPaste: () -> Bool

    func makeNSView(context: Context) -> EventView {
        let view = EventView()
        view.onLeftMouse = onLeftMouse
        view.onRightMouse = onRightMouse
        view.onMiddleMouse = onMiddleMouse
        view.onHover = onHover
        view.onScroll = onScroll
        view.onMagnify = onMagnify
        view.onCut = onCut
        view.onCopy = onCopy
        view.onPaste = onPaste
        view.onDelete = onDelete
        view.onDeselect = onDeselect
        view.onCancelSelection = onCancelSelection
        view.onCommitSelection = onCommitSelection
        view.onNudgeSelection = onNudgeSelection
        view.canCutOrCopy = canCutOrCopy
        view.canPaste = canPaste
        return view
    }

    func updateNSView(_ nsView: EventView, context: Context) {
        nsView.onLeftMouse = onLeftMouse
        nsView.onRightMouse = onRightMouse
        nsView.onMiddleMouse = onMiddleMouse
        nsView.onHover = onHover
        nsView.onScroll = onScroll
        nsView.onMagnify = onMagnify
        nsView.onCut = onCut
        nsView.onCopy = onCopy
        nsView.onPaste = onPaste
        nsView.onDelete = onDelete
        nsView.onDeselect = onDeselect
        nsView.onCancelSelection = onCancelSelection
        nsView.onCommitSelection = onCommitSelection
        nsView.onNudgeSelection = onNudgeSelection
        nsView.canCutOrCopy = canCutOrCopy
        nsView.canPaste = canPaste
    }

    final class EventView: NSView, NSUserInterfaceValidations {
        var onLeftMouse: ((MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void)?
        var onRightMouse: ((MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void)?
        var onMiddleMouse: ((MousePhase, CGPoint, NSEvent.ModifierFlags) -> Void)?
        var onHover: ((CGPoint?) -> Void)?
        var onScroll: ((CGPoint, CGFloat, NSEvent.ModifierFlags, Bool) -> Void)?
        var onMagnify: ((CGPoint, CGFloat, NSEvent.ModifierFlags) -> Void)?
        var onCut: (() -> Void)?
        var onCopy: (() -> Void)?
        var onPaste: (() -> Void)?
        var onDelete: (() -> Void)?
        var onDeselect: (() -> Void)?
        var onCancelSelection: (() -> Void)?
        var onCommitSelection: (() -> Void)?
        var onNudgeSelection: ((Int, Int) -> Void)?
        var canCutOrCopy: (() -> Bool)?
        var canPaste: (() -> Bool)?

        override var acceptsFirstResponder: Bool {
            true
        }

        override var isFlipped: Bool {
            true
        }

        override func acceptsFirstMouse(for event: NSEvent?) -> Bool {
            true
        }

        func validateUserInterfaceItem(_ item: NSValidatedUserInterfaceItem) -> Bool {
            switch item.action {
            case #selector(cut(_:)), #selector(copy(_:)), #selector(delete(_:)):
                return canCutOrCopy?() ?? false
            case #selector(paste(_:)):
                return canPaste?() ?? false
            default:
                return true
            }
        }

        @objc func cut(_ sender: Any?) {
            _ = sender
            onCut?()
        }

        @objc func copy(_ sender: Any?) {
            _ = sender
            onCopy?()
        }

        @objc func paste(_ sender: Any?) {
            _ = sender
            onPaste?()
        }

        @objc func delete(_ sender: Any?) {
            _ = sender
            onDelete?()
        }

        override func cancelOperation(_ sender: Any?) {
            _ = sender
            onCancelSelection?()
        }

        override func performKeyEquivalent(with event: NSEvent) -> Bool {
            let flags = event.modifierFlags.intersection([.command, .shift, .option, .control])
            guard flags == .command,
                  let key = event.charactersIgnoringModifiers?.lowercased() else {
                return super.performKeyEquivalent(with: event)
            }

            switch key {
            case "x":
                onCut?()
            case "c":
                onCopy?()
            case "v":
                onPaste?()
            case "d":
                onDeselect?()
            default:
                return super.performKeyEquivalent(with: event)
            }
            return true
        }

        override func keyDown(with event: NSEvent) {
            let flags = event.modifierFlags.intersection([.command, .shift, .option, .control])
            switch event.keyCode {
            case 51, 117:
                onDelete?()
            case 36, 76:
                guard flags.isSubset(of: [.shift]) else {
                    super.keyDown(with: event)
                    return
                }
                onCommitSelection?()
            case 123:
                guard flags.isSubset(of: [.shift]) else {
                    super.keyDown(with: event)
                    return
                }
                onNudgeSelection?(-(flags.contains(.shift) ? 8 : 1), 0)
            case 124:
                guard flags.isSubset(of: [.shift]) else {
                    super.keyDown(with: event)
                    return
                }
                onNudgeSelection?(flags.contains(.shift) ? 8 : 1, 0)
            case 125:
                guard flags.isSubset(of: [.shift]) else {
                    super.keyDown(with: event)
                    return
                }
                onNudgeSelection?(0, flags.contains(.shift) ? 8 : 1)
            case 126:
                guard flags.isSubset(of: [.shift]) else {
                    super.keyDown(with: event)
                    return
                }
                onNudgeSelection?(0, -(flags.contains(.shift) ? 8 : 1))
            default:
                super.keyDown(with: event)
            }
        }

        override func updateTrackingAreas() {
            super.updateTrackingAreas()
            trackingAreas.forEach(removeTrackingArea)
            addTrackingArea(NSTrackingArea(
                rect: bounds,
                options: [.mouseMoved, .mouseEnteredAndExited, .activeInKeyWindow, .inVisibleRect],
                owner: self
            ))
        }

        override func mouseDown(with event: NSEvent) {
            window?.makeFirstResponder(self)
            onLeftMouse?(.down, point(from: event), event.modifierFlags)
        }

        override func mouseDragged(with event: NSEvent) {
            onLeftMouse?(.dragged, point(from: event), event.modifierFlags)
        }

        override func mouseUp(with event: NSEvent) {
            onLeftMouse?(.up, point(from: event), event.modifierFlags)
        }

        override func rightMouseDown(with event: NSEvent) {
            window?.makeFirstResponder(self)
            onRightMouse?(.down, point(from: event), event.modifierFlags)
        }

        override func rightMouseDragged(with event: NSEvent) {
            onRightMouse?(.dragged, point(from: event), event.modifierFlags)
        }

        override func rightMouseUp(with event: NSEvent) {
            onRightMouse?(.up, point(from: event), event.modifierFlags)
        }

        override func otherMouseDown(with event: NSEvent) {
            guard event.buttonNumber == 2 else { return }
            window?.makeFirstResponder(self)
            onMiddleMouse?(.down, point(from: event), event.modifierFlags)
        }

        override func otherMouseDragged(with event: NSEvent) {
            guard event.buttonNumber == 2 else { return }
            onMiddleMouse?(.dragged, point(from: event), event.modifierFlags)
        }

        override func otherMouseUp(with event: NSEvent) {
            guard event.buttonNumber == 2 else { return }
            onMiddleMouse?(.up, point(from: event), event.modifierFlags)
        }

        override func mouseMoved(with event: NSEvent) {
            onHover?(point(from: event))
        }

        override func mouseExited(with event: NSEvent) {
            onHover?(nil)
        }

        override func scrollWheel(with event: NSEvent) {
            onScroll?(
                point(from: event),
                event.scrollingDeltaY,
                event.modifierFlags,
                event.hasPreciseScrollingDeltas
            )
        }

        override func magnify(with event: NSEvent) {
            onMagnify?(point(from: event), event.magnification, event.modifierFlags)
        }

        private func point(from event: NSEvent) -> CGPoint {
            convert(event.locationInWindow, from: nil)
        }
    }
}
