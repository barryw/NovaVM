/* naudio.h — NovaVM audio subsystem (Linux userspace port of ps_fio/src/audio.c).
 *
 * Faithful C-to-C port of the bare-metal PS audio engine: two software MOS-6581
 * SIDs (sid.c) kept for the sandboxed .sid mirror + a TinySoundFont (SF2) /
 * TinyMIDILoader (MIDI) wavetable synth, rendered to 48 kHz / 16-bit stereo and
 * streamed into the PL HDMI audio FIFO (R_AUDIO) paced by R_AUDIO_SPACE.
 *
 * Linux adaptation: no GIC/TTC0 1 kHz ISR. Instead audio_start() spawns a
 * pthread feeder (model: the audio_feed() silence thread in novavm.c) that drains
 * the PL register-event FIFO (R_AUDIO_EVT) and tops up the HDMI FIFO on a steady
 * cadence. audio_poll() exposes the same work for a poll-driven service loop.
 *
 * novavm.c owns g_reg; this module reaches the PL via wr()/rd()/poke() from
 * novavm.h. Do NOT redefine those.
 */
#ifndef NAUDIO_H
#define NAUDIO_H

/* ---- lifecycle (novavm.c calls these) -------------------------------------- */
void audio_init(void);    /* reset software SID/synth state + SID mix level (call once at boot,
                             after the ROM is staged / CPU released, like ps_fio audio_init()) */
void audio_start(void);   /* spawn the feeder pthread (replaces audio_feed() in novavm.c) */
void audio_poll(void);    /* drain events + top up the HDMI FIFO once; optional poll model.
                             Safe to call from the main loop instead of / alongside the thread.
                             (The feeder thread already calls this internally each cadence tick.) */

/* ---- FIO command handlers (dispatch from the novavm.c FIO switch) ----------- */
void fio_volume(void);    /* FIO_CMD_VOLUME  (0x0C): master gain from FIO_SRC_LO (0-255 -> 0..4x) */
void fio_midplay(void);   /* FIO_CMD_MIDPLAY (0x13): play /data/nova/soundfonts/<name>.mid */
void fio_midstop(void);   /* FIO_CMD_MIDSTOP (0x14): stop playback                          */
void fio_sfload(void);    /* FIO_CMD_SFLOAD  (0x15): load /data/nova/soundfonts/<name>.sf2  */
void fio_sidplay(void);   /* FIO_CMD_SIDPLAY (0x08): play <name>.sid from the mounted NDI  */
void fio_sidstop(void);   /* FIO_CMD_SIDSTOP (0x09): stop SID playback                     */

/* ---- runtime tuning (optional; mirrors ps_fio audio.c public surface) ------- */
void audio_stop(void);
int  audio_is_playing(void);
void audio_set_gain(float g);                          /* linear master gain, 1.0 = unity */
void audio_set_master(int pct);                        /* master mix level 0..200% (SID+WTS) */
void audio_set_mix(float sid1, float sid2, float wts); /* per-source linear volumes        */
void audio_test_tone(int on);                          /* 440 Hz sine into the FIFO         */
int  audio_load_soundfont(const char *path);           /* load .sf2.  0 ok, <0 err          */
int  audio_play_midi(const char *path);                /* start .mid. 0 ok, <0 err          */

#endif /* NAUDIO_H */
