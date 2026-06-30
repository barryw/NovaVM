// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "NovaDraw",
    platforms: [.macOS(.v14)],
    products: [
        .executable(name: "NovaDraw", targets: ["NovaDraw"]),
        .executable(name: "NovaDrawMCP", targets: ["NovaDrawMCP"]),
        .library(name: "NovaDrawMCPKit", targets: ["NovaDrawMCPKit"]),
    ],
    dependencies: [
        .package(url: "https://github.com/modelcontextprotocol/swift-sdk.git", from: "0.11.0"),
        .package(url: "https://github.com/barryw/PixelCanvasKit.git", from: "0.1.0"),
    ],
    targets: [
        .executableTarget(
            name: "NovaDraw",
            dependencies: [
                .product(name: "PixelCanvasKit", package: "PixelCanvasKit"),
            ],
            path: "Sources",
            linkerSettings: [
                .unsafeFlags([
                    "-Xlinker", "-sectcreate",
                    "-Xlinker", "__TEXT",
                    "-Xlinker", "__info_plist",
                    "-Xlinker", "Info.plist"
                ])
            ]
        ),
        .target(
            name: "NovaDrawMCPKit",
            path: "MCPKit"
        ),
        .executableTarget(
            name: "NovaDrawMCP",
            dependencies: [
                "NovaDrawMCPKit",
                .product(name: "MCP", package: "swift-sdk"),
            ],
            path: "MCPServer"
        ),
        .testTarget(
            name: "NovaDrawTests",
            dependencies: [
                "NovaDraw",
                "NovaDrawMCPKit",
                .product(name: "PixelCanvasKit", package: "PixelCanvasKit"),
            ],
            path: "Tests"
        ),
        .testTarget(
            name: "NovaDrawMCPTests",
            dependencies: ["NovaDrawMCPKit"],
            path: "MCPTests"
        )
    ]
)
