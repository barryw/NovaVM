import SwiftUI
import PixelCanvasKit

struct ImageStripView: View {
    let document: PixelDocument

    private let thumbnailSize: CGFloat = 38

    var body: some View {
        HStack(spacing: 8) {
            ScrollView(.horizontal, showsIndicators: false) {
                HStack(spacing: 6) {
                    ForEach(Array(document.images.enumerated()), id: \.element.id) { index, image in
                        imageButton(image, index: index)
                    }
                }
                .padding(.vertical, 6)
            }

            Divider()
                .frame(height: 28)

            Button {
                _ = document.addImage()
            } label: {
                Image(systemName: "plus")
                    .frame(width: 18, height: 18)
            }
            .buttonStyle(.borderless)
            .disabled(!document.canAddImage)
            .help("Add Image")

            Button {
                _ = document.deleteSelectedImage()
            } label: {
                Image(systemName: "trash")
                    .frame(width: 18, height: 18)
            }
            .buttonStyle(.borderless)
            .disabled(!document.canDeleteImage)
            .help("Delete Image")

            Text(document.selectedImageLabel)
                .font(.system(size: 11, design: .monospaced))
                .foregroundStyle(.secondary)
                .frame(width: 44, alignment: .trailing)
        }
        .padding(.horizontal, 8)
        .frame(height: 58)
        .background(.bar)
    }

    private func imageButton(_ image: CanvasImage, index: Int) -> some View {
        let isSelected = index == document.selectedImageIndex
        return Button {
            document.selectImage(at: index)
        } label: {
            VStack(spacing: 3) {
                thumbnail(image)
                    .frame(width: thumbnailSize, height: thumbnailSize)
                    .background(checkerboard)
                    .clipShape(RoundedRectangle(cornerRadius: 4))
                    .overlay(
                        RoundedRectangle(cornerRadius: 4)
                            .stroke(isSelected ? Color.accentColor : Color.white.opacity(0.18),
                                    lineWidth: isSelected ? 2 : 1)
                    )

                Text(image.name)
                    .font(.system(size: 9, weight: isSelected ? .semibold : .regular, design: .monospaced))
                    .foregroundStyle(isSelected ? .primary : .secondary)
                    .lineLimit(1)
                    .truncationMode(.tail)
                    .frame(width: 54)
            }
            .frame(width: 58, height: 50)
        }
        .buttonStyle(.plain)
        .help(image.name)
    }

    private func thumbnail(_ image: CanvasImage) -> some View {
        Canvas(rendersAsynchronously: false) { context, size in
            let scale = min(size.width / CGFloat(document.width),
                            size.height / CGFloat(document.height))
            let drawSize = CGSize(
                width: CGFloat(document.width) * scale,
                height: CGFloat(document.height) * scale
            )
            let origin = CGPoint(
                x: (size.width - drawSize.width) / 2,
                y: (size.height - drawSize.height) / 2
            )

            for y in 0..<document.height {
                for x in 0..<document.width {
                    let index = y * document.width + x
                    guard index < image.paintedPixels.count,
                          image.paintedPixels[index] != 0 else { continue }
                    let color = NovaPalette.swiftUIColor(Int(image.pixels[index]))
                    let rect = CGRect(
                        x: origin.x + CGFloat(x) * scale,
                        y: origin.y + CGFloat(y) * scale,
                        width: scale,
                        height: scale
                    )
                    context.fill(Path(rect), with: .color(color))
                }
            }
        }
    }

    private var checkerboard: some View {
        Canvas { context, size in
            let cell: CGFloat = 6
            let columns = Int(ceil(size.width / cell))
            let rows = Int(ceil(size.height / cell))
            for y in 0..<rows {
                for x in 0..<columns {
                    let shade = (x + y).isMultiple(of: 2) ? 0.24 : 0.18
                    let rect = CGRect(
                        x: CGFloat(x) * cell,
                        y: CGFloat(y) * cell,
                        width: cell,
                        height: cell
                    )
                    context.fill(Path(rect), with: .color(Color(white: shade)))
                }
            }
        }
    }
}
