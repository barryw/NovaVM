import SwiftUI

struct NewCanvasSheet: View {
    let onCreate: (Int, Int) -> Void
    let onOpenProject: (() -> Void)?
    @State private var selectedPreset = 3
    @State private var customWidth = 64
    @State private var customHeight = 64

    private let presets: [(String, Int, Int)] = [
        ("Sprite (16×16)", 16, 16),
        ("32×32", 32, 32),
        ("Custom", 0, 0),
        ("Full Screen (320×200)", 320, 200),
    ]

    var body: some View {
        VStack(spacing: 16) {
            Text("New Canvas").font(.title2)
            Picker("Size", selection: $selectedPreset) {
                ForEach(0..<presets.count, id: \.self) { i in
                    Text(presets[i].0).tag(i)
                }
            }
            .pickerStyle(.radioGroup)

            if selectedPreset == 2 {
                HStack {
                    TextField("W", value: $customWidth, format: .number)
                        .frame(width: 60)
                    Text("×")
                    TextField("H", value: $customHeight, format: .number)
                        .frame(width: 60)
                }
            }

            HStack {
                if let onOpenProject {
                    Button("Open Project...") {
                        onOpenProject()
                    }
                }

                Spacer()

                Button("Create") {
                    let (w, h): (Int, Int)
                    if selectedPreset == 2 {
                        w = min(max(customWidth, 1), 320)
                        h = min(max(customHeight, 1), 200)
                    } else {
                        w = presets[selectedPreset].1
                        h = presets[selectedPreset].2
                    }
                    onCreate(w, h)
                }
                .keyboardShortcut(.return)
            }
        }
        .padding(32)
        .frame(width: 300)
    }
}
