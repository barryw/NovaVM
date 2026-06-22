// sid.h — MOS 6581 SID software emulation for the Arty PS audio engine.
// Ported from e6502.Avalonia/Hardware/SidChip.cs (itself from cadaver's SID.cs,
// MIT; waveforms/filter from Hermit's jsSID). Renders mono float at 48 kHz.
// NovaBASIC extension: regs $1D-$1F = per-voice volume (0-15), default 15.
#ifndef SID_H
#define SID_H

typedef struct {
    unsigned short Frequency, Pulse;
    unsigned char  Ad, Sr, Waveform, VoiceVolume;
    int            DoSync;
    int            state;            // 0=Attack, 1=Decay, 2=Release
    unsigned int   accumulator;
    unsigned int   noiseGenerator;
    unsigned short adsrCounter;
    unsigned char  adsrExpCounter;
    unsigned char  volumeLevel;
} sid_channel;

typedef struct {
    unsigned char regs[32];          // $00-$1F (write-only register file)
    sid_channel   ch[3];
    float         cyclesPerSample;   // CPU clocks per output sample
    float         cycleAccumulator;
    float         prevBandPass, prevLowPass;   // state-variable filter
} sidchip;

void sid_reset(sidchip *s);                         // clear + default per-voice vols
void sid_write(sidchip *s, int reg, unsigned char data);   // reg 0..31
void sid_render(sidchip *s, float *out_mono, int count);   // count mono samples @48k

#endif
