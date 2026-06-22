// ndi_shim.cpp — C binding over the shared ndi::NdiImage parser (ndi.h).
//
// The .ndi parsing logic is NOT reimplemented here: it lives in the
// cross-platform e6502.ESP32/novahost/ndi_image.{h,cpp}, compiled into this
// firmware unchanged. This file only adapts that C++ parser to (a) a FatFs
// backing stream and (b) the plain-C API the FIO host (main.c) calls. One image
// is open at a time — the boot drive — so a single static instance suffices.

#include "ndi_image.h"

extern "C" {
#include "ff.h"
#include "ndi.h"
}

namespace {

// ndi::IStream backed by a FatFs file (the .ndi on the microSD).
class FatfsStream : public ndi::IStream {
public:
    FIL  f;
    bool is_open = false;

    bool open_path(const char *path) {
        if (is_open) { f_close(&f); is_open = false; }
        // Read-write: saves (FCREATE/FWRITE) write entries back into the .ndi.
        // Fall back to read-only if the image is on read-only media.
        if (f_open(&f, path, FA_READ | FA_WRITE) != FR_OK &&
            f_open(&f, path, FA_READ) != FR_OK) return false;
        is_open = true;
        return true;
    }
    void close_file() {
        if (is_open) { f_close(&f); is_open = false; }
    }

    bool seek(uint64_t offset) override {
        return is_open && f_lseek(&f, (FSIZE_t)offset) == FR_OK;
    }
    int read(uint8_t *buf, size_t len) override {
        if (!is_open) return -1;
        UINT br = 0;
        if (f_read(&f, buf, (UINT)len, &br) != FR_OK) return -1;
        return (int)br;
    }
    int write(const uint8_t *buf, size_t len) override {
        if (!is_open) return -1;
        UINT bw = 0;
        if (f_write(&f, buf, (UINT)len, &bw) != FR_OK) return -1;
        return (int)bw;
    }
    void flush() override { if (is_open) f_sync(&f); }
};

FatfsStream    g_stream;
ndi::NdiImage  g_image;

}  // namespace

extern "C" {

int ndi_open(ndi_t *img, const char *sd_path) {
    img->open = 0;
    g_image.close();
    if (!g_stream.open_path(sd_path)) return -1;
    if (!g_image.open(&g_stream)) { g_stream.close_file(); return -2; }
    img->open = 1;
    return 0;
}

void ndi_close(ndi_t *img) {
    g_image.close();
    g_stream.close_file();
    img->open = 0;
}

int ndi_find(ndi_t *img, const char *name, uint16_t parent) {
    (void)img;
    return g_image.find_entry(name, parent);
}

static void copy_entry(const ndi::DirEntry &de, ndi_entry_t *out) {
    out->index        = de.index;
    out->active       = de.is_active() ? 1 : 0;
    out->is_dir       = de.is_directory() ? 1 : 0;
    out->type         = de.file_type;
    out->parent       = de.parent_index;
    out->start_sector = de.start_sector;
    out->size         = de.size_bytes;
    for (int i = 0; i < 33; i++) { out->name[i] = de.filename[i]; if (!de.filename[i]) break; }
    out->name[32] = 0;
}

int ndi_get(ndi_t *img, int index, ndi_entry_t *out) {
    (void)img;
    ndi::DirEntry de;
    if (!g_image.get_entry(index, de)) return -1;
    copy_entry(de, out);
    return 0;
}

int ndi_list(ndi_t *img, uint16_t parent, ndi_entry_t *out, int max) {
    (void)img;
    static ndi::DirEntry tmp[64];
    int want = max < 64 ? max : 64;
    int count = 0;
    g_image.list_entries(parent, tmp, want, count);
    for (int i = 0; i < count; i++) copy_entry(tmp[i], &out[i]);
    return count;
}

int ndi_read(ndi_t *img, int index, uint32_t file_offset, void *buf, uint32_t len) {
    (void)img;
    return g_image.read_file_chunk_by_index(index, file_offset, (uint8_t *)buf, len);
}

int ndi_create(ndi_t *img, const char *name, uint8_t type, uint16_t parent, uint32_t size) {
    (void)img;
    return g_image.create_file(name, (ndi::FileType)type, parent, size);
}

int ndi_write(ndi_t *img, int index, uint32_t file_offset, const void *buf, uint32_t len) {
    (void)img;
    return g_image.write_file_chunk_by_index(index, file_offset, (const uint8_t *)buf, len) ? 0 : -1;
}

int ndi_zero_tail(ndi_t *img, int index) {
    (void)img;
    return g_image.zero_file_tail_by_index(index) ? 0 : -1;
}

int ndi_delete(ndi_t *img, const char *name, uint16_t parent) {
    (void)img;
    return g_image.delete_file(name, parent) ? 0 : -1;
}

void ndi_flush(ndi_t *img) {
    (void)img;
    g_stream.flush();
}

}  // extern "C"
