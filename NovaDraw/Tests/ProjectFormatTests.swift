import AppKit
import Foundation
import NovaDrawMCPKit
import Testing
import PixelCanvasKit
@testable import NovaDraw

@MainActor
@Suite struct ProjectFormatTests {
    @Test func roundTripPreservesCanvasAndProjectSettings() {
        let doc = PixelDocument(width: 4, height: 3)
        doc.setPixel(1, 0, 0)
        doc.setPixel(0, 0, 1)
        doc.setPixel(3, 2, 9)
        doc.backdropOpacity = 0.35
        doc.backdropImageOpacity = 0.62
        doc.gridOn = false
        doc.gridMajor = 8
        doc.foregroundColor = 12
        doc.backgroundColor = 3
        doc.currentTool = .rect
        doc.previousTool = .line
        doc.zoom = 8.5
        doc.panOffset = CGPoint(x: -12.25, y: 42.75)
        doc.backdropPlacementMode = .floating
        doc.backdropScale = 0.055
        doc.backdropOffset = CGPoint(x: -20.5, y: 3.25)

        let decoded = ProjectFormat.decode(data: ProjectFormat.encode(document: doc))

        #expect(decoded?.width == 4)
        #expect(decoded?.height == 3)
        #expect(decoded?.getPixel(0, 0) == 1)
        #expect(decoded?.getPixel(1, 0) == 0)
        #expect(decoded?.isPixelPainted(1, 0) == true)
        #expect(decoded?.isPixelPainted(2, 0) == false)
        #expect(decoded?.getPixel(3, 2) == 9)
        #expect(decoded?.backdropOpacity == 0.35)
        #expect(decoded?.backdropImageOpacity == 0.62)
        #expect(decoded?.gridOn == false)
        #expect(decoded?.gridMajor == 8)
        #expect(decoded?.foregroundColor == 12)
        #expect(decoded?.backgroundColor == 3)
        #expect(decoded?.currentTool == .rect)
        #expect(decoded?.previousTool == .line)
        #expect(decoded?.zoom == 8.5)
        #expect(decoded?.panOffset == CGPoint(x: -12.25, y: 42.75))
        #expect(decoded?.backdropPlacementMode == .floating)
        #expect(decoded?.backdropScale == 0.055)
        #expect(decoded?.backdropOffset == CGPoint(x: -20.5, y: 3.25))
        #expect(decoded?.hasSavedViewport == true)
        #expect(decoded?.isDirty == false)
    }

    @Test func roundTripPreservesBackdropImage() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.backdropImage = makeImage()
        doc.backdropImageOpacity = 0.42

        let decoded = ProjectFormat.decode(data: ProjectFormat.encode(document: doc))

        #expect(decoded?.backdropImage != nil)
        #expect(decoded?.backdropImageOpacity == 0.42)
        #expect(decoded?.isDirty == false)
    }

    @Test func roundTripPreservesMultipleImages() {
        let doc = PixelDocument(width: 2, height: 2)
        doc.renameSelectedImage(to: "pawn")
        doc.setPixel(0, 0, 2)
        doc.setPixel(1, 0, 0)
        #expect(doc.addImage())
        doc.renameSelectedImage(to: "knight")
        doc.setPixel(0, 1, 5)
        doc.selectImage(at: 0)

        let decoded = ProjectFormat.decode(data: ProjectFormat.encode(document: doc))

        #expect(decoded?.images.count == 2)
        #expect(decoded?.images[0].name == "pawn")
        #expect(decoded?.images[1].name == "knight")
        #expect(decoded?.selectedImageIndex == 0)
        #expect(decoded?.getPixel(0, 0) == 2)
        #expect(decoded?.getPixel(1, 0) == 0)
        #expect(decoded?.isPixelPainted(1, 0) == true)

        decoded?.selectImage(at: 1)
        #expect(decoded?.getPixel(0, 1) == 5)
        #expect(decoded?.isPixelPainted(0, 1) == true)
        #expect(decoded?.isPixelPainted(0, 0) == false)
    }

    @Test func legacyProjectsStillDecode() throws {
        let header = ProjectFormat.Header(
            width: 2,
            height: 2,
            backdropOpacity: 0.75,
            backdropImageOpacity: nil,
            backdropImageLength: nil,
            gridOn: true,
            gridMajor: 16
        )
        var data = try JSONEncoder().encode(header)
        data.append(0x0A)
        data.append(contentsOf: [1, 2, 3, 4] as [UInt8])

        let decoded = ProjectFormat.decode(data: data)

        #expect(decoded?.width == 2)
        #expect(decoded?.height == 2)
        #expect(decoded?.images.count == 1)
        #expect(decoded?.selectedImageIndex == 0)
        #expect(decoded?.pixels == [1, 2, 3, 4])
        #expect(decoded?.paintedPixels == [255, 255, 255, 255])
        #expect(decoded?.backdropImage == nil)
        #expect(decoded?.foregroundColor == 1)
        #expect(decoded?.currentTool == .pencil)
        #expect(decoded?.zoom == 4.0)
        #expect(decoded?.panOffset == .zero)
        #expect(decoded?.backdropPlacementMode == .fitToCanvas)
        #expect(decoded?.backdropScale == 1.0)
        #expect(decoded?.backdropOffset == .zero)
        #expect(decoded?.hasSavedViewport == false)
    }

    @Test func truncatedProjectReturnsNil() {
        let doc = PixelDocument(width: 2, height: 2)
        var data = ProjectFormat.encode(document: doc)
        data.removeLast()

        #expect(ProjectFormat.decode(data: data) == nil)
    }

    @Test func mcpCreatedProjectDecodesInNovaDraw() throws {
        var project = try NovaDrawProject(width: 3, height: 2, imageNames: ["agent art"])
        try project.setPixel(imageIndex: 0, x: 1, y: 0, color: 12)
        try project.setPixel(imageIndex: 0, x: 2, y: 1, color: 0)

        let decoded = ProjectFormat.decode(data: try NovaDrawProjectCodec.encode(project))

        #expect(decoded?.width == 3)
        #expect(decoded?.height == 2)
        #expect(decoded?.images.first?.name == "agent art")
        #expect(decoded?.getPixel(1, 0) == 12)
        #expect(decoded?.isPixelPainted(1, 0) == true)
        #expect(decoded?.getPixel(2, 1) == 0)
        #expect(decoded?.isPixelPainted(2, 1) == true)
    }

    private func makeImage() -> NSImage {
        let image = NSImage(size: NSSize(width: 2, height: 2))
        image.lockFocus()
        NSColor.red.setFill()
        NSRect(x: 0, y: 0, width: 2, height: 2).fill()
        image.unlockFocus()
        return image
    }
}
