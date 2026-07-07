import AppKit
import SwiftUI

/// Preferences window (⌘,). Currently just the nova CLI location that
/// `NdiBridge` uses to read/write .nsp files inside NDI disk images.
struct SettingsView: View {
    @AppStorage(NovaCLISettings.pathKey) private var cliPath: String = NovaCLISettings.defaultPath

    private enum VerifyState: Equatable {
        case unknown
        case ok
        case failed(String)
    }

    @State private var verifyState: VerifyState = .unknown

    var body: some View {
        Form {
            Section("Nova CLI") {
                Text("NovaDraw shells out to the nova command to read and write .nsp sprite banks inside NDI disk images. Enter a command on your PATH (e.g. \"nova\") or an absolute path to the executable.")
                    .font(.callout)
                    .foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)

                HStack {
                    TextField("nova", text: $cliPath)
                        .textFieldStyle(.roundedBorder)
                    Button("Choose…") { chooseExecutable() }
                }

                HStack(spacing: 8) {
                    Button("Verify") { verify() }
                    switch verifyState {
                    case .unknown:
                        EmptyView()
                    case .ok:
                        Label("Found", systemImage: "checkmark.circle.fill")
                            .foregroundStyle(.green)
                    case .failed(let message):
                        Label(message, systemImage: "xmark.circle.fill")
                            .foregroundStyle(.red)
                            .lineLimit(2)
                    }
                    Spacer()
                }
            }
        }
        .formStyle(.grouped)
        .frame(width: 520)
        .onChange(of: cliPath) { _, _ in verifyState = .unknown }
    }

    private func chooseExecutable() {
        let panel = NSOpenPanel()
        panel.title = "Choose the nova CLI executable"
        panel.canChooseFiles = true
        panel.canChooseDirectories = false
        panel.allowsMultipleSelection = false
        panel.treatsFilePackagesAsDirectories = true
        if panel.runModal() == .OK, let url = panel.url {
            cliPath = url.path
        }
    }

    private func verify() {
        let path = cliPath.isEmpty ? NovaCLISettings.defaultPath : cliPath
        Task.detached {
            let message = NdiBridge(executable: path).verify()
            await MainActor.run {
                verifyState = message == nil ? .ok : .failed(message!)
            }
        }
    }
}
