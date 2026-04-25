#include "device_manager.h"
#include <Arduino.h>
#include <ctype.h>
#include <string.h>

DeviceManager::DeviceManager()
    : _default_slot(HD0) {
    for (int i = 0; i < NUM_SLOTS; i++) {
        _streams[i] = nullptr;
        _images[i]  = nullptr;
        _current_dirs[i] = ndi::ROOT_PARENT;
    }
}

DeviceManager::~DeviceManager() {
    for (int i = 0; i < NUM_SLOTS; i++) unmount(i);
}

// ---------------------------------------------------------------------------
// Mount / unmount
// ---------------------------------------------------------------------------
bool DeviceManager::mount(int slot, const char* sd_path) {
    if (slot < 0 || slot >= NUM_SLOTS || !sd_path) return false;
    unmount(slot);

    _files[slot] = SD_MMC.open(sd_path, FILE_WRITE);
    if (!_files[slot]) {
        Serial.printf("[dm] mount %s: open failed\n", sd_path);
        return false;
    }

    _streams[slot] = new SdStream(_files[slot]);
    _images[slot]  = new ndi::NdiImage();
    if (!_images[slot]->open(_streams[slot])) {
        Serial.printf("[dm] mount %s: NdiImage::open failed (bad magic?)\n",
                      sd_path);
        unmount(slot);
        return false;
    }

    _current_dirs[slot] = ndi::ROOT_PARENT;
    Serial.printf("[dm] %s ← %s (label=%s, total=%u, free=%d)\n",
                  prefix_for_slot(slot), sd_path,
                  _images[slot]->header().volume_label,
                  (unsigned)_images[slot]->header().total_sectors,
                  _images[slot]->free_sectors());
    return true;
}

void DeviceManager::unmount(int slot) {
    if (slot < 0 || slot >= NUM_SLOTS) return;
    if (_images[slot])  { delete _images[slot];  _images[slot]  = nullptr; }
    if (_streams[slot]) { delete _streams[slot]; _streams[slot] = nullptr; }
    if (_files[slot])   { _files[slot].close(); }
    _current_dirs[slot] = ndi::ROOT_PARENT;
}

bool DeviceManager::is_mounted(int slot) const {
    if (slot < 0 || slot >= NUM_SLOTS) return false;
    return _images[slot] != nullptr;
}

ndi::NdiImage* DeviceManager::image(int slot) {
    if (slot < 0 || slot >= NUM_SLOTS) return nullptr;
    return _images[slot];
}

uint16_t DeviceManager::current_dir(int slot) const {
    if (slot < 0 || slot >= NUM_SLOTS) return ndi::ROOT_PARENT;
    return _current_dirs[slot];
}

// ---------------------------------------------------------------------------
// Path helpers
// ---------------------------------------------------------------------------
static int ascii_lower(int c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}
static int ci_strcmp(const char* a, const char* b) {
    for (;;) {
        int ca = ascii_lower((unsigned char)*a);
        int cb = ascii_lower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        if (ca == 0)  return 0;
        a++; b++;
    }
}

int DeviceManager::slot_for_prefix(const char* p) {
    if (!p) return -1;
    if (ci_strcmp(p, "hd0") == 0) return HD0;
    if (ci_strcmp(p, "hd1") == 0) return HD1;
    if (ci_strcmp(p, "fd0") == 0) return FD0;
    if (ci_strcmp(p, "fd1") == 0) return FD1;
    if (ci_strcmp(p, "fd2") == 0) return FD2;
    if (ci_strcmp(p, "fd3") == 0) return FD3;
    return -1;
}

const char* DeviceManager::prefix_for_slot(int slot) {
    switch (slot) {
        case HD0: return "hd0"; case HD1: return "hd1";
        case FD0: return "fd0"; case FD1: return "fd1";
        case FD2: return "fd2"; case FD3: return "fd3";
        default:  return nullptr;
    }
}

// Walk a slash-separated path under `slot` starting from current dir or
// root (depending on whether `path` started with '/'). The path is
// mutated in place — slashes become NUL bytes. Returns the parent index
// of the directory containing the final segment, plus a pointer to the
// basename within `path`.
bool DeviceManager::walk_to_parent(int slot, char* path, uint16_t& out_parent,
                                    char*& out_basename) {
    auto* img = image(slot);
    if (!img) return false;

    bool absolute = (path[0] == '/');
    if (absolute) path++;
    uint16_t parent = absolute ? ndi::ROOT_PARENT : _current_dirs[slot];

    // Find the LAST '/' — everything before is directory walk, after
    // is basename.
    char* last_slash = nullptr;
    for (char* p = path; *p; p++) if (*p == '/') last_slash = p;

    if (last_slash) {
        *last_slash = 0;
        out_basename = last_slash + 1;
        // Walk segments split by '/'.
        char* seg = path;
        while (*seg) {
            char* slash = strchr(seg, '/');
            if (slash) *slash = 0;
            if (*seg) {
                int idx = img->find_entry(seg, parent);
                if (idx < 0) return false;
                ndi::DirEntry e;
                if (!img->get_entry(idx, e) || !e.is_directory()) return false;
                parent = (uint16_t)idx;
            }
            seg = slash ? (slash + 1) : (seg + strlen(seg));
        }
    } else {
        out_basename = path;
    }

    out_parent = parent;
    return true;
}

bool DeviceManager::cd(int slot, const char* path) {
    if (slot < 0 || slot >= NUM_SLOTS) return false;
    auto* img = image(slot);
    if (!img) return false;

    if (!path || path[0] == 0 || (path[0] == '/' && path[1] == 0)) {
        _current_dirs[slot] = ndi::ROOT_PARENT;
        return true;
    }

    char buf[128];
    strncpy(buf, path, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;

    bool absolute = (buf[0] == '/');
    char* p = absolute ? buf + 1 : buf;
    uint16_t parent = absolute ? ndi::ROOT_PARENT : _current_dirs[slot];

    while (*p) {
        char* slash = strchr(p, '/');
        if (slash) *slash = 0;
        if (*p) {
            int idx = img->find_entry(p, parent);
            if (idx < 0) return false;
            ndi::DirEntry e;
            if (!img->get_entry(idx, e) || !e.is_directory()) return false;
            parent = (uint16_t)idx;
        }
        p = slash ? (slash + 1) : (p + strlen(p));
    }
    _current_dirs[slot] = parent;
    return true;
}

bool DeviceManager::resolve_path(const char* path, int& out_slot,
                                  uint16_t& out_parent, char* scratch) {
    if (!path || !scratch) return false;

    // Look for a "<prefix>:" device. Prefix is up to 8 chars before ':'.
    const char* colon = strchr(path, ':');
    int slot = _default_slot;
    const char* tail = path;

    if (colon && (colon - path) <= 8) {
        char prefix_buf[16];
        size_t plen = (size_t)(colon - path);
        memcpy(prefix_buf, path, plen);
        prefix_buf[plen] = 0;
        int s = slot_for_prefix(prefix_buf);
        if (s < 0) return false;
        slot = s;
        tail = colon + 1;
    }

    if (!is_mounted(slot)) return false;

    // Copy tail into scratch (mutable for walk_to_parent).
    strncpy(scratch, tail, 63);
    scratch[63] = 0;

    char* basename = nullptr;
    uint16_t parent;
    if (!walk_to_parent(slot, scratch, parent, basename)) return false;

    out_slot = slot;
    out_parent = parent;

    // Move basename to start of scratch so caller has a stable pointer.
    if (basename != scratch) {
        memmove(scratch, basename, strlen(basename) + 1);
    }
    return true;
}

// ---------------------------------------------------------------------------
// Auto-mount HDs
// ---------------------------------------------------------------------------
int DeviceManager::auto_mount_hds() {
    int mounted = 0;
    File root = SD_MMC.open("/");
    if (!root || !root.isDirectory()) return 0;

    File entry;
    while ((entry = root.openNextFile())) {
        const char* name = entry.name();
        // Match hd<N>.ndi where N is a single digit.
        if (strncasecmp(name, "hd", 2) == 0) {
            const char* p = name + 2;
            if (isdigit((unsigned char)*p)) {
                int n = *p - '0';
                p++;
                if (strcasecmp(p, ".ndi") == 0 && n >= 0 && n < NUM_HD) {
                    char path[64];
                    snprintf(path, sizeof(path), "/%s", name);
                    if (mount(n, path)) mounted++;
                }
            }
        }
        entry.close();
    }
    root.close();
    return mounted;
}
