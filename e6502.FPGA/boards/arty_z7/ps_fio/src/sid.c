// sid.c — MOS 6581 software emulation (see sid.h). Faithful port of SidChip.cs.
#include "sid.h"
#include <string.h>
#include <math.h>

#pragma GCC optimize ("O3")            // hot per-sample synthesis (runs in the audio path)

#define SID_SAMPLE_RATE 48000.0f
#define SID_CPU_CLOCK   985248.0f      // PAL C64

#define ST_ATTACK  0
#define ST_DECAY   1
#define ST_RELEASE 2

static const unsigned short AdsrRateTable[16] =
    { 9, 32, 63, 95, 149, 220, 267, 313, 392, 977, 1954, 3126, 3907, 11720, 19532, 31251 };
static const unsigned char SustainLevels[16] =
    { 0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF };
static const unsigned char ExpTargetTable[93] = {
    1,30,30,30,30,30,16,16,16,16,16,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
};

static inline int imin(int a, int b) { return a < b ? a : b; }

static void chan_reset(sid_channel *c) {
    c->Frequency = 0; c->Ad = 0; c->Sr = 0; c->Pulse = 0; c->Waveform = 0;
    c->VoiceVolume = 0x0F; c->DoSync = 0; c->state = ST_RELEASE;
    c->accumulator = 0; c->noiseGenerator = 0x7FFFF8;
    c->adsrCounter = 0; c->adsrExpCounter = 0; c->volumeLevel = 0;
}

void sid_reset(sidchip *s) {
    memset(s->regs, 0, sizeof s->regs);
    s->regs[0x1D] = 0x0F; s->regs[0x1E] = 0x0F; s->regs[0x1F] = 0x0F;
    for (int i = 0; i < 3; i++) chan_reset(&s->ch[i]);
    s->prevBandPass = 0; s->prevLowPass = 0; s->cycleAccumulator = 0;
    s->cyclesPerSample = SID_CPU_CLOCK / SID_SAMPLE_RATE;
}

void sid_write(sidchip *s, int reg, unsigned char data) {
    if (reg >= 0 && reg < 32) s->regs[reg] = data;
}

// Advance one channel by `cycles` CPU clocks. target/source are the sync ring
// neighbours (target = ch[(i+1)%3], source = ch[(i+2)%3]).
static void chan_clock(sid_channel *c, sid_channel *target, sid_channel *source, int cycles) {
    if (c->Waveform & 0x1) { if (c->state == ST_RELEASE) c->state = ST_ATTACK; }
    else                     c->state = ST_RELEASE;

    int adsrCycles = cycles;
    while (adsrCycles > 0) {
        unsigned short adsrTarget =
            (c->state == ST_ATTACK) ? AdsrRateTable[c->Ad >> 4] :
            (c->state == ST_DECAY)  ? AdsrRateTable[c->Ad & 0xF] :
                                      AdsrRateTable[c->Sr & 0xF];
        int adsrCyclesNow = imin(adsrCycles,
            c->adsrCounter < adsrTarget ? (adsrTarget - c->adsrCounter)
                                        : (0x8000 + adsrTarget - c->adsrCounter));
        c->adsrCounter = (unsigned short)((c->adsrCounter + adsrCyclesNow) & 0x7FFF);
        if (c->adsrCounter == adsrTarget) {
            c->adsrCounter = 0;
            if (c->state == ST_ATTACK) {
                c->adsrExpCounter = 0;
                if (++c->volumeLevel == 0xFF) c->state = ST_DECAY;
            } else if (c->state == ST_DECAY) {
                unsigned char tgt = c->volumeLevel < 0x5D ? ExpTargetTable[c->volumeLevel] : 1;
                if (++c->adsrExpCounter >= tgt) {
                    c->adsrExpCounter = 0;
                    if (c->volumeLevel > SustainLevels[c->Sr >> 4]) --c->volumeLevel;
                }
            } else { // release
                if (c->volumeLevel > 0) {
                    unsigned char tgt = c->volumeLevel < 0x5D ? ExpTargetTable[c->volumeLevel] : 1;
                    if (++c->adsrExpCounter >= tgt) { c->adsrExpCounter = 0; --c->volumeLevel; }
                }
            }
        }
        adsrCycles -= adsrCyclesNow;
    }

    if (c->Waveform & 0x8) { c->accumulator = 0; return; }   // test bit
    if (c->Frequency == 0) return;

    if ((c->Waveform & 0x80) == 0 && (target->Waveform & 0x2) == 0) {
        c->accumulator = (c->accumulator + c->Frequency * (unsigned)cycles) & 0xFFFFFF;
    } else {
        int accCycles = cycles;
        while (accCycles > 0) {
            int now = accCycles;
            if (c->Waveform & 0x80) {
                if ((c->accumulator & 0xFFFFF) < 0x80000)
                    now = imin(now, (int)(0x80000 - (c->accumulator & 0xFFFFF)) / c->Frequency + 1);
                else
                    now = imin(now, (int)(0x180000 - (c->accumulator & 0xFFFFF)) / c->Frequency + 1);
            }
            if (target->Waveform & 0x2) {
                if (c->accumulator < 0x800000)
                    now = imin(now, (int)(0x800000 - c->accumulator) / c->Frequency + 1);
                else
                    now = imin(now, (int)(0x1800000 - c->accumulator) / c->Frequency + 1);
            }
            unsigned last = c->accumulator;
            c->accumulator = (c->accumulator + c->Frequency * (unsigned)now) & 0xFFFFFF;
            if ((c->Waveform & 0x80) && (last & 0x80000) == 0 && (c->accumulator & 0x80000)) {
                unsigned t = c->noiseGenerator;
                unsigned step = (t & 0x400000) ^ ((t & 0x20000) << 5);
                t <<= 1; if (step) t |= 1;
                c->noiseGenerator = t & 0x7FFFFF;
            }
            c->DoSync = (last & 0x800000) == 0 && (c->accumulator & 0x800000) != 0;
            accCycles -= now;
        }
    }
    (void)source;
}

static inline unsigned chan_triangle(sid_channel *c, sid_channel *source) {
    unsigned t = c->accumulator ^ ((c->Waveform & 0x4) ? source->accumulator : 0);
    return ((t >= 0x800000 ? (c->accumulator ^ 0xFFFFFF) : c->accumulator) >> 7) & 0xFFFF;
}
static inline unsigned chan_saw(sid_channel *c)   { return c->accumulator >> 8; }
static inline unsigned chan_pulse(sid_channel *c) { return ((c->accumulator >> 12) >= (c->Pulse & 0xFFF)) ? 0xFFFF : 0; }
static inline unsigned chan_noise(sid_channel *c) {
    unsigned n = c->noiseGenerator;
    return ((n & 0x100000) >> 5) + ((n & 0x40000) >> 4) + ((n & 0x4000) >> 1) +
           ((n & 0x800) << 1) + ((n & 0x200) << 2) + ((n & 0x20) << 5) +
           ((n & 0x04) << 7) + ((n & 0x01) << 8);
}

static float chan_output(sid_channel *c, sid_channel *source) {
    if (c->volumeLevel == 0) return 0.0f;
    unsigned w = 0;
    switch (c->Waveform & 0xF0) {
        case 0x10: w = chan_triangle(c, source); break;
        case 0x20: w = chan_saw(c); break;
        case 0x40: w = chan_pulse(c); break;
        case 0x50: { unsigned tri = chan_triangle(c, source), pul = chan_pulse(c);
                     w = ((pul & tri & (tri >> 1)) & (tri << 1)) << 1; if (w > 0xFFFF) w = 0xFFFF; break; }
        case 0x60: { unsigned saw = chan_saw(c), pul = chan_pulse(c);
                     w = ((pul & saw & (saw >> 1)) & (saw << 1)) << 1; if (w > 0xFFFF) w = 0xFFFF; break; }
        case 0x70: { unsigned ts = chan_triangle(c, source) & chan_saw(c), pul = chan_pulse(c);
                     w = ((pul & ts & (ts >> 1)) & (ts << 1)) << 1; if (w > 0xFFFF) w = 0xFFFF; break; }
        case 0x80: w = chan_noise(c); break;
    }
    return ((int)w - 0x8000) * c->volumeLevel / 16777216.0f;
}

void sid_render(sidchip *s, float *out_mono, int count) {
    // Latch registers into channel state.
    for (int i = 0; i < 3; i++) {
        int o = i * 7;
        s->ch[i].Frequency   = (unsigned short)(s->regs[o] | (s->regs[o + 1] << 8));
        s->ch[i].Pulse       = (unsigned short)(s->regs[o + 2] | (s->regs[o + 3] << 8));
        s->ch[i].Waveform    = s->regs[o + 4];
        s->ch[i].Ad          = s->regs[o + 5];
        s->ch[i].Sr          = s->regs[o + 6];
        s->ch[i].VoiceVolume = s->regs[0x1D + i];
    }
    float masterVol = (s->regs[0x18] & 0xF) / 22.5f;
    unsigned char filterSelect = s->regs[0x18] & 0x70;
    unsigned char filterCtrl   = s->regs[0x17];
    float cutoff = 0.05f + 0.85f * (sinf((s->regs[0x16] / 255.0f - 0.5f) * 3.14159265f) * 0.5f + 0.5f);
    cutoff = powf(cutoff, 1.3f);
    cutoff *= (44100.0f / SID_SAMPLE_RATE);   // SidChip.cs tunes the filter for 44.1 kHz;
                                              // rescale the cutoff coeff for our 48 kHz rate
    float resonance = (s->regs[0x17] > 0x3F) ? 7.0f / (s->regs[0x17] >> 4) : 1.75f;

    for (int i = 0; i < count; i++) {
        int cyclesToRun = (int)ceilf(s->cyclesPerSample - s->cycleAccumulator);
        if (cyclesToRun < 1) cyclesToRun = 1;
        for (int j = 0; j < 3; j++)
            chan_clock(&s->ch[j], &s->ch[(j + 1) % 3], &s->ch[(j + 2) % 3], cyclesToRun);
        for (int j = 0; j < 3; j++)
            if (s->ch[j].DoSync && (s->ch[(j + 1) % 3].Waveform & 0x2))
                s->ch[(j + 1) % 3].accumulator = 0;
        s->cycleAccumulator += cyclesToRun;
        if (s->cycleAccumulator >= s->cyclesPerSample) s->cycleAccumulator -= s->cyclesPerSample;

        // Mix + state-variable filter.
        float output = 0.0f, filterInput = 0.0f;
        for (int ch = 0; ch < 3; ch++) {
            float o = chan_output(&s->ch[ch], &s->ch[(ch + 2) % 3]) * ((s->ch[ch].VoiceVolume & 0xF) / 15.0f);
            if ((filterCtrl & (1 << ch)) == 0) output += o; else filterInput += o;
        }
        float temp = filterInput + s->prevBandPass * resonance + s->prevLowPass;
        if (filterSelect & 0x40) output -= temp;          // highpass
        temp = s->prevBandPass - temp * cutoff; s->prevBandPass = temp;
        if (filterSelect & 0x20) output -= temp;          // bandpass
        temp = s->prevLowPass + temp * cutoff; s->prevLowPass = temp;
        if (filterSelect & 0x10) output += temp;          // lowpass
        output *= masterVol;
        out_mono[i] = output;
    }
}
