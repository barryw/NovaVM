import PixelCanvasKit

/// NovaDraw-specific conveniences on the shared `PixelDocument`. The package
/// stays product-agnostic; the `.novadraw` project naming lives here.
@MainActor
extension PixelDocument {
    /// Default file name offered in the save panel for a project export.
    var suggestedProjectName: String {
        "\(displayName).novadraw"
    }
}
