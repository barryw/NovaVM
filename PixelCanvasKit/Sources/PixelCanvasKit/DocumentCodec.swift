import Foundation

/// The platform/container seam for *whole-document* persistence.
///
/// PixelCanvasKit deliberately does **not** know how a document is serialised —
/// that is product-specific (NovaDraw's `.novadraw` project bundle, its `.nvg`
/// image container, a future FamiForge CHR/attribute format, …). Each consumer
/// provides a type conforming to `DocumentCodec`; the engine just moves
/// `Value`s through it.
///
/// `Value` is generic rather than fixed to ``PixelDocument`` so a codec may
/// round-trip either a live document or a lighter value type (e.g. a decoded
/// `(width, height, pixels)` payload) as appropriate.
public protocol DocumentCodec {
    associatedtype Value

    /// File extension this codec reads/writes, without the leading dot (e.g. `"nvg"`).
    static var fileExtension: String { get }

    /// Human-readable name for menus / open panels (e.g. `"Nova Graphics"`).
    static var displayName: String { get }

    func encode(_ value: Value) throws -> Data
    func decode(_ data: Data) throws -> Value
}
