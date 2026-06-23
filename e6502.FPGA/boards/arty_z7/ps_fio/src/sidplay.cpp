// sidplay.cpp — glue between the reused nova_sid::SidVm (ESP NovaHost) and the
// Arty software SID. The SidVm runs the tune's 6502 init/play in a sandbox; its
// SID register writes are mirrored into our software SID via audio_sid_reg_write.
#include "sidplay.h"
#include "sid_vm.h"
#include "xil_printf.h"
#include <math.h>

extern "C" void audio_sid_reg_write(int chip, int reg, unsigned char val);
extern "C" void audio_set_sid_stereo(int on);
extern "C" void audio_mmio_poke(unsigned addr, unsigned char v);

using namespace nova_sid;

static SidVm    g_vm;
static bool     g_active  = false;
static unsigned g_period  = 960;   // output samples per play frame (50 Hz @ 48 kHz)
static unsigned g_acc     = 0;
static int      g_err_count = 0;

// Mirror each emulated SID write to the real reDIP-SID ($D420+ -> chip 1).
static bool sid_write_cb(void *user, uint16_t addr, uint8_t val) {
    (void)user;
    audio_sid_reg_write((addr >= 0xD420) ? 1 : 0, addr & 0x1F, val);
    return true;
}

extern "C" int sidplay_load(const unsigned char *buf, int len, int song) {
    SidFileInfo info;
    if (!parse_sid_header(buf, (uint32_t)len, info) || !info.valid) {
        xil_printf("[sid] not a valid PSID/RSID file\r\n");
        return -1;
    }
    // When the PSID load field is 0 the real load address is the first 2 bytes
    // of the payload (little-endian C64 PRG style); parse leaves it for us.
    uint16_t loadAddr = info.loadAddress;
    if (info.loadAddressInPayload)
        loadAddr = (uint16_t)(buf[info.dataOffset] | (buf[info.dataOffset + 1] << 8));
    g_vm.reset();
    if (!g_vm.loadPayload(loadAddr, buf + info.payloadFileOffset, info.payloadBytes)) {
        xil_printf("[sid] loadPayload failed\r\n");
        return -2;
    }
    info.loadAddress = loadAddr;          // for the log below
    g_vm.setEntryPoints(info.initAddress, info.playAddress);
    g_vm.setSidWriteHandler(sid_write_cb, nullptr);
    // Set the fabric SID model/clock from the PSID flags ($76-$77, big-endian),
    // like the ESP's configure_sid. $D440: bit0=SID1 model, bit2=SID2 model
    // (0=6581/1=8580), bit1=NTSC. An 8580 tune played as 6581 (with the 6581
    // filter curve) distorts badly -- this is what wrecked Love Fileosophy.
    if (len > 0x77) {
        uint16_t flags = (uint16_t)((buf[0x76] << 8) | buf[0x77]);
        uint8_t  model = (uint8_t)((flags >> 4) & 0x03);   // 1=6581, 2=8580
        uint8_t  clk   = (uint8_t)((flags >> 2) & 0x03);   // 1=PAL, 2=NTSC
        uint8_t  cfg   = 0;
        if (model == 2) cfg |= 0x05;     // 8580 on both SID1 ($D440.0) and SID2 ($D440.2)
        if (clk   == 2) cfg |= 0x02;     // NTSC clock
        audio_mmio_poke(0xD440, cfg);
        // Music-block metadata for the keyboard visualizer (source + legend):
        // AUDIO_META_TYPE ($BAB0)=SID, AUDIO_META_FLAGS ($BB1C)=model/stereo/clock.
        uint8_t meta = (model == 2) ? 0x02 : 0x01;          // 8580 : 6581
        if (len > 0x7B && buf[0x7A]) meta |= 0x04;          // stereo (2SID)
        if (clk == 2)                meta |= 0x08;          // NTSC
        audio_mmio_poke(0xBB1C, meta);                      // AUDIO_META_FLAGS
        audio_mmio_poke(0xBAB0, 0x01);                      // AUDIO_META_TYPE = SID
        // Song name / artist / released for the visualizer header. PSID stores these
        // as 32-byte null-padded strings: title@$16, author@$36, released@$56.
        for (int i = 0; i < 32; i++) {
            audio_mmio_poke(0xBAB3 + i, buf[0x16 + i]);     // AUDIO_META_TITLE
            audio_mmio_poke(0xBAD3 + i, buf[0x36 + i]);     // AUDIO_META_AUTHOR
            audio_mmio_poke(0xBAF3 + i, buf[0x56 + i]);     // AUDIO_META_COPYRIGHT (released)
        }
        audio_mmio_poke(0xBB19, (unsigned char)info.songs); // AUDIO_META_SONGS
        audio_mmio_poke(0xBB13, loadAddr & 0xFF);            audio_mmio_poke(0xBB14, (loadAddr >> 8) & 0xFF);
        audio_mmio_poke(0xBB15, info.initAddress & 0xFF);    audio_mmio_poke(0xBB16, (info.initAddress >> 8) & 0xFF);
        audio_mmio_poke(0xBB17, info.playAddress & 0xFF);    audio_mmio_poke(0xBB18, (info.playAddress >> 8) & 0xFF);
    }
    int s = (song > 0) ? song : (info.startSong ? info.startSong : 1);
    // 6502 A = 0-based subtune index (the ESP calls runInit(song - 1)); passing the
    // 1-based song selected the wrong/empty subtune on some tunes (Love Fileosophy).
    RunResult r = g_vm.runInit((uint8_t)(s - 1));
    uint32_t us = sid_frame_period_us(info, (uint8_t)s);
    if (us == 0) us = 20000;                       // default 50 Hz
    g_period = (unsigned)(((uint64_t)us * 48000ULL) / 1000000ULL);
    if (g_period < 1) g_period = 960;
    // Multi-speed: honor the tune's CIA Timer A latch (cycles between play calls) if
    // it set one in init. The fixed 50/60 Hz under-drives multi-speed tunes -- fast
    // sections mangle while sustained parts sound fine (Love Fileosophy). PAL CPU =
    // 985248 Hz; g_period = latch * 48000 / 985248 output samples per play call.
    uint16_t cia = g_vm.ciaTimer();
    if (cia >= 2000 && cia <= 22000) {
        unsigned p = (unsigned)(((uint64_t)cia * 48000ULL) / 985248ULL);
        if (p >= 60) g_period = p;        // cap play rate ~800 Hz
    }
    g_acc = 0;
    g_active = true;
    g_err_count = 0;
    xil_printf("[sid] initWrites=%u unsup=%u cia=%u period=%u\r\n",
               (unsigned)g_vm.stats().sidWrites, (unsigned)g_vm.stats().unsupportedCalls,
               (unsigned)cia, g_period);
    // PSID v3+ secondSIDAddress (header $7A): nonzero => 2SID tune => pan SID1->L /
    // SID2->R. Single-SID tunes stay mono (centered).
    audio_set_sid_stereo((len > 0x7B && buf[0x7A] != 0) ? 1 : 0);
    xil_printf("[sid] play load=%04X init=%04X play=%04X song=%d period=%u (init:%s)\r\n",
               info.loadAddress, info.initAddress, info.playAddress, s, g_period,
               run_status_name(r.status));
    return 0;
}

// Convert a 16-bit SID frequency register to a MIDI note (0 = silent/sub-audible).
// PAL: f_Hz = Fn * 985248 / 16777216; note = 69 + 12*log2(f/440).
static unsigned char sid_freq_to_note(unsigned fn) {
    if (fn < 50) return 0;                      // truly silent / sub-audible
    float hz = (float)fn * 985248.0f / 16777216.0f;
    int note = (int)(69.0f + 12.0f * log2f(hz / 440.0f) + 0.5f);
    if (note < 12)  return 0;                   // below the visualizer's C0
    if (note > 127) note = 127;
    return (unsigned char)note;
}

// Publish the 6 SID voices' current notes to the $BA50 music block so the keyboard
// visualizer can show what a .sid is playing. A voice is sounding when its gate
// (control bit0) is set; freq comes from the SidVm register shadow.
static void sid_publish_notes(void) {
    static const int FLO[3] = { 0, 7, 14 };    // voice freq-lo register offsets
    static const int CTL[3] = { 4, 11, 18 };   // voice control register offsets
    audio_mmio_poke(0xBA50, 0x06);             // MUSIC_STATUS: bit1=music + bit2=SID
    int slot = 0;
    for (int v = 0; v < 3; v++) {
        unsigned f = g_vm.sid1(FLO[v]) | ((unsigned)g_vm.sid1(FLO[v] + 1) << 8);
        audio_mmio_poke(0xBA51 + slot++, (g_vm.sid1(CTL[v]) & 0x01) ? sid_freq_to_note(f) : 0);
    }
    for (int v = 0; v < 3; v++) {
        unsigned f = g_vm.sid2(FLO[v]) | ((unsigned)g_vm.sid2(FLO[v] + 1) << 8);
        audio_mmio_poke(0xBA51 + slot++, (g_vm.sid2(CTL[v]) & 0x01) ? sid_freq_to_note(f) : 0);
    }
    while (slot < 14) audio_mmio_poke(0xBA51 + slot++, 0);   // remaining voices silent
}

extern "C" void sidplay_advance(int samples) {
    if (!g_active) return;
    g_acc += (unsigned)samples;
    int guard = 0;
    bool fired = false;
    while (g_acc >= g_period && ++guard < 8) {     // fire play frame(s) at the tune rate
        g_acc -= g_period;
        RunResult r = g_vm.runPlayFrame();
        if (r.status != RunStatus::Ok && g_err_count < 4) {   // log a few play errors
            g_err_count++;
            xil_printf("[sid] play error: %s pc=%04X op=%02X\r\n",
                       run_status_name(r.status), r.pc, r.opcode);
        }
        fired = true;
    }
    if (fired) sid_publish_notes();                // refresh the visualizer at the tune rate
}

extern "C" void sidplay_stop(void) { g_active = false; }
extern "C" int  sidplay_active(void) { return g_active ? 1 : 0; }
