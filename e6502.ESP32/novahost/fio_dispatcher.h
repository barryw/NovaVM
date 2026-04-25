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

#include "fpga_bridge.h"
#include "device_manager.h"

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

private:
    FpgaBridge&    _bridge;
    DeviceManager& _dm;

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
    static constexpr uint8_t CMD_CD       = 0x20;
    static constexpr uint8_t CMD_MKDIR    = 0x21;
    static constexpr uint8_t CMD_RMDIR    = 0x22;
    static constexpr uint8_t CMD_MOUNT    = 0x24;
    static constexpr uint8_t CMD_UNMOUNT  = 0x25;

    // Per-event state — only valid inside handle_event().
    uint8_t _bank[80];

    // Fast accessors.
    uint8_t  cmd()     const { return _bank[OFF_CMD]; }
    uint8_t  namelen() const { return _bank[OFF_NAMELEN]; }
    uint16_t src()     const { return _bank[OFF_SRC_LO] |
                                       ((uint16_t)_bank[OFF_SRC_HI] << 8); }
    uint16_t end()     const { return _bank[OFF_END_LO] |
                                       ((uint16_t)_bank[OFF_END_HI] << 8); }

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
    void handle_dir_open();
    void handle_dir_read();
    void handle_delete();
    void handle_cd();
    void handle_mkdir();
    void handle_rmdir();
    void handle_mount();
    void handle_unmount();

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
