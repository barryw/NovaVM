import AppKit
import SwiftUI

struct TraceSheetImportRequest: Identifiable {
    let id = UUID()
    let url: URL
    let image: NSImage
    let sourcePixelWidth: Int
    let sourcePixelHeight: Int
}

struct TraceSheetImportSheet: View {
    let request: TraceSheetImportRequest
    let onCancel: () -> Void
    let onImport: (TraceSheetImportRequest, TraceSheetImportSettings) -> Bool

    @State private var settings: TraceSheetImportSettings

    init(
        request: TraceSheetImportRequest,
        onCancel: @escaping () -> Void,
        onImport: @escaping (TraceSheetImportRequest, TraceSheetImportSettings) -> Bool
    ) {
        self.request = request
        self.onCancel = onCancel
        self.onImport = onImport
        _settings = State(initialValue: TraceSheetImportSettings.recommended(
            sourcePixelWidth: request.sourcePixelWidth,
            sourcePixelHeight: request.sourcePixelHeight
        ))
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 14) {
            header

            HStack(alignment: .top, spacing: 18) {
                preview

                VStack(alignment: .leading, spacing: 12) {
                    section("Source Grid") {
                        numberPair(
                            firstTitle: "Columns",
                            firstValue: $settings.columns,
                            firstRange: 1...16,
                            secondTitle: "Rows",
                            secondValue: $settings.rows,
                            secondRange: 1...16
                        )

                        stepperRow(
                            title: "Inset",
                            value: $settings.sourceInset,
                            range: 0...maxSourceInset,
                            suffix: "px"
                        )
                    }

                    section("Target Cells") {
                        numberPair(
                            firstTitle: "Width",
                            firstValue: $settings.cellWidth,
                            firstRange: 1...64,
                            secondTitle: "Height",
                            secondValue: $settings.cellHeight,
                            secondRange: 1...64
                        )

                        LabeledContent("Canvas") {
                            Text("\(settings.outputWidth) x \(settings.outputHeight)")
                                .monospacedDigit()
                                .foregroundStyle(.secondary)
                        }
                    }

                    section("Guide") {
                        Picker("Mode", selection: $settings.renderMode) {
                            ForEach(TraceSheetRenderMode.allCases, id: \.self) { mode in
                                Text(mode.label).tag(mode)
                            }
                        }
                        .pickerStyle(.segmented)
                        .controlSize(.small)
                    }

                    section("Opacity") {
                        sliderRow(title: "Backdrop", value: $settings.backdropImageOpacity)
                        sliderRow(title: "Drawing", value: $settings.drawingOpacity)
                    }
                }
                .frame(width: 250)
            }

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

                Button("Create \(settings.outputWidth)x\(settings.outputHeight) Trace Canvas") {
                    if onImport(request, settings) {
                        onCancel()
                    }
                }
                .keyboardShortcut(.return)
                .disabled(validationMessage != nil)
            }
        }
        .padding(20)
        .frame(width: 680)
    }

    private var header: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("Trace Sheet Import")
                .font(.title3.weight(.semibold))
            Text("\(request.url.lastPathComponent) - \(request.sourcePixelWidth)x\(request.sourcePixelHeight)")
                .font(.caption)
                .foregroundStyle(.secondary)
                .lineLimit(1)
                .truncationMode(.middle)
        }
    }

    private var preview: some View {
        VStack(alignment: .leading, spacing: 6) {
            ZStack {
                Rectangle()
                    .fill(.black.opacity(0.28))

                if let image = previewImage {
                    Image(nsImage: image)
                        .interpolation(.none)
                        .resizable()
                        .scaledToFit()
                        .padding(10)
                }
            }
            .frame(width: 340, height: 180)
            .clipShape(RoundedRectangle(cornerRadius: 6))
            .overlay(
                RoundedRectangle(cornerRadius: 6)
                    .stroke(.white.opacity(0.14), lineWidth: 1)
            )

            Text("\(settings.columns)x\(settings.rows) cells, \(sourceCellSummary) source -> \(settings.cellWidth)x\(settings.cellHeight)")
                .font(.caption)
                .foregroundStyle(.secondary)
                .lineLimit(1)
        }
    }

    private var previewImage: NSImage? {
        TraceSheetRenderer.renderBackdrop(
            image: request.image,
            sourcePixelWidth: request.sourcePixelWidth,
            sourcePixelHeight: request.sourcePixelHeight,
            settings: settings
        )
    }

    private var sourceCellSummary: String {
        guard settings.columns > 0, settings.rows > 0 else { return "--" }
        let width = Double(request.sourcePixelWidth) / Double(settings.columns)
        let height = Double(request.sourcePixelHeight) / Double(settings.rows)
        return "\(Int(width.rounded()))x\(Int(height.rounded()))"
    }

    private var maxSourceInset: Int {
        guard settings.columns > 0, settings.rows > 0 else { return 0 }
        let sourceCellWidth = Double(request.sourcePixelWidth) / Double(settings.columns)
        let sourceCellHeight = Double(request.sourcePixelHeight) / Double(settings.rows)
        return max(0, Int(min(sourceCellWidth, sourceCellHeight) / 2) - 1)
    }

    private var validationMessage: String? {
        settings.validationMessage(
            sourcePixelWidth: request.sourcePixelWidth,
            sourcePixelHeight: request.sourcePixelHeight
        )
    }

    private func section<Content: View>(_ title: String, @ViewBuilder content: () -> Content) -> some View {
        VStack(alignment: .leading, spacing: 7) {
            Text(title)
                .font(.system(size: 11, weight: .semibold))
                .foregroundStyle(.secondary)
                .textCase(.uppercase)
            content()
        }
    }

    private func numberPair(
        firstTitle: String,
        firstValue: Binding<Int>,
        firstRange: ClosedRange<Int>,
        secondTitle: String,
        secondValue: Binding<Int>,
        secondRange: ClosedRange<Int>
    ) -> some View {
        HStack(spacing: 8) {
            numericField(title: firstTitle, value: firstValue, range: firstRange)
            Text("x")
                .foregroundStyle(.secondary)
            numericField(title: secondTitle, value: secondValue, range: secondRange)
        }
    }

    private func numericField(title: String, value: Binding<Int>, range: ClosedRange<Int>) -> some View {
        VStack(alignment: .leading, spacing: 3) {
            Text(title)
                .font(.caption)
                .foregroundStyle(.secondary)
            Stepper(value: clamped(value, range: range), in: range) {
                TextField(title, value: clamped(value, range: range), format: .number)
                    .textFieldStyle(.roundedBorder)
                    .monospacedDigit()
                    .frame(width: 54)
            }
        }
    }

    private func stepperRow(title: String, value: Binding<Int>, range: ClosedRange<Int>, suffix: String) -> some View {
        LabeledContent(title) {
            Stepper(value: clamped(value, range: range), in: range) {
                Text("\(value.wrappedValue) \(suffix)")
                    .monospacedDigit()
                    .frame(minWidth: 54, alignment: .trailing)
            }
        }
    }

    private func sliderRow(title: String, value: Binding<Double>) -> some View {
        VStack(alignment: .leading, spacing: 3) {
            HStack {
                Text(title)
                    .font(.caption)
                    .foregroundStyle(.secondary)
                Spacer()
                Text("\(Int((value.wrappedValue * 100).rounded()))%")
                    .font(.caption)
                    .monospacedDigit()
                    .foregroundStyle(.secondary)
            }
            Slider(value: value, in: 0...1)
                .controlSize(.small)
        }
    }

    private func clamped(_ value: Binding<Int>, range: ClosedRange<Int>) -> Binding<Int> {
        Binding(
            get: { min(max(value.wrappedValue, range.lowerBound), range.upperBound) },
            set: { value.wrappedValue = min(max($0, range.lowerBound), range.upperBound) }
        )
    }
}
