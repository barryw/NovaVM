import Testing
@testable import NovaDraw

@MainActor
@Suite struct ToolEngineTests {
    private func makeDoc(_ w: Int = 16, _ h: Int = 16) -> (NovaDocument, ToolEngine) {
        let doc = NovaDocument(width: w, height: h)
        doc.foregroundColor = 1
        return (doc, ToolEngine(document: doc))
    }

    @Test func pencilDrawsPixel() {
        let (doc, engine) = makeDoc()
        engine.mouseDown(x: 5, y: 5, shift: false)
        #expect(doc.getPixel(5, 5) == 1)
        #expect(doc.isPixelPainted(5, 5))
    }

    @Test func pencilDrawsBlackPixel() {
        let (doc, engine) = makeDoc()
        doc.foregroundColor = 0
        engine.mouseDown(x: 5, y: 5, shift: false)
        #expect(doc.getPixel(5, 5) == 0)
        #expect(doc.isPixelPainted(5, 5))
    }

    @Test func eraserClearsPixel() {
        let (doc, engine) = makeDoc()
        doc.setPixel(5, 5, 7)
        doc.currentTool = .eraser
        engine.mouseDown(x: 5, y: 5, shift: false)
        #expect(doc.getPixel(5, 5) == 0)
        #expect(!doc.isPixelPainted(5, 5))
    }

    @Test func lineHorizontal() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .line
        engine.mouseDown(x: 0, y: 0, shift: false)
        engine.mouseUp(x: 4, y: 0)
        for x in 0...4 { #expect(doc.getPixel(x, 0) == 1) }
        #expect(doc.getPixel(5, 0) == 0)
    }

    @Test func lineVertical() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .line
        engine.mouseDown(x: 3, y: 1, shift: false)
        engine.mouseUp(x: 3, y: 5)
        for y in 1...5 { #expect(doc.getPixel(3, y) == 1) }
    }

    @Test func lineDiagonal() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .line
        engine.mouseDown(x: 0, y: 0, shift: false)
        engine.mouseUp(x: 3, y: 3)
        for i in 0...3 { #expect(doc.getPixel(i, i) == 1) }
    }

    @Test func rectOutline() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .rect
        engine.mouseDown(x: 1, y: 1, shift: false)
        engine.mouseUp(x: 4, y: 3)
        for x in 1...4 {
            #expect(doc.getPixel(x, 1) == 1)
            #expect(doc.getPixel(x, 3) == 1)
        }
        for y in 1...3 {
            #expect(doc.getPixel(1, y) == 1)
            #expect(doc.getPixel(4, y) == 1)
        }
        #expect(doc.getPixel(2, 2) == 0)
    }

    @Test func rectFilled() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .rect
        engine.mouseDown(x: 1, y: 1, shift: true)
        engine.mouseUp(x: 3, y: 3)
        for y in 1...3 {
            for x in 1...3 {
                #expect(doc.getPixel(x, y) == 1)
            }
        }
    }

    @Test func floodFill() {
        let (doc, engine) = makeDoc(8, 8)
        doc.foregroundColor = 2
        doc.currentTool = .rect
        engine.mouseDown(x: 1, y: 1, shift: false)
        engine.mouseUp(x: 6, y: 6)
        doc.foregroundColor = 3
        doc.currentTool = .fill
        engine.mouseDown(x: 3, y: 3, shift: false)
        #expect(doc.getPixel(3, 3) == 3)
        #expect(doc.getPixel(2, 2) == 3)
        #expect(doc.getPixel(1, 1) == 2)
        #expect(doc.getPixel(0, 0) == 0)
    }

    @Test func floodFillPaintsTransparentBlack() {
        let (doc, engine) = makeDoc(4, 4)
        doc.foregroundColor = 0
        doc.currentTool = .fill
        engine.mouseDown(x: 0, y: 0, shift: false)
        #expect(doc.getPixel(0, 0) == 0)
        #expect(doc.isPixelPainted(0, 0))
    }

    @Test func eyedropperPicksColor() {
        let (doc, engine) = makeDoc()
        doc.setPixel(5, 5, 9)
        doc.currentTool = .eyedropper
        doc.previousTool = .pencil
        engine.mouseDown(x: 5, y: 5, shift: false)
        #expect(doc.foregroundColor == 9)
        #expect(doc.currentTool == .pencil)
    }

    @Test func circleOutline() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .circle
        engine.mouseDown(x: 8, y: 8, shift: false)
        engine.mouseUp(x: 11, y: 8)
        #expect(doc.getPixel(8, 8) == 0) // center empty
        #expect(doc.getPixel(8, 5) == 1) // top
    }

    @Test func circleFilled() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .circle
        engine.mouseDown(x: 8, y: 8, shift: true)
        engine.mouseUp(x: 10, y: 8)
        #expect(doc.getPixel(8, 8) == 1) // center filled
    }

    @Test func pencilDragDrawsLine() {
        let (doc, engine) = makeDoc()
        engine.mouseDown(x: 0, y: 0, shift: false)
        engine.mouseDragged(x: 3, y: 0, shift: false)
        for x in 0...3 { #expect(doc.getPixel(x, 0) == 1) }
    }

    @Test func rightMouseEraseDrawsLineWithoutChangingTool() {
        let (doc, engine) = makeDoc()
        for x in 0...3 { doc.setPixel(x, 0, 7) }
        doc.currentTool = .pencil
        doc.markClean()

        engine.eraseMouseDown(x: 0, y: 0)
        engine.eraseMouseDragged(x: 3, y: 0)
        engine.eraseMouseUp()

        for x in 0...3 { #expect(doc.getPixel(x, 0) == 0) }
        #expect(doc.currentTool == .pencil)
        #expect(doc.isDirty)

        doc.undo()
        for x in 0...3 { #expect(doc.getPixel(x, 0) == 7) }
    }

    @Test func copyPasteSelectionPreservesPaintedMaskAndSelectsPaste() {
        let (doc, engine) = makeDoc(8, 8)
        doc.setPixel(1, 1, 0)
        doc.setPixel(2, 1, 7)
        doc.selection = (1, 1, 2, 2)

        engine.copySelection()

        doc.selection = (4, 4, 2, 2)
        doc.currentTool = .pencil
        engine.pasteClipboard()

        #expect(doc.currentTool == .select)
        #expect(doc.selection?.x == 4)
        #expect(doc.selection?.y == 4)
        #expect(doc.selection?.w == 2)
        #expect(doc.selection?.h == 2)
        #expect(doc.getPixel(4, 4) == 0)
        #expect(doc.isPixelPainted(4, 4))
        #expect(doc.getPixel(5, 4) == 7)
        #expect(doc.isPixelPainted(5, 4))
        #expect(!doc.isPixelPainted(4, 5))
    }

    @Test func cutSelectionCopiesClearsAndCanUndo() {
        let (doc, engine) = makeDoc(8, 8)
        doc.setPixel(0, 0, 3)
        doc.setPixel(1, 0, 4)
        doc.selection = (0, 0, 2, 1)

        engine.cutSelection()

        #expect(doc.clipboard?.w == 2)
        #expect(doc.clipboard?.h == 1)
        #expect(doc.getPixel(0, 0) == 0)
        #expect(!doc.isPixelPainted(0, 0))
        #expect(doc.getPixel(1, 0) == 0)
        #expect(!doc.isPixelPainted(1, 0))
        #expect(doc.selection == nil)

        doc.undo()
        #expect(doc.getPixel(0, 0) == 3)
        #expect(doc.getPixel(1, 0) == 4)
    }

    @Test func deselectSelectionClearsOnlySelection() {
        let (doc, engine) = makeDoc(8, 8)
        doc.setPixel(2, 2, 5)
        doc.selection = (2, 2, 1, 1)
        doc.markClean()

        engine.deselectSelection()

        #expect(doc.selection == nil)
        #expect(doc.getPixel(2, 2) == 5)
        #expect(!doc.isDirty)
    }

    @Test func linePreviewUpdates() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .line
        engine.mouseDown(x: 0, y: 0, shift: false)
        engine.mouseDragged(x: 5, y: 0, shift: false)
        // During drag, line at y=0 from 0..5
        for x in 0...5 { #expect(doc.getPixel(x, 0) == 1) }
        // Now drag to different endpoint — old preview cleared
        engine.mouseDragged(x: 0, y: 5, shift: false)
        #expect(doc.getPixel(5, 0) == 0) // old endpoint cleared
        #expect(doc.getPixel(0, 5) == 1) // new endpoint drawn
    }
}
