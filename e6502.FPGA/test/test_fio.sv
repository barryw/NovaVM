// fio.sv unit test. Stand-alone — does not need sid_tb.svh or vgc_tb.svh.
//
// Verifies:
//   - 80-byte register file at $B9A0-$B9EF: CPU r/w roundtrip
//   - Addresses outside the bank are ignored
//   - Writing non-zero to FioCmd ($B9A0) asserts fio_event for 1 clock
//   - Writing zero to FioCmd does NOT assert fio_event
//   - Writes to other bank addresses do NOT assert fio_event
//   - Debug-bridge writes land in storage but do NOT assert fio_event
//   - Debug writes and CPU reads see the same value (shared storage)
`timescale 1ns/1ps

module test_fio;
    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;
    logic cpu_ce = 0;
    logic [15:0] cpu_addr = 0;
    logic [7:0]  cpu_wdata = 0;
    logic        cpu_we = 0;
    wire  [7:0]  cpu_rdata;

    logic        dbg_we = 0;
    logic [6:0]  dbg_addr = 0;
    logic [7:0]  dbg_wdata = 0;

    wire         fio_event;

    fio uut (
        .clk(clk), .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata), .cpu_we(cpu_we),
        .cpu_rdata(cpu_rdata),
        .dbg_we(dbg_we), .dbg_addr(dbg_addr), .dbg_wdata(dbg_wdata),
        .fio_event(fio_event)
    );

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

    task automatic check_eq(input string name, input int a, input int e);
        test_num++;
        if (a == e) begin
            $display("  PASS [%0d] %s (=0x%0h)", test_num, name, a);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%0h, want 0x%0h)",
                     test_num, name, a, e);
            fail_count++;
        end
    endtask

    // CPU write: assert ce + we + addr + data for one cycle.
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

    // CPU read: set addr, read combinational rdata on next clock.
    task automatic cpu_read(input [15:0] a, output logic [7:0] d);
        @(posedge clk);
        cpu_addr <= a;
        cpu_ce   <= 0;
        cpu_we   <= 0;
        @(posedge clk);
        d = cpu_rdata;
    endtask

    // Debug write: assert dbg_we for one cycle.
    task automatic dbg_write(input [6:0] off, input [7:0] d);
        @(posedge clk);
        dbg_we    <= 1;
        dbg_addr  <= off;
        dbg_wdata <= d;
        @(posedge clk);
        dbg_we    <= 0;
    endtask

    // Sample fio_event during the write cycle. Run a CPU write and
    // record whether fio_event pulsed during it.
    task automatic cpu_write_watch_event(input [15:0] a,
                                          input [7:0] d,
                                          output logic saw_event);
        saw_event = 0;
        @(posedge clk);
        cpu_ce    <= 1;
        cpu_we    <= 1;
        cpu_addr  <= a;
        cpu_wdata <= d;
        @(posedge clk);
        // Sample at the rising edge after the write registers.
        saw_event = fio_event;
        cpu_ce    <= 0;
        cpu_we    <= 0;
        // One more clock to let any pulse deassert naturally.
        @(posedge clk);
    endtask

    initial begin
        logic [7:0] d;
        logic saw;

        $display("=== fio.sv ===");

        rst = 1;
        repeat(5) @(posedge clk);
        rst = 0;
        @(posedge clk);

        // ── Roundtrip CPU write then CPU read, several addresses ──
        cpu_write(16'hB9A1, 8'h02);   // FioStatus
        cpu_read(16'hB9A1, d);
        check_eq("FioStatus roundtrip", int'(d), 8'h02);

        cpu_write(16'hB9A3, 8'h10);   // FioNameLen
        cpu_read(16'hB9A3, d);
        check_eq("FioNameLen roundtrip", int'(d), 8'h10);

        cpu_write(16'hB9B0, 8'h41);   // FioName[0] = 'A'
        cpu_read(16'hB9B0, d);
        check_eq("FioName[0] roundtrip", int'(d), 8'h41);

        cpu_write(16'hB9EF, 8'h5A);   // last byte of bank
        cpu_read(16'hB9EF, d);
        check_eq("FioName[63] (last) roundtrip", int'(d), 8'h5A);

        // ── Out-of-range CPU read returns 0 ──
        cpu_read(16'hB99F, d);        // one before base
        check_eq("out-of-range read (below) = 0", int'(d), 8'h00);

        cpu_read(16'hB9F0, d);        // one after bank
        check_eq("out-of-range read (above) = 0", int'(d), 8'h00);

        // ── Writing zero to FioCmd must NOT fire event ──
        cpu_write_watch_event(16'hB9A0, 8'h00, saw);
        check("no event on FioCmd=0", !saw);

        // ── Writing non-zero to FioCmd MUST fire event ──
        cpu_write_watch_event(16'hB9A0, 8'h01, saw);
        check("event on FioCmd=1", saw);

        // Verify FioCmd was stored
        cpu_read(16'hB9A0, d);
        check_eq("FioCmd=1 stored", int'(d), 8'h01);

        // Different non-zero value also fires event
        cpu_write_watch_event(16'hB9A0, 8'h24, saw);   // FioCmdMount
        check("event on FioCmd=0x24", saw);

        // ── Writes to other bank addresses do NOT fire event ──
        cpu_write_watch_event(16'hB9A1, 8'hFF, saw);
        check("no event on FioStatus write", !saw);

        cpu_write_watch_event(16'hB9B0, 8'h42, saw);
        check("no event on FioName write", !saw);

        cpu_write_watch_event(16'hB9EF, 8'h99, saw);
        check("no event on last-byte write", !saw);

        // ── Writes outside the bank do NOT fire event ──
        cpu_write_watch_event(16'hB99F, 8'hFF, saw);
        check("no event on out-of-range write below", !saw);

        cpu_write_watch_event(16'hB9F0, 8'hFF, saw);
        check("no event on out-of-range write above", !saw);

        // ── Debug-bridge writes land in storage but do NOT fire event ──
        // dbg_addr[6:0] is offset within the 80-byte bank.
        fork
            begin
                // Watch fio_event during dbg_write.
                @(posedge clk);
                dbg_we    <= 1;
                dbg_addr  <= 7'h01;   // FioStatus offset
                dbg_wdata <= 8'h02;
                @(posedge clk);
                saw = fio_event;
                dbg_we    <= 0;
            end
        join
        check("no event on dbg write", !saw);

        // Verify the dbg write landed
        cpu_read(16'hB9A1, d);
        check_eq("dbg write FioStatus=2 visible to CPU", int'(d), 8'h02);

        // Dbg write to FioCmd offset 0 should NOT fire event even though
        // it's the cmd register.
        fork
            begin
                @(posedge clk);
                dbg_we    <= 1;
                dbg_addr  <= 7'h00;
                dbg_wdata <= 8'h99;
                @(posedge clk);
                saw = fio_event;
                dbg_we    <= 0;
            end
        join
        check("no event on dbg write to FioCmd", !saw);
        cpu_read(16'hB9A0, d);
        check_eq("dbg FioCmd=0x99 visible to CPU", int'(d), 8'h99);

        // ── Reset clears all registers ──
        rst = 1;
        repeat(3) @(posedge clk);
        rst = 0;
        @(posedge clk);

        cpu_read(16'hB9A0, d);
        check_eq("reset clears FioCmd", int'(d), 8'h00);
        cpu_read(16'hB9A1, d);
        check_eq("reset clears FioStatus", int'(d), 8'h00);
        cpu_read(16'hB9EF, d);
        check_eq("reset clears last byte", int'(d), 8'h00);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end
endmodule
