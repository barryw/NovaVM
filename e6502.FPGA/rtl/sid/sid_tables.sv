// SID combined waveform tables and filter frequency calculation.
// Adapted from MiSTer C64 core (MiSTer-devel/C64_MiSTer).
// Filter curves from reDIP-SID by Dag Lem (CERN-OHL-S v2).
// Simplified: removed dynamic filter loading, single filter curve.

module sid_tables
(
	input             clock,
	input             mode,

	// waves
	input      [11:0] acc_t,
	output reg  [7:0] _st_out,
	output reg  [7:0] p_t_out,
	output reg  [7:0] ps__out,
	output reg  [7:0] pst_out,

	// filter
	input      [10:0] Fc,
	output     [15:0] F0
);

// P + T
always @(posedge clock) begin
	p_t_out <= mode ? wave8580_p_t[acc_t[10:0]] : wave6581_p_t[acc_t[10:0]];
end

// P + S
always @(posedge clock) begin
	ps__out <= mode ? wave8580_ps_[acc_t] : wave6581_ps_[acc_t[10:0]];
end

// S + T
always @(posedge clock) begin
	_st_out <= mode ? {
		((acc_t & 'he7e) == 'he7e) | ((acc_t & 'he80) == 'he80) | ((acc_t & 'hf00) == 'hf00) | ((acc_t & 'he7d) == 'he7d),
		((acc_t & 'h7f8) == 'h7f8) | ((acc_t & 'hf00) == 'hf00),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'hf0f) == 'hf0f) | ((acc_t & 'hf1b) == 'hf1b) | ((acc_t & 'hbfe) == 'hbfe) | ((acc_t & 'hf1e) == 'hf1e) | ((acc_t & 'hf40) == 'hf40) | ((acc_t & 'hf30) == 'hf30) | ((acc_t & 'hf29) == 'hf29) | ((acc_t & 'hf26) == 'hf26) | ((acc_t & 'hf80) == 'hf80),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'h3f0) == 'h3f0) | ((acc_t & 'hdfe) == 'hdfe) | ((acc_t & 'h5ff) == 'h5ff) | ((acc_t & 'hf80) == 'hf80),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'h3f0) == 'h3f0) | ((acc_t & 'hfc0) == 'hfc0) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'heff) == 'heff),
		((acc_t & 'h0fc) == 'h0fc) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'h3f0) == 'h3f0) | ((acc_t & 'hfe0) == 'hfe0),
		((acc_t & 'h07e) == 'h07e) | ((acc_t & 'hff0) == 'hff0) | ((acc_t & 'h7f7) == 'h7f7) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'h0fc) == 'h0fc),
		((acc_t & 'hdbf) == 'hdbf) | ((acc_t & 'h0fc) == 'h0fc) | ((acc_t & 'h3fa) == 'h3fa) | ((acc_t & 'h7f8) == 'h7f8) | ((acc_t & 'h3bf) == 'h3bf) | ((acc_t & 'h07e) == 'h07e)
	} : {
		1'b0,
		((acc_t & 'h7fc) == 'h7fc),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'h3fe) == 'h3fe),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'h5ff) == 'h5ff) | ((acc_t & 'h3f0) == 'h3f0),
		((acc_t & 'h7e0) == 'h7e0) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'h3f0) == 'h3f0),
		((acc_t & 'h0fc) == 'h0fc) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'h3f0) == 'h3f0),
		((acc_t & 'h07e) == 'h07e) | ((acc_t & 'h1f8) == 'h1f8) | ((acc_t & 'h0fc) == 'h0fc),
		((acc_t & 'h13f) == 'h13f) | ((acc_t & 'h07e) == 'h07e) | ((acc_t & 'h7fa) == 'h7fa) | ((acc_t & 'h0bf) == 'h0bf) | ((acc_t & 'h0fc) == 'h0fc)
	};
end

// P + S + T
always @(posedge clock) begin
	pst_out <= mode ? {
		((acc_t & 'he89) == 'he89) | ((acc_t & 'he3e) == 'he3e) | ((acc_t & 'hec0) == 'hec0) | ((acc_t & 'he8a) == 'he8a) | ((acc_t & 'hdf7) == 'hdf7) | ((acc_t & 'hdf8) == 'hdf8) | ((acc_t & 'he85) == 'he85) | ((acc_t & 'he6a) == 'he6a) | ((acc_t & 'he90) == 'he90) | ((acc_t & 'he83) == 'he83) | ((acc_t & 'he67) == 'he67) | ((acc_t & 'hea0) == 'hea0) | ((acc_t & 'hf00) == 'hf00) | ((acc_t & 'he5e) == 'he5e) | ((acc_t & 'he70) == 'he70) | ((acc_t & 'he6c) == 'he6c),
		((acc_t & 'heee) == 'heee) | ((acc_t & 'h7ef) == 'h7ef) | ((acc_t & 'h7f2) == 'h7f2) | ((acc_t & 'h7f4) == 'h7f4) | ((acc_t & 'hef0) == 'hef0) | ((acc_t & 'h7f8) == 'h7f8) | ((acc_t & 'hf00) == 'hf00) | ((acc_t & 'h7f1) == 'h7f1),
		((acc_t & 'hf78) == 'hf78) | ((acc_t & 'h7f0) == 'h7f0) | ((acc_t & 'h7ee) == 'h7ee) | ((acc_t & 'hf74) == 'hf74) | ((acc_t & 'hf6f) == 'hf6f) | ((acc_t & 'hf80) == 'hf80) | ((acc_t & 'hbff) == 'hbff),
		((acc_t & 'hdff) == 'hdff) | ((acc_t & 'hbfe) == 'hbfe) | ((acc_t & 'h7ef) == 'h7ef) | ((acc_t & 'h7f2) == 'h7f2) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7f4) == 'h7f4) | ((acc_t & 'hfc0) == 'hfc0) | ((acc_t & 'hfb8) == 'hfb8) | ((acc_t & 'h7f8) == 'h7f8) | ((acc_t & 'hfb6) == 'hfb6),
		((acc_t & 'hbfe) == 'hbfe) | ((acc_t & 'hfdc) == 'hfdc) | ((acc_t & 'hdfe) == 'hdfe) | ((acc_t & 'h7f7) == 'h7f7) | ((acc_t & 'hfda) == 'hfda) | ((acc_t & 'hbfd) == 'hbfd) | ((acc_t & 'h7f8) == 'h7f8) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'hfe0) == 'hfe0) | ((acc_t & 'heff) == 'heff),
		((acc_t & 'hfeb) == 'hfeb) | ((acc_t & 'h7fa) == 'h7fa) | ((acc_t & 'hbfe) == 'hbfe) | ((acc_t & 'hdfe) == 'hdfe) | ((acc_t & 'hff0) == 'hff0) | ((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'hfec) == 'hfec) | ((acc_t & 'heff) == 'heff),
		((acc_t & 'hff6) == 'hff6) | ((acc_t & 'hdff) == 'hdff) | ((acc_t & 'hf7f) == 'hf7f) | ((acc_t & 'hbfe) == 'hbfe) | ((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'hff5) == 'hff5) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'hff8) == 'hff8) | ((acc_t & 'heff) == 'heff),
		((acc_t & 'hdff) == 'hdff) | ((acc_t & 'hf7f) == 'hf7f) | ((acc_t & 'hffa) == 'hffa) | ((acc_t & 'h7fe) == 'h7fe) | ((acc_t & 'hff9) == 'hff9) | ((acc_t & 'hffc) == 'hffc) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'heff) == 'heff)
	} : {
		1'b0,
		((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'h7fb) == 'h7fb),
		((acc_t & 'h7ef) == 'h7ef) | ((acc_t & 'h7f7) == 'h7f7) | ((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'h7fb) == 'h7fb) | ((acc_t & 'h3ff) == 'h3ff),
		((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7f7) == 'h7f7) | ((acc_t & 'h7fb) == 'h7fb),
		((acc_t & 'h7fc) == 'h7fc) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7fb) == 'h7fb),
		((acc_t & 'h7fd) == 'h7fd) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7fe) == 'h7fe),
		((acc_t & 'h7fd) == 'h7fd) | ((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7fe) == 'h7fe),
		((acc_t & 'h3ff) == 'h3ff) | ((acc_t & 'h7fe) == 'h7fe)
	};
end

reg [7:0] wave6581_p_t[0:2047];
initial $readmemh("rom/wave6581_p_t.hex", wave6581_p_t);

reg [7:0] wave6581_ps_[0:2047];
initial $readmemh("rom/wave6581_ps_.hex", wave6581_ps_);

reg [7:0] wave8580_p_t[0:2047];
initial $readmemh("rom/wave8580_p_t.hex", wave8580_p_t);

reg [7:0] wave8580_ps_[0:4095];
initial $readmemh("rom/wave8580_ps_.hex", wave8580_ps_);

/////////////////////////////////////////////////////////////////////////////////////

reg [15:0] f0;
always @(posedge clock) f0 <= f6581_curve[Fc[10:1]];

// No Fc_offset adjustment — use default curve for 6581, linear for 8580
assign F0 = mode ? ({ 3'b000, Fc, 2'b00 } + Fc) : {1'b0, f0[15:1]};

// value = pi * 1.048576 * f0[fc]
reg [15:0] f6581_curve[0:4095];
initial $readmemh("rom/f6581_curve.hex", f6581_curve);

// f6581_adj table removed — not needed without Fc_offset adjustment
/* verilator lint_off UNUSED */
reg [14:0] _unused_f6581_adj[0:1023];
initial $readmemh("rom/_unused_f6581_adj.hex", _unused_f6581_adj);
/* verilator lint_on UNUSED */

endmodule
