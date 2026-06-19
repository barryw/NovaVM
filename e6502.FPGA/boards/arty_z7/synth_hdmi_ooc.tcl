# OOC synth of hdl-util-hdmi with the Arty instantiation's generics baked in ->
# build/hdmi_ooc.dcp. Isolates hdmi/tmds_channel synth from the full design
# (which intermittently crashes Vivado synth on tmds_channel once fio_bridge is
# added). arty_z7_full instantiates `hdmi` WITHOUT params so it links this dcp.
set h ../../rtl/thirdparty/hdl-util-hdmi
read_verilog -sv [list \
  $h/hdmi.sv $h/tmds_channel.sv $h/serializer.sv \
  $h/packet_assembler.sv $h/packet_picker.sv \
  $h/audio_clock_regeneration_packet.sv $h/audio_info_frame.sv $h/audio_sample_packet.sv \
  $h/auxiliary_video_information_info_frame.sv $h/source_product_description_info_frame.sv ]
synth_design -mode out_of_context -top hdmi -part xc7z020clg400-1 \
  -verilog_define SYNTHESIS=1 -verilog_define VIDEO_720X480=1 \
  -generic VIDEO_ID_CODE=2 -generic DVI_OUTPUT=1 -generic IT_CONTENT=1 \
  -generic VIDEO_REFRESH_RATE_MILLIHZ=59940 -generic START_X=855 -generic START_Y=524
write_checkpoint -force build/hdmi_ooc.dcp
puts "=== HDMI OOC DONE ==="
exit
