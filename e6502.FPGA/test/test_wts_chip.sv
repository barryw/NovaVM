// WTS unit regression.
//
// Verifies the hardware WTS register ABI mirrors Avalonia's WavetableSynth
// surface: per-voice latches, global defaults, instrument enumeration, status
// bits, all-notes-off, and non-zero stereo audio from a resident instrument.

`timescale 1ns/1ps

module test_wts_chip;

    logic clk = 0;
    always #5 clk = ~clk;

    logic rst;
    logic cpu_ce = 1'b1;
    logic [15:0] cpu_addr;
    logic [7:0] cpu_wdata;
    logic cpu_we;
    wire [7:0] cpu_rdata;

    logic dbg_we = 1'b0;
    logic [7:0] dbg_addr = 8'h00;
    logic [7:0] dbg_wdata = 8'h00;
    logic [7:0] dbg_raddr = 8'h00;
    wire [7:0] dbg_rdata;

    logic host_event_we = 1'b0;
    logic [7:0] host_event_data = 8'h00;
    wire host_event_ready;

    wire signed [17:0] audio_l;
    wire signed [17:0] audio_r;
    wire [24:0] sdram_addrB;
    wire [7:0] sdram_dinB;
    wire sdram_weB;
    wire sdram_oeB;
    logic [7:0] sdram_doutB;
    logic saw_sdram_read;

    always_ff @(posedge clk) begin
        if (rst) begin
            saw_sdram_read <= 1'b0;
        end else if (sdram_oeB) begin
            saw_sdram_read <= 1'b1;
            sdram_doutB <= sdram_addrB[0] ? 8'sh40 : 8'shC0;
        end
    end

    wts_chip dut (
        .clk(clk),
        .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr),
        .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we),
        .cpu_rdata(cpu_rdata),
        .dbg_we(dbg_we),
        .dbg_addr(dbg_addr),
        .dbg_wdata(dbg_wdata),
        .dbg_raddr(dbg_raddr),
        .dbg_rdata(dbg_rdata),
        .host_event_we(host_event_we),
        .host_event_data(host_event_data),
        .host_event_ready(host_event_ready),
        .sdram_addrB(sdram_addrB),
        .sdram_dinB(sdram_dinB),
        .sdram_weB(sdram_weB),
        .sdram_oeB(sdram_oeB),
        .sdram_doutB(sdram_doutB),
        .audio_l(audio_l),
        .audio_r(audio_r)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    task automatic check(input string name, input logic condition);
        test_num++;
        if (condition) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    task automatic check_eq8(input string name, input logic [7:0] actual,
                             input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic cpu_write(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr;
        cpu_wdata <= data;
        cpu_we <= 1'b1;
        @(posedge clk);
        cpu_we <= 1'b0;
    endtask

    task automatic cpu_read(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr;
        cpu_we <= 1'b0;
        #1;
        data = cpu_rdata;
    endtask

    task automatic host_event_byte(input logic [7:0] data);
        begin
            @(posedge clk);
            host_event_data <= data;
            host_event_we <= 1'b1;
            @(posedge clk);
            host_event_we <= 1'b0;
        end
    endtask

    task automatic host_event_record(
        input logic [31:0] event_frame,
        input logic [7:0] off,
        input logic [7:0] data
    );
        begin
            host_event_byte(event_frame[7:0]);
            host_event_byte(event_frame[15:8]);
            host_event_byte(event_frame[23:16]);
            host_event_byte(event_frame[31:24]);
            host_event_byte(off);
            host_event_byte(data);
        end
    endtask

    function automatic int abs18(input logic signed [17:0] value);
        begin
            abs18 = value < 0 ? -int'(value) : int'(value);
        end
    endfunction

    initial begin
        logic [7:0] d;
        int max_l;
        int max_r;

        $display("=== WTS chip unit test ===");

        rst = 1'b1;
        cpu_addr = 16'h0000;
        cpu_wdata = 8'h00;
        cpu_we = 1'b0;
        repeat(8) @(posedge clk);
        rst = 1'b0;
        repeat(4) @(posedge clk);

        cpu_read(16'hA180, d);
        check_eq8("default reverb level", d, 8'd80);
        cpu_read(16'hA181, d);
        check_eq8("default chorus level", d, 8'd40);
        cpu_read(16'hA182, d);
        check_eq8("default master volume", d, 8'hFF);
        cpu_read(16'hA183, d);
        check_eq8("resident sound bank reports ready", d, 8'h01);
        cpu_read(16'hA184, d);
        check_eq8("resident instrument count", d, 8'h08);
        cpu_read(16'hA186, d);
        check_eq8("hardware WTS signature", d, "W");
        cpu_read(16'hA143, d);
        check_eq8("voice volume default", d, 8'hFF);
        cpu_read(16'hA144, d);
        check_eq8("voice pan default", d, 8'h80);

        cpu_write(16'hA1A0, 8'h02);
        cpu_read(16'hA1A1, d);
        check_eq8("enum bank", d, 8'h00);
        cpu_read(16'hA1A2, d);
        check_eq8("enum program mirrors resident index", d, 8'h02);
        cpu_read(16'hA1A3, d);
        check_eq8("enum name byte 0", d, "S");
        cpu_read(16'hA1A4, d);
        check_eq8("enum name byte 1", d, "Q");

        cpu_write(16'hA141, 8'd100);
        cpu_write(16'hA142, 8'h00);
        cpu_write(16'hA143, 8'hFF);
        cpu_write(16'hA144, 8'h80);
        cpu_write(16'hA145, 8'h00);
        cpu_write(16'hA146, 8'h80);
        cpu_write(16'hA140, 8'd69);
        repeat(8) @(posedge clk);

        cpu_read(16'hA147, d);
        check("voice status active after note-on", d[0] === 1'b1);
        cpu_read(16'hA187, d);
        check_eq8("active voice mask after note-on", d, 8'h01);

        max_l = 0;
        max_r = 0;
        repeat(40000) begin
            @(posedge clk);
            if (abs18(audio_l) > max_l) max_l = abs18(audio_l);
            if (abs18(audio_r) > max_r) max_r = abs18(audio_r);
        end
        check("note-on produces left audio", max_l > 0);
        check("note-on produces right audio", max_r > 0);

        cpu_write(16'hA140, 8'h00);
        repeat(8) @(posedge clk);
        cpu_read(16'hA147, d);
        check("note-off marks voice releasing", d[1] === 1'b1);
        repeat(100000) @(posedge clk);
        cpu_read(16'hA147, d);
        check("note-off keeps a musical release tail", d[0] === 1'b1 && d[1] === 1'b1);

        cpu_write(16'hA185, 8'h01);
        repeat(8) @(posedge clk);
        cpu_read(16'hA187, d);
        check_eq8("all-notes-off clears active mask", d, 8'h00);

        cpu_write(16'hA188, 8'h00);
        cpu_write(16'hA19E, 8'h00);
        cpu_write(16'hA19F, 8'h34);
        cpu_write(16'hA19E, 8'h01);
        cpu_write(16'hA19F, 8'h12);
        cpu_write(16'hA19E, 8'h02);
        cpu_write(16'hA19F, 8'h78);
        cpu_write(16'hA19E, 8'h03);
        cpu_write(16'hA19F, 8'h56);
        cpu_write(16'hA19E, 8'h04);
        cpu_write(16'hA19F, 8'hBC);
        cpu_write(16'hA19E, 8'h05);
        cpu_write(16'hA19F, 8'h9A);
        cpu_write(16'hA19E, 8'h06);
        cpu_write(16'hA19F, 8'h00);
        cpu_write(16'hA19E, 8'h07);
        cpu_write(16'hA19F, 8'h70);
        cpu_write(16'hA19E, 8'h00);
        cpu_read(16'hA19F, d);
        check_eq8("envelope attack low readback", d, 8'h34);
        cpu_write(16'hA19E, 8'h01);
        cpu_read(16'hA19F, d);
        check_eq8("envelope attack high readback", d, 8'h12);
        cpu_write(16'hA19E, 8'h05);
        cpu_read(16'hA19F, d);
        check_eq8("envelope sustain high readback", d, 8'h9A);

        cpu_write(16'hA140, 8'd69);
        repeat(40000) @(posedge clk);
        cpu_write(16'hA140, 8'h00);
        repeat(20000) @(posedge clk);
        cpu_read(16'hA147, d);
        check("programmed fast release clears voice", d[0] === 1'b0 && d[1] === 1'b0);

        cpu_write(16'hA180, 8'h01);
        cpu_write(16'hA181, 8'h02);
        cpu_write(16'hA182, 8'h03);
        cpu_write(16'hA185, 8'h02);
        cpu_read(16'hA180, d);
        check_eq8("reset effects restores reverb", d, 8'd80);
        cpu_read(16'hA181, d);
        check_eq8("reset effects restores chorus", d, 8'd40);
        cpu_read(16'hA182, d);
        check_eq8("reset effects restores master", d, 8'hFF);

        cpu_write(16'hA185, 8'h03);
        repeat(4) @(posedge clk);
        cpu_read(16'hA1E0, d);
        check("event FIFO ready after reset", d[0] === 1'b1);
        host_event_record(32'd4, 8'h01, 8'd100);
        host_event_record(32'd4, 8'h03, 8'hFF);
        host_event_record(32'd4, 8'h04, 8'h80);
        host_event_record(32'd4, 8'h00, 8'd60);
        repeat(4) @(posedge clk);
        cpu_read(16'hA1E1, d);
        check_eq8("event FIFO queued records", d, 8'd4);
        cpu_write(16'hA185, 8'h04);
        repeat(4000) @(posedge clk);
        cpu_read(16'hA187, d);
        check_eq8("event stream fired scheduled note-on", d, 8'h01);
        cpu_read(16'hA1E1, d);
        check_eq8("event FIFO drained scheduled records", d, 8'd0);
        cpu_write(16'hA185, 8'h01);
        repeat(8) @(posedge clk);

        cpu_write(16'hA188, 8'h00);
        cpu_write(16'hA189, 8'h00);
        cpu_write(16'hA18A, 8'h20);
        cpu_write(16'hA18B, 8'h81);
        cpu_write(16'hA18C, 8'h00);
        cpu_write(16'hA18D, 8'h40);
        cpu_write(16'hA18E, 8'h20);
        cpu_write(16'hA18F, 8'h81);
        cpu_write(16'hA190, 8'h00);
        cpu_write(16'hA191, 8'h10);
        cpu_write(16'hA192, 8'h20);
        cpu_write(16'hA193, 8'h81);
        cpu_write(16'hA194, 8'h00);
        cpu_write(16'hA195, 8'h30);
        cpu_write(16'hA196, 8'h20);
        cpu_write(16'hA197, 8'h81);
        cpu_write(16'hA198, 8'h00);
        cpu_write(16'hA199, 8'h00);
        cpu_write(16'hA19A, 8'h01);
        cpu_write(16'hA19B, 8'h00);
        cpu_write(16'hA19C, 8'h00);
        cpu_write(16'hA19D, 8'h03);
        cpu_read(16'hA18B, d);
        check_eq8("sample start high byte readback", d, 8'h81);
        cpu_read(16'hA19D, d);
        check_eq8("sample mode/loop flags readback", d, 8'h03);

        cpu_write(16'hA141, 8'd127);
        cpu_write(16'hA143, 8'hFF);
        cpu_write(16'hA144, 8'h80);
        cpu_write(16'hA140, 8'd60);
        repeat(100000) begin
            @(posedge clk);
            if (abs18(audio_l) > max_l) max_l = abs18(audio_l);
            if (abs18(audio_r) > max_r) max_r = abs18(audio_r);
        end
        check("sample-mode WTS requested SDRAM", saw_sdram_read === 1'b1);
        check("sample-mode WTS keeps voice active through loop", dut.active[0] === 1'b1);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #10000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
