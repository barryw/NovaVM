# NovaDraw Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a macOS SwiftUI pixel art editor for Nova's 16-color palette with export to NVG1/GFX/Sprite/VSprite formats and PNG backdrop tracing.

**Architecture:** SPM executable using `@Observable` document model, `NSViewRepresentable` wrapping a Core Graphics-based `NSView` for performant pixel rendering. File I/O isolated into pure-function codecs for testability. Tools implemented as a strategy pattern driven by mouse events.

**Tech Stack:** Swift 6, SwiftUI (macOS 14+), AppKit (NSView for canvas), Core Graphics, Swift Package Manager.

**Design doc:** `docs/plans/2026-05-06-novadraw-design.md`

---

## File Tree

```
NovaDraw/
├── Package.swift
├── Sources/
│   ├── App/
│   │   └── NovaDrawApp.swift
│   ├── Models/
│   │   ├── NovaPalette.swift
│   │   ├── NovaDocument.swift
│   │   └── DrawingTool.swift
│   ├── Canvas/
│   │   ├── PixelCanvasView.swift        # NSViewRepresentable bridge
│   │   ├── PixelCanvasNSView.swift      # NSView — rendering + mouse
│   │   └── ToolEngine.swift             # Tool logic (line, fill, etc.)
│   ├── Views/
│   │   ├── ContentView.swift
│   │   ├── PaletteView.swift
│   │   ├── InspectorView.swift
│   │   ├── NewCanvasSheet.swift
│   │   └── StatusBarView.swift
│   └── IO/
│       ├── NvgFormat.swift
│       ├── GfxFormat.swift
│       ├── SpriteFormat.swift
│       ├── VSpriteFormat.swift
│       ├── ProjectFormat.swift
│       └── PaletteQuantizer.swift
└── Tests/
    ├── NvgFormatTests.swift
    ├── GfxFormatTests.swift
    ├── SpriteFormatTests.swift
    ├── VSpriteFormatTests.swift
    ├── ToolEngineTests.swift
    └── DocumentTests.swift
```

---

## Task 1: Project Scaffold

**Files:**
- Create: `NovaDraw/Package.swift`
- Create: `NovaDraw/Sources/App/NovaDrawApp.swift`
- Create: `NovaDraw/Sources/Views/ContentView.swift`
- Create: `NovaDraw/Sources/Models/NovaPalette.swift`

**Step 1: Create Package.swift**

```swift
// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "NovaDraw",
    platforms: [.macOS(.v14)],
    targets: [
        .executableTarget(
            name: "NovaDraw",
            path: "Sources"
        ),
        .testTarget(
            name: "NovaDrawTests",
            dependencies: ["NovaDraw"],
            path: "Tests"
        )
    ]
)
```

**Step 2: Create NovaDrawApp.swift**

```swift
import SwiftUI

@main
struct NovaDrawApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .defaultSize(width: 1200, height: 800)
    }
}
```

**Step 3: Create ContentView.swift (placeholder)**

```swift
import SwiftUI

struct ContentView: View {
    var body: some View {
        Text("NovaDraw")
            .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}
```

**Step 4: Create NovaPalette.swift**

```swift
import AppKit

enum NovaPalette {
    static let colors: [(r: UInt8, g: UInt8, b: UInt8)] = [
        (0,   0,   0),      // 0  Black
        (255, 255, 255),     // 1  White
        (136, 0,   0),       // 2  Red
        (170, 255, 238),     // 3  Cyan
        (204, 68,  204),     // 4  Purple
        (0,   204, 85),      // 5  Green
        (0,   0,   170),     // 6  Blue
        (238, 238, 119),     // 7  Yellow
        (221, 136, 85),      // 8  Orange
        (102, 68,  0),       // 9  Brown
        (255, 119, 119),     // 10 Light Red
        (51,  51,  51),      // 11 Dark Grey
        (119, 119, 119),     // 12 Med Grey
        (170, 255, 102),     // 13 Light Green
        (0,   136, 255),     // 14 Light Blue
        (187, 187, 187),     // 15 Light Grey
    ]

    static let names: [String] = [
        "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue", "Yellow",
        "Orange", "Brown", "Light Red", "Dark Grey", "Med Grey", "Light Green",
        "Light Blue", "Light Grey"
    ]

    static func nsColor(_ index: Int) -> NSColor {
        let c = colors[index & 0x0F]
        return NSColor(red: CGFloat(c.r) / 255, green: CGFloat(c.g) / 255,
                       blue: CGFloat(c.b) / 255, alpha: 1)
    }

    static func swiftUIColor(_ index: Int) -> Color {
        let c = colors[index & 0x0F]
        return Color(red: Double(c.r) / 255, green: Double(c.g) / 255,
                     blue: Double(c.b) / 255)
    }
}
```

**Step 5: Build and run**

```bash
cd NovaDraw && swift build
swift run NovaDraw
```

Expected: Window appears with "NovaDraw" text.

**Step 6: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): scaffold SwiftUI app with Nova palette"
```

---

## Task 2: Document Model + Canvas Data

**Files:**
- Create: `NovaDraw/Sources/Models/NovaDocument.swift`
- Create: `NovaDraw/Sources/Models/DrawingTool.swift`
- Create: `NovaDraw/Tests/DocumentTests.swift`

**Step 1: Create DrawingTool.swift**

```swift
enum DrawingTool: String, CaseIterable {
    case pencil, eraser, line, rect, circle, fill, eyedropper, select
    
    var shortcut: Character {
        switch self {
        case .pencil: "p"
        case .eraser: "e"
        case .line: "l"
        case .rect: "r"
        case .circle: "c"
        case .fill: "f"
        case .eyedropper: "i"
        case .select: "s"
        }
    }
    
    var label: String { rawValue.capitalized }
}
```

**Step 2: Create NovaDocument.swift**

```swift
import SwiftUI

@Observable
@MainActor
final class NovaDocument {
    let width: Int
    let height: Int
    private(set) var pixels: [UInt8]

    var foregroundColor: UInt8 = 1
    var backgroundColor: UInt8 = 0
    var currentTool: DrawingTool = .pencil
    var previousTool: DrawingTool = .pencil

    var backdropImage: NSImage?
    var backdropOpacity: Double = 0.7
    var backdropVisible: Bool = true

    var gridOn: Bool = true
    var gridMajor: Int = 16

    var zoom: CGFloat = 4.0
    var panOffset: CGPoint = .zero

    private var undoStack: [[UInt8]] = []
    private var redoStack: [[UInt8]] = []

    init(width: Int, height: Int) {
        self.width = width
        self.height = height
        self.pixels = [UInt8](repeating: 0, count: width * height)
    }

    func getPixel(_ x: Int, _ y: Int) -> UInt8 {
        guard x >= 0, x < width, y >= 0, y < height else { return 0 }
        return pixels[y * width + x]
    }

    func setPixel(_ x: Int, _ y: Int, _ color: UInt8) {
        guard x >= 0, x < width, y >= 0, y < height else { return }
        pixels[y * width + x] = color & 0x0F
    }

    func pushUndo() {
        undoStack.append(pixels)
        redoStack.removeAll()
        if undoStack.count > 100 { undoStack.removeFirst() }
    }

    func undo() {
        guard let prev = undoStack.popLast() else { return }
        redoStack.append(pixels)
        pixels = prev
    }

    func redo() {
        guard let next = redoStack.popLast() else { return }
        undoStack.append(pixels)
        pixels = next
    }

    var canUndo: Bool { !undoStack.isEmpty }
    var canRedo: Bool { !redoStack.isEmpty }

    func clear() {
        pushUndo()
        pixels = [UInt8](repeating: 0, count: width * height)
    }

    func loadPixels(_ data: [UInt8]) {
        guard data.count == width * height else { return }
        pushUndo()
        pixels = data
    }
}
```

**Step 3: Write tests**

```swift
import Testing
@testable import NovaDraw

@MainActor
@Suite struct DocumentTests {
    @Test func pixelGetSet() {
        let doc = NovaDocument(width: 16, height: 16)
        doc.setPixel(5, 3, 7)
        #expect(doc.getPixel(5, 3) == 7)
    }

    @Test func outOfBoundsReturnsZero() {
        let doc = NovaDocument(width: 16, height: 16)
        #expect(doc.getPixel(-1, 0) == 0)
        #expect(doc.getPixel(16, 0) == 0)
    }

    @Test func colorMaskedTo4Bits() {
        let doc = NovaDocument(width: 16, height: 16)
        doc.setPixel(0, 0, 0xFF)
        #expect(doc.getPixel(0, 0) == 0x0F)
    }

    @Test func undoRedo() {
        let doc = NovaDocument(width: 4, height: 4)
        doc.pushUndo()
        doc.setPixel(0, 0, 5)
        #expect(doc.getPixel(0, 0) == 5)
        doc.undo()
        #expect(doc.getPixel(0, 0) == 0)
        doc.redo()
        #expect(doc.getPixel(0, 0) == 5)
    }

    @Test func clear() {
        let doc = NovaDocument(width: 4, height: 4)
        doc.setPixel(1, 1, 3)
        doc.clear()
        #expect(doc.getPixel(1, 1) == 0)
        #expect(doc.canUndo)
    }
}
```

**Step 4: Run tests**

```bash
cd NovaDraw && swift test
```

Expected: All pass.

**Step 5: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): document model with pixel buffer and undo"
```

---

## Task 3: Canvas Rendering

**Files:**
- Create: `NovaDraw/Sources/Canvas/PixelCanvasNSView.swift`
- Create: `NovaDraw/Sources/Canvas/PixelCanvasView.swift`
- Modify: `NovaDraw/Sources/Views/ContentView.swift`

**Step 1: Create PixelCanvasNSView.swift**

This is the core rendering view. Uses Core Graphics to draw the pixel buffer as a scaled bitmap with nearest-neighbor interpolation.

```swift
import AppKit

final class PixelCanvasNSView: NSView {
    weak var document: NovaDocument?

    private var trackingArea: NSTrackingArea?
    var onMouseDown: ((NSEvent) -> Void)?
    var onMouseDragged: ((NSEvent) -> Void)?
    var onMouseUp: ((NSEvent) -> Void)?
    var onMouseMoved: ((NSEvent) -> Void)?
    var onScrollWheel: ((NSEvent) -> Void)?

    override var isFlipped: Bool { true }
    override var acceptsFirstResponder: Bool { true }

    override func updateTrackingAreas() {
        super.updateTrackingAreas()
        if let ta = trackingArea { removeTrackingArea(ta) }
        let ta = NSTrackingArea(
            rect: bounds,
            options: [.mouseMoved, .activeInKeyWindow, .inVisibleRect],
            owner: self
        )
        addTrackingArea(ta)
        trackingArea = ta
    }

    func pixelCoord(from event: NSEvent) -> (x: Int, y: Int)? {
        guard let doc = document else { return nil }
        let loc = convert(event.locationInWindow, from: nil)
        let x = Int(floor((loc.x - doc.panOffset.x) / doc.zoom))
        let y = Int(floor((loc.y - doc.panOffset.y) / doc.zoom))
        guard x >= 0, x < doc.width, y >= 0, y < doc.height else { return nil }
        return (x, y)
    }

    override func draw(_ dirtyRect: NSRect) {
        guard let doc = document, let ctx = NSGraphicsContext.current?.cgContext else { return }

        ctx.setFillColor(NSColor.darkGray.cgColor)
        ctx.fill(bounds)

        let zoom = doc.zoom
        let ox = doc.panOffset.x
        let oy = doc.panOffset.y

        // Draw backdrop
        if doc.backdropVisible, let backdrop = doc.backdropImage {
            let dest = CGRect(x: ox, y: oy,
                              width: CGFloat(doc.width) * zoom,
                              height: CGFloat(doc.height) * zoom)
            backdrop.draw(in: NSRect(origin: dest.origin, size: dest.size),
                          from: .zero, operation: .sourceOver, fraction: 1.0)
        }

        // Build RGBA bitmap from pixel buffer
        let w = doc.width, h = doc.height
        var rgba = [UInt8](repeating: 0, count: w * h * 4)
        let opacity = doc.backdropVisible ? doc.backdropOpacity : 1.0
        let alpha = UInt8(opacity * 255)
        for i in 0..<(w * h) {
            let c = NovaPalette.colors[Int(doc.pixels[i]) & 0x0F]
            rgba[i * 4] = c.r
            rgba[i * 4 + 1] = c.g
            rgba[i * 4 + 2] = c.b
            rgba[i * 4 + 3] = doc.pixels[i] == 0 && doc.backdropVisible ? 0 : alpha
        }

        let colorSpace = CGColorSpaceCreateDeviceRGB()
        if let provider = CGDataProvider(data: Data(rgba) as CFData),
           let image = CGImage(width: w, height: h, bitsPerComponent: 8,
                               bitsPerPixel: 32, bytesPerRow: w * 4,
                               space: colorSpace,
                               bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue),
                               provider: provider, decode: nil,
                               shouldInterpolate: false, intent: .defaultIntent) {
            let dest = CGRect(x: ox, y: oy,
                              width: CGFloat(w) * zoom, height: CGFloat(h) * zoom)
            ctx.interpolationQuality = .none
            ctx.draw(image, in: dest)
        }

        // Draw grid
        if doc.gridOn {
            drawGrid(ctx: ctx, doc: doc)
        }
    }

    private func drawGrid(ctx: CGContext, doc: NovaDocument) {
        let zoom = doc.zoom
        let ox = doc.panOffset.x
        let oy = doc.panOffset.y
        let w = doc.width, h = doc.height

        if zoom >= 4 {
            ctx.setStrokeColor(NSColor(white: 0.5, alpha: 0.3).cgColor)
            ctx.setLineWidth(0.5)
            for x in 0...w {
                let px = ox + CGFloat(x) * zoom
                ctx.move(to: CGPoint(x: px, y: oy))
                ctx.addLine(to: CGPoint(x: px, y: oy + CGFloat(h) * zoom))
            }
            for y in 0...h {
                let py = oy + CGFloat(y) * zoom
                ctx.move(to: CGPoint(x: ox, y: py))
                ctx.addLine(to: CGPoint(x: ox + CGFloat(w) * zoom, y: py))
            }
            ctx.strokePath()
        }

        let major = doc.gridMajor
        ctx.setStrokeColor(NSColor(white: 0.5, alpha: 0.6).cgColor)
        ctx.setLineWidth(1.0)
        for x in stride(from: 0, through: w, by: major) {
            let px = ox + CGFloat(x) * zoom
            ctx.move(to: CGPoint(x: px, y: oy))
            ctx.addLine(to: CGPoint(x: px, y: oy + CGFloat(h) * zoom))
        }
        for y in stride(from: 0, through: h, by: major) {
            let py = oy + CGFloat(y) * zoom
            ctx.move(to: CGPoint(x: ox, y: py))
            ctx.addLine(to: CGPoint(x: ox + CGFloat(w) * zoom, y: py))
        }
        ctx.strokePath()
    }

    override func mouseDown(with event: NSEvent) { onMouseDown?(event) }
    override func mouseDragged(with event: NSEvent) { onMouseDragged?(event) }
    override func mouseUp(with event: NSEvent) { onMouseUp?(event) }
    override func mouseMoved(with event: NSEvent) { onMouseMoved?(event) }
    override func scrollWheel(with event: NSEvent) { onScrollWheel?(event) }

    override func rightMouseDown(with event: NSEvent) {
        guard let doc = document, let coord = pixelCoord(from: event) else { return }
        doc.backgroundColor = doc.getPixel(coord.x, coord.y)
    }
}
```

**Step 2: Create PixelCanvasView.swift (NSViewRepresentable)**

```swift
import SwiftUI

struct PixelCanvasView: NSViewRepresentable {
    let document: NovaDocument
    @Binding var cursorPosition: (x: Int, y: Int)?
    let toolEngine: ToolEngine

    func makeNSView(context: Context) -> PixelCanvasNSView {
        let view = PixelCanvasNSView()
        view.document = document
        setupHandlers(view)
        return view
    }

    func updateNSView(_ nsView: PixelCanvasNSView, context: Context) {
        nsView.document = document
        nsView.needsDisplay = true
    }

    private func setupHandlers(_ view: PixelCanvasNSView) {
        view.onMouseDown = { event in
            guard let coord = view.pixelCoord(from: event) else { return }
            toolEngine.mouseDown(x: coord.x, y: coord.y, shift: event.modifierFlags.contains(.shift))
            view.needsDisplay = true
        }
        view.onMouseDragged = { event in
            guard let coord = view.pixelCoord(from: event) else { return }
            toolEngine.mouseDragged(x: coord.x, y: coord.y, shift: event.modifierFlags.contains(.shift))
            view.needsDisplay = true
        }
        view.onMouseUp = { event in
            let coord = view.pixelCoord(from: event)
            toolEngine.mouseUp(x: coord?.x ?? 0, y: coord?.y ?? 0)
            view.needsDisplay = true
        }
        view.onMouseMoved = { event in
            cursorPosition = view.pixelCoord(from: event)
        }
        view.onScrollWheel = { event in
            if event.modifierFlags.contains(.command) || event.momentumPhase != [] {
                return
            }
            let oldZoom = document.zoom
            let factor: CGFloat = event.scrollingDeltaY > 0 ? 1.25 : 0.8
            let newZoom = max(1, min(32, oldZoom * factor))

            let loc = view.convert(event.locationInWindow, from: nil)
            let canvasX = (loc.x - document.panOffset.x) / oldZoom
            let canvasY = (loc.y - document.panOffset.y) / oldZoom
            document.zoom = newZoom
            document.panOffset.x = loc.x - canvasX * newZoom
            document.panOffset.y = loc.y - canvasY * newZoom

            view.needsDisplay = true
        }
    }
}
```

**Step 3: Update ContentView.swift**

```swift
import SwiftUI

struct ContentView: View {
    @State private var document = NovaDocument(width: 320, height: 200)
    @State private var toolEngine: ToolEngine?
    @State private var cursorPosition: (x: Int, y: Int)?
    @State private var showNewCanvas = true

    var body: some View {
        if let engine = toolEngine {
            mainEditor(engine: engine)
        } else {
            NewCanvasSheet { w, h in
                document = NovaDocument(width: w, height: h)
                toolEngine = ToolEngine(document: document)
                showNewCanvas = false
            }
        }
    }

    @ViewBuilder
    private func mainEditor(engine: ToolEngine) -> some View {
        VStack(spacing: 0) {
            toolbarRow
            Divider()
            HStack(spacing: 0) {
                PaletteView(document: document)
                    .frame(width: 52)
                Divider()
                PixelCanvasView(document: document,
                                cursorPosition: $cursorPosition,
                                toolEngine: engine)
                Divider()
                InspectorView(document: document)
                    .frame(width: 200)
            }
            Divider()
            statusBar
        }
    }

    private var toolbarRow: some View {
        HStack {
            ForEach(DrawingTool.allCases, id: \.self) { tool in
                Button(tool.label) { document.currentTool = tool }
                    .buttonStyle(.bordered)
                    .tint(document.currentTool == tool ? .accentColor : nil)
                    .keyboardShortcut(KeyEquivalent(tool.shortcut), modifiers: [])
            }
            Spacer()
            Toggle("Grid", isOn: Binding(
                get: { document.gridOn },
                set: { document.gridOn = $0 }
            ))
            .keyboardShortcut("g", modifiers: [])
            Text("Zoom: \(Int(document.zoom))x")
                .monospacedDigit()
                .frame(width: 80)
        }
        .padding(.horizontal, 8)
        .padding(.vertical, 4)
    }

    private var statusBar: some View {
        HStack {
            if let pos = cursorPosition {
                Text("(\(pos.x), \(pos.y))")
                    .monospacedDigit()
            } else {
                Text("—")
            }
            Spacer()
            Text("\(document.width)×\(document.height)")
            Text("\(Int(document.zoom))x")
                .monospacedDigit()
        }
        .font(.caption)
        .padding(.horizontal, 8)
        .padding(.vertical, 2)
    }
}
```

**Step 4: Create stub files for compilation**

Create `NovaDraw/Sources/Canvas/ToolEngine.swift`:

```swift
import Foundation

@Observable
@MainActor
final class ToolEngine {
    let document: NovaDocument

    init(document: NovaDocument) {
        self.document = document
    }

    func mouseDown(x: Int, y: Int, shift: Bool) {
        document.pushUndo()
        applyTool(x: x, y: y, shift: shift)
    }

    func mouseDragged(x: Int, y: Int, shift: Bool) {
        applyTool(x: x, y: y, shift: shift)
    }

    func mouseUp(x: Int, y: Int) {}

    private func applyTool(x: Int, y: Int, shift: Bool) {
        switch document.currentTool {
        case .pencil:
            document.setPixel(x, y, document.foregroundColor)
        case .eraser:
            document.setPixel(x, y, 0)
        default:
            break
        }
    }
}
```

Create `NovaDraw/Sources/Views/PaletteView.swift`:

```swift
import SwiftUI

struct PaletteView: View {
    let document: NovaDocument

    var body: some View {
        VStack(spacing: 2) {
            HStack(spacing: 2) {
                Rectangle()
                    .fill(NovaPalette.swiftUIColor(Int(document.foregroundColor)))
                    .frame(width: 20, height: 20)
                    .overlay(Rectangle().stroke(.white, lineWidth: 1))
                Rectangle()
                    .fill(NovaPalette.swiftUIColor(Int(document.backgroundColor)))
                    .frame(width: 20, height: 20)
                    .overlay(Rectangle().stroke(.gray, lineWidth: 1))
            }
            .padding(.bottom, 4)

            ForEach(0..<16, id: \.self) { i in
                Rectangle()
                    .fill(NovaPalette.swiftUIColor(i))
                    .frame(width: 40, height: 20)
                    .overlay(
                        Rectangle().stroke(
                            document.foregroundColor == UInt8(i) ? .white : .clear,
                            lineWidth: 2
                        )
                    )
                    .onTapGesture { document.foregroundColor = UInt8(i) }
            }
            Spacer()
        }
        .padding(4)
    }
}
```

Create `NovaDraw/Sources/Views/InspectorView.swift`:

```swift
import SwiftUI

struct InspectorView: View {
    let document: NovaDocument

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Section("Canvas") {
                Text("\(document.width) × \(document.height)")
            }
            Section("Backdrop") {
                Toggle("Visible", isOn: Binding(
                    get: { document.backdropVisible },
                    set: { document.backdropVisible = $0 }
                ))
                HStack {
                    Text("Drawing Opacity")
                    Slider(
                        value: Binding(
                            get: { document.backdropOpacity },
                            set: { document.backdropOpacity = $0 }
                        ),
                        in: 0...1
                    )
                }
                Button("Load Backdrop...") { loadBackdrop() }
            }
            Section("Grid") {
                Picker("Major Lines", selection: Binding(
                    get: { document.gridMajor },
                    set: { document.gridMajor = $0 }
                )) {
                    Text("8px").tag(8)
                    Text("16px").tag(16)
                }
                .pickerStyle(.segmented)
            }
            Spacer()
        }
        .padding(8)
    }

    private func loadBackdrop() {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.png, .jpeg]
        panel.allowsMultipleSelection = false
        if panel.runModal() == .OK, let url = panel.url {
            document.backdropImage = NSImage(contentsOf: url)
        }
    }
}
```

Create `NovaDraw/Sources/Views/NewCanvasSheet.swift`:

```swift
import SwiftUI

struct NewCanvasSheet: View {
    let onCreate: (Int, Int) -> Void
    @State private var selectedPreset = 3
    @State private var customWidth = 64
    @State private var customHeight = 64

    private let presets: [(String, Int, Int)] = [
        ("Sprite (16×16)", 16, 16),
        ("32×32", 32, 32),
        ("Custom", 0, 0),
        ("Full Screen (320×200)", 320, 200),
    ]

    var body: some View {
        VStack(spacing: 16) {
            Text("New Canvas").font(.title2)
            Picker("Size", selection: $selectedPreset) {
                ForEach(0..<presets.count, id: \.self) { i in
                    Text(presets[i].0).tag(i)
                }
            }
            .pickerStyle(.radioGroup)

            if selectedPreset == 2 {
                HStack {
                    TextField("W", value: $customWidth, format: .number)
                        .frame(width: 60)
                    Text("×")
                    TextField("H", value: $customHeight, format: .number)
                        .frame(width: 60)
                }
            }

            Button("Create") {
                let (w, h): (Int, Int)
                if selectedPreset == 2 {
                    w = min(max(customWidth, 1), 320)
                    h = min(max(customHeight, 1), 200)
                } else {
                    w = presets[selectedPreset].1
                    h = presets[selectedPreset].2
                }
                onCreate(w, h)
            }
            .keyboardShortcut(.return)
        }
        .padding(32)
        .frame(width: 300)
    }
}
```

**Step 5: Build and run**

```bash
cd NovaDraw && swift build && swift run NovaDraw
```

Expected: New Canvas dialog → select size → editor appears with palette, canvas, grid, pencil drawing works.

**Step 6: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): canvas rendering with grid, palette, and pencil/eraser"
```

---

## Task 4: Drawing Tools — Line, Rect, Circle, Fill, Eyedropper

**Files:**
- Modify: `NovaDraw/Sources/Canvas/ToolEngine.swift`
- Create: `NovaDraw/Tests/ToolEngineTests.swift`

**Step 1: Write tests**

```swift
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
    }

    @Test func eraserClearsPixel() {
        let (doc, engine) = makeDoc()
        doc.setPixel(5, 5, 7)
        doc.currentTool = .eraser
        engine.mouseDown(x: 5, y: 5, shift: false)
        #expect(doc.getPixel(5, 5) == 0)
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

    @Test func rectOutline() {
        let (doc, engine) = makeDoc()
        doc.currentTool = .rect
        engine.mouseDown(x: 1, y: 1, shift: false)
        engine.mouseUp(x: 4, y: 3)
        // top and bottom edges
        for x in 1...4 {
            #expect(doc.getPixel(x, 1) == 1)
            #expect(doc.getPixel(x, 3) == 1)
        }
        // left and right edges
        for y in 1...3 {
            #expect(doc.getPixel(1, y) == 1)
            #expect(doc.getPixel(4, y) == 1)
        }
        // interior empty
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
        // Draw a box outline in color 2
        doc.foregroundColor = 2
        doc.currentTool = .rect
        engine.mouseDown(x: 1, y: 1, shift: false)
        engine.mouseUp(x: 6, y: 6)
        // Fill inside with color 3
        doc.foregroundColor = 3
        doc.currentTool = .fill
        engine.mouseDown(x: 3, y: 3, shift: false)
        #expect(doc.getPixel(3, 3) == 3)
        #expect(doc.getPixel(2, 2) == 3)
        // Border untouched
        #expect(doc.getPixel(1, 1) == 2)
        // Outside untouched
        #expect(doc.getPixel(0, 0) == 0)
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
        engine.mouseUp(x: 11, y: 8)  // radius 3
        // center should be empty (outline only)
        #expect(doc.getPixel(8, 8) == 0)
        // top of circle
        #expect(doc.getPixel(8, 5) == 1)
    }
}
```

**Step 2: Run tests — expect failures**

```bash
cd NovaDraw && swift test 2>&1 | tail -20
```

**Step 3: Implement full ToolEngine**

Replace `ToolEngine.swift` with:

```swift
import Foundation

@Observable
@MainActor
final class ToolEngine {
    let document: NovaDocument

    private var dragStart: (x: Int, y: Int)?
    private var dragShift = false
    private var previewPixels: [UInt8]?

    init(document: NovaDocument) {
        self.document = document
    }

    func mouseDown(x: Int, y: Int, shift: Bool) {
        dragStart = (x, y)
        dragShift = shift

        switch document.currentTool {
        case .pencil:
            document.pushUndo()
            document.setPixel(x, y, document.foregroundColor)
        case .eraser:
            document.pushUndo()
            document.setPixel(x, y, 0)
        case .fill:
            document.pushUndo()
            floodFill(x: x, y: y, newColor: document.foregroundColor)
        case .eyedropper:
            document.foregroundColor = document.getPixel(x, y)
            document.currentTool = document.previousTool
        case .line, .rect, .circle:
            previewPixels = document.pixels
            document.pushUndo()
        case .select:
            break
        }
    }

    func mouseDragged(x: Int, y: Int, shift: Bool) {
        guard let start = dragStart else { return }

        switch document.currentTool {
        case .pencil:
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
            dragStart = (x, y)
        case .eraser:
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: 0)
            dragStart = (x, y)
        case .line:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
        case .rect:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            drawRect(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor, filled: shift)
        case .circle:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            let r = Int(sqrt(Double((x - start.x) * (x - start.x) + (y - start.y) * (y - start.y))).rounded())
            drawCircle(cx: start.x, cy: start.y, r: r, color: document.foregroundColor, filled: shift)
        default:
            break
        }
    }

    func mouseUp(x: Int, y: Int) {
        guard let start = dragStart else { return }

        switch document.currentTool {
        case .line:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            drawLine(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor)
        case .rect:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            drawRect(x0: start.x, y0: start.y, x1: x, y1: y, color: document.foregroundColor, filled: dragShift)
        case .circle:
            if let base = previewPixels { document.loadPixelsRaw(base) }
            let r = Int(sqrt(Double((x - start.x) * (x - start.x) + (y - start.y) * (y - start.y))).rounded())
            drawCircle(cx: start.x, cy: start.y, r: r, color: document.foregroundColor, filled: dragShift)
        default:
            break
        }

        dragStart = nil
        previewPixels = nil
    }

    // MARK: - Algorithms

    func drawLine(x0: Int, y0: Int, x1: Int, y1: Int, color: UInt8) {
        var x0 = x0, y0 = y0
        let dx = abs(x1 - x0)
        let dy = -abs(y1 - y0)
        let sx = x0 < x1 ? 1 : -1
        let sy = y0 < y1 ? 1 : -1
        var err = dx + dy
        while true {
            document.setPixel(x0, y0, color)
            if x0 == x1 && y0 == y1 { break }
            let e2 = 2 * err
            if e2 >= dy { err += dy; x0 += sx }
            if e2 <= dx { err += dx; y0 += sy }
        }
    }

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

    func floodFill(x: Int, y: Int, newColor: UInt8) {
        let target = document.getPixel(x, y)
        if target == newColor { return }
        var stack = [(x, y)]
        while let (cx, cy) = stack.popLast() {
            guard cx >= 0, cx < document.width, cy >= 0, cy < document.height,
                  document.getPixel(cx, cy) == target else { continue }
            document.setPixel(cx, cy, newColor)
            stack.append((cx + 1, cy))
            stack.append((cx - 1, cy))
            stack.append((cx, cy + 1))
            stack.append((cx, cy - 1))
        }
    }
}
```

Add `loadPixelsRaw` to `NovaDocument.swift` (raw load without undo push):

```swift
func loadPixelsRaw(_ data: [UInt8]) {
    guard data.count == width * height else { return }
    pixels = data
}
```

**Step 4: Run tests**

```bash
cd NovaDraw && swift test
```

Expected: All pass.

**Step 5: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): line, rect, circle, flood fill, eyedropper tools"
```

---

## Task 5: Selection Tool

**Files:**
- Modify: `NovaDraw/Sources/Canvas/ToolEngine.swift`
- Modify: `NovaDraw/Sources/Canvas/PixelCanvasNSView.swift`
- Modify: `NovaDraw/Sources/Models/NovaDocument.swift`

**Step 1: Add selection state to NovaDocument**

```swift
// Add to NovaDocument:
var selection: (x: Int, y: Int, w: Int, h: Int)?
var clipboard: (w: Int, h: Int, pixels: [UInt8])?
```

**Step 2: Add select tool handling to ToolEngine**

In `mouseDown`, for `.select`:
- If click is inside existing selection, begin move (store offset)
- If click is outside, start new selection rectangle

In `mouseDragged`, for `.select`:
- If moving, offset the selection contents
- If selecting, update the selection rectangle

In `mouseUp`, for `.select`:
- Finalize the selection rectangle

Add `copySelection()` and `pasteClipboard()` methods:
- Copy: extract pixels from selection rect into `document.clipboard`
- Paste: push undo, stamp clipboard at selection origin
- Delete: push undo, fill selection with color 0

**Step 3: Add selection overlay rendering to PixelCanvasNSView**

Draw a dashed rectangle (marching ants) around `document.selection` in the `draw` method.

**Step 4: Wire Cmd+C / Cmd+V / Delete keyboard shortcuts**

Add to `ContentView` toolbar or handle via `.onKeyPress` or menu commands.

**Step 5: Test manually — build and run**

```bash
cd NovaDraw && swift build && swift run NovaDraw
```

Expected: Can select region, move it, copy/paste, delete.

**Step 6: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): selection tool with move, copy, paste, delete"
```

---

## Task 6: Zoom Keyboard Shortcuts + Fit-to-Window + Pan

**Files:**
- Modify: `NovaDraw/Sources/Views/ContentView.swift`

**Step 1: Add zoom presets and fit-to-window**

Add to `ContentView`:

```swift
private func setZoom(_ level: CGFloat) {
    document.zoom = level
    centerCanvas()
}

private func fitToWindow(viewSize: CGSize) {
    let scaleX = viewSize.width / CGFloat(document.width)
    let scaleY = viewSize.height / CGFloat(document.height)
    document.zoom = min(scaleX, scaleY)
    centerCanvas()
}

private func centerCanvas() {
    // center will be calculated based on the canvas view bounds
    document.panOffset = .zero
}
```

**Step 2: Add keyboard handling**

Wire number keys 1-5 for zoom levels 1x, 2x, 4x, 8x, 16x. Cmd+0 for fit. Space+drag for pan (already handled via NSView event forwarding — add space-drag detection to PixelCanvasNSView).

**Step 3: Add Cmd+B for backdrop toggle, Cmd+Z/Shift+Cmd+Z for undo/redo**

```swift
.onKeyPress("b", modifiers: .command) {
    document.backdropVisible.toggle()
    return .handled
}
```

**Step 4: Build and test manually**

```bash
cd NovaDraw && swift build && swift run NovaDraw
```

**Step 5: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): zoom presets, fit-to-window, pan, keyboard shortcuts"
```

---

## Task 7: File I/O — Export Formats

**Files:**
- Create: `NovaDraw/Sources/IO/NvgFormat.swift`
- Create: `NovaDraw/Sources/IO/GfxFormat.swift`
- Create: `NovaDraw/Sources/IO/SpriteFormat.swift`
- Create: `NovaDraw/Sources/IO/VSpriteFormat.swift`
- Create: `NovaDraw/Tests/NvgFormatTests.swift`
- Create: `NovaDraw/Tests/GfxFormatTests.swift`
- Create: `NovaDraw/Tests/SpriteFormatTests.swift`
- Create: `NovaDraw/Tests/VSpriteFormatTests.swift`

**Step 1: Write NVG1 tests**

```swift
import Testing
import Foundation
@testable import NovaDraw

@Suite struct NvgFormatTests {
    @Test func roundTrip() {
        var pixels = [UInt8](repeating: 0, count: 320 * 200)
        // Draw a short horizontal line
        for x in 10..<15 { pixels[0 * 320 + x] = 7 }
        // Draw a pixel on row 100
        pixels[100 * 320 + 50] = 3

        let data = NvgFormat.encode(pixels: pixels, width: 320, height: 200)

        // Check magic
        #expect(data[0] == 0x4E) // 'N'
        #expect(data[1] == 0x56) // 'V'
        #expect(data[2] == 0x47) // 'G'
        #expect(data[3] == 0x31) // '1'

        let decoded = NvgFormat.decode(data: data)!
        #expect(decoded.width == 320)
        #expect(decoded.height == 200)
        #expect(decoded.pixels.count == 320 * 200)
        for x in 10..<15 { #expect(decoded.pixels[x] == 7) }
        #expect(decoded.pixels[100 * 320 + 50] == 3)
        #expect(decoded.pixels[0] == 0)
    }

    @Test func allBlackIsMinimal() {
        let pixels = [UInt8](repeating: 0, count: 320 * 200)
        let data = NvgFormat.encode(pixels: pixels, width: 320, height: 200)
        // Header only: 4 magic + 2 width + 2 height + 4 span_count = 12 bytes
        #expect(data.count == 12)
    }
}
```

**Step 2: Write GFX tests**

```swift
import Testing
import Foundation
@testable import NovaDraw

@Suite struct GfxFormatTests {
    @Test func roundTrip() {
        var pixels = [UInt8](repeating: 0, count: 320 * 200)
        pixels[0] = 5; pixels[63999] = 15
        let data = GfxFormat.encode(pixels: pixels)
        #expect(data.count == 64000)
        let decoded = GfxFormat.decode(data: data, width: 320, height: 200)!
        #expect(decoded[0] == 5)
        #expect(decoded[63999] == 15)
    }
}
```

**Step 3: Write Sprite tests**

```swift
import Testing
import Foundation
@testable import NovaDraw

@Suite struct SpriteFormatTests {
    @Test func singleSprite() {
        var pixels = [UInt8](repeating: 0, count: 16 * 16)
        pixels[0] = 1   // top-left
        pixels[1] = 2   // second pixel
        pixels[16] = 5  // first pixel of row 1

        let data = SpriteFormat.encode(pixels: pixels, width: 16, height: 16)
        #expect(data.count == 128)
        // byte 0: low nibble = pixel 0 (1), high nibble = pixel 1 (2)
        #expect(data[0] == 0x21)
        // byte 8: row 1, pixel 0 = 5
        #expect(data[8] == 0x05)

        let decoded = SpriteFormat.decode(data: data)!
        #expect(decoded.width == 16)
        #expect(decoded.height == 16)
        #expect(decoded.pixels[0] == 1)
        #expect(decoded.pixels[1] == 2)
        #expect(decoded.pixels[16] == 5)
    }

    @Test func spriteSheet() {
        // 32x32 canvas = 4 sprites
        var pixels = [UInt8](repeating: 0, count: 32 * 32)
        pixels[0] = 1                     // sprite 0, top-left
        pixels[16] = 2                    // sprite 1, top-left
        pixels[32 * 16] = 3              // sprite 2, top-left
        pixels[32 * 16 + 16] = 4         // sprite 3, top-left

        let data = SpriteFormat.encodeSheet(pixels: pixels, width: 32, height: 32)
        #expect(data.count == 128 * 4)
    }
}
```

**Step 4: Write VSprite tests**

```swift
import Testing
import Foundation
@testable import NovaDraw

@Suite struct VSpriteFormatTests {
    @Test func roundTrip() {
        let pixels: [UInt8] = [1, 2, 3, 4, 5, 6]
        let data = VSpriteFormat.encode(pixels: pixels, width: 3, height: 2)
        // 4-byte header + 6 pixels
        #expect(data.count == 10)

        let decoded = VSpriteFormat.decode(data: data)!
        #expect(decoded.width == 3)
        #expect(decoded.height == 2)
        #expect(decoded.pixels == [1, 2, 3, 4, 5, 6])
    }
}
```

**Step 5: Run tests — expect failures**

```bash
cd NovaDraw && swift test 2>&1 | tail -20
```

**Step 6: Implement NvgFormat.swift**

```swift
import Foundation

enum NvgFormat {
    struct Decoded {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var spans: [(addr: UInt16, data: [UInt8])] = []
        for y in 0..<height {
            var x = 0
            let row = y * width
            while x < width {
                if pixels[row + x] == 0 { x += 1; continue }
                let start = x
                var run: [UInt8] = []
                while x < width && pixels[row + x] != 0 && run.count < 255 {
                    run.append(pixels[row + x])
                    x += 1
                }
                spans.append((UInt16(row + start), run))
            }
        }

        var data = Data()
        data.append(contentsOf: [0x4E, 0x56, 0x47, 0x31]) // "NVG1"
        data.append(contentsOf: withUnsafeBytes(of: UInt16(width).littleEndian) { Array($0) })
        data.append(contentsOf: withUnsafeBytes(of: UInt16(height).littleEndian) { Array($0) })
        data.append(contentsOf: withUnsafeBytes(of: UInt32(spans.count).littleEndian) { Array($0) })
        for span in spans {
            data.append(contentsOf: withUnsafeBytes(of: span.addr.littleEndian) { Array($0) })
            data.append(UInt8(span.data.count))
            data.append(contentsOf: span.data)
        }
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 12,
              data[0] == 0x4E, data[1] == 0x56, data[2] == 0x47, data[3] == 0x31
        else { return nil }

        let width = Int(data[4]) | (Int(data[5]) << 8)
        let height = Int(data[6]) | (Int(data[7]) << 8)
        let spanCount = Int(data[8]) | (Int(data[9]) << 8) | (Int(data[10]) << 16) | (Int(data[11]) << 24)

        var pixels = [UInt8](repeating: 0, count: width * height)
        var offset = 12
        for _ in 0..<spanCount {
            guard offset + 3 <= data.count else { return nil }
            let addr = Int(data[offset]) | (Int(data[offset + 1]) << 8)
            let len = Int(data[offset + 2])
            offset += 3
            guard offset + len <= data.count else { return nil }
            for i in 0..<len {
                let idx = addr + i
                if idx < pixels.count { pixels[idx] = data[offset + i] }
            }
            offset += len
        }
        return Decoded(width: width, height: height, pixels: pixels)
    }
}
```

**Step 7: Implement GfxFormat.swift**

```swift
import Foundation

enum GfxFormat {
    static func encode(pixels: [UInt8]) -> Data {
        Data(pixels)
    }

    static func decode(data: Data, width: Int, height: Int) -> [UInt8]? {
        guard data.count == width * height else { return nil }
        return Array(data)
    }
}
```

**Step 8: Implement SpriteFormat.swift**

```swift
import Foundation

enum SpriteFormat {
    struct Decoded {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        // Single 16x16 sprite
        var data = Data(count: 128)
        for y in 0..<16 {
            for x in stride(from: 0, to: 16, by: 2) {
                let left = (y < height && x < width) ? pixels[y * width + x] & 0x0F : 0
                let right = (y < height && x + 1 < width) ? pixels[y * width + x + 1] & 0x0F : 0
                data[y * 8 + x / 2] = left | (right << 4)
            }
        }
        return data
    }

    static func encodeSheet(pixels: [UInt8], width: Int, height: Int) -> Data {
        var data = Data()
        let cols = width / 16
        let rows = height / 16
        for row in 0..<rows {
            for col in 0..<cols {
                var sprite = [UInt8](repeating: 0, count: 16 * 16)
                for y in 0..<16 {
                    for x in 0..<16 {
                        let srcX = col * 16 + x
                        let srcY = row * 16 + y
                        if srcX < width && srcY < height {
                            sprite[y * 16 + x] = pixels[srcY * width + srcX]
                        }
                    }
                }
                data.append(encode(pixels: sprite, width: 16, height: 16))
            }
        }
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 128 else { return nil }
        let spriteCount = data.count / 128
        let cols = spriteCount == 1 ? 1 : Int(ceil(sqrt(Double(spriteCount))))
        let rows = spriteCount == 1 ? 1 : (spriteCount + cols - 1) / cols
        let width = cols * 16
        let height = rows * 16
        var pixels = [UInt8](repeating: 0, count: width * height)

        for s in 0..<spriteCount {
            let col = s % cols
            let row = s / cols
            let base = s * 128
            for y in 0..<16 {
                for x in stride(from: 0, to: 16, by: 2) {
                    let b = data[base + y * 8 + x / 2]
                    let dstX = col * 16 + x
                    let dstY = row * 16 + y
                    pixels[dstY * width + dstX] = b & 0x0F
                    pixels[dstY * width + dstX + 1] = (b >> 4) & 0x0F
                }
            }
        }
        return Decoded(width: width, height: height, pixels: pixels)
    }
}
```

**Step 9: Implement VSpriteFormat.swift**

```swift
import Foundation

enum VSpriteFormat {
    struct Decoded {
        let width: Int
        let height: Int
        let pixels: [UInt8]
    }

    static func encode(pixels: [UInt8], width: Int, height: Int) -> Data {
        var data = Data()
        data.append(contentsOf: withUnsafeBytes(of: UInt16(width).littleEndian) { Array($0) })
        data.append(contentsOf: withUnsafeBytes(of: UInt16(height).littleEndian) { Array($0) })
        data.append(contentsOf: pixels)
        return data
    }

    static func decode(data: Data) -> Decoded? {
        guard data.count >= 4 else { return nil }
        let width = Int(data[0]) | (Int(data[1]) << 8)
        let height = Int(data[2]) | (Int(data[3]) << 8)
        guard data.count == 4 + width * height else { return nil }
        return Decoded(width: width, height: height, pixels: Array(data[4...]))
    }
}
```

**Step 10: Run tests**

```bash
cd NovaDraw && swift test
```

Expected: All pass.

**Step 11: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): NVG1, GFX, Sprite, VSprite format codecs with tests"
```

---

## Task 8: File I/O — Import + PNG Quantizer

**Files:**
- Create: `NovaDraw/Sources/IO/PaletteQuantizer.swift`
- Create: `NovaDraw/Sources/IO/ProjectFormat.swift`
- Modify: `NovaDraw/Sources/Views/InspectorView.swift`

**Step 1: Implement PaletteQuantizer.swift**

Port the hue-aware mapping from `tools/convert_novavm_logo.py`:

```swift
import AppKit

enum PaletteQuantizer {
    static func quantize(image: NSImage, width: Int, height: Int) -> [UInt8] {
        let size = NSSize(width: width, height: height)
        guard let bitmapRep = NSBitmapImageRep(
            bitmapDataPlanes: nil, pixelsWide: width, pixelsHigh: height,
            bitsPerSample: 8, samplesPerPixel: 4, hasAlpha: true,
            isPlanar: false, colorSpaceName: .deviceRGB,
            bytesPerRow: width * 4, bitsPerPixel: 32
        ) else { return [UInt8](repeating: 0, count: width * height) }

        NSGraphicsContext.saveGraphicsState()
        NSGraphicsContext.current = NSGraphicsContext(bitmapImageRep: bitmapRep)
        image.draw(in: NSRect(origin: .zero, size: size),
                   from: .zero, operation: .copy, fraction: 1.0)
        NSGraphicsContext.restoreGraphicsState()

        var result = [UInt8](repeating: 0, count: width * height)
        guard let data = bitmapRep.bitmapData else { return result }

        for i in 0..<(width * height) {
            let r = Double(data[i * 4]) / 255.0
            let g = Double(data[i * 4 + 1]) / 255.0
            let b = Double(data[i * 4 + 2]) / 255.0
            result[i] = nearestPaletteIndex(r: r, g: g, b: b)
        }
        return result
    }

    private static let grayIndices: [Int] = [0, 11, 12, 15, 1]

    static func nearestPaletteIndex(r: Double, g: Double, b: Double) -> UInt8 {
        let lum = 0.2126 * r + 0.7152 * g + 0.0722 * b
        let (h, s, v) = rgbToHsv(r: r, g: g, b: b)

        if v < 0.045 || lum < 0.022 { return 0 }

        if s < 0.18 {
            let best = grayIndices.min { a, b in
                let ca = NovaPalette.colors[a]
                let cb = NovaPalette.colors[b]
                let la = luma(ca)
                let lb = luma(cb)
                return abs(la - lum) < abs(lb - lum)
            }!
            return UInt8(best)
        }

        let deg = h * 360.0
        if deg < 20 || deg >= 335 { return v >= 0.50 ? 10 : 2 }
        if deg < 44 { return v >= 0.45 ? 8 : 9 }
        if deg < 72 { return 7 }
        if deg < 145 { return v >= 0.50 ? 13 : 5 }
        if deg < 195 { return v >= 0.55 ? 3 : 14 }
        if deg < 230 { return v >= 0.45 ? 14 : 6 }
        if deg < 270 { return v >= 0.62 ? 14 : 6 }
        if deg < 320 { return v >= 0.35 ? 4 : 6 }
        return v >= 0.50 ? 10 : 2
    }

    private static func luma(_ c: (r: UInt8, g: UInt8, b: UInt8)) -> Double {
        0.2126 * Double(c.r) / 255 + 0.7152 * Double(c.g) / 255 + 0.0722 * Double(c.b) / 255
    }

    private static func rgbToHsv(r: Double, g: Double, b: Double) -> (h: Double, s: Double, v: Double) {
        let maxC = max(r, g, b)
        let minC = min(r, g, b)
        let delta = maxC - minC
        let v = maxC
        let s = maxC == 0 ? 0 : delta / maxC
        var h: Double = 0
        if delta > 0 {
            if maxC == r { h = ((g - b) / delta).truncatingRemainder(dividingBy: 6) }
            else if maxC == g { h = (b - r) / delta + 2 }
            else { h = (r - g) / delta + 4 }
            h /= 6
            if h < 0 { h += 1 }
        }
        return (h, s, v)
    }
}
```

**Step 2: Implement ProjectFormat.swift**

```swift
import Foundation

enum ProjectFormat {
    struct Header: Codable {
        var version: Int = 1
        var width: Int
        var height: Int
        var backdrop: String?
        var backdropOpacity: Double
        var gridOn: Bool
        var gridMajor: Int
    }

    static func encode(document: NovaDocument) -> Data {
        let header = Header(
            width: document.width, height: document.height,
            backdrop: nil, // backdrop path stored separately
            backdropOpacity: document.backdropOpacity,
            gridOn: document.gridOn, gridMajor: document.gridMajor
        )
        var data = try! JSONEncoder().encode(header)
        data.append(0x0A) // newline separator
        data.append(contentsOf: document.pixels)
        return data
    }

    @MainActor
    static func decode(data: Data) -> NovaDocument? {
        guard let newline = data.firstIndex(of: 0x0A) else { return nil }
        let jsonData = data[data.startIndex..<newline]
        guard let header = try? JSONDecoder().decode(Header.self, from: jsonData) else { return nil }

        let pixelStart = data.index(after: newline)
        let pixels = Array(data[pixelStart...])
        guard pixels.count == header.width * header.height else { return nil }

        let doc = NovaDocument(width: header.width, height: header.height)
        doc.loadPixelsRaw(pixels)
        doc.backdropOpacity = header.backdropOpacity
        doc.gridOn = header.gridOn
        doc.gridMajor = header.gridMajor
        return doc
    }
}
```

**Step 3: Wire export/import into InspectorView**

Add export buttons that show `NSSavePanel` with appropriate file types. Add import via `File > Open` menu. Export buttons disabled when canvas size doesn't match format.

**Step 4: Add File menu with Open/Save/Export**

Update `NovaDrawApp.swift` to include `.commands` with standard file menu items plus Export submenu.

**Step 5: Run and test manually**

```bash
cd NovaDraw && swift build && swift run NovaDraw
```

Test: draw something → export as .nvg → import back → verify pixels match.

**Step 6: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): import/export all formats, PNG quantizer, project save/load"
```

---

## Task 9: Polish — Menus, Undo Integration, Final Layout

**Files:**
- Modify: `NovaDraw/Sources/App/NovaDrawApp.swift`
- Modify: `NovaDraw/Sources/Views/ContentView.swift`
- Modify: `NovaDraw/Sources/Views/InspectorView.swift`

**Step 1: Add proper macOS menu bar**

```swift
@main
struct NovaDrawApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .defaultSize(width: 1200, height: 800)
        .commands {
            CommandGroup(after: .newItem) {
                // Open, handled by ContentView
            }
        }
    }
}
```

**Step 2: Add Cmd+Z / Shift+Cmd+Z for undo/redo**

Wire to `document.undo()` and `document.redo()`.

**Step 3: Add "New" menu to create fresh canvas**

Reset document and show new canvas sheet.

**Step 4: Final build and test**

```bash
cd NovaDraw && swift build && swift run NovaDraw
```

Test full workflow:
1. Create 320x200 canvas
2. Load PNG backdrop
3. Adjust opacity, trace over it with pencil
4. Use line, rect, circle tools
5. Export as .nvg
6. Close, reopen — import .nvg
7. Create 16x16 canvas, draw sprite, export as .spr
8. Test undo/redo across all tools
9. Test all keyboard shortcuts

**Step 5: Commit**

```bash
git add NovaDraw/
git commit -m "feat(novadraw): menus, undo/redo, polish"
```

---

## Task Summary

| Task | Description | Test |
|------|-------------|------|
| 1 | Scaffold + palette | Builds, window appears |
| 2 | Document model + undo | Unit tests |
| 3 | Canvas rendering + grid + palette + pencil | Visual |
| 4 | Line, rect, circle, fill, eyedropper | Unit tests + visual |
| 5 | Selection tool | Visual |
| 6 | Zoom/pan/keyboard shortcuts | Visual |
| 7 | Export formats (NVG1, GFX, Sprite, VSprite) | Unit tests |
| 8 | Import + PNG quantizer + project file | Visual + manual |
| 9 | Menus, undo, final polish | Full workflow test |
