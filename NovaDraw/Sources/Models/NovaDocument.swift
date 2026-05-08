import SwiftUI

enum BackdropPlacementMode: String, CaseIterable, Codable, Sendable {
    case fitToCanvas
    case floating

    var label: String {
        switch self {
        case .fitToCanvas:
            return "Fit"
        case .floating:
            return "Float"
        }
    }
}

struct NovaPixelSnapshot {
    let pixels: [UInt8]
    let paintedPixels: [UInt8]
}

struct NovaCanvasImage: Identifiable, Equatable, Sendable {
    let id: UUID
    var name: String
    var pixels: [UInt8]
    var paintedPixels: [UInt8]

    init(id: UUID = UUID(), name: String, pixels: [UInt8], paintedPixels: [UInt8]) {
        self.id = id
        self.name = name
        self.pixels = pixels
        self.paintedPixels = paintedPixels
    }

    static func blank(name: String, pixelCount: Int) -> NovaCanvasImage {
        NovaCanvasImage(
            name: name,
            pixels: [UInt8](repeating: 0, count: pixelCount),
            paintedPixels: [UInt8](repeating: 0, count: pixelCount)
        )
    }
}

@Observable
@MainActor
final class NovaDocument {
    static let minimumBackdropScale: CGFloat = 0.001
    static let maximumBackdropScale: CGFloat = 64
    static let maximumImageCount = 12

    let width: Int
    let height: Int
    private(set) var pixels: [UInt8]
    private(set) var paintedPixels: [UInt8]
    private(set) var images: [NovaCanvasImage]
    private(set) var selectedImageIndex = 0
    private(set) var fileURL: URL?
    private(set) var isDirty = false
    private(set) var hasSavedViewport = false

    var foregroundColor: UInt8 = 1 {
        didSet {
            if foregroundColor != oldValue { markDirty() }
        }
    }
    var backgroundColor: UInt8 = 0 {
        didSet {
            if backgroundColor != oldValue { markDirty() }
        }
    }
    var currentTool: DrawingTool = .pencil {
        didSet {
            if currentTool != oldValue {
                if currentTool != .select {
                    selection = nil
                }
                markDirty()
            }
        }
    }
    var previousTool: DrawingTool = .pencil {
        didSet {
            if previousTool != oldValue { markDirty() }
        }
    }

    var backdropImage: NSImage? {
        didSet {
            markDirty()
        }
    }
    var backdropOpacity: Double = 0.7 {
        didSet {
            if backdropOpacity != oldValue { markDirty() }
        }
    }
    var backdropImageOpacity: Double = 1.0 {
        didSet {
            if backdropImageOpacity != oldValue { markDirty() }
        }
    }
    var backdropPlacementMode: BackdropPlacementMode = .fitToCanvas {
        didSet {
            if backdropPlacementMode != oldValue { markDirty() }
        }
    }
    var backdropScale: CGFloat = 1.0 {
        didSet {
            if backdropScale != oldValue { markDirty() }
        }
    }
    var backdropOffset: CGPoint = .zero {
        didSet {
            if backdropOffset != oldValue { markDirty() }
        }
    }

    var gridOn: Bool = true {
        didSet {
            if gridOn != oldValue { markDirty() }
        }
    }
    var gridMajor: Int = 16 {
        didSet {
            if gridMajor != oldValue { markDirty() }
        }
    }

    var zoom: CGFloat = 4.0 {
        didSet {
            if zoom != oldValue { markDirty() }
        }
    }
    var panOffset: CGPoint = .zero {
        didSet {
            if panOffset != oldValue { markDirty() }
        }
    }

    var selection: (x: Int, y: Int, w: Int, h: Int)?
    var clipboard: (w: Int, h: Int, pixels: [UInt8], paintedPixels: [UInt8])?

    private var undoStack: [NovaPixelSnapshot] = []
    private var redoStack: [NovaPixelSnapshot] = []
    private var suppressDirtyTracking = 0

    init(width: Int, height: Int) {
        self.width = width
        self.height = height
        self.pixels = [UInt8](repeating: 0, count: width * height)
        self.paintedPixels = [UInt8](repeating: 0, count: width * height)
        self.images = [
            NovaCanvasImage.blank(name: "Image 1", pixelCount: width * height)
        ]
    }

    var displayName: String {
        fileURL?.deletingPathExtension().lastPathComponent ?? "Untitled"
    }

    var suggestedProjectName: String {
        "\(displayName).novadraw"
    }

    var canvasCenter: CGPoint {
        CGPoint(x: CGFloat(width) / 2, y: CGFloat(height) / 2)
    }

    var canAddImage: Bool {
        images.count < Self.maximumImageCount
    }

    var canDeleteImage: Bool {
        images.count > 1
    }

    var selectedImageLabel: String {
        "\(selectedImageIndex + 1)/\(images.count)"
    }

    func markDirty() {
        guard suppressDirtyTracking == 0 else { return }
        isDirty = true
    }

    func markClean() {
        isDirty = false
    }

    func markSaved(to url: URL) {
        fileURL = url
        isDirty = false
    }

    func performWithoutMarkingDirty(_ updates: () -> Void) {
        let wasDirty = isDirty
        suppressDirtyTracking += 1
        updates()
        suppressDirtyTracking -= 1
        isDirty = wasDirty
    }

    func markHasSavedViewport() {
        hasSavedViewport = true
    }

    func selectTool(_ tool: DrawingTool) {
        guard currentTool != tool else { return }
        previousTool = currentTool
        currentTool = tool
    }

    func clearSelection() {
        selection = nil
    }

    @discardableResult
    func addImage() -> Bool {
        guard canAddImage else { return false }
        syncSelectedImageFromCanvas()
        let newIndex = images.count
        images.append(NovaCanvasImage.blank(
            name: "Image \(newIndex + 1)",
            pixelCount: width * height
        ))
        selectImage(at: newIndex, markDirty: false)
        markDirty()
        return true
    }

    @discardableResult
    func deleteSelectedImage() -> Bool {
        guard canDeleteImage, images.indices.contains(selectedImageIndex) else { return false }
        images.remove(at: selectedImageIndex)
        selectedImageIndex = min(selectedImageIndex, images.count - 1)
        loadSelectedImageIntoCanvas()
        clearTransientPixelState()
        markDirty()
        return true
    }

    func selectImage(at index: Int, markDirty: Bool = true) {
        guard images.indices.contains(index), index != selectedImageIndex else { return }
        syncSelectedImageFromCanvas()
        selectedImageIndex = index
        loadSelectedImageIntoCanvas()
        clearTransientPixelState()
        if markDirty {
            self.markDirty()
        }
    }

    func renameImage(at index: Int, to name: String) {
        guard images.indices.contains(index) else { return }
        let normalizedName = Self.normalizedImageName(name, fallbackIndex: index)
        guard images[index].name != normalizedName else { return }
        images[index].name = normalizedName
        markDirty()
    }

    func renameSelectedImage(to name: String) {
        renameImage(at: selectedImageIndex, to: name)
    }

    func configureBackdrop(_ image: NSImage, placement: BackdropPlacementMode) {
        backdropImage = image
        backdropImageOpacity = 1.0
        backdropPlacementMode = placement
        resetBackdropPlacement()
    }

    func resetBackdropPlacement() {
        backdropScale = 1.0
        backdropOffset = .zero
    }

    func moveBackdrop(by delta: CGPoint) {
        backdropOffset = CGPoint(
            x: backdropOffset.x + delta.x,
            y: backdropOffset.y + delta.y
        )
    }

    func scaleBackdrop(by factor: CGFloat, around anchor: CGPoint) {
        let oldScale = Self.clampedBackdropScale(backdropScale)
        setBackdropScale(oldScale * factor, around: anchor)
    }

    func setBackdropScale(_ scale: CGFloat, around anchor: CGPoint) {
        let oldScale = Self.clampedBackdropScale(backdropScale)
        let newScale = Self.clampedBackdropScale(scale)
        guard newScale != oldScale else { return }

        let imageX = (anchor.x - backdropOffset.x) / oldScale
        let imageY = (anchor.y - backdropOffset.y) / oldScale
        backdropScale = newScale
        backdropOffset = CGPoint(
            x: anchor.x - imageX * newScale,
            y: anchor.y - imageY * newScale
        )
    }

    static func backdropScrollScaleFactor(deltaY: CGFloat, hasPreciseScrollingDeltas: Bool) -> CGFloat {
        guard deltaY.isFinite, deltaY != 0 else { return 1 }

        let clampedDelta = min(max(deltaY, -240), 240)
        let sensitivity: CGFloat = hasPreciseScrollingDeltas ? 0.0015 : 0.006
        return CGFloat(exp(Double(clampedDelta * sensitivity)))
    }

    func getPixel(_ x: Int, _ y: Int) -> UInt8 {
        guard x >= 0, x < width, y >= 0, y < height else { return 0 }
        return pixels[y * width + x]
    }

    func isPixelPainted(_ x: Int, _ y: Int) -> Bool {
        guard x >= 0, x < width, y >= 0, y < height else { return false }
        return isPixelPainted(at: y * width + x)
    }

    func isPixelPainted(at index: Int) -> Bool {
        guard index >= 0, index < paintedPixels.count else { return false }
        return paintedPixels[index] != 0
    }

    func setPixel(_ x: Int, _ y: Int, _ color: UInt8) {
        guard x >= 0, x < width, y >= 0, y < height else { return }
        let index = y * width + x
        let newColor = color & 0x0F
        guard pixels[index] != newColor || paintedPixels[index] == 0 else { return }
        pixels[index] = newColor
        paintedPixels[index] = 255
        syncPixelToSelectedImage(at: index)
        markDirty()
    }

    func clearPixel(_ x: Int, _ y: Int) {
        guard x >= 0, x < width, y >= 0, y < height else { return }
        let index = y * width + x
        guard pixels[index] != 0 || paintedPixels[index] != 0 else { return }
        pixels[index] = 0
        paintedPixels[index] = 0
        syncPixelToSelectedImage(at: index)
        markDirty()
    }

    func pushUndo() {
        undoStack.append(makePixelSnapshot())
        redoStack.removeAll()
        if undoStack.count > 100 { undoStack.removeFirst() }
    }

    func undo() {
        guard let prev = undoStack.popLast() else { return }
        redoStack.append(makePixelSnapshot())
        applyPixelSnapshot(prev)
        syncSelectedImageFromCanvas()
        markDirty()
    }

    func redo() {
        guard let next = redoStack.popLast() else { return }
        undoStack.append(makePixelSnapshot())
        applyPixelSnapshot(next)
        syncSelectedImageFromCanvas()
        markDirty()
    }

    var canUndo: Bool { !undoStack.isEmpty }
    var canRedo: Bool { !redoStack.isEmpty }

    func clear() {
        pushUndo()
        pixels = [UInt8](repeating: 0, count: width * height)
        paintedPixels = [UInt8](repeating: 0, count: width * height)
        syncSelectedImageFromCanvas()
        markDirty()
    }

    func loadPixels(_ data: [UInt8]) {
        guard data.count == width * height else { return }
        pushUndo()
        pixels = data
        paintedPixels = Self.solidPaintedPixels(pixelCount: width * height)
        syncSelectedImageFromCanvas()
        markDirty()
    }

    func loadPixelsRaw(_ data: [UInt8], paintedPixels mask: [UInt8]? = nil, markDirty: Bool = false) {
        guard data.count == width * height else { return }
        pixels = data
        paintedPixels = Self.sanitizedPaintedPixels(mask, pixelCount: width * height)
        syncSelectedImageFromCanvas()
        if markDirty {
            self.markDirty()
        }
    }

    func loadImagesRaw(_ loadedImages: [NovaCanvasImage], selectedIndex: Int = 0, markDirty: Bool = false) {
        let pixelCount = width * height
        let validImages = loadedImages
            .prefix(Self.maximumImageCount)
            .filter { $0.pixels.count == pixelCount }
            .enumerated()
            .map { index, image in
                NovaCanvasImage(
                    id: image.id,
                    name: Self.normalizedImageName(image.name, fallbackIndex: index),
                    pixels: image.pixels,
                    paintedPixels: Self.sanitizedPaintedPixels(image.paintedPixels, pixelCount: pixelCount)
                )
            }

        guard !validImages.isEmpty else { return }
        images = Array(validImages)
        selectedImageIndex = min(max(selectedIndex, 0), images.count - 1)
        loadSelectedImageIntoCanvas()
        clearTransientPixelState()
        if markDirty {
            self.markDirty()
        }
    }

    func makePixelSnapshot() -> NovaPixelSnapshot {
        NovaPixelSnapshot(pixels: pixels, paintedPixels: paintedPixels)
    }

    func loadPixelSnapshotRaw(_ snapshot: NovaPixelSnapshot, markDirty: Bool = false) {
        guard snapshot.pixels.count == width * height else { return }
        applyPixelSnapshot(snapshot)
        syncSelectedImageFromCanvas()
        if markDirty {
            self.markDirty()
        }
    }

    func flipHorizontal() {
        pushUndo()
        for y in 0..<height {
            let row = y * width
            for x in 0..<(width / 2) {
                let left = row + x
                let right = row + width - 1 - x
                pixels.swapAt(left, right)
                paintedPixels.swapAt(left, right)
            }
        }
        syncSelectedImageFromCanvas()
        markDirty()
    }

    func flipVertical() {
        pushUndo()
        for y in 0..<(height / 2) {
            let topRow = y * width
            let botRow = (height - 1 - y) * width
            for x in 0..<width {
                pixels.swapAt(topRow + x, botRow + x)
                paintedPixels.swapAt(topRow + x, botRow + x)
            }
        }
        syncSelectedImageFromCanvas()
        markDirty()
    }

    static func clampedBackdropScale(_ scale: CGFloat, fallback: CGFloat = 1.0) -> CGFloat {
        guard scale.isFinite else {
            return clampedBackdropScale(fallback)
        }
        return min(max(scale, minimumBackdropScale), maximumBackdropScale)
    }

    static func legacyPaintedPixels(from pixels: [UInt8]) -> [UInt8] {
        pixels.map { ($0 & 0x0F) == 0 ? 0 : 255 }
    }

    static func solidPaintedPixels(pixelCount: Int) -> [UInt8] {
        [UInt8](repeating: 255, count: pixelCount)
    }

    static func normalizedImageName(_ name: String, fallbackIndex: Int) -> String {
        let trimmed = name.trimmingCharacters(in: .whitespacesAndNewlines)
        return trimmed.isEmpty ? "Image \(fallbackIndex + 1)" : trimmed
    }

    private func syncPixelToSelectedImage(at index: Int) {
        guard images.indices.contains(selectedImageIndex),
              index >= 0, index < images[selectedImageIndex].pixels.count else { return }
        images[selectedImageIndex].pixels[index] = pixels[index]
        images[selectedImageIndex].paintedPixels[index] = paintedPixels[index]
    }

    private func syncSelectedImageFromCanvas() {
        guard images.indices.contains(selectedImageIndex),
              pixels.count == width * height,
              paintedPixels.count == width * height else { return }
        images[selectedImageIndex].pixels = pixels
        images[selectedImageIndex].paintedPixels = paintedPixels
    }

    private func loadSelectedImageIntoCanvas() {
        guard images.indices.contains(selectedImageIndex) else { return }
        pixels = images[selectedImageIndex].pixels
        paintedPixels = Self.sanitizedPaintedPixels(
            images[selectedImageIndex].paintedPixels,
            pixelCount: width * height
        )
        syncSelectedImageFromCanvas()
    }

    private func clearTransientPixelState() {
        selection = nil
        undoStack.removeAll()
        redoStack.removeAll()
    }

    private func applyPixelSnapshot(_ snapshot: NovaPixelSnapshot) {
        pixels = snapshot.pixels
        paintedPixels = Self.sanitizedPaintedPixels(snapshot.paintedPixels, pixelCount: width * height)
    }

    private static func sanitizedPaintedPixels(_ mask: [UInt8]?, pixelCount: Int) -> [UInt8] {
        guard let mask, mask.count == pixelCount else {
            return solidPaintedPixels(pixelCount: pixelCount)
        }
        return mask.map { $0 == 0 ? 0 : 255 }
    }
}
