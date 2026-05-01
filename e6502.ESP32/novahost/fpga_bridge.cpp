// FpgaBridge — binary debug protocol implementation
// See debug_bridge.sv for the FPGA side of this protocol.

#include "fpga_bridge.h"

// Binary command bytes (must match debug_bridge.sv)
#define CMD_PEEK        0x01
#define CMD_POKE        0x02
#define CMD_SEND_KEY    0x03
#define CMD_CPU_STATE   0x06
#define CMD_PAUSE       0x07
#define CMD_RESUME      0x08
#define CMD_PEEK_BLOCK  0x09
#define CMD_POKE_ROM    0x0A
#define CMD_RESET_HOLD  0x0B
#define CMD_RESET_REL   0x0C
#define CMD_POKE_ROM_BLK 0x0D
#define CMD_POKE_SDRAM_BLK 0x0E
#define CMD_POKE_BLOCK   0x0F
#define CMD_POKE_VGC_BLK 0x10
#define CMD_FILL_VGC_BLK 0x11
#define CMD_READ_VGC_BLK 0x12
#define CMD_BREAK_SET    0x13
#define CMD_BREAK_CLR    0x14
#define CMD_BREAK_LIST   0x15
#define CMD_STEP         0x16
#define CMD_TRACE_READ   0x17
#define CMD_SYS_RESET_HOLD 0x18
#define CMD_SYS_RESET_REL  0x19

#define VGC_SPACE_CHAR  0x01
#define VGC_SPACE_COLOR 0x02
#define TEXT_BYTES      4000

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
        delay(1);
    }
    return _serial.read();
}

bool FpgaBridge::recvBytes(uint8_t* buf, int count) {
    unsigned long start = millis();
    for (int i = 0; i < count; i++) {
        while (!_serial.available()) {
            if (millis() - start >= BULK_TIMEOUT_MS) return false;
            delay(1);
        }
        int b = _serial.read();
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
    const uint16_t BLOCK = 256;
    for (uint16_t off = 0; off < TEXT_BYTES; off += BLOCK) {
        uint16_t chunk = (TEXT_BYTES - off >= BLOCK) ? BLOCK : (TEXT_BYTES - off);
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!readVgcBlock(VGC_SPACE_CHAR, off, wire_count, buf + off))
            return false;
    }
    return true;
}

bool FpgaBridge::readColor(uint8_t* buf) {
    const uint16_t BLOCK = 256;
    for (uint16_t off = 0; off < TEXT_BYTES; off += BLOCK) {
        uint16_t chunk = (TEXT_BYTES - off >= BLOCK) ? BLOCK : (TEXT_BYTES - off);
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!readVgcBlock(VGC_SPACE_COLOR, off, wire_count, buf + off))
            return false;
    }
    return true;
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
    st.status = buf[7];
    st.waiting = (buf[7] & 0x01) != 0;
    st.stopped = (buf[7] & 0x02) != 0;
    st.paused  = (buf[7] & 0x04) != 0;
    st.breakpointHit = (buf[7] & 0x08) != 0;
    st.stepActive = (buf[7] & 0x10) != 0;
    return true;
}

bool FpgaBridge::step(CpuState& st) {
    drain();
    _serial.write(CMD_STEP);
    if (!recvStatus()) return false;
    uint8_t buf[8];
    if (!recvBytes(buf, 8)) return false;
    st.pc    = ((uint16_t)buf[0] << 8) | buf[1];
    st.a     = buf[2];
    st.x     = buf[3];
    st.y     = buf[4];
    st.sp    = buf[5];
    st.flags = buf[6];
    st.status = buf[7];
    st.waiting = (buf[7] & 0x01) != 0;
    st.stopped = (buf[7] & 0x02) != 0;
    st.paused  = (buf[7] & 0x04) != 0;
    st.breakpointHit = (buf[7] & 0x08) != 0;
    st.stepActive = (buf[7] & 0x10) != 0;
    return true;
}

bool FpgaBridge::breakSet(uint8_t slot, uint16_t addr, bool enabled) {
    if (slot >= 4) return false;
    drain();
    uint8_t buf[5] = {
        CMD_BREAK_SET,
        slot,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        enabled ? (uint8_t)1 : (uint8_t)0
    };
    _serial.write(buf, 5);
    return recvStatus();
}

bool FpgaBridge::breakClear(uint8_t slot) {
    if (slot >= 4) return false;
    drain();
    uint8_t buf[2] = { CMD_BREAK_CLR, slot };
    _serial.write(buf, 2);
    return recvStatus();
}

bool FpgaBridge::breakList(BreakpointState& state) {
    drain();
    _serial.write(CMD_BREAK_LIST);
    if (!recvStatus()) return false;
    uint8_t buf[14];
    if (!recvBytes(buf, 14)) return false;
    state.flags = buf[0];
    state.hitSlot = buf[1] & 0x03;
    for (int i = 0; i < 4; i++) {
        int base = 2 + i * 3;
        state.slots[i].enabled = (buf[base] & 0x01) != 0;
        state.slots[i].address = ((uint16_t)buf[base + 1] << 8) | buf[base + 2];
    }
    return true;
}

bool FpgaBridge::traceRead(uint8_t count, uint8_t* buf, uint16_t& bytesRead) {
    if (count > TRACE_RECORDS) return false;
    drain();
    uint8_t cmd[2] = { CMD_TRACE_READ, count };
    _serial.write(cmd, 2);
    if (!recvStatus()) return false;
    uint8_t actualRecords = (count == 0) ? TRACE_RECORDS : count;
    bytesRead = (uint16_t)actualRecords * TRACE_RECORD_BYTES;
    return recvBytes(buf, bytesRead);
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

bool FpgaBridge::resetHold() {
    drain();
    _serial.write(CMD_RESET_HOLD);
    return recvStatus();
}

bool FpgaBridge::resetRelease() {
    drain();
    _serial.write(CMD_RESET_REL);
    return recvStatus();
}

bool FpgaBridge::systemResetHold() {
    drain();
    _serial.write(CMD_SYS_RESET_HOLD);
    return recvStatus();
}

bool FpgaBridge::systemResetRelease() {
    drain();
    _serial.write(CMD_SYS_RESET_REL);
    return recvStatus();
}

bool FpgaBridge::pokeRom(uint8_t idx, uint16_t addr, uint8_t value) {
    drain();
    uint8_t buf[5] = {
        CMD_POKE_ROM,
        (uint8_t)(idx & 0x01),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        value
    };
    _serial.write(buf, 5);
    return recvStatus();
}

bool FpgaBridge::pokeRomBlock(uint8_t idx, uint16_t start_addr,
                              const uint8_t* data, uint16_t count) {
    // count==0 means 256 in the wire protocol; reject anything larger.
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_ROM_BLK,
        (uint8_t)(idx & 0x01),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        (uint8_t)(count & 0xFF)     // 256 encodes as 0
    };
    _serial.write(header, 5);
    _serial.write(data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::loadRom(uint8_t idx, const uint8_t* data, size_t len) {
    // Split into 256-byte blocks. At 3.125 Mbaud, 16KB is ~52ms.
    const size_t BLOCK = 256;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!pokeRomBlock(idx, (uint16_t)off, data + off, wire_count))
            return false;
    }
    return true;
}

bool FpgaBridge::pokeSdramBlock(uint32_t addr, const uint8_t* data, uint16_t count) {
    // count==0 means 256 in the wire protocol; reject anything larger.
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_SDRAM_BLK,
        (uint8_t)((addr >> 16) & 0xFF),
        (uint8_t)((addr >>  8) & 0xFF),
        (uint8_t)( addr        & 0xFF),
        (uint8_t)(count & 0xFF)        // 256 encodes as 0
    };
    _serial.write(header, 5);
    _serial.write(data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::loadSdram(uint32_t base_addr, const uint8_t* data, size_t len) {
    const size_t BLOCK = 256;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!pokeSdramBlock(base_addr + off, data + off, wire_count))
            return false;
    }
    return true;
}

bool FpgaBridge::pokeBlock(uint16_t addr, const uint8_t* data, uint16_t count) {
    if (count > 256) return false;
    drain();
    uint8_t header[4] = {
        CMD_POKE_BLOCK,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
        (uint8_t)(count & 0xFF)            // 256 encodes as 0
    };
    _serial.write(header, 4);
    _serial.write(data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::pokeVgcBlock(uint8_t space, uint16_t start_addr,
                              const uint8_t* data, uint16_t count) {
    if (count > 256) return false;
    drain();
    uint8_t header[5] = {
        CMD_POKE_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        (uint8_t)(count & 0xFF)            // 256 encodes as 0
    };
    _serial.write(header, 5);
    _serial.write(data, (count == 0) ? 256 : count);
    return recvStatus();
}

bool FpgaBridge::fillVgcBlock(uint8_t space, uint16_t start_addr, uint8_t value) {
    drain();
    uint8_t header[5] = {
        CMD_FILL_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        value
    };
    _serial.write(header, 5);
    return recvStatus();
}

bool FpgaBridge::readVgcBlock(uint8_t space, uint16_t start_addr,
                              uint8_t count, uint8_t* buf) {
    drain();
    uint8_t header[5] = {
        CMD_READ_VGC_BLK,
        (uint8_t)(space & 0x07),
        (uint8_t)(start_addr >> 8),
        (uint8_t)(start_addr & 0xFF),
        count
    };
    _serial.write(header, 5);
    if (!recvStatus()) return false;
    int n = (count == 0) ? 256 : count;
    return recvBytes(buf, n);
}

bool FpgaBridge::loadRam(uint16_t base_addr, const uint8_t* data, size_t len) {
    const size_t BLOCK = 256;
    for (size_t off = 0; off < len; off += BLOCK) {
        size_t chunk = (len - off >= BLOCK) ? BLOCK : (len - off);
        uint16_t wire_count = (chunk == 256) ? 0 : (uint16_t)chunk;
        if (!pokeBlock((uint16_t)(base_addr + off), data + off, wire_count))
            return false;
    }
    return true;
}
