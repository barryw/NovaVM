import SwiftUI
import PixelCanvasKit

struct InspectorView: View {
    let document: PixelDocument

    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            inspectorSection("Canvas") {
                LabeledContent("Size") {
                    Text("\(document.width) × \(document.height)")
                        .foregroundStyle(.secondary)
                }
            }

            inspectorSection("Image") {
                TextField("Name", text: selectedImageNameBinding)
                    .textFieldStyle(.roundedBorder)
                    .font(.system(.body, design: .monospaced))

                LabeledContent("Slot") {
                    Text(document.selectedImageLabel)
                        .font(.caption.monospacedDigit())
                        .foregroundStyle(.secondary)
                }
            }

            inspectorSection("Backdrop") {
                Picker("Placement", selection: Binding(
                    get: { document.backdropPlacementMode },
                    set: { document.backdropPlacementMode = $0 }
                )) {
                    ForEach(BackdropPlacementMode.allCases, id: \.self) { mode in
                        Text(mode.label).tag(mode)
                    }
                }
                .pickerStyle(.segmented)
                .controlSize(.small)
                .disabled(document.backdropImage == nil)

                VStack(alignment: .leading, spacing: 4) {
                    Text("Backdrop")
                        .font(.caption)
                        .foregroundStyle(.secondary)
                    Slider(
                        value: Binding(
                            get: { document.backdropImageOpacity },
                            set: { document.backdropImageOpacity = $0 }
                        ),
                        in: 0...1
                    )
                    .controlSize(.small)
                }

                if document.backdropPlacementMode == .floating {
                    VStack(alignment: .leading, spacing: 4) {
                        HStack {
                            Text("Scale")
                                .font(.caption)
                                .foregroundStyle(.secondary)
                            Spacer()
                            Text("\(Double(document.backdropScale), format: .number.precision(.fractionLength(3)))×")
                                .font(.caption.monospacedDigit())
                                .foregroundStyle(.secondary)
                        }
                        Slider(value: backdropScaleBinding, in: -8...4)
                            .controlSize(.small)
                            .disabled(document.backdropImage == nil)
                            .help("Scale floating backdrop")
                    }

                    LabeledContent("Offset") {
                        Text("\(Int(document.backdropOffset.x.rounded())), \(Int(document.backdropOffset.y.rounded()))")
                            .font(.caption.monospacedDigit())
                            .foregroundStyle(.secondary)
                    }

                    Button {
                        document.resetBackdropPlacement()
                    } label: {
                        Label("Reset Placement", systemImage: "arrow.counterclockwise")
                    }
                    .controlSize(.small)
                    .disabled(document.backdropImage == nil)
                }

                VStack(alignment: .leading, spacing: 4) {
                    Text("Drawing")
                        .font(.caption)
                        .foregroundStyle(.secondary)
                    Slider(
                        value: Binding(
                            get: { document.backdropOpacity },
                            set: { document.backdropOpacity = $0 }
                        ),
                        in: 0...1
                    )
                    .controlSize(.small)
                }
            }

            inspectorSection("Grid") {
                VStack(alignment: .leading, spacing: 4) {
                    Text("Major lines")
                        .font(.caption)
                        .foregroundStyle(.secondary)
                        .lineLimit(1)

                    Picker("Major lines", selection: Binding(
                        get: { document.gridMajor },
                        set: { document.gridMajor = $0 }
                    )) {
                        Text("8px").tag(8)
                        Text("16px").tag(16)
                        Text("20px").tag(20)
                    }
                    .labelsHidden()
                    .pickerStyle(.segmented)
                    .controlSize(.small)
                }
            }

            Spacer()
        }
        .padding(10)
        .frame(width: 180)
        .background(.bar)
    }

    private func inspectorSection<Content: View>(_ title: String, @ViewBuilder content: () -> Content) -> some View {
        VStack(alignment: .leading, spacing: 6) {
            Text(title)
                .font(.system(size: 11, weight: .semibold))
                .foregroundStyle(.secondary)
                .textCase(.uppercase)
            content()
        }
    }

    private var backdropScaleBinding: Binding<Double> {
        Binding(
            get: {
                log2(Double(PixelDocument.clampedBackdropScale(document.backdropScale)))
            },
            set: {
                document.setBackdropScale(CGFloat(pow(2, $0)), around: document.canvasCenter)
            }
        )
    }

    private var selectedImageNameBinding: Binding<String> {
        Binding(
            get: {
                guard document.images.indices.contains(document.selectedImageIndex) else { return "" }
                return document.images[document.selectedImageIndex].name
            },
            set: {
                document.renameSelectedImage(to: $0)
            }
        )
    }
}
