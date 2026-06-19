// hdmi_bb.sv — black-box stub for the hdl-util-hdmi `hdmi` module.
//
// hdmi is synthesized out-of-context (synth_hdmi_ooc.tcl -> build/hdmi_ooc.dcp)
// because Vivado synth intermittently crashes on hdl-util-hdmi tmds_channel
// (Synth 8-6156, no cause) when the full design includes fio_bridge. This stub
// lets the top synth treat hdmi as a black box; the dcp (added used_in_synthesis
// false) supplies the netlist at implementation. Port widths match the OOC
// generics (VIDEO_ID_CODE=2 -> BIT_WIDTH=BIT_HEIGHT=10; AUDIO_BIT_WIDTH=16).

(* black_box *)
module hdmi (
    input  logic              clk_pixel_x5,
    input  logic              clk_pixel,
    input  logic              clk_audio,
    input  logic              reset,
    input  logic [23:0]       rgb,
    input  logic [1:0][15:0]  audio_sample_word,
    output logic [2:0]        tmds,
    output logic              tmds_clock,
    output logic [9:0]        cx,
    output logic [9:0]        cy,
    output logic [9:0]        frame_width,
    output logic [9:0]        frame_height,
    output logic [9:0]        screen_width,
    output logic [9:0]        screen_height
);
endmodule
