// osd_overlay.sv — HDMI-path On-Screen Display compositor.
//
// Sits between the VGC's RGB output and the HDMI encoder. When osd_enable is
// high it dims the VGC pixels (>>1) and paints a text grid on top: a 90x30 grid
// of 8x16 cells over the 720x480 active area. Each cell is {transparent, bg, fg,
// char}; transparent cells show the dimmed VGC, opaque cells fill bg and draw
// char in fg. The whole machine is paused (top.system_pause) while this is up,
// so the VGC frame underneath is frozen — only the scan-out keeps running.
//
// Independent of the VGC: its own framebuffer BRAM (written by the PS through
// fio_bridge OSD_FB) and its own fixed 16-colour palette, so a program repainting
// the VGC palette can't corrupt the menu. Glyphs reuse the VGC 8x8 font (bank 0),
// doubled vertically to 8x16.
//
// Pixel coordinates are recovered from vid_de alone (polarity-agnostic): px
// counts active pixels per line; a line ends on de-falling; a frame restarts
// after the long vertical-blank gap. The VGC RGB is delayed through the same
// pipeline as the BRAM lookups so overlay and program stay pixel-aligned (a
// constant ~4px shift, absorbed by the 40px border).

`default_nettype none

module osd_overlay #(
    parameter int COLS     = 90,
    parameter int ROWS     = 30,
    parameter int H_TOTAL  = 858,                 // for vblank-gap frame detect
    parameter     FONT_HEX = "rom/fonts.hex"      // VGC font; bank 0 = ASCII
)(
    input  wire        clk,
    input  wire        rst,

    // ---- VGC video in ----
    input  wire [3:0]  vid_r,
    input  wire [3:0]  vid_g,
    input  wire [3:0]  vid_b,
    input  wire        vid_de,

    // ---- control + framebuffer write (from fio_bridge) ----
    input  wire        osd_enable,
    input  wire        fb_we,                     // 1-cycle cell write
    input  wire [31:0] fb_wdata,                  // {transp,bg[3:0],fg[3:0],char[7:0],addr[12:0]}

    // ---- composited RGB out (to HDMI encoder; black during blanking) ----
    output reg  [23:0] rgb_out,
    output reg         active_out
);
    localparam int NCELL = COLS * ROWS;           // 2700

    // ---- framebuffer BRAM: cell = {transp, bg[3:0], fg[3:0], char[7:0]} ----
    (* ram_style = "block" *) logic [16:0] cell_mem [0:NCELL-1];
    logic [16:0] cell_dout;

    wire [12:0] fb_addr  = fb_wdata[12:0];
    wire [7:0]  fb_char  = fb_wdata[20:13];
    wire [3:0]  fb_fg    = fb_wdata[24:21];
    wire [3:0]  fb_bg    = fb_wdata[28:25];
    wire        fb_transp= fb_wdata[29];

    // ---- font ROM: VGC 8x8 font (bank 0), bit7 = leftmost ----
    (* rom_style = "block" *) logic [7:0] font_rom [0:8191];
    initial $readmemh(FONT_HEX, font_rom);
    logic [7:0] font_dout;

    // ---- raster recovery from vid_de --------------------------------------
    logic        de_d;
    logic [10:0] px;            // active pixel within line  (0..719)
    logic [9:0]  py;            // active line within frame  (0..479)
    logic [11:0] gap;           // consecutive de-low cycles (vblank detect)

    always_ff @(posedge clk) begin
        if (rst) begin
            de_d <= 1'b0; px <= 0; py <= 0; gap <= 0;
        end else begin
            de_d <= vid_de;
            if (!vid_de) begin
                px  <= 0;
                if (gap <= H_TOTAL) gap <= gap + 1'b1;   // saturate past one line
            end else begin
                px  <= px + 1'b1;
                gap <= 0;
                if (!de_d)                               // de rising: new active line
                    py <= (gap > H_TOTAL) ? 10'd0 : py + 1'b1;
            end
        end
    end

    // ---- pipeline ---------------------------------------------------------
    // R-stage values come straight from the raster counter + VGC RGB this clk.
    wire [6:0]  col   = px[9:3];        // 0..89
    wire [4:0]  row   = py[8:4];        // 0..29
    wire [2:0]  fcol  = px[2:0];        // glyph column
    wire [2:0]  frow  = py[3:1];        // glyph row (8x8 doubled -> 8x16)
    wire [12:0] caddr = row*COLS + col;

    // p0: latch addr + carry signals
    logic [12:0] caddr_0; logic [2:0] fcol_0; logic [2:0] frow_0;
    logic [11:0] rgb_0;   logic de_0;
    // p1: present cell addr; carry
    logic [2:0] fcol_1; logic [2:0] frow_1; logic [11:0] rgb_1; logic de_1;
    // p2: cell valid; present font addr; carry
    logic [16:0] cell_2; logic [2:0] fcol_2; logic [11:0] rgb_2; logic de_2;

    wire [11:0] rgb_in = {vid_r, vid_g, vid_b};

    always_ff @(posedge clk) begin
        // p0
        caddr_0 <= caddr; fcol_0 <= fcol; frow_0 <= frow;
        rgb_0   <= rgb_in; de_0 <= vid_de;
        // p1 — cell read launched from caddr_0
        cell_dout <= cell_mem[caddr_0];
        fcol_1 <= fcol_0; frow_1 <= frow_0; rgb_1 <= rgb_0; de_1 <= de_0;
        // p2 — cell_dout valid; font read launched from {char, frow_1}
        cell_2  <= cell_dout;
        font_dout <= font_rom[{cell_dout[7:0], frow_1}];
        fcol_2 <= fcol_1; rgb_2 <= rgb_1; de_2 <= de_1;
    end

    // framebuffer write (own port)
    always_ff @(posedge clk)
        if (fb_we) cell_mem[fb_addr] <= {fb_transp, fb_bg, fb_fg, fb_char};

    // ---- composite (p3) ---------------------------------------------------
    wire        c_transp = cell_2[16];
    wire [3:0]  c_bg     = cell_2[15:12];
    wire [3:0]  c_fg     = cell_2[11:8];
    wire        font_bit = font_dout[7 - fcol_2];     // bit7 = leftmost

    function automatic [11:0] pal(input [3:0] i);
        case (i)                                       // RGB444, C64-ish
            4'h0: pal = 12'h000; 4'h1: pal = 12'hFFF; 4'h2: pal = 12'hA00;
            4'h3: pal = 12'h0CC; 4'h4: pal = 12'hC0C; 4'h5: pal = 12'h0C0;
            4'h6: pal = 12'h00A; 4'h7: pal = 12'hFF0; 4'h8: pal = 12'hF80;
            4'h9: pal = 12'h640; 4'hA: pal = 12'hF88; 4'hB: pal = 12'h444;
            4'hC: pal = 12'h888; 4'hD: pal = 12'h8F8; 4'hE: pal = 12'h88F;
            4'hF: pal = 12'hBBB;
        endcase
    endfunction

    // dimmed program (each 4-bit channel >>1)
    wire [11:0] dim_rgb = {1'b0, rgb_2[11:9], 1'b0, rgb_2[7:5], 1'b0, rgb_2[3:1]};

    logic [11:0] sel444;
    always_comb begin
        if (osd_enable && !c_transp)
            sel444 = font_bit ? pal(c_fg) : pal(c_bg);
        else if (osd_enable)
            sel444 = dim_rgb;          // transparent cell over dimmed program
        else
            sel444 = rgb_2;            // OSD off: passthrough
    end

    // expand RGB444 -> RGB888 ({r,r,g,g,b,b}); black during blanking
    always_ff @(posedge clk) begin
        rgb_out <= de_2 ? {sel444[11:8], sel444[11:8],
                           sel444[7:4],  sel444[7:4],
                           sel444[3:0],  sel444[3:0]} : 24'h000000;
        active_out <= de_2;
    end

endmodule

`default_nettype wire
