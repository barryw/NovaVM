// Debug bridge RX FIFO regression.
//
// Streams a full 256-byte CMD_POKE_SDRAM_BLK packet into the same AXI FIFO
// used by fpga_top while the modeled SDRAM port-B completion is deliberately
// slow. The expected behavior is that the FIFO absorbs the full host burst and
// the bridge drains it losslessly as SDRAM accepts each byte.

`timescale 1ns/1ps

module test_debug_bridge_rx_fifo;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;

    logic [7:0] fifo_in_data;
    logic       fifo_in_valid;
    wire        fifo_in_ready;
    wire [7:0]  bridge_rx_data;
    wire        bridge_rx_valid;
    wire        bridge_rx_ready;
    wire        fifo_overflow;

    wire [7:0] tx_data;
    wire       tx_start;
    logic      tx_busy;

    wire        dbg_peek_en;
    wire [15:0] dbg_peek_addr;
    logic [7:0] dbg_peek_data;
    wire        dbg_poke_en;
    wire [15:0] dbg_poke_addr;
    wire [7:0]  dbg_poke_data;
    wire        dbg_pause;
    wire        dbg_vmem_we;
    wire        dbg_vmem_re;
    wire [2:0]  dbg_vmem_space;
    wire [16:0] dbg_vmem_addr;
    wire [7:0]  dbg_vmem_data;
    logic [7:0] dbg_vmem_rdata;
    wire        dbg_rom_we;
    wire        dbg_rom_idx;
    wire [13:0] dbg_rom_addr;
    wire [7:0]  dbg_rom_data;
    wire        dbg_cpu_reset;
    wire        dbg_system_reset;
    wire        dbg_cpu_resume;

    logic [15:0] dbg_cpu_pc    = 16'hC000;
    logic [7:0]  dbg_cpu_a     = 8'h00;
    logic [7:0]  dbg_cpu_x     = 8'h00;
    logic [7:0]  dbg_cpu_y     = 8'h00;
    logic [7:0]  dbg_cpu_sp    = 8'hFF;
    logic [7:0]  dbg_cpu_flags = 8'h20;
    logic [5:0]  dbg_cpu_state = 6'd12;
    logic [7:0]  dbg_cpu_ir    = 8'hEA;
    logic [15:0] dbg_cpu_addr  = 16'hC000;
    logic [7:0]  dbg_cpu_din   = 8'hEA;
    logic [7:0]  dbg_cpu_dout  = 8'h00;
    logic        dbg_cpu_we    = 1'b0;
    logic        dbg_cpu_rdy   = 1'b0;
    logic        dbg_cpu_irq   = 1'b0;
    logic        dbg_cpu_nmi   = 1'b0;
    logic        dbg_cpu_waiting = 1'b0;
    logic        dbg_cpu_stopped = 1'b0;

    wire        key_inject_valid;
    wire [7:0]  key_inject_data;

    wire        sdram_b_we;
    wire        sdram_b_oe;
    wire [24:0] sdram_b_addr;
    wire [7:0]  sdram_b_din;
    logic [7:0] sdram_b_dout;
    logic       sdram_b_done_toggle;
    logic       fio_event;
    wire [7:0]  host_status;

    localparam int SDRAM_DELAY_CYCLES = 40;
    localparam int PACKET_BYTES = 5 + 256;

    debug_byte_fifo #(
        .ADDR_WIDTH(9)
    ) rx_fifo (
        .clk      (clk),
        .rst      (rst),
        .s_data   (fifo_in_data),
        .s_valid  (fifo_in_valid),
        .s_ready  (fifo_in_ready),
        .m_data   (bridge_rx_data),
        .m_valid  (bridge_rx_valid),
        .m_ready  (bridge_rx_ready),
        .overflow (fifo_overflow),
        .fill     ()
    );

    debug_bridge #(
        .BOOT_AUTO_RELEASE_CYCLES(1024)
    ) dut (
        .clk(clk), .rst(rst),
        .rx_data(bridge_rx_data), .rx_valid(bridge_rx_valid),
        .rx_ready(bridge_rx_ready),
        .tx_data(tx_data), .tx_start(tx_start), .tx_busy(tx_busy),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_vmem_we(dbg_vmem_we), .dbg_vmem_re(dbg_vmem_re),
        .dbg_vmem_space(dbg_vmem_space), .dbg_vmem_addr(dbg_vmem_addr),
        .dbg_vmem_data(dbg_vmem_data), .dbg_vmem_rdata(dbg_vmem_rdata),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(dbg_system_reset),
        .dbg_cpu_resume(dbg_cpu_resume),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags),
        .dbg_cpu_state(dbg_cpu_state),
        .dbg_cpu_ir(dbg_cpu_ir),
        .dbg_cpu_addr(dbg_cpu_addr),
        .dbg_cpu_din(dbg_cpu_din),
        .dbg_cpu_dout(dbg_cpu_dout),
        .dbg_cpu_we(dbg_cpu_we),
        .dbg_cpu_rdy(dbg_cpu_rdy),
        .dbg_cpu_irq(dbg_cpu_irq),
        .dbg_cpu_nmi(dbg_cpu_nmi),
        .dbg_cpu_waiting(dbg_cpu_waiting),
        .dbg_cpu_stopped(dbg_cpu_stopped),
        .key_inject_valid(key_inject_valid),
        .key_inject_data(key_inject_data),
        .sdram_b_we(sdram_b_we),
        .sdram_b_oe(sdram_b_oe),
        .sdram_b_addr(sdram_b_addr),
        .sdram_b_din(sdram_b_din),
        .sdram_b_dout(sdram_b_dout),
        .sdram_b_done_toggle(sdram_b_done_toggle),
        .fio_event(fio_event),
        .host_status(host_status)
    );

    logic [7:0] sdram_shadow [0:255];
    logic       sdram_prev_we;
    logic       sdram_write_pending;
    logic [7:0] sdram_delay;
    logic [24:0] sdram_pending_addr;
    logic [7:0]  sdram_pending_data;

    always_comb begin
        sdram_b_dout = sdram_shadow[sdram_b_addr[7:0]];
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            sdram_b_done_toggle <= 1'b0;
            sdram_prev_we       <= 1'b0;
            sdram_write_pending <= 1'b0;
            sdram_delay         <= 8'd0;
            sdram_pending_addr  <= 25'd0;
            sdram_pending_data  <= 8'd0;
        end else begin
            if (sdram_b_we && !sdram_prev_we) begin
                if (sdram_write_pending)
                    $fatal(1, "new SDRAM write while previous write pending");
                sdram_write_pending <= 1'b1;
                sdram_delay         <= SDRAM_DELAY_CYCLES[7:0];
                sdram_pending_addr  <= sdram_b_addr;
                sdram_pending_data  <= sdram_b_din;
            end

            if (sdram_write_pending) begin
                if (sdram_delay == 0) begin
                    if (sdram_pending_addr < 25'd256)
                        sdram_shadow[sdram_pending_addr[7:0]] <= sdram_pending_data;
                    sdram_b_done_toggle <= ~sdram_b_done_toggle;
                    sdram_write_pending <= 1'b0;
                end else begin
                    sdram_delay <= sdram_delay - 8'd1;
                end
            end

            sdram_prev_we <= sdram_b_we;
        end
    end

    int pushed_bytes;

    task automatic push_byte(input logic [7:0] b);
        while (!fifo_in_ready) @(posedge clk);
        fifo_in_data  <= b;
        fifo_in_valid <= 1'b1;
        @(posedge clk);
        fifo_in_valid <= 1'b0;
        pushed_bytes++;
    endtask

    task automatic wait_tx(output logic [7:0] b);
        int timeout = 200000;
        while (!tx_start && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0)
            $fatal(1, "timeout waiting for bridge TX");
        b = tx_data;
        @(posedge clk);
    endtask

    task automatic check_eq8(input string name, input logic [7:0] actual,
                             input logic [7:0] expected,
                             inout int fail_count);
        if (actual === expected) begin
            $display("  PASS %s (=0x%02X)", name, actual);
        end else begin
            $display("  FAIL %s (got 0x%02X, want 0x%02X)",
                     name, actual, expected);
            fail_count++;
        end
    endtask

    initial begin
        logic [7:0] ack;
        int failures;

        $display("=== debug_bridge RX FIFO SDRAM burst regression ===");

        failures      = 0;
        rst           = 1'b1;
        fifo_in_data  = 8'h00;
        fifo_in_valid = 1'b0;
        tx_busy       = 1'b0;
        dbg_peek_data = 8'h00;
        dbg_vmem_rdata = 8'h00;
        fio_event     = 1'b0;
        pushed_bytes  = 0;
        for (int i = 0; i < 256; i++)
            sdram_shadow[i] = 8'hFF;

        repeat(20) @(posedge clk);
        rst = 1'b0;
        repeat(10) @(posedge clk);

        push_byte(8'h0E);   // CMD_POKE_SDRAM_BLK
        push_byte(8'h00);   // addr_hi
        push_byte(8'h00);   // addr_mid
        push_byte(8'h00);   // addr_lo
        push_byte(8'h00);   // count=0 -> 256 bytes
        for (int i = 0; i < 256; i++)
            push_byte(8'h40 + 8'(i));

        wait_tx(ack);
        check_eq8("ack status", ack, 8'h00, failures);
        check_eq8("fifo overflow flag", {7'b0, fifo_overflow}, 8'h00, failures);
        if (pushed_bytes != PACKET_BYTES) begin
            $display("  FAIL pushed byte count (got %0d, want %0d)",
                     pushed_bytes, PACKET_BYTES);
            failures++;
        end else begin
            $display("  PASS pushed byte count (= %0d)", pushed_bytes);
        end

        for (int i = 0; i < 256; i++)
            check_eq8($sformatf("sdram_shadow[%0d]", i),
                      sdram_shadow[i], 8'h40 + 8'(i), failures);

        if (failures != 0)
            $fatal(1, "%0d failures", failures);

        $display("ALL TESTS PASSED");
        $finish;
    end

    initial begin
        #50000000;
        $fatal(1, "global timeout");
    end

endmodule
