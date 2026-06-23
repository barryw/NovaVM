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
#include "xil_exception.h"   // Xil_ExceptionDisable/Enable -- guard engine swaps
                             // against the high-priority audio TTC0 ISR
#include "sid.h"             // software MOS 6581 (2 instances, 6502 register-driven)
#include "sidplay.h"         // .sid file playback via the reused nova_sid::SidVm

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

// ---- 6502 register-driven sources: 2 software SIDs + the wavetable (via TSF) ---
static sidchip      g_sid1, g_sid2;        // $D400 / $D420
static float        g_sidbuf[BLOCK_FRAMES];// scratch mono SID render buffer
static int          g_regs_active = 0;     // set once the 6502 touches any SID/WTS reg
// SID output is already near full-scale; keep it below the master gain+limiter
// so loud bass/drums don't over-drive into buzz (tunable live via audio_mix).
static float        g_vol_sid1 = 0.4f, g_vol_sid2 = 0.4f, g_vol_wts = 1.0f;
// WTS voice state (8 voices -> TSF channels 0-7): current note + latched vel/bend.
static unsigned char g_wts_note[8];
static unsigned char g_wts_vel[8];
static unsigned char g_wts_bendlo[8];

void audio_init(void) {
    sid_reset(&g_sid1);
    sid_reset(&g_sid2);
    for (int i = 0; i < 8; i++) { g_wts_note[i] = 0; g_wts_vel[i] = 100; g_wts_bendlo[i] = 0; }
    xil_printf("[audio] software SIDs ready ($D400/$D420)\r\n");
}

void audio_set_mix(float sid1, float sid2, float wts) {
    g_vol_sid1 = sid1; g_vol_sid2 = sid2; g_vol_wts = wts;
    xil_printf("[audio] mix sid1=%d%% sid2=%d%% wts=%d%%\r\n",
               (int)(sid1 * 100), (int)(sid2 * 100), (int)(wts * 100));
}

// Apply one captured wavetable register write ($A140-$A1DF) to TSF.
static void wts_apply(int off, unsigned char data) {
    if (!g_tsf) return;
    if (off < 64) {                       // per-voice: 8 voices x 8 regs
        int v = off >> 3, r = off & 7;
        switch (r) {
        case 0:                            // NOTE: >0 note-on, 0 note-off (monophonic/voice)
            if (g_wts_note[v]) tsf_channel_note_off(g_tsf, v, g_wts_note[v]);
            if (data) { tsf_channel_note_on(g_tsf, v, data, g_wts_vel[v] / 127.0f); g_wts_note[v] = data; }
            else      g_wts_note[v] = 0;
            break;
        case 1: g_wts_vel[v] = data; break;                                  // VELOCITY (latch)
        case 2: tsf_channel_set_presetnumber(g_tsf, v, data, 0); break;      // INSTRUMENT
        case 3: tsf_channel_midi_control(g_tsf, v, 7,  data >> 1); break;    // VOLUME -> CC7
        case 4: tsf_channel_midi_control(g_tsf, v, 10, data >> 1); break;    // PAN -> CC10
        case 5: g_wts_bendlo[v] = data; break;                              // BEND lo (latch)
        case 6: tsf_channel_set_pitchwheel(g_tsf, v,
                    (((data << 8) | g_wts_bendlo[v]) >> 2)); break;          // BEND hi -> apply 14-bit
        default: break;                                                     // 7 = status (R)
        }
    } else {                              // global ($A180+)
        switch (off) {
        case 66: g_vol_wts = data / 255.0f; break;                          // $A182 master
        case 69: if (data == 1)                                            // $A185 command: all-notes-off
                     for (int v = 0; v < 8; v++) { if (g_wts_note[v]) tsf_channel_note_off(g_tsf, v, g_wts_note[v]); g_wts_note[v] = 0; }
                 break;
        default: break;                    // reverb/chorus ($A180/$A181): TODO effects port
        }
    }
}

static void *slurp(const char *path, UINT *len_out);   // defined below

// Sink for SID register writes from the .sid player (sidplay.cpp). Pokes the real
// reDIP-SID in the fabric ($D400/$D420) via the bridge -> faithful, VICE-grade
// audio mixed in the PL. (The live 6502's own $D400 writes reach the RTL SID
// directly; only the sandboxed .sid emulator needs this sink.)
void audio_sid_reg_write(int chip, int reg, unsigned char val) {
    audio_mmio_poke((chip ? 0xD420u : 0xD400u) + (unsigned)(reg & 0x1F), val);
    g_regs_active = 1;
}

// Load + start a .sid file (the tune runs sandboxed in nova_sid::SidVm; its SID
// writes mirror into our software SID). buf may be freed after this returns.
int audio_play_sidfile(const char *sd_path) {
    UINT len = 0;
    void *buf = slurp(sd_path, &len);
    if (!buf) { xil_printf("[audio] SID read failed: %s\r\n", sd_path); return -1; }
    int rc = sidplay_load((const unsigned char *)buf, (int)len, 0);
    free(buf);
    if (rc == 0) g_regs_active = 1;
    return rc;
}

// Drain the PL capture FIFO: apply every queued 6502 SID/WTS register write.
static void audio_drain_events(void) {
    for (int guard = 0; guard < 512; guard++) {
        unsigned e = audio_evt_read();
        if (!(e & 0x10000)) break;         // bit16 = valid; empty -> done
        int idx = (e >> 8) & 0xFF;
        unsigned char data = e & 0xFF;
        // SID writes (idx<64) go straight to the real reDIP-SID in the fabric;
        // only the wavetable ($A140, idx>=64) is applied here (-> TSF).
        if (idx >= 64) { wts_apply(idx - 64, data); g_regs_active = 1; }
    }
}

// $BA50 music-status block (the keyboard visualizer reads it).
#define MUSIC_STATUS   0xBA50            // bit1=music, bit3=WTS, bit4=loading
#define MUSIC_NOTE1    0xBA51            // 14 per-voice current-note bytes (0=silent)
#define MUSIC_ELAPSEDL 0xBA5F
#define MUSIC_TOTALL   0xBA61

// Publish playback state + up to 14 active notes into the $BA50 block so the
// 6502 keyboard visualizer lights up to the live MIDI.
static void audio_publish_status(void) {
    audio_mmio_poke(MUSIC_STATUS, g_playing ? (0x02 | 0x08) : 0x00);  // MUSIC | WTS
    // SID or MIDI plays alone; the visualizer reads MIDI on voices 0-7. Publish the
    // active MIDI notes into slots 0-7.
    int slot = 0;
    for (int n = 0; n < 128 && slot < 8; n++)
        if (g_note_active[n]) audio_mmio_poke(MUSIC_NOTE1 + slot++, (unsigned char)n);
    while (slot < 14) audio_mmio_poke(MUSIC_NOTE1 + slot++, 0);
    unsigned el = (unsigned)(g_msec / 16.6667);
    if (el > g_total_frames) el = g_total_frames;   // never report past the end
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
    UINT len = 0;
    void *buf = slurp(sd_path, &len);     // file I/O runs lock-free (audio keeps playing)
    if (!buf) { xil_printf("[audio] SF2 read failed: %s\r\n", sd_path); return -1; }
    tsf *nt = tsf_load_memory(buf, (int)len);
    free(buf);                            // TSF keeps its own copy of what it needs
    if (!nt) { xil_printf("[audio] tsf_load_memory failed\r\n"); return -2; }
    tsf_channel_set_bank_preset(nt, 9, 128, 0);          // GM drum kit on ch 10
    // Render at unity into float; master gain + limiter are applied per-block in
    // render_limit (so we can run hot for loudness without internal clipping).
    tsf_set_output(nt, TSF_STEREO_INTERLEAVED, SAMPLE_RATE, 0.0f);
    // Preallocate the voice pool so note-on (in the audio ISR) never reallocs at
    // render time -- keeps the ISR free of newlib malloc() (the main loop also
    // uses newlib malloc for loads, so they must never overlap).
    tsf_set_max_voices(nt, 128);
    // Swap in atomically w.r.t. the audio ISR (IRQs off for the brief swap).
    Xil_ExceptionDisable();
    tsf *old = g_tsf; g_tsf = nt; g_lim = 1.0f;
    Xil_ExceptionEnable();
    if (old) tsf_close(old);
    xil_printf("[audio] SF2 loaded: %d presets (%u bytes)\r\n",
               tsf_get_presetcount(nt), (unsigned)len);
    return 0;
}

// Scan a MIDI file for a meta event (FF <type> <len> <text>) and copy its text to
// dst (zero-filled, 33B). Heuristic + bounded: meta events sit near the track heads,
// and we require printable ASCII to reject false hits inside VLQ delta-times.
static void midi_find_meta(const unsigned char *buf, unsigned len, unsigned char type,
                           char *dst, int dstn) {
    unsigned scan = (len < 8192) ? len : 8192;
    for (unsigned i = 0; i + 3 < scan; i++) {
        if (buf[i] != 0xFF || buf[i + 1] != type) continue;
        unsigned n = buf[i + 2];
        if (n == 0 || n > 64 || i + 3 + n > len) continue;
        int ok = 1;
        for (unsigned j = 0; j < n; j++) {
            unsigned char c = buf[i + 3 + j];
            if (c < 0x20 || c > 0x7E) { ok = 0; break; }
        }
        if (!ok) continue;
        int k = 0;
        for (unsigned j = 0; j < n && k < dstn - 1; j++) dst[k++] = (char)buf[i + 3 + j];
        return;                                // dst is pre-zeroed
    }
}

int audio_play_midi(const char *sd_path) {
    if (!g_tsf) { xil_printf("[audio] no soundfont loaded\r\n"); return -1; }
    UINT len = 0;
    void *buf = slurp(sd_path, &len);     // lock-free file I/O
    if (!buf) { xil_printf("[audio] MIDI read failed: %s\r\n", sd_path); return -2; }
    tml_message *nh = tml_load_memory(buf, (int)len);
    if (!nh) { free(buf); xil_printf("[audio] tml_load_memory failed\r\n"); return -3; }
    // Metadata from the MIDI meta events (FF 03 sequence/track name = title, FF 01
    // text, FF 02 copyright). Parse before freeing buf; fall back to the filename.
    char title[33] = {0}, author[33] = {0}, copyr[33] = {0};
    midi_find_meta((const unsigned char *)buf, len, 0x03, title,  sizeof title);
    midi_find_meta((const unsigned char *)buf, len, 0x01, author, sizeof author);
    midi_find_meta((const unsigned char *)buf, len, 0x02, copyr,  sizeof copyr);
    free(buf);
    if (!title[0]) {                       // no embedded name -> use the filename
        const char *base = sd_path;
        for (const char *p = sd_path; *p; p++) if (*p == '/' || *p == ':') base = p + 1;
        int k = 0; while (base[k] && k < 32) { title[k] = base[k]; k++; }
    }
    unsigned int len_ms = 0;
    tml_get_info(nh, 0, 0, 0, 0, &len_ms);
    // tml_get_info reports the last NOTE time, but playback runs to the last EVENT
    // (trailing note-offs / end-of-track). Use the last event time so the
    // visualizer's total matches actual play length.
    unsigned int end_ms = len_ms;
    for (tml_message *m = nh; m; m = m->next) if (m->time > end_ms) end_ms = m->time;
    // Switch playback to the new song atomically w.r.t. the audio ISR.
    Xil_ExceptionDisable();
    tml_message *old = g_head;
    g_head = nh; g_cur = nh; g_msec = 0.0;
    g_total_frames = (unsigned)(end_ms / 16.6667);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    tsf_reset(g_tsf);
    g_testtone = 0; g_playing = 1;
    Xil_ExceptionEnable();
    if (old) tml_free(old);
    // Publish music-block metadata for the visualizer header.
    audio_mmio_poke(0xBAB0, 0x03);             // AUDIO_META_TYPE = MIDI
    for (int i = 0; i < 32; i++) {
        audio_mmio_poke(0xBAB3 + i, (unsigned char)title[i]);   // AUDIO_META_TITLE
        audio_mmio_poke(0xBAD3 + i, (unsigned char)author[i]);  // AUDIO_META_AUTHOR
        audio_mmio_poke(0xBAF3 + i, (unsigned char)copyr[i]);   // AUDIO_META_COPYRIGHT
    }
    audio_mmio_poke(0xBB1C, 0);                // AUDIO_META_FLAGS (no SID flags)
    xil_printf("[audio] MIDI playing: %s (\"%s\")\r\n", sd_path, title);
    return 0;
}

void audio_stop(void) {
    Xil_ExceptionDisable();
    g_playing = 0; g_testtone = 0;
    sidplay_stop();        // stop the sandboxed .sid -- it kept bridge-poking $D4xx
    g_regs_active = 0;     // every tick, which blocked/overwrote live 6502 writes
    if (g_tsf) tsf_reset(g_tsf);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    Xil_ExceptionEnable();
    audio_publish_status();
}

int audio_is_playing(void) { return g_playing || g_testtone; }

void audio_test_tone(int on) {
    Xil_ExceptionDisable();
    g_testtone = on ? 1 : 0;
    g_playing  = 0;
    g_tone_phase = 0;
    Xil_ExceptionEnable();
    xil_printf("[audio] test tone %s\r\n", on ? "ON" : "OFF");
}

// Max stereo blocks rendered per audio_service() call. Runs in the 1 kHz TTC0
// ISR, so keep it short: 1 block (2 SIDs + TSF render) is ~0.8 ms worst case.
// Steady state renders ~1 block per 5 ticks (the FIFO stays near full).
#define ISR_MAX_BLOCKS 1

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
    audio_drain_events();                  // apply queued 6502 SID/WTS writes (cheap when idle)
    // Render whenever ANY source is live: MIDI file, test tone, or the 6502 has
    // touched a SID/WTS register (then the engine mixes continuously).
    if (!g_playing && !g_testtone && !g_regs_active) return;

    int guard = 0;
    for (;;) {
        if ((audio_fifo_space() >> 2) < BLOCK_FRAMES) break;   // FIFO full enough
        if (++guard > ISR_MAX_BLOCKS) break;                   // keep the ISR short
        audio_drain_events();              // apply writes at block granularity

        if (g_testtone) {
            render_test_tone(g_block, BLOCK_FRAMES);
            audio_fifo_write((const unsigned char *)g_block, BLOCK_FRAMES * 4);
            continue;
        }

        // .sid playback: run the tune's play routine(s) at its frame rate; the
        // emulated SID writes mirror into g_sid1/g_sid2 (rendered just below).
        if (sidplay_active()) sidplay_advance(BLOCK_FRAMES);

        // Wavetable + MIDI file -> g_fblock (stereo). g_tsf may be null (SID-only).
        if (g_playing) {
            g_msec += (double)BLOCK_FRAMES * 1000.0 / (double)SAMPLE_RATE;
            fire_events(g_msec);
            if (!g_cur) {                  // song finished -> stop now so the visualizer exits
                g_playing = 0;
                for (int i = 0; i < 128; i++) g_note_active[i] = 0;
                if (g_tsf) tsf_reset(g_tsf);
                audio_publish_status();    // status -> 0 (cleared) for the visualizer
            }
        }
        if (g_tsf) tsf_render_float(g_tsf, g_fblock, BLOCK_FRAMES, 0);
        else       memset(g_fblock, 0, sizeof g_fblock);

        // SID audio is produced by the real reDIP-SID in the PL and mixed into the
        // HDMI stream there; here we only render the wavetable/MIDI. Apply WTS vol.
        if (g_vol_wts != 1.0f)
            for (int i = 0; i < BLOCK_FRAMES * 2; i++) g_fblock[i] *= g_vol_wts;

        render_limit(g_fblock, g_block, BLOCK_FRAMES);   // master gain + peak limiter -> int16
        audio_fifo_write((const unsigned char *)g_block, BLOCK_FRAMES * 4);

        if (g_playing && !g_cur && tsf_active_voice_count(g_tsf) == 0) {  // MIDI file done
            g_playing = 0;
            for (int i = 0; i < 128; i++) g_note_active[i] = 0;
            audio_publish_status();
            xil_printf("[audio] MIDI playback complete\r\n");
        }
    }
    if (g_playing) audio_publish_status();   // live notes + elapsed for the visualizer
}
