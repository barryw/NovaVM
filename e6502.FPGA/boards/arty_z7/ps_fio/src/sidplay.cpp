// sidplay.cpp — glue between the reused nova_sid::SidVm (ESP NovaHost) and the
// Arty software SID. The SidVm runs the tune's 6502 init/play in a sandbox; its
// SID register writes are mirrored into our software SID via audio_sid_reg_write.
#include "sidplay.h"
#include "sid_vm.h"
#include "xil_printf.h"

extern "C" void audio_sid_reg_write(int chip, int reg, unsigned char val);

using namespace nova_sid;

static SidVm    g_vm;
static bool     g_active  = false;
static unsigned g_period  = 960;   // output samples per play frame (50 Hz @ 48 kHz)
static unsigned g_acc     = 0;

// Mirror each emulated SID write to the software SID ($D420+ -> chip 2).
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
    int s = (song > 0) ? song : (info.startSong ? info.startSong : 1);
    RunResult r = g_vm.runInit((uint8_t)s);
    uint32_t us = sid_frame_period_us(info, (uint8_t)s);
    if (us == 0) us = 20000;                       // default 50 Hz
    g_period = (unsigned)(((uint64_t)us * 48000ULL) / 1000000ULL);
    if (g_period < 1) g_period = 960;
    g_acc = 0;
    g_active = true;
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
        g_vm.runPlayFrame();
    }
}

extern "C" void sidplay_stop(void) { g_active = false; }
extern "C" int  sidplay_active(void) { return g_active ? 1 : 0; }
