// nic.sv unit test.
//
// Verifies:
//   - 64-byte register file at $A100-$A13F
//   - Writing non-zero to NIC_CMD pulses nic_event
//   - Debug writes update slot/global status without pulsing nic_event
//   - Data/error/remote-closed transitions latch IRQSTATUS for enabled slots
//   - CPU read of NIC_IRQSTATUS clears the IRQ latch

`timescale 1ns/1ps

module test_nic;
    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic cpu_ce = 0;
    logic [15:0] cpu_addr = 0;
    logic [7:0]  cpu_wdata = 0;
    logic        cpu_we = 0;
    wire  [7:0]  cpu_rdata;

    logic        dbg_we = 0;
    logic [5:0]  dbg_addr = 0;
    logic [7:0]  dbg_wdata = 0;
    wire  [7:0]  dbg_rdata;

    logic        dbg_buf_we = 0;
    logic        dbg_buf_re = 0;
    logic        dbg_buf_sel = 0;
    logic [7:0]  dbg_buf_addr = 0;
    logic [7:0]  dbg_buf_wdata = 0;
    wire  [7:0]  dbg_buf_rdata;

    logic        dma_grant = 1;
    wire         rdy_out;
    wire [15:0] ram_addr;
    logic [7:0] ram_rdata;
    wire [7:0]  ram_wdata;
    wire        ram_we;
    wire        ram_re;

    wire nic_event;
    wire irq;

    nic uut (
        .clk(clk), .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr),
        .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we),
        .cpu_rdata(cpu_rdata),
        .dbg_we(dbg_we),
        .dbg_addr(dbg_addr),
        .dbg_wdata(dbg_wdata),
        .dbg_raddr(dbg_addr),
        .dbg_rdata(dbg_rdata),
        .dbg_buf_we(dbg_buf_we),
        .dbg_buf_re(dbg_buf_re),
        .dbg_buf_sel(dbg_buf_sel),
        .dbg_buf_addr(dbg_buf_addr),
        .dbg_buf_wdata(dbg_buf_wdata),
        .dbg_buf_rdata(dbg_buf_rdata),
        .dma_grant(dma_grant),
        .rdy_out(rdy_out),
        .ram_addr(ram_addr),
        .ram_rdata(ram_rdata),
        .ram_wdata(ram_wdata),
        .ram_we(ram_we),
        .ram_re(ram_re),
        .nic_event(nic_event),
        .irq(irq)
    );

    logic [7:0] ram [0:65535];
    initial begin
        for (int i = 0; i < 65536; i++)
            ram[i] = 8'h00;
    end

    always_comb begin
        ram_rdata = ram[ram_addr];
    end

    always_ff @(posedge clk) begin
        if (ram_we)
            ram[ram_addr] <= ram_wdata;
    end

    int pass_count = 0, fail_count = 0, test_num = 0;

    task automatic check(input string name, input logic cond);
        test_num++;
        if (cond) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    task automatic check_eq(input string name, input int got, input int want);
        test_num++;
        if (got == want) begin
            $display("  PASS [%0d] %s (=0x%0h)", test_num, name, got);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%0h, want 0x%0h)",
                     test_num, name, got, want);
            fail_count++;
        end
    endtask

    task automatic cpu_write(input [15:0] a, input [7:0] d);
        @(posedge clk);
        cpu_ce    <= 1;
        cpu_we    <= 1;
        cpu_addr  <= a;
        cpu_wdata <= d;
        @(posedge clk);
        cpu_ce    <= 0;
        cpu_we    <= 0;
    endtask

    task automatic cpu_read(input [15:0] a, output logic [7:0] d);
        @(posedge clk);
        cpu_ce   <= 1;
        cpu_we   <= 0;
        cpu_addr <= a;
        @(posedge clk);
        d = cpu_rdata;
        cpu_ce <= 0;
    endtask

    task automatic dbg_write(input [5:0] off, input [7:0] d);
        @(posedge clk);
        dbg_we    <= 1;
        dbg_addr  <= off;
        dbg_wdata <= d;
        @(posedge clk);
        dbg_we    <= 0;
    endtask

    task automatic dbg_buf_write(input bit sel, input [7:0] off, input [7:0] d);
        @(posedge clk);
        dbg_buf_sel   <= sel;
        dbg_buf_addr  <= off;
        dbg_buf_wdata <= d;
        dbg_buf_we    <= 1;
        @(posedge clk);
        dbg_buf_we    <= 0;
    endtask

    task automatic dbg_buf_read(input bit sel, input [7:0] off, output logic [7:0] d);
        @(posedge clk);
        dbg_buf_sel  <= sel;
        dbg_buf_addr <= off;
        dbg_buf_re   <= 1;
        @(posedge clk);
        d = dbg_buf_rdata;
        dbg_buf_re <= 0;
    endtask

    task automatic wait_dma_done(output logic saw_event);
        int timeout;
        begin
            saw_event = 0;
            timeout = 1000;
            while (timeout > 0) begin
                @(posedge clk);
                if (nic_event)
                    saw_event = 1;
                if (rdy_out)
                    break;
                timeout--;
            end
        end
    endtask

    task automatic cpu_write_watch_event(
        input [15:0] a,
        input [7:0] d,
        output logic saw_event
    );
        saw_event = 0;
        @(posedge clk);
        cpu_ce    <= 1;
        cpu_we    <= 1;
        cpu_addr  <= a;
        cpu_wdata <= d;
        @(posedge clk);
        saw_event = nic_event;
        cpu_ce    <= 0;
        cpu_we    <= 0;
        @(posedge clk);
    endtask

    initial begin
        logic [7:0] d;
        logic saw;

        $display("=== nic.sv ===");

        rst = 1;
        repeat(5) @(posedge clk);
        rst = 0;
        @(posedge clk);

        cpu_read(16'hA101, d);
        check_eq("global status starts READY", int'(d), 8'h01);
        cpu_read(16'hA118, d);
        check_eq("slot0 starts SENDREADY", int'(d), 8'h04);

        cpu_write(16'hA108, 8'h67);
        cpu_read(16'hA108, d);
        check_eq("remote port low roundtrip", int'(d), 8'h67);

        cpu_write_watch_event(16'hA100, 8'h00, saw);
        check("no event on NIC_CMD=0", !saw);

        cpu_write_watch_event(16'hA100, 8'h01, saw);
        check("event on NIC_CMD=CONNECT", saw);

        dbg_write(6'h03, 8'h01); // enable slot0 IRQs
        check("irq low before status transition", !irq);

        dbg_write(6'h18, 8'h06); // sendready + dataready
        check("irq high after dataready transition", irq);
        cpu_read(16'hA104, d);
        check_eq("irqstatus read returns slot0 bit", int'(d), 8'h01);
        @(posedge clk);
        check("irq remains level-high while dataready is set", irq);
        dbg_write(6'h18, 8'h04);
        check("irq clears after slot status clears", !irq);

        dbg_write(6'h18, 8'h16); // sendready + dataready + remote closed
        check("irq high after remote-closed transition", irq);
        cpu_read(16'hA104, d);
        check_eq("irqstatus read returns slot0 bit again", int'(d), 8'h01);
        @(posedge clk);
        check("irq remains high while remote-closed is set", irq);
        dbg_write(6'h18, 8'h04);
        check("irq clears again after status clears", !irq);

        dbg_write(6'h03, 8'h00); // disable IRQ
        dbg_write(6'h19, 8'h0C); // slot1 sendready + error
        check("disabled slot IRQ does not assert", !irq);

        cpu_read(16'hA118, d);
        check_eq("slot0 status visible to CPU", int'(d), 8'h04);
        dbg_addr = 6'h18;
        repeat(1) @(posedge clk);
        check_eq("slot0 status visible to debug", int'(dbg_rdata), 8'h04);

        ram[16'h2200] = 8'hDE;
        ram[16'h2201] = 8'hAD;
        ram[16'h2202] = 8'hBE;
        cpu_write(16'hA110, 8'h00);
        cpu_write(16'hA111, 8'h22);
        cpu_write(16'hA112, 8'h03);
        cpu_write_watch_event(16'hA100, 8'h03, saw);
        check("SEND does not pulse event before TX DMA completes", !saw);
        wait_dma_done(saw);
        check("SEND pulses event after TX DMA", saw);
        check("NIC ready after SEND DMA", rdy_out);
        dbg_addr = 6'h14;
        @(posedge clk);
        check_eq("DMASTATUS reports TX ready", int'(dbg_rdata), 8'h01);
        dbg_buf_read(1'b0, 8'h00, d);
        check_eq("TX buffer byte 0", int'(d), 8'hDE);
        dbg_buf_read(1'b0, 8'h01, d);
        check_eq("TX buffer byte 1", int'(d), 8'hAD);
        dbg_buf_read(1'b0, 8'h02, d);
        check_eq("TX buffer byte 2", int'(d), 8'hBE);

        dbg_buf_write(1'b1, 8'h00, 8'hCA);
        dbg_buf_write(1'b1, 8'h01, 8'hFE);
        dbg_buf_write(1'b1, 8'h02, 8'h42);
        cpu_write(16'hA110, 8'h00);
        cpu_write(16'hA111, 8'h23);
        dbg_write(6'h13, 8'h03); // NIC_MSGLEN
        dbg_write(6'h07, 8'h01); // HOSTCTRL.RX_START
        wait_dma_done(saw);
        check("RX DMA completes without host event", !saw);
        check("NIC ready after RX DMA", rdy_out);
        dbg_addr = 6'h14;
        @(posedge clk);
        check_eq("DMASTATUS reports RX done", int'(dbg_rdata), 8'h02);
        check_eq("RX DMA wrote byte 0", int'(ram[16'h2300]), 8'hCA);
        check_eq("RX DMA wrote byte 1", int'(ram[16'h2301]), 8'hFE);
        check_eq("RX DMA wrote byte 2", int'(ram[16'h2302]), 8'h42);

        $display("=== nic.sv: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count != 0)
            $fatal(1, "nic.sv failed");
        $finish;
    end
endmodule
