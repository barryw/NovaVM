import Testing
@testable import PixelCanvasKit

@Suite struct PaletteTests {
    @Test func novaPaletteHasSixteenNamedColors() {
        #expect(Palette.nova.count == 16)
        #expect(Palette.nova.entryNames.count == 16)
        #expect(Palette.nova.color(at: 0) == PaletteColor(0, 0, 0))
        #expect(Palette.nova.color(at: 1) == PaletteColor(255, 255, 255))
        #expect(Palette.nova.color(at: 2) == PaletteColor(136, 0, 0))
        #expect(Palette.nova.name(at: 0) == "Black")
        #expect(Palette.nova.name(at: 14) == "Light Blue")
    }

    @Test func indexWrapsModuloCountLikeFourBitMask() {
        // For a 16-entry palette this matches the historical `index & 0x0F`.
        #expect(Palette.nova.color(at: 16) == Palette.nova.color(at: 0))
        #expect(Palette.nova.color(at: 17) == Palette.nova.color(at: 1))
        #expect(Palette.nova.color(at: 255) == Palette.nova.color(at: 15))
        // Negative indices wrap into range rather than trapping.
        #expect(Palette.nova.color(at: -1) == Palette.nova.color(at: 15))
    }

    @Test func nearestIndexMatchesExactColors() {
        for (i, color) in Palette.nova.colors.enumerated() {
            let index = Palette.nova.nearestIndex(
                r: Double(color.r) / 255,
                g: Double(color.g) / 255,
                b: Double(color.b) / 255
            )
            #expect(index == UInt8(i))
        }
    }

    @Test func nearestIndexHandlesArbitraryInjectedPalette() {
        let palette = Palette(
            name: "RGB3",
            colors: [PaletteColor(255, 0, 0), PaletteColor(0, 255, 0), PaletteColor(0, 0, 255)],
            entryNames: ["Red", "Green", "Blue"]
        )
        #expect(palette.nearestIndex(r: 0.9, g: 0.1, b: 0.05) == 0)
        #expect(palette.nearestIndex(r: 0.1, g: 0.85, b: 0.1) == 1)
        #expect(palette.nearestIndex(r: 0.05, g: 0.1, b: 0.95) == 2)
        #expect(palette.name(at: 1) == "Green")
    }

    @Test func emptyPaletteDegradesGracefully() {
        let palette = Palette(name: "Empty", colors: [])
        #expect(palette.count == 0)
        #expect(palette.color(at: 3) == PaletteColor(0, 0, 0))
        #expect(palette.nearestIndex(r: 0.5, g: 0.5, b: 0.5) == 0)
        #expect(palette.name(at: 0) == "—")
    }
}
