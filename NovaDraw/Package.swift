// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "NovaDraw",
    platforms: [.macOS(.v14)],
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
        .testTarget(
            name: "NovaDrawTests",
            dependencies: ["NovaDraw"],
            path: "Tests"
        )
    ]
)
