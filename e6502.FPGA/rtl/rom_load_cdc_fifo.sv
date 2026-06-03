// Boot-bridge ROM-load CDC FIFO: pixel clk -> sdram_clk.
//
// The boot-bridge ROM-load writes (dbg_rom_addr/dbg_rom_data/dbg_rom_we for
// ext_rom, idx=1) arrive in the pixel/debug_bridge clock domain. ext_rom port
// A now lives in the sdram_clk domain (dpram_dc, Task 11b), so the writes must
// cross domains. NovaHost / the tests stream the 16 KB image as BACK-TO-BACK
// writes (wr_valid high every other cycle, payload advancing each write).
//
// A level-synchronizer + edge-detect that samples the payload LIVE when the
// synchronized we-pulse fires is WRONG for back-to-back traffic: the pulse
// lands 2+ sdram_clk cycles after the real edge, by which point addr/data have
// already advanced, so the captured payload is skewed and byte 0 is lost. The
// robust fix is a small dual-clock FIFO that captures {addr,data} COHERENTLY
// with each write on the source clock and hands it to the sink intact.
//
// This is a width-parameterized adaptation of debug_async_byte_fifo.sv (the
// tested Gray-code async FIFO used by debug_spi_slave). Same pointer scheme,
// same 2-FF Gray-code pointer synchronizers, same registered-read prefetch.
// Carrying {addr,data} as one word keeps them inseparable across the CDC.
//
// The sink (sdram_clk, ~100 MHz) drains far faster than the source (pixel clk,
// ~25 MHz) fills, so a SHALLOW FIFO (default 8 deep) never fills for the boot
// stream even with back-to-back source writes. wr_ready de-asserts on full as
// back-pressure, but the boot bridge is throttled by the host link and the
// FIFO drains in a couple sink cycles, so full is not reached in practice.

module rom_load_cdc_fifo #(
    parameter int WIDTH      = 22,  // {addr[13:0], data[7:0]}
    parameter int ADDR_WIDTH = 3    // depth = 8 entries
) (
    input  logic             wr_clk,
    input  logic             wr_rst,
    input  logic [WIDTH-1:0] wr_data,
    input  logic             wr_valid,
    output logic             wr_ready,

    input  logic             rd_clk,
    input  logic             rd_rst,
    output logic [WIDTH-1:0] rd_data,
    output logic             rd_valid,
    input  logic             rd_ready,

    output logic             overflow,
    output logic             underflow
);

    localparam int DEPTH = 1 << ADDR_WIDTH;

    (* ram_style = "block", ramstyle = "block" *)
    logic [WIDTH-1:0] mem [0:DEPTH-1];

    logic [ADDR_WIDTH:0] wr_bin;
    logic [ADDR_WIDTH:0] wr_gray;
    logic [ADDR_WIDTH:0] rd_bin;
    logic [ADDR_WIDTH:0] rd_gray;

    (* async_reg = "true" *) logic [ADDR_WIDTH:0] rd_gray_wr_1;
    (* async_reg = "true" *) logic [ADDR_WIDTH:0] rd_gray_wr_2;
    (* async_reg = "true" *) logic [ADDR_WIDTH:0] wr_gray_rd_1;
    (* async_reg = "true" *) logic [ADDR_WIDTH:0] wr_gray_rd_2;

    function automatic logic [ADDR_WIDTH:0] bin_to_gray;
        input logic [ADDR_WIDTH:0] value;
        begin
            bin_to_gray = (value >> 1) ^ value;
        end
    endfunction

    wire [ADDR_WIDTH:0] wr_bin_next  = wr_bin + 1'b1;
    wire [ADDR_WIDTH:0] wr_gray_next = bin_to_gray(wr_bin_next);
    wire full = (wr_gray_next == {
        ~rd_gray_wr_2[ADDR_WIDTH:ADDR_WIDTH-1],
         rd_gray_wr_2[ADDR_WIDTH-2:0]
    });

    wire fifo_empty = (rd_gray == wr_gray_rd_2);
    wire push = wr_valid && !full;

    assign wr_ready = !full;

    logic [WIDTH-1:0] rd_data_reg;
    logic             rd_valid_reg;

    wire output_pop = rd_valid_reg && rd_ready;
    wire prefetch   = (!rd_valid_reg || output_pop) && !fifo_empty;

    assign rd_valid = rd_valid_reg;
    assign rd_data  = rd_data_reg;

    // POR init for every reg so post-reset state is defined (ECP5 reset
    // discipline — see reference_ecp5_reset_canonical).
    initial begin
        wr_bin       = '0;
        wr_gray      = '0;
        rd_bin       = '0;
        rd_gray      = '0;
        rd_gray_wr_1 = '0;
        rd_gray_wr_2 = '0;
        wr_gray_rd_1 = '0;
        wr_gray_rd_2 = '0;
        rd_data_reg  = '0;
        rd_valid_reg = 1'b0;
        overflow     = 1'b0;
        underflow    = 1'b0;
    end

    always_ff @(posedge wr_clk) begin
        if (push)
            mem[wr_bin[ADDR_WIDTH-1:0]] <= wr_data;
    end

    always_ff @(posedge wr_clk or posedge wr_rst) begin
        if (wr_rst) begin
            wr_bin       <= '0;
            wr_gray      <= '0;
            rd_gray_wr_1 <= '0;
            rd_gray_wr_2 <= '0;
            overflow     <= 1'b0;
        end else begin
            rd_gray_wr_1 <= rd_gray;
            rd_gray_wr_2 <= rd_gray_wr_1;

            if (wr_valid && full)
                overflow <= 1'b1;

            if (push) begin
                wr_bin  <= wr_bin_next;
                wr_gray <= wr_gray_next;
            end
        end
    end

    always_ff @(posedge rd_clk) begin
        if (prefetch)
            rd_data_reg <= mem[rd_bin[ADDR_WIDTH-1:0]];
    end

    always_ff @(posedge rd_clk or posedge rd_rst) begin
        if (rd_rst) begin
            rd_bin       <= '0;
            rd_gray      <= '0;
            wr_gray_rd_1 <= '0;
            wr_gray_rd_2 <= '0;
            rd_valid_reg <= 1'b0;
            underflow    <= 1'b0;
        end else begin
            wr_gray_rd_1 <= wr_gray;
            wr_gray_rd_2 <= wr_gray_rd_1;

            if (rd_ready && !rd_valid_reg && fifo_empty)
                underflow <= 1'b1;

            if (prefetch) begin
                rd_bin       <= rd_bin + 1'b1;
                rd_gray      <= bin_to_gray(rd_bin + 1'b1);
                rd_valid_reg <= 1'b1;
            end else if (output_pop) begin
                rd_valid_reg <= 1'b0;
            end
        end
    end

endmodule
