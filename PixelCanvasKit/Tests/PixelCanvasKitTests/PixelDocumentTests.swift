import Testing
import Foundation
import CoreGraphics
@testable import PixelCanvasKit

@MainActor
@Suite struct PixelDocumentTests {
    @Test func pixelGetSet() {
        let doc = PixelDocument(width: 16, height: 16)
        doc.setPixel(5, 3, 7)
        #expect(doc.getPixel(5, 3) == 7)
        #expect(doc.isPixelPainted(5, 3))
    }

    @Test func blackPixelIsPainted() {
        let doc = PixelDocument(width: 16, height: 16)
        doc.setPixel(5, 3, 0)
        #expect(doc.getPixel(5, 3) == 0)
        #expect(doc.isPixelPainted(5, 3))
    }

    @Test func clearPixelMakesBlackTransparent() {
        let doc = PixelDocument(width: 16, height: 16)
        doc.setPixel(5, 3, 0)
        doc.clearPixel(5, 3)
        #expect(doc.getPixel(5, 3) == 0)
        #expect(!doc.isPixelPainted(5, 3))
    }

    @Test func imageSlotsAreSameSizeAndIndependent() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.setPixel(0, 0, 2)

        #expect(doc.addImage())
        #expect(doc.images.count == 2)
        #expect(doc.selectedImageIndex == 1)
        #expect(doc.getPixel(0, 0) == 0)
        #expect(!doc.isPixelPainted(0, 0))

        doc.setPixel(1, 1, 0)
        #expect(doc.isPixelPainted(1, 1))

        doc.selectImage(at: 0)
        #expect(doc.getPixel(0, 0) == 2)
        #expect(doc.isPixelPainted(0, 0))
        #expect(!doc.isPixelPainted(1, 1))

        doc.selectImage(at: 1)
        #expect(doc.getPixel(0, 0) == 0)
        #expect(!doc.isPixelPainted(0, 0))
        #expect(doc.getPixel(1, 1) == 0)
        #expect(doc.isPixelPainted(1, 1))
    }

    @Test func imageSlotsAreCappedAtTwelve() {
        let doc = PixelDocument(width: 1, height: 1)

        for _ in 1..<PixelDocument.maximumImageCount {
            #expect(doc.addImage())
        }

        #expect(doc.images.count == PixelDocument.maximumImageCount)
        #expect(!doc.addImage())
    }

    @Test func imageSlotsCanBeNamed() {
        let doc = PixelDocument(width: 1, height: 1)
        doc.renameSelectedImage(to: "pawn")

        #expect(doc.images[0].name == "pawn")
        #expect(doc.isDirty)

        doc.markClean()
        doc.renameSelectedImage(to: " pawn ")
        #expect(doc.images[0].name == "pawn")
        #expect(!doc.isDirty)

        doc.renameSelectedImage(to: "")
        #expect(doc.images[0].name == "Image 1")
        #expect(doc.isDirty)
    }

    @Test func deletingSelectedImageLoadsRemainingImage() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.setPixel(0, 0, 5)
        doc.renameSelectedImage(to: "pawn")
        _ = doc.addImage()
        doc.renameSelectedImage(to: "knight")
        doc.setPixel(1, 1, 6)

        #expect(doc.deleteSelectedImage())

        #expect(doc.images.count == 1)
        #expect(doc.selectedImageIndex == 0)
        #expect(doc.images[0].name == "pawn")
        #expect(doc.getPixel(0, 0) == 5)
        #expect(doc.getPixel(1, 1) == 0)
        #expect(!doc.deleteSelectedImage())
    }

    @Test func outOfBoundsReturnsZero() {
        let doc = PixelDocument(width: 16, height: 16)
        #expect(doc.getPixel(-1, 0) == 0)
        #expect(doc.getPixel(16, 0) == 0)
        #expect(doc.getPixel(0, -1) == 0)
        #expect(doc.getPixel(0, 16) == 0)
    }

    @Test func outOfBoundsSetIsNoOp() {
        let doc = PixelDocument(width: 16, height: 16)
        doc.setPixel(-1, 0, 5)
        doc.setPixel(16, 0, 5)
        // No crash = pass
    }

    @Test func colorMaskedTo4Bits() {
        let doc = PixelDocument(width: 16, height: 16)
        doc.setPixel(0, 0, 0xFF)
        #expect(doc.getPixel(0, 0) == 0x0F)
    }

    @Test func undoRedo() {
        let doc = PixelDocument(width: 4, height: 4)
        doc.pushUndo()
        doc.setPixel(0, 0, 5)
        #expect(doc.getPixel(0, 0) == 5)
        doc.undo()
        #expect(doc.getPixel(0, 0) == 0)
        doc.redo()
        #expect(doc.getPixel(0, 0) == 5)
    }

    @Test func undoOnEmptyStackDoesNothing() {
        let doc = PixelDocument(width: 4, height: 4)
        doc.setPixel(0, 0, 3)
        doc.undo()
        #expect(doc.getPixel(0, 0) == 3)
    }

    @Test func clear() {
        let doc = PixelDocument(width: 4, height: 4)
        doc.setPixel(1, 1, 3)
        doc.clear()
        #expect(doc.getPixel(1, 1) == 0)
        #expect(doc.canUndo)
    }

    @Test func loadPixels() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.loadPixels([0, 2, 3, 4])
        #expect(doc.getPixel(0, 0) == 0)
        #expect(doc.getPixel(1, 1) == 4)
        #expect(doc.isPixelPainted(0, 0))
        #expect(doc.canUndo)
    }

    @Test func loadPixelsWrongSizeIgnored() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.setPixel(0, 0, 5)
        doc.loadPixels([1, 2, 3])
        #expect(doc.getPixel(0, 0) == 5)
    }

    @Test func loadPixelsRawNoUndo() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.loadPixelsRaw([1, 2, 3, 4])
        #expect(doc.getPixel(0, 0) == 1)
        #expect(!doc.canUndo)
        #expect(!doc.isDirty)
    }

    @Test func markSavedAndDirtyState() {
        let doc = PixelDocument(width: 2, height: 2)
        let url = URL(fileURLWithPath: "/tmp/test.novadraw")
        doc.setPixel(0, 0, 1)
        #expect(doc.isDirty)
        doc.markSaved(to: url)
        #expect(doc.fileURL == url)
        #expect(!doc.isDirty)
        doc.setPixel(0, 1, 2)
        #expect(doc.isDirty)
    }

    @Test func workspaceStateMarksDirty() {
        let doc = PixelDocument(width: 2, height: 2)

        doc.markClean()
        doc.foregroundColor = 4
        #expect(doc.isDirty)

        doc.markClean()
        doc.currentTool = .fill
        #expect(doc.isDirty)

        doc.markClean()
        doc.zoom = 8
        #expect(doc.isDirty)

        doc.markClean()
        doc.panOffset = CGPoint(x: -10, y: 12)
        #expect(doc.isDirty)

        doc.markClean()
        doc.backdropPlacementMode = .floating
        #expect(doc.isDirty)

        doc.markClean()
        doc.backdropScale = 0.5
        #expect(doc.isDirty)

        doc.markClean()
        doc.backdropOffset = CGPoint(x: -4, y: 8)
        #expect(doc.isDirty)
    }

    @Test func workspaceStateCanBeAppliedWithoutDirtyingDocument() {
        let doc = PixelDocument(width: 2, height: 2)

        doc.performWithoutMarkingDirty {
            doc.foregroundColor = 7
            doc.currentTool = .rect
            doc.zoom = 6
            doc.panOffset = CGPoint(x: 4, y: 5)
            doc.backdropPlacementMode = .floating
            doc.backdropScale = 0.25
            doc.backdropOffset = CGPoint(x: -2, y: 3)
            doc.markHasSavedViewport()
        }

        #expect(doc.foregroundColor == 7)
        #expect(doc.currentTool == .rect)
        #expect(doc.zoom == 6)
        #expect(doc.panOffset == CGPoint(x: 4, y: 5))
        #expect(doc.backdropPlacementMode == .floating)
        #expect(doc.backdropScale == 0.25)
        #expect(doc.backdropOffset == CGPoint(x: -2, y: 3))
        #expect(doc.hasSavedViewport)
        #expect(!doc.isDirty)
    }

    @Test func switchingAwayFromSelectionToolClearsSelection() {
        let doc = PixelDocument(width: 4, height: 4)
        doc.selectTool(.select)
        doc.selection = (1, 1, 2, 2)

        doc.selectTool(.pencil)

        #expect(doc.previousTool == .select)
        #expect(doc.currentTool == .pencil)
        #expect(doc.selection == nil)
    }

    @Test func backdropScaleAroundAnchorPreservesImagePoint() {
        let doc = PixelDocument(width: 20, height: 20)
        doc.backdropPlacementMode = .floating
        doc.backdropScale = 1
        doc.backdropOffset = CGPoint(x: -10, y: -20)

        doc.scaleBackdrop(by: 0.5, around: CGPoint(x: 5, y: 5))

        #expect(doc.backdropScale == 0.5)
        #expect(doc.backdropOffset == CGPoint(x: -2.5, y: -7.5))
    }

    @Test func setBackdropScaleAroundAnchorPreservesImagePoint() {
        let doc = PixelDocument(width: 20, height: 20)
        doc.backdropPlacementMode = .floating
        doc.backdropScale = 0.05
        doc.backdropOffset = CGPoint(x: 3, y: -4)

        doc.setBackdropScale(0.055, around: doc.canvasCenter)

        #expect(abs(doc.backdropScale - 0.055) < 0.0001)
        #expect(abs(doc.backdropOffset.x - 2.3) < 0.0001)
        #expect(abs(doc.backdropOffset.y - -5.4) < 0.0001)
    }

    @Test func backdropScaleIsClamped() {
        let doc = PixelDocument(width: 20, height: 20)

        doc.scaleBackdrop(by: 0, around: .zero)
        #expect(doc.backdropScale == PixelDocument.minimumBackdropScale)

        doc.scaleBackdrop(by: 1_000_000, around: .zero)
        #expect(doc.backdropScale == PixelDocument.maximumBackdropScale)
    }

    @Test func backdropScrollScalingUsesFineSteps() {
        let wheelFactor = PixelDocument.backdropScrollScaleFactor(
            deltaY: 3,
            hasPreciseScrollingDeltas: false
        )
        let trackpadFactor = PixelDocument.backdropScrollScaleFactor(
            deltaY: 10,
            hasPreciseScrollingDeltas: true
        )

        #expect(wheelFactor > 1)
        #expect(wheelFactor < 1.02)
        #expect(trackpadFactor > 1)
        #expect(trackpadFactor < 1.02)
        #expect(PixelDocument.backdropScrollScaleFactor(deltaY: -3, hasPreciseScrollingDeltas: false) < 1)
    }

    @Test func initialState() {
        let doc = PixelDocument(width: 320, height: 200)
        #expect(doc.width == 320)
        #expect(doc.height == 200)
        #expect(doc.pixels.count == 64000)
        #expect(doc.foregroundColor == 1)
        #expect(doc.backgroundColor == 0)
        #expect(doc.currentTool == .pencil)
        #expect(doc.gridOn == true)
        #expect(doc.images.count == 1)
        #expect(doc.selectedImageIndex == 0)
        #expect(doc.backdropPlacementMode == .fitToCanvas)
        #expect(doc.backdropScale == 1)
        #expect(doc.backdropOffset == .zero)
        #expect(!doc.canUndo)
        #expect(!doc.canRedo)
    }
}
