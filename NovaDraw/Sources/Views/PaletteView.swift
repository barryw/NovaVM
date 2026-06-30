import SwiftUI
import PixelCanvasKit

struct PaletteView: View {
    let document: PixelDocument

    private let columns = [
        GridItem(.fixed(22), spacing: 2),
        GridItem(.fixed(22), spacing: 2),
    ]

    var body: some View {
        VStack(spacing: 8) {
            fgBgIndicator
                .padding(.top, 6)

            LazyVGrid(columns: columns, spacing: 2) {
                ForEach(0..<16, id: \.self) { i in
                    colorSwatch(i)
                }
            }
            .padding(.horizontal, 4)

            Spacer()
        }
        .frame(width: 56)
        .background(.bar)
    }

    private var fgBgIndicator: some View {
        ZStack(alignment: .bottomTrailing) {
            RoundedRectangle(cornerRadius: 3)
                .fill(NovaPalette.swiftUIColor(Int(document.backgroundColor)))
                .frame(width: 22, height: 22)
                .overlay(RoundedRectangle(cornerRadius: 3).stroke(.tertiary, lineWidth: 1))
                .offset(x: 6, y: 6)

            RoundedRectangle(cornerRadius: 3)
                .fill(NovaPalette.swiftUIColor(Int(document.foregroundColor)))
                .frame(width: 22, height: 22)
                .overlay(RoundedRectangle(cornerRadius: 3).stroke(.white.opacity(0.8), lineWidth: 1.5))
        }
        .frame(width: 34, height: 34)
    }

    private func colorSwatch(_ index: Int) -> some View {
        let isSelected = document.foregroundColor == UInt8(index)
        return RoundedRectangle(cornerRadius: 3)
            .fill(NovaPalette.swiftUIColor(index))
            .frame(width: 22, height: 22)
            .overlay(
                RoundedRectangle(cornerRadius: 3)
                    .stroke(isSelected ? .white : .white.opacity(0.15), lineWidth: isSelected ? 2 : 0.5)
            )
            .shadow(color: isSelected ? .white.opacity(0.4) : .clear, radius: 2)
            .onTapGesture { document.foregroundColor = UInt8(index) }
            .help(NovaPalette.names[index])
    }
}
