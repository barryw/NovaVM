//
// sdram.v
//
// sdram controller implementation for the MiST board adaptation
// of Luddes NES core
// http://code.google.com/p/mist-board/
// 
// Copyright (c) 2013 Till Harbaum <till@harbaum.org> 
// 
// This source file is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program.  If not, see <http://www.gnu.org/licenses/>. 
//

module sdram
(
	// interface to the MT48LC16M16 chip
        input     [15:0] sd_data_in,
        output    [15:0] sd_data_out,
	output    [12:0] sd_addr,    // 13 bit multiplexed address bus
	output     [1:0] sd_dqm,     // two byte masks
	output     [1:0] sd_ba,      // two banks
	output           sd_cs,      // a single chip select
	output           sd_we,      // write enable
	output           sd_ras,     // row address select
	output           sd_cas,     // columns address select

	// cpu/chipset interface
	input            init,       // init signal after FPGA config to initialize RAM
	input            clk,        // sdram is accessed at up to 128MHz
	input            clkref,     // reference clock to sync to
	output           we_out, // Tristate control signal
	
	input     [24:0] addrA,      // 25 bit byte address
	input            weA,        // cpu/chipset requests write
	input      [7:0] dinA,       // data input from chipset/cpu
	input            oeA,        // cpu requests data
	output reg [7:0] doutA,      // data output to cpu
	output reg       doneA,      // one clk pulse when port A transfer completed

	input     [24:0] addrB,      // 25 bit byte address
	input            weB,        // cpu/chipset requests write
	input      [7:0] dinB,       // data input from chipset/cpu
	input            oeB,        // ppu requests data
	output reg [7:0] doutB,      // data output to ppu
	output reg       doneB       // one clk pulse when port B transfer completed
);

// no burst configured
localparam RASCAS_DELAY   = 3'd2;   // tRCD=20ns -> 2 cycles@85MHz
localparam BURST_LENGTH   = 3'b000; // 000=1, 001=2, 010=4, 011=8
localparam ACCESS_TYPE    = 1'b0;   // 0=sequential, 1=interleaved
localparam CAS_LATENCY    = 3'd3;   // 2/3 allowed
localparam OP_MODE        = 2'b00;  // only 00 (standard operation) allowed
localparam NO_WRITE_BURST = 1'b1;   // 0= write burst enabled, 1=only single access write

localparam MODE = { 3'b000, NO_WRITE_BURST, OP_MODE, CAS_LATENCY, ACCESS_TYPE, BURST_LENGTH}; 


// ---------------------------------------------------------------------
// ------------------------ cycle state machine ------------------------
// ---------------------------------------------------------------------

localparam STATE_FIRST     = 3'd0;   // first state in cycle
localparam STATE_CMD_START = 3'd1;   // state in which a new command can be started
localparam STATE_CMD_CONT  = STATE_CMD_START + RASCAS_DELAY;      // 3 command can be continued
localparam STATE_CMD_READ  = STATE_CMD_CONT + CAS_LATENCY + 1'd1; // 6 read state
localparam STATE_LAST      = 3'd7;  // last state in cycle

reg clkref_last;
reg [2:0] q;
always @(posedge clk) begin
	// SDRAM (state machine) clock is 85MHz. Synchronize this to systems 21.477 Mhz clock
   // force counter to pass state LAST->FIRST exactly after the rising edge of clkref
   clkref_last <= clkref;

   q <= q + 1'd1;
   if (q==STATE_LAST) q<=STATE_FIRST;
   if (~clkref_last & clkref) q<=STATE_FIRST + 1'd1;

end

// ---------------------------------------------------------------------
// --------------------------- startup/reset ---------------------------
// ---------------------------------------------------------------------

// wait 1ms (85000 cycles) after FPGA config is done before going
// into normal operation. Initialize the ram in the last 16 reset cycles (cycles 15-0)
reg [16:0] reset;
always @(posedge clk) begin
	//if(init)	reset <= 17'h14c08;
	if(init)	reset <= 17'h1f;
	else if((q == STATE_LAST) && (reset != 0))
		reset <= reset - 17'd1;
end

// ---------------------------------------------------------------------
// ------------------ generate ram control signals ---------------------
// ---------------------------------------------------------------------

// all possible commands
localparam CMD_INHIBIT         = 4'b1111;
localparam CMD_NOP             = 4'b0111;
localparam CMD_ACTIVE          = 4'b0011;
localparam CMD_READ            = 4'b0101;
localparam CMD_WRITE           = 4'b0100;
localparam CMD_BURST_TERMINATE = 4'b0110;
localparam CMD_PRECHARGE       = 4'b0010;
localparam CMD_AUTO_REFRESH    = 4'b0001;
localparam CMD_LOAD_MODE       = 4'b0000;

wire [3:0] sd_cmd;   // current command sent to sd ram

// clkref high - port A
// clkref low  - port B
wire        req_we   = clkref ? weA   : weB;
wire        req_oe   = clkref ? oeA   : oeB;
wire [24:0] req_addr = clkref ? addrA : addrB;
wire  [7:0] req_din  = clkref ? dinA  : dinB;

reg        cycle_port_a;
reg        cycle_we;
reg        cycle_oe;
reg [24:0] cycle_addr;
reg  [7:0] cycle_din;

wire [7:0] dout = cycle_addr[0]?sd_data_in[7:0]:sd_data_in[15:8];
assign     we_out = (q == STATE_CMD_CONT) && cycle_we;

always @(posedge clk) begin
	doneA <= 1'b0;
	doneB <= 1'b0;
	if(init || (reset != 0)) begin
		cycle_port_a <= 1'b0;
		cycle_we     <= 1'b0;
		cycle_oe     <= 1'b0;
		cycle_addr   <= 25'd0;
		cycle_din    <= 8'd0;
	end else begin
		if(q == STATE_CMD_START) begin
			cycle_port_a <= clkref;
			cycle_we     <= req_we;
			cycle_oe     <= !req_we && req_oe;
			cycle_addr   <= req_addr;
			cycle_din    <= req_din;
		end
		if(q == STATE_CMD_READ && cycle_oe) begin
			if(cycle_port_a) begin
				doutA <= dout;
				doneA <= 1'b1;
			end
			else begin
				doutB <= dout;
				doneB <= 1'b1;
			end
			cycle_oe <= 1'b0;
		end
		if(q == STATE_CMD_CONT && cycle_we) begin
			if(cycle_port_a) doneA <= 1'b1;
			else             doneB <= 1'b1;
			cycle_we <= 1'b0;
		end
	end
end

wire [3:0] reset_cmd = 
	((q == STATE_CMD_START) && (reset == 13))?CMD_PRECHARGE:
	((q == STATE_CMD_START) && (reset ==  2))?CMD_LOAD_MODE:
	CMD_INHIBIT;

wire [3:0] run_cmd =
	((req_we || req_oe) && (q == STATE_CMD_START))?CMD_ACTIVE:
	( cycle_we          && (q == STATE_CMD_CONT ))?CMD_WRITE:
	( cycle_oe          && (q == STATE_CMD_CONT ))?CMD_READ:
	(!(req_we || req_oe) && (q == STATE_CMD_START))?CMD_AUTO_REFRESH:
	CMD_INHIBIT;

assign sd_cmd = (reset != 0)?reset_cmd:run_cmd;

wire [12:0] reset_addr = (reset == 13)?13'b0010000000000:MODE;

wire [12:0] run_addr =
	(q == STATE_CMD_START)?req_addr[21:9]:{ 4'b0010, cycle_addr[24], cycle_addr[8:1]};

assign sd_data_out = we_out?{ cycle_din, cycle_din }:16'b0;
//register SDRAM output signals
assign sd_addr = (reset != 0)?reset_addr:run_addr;

assign sd_ba = (reset != 0)?2'b00:
	(q == STATE_CMD_START)?req_addr[23:22]:cycle_addr[23:22];

assign sd_dqm = we_out?{ cycle_addr[0], ~cycle_addr[0] }:2'b00;

// drive control signals according to current command
assign sd_cs  = sd_cmd[3];
assign sd_ras = sd_cmd[2];
assign sd_cas = sd_cmd[1];
assign sd_we  = sd_cmd[0];

endmodule
