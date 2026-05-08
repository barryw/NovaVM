import Testing
import Foundation
@testable import NovaDraw

@Suite struct Ca65ExporterTests {
    @Test func blitterRectanglesPreservePaintedBlackAndKeyUnpaintedPixels() throws {
        let image = NovaCanvasImage(
            name: "Piece",
            pixels: [0, 1, 2, 3],
            paintedPixels: [1, 1, 0, 1]
        )

        let source = try Ca65Exporter.encode(
            images: [image],
            width: 2,
            height: 2,
            options: Ca65ExportOptions(
                kind: .blitterRectangles,
                scope: .allImages,
                symbolPrefix: "pieces",
                segmentName: "RODATA",
                transparentColor: 2
            )
        )

        #expect(source.contains("pieces_width = 2"))
        #expect(source.contains("pieces_height = 2"))
        #expect(source.contains("pieces_transparent = $02"))
        #expect(source.contains("pieces_piece:"))
        #expect(!source.contains("pieces_ptr:"))
        #expect(source.contains("        .byte $00, $01"))
        #expect(source.contains("        .byte $02, $03"))
    }

    @Test func blitterRectanglesUseImageNamesForStableLabels() throws {
        let names = ["pawn", "knight", "bishop", "rook", "queen", "king"]
        let images = names.enumerated().map { index, name in
            NovaCanvasImage(
                name: name,
                pixels: [UInt8](repeating: UInt8(index), count: 20 * 20),
                paintedPixels: [UInt8](repeating: 255, count: 20 * 20)
            )
        }

        let source = try Ca65Exporter.encode(
            images: images,
            width: 20,
            height: 20,
            options: Ca65ExportOptions(
                kind: .blitterRectangles,
                symbolPrefix: "chess_piece",
                segmentName: "RODATA",
                transparentColor: 2
            )
        )

        #expect(source.contains(".segment \"RODATA\""))
        #expect(source.contains("chess_piece_width = 20"))
        #expect(source.contains("chess_piece_height = 20"))
        #expect(source.contains("chess_piece_stride = 20"))
        #expect(source.contains("chess_piece_image_size = 400"))
        #expect(source.contains("chess_piece_transparent = $02"))
        #expect(!source.contains("chess_piece_count"))
        #expect(!source.contains("chess_piece_ptr:"))
        #expect(source.contains("chess_piece_lo:\n        .byte <chess_piece_pawn, <chess_piece_knight, <chess_piece_bishop\n        .byte <chess_piece_rook, <chess_piece_queen, <chess_piece_king"))
        #expect(source.contains("chess_piece_hi:\n        .byte >chess_piece_pawn, >chess_piece_knight, >chess_piece_bishop\n        .byte >chess_piece_rook, >chess_piece_queen, >chess_piece_king"))
        for name in names {
            #expect(source.contains("chess_piece_\(name):"))
        }
    }

    @Test func vSpriteRecordsIncludeDimensionsBeforePixels() throws {
        let image = NovaCanvasImage(
            name: "Piece",
            pixels: [4, 5, 6, 7],
            paintedPixels: [1, 1, 1, 1]
        )

        let source = try Ca65Exporter.encode(
            images: [image],
            width: 2,
            height: 2,
            options: Ca65ExportOptions(kind: .vSpriteRecords, symbolPrefix: "pieces")
        )

        #expect(source.contains("pieces_record_size = 8"))
        #expect(source.contains("pieces_piece:\n        .word 2, 2\n        .byte $04, $05"))
    }

    @Test func bitMasksPackPaintedPixelsMostSignificantBitFirst() throws {
        let image = NovaCanvasImage(
            name: "Mask",
            pixels: [UInt8](repeating: 0, count: 10),
            paintedPixels: [1, 0, 0, 0, 0, 0, 0, 1, 1, 0]
        )

        let source = try Ca65Exporter.encode(
            images: [image],
            width: 10,
            height: 1,
            options: Ca65ExportOptions(kind: .bitMasks, symbolPrefix: "shape")
        )

        #expect(source.contains("shape_row_bytes = 2"))
        #expect(source.contains("shape_tail_bits = 2"))
        #expect(source.contains("        .byte %10000001, %10000000"))
    }

    @Test func hardwareSpritesPackNibblesAndUseTransparentColorForUnpaintedPixels() throws {
        let image = NovaCanvasImage(
            name: "Sprite",
            pixels: [0x0A, 0x0B],
            paintedPixels: [1, 0]
        )

        let source = try Ca65Exporter.encode(
            images: [image],
            width: 2,
            height: 1,
            options: Ca65ExportOptions(
                kind: .hardwareSprites,
                symbolPrefix: "sprite",
                transparentColor: 2
            )
        )

        #expect(source.contains("sprite_bytes_per_sprite = 128"))
        #expect(source.contains("        .byte $2A, $22, $22, $22, $22, $22, $22, $22"))
    }

    @Test func hardwareSpritesRejectOversizedCanvas() throws {
        let image = NovaCanvasImage(
            name: "Large",
            pixels: [UInt8](repeating: 0, count: 17),
            paintedPixels: [UInt8](repeating: 0, count: 17)
        )

        var thrownError: Ca65ExportError?
        do {
            _ = try Ca65Exporter.encode(
                images: [image],
                width: 17,
                height: 1,
                options: Ca65ExportOptions(kind: .hardwareSprites)
            )
        } catch let error as Ca65ExportError {
            thrownError = error
        }

        #expect(thrownError == .hardwareSpriteSize(width: 17, height: 1))
    }
}
