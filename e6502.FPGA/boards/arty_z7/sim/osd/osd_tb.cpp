// osd_tb.cpp — Verilator bench for osd_overlay.sv.
//
// Drives a fake 720x480 video stream (a gradient "program") and writes a sample
// disk-mount menu into the OSD framebuffer, then renders the composited RGB out
// to a PPM so we can SEE the overlay before committing a bitstream. This is the
// Stage-1 gate for the OSD feature.
#include "Vosd_overlay.h"
#include "verilated.h"
#include <cstdio>
#include <cstdint>
#include <vector>

static Vosd_overlay* dut;
static inline void tick() { dut->clk = 0; dut->eval(); dut->clk = 1; dut->eval(); }

static const int HACT = 720, VACT = 480, HTOT = 858, VTOT = 525;
static const int COLS = 90, ROWS = 30;

// cell write: {transp[29], bg[28:25], fg[24:21], char[20:13], addr[12:0]}
static void wcell(int addr, int ch, int fg, int bg, int transp) {
    dut->fb_we = 1;
    dut->fb_wdata = ((uint32_t)(transp & 1) << 29) | ((uint32_t)(bg & 0xF) << 25) |
                    ((uint32_t)(fg & 0xF) << 21) | ((uint32_t)(ch & 0xFF) << 13) |
                    ((uint32_t)(addr & 0x1FFF));
    tick();
    dut->fb_we = 0; dut->fb_wdata = 0;
}
static void wtext(int col, int row, const char* s, int fg, int bg) {
    for (int i = 0; s[i]; i++) wcell(row * COLS + col + i, (uint8_t)s[i], fg, bg, 0);
}
static void fillrect(int c0, int r0, int w, int h, int ch, int fg, int bg, int transp) {
    for (int r = 0; r < h; r++) for (int c = 0; c < w; c++)
        wcell((r0 + r) * COLS + c0 + c, ch, fg, bg, transp);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    dut = new Vosd_overlay;
    dut->rst = 1; dut->osd_enable = 0; dut->vid_de = 0;
    dut->vid_r = 0; dut->vid_g = 0; dut->vid_b = 0; dut->fb_we = 0; dut->fb_wdata = 0;
    for (int i = 0; i < 4; i++) tick();
    dut->rst = 0;

    // ---- sample disk-mount menu in the framebuffer ----
    for (int i = 0; i < COLS * ROWS; i++) wcell(i, ' ', 1, 0, 1);   // clear -> transparent
    int pw = 36, ph = 11, pc = (COLS - pw) / 2, pr = (ROWS - ph) / 2;
    fillrect(pc, pr, pw, ph, ' ', 1, 6, 0);                        // opaque blue panel
    wtext(pc + 2, pr + 1, "N O V A   D I S K S", 7, 6);            // yellow title
    wtext(pc + 2, pr + 3, "fd0    <empty>", 1, 6);
    wtext(pc + 2, pr + 4, "fd1    GAME.NDI", 1, 6);
    wtext(pc + 2, pr + 5, "hd0    <empty>", 1, 6);
    wtext(pc + 2, pr + 6, "hd1    SYSTEM.NDI", 1, 6);
    wtext(pc + 2, pr + 8, "Cancel", 1, 6);
    fillrect(pc + 1, pr + 4, pw - 2, 1, ' ', 6, 7, 0);             // highlight row fd1
    wtext(pc + 2, pr + 4, "fd1    GAME.NDI", 6, 7);
    dut->osd_enable = 1;

    // ---- drive 2 frames (warm pipeline, capture 2nd) ----
    const int VBLANK = HTOT * 8;          // long de-low gap forces frame restart
    const int LAT = 4;                    // osd_overlay pipeline latency
    std::vector<uint8_t> img(HACT * VACT * 3, 0);

    auto run_frame = [&](bool save) {
        std::vector<uint32_t> seq; seq.reserve((long)VTOT * HTOT + 32);
        for (int i = 0; i < VBLANK; i++) { dut->vid_de = 0; tick(); seq.push_back(dut->rgb_out); }
        for (int y = 0; y < VACT; y++) {
            for (int x = 0; x < HACT; x++) {
                dut->vid_de = 1; dut->vid_r = (x >> 5) & 0xF; dut->vid_g = (y >> 5) & 0xF; dut->vid_b = 8;
                tick(); seq.push_back(dut->rgb_out);
            }
            for (int x = 0; x < HTOT - HACT; x++) { dut->vid_de = 0; tick(); seq.push_back(dut->rgb_out); }
        }
        for (int i = 0; i < 16; i++) { dut->vid_de = 0; tick(); seq.push_back(dut->rgb_out); }
        if (!save) return;
        for (int y = 0; y < VACT; y++) for (int x = 0; x < HACT; x++) {
            long idx = (long)VBLANK + (long)y * HTOT + x + LAT;
            uint32_t px = (idx < (long)seq.size()) ? seq[idx] : 0;
            int o = (y * HACT + x) * 3;
            img[o] = (px >> 16) & 0xFF; img[o + 1] = (px >> 8) & 0xFF; img[o + 2] = px & 0xFF;
        }
    };
    run_frame(false);
    run_frame(true);

    FILE* f = fopen("osd.ppm", "wb");
    fprintf(f, "P6\n%d %d\n255\n", HACT, VACT);
    fwrite(img.data(), 1, img.size(), f); fclose(f);
    printf("wrote osd.ppm (%dx%d)\n", HACT, VACT);
    delete dut;
    return 0;
}
