// Global synthesis defines for the Arty full build. Added to the project as a
// GLOBAL INCLUDE (set_property is_global_include true), so they are visible to
// ALL sources regardless of compile order. hdl-util-hdmi's serializer/
// tmds_channel select the Xilinx OSERDESE2 path on `SYNTHESIS, and the VGC/HDMI
// use the 720x480 timing on `VIDEO_720X480. (The STEPS MORE_OPTIONS /
// fileset verilog_define mechanisms were flaky on freshly-recreated projects.)
`define SYNTHESIS 1
`define VIDEO_720X480 1
