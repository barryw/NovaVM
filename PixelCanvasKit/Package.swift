// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "PixelCanvasKit",
    platforms: [.macOS(.v14)],
    products: [
        .library(name: "PixelCanvasKit", targets: ["PixelCanvasKit"]),
    ],
    targets: [
        .target(
            name: "PixelCanvasKit"
        ),
        .testTarget(
            name: "PixelCanvasKitTests",
            dependencies: ["PixelCanvasKit"]
        ),
    ]
)
