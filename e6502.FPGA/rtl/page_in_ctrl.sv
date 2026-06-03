// page_in_ctrl.sv — Tier-2 library page-in MMIO front-end + CDC (Task 11d).
//
// The proven page-in copy engine `page_dma` runs ENTIRELY in the sdram_clk
// domain (Task 11c). This module is the thin PIXEL-domain layer that lets the
// CPU trigger a page-in via MMIO and observe completion, plus the clock-domain
// crossing that links the two:
//
//   PIXEL DOMAIN (this module's register block + status)
//     $BA76 PGD_CMD     write 0x01 = start a page-in
//     $BA77 PGD_STATUS  read: bit0 = busy, bit1 = done/ok, others 0
//     $BA78 PGD_SRCL    24-bit XRAM/SDRAM source BYTE base, low
//     $BA79 PGD_SRCM                                        mid
//     $BA7A PGD_SRCH                                        high
//     $BA7B PGD_WORDSL  # of 16-bit words to copy (<=8192 for a 16 KB module)
//     $BA7C PGD_WORDSH
//
//   CDC: pixel -> sdram   start pulse (toggle + 3-FF sync + edge detect) plus
//                         the src_base/words payload (held stable while busy,
//                         so the sdram side samples a quiescent value).
//        sdram -> pixel   page_dma `done` (toggle + 3-FF sync + edge detect)
//                         and `pgd_active` (level, 2-FF sync) for STATUS/rdy.
//
//   SDRAM DOMAIN (instantiated here): the unchanged `page_dma` core, whose
//   stream port is brought straight back out as this module's ports so top.sv
//   can route it to the real sdram.v stream port.
//
// CPU STALL: while a page-in is in flight `busy` is high; `rdy_out` is driven
// low so the CPU is frozen for the whole copy. The page-in is therefore atomic
// from the CPU's view (no bus race against the sdram_clk byte-writer), and the
// CPU resumes only after `done` crosses back and clears `busy`. The next
// STATUS read then shows done/ok.
//
// All pixel-domain regs reset synchronously to rst (pixel clk). The sdram-side
// page_dma core resets to its own rst (sdram clk). Both reset inputs are tied
// by top.sv to the appropriately-synchronized custom reset.

module page_in_ctrl (
    // ---- pixel clock domain ----
    input  wire        clk,            // pixel clk
    input  wire        rst,            // synchronous reset (pixel domain)

    // CPU bus (snooped for register writes, drives rdata on reads) — same
    // shape as dma.sv. Selection ($BA76-$BA7C) is decoded internally.
    input  wire [15:0] cpu_addr,
    input  wire [7:0]  cpu_wdata,
    input  wire        cpu_we,
    output reg  [7:0]  cpu_rdata,

    // CPU stall (active low — feeds the CPU RDY chain). Low while busy.
    output wire        rdy_out,

    // ---- sdram clock domain ----
    input  wire        sdram_clk,
    input  wire        sdram_rst,      // synchronous reset (sdram domain)

    // ext_rom port-A write image (drives top.sv's ext_rom write mux).
    output wire        pgd_active,
    output wire        pgd_erom_we,
    output wire [13:0] pgd_erom_addr,
    output wire [7:0]  pgd_erom_data,

    // sdram.v page-mode stream port — routed by top.sv to the real sdram.v.
    output wire        stream_req,
    output wire [24:0] stream_addr,
    output wire [13:0] stream_words,
    output wire        stream_ready,
    input  wire [15:0] stream_dout,
    input  wire        stream_valid,
    input  wire        stream_busy,
    input  wire        stream_done
);

    // =========================================================================
    // Register decode ($BA76-$BA7C), pixel domain
    // =========================================================================
    localparam [15:0] PGD_BASE = 16'hBA76;
    localparam [15:0] PGD_END  = 16'hBA7C;

    localparam [2:0] R_CMD    = 3'd0;   // $BA76
    localparam [2:0] R_STATUS = 3'd1;   // $BA77
    localparam [2:0] R_SRCL   = 3'd2;   // $BA78
    localparam [2:0] R_SRCM   = 3'd3;   // $BA79
    localparam [2:0] R_SRCH   = 3'd4;   // $BA7A
    localparam [2:0] R_WORDSL = 3'd5;   // $BA7B
    localparam [2:0] R_WORDSH = 3'd6;   // $BA7C

    wire        sel    = (cpu_addr >= PGD_BASE) && (cpu_addr <= PGD_END);
    wire [2:0]  reg_off = cpu_addr[2:0] - PGD_BASE[2:0];

    // CPU-writable shadow registers (SRC + WORDS). CMD is a strobe, STATUS is
    // read-only, so neither has a stored byte here.
    reg [7:0] src_l, src_m, src_h;
    reg [7:0] words_l, words_h;

    // Pixel-domain busy/ok state.
    reg       busy;   // page-in in flight (set on CMD start, cleared on done)
    reg       ok;     // last page-in completed (sticky until next start)

    // Latched copy args presented to the sdram side (stable while busy).
    reg [24:0] src_base_q;
    reg [13:0] words_q;

    // Start handshake: pixel toggles start_tog on a CMD=0x01 write while idle.
    reg        start_tog;

    // sdram-domain handshake nets (declared here so the pixel synchronizer
    // below can tap done_tog; the always blocks driving them are further down).
    reg        done_tog;                       // sdram domain, toggles each done
    wire       done_tog_px_raw = done_tog;     // tapped by the pixel synchronizer
    wire       pgd_done;                        // page_dma 1-clk completion pulse

    // =========================================================================
    // pixel <- sdram synchronizers
    // =========================================================================
    // done toggle from page_dma (sdram domain), 2-FF synced + edge detected.
    (* async_reg = "true" *) reg [2:0] done_tog_sync;
    wire done_pulse_px = done_tog_sync[2] ^ done_tog_sync[1];

    // pgd_active level synced for status/rdy debug visibility (not load-bearing
    // for completion — busy is cleared by done_pulse_px).
    (* async_reg = "true" *) reg [1:0] active_sync;

    // =========================================================================
    // sdram <- pixel synchronizers
    // =========================================================================
    // start toggle from pixel, 2-FF synced + edge detected in sdram domain.
    (* async_reg = "true" *) reg [2:0] start_tog_sync;
    wire start_pulse_sd = start_tog_sync[2] ^ start_tog_sync[1];

    // =========================================================================
    // CPU read
    // =========================================================================
    always @(*) begin
        cpu_rdata = 8'h00;
        if (sel) begin
            case (reg_off)
                R_STATUS: cpu_rdata = {6'b0, ok, busy};
                R_SRCL:   cpu_rdata = src_l;
                R_SRCM:   cpu_rdata = src_m;
                R_SRCH:   cpu_rdata = src_h;
                R_WORDSL: cpu_rdata = words_l;
                R_WORDSH: cpu_rdata = words_h;
                default:  cpu_rdata = 8'h00;   // CMD reads as 0
            endcase
        end
    end

    // =========================================================================
    // Pixel-domain register block + handshake
    // =========================================================================
    always @(posedge clk) begin
        if (rst) begin
            src_l       <= 8'h00;
            src_m       <= 8'h00;
            src_h       <= 8'h00;
            words_l     <= 8'h00;
            words_h     <= 8'h20;   // default 8192 words (0x2000) = 16 KB
            busy        <= 1'b0;
            ok          <= 1'b0;
            src_base_q  <= 25'd0;
            words_q     <= 14'd0;
            start_tog   <= 1'b0;
            done_tog_sync <= 3'b000;
            active_sync   <= 2'b00;
        end else begin
            // synchronize sdram-domain signals into pixel
            done_tog_sync <= {done_tog_sync[1:0], done_tog_px_raw};
            active_sync   <= {active_sync[0], pgd_active};

            // CPU register writes (SRC/WORDS storage + CMD strobe)
            if (cpu_we && sel) begin
                case (reg_off)
                    R_SRCL:   src_l   <= cpu_wdata;
                    R_SRCM:   src_m   <= cpu_wdata;
                    R_SRCH:   src_h   <= cpu_wdata;
                    R_WORDSL: words_l <= cpu_wdata;
                    R_WORDSH: words_h <= cpu_wdata;
                    R_CMD: begin
                        if (cpu_wdata == 8'h01 && !busy) begin
                            // Latch args, raise busy, toggle start across CDC.
                            src_base_q <= {1'b0, src_h, src_m, src_l};
                            words_q    <= {words_h[5:0], words_l};
                            busy       <= 1'b1;
                            ok         <= 1'b0;
                            start_tog  <= ~start_tog;
                        end
                    end
                    default: ; // STATUS read-only
                endcase
            end

            // Completion: done crossing from the sdram domain clears busy.
            if (done_pulse_px) begin
                busy <= 1'b0;
                ok   <= 1'b1;
            end
        end
    end

    // rdy_out: stall the CPU while a page-in is in flight.
    assign rdy_out = ~busy;

    // =========================================================================
    // sdram-domain handshake + page_dma core
    // =========================================================================
    // start toggle synchronizer (pixel -> sdram) and the done toggle generator
    // (sdram -> pixel, tapped above via done_tog_px_raw).
    always @(posedge sdram_clk) begin
        if (sdram_rst) begin
            start_tog_sync <= 3'b000;
            done_tog       <= 1'b0;
        end else begin
            start_tog_sync <= {start_tog_sync[1:0], start_tog};
            if (pgd_done) done_tog <= ~done_tog;
        end
    end

    page_dma pgd_core (
        .clk          (sdram_clk),
        .rst          (sdram_rst),
        .start        (start_pulse_sd),
        // src_base/words are latched in the pixel domain and held stable for
        // the entire copy (busy is high until done crosses back), so sampling
        // them combinationally in the sdram domain at start is safe — the
        // start pulse itself is the synchronized event and the data is quiescent.
        .src_base     (src_base_q),
        .words        (words_q),
        .stream_req   (stream_req),
        .stream_addr  (stream_addr),
        .stream_words (stream_words),
        .stream_ready (stream_ready),
        .stream_dout  (stream_dout),
        .stream_valid (stream_valid),
        .stream_busy  (stream_busy),
        .stream_done  (stream_done),
        .pgd_erom_we  (pgd_erom_we),
        .pgd_erom_addr(pgd_erom_addr),
        .pgd_erom_data(pgd_erom_data),
        .pgd_active   (pgd_active),
        .done         (pgd_done)
    );

endmodule
