#include "fio_dispatcher.h"
#include <Arduino.h>
#include <esp_system.h>
#include <string.h>

extern void logLn(const char* fmt, ...);
extern void novaHostFioActivityStarted();
extern void novaHostFioActivityFinished(bool ok);

#ifndef NOVAHOST_XLOAD_VERIFY
#define NOVAHOST_XLOAD_VERIFY 0
#endif

namespace {
ndi::FileType file_type_for_name(const char* name) {
    const char* ext = strrchr(name, '.');
    if (!ext) return ndi::FT_BAS;
    if      (strcasecmp(ext, ".bas")  == 0) return ndi::FT_BAS;
    else if (strcasecmp(ext, ".sid")  == 0) return ndi::FT_SID;
    else if (strcasecmp(ext, ".bin")  == 0) return ndi::FT_BIN;
    else if (strcasecmp(ext, ".xram") == 0) return ndi::FT_BIN;
    else if (strcasecmp(ext, ".mid")  == 0) return ndi::FT_MID;
    else if (strcasecmp(ext, ".gfx")  == 0) return ndi::FT_GFX;
    else if (strcasecmp(ext, ".nvg")  == 0) return ndi::FT_GFX;
    return ndi::FT_BIN;
}

uint32_t vgc_space_bytes(uint8_t space) {
    switch (space) {
        case 0x01: return 4000;   // char RAM
        case 0x02: return 4000;   // color RAM
        case 0x03: return 64000;  // bitmap graphics
        case 0x04: return 32768;  // sprite shapes
        case 0x07: return 4000;   // text attributes
        default:   return 0;
    }
}

constexpr uint8_t VGC_SPACE_GFX = 0x03;
constexpr uint32_t NVG_BITMAP_BYTES = 320UL * 200UL;

bool clear_vgc_gfx(FpgaBridge& bridge, uint8_t* zero_buf) {
    memset(zero_buf, 0, 256);
    for (uint32_t off = 0; off < NVG_BITMAP_BYTES; off += 256) {
        uint16_t chunk = (NVG_BITMAP_BYTES - off >= 256)
            ? 0
            : (uint16_t)(NVG_BITMAP_BYTES - off);
        if (!bridge.pokeVgcBlock(VGC_SPACE_GFX, (uint16_t)off, zero_buf, chunk)) {
            logLn("[fio] NVGLOAD gfx clear failed at VGC gfx:$%04X\n",
                  (unsigned)off);
            return false;
        }
    }
    return true;
}

bool has_extension(const char* name) {
    return name && strrchr(name, '.') != nullptr;
}

uint16_t read_be16(const uint8_t* data, size_t off) {
    return ((uint16_t)data[off] << 8) | (uint16_t)data[off + 1];
}

uint32_t read_be32(const uint8_t* data, size_t off) {
    return ((uint32_t)data[off] << 24) |
           ((uint32_t)data[off + 1] << 16) |
           ((uint32_t)data[off + 2] << 8) |
            (uint32_t)data[off + 3];
}

int find_load_entry(ndi::NdiImage* img, const char* name, uint16_t parent,
                    ndi::DirEntry& entry, bool& is_bin) {
    if (!img || !name) return -1;

    if (has_extension(name)) {
        int idx = img->find_entry(name, parent);
        if (idx < 0 || !img->get_entry(idx, entry)) return -1;
        is_bin = entry.file_type == ndi::FT_BIN;
        return idx;
    }

    char candidate[64];
    snprintf(candidate, sizeof(candidate), "%s.bas", name);
    int idx = img->find_entry(candidate, parent);
    if (idx >= 0 && img->get_entry(idx, entry)) {
        is_bin = false;
        return idx;
    }

    snprintf(candidate, sizeof(candidate), "%s.bin", name);
    idx = img->find_entry(candidate, parent);
    if (idx >= 0 && img->get_entry(idx, entry)) {
        is_bin = true;
        return idx;
    }

    return -1;
}

int find_sid_entry(ndi::NdiImage* img, char* name, size_t name_size,
                   uint16_t parent, ndi::DirEntry& entry) {
    if (!img || !name) return -1;

    int idx = img->find_entry(name, parent);
    if (idx >= 0 && img->get_entry(idx, entry) && !entry.is_directory())
        return idx;

    if (has_extension(name))
        return -1;

    size_t used = strlen(name);
    if (used + 4 >= name_size)
        return -1;

    strcat(name, ".sid");
    idx = img->find_entry(name, parent);
    if (idx >= 0 && img->get_entry(idx, entry) && !entry.is_directory())
        return idx;

    return -1;
}

class NdiFileReader {
public:
    NdiFileReader(ndi::NdiImage* img, int index, uint32_t size,
                  uint8_t* cache, size_t cache_size)
        : _img(img), _index(index), _size(size),
          _cache(cache), _cache_size(cache_size) {}

    bool read(uint8_t* dest, size_t len) {
        size_t copied = 0;
        while (copied < len) {
            if (_pos >= _size) return false;
            if (_pos < _cache_start ||
                _pos >= _cache_start + _cache_valid) {
                if (!fill_cache()) return false;
            }

            size_t cache_off = (size_t)(_pos - _cache_start);
            size_t available = _cache_valid - cache_off;
            size_t take = len - copied;
            if (take > available) take = available;
            memcpy(dest + copied, _cache + cache_off, take);
            _pos += (uint32_t)take;
            copied += take;
        }
        return true;
    }

    bool read_u8(uint8_t& value) {
        return read(&value, 1);
    }

    bool read_u16(uint16_t& value) {
        uint8_t b[2];
        if (!read(b, sizeof(b))) return false;
        value = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
        return true;
    }

    bool read_u32(uint32_t& value) {
        uint8_t b[4];
        if (!read(b, sizeof(b))) return false;
        value = (uint32_t)b[0] |
                ((uint32_t)b[1] << 8) |
                ((uint32_t)b[2] << 16) |
                ((uint32_t)b[3] << 24);
        return true;
    }

private:
    bool fill_cache() {
        _cache_start = _pos;
        size_t wanted = _cache_size;
        uint32_t remaining = _size - _pos;
        if (wanted > remaining) wanted = remaining;
        int got = _img->read_file_chunk_by_index(_index, _pos, _cache, wanted);
        if (got <= 0) return false;
        _cache_valid = (size_t)got;
        return true;
    }

    ndi::NdiImage* _img;
    int _index;
    uint32_t _size;
    uint8_t* _cache;
    size_t _cache_size;
    uint32_t _pos = 0;
    uint32_t _cache_start = 0;
    size_t _cache_valid = 0;
};

constexpr uint16_t SID_IRQ_VECTOR_ADDR   = 0x020D;
constexpr uint16_t SID_WORKSPACE_MIN     = 0x0400;
constexpr uint16_t SID_WORKSPACE_TOP     = 0xA000;
constexpr uint16_t SID_WORKSPACE_BYTES   = 0x0300;
constexpr uint16_t SID_IRQ_ENTRY_OFFSET  = 0x0010;
constexpr uint16_t SID_HOST_ZP_OFFSET    = 0x0100;
constexpr uint16_t SID_PLAYER_ZP_OFFSET  = 0x0200;
constexpr uint16_t VGC_IRQ_ENABLE_ADDR   = 0xA0F0;
constexpr uint16_t VGC_IRQ_STATUS_ADDR   = 0xA0F1;
constexpr uint16_t VGC_IRQ_TIMER_LO_ADDR = 0xA0F4;
constexpr uint16_t VGC_IRQ_TIMER_MID_ADDR = 0xA0F5;
constexpr uint16_t VGC_IRQ_TIMER_HI_ADDR = 0xA0F6;
constexpr uint16_t VGC_IRQ_TIMER_CTL_ADDR = 0xA0F7;
constexpr uint8_t  VGC_IRQ_TIMER         = 0x80;
constexpr uint8_t  VGC_IRQ_NON_SID_MASK  = 0x7F;
constexpr uint8_t  VGC_IRQ_SPRCOLL       = 0x20;
constexpr uint8_t  IRQ_BASIC_IRQ_BASE_ZP = 0xDF;
constexpr uint32_t SID_TIMER_PAL_PERIOD  = 540000; // 27 MHz / 50 Hz
constexpr uint32_t SID_TIMER_NTSC_PERIOD = 450000; // 27 MHz / 60 Hz
constexpr uint16_t SID1_BASE             = 0xD400;
constexpr uint16_t SID2_BASE             = 0xD420;
constexpr uint16_t SID_CFG_ADDR          = 0xD440;
constexpr uint8_t  SID_CFG_MODEL_8580    = 0x01;
constexpr uint8_t  SID_CFG_CLOCK_NTSC    = 0x02;

constexpr uint8_t SID_CLOCK_NTSC    = 0x02;
constexpr uint8_t SID_MODEL_8580    = 0x02;

constexpr uint8_t BASIC_IRQ_CODE[] = {
    0x48,                         // PHA
    0xA5, IRQ_BASIC_IRQ_BASE_ZP,   // LDA IrqBase
    0x4A,                         // LSR
    0x05, IRQ_BASIC_IRQ_BASE_ZP,   // ORA IrqBase
    0x85, IRQ_BASIC_IRQ_BASE_ZP,   // STA IrqBase
    0xA9, VGC_IRQ_SPRCOLL,        // LDA #VGC_IRQ_SPRCOLL
    0x8D, 0xF1, 0xA0,             // STA VGC_IRQ_STATUS
    0x68,                         // PLA
    0x40                          // RTI
};

struct SidInfo {
    bool valid = false;
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

bool parse_sid_header(const uint8_t* header, uint32_t file_size, SidInfo& sid) {
    if (file_size < 124)
        return false;

    if ((memcmp(header, "PSID", 4) != 0) && (memcmp(header, "RSID", 4) != 0))
        return false;

    memcpy(sid.magic, header, 4);
    sid.magic[4] = 0;
    sid.dataOffset = read_be16(header, 6);
    sid.loadAddress = read_be16(header, 8);
    sid.initAddress = read_be16(header, 10);
    sid.playAddress = read_be16(header, 12);
    sid.songs = read_be16(header, 14);
    sid.startSong = read_be16(header, 16);
    sid.speed = read_be32(header, 18);
    sid.flags = sid.dataOffset >= 0x7C ? read_be16(header, 0x76) : 0;
    sid.loadAddressInPayload = sid.loadAddress == 0;

    if (sid.dataOffset < 124 || sid.dataOffset > file_size)
        return false;
    if (sid.loadAddressInPayload && sid.dataOffset + 2 > file_size)
        return false;

    sid.payloadFileOffset = sid.dataOffset + (sid.loadAddressInPayload ? 2 : 0);
    if (sid.payloadFileOffset > file_size)
        return false;

    sid.payloadBytes = file_size - sid.payloadFileOffset;
    sid.valid = true;
    return true;
}

bool sid_uses_cia_timing(const SidInfo& sid, uint8_t song) {
    if (song == 0)
        return false;
    return (sid.speed & (1UL << (song - 1))) != 0;
}

uint32_t sid_timer_period(const SidInfo& sid, uint8_t song) {
    if (sid_uses_cia_timing(sid, song))
        return SID_TIMER_NTSC_PERIOD;

    uint8_t clock = (uint8_t)((sid.flags >> 2) & 0x03);
    return clock == SID_CLOCK_NTSC ? SID_TIMER_NTSC_PERIOD : SID_TIMER_PAL_PERIOD;
}

uint8_t sid_config_for(const SidInfo& sid) {
    uint8_t cfg = 0;
    uint8_t clock = (uint8_t)((sid.flags >> 2) & 0x03);
    uint8_t model = (uint8_t)((sid.flags >> 4) & 0x03);

    if (clock == SID_CLOCK_NTSC)
        cfg |= SID_CFG_CLOCK_NTSC;
    if (model == SID_MODEL_8580)
        cfg |= SID_CFG_MODEL_8580;

    return cfg;
}

bool configure_sid(FpgaBridge& bridge, uint8_t config) {
    return bridge.poke(SID_CFG_ADDR, config);
}

bool ranges_overlap(uint32_t a, uint32_t a_len, uint32_t b, uint32_t b_len) {
    if (a_len == 0 || b_len == 0)
        return false;
    return a < b + b_len && b < a + a_len;
}

uint16_t choose_sid_workspace(uint16_t load, uint32_t payload_len) {
    for (int base = SID_WORKSPACE_TOP - SID_WORKSPACE_BYTES;
         base >= SID_WORKSPACE_MIN;
         base -= 0x0100) {
        if (!ranges_overlap((uint32_t)base, SID_WORKSPACE_BYTES,
                            load, payload_len)) {
            return (uint16_t)base;
        }
    }
    return 0;
}

bool build_sid_irq_code(uint16_t workspace, uint16_t init_addr,
                        uint16_t play_addr, uint8_t song_arg,
                        uint8_t* page) {
    memset(page, 0, 256);

    uint16_t entry = workspace + SID_IRQ_ENTRY_OFFSET;
    uint16_t host_zp = workspace + SID_HOST_ZP_OFFSET;
    uint16_t sid_zp = workspace + SID_PLAYER_ZP_OFFSET;
    uint16_t active_flag = workspace;
    uint16_t init_flag = workspace + 1;
    uint16_t song_value = workspace + 2;
    uint16_t irq_status_save = workspace + 3;
    uint16_t non_sid_status = workspace + 4;
    uint16_t frame_counter = workspace + 5;
    uint16_t play_counter = workspace + 7;
    uint16_t init_counter = workspace + 9;
    uint16_t irq_enable_save = workspace + 11;

    page[0] = 1;          // active
    page[1] = 1;          // init pending
    page[2] = song_arg;   // PSID init takes zero-based song number in A

    size_t n = SID_IRQ_ENTRY_OFFSET;
    bool ok = true;
    auto emit = [&](uint8_t v) {
        if (n >= 256) {
            ok = false;
            return;
        }
        page[n++] = v;
    };
    auto emit16 = [&](uint16_t v) {
        emit((uint8_t)(v & 0xFF));
        emit((uint8_t)(v >> 8));
    };
    auto lda_abs = [&](uint16_t addr) { emit(0xAD); emit16(addr); };
    auto sta_abs = [&](uint16_t addr) { emit(0x8D); emit16(addr); };
    auto lda_abs_x = [&](uint16_t addr) { emit(0xBD); emit16(addr); };
    auto sta_abs_x = [&](uint16_t addr) { emit(0x9D); emit16(addr); };
    auto inc_abs = [&](uint16_t addr) { emit(0xEE); emit16(addr); };
    auto beq_placeholder = [&]() -> size_t { emit(0xF0); size_t op = n; emit(0x00); return op; };
    auto bne_placeholder = [&]() -> size_t { emit(0xD0); size_t op = n; emit(0x00); return op; };
    auto jmp_abs_placeholder = [&]() -> size_t {
        emit(0x4C);
        size_t op = n;
        emit(0x00);
        emit(0x00);
        return op;
    };
    auto patch_branch = [&](size_t operand, size_t target) {
        int rel = (int)target - (int)(operand + 1);
        if (rel < -128 || rel > 127) {
            ok = false;
            return;
        }
        page[operand] = (uint8_t)(int8_t)rel;
    };
    auto patch_jmp = [&](size_t operand, size_t target) {
        if (operand + 1 >= 256) {
            ok = false;
            return;
        }
        uint16_t addr = (uint16_t)(workspace + target);
        page[operand] = (uint8_t)(addr & 0xFF);
        page[operand + 1] = (uint8_t)(addr >> 8);
    };
    auto inc_abs16 = [&](uint16_t addr) {
        inc_abs(addr);
        size_t no_carry_bne = bne_placeholder();
        inc_abs((uint16_t)(addr + 1));
        patch_branch(no_carry_bne, n);
    };

    emit(0x48);                    // PHA
    emit(0x8A); emit(0x48);        // TXA / PHA
    emit(0x98); emit(0x48);        // TYA / PHA
    emit(0xD8);                    // CLD

    lda_abs(VGC_IRQ_ENABLE_ADDR);
    sta_abs(irq_enable_save);

    lda_abs(VGC_IRQ_STATUS_ADDR);
    sta_abs(irq_status_save);
    emit(0x29); emit(VGC_IRQ_TIMER);        // AND #TIMER
    size_t has_timer_bne = bne_placeholder();
    size_t no_timer_jmp = jmp_abs_placeholder();
    patch_branch(has_timer_bne, n);

    lda_abs(irq_enable_save);
    emit(0x29); emit((uint8_t)~VGC_IRQ_TIMER);
    sta_abs(VGC_IRQ_ENABLE_ADDR);           // mask timer while SID IRQ is active
    emit(0xA9); emit(VGC_IRQ_TIMER);        // LDA #TIMER
    sta_abs(VGC_IRQ_STATUS_ADDR);           // ack SID-owned timer IRQ
    inc_abs16(frame_counter);
    lda_abs(active_flag);
    size_t active_bne = bne_placeholder();
    size_t inactive_jmp = jmp_abs_placeholder();
    patch_branch(active_bne, n);

    lda_abs(init_flag);
    size_t init_pending_bne = bne_placeholder();
    patch_branch(init_pending_bne, n);

    emit(0xA2); emit(0x00);                 // LDX #0
    size_t save_host_loop = n;
    emit(0xB5); emit(0x00);                 // LDA $00,X
    sta_abs_x(host_zp);
    emit(0xE8);                             // INX
    size_t save_host_bne = bne_placeholder();

    emit(0xA2); emit(0x00);                 // LDX #0
    size_t restore_sid_loop = n;
    lda_abs_x(sid_zp);
    emit(0x95); emit(0x00);                 // STA $00,X
    emit(0xE8);                             // INX
    size_t restore_sid_bne = bne_placeholder();

    lda_abs(init_flag);
    size_t skip_init_beq = beq_placeholder();
    inc_abs(init_counter);
    emit(0xA2); emit(0x00);                 // LDX #0
    emit(0xA0); emit(0x00);                 // LDY #0
    lda_abs(song_value);
    emit(0x20); emit16(init_addr);          // JSR init
    emit(0xA9); emit(0x00);                 // LDA #0
    sta_abs(init_flag);

    size_t call_play = n;
    inc_abs16(play_counter);
    emit(0xA2); emit(0x00);                 // LDX #0
    emit(0xA0); emit(0x00);                 // LDY #0
    emit(0x20); emit16(play_addr);          // JSR play

    emit(0xA2); emit(0x00);                 // LDX #0
    size_t save_sid_loop = n;
    emit(0xB5); emit(0x00);                 // LDA $00,X
    sta_abs_x(sid_zp);
    emit(0xE8);                             // INX
    size_t save_sid_bne = bne_placeholder();

    emit(0xA2); emit(0x00);                 // LDX #0
    size_t restore_host_loop = n;
    lda_abs_x(host_zp);
    emit(0x95); emit(0x00);                 // STA $00,X
    emit(0xE8);                             // INX
    size_t restore_host_bne = bne_placeholder();

    size_t maybe_basic_irq = n;
    lda_abs(irq_status_save);
    emit(0x29); emit(VGC_IRQ_NON_SID_MASK);
    size_t no_basic_irq_beq = beq_placeholder();
    sta_abs(non_sid_status);
    emit(0xA5); emit(IRQ_BASIC_IRQ_BASE_ZP); // LDA IrqBase
    emit(0x4A);                              // LSR
    emit(0x05); emit(IRQ_BASIC_IRQ_BASE_ZP); // ORA IrqBase
    emit(0x85); emit(IRQ_BASIC_IRQ_BASE_ZP); // STA IrqBase
    lda_abs(non_sid_status);
    sta_abs(VGC_IRQ_STATUS_ADDR);            // ack non-vblank VGC sources

    size_t done = n;
    lda_abs(irq_enable_save);
    sta_abs(VGC_IRQ_ENABLE_ADDR);
    emit(0x68); emit(0xA8);        // PLA / TAY
    emit(0x68); emit(0xAA);        // PLA / TAX
    emit(0x68);                    // PLA
    emit(0x40);                    // RTI

    patch_jmp(no_timer_jmp, maybe_basic_irq);
    patch_jmp(inactive_jmp, maybe_basic_irq);
    patch_branch(save_host_bne, save_host_loop);
    patch_branch(restore_sid_bne, restore_sid_loop);
    patch_branch(skip_init_beq, call_play);
    patch_branch(save_sid_bne, save_sid_loop);
    patch_branch(restore_host_bne, restore_host_loop);
    patch_branch(no_basic_irq_beq, done);

    return ok && n <= 256 && entry == workspace + SID_IRQ_ENTRY_OFFSET;
}

bool set_sid_timer(FpgaBridge& bridge, uint32_t period, bool enabled) {
    uint8_t mask = 0;
    if (!bridge.peek(VGC_IRQ_ENABLE_ADDR, mask))
        return false;

    mask = enabled ? (uint8_t)(mask | VGC_IRQ_TIMER)
                   : (uint8_t)(mask & (uint8_t)~VGC_IRQ_TIMER);

    if (!bridge.poke(VGC_IRQ_TIMER_CTL_ADDR, 0))
        return false;
    if (!bridge.poke(VGC_IRQ_TIMER_LO_ADDR, (uint8_t)(period & 0xFF)))
        return false;
    if (!bridge.poke(VGC_IRQ_TIMER_MID_ADDR, (uint8_t)((period >> 8) & 0xFF)))
        return false;
    if (!bridge.poke(VGC_IRQ_TIMER_HI_ADDR, (uint8_t)((period >> 16) & 0xFF)))
        return false;
    if (!bridge.poke(VGC_IRQ_STATUS_ADDR, VGC_IRQ_TIMER))
        return false;
    if (!bridge.poke(VGC_IRQ_ENABLE_ADDR, mask))
        return false;
    if (enabled && !bridge.poke(VGC_IRQ_TIMER_CTL_ADDR, 1))
        return false;
    return true;
}

bool restore_basic_irq(FpgaBridge& bridge) {
    return bridge.loadRam(SID_IRQ_VECTOR_ADDR, BASIC_IRQ_CODE,
                          sizeof(BASIC_IRQ_CODE));
}

bool silence_sid_chips(FpgaBridge& bridge) {
    for (uint16_t off = 0; off <= 0x18; off++) {
        if (!bridge.poke((uint16_t)(SID1_BASE + off), 0))
            return false;
        if (!bridge.poke((uint16_t)(SID2_BASE + off), 0))
            return false;
    }
    for (uint16_t off = 0x1D; off <= 0x1F; off++) {
        if (!bridge.poke((uint16_t)(SID1_BASE + off), 0x0F))
            return false;
        if (!bridge.poke((uint16_t)(SID2_BASE + off), 0x0F))
            return false;
    }
    return true;
}

bool disable_sid_player(FpgaBridge& bridge) {
    return set_sid_timer(bridge, 0, false) &&
           configure_sid(bridge, 0) &&
           restore_basic_irq(bridge) &&
           silence_sid_chips(bridge);
}

bool install_sid_player(FpgaBridge& bridge, uint16_t workspace,
                        uint16_t init_addr, uint16_t play_addr,
                        uint8_t song_arg, uint32_t timer_period,
                        uint8_t sid_config) {
    uint8_t page[256];
    uint8_t zero_page[256];
    uint8_t irq_jump[3] = {
        0x4C,
        (uint8_t)((workspace + SID_IRQ_ENTRY_OFFSET) & 0xFF),
        (uint8_t)((workspace + SID_IRQ_ENTRY_OFFSET) >> 8)
    };

    if (!build_sid_irq_code(workspace, init_addr, play_addr, song_arg, page))
        return false;

    memset(zero_page, 0, sizeof(zero_page));
    return bridge.loadRam(workspace, page, sizeof(page)) &&
           bridge.loadRam((uint16_t)(workspace + SID_HOST_ZP_OFFSET),
                          zero_page, sizeof(zero_page)) &&
           bridge.loadRam((uint16_t)(workspace + SID_PLAYER_ZP_OFFSET),
                          zero_page, sizeof(zero_page)) &&
           bridge.loadRam(SID_IRQ_VECTOR_ADDR, irq_jump, sizeof(irq_jump)) &&
           configure_sid(bridge, sid_config) &&
           set_sid_timer(bridge, timer_period, true);
}
}

void FioDispatcher::handle_event() {
    if (_handling) {
        return;
    }
    _handling = true;
    novaHostFioActivityStarted();

    // Read the entire 80-byte register bank in one shot.
    if (!_bridge.peekBlock(BANK_BASE, 80, _bank)) {
        Serial.println("[fio] peekBlock failed — bank read aborted");
        novaHostFioActivityFinished(false);
        _handling = false;
        return;
    }

    uint8_t c = cmd();
    if (c == 0) {
        // Spurious event: bank shows no pending cmd. Drop.
        novaHostFioActivityFinished(true);
        _handling = false;
        return;
    }
    logLn("[fio] cmd=0x%02X namelen=%u\n",
                  (unsigned)c, (unsigned)namelen());

    switch (c) {
        case CMD_LOAD:     handle_load();     break;
        case CMD_SAVE:     handle_save();     break;
        case CMD_GLOAD:    handle_gload();    break;
        case CMD_GSAVE:    handle_gsave();    break;
        case CMD_XLOAD:    handle_xload();    break;
        case CMD_XSAVE:    handle_xsave();    break;
        case CMD_XPAGE:    handle_xpage();    break;
        case CMD_DIR_OPEN: handle_dir_open(); break;
        case CMD_DIR_READ: handle_dir_read(); break;
        case CMD_DELETE:   handle_delete();   break;
        case CMD_SIDPLAY:  handle_sidplay();  break;
        case CMD_SIDSTOP:  handle_sidstop();  break;
        case CMD_MIDPLAY:  handle_unsupported_sd_command("MIDPLAY"); break;
        case CMD_SFLOAD:   handle_unsupported_sd_command("SFLOAD");  break;
        case CMD_CD:       handle_cd();       break;
        case CMD_MKDIR:    handle_mkdir();    break;
        case CMD_RMDIR:    handle_rmdir();    break;
        case CMD_FORMAT:   handle_unsupported_sd_command("FORMAT");  break;
        case CMD_MOUNT:    handle_mount();    break;
        case CMD_UNMOUNT:  handle_unmount();  break;
        case CMD_PWD:      handle_pwd();      break;
        case CMD_CLEARERR: handle_clear_error(); break;
        case CMD_LOADRUNTIME: handle_load_runtime(); break;
        case CMD_RNG:      handle_rng();      break;
        case CMD_NVGLOAD:  handle_nvgload();  break;
        default:
            logLn("[fio] unknown cmd 0x%02X\n", (unsigned)c);
            respond_err(ERR_IO);
            break;
    }
    _handling = false;
}

void FioDispatcher::poll_pending() {
    if (_handling) {
        return;
    }

    uint8_t pending = 0;
    if (!_bridge.peek(BANK_BASE + OFF_CMD, pending)) {
        return;
    }
    if (pending != 0) {
        handle_event();
    }
}

// ---------------------------------------------------------------------------
// Common helpers
// ---------------------------------------------------------------------------
void FioDispatcher::copy_filename(char* out) {
    int n = namelen();
    if (n < 0) n = 0;
    if (n > 63) n = 63;
    memcpy(out, &_bank[OFF_NAME], n);
    out[n] = 0;
}

void FioDispatcher::respond_ok() {
    _bridge.poke(BANK_BASE + OFF_ERRCODE, ERR_NONE);
    _bridge.poke(BANK_BASE + OFF_CMD,     0);   // unblock Nova busy-wait
    _bridge.poke(BANK_BASE + OFF_STATUS,  ST_OK);
    novaHostFioActivityFinished(true);
}

void FioDispatcher::respond_err(uint8_t err_code) {
    _bridge.poke(BANK_BASE + OFF_ERRCODE, err_code);
    _bridge.poke(BANK_BASE + OFF_CMD,     0);
    _bridge.poke(BANK_BASE + OFF_STATUS,  ST_ERR);
    novaHostFioActivityFinished(false);
}

void FioDispatcher::write_size(uint32_t size) {
    _bridge.poke(BANK_BASE + OFF_SIZE_LO, (uint8_t)(size & 0xFF));
    _bridge.poke(BANK_BASE + OFF_SIZE_HI, (uint8_t)((size >> 8) & 0xFF));
}

void FioDispatcher::handle_clear_error() {
    logLn("[fio] CLEARERR\n");
    respond_ok();
}

void FioDispatcher::handle_load_runtime() {
    char name[64];
    copy_filename(name);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        logLn("[fio] LOADRUNTIME resolve failed: %s\n", name);
        respond_err(ERR_NOT_FOUND);
        return;
    }

    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int idx = img->find_entry(scratch, parent);
    if (idx < 0 && !has_extension(scratch)) {
        char candidate[64];
        snprintf(candidate, sizeof(candidate), "%s.bin", scratch);
        idx = img->find_entry(candidate, parent);
        if (idx >= 0) {
            strncpy(scratch, candidate, sizeof(scratch) - 1);
            scratch[sizeof(scratch) - 1] = 0;
        }
    }
    if (idx < 0) {
        logLn("[fio] LOADRUNTIME: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    img->get_entry(idx, e);
    if (e.size_bytes != RUNTIME_ROM_BYTES) {
        logLn("[fio] LOADRUNTIME %s: %u bytes, expected %u\n",
              scratch, (unsigned)e.size_bytes, (unsigned)RUNTIME_ROM_BYTES);
        respond_err(ERR_IO);
        return;
    }

    // The caller must be running from RAM: this overwrites the primary ROM bank
    // currently used for NovaBASIC so the launcher can jump into another runtime.
    uint32_t off = 0;
    while (off < RUNTIME_ROM_BYTES) {
        uint16_t chunk = (RUNTIME_ROM_BYTES - off >= 256)
            ? 256
            : (uint16_t)(RUNTIME_ROM_BYTES - off);
        int got = img->read_file_chunk_by_index(idx, off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            logLn("[fio] LOADRUNTIME %s: chunk read failed at %u\n",
                  scratch, (unsigned)off);
            respond_err(ERR_IO);
            return;
        }
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
        if (!_bridge.pokeRomBlock(0, (uint16_t)off, _transfer_buf, wire_count)) {
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    write_size(RUNTIME_ROM_BYTES);
    logLn("[fio] LOADRUNTIME %s -> primary ROM bank (%d bytes) OK\n",
          scratch, RUNTIME_ROM_BYTES);
    respond_ok();
}

void FioDispatcher::handle_rng() {
    uint32_t value = esp_random();
    _bridge.poke(BANK_BASE + OFF_SRC_LO, (uint8_t)(value & 0xFF));
    _bridge.poke(BANK_BASE + OFF_SRC_HI, (uint8_t)((value >> 8) & 0xFF));
    _bridge.poke(BANK_BASE + OFF_END_LO, (uint8_t)((value >> 16) & 0xFF));
    _bridge.poke(BANK_BASE + OFF_END_HI, (uint8_t)((value >> 24) & 0xFF));
    respond_ok();
}

void FioDispatcher::handle_unsupported_sd_command(const char* name) {
    logLn("[fio] %s is not implemented on ESP SD host\n", name ? name : "command");
    respond_err(ERR_IO);
}

// ---------------------------------------------------------------------------
// SIDPLAY / SIDSTOP
// ---------------------------------------------------------------------------
void FioDispatcher::handle_sidplay() {
    char name[64];
    copy_filename(name);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        logLn("[fio] SIDPLAY resolve failed: %s\n", name);
        respond_err(ERR_NOT_FOUND);
        return;
    }

    auto* img = _dm.image(slot);
    if (!img) {
        respond_err(ERR_NO_MOUNT);
        return;
    }

    ndi::DirEntry e;
    int idx = find_sid_entry(img, scratch, sizeof(scratch), parent, e);
    if (idx < 0) {
        logLn("[fio] SIDPLAY: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    if (e.size_bytes < 124) {
        respond_err(ERR_IO);
        return;
    }

    int got = img->read_file_chunk_by_index(idx, 0, _transfer_buf, 124);
    if (got != 124) {
        respond_err(ERR_IO);
        return;
    }

    SidInfo sid;
    if (!parse_sid_header(_transfer_buf, e.size_bytes, sid)) {
        logLn("[fio] SIDPLAY %s: invalid SID header\n", scratch);
        respond_err(ERR_IO);
        return;
    }

    if (sid.loadAddressInPayload) {
        got = img->read_file_chunk_by_index(idx, sid.dataOffset,
                                            _transfer_buf, 2);
        if (got != 2) {
            respond_err(ERR_IO);
            return;
        }
        sid.loadAddress = (uint16_t)_transfer_buf[0] |
                          ((uint16_t)_transfer_buf[1] << 8);
    }

    uint8_t song = _bank[OFF_SRC_LO];
    if (song < 1)
        song = sid.startSong == 0 ? 1 : (uint8_t)sid.startSong;
    if (sid.songs != 0 && song > sid.songs) {
        logLn("[fio] SIDPLAY %s: song %u outside 1..%u\n",
              scratch, (unsigned)song, (unsigned)sid.songs);
        respond_err(ERR_IO);
        return;
    }

    uint32_t load_end = (uint32_t)sid.loadAddress + sid.payloadBytes;
    if (sid.payloadBytes == 0 ||
        sid.loadAddress < SID_WORKSPACE_MIN ||
        load_end > SID_WORKSPACE_TOP ||
        sid.initAddress < sid.loadAddress ||
        sid.initAddress >= load_end ||
        sid.playAddress == 0 ||
        sid.playAddress < sid.loadAddress ||
        sid.playAddress >= load_end) {
        logLn("[fio] SIDPLAY %s: unsupported memory layout load=$%04X size=%u init=$%04X play=$%04X\n",
              scratch, (unsigned)sid.loadAddress, (unsigned)sid.payloadBytes,
              (unsigned)sid.initAddress, (unsigned)sid.playAddress);
        respond_err(ERR_IO);
        return;
    }

    uint16_t workspace = choose_sid_workspace(sid.loadAddress, sid.payloadBytes);
    if (workspace == 0) {
        logLn("[fio] SIDPLAY %s: no free IRQ-player workspace\n", scratch);
        respond_err(ERR_IO);
        return;
    }

    bool ok = disable_sid_player(_bridge);
    if (ok) {
        uint32_t off = 0;
        while (off < sid.payloadBytes) {
            uint32_t remaining = sid.payloadBytes - off;
            uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
            got = img->read_file_chunk_by_index(idx, sid.payloadFileOffset + off,
                                                _transfer_buf, chunk);
            if (got != (int)chunk) {
                ok = false;
                break;
            }
            if (!_bridge.loadRam((uint16_t)(sid.loadAddress + off),
                                _transfer_buf, chunk)) {
                ok = false;
                break;
            }
            off += chunk;
        }
    }

    if (ok) {
        uint32_t timer_period = sid_timer_period(sid, song);
        uint8_t sid_config = sid_config_for(sid);
        ok = install_sid_player(_bridge, workspace, sid.initAddress,
                                sid.playAddress, (uint8_t)(song - 1),
                                timer_period, sid_config);
    }

    if (!ok) {
        logLn("[fio] SIDPLAY %s failed\n", scratch);
        respond_err(ERR_IO);
        return;
    }

    write_size(sid.payloadBytes);
    logLn("[fio] SIDPLAY %s %s load=$%04X init=$%04X play=$%04X song=%u workspace=$%04X speed=$%08lX flags=$%04X period=%lu cfg=$%02X OK\n",
          scratch, sid.magic, (unsigned)sid.loadAddress,
          (unsigned)sid.initAddress, (unsigned)sid.playAddress,
          (unsigned)song, (unsigned)workspace,
          (unsigned long)sid.speed, (unsigned)sid.flags,
          (unsigned long)sid_timer_period(sid, song),
          (unsigned)sid_config_for(sid));
    respond_ok();
}

void FioDispatcher::handle_sidstop() {
    bool ok = disable_sid_player(_bridge);

    if (!ok) {
        respond_err(ERR_IO);
        return;
    }

    logLn("[fio] SIDSTOP OK\n");
    respond_ok();
}

// ---------------------------------------------------------------------------
// LOAD
// ---------------------------------------------------------------------------
void FioDispatcher::handle_load() {
    char name[64];
    copy_filename(name);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        logLn("[fio] LOAD resolve failed: %s\n", name);
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    ndi::DirEntry e;
    bool is_bin = false;
    int idx = find_load_entry(img, scratch, parent, e, is_bin);
    if (idx < 0) {
        logLn("[fio] LOAD: '%s' not found in dev=%s\n",
                      scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    if (e.size_bytes < 2) {
        respond_err(ERR_IO);
        return;
    }

    int got = img->read_file_chunk_by_index(idx, 0, _transfer_buf, 2);
    if (got != 2) { respond_err(ERR_IO); return; }

    uint16_t dest = src();
    if (is_bin) {
        dest = (uint16_t)_transfer_buf[0] | ((uint16_t)_transfer_buf[1] << 8);
        _bridge.poke(BANK_BASE + OFF_SRC_LO, (uint8_t)(dest & 0xFF));
        _bridge.poke(BANK_BASE + OFF_SRC_HI, (uint8_t)(dest >> 8));
    }

    uint32_t payload_bytes = e.size_bytes - 2;
    if (payload_bytes > 0x10000UL || dest > 0x10000UL - payload_bytes) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t off = 0;
    while (off < payload_bytes) {
        uint32_t remaining = payload_bytes - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        got = img->read_file_chunk_by_index(idx, 2 + off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
        if (!_bridge.loadRam((uint16_t)(dest + off), _transfer_buf, chunk)) {
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }
    write_size(payload_bytes);
    _bridge.poke(BANK_BASE + OFF_DIRTYPE, is_bin ? ndi::FT_BIN : ndi::FT_BAS);
    logLn("[fio] LOAD %s → $%04X (%d bytes payload) OK\n",
                  scratch, dest, (int)payload_bytes);
    respond_ok();
}

// ---------------------------------------------------------------------------
// SAVE
// ---------------------------------------------------------------------------
void FioDispatcher::handle_save() {
    char name[64];
    copy_filename(name);

    uint16_t s = src();
    uint16_t e = end();
    if (e < s) { respond_err(ERR_IO); return; }
    uint32_t size = (uint32_t)(e - s + 1);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    ndi::FileType ftype = file_type_for_name(scratch);

    // If a file with the same name already exists, delete it first
    // (mirrors Avalonia behavior — SAVE overwrites).
    int existing = img->find_entry(scratch, parent);
    if (existing >= 0) img->delete_file(scratch, parent);

    int new_idx = img->create_file(scratch, ftype, parent, size);
    if (new_idx < 0) {
        respond_err(ERR_FULL);
        return;
    }

    uint32_t off = 0;
    while (off < size) {
        uint16_t chunk = (size - off >= 256) ? 256 : (uint16_t)(size - off);
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!_bridge.peekBlock((uint16_t)(s + off), wire_count, _transfer_buf)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        if (!img->write_file_chunk_by_index(new_idx, off, _transfer_buf, chunk)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    if (!img->zero_file_tail_by_index(new_idx)) {
        img->delete_file(scratch, parent);
        respond_err(ERR_IO);
        return;
    }

    logLn("[fio] SAVE %s ($%04X-$%04X, %u bytes) OK\n",
                  scratch, s, e, (unsigned)size);
    respond_ok();
}

// ---------------------------------------------------------------------------
// GLOAD / GSAVE — direct file <-> VGC memory streaming.
// ---------------------------------------------------------------------------
void FioDispatcher::handle_gload() {
    char name[64];
    copy_filename(name);

    uint8_t space = gspace();
    uint16_t dest = gaddr();

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int idx = img->find_entry(scratch, parent);
    if (idx < 0) {
        logLn("[fio] GLOAD: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    if (!img->get_entry(idx, e) || e.is_directory()) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t requested = transfer_len();
    uint32_t total = (requested == 0 || requested > e.size_bytes)
        ? e.size_bytes
        : requested;
    uint32_t space_size = vgc_space_bytes(space);
    if (space_size == 0 ||
        dest > space_size ||
        total > space_size ||
        dest > space_size - total) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t off = 0;
    while (off < total) {
        uint32_t remaining = total - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
        if (!_bridge.pokeVgcBlock(space, (uint16_t)(dest + off),
                                  _transfer_buf, wire_count)) {
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    write_size(total);
    logLn("[fio] GLOAD %s -> VGC space %u:$%04X (%u bytes) OK\n",
          scratch, (unsigned)space, (unsigned)dest, (unsigned)total);
    respond_ok();
}

void FioDispatcher::handle_gsave() {
    char name[64];
    copy_filename(name);

    uint8_t space = gspace();
    uint16_t src_addr = gaddr();
    uint32_t total = transfer_len();
    uint32_t space_size = vgc_space_bytes(space);
    if (total == 0 ||
        space_size == 0 ||
        src_addr > space_size ||
        total > space_size ||
        src_addr > space_size - total) {
        respond_err(ERR_IO);
        return;
    }

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int existing = img->find_entry(scratch, parent);
    if (existing >= 0) img->delete_file(scratch, parent);

    int new_idx = img->create_file(scratch, ndi::FT_GFX, parent, total);
    if (new_idx < 0) {
        respond_err(ERR_FULL);
        return;
    }

    uint32_t off = 0;
    while (off < total) {
        uint32_t remaining = total - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!_bridge.readVgcBlock(space, (uint16_t)(src_addr + off),
                                  wire_count, _transfer_buf)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        if (!img->write_file_chunk_by_index(new_idx, off, _transfer_buf, chunk)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    if (!img->zero_file_tail_by_index(new_idx)) {
        img->delete_file(scratch, parent);
        respond_err(ERR_IO);
        return;
    }

    write_size(total);
    logLn("[fio] GSAVE %s <- VGC space %u:$%04X (%u bytes) OK\n",
          scratch, (unsigned)space, (unsigned)src_addr, (unsigned)total);
    respond_ok();
}

void FioDispatcher::handle_nvgload() {
    char name[64];
    copy_filename(name);

    uint8_t space = gspace();
    uint16_t dest = gaddr();
    if (space != 0x03) {
        respond_err(ERR_IO);
        return;
    }

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    if (!has_extension(scratch)) {
        size_t used = strlen(scratch);
        if (used + 4 >= sizeof(scratch)) {
            respond_err(ERR_IO);
            return;
        }
        strcat(scratch, ".nvg");
    }

    int idx = img->find_entry(scratch, parent);
    if (idx < 0) {
        logLn("[fio] NVGLOAD: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    if (!img->get_entry(idx, e) || e.is_directory()) {
        respond_err(ERR_IO);
        return;
    }

    uint8_t* cache = _transfer_buf;
    uint8_t* pixels = _transfer_buf + 256;
    NdiFileReader reader(img, idx, e.size_bytes, cache, 256);

    uint8_t magic[4];
    if (!reader.read(magic, sizeof(magic)) ||
        magic[0] != 'N' || magic[1] != 'V' ||
        magic[2] != 'G' || magic[3] != '1') {
        respond_err(ERR_IO);
        return;
    }

    uint16_t width = 0, height = 0;
    uint32_t span_count = 0;
    if (!reader.read_u16(width) ||
        !reader.read_u16(height) ||
        !reader.read_u32(span_count) ||
        width == 0 || height == 0 ||
        width > 320 || height > 200) {
        respond_err(ERR_IO);
        return;
    }

    uint16_t base_x = dest % 320;
    uint16_t base_y = dest / 320;
    if ((uint32_t)base_x + width > 320 ||
        (uint32_t)base_y + height > 200) {
        respond_err(ERR_IO);
        return;
    }

    if (!clear_vgc_gfx(_bridge, pixels)) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t image_len = (uint32_t)width * height;
    uint32_t written = 0;
    for (uint32_t span = 0; span < span_count; span++) {
        uint16_t addr = 0;
        uint8_t len = 0;
        if (!reader.read_u16(addr) ||
            !reader.read_u8(len) ||
            len == 0 ||
            (uint32_t)addr + len > image_len ||
            !reader.read(pixels, len)) {
            respond_err(ERR_IO);
            return;
        }

        uint32_t image_pos = addr;
        uint8_t pos = 0;
        while (pos < len) {
            uint16_t src_x = image_pos % width;
            uint16_t src_y = image_pos / width;
            uint16_t row_remaining = width - src_x;
            uint8_t chunk = len - pos;
            if (chunk > row_remaining) chunk = (uint8_t)row_remaining;

            uint32_t gfx_addr = (uint32_t)dest + ((uint32_t)src_y * 320UL) + src_x;
            if (gfx_addr + chunk > NVG_BITMAP_BYTES) {
                respond_err(ERR_IO);
                return;
            }
            uint16_t wire_count = (chunk == 256) ? 0 : chunk;
            if (!_bridge.pokeVgcBlock(space, (uint16_t)gfx_addr, pixels + pos, wire_count)) {
                logLn("[fio] NVGLOAD gfx write failed at VGC gfx:$%04X\n",
                      (unsigned)gfx_addr);
                respond_err(ERR_IO);
                return;
            }

            image_pos += chunk;
            pos += chunk;
            written += chunk;
        }
    }

    write_size(written);
    logLn("[fio] NVGLOAD %s -> VGC gfx:$%04X (%u pixels) OK\n",
          scratch, (unsigned)dest, (unsigned)written);
    respond_ok();
}

// ---------------------------------------------------------------------------
// XLOAD / XSAVE — direct file <-> XRAM streaming through SDRAM.
// ---------------------------------------------------------------------------
void FioDispatcher::handle_xload() {
    char name[64];
    copy_filename(name);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int idx = img->find_entry(scratch, parent);
    if (idx < 0) {
        logLn("[fio] XLOAD: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    if (!img->get_entry(idx, e) || e.is_directory()) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t dest = xram_addr();
    uint32_t requested = transfer_len();
    uint32_t total = (requested == 0 || requested > e.size_bytes)
        ? e.size_bytes
        : requested;

    if (dest > XRAM_BYTES || total > XRAM_BYTES || dest > XRAM_BYTES - total) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t off = 0;
    while (off < total) {
        uint32_t remaining = total - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
#if NOVAHOST_XLOAD_VERIFY
        bool verified = false;
        for (int attempt = 0; attempt < 5; attempt++) {
            if (!_bridge.pokeSdramBlock(dest + off, _transfer_buf, wire_count)) {
                continue;
            }
            if (!_bridge.readSdramBlock(dest + off, (uint8_t)wire_count, _transfer_buf + 256)) {
                continue;
            }
            if (memcmp(_transfer_buf, _transfer_buf + 256, chunk) == 0) {
                verified = true;
                break;
            }
            logLn("[fio] XLOAD verify mismatch at XRAM $%06X, retry %d\n",
                  (unsigned)(dest + off), attempt + 1);
        }
        if (!verified) {
            logLn("[fio] XLOAD verify failed at XRAM $%06X\n",
                  (unsigned)(dest + off));
            respond_err(ERR_IO);
            return;
        }
#else
        if (!_bridge.pokeSdramBlock(dest + off, _transfer_buf, wire_count)) {
            logLn("[fio] XLOAD write failed at XRAM $%06X\n",
                  (unsigned)(dest + off));
            respond_err(ERR_IO);
            return;
        }
#endif
        off += chunk;
    }

    write_size(total);
    logLn("[fio] XLOAD %s -> XRAM $%06X (%u bytes) OK\n",
          scratch, (unsigned)dest, (unsigned)total);
    respond_ok();
}

void FioDispatcher::handle_xsave() {
    char name[64];
    copy_filename(name);

    uint32_t src_addr = xram_addr();
    uint32_t total = transfer_len();
    if (total == 0 ||
        src_addr > XRAM_BYTES ||
        total > XRAM_BYTES ||
        src_addr > XRAM_BYTES - total) {
        respond_err(ERR_IO);
        return;
    }

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int existing = img->find_entry(scratch, parent);
    if (existing >= 0) img->delete_file(scratch, parent);

    int new_idx = img->create_file(scratch, file_type_for_name(scratch),
                                   parent, total);
    if (new_idx < 0) {
        respond_err(ERR_FULL);
        return;
    }

    uint32_t off = 0;
    while (off < total) {
        uint32_t remaining = total - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!_bridge.readSdramBlock(src_addr + off, wire_count, _transfer_buf)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        if (!img->write_file_chunk_by_index(new_idx, off, _transfer_buf, chunk)) {
            img->delete_file(scratch, parent);
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    if (!img->zero_file_tail_by_index(new_idx)) {
        img->delete_file(scratch, parent);
        respond_err(ERR_IO);
        return;
    }

    write_size(total);
    logLn("[fio] XSAVE %s <- XRAM $%06X (%u bytes) OK\n",
          scratch, (unsigned)src_addr, (unsigned)total);
    respond_ok();
}

void FioDispatcher::handle_xpage() {
    char name[64];
    copy_filename(name);

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    int idx = img->find_entry(scratch, parent);
    if (idx < 0) {
        logLn("[fio] XPAGE: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    if (!img->get_entry(idx, e) || e.is_directory()) {
        respond_err(ERR_IO);
        return;
    }

    uint8_t target = page_target();
    uint32_t dest = (target == PAGE_TARGET_XRAM) ? xram_addr() : gaddr();
    uint32_t src = file_offset();
    uint32_t requested = transfer_len();
    if (requested == 0 || src >= e.size_bytes) {
        respond_err(ERR_IO);
        return;
    }

    uint32_t total = requested;
    if (total > e.size_bytes - src) {
        total = e.size_bytes - src;
    }

    switch (target) {
        case PAGE_TARGET_XRAM:
            if (dest > XRAM_BYTES || total > XRAM_BYTES || dest > XRAM_BYTES - total) {
                respond_err(ERR_IO);
                return;
            }
            break;
        case PAGE_TARGET_RAM:
            if (dest > 0x10000UL || total > 0x10000UL || dest > 0x10000UL - total) {
                respond_err(ERR_IO);
                return;
            }
            break;
        case PAGE_TARGET_VGC:
        {
            uint32_t space_size = vgc_space_bytes(gspace());
            if (space_size == 0 ||
                dest > space_size ||
                total > space_size ||
                dest > space_size - total) {
                respond_err(ERR_IO);
                return;
            }
            break;
        }
        default:
            respond_err(ERR_IO);
            return;
    }

    uint32_t off = 0;
    while (off < total) {
        uint32_t remaining = total - off;
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, src + off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
        bool ok = false;
        switch (target) {
            case PAGE_TARGET_XRAM:
                ok = _bridge.pokeSdramBlock(dest + off, _transfer_buf, wire_count);
                break;
            case PAGE_TARGET_RAM:
                ok = _bridge.loadRam((uint16_t)(dest + off), _transfer_buf, chunk);
                break;
            case PAGE_TARGET_VGC:
                ok = _bridge.pokeVgcBlock(gspace(), (uint16_t)(dest + off),
                                           _transfer_buf, wire_count);
                break;
        }
        if (!ok) {
            logLn("[fio] XPAGE write failed target=%u addr=$%06X\n",
                  (unsigned)target, (unsigned)(dest + off));
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    write_size(total);
    logLn("[fio] XPAGE %s+$%06X -> target %u:$%06X (%u bytes) OK\n",
          scratch, (unsigned)src, (unsigned)target, (unsigned)dest, (unsigned)total);
    respond_ok();
}

// ---------------------------------------------------------------------------
// DIR_OPEN / DIR_READ — minimal iteration
// ---------------------------------------------------------------------------
void FioDispatcher::handle_dir_open() {
    // Filename buffer holds an optional path pattern. Empty → list
    // current directory of default device.
    char name[64];
    copy_filename(name);

    int slot = _dm.default_slot();
    uint16_t parent = _dm.current_dir(slot);

    if (name[0] != 0) {
        char scratch[64];
        int s;
        uint16_t p;
        // Treat the name as either "hd0:" (just dev) or
        // "hd0:dir" (dev + path). resolve_path expects a basename, so
        // we trick it with a dummy trailing element.
        // Simpler: just use slot_for_prefix to find slot.
        const char* colon = strchr(name, ':');
        if (colon && (colon - name) <= 8) {
            char prefix[16];
            size_t plen = (size_t)(colon - name);
            memcpy(prefix, name, plen);
            prefix[plen] = 0;
            int new_slot = DeviceManager::slot_for_prefix(prefix);
            if (new_slot >= 0) slot = new_slot;
        }
        // Note: subdir traversal in DIR_OPEN isn't supported in this
        // first cut — just pattern-by-prefix. CD into the subdir first.
        (void)s; (void)p; (void)scratch;
        parent = _dm.current_dir(slot);
    }

    if (!_dm.is_mounted(slot)) { respond_err(ERR_NO_MOUNT); return; }

    _dir_iter.active     = true;
    _dir_iter.slot       = slot;
    _dir_iter.parent     = parent;
    _dir_iter.next_index = 0;
    respond_ok();
}

void FioDispatcher::handle_dir_read() {
    if (!_dir_iter.active) { respond_err(ERR_IO); return; }
    auto* img = _dm.image(_dir_iter.slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    // Walk forward until we find the next active entry whose parent
    // matches. Returns ERR_NOT_FOUND when iteration is exhausted.
    while (_dir_iter.next_index < img->directory_entry_count()) {
        ndi::DirEntry e;
        if (!img->get_entry(_dir_iter.next_index, e)) break;
        _dir_iter.next_index++;
        if (e.is_active() && e.parent_index == _dir_iter.parent) {
            // Write filename + size + type back into the bank for Nova.
            int n = (int)strlen(e.filename);
            if (n > 32) n = 32;
            for (int i = 0; i < n; i++)
                _bridge.poke(BANK_BASE + OFF_NAME + i,
                             (uint8_t)e.filename[i]);
            _bridge.poke(BANK_BASE + OFF_NAME + n, 0);
            _bridge.poke(BANK_BASE + OFF_NAMELEN, (uint8_t)n);
            _bridge.poke(BANK_BASE + OFF_SIZE_LO, (uint8_t)(e.size_bytes & 0xFF));
            _bridge.poke(BANK_BASE + OFF_SIZE_HI, (uint8_t)((e.size_bytes >> 8) & 0xFF));
            _bridge.poke(BANK_BASE + OFF_DIRTYPE, e.is_directory()
                                                  ? (uint8_t)ndi::FT_DIR
                                                  : (uint8_t)e.file_type);
            respond_ok();
            return;
        }
    }
    _dir_iter.active = false;
    respond_err(ERR_NOT_FOUND);
}

// ---------------------------------------------------------------------------
// DELETE
// ---------------------------------------------------------------------------
void FioDispatcher::handle_delete() {
    char name[64];
    copy_filename(name);
    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND); return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }
    if (!img->delete_file(scratch, parent)) {
        respond_err(ERR_NOT_FOUND); return;
    }
    respond_ok();
}

// ---------------------------------------------------------------------------
// CD / MKDIR / RMDIR
// ---------------------------------------------------------------------------
void FioDispatcher::handle_cd() {
    char name[64];
    copy_filename(name);

    // Special case: bare "<prefix>:" sets default device.
    const char* colon = strchr(name, ':');
    int slot = _dm.default_slot();
    const char* tail = name;
    if (colon && (colon - name) <= 8) {
        char prefix[16];
        size_t plen = (size_t)(colon - name);
        memcpy(prefix, name, plen);
        prefix[plen] = 0;
        int s = DeviceManager::slot_for_prefix(prefix);
        if (s < 0) { respond_err(ERR_NOT_FOUND); return; }
        slot = s;
        _dm.set_default_slot(slot);
        tail = colon + 1;
    }
    if (!_dm.is_mounted(slot)) { respond_err(ERR_NO_MOUNT); return; }
    if (!_dm.cd(slot, tail))    { respond_err(ERR_NOT_FOUND); return; }
    respond_ok();
}

void FioDispatcher::handle_mkdir() {
    char name[64];
    copy_filename(name);
    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND); return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }
    if (img->make_directory(scratch, parent) < 0) {
        respond_err(ERR_FULL); return;
    }
    respond_ok();
}

void FioDispatcher::handle_rmdir() {
    char name[64];
    copy_filename(name);
    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND); return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }
    if (!img->remove_directory(scratch, parent)) {
        respond_err(ERR_IO); return;
    }
    respond_ok();
}

// ---------------------------------------------------------------------------
// MOUNT / UNMOUNT
// ---------------------------------------------------------------------------
void FioDispatcher::handle_mount() {
    // BASIC LAB_MOUNT packs both args into the name buffer separated by a
    // NULL byte: e.g. `MOUNT "FD0:","fd0"` becomes "FD0:\0fd0" (FIO_NAMELEN
    // covers all 8 bytes). Skip past one or more NULL separators to find
    // the second string.
    char name[64];
    copy_filename(name);
    int total = namelen();
    if (total < 0) total = 0;
    if (total > 63) total = 63;

    char* colon = strchr(name, ':');
    if (!colon) { respond_err(ERR_IO); return; }
    *colon = 0;
    int slot = DeviceManager::slot_for_prefix(name);
    if (slot < 0) { respond_err(ERR_IO); return; }

    // Walk past null separator(s) to the start of the image-name string.
    char* second = colon + 1;
    char* end    = name + total;
    while (second < end && *second == 0) second++;
    if (second >= end || *second == 0) { respond_err(ERR_IO); return; }

    char sd_path[96];
    snprintf(sd_path, sizeof(sd_path), "/%s.ndi", second);

    if (!_dm.mount(slot, sd_path)) {
        respond_err(ERR_NOT_FOUND); return;
    }
    respond_ok();
}

void FioDispatcher::handle_unmount() {
    char name[64];
    copy_filename(name);
    char* colon = strchr(name, ':');
    if (colon) *colon = 0;
    int slot = DeviceManager::slot_for_prefix(name);
    if (slot < 0) { respond_err(ERR_IO); return; }
    _dm.unmount(slot);
    respond_ok();
}

void FioDispatcher::handle_pwd() {
    int slot = _dm.default_slot();
    if (!_dm.is_mounted(slot)) {
        respond_err(ERR_NO_MOUNT);
        return;
    }

    const char* prefix = DeviceManager::prefix_for_slot(slot);
    const char* path = _dm.current_path(slot);
    if (!prefix || !path) {
        respond_err(ERR_IO);
        return;
    }

    char pwd[64];
    snprintf(pwd, sizeof(pwd), "%s:%s", prefix, path);
    int len = (int)strlen(pwd);
    if (len > 63) len = 63;

    bool ok = _bridge.poke(BANK_BASE + OFF_NAMELEN, (uint8_t)len);
    for (int i = 0; i < len && ok; i++) {
        ok = _bridge.poke(BANK_BASE + OFF_NAME + i, (uint8_t)pwd[i]);
    }
    if (ok && len < 64) {
        ok = _bridge.poke(BANK_BASE + OFF_NAME + len, 0);
    }
    if (!ok) {
        respond_err(ERR_IO);
        return;
    }

    logLn("[fio] PWD %s\n", pwd);
    respond_ok();
}
