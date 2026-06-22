// sidplay.cpp — glue between the reused nova_sid::SidVm (ESP NovaHost) and the
// Arty software SID. The SidVm runs the tune's 6502 init/play in a sandbox; its
// SID register writes are mirrored into our software SID via audio_sid_reg_write.
#include "sidplay.h"
#include "sid_vm.h"
#include "xil_printf.h"

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

extern "C" void sidplay_advance(int samples) {
    if (!g_active) return;
    g_acc += (unsigned)samples;
    int guard = 0;
    while (g_acc >= g_period && ++guard < 8) {     // fire play frame(s) at the tune rate
        g_acc -= g_period;
        RunResult r = g_vm.runPlayFrame();
        if (r.status != RunStatus::Ok && g_err_count < 4) {   // log a few play errors
            g_err_count++;
            xil_printf("[sid] play error: %s pc=%04X op=%02X\r\n",
                       run_status_name(r.status), r.pc, r.opcode);
        }
    }
}

extern "C" void sidplay_stop(void) { g_active = false; }
extern "C" int  sidplay_active(void) { return g_active ? 1 : 0; }
