import Foundation
import Testing
@testable import NovaDrawMCPKit

@Suite struct NovaDrawMCPToolTests {
    @Test func createSetPixelsAndPreviewRoundTrip() throws {
        let runner = NovaDrawMCPToolRunner()
        let url = temporaryProjectURL()

        let created = try runner.call(toolName: "novadraw_create_project", arguments: [
            "path": url.path,
            "width": 4,
            "height": 3,
            "image_names": ["sprite"],
            "overwrite": true,
        ])
        #expect(created.contains("4x3"))

        _ = try runner.call(toolName: "novadraw_set_pixels", arguments: [
            "path": url.path,
            "x": 1,
            "y": 1,
            "rows": [
                "12",
                ".F",
            ],
        ])

        let preview = try runner.call(toolName: "novadraw_ascii_preview", arguments: [
            "path": url.path,
            "x": 0,
            "y": 0,
            "width": 4,
            "height": 3,
        ])

        #expect(preview == """
        ....
        .12.
        ..F.
        """)
    }

    @Test func duplicateRenameAndInspectProject() throws {
        let runner = NovaDrawMCPToolRunner()
        let url = temporaryProjectURL()

        _ = try runner.call(toolName: "novadraw_create_project", arguments: [
            "path": url.path,
            "width": 2,
            "height": 2,
            "overwrite": true,
        ])
        _ = try runner.call(toolName: "novadraw_rename_image", arguments: [
            "path": url.path,
            "name": "pawn",
        ])
        _ = try runner.call(toolName: "novadraw_duplicate_image", arguments: [
            "path": url.path,
            "name": "knight",
        ])
        _ = try runner.call(toolName: "novadraw_select_image", arguments: [
            "path": url.path,
            "image_index": 1,
        ])

        let inspected = try runner.call(toolName: "novadraw_inspect_project", arguments: [
            "path": url.path,
        ])

        #expect(inspected.contains("\"imageCount\" : 2"))
        #expect(inspected.contains("\"selectedImageIndex\" : 1"))
        #expect(inspected.contains("\"name\" : \"pawn\""))
        #expect(inspected.contains("\"name\" : \"knight\""))
    }

    @Test func drawingToolsMutateProjectPixels() throws {
        let runner = NovaDrawMCPToolRunner()
        let url = temporaryProjectURL()

        _ = try runner.call(toolName: "novadraw_create_project", arguments: [
            "path": url.path,
            "width": 6,
            "height": 4,
            "overwrite": true,
        ])
        _ = try runner.call(toolName: "novadraw_draw_rect", arguments: [
            "path": url.path,
            "x": 1,
            "y": 1,
            "width": 4,
            "height": 2,
            "color": 3,
            "filled": false,
        ])
        _ = try runner.call(toolName: "novadraw_replace_color", arguments: [
            "path": url.path,
            "from": 3,
            "to": 9,
        ])

        let preview = try runner.call(toolName: "novadraw_ascii_preview", arguments: [
            "path": url.path,
            "width": 6,
            "height": 4,
        ])

        #expect(preview == """
        ......
        .9999.
        .9999.
        ......
        """)
    }

    @Test func setBackdropAndExportNvg() throws {
        let runner = NovaDrawMCPToolRunner()
        let url = temporaryProjectURL()
        let imageURL = FileManager.default.temporaryDirectory
            .appendingPathComponent(UUID().uuidString)
            .appendingPathExtension("png")
        let outputURL = FileManager.default.temporaryDirectory
            .appendingPathComponent(UUID().uuidString)
            .appendingPathExtension("nvg")
        let pngData = Data(base64Encoded: "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8DwHwAFBQIAX8jx0gAAAABJRU5ErkJggg==")!
        try pngData.write(to: imageURL)

        _ = try runner.call(toolName: "novadraw_create_project", arguments: [
            "path": url.path,
            "width": 320,
            "height": 200,
            "overwrite": true,
        ])
        _ = try runner.call(toolName: "novadraw_set_pixels", arguments: [
            "path": url.path,
            "x": 10,
            "y": 10,
            "rows": ["7"],
        ])
        let backdrop = try runner.call(toolName: "novadraw_set_backdrop_image", arguments: [
            "path": url.path,
            "image_path": imageURL.path,
            "image_opacity": 1.0,
            "drawing_opacity": 0.8,
        ])
        #expect(backdrop.contains("fit-to-canvas backdrop"))

        let exported = try runner.call(toolName: "novadraw_export_nvg", arguments: [
            "path": url.path,
            "output_path": outputURL.path,
        ])
        #expect(exported.contains("Exported NVG1"))

        let data = try Data(contentsOf: outputURL)
        #expect(Array(data.prefix(4)) == [0x4E, 0x56, 0x47, 0x31])
        #expect(data.count > 12)
    }

    private func temporaryProjectURL() -> URL {
        FileManager.default.temporaryDirectory
            .appendingPathComponent(UUID().uuidString)
            .appendingPathExtension("novadraw")
    }
}
