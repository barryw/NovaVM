// sid_stubs.sv — Arty bring-up stubs for the SID audio path.
//
// The real SID (rtl/sid/*, rtl/vendor/redip_sid/*) uses SystemVerilog packages
// + `default_nettype none` that the reDIP code relies on yosys/verilator to
// accept; Vivado's stricter elaboration rejects package types in the module
// port headers. SID/audio is not needed to boot EhBASIC and render video, so
// for the Arty CPU bring-up we substitute silent stubs with the SAME port
// interfaces as rtl/sid/sid_chip.sv and rtl/sid/sid_curve_reader.sv.
//
// TODO: port reDIP SID to Vivado (package-typed ports) to restore real audio.

`default_nettype none

module sid_chip (
    input  wire        clk,
    input  wire        rst,
    input  wire        ce_1m,
    input  wire        mode,
    input  wire        cs,
    input  wire        we,
    input  wire [4:0]  addr,
    input  wire [7:0]  din,
    output wire [7:0]  dout,
    output wire signed [17:0] audio_out,
    output wire [10:0] filter_fc_out,
    input  wire [15:0] filter_f0_in
);
    // Silent / read-as-zero. Inputs intentionally unused.
    wire _unused = &{1'b0, clk, rst, ce_1m, mode, cs, we, addr, din, filter_f0_in};
    assign dout          = 8'h00;
    assign audio_out     = 18'sd0;
    assign filter_fc_out = 11'd0;
endmodule

module sid_curve_reader #(
    parameter [24:0] CURVE_BASE = 25'h0_8_00_00
) (
    input  wire        clk,
    input  wire        rst,
    input  wire [10:0] sid1_Fc,
    output wire [15:0] sid1_f0,
    input  wire [10:0] sid2_Fc,
    output wire [15:0] sid2_f0,
    output wire [24:0] sdram_addrB,
    output wire        sdram_weB,
    output wire [7:0]  sdram_dinB,
    output wire        sdram_oeB,
    input  wire [7:0]  sdram_doutB
);
    wire _unused = &{1'b0, clk, rst, sid1_Fc, sid2_Fc, sdram_doutB, CURVE_BASE};
    // No SDRAM access; report zero filter curve.
    assign sid1_f0     = 16'd0;
    assign sid2_f0     = 16'd0;
    assign sdram_addrB = 25'd0;
    assign sdram_weB   = 1'b0;
    assign sdram_dinB  = 8'd0;
    assign sdram_oeB   = 1'b0;
endmodule

`default_nettype wire
