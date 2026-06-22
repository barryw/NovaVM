// audio.c — PS audio engine for the Arty: TinySoundFont (SF2 wavetable synth)
// + TinyMIDILoader, rendered to 48 kHz / 16-bit stereo and streamed into the
// HDMI audio FIFO via fio_bridge R_AUDIO (the PL embeds it in HDMI audio
// islands). Time advances by samples rendered, so playback is sample-accurate
// and self-paced by the FIFO's free space (audio_fifo_space()).

#include "audio.h"
#include "ff.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "xil_printf.h"

// The ps_fio app is built -O0 (-g3); TSF's per-sample float synthesis is far too
// slow unoptimized and the HDMI FIFO underruns (scratchy, lurching playback).
// Force -O3 for this translation unit so the TSF/TML render keeps real time.
#pragma GCC optimize ("O3")

#define TSF_NO_STDIO
#define TSF_IMPLEMENTATION
#include "thirdparty/tsf.h"
#define TML_IMPLEMENTATION
#include "thirdparty/tml.h"

#define SAMPLE_RATE  48000
#define BLOCK_FRAMES 256                 // render granularity (stereo frames)
#define LIM_THRESH   0.95f               // peak ceiling (just under full scale)
#define LIM_RELEASE  0.0006f             // limiter release (~35 ms recovery)

static tsf*         g_tsf  = 0;
static tml_message* g_head = 0;          // full message list (kept for tml_free)
static tml_message* g_cur  = 0;          // playback cursor
static double       g_msec = 0.0;        // playback position (ms)
static int          g_playing  = 0;
static int          g_testtone = 0;
static unsigned     g_tone_phase = 0;
static float        g_gain = 2.5f;       // runtime linear master gain (pre-limiter)
static float        g_lim  = 1.0f;        // limiter gain-reduction envelope
static short        g_block[BLOCK_FRAMES * 2];
static float        g_fblock[BLOCK_FRAMES * 2];
static unsigned char g_note_active[128];  // count of sounding voices per MIDI note
static unsigned     g_total_frames = 0;   // song length in 60 Hz frames

// $BA50 music-status block (the keyboard visualizer reads it).
#define MUSIC_STATUS   0xBA50            // bit1=music, bit3=WTS, bit4=loading
#define MUSIC_NOTE1    0xBA51            // 14 per-voice current-note bytes (0=silent)
#define MUSIC_ELAPSEDL 0xBA5F
#define MUSIC_TOTALL   0xBA61

// Publish playback state + up to 14 active notes into the $BA50 block so the
// 6502 keyboard visualizer lights up to the live MIDI.
static void audio_publish_status(void) {
    audio_mmio_poke(MUSIC_STATUS, g_playing ? (0x02 | 0x08) : 0x00);
    int slot = 0;
    for (int n = 0; n < 128 && slot < 14; n++)
        if (g_note_active[n]) audio_mmio_poke(MUSIC_NOTE1 + slot++, (unsigned char)n);
    while (slot < 14) audio_mmio_poke(MUSIC_NOTE1 + slot++, 0);
    unsigned el = (unsigned)(g_msec / 16.6667);
    audio_mmio_poke(MUSIC_ELAPSEDL,     el & 0xFF);
    audio_mmio_poke(MUSIC_ELAPSEDL + 1, (el >> 8) & 0xFF);
    audio_mmio_poke(MUSIC_TOTALL,       g_total_frames & 0xFF);
    audio_mmio_poke(MUSIC_TOTALL + 1,   (g_total_frames >> 8) & 0xFF);
}

void audio_set_gain(float g) {
    g_gain = g;                           // applied pre-limiter (see render_limit)
    xil_printf("[audio] gain = %d%%\r\n", (int)(g * 100.0f));
}

// Master gain + soft peak limiter (fast attack / slow release) on the float mix,
// then convert to int16. Lets the gain run hot for loudness while peaks (esp.
// bass-heavy passages) are shaved smoothly instead of hard-clipping.
static void render_limit(const float *in, short *out, int frames) {
    for (int i = 0; i < frames; i++) {
        float l = in[2 * i]     * g_gain;
        float r = in[2 * i + 1] * g_gain;
        float al = l < 0 ? -l : l, ar = r < 0 ? -r : r;
        float peak = al > ar ? al : ar;
        float tgt = (peak > LIM_THRESH) ? (LIM_THRESH / peak) : 1.0f;
        if (tgt < g_lim) g_lim = tgt;                       // instant attack
        else             g_lim += (1.0f - g_lim) * LIM_RELEASE;   // slow release
        l *= g_lim; r *= g_lim;
        if (l >  1.0f) l =  1.0f; else if (l < -1.0f) l = -1.0f;  // safety
        if (r >  1.0f) r =  1.0f; else if (r < -1.0f) r = -1.0f;
        out[2 * i]     = (short)(l * 32767.0f);
        out[2 * i + 1] = (short)(r * 32767.0f);
    }
}

// Read an entire SD file into a malloc'd buffer (*len set). Returns buf or 0.
static void *slurp(const char *path, UINT *len_out) {
    FIL f;
    if (f_open(&f, path, FA_READ) != FR_OK) return 0;
    FSIZE_t sz = f_size(&f);
    void *buf = malloc((size_t)sz);
    if (!buf) { xil_printf("[audio] malloc %u failed\r\n", (unsigned)sz); f_close(&f); return 0; }
    UINT off = 0, br = 0;
    while (off < sz) {
        UINT want = (sz - off > 0x100000u) ? 0x100000u : (UINT)(sz - off);
        if (f_read(&f, (char *)buf + off, want, &br) != FR_OK || br == 0) break;
        off += br;
    }
    f_close(&f);
    if (off != (UINT)sz) { free(buf); return 0; }
    *len_out = (UINT)sz;
    return buf;
}

int audio_load_soundfont(const char *sd_path) {
    if (g_tsf) { tsf_close(g_tsf); g_tsf = 0; }
    UINT len = 0;
    void *buf = slurp(sd_path, &len);
    if (!buf) { xil_printf("[audio] SF2 read failed: %s\r\n", sd_path); return -1; }
    g_tsf = tsf_load_memory(buf, (int)len);
    free(buf);                            // TSF keeps its own copy of what it needs
    if (!g_tsf) { xil_printf("[audio] tsf_load_memory failed\r\n"); return -2; }
    tsf_channel_set_bank_preset(g_tsf, 9, 128, 0);          // GM drum kit on ch 10
    // Render at unity into float; master gain + limiter are applied per-block in
    // render_limit (so we can run hot for loudness without internal clipping).
    tsf_set_output(g_tsf, TSF_STEREO_INTERLEAVED, SAMPLE_RATE, 0.0f);
    g_lim = 1.0f;
    xil_printf("[audio] SF2 loaded: %d presets (%u bytes)\r\n",
               tsf_get_presetcount(g_tsf), (unsigned)len);
    return 0;
}

int audio_play_midi(const char *sd_path) {
    if (!g_tsf) { xil_printf("[audio] no soundfont loaded\r\n"); return -1; }
    if (g_head) { tml_free(g_head); g_head = 0; }
    UINT len = 0;
    void *buf = slurp(sd_path, &len);
    if (!buf) { xil_printf("[audio] MIDI read failed: %s\r\n", sd_path); return -2; }
    g_head = tml_load_memory(buf, (int)len);
    free(buf);
    if (!g_head) { xil_printf("[audio] tml_load_memory failed\r\n"); return -3; }
    unsigned int len_ms = 0;
    tml_get_info(g_head, 0, 0, 0, 0, &len_ms);
    g_total_frames = (unsigned)(len_ms / 16.6667);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    tsf_reset(g_tsf);
    g_cur = g_head; g_msec = 0.0; g_testtone = 0; g_playing = 1;
    xil_printf("[audio] MIDI playing: %s\r\n", sd_path);
    return 0;
}

void audio_stop(void) {
    g_playing = 0; g_testtone = 0;
    if (g_tsf) tsf_reset(g_tsf);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    audio_publish_status();
}

int audio_is_playing(void) { return g_playing || g_testtone; }

void audio_test_tone(int on) {
    g_testtone = on ? 1 : 0;
    g_playing  = 0;
    g_tone_phase = 0;
    xil_printf("[audio] test tone %s\r\n", on ? "ON" : "OFF");
}

// Fire all MIDI events due by 'msec' into TSF.
static void fire_events(double msec) {
    for (; g_cur && (double)g_cur->time <= msec; g_cur = g_cur->next) {
        int ch = g_cur->channel;
        switch (g_cur->type) {
        case TML_PROGRAM_CHANGE:
            tsf_channel_set_presetnumber(g_tsf, ch, (unsigned char)g_cur->program, (ch == 9));
            break;
        case TML_NOTE_ON:
            if ((unsigned char)g_cur->velocity) {
                tsf_channel_note_on(g_tsf, ch, (unsigned char)g_cur->key,
                                    (unsigned char)g_cur->velocity / 127.0f);
                if ((unsigned char)g_cur->key < 128) g_note_active[(unsigned char)g_cur->key]++;
            } else {
                tsf_channel_note_off(g_tsf, ch, (unsigned char)g_cur->key);    // vel 0 = note off
                if ((unsigned char)g_cur->key < 128 && g_note_active[(unsigned char)g_cur->key])
                    g_note_active[(unsigned char)g_cur->key]--;
            }
            break;
        case TML_NOTE_OFF:
            tsf_channel_note_off(g_tsf, ch, (unsigned char)g_cur->key);
            if ((unsigned char)g_cur->key < 128 && g_note_active[(unsigned char)g_cur->key])
                g_note_active[(unsigned char)g_cur->key]--;
            break;
        case TML_PITCH_BEND:
            tsf_channel_set_pitchwheel(g_tsf, ch, g_cur->pitch_bend);
            break;
        case TML_CONTROL_CHANGE:
            tsf_channel_midi_control(g_tsf, ch, (unsigned char)g_cur->control,
                                     (unsigned char)g_cur->control_value);
            break;
        default: break;
        }
    }
}

static void render_test_tone(short *out, int frames) {
    // 440 Hz sine, ~half scale. phase in turns * 2^32.
    const unsigned inc = (unsigned)((440.0 / SAMPLE_RATE) * 4294967296.0);
    for (int i = 0; i < frames; i++) {
        float s = sinf((float)g_tone_phase / 4294967296.0f * 6.2831853f);
        short v = (short)(s * 12000.0f);
        out[i * 2] = v; out[i * 2 + 1] = v;
        g_tone_phase += inc;
    }
}

void audio_service(void) {
    if (!g_tsf && !g_testtone) return;
    if (!g_playing && !g_testtone) return;

    int guard = 0;
    for (;;) {
        if ((audio_fifo_space() >> 2) < BLOCK_FRAMES) break;   // wait for FIFO room
        if (++guard > 64) break;                               // don't hog the loop

        if (g_testtone) {
            render_test_tone(g_block, BLOCK_FRAMES);
        } else {
            g_msec += (double)BLOCK_FRAMES * 1000.0 / (double)SAMPLE_RATE;
            fire_events(g_msec);
            tsf_render_float(g_tsf, g_fblock, BLOCK_FRAMES, 0);
            render_limit(g_fblock, g_block, BLOCK_FRAMES);
            if (!g_cur && tsf_active_voice_count(g_tsf) == 0) {  // song + tails done
                audio_fifo_write((const unsigned char *)g_block, BLOCK_FRAMES * 4);
                g_playing = 0;
                for (int i = 0; i < 128; i++) g_note_active[i] = 0;
                audio_publish_status();
                xil_printf("[audio] MIDI playback complete\r\n");
                break;
            }
        }
        audio_fifo_write((const unsigned char *)g_block, BLOCK_FRAMES * 4);
    }
    if (g_playing) audio_publish_status();   // live notes + elapsed for the visualizer
}
