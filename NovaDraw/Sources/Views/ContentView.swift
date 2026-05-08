import SwiftUI
import UniformTypeIdentifiers

struct ContentView: View {
    @Binding var document: NovaDocument?
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
    private func mainEditor(doc: NovaDocument, engine: ToolEngine) -> some View {
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
                        .onKeyPress("g") { doc.gridOn.toggle(); return .handled }
                        .onKeyPress("p") { selectTool(doc, .pencil); return .handled }
                        .onKeyPress("e") { selectTool(doc, .eraser); return .handled }
                        .onKeyPress("l") { selectTool(doc, .line); return .handled }
                        .onKeyPress("r") { selectTool(doc, .rect); return .handled }
                        .onKeyPress("c") { selectTool(doc, .circle); return .handled }
                        .onKeyPress("f") { selectTool(doc, .fill); return .handled }
                        .onKeyPress("i") { selectTool(doc, .eyedropper); return .handled }
                        .onKeyPress("s") { selectTool(doc, .select); return .handled }
                        .onKeyPress(keys: [.init("z")], phases: .down) { press in
                            let cmd = press.modifiers.contains(.command)
                            let shift = press.modifiers.contains(.shift)
                            if cmd && shift { doc.redo(); return .handled }
                            if cmd { doc.undo(); return .handled }
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
                .help("Toggle Grid (G)")

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

    private func updateViewportForCanvasSize(doc: NovaDocument, size: CGSize) {
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

    private func fitToView(doc: NovaDocument, size: CGSize, markDirty: Bool = true) {
        let applyFit = {
            CanvasViewport.fitToView(document: doc, size: size)
        }

        if markDirty {
            applyFit()
        } else {
            doc.performWithoutMarkingDirty(applyFit)
        }
    }

    private func setZoom(_ doc: NovaDocument, _ zoom: CGFloat) {
        CanvasViewport.applyCenteredZoom(document: doc, size: canvasSize, zoom: zoom)
    }

    private func zoomIn(_ doc: NovaDocument) {
        setZoom(doc, CanvasViewport.nextZoomStop(after: doc.zoom))
    }

    private func zoomOut(_ doc: NovaDocument) {
        setZoom(doc, CanvasViewport.previousZoomStop(before: doc.zoom))
    }

    private func createDocument(width: Int, height: Int) {
        let doc = NovaDocument(width: width, height: height)
        document = doc
        toolEngine = ToolEngine(document: doc)
    }

    private func selectTool(_ doc: NovaDocument, _ tool: DrawingTool) {
        doc.selectTool(tool)
    }

    private func toolPicker(doc: NovaDocument) -> some View {
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

    private func statusBar(doc: NovaDocument) -> some View {
        HStack(spacing: 12) {
            if let pos = cursorPosition {
                Label {
                    Text("\(pos.x), \(pos.y)")
                        .monospacedDigit()
                } icon: {
                    Image(systemName: "scope")
                }
            } else {
                Label("—", systemImage: "scope")
            }

            Spacer()

            Text(doc.currentTool.label)
                .foregroundStyle(.secondary)

            Divider().frame(height: 12)

            Text("Image \(doc.selectedImageLabel)")
                .monospacedDigit()
                .foregroundStyle(.secondary)

            Divider().frame(height: 12)

            Text("\(doc.width)×\(doc.height)")
                .monospacedDigit()
                .foregroundStyle(.secondary)

            Text("\(Int(doc.zoom))×")
                .monospacedDigit()
                .foregroundStyle(.secondary)
        }
        .font(.system(size: 11))
        .padding(.horizontal, 12)
        .padding(.vertical, 4)
        .background(.bar)
    }
}

// MARK: - Focused Value for Menu Commands

struct NovaDocumentKey: FocusedValueKey {
    typealias Value = NovaDocument
}

extension FocusedValues {
    var novaDocument: NovaDocument? {
        get { self[NovaDocumentKey.self] }
        set { self[NovaDocumentKey.self] = newValue }
    }
}
