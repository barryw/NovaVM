// DeviceManager — owns the 6 disk-image slots (HD0/HD1, FD0..FD3) and
// the current-directory state for each. Mirrors the Avalonia-side
// e6502.Storage/DeviceManager.cs but trimmed for ESP32 RAM constraints.
//
// Each slot holds an SD_MMC `File` + an `SdStream` adapter + an
// `ndi::NdiImage`. Mount opens the .ndi file; Unmount disposes
// everything. Path resolution turns "hd0:games/launcher" or a bare
// "file.bas" (against the default device) into (slot, parent_index,
// filename).

#ifndef NOVAHOST_DEVICE_MANAGER_H
#define NOVAHOST_DEVICE_MANAGER_H

#include <FS.h>
#include <SD.h>
#include "ndi_image.h"
#include "sd_stream.h"

class DeviceManager {
public:
    static constexpr int NUM_HD     = 2;
    static constexpr int NUM_FD     = 4;
    static constexpr int NUM_SLOTS  = NUM_HD + NUM_FD;
    static constexpr int HD0        = 0;
    static constexpr int HD1        = 1;
    static constexpr int FD0        = 2;
    static constexpr int FD1        = 3;
    static constexpr int FD2        = 4;
    static constexpr int FD3        = 5;

    DeviceManager();
    ~DeviceManager();

    // Mount an .ndi file into the given slot. sd_path is absolute on
    // the SD root, e.g. "/hd0.ndi". Returns true on success.
    bool mount(int slot, const char* sd_path);

    // Unmount a slot, freeing all associated handles.
    void unmount(int slot);

    bool is_mounted(int slot) const;

    // Direct access to the parsed image — caller must NOT delete.
    ndi::NdiImage* image(int slot);

    // Per-slot directory walking. cd() accepts:
    //   "/"        → root
    //   "subdir"   → relative to current dir
    //   "/a/b/c"   → absolute from root
    // Returns true on success, false if any segment isn't a directory.
    uint16_t current_dir(int slot) const;
    bool     cd(int slot, const char* path);

    // Default device for un-prefixed paths (e.g. "myfile" with no
    // "hd0:" or "fd1:" prefix). Set by `CD "hd0:"`.
    int  default_slot() const { return _default_slot; }
    void set_default_slot(int s) { _default_slot = s; }

    // Slot ↔ prefix mapping. Recognises "hd0", "hd1", "fd0".."fd3".
    // Case-insensitive. Returns -1 / nullptr on miss.
    static int        slot_for_prefix(const char* prefix);
    static const char* prefix_for_slot(int slot);

    // Resolve a path like "hd0:games/spaceinv" or "myfile.bas" into
    // (slot, parent_index, basename). The path may include nested
    // subdirs: "hd0:games/spaceinv" → slot=HD0, parent="games" subdir,
    // basename="spaceinv". Returns false on lookup failure.
    //
    // out_basename is a pointer into a caller-owned scratch buffer
    // (`scratch`, must be at least 64 bytes). It's null-terminated.
    bool resolve_path(const char* path, int& out_slot,
                      uint16_t& out_parent, char* scratch);

    // Auto-mount all hd*.ndi files at SD root. Returns count mounted.
    int auto_mount_hds();

private:
    File         _files[NUM_SLOTS];
    SdStream*    _streams[NUM_SLOTS];
    ndi::NdiImage* _images[NUM_SLOTS];
    uint16_t     _current_dirs[NUM_SLOTS];
    int          _default_slot;

    // Helper: walk a "/-separated path under the given slot, returning
    // the resolved parent index for the directory containing the
    // basename. `path` is mutable scratch — gets split with NUL bytes.
    bool walk_to_parent(int slot, char* path, uint16_t& out_parent,
                         char*& out_basename);
};

#endif
