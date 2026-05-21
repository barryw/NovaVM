#include "fio_dispatcher.h"
#include <Arduino.h>
#include <esp_attr.h>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <math.h>
#include <string.h>
#include <utility>
#include <vector>

extern void logLn(const char* fmt, ...);
extern void novaHostFioActivityStarted();
extern void novaHostFioActivityFinished(bool ok);

#ifndef NOVAHOST_XLOAD_VERIFY
#define NOVAHOST_XLOAD_VERIFY 0
#endif

namespace {
constexpr uint32_t MIDI_DEBUG_MAGIC = 0x4D494442UL; // MIDB

struct MidiDebugState {
    uint32_t magic;
    uint32_t phase;
    uint32_t marker;
    uint32_t note_ons;
    uint32_t sample_note_ons;
    uint32_t sample_misses;
    uint32_t sample_pos;
    uint32_t sample_end;
    uint32_t heap_free;
    uint32_t heap_largest;
    uint32_t millis_at_phase;
};

RTC_NOINIT_ATTR MidiDebugState g_midi_debug;

void midi_debug_init() {
    if (g_midi_debug.magic == MIDI_DEBUG_MAGIC)
        return;
    memset(&g_midi_debug, 0, sizeof(g_midi_debug));
    g_midi_debug.magic = MIDI_DEBUG_MAGIC;
}

void midi_debug_phase(uint32_t phase, uint32_t marker = 0) {
    midi_debug_init();
    g_midi_debug.phase = phase;
    g_midi_debug.marker = marker;
    g_midi_debug.heap_free = ESP.getFreeHeap();
    g_midi_debug.heap_largest = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    g_midi_debug.millis_at_phase = millis();
}

void midi_debug_counts(uint32_t note_ons, uint32_t sample_note_ons,
                       uint32_t sample_misses) {
    midi_debug_init();
    g_midi_debug.note_ons = note_ons;
    g_midi_debug.sample_note_ons = sample_note_ons;
    g_midi_debug.sample_misses = sample_misses;
}

void midi_debug_sample(uint32_t pos, uint32_t end) {
    midi_debug_init();
    g_midi_debug.sample_pos = pos;
    g_midi_debug.sample_end = end;
}

uint32_t fnv1a_update(uint32_t hash, const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619UL;
    }
    return hash;
}

uint32_t read_le32(const uint8_t* data) {
    return (uint32_t)data[0] |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

constexpr uint16_t LOCAL_WTS_EVENT_BYTES = 6;

bool append_wts_event_record(uint8_t* out, uint16_t out_capacity,
                             uint16_t& out_bytes, uint32_t frame,
                             uint8_t off, uint8_t value) {
    if (!out || out_bytes + LOCAL_WTS_EVENT_BYTES > out_capacity)
        return false;
    out[out_bytes++] = (uint8_t)(frame & 0xFF);
    out[out_bytes++] = (uint8_t)((frame >> 8) & 0xFF);
    out[out_bytes++] = (uint8_t)((frame >> 16) & 0xFF);
    out[out_bytes++] = (uint8_t)((frame >> 24) & 0xFF);
    out[out_bytes++] = off;
    out[out_bytes++] = value;
    return true;
}

bool append_wts_addr25(uint8_t* out, uint16_t out_capacity,
                       uint16_t& out_bytes, uint32_t frame, uint8_t off,
                       uint32_t value) {
    return append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 0, (uint8_t)(value & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 1,
                                   (uint8_t)((value >> 8) & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 2,
                                   (uint8_t)((value >> 16) & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 3,
                                   (uint8_t)((value >> 24) & 0x01));
}

bool append_wts_u32(uint8_t* out, uint16_t out_capacity, uint16_t& out_bytes,
                    uint32_t frame, uint8_t off, uint32_t value) {
    return append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 0, (uint8_t)(value & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 1,
                                   (uint8_t)((value >> 8) & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 2,
                                   (uint8_t)((value >> 16) & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame,
                                   off + 3,
                                   (uint8_t)((value >> 24) & 0xFF));
}

bool append_wts_env_u16(uint8_t* out, uint16_t out_capacity,
                        uint16_t& out_bytes, uint32_t frame, uint8_t index,
                        uint16_t value) {
    return append_wts_event_record(out, out_capacity, out_bytes, frame, 0x5E,
                                   index) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame, 0x5F,
                                   (uint8_t)(value & 0xFF)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame, 0x5E,
                                   (uint8_t)(index + 1)) &&
           append_wts_event_record(out, out_capacity, out_bytes, frame, 0x5F,
                                   (uint8_t)(value >> 8));
}

uint8_t hardware_instrument_for(uint8_t channel, uint8_t program) {
    if (channel == 9) return 6;
    if (program <= 31) return 2;
    if (program <= 39) return 3;
    if (program <= 55) return 1;
    if (program <= 63) return 6;
    if (program <= 71) return 5;
    if (program <= 79) return 0;
    if (program <= 95) return 7;
    return 4;
}

ndi::FileType file_type_for_name(const char* name) {
    const char* ext = strrchr(name, '.');
    if (!ext) return ndi::FT_BAS;
    if      (strcasecmp(ext, ".bas")  == 0) return ndi::FT_BAS;
    else if (strcasecmp(ext, ".sid")  == 0) return ndi::FT_SID;
    else if (strcasecmp(ext, ".bin")  == 0) return ndi::FT_BIN;
    else if (strcasecmp(ext, ".xram") == 0) return ndi::FT_BIN;
    else if (strcasecmp(ext, ".mid")  == 0) return ndi::FT_MID;
    else if (strcasecmp(ext, ".nms")  == 0) return ndi::FT_MID;
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

bool has_extension_ci(const char* name, const char* ext) {
    if (!name || !ext)
        return false;
    const char* actual = strrchr(name, '.');
    return actual && strcasecmp(actual, ext) == 0;
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

int find_music_entry(ndi::NdiImage* img, char* name, size_t name_size,
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

    strcat(name, ".nms");
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

    size_t pos() const { return _pos; }
    size_t size() const { return _size; }

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
    bool seek(size_t pos) {
        if (pos > _size) return false;
        _pos = (uint32_t)pos;
        _cache_valid = 0;
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
        yield();
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
constexpr uint16_t VGC_IRQ_ENABLE_ADDR   = 0xA0F0;
constexpr uint16_t VGC_IRQ_STATUS_ADDR   = 0xA0F1;
constexpr uint16_t VGC_IRQ_TIMER_LO_ADDR = 0xA0F4;
constexpr uint16_t VGC_IRQ_TIMER_MID_ADDR = 0xA0F5;
constexpr uint16_t VGC_IRQ_TIMER_HI_ADDR = 0xA0F6;
constexpr uint16_t VGC_IRQ_TIMER_CTL_ADDR = 0xA0F7;
constexpr uint8_t  VGC_IRQ_TIMER         = 0x80;
constexpr uint8_t  VGC_IRQ_SPRCOLL       = 0x20;
constexpr uint8_t  IRQ_BASIC_IRQ_BASE_ZP = 0xDF;
constexpr uint16_t SID1_BASE             = 0xD400;
constexpr uint16_t SID2_BASE             = 0xD420;
constexpr uint16_t SID_CFG_ADDR          = 0xD440;

constexpr uint16_t WTS_VOICE_BASE        = 0xA140;
constexpr uint16_t WTS_VOICE_STRIDE      = 8;
constexpr uint16_t WTS_MASTER_VOLUME     = 0xA182;
constexpr uint16_t WTS_SOUNDFONT_STATUS  = 0xA183;
constexpr uint16_t WTS_INSTRUMENT_COUNT  = 0xA184;
constexpr uint16_t WTS_COMMAND           = 0xA185;
constexpr uint16_t WTS_SIGNATURE         = 0xA186;
constexpr uint16_t WTS_ACTIVE_MASK       = 0xA187;
constexpr uint8_t  WTS_CMD_ALL_NOTES_OFF = 0x01;
constexpr uint8_t  WTS_CMD_EVENT_RESET   = 0x03;
constexpr uint8_t  WTS_CMD_EVENT_START   = 0x04;
constexpr uint8_t  WTS_CMD_EVENT_STOP    = 0x05;
constexpr uint8_t  WTS_SIGNATURE_VALUE   = 0x57;
constexpr uint16_t WTS_SAMPLE_VOICE      = 0xA188;
constexpr uint16_t WTS_SAMPLE_START      = 0xA189;
constexpr uint16_t WTS_SAMPLE_END        = 0xA18D;
constexpr uint16_t WTS_SAMPLE_LOOP_START = 0xA191;
constexpr uint16_t WTS_SAMPLE_LOOP_END   = 0xA195;
constexpr uint16_t WTS_SAMPLE_PHASE_STEP = 0xA199;
constexpr uint16_t WTS_SAMPLE_FLAGS      = 0xA19D;
constexpr uint16_t WTS_ENV_PARAM_INDEX   = 0xA19E;
constexpr uint16_t WTS_ENV_PARAM_DATA    = 0xA19F;
constexpr uint16_t WTS_EVENT_STATUS      = 0xA1E0;
constexpr uint16_t WTS_EVENT_COUNT_L     = 0xA1E1;
constexpr uint16_t WTS_EVENT_COUNT_H     = 0xA1E2;
constexpr uint16_t WTS_EVENT_FREE_L      = 0xA1E3;
constexpr uint16_t WTS_EVENT_FREE_H      = 0xA1E4;
constexpr uint8_t  WTS_EVENT_STATUS_READY = 0x01;
constexpr uint8_t  WTS_EVENT_STATUS_EMPTY = 0x04;
constexpr uint8_t  WTS_EVENT_STATUS_ERROR = 0x80;
constexpr uint16_t WTS_DEFAULT_ATTACK_STEP = 0x0300;
constexpr uint16_t WTS_DEFAULT_DECAY_STEP = 0x0020;
constexpr uint16_t WTS_DEFAULT_SUSTAIN_LEVEL = 0xA000;
constexpr uint16_t WTS_DEFAULT_RELEASE_STEP = 0x0004;
constexpr uint32_t WTS_SAMPLE_SDRAM_BASE = 0x082000UL;
constexpr uint32_t WTS_SAMPLE_SDRAM_LIMIT = 0x1000000UL;
constexpr uint32_t WTS_AUDIO_SAMPLE_RATE = 48000UL;
constexpr uint16_t MIDI_WTS_EVENT_BYTES  = 6;
constexpr uint16_t MIDI_WTS_EVENT_PUMP_RECORDS = 96;
constexpr uint16_t MIDI_WTS_EVENT_NOTE_RECORDS_MAX = 64;
constexpr uint32_t NSFB_MAGIC            = 0x4246534EUL;
constexpr uint16_t NSFB_VERSION_PCM8     = 1;
constexpr uint16_t NSFB_VERSION_PCM16    = 2;
constexpr uint16_t NSFB_HEADER_SIZE_V1   = 34;
constexpr uint16_t NSFB_HEADER_SIZE_V2   = 38;
constexpr uint16_t NSFB_SAMPLE_FORMAT_PCM8  = 1;
constexpr uint16_t NSFB_SAMPLE_FORMAT_PCM16 = 2;
constexpr uint32_t NMS_MAGIC             = 0x32534D4EUL; // "NMS2"
constexpr uint16_t NMS_VERSION           = 2;
constexpr uint16_t NMS_HEADER_SIZE       = 32;
constexpr uint16_t NMS_EVENT_RECORD_SIZE = 10;
constexpr uint8_t  NMS_EVENT_NOTE_OFF    = 0;
constexpr uint8_t  NMS_EVENT_NOTE_ON     = 1;
constexpr uint16_t NMS_WTS_NOTE_ON_RECORDS = 41;
constexpr uint16_t NMS_WTS_NOTE_OFF_RECORDS = 1;

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

uint16_t wts_env_step_from_frames(uint16_t frames, uint16_t span) {
    if (span == 0)
        return 0;
    if (frames == 0)
        return 0xFFFF;

    uint32_t step = ((uint32_t)span + frames - 1) / frames;
    if (step == 0)
        step = 1;
    if (step > 0xFFFF)
        step = 0xFFFF;
    return (uint16_t)step;
}

uint16_t wts_attack_step_from_frames(uint16_t frames) {
    return wts_env_step_from_frames(frames, 0xFFFF);
}

uint16_t wts_decay_step_from_frames(uint16_t frames, uint16_t sustain) {
    return wts_env_step_from_frames(frames, (uint16_t)(0xFFFF - sustain));
}

uint16_t wts_release_step_from_frames(uint16_t frames) {
    return wts_env_step_from_frames(frames, 0xFFFF);
}

bool configure_sid(FpgaBridge& bridge, uint8_t config) {
    return bridge.poke(SID_CFG_ADDR, config);
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
        case CMD_MIDPLAY:  handle_midplay();  break;
        case CMD_MIDSTOP:  handle_midstop();  break;
        case CMD_SFLOAD:   handle_sfload();   break;
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
    pump_audio();

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

void FioDispatcher::pump_audio() {
    tick_midi_playback();
    tick_sid_playback();
}

void FioDispatcher::stop_audio() {
    _midi_last_stop_reason = 4;
    stop_midi_playback(true);
    stop_sid_playback(true);
}

void FioDispatcher::clear_wts_bank_state() {
    _wts_bank_loaded = false;
    _wts_samples_resident = false;
    _wts_bank_name[0] = 0;
    _wts_sample_bytes = 0;
    _wts_sample_frame_bytes = 1;
    _wts_bank_hash = 0;
    _wts_instruments.clear();
    _wts_regions.clear();
    release_wts_sample_cache();
}

FioDispatcher::WtsRegionStore::~WtsRegionStore() {
    clear();
}

FioDispatcher::WtsRegionStore::WtsRegionStore(WtsRegionStore&& other)
    noexcept {
    move_from(other);
}

FioDispatcher::WtsRegionStore&
FioDispatcher::WtsRegionStore::operator=(WtsRegionStore&& other) noexcept {
    if (this != &other) {
        clear();
        move_from(other);
    }
    return *this;
}

void FioDispatcher::WtsRegionStore::clear() {
    for (uint16_t i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i]) {
            heap_caps_free(chunks[i]);
            chunks[i] = nullptr;
        }
    }
    count = 0;
    capacity = 0;
}

bool FioDispatcher::WtsRegionStore::reserve(uint16_t requested) {
    if (requested <= capacity)
        return true;

    uint16_t needed_chunks =
        (requested + CHUNK_SIZE - 1) / CHUNK_SIZE;
    if (needed_chunks > MAX_CHUNKS)
        return false;

    uint16_t have_chunks = capacity / CHUNK_SIZE;
    for (uint16_t i = have_chunks; i < needed_chunks; i++) {
        chunks[i] = (WtsBankRegion*)heap_caps_malloc(
            CHUNK_SIZE * sizeof(FioDispatcher::WtsBankRegion),
            MALLOC_CAP_8BIT);
        if (!chunks[i]) {
            for (uint16_t j = have_chunks; j < i; j++) {
                heap_caps_free(chunks[j]);
                chunks[j] = nullptr;
            }
            capacity = have_chunks * CHUNK_SIZE;
            return false;
        }
    }

    capacity = needed_chunks * CHUNK_SIZE;
    return true;
}

bool FioDispatcher::WtsRegionStore::push_back(
    const WtsBankRegion& region) {
    if (count >= capacity && !reserve(count + 1))
        return false;

    chunks[count / CHUNK_SIZE][count % CHUNK_SIZE] = region;
    count++;
    return true;
}

void FioDispatcher::WtsRegionStore::move_from(WtsRegionStore& other) {
    for (uint16_t i = 0; i < MAX_CHUNKS; i++) {
        chunks[i] = other.chunks[i];
        other.chunks[i] = nullptr;
    }
    count = other.count;
    capacity = other.capacity;
    other.count = 0;
    other.capacity = 0;
}

bool FioDispatcher::reserve_wts_sample_cache(uint32_t bytes) {
    if (bytes == 0)
        return false;
    if (bytes > WTS_RESIDENT_SAMPLE_LIMIT) {
        logLn("[fio] WTS resident sample cache request too large: bytes=%lu "
              "limit=%lu",
              (unsigned long)bytes,
              (unsigned long)WTS_RESIDENT_SAMPLE_LIMIT);
        return false;
    }
    if (_wts_sample_capacity >= bytes)
        return true;

    uint32_t wanted_chunks =
        (bytes + WTS_RESIDENT_SAMPLE_CHUNK_BYTES - 1) /
        WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
    if (wanted_chunks == 0)
        return false;

    while (_wts_sample_chunks.size() < wanted_chunks) {
        uint32_t free_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        uint32_t largest_before =
            heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        if (free_before <=
            WTS_RESIDENT_HEAP_GUARD + WTS_RESIDENT_SAMPLE_CHUNK_BYTES) {
            logLn("[fio] WTS resident sample cache chunk request leaves too "
                  "little heap: want=%lu capacity=%lu free=%u guard=%lu "
                  "largest=%u",
                  (unsigned long)bytes,
                  (unsigned long)_wts_sample_capacity,
                  (unsigned)free_before,
                  (unsigned long)WTS_RESIDENT_HEAP_GUARD,
                  (unsigned)largest_before);
            return false;
        }

        uint8_t* chunk = (uint8_t*)heap_caps_malloc(
            WTS_RESIDENT_SAMPLE_CHUNK_BYTES, MALLOC_CAP_8BIT);
        if (!chunk) {
            logLn("[fio] WTS resident sample cache chunk allocation failed: "
                  "want=%lu capacity=%lu heapFree=%u heapLargest=%u",
                  (unsigned long)bytes,
                  (unsigned long)_wts_sample_capacity,
                  (unsigned)free_before,
                  (unsigned)largest_before);
            return false;
        }

        _wts_sample_chunks.push_back(chunk);
        _wts_sample_capacity =
            (uint32_t)_wts_sample_chunks.size() *
            WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
        yield();
    }

    logLn("[fio] WTS resident sample cache reserved: bytes=%lu chunks=%u "
          "heapFree=%u heapLargest=%u",
          (unsigned long)_wts_sample_capacity,
          (unsigned)_wts_sample_chunks.size(),
          (unsigned)ESP.getFreeHeap(),
          (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    return true;
}

void FioDispatcher::release_wts_sample_cache() {
    for (uint8_t* chunk : _wts_sample_chunks) {
        if (chunk)
            heap_caps_free(chunk);
    }
    _wts_sample_chunks.clear();
    _wts_sample_capacity = 0;
    logLn("[fio] WTS resident sample cache released heapFree=%u heapLargest=%u",
          (unsigned)ESP.getFreeHeap(),
          (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

bool FioDispatcher::cache_wts_sample_bytes(uint32_t offset,
                                           const uint8_t* data,
                                           uint16_t len) {
    if (!data || len == 0)
        return true;
    if ((uint64_t)offset + len > _wts_sample_capacity)
        return false;

    uint32_t copied = 0;
    while (copied < len) {
        uint32_t absolute = offset + copied;
        uint32_t chunk_index = absolute / WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
        uint32_t chunk_offset = absolute % WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
        if (chunk_index >= _wts_sample_chunks.size() ||
            !_wts_sample_chunks[chunk_index]) {
            return false;
        }

        uint32_t chunk_len = WTS_RESIDENT_SAMPLE_CHUNK_BYTES - chunk_offset;
        uint32_t remaining = len - copied;
        if (chunk_len > remaining)
            chunk_len = remaining;
        memcpy(_wts_sample_chunks[chunk_index] + chunk_offset,
               data + copied, chunk_len);
        copied += chunk_len;
    }

    return true;
}

uint8_t FioDispatcher::read_wts_sample_byte(uint32_t offset) const {
    if (offset >= _wts_sample_capacity)
        return 0;

    uint32_t chunk_index = offset / WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
    uint32_t chunk_offset = offset % WTS_RESIDENT_SAMPLE_CHUNK_BYTES;
    if (chunk_index >= _wts_sample_chunks.size() ||
        !_wts_sample_chunks[chunk_index]) {
        return 0;
    }

    return _wts_sample_chunks[chunk_index][chunk_offset];
}

bool FioDispatcher::write_wts_sample_chunk_to_sdram(uint32_t offset,
                                                    const uint8_t* data,
                                                    uint16_t len) {
    if (!data || len == 0)
        return true;

    midi_debug_sample(offset, g_midi_debug.sample_end);
    if (!_bridge.pokeSdramStream(WTS_SAMPLE_SDRAM_BASE + offset, data, len)) {
        midi_debug_phase(498, offset);
        return false;
    }

    return true;
}

void FioDispatcher::write_audio_status_json(char* out, size_t out_len) const {
    if (!out || out_len == 0)
        return;

    midi_debug_init();
    snprintf(out, out_len,
             "{\"midiPlaying\":%s,\"wtsBankLoaded\":%s,"
             "\"wtsSamplesResident\":%s,\"wtsBankName\":\"%s\","
             "\"wtsSampleBytes\":%lu,\"wtsSampleCapacity\":%lu,"
             "\"wtsSampleFrameBytes\":%u,\"wtsBankHash\":%lu,"
             "\"midiHardwareWts\":%s,\"midiNoteOns\":%lu,"
             "\"midiSampleNoteOns\":%lu,\"midiSampleMisses\":%lu,"
             "\"midiStopReason\":%u,"
             "\"sidPlaying\":%s,\"sidName\":\"%s\","
             "\"sidSong\":%u,\"sidFrames\":%lu,"
             "\"sidFramePeriodUs\":%lu,\"sidPages\":%u,"
             "\"sidBytes\":%lu,\"sidWrites\":%lu,"
             "\"sidStopReason\":%u,\"sidLastError\":\"%s\","
             "\"musicEventBytes\":%lu,\"musicEventRead\":%lu,"
             "\"musicEventCount\":%lu,\"musicTotalFrames\":%lu,"
             "\"midiEventIndex\":%u,"
             "\"midiTick\":%lu,\"midiLastError\":\"%s\","
             "\"heapFree\":%u,\"heapLargest\":%u,"
             "\"timing\":{\"pumpCalls\":%lu,\"lastPumpGapUs\":%lu,"
             "\"maxPumpGapUs\":%lu,\"lastPumpUs\":%lu,"
             "\"maxPumpUs\":%lu},"
             "\"debug\":{\"phase\":%lu,\"marker\":%lu,"
             "\"noteOns\":%lu,\"sampleNoteOns\":%lu,"
             "\"sampleMisses\":%lu,"
             "\"samplePos\":%lu,\"sampleEnd\":%lu,"
             "\"heapFree\":%lu,\"heapLargest\":%lu,\"millis\":%lu}}",
             _midi_playing ? "true" : "false",
             _wts_bank_loaded ? "true" : "false",
             _wts_samples_resident ? "true" : "false",
             _wts_bank_name,
             (unsigned long)_wts_sample_bytes,
             (unsigned long)_wts_sample_capacity,
             (unsigned)_wts_sample_frame_bytes,
             (unsigned long)_wts_bank_hash,
             _midi_use_hardware_wts ? "true" : "false",
             (unsigned long)_midi_note_ons,
             (unsigned long)_midi_sample_note_ons,
             (unsigned long)_midi_sample_misses,
             (unsigned)_midi_last_stop_reason,
             _sid_playing ? "true" : "false",
             _sid_name,
             (unsigned)_sid_song,
             (unsigned long)_sid_frames,
             (unsigned long)_sid_frame_period_us,
             (unsigned)_sid_vm.stats().pagesAllocated,
             (unsigned long)_sid_vm.stats().bytesAllocated,
             (unsigned long)_sid_vm.stats().sidWrites,
             (unsigned)_sid_last_stop_reason,
             _sid_last_error,
             (unsigned long)_music_event_bytes,
             (unsigned long)_music_event_read,
             (unsigned long)_music_event_count,
             (unsigned long)_music_total_frames,
             (unsigned)_midi_event_index,
             (unsigned long)_midi_tick_accum,
             _midi_last_error,
             (unsigned)ESP.getFreeHeap(),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
             (unsigned long)_midi_pump_calls,
             (unsigned long)_midi_last_pump_gap_us,
             (unsigned long)_midi_max_pump_gap_us,
             (unsigned long)_midi_last_pump_duration_us,
             (unsigned long)_midi_max_pump_duration_us,
             (unsigned long)g_midi_debug.phase,
             (unsigned long)g_midi_debug.marker,
             (unsigned long)g_midi_debug.note_ons,
             (unsigned long)g_midi_debug.sample_note_ons,
             (unsigned long)g_midi_debug.sample_misses,
             (unsigned long)g_midi_debug.sample_pos,
             (unsigned long)g_midi_debug.sample_end,
             (unsigned long)g_midi_debug.heap_free,
             (unsigned long)g_midi_debug.heap_largest,
             (unsigned long)g_midi_debug.millis_at_phase);
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

bool FioDispatcher::load_file_to_vector(ndi::NdiImage* img, int idx,
                                        uint32_t size,
                                        std::vector<uint8_t>& out) {
    if (!img || size == 0 || size > 1024UL * 1024UL)
        return false;

    out.clear();
    out.resize(size);

    uint32_t off = 0;
    while (off < size) {
        uint16_t chunk = (size - off >= 256) ? 256 : (uint16_t)(size - off);
        int got = img->read_file_chunk_by_index(idx, off, _transfer_buf, chunk);
        if (got != (int)chunk)
            return false;
        memcpy(out.data() + off, _transfer_buf, chunk);
        off += chunk;
    }

    return true;
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

bool FioDispatcher::on_sid_write_static(void* user, uint16_t addr,
                                        uint8_t value) {
    return static_cast<FioDispatcher*>(user)->on_sid_write(addr, value);
}

bool FioDispatcher::on_sid_write(uint16_t addr, uint8_t value) {
    if (_sid_write_count >= SID_WRITE_BATCH_MAX) {
        _sid_write_overflow = true;
        return false;
    }

    _sid_write_addrs[_sid_write_count] = addr;
    _sid_write_values[_sid_write_count] = value;
    _sid_write_count++;
    return true;
}

bool FioDispatcher::flush_sid_writes() {
    if (_sid_write_overflow) {
        snprintf(_sid_last_error, sizeof(_sid_last_error),
                 "SID write batch overflow");
        return false;
    }
    if (_sid_write_count == 0)
        return true;

    bool ok = _bridge.pokeMulti(_sid_write_addrs, _sid_write_values,
                                _sid_write_count);
    _sid_write_count = 0;
    if (!ok) {
        snprintf(_sid_last_error, sizeof(_sid_last_error),
                 "SID register flush failed");
    }
    return ok;
}

void FioDispatcher::tick_sid_playback() {
    if (!_sid_playing)
        return;

    uint32_t now = micros();
    if ((int32_t)(now - _sid_next_frame_us) < 0)
        return;

    _sid_write_count = 0;
    _sid_write_overflow = false;
    nova_sid::RunResult r = _sid_vm.runPlayFrame();
    if (r.status != nova_sid::RunStatus::Ok || !flush_sid_writes()) {
        snprintf(_sid_last_error, sizeof(_sid_last_error),
                 "SID frame failed: %s pc=$%04X op=$%02X",
                 nova_sid::run_status_name(r.status),
                 (unsigned)r.pc, (unsigned)r.opcode);
        logLn("[fio] %s", _sid_last_error);
        _sid_last_stop_reason = 2;
        stop_sid_playback(true);
        return;
    }

    _sid_frames++;
    _sid_next_frame_us += _sid_frame_period_us;
    if ((int32_t)(now - _sid_next_frame_us) > (int32_t)_sid_frame_period_us)
        _sid_next_frame_us = now + _sid_frame_period_us;
}

void FioDispatcher::stop_sid_playback(bool silence) {
    _sid_playing = false;
    _sid_write_count = 0;
    _sid_write_overflow = false;
    _sid_vm.reset();
    _sid_vm.setSidWriteHandler(on_sid_write_static, this);

    if (silence) {
        set_sid_timer(_bridge, 0, false);
        restore_basic_irq(_bridge);
        configure_sid(_bridge, 0);
        silence_sid_chips(_bridge);
    }
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

    nova_sid::SidFileInfo sid;
    if (!nova_sid::parse_sid_header(_transfer_buf, e.size_bytes, sid)) {
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
    if (sid.payloadBytes == 0 || sid.payloadBytes > 65536UL ||
        load_end > 0x10000UL || sid.initAddress == 0) {
        logLn("[fio] SIDPLAY %s: unsupported SID layout load=$%04X size=%lu init=$%04X play=$%04X\n",
              scratch, (unsigned)sid.loadAddress,
              (unsigned long)sid.payloadBytes,
              (unsigned)sid.initAddress, (unsigned)sid.playAddress);
        respond_err(ERR_IO);
        return;
    }

    stop_midi_playback(true);
    stop_sid_playback(true);
    _sid_vm.setSidWriteHandler(on_sid_write_static, this);

    bool ok = configure_sid(_bridge, nova_sid::sid_fpga_config(sid));
    uint32_t off = 0;
    while (ok && off < sid.payloadBytes) {
        uint32_t remaining = sid.payloadBytes - off;
        uint16_t chunk = remaining > TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
        got = img->read_file_chunk_by_index(idx, sid.payloadFileOffset + off,
                                            _transfer_buf, chunk);
        if (got != (int)chunk) {
            ok = false;
            break;
        }
        if (!_sid_vm.loadPayload((uint16_t)(sid.loadAddress + off),
                                 _transfer_buf, chunk)) {
            snprintf(_sid_last_error, sizeof(_sid_last_error),
                     "SID VM out of memory");
            ok = false;
            break;
        }
        off += chunk;
        yield();
    }

    if (ok) {
        _sid_vm.setEntryPoints(sid.initAddress, sid.playAddress);
        _sid_write_count = 0;
        _sid_write_overflow = false;
        nova_sid::RunResult init = _sid_vm.runInit((uint8_t)(song - 1));
        if (init.status != nova_sid::RunStatus::Ok) {
            snprintf(_sid_last_error, sizeof(_sid_last_error),
                     "SID init failed: %s pc=$%04X op=$%02X",
                     nova_sid::run_status_name(init.status),
                     (unsigned)init.pc, (unsigned)init.opcode);
            ok = false;
        } else if (!flush_sid_writes()) {
            ok = false;
        }
    }

    if (!ok) {
        logLn("[fio] SIDPLAY %s failed: %s\n", scratch, _sid_last_error);
        stop_sid_playback(true);
        respond_err(ERR_IO);
        return;
    }

    snprintf(_sid_name, sizeof(_sid_name), "%s", scratch);
    _sid_song = song;
    _sid_frames = 0;
    _sid_last_stop_reason = 0;
    _sid_last_error[0] = 0;
    _sid_frame_period_us = nova_sid::sid_frame_period_us(sid, song);
    _sid_next_frame_us = micros() + _sid_frame_period_us;
    _sid_playing = sid.playAddress != 0;

    write_size(sid.payloadBytes);
    logLn("[fio] SIDPLAY %s %s load=$%04X init=$%04X play=$%04X song=%u speed=$%08lX flags=$%04X period_us=%lu cfg=$%02X pages=%u OK\n",
          scratch, sid.magic, (unsigned)sid.loadAddress,
          (unsigned)sid.initAddress, (unsigned)sid.playAddress,
          (unsigned)song,
          (unsigned long)sid.speed, (unsigned)sid.flags,
          (unsigned long)_sid_frame_period_us,
          (unsigned)nova_sid::sid_fpga_config(sid),
          (unsigned)_sid_vm.stats().pagesAllocated);
    respond_ok();
}

void FioDispatcher::handle_sidstop() {
    stop_sid_playback(true);
    logLn("[fio] SIDSTOP OK\n");
    respond_ok();
}

// ---------------------------------------------------------------------------
// MIDPLAY / MIDSTOP
// ---------------------------------------------------------------------------
bool FioDispatcher::load_wts_bank_from_entry(ndi::NdiImage* img, int idx,
                                             uint32_t size,
                                             const char* label) {
    midi_debug_phase(450, size);
    if (!img || idx < 0 || size < NSFB_HEADER_SIZE_V1) {
        midi_debug_phase(590, size);
        return false;
    }

    uint32_t bank_hash = 2166136261UL;
    for (uint32_t hash_off = 0; hash_off < size; ) {
        uint32_t remaining = size - hash_off;
        uint16_t chunk = remaining >= TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, hash_off,
                                                _transfer_buf, chunk);
        if (got != (int)chunk)
            return false;
        bank_hash = fnv1a_update(bank_hash, _transfer_buf, chunk);
        hash_off += chunk;
    }

    NdiFileReader reader(img, idx, size, _transfer_buf, 256);
    uint32_t magic = 0;
    uint16_t version = 0;
    uint16_t header_size = 0;
    uint16_t instrument_record_size = 0;
    uint16_t region_record_size = 0;
    uint16_t instrument_count = 0;
    uint16_t region_count = 0;
    uint16_t output_rate = 0;
    uint32_t instrument_table_offset = 0;
    uint32_t region_table_offset = 0;
    uint32_t sample_data_offset = 0;
    uint32_t sample_data_bytes = 0;
    uint16_t sample_format = NSFB_SAMPLE_FORMAT_PCM8;
    uint16_t reserved16 = 0;

    if (!reader.read_u32(magic) ||
        !reader.read_u16(version) ||
        !reader.read_u16(header_size) ||
        !reader.read_u16(instrument_record_size) ||
        !reader.read_u16(region_record_size) ||
        !reader.read_u16(instrument_count) ||
        !reader.read_u16(region_count) ||
        !reader.read_u16(output_rate) ||
        !reader.read_u32(instrument_table_offset) ||
        !reader.read_u32(region_table_offset) ||
        !reader.read_u32(sample_data_offset) ||
        !reader.read_u32(sample_data_bytes)) {
        midi_debug_phase(591, size);
        return false;
    }

    if (version == NSFB_VERSION_PCM16) {
        if (header_size < NSFB_HEADER_SIZE_V2 ||
            !reader.read_u16(sample_format) ||
            !reader.read_u16(reserved16)) {
            midi_debug_phase(592, header_size);
            return false;
        }
    } else {
        sample_format = NSFB_SAMPLE_FORMAT_PCM8;
    }

    uint8_t sample_frame_bytes = sample_format == NSFB_SAMPLE_FORMAT_PCM16
        ? 2
        : 1;

    if (magic != NSFB_MAGIC ||
        (version != NSFB_VERSION_PCM8 && version != NSFB_VERSION_PCM16) ||
        header_size < NSFB_HEADER_SIZE_V1 ||
        (version == NSFB_VERSION_PCM16 && header_size < NSFB_HEADER_SIZE_V2) ||
        (sample_format != NSFB_SAMPLE_FORMAT_PCM8 &&
         sample_format != NSFB_SAMPLE_FORMAT_PCM16) ||
        instrument_table_offset < header_size ||
        instrument_record_size < 40 ||
        region_record_size < 40 || instrument_count == 0 ||
        region_count == 0 || sample_data_bytes == 0 ||
        sample_data_offset > size ||
        sample_data_bytes > size - sample_data_offset ||
        sample_data_bytes > WTS_SAMPLE_SDRAM_LIMIT - WTS_SAMPLE_SDRAM_BASE) {
        logLn("[fio] WTS bank %s: invalid NSFB header", label ? label : "");
        midi_debug_phase(593, sample_data_bytes);
        return false;
    }
    midi_debug_phase(451, sample_data_bytes);
    midi_debug_sample(0, sample_data_bytes);

    _bridge.poke(WTS_COMMAND, WTS_CMD_ALL_NOTES_OFF);
    clear_wts_bank_state();

    std::vector<WtsBankInstrument> instruments;
    WtsRegionStore regions;
    instruments.reserve(instrument_count);
    if (!regions.reserve(region_count)) {
        midi_debug_phase(594, region_count);
        logLn("[fio] WTS bank %s: region metadata allocation failed "
              "regions=%u heapFree=%u heapLargest=%u",
              label ? label : "", (unsigned)region_count,
              (unsigned)ESP.getFreeHeap(),
              (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        return false;
    }
    midi_debug_phase(452, instrument_count);
    if (!reader.seek(instrument_table_offset))
        return false;
    for (uint16_t i = 0; i < instrument_count; i++) {
        WtsBankInstrument inst;
        uint8_t discard = 0;
        if (!reader.read_u8(inst.bank) ||
            !reader.read_u8(inst.program) ||
            !reader.read_u16(inst.first_region) ||
            !reader.read_u16(inst.region_count)) {
            return false;
        }
        for (int n = 0; n < 32; n++) {
            if (!reader.read_u8(discard)) return false;
        }
        for (uint16_t n = 38; n < instrument_record_size; n++) {
            if (!reader.read_u8(discard)) return false;
        }
        if ((uint32_t)inst.first_region + inst.region_count > region_count)
            return false;
        instruments.push_back(inst);
    }

    midi_debug_phase(453, region_count);
    if (!reader.seek(region_table_offset))
        return false;
    for (uint16_t i = 0; i < region_count; i++) {
        WtsBankRegion region;
        uint16_t tuning_raw = 0;
        uint8_t discard = 0;
        if (!reader.read_u8(region.key_lo) ||
            !reader.read_u8(region.key_hi) ||
            !reader.read_u8(region.vel_lo) ||
            !reader.read_u8(region.vel_hi) ||
            !reader.read_u8(region.root_key) ||
            !reader.read_u8(region.flags) ||
            !reader.read_u16(region.sample_rate) ||
            !reader.read_u16(tuning_raw) ||
            !reader.read_u32(region.sample_start) ||
            !reader.read_u32(region.sample_length) ||
            !reader.read_u32(region.loop_start) ||
            !reader.read_u32(region.loop_end)) {
            return false;
        }
        region.tuning_cents = (int16_t)tuning_raw;
        if (!reader.read_u16(region.attack_frames) ||
            !reader.read_u16(region.decay_frames) ||
            !reader.read_u16(region.sustain_level) ||
            !reader.read_u16(region.release_frames)) {
            return false;
        }
        for (uint16_t n = 34; n < region_record_size; n++) {
            if (!reader.read_u8(discard)) return false;
        }
        uint64_t sample_start_bytes =
            (uint64_t)region.sample_start * sample_frame_bytes;
        uint64_t sample_length_bytes =
            (uint64_t)region.sample_length * sample_frame_bytes;
        if (sample_start_bytes > sample_data_bytes ||
            sample_length_bytes > sample_data_bytes - sample_start_bytes) {
            return false;
        }
        if (!regions.push_back(region)) {
            midi_debug_phase(594, i);
            return false;
        }
    }

    bool keep_resident = reserve_wts_sample_cache(sample_data_bytes);
    midi_debug_phase(454, keep_resident ? 1 : 0);
    if (keep_resident) {
        logLn("[fio] WTS bank %s: caching %lu sample bytes resident",
              label ? label : "", (unsigned long)sample_data_bytes);
    } else {
        logLn("[fio] WTS bank %s: %lu sample bytes not resident; "
              "SDRAM/hardware playback required",
              label ? label : "", (unsigned long)sample_data_bytes);
        release_wts_sample_cache();
    }

    uint32_t off = 0;
    while (off < sample_data_bytes) {
        if ((off & 0xFFFFUL) == 0)
            midi_debug_phase(455, off);
        midi_debug_sample(off, sample_data_bytes);
        uint32_t remaining = sample_data_bytes - off;
        uint16_t chunk = remaining >= TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, sample_data_offset + off,
                                                _transfer_buf, chunk);
        if (got != (int)chunk)
            return false;
        if (keep_resident &&
            !cache_wts_sample_bytes(off, _transfer_buf, chunk)) {
            return false;
        }
        if (!write_wts_sample_chunk_to_sdram(off, _transfer_buf, chunk)) {
            return false;
        }
        off += chunk;
    }

    midi_debug_phase(456, sample_data_bytes);
    midi_debug_sample(sample_data_bytes, sample_data_bytes);
    _wts_instruments = std::move(instruments);
    _wts_regions = std::move(regions);
    _wts_sample_bytes = sample_data_bytes;
    _wts_bank_loaded = true;
    _wts_samples_resident = keep_resident;
    _wts_sample_frame_bytes = sample_frame_bytes;
    _wts_bank_hash = bank_hash;
    snprintf(_wts_bank_name, sizeof(_wts_bank_name), "%s", label ? label : "");
    logLn("[fio] WTS bank %s loaded: instruments=%u regions=%u samples=%u "
          "frameBytes=%u resident=%u @ SDRAM $%06X",
          label ? label : "",
          (unsigned)_wts_instruments.size(),
          (unsigned)_wts_regions.size(),
          (unsigned)_wts_sample_bytes,
          (unsigned)_wts_sample_frame_bytes,
          (unsigned)_wts_samples_resident,
          (unsigned)WTS_SAMPLE_SDRAM_BASE);
    return true;
}

bool FioDispatcher::load_wts_bank_from_sd_path(const char* sd_path) {
    midi_debug_phase(400);
    if (!sd_path || sd_path[0] == 0)
        return false;

    File f = SD.open(sd_path, FILE_READ);
    if (!f) {
        midi_debug_phase(490);
        logLn("[fio] WTS bank open failed: %s", sd_path);
        return false;
    }

    uint32_t size = (uint32_t)f.size();
    midi_debug_phase(401, size);
    uint32_t bank_hash = 2166136261UL;
    for (uint32_t hash_off = 0; hash_off < size; ) {
        uint32_t remaining = size - hash_off;
        uint16_t chunk = remaining >= TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
        int got = f.read(_transfer_buf, chunk);
        if (got != (int)chunk) {
            f.close();
            midi_debug_phase(492, hash_off);
            return false;
        }
        bank_hash = fnv1a_update(bank_hash, _transfer_buf, chunk);
        hash_off += chunk;
        yield();
    }
    if (!f.seek(0)) {
        f.close();
        midi_debug_phase(492, size);
        return false;
    }
    auto read_u8 = [&](uint8_t& value) -> bool {
        int b = f.read();
        if (b < 0)
            return false;
        value = (uint8_t)b;
        return true;
    };
    auto read_u16 = [&](uint16_t& value) -> bool {
        uint8_t b0 = 0;
        uint8_t b1 = 0;
        if (!read_u8(b0) || !read_u8(b1))
            return false;
        value = (uint16_t)b0 | ((uint16_t)b1 << 8);
        return true;
    };
    auto read_u32 = [&](uint32_t& value) -> bool {
        uint8_t b0 = 0;
        uint8_t b1 = 0;
        uint8_t b2 = 0;
        uint8_t b3 = 0;
        if (!read_u8(b0) || !read_u8(b1) || !read_u8(b2) || !read_u8(b3))
            return false;
        value = (uint32_t)b0 |
                ((uint32_t)b1 << 8) |
                ((uint32_t)b2 << 16) |
                ((uint32_t)b3 << 24);
        return true;
    };

    uint32_t magic = 0;
    uint16_t version = 0;
    uint16_t header_size = 0;
    uint16_t instrument_record_size = 0;
    uint16_t region_record_size = 0;
    uint16_t instrument_count = 0;
    uint16_t region_count = 0;
    uint16_t output_rate = 0;
    uint32_t instrument_table_offset = 0;
    uint32_t region_table_offset = 0;
    uint32_t sample_data_offset = 0;
    uint32_t sample_data_bytes = 0;
    uint16_t sample_format = NSFB_SAMPLE_FORMAT_PCM8;
    uint16_t reserved16 = 0;

    if (size < NSFB_HEADER_SIZE_V1 ||
        !read_u32(magic) ||
        !read_u16(version) ||
        !read_u16(header_size) ||
        !read_u16(instrument_record_size) ||
        !read_u16(region_record_size) ||
        !read_u16(instrument_count) ||
        !read_u16(region_count) ||
        !read_u16(output_rate) ||
        !read_u32(instrument_table_offset) ||
        !read_u32(region_table_offset) ||
        !read_u32(sample_data_offset) ||
        !read_u32(sample_data_bytes)) {
        f.close();
        midi_debug_phase(491, size);
        logLn("[fio] WTS bank %s: short or unreadable NSFB header (size=%lu)",
              sd_path, (unsigned long)size);
        return false;
    }

    if (version == NSFB_VERSION_PCM16) {
        if (header_size < NSFB_HEADER_SIZE_V2 ||
            !read_u16(sample_format) ||
            !read_u16(reserved16)) {
            f.close();
            midi_debug_phase(492, header_size);
            logLn("[fio] WTS bank %s: short NSFB v2 header", sd_path);
            return false;
        }
    } else {
        sample_format = NSFB_SAMPLE_FORMAT_PCM8;
    }

    uint8_t sample_frame_bytes = sample_format == NSFB_SAMPLE_FORMAT_PCM16
        ? 2
        : 1;

    if (magic != NSFB_MAGIC ||
        (version != NSFB_VERSION_PCM8 && version != NSFB_VERSION_PCM16) ||
        header_size < NSFB_HEADER_SIZE_V1 ||
        (version == NSFB_VERSION_PCM16 && header_size < NSFB_HEADER_SIZE_V2) ||
        (sample_format != NSFB_SAMPLE_FORMAT_PCM8 &&
         sample_format != NSFB_SAMPLE_FORMAT_PCM16) ||
        instrument_table_offset < header_size ||
        instrument_record_size < 40 ||
        region_record_size < 40 || instrument_count == 0 ||
        region_count == 0 || sample_data_bytes == 0 ||
        sample_data_offset > size ||
        sample_data_bytes > size - sample_data_offset ||
        sample_data_bytes > WTS_SAMPLE_SDRAM_LIMIT - WTS_SAMPLE_SDRAM_BASE) {
        f.close();
        midi_debug_phase(493, sample_data_bytes);
        logLn("[fio] WTS bank %s: invalid NSFB header magic=$%08lX version=%u "
              "header=%u instRec=%u regionRec=%u inst=%u regions=%u "
              "sampleOff=%lu sampleBytes=%lu size=%lu",
              sd_path, (unsigned long)magic, (unsigned)version,
              (unsigned)header_size, (unsigned)instrument_record_size,
              (unsigned)region_record_size, (unsigned)instrument_count,
              (unsigned)region_count, (unsigned long)sample_data_offset,
              (unsigned long)sample_data_bytes, (unsigned long)size);
        return false;
    }
    midi_debug_phase(402, sample_data_bytes);
    midi_debug_sample(0, sample_data_bytes);

    _bridge.poke(WTS_COMMAND, WTS_CMD_ALL_NOTES_OFF);
    clear_wts_bank_state();

    std::vector<WtsBankInstrument> instruments;
    WtsRegionStore regions;
    instruments.reserve(instrument_count);
    if (!regions.reserve(region_count)) {
        f.close();
        midi_debug_phase(499, region_count);
        logLn("[fio] WTS bank %s: region metadata allocation failed "
              "regions=%u heapFree=%u heapLargest=%u",
              sd_path, (unsigned)region_count,
              (unsigned)ESP.getFreeHeap(),
              (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
        return false;
    }
    midi_debug_phase(403, instrument_count);
    if (!f.seek(instrument_table_offset)) {
        f.close();
        midi_debug_phase(494, instrument_table_offset);
        logLn("[fio] WTS bank %s: instrument table seek failed at %lu",
              sd_path, (unsigned long)instrument_table_offset);
        return false;
    }
    for (uint16_t i = 0; i < instrument_count; i++) {
        WtsBankInstrument inst;
        uint8_t discard = 0;
        if (!read_u8(inst.bank) ||
            !read_u8(inst.program) ||
            !read_u16(inst.first_region) ||
            !read_u16(inst.region_count)) {
            f.close();
            midi_debug_phase(494, i);
            logLn("[fio] WTS bank %s: instrument %u read failed",
                  sd_path, (unsigned)i);
            return false;
        }
        for (int n = 0; n < 32; n++) {
            if (!read_u8(discard)) {
                f.close();
                midi_debug_phase(494, i);
                logLn("[fio] WTS bank %s: instrument %u name read failed",
                      sd_path, (unsigned)i);
                return false;
            }
        }
        for (uint16_t n = 38; n < instrument_record_size; n++) {
            if (!read_u8(discard)) {
                f.close();
                midi_debug_phase(494, i);
                logLn("[fio] WTS bank %s: instrument %u padding read failed",
                      sd_path, (unsigned)i);
                return false;
            }
        }
        if ((uint32_t)inst.first_region + inst.region_count > region_count) {
            f.close();
            midi_debug_phase(494, i);
            logLn("[fio] WTS bank %s: instrument %u region range invalid "
                  "(first=%u count=%u total=%u)",
                  sd_path, (unsigned)i, (unsigned)inst.first_region,
                  (unsigned)inst.region_count, (unsigned)region_count);
            return false;
        }
        instruments.push_back(inst);
    }

    midi_debug_phase(404, region_count);
    if (!f.seek(region_table_offset)) {
        f.close();
        midi_debug_phase(495, region_table_offset);
        logLn("[fio] WTS bank %s: region table seek failed at %lu",
              sd_path, (unsigned long)region_table_offset);
        return false;
    }
    for (uint16_t i = 0; i < region_count; i++) {
        WtsBankRegion region;
        uint16_t tuning_raw = 0;
        uint8_t discard = 0;
        if (!read_u8(region.key_lo) ||
            !read_u8(region.key_hi) ||
            !read_u8(region.vel_lo) ||
            !read_u8(region.vel_hi) ||
            !read_u8(region.root_key) ||
            !read_u8(region.flags) ||
            !read_u16(region.sample_rate) ||
            !read_u16(tuning_raw) ||
            !read_u32(region.sample_start) ||
            !read_u32(region.sample_length) ||
            !read_u32(region.loop_start) ||
            !read_u32(region.loop_end)) {
            f.close();
            midi_debug_phase(495, i);
            logLn("[fio] WTS bank %s: region %u read failed",
                  sd_path, (unsigned)i);
            return false;
        }
        region.tuning_cents = (int16_t)tuning_raw;
        if (!read_u16(region.attack_frames) ||
            !read_u16(region.decay_frames) ||
            !read_u16(region.sustain_level) ||
            !read_u16(region.release_frames)) {
            f.close();
            midi_debug_phase(495, i);
            logLn("[fio] WTS bank %s: region %u envelope read failed",
                  sd_path, (unsigned)i);
            return false;
        }
        for (uint16_t n = 34; n < region_record_size; n++) {
            if (!read_u8(discard)) {
                f.close();
                midi_debug_phase(495, i);
                logLn("[fio] WTS bank %s: region %u padding read failed",
                      sd_path, (unsigned)i);
                return false;
            }
        }
        uint64_t sample_start_bytes =
            (uint64_t)region.sample_start * sample_frame_bytes;
        uint64_t sample_length_bytes =
            (uint64_t)region.sample_length * sample_frame_bytes;
        if (sample_start_bytes > sample_data_bytes ||
            sample_length_bytes > sample_data_bytes - sample_start_bytes) {
            f.close();
            midi_debug_phase(495, i);
            logLn("[fio] WTS bank %s: region %u sample range invalid "
                  "(start=%lu len=%lu frameBytes=%u sampleBytes=%lu)",
                  sd_path, (unsigned)i, (unsigned long)region.sample_start,
                  (unsigned long)region.sample_length,
                  (unsigned)sample_frame_bytes,
                  (unsigned long)sample_data_bytes);
            return false;
        }
        if (!regions.push_back(region)) {
            f.close();
            midi_debug_phase(499, i);
            logLn("[fio] WTS bank %s: region metadata push failed at %u",
                  sd_path, (unsigned)i);
            return false;
        }
    }

    midi_debug_phase(405, sample_data_offset);
    if (!f.seek(sample_data_offset)) {
        f.close();
        midi_debug_phase(496, sample_data_offset);
        logLn("[fio] WTS bank %s: sample data seek failed at %lu",
              sd_path, (unsigned long)sample_data_offset);
        return false;
    }

    uint32_t load_start_ms = millis();
    bool keep_resident = reserve_wts_sample_cache(sample_data_bytes);
    midi_debug_phase(406, keep_resident ? 1 : 0);
    if (keep_resident) {
        logLn("[fio] WTS bank %s: caching %lu sample bytes resident",
              sd_path, (unsigned long)sample_data_bytes);
    } else {
        logLn("[fio] WTS bank %s: %lu sample bytes not resident "
              "(capacity=%lu); SDRAM/hardware playback required",
              sd_path, (unsigned long)sample_data_bytes,
              (unsigned long)_wts_sample_capacity);
        release_wts_sample_cache();
    }

    uint32_t off = 0;
    while (off < sample_data_bytes) {
        if ((off & 0xFFFFUL) == 0)
            midi_debug_phase(407, off);
        midi_debug_sample(off, sample_data_bytes);
        uint32_t remaining = sample_data_bytes - off;
        uint16_t chunk = remaining >= TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
        int got = f.read(_transfer_buf, chunk);
        if (got != (int)chunk) {
            f.close();
            midi_debug_phase(497, off);
            logLn("[fio] WTS bank %s: sample read failed at %lu got=%d want=%u",
                  sd_path, (unsigned long)off, got, (unsigned)chunk);
            return false;
        }
        if (keep_resident &&
            !cache_wts_sample_bytes(off, _transfer_buf, chunk)) {
            f.close();
            midi_debug_phase(497, off);
            logLn("[fio] WTS bank %s: resident cache write failed at %lu "
                  "chunk=%u",
                  sd_path, (unsigned long)off, (unsigned)chunk);
            return false;
        }
        if (!write_wts_sample_chunk_to_sdram(off, _transfer_buf, chunk)) {
            f.close();
            logLn("[fio] WTS bank %s: sample SDRAM write failed at %lu "
                  "addr=$%06lX chunk=%u",
                  sd_path, (unsigned long)off,
                  (unsigned long)(WTS_SAMPLE_SDRAM_BASE + off),
                  (unsigned)chunk);
            return false;
        }
        off += chunk;
        yield();
    }

    f.close();
    midi_debug_phase(408, sample_data_bytes);
    midi_debug_sample(sample_data_bytes, sample_data_bytes);
    _wts_instruments = std::move(instruments);
    _wts_regions = std::move(regions);
    _wts_sample_bytes = sample_data_bytes;
    _wts_bank_loaded = true;
    _wts_samples_resident = keep_resident;
    _wts_sample_frame_bytes = sample_frame_bytes;
    _wts_bank_hash = bank_hash;
    snprintf(_wts_bank_name, sizeof(_wts_bank_name), "%s", sd_path);
    logLn("[fio] WTS bank %s loaded from SD: instruments=%u regions=%u "
          "samples=%u frameBytes=%u resident=%u @ SDRAM $%06X in %lu ms",
          sd_path,
          (unsigned)_wts_instruments.size(),
          (unsigned)_wts_regions.size(),
          (unsigned)_wts_sample_bytes,
          (unsigned)_wts_sample_frame_bytes,
          (unsigned)_wts_samples_resident,
          (unsigned)WTS_SAMPLE_SDRAM_BASE,
          (unsigned long)(millis() - load_start_ms));
    return true;
}

bool FioDispatcher::load_wts_bank_by_name(const char* requested_name) {
    char path[96];
    char scratch[64];
    int slot = 0;
    uint16_t parent = 0;

    if (!requested_name || requested_name[0] == 0)
        return false;

    if (strchr(requested_name, ':') == nullptr) {
        char sd_path[128];
        bool has_ext = has_extension(requested_name);
        if (requested_name[0] == '/') {
            snprintf(sd_path, sizeof(sd_path), "%s%s", requested_name,
                     has_ext ? "" : ".nsfb");
        } else if (strchr(requested_name, '/') != nullptr) {
            snprintf(sd_path, sizeof(sd_path), "/%s%s", requested_name,
                     has_ext ? "" : ".nsfb");
        } else {
            snprintf(sd_path, sizeof(sd_path), "/soundfonts/%s%s",
                     requested_name, has_ext ? "" : ".nsfb");
        }
        logLn("[fio] WTS bank raw SD candidate: %s", sd_path);
        if (load_wts_bank_from_sd_path(sd_path))
            return true;
    }

    auto try_path = [&](const char* candidate) -> bool {
        char candidate_path[96];
        strncpy(candidate_path, candidate, sizeof(candidate_path) - 1);
        candidate_path[sizeof(candidate_path) - 1] = 0;
        if (!_dm.resolve_path(candidate_path, slot, parent, scratch))
            return false;
        auto* img = _dm.image(slot);
        if (!img)
            return false;
        ndi::DirEntry e;
        int idx = img->find_entry(scratch, parent);
        if (idx < 0 || !img->get_entry(idx, e) || e.is_directory())
            return false;
        return load_wts_bank_from_entry(img, idx, e.size_bytes, scratch);
    };

    if (try_path(requested_name))
        return true;

    bool has_ext = has_extension(requested_name);
    if (has_ext) {
        snprintf(path, sizeof(path), "soundfonts/%s", requested_name);
        if (try_path(path))
            return true;
    } else {
        snprintf(path, sizeof(path), "%s.nsfb", requested_name);
        if (try_path(path))
            return true;
        snprintf(path, sizeof(path), "soundfonts/%s.nsfb", requested_name);
        if (try_path(path))
            return true;
    }

    return false;
}

bool FioDispatcher::load_first_wts_bank_from_sd_dir(const char* dir_path) {
    if (!dir_path || dir_path[0] == 0)
        return false;

    File dir = SD.open(dir_path);
    if (!dir || !dir.isDirectory()) {
        if (dir)
            dir.close();
        return false;
    }

    bool loaded = false;
    while (true) {
        File entry = dir.openNextFile();
        if (!entry)
            break;

        const char* entry_name = entry.name();
        const char* base_name = strrchr(entry_name, '/');
        base_name = base_name ? base_name + 1 : entry_name;
        if (!entry.isDirectory() && has_extension_ci(base_name, ".nsfb")) {
            char path[128];
            snprintf(path, sizeof(path), "%s/%s", dir_path, base_name);
            entry.close();
            logLn("[fio] WTS auto-loading first soundfont: %s", path);
            loaded = load_wts_bank_from_sd_path(path);
            break;
        }
        entry.close();
        yield();
    }

    dir.close();
    return loaded;
}

bool FioDispatcher::load_first_wts_bank_from_slot(int slot) {
    if (!_dm.is_mounted(slot))
        return false;

    auto* img = _dm.image(slot);
    if (!img)
        return false;

    int dir_idx = img->find_entry("soundfonts", ndi::ROOT_PARENT);
    if (dir_idx < 0)
        return false;

    ndi::DirEntry dir_entry;
    if (!img->get_entry(dir_idx, dir_entry) || !dir_entry.is_directory())
        return false;

    for (int i = 0; i < img->directory_entry_count(); i++) {
        ndi::DirEntry e;
        if (!img->get_entry(i, e) || !e.is_active() || e.is_directory() ||
            e.parent_index != (uint16_t)dir_idx ||
            !has_extension_ci(e.filename, ".nsfb")) {
            continue;
        }

        logLn("[fio] WTS auto-loading first soundfont from %s:soundfonts/%s",
              DeviceManager::prefix_for_slot(slot), e.filename);
        return load_wts_bank_from_entry(img, i, e.size_bytes, e.filename);
    }

    return false;
}

bool FioDispatcher::ensure_wts_bank_loaded(int preferred_slot) {
    if (_wts_bank_loaded) {
        logLn("[fio] WTS bank already loaded: %s instruments=%u regions=%u samples=%u resident=%u",
              _wts_bank_name[0] ? _wts_bank_name : "(unnamed)",
              (unsigned)_wts_instruments.size(),
              (unsigned)_wts_regions.size(),
              (unsigned)_wts_sample_bytes,
              (unsigned)_wts_samples_resident);
        return true;
    }

    logLn("[fio] WTS bank not loaded; auto-loading first available soundfont");

    if (load_first_wts_bank_from_sd_dir("/soundfonts"))
        return true;

    if (preferred_slot >= 0 &&
        preferred_slot < DeviceManager::NUM_SLOTS &&
        load_first_wts_bank_from_slot(preferred_slot)) {
        return true;
    }

    for (int slot = 0; slot < DeviceManager::NUM_SLOTS; slot++) {
        if (!_dm.is_mounted(slot) || slot == preferred_slot)
            continue;
        if (load_first_wts_bank_from_slot(slot))
            return true;
    }

    logLn("[fio] WTS auto-load failed: Missing soundfont");
    return false;
}

bool FioDispatcher::find_wts_region(uint8_t bank, uint8_t program,
                                    uint8_t note, uint8_t velocity,
                                    uint32_t& sample_start,
                                    uint32_t& sample_end,
                                    uint32_t& loop_start,
                                    uint32_t& loop_end,
                                    uint32_t& phase_step,
                                    bool& loop,
                                    uint16_t& attack_step,
                                    uint16_t& decay_step,
                                    uint16_t& sustain_level,
                                    uint16_t& release_step) {
    if (!_wts_bank_loaded)
        return false;

    int inst_index = -1;
    for (size_t i = 0; i < _wts_instruments.size(); i++) {
        const auto& inst = _wts_instruments[i];
        if (inst.bank == bank && inst.program == program) {
            inst_index = (int)i;
            break;
        }
    }
    if (inst_index < 0 && bank != 0) {
        for (size_t i = 0; i < _wts_instruments.size(); i++) {
            const auto& inst = _wts_instruments[i];
            if (inst.bank == 0 && inst.program == program) {
                inst_index = (int)i;
                break;
            }
        }
    }
    if (inst_index < 0)
        return false;

    const auto& inst = _wts_instruments[inst_index];
    for (uint16_t i = 0; i < inst.region_count; i++) {
        const auto& r = _wts_regions[inst.first_region + i];
        if (note < r.key_lo || note > r.key_hi ||
            velocity < r.vel_lo || velocity > r.vel_hi) {
            continue;
        }

        sample_start = WTS_SAMPLE_SDRAM_BASE + r.sample_start;
        sample_end = sample_start + r.sample_length;
        loop_start = WTS_SAMPLE_SDRAM_BASE + r.loop_start;
        loop_end = WTS_SAMPLE_SDRAM_BASE + r.loop_end;
        loop = (r.flags & 0x01) != 0 && loop_end > loop_start;

        double semitones = (double)((int)note - (int)r.root_key) +
                           ((double)r.tuning_cents / 100.0);
        double ratio = pow(2.0, semitones / 12.0);
        double step = ((double)r.sample_rate * ratio * 256.0) / 48000.0;
        if (step < 1.0)
            step = 1.0;
        if (step > 4294967295.0)
            step = 4294967295.0;
        phase_step = (uint32_t)(step + 0.5);
        sustain_level = r.sustain_level;
        attack_step = wts_attack_step_from_frames(r.attack_frames);
        decay_step = wts_decay_step_from_frames(r.decay_frames, sustain_level);
        release_step = wts_release_step_from_frames(r.release_frames);
        return true;
    }

    return false;
}

bool FioDispatcher::hardware_wts_available() {
    uint8_t status = 0;
    uint8_t count = 0;
    uint8_t signature = 0;

    if (!_bridge.peek(WTS_SOUNDFONT_STATUS, status) || status != 1)
        return false;
    if (!_bridge.peek(WTS_INSTRUMENT_COUNT, count) || count == 0)
        return false;
    if (!_bridge.peek(WTS_SIGNATURE, signature) ||
        signature != WTS_SIGNATURE_VALUE) {
        return false;
    }

    return true;
}

bool FioDispatcher::configure_wts_for_midi() {
    bool ok = true;
    ok = _bridge.poke(WTS_COMMAND, WTS_CMD_ALL_NOTES_OFF) && ok;
    ok = _bridge.poke(WTS_MASTER_VOLUME, 255) && ok;
    memset(_wts_voice_env_valid, 0, sizeof(_wts_voice_env_valid));

    for (uint8_t v = 0; v < WtsVoiceCount; v++) {
        uint16_t base = WTS_VOICE_BASE + (uint16_t)v * WTS_VOICE_STRIDE;
        uint8_t pan = WtsVoiceCount <= 1
            ? 128
            : (uint8_t)(32 + ((uint16_t)v * 191) / (WtsVoiceCount - 1));
        ok = _bridge.poke(base + 0, 0) && ok;
        ok = _bridge.poke(base + 3, 255) && ok;
        ok = _bridge.poke(base + 4, pan) && ok;
        ok = _bridge.poke(base + 5, 0) && ok;
        ok = _bridge.poke(base + 6, 0x80) && ok;
        ok = configure_wts_voice_envelope(v, WTS_DEFAULT_ATTACK_STEP,
                                          WTS_DEFAULT_DECAY_STEP,
                                          WTS_DEFAULT_SUSTAIN_LEVEL,
                                          WTS_DEFAULT_RELEASE_STEP) && ok;
    }

    return ok;
}

bool FioDispatcher::configure_wts_voice_envelope(uint8_t voice,
                                                 uint16_t attack_step,
                                                 uint16_t decay_step,
                                                 uint16_t sustain_level,
                                                 uint16_t release_step) {
    if (voice >= WtsVoiceCount)
        return false;

    if (_wts_voice_env_valid[voice] &&
        _wts_voice_attack_step[voice] == attack_step &&
        _wts_voice_decay_step[voice] == decay_step &&
        _wts_voice_sustain_level[voice] == sustain_level &&
        _wts_voice_release_step[voice] == release_step) {
        return true;
    }

    if (_bridge.supportsPokeMulti()) {
        uint8_t env_bytes[8] = {
            (uint8_t)(attack_step & 0xFF),
            (uint8_t)(attack_step >> 8),
            (uint8_t)(decay_step & 0xFF),
            (uint8_t)(decay_step >> 8),
            (uint8_t)(sustain_level & 0xFF),
            (uint8_t)(sustain_level >> 8),
            (uint8_t)(release_step & 0xFF),
            (uint8_t)(release_step >> 8),
        };
        uint16_t addrs[17];
        uint8_t values[17];
        uint8_t count = 0;
        addrs[count] = WTS_SAMPLE_VOICE;
        values[count++] = voice;
        for (uint8_t i = 0; i < sizeof(env_bytes); i++) {
            addrs[count] = WTS_ENV_PARAM_INDEX;
            values[count++] = i;
            addrs[count] = WTS_ENV_PARAM_DATA;
            values[count++] = env_bytes[i];
        }

        bool ok = _bridge.pokeMulti(addrs, values, count);
        if (ok) {
            _wts_voice_env_valid[voice] = true;
            _wts_voice_attack_step[voice] = attack_step;
            _wts_voice_decay_step[voice] = decay_step;
            _wts_voice_sustain_level[voice] = sustain_level;
            _wts_voice_release_step[voice] = release_step;
        }
        return ok;
    }

    auto poke_env_byte = [&](uint8_t index, uint8_t value) -> bool {
        return _bridge.poke(WTS_ENV_PARAM_INDEX, index) &&
               _bridge.poke(WTS_ENV_PARAM_DATA, value);
    };
    auto poke_env_u16 = [&](uint8_t index, uint16_t value) -> bool {
        return poke_env_byte(index, (uint8_t)(value & 0xFF)) &&
               poke_env_byte(index + 1, (uint8_t)(value >> 8));
    };

    bool ok = _bridge.poke(WTS_SAMPLE_VOICE, voice) &&
              poke_env_u16(0, attack_step) &&
              poke_env_u16(2, decay_step) &&
              poke_env_u16(4, sustain_level) &&
              poke_env_u16(6, release_step);
    if (ok) {
        _wts_voice_env_valid[voice] = true;
        _wts_voice_attack_step[voice] = attack_step;
        _wts_voice_decay_step[voice] = decay_step;
        _wts_voice_sustain_level[voice] = sustain_level;
        _wts_voice_release_step[voice] = release_step;
    }
    return ok;
}

bool FioDispatcher::configure_wts_sample_voice(uint8_t voice,
                                               uint32_t sample_start,
                                               uint32_t sample_end,
                                               uint32_t loop_start,
                                               uint32_t loop_end,
                                               uint32_t phase_step,
                                               bool loop,
                                               uint16_t attack_step,
                                               uint16_t decay_step,
                                               uint16_t sustain_level,
                                               uint16_t release_step) {
    if (voice >= WtsVoiceCount ||
        sample_start >= WTS_SAMPLE_SDRAM_LIMIT ||
        sample_end > WTS_SAMPLE_SDRAM_LIMIT ||
        sample_end <= sample_start) {
        return false;
    }

    uint8_t regs[22] = {};
    regs[0] = voice;
    auto put_addr25 = [&](uint8_t off, uint32_t value) {
        regs[off + 0] = (uint8_t)(value & 0xFF);
        regs[off + 1] = (uint8_t)((value >> 8) & 0xFF);
        regs[off + 2] = (uint8_t)((value >> 16) & 0xFF);
        regs[off + 3] = (uint8_t)((value >> 24) & 0x01);
    };
    auto put_u32 = [&](uint8_t off, uint32_t value) {
        regs[off + 0] = (uint8_t)(value & 0xFF);
        regs[off + 1] = (uint8_t)((value >> 8) & 0xFF);
        regs[off + 2] = (uint8_t)((value >> 16) & 0xFF);
        regs[off + 3] = (uint8_t)((value >> 24) & 0xFF);
    };
    put_addr25(1, sample_start);
    put_addr25(5, sample_end);
    put_addr25(9, loop_start);
    put_addr25(13, loop_end);
    put_u32(17, phase_step);
    regs[21] = loop ? 0x03 : 0x01;

    return _bridge.pokeBlock(WTS_SAMPLE_VOICE, regs, sizeof(regs)) &&
           configure_wts_voice_envelope(voice, attack_step, decay_step,
                                        sustain_level, release_step);
}

void FioDispatcher::reset_midi_timing_metrics() {
    _midi_pump_calls = 0;
    _midi_last_pump_enter_us = 0;
    _midi_last_pump_gap_us = 0;
    _midi_max_pump_gap_us = 0;
    _midi_last_pump_duration_us = 0;
    _midi_max_pump_duration_us = 0;
}

void FioDispatcher::finish_midi_hardware_pump(uint32_t pump_start_us) {
    uint32_t duration_us = micros() - pump_start_us;
    _midi_last_pump_duration_us = duration_us;
    if (duration_us > _midi_max_pump_duration_us)
        _midi_max_pump_duration_us = duration_us;
}

void FioDispatcher::stop_midi_playback(bool silence) {
    if (silence && _midi_use_hardware_wts)
        _bridge.poke(WTS_COMMAND, WTS_CMD_ALL_NOTES_OFF);
    else if (_midi_use_hardware_wts)
        _bridge.poke(WTS_COMMAND, WTS_CMD_EVENT_STOP);

    _midi_playing = false;
    _midi_slot = -1;
    _midi_file_index = -1;
    _midi_file_size = 0;
    _music_event_offset = 0;
    _music_event_bytes = 0;
    _music_event_read = 0;
    _music_event_count = 0;
    _music_total_frames = 0;
    _midi_event_index = 0;
    _midi_tick_accum = 0.0;
    _midi_use_hardware_wts = false;
    _midi_wts_all_events_queued = false;
}

bool FioDispatcher::read_wts_event_fifo_status(uint16_t& free_records,
                                               bool& empty,
                                               bool& parser_error) {
    uint8_t status = 0;
    uint8_t free_l = 0;
    uint8_t free_h = 0;
    if (!_bridge.peek(WTS_EVENT_STATUS, status) ||
        !_bridge.peek(WTS_EVENT_FREE_L, free_l) ||
        !_bridge.peek(WTS_EVENT_FREE_H, free_h)) {
        return false;
    }

    parser_error = (status & WTS_EVENT_STATUS_ERROR) != 0;
    empty = (status & WTS_EVENT_STATUS_EMPTY) != 0;
    free_records = (uint16_t)free_l | ((uint16_t)(free_h & 0x07) << 8);
    return true;
}

bool FioDispatcher::read_music_stream_header(ndi::NdiImage* img, int idx,
                                             uint32_t size) {
    if (!img || idx < 0 || size < NMS_HEADER_SIZE) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "music stream too small");
        return false;
    }

    NdiFileReader reader(img, idx, size, _transfer_buf, 256);
    uint32_t magic = 0;
    uint16_t version = 0;
    uint16_t header_size = 0;
    uint16_t record_size = 0;
    uint16_t sample_rate = 0;
    uint32_t event_offset = 0;
    uint32_t event_bytes = 0;
    uint32_t event_count = 0;
    uint32_t total_frames = 0;
    uint32_t reserved = 0;

    if (!reader.read_u32(magic) ||
        !reader.read_u16(version) ||
        !reader.read_u16(header_size) ||
        !reader.read_u16(record_size) ||
        !reader.read_u16(sample_rate) ||
        !reader.read_u32(event_offset) ||
        !reader.read_u32(event_bytes) ||
        !reader.read_u32(event_count) ||
        !reader.read_u32(total_frames) ||
        !reader.read_u32(reserved)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "music stream header read failed");
        return false;
    }

    if (magic != NMS_MAGIC || version != NMS_VERSION ||
        header_size < NMS_HEADER_SIZE || record_size != NMS_EVENT_RECORD_SIZE ||
        sample_rate != WTS_AUDIO_SAMPLE_RATE || event_offset < header_size ||
        event_offset > size || event_bytes > size - event_offset ||
        event_bytes % NMS_EVENT_RECORD_SIZE != 0 ||
        event_count != event_bytes / NMS_EVENT_RECORD_SIZE) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "invalid Nova music stream");
        return false;
    }

    _music_event_offset = event_offset;
    _music_event_bytes = event_bytes;
    _music_event_read = 0;
    _music_event_count = event_count;
    _music_total_frames = total_frames;
    _midi_event_index = 0;
    _midi_tick_accum = 0.0;
    _midi_wts_all_events_queued = event_bytes == 0;
    return true;
}

bool FioDispatcher::build_music_wts_event(const uint8_t* song_event,
                                          uint8_t* out,
                                          uint16_t& out_bytes) {
    out_bytes = 0;
    uint32_t frame = read_le32(song_event);
    uint8_t kind = song_event[4];
    uint8_t voice = song_event[5];
    uint8_t channel = song_event[6];
    uint8_t note = song_event[7];
    uint8_t velocity = song_event[8];
    uint8_t program = song_event[9];
    constexpr uint16_t out_capacity =
        NMS_WTS_NOTE_ON_RECORDS * MIDI_WTS_EVENT_BYTES;

    if (voice >= WtsVoiceCount) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "music stream voice out of range");
        return false;
    }

    uint8_t voice_base = (uint8_t)(voice * WTS_VOICE_STRIDE);
    if (kind == NMS_EVENT_NOTE_OFF) {
        return append_wts_event_record(out, out_capacity, out_bytes, frame,
                                       voice_base, 0);
    }

    if (kind != NMS_EVENT_NOTE_ON) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "music stream event type unsupported");
        return false;
    }

    uint8_t bank = channel == 9 ? 128 : 0;
    uint8_t lookup_program = channel == 9 ? 0 : program;
    uint32_t sample_start = 0;
    uint32_t sample_end = 0;
    uint32_t loop_start = 0;
    uint32_t loop_end = 0;
    uint32_t phase_step = 0;
    bool loop = false;
    uint16_t attack_step = 0;
    uint16_t decay_step = 0;
    uint16_t sustain_level = 0;
    uint16_t release_step = 0;

    if (!find_wts_region(bank, lookup_program, note, velocity,
                         sample_start, sample_end, loop_start, loop_end,
                         phase_step, loop, attack_step, decay_step,
                         sustain_level, release_step)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "soundfont missing instrument");
        return false;
    }

    bool ok = true;
    ok = append_wts_event_record(out, out_capacity, out_bytes, frame, 0x48,
                                 voice) && ok;
    ok = append_wts_addr25(out, out_capacity, out_bytes, frame, 0x49,
                           sample_start) && ok;
    ok = append_wts_addr25(out, out_capacity, out_bytes, frame, 0x4D,
                           sample_end) && ok;
    ok = append_wts_addr25(out, out_capacity, out_bytes, frame, 0x51,
                           loop_start) && ok;
    ok = append_wts_addr25(out, out_capacity, out_bytes, frame, 0x55,
                           loop_end) && ok;
    ok = append_wts_u32(out, out_capacity, out_bytes, frame, 0x59,
                        phase_step) && ok;
    ok = append_wts_event_record(out, out_capacity, out_bytes, frame, 0x5D,
                                 (uint8_t)(loop ? 0x03 : 0x01)) && ok;

    ok = append_wts_env_u16(out, out_capacity, out_bytes, frame, 0,
                            attack_step) && ok;
    ok = append_wts_env_u16(out, out_capacity, out_bytes, frame, 2,
                            decay_step) && ok;
    ok = append_wts_env_u16(out, out_capacity, out_bytes, frame, 4,
                            sustain_level) && ok;
    ok = append_wts_env_u16(out, out_capacity, out_bytes, frame, 6,
                            release_step) && ok;

    ok = append_wts_event_record(out, out_capacity, out_bytes, frame,
                                 (uint8_t)(voice_base + 1), velocity) && ok;
    ok = append_wts_event_record(out, out_capacity, out_bytes, frame,
                                 (uint8_t)(voice_base + 2),
                                 hardware_instrument_for(channel,
                                                         lookup_program)) &&
         ok;
    ok = append_wts_event_record(out, out_capacity, out_bytes, frame,
                                 voice_base, note) && ok;
    if (!ok) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event expansion overflow");
        return false;
    }
    return true;
}

bool FioDispatcher::queue_music_wts_events() {
    if (_midi_wts_all_events_queued)
        return true;

    uint16_t free_records = 0;
    bool fifo_empty = false;
    bool parser_error = false;
    if (!read_wts_event_fifo_status(free_records, fifo_empty, parser_error)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event FIFO status failed");
        return false;
    }
    if (parser_error) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event FIFO parser error");
        return false;
    }
    if (free_records == 0)
        return true;

    if (_music_event_read >= _music_event_bytes) {
        _midi_wts_all_events_queued = true;
        return true;
    }

    auto* img = _dm.image(_midi_slot);
    if (!img) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "music image unavailable");
        return false;
    }

    uint16_t out_bytes = 0;
    uint32_t consumed = 0;
    uint16_t records_available = free_records;
    uint8_t expanded[NMS_WTS_NOTE_ON_RECORDS * MIDI_WTS_EVENT_BYTES];

    while (_music_event_read + consumed < _music_event_bytes) {
        uint32_t remaining = _music_event_bytes - _music_event_read - consumed;
        uint16_t cache_bytes = sizeof(_midi_read_cache);
        if (cache_bytes > remaining)
            cache_bytes = (uint16_t)remaining;
        cache_bytes = (uint16_t)((cache_bytes / NMS_EVENT_RECORD_SIZE) *
                                 NMS_EVENT_RECORD_SIZE);
        if (cache_bytes == 0)
            break;

        int got = img->read_file_chunk_by_index(
            _midi_file_index, _music_event_offset + _music_event_read + consumed,
            _midi_read_cache, cache_bytes);
        if (got != (int)cache_bytes) {
            snprintf(_midi_last_error, sizeof(_midi_last_error),
                     "music event read failed");
            return false;
        }

        bool stopped_for_room = false;
        for (uint16_t cache_off = 0; cache_off < cache_bytes;
             cache_off += NMS_EVENT_RECORD_SIZE) {
            uint16_t event_bytes = 0;
            if (!build_music_wts_event(_midi_read_cache + cache_off, expanded,
                                       event_bytes)) {
                return false;
            }

            uint16_t event_records = event_bytes / MIDI_WTS_EVENT_BYTES;
            if (event_records > records_available ||
                out_bytes + event_bytes > TRANSFER_BUF_BYTES) {
                stopped_for_room = true;
                break;
            }

            memcpy(_transfer_buf + out_bytes, expanded, event_bytes);
            out_bytes += event_bytes;
            consumed += NMS_EVENT_RECORD_SIZE;
            records_available -= event_records;
            if (records_available == 0)
                break;
        }

        if (stopped_for_room || records_available == 0 ||
            out_bytes >= TRANSFER_BUF_BYTES) {
            break;
        }
        yield();
    }

    if (out_bytes == 0)
        return true;

    if (!_bridge.writeWtsEvents(_transfer_buf, out_bytes)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event write failed");
        return false;
    }

    _music_event_read += consumed;
    _midi_event_index = _music_event_read / NMS_EVENT_RECORD_SIZE;
    if (_music_event_read >= _music_event_bytes)
        _midi_wts_all_events_queued = true;
    return true;
}

bool FioDispatcher::start_music_wts_event_stream() {
    if (!_bridge.supportsWtsEventStream()) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event stream unsupported");
        return false;
    }
    if (!_bridge.poke(WTS_COMMAND, WTS_CMD_EVENT_RESET)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event reset failed");
        return false;
    }

    _midi_wts_all_events_queued = _music_event_bytes == 0;
    for (uint8_t i = 0; i < 8 && !_midi_wts_all_events_queued; i++) {
        if (!queue_music_wts_events())
            return false;
        yield();
    }

    if (!_bridge.poke(WTS_COMMAND, WTS_CMD_EVENT_START)) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "WTS event start failed");
        return false;
    }
    return true;
}

void FioDispatcher::tick_midi_hardware_wts() {
    if (!_midi_playing || !_midi_use_hardware_wts)
        return;

    uint32_t pump_start_us = micros();
    _midi_pump_calls++;
    if (_midi_last_pump_enter_us != 0) {
        _midi_last_pump_gap_us = pump_start_us - _midi_last_pump_enter_us;
        if (_midi_last_pump_gap_us > _midi_max_pump_gap_us)
            _midi_max_pump_gap_us = _midi_last_pump_gap_us;
    }
    _midi_last_pump_enter_us = pump_start_us;

    if (!queue_music_wts_events()) {
        logLn("[fio] MIDPLAY WTS event stream failed: %s",
              _midi_last_error);
        _midi_last_stop_reason = 10;
        stop_midi_playback(true);
        finish_midi_hardware_pump(pump_start_us);
        return;
    }

    if (_midi_wts_all_events_queued) {
        uint16_t free_records = 0;
        bool fifo_empty = false;
        bool parser_error = false;
        uint8_t active_mask = 0;
        if (read_wts_event_fifo_status(free_records, fifo_empty,
                                       parser_error) &&
            !parser_error && fifo_empty &&
            _bridge.peek(WTS_ACTIVE_MASK, active_mask) && active_mask == 0) {
            logLn("[fio] MIDPLAY WTS event stream complete");
            _midi_last_stop_reason = 3;
            stop_midi_playback(false);
        }
    }
    finish_midi_hardware_pump(pump_start_us);
}

void FioDispatcher::tick_midi_playback() {
    if (!_midi_playing)
        return;

    tick_midi_hardware_wts();
}

void FioDispatcher::handle_midplay() {
    midi_debug_phase(100);
    char name[64];
    copy_filename(name);
    logLn("[fio] MIDPLAY request: %s", name);

    char scratch[64];
    int slot;
    uint16_t parent;
    logLn("[fio] MIDPLAY resolving: %s", name);
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        logLn("[fio] MIDPLAY resolve failed: %s\n", name);
        respond_err(ERR_NOT_FOUND);
        return;
    }
    midi_debug_phase(101, (uint32_t)slot);
    logLn("[fio] MIDPLAY resolved dev=%s parent=%u name=%s",
          DeviceManager::prefix_for_slot(slot), (unsigned)parent, scratch);

    auto* img = _dm.image(slot);
    if (!img) {
        respond_err(ERR_NO_MOUNT);
        return;
    }

    ndi::DirEntry e;
    logLn("[fio] MIDPLAY locating entry: %s", scratch);
    int idx = find_music_entry(img, scratch, sizeof(scratch), parent, e);
    if (idx < 0) {
        logLn("[fio] MIDPLAY: '%s' not found in dev=%s\n",
              scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }
    logLn("[fio] MIDPLAY entry idx=%d size=%u", idx, (unsigned)e.size_bytes);
    midi_debug_phase(102, (uint32_t)e.size_bytes);

    stop_midi_playback(true);
    stop_sid_playback(true);
    _midi_note_ons = 0;
    _midi_sample_note_ons = 0;
    _midi_sample_misses = 0;
    _midi_last_stop_reason = 0;
    _midi_last_error[0] = 0;
    reset_midi_timing_metrics();

    if (!_wts_bank_loaded) {
        midi_debug_phase(103);
        if (!ensure_wts_bank_loaded(slot)) {
            logLn("[fio] MIDPLAY Missing soundfont");
            snprintf(_midi_last_error, sizeof(_midi_last_error),
                     "Missing soundfont");
            respond_err(ERR_IO);
            return;
        }
    }
#ifndef NOVAHOST_ENABLE_HARDWARE_WTS
#define NOVAHOST_ENABLE_HARDWARE_WTS 1
#endif
    bool hardware_wts_ready = NOVAHOST_ENABLE_HARDWARE_WTS &&
                              hardware_wts_available();
    bool wts_event_stream_ready = _bridge.supportsWtsEventStream();
    bool use_hardware_wts =
        hardware_wts_ready &&
        _wts_bank_loaded &&
        wts_event_stream_ready &&
        _wts_sample_frame_bytes == 1;
    if (use_hardware_wts) {
        logLn("[fio] MIDPLAY using hardware WTS timestamped event stream");
    } else if (NOVAHOST_ENABLE_HARDWARE_WTS && !hardware_wts_ready) {
        logLn("[fio] MIDPLAY hardware WTS unavailable in this bitstream");
    } else if (!wts_event_stream_ready) {
        logLn("[fio] MIDPLAY WTS event stream unsupported in this bitstream");
    } else if (_wts_sample_frame_bytes != 1) {
        logLn("[fio] MIDPLAY WTS sample format unsupported by FPGA: frameBytes=%u",
              (unsigned)_wts_sample_frame_bytes);
    }
    if (!use_hardware_wts) {
        snprintf(_midi_last_error, sizeof(_midi_last_error),
                 "hardware WTS event stream unavailable");
        respond_err(ERR_IO);
        return;
    }

    logLn("[fio] MIDPLAY reading Nova music stream header");
    midi_debug_phase(104);
    if (!read_music_stream_header(img, idx, e.size_bytes)) {
        logLn("[fio] MIDPLAY %s: %s\n", scratch, _midi_last_error);
        respond_err(ERR_IO);
        return;
    }

    _midi_slot = slot;
    _midi_file_index = idx;
    _midi_file_size = e.size_bytes;

    logLn("[fio] MIDPLAY scheduling playback");
    midi_debug_phase(108);
    _midi_event_index = 0;
    _midi_tick_accum = 0.0;
    _midi_use_hardware_wts = use_hardware_wts;

    if (!configure_wts_for_midi()) {
        logLn("[fio] MIDPLAY hardware-WTS setup failed");
        _midi_last_stop_reason = 5;
        stop_midi_playback(true);
        respond_err(ERR_IO);
        return;
    }
    _midi_playing = true;
    if (!start_music_wts_event_stream()) {
        logLn("[fio] MIDPLAY WTS event stream setup failed: %s",
              _midi_last_error);
        _midi_last_stop_reason = 10;
        stop_midi_playback(true);
        respond_err(ERR_IO);
        return;
    }

    write_size(e.size_bytes);
    midi_debug_phase(120);
    logLn("[fio] MIDPLAY %s WTS stream events=%lu frames=%lu OK\n",
          scratch, (unsigned long)_music_event_count,
          (unsigned long)_music_total_frames);
    respond_ok();
}

void FioDispatcher::handle_midstop() {
    stop_midi_playback(true);
    logLn("[fio] MIDSTOP OK\n");
    respond_ok();
}

void FioDispatcher::handle_sfload() {
    char name[64];
    copy_filename(name);

    if (load_wts_bank_by_name(name)) {
        logLn("[fio] SFLOAD %s OK\n", name);
        respond_ok();
        return;
    }

    logLn("[fio] SFLOAD %s failed\n", name);
    respond_err(ERR_NOT_FOUND);
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
#if NOVAHOST_XLOAD_VERIFY
        uint16_t chunk = remaining >= 256 ? 256 : (uint16_t)remaining;
#else
        uint16_t chunk = remaining >= TRANSFER_BUF_BYTES
            ? TRANSFER_BUF_BYTES
            : (uint16_t)remaining;
#endif
        int got = img->read_file_chunk_by_index(idx, off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
#if NOVAHOST_XLOAD_VERIFY
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
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
        if (!_bridge.pokeSdramStream(dest + off, _transfer_buf, chunk)) {
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
        uint16_t max_chunk = (target == PAGE_TARGET_XRAM)
            ? TRANSFER_BUF_BYTES
            : 256;
        uint16_t chunk = remaining >= max_chunk ? max_chunk : (uint16_t)remaining;
        int got = img->read_file_chunk_by_index(idx, src + off, _transfer_buf, chunk);
        if (got != (int)chunk) {
            respond_err(ERR_IO);
            return;
        }
        uint16_t wire_count = (chunk == 256) ? 0 : chunk;
        bool ok = false;
        switch (target) {
            case PAGE_TARGET_XRAM:
                ok = _bridge.pokeSdramStream(dest + off, _transfer_buf, chunk);
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
    if (write_next_dir_entry()) {
        respond_ok();
    } else {
        _dir_iter.active = false;
        respond_err(ERR_NOT_FOUND);
    }
}

bool FioDispatcher::write_next_dir_entry() {
    if (!_dir_iter.active)
        return false;

    auto* img = _dm.image(_dir_iter.slot);
    if (!img)
        return false;

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
            _bridge.poke(BANK_BASE + OFF_SIZE_2, (uint8_t)((e.size_bytes >> 16) & 0xFF));
            _bridge.poke(BANK_BASE + OFF_DIRTYPE, e.is_directory()
                                                  ? (uint8_t)ndi::FT_DIR
                                                  : (uint8_t)e.file_type);
            return true;
        }
    }
    _dir_iter.active = false;
    return false;
}

void FioDispatcher::handle_dir_read() {
    if (!_dir_iter.active) { respond_err(ERR_IO); return; }
    auto* img = _dm.image(_dir_iter.slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    if (write_next_dir_entry()) {
        respond_ok();
        return;
    }

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
