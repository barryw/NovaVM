// audio.h — PS audio engine: TinySoundFont (SF2 wavetable) + TinyMIDILoader,
// rendered to 48 kHz / 16-bit stereo and streamed into the HDMI audio FIFO
// (fio_bridge R_AUDIO). The PL embeds the stream into HDMI audio data islands.
//
// SID + per-source-volume mixing layer onto this (the final software mix is what
// gets streamed). For now this drives the wavetable/MIDI path (Stars & Stripes).
#ifndef AUDIO_H
#define AUDIO_H

int  audio_load_soundfont(const char *sd_path);   // load .sf2 into TSF. 0 ok, <0 err
int  audio_play_midi(const char *sd_path);         // start .mid playback. 0 ok, <0 err
void audio_stop(void);
void audio_service(void);                          // call frequently from the main loop
int  audio_is_playing(void);
void audio_test_tone(int on);                      // 440 Hz sine into the FIFO (P1b proof)
void audio_set_gain(float g);                      // runtime linear master gain (1.0 = unity)

// Implemented in main.c (owns the fio_bridge register base):
void audio_fifo_write(const unsigned char *buf, int n);   // push PCM bytes to R_AUDIO
int  audio_fifo_space(void);                              // free bytes in the HDMI FIFO

#endif
