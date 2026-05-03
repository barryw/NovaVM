// Debug SDRAM port-B clock-domain bridge.
//
// debug_bridge runs on clk_pixel, while sdram.v runs on clk_sdram. The source
// side presents the same level-held request shape debug_bridge already uses:
// addr/din plus either req_we or req_oe held until done_toggle changes. This
// module converts that into a request/acknowledge toggle crossing and drives
// sdram.v port B only from the SDRAM clock domain.

module debug_sdram_port_b_cdc (
    input  logic        clk,
    input  logic        sdram_clk,
    input  logic        rst,

    // debug_bridge-side request port (clk domain)
    input  logic        req_we,
    input  logic        req_oe,
    input  logic [24:0] req_addr,
    input  logic [7:0]  req_din,
    output logic [7:0]  req_dout,
    output logic        done_toggle,

    // sdram.v port-B side (sdram_clk domain)
    output logic        sdram_we,
    output logic        sdram_oe,
    output logic [24:0] sdram_addr,
    output logic [7:0]  sdram_din,
    input  logic [7:0]  sdram_dout,
    input  logic        sdram_done,
    output logic        sdram_busy
);

    // When debug traffic takes over port B, a SID curve read may already be
    // in flight inside sdram.v. Ignore early done pulses so a stale completion
    // cannot acknowledge the new debug request. Holding the request asserted
    // is safe: sdram.v may repeat the same byte operation, which is harmless
    // for same-address writes and reads.
    localparam int MIN_ACTIVE_CYCLES = 32;

    typedef enum logic [1:0] {
        SD_IDLE,
        SD_ACTIVE,
        SD_CAPTURE_READ
    } sd_state_t;

    // Source-domain request registers. These remain stable until the SDRAM
    // domain acknowledges the request, so the SDRAM side can sample them after
    // synchronizing req_toggle_src.
    logic        src_busy;
    logic        src_req_active_prev;
    logic        req_toggle_src;
    logic        ack_seen_src;
    logic [24:0] req_addr_src;
    logic [7:0]  req_din_src;
    logic        req_is_write_src;
    logic [7:0]  req_dout_src;

    (* async_reg = "true" *) logic [2:0] ack_sync_src;

    // SDRAM-domain request/ack state.
    (* async_reg = "true" *) logic [2:0] req_sync_sd;
    (* async_reg = "true" *) logic [1:0] rst_sync_sd = 2'b11;
    logic        req_seen_sd;
    logic        ack_toggle_sd;
    logic [7:0]  result_sd;

    sd_state_t   sd_state;
    logic [24:0] sd_addr_r;
    logic [7:0]  sd_din_r;
    logic        sd_is_write_r;
    logic        sd_we_r;
    logic        sd_oe_r;
    logic [5:0]  sd_active_guard;

    wire req_active = req_we | req_oe;

    assign req_dout   = req_dout_src;
    assign sdram_addr = sd_addr_r;
    assign sdram_din  = sd_din_r;
    assign sdram_we   = sd_we_r;
    assign sdram_oe   = sd_oe_r;
    assign sdram_busy = (sd_state != SD_IDLE);

    always_ff @(posedge clk) begin
        if (rst) begin
            src_busy            <= 1'b0;
            src_req_active_prev <= 1'b0;
            req_toggle_src      <= 1'b0;
            ack_seen_src        <= 1'b0;
            req_addr_src        <= 25'd0;
            req_din_src         <= 8'd0;
            req_is_write_src    <= 1'b0;
            req_dout_src        <= 8'd0;
            done_toggle         <= 1'b0;
            ack_sync_src        <= 3'b000;
        end else begin
            ack_sync_src <= {ack_sync_src[1:0], ack_toggle_sd};

            if (!src_busy && req_active && !src_req_active_prev) begin
                req_addr_src     <= req_addr;
                req_din_src      <= req_din;
                req_is_write_src <= req_we;
                req_toggle_src   <= ~req_toggle_src;
                src_busy         <= 1'b1;
            end

            if (src_busy && (ack_sync_src[2] != ack_seen_src)) begin
                ack_seen_src <= ack_sync_src[2];
                req_dout_src <= result_sd;
                done_toggle  <= ~done_toggle;
                src_busy     <= 1'b0;
            end

            src_req_active_prev <= req_active;
        end
    end

    always_ff @(posedge sdram_clk) begin
        rst_sync_sd <= {rst_sync_sd[0], rst};
    end

    always_ff @(posedge sdram_clk) begin
        if (rst_sync_sd[1]) begin
            req_sync_sd   <= 3'b000;
            req_seen_sd   <= 1'b0;
            ack_toggle_sd <= 1'b0;
            result_sd     <= 8'd0;
            sd_state      <= SD_IDLE;
            sd_addr_r     <= 25'd0;
            sd_din_r      <= 8'd0;
            sd_is_write_r <= 1'b0;
            sd_we_r       <= 1'b0;
            sd_oe_r       <= 1'b0;
            sd_active_guard <= 6'd0;
        end else begin
            req_sync_sd <= {req_sync_sd[1:0], req_toggle_src};

            case (sd_state)
                SD_IDLE: begin
                    sd_we_r <= 1'b0;
                    sd_oe_r <= 1'b0;

                    if (req_sync_sd[2] != req_seen_sd) begin
                        req_seen_sd   <= req_sync_sd[2];
                        sd_addr_r     <= req_addr_src;
                        sd_din_r      <= req_din_src;
                        sd_is_write_r <= req_is_write_src;
                        sd_we_r       <= req_is_write_src;
                        sd_oe_r       <= !req_is_write_src;
                        sd_active_guard <= MIN_ACTIVE_CYCLES[5:0];
                        sd_state      <= SD_ACTIVE;
                    end
                end

                SD_ACTIVE: begin
                    if (sd_active_guard != 0) begin
                        sd_active_guard <= sd_active_guard - 6'd1;
                    end else if (sdram_done) begin
                        sd_we_r <= 1'b0;
                        sd_oe_r <= 1'b0;

                        if (sd_is_write_r) begin
                            ack_toggle_sd <= ~ack_toggle_sd;
                            sd_state      <= SD_IDLE;
                        end else begin
                            sd_state <= SD_CAPTURE_READ;
                        end
                    end
                end

                SD_CAPTURE_READ: begin
                    result_sd     <= sdram_dout;
                    ack_toggle_sd <= ~ack_toggle_sd;
                    sd_state      <= SD_IDLE;
                end

                default: sd_state <= SD_IDLE;
            endcase
        end
    end

endmodule
