#include "fio_dispatcher.h"
#include <Arduino.h>
#include <string.h>

extern void logLn(const char* fmt, ...);

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
    return ndi::FT_BIN;
}
}

void FioDispatcher::handle_event() {
    // Read the entire 80-byte register bank in one shot.
    if (!_bridge.peekBlock(BANK_BASE, 80, _bank)) {
        Serial.println("[fio] peekBlock failed — bank read aborted");
        return;
    }

    uint8_t c = cmd();
    if (c == 0) {
        // Spurious event: bank shows no pending cmd. Drop.
        return;
    }
    logLn("[fio] cmd=0x%02X namelen=%u\n",
                  (unsigned)c, (unsigned)namelen());

    switch (c) {
        case CMD_LOAD:     handle_load();     break;
        case CMD_SAVE:     handle_save();     break;
        case CMD_XLOAD:    handle_xload();    break;
        case CMD_XSAVE:    handle_xsave();    break;
        case CMD_DIR_OPEN: handle_dir_open(); break;
        case CMD_DIR_READ: handle_dir_read(); break;
        case CMD_DELETE:   handle_delete();   break;
        case CMD_CD:       handle_cd();       break;
        case CMD_MKDIR:    handle_mkdir();    break;
        case CMD_RMDIR:    handle_rmdir();    break;
        case CMD_MOUNT:    handle_mount();    break;
        case CMD_UNMOUNT:  handle_unmount();  break;
        default:
            logLn("[fio] unknown cmd 0x%02X\n", (unsigned)c);
            respond_err(ERR_IO);
            break;
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
}

void FioDispatcher::respond_err(uint8_t err_code) {
    _bridge.poke(BANK_BASE + OFF_ERRCODE, err_code);
    _bridge.poke(BANK_BASE + OFF_CMD,     0);
    _bridge.poke(BANK_BASE + OFF_STATUS,  ST_ERR);
}

void FioDispatcher::write_size(uint32_t size) {
    _bridge.poke(BANK_BASE + OFF_SIZE_LO, (uint8_t)(size & 0xFF));
    _bridge.poke(BANK_BASE + OFF_SIZE_HI, (uint8_t)((size >> 8) & 0xFF));
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

    int idx = img->find_entry(scratch, parent);
    if (idx < 0) {
        logLn("[fio] LOAD: '%s' not found in dev=%s\n",
                      scratch, DeviceManager::prefix_for_slot(slot));
        respond_err(ERR_NOT_FOUND);
        return;
    }

    ndi::DirEntry e;
    img->get_entry(idx, e);
    if (e.size_bytes > TRANSFER_BUF_BYTES) {
        // Future: stream in chunks. For now, hard cap.
        logLn("[fio] LOAD %s: %u bytes exceeds %d cap\n",
                      scratch, (unsigned)e.size_bytes, TRANSFER_BUF_BYTES);
        respond_err(ERR_IO);
        return;
    }

    int got = img->read_file_by_index(idx, _transfer_buf, sizeof(_transfer_buf));
    if (got < 0) { respond_err(ERR_IO); return; }

    uint16_t dest = src();
    if (!_bridge.loadRam(dest, _transfer_buf, (size_t)got)) {
        respond_err(ERR_IO);
        return;
    }
    write_size((uint32_t)got);
    logLn("[fio] LOAD %s → $%04X (%d bytes) OK\n",
                  scratch, dest, got);
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
    if (size > TRANSFER_BUF_BYTES) { respond_err(ERR_IO); return; }

    char scratch[64];
    int slot;
    uint16_t parent;
    if (!_dm.resolve_path(name, slot, parent, scratch)) {
        respond_err(ERR_NOT_FOUND);
        return;
    }
    auto* img = _dm.image(slot);
    if (!img) { respond_err(ERR_NO_MOUNT); return; }

    // Read the source range from CPU RAM via 256-byte peek blocks.
    uint32_t off = 0;
    while (off < size) {
        uint16_t chunk = (size - off >= 256) ? 256 : (uint16_t)(size - off);
        // peekBlock count param: 0 means 256 in protocol.
        uint8_t wire_count = (chunk == 256) ? 0 : (uint8_t)chunk;
        if (!_bridge.peekBlock((uint16_t)(s + off), wire_count, _transfer_buf + off)) {
            respond_err(ERR_IO);
            return;
        }
        off += chunk;
    }

    ndi::FileType ftype = file_type_for_name(scratch);

    // If a file with the same name already exists, delete it first
    // (mirrors Avalonia behavior — SAVE overwrites).
    int existing = img->find_entry(scratch, parent);
    if (existing >= 0) img->delete_file(scratch, parent);

    int new_idx = img->write_file(scratch, ftype, parent, _transfer_buf, size);
    if (new_idx < 0) {
        respond_err(ERR_FULL);
        return;
    }
    logLn("[fio] SAVE %s ($%04X-$%04X, %u bytes) OK\n",
                  scratch, s, e, (unsigned)size);
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
        if (!_bridge.pokeSdramBlock(dest + off, _transfer_buf, wire_count)) {
            respond_err(ERR_IO);
            return;
        }
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
