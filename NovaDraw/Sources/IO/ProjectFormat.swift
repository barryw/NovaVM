import Foundation
import AppKit
import UniformTypeIdentifiers

extension UTType {
    static let novaDrawProject = UTType(exportedAs: "com.nova.novadraw.project")
}

enum ProjectFormat {
    private static let magic = Data([0x4E, 0x44, 0x52, 0x57, 0x31]) // NDRW1

    struct Header: Codable, Sendable {
        var version: Int = 1
        var width: Int
        var height: Int
        var backdropOpacity: Double
        var backdropImageOpacity: Double?
        var backdropImageLength: Int?
        var paintedPixelLength: Int? = nil
        var imageCount: Int? = nil
        var selectedImageIndex: Int? = nil
        var imageNames: [String]? = nil
        var gridOn: Bool
        var gridMajor: Int
        var foregroundColor: UInt8? = nil
        var backgroundColor: UInt8? = nil
        var currentTool: DrawingTool? = nil
        var previousTool: DrawingTool? = nil
        var zoom: Double? = nil
        var panOffsetX: Double? = nil
        var panOffsetY: Double? = nil
        var backdropPlacementMode: BackdropPlacementMode? = nil
        var backdropScale: Double? = nil
        var backdropOffsetX: Double? = nil
        var backdropOffsetY: Double? = nil
    }

    @MainActor
    static func encode(document: NovaDocument) -> Data {
        let backdropData = document.backdropImage.flatMap(Self.pngData)
        let header = Header(
            width: document.width, height: document.height,
            backdropOpacity: document.backdropOpacity,
            backdropImageOpacity: document.backdropImageOpacity,
            backdropImageLength: backdropData?.count ?? 0,
            paintedPixelLength: document.images.count * document.width * document.height,
            imageCount: document.images.count,
            selectedImageIndex: document.selectedImageIndex,
            imageNames: document.images.map(\.name),
            gridOn: document.gridOn, gridMajor: document.gridMajor,
            foregroundColor: document.foregroundColor & 0x0F,
            backgroundColor: document.backgroundColor & 0x0F,
            currentTool: document.currentTool,
            previousTool: document.previousTool,
            zoom: Double(document.zoom),
            panOffsetX: Double(document.panOffset.x),
            panOffsetY: Double(document.panOffset.y),
            backdropPlacementMode: document.backdropPlacementMode,
            backdropScale: Double(document.backdropScale),
            backdropOffsetX: Double(document.backdropOffset.x),
            backdropOffsetY: Double(document.backdropOffset.y)
        )
        let headerData = try! JSONEncoder().encode(header)
        var data = magic
        var headerLength = UInt32(headerData.count).littleEndian
        withUnsafeBytes(of: &headerLength) { data.append(contentsOf: $0) }
        data.append(headerData)
        for image in document.images {
            data.append(contentsOf: image.pixels)
        }
        for image in document.images {
            data.append(contentsOf: image.paintedPixels)
        }
        if let backdropData {
            data.append(backdropData)
        }
        return data
    }

    @MainActor
    static func decode(data: Data) -> NovaDocument? {
        if data.starts(with: magic) {
            return decodeCurrent(data: data)
        }
        return decodeLegacy(data: data)
    }

    @MainActor
    private static func decodeCurrent(data: Data) -> NovaDocument? {
        let headerLengthOffset = magic.count
        let headerOffset = headerLengthOffset + MemoryLayout<UInt32>.size
        guard data.count >= headerOffset else { return nil }

        let headerLength = data[headerLengthOffset..<headerOffset].enumerated().reduce(UInt32(0)) { partial, byte in
            partial | (UInt32(byte.element) << UInt32(byte.offset * 8))
        }
        let headerEnd = headerOffset + Int(headerLength)
        guard headerEnd <= data.count else { return nil }

        let jsonData = data[headerOffset..<headerEnd]
        guard let header = try? JSONDecoder().decode(Header.self, from: jsonData),
              header.version == 1 else {
            return nil
        }

        return makeDocument(header: header, payload: data[headerEnd...])
    }

    @MainActor
    private static func decodeLegacy(data: Data) -> NovaDocument? {
        guard let newline = data.firstIndex(of: 0x0A) else { return nil }
        let jsonData = data[data.startIndex..<newline]
        guard let header = try? JSONDecoder().decode(Header.self, from: jsonData) else { return nil }

        let pixelStart = data.index(after: newline)
        return makeDocument(header: header, payload: data[pixelStart...])
    }

    @MainActor
    private static func makeDocument(header: Header, payload: Data.SubSequence) -> NovaDocument? {
        guard header.width > 0, header.height > 0 else { return nil }
        let pixelCount = header.width * header.height
        let imageCount = header.imageCount ?? 1
        guard imageCount > 0, imageCount <= NovaDocument.maximumImageCount else { return nil }
        let pixelDataLength = pixelCount * imageCount
        guard payload.count >= pixelDataLength else { return nil }

        let pixelData = Array(payload.prefix(pixelDataLength))
        let paintedPixelLength = header.paintedPixelLength ?? 0
        guard paintedPixelLength == 0 || paintedPixelLength == pixelDataLength else { return nil }
        let backdropLength = header.backdropImageLength ?? 0
        guard backdropLength >= 0 else { return nil }
        let paintedPixelStart = payload.startIndex + pixelDataLength
        let paintedPixelEnd = paintedPixelStart + paintedPixelLength
        guard paintedPixelEnd <= payload.endIndex else { return nil }
        let paintedPixelData: [UInt8]
        if paintedPixelLength > 0 {
            paintedPixelData = Array(payload[paintedPixelStart..<paintedPixelEnd])
        } else {
            paintedPixelData = NovaDocument.legacyPaintedPixels(from: pixelData)
        }

        let backdropStart = paintedPixelEnd
        let backdropEnd = backdropStart + backdropLength
        guard backdropEnd == payload.endIndex else { return nil }
        guard pixelData.count == pixelDataLength else { return nil }

        let images = (0..<imageCount).map { index in
            let start = index * pixelCount
            let end = start + pixelCount
            let imageName = header.imageNames?.indices.contains(index) == true
                ? header.imageNames?[index] ?? ""
                : ""
            return NovaCanvasImage(
                name: NovaDocument.normalizedImageName(imageName, fallbackIndex: index),
                pixels: Array(pixelData[start..<end]),
                paintedPixels: Array(paintedPixelData[start..<end])
            )
        }

        let doc = NovaDocument(width: header.width, height: header.height)
        doc.loadImagesRaw(images, selectedIndex: header.selectedImageIndex ?? 0)
        doc.performWithoutMarkingDirty {
            doc.backdropOpacity = header.backdropOpacity
            doc.backdropImageOpacity = header.backdropImageOpacity ?? 1.0
            if backdropLength > 0 {
                doc.backdropImage = NSImage(data: Data(payload[backdropStart..<backdropEnd]))
            }
            doc.gridOn = header.gridOn
            doc.gridMajor = header.gridMajor
            doc.foregroundColor = header.foregroundColor.map(sanitizeColor) ?? doc.foregroundColor
            doc.backgroundColor = header.backgroundColor.map(sanitizeColor) ?? doc.backgroundColor
            doc.currentTool = header.currentTool ?? doc.currentTool
            doc.previousTool = header.previousTool ?? doc.previousTool
            doc.backdropPlacementMode = header.backdropPlacementMode ?? doc.backdropPlacementMode
            if let backdropScale = header.backdropScale, backdropScale.isFinite, backdropScale > 0 {
                doc.backdropScale = NovaDocument.clampedBackdropScale(CGFloat(backdropScale))
            }
            if let backdropOffset = makePanOffset(x: header.backdropOffsetX, y: header.backdropOffsetY) {
                doc.backdropOffset = backdropOffset
            }

            if let zoom = header.zoom, zoom.isFinite, zoom > 0 {
                doc.zoom = CGFloat(zoom)
            }
            if let panOffset = makePanOffset(x: header.panOffsetX, y: header.panOffsetY) {
                doc.panOffset = panOffset
            }
            if header.zoom != nil || header.panOffsetX != nil || header.panOffsetY != nil {
                doc.markHasSavedViewport()
            }
        }
        doc.markClean()
        return doc
    }

    private static func makePanOffset(x: Double?, y: Double?) -> CGPoint? {
        guard let x, let y, x.isFinite, y.isFinite else { return nil }
        return CGPoint(x: x, y: y)
    }

    private static func sanitizeColor(_ color: UInt8) -> UInt8 {
        color & 0x0F
    }

    private static func pngData(from image: NSImage) -> Data? {
        guard let tiffData = image.tiffRepresentation,
              let bitmap = NSBitmapImageRep(data: tiffData) else {
            return nil
        }
        return bitmap.representation(using: .png, properties: [:])
    }
}
