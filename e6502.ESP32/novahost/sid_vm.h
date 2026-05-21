#ifndef NOVAHOST_SID_VM_H
#define NOVAHOST_SID_VM_H

#include <stddef.h>
#include <stdint.h>

namespace nova_sid {

struct SidFileInfo {
    bool valid = false;
    bool rsid = false;
    bool loadAddressInPayload = false;
    char magic[5] = {};
    uint16_t dataOffset = 0;
    uint16_t loadAddress = 0;
    uint16_t initAddress = 0;
    uint16_t playAddress = 0;
    uint16_t songs = 0;
    uint16_t startSong = 0;
    uint32_t speed = 0;
    uint16_t flags = 0;
    uint32_t payloadFileOffset = 0;
    uint32_t payloadBytes = 0;
};

bool parse_sid_header(const uint8_t* header, uint32_t file_size,
                      SidFileInfo& info);
bool sid_uses_cia_timing(const SidFileInfo& info, uint8_t song);
uint32_t sid_frame_period_us(const SidFileInfo& info, uint8_t song);
uint8_t sid_fpga_config(const SidFileInfo& info);

class SparseMemory {
public:
    SparseMemory();
    ~SparseMemory();

    SparseMemory(const SparseMemory&) = delete;
    SparseMemory& operator=(const SparseMemory&) = delete;

    void clear();
    bool write(uint16_t addr, uint8_t value);
    uint8_t read(uint16_t addr) const;
    bool load(uint16_t addr, const uint8_t* data, size_t len);
    bool pageAllocated(uint8_t page) const;
    uint16_t pageCount() const { return _page_count; }
    uint32_t bytesAllocated() const { return (uint32_t)_page_count * PageBytes; }

    static constexpr uint16_t PageBytes = 256;
    static constexpr uint16_t PageCount = 256;

private:
    uint8_t* ensure_page(uint8_t page);

    uint8_t* _pages[PageCount];
    uint16_t _page_count = 0;
};

enum class RunStatus : uint8_t {
    Ok,
    Hung,
    OutOfMemory,
    UnsupportedOpcode,
    UnsupportedRomCall,
    InternalError,
};

struct RunResult {
    RunStatus status = RunStatus::Ok;
    uint16_t pc = 0;
    uint8_t opcode = 0;
    uint32_t cycles = 0;
};

struct SidVmStats {
    uint32_t initCalls = 0;
    uint32_t playCalls = 0;
    uint32_t sidWrites = 0;
    uint32_t unsupportedCalls = 0;
    uint16_t pagesAllocated = 0;
    uint32_t bytesAllocated = 0;
    uint16_t lastPc = 0;
    uint8_t lastOpcode = 0;
};

class SidVm {
public:
    using SidWriteHandler = bool (*)(void* user, uint16_t addr, uint8_t value);

    SidVm();
    ~SidVm() = default;

    void reset();
    bool loadPayload(uint16_t load_addr, const uint8_t* data, size_t len);
    void setEntryPoints(uint16_t init_addr, uint16_t play_addr);
    void setSidWriteHandler(SidWriteHandler handler, void* user);

    RunResult runInit(uint8_t song);
    RunResult runPlayFrame();

    const SidVmStats& stats() const { return _stats; }
    const uint8_t* sid1Shadow() const { return _sid1; }
    const uint8_t* sid2Shadow() const { return _sid2; }
    uint8_t sid1(uint8_t off) const { return _sid1[off & 0x1F]; }
    uint8_t sid2(uint8_t off) const { return _sid2[off & 0x1F]; }

private:
    static constexpr uint16_t TrapAddress = 0x03D0;
    static constexpr uint32_t MaxInitCycles = 1000000;
    static constexpr uint32_t MaxPlayCycles = 50000;

    static constexpr uint8_t FlagC = 0x01;
    static constexpr uint8_t FlagZ = 0x02;
    static constexpr uint8_t FlagI = 0x04;
    static constexpr uint8_t FlagD = 0x08;
    static constexpr uint8_t FlagB = 0x10;
    static constexpr uint8_t FlagU = 0x20;
    static constexpr uint8_t FlagV = 0x40;
    static constexpr uint8_t FlagN = 0x80;

    struct Cpu {
        uint8_t a = 0;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t sp = 0xFD;
        uint8_t p = FlagI | FlagU;
        uint16_t pc = 0;
    };

    RunResult runVirtualJsr(uint16_t addr, uint8_t a, uint32_t max_cycles);
    RunStatus step(uint8_t& opcode, uint32_t& cycles);
    bool handleRomStub(uint16_t pc, bool& handled, RunStatus& status);

    uint8_t read(uint16_t addr);
    bool write(uint16_t addr, uint8_t value);
    bool writeSid(uint16_t addr, uint8_t value);
    bool rmw(uint16_t addr, uint8_t value);

    uint8_t fetch8();
    uint16_t fetch16();
    uint8_t readZp(uint8_t addr);
    bool writeZp(uint8_t addr, uint8_t value);
    uint16_t addrZp();
    uint16_t addrZpX();
    uint16_t addrZpY();
    uint16_t addrAbs();
    uint16_t addrAbsX();
    uint16_t addrAbsY();
    uint16_t addrIndX();
    uint16_t addrIndY();

    bool push(uint8_t value);
    uint8_t pop();
    bool rts();
    bool rti();

    void setZN(uint8_t value);
    bool flag(uint8_t bit) const { return (_cpu.p & bit) != 0; }
    void setFlag(uint8_t bit, bool on);
    void adc(uint8_t value);
    void sbc(uint8_t value);
    void cmp(uint8_t reg, uint8_t value);
    void testBits(uint8_t value);
    uint8_t asl(uint8_t value);
    uint8_t lsr(uint8_t value);
    uint8_t rol(uint8_t value);
    uint8_t ror(uint8_t value);
    void branch(bool take);

    bool executeUnofficial(uint8_t opcode, RunStatus& status);
    void nopRead(uint16_t addr);

    SparseMemory _mem;
    Cpu _cpu;
    uint16_t _init_addr = 0;
    uint16_t _play_addr = 0;
    uint32_t _frame = 0;
    uint8_t _sid1[32] = {};
    uint8_t _sid2[32] = {};
    uint8_t _io[512] = {};
    uint16_t _cia_timer = 0;
    uint32_t _rng = 0x4E4F5641UL;
    SidWriteHandler _sid_write = nullptr;
    void* _sid_write_user = nullptr;
    SidVmStats _stats;
};

const char* run_status_name(RunStatus status);

} // namespace nova_sid

#endif
