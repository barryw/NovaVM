/* naudio.c — NovaVM audio subsystem (Linux userspace port of ps_fio/src/audio.c).
 *
 * Faithful C-to-C port of the bare-metal PS audio engine. See naudio.h for the
 * exposed API. The only substantive changes from the bare-metal source:
 *
 *   - register access:  Xil_Out32(reg)/Xil_In32(reg)  ->  wr(off,v)/rd(off)
 *                       (offsets R_AUDIO/R_AUDIO_SPACE/R_AUDIO_EVT/R_SID_VOL from
 *                       novavm.h; reg = bare-metal absolute = FIO_BASE + offset).
 *   - file I/O:         FatFs f_open/f_read/f_size  ->  stdio fopen/fread/ftell.
 *   - timing:           the 1 kHz TTC0 GIC ISR that drove audio_service()  ->
 *                       a pthread feeder on a steady cadence (clock_nanosleep),
 *                       modelled on the audio_feed() silence thread in novavm.c,
 *                       plus audio_poll() for a poll-driven service loop.
 *   - atomic swaps:     Xil_ExceptionDisable/Enable (mask the audio ISR) ->
 *                       a pthread mutex held across the engine/song swap and the
 *                       render so MIDPLAY/SFLOAD never tear a render in flight.
 *
 * As in the bare-metal engine, SID audio is produced by the reDIP-SID in the PL
 * and mixed into the HDMI stream there; this module renders only the wavetable /
 * MIDI path to R_AUDIO. The software SIDs (sid.c) are reset here and kept as the
 * register/mirror state, matching ps_fio audio_init().
 */
#include "naudio.h"
#include "novavm.h"          /* wr/rd/poke + R_AUDIO/R_AUDIO_SPACE/R_AUDIO_EVT/R_SID_VOL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

/* Software MOS 6581 (sid.c is plain C, no Xilinx deps). Compile it straight into
 * this TU so the audio engine stays a single self-contained module. */
#include "../../ps_fio/src/sid.h"
#include "../../ps_fio/src/sid.c"

/* TinySoundFont (SF2 wavetable) + TinyMIDILoader, header-only. Memory-loaded
 * only (TSF_NO_STDIO), so no FatFs/stdio dependency leaks in here. */
#define TSF_NO_STDIO
#define TSF_IMPLEMENTATION
#include "../../ps_fio/src/thirdparty/tsf.h"
#define TML_IMPLEMENTATION
#include "../../ps_fio/src/thirdparty/tml.h"

#define SAMPLE_RATE  48000
#define BLOCK_FRAMES 256                 /* render granularity (stereo frames)        */
#define LIM_THRESH   0.95f               /* peak ceiling (just under full scale)       */
#define LIM_RELEASE  0.0006f             /* limiter release (~35 ms recovery)          */

static tsf*         g_tsf  = 0;
static tml_message* g_head = 0;          /* full message list (kept for tml_free) */
static tml_message* g_cur  = 0;          /* playback cursor                       */
static double       g_msec = 0.0;        /* playback position (ms)                */
static int          g_playing  = 0;
static int          g_testtone = 0;
static unsigned     g_tone_phase = 0;
static float        g_gain = 2.5f;       /* runtime linear master gain (pre-limiter) */
static float        g_lim  = 1.0f;       /* limiter gain-reduction envelope          */
static short        g_block[BLOCK_FRAMES * 2];
static float        g_fblock[BLOCK_FRAMES * 2];
static unsigned char g_note_active[128]; /* count of sounding voices per MIDI note */
static unsigned     g_total_frames = 0;  /* song length in 60 Hz frames            */

/* ---- 6502 register-driven sources: 2 software SIDs + the wavetable (via TSF) --- */
static sidchip      g_sid1, g_sid2;        /* $D400 / $D420 (mirror; PL renders SID) */
static int          g_regs_active = 0;     /* set once the 6502 touches any SID/WTS reg */
static float        g_vol_sid1 = 0.4f, g_vol_sid2 = 0.4f, g_vol_wts = 1.0f;
/* WTS voice state (8 voices -> TSF channels 0-7): current note + latched vel/bend. */
static unsigned char g_wts_note[8];
static unsigned char g_wts_vel[8];
static unsigned char g_wts_bendlo[8];

/* Guards engine/song swaps (SFLOAD/MIDPLAY) against an in-flight render. Replaces
 * the bare-metal Xil_ExceptionDisable/Enable around g_tsf / g_head reassignment. */
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK()   pthread_mutex_lock(&g_lock)
#define UNLOCK() pthread_mutex_unlock(&g_lock)

/* ---- PL register helpers (were non-static wrappers in ps_fio main.c) ----------
 * Bare-metal used absolute addresses (FIO_BASE + 0x30 etc.); here the wr/rd from
 * novavm.h take the offset, so we pass R_AUDIO/R_AUDIO_SPACE/... directly. */
volatile int g_naudio_mute = 0;   /* OSD pause: feed silence to the HDMI audio FIFO */
void naudio_set_mute(int on) { g_naudio_mute = on ? 1 : 0; }
static inline void     audio_fifo_write_bytes(const unsigned char *buf, int n) {
    for (int i = 0; i < n; i++) wr(R_AUDIO, g_naudio_mute ? 0 : buf[i]);
}
static inline int      audio_fifo_space(void) { return (int)(rd(R_AUDIO_SPACE) & 0xFFFFu); }
static inline unsigned audio_evt_read(void)   { return rd(R_AUDIO_EVT); }
static inline void     audio_mmio_poke(unsigned addr, unsigned char v) { poke(addr, v); }

/* reDIP-SID mix control (R_SID_VOL): [7:0] = level (32=x1, 64=x2), [8] = stereo. */
static unsigned g_sid_lvl = 128, g_sid_stereo = 0;   /* x2 with the DC blocker at PCM_GAIN=1 */
static void sid_vol_write(void) {
    wr(R_SID_VOL, ((g_sid_stereo & 1u) << 8) | (g_sid_lvl & 0xFFu));
}
void audio_set_sid_vol(unsigned v)   { g_sid_lvl = v & 0xFFu; sid_vol_write(); }
void audio_set_sid_stereo(int on)    { g_sid_stereo = on ? 1u : 0u; sid_vol_write(); }

/* =========================================================================== */

void audio_init(void) {
    sid_reset(&g_sid1);
    sid_reset(&g_sid2);
    for (int i = 0; i < 8; i++) { g_wts_note[i] = 0; g_wts_vel[i] = 100; g_wts_bendlo[i] = 0; }
    sid_vol_write();                       /* publish the default reDIP-SID mix level */
    printf("[audio] software SIDs ready ($D400/$D420)\n");
}

void audio_set_mix(float sid1, float sid2, float wts) {
    g_vol_sid1 = sid1; g_vol_sid2 = sid2; g_vol_wts = wts;
    printf("[audio] mix sid1=%d%% sid2=%d%% wts=%d%%\n",
           (int)(sid1 * 100), (int)(sid2 * 100), (int)(wts * 100));
}

/* Apply one captured wavetable register write ($A140-$A1DF) to TSF. */
static void wts_apply(int off, unsigned char data) {
    if (!g_tsf) return;
    if (off < 64) {                        /* per-voice: 8 voices x 8 regs */
        int v = off >> 3, r = off & 7;
        switch (r) {
        case 0:                            /* NOTE: >0 note-on, 0 note-off */
            if (g_wts_note[v]) tsf_channel_note_off(g_tsf, v, g_wts_note[v]);
            if (data) { tsf_channel_note_on(g_tsf, v, data, g_wts_vel[v] / 127.0f); g_wts_note[v] = data; }
            else      g_wts_note[v] = 0;
            break;
        case 1: g_wts_vel[v] = data; break;                                  /* VELOCITY (latch) */
        case 2: tsf_channel_set_presetnumber(g_tsf, v, data, 0); break;      /* INSTRUMENT       */
        case 3: tsf_channel_midi_control(g_tsf, v, 7,  data >> 1); break;    /* VOLUME -> CC7    */
        case 4: tsf_channel_midi_control(g_tsf, v, 10, data >> 1); break;    /* PAN -> CC10      */
        case 5: g_wts_bendlo[v] = data; break;                              /* BEND lo (latch)  */
        case 6: tsf_channel_set_pitchwheel(g_tsf, v,
                    (((data << 8) | g_wts_bendlo[v]) >> 2)); break;          /* BEND hi -> 14-bit */
        default: break;                                                     /* 7 = status (R)    */
        }
    } else {                               /* global ($A180+) */
        switch (off) {
        case 66: g_vol_wts = data / 255.0f; break;                          /* $A182 master */
        case 69: if (data == 1)                                            /* $A185 all-notes-off */
                     for (int v = 0; v < 8; v++) { if (g_wts_note[v]) tsf_channel_note_off(g_tsf, v, g_wts_note[v]); g_wts_note[v] = 0; }
                 break;
        default: break;                    /* reverb/chorus ($A180/$A181): TODO effects port */
        }
    }
}

/* Drain the PL capture FIFO: apply every queued 6502 SID/WTS register write.
 * SID writes (idx<64) reach the reDIP-SID in the fabric directly; only the
 * wavetable ($A140, idx>=64) is applied here (-> TSF). */
static void audio_drain_events(void) {
    for (int guard = 0; guard < 512; guard++) {
        unsigned e = audio_evt_read();
        if (!(e & 0x10000)) break;         /* bit16 = valid; empty -> done */
        int idx = (e >> 8) & 0xFF;
        unsigned char data = e & 0xFF;
        if (idx >= 64) { wts_apply(idx - 64, data); g_regs_active = 1; }
    }
}

/* $BA50 music-status block (the keyboard visualizer reads it). */
#define MUSIC_STATUS   0xBA50            /* bit1=music, bit3=WTS, bit4=loading */
#define MUSIC_NOTE1    0xBA51            /* 14 per-voice current-note bytes (0=silent) */
#define MUSIC_ELAPSEDL 0xBA5F
#define MUSIC_TOTALL   0xBA61

/* Publish playback state + up to 14 active notes into the $BA50 block so the
 * 6502 keyboard visualizer lights up to the live MIDI. */
static void audio_publish_status(void) {
    audio_mmio_poke(MUSIC_STATUS, g_playing ? (0x02 | 0x08) : 0x00);  /* MUSIC | WTS */
    int slot = 0;
    for (int n = 0; n < 128 && slot < 8; n++)
        if (g_note_active[n]) audio_mmio_poke(MUSIC_NOTE1 + slot++, (unsigned char)n);
    while (slot < 14) audio_mmio_poke(MUSIC_NOTE1 + slot++, 0);
    unsigned el = (unsigned)(g_msec / 16.6667);
    if (el > g_total_frames) el = g_total_frames;   /* never report past the end */
    audio_mmio_poke(MUSIC_ELAPSEDL,     el & 0xFF);
    audio_mmio_poke(MUSIC_ELAPSEDL + 1, (el >> 8) & 0xFF);
    audio_mmio_poke(MUSIC_TOTALL,       g_total_frames & 0xFF);
    audio_mmio_poke(MUSIC_TOTALL + 1,   (g_total_frames >> 8) & 0xFF);
}

void audio_set_gain(float g) {
    g_gain = g;                           /* applied pre-limiter (see render_limit) */
    printf("[audio] gain = %d%%\n", (int)(g * 100.0f));
}

/* Master output level at the SID+WTS -> HDMI mix point: scales the PL SID mix
 * (R_SID_VOL) and the WTS software gain together. pct 0..200, 100% = shipping
 * defaults (SID x4, WTS gain 2.5). Independent of the 6502's $D418/CC7. */
void audio_set_master(int pct) {
    if (pct < 0)   pct = 0;
    if (pct > 200) pct = 200;
    unsigned sid = (unsigned)pct * 128u / 100u;     /* 100% -> 128 (x4) */
    if (sid > 255u) sid = 255u;
    audio_set_sid_vol(sid);
    audio_set_gain((float)pct * 2.5f / 100.0f);     /* 100% -> 2.5 */
    printf("[audio] master = %d%%\n", pct);
}

/* Master gain + soft peak limiter (fast attack / slow release) on the float mix,
 * then convert to int16. */
static void render_limit(const float *in, short *out, int frames) {
    for (int i = 0; i < frames; i++) {
        float l = in[2 * i]     * g_gain;
        float r = in[2 * i + 1] * g_gain;
        float al = l < 0 ? -l : l, ar = r < 0 ? -r : r;
        float peak = al > ar ? al : ar;
        float tgt = (peak > LIM_THRESH) ? (LIM_THRESH / peak) : 1.0f;
        if (tgt < g_lim) g_lim = tgt;                       /* instant attack */
        else             g_lim += (1.0f - g_lim) * LIM_RELEASE;   /* slow release */
        l *= g_lim; r *= g_lim;
        if (l >  1.0f) l =  1.0f; else if (l < -1.0f) l = -1.0f;  /* safety */
        if (r >  1.0f) r =  1.0f; else if (r < -1.0f) r = -1.0f;
        out[2 * i]     = (short)(l * 32767.0f);
        out[2 * i + 1] = (short)(r * 32767.0f);
    }
}

/* Read an entire file into a malloc'd buffer (*len set). Returns buf or 0.
 * Linux replacement for the bare-metal FatFs slurp(). */
static void *slurp(const char *path, unsigned *len_out) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return 0; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return 0; }
    void *buf = malloc((size_t)sz);
    if (!buf) { printf("[audio] malloc %ld failed\n", sz); fclose(f); return 0; }
    size_t br = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (br != (size_t)sz) { free(buf); return 0; }
    *len_out = (unsigned)sz;
    return buf;
}

int audio_load_soundfont(const char *path) {
    unsigned len = 0;
    void *buf = slurp(path, &len);        /* file I/O runs lock-free (audio keeps playing) */
    if (!buf) { printf("[audio] SF2 read failed: %s\n", path); return -1; }
    tsf *nt = tsf_load_memory(buf, (int)len);
    free(buf);                            /* TSF keeps its own copy of what it needs */
    if (!nt) { printf("[audio] tsf_load_memory failed\n"); return -2; }
    tsf_channel_set_bank_preset(nt, 9, 128, 0);          /* GM drum kit on ch 10 */
    tsf_set_output(nt, TSF_STEREO_INTERLEAVED, SAMPLE_RATE, 0.0f);
    tsf_set_max_voices(nt, 128);          /* preallocate the voice pool (no realloc at render) */
    LOCK();
    tsf *old = g_tsf; g_tsf = nt; g_lim = 1.0f;
    UNLOCK();
    if (old) tsf_close(old);
    printf("[audio] SF2 loaded: %d presets (%u bytes)\n", tsf_get_presetcount(nt), len);
    return 0;
}

/* Scan a MIDI file for a meta event (FF <type> <len> <text>) and copy its text to
 * dst (zero-filled, 33B). */
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
        return;                            /* dst is pre-zeroed */
    }
}

int audio_play_midi(const char *path) {
    if (!g_tsf) { printf("[audio] no soundfont loaded\n"); return -1; }
    unsigned len = 0;
    void *buf = slurp(path, &len);        /* lock-free file I/O */
    if (!buf) { printf("[audio] MIDI read failed: %s\n", path); return -2; }
    tml_message *nh = tml_load_memory(buf, (int)len);
    if (!nh) { free(buf); printf("[audio] tml_load_memory failed\n"); return -3; }
    char title[33] = {0}, author[33] = {0}, copyr[33] = {0};
    midi_find_meta((const unsigned char *)buf, len, 0x03, title,  sizeof title);
    midi_find_meta((const unsigned char *)buf, len, 0x01, author, sizeof author);
    midi_find_meta((const unsigned char *)buf, len, 0x02, copyr,  sizeof copyr);
    free(buf);
    if (!title[0]) {                       /* no embedded name -> use the filename */
        const char *base = path;
        for (const char *p = path; *p; p++) if (*p == '/' || *p == ':') base = p + 1;
        int k = 0; while (base[k] && k < 32) { title[k] = base[k]; k++; }
    }
    unsigned int len_ms = 0;
    tml_get_info(nh, 0, 0, 0, 0, &len_ms);
    unsigned int end_ms = len_ms;
    for (tml_message *m = nh; m; m = m->next) if (m->time > end_ms) end_ms = m->time;
    LOCK();
    tml_message *old = g_head;
    g_head = nh; g_cur = nh; g_msec = 0.0;
    g_total_frames = (unsigned)(end_ms / 16.6667);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    tsf_reset(g_tsf);
    g_testtone = 0; g_playing = 1;
    UNLOCK();
    if (old) tml_free(old);
    audio_mmio_poke(0xBAB0, 0x03);             /* AUDIO_META_TYPE = MIDI */
    for (int i = 0; i < 32; i++) {
        audio_mmio_poke(0xBAB3 + i, (unsigned char)title[i]);   /* AUDIO_META_TITLE     */
        audio_mmio_poke(0xBAD3 + i, (unsigned char)author[i]);  /* AUDIO_META_AUTHOR    */
        audio_mmio_poke(0xBAF3 + i, (unsigned char)copyr[i]);   /* AUDIO_META_COPYRIGHT */
    }
    audio_mmio_poke(0xBB1C, 0);                /* AUDIO_META_FLAGS (no SID flags) */
    printf("[audio] MIDI playing: %s (\"%s\")\n", path, title);
    return 0;
}

void audio_stop(void) {
    LOCK();
    g_playing = 0; g_testtone = 0;
    g_regs_active = 0;
    if (g_tsf) tsf_reset(g_tsf);
    for (int i = 0; i < 128; i++) g_note_active[i] = 0;
    UNLOCK();
    audio_publish_status();
}

int audio_is_playing(void) { return g_playing || g_testtone; }

void audio_test_tone(int on) {
    LOCK();
    g_testtone = on ? 1 : 0;
    g_playing  = 0;
    g_tone_phase = 0;
    UNLOCK();
    printf("[audio] test tone %s\n", on ? "ON" : "OFF");
}

/* Fire all MIDI events due by 'msec' into TSF. */
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
                tsf_channel_note_off(g_tsf, ch, (unsigned char)g_cur->key);    /* vel 0 = note off */
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
    /* 440 Hz sine, ~half scale. phase in turns * 2^32. */
    const unsigned inc = (unsigned)((440.0 / SAMPLE_RATE) * 4294967296.0);
    for (int i = 0; i < frames; i++) {
        float s = sinf((float)g_tone_phase / 4294967296.0f * 6.2831853f);
        short v = (short)(s * 12000.0f);
        out[i * 2] = v; out[i * 2 + 1] = v;
        g_tone_phase += inc;
    }
}

/* Max stereo blocks rendered per audio_service() call. The bare-metal engine ran
 * this in the 1 kHz TTC0 ISR and capped at 1 block to keep the ISR short. On
 * Linux the feeder thread isn't a hard-real-time ISR, so we let it top the FIFO
 * up toward full per cadence tick (still bounded by FIFO space + a guard). */
#define SERVICE_MAX_BLOCKS 8

/* One service pass: drain queued register writes, then render/feed PCM toward a
 * full HDMI FIFO. Faithful port of ps_fio audio_service(). g_lock is held by the
 * caller so SFLOAD/MIDPLAY can't swap g_tsf/g_head mid-render. */
static void audio_service_locked(void) {
    audio_drain_events();                  /* apply queued 6502 SID/WTS writes (cheap when idle) */

    if (!g_playing && !g_testtone && !g_regs_active) {
        /* Nothing live: keep the HDMI audio islands valid with silence so strict
         * sinks (Elgato capture) don't reject an underrunning stream. This is the
         * silence-feeder behaviour from novavm.c's audio_feed(), folded in here. */
        int space = audio_fifo_space();
        for (int i = 0; i < space; i++) wr(R_AUDIO, 0);
        return;
    }

    int guard = 0;
    for (;;) {
        if ((audio_fifo_space() >> 2) < BLOCK_FRAMES) break;   /* FIFO full enough */
        if (++guard > SERVICE_MAX_BLOCKS) break;               /* keep the pass bounded */
        audio_drain_events();              /* apply writes at block granularity */

        if (g_testtone) {
            render_test_tone(g_block, BLOCK_FRAMES);
            audio_fifo_write_bytes((const unsigned char *)g_block, BLOCK_FRAMES * 4);
            continue;
        }

        /* Wavetable + MIDI file -> g_fblock (stereo). g_tsf may be null (SID-only). */
        if (g_playing) {
            g_msec += (double)BLOCK_FRAMES * 1000.0 / (double)SAMPLE_RATE;
            fire_events(g_msec);
            if (!g_cur) {                  /* song finished -> stop now so the visualizer exits */
                g_playing = 0;
                for (int i = 0; i < 128; i++) g_note_active[i] = 0;
                if (g_tsf) tsf_reset(g_tsf);
                audio_publish_status();    /* status -> 0 (cleared) for the visualizer */
            }
        }
        if (g_tsf) tsf_render_float(g_tsf, g_fblock, BLOCK_FRAMES, 0);
        else       memset(g_fblock, 0, sizeof g_fblock);

        /* SID audio comes from the PL reDIP-SID; here we render the wavetable/MIDI
         * only. Apply WTS volume. */
        if (g_vol_wts != 1.0f)
            for (int i = 0; i < BLOCK_FRAMES * 2; i++) g_fblock[i] *= g_vol_wts;

        render_limit(g_fblock, g_block, BLOCK_FRAMES);   /* master gain + peak limiter -> int16 */
        audio_fifo_write_bytes((const unsigned char *)g_block, BLOCK_FRAMES * 4);

        if (g_playing && !g_cur && tsf_active_voice_count(g_tsf) == 0) {  /* MIDI file done */
            g_playing = 0;
            for (int i = 0; i < 128; i++) g_note_active[i] = 0;
            audio_publish_status();
            printf("[audio] MIDI playback complete\n");
        }
    }
    if (g_playing) audio_publish_status();   /* live notes + elapsed for the visualizer */
}

/* Poll entry point: the novavm service loop may call this each iteration instead
 * of (or alongside) the feeder thread. */
void audio_poll(void) {
    LOCK();
    audio_service_locked();
    UNLOCK();
}

/* Feeder thread: drains events + tops up the HDMI FIFO on a steady ~1 ms cadence,
 * the Linux stand-in for the bare-metal 1 kHz TTC0 audio ISR. Modelled on the
 * audio_feed() silence thread in novavm.c (which this replaces). */
static void *audio_feeder(void *arg) {
    (void)arg;
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    for (;;) {
        audio_poll();
        next.tv_nsec += 1000000;           /* 1 ms cadence */
        if (next.tv_nsec >= 1000000000) { next.tv_nsec -= 1000000000; next.tv_sec++; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}

void audio_start(void) {
    pthread_t t;
    pthread_create(&t, NULL, audio_feeder, NULL);
    pthread_detach(t);
    printf("[audio] feeder thread started (48 kHz stereo -> HDMI audio FIFO)\n");
}

/* =========================================================================== */
/* FIO command handlers — mirror ps_fio main.c fio_volume/midplay/sfload.       */

/* The 6502 puts the file basename in the FIO name buffer ($B9B0, length $B9A3).
 * Read it (trimmed), like novavm.c's fio_read_name(), without depending on its
 * static. */
static int audio_fio_read_name(char *out, int maxlen) {
    int n = peek(FIO_NAMELEN);
    if (n < 1 || n > 63 || n >= maxlen) return -1;
    for (int i = 0; i < n; i++) out[i] = (char)peek(FIO_NAME + i);
    out[n] = 0;
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == 0)) out[--n] = 0;
    return n;
}

/* Bare-metal built "0:/<name>.<ext>" on the SD card; on Linux the media files
 * live under /data/nova/soundfonts. Append the extension if the name has none. */
static void audio_path(const char *name, const char *ext, char *path, int sz) {
    if (strrchr(name, '.')) snprintf(path, sz, "%s/soundfonts/%s",     NOVA_FS_ROOT, name);
    else                    snprintf(path, sz, "%s/soundfonts/%s.%s",  NOVA_FS_ROOT, name, ext);
}

void fio_volume(void) {
    unsigned v = peek(FIO_SRC_LO);         /* 0-255 -> 0.0..4.0 linear gain */
    audio_set_gain((float)v * 4.0f / 255.0f);
    fio_ok();
}

void fio_midplay(void) {
    char name[80], path[160];
    if (audio_fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    audio_path(name, "mid", path, sizeof path);
    if (audio_play_midi(path) == 0) fio_ok(); else fio_fail(FIO_ERR_NOTFOUND);
}

void fio_midstop(void) {
    audio_stop();
    fio_ok();
}

void fio_sfload(void) {
    char name[80], path[160];
    if (audio_fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    audio_path(name, "sf2", path, sizeof path);
    if (audio_load_soundfont(path) == 0) fio_ok(); else fio_fail(FIO_ERR_IO);
}
