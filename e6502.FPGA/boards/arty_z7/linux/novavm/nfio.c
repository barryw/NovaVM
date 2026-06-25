/* nfio.c — NovaVM XRAM + file-handle + .ndi disk-image FIO commands.
 *
 * Linux userspace port of the bare-metal ps_fio/src/main.c FIO host. See nfio.h
 * for the contract + adaptation notes. Sources of truth (ported faithfully):
 *   ps_fio/src/main.c    — FIO handlers, boot_image()/img_find(), g_fh[]/g_wbuf
 *   ps_fio/src/ndi.c     — the .ndi reader C shim (over ndi::NdiImage)
 *   ps_fio/src/drives.c  — the drive mount table
 *   e6502.ESP32/novahost/ndi_image.cpp — the actual .ndi parser (re-ported to C)
 *   linux/novavm/nservers.c — the mgmt mount table -> /data/nova/config/mounts.txt
 *
 * ===========================================================================
 * FIO_CMD_* -> handler map (add these cases to novavm.c's FIO switch):
 *   FIO_CMD_XLOAD       0x18 -> fio_xload()         (stream image file -> XRAM)
 *   FIO_CMD_LOADRUNTIME 0x28 -> fio_load_runtime()  (16KB ROM -> $C000 bank)
 *   FIO_CMD_XPAGE       0x29 -> fio_xpage()         (file slice -> XRAM/CPU RAM)
 *   FIO_CMD_FOPEN       0x2D -> fio_fopen()
 *   FIO_CMD_FCREATE     0x2E -> fio_fcreate()
 *   FIO_CMD_FCLOSE      0x2F -> fio_fclose()
 *   FIO_CMD_FREAD       0x30 -> fio_fread()
 *   FIO_CMD_FWRITE      0x31 -> fio_fwrite()
 *   FIO_CMD_FSEEK       0x32 -> fio_fseek()
 *   FIO_CMD_FTELL       0x33 -> fio_ftell()
 *   FIO_CMD_FSIZE       0x34 -> fio_fsize()
 * (FIO_CMD_LOAD 0x02 stays in novavm.c — call nfio_image_load() first, then its
 *  existing /data/nova/programs fallback. See nfio_image_load() below.)
 * ===========================================================================
 *
 * Cross-compile (clean):
 *   arm-linux-gnueabihf-gcc -O2 -pthread -Wall -c -I. nfio.c
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>

#include "novavm.h"
#include "nbootcfg.h"
#include "nfio.h"

/* ===========================================================================
 *  FIO mailbox extensions (novavm.h has the base set; these are the extra
 *  registers + opcodes the bare-metal main.c uses for XRAM / file handles).
 * =========================================================================== */
#ifndef FIO_SIZE2
#define FIO_SIZE2     0xB9AA   /* size byte 2 (24-bit sizes) — aliases FIO_GSPACE */
#endif
#define FIO_GSPACE    0xB9AA   /* XRAM addr high byte (also size byte2)            */
#define FIO_GADDR_LO  0xB9AB   /* XRAM addr low                                   */
#define FIO_GADDR_HI  0xB9AC   /* XRAM addr mid                                   */
#define FIO_GLEN_LO   0xB9AD   /* transfer length low                             */
#define FIO_GLEN_HI   0xB9AE   /* transfer length high                            */

#define FIO_CMD_XLOAD       0x18
#define FIO_CMD_LOADRUNTIME 0x28
#define FIO_CMD_XPAGE       0x29
#define FIO_CMD_FOPEN       0x2D
#define FIO_CMD_FCREATE     0x2E
#define FIO_CMD_FCLOSE      0x2F
#define FIO_CMD_FREAD       0x30
#define FIO_CMD_FWRITE      0x31
#define FIO_CMD_FSEEK       0x32
#define FIO_CMD_FTELL       0x33
#define FIO_CMD_FSIZE       0x34

#define FIO_PAGE_XRAM   0x00   /* XPAGE target: flat XRAM       */
#define FIO_PAGE_RAM    0x01   /* XPAGE target: CPU RAM         */
#define FIO_TARGET_MASK 0x30   /* FREAD/FWRITE target high bits */
#define FIO_TARGET_XRAM 0x10   /* 0x00 = CPU RAM, 0x10 = XRAM   */

#define FIO_ERR_NOTMOUNTED 5

/* NDI / 6502 file types (wire values) */
#define DT_BAS 0
#define DT_SID 1
#define DT_BIN 2
#define DT_MID 3
#define DT_GFX 4
#define DT_DIR 5
#define DT_FORTH 6

/* XRAM shelf offsets (within the 1 MB g_xram window). Preserved from bare-metal
 * (SHELF_BASE/SHELF_SLOT used by the module loader; config page at +0x80000). */
#define SHELF_BASE  0x00060000u
#define SHELF_SLOT  0x00004000u   /* 16 KB */

/* ===========================================================================
 *  XRAM access (PS-DDR window mmap'd by novavm.c as g_xram; base = XRAM_DDR_BASE,
 *  size XRAM_BYTES). The bare-metal wrote PS DDR at XRAM_DDR_BASE+off + flushed
 *  the cache so the PL HP0 port saw it; on Linux the mapping is coherent, so a
 *  plain store into g_xram[off] suffices (off is the XRAM byte offset, 0 == DDR
 *  XRAM_DDR_BASE). EVERY XRAM command guards on g_xram != NULL first.
 * =========================================================================== */
static void xram_write(unsigned off, const void *src, unsigned n) {
    memcpy((void *)(g_xram + off), src, n);
}
static void xram_read(unsigned off, void *dst, unsigned n) {
    memcpy(dst, (const void *)(g_xram + off), n);
}

/* ===========================================================================
 *  .ndi disk-image reader/writer — pure-C port of ndi::NdiImage (ndi_image.cpp)
 *  over a POSIX FILE* stream. One image open at a time (the boot drive), matching
 *  the bare-metal boot_image() / single-shim-singleton model.
 * =========================================================================== */
#define NDI_SECTOR_SIZE   256
#define NDI_FORMAT_VER    2
#define NDI_ENTRY_SIZE    64
#define NDI_ENTRIES_PER_SEC 4
#define NDI_MAX_NAME      32
#define NDI_ROOT          0xFFFFu   /* parent index of root-level entries */

/* directory entry byte offsets within a 64-byte slot */
#define OFF_FLAGS      0x00
#define OFF_TYPE       0x01
#define OFF_PARENT     0x02   /* u16 LE */
#define OFF_START_SEC  0x04   /* u32 LE */
#define OFF_SIZE       0x08   /* u32 LE */
#define OFF_FILENAME   0x0C   /* 32 bytes */
#define OFF_SECCOUNT   0x2C   /* u32 LE */

#define FL_ACTIVE    0x01
#define FL_DIRECTORY 0x02

typedef struct {
    int      open;
    FILE    *fp;
    /* parsed header */
    uint32_t total_sectors;
    uint32_t directory_start_sector;
    uint32_t directory_sector_count;
    uint32_t data_start_sector;
    /* BAM bitmap (1 bit/data sector) */
    uint8_t *bam_bits;
    int      bam_byte_count;
    int      data_sector_count;
    int      free_count;
    /* directory single-sector write-back cache */
    uint8_t  dir_cache[NDI_SECTOR_SIZE];
    int      dir_cached_sector;
    int      dir_cache_dirty;
    int      dir_sector_count;
    int      dir_entry_count;     /* slots = sectors * 4 */
} ndi_t;

typedef struct {
    int      index;
    uint8_t  flags;
    uint8_t  file_type;
    uint16_t parent_index;
    uint32_t start_sector;
    uint32_t size_bytes;
    uint32_t sector_count;
    char     filename[NDI_MAX_NAME + 1];
} ndi_entry_t;

/* ---- LE helpers ----------------------------------------------------------- */
static uint16_t rd_u16(const uint8_t *p) { return (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); }
static uint32_t rd_u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static void wr_u16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void wr_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

static int ndi_lower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
static int ndi_name_eq_ci(const char *a, const char *b) {
    for (int i = 0; i < NDI_MAX_NAME; i++) {
        char ca = a[i], cb = b[i];
        if (ca == 0 && cb == 0) return 1;
        if (ndi_lower((unsigned char)ca) != ndi_lower((unsigned char)cb)) return 0;
        if (ca == 0) return 1;
    }
    return 1;
}

/* ---- stream primitives (POSIX FILE*; offsets absolute from file start) ----- */
static int ndi_pread(ndi_t *m, uint64_t off, void *buf, int len) {
    if (fseek(m->fp, (long)off, SEEK_SET) != 0) return -1;
    return (int)fread(buf, 1, (size_t)len, m->fp);
}
static int ndi_pwrite(ndi_t *m, uint64_t off, const void *buf, int len) {
    if (fseek(m->fp, (long)off, SEEK_SET) != 0) return -1;
    return (int)fwrite(buf, 1, (size_t)len, m->fp);
}

/* ---- BAM helpers ---------------------------------------------------------- */
static int ndi_bam_is_alloc(const ndi_t *m, int sector) {
    if (sector < 0 || sector >= m->data_sector_count) return 1;  /* OOB = occupied */
    return (m->bam_bits[sector >> 3] & (1 << (sector & 7))) != 0;
}
static int ndi_bam_alloc_contig(ndi_t *m, int count) {
    if (count <= 0) return -1;
    int run_start = -1, run_len = 0;
    for (int i = 0; i < m->data_sector_count; i++) {
        if (!ndi_bam_is_alloc(m, i)) {
            if (run_start < 0) run_start = i;
            run_len++;
            if (run_len == count) {
                for (int j = run_start; j < run_start + count; j++)
                    m->bam_bits[j >> 3] |= (uint8_t)(1 << (j & 7));
                m->free_count -= count;
                return run_start;
            }
        } else { run_start = -1; run_len = 0; }
    }
    return -1;
}
static void ndi_bam_free(ndi_t *m, int start, int count) {
    if (start < 0 || count <= 0) return;
    if (start + count > m->data_sector_count) return;
    for (int i = start; i < start + count; i++) {
        if (ndi_bam_is_alloc(m, i)) { m->bam_bits[i >> 3] &= (uint8_t)~(1 << (i & 7)); m->free_count++; }
    }
}

/* ---- directory cache ------------------------------------------------------ */
static int ndi_flush_dir_cache(ndi_t *m) {
    if (!m->dir_cache_dirty || m->dir_cached_sector < 0) return 1;
    uint64_t off = (uint64_t)(m->directory_start_sector + (uint32_t)m->dir_cached_sector) * NDI_SECTOR_SIZE;
    if (ndi_pwrite(m, off, m->dir_cache, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return 0;
    m->dir_cache_dirty = 0;
    return 1;
}
static int ndi_load_dir_sector(ndi_t *m, int sector) {
    if (sector < 0 || sector >= m->dir_sector_count) return 0;
    if (m->dir_cached_sector == sector) return 1;
    if (!ndi_flush_dir_cache(m)) return 0;
    uint64_t off = (uint64_t)(m->directory_start_sector + (uint32_t)sector) * NDI_SECTOR_SIZE;
    if (ndi_pread(m, off, m->dir_cache, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return 0;
    m->dir_cached_sector = sector;
    m->dir_cache_dirty = 0;
    return 1;
}
static uint8_t *ndi_dir_entry_ptr(ndi_t *m, int slot) {
    if (slot < 0 || slot >= m->dir_entry_count) return NULL;
    int sector = slot / NDI_ENTRIES_PER_SEC;
    int entry  = slot % NDI_ENTRIES_PER_SEC;
    if (!ndi_load_dir_sector(m, sector)) return NULL;
    return m->dir_cache + entry * NDI_ENTRY_SIZE;
}
static void ndi_dir_read_entry(ndi_t *m, int slot, ndi_entry_t *out) {
    uint8_t *p = ndi_dir_entry_ptr(m, slot);
    memset(out, 0, sizeof *out);
    out->index = slot;
    if (!p) return;
    out->flags        = p[OFF_FLAGS];
    out->file_type    = p[OFF_TYPE];
    out->parent_index = rd_u16(&p[OFF_PARENT]);
    out->start_sector = rd_u32(&p[OFF_START_SEC]);
    out->size_bytes   = rd_u32(&p[OFF_SIZE]);
    int n = 0;
    for (; n < NDI_MAX_NAME; n++) { char c = (char)p[OFF_FILENAME + n]; if (!c) break; out->filename[n] = c; }
    out->filename[n]  = 0;
    out->sector_count = rd_u32(&p[OFF_SECCOUNT]);
}
static void ndi_dir_write_entry(ndi_t *m, int slot, uint8_t flags, uint8_t type,
                                uint16_t parent, uint32_t start_sector, uint32_t size,
                                const char *name, uint32_t sector_count) {
    uint8_t *p = ndi_dir_entry_ptr(m, slot);
    if (!p) return;
    p[OFF_FLAGS] = flags;
    p[OFF_TYPE]  = type;
    wr_u16(&p[OFF_PARENT], parent);
    wr_u32(&p[OFF_START_SEC], start_sector);
    wr_u32(&p[OFF_SIZE], size);
    memset(&p[OFF_FILENAME], 0, NDI_MAX_NAME);
    for (int n = 0; name[n] && n < NDI_MAX_NAME; n++) p[OFF_FILENAME + n] = (uint8_t)name[n];
    wr_u32(&p[OFF_SECCOUNT], sector_count);
    m->dir_cache_dirty = 1;
}
static void ndi_dir_clear_slot(ndi_t *m, int slot) {
    uint8_t *p = ndi_dir_entry_ptr(m, slot);
    if (p) { p[OFF_FLAGS] = 0; m->dir_cache_dirty = 1; }
}
static int ndi_dir_find_free_slot(ndi_t *m) {
    for (int i = 0; i < m->dir_entry_count; i++) {
        uint8_t *p = ndi_dir_entry_ptr(m, i);
        if (p && p[OFF_FLAGS] == 0) return i;
    }
    return -1;
}

/* ---- header + BAM read (open) --------------------------------------------- */
static int ndi_read_header(ndi_t *m) {
    uint8_t buf[NDI_SECTOR_SIZE];
    if (ndi_pread(m, 0, buf, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return -1;
    if (buf[0] != 'N' || buf[1] != 'D' || buf[2] != 'I' || buf[3] != 0x1A) return -1;
    if (buf[4] != NDI_FORMAT_VER) return -1;
    if (rd_u16(&buf[5]) != NDI_SECTOR_SIZE) return -1;
    m->total_sectors          = rd_u32(&buf[0x08]);
    m->directory_start_sector = rd_u32(&buf[0x2C]);
    m->directory_sector_count = rd_u32(&buf[0x30]);
    m->data_start_sector      = rd_u32(&buf[0x34]);
    if (m->directory_start_sector < 1) return -1;
    if (m->data_start_sector <= m->directory_start_sector) return -1;
    if (m->total_sectors < m->data_start_sector) return -1;
    if (m->total_sectors > 0x7FFFFFFFUL) return -1;
    return 0;
}
static int ndi_read_bam(ndi_t *m) {
    int bam_sectors = (int)(m->directory_start_sector - 1);
    if (bam_sectors < 1) return -1;
    m->data_sector_count = (int)(m->total_sectors - m->data_start_sector);
    m->bam_byte_count    = (m->data_sector_count + 7) >> 3;
    if (m->bam_byte_count > bam_sectors * NDI_SECTOR_SIZE) return -1;
    m->bam_bits = (uint8_t *)malloc((size_t)m->bam_byte_count);
    if (!m->bam_bits) return -1;
    if (ndi_pread(m, NDI_SECTOR_SIZE, m->bam_bits, m->bam_byte_count) != m->bam_byte_count) return -1;
    m->free_count = 0;
    for (int i = 0; i < m->data_sector_count; i++) if (!ndi_bam_is_alloc(m, i)) m->free_count++;
    return 0;
}

/* ---- flush metadata (header + BAM + dir) on save --------------------------- */
static int ndi_flush_metadata(ndi_t *m) {
    uint8_t hdr[NDI_SECTOR_SIZE];
    /* re-read sector 0 to preserve label + reserved bytes, then patch fields */
    if (ndi_pread(m, 0, hdr, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return -1;
    wr_u32(&hdr[0x38], (uint32_t)m->free_count);            /* free_sector_count */
    if (ndi_pwrite(m, 0, hdr, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return -1;

    /* BAM, padded to its sector span */
    int bam_sectors = (int)(m->directory_start_sector - 1);
    int padded = bam_sectors * NDI_SECTOR_SIZE;
    uint8_t sec[NDI_SECTOR_SIZE];
    int written = 0;
    while (written < padded) {
        memset(sec, 0, sizeof sec);
        if (written < m->bam_byte_count) {
            int copy = m->bam_byte_count - written;
            if (copy > NDI_SECTOR_SIZE) copy = NDI_SECTOR_SIZE;
            memcpy(sec, m->bam_bits + written, copy);
        }
        if (ndi_pwrite(m, NDI_SECTOR_SIZE + written, sec, NDI_SECTOR_SIZE) != NDI_SECTOR_SIZE) return -1;
        written += NDI_SECTOR_SIZE;
    }
    if (!ndi_flush_dir_cache(m)) return -1;
    fflush(m->fp);
    return 0;
}

/* ---- public ndi API (mirrors ps_fio/src/ndi.h) ---------------------------- */
static int ndi_open(ndi_t *m, const char *host_path) {
    memset(m, 0, sizeof *m);
    m->dir_cached_sector = -1;
    m->fp = fopen(host_path, "r+b");
    if (!m->fp) m->fp = fopen(host_path, "rb");   /* read-only mount if not writable */
    if (!m->fp) return -1;
    if (ndi_read_header(m) != 0) { fclose(m->fp); m->fp = NULL; return -1; }
    if (ndi_read_bam(m) != 0)    { if (m->bam_bits) free(m->bam_bits); fclose(m->fp); m->fp = NULL; return -1; }
    m->dir_sector_count = (int)m->directory_sector_count;
    if (m->dir_sector_count <= 0) { free(m->bam_bits); fclose(m->fp); m->fp = NULL; return -1; }
    m->dir_entry_count = m->dir_sector_count * NDI_ENTRIES_PER_SEC;
    m->open = 1;
    return 0;
}
static void ndi_close(ndi_t *m) {
    if (!m->open) return;
    ndi_flush_dir_cache(m);
    if (m->fp) fclose(m->fp);
    if (m->bam_bits) free(m->bam_bits);
    memset(m, 0, sizeof *m);
    m->dir_cached_sector = -1;
}
static int ndi_find(ndi_t *m, const char *name, uint16_t parent) {
    ndi_entry_t e;
    for (int i = 0; i < m->dir_entry_count; i++) {
        ndi_dir_read_entry(m, i, &e);
        if (!(e.flags & FL_ACTIVE)) continue;
        if (e.parent_index != parent) continue;
        if (!ndi_name_eq_ci(e.filename, name)) continue;
        return i;
    }
    return -1;
}
static int ndi_get(ndi_t *m, int index, ndi_entry_t *out) {
    if (index < 0 || index >= m->dir_entry_count) return -1;
    ndi_dir_read_entry(m, index, out);
    return 0;
}
/* Read up to `len` bytes of file `index` starting at `file_offset`. Returns bytes
 * read, 0 at EOF, or -1 on error (mirrors read_file_chunk_by_index). */
static int ndi_read(ndi_t *m, int index, uint32_t file_offset, void *buf, uint32_t len) {
    ndi_entry_t e;
    if (ndi_get(m, index, &e) != 0) return -1;
    if (!(e.flags & FL_ACTIVE) || (e.flags & FL_DIRECTORY)) return -1;
    if (file_offset > e.size_bytes) return -1;
    uint32_t remaining = e.size_bytes - file_offset;
    if (len > remaining) len = remaining;
    if (len == 0) return 0;
    uint64_t off = ((uint64_t)m->data_start_sector + e.start_sector) * NDI_SECTOR_SIZE + file_offset;
    int n = ndi_pread(m, off, buf, (int)len);
    return (n == (int)len) ? n : -1;
}
static int ndi_create(ndi_t *m, const char *name, uint8_t type, uint16_t parent, uint32_t size) {
    int sector_count = (int)((size + NDI_SECTOR_SIZE - 1) / NDI_SECTOR_SIZE);
    if (sector_count == 0) sector_count = 1;
    int start = ndi_bam_alloc_contig(m, sector_count);
    if (start < 0) return -1;
    int slot = ndi_dir_find_free_slot(m);
    if (slot < 0) { ndi_bam_free(m, start, sector_count); return -1; }
    ndi_dir_write_entry(m, slot, FL_ACTIVE, type, parent, (uint32_t)start, size, name, (uint32_t)sector_count);
    if (ndi_flush_metadata(m) != 0) return -1;
    return slot;
}
static int ndi_write(ndi_t *m, int index, uint32_t file_offset, const void *buf, uint32_t len) {
    ndi_entry_t e;
    if (ndi_get(m, index, &e) != 0) return -1;
    if (!(e.flags & FL_ACTIVE) || (e.flags & FL_DIRECTORY)) return -1;
    if (file_offset > e.size_bytes) return -1;
    if (len > e.size_bytes - file_offset) return -1;
    if (len == 0) return 0;
    uint64_t off = ((uint64_t)m->data_start_sector + e.start_sector) * NDI_SECTOR_SIZE + file_offset;
    return (ndi_pwrite(m, off, buf, (int)len) == (int)len) ? 0 : -1;
}
static int ndi_zero_tail(ndi_t *m, int index) {
    ndi_entry_t e;
    if (ndi_get(m, index, &e) != 0) return -1;
    if (!(e.flags & FL_ACTIVE) || (e.flags & FL_DIRECTORY)) return -1;
    uint32_t allocated = e.sector_count * NDI_SECTOR_SIZE;
    if (allocated <= e.size_bytes) return 0;
    uint32_t pad = allocated - e.size_bytes;
    uint64_t off = ((uint64_t)m->data_start_sector + e.start_sector) * NDI_SECTOR_SIZE + e.size_bytes;
    uint8_t zero[NDI_SECTOR_SIZE];
    memset(zero, 0, sizeof zero);
    while (pad > 0) {
        uint32_t chunk = pad > NDI_SECTOR_SIZE ? NDI_SECTOR_SIZE : pad;
        if (ndi_pwrite(m, off, zero, (int)chunk) != (int)chunk) return -1;
        off += chunk; pad -= chunk;
    }
    fflush(m->fp);
    return 0;
}
static int ndi_delete(ndi_t *m, const char *name, uint16_t parent) {
    int idx = ndi_find(m, name, parent);
    if (idx < 0) return -1;
    ndi_entry_t e;
    ndi_dir_read_entry(m, idx, &e);
    if (e.flags & FL_DIRECTORY) return -1;
    ndi_bam_free(m, (int)e.start_sector, (int)e.sector_count);
    ndi_dir_clear_slot(m, idx);
    return ndi_flush_metadata(m);
}
static void ndi_flush(ndi_t *m) { ndi_flush_metadata(m); }

/* ===========================================================================
 *  DRIVE MOUNT TABLE — reads the SAME /data/nova/config/mounts.txt that
 *  nservers.c (the mgmt server) writes, so `nova drive mount` and the image
 *  resolution here agree. Format (one line per mounted slot):
 *      <slotname> <client-path>\n      e.g.  fd0 /games/zork.ndi
 *  The client-path is host-resolved as NOVA_FS_ROOT + path (sd_path() in
 *  nservers.c). Port of drives.c, except the SD "0:%s" prefix becomes the Linux
 *  NOVA_FS_ROOT prefix.
 *
 *  The mgmt server can re-write mounts.txt at runtime (mount/unmount). We re-stat
 *  the file each boot_image() call and reload when it changes, so a mount taken
 *  WITHOUT a reboot is still picked up. (Cheap: a stat per LOAD/FOPEN.)
 * =========================================================================== */
#include <sys/stat.h>
#include <sys/types.h>

#define DRIVE_SLOTS 6
static const char *SLOT_NAMES[DRIVE_SLOTS] = { "fd0", "fd1", "fd2", "fd3", "hd0", "hd1" };
static char  g_drive_path[DRIVE_SLOTS][160];
static time_t g_mounts_mtime = 0;
static int    g_mounts_loaded = 0;

#define MOUNTS_FILE NOVA_FS_ROOT "/config/boot.json"   /* refresh trigger; parsed via nbootcfg */

static int drive_slot_index(const char *name) {
    if (!name) return -1;
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (strcasecmp(name, SLOT_NAMES[i]) == 0) return i;
    return -1;
}
static const char *drive_slot_name(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? SLOT_NAMES[idx] : 0;
}
static const char *drive_path(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? g_drive_path[idx] : "";
}
static int drive_boot_slot(void) {
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (g_drive_path[i][0]) return i;       /* fd0..fd3 then hd0,hd1 */
    return -1;
}
static void drives_load(void) {
    /* mounts live in /config/boot.json (the ULX3S schema), read via nbootcfg. */
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (!bootcfg_mount_get(SLOT_NAMES[i], g_drive_path[i], sizeof g_drive_path[i]))
            g_drive_path[i][0] = 0;
    g_mounts_loaded = 1;
}
/* Reload the mount table if mounts.txt changed since the last read. */
static void drives_refresh(void) {
    struct stat st;
    if (stat(MOUNTS_FILE, &st) != 0) {
        if (!g_mounts_loaded || g_mounts_mtime != 0) { drives_load(); g_mounts_mtime = 0; }
        return;
    }
    if (!g_mounts_loaded || st.st_mtime != g_mounts_mtime) {
        drives_load();
        g_mounts_mtime = st.st_mtime;
    }
}

/* ===========================================================================
 *  MOUNTED BOOT IMAGE — port of main.c boot_image() / img_find().
 * =========================================================================== */
static ndi_t g_img;
static int   g_img_slot = -1;

/* Resolve + (re)open the boot drive's .ndi. Returns the open image, or NULL if
 * nothing is mounted / the image failed to open. Path "/x.ndi" (client form) is
 * host-resolved as NOVA_FS_ROOT + path (matching nservers.c's sd_path()). */
static ndi_t *boot_image(void) {
    drives_refresh();
    int slot = drive_boot_slot();
    if (slot < 0) {
        if (g_img_slot >= 0) { ndi_close(&g_img); g_img_slot = -1; }
        return 0;
    }
    /* Re-open if the boot slot changed OR its path changed under us (remount). */
    static char open_path[200];
    char full[200];
    const char *p = drive_path(slot);
    if (p[0] == '/') snprintf(full, sizeof full, "%s%s", NOVA_FS_ROOT, p);
    else             snprintf(full, sizeof full, "%s/%s", NOVA_FS_ROOT, p);

    if (slot != g_img_slot || strcmp(full, open_path) != 0) {
        if (g_img_slot >= 0) ndi_close(&g_img);
        if (ndi_open(&g_img, full) != 0) { g_img_slot = -1; return 0; }
        g_img_slot = slot;
        snprintf(open_path, sizeof open_path, "%s", full);
        printf("[fio] boot image %s = %s\n", drive_slot_name(slot), full);
    }
    return &g_img;
}

/* Resolve a 6502 filename inside the mounted image: exact (ci), then "+.bin". */
static int img_find(ndi_t *img, const char *name) {
    int idx = ndi_find(img, name, NDI_ROOT);
    if (idx < 0 && !strchr(name, '.')) {
        char tmp[40];
        snprintf(tmp, sizeof tmp, "%s.bin", name);
        idx = ndi_find(img, tmp, NDI_ROOT);
    }
    return idx;
}

/* ===========================================================================
 *  STAGING BUFFERS + FILE-HANDLE TABLE — port of main.c g_fbuf/g_fh[]/g_wbuf.
 * =========================================================================== */
static unsigned char g_fbuf[65536];     /* file <-> 6502-RAM / XRAM staging      */

#define FH_MAX 8
static struct {
    int      used;
    int      idx;            /* NDI dir index (read handles)                     */
    uint32_t pos;
    uint32_t size;
    int      writing;        /* FCREATE write handle: staged in g_wbuf til FCLOSE */
    uint8_t  wtype;          /* NDI file type for the committed entry             */
    uint16_t wparent;        /* NDI parent index for the committed entry          */
    char     wname[34];      /* committed entry name                              */
} g_fh[FH_MAX];

#define WBUF_BYTES (256 * 1024)
static unsigned char g_wbuf[WBUF_BYTES];
static int g_wbuf_handle = -1;

/* ---- name parsing (port of main.c fio_read_name / fio_name_type) ---------- */
static int fio_read_name(char *out, int maxlen) {
    int n = peek(FIO_NAMELEN);
    if (n < 1 || n > 63 || n >= maxlen) return -1;
    for (int i = 0; i < n; i++) out[i] = (char)peek(FIO_NAME + i);
    out[n] = 0;
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == 0)) out[--n] = 0;
    return n;
}
static uint8_t fio_name_type(const char *name) {
    const char *d = strrchr(name, '.');
    if (!d) return DT_BIN;
    if (!strncasecmp(d, ".bas", 5)) return DT_BAS;
    if (!strncasecmp(d, ".sid", 5)) return DT_SID;
    if (!strncasecmp(d, ".mid", 5)) return DT_MID;
    if (!strncasecmp(d, ".gfx", 5)) return DT_GFX;
    if (!strncasecmp(d, ".4th", 5)) return DT_FORTH;
    return DT_BIN;
}

/* ===========================================================================
 *  MOUNTED-IMAGE LOAD HOOK (for novavm.c's fio_load) — port of the image branch
 *  of main.c fio_load(). Loads the named file from the mounted image into 6502
 *  RAM and fills FIO_SRC/FIO_SIZE/FIO_DIRTYPE. Returns 0 on a successful image
 *  load, -1 if not mounted / not found. Caller owns fio_ok()/fio_fail().
 * =========================================================================== */
int nfio_image_load(const char *name) {
    ndi_t *img = boot_image();
    if (!img) return -1;
    int idx = img_find(img, name);
    if (idx < 0) return -1;
    ndi_entry_t e;
    ndi_get(img, idx, &e);
    int n = ndi_read(img, idx, 0, g_fbuf, sizeof g_fbuf);
    if (n < 2) return -1;
    int is_bin = (e.file_type == DT_BIN);
    unsigned dst = is_bin ? (g_fbuf[0] | (g_fbuf[1] << 8))
                          : (peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8));
    unsigned len = (unsigned)n - 2;
    if (dst + len > 0x10000) return -1;
    for (unsigned i = 0; i < len; i++) poke((dst + i) & 0xFFFF, g_fbuf[2 + i]);
    if (is_bin) { poke(FIO_SRC_LO, dst & 0xFF); poke(FIO_SRC_HI, (dst >> 8) & 0xFF); }
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
    poke(FIO_DIRTYPE, e.file_type);
    printf("[fio] LOAD(ndi) %s -> $%04x (%u bytes, type %d)\n", e.filename, dst, len, e.file_type);
    return 0;
}

/* ===========================================================================
 *  FIO HANDLERS — direct ports of the bare-metal main.c handlers. Same FIO
 *  mailbox parsing (identical peek() addresses), FatFs/Xil adapted per nfio.h.
 * =========================================================================== */

/* FIO_CMD_LOADRUNTIME (0x28): stream a 16KB runtime ROM (e.g. NOVAZ.BIN) from the
 * mounted image into the $C000 primary bank (R_ROMW idx=0). */
void fio_load_runtime(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    if (e.size_bytes != 16384) {
        printf("[fio] LOADRUNTIME %s wrong size %u\n", e.filename, (unsigned)e.size_bytes);
        fio_fail(FIO_ERR_IO); return;
    }
    if (ndi_read(img, idx, 0, g_fbuf, 16384) != 16384) { fio_fail(FIO_ERR_IO); return; }
    for (unsigned a = 0; a < 16384; a++)
        wr(R_ROMW, (a << 8) | g_fbuf[a]);             /* idx=0 (bit22=0) = primary $C000 bank */
    poke(FIO_SIZE_LO, 0x00); poke(FIO_SIZE_HI, 0x40); /* 16384 */
    printf("[fio] LOADRUNTIME %s -> $C000 primary bank (16384 bytes)\n", e.filename);
    fio_ok();
}

/* FIO_CMD_XLOAD (0x18): stream a file from the mounted image straight into XRAM.
 * XRAM dest in FIO_GSPACE/GADDR; optional length in FIO_GLEN (0 = whole file). */
void fio_xload(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    if (!g_xram) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    unsigned xaddr  = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
    unsigned reqlen = peek(FIO_GLEN_LO)  | (peek(FIO_GLEN_HI) << 8);
    unsigned total  = e.size_bytes;
    if (reqlen > 0 && reqlen < total) total = reqlen;

    unsigned done = 0;
    while (done < total) {
        unsigned chunk = total - done;
        if (chunk > sizeof g_fbuf) chunk = sizeof g_fbuf;
        int n = ndi_read(img, idx, done, g_fbuf, chunk);
        if (n <= 0) break;
        if (xaddr + done + (unsigned)n > XRAM_BYTES) break;    /* clamp to the mapped window */
        xram_write(xaddr + done, g_fbuf, (unsigned)n);
        done += (unsigned)n;
        if ((unsigned)n < chunk) break;
    }
    poke(FIO_SIZE_LO, done & 0xFF); poke(FIO_SIZE_HI, (done >> 8) & 0xFF); poke(FIO_SIZE2, (done >> 16) & 0xFF);
    printf("[fio] XLOAD %s -> XRAM $%06x (%u bytes)\n", e.filename, xaddr, done);
    fio_ok();
}

/* FIO_CMD_XPAGE (0x29): stream a SLICE (file offset + length) of a mounted file
 * into XRAM or CPU RAM. File offset = FIO_SRC | FIO_END_LO<<16; length = FIO_GLEN;
 * target = FIO_DIRTYPE (0=XRAM @ FIO_GSPACE/GADDR, 1=CPU RAM @ FIO_GADDR). */
void fio_xpage(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);

    unsigned char target = peek(FIO_DIRTYPE);
    unsigned reqlen = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    unsigned foff   = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8) | (peek(FIO_END_LO) << 16);
    if (reqlen == 0 || foff >= e.size_bytes) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = reqlen;
    if (len > e.size_bytes - foff) len = e.size_bytes - foff;

    if (target == FIO_PAGE_XRAM) {
        if (!g_xram) { fio_fail(FIO_ERR_IO); return; }
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        unsigned done = 0;
        while (done < len) {
            unsigned chunk = len - done;
            if (chunk > sizeof g_fbuf) chunk = sizeof g_fbuf;
            int n = ndi_read(img, idx, foff + done, g_fbuf, chunk);
            if (n <= 0) break;
            if (xaddr + done + (unsigned)n > XRAM_BYTES) break;
            xram_write(xaddr + done, g_fbuf, (unsigned)n);
            done += (unsigned)n;
            if ((unsigned)n < chunk) break;
        }
        len = done;
    } else if (target == FIO_PAGE_RAM) {
        unsigned addr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8);
        if (len > sizeof g_fbuf) len = sizeof g_fbuf;
        int n = ndi_read(img, idx, foff, g_fbuf, len);
        if (n < 0) { fio_fail(FIO_ERR_IO); return; }
        for (int i = 0; i < n; i++) poke((addr + i) & 0xFFFF, g_fbuf[i]);
        len = (unsigned)n;
    } else {
        fio_fail(FIO_ERR_IO); return;          /* VGC / gfx4 picture targets = workstream C */
    }
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF); poke(FIO_SIZE2, (len >> 16) & 0xFF);
    fio_ok();
}

/* FIO_CMD_FOPEN (0x2D): open an image file -> read handle (slot+1). */
void fio_fopen(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    int idx = img_find(img, name);
    if (idx < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    ndi_entry_t e; ndi_get(img, idx, &e);
    int h = -1;
    for (int i = 0; i < FH_MAX; i++) if (!g_fh[i].used) { h = i; break; }
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    g_fh[h].used = 1; g_fh[h].idx = idx; g_fh[h].pos = 0; g_fh[h].size = e.size_bytes;
    g_fh[h].writing = 0;
    poke(FIO_SRC_LO, (h + 1) & 0xFF); poke(FIO_SRC_HI, 0);   /* handle = slot+1 */
    poke(FIO_SIZE_LO, e.size_bytes & 0xFF); poke(FIO_SIZE_HI, (e.size_bytes >> 8) & 0xFF); poke(FIO_SIZE2, (e.size_bytes >> 16) & 0xFF);
    printf("[fio] FOPEN %s -> h%d (%u bytes)\n", e.filename, h + 1, (unsigned)e.size_bytes);
    fio_ok();
}

static int fh_from_regs(void) {
    int handle = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8);
    int h = handle - 1;
    if (h < 0 || h >= FH_MAX || !g_fh[h].used) return -1;
    return h;
}

/* FIO_CMD_FCREATE (0x2E): open a write handle. Data is staged in g_wbuf during
 * FWRITE; the NDI entry is created + filled on FCLOSE (final size not known up
 * front). One writer at a time. */
void fio_fcreate(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    if (g_wbuf_handle >= 0) { fio_fail(FIO_ERR_IO); return; }   /* one writer at a time */
    int h = -1;
    for (int i = 0; i < FH_MAX; i++) if (!g_fh[i].used) { h = i; break; }
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    g_fh[h].used = 1; g_fh[h].idx = -1; g_fh[h].pos = 0; g_fh[h].size = 0;
    g_fh[h].writing = 1; g_fh[h].wtype = fio_name_type(name); g_fh[h].wparent = NDI_ROOT;
    /* NDI names are capped at NDI_MAX_NAME (32) chars; copy bounded + NUL-term. */
    {
        size_t nl = strlen(name);
        if (nl > sizeof g_fh[h].wname - 1) nl = sizeof g_fh[h].wname - 1;
        memcpy(g_fh[h].wname, name, nl);
        g_fh[h].wname[nl] = 0;
    }
    g_wbuf_handle = h;
    poke(FIO_SRC_LO, (h + 1) & 0xFF); poke(FIO_SRC_HI, 0);      /* handle = slot+1 */
    poke(FIO_SIZE_LO, 0); poke(FIO_SIZE_HI, 0); poke(FIO_SIZE2, 0);
    printf("[fio] FCREATE %s -> h%d (write)\n", name, h + 1);
    fio_ok();
}

/* FIO_CMD_FWRITE (0x31): append FIO_GLEN bytes from CPU RAM (FIO_END) or XRAM
 * (FIO_GADDR/GSPACE) to the write handle's staging buffer. */
void fio_fwrite(void) {
    int h = fh_from_regs();
    if (h < 0 || !g_fh[h].writing) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    if (len == 0) { poke(FIO_SIZE_LO, 0); poke(FIO_SIZE_HI, 0); poke(FIO_SIZE2, 0); fio_ok(); return; }
    if (g_fh[h].pos > WBUF_BYTES || len > WBUF_BYTES - g_fh[h].pos) { fio_fail(FIO_ERR_IO); return; }
    unsigned char target = peek(FIO_DIRTYPE);
    if ((target & FIO_TARGET_MASK) == FIO_TARGET_XRAM) {
        if (!g_xram) { fio_fail(FIO_ERR_IO); return; }
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        if (xaddr + len > XRAM_BYTES) { fio_fail(FIO_ERR_IO); return; }
        xram_read(xaddr, g_wbuf + g_fh[h].pos, len);
    } else {
        unsigned src = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
        for (unsigned i = 0; i < len; i++) g_wbuf[g_fh[h].pos + i] = peek((src + i) & 0xFFFF);
    }
    g_fh[h].pos += len;
    if (g_fh[h].pos > g_fh[h].size) g_fh[h].size = g_fh[h].pos;
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF); poke(FIO_SIZE2, (len >> 16) & 0xFF);
    fio_ok();
}

/* Commit a write handle's staged bytes into the mounted .ndi (replace if the
 * name already exists). Returns 0 on success. Port of main.c fio_commit_write. */
static int fio_commit_write(int h) {
    ndi_t *img = boot_image();
    if (!img) return -1;
    uint32_t total = g_fh[h].size;
    ndi_delete(img, g_fh[h].wname, g_fh[h].wparent);           /* overwrite existing slot */
    int idx = ndi_create(img, g_fh[h].wname, g_fh[h].wtype, g_fh[h].wparent, total ? total : 1);
    if (idx < 0) return -1;
    uint32_t off = 0;
    while (off < total) {
        uint32_t chunk = total - off; if (chunk > 16384) chunk = 16384;
        if (ndi_write(img, idx, off, g_wbuf + off, chunk) != 0) {
            ndi_delete(img, g_fh[h].wname, g_fh[h].wparent); return -1;
        }
        off += chunk;
    }
    ndi_zero_tail(img, idx);
    ndi_flush(img);
    return 0;
}

/* FIO_CMD_FCLOSE (0x2F): close a handle (commit staged data if it's a writer). */
void fio_fclose(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    if (g_fh[h].writing) {
        int rc = fio_commit_write(h);
        char saved[34]; snprintf(saved, sizeof saved, "%s", g_fh[h].wname);
        unsigned sz = g_fh[h].size;
        g_fh[h].used = 0; g_fh[h].writing = 0; g_wbuf_handle = -1;
        if (rc == 0) { printf("[fio] FCLOSE(save) %s (%u bytes)\n", saved, sz); fio_ok(); }
        else         { printf("[fio] FCLOSE(save) %s FAILED\n", saved); fio_fail(FIO_ERR_IO); }
        return;
    }
    g_fh[h].used = 0;
    fio_ok();
}

/* FIO_CMD_FREAD (0x30): read a chunk -> CPU RAM (FIO_END) or XRAM (FIO_GADDR). */
void fio_fread(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    ndi_t *img = boot_image();
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    unsigned want = peek(FIO_GLEN_LO) | (peek(FIO_GLEN_HI) << 8);
    if (want == 0 || want > sizeof g_fbuf) want = sizeof g_fbuf;
    int n = ndi_read(img, g_fh[h].idx, g_fh[h].pos, g_fbuf, want);
    if (n < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned char dirtype = peek(FIO_DIRTYPE);
    if ((dirtype & FIO_TARGET_MASK) == FIO_TARGET_XRAM) {
        if (!g_xram) { fio_fail(FIO_ERR_IO); return; }
        unsigned xaddr = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8) | (peek(FIO_GSPACE) << 16);
        if (xaddr + (unsigned)n > XRAM_BYTES) { fio_fail(FIO_ERR_IO); return; }
        xram_write(xaddr, g_fbuf, (unsigned)n);
    } else {
        unsigned dst = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
        for (int i = 0; i < n; i++) poke((dst + i) & 0xFFFF, g_fbuf[i]);
    }
    g_fh[h].pos += (unsigned)n;
    poke(FIO_SIZE_LO, n & 0xFF); poke(FIO_SIZE_HI, (n >> 8) & 0xFF); poke(FIO_SIZE2, (n >> 16) & 0xFF);
    fio_ok();
}

/* FIO_CMD_FSEEK (0x32): set the handle position (clamped to size). */
void fio_fseek(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned off = peek(FIO_SIZE_LO) | (peek(FIO_SIZE_HI) << 8) | (peek(FIO_SIZE2) << 16);
    if (off > g_fh[h].size) off = g_fh[h].size;
    g_fh[h].pos = off;
    fio_ok();
}

/* FIO_CMD_FTELL (0x33): report the handle position. */
void fio_ftell(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned p = g_fh[h].pos;
    poke(FIO_SIZE_LO, p & 0xFF); poke(FIO_SIZE_HI, (p >> 8) & 0xFF); poke(FIO_SIZE2, (p >> 16) & 0xFF);
    fio_ok();
}

/* FIO_CMD_FSIZE (0x34): report the handle size. */
void fio_fsize(void) {
    int h = fh_from_regs();
    if (h < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned s = g_fh[h].size;
    poke(FIO_SIZE_LO, s & 0xFF); poke(FIO_SIZE_HI, (s >> 8) & 0xFF); poke(FIO_SIZE2, (s >> 16) & 0xFF);
    fio_ok();
}

/* ===========================================================================
 *  STARTUP STATE
 * =========================================================================== */
void nfio_init(void) {
    for (int i = 0; i < FH_MAX; i++) g_fh[i].used = 0;
    g_wbuf_handle = -1;
    g_img_slot = -1;
    g_mounts_loaded = 0;
    g_mounts_mtime = 0;
    drives_load();      /* prime the mount table (boot_image() refreshes on change) */
}
