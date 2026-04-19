// FpgaBridge — binary debug protocol over UART (ESP32 ↔ FPGA)
// Translates high-level operations into the binary command protocol
// defined in debug_bridge.sv.

#ifndef FPGA_BRIDGE_H
#define FPGA_BRIDGE_H

#include <Arduino.h>
#include <HardwareSerial.h>

class FpgaBridge {
public:
    FpgaBridge(HardwareSerial& serial) : _serial(serial) {}

    // Single-byte memory read/write
    bool peek(uint16_t addr, uint8_t& value);
    bool poke(uint16_t addr, uint8_t value);

    // Key injection
    bool sendKey(uint8_t key);

    // Bulk screen/color read (buf must hold 2000 bytes)
    bool readScreen(uint8_t* buf);
    bool readColor(uint8_t* buf);

    // CPU register snapshot
    struct CpuState {
        uint16_t pc;
        uint8_t a, x, y, sp, flags;
    };
    bool cpuState(CpuState& st);

    // Execution control
    bool pause();
    bool resume();

    // Block read (count=0 means 256)
    bool peekBlock(uint16_t addr, uint8_t count, uint8_t* buf);

    // Drain stale bytes from serial
    void drain();

private:
    HardwareSerial& _serial;
    static const unsigned long BYTE_TIMEOUT_MS = 200;

    bool recvStatus();
    bool recvBytes(uint8_t* buf, int count);
    int  recvByte();
};

#endif
