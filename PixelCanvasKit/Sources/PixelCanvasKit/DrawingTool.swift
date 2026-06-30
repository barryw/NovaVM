/// The pixel-editing tools a ``ToolEngine`` can apply to a ``PixelDocument``.
///
/// `shortcut` and `systemImage` are presentation hints (keyboard mnemonic and
/// SF Symbol name) consumers may use to build a tool palette; the engine itself
/// only switches on the case.
public enum DrawingTool: String, CaseIterable, Codable, Sendable {
    case pencil, eraser, line, rect, circle, fill, eyedropper, select

    public var shortcut: Character {
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

    public var label: String { rawValue.capitalized }

    public var systemImage: String {
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
