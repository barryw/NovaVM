import Foundation

public struct NovaDrawMCPToolRunner: Sendable {
    public init() {}

    public static func toolDefinitions() -> [[String: Any]] {
        [
        tool(
            name: "novadraw_create_project",
            description: "Create a NovaDraw .novadraw project file with one or more blank images.",
            properties: [
                "path": string("Absolute or relative path for the .novadraw file."),
                "width": integer("Canvas width in pixels.", minimum: 1),
                "height": integer("Canvas height in pixels.", minimum: 1),
                "image_names": array(string("Image name."), "Optional image names. Defaults to one image."),
                "background_color": integer("Initial color index, 0-15. Defaults to 0.", minimum: 0, maximum: 15),
                "painted_background": boolean("Whether the background pixels are painted instead of transparent. Defaults to false."),
                "overwrite": boolean("Overwrite an existing project file. Defaults to false."),
            ],
            required: ["path", "width", "height"]
        ),
        tool(
            name: "novadraw_inspect_project",
            description: "Return project dimensions, image names, selected image, and painted pixel counts.",
            properties: ["path": string("Path to the .novadraw project.")],
            required: ["path"]
        ),
        tool(
            name: "novadraw_ascii_preview",
            description: "Return an exact text preview of a region. Hex digits are painted colors, dots are transparent pixels.",
            properties: regionProperties(extra: [
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
            ]),
            required: ["path"]
        ),
        tool(
            name: "novadraw_set_pixels",
            description: "Paint or clear a rectangular region. Use rows with 0-9/A-F for colors and . for transparent pixels, or provide pixels plus width/height.",
            properties: regionProperties(extra: [
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "rows": array(string("A row of hex color digits and dots."), "Text rows to paint."),
                "pixels": array(integer("Color index, 0-15.", minimum: 0, maximum: 15), "Row-major pixel colors."),
                "painted": array(boolean("Whether the matching pixel is painted."), "Optional row-major painted mask."),
            ]),
            required: ["path", "x", "y"]
        ),
        tool(
            name: "novadraw_draw_line",
            description: "Draw a one-pixel line into an image.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "x0": integer("Start x."),
                "y0": integer("Start y."),
                "x1": integer("End x."),
                "y1": integer("End y."),
                "color": integer("Color index, 0-15.", minimum: 0, maximum: 15),
            ],
            required: ["path", "x0", "y0", "x1", "y1", "color"]
        ),
        tool(
            name: "novadraw_draw_rect",
            description: "Draw an outlined or filled rectangle into an image.",
            properties: regionProperties(extra: [
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "color": integer("Color index, 0-15.", minimum: 0, maximum: 15),
                "filled": boolean("Fill the rectangle. Defaults to false."),
            ]),
            required: ["path", "x", "y", "width", "height", "color"]
        ),
        tool(
            name: "novadraw_draw_circle",
            description: "Draw an outlined or filled circle into an image.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "cx": integer("Center x."),
                "cy": integer("Center y."),
                "radius": integer("Radius in pixels.", minimum: 0),
                "color": integer("Color index, 0-15.", minimum: 0, maximum: 15),
                "filled": boolean("Fill the circle. Defaults to false."),
            ],
            required: ["path", "cx", "cy", "radius", "color"]
        ),
        tool(
            name: "novadraw_flood_fill",
            description: "Flood fill from a point using 4-connected pixels and matching painted/transparent state.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "x": integer("Start x."),
                "y": integer("Start y."),
                "color": integer("Color index, 0-15.", minimum: 0, maximum: 15),
            ],
            required: ["path", "x", "y", "color"]
        ),
        tool(
            name: "novadraw_clear_region",
            description: "Clear a rectangular region to transparent pixels.",
            properties: regionProperties(extra: [
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
            ]),
            required: ["path", "x", "y", "width", "height"]
        ),
        tool(
            name: "novadraw_replace_color",
            description: "Replace one color with another in an image.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "from": integer("Source color index, 0-15.", minimum: 0, maximum: 15),
                "to": integer("Destination color index, 0-15.", minimum: 0, maximum: 15),
                "painted_only": boolean("Only replace painted pixels. Defaults to true."),
            ],
            required: ["path", "from", "to"]
        ),
        tool(
            name: "novadraw_rename_image",
            description: "Rename an image in a NovaDraw project.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "name": string("New image name."),
            ],
            required: ["path", "name"]
        ),
        tool(
            name: "novadraw_duplicate_image",
            description: "Duplicate an existing image, preserving pixels and transparency.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
                "name": string("Optional new image name."),
            ],
            required: ["path"]
        ),
        tool(
            name: "novadraw_select_image",
            description: "Set the selected image index saved in the project. The NovaDraw UI will switch to it after live reload.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_index": integer("Image index to select.", minimum: 0),
            ],
            required: ["path", "image_index"]
        ),
        tool(
            name: "novadraw_set_backdrop_image",
            description: "Attach an external image as a fit-to-canvas tracing backdrop for a NovaDraw project.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "image_path": string("Path to a PNG/JPEG image to embed as the backdrop."),
                "image_opacity": number("Backdrop image opacity, 0.0-1.0. Defaults to 1.0.", minimum: 0, maximum: 1),
                "drawing_opacity": number("Drawn pixel layer opacity while tracing, 0.0-1.0. Defaults to 0.75.", minimum: 0, maximum: 1),
            ],
            required: ["path", "image_path"]
        ),
        tool(
            name: "novadraw_export_nvg",
            description: "Export the selected or requested 320x200 image to Nova NVG1 format.",
            properties: [
                "path": string("Path to the .novadraw project."),
                "output_path": string("Path for the output .nvg file."),
                "image_index": integer("Image index. Defaults to the project selected image.", minimum: 0),
            ],
            required: ["path", "output_path"]
        ),
    ]
    }

    public func call(toolName: String, arguments: [String: Any]) throws -> String {
        switch toolName {
        case "novadraw_create_project":
            return try createProject(arguments)
        case "novadraw_inspect_project":
            return try inspectProject(arguments)
        case "novadraw_ascii_preview":
            return try asciiPreview(arguments)
        case "novadraw_set_pixels":
            return try mutate(arguments) { project, imageIndex in
                let x = try requiredInt(arguments, "x")
                let y = try requiredInt(arguments, "y")
                let block = try pixelBlock(arguments)
                for row in 0..<block.height {
                    for column in 0..<block.width {
                        let sourceIndex = row * block.width + column
                        if block.paintedPixels[sourceIndex] == 0 {
                            try project.clearPixel(imageIndex: imageIndex, x: x + column, y: y + row)
                        } else {
                            try project.setPixel(
                                imageIndex: imageIndex,
                                x: x + column,
                                y: y + row,
                                color: block.pixels[sourceIndex]
                            )
                        }
                    }
                }
                return "Set \(block.width)x\(block.height) pixels at \(x),\(y) in image \(imageIndex)."
            }
        case "novadraw_draw_line":
            return try mutate(arguments) { project, imageIndex in
                try NovaDrawDrawing.drawLine(
                    project: &project,
                    imageIndex: imageIndex,
                    x0: try requiredInt(arguments, "x0"),
                    y0: try requiredInt(arguments, "y0"),
                    x1: try requiredInt(arguments, "x1"),
                    y1: try requiredInt(arguments, "y1"),
                    color: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "color"))
                )
                return "Drew line in image \(imageIndex)."
            }
        case "novadraw_draw_rect":
            return try mutate(arguments) { project, imageIndex in
                try NovaDrawDrawing.drawRect(
                    project: &project,
                    imageIndex: imageIndex,
                    x: try requiredInt(arguments, "x"),
                    y: try requiredInt(arguments, "y"),
                    width: try requiredInt(arguments, "width"),
                    height: try requiredInt(arguments, "height"),
                    color: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "color")),
                    filled: bool(arguments, "filled") ?? false
                )
                return "Drew rectangle in image \(imageIndex)."
            }
        case "novadraw_draw_circle":
            return try mutate(arguments) { project, imageIndex in
                try NovaDrawDrawing.drawCircle(
                    project: &project,
                    imageIndex: imageIndex,
                    cx: try requiredInt(arguments, "cx"),
                    cy: try requiredInt(arguments, "cy"),
                    radius: try requiredInt(arguments, "radius"),
                    color: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "color")),
                    filled: bool(arguments, "filled") ?? false
                )
                return "Drew circle in image \(imageIndex)."
            }
        case "novadraw_flood_fill":
            return try mutate(arguments) { project, imageIndex in
                let changed = try NovaDrawDrawing.floodFill(
                    project: &project,
                    imageIndex: imageIndex,
                    x: try requiredInt(arguments, "x"),
                    y: try requiredInt(arguments, "y"),
                    color: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "color"))
                )
                return "Flood filled \(changed) pixels in image \(imageIndex)."
            }
        case "novadraw_clear_region":
            return try mutate(arguments) { project, imageIndex in
                try NovaDrawDrawing.clearRegion(
                    project: &project,
                    imageIndex: imageIndex,
                    x: try requiredInt(arguments, "x"),
                    y: try requiredInt(arguments, "y"),
                    width: try requiredInt(arguments, "width"),
                    height: try requiredInt(arguments, "height")
                )
                return "Cleared region in image \(imageIndex)."
            }
        case "novadraw_replace_color":
            return try mutate(arguments) { project, imageIndex in
                let changed = try NovaDrawDrawing.replaceColor(
                    project: &project,
                    imageIndex: imageIndex,
                    from: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "from")),
                    to: NovaDrawDrawing.sanitizeColor(try requiredInt(arguments, "to")),
                    paintedOnly: bool(arguments, "painted_only") ?? true
                )
                return "Replaced \(changed) pixels in image \(imageIndex)."
            }
        case "novadraw_rename_image":
            return try mutate(arguments) { project, imageIndex in
                try project.renameImage(index: imageIndex, name: try requiredString(arguments, "name"))
                return "Renamed image \(imageIndex)."
            }
        case "novadraw_duplicate_image":
            return try mutate(arguments) { project, imageIndex in
                try project.duplicateImage(index: imageIndex, name: string(arguments, "name"))
                return "Duplicated image \(imageIndex)."
            }
        case "novadraw_select_image":
            return try mutate(arguments, imageIndexArgumentRequired: true) { project, imageIndex in
                project.selectedImageIndex = imageIndex
                return "Selected image \(imageIndex)."
            }
        case "novadraw_set_backdrop_image":
            return try mutateBackdrop(arguments)
        case "novadraw_export_nvg":
            return try exportNvg(arguments)
        default:
            throw NovaDrawProjectError.invalidArgument("toolName")
        }
    }

    private func createProject(_ arguments: [String: Any]) throws -> String {
        let url = try projectURL(arguments)
        let overwrite = bool(arguments, "overwrite") ?? false
        if FileManager.default.fileExists(atPath: url.path) && !overwrite {
            throw NovaDrawProjectError.invalidArgument("overwrite")
        }
        let parent = url.deletingLastPathComponent()
        try FileManager.default.createDirectory(at: parent, withIntermediateDirectories: true)
        let names = stringArray(arguments, "image_names") ?? ["Image 1"]
        let project = try NovaDrawProject(
            width: try requiredInt(arguments, "width"),
            height: try requiredInt(arguments, "height"),
            imageNames: names,
            backgroundColor: NovaDrawDrawing.sanitizeColor(int(arguments, "background_color") ?? 0),
            paintedBackground: bool(arguments, "painted_background") ?? false
        )
        try NovaDrawProjectCodec.save(project, to: url)
        return "Created \(url.path) with \(project.width)x\(project.height), images: \(project.images.count)."
    }

    private func inspectProject(_ arguments: [String: Any]) throws -> String {
        let project = try NovaDrawProjectCodec.load(from: try projectURL(arguments))
        let images = project.images.enumerated().map { index, image in
            [
                "index": index,
                "name": image.name,
                "paintedPixels": image.paintedPixels.filter { $0 != 0 }.count,
            ] as [String: Any]
        }
        return try jsonString([
            "width": project.width,
            "height": project.height,
            "imageCount": project.images.count,
            "selectedImageIndex": project.selectedImageIndex,
            "images": images,
        ])
    }

    private func asciiPreview(_ arguments: [String: Any]) throws -> String {
        let project = try NovaDrawProjectCodec.load(from: try projectURL(arguments))
        let imageIndex = try imageIndex(arguments, project: project)
        let x = int(arguments, "x") ?? 0
        let y = int(arguments, "y") ?? 0
        let width = int(arguments, "width") ?? project.width
        let height = int(arguments, "height") ?? project.height
        let rows = try NovaDrawDrawing.rows(
            from: project,
            imageIndex: imageIndex,
            x: x,
            y: y,
            width: width,
            height: height
        )
        return rows.joined(separator: "\n")
    }

    private func mutate(_ arguments: [String: Any],
                        imageIndexArgumentRequired: Bool = false,
                        _ update: (inout NovaDrawProject, Int) throws -> String) throws -> String {
        let url = try projectURL(arguments)
        var project = try NovaDrawProjectCodec.load(from: url)
        let imageIndex = imageIndexArgumentRequired
            ? try requiredInt(arguments, "image_index")
            : try imageIndex(arguments, project: project)
        try project.validateImageIndex(imageIndex)
        let message = try update(&project, imageIndex)
        try NovaDrawProjectCodec.save(project, to: url)
        return message
    }

    private func mutateBackdrop(_ arguments: [String: Any]) throws -> String {
        let url = try projectURL(arguments)
        let imageURL = try fileURL(try requiredString(arguments, "image_path"))
        var project = try NovaDrawProjectCodec.load(from: url)
        project.backdropData = try Data(contentsOf: imageURL)
        project.header.backdropImageLength = project.backdropData.count
        project.header.backdropPlacementMode = "fitToCanvas"
        project.header.backdropScale = 1.0
        project.header.backdropOffsetX = 0
        project.header.backdropOffsetY = 0
        project.header.backdropImageOpacity = clampedDouble(arguments, "image_opacity") ?? 1.0
        project.header.backdropOpacity = clampedDouble(arguments, "drawing_opacity") ?? 0.75
        try NovaDrawProjectCodec.save(project, to: url)
        return "Set fit-to-canvas backdrop from \(imageURL.path)."
    }

    private func exportNvg(_ arguments: [String: Any]) throws -> String {
        let project = try NovaDrawProjectCodec.load(from: try projectURL(arguments))
        guard project.width == 320, project.height == 200 else {
            throw NovaDrawProjectError.invalidArgument("canvas")
        }
        let imageIndex = try imageIndex(arguments, project: project)
        let outputURL = try fileURL(try requiredString(arguments, "output_path"))
        try FileManager.default.createDirectory(
            at: outputURL.deletingLastPathComponent(),
            withIntermediateDirectories: true
        )
        let data = NovaDrawNvgFormat.encode(
            pixels: project.images[imageIndex].pixels,
            paintedPixels: project.images[imageIndex].paintedPixels,
            width: project.width,
            height: project.height
        )
        try data.write(to: outputURL, options: Data.WritingOptions.atomic)
        return "Exported NVG1 to \(outputURL.path)."
    }

    private struct PixelBlock {
        var width: Int
        var height: Int
        var pixels: [UInt8]
        var paintedPixels: [UInt8]
    }

    private func pixelBlock(_ arguments: [String: Any]) throws -> PixelBlock {
        if let rows = stringArray(arguments, "rows") {
            return try pixelBlock(rows: rows)
        }

        let width = try requiredInt(arguments, "width")
        let height = try requiredInt(arguments, "height")
        let expected = width * height
        let pixels = try intArray(arguments, "pixels").map(NovaDrawDrawing.sanitizeColor)
        guard pixels.count == expected else {
            throw NovaDrawProjectError.invalidPixelCount(expected: expected, actual: pixels.count)
        }
        let painted: [UInt8]
        if let paintedValues = boolArray(arguments, "painted") {
            guard paintedValues.count == expected else {
                throw NovaDrawProjectError.invalidPixelCount(expected: expected, actual: paintedValues.count)
            }
            painted = paintedValues.map { $0 ? 255 : 0 }
        } else {
            painted = [UInt8](repeating: 255, count: expected)
        }
        return PixelBlock(width: width, height: height, pixels: pixels, paintedPixels: painted)
    }

    private func pixelBlock(rows: [String]) throws -> PixelBlock {
        guard !rows.isEmpty else { throw NovaDrawProjectError.invalidArgument("rows") }
        let width = rows.map(\.count).max() ?? 0
        let height = rows.count
        guard width > 0 else { throw NovaDrawProjectError.invalidArgument("rows") }

        var pixels: [UInt8] = []
        var painted: [UInt8] = []
        for row in rows {
            for character in row {
                if character == "." || character == " " {
                    pixels.append(0)
                    painted.append(0)
                    continue
                }
                guard let value = character.hexDigitValue, value >= 0, value <= 15 else {
                    throw NovaDrawProjectError.invalidArgument("rows")
                }
                pixels.append(UInt8(value))
                painted.append(255)
            }
            while pixels.count % width != 0 {
                pixels.append(0)
                painted.append(0)
            }
        }
        return PixelBlock(width: width, height: height, pixels: pixels, paintedPixels: painted)
    }
}

private func tool(name: String,
                  description: String,
                  properties: [String: Any],
                  required: [String]) -> [String: Any] {
    [
        "name": name,
        "description": description,
        "inputSchema": [
            "type": "object",
            "properties": properties,
            "required": required,
            "additionalProperties": false,
        ] as [String: Any],
    ]
}

private func regionProperties(extra: [String: Any] = [:]) -> [String: Any] {
    var properties: [String: Any] = [
        "path": string("Path to the .novadraw project."),
        "x": integer("Region x.", minimum: 0),
        "y": integer("Region y.", minimum: 0),
        "width": integer("Region width.", minimum: 1),
        "height": integer("Region height.", minimum: 1),
    ]
    for (key, value) in extra {
        properties[key] = value
    }
    return properties
}

private func string(_ description: String) -> [String: Any] {
    ["type": "string", "description": description]
}

private func integer(_ description: String, minimum: Int? = nil, maximum: Int? = nil) -> [String: Any] {
    var schema: [String: Any] = ["type": "integer", "description": description]
    if let minimum { schema["minimum"] = minimum }
    if let maximum { schema["maximum"] = maximum }
    return schema
}

private func boolean(_ description: String) -> [String: Any] {
    ["type": "boolean", "description": description]
}

private func number(_ description: String, minimum: Double? = nil, maximum: Double? = nil) -> [String: Any] {
    var schema: [String: Any] = ["type": "number", "description": description]
    if let minimum { schema["minimum"] = minimum }
    if let maximum { schema["maximum"] = maximum }
    return schema
}

private func array(_ items: [String: Any], _ description: String) -> [String: Any] {
    ["type": "array", "description": description, "items": items]
}

private func projectURL(_ arguments: [String: Any]) throws -> URL {
    let path = try requiredString(arguments, "path")
    return try fileURL(path)
}

private func fileURL(_ path: String) throws -> URL {
    let expanded = (path as NSString).expandingTildeInPath
    return URL(fileURLWithPath: expanded)
}

private func imageIndex(_ arguments: [String: Any], project: NovaDrawProject) throws -> Int {
    let index = int(arguments, "image_index") ?? project.selectedImageIndex
    guard project.images.indices.contains(index) else {
        throw NovaDrawProjectError.invalidImageIndex(index)
    }
    return index
}

private func requiredString(_ arguments: [String: Any], _ name: String) throws -> String {
    guard let value = string(arguments, name) else {
        throw NovaDrawProjectError.missingArgument(name)
    }
    return value
}

private func string(_ arguments: [String: Any], _ name: String) -> String? {
    arguments[name] as? String
}

private func requiredInt(_ arguments: [String: Any], _ name: String) throws -> Int {
    guard let value = int(arguments, name) else {
        throw NovaDrawProjectError.missingArgument(name)
    }
    return value
}

private func int(_ arguments: [String: Any], _ name: String) -> Int? {
    if let value = arguments[name] as? Int { return value }
    if let value = arguments[name] as? Double { return Int(value) }
    if let value = arguments[name] as? NSNumber { return value.intValue }
    return nil
}

private func bool(_ arguments: [String: Any], _ name: String) -> Bool? {
    if let value = arguments[name] as? Bool { return value }
    if let value = arguments[name] as? NSNumber { return value.boolValue }
    return nil
}

private func double(_ arguments: [String: Any], _ name: String) -> Double? {
    if let value = arguments[name] as? Double { return value }
    if let value = arguments[name] as? Int { return Double(value) }
    if let value = arguments[name] as? NSNumber { return value.doubleValue }
    return nil
}

private func clampedDouble(_ arguments: [String: Any], _ name: String) -> Double? {
    guard let value = double(arguments, name), value.isFinite else { return nil }
    return min(max(value, 0), 1)
}

private func stringArray(_ arguments: [String: Any], _ name: String) -> [String]? {
    arguments[name] as? [String]
}

private func intArray(_ arguments: [String: Any], _ name: String) throws -> [Int] {
    guard let values = arguments[name] as? [Any] else {
        throw NovaDrawProjectError.missingArgument(name)
    }
    return try values.enumerated().map { offset, value in
        if let int = value as? Int { return int }
        if let double = value as? Double { return Int(double) }
        if let number = value as? NSNumber { return number.intValue }
        throw NovaDrawProjectError.invalidArgument("\(name)[\(offset)]")
    }
}

private func boolArray(_ arguments: [String: Any], _ name: String) -> [Bool]? {
    guard let values = arguments[name] as? [Any] else { return nil }
    return values.map { value in
        if let bool = value as? Bool { return bool }
        if let number = value as? NSNumber { return number.boolValue }
        return false
    }
}

private func jsonString(_ object: Any) throws -> String {
    let data = try JSONSerialization.data(withJSONObject: object, options: [.prettyPrinted, .sortedKeys])
    return String(data: data, encoding: .utf8) ?? "{}"
}
