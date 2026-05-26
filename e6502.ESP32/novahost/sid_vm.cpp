#include "sid_vm.h"

#include <stdlib.h>
#include <string.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif

namespace nova_sid {
namespace {

constexpr uint8_t SID_CLOCK_NTSC = 0x02;
constexpr uint8_t SID_MODEL_8580 = 0x02;
constexpr uint8_t FPGA_CFG_MODEL_8580 = 0x01;
constexpr uint8_t FPGA_CFG_CLOCK_NTSC = 0x02;

uint16_t read_be16(const uint8_t* data, size_t off) {
    return ((uint16_t)data[off] << 8) | (uint16_t)data[off + 1];
}

uint32_t read_be32(const uint8_t* data, size_t off) {
    return ((uint32_t)data[off] << 24) |
           ((uint32_t)data[off + 1] << 16) |
           ((uint32_t)data[off + 2] << 8) |
           (uint32_t)data[off + 3];
}

void sid_yield() {
#if defined(ARDUINO)
    yield();
#endif
}

} // namespace

bool parse_sid_header(const uint8_t* header, uint32_t file_size,
                      SidFileInfo& info) {
    info = {};
    if (!header || file_size < 124)
        return false;

    bool psid = memcmp(header, "PSID", 4) == 0;
    bool rsid = memcmp(header, "RSID", 4) == 0;
    if (!psid && !rsid)
        return false;

    memcpy(info.magic, header, 4);
    info.magic[4] = 0;
    info.rsid = rsid;
    info.dataOffset = read_be16(header, 6);
    info.loadAddress = read_be16(header, 8);
    info.initAddress = read_be16(header, 10);
    info.playAddress = read_be16(header, 12);
    info.songs = read_be16(header, 14);
    info.startSong = read_be16(header, 16);
    info.speed = read_be32(header, 18);
    info.flags = info.dataOffset >= 0x7C ? read_be16(header, 0x76) : 0;
    info.loadAddressInPayload = info.loadAddress == 0;

    if (info.dataOffset < 124 || info.dataOffset > file_size)
        return false;
    if (info.loadAddressInPayload && info.dataOffset + 2 > file_size)
        return false;

    info.payloadFileOffset = info.dataOffset + (info.loadAddressInPayload ? 2 : 0);
    if (info.payloadFileOffset > file_size)
        return false;

    info.payloadBytes = file_size - info.payloadFileOffset;
    info.valid = info.payloadBytes > 0;
    return info.valid;
}

bool sid_uses_cia_timing(const SidFileInfo& info, uint8_t song) {
    if (song == 0)
        return false;
    return (info.speed & (1UL << (song - 1))) != 0;
}

uint32_t sid_frame_period_us(const SidFileInfo& info, uint8_t song) {
    if (sid_uses_cia_timing(info, song))
        return 1000000UL / 60UL;

    uint8_t clock = (uint8_t)((info.flags >> 2) & 0x03);
    return clock == SID_CLOCK_NTSC ? (1000000UL / 60UL)
                                   : (1000000UL / 50UL);
}

uint8_t sid_fpga_config(const SidFileInfo& info) {
    uint8_t cfg = 0;
    uint8_t clock = (uint8_t)((info.flags >> 2) & 0x03);
    uint8_t model = (uint8_t)((info.flags >> 4) & 0x03);
    if (clock == SID_CLOCK_NTSC)
        cfg |= FPGA_CFG_CLOCK_NTSC;
    if (model == SID_MODEL_8580)
        cfg |= FPGA_CFG_MODEL_8580;
    return cfg;
}

SparseMemory::SparseMemory() {
    memset(_pages, 0, sizeof(_pages));
}

SparseMemory::~SparseMemory() {
    clear();
}

void SparseMemory::clear() {
    for (uint16_t i = 0; i < PageCount; i++) {
        if (_pages[i]) {
            free(_pages[i]);
            _pages[i] = nullptr;
        }
    }
    _page_count = 0;
}

SparseMemory::Page* SparseMemory::ensure_page(uint8_t page) {
    if (_pages[page])
        return _pages[page];
    Page* p = (Page*)malloc(sizeof(Page));
    if (!p)
        return nullptr;
    memset(p, 0, sizeof(Page));
    _pages[page] = p;
    _page_count++;
    return p;
}

bool SparseMemory::write(uint16_t addr, uint8_t value) {
    Page* page = ensure_page((uint8_t)(addr >> 8));
    if (!page)
        return false;
    uint8_t off = addr & 0xFF;
    page->bytes[off] = value;
    page->written[off >> 3] |= (uint8_t)(1u << (off & 7));
    return true;
}

uint8_t SparseMemory::read(uint16_t addr) const {
    const Page* page = _pages[addr >> 8];
    return page ? page->bytes[addr & 0xFF] : 0;
}

bool SparseMemory::load(uint16_t addr, const uint8_t* data, size_t len) {
    if (!data && len != 0)
        return false;
    for (size_t i = 0; i < len; i++) {
        if (!write((uint16_t)(addr + i), data[i]))
            return false;
    }
    return true;
}

bool SparseMemory::pageAllocated(uint8_t page) const {
    return _pages[page] != nullptr;
}

bool SparseMemory::addressWritten(uint16_t addr) const {
    const Page* page = _pages[addr >> 8];
    if (!page)
        return false;
    uint8_t off = addr & 0xFF;
    return (page->written[off >> 3] & (uint8_t)(1u << (off & 7))) != 0;
}

SidVm::SidVm() {
    reset();
}

void SidVm::reset() {
    _mem.clear();
    memset(_sid1, 0, sizeof(_sid1));
    memset(_sid2, 0, sizeof(_sid2));
    memset(_io, 0, sizeof(_io));
    _cpu = {};
    _init_addr = 0;
    _play_addr = 0;
    _frame = 0;
    _cia_timer = 0;
    _rng = 0x4E4F5641UL;
    _stats = {};
}

bool SidVm::loadPayload(uint16_t load_addr, const uint8_t* data, size_t len) {
    bool ok = _mem.load(load_addr, data, len);
    _stats.pagesAllocated = _mem.pageCount();
    _stats.bytesAllocated = _mem.bytesAllocated();
    return ok;
}

void SidVm::setEntryPoints(uint16_t init_addr, uint16_t play_addr) {
    _init_addr = init_addr;
    _play_addr = play_addr;
}

void SidVm::setSidWriteHandler(SidWriteHandler handler, void* user) {
    _sid_write = handler;
    _sid_write_user = user;
}

RunResult SidVm::runInit(uint8_t song) {
    RunResult r = runVirtualJsr(_init_addr, song, MaxInitCycles);
    if (r.status == RunStatus::Ok)
        _stats.initCalls++;
    return r;
}

RunResult SidVm::runPlayFrame() {
    _frame++;
    if (_play_addr == 0)
        return {};
    RunResult r = runVirtualJsr(_play_addr, 0, MaxPlayCycles);
    if (r.status == RunStatus::Ok)
        _stats.playCalls++;
    return r;
}

RunResult SidVm::runVirtualJsr(uint16_t addr, uint8_t a, uint32_t max_cycles) {
    RunResult result;
    if (addr == 0) {
        result.status = RunStatus::InternalError;
        return result;
    }

    _cpu.a = a;
    _cpu.x = 0;
    _cpu.y = 0;
    _cpu.sp = 0xFD;
    _cpu.p = FlagI | FlagU;
    _cpu.pc = addr;

    uint16_t ret = (uint16_t)(TrapAddress - 1);
    if (!_mem.write(0x01FF, (uint8_t)(ret >> 8)) ||
        !_mem.write(0x01FE, (uint8_t)(ret & 0xFF))) {
        result.status = RunStatus::OutOfMemory;
        return result;
    }

    uint32_t cycles = 0;
    while (cycles < max_cycles) {
        if (_cpu.pc == TrapAddress)
            break;

        bool handled_stub = false;
        RunStatus stub_status = RunStatus::Ok;
        if (!handleRomStub(_cpu.pc, handled_stub, stub_status)) {
            result.status = RunStatus::OutOfMemory;
            break;
        }
        if (stub_status != RunStatus::Ok) {
            result.status = stub_status;
            break;
        }
        if (handled_stub) {
            cycles += 6;
            continue;
        }

        uint8_t opcode = 0;
        uint32_t op_cycles = 0;
        RunStatus status = step(opcode, op_cycles);
        cycles += op_cycles == 0 ? 2 : op_cycles;
        result.opcode = opcode;
        if (status != RunStatus::Ok) {
            result.status = status;
            break;
        }

        if ((cycles & 0x1FFF) == 0)
            sid_yield();
    }

    if (cycles >= max_cycles && result.status == RunStatus::Ok)
        result.status = RunStatus::Hung;

    result.pc = _cpu.pc;
    result.cycles = cycles;
    _stats.lastPc = result.pc;
    _stats.lastOpcode = result.opcode;
    _stats.pagesAllocated = _mem.pageCount();
    _stats.bytesAllocated = _mem.bytesAllocated();
    return result;
}

bool SidVm::handleRomStub(uint16_t pc, bool& handled, RunStatus& status) {
    handled = false;
    status = RunStatus::Ok;

    bool rom_addr = (pc >= 0xA000 && pc <= 0xBFFF) ||
                    (pc >= 0xE000 && pc <= 0xFFFF);
    if (rom_addr && _mem.addressWritten(pc))
        return true;

    auto clear_carry = [&]() { setFlag(FlagC, false); };
    auto set_zero_a = [&]() { _cpu.a = 0; setZN(_cpu.a); };

    switch (pc) {
        case 0xFF81: // CINT
        case 0xFF84: // IOINIT
        case 0xFF87: // RAMTAS
        case 0xFF8A: // RESTOR
        case 0xFF90: // SETMSG
        case 0xFF93: // SECOND
        case 0xFF96: // TKSA
        case 0xFFAB: // LISTEN
        case 0xFFAE: // UNLSN
        case 0xFFB1: // TALK
        case 0xFFB4: // UNTLK
        case 0xFFBA: // SETLFS
        case 0xFFBD: // SETNAM
        case 0xFFC0: // OPEN
        case 0xFFC3: // CLOSE
        case 0xFFC6: // CHKIN
        case 0xFFC9: // CHKOUT
        case 0xFFCC: // CLRCHN
        case 0xFFD2: // CHROUT
        case 0xFFDB: // SETTIM
        case 0xFFDE: // RDTIM
        case 0xFFE1: // STOP
        case 0xFFE7: // CLALL
        case 0xFFEA: // UDTIM
        case 0xFFED: // SCREEN
        case 0xFFF0: // PLOT
        case 0xFFF3: // IOBASE
            clear_carry();
            handled = true;
            return rts();
        case 0xFFE4: // GETIN
        case 0xFFCF: // CHRIN
            set_zero_a();
            clear_carry();
            handled = true;
            return rts();
        case 0xFF99: // MEMTOP
        case 0xFF9C: // MEMBOT
            clear_carry();
            handled = true;
            return rts();
        default:
            break;
    }

    if (rom_addr) {
        status = RunStatus::UnsupportedRomCall;
        _stats.unsupportedCalls++;
    }
    return true;
}

uint8_t SidVm::read(uint16_t addr) {
    if (addr >= 0xD400 && addr <= 0xD41F) {
        if (addr == 0xD41B || addr == 0xD41C) {
            _rng = _rng * 1664525UL + 1013904223UL;
            return (uint8_t)(_rng >> 24);
        }
        return _sid1[addr & 0x1F];
    }
    if (addr >= 0xD420 && addr <= 0xD43F)
        return _sid2[addr & 0x1F];
    if (addr >= 0xD500 && addr <= 0xD51F)
        return _sid2[addr & 0x1F];
    if (addr >= 0xD000 && addr <= 0xD1FF) {
        if (addr == 0xD012)
            return (uint8_t)(_frame & 0xFF);
        if (addr == 0xD019)
            return _io[addr & 0x1FF] | 0x01;
        return _io[addr & 0x1FF];
    }
    if (addr >= 0xDC00 && addr <= 0xDDFF) {
        uint16_t off = addr & 0x1FF;
        if ((off & 0x0F) == 0x04)
            return (uint8_t)(_cia_timer & 0xFF);
        if ((off & 0x0F) == 0x05)
            return (uint8_t)(_cia_timer >> 8);
        return _io[off];
    }
    return _mem.read(addr);
}

bool SidVm::write(uint16_t addr, uint8_t value) {
    if ((addr >= 0xD400 && addr <= 0xD41F) ||
        (addr >= 0xD420 && addr <= 0xD43F) ||
        (addr >= 0xD500 && addr <= 0xD51F)) {
        return writeSid(addr, value);
    }
    if (addr >= 0xD000 && addr <= 0xD1FF) {
        _io[addr & 0x1FF] = value;
        return true;
    }
    if (addr >= 0xDC00 && addr <= 0xDDFF) {
        _io[addr & 0x1FF] = value;
        if ((addr & 0x0F) == 0x04)
            _cia_timer = (_cia_timer & 0xFF00) | value;
        else if ((addr & 0x0F) == 0x05)
            _cia_timer = (_cia_timer & 0x00FF) | ((uint16_t)value << 8);
        return true;
    }
    return _mem.write(addr, value);
}

bool SidVm::writeSid(uint16_t addr, uint8_t value) {
    uint16_t mapped = 0;
    uint8_t* shadow = nullptr;
    if (addr >= 0xD400 && addr <= 0xD41F) {
        mapped = addr;
        shadow = _sid1;
    } else if (addr >= 0xD420 && addr <= 0xD43F) {
        mapped = addr;
        shadow = _sid2;
    } else if (addr >= 0xD500 && addr <= 0xD51F) {
        mapped = (uint16_t)(0xD420 + (addr & 0x1F));
        shadow = _sid2;
    } else {
        return false;
    }

    shadow[mapped & 0x1F] = value;
    _stats.sidWrites++;
    return !_sid_write || _sid_write(_sid_write_user, mapped, value);
}

bool SidVm::rmw(uint16_t addr, uint8_t value) {
    return write(addr, value);
}

uint8_t SidVm::fetch8() {
    uint8_t v = read(_cpu.pc);
    _cpu.pc++;
    return v;
}

uint16_t SidVm::fetch16() {
    uint8_t lo = fetch8();
    uint8_t hi = fetch8();
    return (uint16_t)lo | ((uint16_t)hi << 8);
}

uint8_t SidVm::readZp(uint8_t addr) {
    return read(addr);
}

bool SidVm::writeZp(uint8_t addr, uint8_t value) {
    return write(addr, value);
}

uint16_t SidVm::addrZp() { return fetch8(); }
uint16_t SidVm::addrZpX() { return (uint8_t)(fetch8() + _cpu.x); }
uint16_t SidVm::addrZpY() { return (uint8_t)(fetch8() + _cpu.y); }
uint16_t SidVm::addrAbs() { return fetch16(); }
uint16_t SidVm::addrAbsX() { return (uint16_t)(fetch16() + _cpu.x); }
uint16_t SidVm::addrAbsY() { return (uint16_t)(fetch16() + _cpu.y); }

uint16_t SidVm::addrIndX() {
    uint8_t zp = (uint8_t)(fetch8() + _cpu.x);
    uint8_t lo = readZp(zp);
    uint8_t hi = readZp((uint8_t)(zp + 1));
    return (uint16_t)lo | ((uint16_t)hi << 8);
}

uint16_t SidVm::addrIndY() {
    uint8_t zp = fetch8();
    uint8_t lo = readZp(zp);
    uint8_t hi = readZp((uint8_t)(zp + 1));
    return (uint16_t)((uint16_t)lo | ((uint16_t)hi << 8)) + _cpu.y;
}

bool SidVm::push(uint8_t value) {
    bool ok = write((uint16_t)(0x0100 | _cpu.sp), value);
    _cpu.sp--;
    return ok;
}

uint8_t SidVm::pop() {
    _cpu.sp++;
    return read((uint16_t)(0x0100 | _cpu.sp));
}

bool SidVm::rts() {
    uint8_t lo = pop();
    uint8_t hi = pop();
    _cpu.pc = (uint16_t)(((uint16_t)lo | ((uint16_t)hi << 8)) + 1);
    return true;
}

bool SidVm::rti() {
    _cpu.p = pop() | FlagU;
    uint8_t lo = pop();
    uint8_t hi = pop();
    _cpu.pc = (uint16_t)lo | ((uint16_t)hi << 8);
    return true;
}

void SidVm::setZN(uint8_t value) {
    setFlag(FlagZ, value == 0);
    setFlag(FlagN, (value & 0x80) != 0);
}

void SidVm::setFlag(uint8_t bit, bool on) {
    if (on)
        _cpu.p |= bit;
    else
        _cpu.p &= (uint8_t)~bit;
    _cpu.p |= FlagU;
}

void SidVm::adc(uint8_t value) {
    uint8_t a0 = _cpu.a;
    uint16_t sum = (uint16_t)_cpu.a + value + (flag(FlagC) ? 1 : 0);
    setFlag(FlagV, (~(a0 ^ value) & (a0 ^ sum) & 0x80) != 0);

    if (flag(FlagD)) {
        uint16_t lo = (a0 & 0x0F) + (value & 0x0F) + (flag(FlagC) ? 1 : 0);
        uint16_t hi = (a0 >> 4) + (value >> 4);
        if (lo > 9) {
            lo += 6;
            hi++;
        }
        if (hi > 9)
            hi += 6;
        setFlag(FlagC, hi > 0x0F);
        _cpu.a = (uint8_t)((hi << 4) | (lo & 0x0F));
    } else {
        setFlag(FlagC, sum > 0xFF);
        _cpu.a = (uint8_t)sum;
    }
    setZN(_cpu.a);
}

void SidVm::sbc(uint8_t value) {
    adc((uint8_t)~value);
}

void SidVm::cmp(uint8_t reg, uint8_t value) {
    uint16_t diff = (uint16_t)reg - value;
    setFlag(FlagC, reg >= value);
    setZN((uint8_t)diff);
}

void SidVm::testBits(uint8_t value) {
    setFlag(FlagZ, (_cpu.a & value) == 0);
    setFlag(FlagN, (value & 0x80) != 0);
    setFlag(FlagV, (value & 0x40) != 0);
}

uint8_t SidVm::asl(uint8_t value) {
    setFlag(FlagC, (value & 0x80) != 0);
    value <<= 1;
    setZN(value);
    return value;
}

uint8_t SidVm::lsr(uint8_t value) {
    setFlag(FlagC, (value & 0x01) != 0);
    value >>= 1;
    setZN(value);
    return value;
}

uint8_t SidVm::rol(uint8_t value) {
    bool c = flag(FlagC);
    setFlag(FlagC, (value & 0x80) != 0);
    value = (uint8_t)((value << 1) | (c ? 1 : 0));
    setZN(value);
    return value;
}

uint8_t SidVm::ror(uint8_t value) {
    bool c = flag(FlagC);
    setFlag(FlagC, (value & 0x01) != 0);
    value = (uint8_t)((value >> 1) | (c ? 0x80 : 0));
    setZN(value);
    return value;
}

void SidVm::branch(bool take) {
    int8_t rel = (int8_t)fetch8();
    if (take)
        _cpu.pc = (uint16_t)(_cpu.pc + rel);
}

void SidVm::nopRead(uint16_t addr) {
    (void)read(addr);
}

RunStatus SidVm::step(uint8_t& opcode, uint32_t& cycles) {
    uint16_t opcode_pc = _cpu.pc;
    opcode = fetch8();
    cycles = 2;

    auto ldA = [&](uint8_t v) { _cpu.a = v; setZN(_cpu.a); };
    auto ldX = [&](uint8_t v) { _cpu.x = v; setZN(_cpu.x); };
    auto ldY = [&](uint8_t v) { _cpu.y = v; setZN(_cpu.y); };
    auto store = [&](uint16_t addr, uint8_t value) -> RunStatus {
        return write(addr, value) ? RunStatus::Ok : RunStatus::OutOfMemory;
    };
    auto rmw_op = [&](uint16_t addr, uint8_t (SidVm::*op)(uint8_t)) -> RunStatus {
        uint8_t v = read(addr);
        v = (this->*op)(v);
        return rmw(addr, v) ? RunStatus::Ok : RunStatus::OutOfMemory;
    };

    switch (opcode) {
        case 0x00: _cpu.pc++; push((uint8_t)(_cpu.pc >> 8)); push((uint8_t)_cpu.pc); push(_cpu.p | FlagB); setFlag(FlagI, true); _cpu.pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8); cycles = 7; break;
        case 0x01: ldA(_cpu.a | read(addrIndX())); cycles = 6; break;
        case 0x05: ldA(_cpu.a | read(addrZp())); cycles = 3; break;
        case 0x06: return rmw_op(addrZp(), &SidVm::asl);
        case 0x08: if (!push(_cpu.p | FlagB | FlagU)) return RunStatus::OutOfMemory; cycles = 3; break;
        case 0x09: ldA(_cpu.a | fetch8()); break;
        case 0x0A: _cpu.a = asl(_cpu.a); break;
        case 0x0D: ldA(_cpu.a | read(addrAbs())); cycles = 4; break;
        case 0x0E: return rmw_op(addrAbs(), &SidVm::asl);
        case 0x10: branch(!flag(FlagN)); cycles = 3; break;
        case 0x11: ldA(_cpu.a | read(addrIndY())); cycles = 5; break;
        case 0x15: ldA(_cpu.a | read(addrZpX())); cycles = 4; break;
        case 0x16: return rmw_op(addrZpX(), &SidVm::asl);
        case 0x18: setFlag(FlagC, false); break;
        case 0x19: ldA(_cpu.a | read(addrAbsY())); cycles = 4; break;
        case 0x1D: ldA(_cpu.a | read(addrAbsX())); cycles = 4; break;
        case 0x1E: return rmw_op(addrAbsX(), &SidVm::asl);

        case 0x20: { uint16_t addr = addrAbs(); uint16_t ret = (uint16_t)(_cpu.pc - 1); if (!push((uint8_t)(ret >> 8)) || !push((uint8_t)ret)) return RunStatus::OutOfMemory; _cpu.pc = addr; cycles = 6; break; }
        case 0x21: ldA(_cpu.a & read(addrIndX())); cycles = 6; break;
        case 0x24: testBits(read(addrZp())); cycles = 3; break;
        case 0x25: ldA(_cpu.a & read(addrZp())); cycles = 3; break;
        case 0x26: return rmw_op(addrZp(), &SidVm::rol);
        case 0x28: _cpu.p = pop() | FlagU; cycles = 4; break;
        case 0x29: ldA(_cpu.a & fetch8()); break;
        case 0x2A: _cpu.a = rol(_cpu.a); break;
        case 0x2C: testBits(read(addrAbs())); cycles = 4; break;
        case 0x2D: ldA(_cpu.a & read(addrAbs())); cycles = 4; break;
        case 0x2E: return rmw_op(addrAbs(), &SidVm::rol);
        case 0x30: branch(flag(FlagN)); cycles = 3; break;
        case 0x31: ldA(_cpu.a & read(addrIndY())); cycles = 5; break;
        case 0x35: ldA(_cpu.a & read(addrZpX())); cycles = 4; break;
        case 0x36: return rmw_op(addrZpX(), &SidVm::rol);
        case 0x38: setFlag(FlagC, true); break;
        case 0x39: ldA(_cpu.a & read(addrAbsY())); cycles = 4; break;
        case 0x3D: ldA(_cpu.a & read(addrAbsX())); cycles = 4; break;
        case 0x3E: return rmw_op(addrAbsX(), &SidVm::rol);

        case 0x40: rti(); cycles = 6; break;
        case 0x41: ldA(_cpu.a ^ read(addrIndX())); cycles = 6; break;
        case 0x45: ldA(_cpu.a ^ read(addrZp())); cycles = 3; break;
        case 0x46: return rmw_op(addrZp(), &SidVm::lsr);
        case 0x48: if (!push(_cpu.a)) return RunStatus::OutOfMemory; cycles = 3; break;
        case 0x49: ldA(_cpu.a ^ fetch8()); break;
        case 0x4A: _cpu.a = lsr(_cpu.a); break;
        case 0x4C: _cpu.pc = addrAbs(); cycles = 3; break;
        case 0x4D: ldA(_cpu.a ^ read(addrAbs())); cycles = 4; break;
        case 0x4E: return rmw_op(addrAbs(), &SidVm::lsr);
        case 0x50: branch(!flag(FlagV)); cycles = 3; break;
        case 0x51: ldA(_cpu.a ^ read(addrIndY())); cycles = 5; break;
        case 0x55: ldA(_cpu.a ^ read(addrZpX())); cycles = 4; break;
        case 0x56: return rmw_op(addrZpX(), &SidVm::lsr);
        case 0x58: setFlag(FlagI, false); break;
        case 0x59: ldA(_cpu.a ^ read(addrAbsY())); cycles = 4; break;
        case 0x5D: ldA(_cpu.a ^ read(addrAbsX())); cycles = 4; break;
        case 0x5E: return rmw_op(addrAbsX(), &SidVm::lsr);

        case 0x60: rts(); cycles = 6; break;
        case 0x61: adc(read(addrIndX())); cycles = 6; break;
        case 0x65: adc(read(addrZp())); cycles = 3; break;
        case 0x66: return rmw_op(addrZp(), &SidVm::ror);
        case 0x68: _cpu.a = pop(); setZN(_cpu.a); cycles = 4; break;
        case 0x69: adc(fetch8()); break;
        case 0x6A: _cpu.a = ror(_cpu.a); break;
        case 0x6C: { uint16_t p = addrAbs(); uint8_t lo = read(p); uint8_t hi = read((uint16_t)((p & 0xFF00) | ((p + 1) & 0x00FF))); _cpu.pc = (uint16_t)lo | ((uint16_t)hi << 8); cycles = 5; break; }
        case 0x6D: adc(read(addrAbs())); cycles = 4; break;
        case 0x6E: return rmw_op(addrAbs(), &SidVm::ror);
        case 0x70: branch(flag(FlagV)); cycles = 3; break;
        case 0x71: adc(read(addrIndY())); cycles = 5; break;
        case 0x75: adc(read(addrZpX())); cycles = 4; break;
        case 0x76: return rmw_op(addrZpX(), &SidVm::ror);
        case 0x78: setFlag(FlagI, true); break;
        case 0x79: adc(read(addrAbsY())); cycles = 4; break;
        case 0x7D: adc(read(addrAbsX())); cycles = 4; break;
        case 0x7E: return rmw_op(addrAbsX(), &SidVm::ror);

        case 0x81: return store(addrIndX(), _cpu.a);
        case 0x84: return store(addrZp(), _cpu.y);
        case 0x85: return store(addrZp(), _cpu.a);
        case 0x86: return store(addrZp(), _cpu.x);
        case 0x88: _cpu.y--; setZN(_cpu.y); break;
        case 0x8A: _cpu.a = _cpu.x; setZN(_cpu.a); break;
        case 0x8C: return store(addrAbs(), _cpu.y);
        case 0x8D: return store(addrAbs(), _cpu.a);
        case 0x8E: return store(addrAbs(), _cpu.x);
        case 0x90: branch(!flag(FlagC)); cycles = 3; break;
        case 0x91: return store(addrIndY(), _cpu.a);
        case 0x94: return store(addrZpX(), _cpu.y);
        case 0x95: return store(addrZpX(), _cpu.a);
        case 0x96: return store(addrZpY(), _cpu.x);
        case 0x98: _cpu.a = _cpu.y; setZN(_cpu.a); break;
        case 0x99: return store(addrAbsY(), _cpu.a);
        case 0x9A: _cpu.sp = _cpu.x; break;
        case 0x9D: return store(addrAbsX(), _cpu.a);

        case 0xA0: ldY(fetch8()); break;
        case 0xA1: ldA(read(addrIndX())); cycles = 6; break;
        case 0xA2: ldX(fetch8()); break;
        case 0xA4: ldY(read(addrZp())); cycles = 3; break;
        case 0xA5: ldA(read(addrZp())); cycles = 3; break;
        case 0xA6: ldX(read(addrZp())); cycles = 3; break;
        case 0xA8: _cpu.y = _cpu.a; setZN(_cpu.y); break;
        case 0xA9: ldA(fetch8()); break;
        case 0xAA: _cpu.x = _cpu.a; setZN(_cpu.x); break;
        case 0xAC: ldY(read(addrAbs())); cycles = 4; break;
        case 0xAD: ldA(read(addrAbs())); cycles = 4; break;
        case 0xAE: ldX(read(addrAbs())); cycles = 4; break;
        case 0xB0: branch(flag(FlagC)); cycles = 3; break;
        case 0xB1: ldA(read(addrIndY())); cycles = 5; break;
        case 0xB4: ldY(read(addrZpX())); cycles = 4; break;
        case 0xB5: ldA(read(addrZpX())); cycles = 4; break;
        case 0xB6: ldX(read(addrZpY())); cycles = 4; break;
        case 0xB8: setFlag(FlagV, false); break;
        case 0xB9: ldA(read(addrAbsY())); cycles = 4; break;
        case 0xBA: _cpu.x = _cpu.sp; setZN(_cpu.x); break;
        case 0xBC: ldY(read(addrAbsX())); cycles = 4; break;
        case 0xBD: ldA(read(addrAbsX())); cycles = 4; break;
        case 0xBE: ldX(read(addrAbsY())); cycles = 4; break;

        case 0xC0: cmp(_cpu.y, fetch8()); break;
        case 0xC1: cmp(_cpu.a, read(addrIndX())); cycles = 6; break;
        case 0xC4: cmp(_cpu.y, read(addrZp())); cycles = 3; break;
        case 0xC5: cmp(_cpu.a, read(addrZp())); cycles = 3; break;
        case 0xC6: { uint16_t a = addrZp(); uint8_t v = read(a) - 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xC8: _cpu.y++; setZN(_cpu.y); break;
        case 0xC9: cmp(_cpu.a, fetch8()); break;
        case 0xCA: _cpu.x--; setZN(_cpu.x); break;
        case 0xCC: cmp(_cpu.y, read(addrAbs())); cycles = 4; break;
        case 0xCD: cmp(_cpu.a, read(addrAbs())); cycles = 4; break;
        case 0xCE: { uint16_t a = addrAbs(); uint8_t v = read(a) - 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xD0: branch(!flag(FlagZ)); cycles = 3; break;
        case 0xD1: cmp(_cpu.a, read(addrIndY())); cycles = 5; break;
        case 0xD5: cmp(_cpu.a, read(addrZpX())); cycles = 4; break;
        case 0xD6: { uint16_t a = addrZpX(); uint8_t v = read(a) - 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xD8: setFlag(FlagD, false); break;
        case 0xD9: cmp(_cpu.a, read(addrAbsY())); cycles = 4; break;
        case 0xDD: cmp(_cpu.a, read(addrAbsX())); cycles = 4; break;
        case 0xDE: { uint16_t a = addrAbsX(); uint8_t v = read(a) - 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }

        case 0xE0: cmp(_cpu.x, fetch8()); break;
        case 0xE1: sbc(read(addrIndX())); cycles = 6; break;
        case 0xE4: cmp(_cpu.x, read(addrZp())); cycles = 3; break;
        case 0xE5: sbc(read(addrZp())); cycles = 3; break;
        case 0xE6: { uint16_t a = addrZp(); uint8_t v = read(a) + 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xE8: _cpu.x++; setZN(_cpu.x); break;
        case 0xE9: sbc(fetch8()); break;
        case 0xEA: break;
        case 0xEC: cmp(_cpu.x, read(addrAbs())); cycles = 4; break;
        case 0xED: sbc(read(addrAbs())); cycles = 4; break;
        case 0xEE: { uint16_t a = addrAbs(); uint8_t v = read(a) + 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xF0: branch(flag(FlagZ)); cycles = 3; break;
        case 0xF1: sbc(read(addrIndY())); cycles = 5; break;
        case 0xF5: sbc(read(addrZpX())); cycles = 4; break;
        case 0xF6: { uint16_t a = addrZpX(); uint8_t v = read(a) + 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }
        case 0xF8: setFlag(FlagD, true); break;
        case 0xF9: sbc(read(addrAbsY())); cycles = 4; break;
        case 0xFD: sbc(read(addrAbsX())); cycles = 4; break;
        case 0xFE: { uint16_t a = addrAbsX(); uint8_t v = read(a) + 1; setZN(v); return rmw(a, v) ? RunStatus::Ok : RunStatus::OutOfMemory; }

        default: {
            RunStatus status = RunStatus::Ok;
            if (!executeUnofficial(opcode, status)) {
                _cpu.pc = opcode_pc;
                return status;
            }
            break;
        }
    }

    return RunStatus::Ok;
}

bool SidVm::executeUnofficial(uint8_t opcode, RunStatus& status) {
    auto store = [&](uint16_t addr, uint8_t value) -> bool {
        if (!write(addr, value)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        return true;
    };
    auto lax = [&](uint8_t value) {
        _cpu.a = value;
        _cpu.x = value;
        setZN(value);
    };
    auto sax = [&](uint16_t addr) -> bool {
        return store(addr, (uint8_t)(_cpu.a & _cpu.x));
    };
    auto dcp = [&](uint16_t addr) -> bool {
        uint8_t v = read(addr) - 1;
        setZN(v);
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        cmp(_cpu.a, v);
        return true;
    };
    auto isc = [&](uint16_t addr) -> bool {
        uint8_t v = read(addr) + 1;
        setZN(v);
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        sbc(v);
        return true;
    };
    auto slo = [&](uint16_t addr) -> bool {
        uint8_t v = asl(read(addr));
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        _cpu.a |= v;
        setZN(_cpu.a);
        return true;
    };
    auto rla = [&](uint16_t addr) -> bool {
        uint8_t v = rol(read(addr));
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        _cpu.a &= v;
        setZN(_cpu.a);
        return true;
    };
    auto sre = [&](uint16_t addr) -> bool {
        uint8_t v = lsr(read(addr));
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        _cpu.a ^= v;
        setZN(_cpu.a);
        return true;
    };
    auto rra = [&](uint16_t addr) -> bool {
        uint8_t v = ror(read(addr));
        if (!rmw(addr, v)) {
            status = RunStatus::OutOfMemory;
            return false;
        }
        adc(v);
        return true;
    };

    switch (opcode) {
        case 0x1A: case 0x3A: case 0x5A: case 0x7A:
        case 0xDA: case 0xFA:
            return true;
        case 0x04: case 0x44: case 0x64: nopRead(addrZp()); return true;
        case 0x14: case 0x34: case 0x54: case 0x74:
        case 0xD4: case 0xF4: nopRead(addrZpX()); return true;
        case 0x0C: nopRead(addrAbs()); return true;
        case 0x1C: case 0x3C: case 0x5C: case 0x7C:
        case 0xDC: case 0xFC: nopRead(addrAbsX()); return true;

        case 0xA3: lax(read(addrIndX())); return true;
        case 0xA7: lax(read(addrZp())); return true;
        case 0xAF: lax(read(addrAbs())); return true;
        case 0xB3: lax(read(addrIndY())); return true;
        case 0xB7: lax(read(addrZpY())); return true;
        case 0xBF: lax(read(addrAbsY())); return true;

        case 0x83: return sax(addrIndX());
        case 0x87: return sax(addrZp());
        case 0x8F: return sax(addrAbs());
        case 0x97: return sax(addrZpY());

        case 0xC3: return dcp(addrIndX());
        case 0xC7: return dcp(addrZp());
        case 0xCF: return dcp(addrAbs());
        case 0xD3: return dcp(addrIndY());
        case 0xD7: return dcp(addrZpX());
        case 0xDB: return dcp(addrAbsY());
        case 0xDF: return dcp(addrAbsX());

        case 0xE3: return isc(addrIndX());
        case 0xE7: return isc(addrZp());
        case 0xEF: return isc(addrAbs());
        case 0xF3: return isc(addrIndY());
        case 0xF7: return isc(addrZpX());
        case 0xFB: return isc(addrAbsY());
        case 0xFF: return isc(addrAbsX());

        case 0x03: return slo(addrIndX());
        case 0x07: return slo(addrZp());
        case 0x0F: return slo(addrAbs());
        case 0x13: return slo(addrIndY());
        case 0x17: return slo(addrZpX());
        case 0x1B: return slo(addrAbsY());
        case 0x1F: return slo(addrAbsX());

        case 0x23: return rla(addrIndX());
        case 0x27: return rla(addrZp());
        case 0x2F: return rla(addrAbs());
        case 0x33: return rla(addrIndY());
        case 0x37: return rla(addrZpX());
        case 0x3B: return rla(addrAbsY());
        case 0x3F: return rla(addrAbsX());

        case 0x43: return sre(addrIndX());
        case 0x47: return sre(addrZp());
        case 0x4F: return sre(addrAbs());
        case 0x53: return sre(addrIndY());
        case 0x57: return sre(addrZpX());
        case 0x5B: return sre(addrAbsY());
        case 0x5F: return sre(addrAbsX());

        case 0x63: return rra(addrIndX());
        case 0x67: return rra(addrZp());
        case 0x6F: return rra(addrAbs());
        case 0x73: return rra(addrIndY());
        case 0x77: return rra(addrZpX());
        case 0x7B: return rra(addrAbsY());
        case 0x7F: return rra(addrAbsX());

        case 0x0B: case 0x2B:
            _cpu.a &= fetch8();
            setZN(_cpu.a);
            setFlag(FlagC, (_cpu.a & 0x80) != 0);
            return true;
        case 0x4B:
            _cpu.a &= fetch8();
            _cpu.a = lsr(_cpu.a);
            return true;
        case 0x6B: {
            _cpu.a &= fetch8();
            bool old_c = flag(FlagC);
            _cpu.a = (uint8_t)((_cpu.a >> 1) | (old_c ? 0x80 : 0));
            setZN(_cpu.a);
            setFlag(FlagC, (_cpu.a & 0x40) != 0);
            setFlag(FlagV, ((_cpu.a >> 6) ^ (_cpu.a >> 5)) & 1);
            return true;
        }
        case 0xCB: {
            uint8_t v = fetch8();
            _cpu.x = (uint8_t)((_cpu.a & _cpu.x) - v);
            setZN(_cpu.x);
            setFlag(FlagC, (_cpu.a & _cpu.x) >= v);
            return true;
        }
        default:
            status = RunStatus::UnsupportedOpcode;
            return false;
    }
}

const char* run_status_name(RunStatus status) {
    switch (status) {
        case RunStatus::Ok: return "ok";
        case RunStatus::Hung: return "hung";
        case RunStatus::OutOfMemory: return "out_of_memory";
        case RunStatus::UnsupportedOpcode: return "unsupported_opcode";
        case RunStatus::UnsupportedRomCall: return "unsupported_rom_call";
        case RunStatus::InternalError: return "internal_error";
        default: return "unknown";
    }
}

} // namespace nova_sid
