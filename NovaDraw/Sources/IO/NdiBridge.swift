import Foundation

/// Reads/writes `.nsp` sprite banks inside NDI disk images by delegating to the
/// `nova` CLI, which owns the tested NDI filesystem. NovaDraw only speaks the
/// NSPR format (`NsprFormat`); the container is the CLI's job.
///
/// Configure how to launch the CLI: `executable` is either the `nova` binary on
/// PATH (default `"nova"`) or an absolute path, and `baseArgs` lets you front a
/// launcher, e.g. `executable: "dotnet", baseArgs: ["run", "--project",
/// "…/e6502.Nova", "--"]`.
struct NdiBridge {
    var executable: String
    var baseArgs: [String]

    init(executable: String = "nova", baseArgs: [String] = []) {
        self.executable = executable
        self.baseArgs = baseArgs
    }

    enum BridgeError: LocalizedError {
        case launchFailed(String)
        case commandFailed(command: String, status: Int32, stderr: String)
        case exportMissing(String)

        var errorDescription: String? {
            switch self {
            case .launchFailed(let m): return "Could not launch the nova CLI: \(m)"
            case .commandFailed(let cmd, let status, let err):
                return "nova \(cmd) failed (exit \(status)): \(err.trimmingCharacters(in: .whitespacesAndNewlines))"
            case .exportMissing(let path): return "nova export produced no file at \(path)"
            }
        }
    }

    /// Extract `name` (e.g. `"HERO.NSP"`) from `ndiPath` and decode it.
    func loadBank(named name: String, fromNdi ndiPath: String) throws -> NsprBank {
        let tmp = try makeTempDir()
        defer { try? FileManager.default.removeItem(at: tmp) }
        try run(["export", ndiPath, name, tmp.path])
        let out = tmp.appendingPathComponent((name as NSString).lastPathComponent)
        guard let data = FileManager.default.contents(atPath: out.path) else {
            throw BridgeError.exportMissing(out.path)
        }
        return try NsprBank.decode(data)
    }

    /// Encode `bank` and import it into `ndiPath` at the root as `name`.
    func saveBank(_ bank: NsprBank, named name: String, toNdi ndiPath: String) throws {
        let data = try bank.encoded()
        let tmp = try makeTempDir()
        defer { try? FileManager.default.removeItem(at: tmp) }
        let host = tmp.appendingPathComponent((name as NSString).lastPathComponent)
        try data.write(to: host)
        try run(["import", ndiPath, host.path, "/"])
    }

    /// Filenames ending in `.nsp` in the image root (for a picker). The parser
    /// is whitespace-lenient over `nova dir` output; tighten if the listing
    /// format changes.
    func listNspFiles(inNdi ndiPath: String) throws -> [String] {
        let out = try run(["dir", ndiPath])
        return out
            .split(whereSeparator: { $0 == "\n" || $0 == "\r" || $0 == " " || $0 == "\t" })
            .map(String.init)
            .filter { $0.lowercased().hasSuffix(".nsp") }
    }

    /// Checks the configured CLI can be launched. Returns `nil` on success, or a
    /// user-facing message if the executable can't be found/run. A non-zero exit
    /// from the probe command still counts as launchable (the path is valid).
    func verify() -> String? {
        do {
            _ = try run(["--help"])
            return nil
        } catch let error as BridgeError {
            if case .launchFailed = error { return error.errorDescription }
            return nil
        } catch {
            return error.localizedDescription
        }
    }

    // MARK: - Process plumbing

    @discardableResult
    private func run(_ args: [String]) throws -> String {
        let process = Process()
        if executable.contains("/") {
            process.executableURL = URL(fileURLWithPath: executable)
            process.arguments = baseArgs + args
        } else {
            // Resolve a bare command name on PATH via env.
            process.executableURL = URL(fileURLWithPath: "/usr/bin/env")
            process.arguments = [executable] + baseArgs + args
        }

        let outPipe = Pipe()
        let errPipe = Pipe()
        process.standardOutput = outPipe
        process.standardError = errPipe

        do {
            try process.run()
        } catch {
            throw BridgeError.launchFailed("\(executable): \(error.localizedDescription)")
        }

        let outData = outPipe.fileHandleForReading.readDataToEndOfFile()
        let errData = errPipe.fileHandleForReading.readDataToEndOfFile()
        process.waitUntilExit()

        if process.terminationStatus != 0 {
            throw BridgeError.commandFailed(
                command: args.joined(separator: " "),
                status: process.terminationStatus,
                stderr: String(decoding: errData, as: UTF8.self))
        }
        return String(decoding: outData, as: UTF8.self)
    }

    private func makeTempDir() throws -> URL {
        let dir = FileManager.default.temporaryDirectory
            .appendingPathComponent("novadraw-ndi-\(UUID().uuidString)")
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        return dir
    }
}
