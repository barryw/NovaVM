// FpgaBridge — binary debug protocol implementation
// See debug_bridge.sv for the FPGA side of this protocol.

#include "fpga_bridge.h"

// Binary command bytes (must match debug_bridge.sv)
#define CMD_PEEK        0x01
#define CMD_POKE        0x02
#define CMD_SEND_KEY    0x03
#define CMD_READ_SCREEN 0x04
#define CMD_READ_COLOR  0x05
#define CMD_CPU_STATE   0x06
#define CMD_PAUSE       0x07
#define CMD_RESUME      0x08
#define CMD_PEEK_BLOCK  0x09

// =========================================================================
// Low-level serial helpers
// =========================================================================

void FpgaBridge::drain() {
    while (_serial.available()) _serial.read();
}

int FpgaBridge::recvByte() {
    unsigned long start = millis();
    while (!_serial.available()) {
        if (millis() - start >= BYTE_TIMEOUT_MS) return -1;
    }
    return _serial.read();
}

bool FpgaBridge::recvBytes(uint8_t* buf, int count) {
    for (int i = 0; i < count; i++) {
        int b = recvByte();
        if (b < 0) return false;
        buf[i] = (uint8_t)b;
    }
    return true;
}

bool FpgaBridge::recvStatus() {
    int s = recvByte();
    return s == 0x00;
}

// =========================================================================
// Commands
// =========================================================================

bool FpgaBridge::peek(uint16_t addr, uint8_t& value) {
    drain();
    uint8_t buf[3] = { CMD_PEEK, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF) };
    _serial.write(buf, 3);
    if (!recvStatus()) return false;
    int b = recvByte();
    if (b < 0) return false;
    value = (uint8_t)b;
    return true;
}

bool FpgaBridge::poke(uint16_t addr, uint8_t value) {
    drain();
    uint8_t buf[4] = { CMD_POKE, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), value };
    _serial.write(buf, 4);
    return recvStatus();
}

bool FpgaBridge::sendKey(uint8_t key) {
    drain();
    uint8_t buf[2] = { CMD_SEND_KEY, key };
    _serial.write(buf, 2);
    return recvStatus();
}

bool FpgaBridge::readScreen(uint8_t* buf) {
    drain();
    _serial.write(CMD_READ_SCREEN);
    if (!recvStatus()) return false;
    return recvBytes(buf, 2000);
}

bool FpgaBridge::readColor(uint8_t* buf) {
    drain();
    _serial.write(CMD_READ_COLOR);
    if (!recvStatus()) return false;
    return recvBytes(buf, 2000);
}

bool FpgaBridge::cpuState(CpuState& st) {
    drain();
    _serial.write(CMD_CPU_STATE);
    if (!recvStatus()) return false;
    uint8_t buf[8];
    if (!recvBytes(buf, 8)) return false;
    st.pc    = ((uint16_t)buf[0] << 8) | buf[1];
    st.a     = buf[2];
    st.x     = buf[3];
    st.y     = buf[4];
    st.sp    = buf[5];
    st.flags = buf[6];
    return true;
}

bool FpgaBridge::pause() {
    drain();
    _serial.write(CMD_PAUSE);
    return recvStatus();
}

bool FpgaBridge::resume() {
    drain();
    _serial.write(CMD_RESUME);
    return recvStatus();
}

bool FpgaBridge::peekBlock(uint16_t addr, uint8_t count, uint8_t* buf) {
    drain();
    uint8_t cmd[4] = { CMD_PEEK_BLOCK, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), count };
    _serial.write(cmd, 4);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}
