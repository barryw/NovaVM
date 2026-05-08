import Foundation
import SwiftUI

struct Ca65ExportSheet: View {
    let document: NovaDocument
    let onCancel: () -> Void
    let onExport: (Ca65ExportOptions) -> Bool

    @State private var options: Ca65ExportOptions

    init(document: NovaDocument,
         onCancel: @escaping () -> Void,
         onExport: @escaping (Ca65ExportOptions) -> Bool) {
        self.document = document
        self.onCancel = onCancel
        self.onExport = onExport
        _options = State(initialValue: Ca65ExportOptions(
            kind: .blitterRectangles,
            scope: .allImages,
            symbolPrefix: Self.defaultSymbolPrefix(for: document.displayName),
            segmentName: "RODATA",
            transparentColor: 2
        ))
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 14) {
            header

            Divider()

            VStack(alignment: .leading, spacing: 12) {
                Picker("Format", selection: $options.kind) {
                    ForEach(Ca65ExportKind.allCases) { kind in
                        Text(kind.label).tag(kind)
                    }
                }

                Picker("Scope", selection: $options.scope) {
                    ForEach(Ca65ImageScope.allCases) { scope in
                        Text(scope.label).tag(scope)
                    }
                }
                .pickerStyle(.segmented)

                LabeledContent("Symbol Prefix") {
                    TextField("symbol", text: $options.symbolPrefix)
                        .textFieldStyle(.roundedBorder)
                        .font(.system(.body, design: .monospaced))
                        .frame(width: 260)
                }

                LabeledContent("Segment") {
                    TextField("segment", text: $options.segmentName)
                        .textFieldStyle(.roundedBorder)
                        .font(.system(.body, design: .monospaced))
                        .frame(width: 140)
                }

                LabeledContent("Transparent Color") {
                    HStack(spacing: 8) {
                        RoundedRectangle(cornerRadius: 3)
                            .fill(NovaPalette.swiftUIColor(Int(options.transparentColor)))
                            .frame(width: 18, height: 18)
                            .overlay(
                                RoundedRectangle(cornerRadius: 3)
                                    .stroke(.secondary.opacity(0.35), lineWidth: 1)
                            )
                        Stepper(value: transparentColorBinding, in: 0...15) {
                            Text("$\(String(format: "%02X", options.transparentColor))")
                                .font(.system(.body, design: .monospaced))
                                .frame(width: 36, alignment: .trailing)
                        }
                    }
                    .disabled(!options.kind.usesTransparentColor)
                    .opacity(options.kind.usesTransparentColor ? 1 : 0.45)
                }
            }

            Divider()

            Grid(alignment: .leadingFirstTextBaseline, horizontalSpacing: 18, verticalSpacing: 6) {
                GridRow {
                    Text("Canvas")
                        .foregroundStyle(.secondary)
                    Text("\(document.width)x\(document.height)")
                        .monospacedDigit()
                }
                GridRow {
                    Text("Images")
                        .foregroundStyle(.secondary)
                    Text("\(selectedImageCount)")
                        .monospacedDigit()
                }
                GridRow {
                    Text("Bytes/Image")
                        .foregroundStyle(.secondary)
                    Text("\(bytesPerImage)")
                        .monospacedDigit()
                }
            }
            .font(.caption)

            if let validationMessage {
                Text(validationMessage)
                    .font(.caption)
                    .foregroundStyle(.red)
            }

            HStack {
                Button("Cancel") {
                    onCancel()
                }

                Spacer()

                Button("Export .inc") {
                    if onExport(options) {
                        onCancel()
                    }
                }
                .keyboardShortcut(.return)
                .disabled(validationMessage != nil)
            }
        }
        .padding(20)
        .frame(width: 460)
    }

    private var header: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("ca65 Export")
                .font(.title3.weight(.semibold))
            Text("\(document.displayName) - \(document.width)x\(document.height)")
                .font(.caption)
                .foregroundStyle(.secondary)
                .lineLimit(1)
                .truncationMode(.middle)
        }
    }

    private var selectedImageCount: Int {
        switch options.scope {
        case .selectedImage:
            return 1
        case .allImages:
            return document.images.count
        }
    }

    private var bytesPerImage: Int {
        switch options.kind {
        case .indexedBitmap, .blitterRectangles:
            return document.width * document.height
        case .vSpriteRecords:
            return 4 + document.width * document.height
        case .hardwareSprites:
            return 128
        case .bitMasks:
            return ((document.width + 7) / 8) * document.height
        }
    }

    private var validationMessage: String? {
        if options.kind == .hardwareSprites && (document.width > 16 || document.height > 16) {
            return "Hardware sprite export requires a canvas no larger than 16x16."
        }
        return nil
    }

    private var transparentColorBinding: Binding<Int> {
        Binding(
            get: { Int(options.transparentColor) },
            set: { options.transparentColor = UInt8(min(max($0, 0), 15)) }
        )
    }

    private static func defaultSymbolPrefix(for displayName: String) -> String {
        let allowed = CharacterSet.alphanumerics.union(CharacterSet(charactersIn: "_"))
        let scalars = displayName.lowercased().unicodeScalars
        var cleaned = String.UnicodeScalarView()
        var lastWasUnderscore = false
        let underscore = UnicodeScalar("_")
        for scalar in scalars {
            let next: UnicodeScalar = allowed.contains(scalar) ? scalar : underscore
            if next == underscore {
                if lastWasUnderscore { continue }
                lastWasUnderscore = true
            } else {
                lastWasUnderscore = false
            }
            cleaned.append(next)
        }

        let result = String(cleaned).trimmingCharacters(in: CharacterSet(charactersIn: "_"))
        return result.isEmpty ? "novadraw_images" : result
    }
}
