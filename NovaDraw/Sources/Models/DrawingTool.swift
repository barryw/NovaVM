enum DrawingTool: String, CaseIterable, Codable, Sendable {
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

    var systemImage: String {
        switch self {
        case .pencil: "pencil"
        case .eraser: "eraser"
        case .line: "line.diagonal"
        case .rect: "rectangle"
        case .circle: "circle"
        case .fill: "paintbrush.fill"
        case .eyedropper: "eyedropper"
        case .select: "selection.pin.in.out"
        }
    }
}
