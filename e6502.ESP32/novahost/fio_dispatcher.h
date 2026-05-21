// FioDispatcher — wakes on FIO event, reads $B9A0-$B9EF, dispatches.
//
// On each EVENT_FIO from the FPGA:
//   1. Drain any pending CMD bytes via FpgaBridge.peekBlock($B9A0, 80)
//   2. Switch on FioCmd byte (offset 0)
//   3. Resolve filename + args
//   4. Execute via DeviceManager / NdiImage
//   5. Stream payload (LOAD/SAVE) via FpgaBridge.loadRam / peekBlock
//   6. Write FioStatus (offset 1) + FioErrCode (offset 2) back via
//      individual FpgaBridge.poke calls
//
// Mirrors the Avalonia FileIoController dispatch table.

#ifndef NOVAHOST_FIO_DISPATCHER_H
#define NOVAHOST_FIO_DISPATCHER_H

#include <stddef.h>
#include <vector>

#include "fpga_bridge.h"
#include "device_manager.h"
#include "sid_vm.h"

#if defined(ARDUINO)
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#endif

class FioDispatcher {
public:
    FioDispatcher(FpgaBridge& bridge, DeviceManager& dm)
        : _bridge(bridge), _dm(dm) {}

    // Trampoline matching FioEventReader::Handler.
    static void onFioEventStatic(void* user) {
        static_cast<FioDispatcher*>(user)->handle_event();
    }

    // Drain registers, dispatch command. Called by event reader.
    void handle_event();
    void poll_pending();
    void pump_audio();
    void stop_audio();
    bool reserve_wts_sample_cache(uint32_t bytes);
    void write_audio_status_json(char* out, size_t out_len) const;

private:
    FpgaBridge&    _bridge;
    DeviceManager& _dm;
    bool           _handling = false;

    // Register-bank wire offsets relative to $B9A0.
    static constexpr uint16_t BANK_BASE = 0xB9A0;
    static constexpr int OFF_CMD       = 0x00;
    static constexpr int OFF_STATUS    = 0x01;
    static constexpr int OFF_ERRCODE   = 0x02;
    static constexpr int OFF_NAMELEN   = 0x03;
    static constexpr int OFF_SRC_LO    = 0x04;
    static constexpr int OFF_SRC_HI    = 0x05;
    static constexpr int OFF_END_LO    = 0x06;
    static constexpr int OFF_END_HI    = 0x07;
    static constexpr int OFF_SIZE_LO   = 0x08;
    static constexpr int OFF_SIZE_HI   = 0x09;
    static constexpr int OFF_GSPACE    = 0x0A;
    static constexpr int OFF_SIZE_2    = OFF_GSPACE; // DIRREAD 24-bit size high byte
    static constexpr int OFF_GADDR_LO  = 0x0B;
    static constexpr int OFF_GADDR_HI  = 0x0C;
    static constexpr int OFF_GLEN_LO   = 0x0D;
    static constexpr int OFF_GLEN_HI   = 0x0E;
    static constexpr int OFF_DIRTYPE   = 0x0F;
    static constexpr int OFF_NAME      = 0x10;   // 64-byte filename buffer

    // Status values.
    static constexpr uint8_t ST_IDLE = 0;
    static constexpr uint8_t ST_OK   = 2;
    static constexpr uint8_t ST_ERR  = 3;

    // Error codes.
    static constexpr uint8_t ERR_NONE       = 0;
    static constexpr uint8_t ERR_NOT_FOUND  = 1;
    static constexpr uint8_t ERR_IO         = 2;
    static constexpr uint8_t ERR_NO_MOUNT   = 3;
    static constexpr uint8_t ERR_FULL       = 4;
    static constexpr uint8_t ERR_BAD_IMG    = 5;
    static constexpr uint8_t ERR_NO_SD      = 6;

    // Command codes — must match VgcConstants.cs.
    static constexpr uint8_t CMD_SAVE     = 0x01;
    static constexpr uint8_t CMD_LOAD     = 0x02;
    static constexpr uint8_t CMD_DIR_OPEN = 0x03;
    static constexpr uint8_t CMD_DIR_READ = 0x04;
    static constexpr uint8_t CMD_DELETE   = 0x05;
    static constexpr uint8_t CMD_GSAVE    = 0x06;
    static constexpr uint8_t CMD_GLOAD    = 0x07;
    static constexpr uint8_t CMD_SIDPLAY  = 0x08;
    static constexpr uint8_t CMD_SIDSTOP  = 0x09;
    static constexpr uint8_t CMD_MIDPLAY  = 0x13;
    static constexpr uint8_t CMD_MIDSTOP  = 0x14;
    static constexpr uint8_t CMD_SFLOAD   = 0x15;
    static constexpr uint8_t CMD_XLOAD    = 0x18;
    static constexpr uint8_t CMD_XSAVE    = 0x19;
    static constexpr uint8_t CMD_CD       = 0x20;
    static constexpr uint8_t CMD_MKDIR    = 0x21;
    static constexpr uint8_t CMD_RMDIR    = 0x22;
    static constexpr uint8_t CMD_FORMAT   = 0x23;
    static constexpr uint8_t CMD_MOUNT    = 0x24;
    static constexpr uint8_t CMD_UNMOUNT  = 0x25;
    static constexpr uint8_t CMD_PWD      = 0x26;
    static constexpr uint8_t CMD_CLEARERR = 0x27;
    static constexpr uint8_t CMD_LOADRUNTIME = 0x28;
    static constexpr uint8_t CMD_XPAGE    = 0x29;
    static constexpr uint8_t CMD_RNG      = 0x2A;
    static constexpr uint8_t CMD_NVGLOAD  = 0x2B;

    // Per-event state — only valid inside handle_event().
    uint8_t _bank[80];

    // Shared transfer buffer. Small CPU/VGC paths still use 256-byte protocol
    // blocks; FpgaBridge further slices SDRAM/XRAM streams to the safe FPGA SPI
    // RX FIFO burst size.
    static constexpr int TRANSFER_BUF_BYTES = 4096;
    static constexpr int RUNTIME_ROM_BYTES = 16 * 1024;
    static constexpr uint32_t XRAM_BYTES = 512UL * 1024UL;
#ifndef NOVAHOST_WTS_RESIDENT_SAMPLE_LIMIT_BYTES
#define NOVAHOST_WTS_RESIDENT_SAMPLE_LIMIT_BYTES (512UL * 1024UL)
#endif
#ifndef NOVAHOST_WTS_RESIDENT_HEAP_GUARD_BYTES
#define NOVAHOST_WTS_RESIDENT_HEAP_GUARD_BYTES (24UL * 1024UL)
#endif
    static constexpr uint32_t WTS_RESIDENT_SAMPLE_CHUNK_BYTES =
        16UL * 1024UL;
    static constexpr uint32_t WTS_RESIDENT_SAMPLE_LIMIT =
        NOVAHOST_WTS_RESIDENT_SAMPLE_LIMIT_BYTES;
    static constexpr uint32_t WTS_RESIDENT_HEAP_GUARD =
        NOVAHOST_WTS_RESIDENT_HEAP_GUARD_BYTES;
    uint8_t _transfer_buf[TRANSFER_BUF_BYTES];
    uint8_t _midi_read_cache[1024];

    // Fast accessors.
    uint8_t  cmd()     const { return _bank[OFF_CMD]; }
    uint8_t  namelen() const { return _bank[OFF_NAMELEN]; }
    uint16_t src()     const { return _bank[OFF_SRC_LO] |
                                       ((uint16_t)_bank[OFF_SRC_HI] << 8); }
    uint16_t end()     const { return _bank[OFF_END_LO] |
                                       ((uint16_t)_bank[OFF_END_HI] << 8); }
    uint32_t nvg_stage_addr() const { return ((uint32_t)_bank[OFF_END_LO] << 16) |
                                             ((uint32_t)_bank[OFF_SRC_HI] << 8) |
                                              (uint32_t)_bank[OFF_SRC_LO]; }
    uint32_t xram_addr() const { return ((uint32_t)_bank[OFF_GSPACE] << 16) |
                                        ((uint32_t)_bank[OFF_GADDR_HI] << 8) |
                                         (uint32_t)_bank[OFF_GADDR_LO]; }
    uint32_t file_offset() const { return ((uint32_t)_bank[OFF_END_LO] << 16) |
                                          ((uint32_t)_bank[OFF_SRC_HI] << 8) |
                                           (uint32_t)_bank[OFF_SRC_LO]; }
    uint8_t  page_target() const { return _bank[OFF_DIRTYPE]; }
    uint8_t  gspace() const { return _bank[OFF_GSPACE]; }
    uint16_t gaddr() const { return _bank[OFF_GADDR_LO] |
                                    ((uint16_t)_bank[OFF_GADDR_HI] << 8); }
    uint16_t transfer_len() const { return _bank[OFF_GLEN_LO] |
                                           ((uint16_t)_bank[OFF_GLEN_HI] << 8); }

    static constexpr uint8_t PAGE_TARGET_XRAM = 0x00;
    static constexpr uint8_t PAGE_TARGET_RAM  = 0x01;
    static constexpr uint8_t PAGE_TARGET_VGC  = 0x02;

    // Returns a null-terminated copy of the filename in `out` (size 64).
    void copy_filename(char* out);

    // Write back result. Always clears FioCmd to 0 so Nova's busy-wait
    // unblocks (FioStatus also flipped to OK or ERR).
    void respond_ok();
    void respond_err(uint8_t err_code);

    // Set returned size (LOAD).
    void write_size(uint32_t size);

    // ---- Command handlers ----
    void handle_load();
    void handle_save();
    void handle_gload();
    void handle_gsave();
    void handle_xload();
    void handle_xsave();
    void handle_xpage();
    void handle_dir_open();
    void handle_dir_read();
    bool write_next_dir_entry();
    void handle_delete();
    void handle_cd();
    void handle_mkdir();
    void handle_rmdir();
    void handle_mount();
    void handle_unmount();
    void handle_pwd();
    void handle_clear_error();
    void handle_load_runtime();
    void handle_rng();
    void handle_nvgload();
    void handle_sidplay();
    void handle_sidstop();
    void handle_midplay();
    void handle_midstop();
    void handle_sfload();
    void handle_unsupported_sd_command(const char* name);

    // MIDI playback is NovaHost-driven: MIDPLAY returns after scheduling the
    // parsed timeline, then poll_pending() keeps the FPGA WTS event FIFO fed.
    void tick_midi_playback();
    void tick_midi_hardware_wts();
    void stop_midi_playback(bool silence);
    void tick_sid_playback();
    void stop_sid_playback(bool silence);
    bool flush_sid_writes();
    static bool on_sid_write_static(void* user, uint16_t addr, uint8_t value);
    bool on_sid_write(uint16_t addr, uint8_t value);
    void reset_midi_timing_metrics();
    void finish_midi_hardware_pump(uint32_t pump_start_us);
    bool read_music_stream_header(ndi::NdiImage* img, int idx,
                                  uint32_t size);
    bool queue_music_wts_events();
    bool read_wts_event_fifo_status(uint16_t& free_records,
                                    bool& empty,
                                    bool& parser_error);
    bool start_music_wts_event_stream();
    bool hardware_wts_available();
    bool configure_wts_for_midi();
    bool configure_wts_voice_envelope(uint8_t voice, uint16_t attack_step,
                                      uint16_t decay_step,
                                      uint16_t sustain_level,
                                      uint16_t release_step);
    bool configure_wts_sample_voice(uint8_t voice, uint32_t sample_start,
                                    uint32_t sample_end, uint32_t loop_start,
                                    uint32_t loop_end, uint32_t phase_step,
                                    bool loop, uint16_t attack_step,
                                    uint16_t decay_step,
                                    uint16_t sustain_level,
                                    uint16_t release_step);
    void clear_wts_bank_state();
    void release_wts_sample_cache();
    bool cache_wts_sample_bytes(uint32_t offset, const uint8_t* data,
                                uint16_t len);
    uint8_t read_wts_sample_byte(uint32_t offset) const;
    bool write_wts_sample_chunk_to_sdram(uint32_t offset, const uint8_t* data,
                                         uint16_t len);
    bool load_file_to_vector(ndi::NdiImage* img, int idx, uint32_t size,
                             std::vector<uint8_t>& out);
    bool load_wts_bank_by_name(const char* requested_name);
    bool load_first_wts_bank_from_sd_dir(const char* dir_path);
    bool load_first_wts_bank_from_slot(int slot);
    bool ensure_wts_bank_loaded(int preferred_slot);
    bool load_wts_bank_from_sd_path(const char* sd_path);
    bool load_wts_bank_from_entry(ndi::NdiImage* img, int idx, uint32_t size,
                                  const char* label);
    bool find_wts_region(uint8_t bank, uint8_t program, uint8_t note,
                         uint8_t velocity, uint32_t& sample_start,
                         uint32_t& sample_end, uint32_t& loop_start,
                         uint32_t& loop_end, uint32_t& phase_step,
                         bool& loop, uint16_t& attack_step,
                         uint16_t& decay_step, uint16_t& sustain_level,
                         uint16_t& release_step);
    bool build_music_wts_event(const uint8_t* song_event, uint8_t* out,
                               uint16_t& out_bytes);

    static constexpr uint8_t WtsVoiceCount = 8;

    bool     _midi_playing = false;
    int      _midi_slot = -1;
    int      _midi_file_index = -1;
    uint32_t _midi_file_size = 0;
    uint32_t _music_event_offset = 0;
    uint32_t _music_event_bytes = 0;
    uint32_t _music_event_read = 0;
    uint32_t _music_event_count = 0;
    uint32_t _music_total_frames = 0;
    size_t   _midi_event_index = 0;
    double   _midi_tick_accum = 0.0;
    uint32_t _midi_note_ons = 0;
    uint32_t _midi_sample_note_ons = 0;
    uint32_t _midi_sample_misses = 0;
    bool     _midi_use_hardware_wts = false;
    uint8_t  _midi_last_stop_reason = 0;
    uint32_t _midi_pump_calls = 0;
    uint32_t _midi_last_pump_enter_us = 0;
    uint32_t _midi_last_pump_gap_us = 0;
    uint32_t _midi_max_pump_gap_us = 0;
    uint32_t _midi_last_pump_duration_us = 0;
    uint32_t _midi_max_pump_duration_us = 0;
    char     _midi_last_error[96] = {};
    bool     _midi_wts_all_events_queued = false;

    static constexpr uint8_t SID_WRITE_BATCH_MAX = 96;
    nova_sid::SidVm _sid_vm;
    bool     _sid_playing = false;
    bool     _sid_write_overflow = false;
    uint8_t  _sid_write_count = 0;
    uint16_t _sid_write_addrs[SID_WRITE_BATCH_MAX] = {};
    uint8_t  _sid_write_values[SID_WRITE_BATCH_MAX] = {};
    uint32_t _sid_next_frame_us = 0;
    uint32_t _sid_frame_period_us = 20000;
    uint32_t _sid_frames = 0;
    uint8_t  _sid_song = 1;
    uint8_t  _sid_last_stop_reason = 0;
    char     _sid_last_error[96] = {};
    char     _sid_name[64] = {};

    struct WtsBankInstrument {
        uint8_t bank = 0;
        uint8_t program = 0;
        uint16_t first_region = 0;
        uint16_t region_count = 0;
    };

    struct WtsBankRegion {
        uint8_t key_lo = 0;
        uint8_t key_hi = 127;
        uint8_t vel_lo = 0;
        uint8_t vel_hi = 127;
        uint8_t root_key = 60;
        uint8_t flags = 0;
        uint16_t sample_rate = 48000;
        int16_t tuning_cents = 0;
        uint32_t sample_start = 0;
        uint32_t sample_length = 0;
        uint32_t loop_start = 0;
        uint32_t loop_end = 0;
        uint16_t attack_frames = 0;
        uint16_t decay_frames = 0;
        uint16_t sustain_level = 0xA000;
        uint16_t release_frames = 0;
    };

    struct WtsRegionStore {
        static constexpr uint16_t CHUNK_SIZE = 64;
        static constexpr uint16_t MAX_CHUNKS = 64;

        WtsBankRegion* chunks[MAX_CHUNKS] = {};
        uint16_t count = 0;
        uint16_t capacity = 0;

        WtsRegionStore() = default;
        ~WtsRegionStore();
        WtsRegionStore(const WtsRegionStore&) = delete;
        WtsRegionStore& operator=(const WtsRegionStore&) = delete;
        WtsRegionStore(WtsRegionStore&& other) noexcept;
        WtsRegionStore& operator=(WtsRegionStore&& other) noexcept;

        void clear();
        bool reserve(uint16_t requested);
        bool push_back(const WtsBankRegion& region);
        size_t size() const { return count; }
        const WtsBankRegion& operator[](uint16_t index) const {
            return chunks[index / CHUNK_SIZE][index % CHUNK_SIZE];
        }

    private:
        void move_from(WtsRegionStore& other);
    };

    std::vector<WtsBankInstrument> _wts_instruments;
    WtsRegionStore _wts_regions;
    std::vector<uint8_t*> _wts_sample_chunks;
    uint32_t _wts_sample_capacity = 0;
    bool _wts_bank_loaded = false;
    bool _wts_samples_resident = false;
    char _wts_bank_name[96] = {};
    uint32_t _wts_sample_bytes = 0;
    uint8_t _wts_sample_frame_bytes = 1;
    uint32_t _wts_bank_hash = 0;
    bool _wts_voice_env_valid[WtsVoiceCount] = {};
    uint16_t _wts_voice_attack_step[WtsVoiceCount] = {};
    uint16_t _wts_voice_decay_step[WtsVoiceCount] = {};
    uint16_t _wts_voice_sustain_level[WtsVoiceCount] = {};
    uint16_t _wts_voice_release_step[WtsVoiceCount] = {};

    // Directory-iterator state for DIR_OPEN/DIR_READ. Single iterator
    // across all sessions — Nova only opens one at a time in practice.
    struct DirIter {
        bool     active;
        int      slot;
        uint16_t parent;
        int      next_index;       // walk pointer in _bank/_dir buffer
    } _dir_iter = { false, 0, 0, 0 };
};

#endif
