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
    ],
    targets: [
        .executableTarget(
            name: "NovaDraw",
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
            dependencies: ["NovaDraw", "NovaDrawMCPKit"],
            path: "Tests"
        ),
        .testTarget(
            name: "NovaDrawMCPTests",
            dependencies: ["NovaDrawMCPKit"],
            path: "MCPTests"
        )
    ]
)
