import SwiftUI
import UniformTypeIdentifiers
import PixelCanvasKit

@main
struct NovaDrawApp: App {
    @State private var document: PixelDocument?
    @State private var toolEngine: ToolEngine?
    @State private var showingNewCanvas = false
    @State private var traceImportRequest: TraceSheetImportRequest?
    @State private var showingCa65Export = false
    @State private var observedProjectWriteDate: Date?
    @State private var recentProjectURLs = RecentProjectsStore.load()

    var body: some Scene {
        WindowGroup {
            ContentView(document: $document,
                        toolEngine: $toolEngine,
                        onOpenProject: openProject)
                .tint(PixelCanvasUI.accent) // WHI per-product accent (Nova cyan) — WAL-29
                // WHI editor chrome is dark by design (PixelCanvasUI is a hard-dark
                // token set); lock the scene so adaptive panels match the dark
                // status strip + canvas instead of going light. WAL-100 / ADR 0006.
                .preferredColorScheme(.dark)
                // Enforce the canonical grammar floor, not just declare it — a user
                // can't shrink below the 1200×800 rail+canvas+inspector layout. WAL-100.
                .frame(minWidth: PixelCanvasUI.Window.minWidth,
                       minHeight: PixelCanvasUI.Window.minHeight)
                .onOpenURL { url in
                    openProject(from: url)
                }
                .task(id: document?.fileURL) {
                    await watchOpenProjectFile()
                }
                .sheet(isPresented: $showingNewCanvas) {
                    NewCanvasSheet(onCreate: createDocument, onOpenProject: nil)
                }
                .sheet(item: $traceImportRequest) { request in
                    TraceSheetImportSheet(
                        request: request,
                        onCancel: { traceImportRequest = nil },
                        onImport: createTraceSheetDocument
                    )
                }
                .sheet(isPresented: $showingCa65Export) {
                    if let document {
                        Ca65ExportSheet(
                            document: document,
                            onCancel: { showingCa65Export = false },
                            onExport: exportCa65
                        )
                    }
                }
        }
        // WHI canonical editor window (WAL-100 / ADR 0006): 1280×840.
        .defaultSize(width: PixelCanvasUI.Window.defaultWidth,
                     height: PixelCanvasUI.Window.defaultHeight)
        .windowStyle(.titleBar)
        .windowToolbarStyle(.unified)
        .commands {
            CommandGroup(replacing: .newItem) {
                Button("New Canvas...") { beginNewCanvas() }
                    .keyboardShortcut("n")
                Button("Open Project...") { openProject() }
                    .keyboardShortcut("o")

                Menu("Open Recent") {
                    if recentProjectURLs.isEmpty {
                        Button("No Recent Projects") {}
                            .disabled(true)
                    } else {
                        ForEach(recentProjectURLs.map(RecentProject.init)) { project in
                            Button(project.title) {
                                openRecentProject(project.url)
                            }
                            .help(project.url.path)
                        }
                        Divider()
                        Button("Clear Menu") { clearRecentProjects() }
                    }
                }
            }

            CommandGroup(replacing: .saveItem) {
                Button("Save") { _ = saveProject() }
                    .keyboardShortcut("s")
                    .disabled(document == nil)
                Button("Save As...") { _ = saveProjectAs() }
                    .keyboardShortcut("s", modifiers: [.command, .shift])
                    .disabled(document == nil)

                Divider()

                Menu("Import") {
                    Button("Image (PNG/JPEG)...") { importImage() }
                        .disabled(document == nil)
                    Button("Trace Sheet Image...") { importTraceSheetImage() }
                    Divider()
                    Button("NVG...") { importFormat { NvgFormat.decode(data: $0)?.pixels } }
                        .disabled(document == nil)
                    Button("GFX...") {
                        importFormat { GfxFormat.decode(data: $0, width: document?.width ?? 320, height: document?.height ?? 200) }
                    }
                    .disabled(document == nil)
                    Button("Sprite...") { importFormat { SpriteFormat.decode(data: $0)?.pixels } }
                        .disabled(document == nil)
                }

                Menu("Export") {
                    Button("ca65 Include (.inc)...") { showingCa65Export = true }
                    Divider()
                    Button("NVG (.nvg)...") { exportNvg() }
                        .disabled(document?.width != 320 || document?.height != 200)
                    Button("GFX (.gfx)...") { exportGfx() }
                        .disabled(document?.width != 320 || document?.height != 200)
                    Button("Sprite (.spr)...") { exportSprite() }
                    Button("VSprite (.vspr)...") { exportVSprite() }
                }
                .disabled(document == nil)
            }

            CommandGroup(replacing: .undoRedo) {
                Button("Undo") { toolEngine?.undo() }
                    .keyboardShortcut("z")
                    .disabled(document?.canUndo != true)
                Button("Redo") { toolEngine?.redo() }
                    .keyboardShortcut("z", modifiers: [.command, .shift])
                    .disabled(document?.canRedo != true)
            }

            CommandMenu("Canvas") {
                Button("Flip Horizontal") { document?.flipHorizontal() }
                    .disabled(document == nil)
                Button("Flip Vertical") { document?.flipVertical() }
                    .disabled(document == nil)
                Divider()
                Button("Clear") { document?.clear() }
                    .disabled(document == nil)
            }

            CommandMenu("Images") {
                Button("Add Image") { _ = document?.addImage() }
                    .disabled(document?.canAddImage != true)
                Button("Delete Image") { _ = document?.deleteSelectedImage() }
                    .disabled(document?.canDeleteImage != true)
                Divider()
                Button("Previous Image") { selectRelativeImage(-1) }
                    .disabled(canSelectRelativeImage(-1) == false)
                Button("Next Image") { selectRelativeImage(1) }
                    .disabled(canSelectRelativeImage(1) == false)
            }

            CommandMenu("Backdrop") {
                Button("Load Image Fit to Canvas...") { loadBackdrop(placement: .fitToCanvas) }
                    .keyboardShortcut("b", modifiers: [.command, .shift])
                    .disabled(document == nil)
                Button("Load Floating Image...") { loadBackdrop(placement: .floating) }
                    .disabled(document == nil)
                Divider()
                Button("Flip Horizontal") { flipBackdropH() }
                    .disabled(document?.backdropImage == nil)
                Button("Flip Vertical") { flipBackdropV() }
                    .disabled(document?.backdropImage == nil)
                Divider()
                Button("Reset Placement") { document?.resetBackdropPlacement() }
                    .disabled(document?.backdropImage == nil)
            }
        }

        Settings {
            SettingsView()
                .preferredColorScheme(.dark)
        }
    }

    // MARK: - Project Documents

    private func beginNewCanvas() {
        guard confirmDiscardChanges() else { return }
        showingNewCanvas = true
    }

    private func createDocument(width: Int, height: Int) {
        let doc = PixelDocument(width: width, height: height)
        document = doc
        toolEngine = ToolEngine(document: doc)
        showingNewCanvas = false
    }

    private func canSelectRelativeImage(_ delta: Int) -> Bool {
        guard let doc = document else { return false }
        return doc.images.indices.contains(doc.selectedImageIndex + delta)
    }

    private func selectRelativeImage(_ delta: Int) {
        guard let doc = document else { return }
        doc.selectImage(at: doc.selectedImageIndex + delta)
    }

    private func openProject() {
        guard confirmDiscardChanges() else { return }

        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.novaDrawProject]
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        panel.canChooseFiles = true

        if panel.runModal() == .OK, let url = panel.url {
            loadProject(from: url)
        }
    }

    private func openProject(from url: URL) {
        guard confirmDiscardChanges() else { return }
        loadProject(from: url)
    }

    private func openRecentProject(_ url: URL) {
        guard confirmDiscardChanges() else { return }
        guard FileManager.default.fileExists(atPath: url.path) else {
            removeRecentProject(url)
            showError("The recent project could not be found.")
            return
        }
        loadProject(from: url)
    }

    @discardableResult
    private func saveProject() -> Bool {
        guard let doc = document else { return false }
        guard let url = doc.fileURL else {
            return saveProjectAs()
        }
        return writeProject(doc, to: url)
    }

    @discardableResult
    private func saveProjectAs() -> Bool {
        guard let doc = document else { return false }

        let panel = NSSavePanel()
        panel.allowedContentTypes = [.novaDrawProject]
        panel.canCreateDirectories = true
        panel.nameFieldStringValue = doc.suggestedProjectName

        guard panel.runModal() == .OK, let url = panel.url else {
            return false
        }
        return writeProject(doc, to: url)
    }

    private func loadProject(from url: URL) {
        do {
            let data = try Data(contentsOf: url)
            guard let loaded = ProjectFormat.decode(data: data) else {
                showError("The selected file is not a valid NovaDraw project.")
                return
            }

            loaded.markSaved(to: url)
            document = loaded
            toolEngine = ToolEngine(document: loaded)
            observedProjectWriteDate = projectWriteDate(url)
            showingNewCanvas = false
            recordRecentProject(url)
        } catch {
            showError("NovaDraw could not open the project.", error: error)
        }
    }

    private func writeProject(_ doc: PixelDocument, to url: URL) -> Bool {
        do {
            try ProjectFormat.encode(document: doc).write(to: url, options: .atomic)
            doc.markSaved(to: url)
            observedProjectWriteDate = projectWriteDate(url)
            recordRecentProject(url)
            return true
        } catch {
            showError("NovaDraw could not save the project.", error: error)
            return false
        }
    }

    private func recordRecentProject(_ url: URL) {
        recentProjectURLs = RecentProjectsStore.adding(url, to: recentProjectURLs)
        RecentProjectsStore.save(recentProjectURLs)
    }

    private func removeRecentProject(_ url: URL) {
        recentProjectURLs = RecentProjectsStore.removing(url, from: recentProjectURLs)
        RecentProjectsStore.save(recentProjectURLs)
    }

    private func clearRecentProjects() {
        recentProjectURLs = []
        RecentProjectsStore.save(recentProjectURLs)
    }

    @MainActor
    private func watchOpenProjectFile() async {
        guard let url = document?.fileURL else { return }
        observedProjectWriteDate = projectWriteDate(url)

        while !Task.isCancelled {
            try? await Task.sleep(for: .milliseconds(400))
            guard let currentURL = document?.fileURL, currentURL == url else { return }
            guard let writeDate = projectWriteDate(url) else { continue }
            guard observedProjectWriteDate != writeDate else { continue }

            reloadProjectAfterExternalChange(from: url, writeDate: writeDate)
        }
    }

    @MainActor
    private func reloadProjectAfterExternalChange(from url: URL, writeDate: Date) {
        do {
            let data = try Data(contentsOf: url)
            guard let loaded = ProjectFormat.decode(data: data) else {
                return
            }

            loaded.markSaved(to: url)
            document = loaded
            toolEngine = ToolEngine(document: loaded)
            observedProjectWriteDate = writeDate
        } catch {
            return
        }
    }

    private func projectWriteDate(_ url: URL) -> Date? {
        guard let attributes = try? FileManager.default.attributesOfItem(atPath: url.path) else {
            return nil
        }
        return attributes[.modificationDate] as? Date
    }

    private func confirmDiscardChanges() -> Bool {
        guard let doc = document, doc.isDirty else { return true }

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.messageText = "Save changes to \(doc.displayName)?"
        alert.informativeText = "Your changes will be lost if you do not save them."
        alert.addButton(withTitle: "Save")
        alert.addButton(withTitle: "Cancel")
        alert.addButton(withTitle: "Don't Save")

        switch alert.runModal() {
        case .alertFirstButtonReturn:
            return saveProject()
        case .alertSecondButtonReturn:
            return false
        case .alertThirdButtonReturn:
            return true
        default:
            return false
        }
    }

    // MARK: - Backdrop

    private func loadBackdrop(placement: BackdropPlacementMode) {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.png, .jpeg]
        panel.allowsMultipleSelection = false
        if panel.runModal() == .OK, let url = panel.url {
            if let image = NSImage(contentsOf: url) {
                document?.configureBackdrop(image, placement: placement)
            }
        }
    }

    private func flipBackdropH() {
        guard let doc = document, let img = doc.backdropImage else { return }
        let flipped = NSImage(size: img.size)
        flipped.lockFocus()
        let xform = NSAffineTransform()
        xform.translateX(by: img.size.width, yBy: 0)
        xform.scaleX(by: -1, yBy: 1)
        xform.concat()
        img.draw(in: NSRect(origin: .zero, size: img.size),
                 from: .zero, operation: .copy, fraction: 1.0)
        flipped.unlockFocus()
        doc.backdropImage = flipped
    }

    private func flipBackdropV() {
        guard let doc = document, let img = doc.backdropImage else { return }
        let flipped = NSImage(size: img.size)
        flipped.lockFocus()
        let xform = NSAffineTransform()
        xform.translateX(by: 0, yBy: img.size.height)
        xform.scaleX(by: 1, yBy: -1)
        xform.concat()
        img.draw(in: NSRect(origin: .zero, size: img.size),
                 from: .zero, operation: .copy, fraction: 1.0)
        flipped.unlockFocus()
        doc.backdropImage = flipped
    }

    // MARK: - Export

    private func exportNvg() {
        guard let doc = document else { return }
        let data = NvgFormat.encode(pixels: Array(doc.pixels), width: doc.width, height: doc.height)
        saveData(data, suggestedName: "splash.nvg", contentType: UTType(filenameExtension: "nvg") ?? .data)
    }

    private func exportGfx() {
        guard let doc = document else { return }
        let data = GfxFormat.encode(pixels: Array(doc.pixels))
        saveData(data, suggestedName: "screen.gfx", contentType: UTType(filenameExtension: "gfx") ?? .data)
    }

    private func exportSprite() {
        guard let doc = document else { return }
        let data: Data
        if doc.width == 16 && doc.height == 16 {
            data = SpriteFormat.encode(pixels: Array(doc.pixels), width: 16, height: 16)
        } else {
            data = SpriteFormat.encodeSheet(pixels: Array(doc.pixels), width: doc.width, height: doc.height)
        }
        saveData(data, suggestedName: "sprites.spr", contentType: UTType(filenameExtension: "spr") ?? .data)
    }

    private func exportVSprite() {
        guard let doc = document else { return }
        let data = VSpriteFormat.encode(pixels: Array(doc.pixels), width: doc.width, height: doc.height)
        saveData(data, suggestedName: "sprite.vspr", contentType: UTType(filenameExtension: "vspr") ?? .data)
    }

    private func exportCa65(options: Ca65ExportOptions) -> Bool {
        guard let doc = document else { return false }
        do {
            let source = try Ca65Exporter.encode(document: doc, options: options)
            let data = Data(source.utf8)
            return saveData(data, suggestedName: "\(doc.displayName).inc", contentType: UTType(filenameExtension: "inc") ?? .plainText)
        } catch {
            showError("NovaDraw could not export ca65 source.", error: error)
            return false
        }
    }

    @discardableResult
    private func saveData(_ data: Data, suggestedName: String, contentType: UTType) -> Bool {
        let panel = NSSavePanel()
        panel.nameFieldStringValue = suggestedName
        panel.allowedContentTypes = [contentType]
        panel.canCreateDirectories = true
        if panel.runModal() == .OK, let url = panel.url {
            do {
                try data.write(to: url, options: .atomic)
                return true
            } catch {
                showError("NovaDraw could not export the file.", error: error)
            }
        }
        return false
    }

    // MARK: - Import

    private func importImage() {
        guard let doc = document else { return }
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.png, .jpeg]
        if panel.runModal() == .OK, let url = panel.url, let img = NSImage(contentsOf: url) {
            let alert = NSAlert()
            alert.messageText = "Import Image"
            alert.informativeText = "Load as backdrop for tracing, or quantize to Nova palette?"
            alert.addButton(withTitle: "Fit Backdrop")
            alert.addButton(withTitle: "Floating Backdrop")
            alert.addButton(withTitle: "Quantize")
            alert.addButton(withTitle: "Cancel")
            let response = alert.runModal()
            if response == .alertFirstButtonReturn {
                doc.configureBackdrop(img, placement: .fitToCanvas)
            } else if response == .alertSecondButtonReturn {
                doc.configureBackdrop(img, placement: .floating)
            } else if response == .alertThirdButtonReturn {
                let pixels = PaletteQuantizer.quantize(image: img, width: doc.width, height: doc.height)
                doc.loadPixels(pixels)
            }
        }
    }

    private func importTraceSheetImage() {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.png, .jpeg]
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        panel.canChooseFiles = true

        guard panel.runModal() == .OK, let url = panel.url else { return }
        guard let image = NSImage(contentsOf: url),
              let pixelSize = TraceSheetRenderer.sourcePixelSize(for: image) else {
            showError("NovaDraw could not read the selected image.")
            return
        }

        traceImportRequest = TraceSheetImportRequest(
            url: url,
            image: image,
            sourcePixelWidth: pixelSize.width,
            sourcePixelHeight: pixelSize.height
        )
    }

    private func createTraceSheetDocument(
        request: TraceSheetImportRequest,
        settings: TraceSheetImportSettings
    ) -> Bool {
        guard confirmDiscardChanges() else { return false }
        guard let backdrop = TraceSheetRenderer.renderBackdrop(
            image: request.image,
            sourcePixelWidth: request.sourcePixelWidth,
            sourcePixelHeight: request.sourcePixelHeight,
            settings: settings
        ) else {
            showError("NovaDraw could not prepare the trace sheet.")
            return false
        }

        let doc = PixelDocument(width: settings.outputWidth, height: settings.outputHeight)
        doc.backdropImage = backdrop
        doc.backdropImageOpacity = settings.backdropImageOpacity
        doc.backdropPlacementMode = .fitToCanvas
        doc.resetBackdropPlacement()
        doc.backdropOpacity = settings.drawingOpacity
        doc.gridOn = true
        doc.gridMajor = settings.cellWidth
        doc.markDirty()

        document = doc
        toolEngine = ToolEngine(document: doc)
        traceImportRequest = nil
        return true
    }

    private func importFormat(decode: (Data) -> [UInt8]?) {
        guard let doc = document else { return }
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.data]
        if panel.runModal() == .OK, let url = panel.url,
           let data = try? Data(contentsOf: url),
           let pixels = decode(data) {
            doc.loadPixels(pixels)
        }
    }

    private func showError(_ message: String, error: Error? = nil) {
        let alert = NSAlert()
        alert.alertStyle = .critical
        alert.messageText = message
        if let error {
            alert.informativeText = error.localizedDescription
        }
        alert.runModal()
    }
}
