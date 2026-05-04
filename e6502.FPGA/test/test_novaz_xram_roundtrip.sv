// NovaZ-shaped XRAM load regression.
//
// Mirrors the NovaHost FIO XLOAD path used by NovaZ:
//   story.bin bytes -> CMD_POKE_SDRAM_BLK chunks at XRAM $000000
//                   -> later CMD_READ_SDRAM_BLK verification
//
// The important part is the transfer shape: flat XRAM address 0, repeated
// 256-byte chunks encoded as count=0, and separate pixel/SDRAM clocks. This
// catches byte drops in the debug-bridge -> SDRAM port-B path before a real
// story file gets involved.

`timescale 1ns/1ps

module test_novaz_xram_roundtrip;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz pixel/debug bridge clock

    logic sdram_clk = 0;
    always #5 sdram_clk = ~sdram_clk;  // 100 MHz SDRAM-side clock

    localparam int STORY_BYTES = 4096;
    localparam int SDRAM_BYTES = 8192;
    localparam int SDRAM_DELAY_CYCLES = 45;

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

    wire        brg_sdram_b_we;
    wire        brg_sdram_b_oe;
    wire [24:0] brg_sdram_b_addr;
    wire [7:0]  brg_sdram_b_din;
    wire [7:0]  brg_sdram_b_dout;
    wire        brg_sdram_b_done_toggle;

    wire        sdram_b_we;
    wire        sdram_b_oe;
    wire [24:0] sdram_b_addr;
    wire [7:0]  sdram_b_din;
    logic [7:0] sdram_b_dout;
    logic       sdram_b_done;
    wire        sdram_b_busy;

    logic       fio_event;
    wire [7:0]  host_status;

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
    ) bridge (
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
        .sdram_b_we(brg_sdram_b_we),
        .sdram_b_oe(brg_sdram_b_oe),
        .sdram_b_addr(brg_sdram_b_addr),
        .sdram_b_din(brg_sdram_b_din),
        .sdram_b_dout(brg_sdram_b_dout),
        .sdram_b_done_toggle(brg_sdram_b_done_toggle),
        .fio_event(fio_event),
        .host_status(host_status)
    );

    debug_sdram_port_b_cdc sdram_cdc (
        .clk        (clk),
        .sdram_clk  (sdram_clk),
        .rst        (rst),
        .req_we     (brg_sdram_b_we),
        .req_oe     (brg_sdram_b_oe),
        .req_addr   (brg_sdram_b_addr),
        .req_din    (brg_sdram_b_din),
        .req_dout   (brg_sdram_b_dout),
        .done_toggle(brg_sdram_b_done_toggle),
        .sdram_we   (sdram_b_we),
        .sdram_oe   (sdram_b_oe),
        .sdram_addr (sdram_b_addr),
        .sdram_din  (sdram_b_din),
        .sdram_dout (sdram_b_dout),
        .sdram_done (sdram_b_done),
        .sdram_busy (sdram_b_busy)
    );

    logic [7:0] story_image [0:STORY_BYTES-1];
    logic [7:0] sdram_shadow [0:SDRAM_BYTES-1];

    logic       sdram_prev_we;
    logic       sdram_prev_oe;
    logic       sdram_op_pending;
    logic       sdram_pending_write;
    logic [7:0] sdram_delay;
    logic [24:0] sdram_pending_addr;
    logic [7:0]  sdram_pending_data;

    always_comb begin
        if (sdram_b_addr < SDRAM_BYTES)
            sdram_b_dout = sdram_shadow[sdram_b_addr];
        else
            sdram_b_dout = 8'h00;
    end

    always_ff @(posedge sdram_clk) begin
        if (rst) begin
            sdram_b_done      <= 1'b0;
            sdram_prev_we     <= 1'b0;
            sdram_prev_oe     <= 1'b0;
            sdram_op_pending  <= 1'b0;
            sdram_pending_write <= 1'b0;
            sdram_delay       <= 8'd0;
            sdram_pending_addr <= 25'd0;
            sdram_pending_data <= 8'd0;
        end else begin
            sdram_b_done <= 1'b0;

            if ((sdram_b_we && !sdram_prev_we) ||
                (sdram_b_oe && !sdram_prev_oe)) begin
                if (sdram_op_pending)
                    $fatal(1, "new SDRAM op while previous op pending");
                sdram_op_pending    <= 1'b1;
                sdram_pending_write <= sdram_b_we;
                sdram_delay         <= SDRAM_DELAY_CYCLES[7:0];
                sdram_pending_addr  <= sdram_b_addr;
                sdram_pending_data  <= sdram_b_din;
            end

            if (sdram_op_pending) begin
                if (sdram_delay == 0) begin
                    if (sdram_pending_write &&
                        sdram_pending_addr < SDRAM_BYTES) begin
                        sdram_shadow[sdram_pending_addr] <= sdram_pending_data;
                    end
                    sdram_b_done   <= 1'b1;
                    sdram_op_pending <= 1'b0;
                end else begin
                    sdram_delay <= sdram_delay - 8'd1;
                end
            end

            sdram_prev_we <= sdram_b_we;
            sdram_prev_oe <= sdram_b_oe;
        end
    end

    int fail_count = 0;
    int write_chunk_count = 0;
    int read_chunk_count = 0;
    int mismatch_count = 0;
    int first_bad_addr = -1;
    int first_bad_expected = 0;
    int first_bad_actual = 0;

    task automatic push_byte(input logic [7:0] b);
        while (!fifo_in_ready) @(posedge clk);
        fifo_in_data  <= b;
        fifo_in_valid <= 1'b1;
        @(posedge clk);
        fifo_in_valid <= 1'b0;
    endtask

    task automatic wait_tx(output logic [7:0] b);
        int timeout = 1000000;
        while (!tx_start && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0)
            $fatal(1, "timeout waiting for bridge TX");
        b = tx_data;
        @(posedge clk);
    endtask

    task automatic check(input string name, input logic condition);
        if (condition) begin
            $display("  PASS %s", name);
        end else begin
            $display("  FAIL %s", name);
            fail_count++;
        end
    endtask

    task automatic write_chunk(input int offset, input int chunk_len);
        logic [7:0] ack;
        logic [7:0] wire_count;
        logic [24:0] addr;
        addr = 25'(offset);
        wire_count = (chunk_len == 256) ? 8'h00 : 8'(chunk_len);

        push_byte(8'h0E);       // CMD_POKE_SDRAM_BLK
        push_byte(addr[23:16]);
        push_byte(addr[15:8]);
        push_byte(addr[7:0]);
        push_byte(wire_count);
        for (int i = 0; i < chunk_len; i++)
            push_byte(story_image[offset + i]);

        wait_tx(ack);
        if (ack !== 8'h00) begin
            $display("  FAIL write chunk %0d ack got 0x%02X", write_chunk_count, ack);
            fail_count++;
        end
        write_chunk_count++;
    endtask

    task automatic verify_chunk(input int offset, input int chunk_len);
        logic [7:0] b;
        logic [7:0] wire_count;
        logic [24:0] addr;
        addr = 25'(offset);
        wire_count = (chunk_len == 256) ? 8'h00 : 8'(chunk_len);

        push_byte(8'h1A);       // CMD_READ_SDRAM_BLK
        push_byte(addr[23:16]);
        push_byte(addr[15:8]);
        push_byte(addr[7:0]);
        push_byte(wire_count);

        wait_tx(b);
        if (b !== 8'h00) begin
            $display("  FAIL read chunk %0d status got 0x%02X", read_chunk_count, b);
            fail_count++;
        end

        for (int i = 0; i < chunk_len; i++) begin
            wait_tx(b);
            if (b !== story_image[offset + i]) begin
                mismatch_count++;
                if (first_bad_addr < 0) begin
                    first_bad_addr = offset + i;
                    first_bad_expected = story_image[offset + i];
                    first_bad_actual = b;
                end
            end
        end
        read_chunk_count++;
    endtask

    task automatic write_all_chunks();
        int offset;
        int remaining;
        int chunk_len;

        offset = 0;
        while (offset < STORY_BYTES) begin
            remaining = STORY_BYTES - offset;
            chunk_len = (remaining >= 256) ? 256 : remaining;
            write_chunk(offset, chunk_len);
            offset += chunk_len;
        end
    endtask

    task automatic verify_all_chunks();
        int offset;
        int remaining;
        int chunk_len;

        offset = 0;
        while (offset < STORY_BYTES) begin
            remaining = STORY_BYTES - offset;
            chunk_len = (remaining >= 256) ? 256 : remaining;
            verify_chunk(offset, chunk_len);
            offset += chunk_len;
        end
    endtask

    initial begin
        $display("=== NovaZ XRAM load/readback regression ===");

        rst            = 1'b1;
        fifo_in_data   = 8'h00;
        fifo_in_valid  = 1'b0;
        tx_busy        = 1'b0;
        dbg_peek_data  = 8'h00;
        dbg_vmem_rdata = 8'h00;
        fio_event      = 1'b0;

        for (int i = 0; i < STORY_BYTES; i++)
            story_image[i] = 8'(i * 17 + 8'h3D);

        // Make the front of the blob look like a plausible Z3 header, because
        // NovaZ's first reads after XLOAD are header bytes from XRAM $000000.
        story_image[0]  = 8'h03;  // version
        story_image[2]  = 8'h12;  // release
        story_image[3]  = 8'h34;
        story_image[4]  = 8'h00;  // high memory
        story_image[5]  = 8'h58;
        story_image[6]  = 8'h4E;  // initial PC
        story_image[7]  = 8'h37;
        story_image[18] = "8";
        story_image[19] = "4";
        story_image[20] = "0";
        story_image[21] = "7";
        story_image[22] = "2";
        story_image[23] = "6";

        for (int i = 0; i < SDRAM_BYTES; i++)
            sdram_shadow[i] = 8'hE7;

        repeat(20) @(posedge clk);
        rst = 1'b0;
        repeat(10) @(posedge clk);

        write_all_chunks();
        verify_all_chunks();

        check("FIFO overflow flag clear", fifo_overflow === 1'b0);
        check("wrote 16 chunks", write_chunk_count == 16);
        check("read 16 chunks", read_chunk_count == 16);
        check("XRAM stream readback matched", mismatch_count == 0);
        if (mismatch_count != 0) begin
            $display("  first mismatch: addr=%0d got=0x%02X want=0x%02X",
                     first_bad_addr, first_bad_actual[7:0], first_bad_expected[7:0]);
            fail_count++;
        end

        if (fail_count != 0)
            $fatal(1, "%0d failures", fail_count);

        $display("ALL TESTS PASSED");
        $finish;
    end

    initial begin
        #400000000;
        $fatal(1, "global timeout");
    end

endmodule
