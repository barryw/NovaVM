import Foundation

/// The platform/target seam for *exporting* a document into a downstream build
/// artifact (assembly source, binary CHR, a sprite sheet, …).
///
/// Unlike ``DocumentCodec`` — which is a reversible persistence format — an
/// `Exporter` is a one-way producer aimed at a specific toolchain target.
/// NovaDraw's ca65 assembly exporter conforms to this; FamiForge will add NES
/// exporters. The engine stays agnostic to every target.
public protocol Exporter {
    associatedtype Input

    /// Stable identifier for the target (e.g. `"ca65"`, `"nes-chr"`).
    static var identifier: String { get }

    /// File extension of the produced artifact, without the leading dot.
    static var fileExtension: String { get }

    func export(_ input: Input) throws -> Data
}
