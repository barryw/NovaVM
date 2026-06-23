// Verifies the VGC dbg_vmem WRITE port lands pixels in the gfx dpram -- the
// exact mechanism the Arty boot splash relies on (PS -> fio_bridge 0x2C ->
// dbg_vmem_we -> gfx). The ULX3S debug_bridge drives this same port; this test
// proves the gfx-space write+readback path before committing to a bitstream.
`timescale 1ns/1ps

module test_vgc_dbg_gfx_write;

    `include "vgc_tb.svh"

    // One dbg_vmem write into gfx space (space=3), addr `a`, low-nibble value `v`.
    task automatic dbg_gfx_write(input logic [16:0] a, input logic [3:0] v);
        @(posedge clk);
        dbg_vmem_space <= VPLANE_GFX_A[2:0];   // 3'd3
        dbg_vmem_addr  <= a;
        dbg_vmem_wdata <= {4'h0, v};
        dbg_vmem_we    <= 1'b1;
        @(posedge clk);
        dbg_vmem_we    <= 1'b0;
        step(2);
    endtask

    initial begin
        do_reset();
        dbg_vmem_we = 0; dbg_vmem_re = 0; dbg_vmem_space = 0;
        dbg_vmem_addr = 0; dbg_vmem_wdata = 0;
        step(10);

        $display("");
        $display("Test: dbg_vmem write into gfx space (boot-splash path)");

        dbg_gfx_write(17'd0,     4'hA);   // first pixel
        dbg_gfx_write(17'd1,     4'h5);   // next pixel (no auto-inc: explicit addr)
        dbg_gfx_write(17'd320,   4'hF);   // row 1, col 0
        dbg_gfx_write(17'd32000, 4'h3);   // row 100, col 0 (mid-image)
        dbg_gfx_write(17'd63999, 4'h7);   // last pixel

        check_eq("gfx[0]     written",  int'(dut.gfx_inst.gfx_mem.mem[0]),     'hA);
        check_eq("gfx[1]     written",  int'(dut.gfx_inst.gfx_mem.mem[1]),     'h5);
        check_eq("gfx[320]   written",  int'(dut.gfx_inst.gfx_mem.mem[320]),   'hF);
        check_eq("gfx[32000] written",  int'(dut.gfx_inst.gfx_mem.mem[32000]), 'h3);
        check_eq("gfx[63999] written",  int'(dut.gfx_inst.gfx_mem.mem[63999]), 'h7);
        // An untouched cell must stay clear (no stray writes).
        check_eq("gfx[2]     untouched",int'(dut.gfx_inst.gfx_mem.mem[2]),     'h0);

        summary();
        $finish;
    end

endmodule
