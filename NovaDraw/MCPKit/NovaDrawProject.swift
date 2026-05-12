import Foundation

public enum NovaDrawProjectError: Error, LocalizedError, Equatable {
    case invalidProject
    case unsupportedVersion(Int)
    case invalidCanvasSize
    case invalidImageIndex(Int)
    case invalidPixelCount(expected: Int, actual: Int)
    case missingArgument(String)
    case invalidArgument(String)

    public var errorDescription: String? {
        switch self {
        case .invalidProject:
            return "The file is not a valid NovaDraw project."
        case .unsupportedVersion(let version):
            return "Unsupported NovaDraw project version \(version)."
        case .invalidCanvasSize:
            return "The canvas width and height must be positive."
        case .invalidImageIndex(let index):
            return "Image index \(index) is outside the project image range."
        case .invalidPixelCount(let expected, let actual):
            return "Expected \(expected) pixels, got \(actual)."
        case .missingArgument(let name):
            return "Missing required argument '\(name)'."
        case .invalidArgument(let name):
            return "Invalid value for argument '\(name)'."
        }
    }
}

public struct NovaDrawProjectHeader: Codable, Equatable, Sendable {
    public var version: Int = 1
    public var width: Int
    public var height: Int
    public var backdropOpacity: Double
    public var backdropImageOpacity: Double?
    public var backdropImageLength: Int?
    public var paintedPixelLength: Int?
    public var imageCount: Int?
    public var selectedImageIndex: Int?
    public var imageNames: [String]?
    public var gridOn: Bool
    public var gridMajor: Int
    public var foregroundColor: UInt8?
    public var backgroundColor: UInt8?
    public var currentTool: String?
    public var previousTool: String?
    public var zoom: Double?
    public var panOffsetX: Double?
    public var panOffsetY: Double?
    public var backdropPlacementMode: String?
    public var backdropScale: Double?
    public var backdropOffsetX: Double?
    public var backdropOffsetY: Double?

    public init(width: Int, height: Int) {
        self.width = width
        self.height = height
        self.backdropOpacity = 0.7
        self.backdropImageOpacity = 1.0
        self.backdropImageLength = 0
        self.paintedPixelLength = width * height
        self.imageCount = 1
        self.selectedImageIndex = 0
        self.imageNames = ["Image 1"]
        self.gridOn = true
        self.gridMajor = 16
        self.foregroundColor = 1
        self.backgroundColor = 0
        self.currentTool = "pencil"
        self.previousTool = "pencil"
        self.zoom = 4.0
        self.panOffsetX = 0
        self.panOffsetY = 0
        self.backdropPlacementMode = "fitToCanvas"
        self.backdropScale = 1.0
        self.backdropOffsetX = 0
        self.backdropOffsetY = 0
    }
}

public struct NovaDrawImage: Equatable, Sendable {
    public var name: String
    public var pixels: [UInt8]
    public var paintedPixels: [UInt8]

    public init(name: String, pixels: [UInt8], paintedPixels: [UInt8]) {
        self.name = name
        self.pixels = pixels
        self.paintedPixels = paintedPixels
    }

    public static func blank(name: String, pixelCount: Int, color: UInt8 = 0, painted: Bool = false) -> NovaDrawImage {
        NovaDrawImage(
            name: name,
            pixels: [UInt8](repeating: color & 0x0F, count: pixelCount),
            paintedPixels: [UInt8](repeating: painted ? 255 : 0, count: pixelCount)
        )
    }
}

public struct NovaDrawProject: Equatable, Sendable {
    public static let maximumImageCount = 12

    public var header: NovaDrawProjectHeader
    public var images: [NovaDrawImage]
    public var backdropData: Data

    public var width: Int { header.width }
    public var height: Int { header.height }
    public var pixelCount: Int { width * height }

    public var selectedImageIndex: Int {
        get { min(max(header.selectedImageIndex ?? 0, 0), max(images.count - 1, 0)) }
        set { header.selectedImageIndex = min(max(newValue, 0), max(images.count - 1, 0)) }
    }

    public init(width: Int,
                height: Int,
                imageNames: [String] = ["Image 1"],
                backgroundColor: UInt8 = 0,
                paintedBackground: Bool = false) throws {
        guard width > 0, height > 0 else { throw NovaDrawProjectError.invalidCanvasSize }
        let names = Array(imageNames.prefix(Self.maximumImageCount)).isEmpty
            ? ["Image 1"]
            : Array(imageNames.prefix(Self.maximumImageCount))
        var header = NovaDrawProjectHeader(width: width, height: height)
        header.imageCount = names.count
        header.imageNames = names.enumerated().map { Self.normalizedImageName($0.element, fallbackIndex: $0.offset) }
        header.paintedPixelLength = width * height * names.count
        self.header = header
        self.images = header.imageNames!.map {
            NovaDrawImage.blank(
                name: $0,
                pixelCount: width * height,
                color: backgroundColor,
                painted: paintedBackground
            )
        }
        self.backdropData = Data()
    }

    public mutating func validateImageIndex(_ index: Int) throws {
        guard images.indices.contains(index) else {
            throw NovaDrawProjectError.invalidImageIndex(index)
        }
    }

    public func pixelIndex(x: Int, y: Int) -> Int? {
        guard x >= 0, x < width, y >= 0, y < height else { return nil }
        return y * width + x
    }

    public func pixel(imageIndex: Int, x: Int, y: Int) throws -> (color: UInt8, painted: Bool) {
        guard images.indices.contains(imageIndex) else {
            throw NovaDrawProjectError.invalidImageIndex(imageIndex)
        }
        guard let index = pixelIndex(x: x, y: y) else {
            return (0, false)
        }
        let image = images[imageIndex]
        return (image.pixels[index] & 0x0F, image.paintedPixels[index] != 0)
    }

    @discardableResult
    public mutating func setPixel(imageIndex: Int, x: Int, y: Int, color: UInt8, painted: Bool = true) throws -> Bool {
        try validateImageIndex(imageIndex)
        guard let index = pixelIndex(x: x, y: y) else { return false }
        images[imageIndex].pixels[index] = color & 0x0F
        images[imageIndex].paintedPixels[index] = painted ? 255 : 0
        return true
    }

    @discardableResult
    public mutating func clearPixel(imageIndex: Int, x: Int, y: Int) throws -> Bool {
        try setPixel(imageIndex: imageIndex, x: x, y: y, color: 0, painted: false)
    }

    public mutating func renameImage(index: Int, name: String) throws {
        try validateImageIndex(index)
        images[index].name = Self.normalizedImageName(name, fallbackIndex: index)
        header.imageNames = images.map(\.name)
    }

    public mutating func duplicateImage(index: Int, name: String? = nil) throws {
        try validateImageIndex(index)
        guard images.count < Self.maximumImageCount else {
            throw NovaDrawProjectError.invalidArgument("imageCount")
        }
        var copy = images[index]
        copy.name = Self.normalizedImageName(name ?? "\(copy.name) copy", fallbackIndex: images.count)
        images.append(copy)
        header.imageCount = images.count
        header.imageNames = images.map(\.name)
        header.paintedPixelLength = pixelCount * images.count
    }

    public static func normalizedImageName(_ name: String, fallbackIndex: Int) -> String {
        let trimmed = name.trimmingCharacters(in: .whitespacesAndNewlines)
        return trimmed.isEmpty ? "Image \(fallbackIndex + 1)" : trimmed
    }
}

public enum NovaDrawProjectCodec {
    private static let magic = Data([0x4E, 0x44, 0x52, 0x57, 0x31])

    public static func load(from url: URL) throws -> NovaDrawProject {
        try decode(data: Data(contentsOf: url))
    }

    public static func save(_ project: NovaDrawProject, to url: URL) throws {
        try encode(project).write(to: url, options: .atomic)
    }

    public static func decode(data: Data) throws -> NovaDrawProject {
        if data.starts(with: magic) {
            return try decodeCurrent(data: data)
        }
        return try decodeLegacy(data: data)
    }

    public static func encode(_ project: NovaDrawProject) throws -> Data {
        guard project.width > 0, project.height > 0 else {
            throw NovaDrawProjectError.invalidCanvasSize
        }
        let pixelCount = project.pixelCount
        for image in project.images {
            guard image.pixels.count == pixelCount else {
                throw NovaDrawProjectError.invalidPixelCount(expected: pixelCount, actual: image.pixels.count)
            }
            guard image.paintedPixels.count == pixelCount else {
                throw NovaDrawProjectError.invalidPixelCount(expected: pixelCount, actual: image.paintedPixels.count)
            }
        }

        var header = project.header
        header.version = 1
        header.width = project.width
        header.height = project.height
        header.backdropImageLength = project.backdropData.count
        header.paintedPixelLength = pixelCount * project.images.count
        header.imageCount = project.images.count
        header.selectedImageIndex = project.selectedImageIndex
        header.imageNames = project.images.map(\.name)
        header.foregroundColor = header.foregroundColor.map { $0 & 0x0F } ?? 1
        header.backgroundColor = header.backgroundColor.map { $0 & 0x0F } ?? 0

        let headerData = try JSONEncoder().encode(header)
        var data = magic
        appendLittleEndian(UInt32(headerData.count), to: &data)
        data.append(headerData)
        for image in project.images {
            data.append(contentsOf: image.pixels.map { $0 & 0x0F })
        }
        for image in project.images {
            data.append(contentsOf: image.paintedPixels.map { $0 == 0 ? 0 : 255 })
        }
        data.append(project.backdropData)
        return data
    }

    private static func decodeCurrent(data: Data) throws -> NovaDrawProject {
        let headerLengthOffset = magic.count
        let headerOffset = headerLengthOffset + MemoryLayout<UInt32>.size
        guard data.count >= headerOffset else { throw NovaDrawProjectError.invalidProject }

        let headerLength = Int(readUInt32LE(data, at: headerLengthOffset))
        let headerEnd = headerOffset + headerLength
        guard headerEnd <= data.count else { throw NovaDrawProjectError.invalidProject }

        let headerData = data[headerOffset..<headerEnd]
        let header = try JSONDecoder().decode(NovaDrawProjectHeader.self, from: headerData)
        guard header.version == 1 else { throw NovaDrawProjectError.unsupportedVersion(header.version) }
        return try makeProject(header: header, payload: data[headerEnd...], legacyPaintedPixels: false)
    }

    private static func decodeLegacy(data: Data) throws -> NovaDrawProject {
        guard let newline = data.firstIndex(of: 0x0A) else {
            throw NovaDrawProjectError.invalidProject
        }
        let header = try JSONDecoder().decode(NovaDrawProjectHeader.self, from: data[..<newline])
        return try makeProject(header: header, payload: data[data.index(after: newline)...], legacyPaintedPixels: true)
    }

    private static func makeProject(header: NovaDrawProjectHeader,
                                    payload: Data.SubSequence,
                                    legacyPaintedPixels: Bool) throws -> NovaDrawProject {
        guard header.width > 0, header.height > 0 else {
            throw NovaDrawProjectError.invalidCanvasSize
        }

        let pixelCount = header.width * header.height
        let imageCount = header.imageCount ?? 1
        guard imageCount > 0, imageCount <= NovaDrawProject.maximumImageCount else {
            throw NovaDrawProjectError.invalidProject
        }

        let pixelDataLength = pixelCount * imageCount
        guard payload.count >= pixelDataLength else {
            throw NovaDrawProjectError.invalidProject
        }
        let pixelData = Array(payload.prefix(pixelDataLength)).map { $0 & 0x0F }

        let paintedPixelLength = legacyPaintedPixels ? 0 : (header.paintedPixelLength ?? 0)
        guard paintedPixelLength == 0 || paintedPixelLength == pixelDataLength else {
            throw NovaDrawProjectError.invalidProject
        }

        let paintedPixelStart = payload.startIndex + pixelDataLength
        let paintedPixelEnd = paintedPixelStart + paintedPixelLength
        guard paintedPixelEnd <= payload.endIndex else {
            throw NovaDrawProjectError.invalidProject
        }

        let paintedPixelData: [UInt8]
        if paintedPixelLength > 0 {
            paintedPixelData = Array(payload[paintedPixelStart..<paintedPixelEnd]).map { $0 == 0 ? 0 : 255 }
        } else {
            paintedPixelData = pixelData.map { $0 == 0 ? 0 : 255 }
        }

        let backdropLength = header.backdropImageLength ?? 0
        guard backdropLength >= 0 else { throw NovaDrawProjectError.invalidProject }
        let backdropStart = paintedPixelEnd
        let backdropEnd = backdropStart + backdropLength
        guard backdropEnd == payload.endIndex else {
            throw NovaDrawProjectError.invalidProject
        }

        var images: [NovaDrawImage] = []
        for imageIndex in 0..<imageCount {
            let start = imageIndex * pixelCount
            let end = start + pixelCount
            let name = header.imageNames?.indices.contains(imageIndex) == true
                ? header.imageNames?[imageIndex] ?? ""
                : ""
            images.append(NovaDrawImage(
                name: NovaDrawProject.normalizedImageName(name, fallbackIndex: imageIndex),
                pixels: Array(pixelData[start..<end]),
                paintedPixels: Array(paintedPixelData[start..<end])
            ))
        }

        return NovaDrawProject(
            header: header,
            images: images,
            backdropData: Data(payload[backdropStart..<backdropEnd])
        )
    }

    private static func appendLittleEndian<T: FixedWidthInteger>(_ value: T, to data: inout Data) {
        var little = value.littleEndian
        withUnsafeBytes(of: &little) { data.append(contentsOf: $0) }
    }

    private static func readUInt32LE(_ data: Data, at offset: Int) -> UInt32 {
        UInt32(data[offset])
            | (UInt32(data[offset + 1]) << 8)
            | (UInt32(data[offset + 2]) << 16)
            | (UInt32(data[offset + 3]) << 24)
    }
}

private extension NovaDrawProject {
    init(header: NovaDrawProjectHeader, images: [NovaDrawImage], backdropData: Data) {
        self.header = header
        self.images = images
        self.backdropData = backdropData
    }
}
