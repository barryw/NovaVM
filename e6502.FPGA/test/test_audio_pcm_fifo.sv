// Host PCM FIFO regression.
//
// Verifies the byte protocol used by NovaHost:
//   Llo, Lhi, Rlo, Rhi
// and checks that underruns produce silence instead of stale samples.

`timescale 1ns/1ps

module test_audio_pcm_fifo;
    logic clk = 1'b0;
    always #10 clk = ~clk;

    logic rst;
    logic byte_we;
    logic [7:0] byte_data;
    wire byte_ready;
    logic sample_en;
    wire [1:0][15:0] sample_word;
    wire sample_valid;
    wire [15:0] byte_space;
    wire [15:0] underrun_count;

    audio_pcm_fifo #(
        .FRAME_ADDR_WIDTH(2)
    ) dut (
        .clk(clk),
        .rst(rst),
        .byte_we(byte_we),
        .byte_data(byte_data),
        .byte_ready(byte_ready),
        .sample_en(sample_en),
        .sample_word(sample_word),
        .sample_valid(sample_valid),
        .byte_space(byte_space),
        .underrun_count(underrun_count)
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

    task automatic reset_dut();
        rst = 1'b1;
        byte_we = 1'b0;
        byte_data = 8'h00;
        sample_en = 1'b0;
        repeat(4) @(posedge clk);
        rst = 1'b0;
        repeat(2) @(posedge clk);
    endtask

    task automatic push_byte(input logic [7:0] b);
        while (!byte_ready) @(posedge clk);
        byte_data <= b;
        byte_we <= 1'b1;
        @(posedge clk);
        byte_we <= 1'b0;
    endtask

    task automatic pop_frame();
        sample_en <= 1'b1;
        @(posedge clk);
        sample_en <= 1'b0;
    endtask

    task automatic test_frame_order();
        $display("");
        $display("Test: little-endian stereo frame order");
        reset_dut();

        push_byte(8'h34);
        push_byte(8'h12);
        push_byte(8'hCD);
        push_byte(8'hAB);
        pop_frame();

        check("sample valid after one full frame", sample_valid);
        check("left word is 0x1234", sample_word[0] == 16'h1234);
        check("right word is 0xABCD", sample_word[1] == 16'hABCD);
        check("space restored after pop", byte_space == 16'd16);
    endtask

    task automatic test_underrun_silence();
        $display("");
        $display("Test: underrun emits silence and increments counter");
        reset_dut();

        pop_frame();
        check("sample invalid on underrun", !sample_valid);
        check("left silence on underrun", sample_word[0] == 16'h0000);
        check("right silence on underrun", sample_word[1] == 16'h0000);
        check("underrun count increments", underrun_count == 16'd1);
    endtask

    task automatic test_partial_frame_not_popped();
        $display("");
        $display("Test: partial frames are not emitted");
        reset_dut();

        push_byte(8'h78);
        push_byte(8'h56);
        pop_frame();
        check("partial frame does not become valid", !sample_valid);
        check("partial-frame underrun counted", underrun_count == 16'd1);

        push_byte(8'h11);
        push_byte(8'h22);
        pop_frame();
        check("completed partial frame becomes valid", sample_valid);
        check("completed left word", sample_word[0] == 16'h5678);
        check("completed right word", sample_word[1] == 16'h2211);
    endtask

    task automatic test_backlog_uses_memory_path();
        $display("");
        $display("Test: queued backlog frame survives BRAM path");
        reset_dut();

        push_byte(8'h01);
        push_byte(8'h02);
        push_byte(8'h03);
        push_byte(8'h04);
        push_byte(8'h11);
        push_byte(8'h12);
        push_byte(8'h13);
        push_byte(8'h14);

        pop_frame();
        check("first queued frame valid", sample_valid);
        check("first queued left word", sample_word[0] == 16'h0201);
        check("first queued right word", sample_word[1] == 16'h0403);

        repeat(3) @(posedge clk);
        pop_frame();
        check("second queued frame valid after prefetch", sample_valid);
        check("second queued left word", sample_word[0] == 16'h1211);
        check("second queued right word", sample_word[1] == 16'h1413);
    endtask

    initial begin
        $display("=== host PCM FIFO tests ===");
        test_frame_order();
        test_underrun_silence();
        test_partial_frame_not_popped();
        test_backlog_uses_memory_path();

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

    initial begin
        #2000000;
        $display("FAIL: global timeout");
        $finish(1);
    end
endmodule
