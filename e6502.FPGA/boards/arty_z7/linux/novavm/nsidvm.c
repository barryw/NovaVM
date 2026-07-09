/* nsidvm.c — C port of e6502.ESP32/novahost/sid_vm.cpp (nova_sid::SidVm).
 *
 * A faithful, line-for-line translation of the tested ESP/ULX3S SID VM: PSID/RSID
 * header parse, sparse 64 KB memory, a 6502 interpreter (official + the unofficial
 * opcodes real SID tunes lean on), C64 KERNAL/CIA/VIC stubs, and $D400/$D420 write
 * capture. The only substantive change from the C++ source is the mechanical
 * C-ification (class -> struct + file-static singleton; lambdas -> static
 * helpers; member-fn-pointer RMW -> plain fn pointers). Behaviour is unchanged.
 *
 * ESP-only bits dropped: Arduino yield() (was cooperative-scheduler-only and
 * #if defined(ARDUINO) guarded already) and ESP logging (this file logs nothing;
 * the naudio.c glue owns user-facing messages). */
#include "nsidvm.h"

#include <stdlib.h>
#include <string.h>

/* ---- header parse / frame timing (nova_sid free functions) ----------------- */

#define SID_CLOCK_NTSC       0x02
#define SID_MODEL_8580       0x02
#define FPGA_CFG_MODEL_8580  0x01
#define FPGA_CFG_CLOCK_NTSC  0x02

static uint16_t read_be16(const uint8_t *d, size_t o) {
    return ((uint16_t)d[o] << 8) | (uint16_t)d[o + 1];
}
static uint32_t read_be32(const uint8_t *d, size_t o) {
    return ((uint32_t)d[o] << 24) | ((uint32_t)d[o + 1] << 16) |
           ((uint32_t)d[o + 2] << 8) | (uint32_t)d[o + 3];
}

int nsid_parse_header(const uint8_t *header, uint32_t file_size, nsid_info_t *info) {
    memset(info, 0, sizeof(*info));
    if (!header || file_size < 124)
        return 0;

    int psid = memcmp(header, "PSID", 4) == 0;
    int rsid = memcmp(header, "RSID", 4) == 0;
    if (!psid && !rsid)
        return 0;

    info->rsid = rsid;
    info->data_offset = read_be16(header, 6);
    info->load_addr   = read_be16(header, 8);
    info->init_addr   = read_be16(header, 10);
    info->play_addr   = read_be16(header, 12);
    info->songs       = read_be16(header, 14);
    info->start_song  = read_be16(header, 16);
    info->speed       = read_be32(header, 18);
    info->flags       = info->data_offset >= 0x7C ? read_be16(header, 0x76) : 0;
    info->load_in_payload = info->load_addr == 0;

    if (info->data_offset < 124 || info->data_offset > file_size)
        return 0;
    if (info->load_in_payload && info->data_offset + 2 > file_size)
        return 0;

    info->payload_offset = info->data_offset + (info->load_in_payload ? 2 : 0);
    if (info->payload_offset > file_size)
        return 0;

    info->payload_bytes = file_size - info->payload_offset;
    info->valid = info->payload_bytes > 0;
    return info->valid;
}

static int sid_uses_cia_timing(const nsid_info_t *info, uint8_t song) {
    if (song == 0)
        return 0;
    return (info->speed & (1UL << (song - 1))) != 0;
}

uint32_t nsid_frame_period_us(const nsid_info_t *info, uint8_t song) {
    if (sid_uses_cia_timing(info, song))
        return 1000000UL / 60UL;
    uint8_t clock = (uint8_t)((info->flags >> 2) & 0x03);
    return clock == SID_CLOCK_NTSC ? (1000000UL / 60UL) : (1000000UL / 50UL);
}

uint8_t nsid_fpga_config(const nsid_info_t *info) {
    uint8_t cfg = 0;
    uint8_t clock = (uint8_t)((info->flags >> 2) & 0x03);
    uint8_t model = (uint8_t)((info->flags >> 4) & 0x03);
    if (clock == SID_CLOCK_NTSC) cfg |= FPGA_CFG_CLOCK_NTSC;
    if (model == SID_MODEL_8580) cfg |= FPGA_CFG_MODEL_8580;
    return cfg;
}

/* ---- sparse memory + CPU state --------------------------------------------- */

#define TRAP_ADDRESS     0x03D0u
#define MAX_INIT_CYCLES  1000000u
#define MAX_PLAY_CYCLES  50000u

#define F_C 0x01u
#define F_Z 0x02u
#define F_I 0x04u
#define F_D 0x08u
#define F_B 0x10u
#define F_U 0x20u
#define F_V 0x40u
#define F_N 0x80u

typedef struct {
    uint8_t bytes[256];
    uint8_t written[32];
} nsid_page;

typedef struct {
    /* sparse memory */
    nsid_page *pages[256];
    uint16_t   page_count;
    /* cpu */
    uint8_t  a, x, y, sp, p;
    uint16_t pc;
    /* tune entry points + frame counter */
    uint16_t init_addr, play_addr;
    uint32_t frame;
    /* device shadows */
    uint8_t  sid1[32];
    uint8_t  sid2[32];
    uint8_t  io[512];
    uint16_t cia_timer;
    uint32_t rng;
    /* write capture */
    nsid_write_fn sid_write_cb;
    void         *sid_write_user;
    /* result of last run */
    uint16_t last_pc;
    uint8_t  last_opcode;
} nsid_vm;

static nsid_vm g_vm;

/* ---- sparse memory --------------------------------------------------------- */

static void mem_clear(nsid_vm *vm) {
    for (int i = 0; i < 256; i++) {
        if (vm->pages[i]) { free(vm->pages[i]); vm->pages[i] = NULL; }
    }
    vm->page_count = 0;
}

static nsid_page *ensure_page(nsid_vm *vm, uint8_t page) {
    if (vm->pages[page]) return vm->pages[page];
    nsid_page *p = (nsid_page *)malloc(sizeof(nsid_page));
    if (!p) return NULL;
    memset(p, 0, sizeof(*p));
    vm->pages[page] = p;
    vm->page_count++;
    return p;
}

static int mem_write(nsid_vm *vm, uint16_t addr, uint8_t value) {
    nsid_page *page = ensure_page(vm, (uint8_t)(addr >> 8));
    if (!page) return 0;
    uint8_t off = addr & 0xFF;
    page->bytes[off] = value;
    page->written[off >> 3] |= (uint8_t)(1u << (off & 7));
    return 1;
}

static uint8_t mem_read(const nsid_vm *vm, uint16_t addr) {
    const nsid_page *page = vm->pages[addr >> 8];
    return page ? page->bytes[addr & 0xFF] : 0;
}

static int mem_load(nsid_vm *vm, uint16_t addr, const uint8_t *data, size_t len) {
    if (!data && len != 0) return 0;
    for (size_t i = 0; i < len; i++)
        if (!mem_write(vm, (uint16_t)(addr + i), data[i])) return 0;
    return 1;
}

static int addr_written(const nsid_vm *vm, uint16_t addr) {
    const nsid_page *page = vm->pages[addr >> 8];
    if (!page) return 0;
    uint8_t off = addr & 0xFF;
    return (page->written[off >> 3] & (uint8_t)(1u << (off & 7))) != 0;
}

/* ---- bus read/write (device decode) ---------------------------------------- */

static int write_sid(nsid_vm *vm, uint16_t addr, uint8_t value) {
    uint16_t mapped;
    uint8_t *shadow;
    if (addr >= 0xD400 && addr <= 0xD41F)      { mapped = addr; shadow = vm->sid1; }
    else if (addr >= 0xD420 && addr <= 0xD43F) { mapped = addr; shadow = vm->sid2; }
    else if (addr >= 0xD500 && addr <= 0xD51F) { mapped = (uint16_t)(0xD420 + (addr & 0x1F)); shadow = vm->sid2; }
    else return 0;

    shadow[mapped & 0x1F] = value;
    return !vm->sid_write_cb || vm->sid_write_cb(vm->sid_write_user, mapped, value);
}

static uint8_t bus_read(nsid_vm *vm, uint16_t addr) {
    if (addr >= 0xD400 && addr <= 0xD41F) {
        if (addr == 0xD41B || addr == 0xD41C) {
            vm->rng = vm->rng * 1664525UL + 1013904223UL;
            return (uint8_t)(vm->rng >> 24);
        }
        return vm->sid1[addr & 0x1F];
    }
    if (addr >= 0xD420 && addr <= 0xD43F) return vm->sid2[addr & 0x1F];
    if (addr >= 0xD500 && addr <= 0xD51F) return vm->sid2[addr & 0x1F];
    if (addr >= 0xD000 && addr <= 0xD1FF) {
        if (addr == 0xD012) return (uint8_t)(vm->frame & 0xFF);
        if (addr == 0xD019) return vm->io[addr & 0x1FF] | 0x01;
        return vm->io[addr & 0x1FF];
    }
    if (addr >= 0xDC00 && addr <= 0xDDFF) {
        uint16_t off = addr & 0x1FF;
        if ((off & 0x0F) == 0x04) return (uint8_t)(vm->cia_timer & 0xFF);
        if ((off & 0x0F) == 0x05) return (uint8_t)(vm->cia_timer >> 8);
        return vm->io[off];
    }
    return mem_read(vm, addr);
}

static int bus_write(nsid_vm *vm, uint16_t addr, uint8_t value) {
    if ((addr >= 0xD400 && addr <= 0xD41F) ||
        (addr >= 0xD420 && addr <= 0xD43F) ||
        (addr >= 0xD500 && addr <= 0xD51F)) {
        return write_sid(vm, addr, value);
    }
    if (addr >= 0xD000 && addr <= 0xD1FF) { vm->io[addr & 0x1FF] = value; return 1; }
    if (addr >= 0xDC00 && addr <= 0xDDFF) {
        vm->io[addr & 0x1FF] = value;
        if ((addr & 0x0F) == 0x04)      vm->cia_timer = (vm->cia_timer & 0xFF00) | value;
        else if ((addr & 0x0F) == 0x05) vm->cia_timer = (vm->cia_timer & 0x00FF) | ((uint16_t)value << 8);
        return 1;
    }
    return mem_write(vm, addr, value);
}

static int rmw(nsid_vm *vm, uint16_t addr, uint8_t value) { return bus_write(vm, addr, value); }

/* ---- fetch / addressing / stack -------------------------------------------- */

static uint8_t fetch8(nsid_vm *vm) { uint8_t v = bus_read(vm, vm->pc); vm->pc++; return v; }
static uint16_t fetch16(nsid_vm *vm) {
    uint8_t lo = fetch8(vm), hi = fetch8(vm);
    return (uint16_t)lo | ((uint16_t)hi << 8);
}
static uint8_t read_zp(nsid_vm *vm, uint8_t addr) { return bus_read(vm, addr); }

static uint16_t a_zp(nsid_vm *vm)   { return fetch8(vm); }
static uint16_t a_zpx(nsid_vm *vm)  { return (uint8_t)(fetch8(vm) + vm->x); }
static uint16_t a_zpy(nsid_vm *vm)  { return (uint8_t)(fetch8(vm) + vm->y); }
static uint16_t a_abs(nsid_vm *vm)  { return fetch16(vm); }
static uint16_t a_absx(nsid_vm *vm) { return (uint16_t)(fetch16(vm) + vm->x); }
static uint16_t a_absy(nsid_vm *vm) { return (uint16_t)(fetch16(vm) + vm->y); }
static uint16_t a_indx(nsid_vm *vm) {
    uint8_t zp = (uint8_t)(fetch8(vm) + vm->x);
    uint8_t lo = read_zp(vm, zp), hi = read_zp(vm, (uint8_t)(zp + 1));
    return (uint16_t)lo | ((uint16_t)hi << 8);
}
static uint16_t a_indy(nsid_vm *vm) {
    uint8_t zp = fetch8(vm);
    uint8_t lo = read_zp(vm, zp), hi = read_zp(vm, (uint8_t)(zp + 1));
    return (uint16_t)((uint16_t)lo | ((uint16_t)hi << 8)) + vm->y;
}

static int push(nsid_vm *vm, uint8_t value) {
    int ok = bus_write(vm, (uint16_t)(0x0100 | vm->sp), value);
    vm->sp--;
    return ok;
}
static uint8_t pop(nsid_vm *vm) { vm->sp++; return bus_read(vm, (uint16_t)(0x0100 | vm->sp)); }
static int rts(nsid_vm *vm) {
    uint8_t lo = pop(vm), hi = pop(vm);
    vm->pc = (uint16_t)(((uint16_t)lo | ((uint16_t)hi << 8)) + 1);
    return 1;
}
static int rti(nsid_vm *vm) {
    vm->p = pop(vm) | F_U;
    uint8_t lo = pop(vm), hi = pop(vm);
    vm->pc = (uint16_t)lo | ((uint16_t)hi << 8);
    return 1;
}

/* ---- flags / ALU ----------------------------------------------------------- */

static int flag(const nsid_vm *vm, uint8_t bit) { return (vm->p & bit) != 0; }
static void set_flag(nsid_vm *vm, uint8_t bit, int on) {
    if (on) vm->p |= bit; else vm->p &= (uint8_t)~bit;
    vm->p |= F_U;
}
static void set_zn(nsid_vm *vm, uint8_t value) {
    set_flag(vm, F_Z, value == 0);
    set_flag(vm, F_N, (value & 0x80) != 0);
}

static void adc(nsid_vm *vm, uint8_t value) {
    uint8_t a0 = vm->a;
    uint16_t sum = (uint16_t)vm->a + value + (flag(vm, F_C) ? 1 : 0);
    set_flag(vm, F_V, (~(a0 ^ value) & (a0 ^ sum) & 0x80) != 0);
    if (flag(vm, F_D)) {
        uint16_t lo = (a0 & 0x0F) + (value & 0x0F) + (flag(vm, F_C) ? 1 : 0);
        uint16_t hi = (a0 >> 4) + (value >> 4);
        if (lo > 9) { lo += 6; hi++; }
        if (hi > 9) hi += 6;
        set_flag(vm, F_C, hi > 0x0F);
        vm->a = (uint8_t)((hi << 4) | (lo & 0x0F));
    } else {
        set_flag(vm, F_C, sum > 0xFF);
        vm->a = (uint8_t)sum;
    }
    set_zn(vm, vm->a);
}
static void sbc(nsid_vm *vm, uint8_t value) { adc(vm, (uint8_t)~value); }
static void cmp_op(nsid_vm *vm, uint8_t reg, uint8_t value) {
    uint16_t diff = (uint16_t)reg - value;
    set_flag(vm, F_C, reg >= value);
    set_zn(vm, (uint8_t)diff);
}
static void test_bits(nsid_vm *vm, uint8_t value) {
    set_flag(vm, F_Z, (vm->a & value) == 0);
    set_flag(vm, F_N, (value & 0x80) != 0);
    set_flag(vm, F_V, (value & 0x40) != 0);
}
static uint8_t op_asl(nsid_vm *vm, uint8_t v) { set_flag(vm, F_C, (v & 0x80) != 0); v <<= 1; set_zn(vm, v); return v; }
static uint8_t op_lsr(nsid_vm *vm, uint8_t v) { set_flag(vm, F_C, (v & 0x01) != 0); v >>= 1; set_zn(vm, v); return v; }
static uint8_t op_rol(nsid_vm *vm, uint8_t v) {
    int c = flag(vm, F_C);
    set_flag(vm, F_C, (v & 0x80) != 0);
    v = (uint8_t)((v << 1) | (c ? 1 : 0)); set_zn(vm, v); return v;
}
static uint8_t op_ror(nsid_vm *vm, uint8_t v) {
    int c = flag(vm, F_C);
    set_flag(vm, F_C, (v & 0x01) != 0);
    v = (uint8_t)((v >> 1) | (c ? 0x80 : 0)); set_zn(vm, v); return v;
}
static void branch(nsid_vm *vm, int take) {
    int8_t rel = (int8_t)fetch8(vm);
    if (take) vm->pc = (uint16_t)(vm->pc + rel);
}
static void nop_read(nsid_vm *vm, uint16_t addr) { (void)bus_read(vm, addr); }

/* ---- small step helpers (were step()'s lambdas) ---------------------------- */

static void ld_a(nsid_vm *vm, uint8_t v) { vm->a = v; set_zn(vm, v); }
static void ld_x(nsid_vm *vm, uint8_t v) { vm->x = v; set_zn(vm, v); }
static void ld_y(nsid_vm *vm, uint8_t v) { vm->y = v; set_zn(vm, v); }
static nsid_status st_mem(nsid_vm *vm, uint16_t addr, uint8_t value) {
    return bus_write(vm, addr, value) ? NSID_OK : NSID_OOM;
}
static nsid_status rmw_op(nsid_vm *vm, uint16_t addr, uint8_t (*op)(nsid_vm *, uint8_t)) {
    uint8_t v = bus_read(vm, addr);
    v = op(vm, v);
    return rmw(vm, addr, v) ? NSID_OK : NSID_OOM;
}

static int execute_unofficial(nsid_vm *vm, uint8_t opcode, nsid_status *status);

/* ---- one instruction (verbatim opcode table from sid_vm.cpp::step) --------- */

static nsid_status step(nsid_vm *vm, uint8_t *opcode_out, uint32_t *cycles_out) {
    uint16_t opcode_pc = vm->pc;
    uint8_t opcode = fetch8(vm);
    uint32_t cycles = 2;
    *opcode_out = opcode;

    switch (opcode) {
        case 0x00: vm->pc++; push(vm, (uint8_t)(vm->pc >> 8)); push(vm, (uint8_t)vm->pc); push(vm, vm->p | F_B); set_flag(vm, F_I, 1); vm->pc = (uint16_t)bus_read(vm, 0xFFFE) | ((uint16_t)bus_read(vm, 0xFFFF) << 8); cycles = 7; break;
        case 0x01: ld_a(vm, vm->a | bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0x05: ld_a(vm, vm->a | bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0x06: *cycles_out = cycles; return rmw_op(vm, a_zp(vm), op_asl);
        case 0x08: if (!push(vm, vm->p | F_B | F_U)) { *cycles_out = cycles; return NSID_OOM; } cycles = 3; break;
        case 0x09: ld_a(vm, vm->a | fetch8(vm)); break;
        case 0x0A: vm->a = op_asl(vm, vm->a); break;
        case 0x0D: ld_a(vm, vm->a | bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0x0E: *cycles_out = cycles; return rmw_op(vm, a_abs(vm), op_asl);
        case 0x10: branch(vm, !flag(vm, F_N)); cycles = 3; break;
        case 0x11: ld_a(vm, vm->a | bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0x15: ld_a(vm, vm->a | bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0x16: *cycles_out = cycles; return rmw_op(vm, a_zpx(vm), op_asl);
        case 0x18: set_flag(vm, F_C, 0); break;
        case 0x19: ld_a(vm, vm->a | bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0x1D: ld_a(vm, vm->a | bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0x1E: *cycles_out = cycles; return rmw_op(vm, a_absx(vm), op_asl);

        case 0x20: { uint16_t addr = a_abs(vm); uint16_t ret = (uint16_t)(vm->pc - 1); if (!push(vm, (uint8_t)(ret >> 8)) || !push(vm, (uint8_t)ret)) { *cycles_out = cycles; return NSID_OOM; } vm->pc = addr; cycles = 6; break; }
        case 0x21: ld_a(vm, vm->a & bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0x24: test_bits(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0x25: ld_a(vm, vm->a & bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0x26: *cycles_out = cycles; return rmw_op(vm, a_zp(vm), op_rol);
        case 0x28: vm->p = pop(vm) | F_U; cycles = 4; break;
        case 0x29: ld_a(vm, vm->a & fetch8(vm)); break;
        case 0x2A: vm->a = op_rol(vm, vm->a); break;
        case 0x2C: test_bits(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0x2D: ld_a(vm, vm->a & bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0x2E: *cycles_out = cycles; return rmw_op(vm, a_abs(vm), op_rol);
        case 0x30: branch(vm, flag(vm, F_N)); cycles = 3; break;
        case 0x31: ld_a(vm, vm->a & bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0x35: ld_a(vm, vm->a & bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0x36: *cycles_out = cycles; return rmw_op(vm, a_zpx(vm), op_rol);
        case 0x38: set_flag(vm, F_C, 1); break;
        case 0x39: ld_a(vm, vm->a & bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0x3D: ld_a(vm, vm->a & bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0x3E: *cycles_out = cycles; return rmw_op(vm, a_absx(vm), op_rol);

        case 0x40: rti(vm); cycles = 6; break;
        case 0x41: ld_a(vm, vm->a ^ bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0x45: ld_a(vm, vm->a ^ bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0x46: *cycles_out = cycles; return rmw_op(vm, a_zp(vm), op_lsr);
        case 0x48: if (!push(vm, vm->a)) { *cycles_out = cycles; return NSID_OOM; } cycles = 3; break;
        case 0x49: ld_a(vm, vm->a ^ fetch8(vm)); break;
        case 0x4A: vm->a = op_lsr(vm, vm->a); break;
        case 0x4C: vm->pc = a_abs(vm); cycles = 3; break;
        case 0x4D: ld_a(vm, vm->a ^ bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0x4E: *cycles_out = cycles; return rmw_op(vm, a_abs(vm), op_lsr);
        case 0x50: branch(vm, !flag(vm, F_V)); cycles = 3; break;
        case 0x51: ld_a(vm, vm->a ^ bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0x55: ld_a(vm, vm->a ^ bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0x56: *cycles_out = cycles; return rmw_op(vm, a_zpx(vm), op_lsr);
        case 0x58: set_flag(vm, F_I, 0); break;
        case 0x59: ld_a(vm, vm->a ^ bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0x5D: ld_a(vm, vm->a ^ bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0x5E: *cycles_out = cycles; return rmw_op(vm, a_absx(vm), op_lsr);

        case 0x60: rts(vm); cycles = 6; break;
        case 0x61: adc(vm, bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0x65: adc(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0x66: *cycles_out = cycles; return rmw_op(vm, a_zp(vm), op_ror);
        case 0x68: vm->a = pop(vm); set_zn(vm, vm->a); cycles = 4; break;
        case 0x69: adc(vm, fetch8(vm)); break;
        case 0x6A: vm->a = op_ror(vm, vm->a); break;
        case 0x6C: { uint16_t p = a_abs(vm); uint8_t lo = bus_read(vm, p); uint8_t hi = bus_read(vm, (uint16_t)((p & 0xFF00) | ((p + 1) & 0x00FF))); vm->pc = (uint16_t)lo | ((uint16_t)hi << 8); cycles = 5; break; }
        case 0x6D: adc(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0x6E: *cycles_out = cycles; return rmw_op(vm, a_abs(vm), op_ror);
        case 0x70: branch(vm, flag(vm, F_V)); cycles = 3; break;
        case 0x71: adc(vm, bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0x75: adc(vm, bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0x76: *cycles_out = cycles; return rmw_op(vm, a_zpx(vm), op_ror);
        case 0x78: set_flag(vm, F_I, 1); break;
        case 0x79: adc(vm, bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0x7D: adc(vm, bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0x7E: *cycles_out = cycles; return rmw_op(vm, a_absx(vm), op_ror);

        case 0x81: *cycles_out = cycles; return st_mem(vm, a_indx(vm), vm->a);
        case 0x84: *cycles_out = cycles; return st_mem(vm, a_zp(vm), vm->y);
        case 0x85: *cycles_out = cycles; return st_mem(vm, a_zp(vm), vm->a);
        case 0x86: *cycles_out = cycles; return st_mem(vm, a_zp(vm), vm->x);
        case 0x88: vm->y--; set_zn(vm, vm->y); break;
        case 0x8A: vm->a = vm->x; set_zn(vm, vm->a); break;
        case 0x8C: *cycles_out = cycles; return st_mem(vm, a_abs(vm), vm->y);
        case 0x8D: *cycles_out = cycles; return st_mem(vm, a_abs(vm), vm->a);
        case 0x8E: *cycles_out = cycles; return st_mem(vm, a_abs(vm), vm->x);
        case 0x90: branch(vm, !flag(vm, F_C)); cycles = 3; break;
        case 0x91: *cycles_out = cycles; return st_mem(vm, a_indy(vm), vm->a);
        case 0x94: *cycles_out = cycles; return st_mem(vm, a_zpx(vm), vm->y);
        case 0x95: *cycles_out = cycles; return st_mem(vm, a_zpx(vm), vm->a);
        case 0x96: *cycles_out = cycles; return st_mem(vm, a_zpy(vm), vm->x);
        case 0x98: vm->a = vm->y; set_zn(vm, vm->a); break;
        case 0x99: *cycles_out = cycles; return st_mem(vm, a_absy(vm), vm->a);
        case 0x9A: vm->sp = vm->x; break;
        case 0x9D: *cycles_out = cycles; return st_mem(vm, a_absx(vm), vm->a);

        case 0xA0: ld_y(vm, fetch8(vm)); break;
        case 0xA1: ld_a(vm, bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0xA2: ld_x(vm, fetch8(vm)); break;
        case 0xA4: ld_y(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xA5: ld_a(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xA6: ld_x(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xA8: vm->y = vm->a; set_zn(vm, vm->y); break;
        case 0xA9: ld_a(vm, fetch8(vm)); break;
        case 0xAA: vm->x = vm->a; set_zn(vm, vm->x); break;
        case 0xAC: ld_y(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xAD: ld_a(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xAE: ld_x(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xB0: branch(vm, flag(vm, F_C)); cycles = 3; break;
        case 0xB1: ld_a(vm, bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0xB4: ld_y(vm, bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0xB5: ld_a(vm, bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0xB6: ld_x(vm, bus_read(vm, a_zpy(vm))); cycles = 4; break;
        case 0xB8: set_flag(vm, F_V, 0); break;
        case 0xB9: ld_a(vm, bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0xBA: vm->x = vm->sp; set_zn(vm, vm->x); break;
        case 0xBC: ld_y(vm, bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0xBD: ld_a(vm, bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0xBE: ld_x(vm, bus_read(vm, a_absy(vm))); cycles = 4; break;

        case 0xC0: cmp_op(vm, vm->y, fetch8(vm)); break;
        case 0xC1: cmp_op(vm, vm->a, bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0xC4: cmp_op(vm, vm->y, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xC5: cmp_op(vm, vm->a, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xC6: { uint16_t a = a_zp(vm); uint8_t v = bus_read(vm, a) - 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xC8: vm->y++; set_zn(vm, vm->y); break;
        case 0xC9: cmp_op(vm, vm->a, fetch8(vm)); break;
        case 0xCA: vm->x--; set_zn(vm, vm->x); break;
        case 0xCC: cmp_op(vm, vm->y, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xCD: cmp_op(vm, vm->a, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xCE: { uint16_t a = a_abs(vm); uint8_t v = bus_read(vm, a) - 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xD0: branch(vm, !flag(vm, F_Z)); cycles = 3; break;
        case 0xD1: cmp_op(vm, vm->a, bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0xD5: cmp_op(vm, vm->a, bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0xD6: { uint16_t a = a_zpx(vm); uint8_t v = bus_read(vm, a) - 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xD8: set_flag(vm, F_D, 0); break;
        case 0xD9: cmp_op(vm, vm->a, bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0xDD: cmp_op(vm, vm->a, bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0xDE: { uint16_t a = a_absx(vm); uint8_t v = bus_read(vm, a) - 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }

        case 0xE0: cmp_op(vm, vm->x, fetch8(vm)); break;
        case 0xE1: sbc(vm, bus_read(vm, a_indx(vm))); cycles = 6; break;
        case 0xE4: cmp_op(vm, vm->x, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xE5: sbc(vm, bus_read(vm, a_zp(vm))); cycles = 3; break;
        case 0xE6: { uint16_t a = a_zp(vm); uint8_t v = bus_read(vm, a) + 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xE8: vm->x++; set_zn(vm, vm->x); break;
        case 0xE9: sbc(vm, fetch8(vm)); break;
        case 0xEA: break;
        case 0xEC: cmp_op(vm, vm->x, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xED: sbc(vm, bus_read(vm, a_abs(vm))); cycles = 4; break;
        case 0xEE: { uint16_t a = a_abs(vm); uint8_t v = bus_read(vm, a) + 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xF0: branch(vm, flag(vm, F_Z)); cycles = 3; break;
        case 0xF1: sbc(vm, bus_read(vm, a_indy(vm))); cycles = 5; break;
        case 0xF5: sbc(vm, bus_read(vm, a_zpx(vm))); cycles = 4; break;
        case 0xF6: { uint16_t a = a_zpx(vm); uint8_t v = bus_read(vm, a) + 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }
        case 0xF8: set_flag(vm, F_D, 1); break;
        case 0xF9: sbc(vm, bus_read(vm, a_absy(vm))); cycles = 4; break;
        case 0xFD: sbc(vm, bus_read(vm, a_absx(vm))); cycles = 4; break;
        case 0xFE: { uint16_t a = a_absx(vm); uint8_t v = bus_read(vm, a) + 1; set_zn(vm, v); *cycles_out = cycles; return rmw(vm, a, v) ? NSID_OK : NSID_OOM; }

        default: {
            nsid_status status = NSID_OK;
            if (!execute_unofficial(vm, opcode, &status)) {
                vm->pc = opcode_pc;
                *cycles_out = cycles;
                return status;
            }
            break;
        }
    }

    *cycles_out = cycles;
    return NSID_OK;
}

/* ---- unofficial opcodes (verbatim from sid_vm.cpp::executeUnofficial) ------- */

static int uo_store(nsid_vm *vm, uint16_t addr, uint8_t value, nsid_status *status) {
    if (!bus_write(vm, addr, value)) { *status = NSID_OOM; return 0; }
    return 1;
}
static void uo_lax(nsid_vm *vm, uint8_t value) { vm->a = value; vm->x = value; set_zn(vm, value); }
static int uo_sax(nsid_vm *vm, uint16_t addr, nsid_status *status) { return uo_store(vm, addr, (uint8_t)(vm->a & vm->x), status); }
static int uo_dcp(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = bus_read(vm, addr) - 1; set_zn(vm, v);
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    cmp_op(vm, vm->a, v); return 1;
}
static int uo_isc(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = bus_read(vm, addr) + 1; set_zn(vm, v);
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    sbc(vm, v); return 1;
}
static int uo_slo(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = op_asl(vm, bus_read(vm, addr));
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    vm->a |= v; set_zn(vm, vm->a); return 1;
}
static int uo_rla(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = op_rol(vm, bus_read(vm, addr));
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    vm->a &= v; set_zn(vm, vm->a); return 1;
}
static int uo_sre(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = op_lsr(vm, bus_read(vm, addr));
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    vm->a ^= v; set_zn(vm, vm->a); return 1;
}
static int uo_rra(nsid_vm *vm, uint16_t addr, nsid_status *status) {
    uint8_t v = op_ror(vm, bus_read(vm, addr));
    if (!rmw(vm, addr, v)) { *status = NSID_OOM; return 0; }
    adc(vm, v); return 1;
}

static int execute_unofficial(nsid_vm *vm, uint8_t opcode, nsid_status *status) {
    switch (opcode) {
        case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xFA:
            return 1;
        case 0x04: case 0x44: case 0x64: nop_read(vm, a_zp(vm)); return 1;
        case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4: nop_read(vm, a_zpx(vm)); return 1;
        case 0x0C: nop_read(vm, a_abs(vm)); return 1;
        case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC: nop_read(vm, a_absx(vm)); return 1;

        case 0xA3: uo_lax(vm, bus_read(vm, a_indx(vm))); return 1;
        case 0xA7: uo_lax(vm, bus_read(vm, a_zp(vm))); return 1;
        case 0xAF: uo_lax(vm, bus_read(vm, a_abs(vm))); return 1;
        case 0xB3: uo_lax(vm, bus_read(vm, a_indy(vm))); return 1;
        case 0xB7: uo_lax(vm, bus_read(vm, a_zpy(vm))); return 1;
        case 0xBF: uo_lax(vm, bus_read(vm, a_absy(vm))); return 1;

        case 0x83: return uo_sax(vm, a_indx(vm), status);
        case 0x87: return uo_sax(vm, a_zp(vm), status);
        case 0x8F: return uo_sax(vm, a_abs(vm), status);
        case 0x97: return uo_sax(vm, a_zpy(vm), status);

        case 0xC3: return uo_dcp(vm, a_indx(vm), status);
        case 0xC7: return uo_dcp(vm, a_zp(vm), status);
        case 0xCF: return uo_dcp(vm, a_abs(vm), status);
        case 0xD3: return uo_dcp(vm, a_indy(vm), status);
        case 0xD7: return uo_dcp(vm, a_zpx(vm), status);
        case 0xDB: return uo_dcp(vm, a_absy(vm), status);
        case 0xDF: return uo_dcp(vm, a_absx(vm), status);

        case 0xE3: return uo_isc(vm, a_indx(vm), status);
        case 0xE7: return uo_isc(vm, a_zp(vm), status);
        case 0xEF: return uo_isc(vm, a_abs(vm), status);
        case 0xF3: return uo_isc(vm, a_indy(vm), status);
        case 0xF7: return uo_isc(vm, a_zpx(vm), status);
        case 0xFB: return uo_isc(vm, a_absy(vm), status);
        case 0xFF: return uo_isc(vm, a_absx(vm), status);

        case 0x03: return uo_slo(vm, a_indx(vm), status);
        case 0x07: return uo_slo(vm, a_zp(vm), status);
        case 0x0F: return uo_slo(vm, a_abs(vm), status);
        case 0x13: return uo_slo(vm, a_indy(vm), status);
        case 0x17: return uo_slo(vm, a_zpx(vm), status);
        case 0x1B: return uo_slo(vm, a_absy(vm), status);
        case 0x1F: return uo_slo(vm, a_absx(vm), status);

        case 0x23: return uo_rla(vm, a_indx(vm), status);
        case 0x27: return uo_rla(vm, a_zp(vm), status);
        case 0x2F: return uo_rla(vm, a_abs(vm), status);
        case 0x33: return uo_rla(vm, a_indy(vm), status);
        case 0x37: return uo_rla(vm, a_zpx(vm), status);
        case 0x3B: return uo_rla(vm, a_absy(vm), status);
        case 0x3F: return uo_rla(vm, a_absx(vm), status);

        case 0x43: return uo_sre(vm, a_indx(vm), status);
        case 0x47: return uo_sre(vm, a_zp(vm), status);
        case 0x4F: return uo_sre(vm, a_abs(vm), status);
        case 0x53: return uo_sre(vm, a_indy(vm), status);
        case 0x57: return uo_sre(vm, a_zpx(vm), status);
        case 0x5B: return uo_sre(vm, a_absy(vm), status);
        case 0x5F: return uo_sre(vm, a_absx(vm), status);

        case 0x63: return uo_rra(vm, a_indx(vm), status);
        case 0x67: return uo_rra(vm, a_zp(vm), status);
        case 0x6F: return uo_rra(vm, a_abs(vm), status);
        case 0x73: return uo_rra(vm, a_indy(vm), status);
        case 0x77: return uo_rra(vm, a_zpx(vm), status);
        case 0x7B: return uo_rra(vm, a_absy(vm), status);
        case 0x7F: return uo_rra(vm, a_absx(vm), status);

        case 0x0B: case 0x2B:
            vm->a &= fetch8(vm); set_zn(vm, vm->a);
            set_flag(vm, F_C, (vm->a & 0x80) != 0); return 1;
        case 0x4B:
            vm->a &= fetch8(vm); vm->a = op_lsr(vm, vm->a); return 1;
        case 0x6B: {
            vm->a &= fetch8(vm);
            int old_c = flag(vm, F_C);
            vm->a = (uint8_t)((vm->a >> 1) | (old_c ? 0x80 : 0));
            set_zn(vm, vm->a);
            set_flag(vm, F_C, (vm->a & 0x40) != 0);
            set_flag(vm, F_V, ((vm->a >> 6) ^ (vm->a >> 5)) & 1);
            return 1;
        }
        case 0xCB: {
            uint8_t v = fetch8(vm);
            vm->x = (uint8_t)((vm->a & vm->x) - v);
            set_zn(vm, vm->x);
            set_flag(vm, F_C, (vm->a & vm->x) >= v);
            return 1;
        }
        default:
            *status = NSID_BADOP;
            return 0;
    }
}

/* ---- KERNAL/CIA/VIC rom stubs (sid_vm.cpp::handleRomStub) ------------------- */

static int handle_rom_stub(nsid_vm *vm, uint16_t pc, int *handled, nsid_status *status) {
    *handled = 0;
    *status = NSID_OK;

    int rom_addr = (pc >= 0xA000 && pc <= 0xBFFF) || (pc >= 0xE000 && pc <= 0xFFFF);
    if (rom_addr && addr_written(vm, pc))
        return 1;

    switch (pc) {
        case 0xFF81: case 0xFF84: case 0xFF87: case 0xFF8A: case 0xFF90:
        case 0xFF93: case 0xFF96: case 0xFFAB: case 0xFFAE: case 0xFFB1:
        case 0xFFB4: case 0xFFBA: case 0xFFBD: case 0xFFC0: case 0xFFC3:
        case 0xFFC6: case 0xFFC9: case 0xFFCC: case 0xFFD2: case 0xFFDB:
        case 0xFFDE: case 0xFFE1: case 0xFFE7: case 0xFFEA: case 0xFFED:
        case 0xFFF0: case 0xFFF3:
            set_flag(vm, F_C, 0); *handled = 1; return rts(vm);
        case 0xFFE4: case 0xFFCF:
            vm->a = 0; set_zn(vm, vm->a); set_flag(vm, F_C, 0); *handled = 1; return rts(vm);
        case 0xFF99: case 0xFF9C:
            set_flag(vm, F_C, 0); *handled = 1; return rts(vm);
        default:
            break;
    }

    if (rom_addr) *status = NSID_BADROM;
    return 1;
}

/* ---- virtual JSR harness (sid_vm.cpp::runVirtualJsr) ------------------------ */

static nsid_status run_virtual_jsr(nsid_vm *vm, uint16_t addr, uint8_t a, uint32_t max_cycles) {
    if (addr == 0) return NSID_INTERNAL;

    vm->a = a; vm->x = 0; vm->y = 0; vm->sp = 0xFD; vm->p = F_I | F_U; vm->pc = addr;

    uint16_t ret = (uint16_t)(TRAP_ADDRESS - 1);
    if (!mem_write(vm, 0x01FF, (uint8_t)(ret >> 8)) ||
        !mem_write(vm, 0x01FE, (uint8_t)(ret & 0xFF)))
        return NSID_OOM;

    nsid_status result = NSID_OK;
    uint8_t last_op = 0;
    uint32_t cycles = 0;
    while (cycles < max_cycles) {
        if (vm->pc == TRAP_ADDRESS) break;

        int handled = 0;
        nsid_status stub_status = NSID_OK;
        if (!handle_rom_stub(vm, vm->pc, &handled, &stub_status)) { result = NSID_OOM; break; }
        if (stub_status != NSID_OK) { result = stub_status; break; }
        if (handled) { cycles += 6; continue; }

        uint8_t opcode = 0;
        uint32_t op_cycles = 0;
        nsid_status status = step(vm, &opcode, &op_cycles);
        cycles += op_cycles == 0 ? 2 : op_cycles;
        last_op = opcode;
        if (status != NSID_OK) { result = status; break; }
    }

    if (cycles >= max_cycles && result == NSID_OK)
        result = NSID_HUNG;

    vm->last_pc = vm->pc;
    vm->last_opcode = last_op;
    return result;
}

/* ---- public API ------------------------------------------------------------ */

void nsid_reset(void) {
    mem_clear(&g_vm);
    memset(g_vm.sid1, 0, sizeof(g_vm.sid1));
    memset(g_vm.sid2, 0, sizeof(g_vm.sid2));
    memset(g_vm.io, 0, sizeof(g_vm.io));
    g_vm.a = g_vm.x = g_vm.y = 0;
    g_vm.sp = 0xFD;
    g_vm.p = F_I | F_U;
    g_vm.pc = 0;
    g_vm.init_addr = 0;
    g_vm.play_addr = 0;
    g_vm.frame = 0;
    g_vm.cia_timer = 0;
    g_vm.rng = 0x4E4F5641UL;
    g_vm.last_pc = 0;
    g_vm.last_opcode = 0;
    /* write handler is deliberately preserved across reset, matching the ESP
     * flow (stop_sid_playback resets then re-arms the handler explicitly). */
}

void nsid_set_write_handler(nsid_write_fn fn, void *user) {
    g_vm.sid_write_cb = fn;
    g_vm.sid_write_user = user;
}

int nsid_load_payload(uint16_t load_addr, const uint8_t *data, size_t len) {
    return mem_load(&g_vm, load_addr, data, len);
}

void nsid_set_entry(uint16_t init_addr, uint16_t play_addr) {
    g_vm.init_addr = init_addr;
    g_vm.play_addr = play_addr;
}

nsid_status nsid_run_init(uint8_t song) {
    return run_virtual_jsr(&g_vm, g_vm.init_addr, song, MAX_INIT_CYCLES);
}

nsid_status nsid_run_play_frame(void) {
    g_vm.frame++;
    if (g_vm.play_addr == 0) return NSID_OK;
    return run_virtual_jsr(&g_vm, g_vm.play_addr, 0, MAX_PLAY_CYCLES);
}

uint16_t nsid_last_pc(void)         { return g_vm.last_pc; }
uint8_t  nsid_last_opcode(void)     { return g_vm.last_opcode; }
uint16_t nsid_pages_allocated(void) { return g_vm.page_count; }

const char *nsid_status_name(nsid_status st) {
    switch (st) {
        case NSID_OK:       return "ok";
        case NSID_HUNG:     return "hung";
        case NSID_OOM:      return "out_of_memory";
        case NSID_BADOP:    return "unsupported_opcode";
        case NSID_BADROM:   return "unsupported_rom_call";
        case NSID_INTERNAL: return "internal_error";
        default:            return "unknown";
    }
}
