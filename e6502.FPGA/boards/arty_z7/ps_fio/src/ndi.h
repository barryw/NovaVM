// ndi.h — thin C binding over the shared ndi::NdiImage parser.
//
// The actual .ndi parsing lives in the cross-platform e6502.ESP32/novahost/
// ndi_image.{h,cpp} (compiled into this firmware unchanged). ndi_shim.cpp
// implements this C API on top of ndi::NdiImage + a FatFs IStream, so the C FIO
// host (main.c) can read a mounted image without duplicating any parser logic.
// One image is open at a time (the boot drive), matching main.c's boot_image().

#ifndef NDI_H
#define NDI_H

#include <stdint.h>

#define NDI_ROOT 0xFFFFu      // parent index of root-level entries

typedef struct { int open; } ndi_t;   // handle; real state is in the shim singleton

typedef struct {
    int      index;
    int      active;
    int      is_dir;
    uint8_t  type;            // FT_BAS=0,SID=1,BIN=2,MID=3,GFX=4,DIR=5,FORTH=6,LOGO=7,PASCAL=8,ASM=9
    uint16_t parent;
    uint32_t start_sector;
    uint32_t size;
    char     name[33];
} ndi_entry_t;

#ifdef __cplusplus
extern "C" {
#endif

int  ndi_open(ndi_t *img, const char *sd_path);                 // 0 ok, <0 error
void ndi_close(ndi_t *img);
int  ndi_find(ndi_t *img, const char *name, uint16_t parent);   // slot, or -1
int  ndi_get(ndi_t *img, int index, ndi_entry_t *out);          // 0 ok, <0 error
int  ndi_list(ndi_t *img, uint16_t parent, ndi_entry_t *out, int max); // count
int  ndi_read(ndi_t *img, int index, uint32_t file_offset, void *buf, uint32_t len);

// ---- write (save support) -------------------------------------------------
// Create a new file entry sized `size` (allocates contiguous data sectors +
// a directory slot). Returns the new entry's index, or <0 on error.
int  ndi_create(ndi_t *img, const char *name, uint8_t type, uint16_t parent, uint32_t size);
// Write a slice of a created file by index. 0 ok, <0 error.
int  ndi_write(ndi_t *img, int index, uint32_t file_offset, const void *buf, uint32_t len);
// Zero the unused tail of the final allocated sector of a created file.
int  ndi_zero_tail(ndi_t *img, int index);
// Delete a file by name under `parent`. 0 ok, <0 if missing/dir.
int  ndi_delete(ndi_t *img, const char *name, uint16_t parent);
// Flush the backing stream to the SD card (persist metadata + data).
void ndi_flush(ndi_t *img);

#ifdef __cplusplus
}
#endif

#endif
