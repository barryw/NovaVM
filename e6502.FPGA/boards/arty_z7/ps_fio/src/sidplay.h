// sidplay.h — .sid file playback for the Arty PS, C interface.
// Reuses the ESP NovaHost's nova_sid::SidVm (e6502.ESP32/novahost/sid_vm.cpp):
// a sandboxed 6502 runs the tune's init/play; every SID register write is
// mirrored into our software SID (audio_sid_reg_write) -> mixed -> HDMI. The real
// 6502 is untouched and the tune can't overlap anything (it owns its own RAM).
#ifndef SIDPLAY_H
#define SIDPLAY_H
#ifdef __cplusplus
extern "C" {
#endif

int  sidplay_load(const unsigned char *buf, int len, int song);  // parse+init. 0 ok, <0 err
void sidplay_advance(int samples);   // advance by N output samples; fires play frames at the tune rate
void sidplay_stop(void);
int  sidplay_active(void);

#ifdef __cplusplus
}
#endif
#endif
