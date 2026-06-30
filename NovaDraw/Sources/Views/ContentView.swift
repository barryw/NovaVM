import SwiftUI
import UniformTypeIdentifiers
import PixelCanvasKit

struct ContentView: View {
    @Binding var document: PixelDocument?
    @Binding var toolEngine: ToolEngine?
    let onOpenProject: () -> Void
    @State private var cursorPosition: (x: Int, y: Int)?
    @State private var showInspector = true
    @State private var initializedViewportDocumentID: ObjectIdentifier?
    @State private var canvasSize: CGSize = .zero

    var body: some View {
        if let doc = document, let engine = toolEngine {
            mainEditor(doc: doc, engine: engine)
        } else {
            NewCanvasSheet(onCreate: createDocument, onOpenProject: onOpenProject)
        }
    }

    // MARK: - Main Editor

    @ViewBuilder
    private func mainEditor(doc: PixelDocument, engine: ToolEngine) -> some View {
        NavigationStack {
        HStack(spacing: 0) {
            PaletteView(document: doc)

            Divider()

            VStack(spacing: 0) {
                ImageStripView(document: doc)

                Divider()

                GeometryReader { geo in
                    PixelCanvasView(document: doc,
                                    cursorPosition: $cursorPosition,
                                    toolEngine: engine)
                        .onChange(of: geo.size, initial: true) { _, newSize in
                            canvasSize = newSize
                            updateViewportForCanvasSize(doc: doc, size: newSize)
                        }
                        .onKeyPress("1") { setZoom(doc, 1); return .handled }
                        .onKeyPress("2") { setZoom(doc, 2); return .handled }
                        .onKeyPress("3") { setZoom(doc, 4); return .handled }
                        .onKeyPress("4") { setZoom(doc, 8); return .handled }
                        .onKeyPress("5") { setZoom(doc, 16); return .handled }
                        .onKeyPress("6") { setZoom(doc, 32); return .handled }
                        .onKeyPress("7") { setZoom(doc, 64); return .handled }
                        .onKeyPress("8") { setZoom(doc, 128); return .handled }
                        .onKeyPress("9") { setZoom(doc, 256); return .handled }
                        // WHI standard pixel-editor keymap (WAL-29 §4.3).
                        // Grid moved off `G` (now fill) to `'` per the audit spec.
                        .onKeyPress("'") { doc.gridOn.toggle(); return .handled }
                        .onKeyPress("b") { selectTool(doc, .pencil); return .handled }
                        .onKeyPress("e") { selectTool(doc, .eraser); return .handled }
                        .onKeyPress("g") { selectTool(doc, .fill); return .handled }
                        .onKeyPress("i") { selectTool(doc, .eyedropper); return .handled }
                        .onKeyPress("l") { selectTool(doc, .line); return .handled }
                        .onKeyPress("r") { selectTool(doc, .rect); return .handled }
                        .onKeyPress("o") { selectTool(doc, .circle); return .handled }
                        .onKeyPress("m") { selectTool(doc, .select); return .handled }
                        .onKeyPress(keys: [.init("z")], phases: .down) { press in
                            let cmd = press.modifiers.contains(.command)
                            let shift = press.modifiers.contains(.shift)
                            if cmd && shift { engine.redo(); return .handled }
                            if cmd { engine.undo(); return .handled }
                            return .ignored
                        }
                        .onKeyPress(keys: [.init("0")], phases: .down) { press in
                            guard press.modifiers.contains(.command) else { return .ignored }
                            fitToView(doc: doc, size: geo.size)
                            return .handled
                        }
                }
            }

            if showInspector {
                Divider()
                InspectorView(document: doc)
            }
        }
        .toolbar {
            ToolbarItemGroup(placement: .automatic) {
                toolPicker(doc: doc)

                Divider()

                Toggle(isOn: Binding(
                    get: { doc.gridOn },
                    set: { doc.gridOn = $0 }
                )) {
                    Image(systemName: "grid")
                }
                .help("Toggle Grid (')")

                Toggle(isOn: $showInspector) {
                    Image(systemName: "sidebar.trailing")
                }
                .help("Toggle Inspector")

                Divider()

                Button {
                    zoomOut(doc)
                } label: {
                    Image(systemName: "minus.magnifyingglass")
                }
                .help("Zoom Out")

                Button {
                    zoomIn(doc)
                } label: {
                    Image(systemName: "plus.magnifyingglass")
                }
                .help("Zoom In")

                Text("\(Int(doc.zoom))×")
                    .font(.system(.body, design: .monospaced))
                    .foregroundStyle(.secondary)
                    .frame(minWidth: 30)
            }
        }
        .focusedSceneValue(\.novaDocument, doc)
        .navigationTitle("NovaDraw")
        .navigationSubtitle("\(doc.width)×\(doc.height)")
        } // NavigationStack
        .safeAreaInset(edge: .bottom, spacing: 0) {
            statusBar(doc: doc)
        }
    }

    // MARK: - Helpers

    private func updateViewportForCanvasSize(doc: PixelDocument, size: CGSize) {
        guard size.width > 0, size.height > 0 else { return }
        let documentID = ObjectIdentifier(doc)
        if initializedViewportDocumentID != documentID {
            initializedViewportDocumentID = documentID

            guard !doc.hasSavedViewport else {
                doc.performWithoutMarkingDirty {
                    CanvasViewport.centerCanvas(document: doc, size: size)
                }
                return
            }
            fitToView(doc: doc, size: size, markDirty: false)
            return
        }

        doc.performWithoutMarkingDirty {
            CanvasViewport.centerCanvas(document: doc, size: size)
        }
    }

    private func fitToView(doc: PixelDocument, size: CGSize, markDirty: Bool = true) {
        let applyFit = {
            CanvasViewport.fitToView(document: doc, size: size)
        }

        if markDirty {
            applyFit()
        } else {
            doc.performWithoutMarkingDirty(applyFit)
        }
    }

    private func setZoom(_ doc: PixelDocument, _ zoom: CGFloat) {
        CanvasViewport.applyCenteredZoom(document: doc, size: canvasSize, zoom: zoom)
    }

    private func zoomIn(_ doc: PixelDocument) {
        setZoom(doc, CanvasViewport.nextZoomStop(after: doc.zoom))
    }

    private func zoomOut(_ doc: PixelDocument) {
        setZoom(doc, CanvasViewport.previousZoomStop(before: doc.zoom))
    }

    private func createDocument(width: Int, height: Int) {
        let doc = PixelDocument(width: width, height: height)
        document = doc
        toolEngine = ToolEngine(document: doc)
    }

    private func selectTool(_ doc: PixelDocument, _ tool: DrawingTool) {
        doc.selectTool(tool)
    }

    private func toolPicker(doc: PixelDocument) -> some View {
        ForEach(DrawingTool.allCases, id: \.self) { tool in
            Toggle(isOn: Binding(
                get: { doc.currentTool == tool },
                set: { if $0 { selectTool(doc, tool) } }
            )) {
                Image(systemName: tool.systemImage)
            }
            .help("\(tool.label) (\(String(tool.shortcut).uppercased()))")
        }
    }

    // Bottom status strip — the reference the other editors model on (WAL-29 §4.2).
    // Styled with the shared PixelCanvasUI tokens (JetBrains-Mono / warm neutrals);
    // keeps NovaDraw's richer content (active image label) beyond the shared minimum.
    private func statusBar(doc: PixelDocument) -> some View {
        HStack(spacing: PixelCanvasUI.Spacing.md) {
            HStack(spacing: PixelCanvasUI.Spacing.xs) {
                Image(systemName: "scope")
                    .foregroundStyle(PixelCanvasUI.Colors.textFaint)
                Text(cursorPosition.map { "\($0.x), \($0.y)" } ?? "—")
            }

            statusSeparator

            Text(doc.currentTool.label)

            Spacer(minLength: PixelCanvasUI.Spacing.sm)

            Text("Image \(doc.selectedImageLabel)")

            statusSeparator

            Text("\(Int(doc.zoom * 100))%")

            statusSeparator

            Text("\(doc.width)×\(doc.height)")
        }
        .font(PixelCanvasUI.Typography.mono(PixelCanvasUI.Typography.statusSize))
        .foregroundStyle(PixelCanvasUI.Colors.textMuted)
        .lineLimit(1)
        .padding(.horizontal, PixelCanvasUI.Spacing.md)
        .padding(.vertical, PixelCanvasUI.Spacing.xs)
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(PixelCanvasUI.Colors.surface)
        .overlay(alignment: .top) {
            Rectangle()
                .fill(PixelCanvasUI.Colors.border)
                .frame(height: 1)
        }
    }

    private var statusSeparator: some View {
        Rectangle()
            .fill(PixelCanvasUI.Colors.border)
            .frame(width: 1, height: 11)
    }
}

// MARK: - Focused Value for Menu Commands

struct NovaDocumentKey: FocusedValueKey {
    typealias Value = PixelDocument
}

extension FocusedValues {
    var novaDocument: PixelDocument? {
        get { self[NovaDocumentKey.self] }
        set { self[NovaDocumentKey.self] = newValue }
    }
}
