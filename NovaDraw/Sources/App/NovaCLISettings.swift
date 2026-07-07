import Foundation

/// Persisted location of the `nova` CLI, used to reach NDI disk images through
/// `NdiBridge`. The value is either a bare command found on PATH (default
/// `"nova"`) or an absolute path to the executable. Stored in `UserDefaults`
/// under `novaCliPath` — the same key the Settings UI binds via `@AppStorage`.
enum NovaCLISettings {
    static let pathKey = "novaCliPath"
    static let defaultPath = "nova"

    static var path: String {
        get { UserDefaults.standard.string(forKey: pathKey) ?? defaultPath }
        set { UserDefaults.standard.set(newValue, forKey: pathKey) }
    }

    /// A bridge configured from the stored CLI location.
    static func bridge() -> NdiBridge {
        NdiBridge(executable: path.isEmpty ? defaultPath : path)
    }
}
