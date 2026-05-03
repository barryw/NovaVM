// XRAM-over-SDRAM wrapper — replaces the old XRAM BRAM with SDRAM port A.
//
// The CPU/XMC side runs on clk_pixel. The SDRAM controller runs on clk_sdram
// and arbitrates port A by clkref phase internally. This bridge therefore uses
// a request/acknowledge toggle crossing instead of fixed pixel-clock delays.
// A CPU read is complete only after sdram.v reports port-A done and the result
// byte has crossed back into the CPU clock domain.

module xram_sdram (
    input  logic        clk,         // CPU/pixel clock
    input  logic        sdram_clk,
    input  logic        rst,

    // CPU/XMC-facing request port
    input  logic [18:0] req_addr,    // 19-bit byte address (512 KB)
    input  logic [7:0]  req_din,
    input  logic        req_we,
    input  logic        req_re,
    output logic [7:0]  req_dout,
    output logic        busy,

    // SDRAM port A connection. These signals are driven in the sdram_clk
    // domain; the CPU side waits for sdram_done to cross back before it
    // releases busy.
    output logic [24:0] sdram_addr,
    output logic [7:0]  sdram_din,
    output logic        sdram_we,
    output logic        sdram_oe,
    input  logic [7:0]  sdram_dout,
    input  logic        sdram_done
);

    typedef enum logic [1:0] {
        SD_IDLE,
        SD_ACTIVE,
        SD_CAPTURE_READ
    } sd_state_t;

    // CPU-domain request registers. They remain stable until the SDRAM-domain
    // acknowledge has crossed back.
    logic        busy_r;
    logic        req_toggle_cpu;
    logic        ack_seen_cpu;
    logic [18:0] req_addr_cpu;
    logic [7:0]  req_din_cpu;
    logic        req_is_write_cpu;
    logic [7:0]  req_dout_cpu;

    (* async_reg = "true" *) logic [2:0] ack_sync_cpu;

    // SDRAM-domain request/ack state.
    (* async_reg = "true" *) logic [2:0] req_sync_sd = 3'b000;
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

    assign busy       = busy_r;
    assign req_dout   = req_dout_cpu;
    assign sdram_addr = sd_addr_r;
    assign sdram_din  = sd_din_r;
    assign sdram_we   = sd_we_r;
    assign sdram_oe   = sd_oe_r;

    always_ff @(posedge clk) begin
        if (rst) begin
            busy_r           <= 1'b0;
            req_toggle_cpu   <= 1'b0;
            ack_seen_cpu     <= 1'b0;
            req_addr_cpu     <= 19'd0;
            req_din_cpu      <= 8'd0;
            req_is_write_cpu <= 1'b0;
            req_dout_cpu     <= 8'd0;
            ack_sync_cpu     <= 3'b000;
        end else begin
            ack_sync_cpu <= {ack_sync_cpu[1:0], ack_toggle_sd};

            if (!busy_r && (req_we || req_re)) begin
                req_addr_cpu     <= req_addr;
                req_din_cpu      <= req_din;
                req_is_write_cpu <= req_we;
                req_toggle_cpu   <= ~req_toggle_cpu;
                busy_r           <= 1'b1;
            end

            if (busy_r && (ack_sync_cpu[2] != ack_seen_cpu)) begin
                ack_seen_cpu <= ack_sync_cpu[2];
                req_dout_cpu <= result_sd;
                busy_r       <= 1'b0;
            end
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
        end else begin
            req_sync_sd <= {req_sync_sd[1:0], req_toggle_cpu};

            case (sd_state)
                SD_IDLE: begin
                    sd_we_r <= 1'b0;
                    sd_oe_r <= 1'b0;

                    if (req_sync_sd[2] != req_seen_sd) begin
                        req_seen_sd   <= req_sync_sd[2];
                        sd_addr_r     <= {6'b0, req_addr_cpu};
                        sd_din_r      <= req_din_cpu;
                        sd_is_write_r <= req_is_write_cpu;
                        sd_we_r       <= req_is_write_cpu;
                        sd_oe_r       <= !req_is_write_cpu;
                        sd_state      <= SD_ACTIVE;
                    end
                end

                SD_ACTIVE: begin
                    if (sdram_done) begin
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
