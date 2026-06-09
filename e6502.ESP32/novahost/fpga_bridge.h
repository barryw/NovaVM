// FpgaBridge — binary debug protocol over SPI (ESP32 ↔ FPGA)
// Translates high-level operations into the binary command protocol
// defined in debug_bridge.sv.

#ifndef FPGA_BRIDGE_H
#define FPGA_BRIDGE_H

#include <Arduino.h>
#include <SPI.h>
#if defined(ARDUINO)
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#endif

class FpgaBridgeCommandGuard;

#ifndef FPGA_SPI_READ_HZ
// Classic ESP32 full-duplex reads over GPIO matrix are only reliable through
// the 80MHz/3 divider. Faster bulk writes can still use FPGA_SPI_HZ.
// 2026-06-08: 80/3 = 26.67 MHz. Tested 80/6 (13.3 MHz) — same ERR_IO miss rate
// (2/12), so the chess-splash miss is NOT clock-rate-dependent MOSI corruption.
// 80/16 (5 MHz) was too slow (boot "degraded"/"storage busy"). Reverted to 80/3.
#define FPGA_SPI_READ_HZ (80000000UL / 3UL)
#endif

class FpgaBridge {
    friend class FpgaBridgeCommandGuard;

public:
    FpgaBridge() = default;

    bool beginSpi(SPIClass& spi, uint8_t csPin, uint32_t hz,
                  uint8_t peerCsPin = 255);
    const char* transportName() const;
    uint32_t requestedWriteHz() const { return _spiHz; }
    uint32_t requestedReadHz() const { return _spiReadHz; }
    uint32_t actualWriteHz() const;
    uint32_t actualReadHz() const;
    bool probeCapabilities();
    bool supportsPokeMulti();
    bool supportsWtsEventStream();
    bool supportsKeyStream();
    // True when the bitstream advertises the bridge RX-resync watchdog, which
    // makes a failed write safe to retry (the bridge re-frames to idle after a
    // stuck/corrupted command, so a resend lands on a clean parser).
    bool supportsRxResync();
    bool lockSharedBus();
    void unlockSharedBus();

    using DrainByteHandler = void (*)(void* user, uint8_t value);
    void onDrainByte(DrainByteHandler handler, void* user) {
        _drainHandler = handler;
        _drainUser = user;
    }

    // Single-byte memory read/write
    bool peek(uint16_t addr, uint8_t& value);
    bool poke(uint16_t addr, uint8_t value);
    bool pokeMulti(const uint16_t* addrs, const uint8_t* values, uint8_t count);

    // Key injection
    bool sendKey(uint8_t key);
    bool sendKeys(const uint8_t* data, uint16_t count);

    // Bulk screen/color read (buf must hold 4000 bytes: 80x50)
    bool readScreen(uint8_t* buf);
    bool readColor(uint8_t* buf);

    // CPU register snapshot
    struct CpuState {
        uint16_t pc;
        uint8_t a, x, y, sp, flags;
        uint8_t status;
        bool waiting;
        bool stopped;
        bool paused;
        bool breakpointHit;
        bool stepActive;
    };
    bool cpuState(CpuState& st);
    bool step(CpuState& st);

    struct Breakpoint {
        bool enabled;
        uint16_t address;
    };
    struct BreakpointState {
        uint8_t flags;
        uint8_t hitSlot;
        Breakpoint slots[4];
    };
    bool breakSet(uint8_t slot, uint16_t addr, bool enabled);
    bool breakClear(uint8_t slot);
    bool breakList(BreakpointState& state);

    static constexpr uint8_t TRACE_RECORD_BYTES = 12;
    static constexpr uint8_t TRACE_RECORDS = 64;
    bool traceRead(uint8_t count, uint8_t* buf, uint16_t& bytesRead);

    // Execution control
    bool pause();
    bool resume();

    // Latch NovaHost status flags into the FPGA for board LEDs.
    bool hostStatus(uint8_t flags);

    // CPU soft-reset (separate from pause — resets 6502 state, not just halts).
    // Call resetHold() before loading a ROM, resetRelease() once load is done
    // so the CPU starts fresh from the freshly-written reset vector.
    bool resetHold();
    bool resetRelease();
    bool systemResetHold();
    bool systemResetRelease();

    // Block read (count=0 means 256)
    bool peekBlock(uint16_t addr, uint8_t count, uint8_t* buf);

    // NIC private payload buffers. SEND snapshots CPU RAM into the TX buffer
    // in FPGA hardware; RECV consumes bytes staged into the RX buffer.
    bool nicReadTxBlock(uint8_t offset, uint8_t count, uint8_t* buf);
    bool nicWriteRxBlock(uint8_t offset, const uint8_t* data, uint16_t count);

    // Write one byte into a ROM bank. idx=0 → basic_rom ($C000-$FFFF when
    // ext_rom_active=0), idx=1 → ext_rom. addr is the 14-bit offset (0..16383).
    bool pokeRom(uint8_t idx, uint16_t addr, uint8_t value);

    // Block-write up to 256 bytes into a ROM bank. count=0 means 256. This is
    // the fast path: one command + ack wraps 256 bytes, vs one ack per byte
    // for pokeRom.
    bool pokeRomBlock(uint8_t idx, uint16_t start_addr, const uint8_t* data, uint16_t count);

    // Bulk-load a ROM bank via 256-byte blocks. Caller must hold the CPU
    // in reset (resetHold()) before calling so the CPU doesn't observe
    // partially-loaded ROM. Returns false on any block-level failure.
    bool loadRom(uint8_t idx, const uint8_t* data, size_t len);

    // Block-write up to 256 bytes into SDRAM via debug-bridge port B.
    // addr is a 24-bit byte address (covers the 16 MB used by curve+XRAM).
    // count=0 means 256. Caller must hold the CPU in reset during the
    // entire sequence for boot-time preloads.
    bool pokeSdramBlock(uint32_t addr, const uint8_t* data, uint16_t count);

    // Stream-write a larger byte range into SDRAM with one bridge ack at the
    // end. The FPGA bridge owns buffering/backpressure; the ESP side sends this
    // as one unpaced bulk SPI write.
    bool pokeSdramStream(uint32_t addr, const uint8_t* data, uint16_t count);

    struct AudioPcmStatus {
        uint16_t bytesFree;
        uint16_t underruns;
    };

    // Host-rendered HDMI audio path. PCM bytes are signed 16-bit stereo,
    // little-endian per frame: Llo,Lhi,Rlo,Rhi.
    bool audioPcmStatus(AudioPcmStatus& status);
    bool writeAudioPcm(const uint8_t* data, uint16_t count);
    bool streamAudioPcm(const uint8_t* data, size_t len);

    // Timestamped WTS event stream. Payload records are 6 bytes each:
    // sample-frame uint32 little-endian, WTS register offset, byte value.
    bool writeWtsEvents(const uint8_t* data, uint16_t count);

    // Block-read up to 256 bytes from SDRAM via debug-bridge port B.
    // count=0 means 256.
    bool readSdramBlock(uint32_t addr, uint8_t count, uint8_t* buf);

    // Block-write up to 256 bytes into CPU RAM. count=0 means 256.
    // Used by FIO LOAD to push file payload from SD into CPU RAM.
    // CPU is NOT held in reset — writes interleave with CPU activity
    // via the existing dbg_poke path.
    bool pokeBlock(uint16_t addr, const uint8_t* data, uint16_t count);

    // Block-write up to 256 bytes directly into a VGC memory space. count=0
    // means 256. Spaces match DMA/VGC IDs: 1=char, 2=color, 3=gfx,
    // 4=sprite shapes, 7=text attributes.
    bool pokeVgcBlock(uint8_t space, uint16_t start_addr,
                      const uint8_t* data, uint16_t count);

    // Fill exactly 256 bytes in a VGC memory space. This is used at boot to
    // clear the graphics plane without sending 64KB over the host bridge.
    bool fillVgcBlock(uint8_t space, uint16_t start_addr, uint8_t value);

    // Block-read up to 256 bytes directly from a VGC memory space. count=0
    // means 256. Debug-only; this does not expose VRAM in the CPU address map.
    bool readVgcBlock(uint8_t space, uint16_t start_addr,
                      uint8_t count, uint8_t* buf);

    // Bulk-load an arbitrary region of CPU RAM via 256-byte blocks.
    // Returns false on any block failure. Wraps pokeBlock.
    bool loadRam(uint16_t base_addr, const uint8_t* data, size_t len);

    // Bulk-load an arbitrary region of SDRAM via 256-byte blocks.
    bool loadSdram(uint32_t base_addr, const uint8_t* data, size_t len);

    // Drain pending async bridge bytes.
    void drain();

private:
    SPIClass* _spi = nullptr;
    uint8_t _spiCsPin = 255;
    uint8_t _spiPeerCsPin = 255;
    uint32_t _spiHz = 500000;
    uint32_t _spiReadHz = 500000;
    bool _spiEnabled = false;
    bool _capabilitiesKnown = false;
    bool _supportsPokeMulti = false;
    bool _supportsWtsEventStream = false;
    bool _supportsKeyStream = false;
    bool _supportsRxResync = false;
#if defined(ARDUINO)
    SemaphoreHandle_t _commandMutex = nullptr;
#endif

    DrainByteHandler _drainHandler = nullptr;
    void* _drainUser = nullptr;
    static const unsigned long BYTE_TIMEOUT_MS = 200;
    static const unsigned long BULK_TIMEOUT_MS = 500;
    static constexpr uint8_t SPI_WRITE_OP = 0x57;       // 'W'
    static constexpr uint8_t SPI_READ_OP = 0x52;        // 'R'
    static constexpr uint8_t SPI_TOKEN_EMPTY = 0x00;
    static constexpr uint8_t SPI_TOKEN_DATA = 0x01;
    static constexpr int SPI_DRAIN_LIMIT = 512;
    static constexpr uint8_t SPI_READ_POLL_BYTES = 8;
    // Total attempts for a VGC block write when the bridge supports RX-resync.
    // A flaky SPI link corrupts a write rarely; the resend lands on a re-framed
    // parser. Each failed attempt costs one recvStatus timeout, so keep small.
    static constexpr int VGC_WRITE_ATTEMPTS = 3;
    // The SPI slave has a 512-byte RX FIFO and MOSI cannot be backpressured.
    // Keep each unpaced write transaction comfortably below that depth so
    // SDRAM/WTS consumers can drain the FIFO after CS rises without losing
    // bytes and desynchronizing the debug protocol.
    static constexpr uint16_t SDRAM_STREAM_MAX_BYTES = 256;
    static constexpr uint16_t WTS_EVENT_STREAM_MAX_BYTES = 60;  // 10 x 6-byte records

    bool spiReady() const { return _spiEnabled && _spi != nullptr; }
    SPISettings spiWriteSettings() const { return SPISettings(_spiHz, MSBFIRST, SPI_MODE0); }
    SPISettings spiControlWriteSettings() const { return SPISettings(_spiReadHz, MSBFIRST, SPI_MODE0); }
    SPISettings spiReadSettings() const { return SPISettings(_spiReadHz, MSBFIRST, SPI_MODE0); }
    uint32_t actualSpiHzFor(uint32_t hz) const;
    bool lockCommand();
    void unlockCommand();

    bool recvStatus();
    bool recvBytes(uint8_t* buf, int count);
    int  recvByte();
    int  recvByte(bool wait);
    bool tryRecvSpiByte(uint8_t& value);
    int  recvSpiByte(bool wait);
    void writeByte(uint8_t value);
    void writeBytes(const uint8_t* data, size_t len);
    void writeBytes(const uint8_t* first, size_t firstLen,
                    const uint8_t* second, size_t secondLen);
    void writeBytesWithSettings(const uint8_t* data, size_t len,
                                const SPISettings& settings);
    void writeBytesWithSettings(const uint8_t* first, size_t firstLen,
                                const uint8_t* second, size_t secondLen,
                                const SPISettings& settings);
    void writeBytesBulk(const uint8_t* first, size_t firstLen,
                        const uint8_t* second, size_t secondLen);
    bool pokeSdramStreamChunk(uint32_t addr, const uint8_t* data, uint16_t count);
    bool writeWtsEventsChunk(const uint8_t* data, uint16_t count);
};

#endif
