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
 * (FIO_CMD_LOAD 0x02 stays in novavm.c — mounted NDI disks only.)
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

#ifndef FIO_ERR_DISKFULL
#define FIO_ERR_DISKFULL   4
#endif

/* NDI / 6502 file types (wire values) */
#define DT_BAS 0
#define DT_SID 1
#define DT_BIN 2
#define DT_MID 3
#define DT_GFX 4
#define NDI_DT_DIR 5
#define DT_FORTH 6
#define DT_LOGO 7
#define DT_PASCAL 8
#define DT_ASM 9

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
    if (ndi_flush_metadata(m) != 0) {
        ndi_bam_free(m, start, sector_count);
        ndi_dir_clear_slot(m, slot);
        ndi_flush_metadata(m);
        return -1;
    }
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
static long   g_mounts_mtime_nsec = -1;
static ino_t  g_mounts_ino = 0;
static off_t  g_mounts_size = -1;
static int    g_mounts_signature_valid = 0;
static int    g_mounts_loaded = 0;

#define MOUNTS_FILE NOVA_FS_ROOT "/config/boot.json"   /* refresh trigger; parsed via nbootcfg */

static long stat_mtime_nsec(const struct stat *st) {
    return st->st_mtim.tv_nsec;
}

static int mount_signature_changed(const struct stat *st) {
    return !g_mounts_signature_valid ||
           st->st_mtime != g_mounts_mtime ||
           stat_mtime_nsec(st) != g_mounts_mtime_nsec ||
           st->st_ino != g_mounts_ino ||
           st->st_size != g_mounts_size;
}

static void mount_signature_store(const struct stat *st) {
    g_mounts_mtime = st->st_mtime;
    g_mounts_mtime_nsec = stat_mtime_nsec(st);
    g_mounts_ino = st->st_ino;
    g_mounts_size = st->st_size;
    g_mounts_signature_valid = 1;
}

static void mount_signature_clear(void) {
    g_mounts_mtime = 0;
    g_mounts_mtime_nsec = -1;
    g_mounts_ino = 0;
    g_mounts_size = -1;
    g_mounts_signature_valid = 0;
}

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
        if (!g_mounts_loaded || g_mounts_signature_valid) { drives_load(); mount_signature_clear(); }
        return;
    }
    if (!g_mounts_loaded || mount_signature_changed(&st)) {
        drives_load();
        mount_signature_store(&st);
    }
}

/* ===========================================================================
 *  MOUNTED IMAGES + DRIVE-AWARE FILESYSTEM
 *  A file is (name + type) — no extensions; the type byte lives in the .ndi dir
 *  entry. The CWD is a drive (+ optional subdir); a "fd0:" prefix overrides it
 *  for one op. SAVE/LOAD/DIR/DELETE/CD/PWD route through here when a disk is
 *  mounted (port of main.c boot_image()/img_find(), generalised to any slot).
 * =========================================================================== */

static ndi_t g_img;
static int   g_img_slot = -1;        /* slot whose .ndi is open in g_img         */
static char  g_img_open_path[200] = "";
static dev_t g_img_dev = 0;
static ino_t g_img_ino = 0;
static off_t g_img_size = -1;
static time_t g_img_mtime = 0;
static long  g_img_mtime_nsec = -1;
static int   g_img_signature_valid = 0;
static int   g_cwd_slot = -1;        /* explicit CD target; -1 = follow boot slot */
static char  g_cwd_dir[160] = "";    /* subdir within the CWD drive ("" = root)  */

static int image_signature_matches(const struct stat *st) {
    return g_img_signature_valid &&
           st->st_dev == g_img_dev &&
           st->st_ino == g_img_ino &&
           st->st_size == g_img_size &&
           st->st_mtime == g_img_mtime &&
           stat_mtime_nsec(st) == g_img_mtime_nsec;
}

static void image_signature_store(const struct stat *st) {
    g_img_dev = st->st_dev;
    g_img_ino = st->st_ino;
    g_img_size = st->st_size;
    g_img_mtime = st->st_mtime;
    g_img_mtime_nsec = stat_mtime_nsec(st);
    g_img_signature_valid = 1;
}

static void close_slot_image(void) {
    if (g_img_slot >= 0) ndi_close(&g_img);
    g_img_slot = -1;
    g_img_open_path[0] = 0;
    g_img_signature_valid = 0;
}

/* Open a given slot's .ndi into the single cached image (re-open on slot/path
 * change). NULL if the slot is empty / the image won't open. */
static ndi_t *slot_image(int slot) {
    if (slot < 0 || slot >= DRIVE_SLOTS || !drive_path(slot)[0]) {
        close_slot_image();
        return 0;
    }
    char full[200];
    const char *p = drive_path(slot);
    if (p[0] == '/') snprintf(full, sizeof full, "%s%s", NOVA_FS_ROOT, p);
    else             snprintf(full, sizeof full, "%s/%s", NOVA_FS_ROOT, p);
    struct stat st;
    if (stat(full, &st) != 0) {
        close_slot_image();
        return 0;
    }
    if (slot != g_img_slot || strcmp(full, g_img_open_path) != 0 || !image_signature_matches(&st)) {
        close_slot_image();
        if (ndi_open(&g_img, full) != 0) { g_img_slot = -1; return 0; }
        g_img_slot = slot;
        snprintf(g_img_open_path, sizeof g_img_open_path, "%s", full);
        image_signature_store(&st);
        printf("[fio] image %s = %s\n", drive_slot_name(slot), full);
    }
    return &g_img;
}

/* The boot drive = first non-empty slot (fd0..hd1). */
static ndi_t *boot_image(void) {
    drives_refresh();
    return slot_image(drive_boot_slot());
}

/* The slot whose disk is "current": explicit CD target, else the boot drive. */
static int nfio_cwd_slot(void) {
    drives_refresh();
    if (g_cwd_slot >= 0 && drive_path(g_cwd_slot)[0]) return g_cwd_slot;
    return drive_boot_slot();
}

static int ndi_walk(ndi_t *m, const char *path, int create,
                    uint16_t *parent_out, char *fname, int fnsz);  /* fwd decl */

/* Resolve a 6502 filename inside the mounted image. A name with '/' is a path
 * (e.g. forth/lib/core.4th) -> walk the real subdirectories; a bare name is
 * matched at the root, exact then "+.bin" (RUNTIME/AUTOBOOT). */
static int img_find(ndi_t *img, const char *name) {
    if (strchr(name, '/')) {
        uint16_t parent; char fname[NDI_MAX_NAME + 1];
        if (ndi_walk(img, name, 0, &parent, fname, sizeof fname) != 0) return -1;
        return ndi_find(img, fname, parent);
    }
    int idx = ndi_find(img, name, NDI_ROOT);
    if (idx < 0 && !strchr(name, '.')) {
        char tmp[40];
        snprintf(tmp, sizeof tmp, "%s.bin", name);
        idx = ndi_find(img, tmp, NDI_ROOT);
    }
    return idx;
}

/* Read a named file from the currently-mounted (boot-drive) NDI into buf. Returns
 * the byte count, or -1 (no image / not found / too big). Lets a program's own
 * disk image carry its assets (e.g. a MIDI) instead of a shared host directory. */
int nfio_disk_read(const char *name, uint8_t *buf, int maxlen) {
    ndi_t *img = boot_image();
    if (!img) return -1;
    int idx = img_find(img, name);
    if (idx < 0) return -1;
    ndi_entry_t e;
    if (ndi_get(img, idx, &e) != 0) return -1;
    if (e.size_bytes > (uint32_t)maxlen) return -1;
    return ndi_read(img, idx, 0, buf, e.size_bytes);
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
    if (!strncasecmp(d, ".pas", 5)) return DT_PASCAL;
    if (!strncasecmp(d, ".sid", 5)) return DT_SID;
    if (!strncasecmp(d, ".mid", 5)) return DT_MID;
    if (!strncasecmp(d, ".gfx", 5)) return DT_GFX;
    if (!strncasecmp(d, ".4th", 5)) return DT_FORTH;
    if (!strncasecmp(d, ".logo", 6)) return DT_LOGO;
    if (!strncasecmp(d, ".lgo", 5)) return DT_LOGO;
    if (!strncasecmp(d, ".s", 3)) return DT_ASM;
    if (!strncasecmp(d, ".asm", 5)) return DT_ASM;
    if (!strncasecmp(d, ".inc", 5)) return DT_ASM;
    return DT_BIN;
}

/* Read directory entry `idx` from `img` into 6502 RAM and set FIO_SRC/SIZE/
 * DIRTYPE — the shared tail of every .ndi -> RAM load. The image and disk load
 * paths differ ONLY in how they resolve idx; this is everything after that. A
 * DT_BIN file's 2-byte load-address header picks the destination, any other type
 * lands at the caller's FIO_SRC. 0 ok, -1 if a directory / unreadable. */
static int ndi_emit_to_ram(ndi_t *img, int idx) {
    ndi_entry_t e;
    ndi_get(img, idx, &e);
    if (e.flags & FL_DIRECTORY) return -1;
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
 *  MOUNTED-IMAGE LOAD HOOK (for novavm.c's fio_load) — port of the image branch
 *  of main.c fio_load(). Resolves `name` at the boot drive's root (+.bin) and
 *  loads it. Returns 0 on a hit, -1 if not mounted / not found. Caller owns the
 *  fio_ok()/fio_fail() result.
 * =========================================================================== */
int nfio_image_load(const char *name) {
    ndi_t *img = boot_image();
    if (!img) return -1;
    int idx = img_find(img, name);
    if (idx < 0) return -1;
    return ndi_emit_to_ram(img, idx);
}

/* ===========================================================================
 *  DRIVE-AWARE SAVE / LOAD / DIR / DELETE / CD / PWD — the user filesystem.
 *  User-visible files live only on mounted NDI disks.
 * =========================================================================== */

/* Create a directory entry (no data sectors). Returns its slot, or -1. */
static int ndi_mkdir(ndi_t *m, const char *name, uint16_t parent) {
    int slot = ndi_dir_find_free_slot(m);
    if (slot < 0) return -1;
    ndi_dir_write_entry(m, slot, FL_ACTIVE | FL_DIRECTORY, NDI_DT_DIR, parent, 0, 0, name, 0);
    if (ndi_flush_metadata(m) != 0) return -1;
    return slot;
}

/* Walk "a/b/file" from root; copy the final component into fname and return its
 * parent dir index. mkdir missing dirs when `create`. 0 ok, -1 error. */
static int ndi_walk(ndi_t *m, const char *path, int create,
                    uint16_t *parent_out, char *fname, int fnsz) {
    uint16_t parent = NDI_ROOT;
    const char *p = path;
    while (*p == '/') p++;
    for (;;) {
        const char *slash = strchr(p, '/');
        if (!slash) {
            if (!*p) return -1;
            snprintf(fname, fnsz, "%s", p);
            *parent_out = parent;
            return 0;
        }
        int cl = (int)(slash - p);
        if (cl <= 0 || cl >= NDI_MAX_NAME) return -1;
        char comp[NDI_MAX_NAME + 1];
        memcpy(comp, p, cl); comp[cl] = 0;
        int idx = ndi_find(m, comp, parent);
        if (idx < 0) {
            if (!create) return -1;
            idx = ndi_mkdir(m, comp, parent);
            if (idx < 0) return -1;
        } else {
            ndi_entry_t e; ndi_dir_read_entry(m, idx, &e);
            if (!(e.flags & FL_DIRECTORY)) return -1;   /* a file blocks the path */
        }
        parent = (uint16_t)idx;
        p = slash + 1;
        while (*p == '/') p++;
    }
}

/* Collapse "a/b/.." -> "a", drop "." and empty components, no surrounding '/'. */
static void ndi_path_normalize(char *path) {
    char stack[32][NDI_MAX_NAME + 1]; int sp = 0;
    const char *p = path;
    while (*p) {
        while (*p == '/') p++;
        if (!*p) break;
        const char *s = p;
        while (*p && *p != '/') p++;
        int cl = (int)(p - s); if (cl > NDI_MAX_NAME) cl = NDI_MAX_NAME;
        char comp[NDI_MAX_NAME + 1]; memcpy(comp, s, cl); comp[cl] = 0;
        if (!strcmp(comp, ".")) continue;
        if (!strcmp(comp, "..")) { if (sp > 0) sp--; continue; }
        if (sp < 32) { snprintf(stack[sp], sizeof stack[sp], "%s", comp); sp++; }
    }
    char out[160]; int ol = 0;
    for (int i = 0; i < sp; i++) {
        if (i && ol < (int)sizeof out - 1) out[ol++] = '/';
        for (const char *c = stack[i]; *c && ol < (int)sizeof out - 1; c++) out[ol++] = *c;
    }
    out[ol] = 0;
    snprintf(path, 160, "%s", out);
}

/* Resolve a 6502 name ("fd0:dir/file" | "dir/file" | "file") to a drive slot +
 * an in-image path. Relative names hang off the CWD subdir. slot>=0, or -1. */
static int nfio_resolve(const char *in, char *path, int psz) {
    const char *colon = strchr(in, ':');
    if (colon) {
        int dl = (int)(colon - in);
        if (dl <= 0 || dl > 8) return -1;
        char drv[12]; memcpy(drv, in, dl); drv[dl] = 0;
        int slot = drive_slot_index(drv);
        if (slot < 0) return -1;
        const char *rest = colon + 1;
        while (*rest == '/') rest++;
        snprintf(path, psz, "%s", rest);
        return slot;
    }
    int slot = nfio_cwd_slot();
    if (slot < 0) return -1;
    if (g_cwd_dir[0]) snprintf(path, psz, "%s/%s", g_cwd_dir, in);
    else              snprintf(path, psz, "%s", in);
    return slot;
}

/* Resolve a path that NAMES a directory -> its dir index (root for empty). -1 err. */
static int nfio_resolve_dir(ndi_t *img, const char *path, uint16_t *parent_out) {
    const char *pp = path; while (*pp == '/') pp++;
    if (!*pp) { *parent_out = NDI_ROOT; return 0; }
    char dpath[210]; snprintf(dpath, sizeof dpath, "%s/.", pp);  /* descend into last comp */
    char dummy[NDI_MAX_NAME + 1];
    return ndi_walk(img, dpath, 0, parent_out, dummy, sizeof dummy);
}

/* Is the current working drive a mounted disk? (Routing gate for novavm.c.) */
int nfio_disk_active(void) { return nfio_cwd_slot() >= 0; }

/* Stage the 6502 RAM range [FIO_SRC, FIO_END) into `buf` as a save image:
 * buf[0..1] = the load address (lo,hi), buf[2..] = the bytes. Returns the payload
 * length (the image is len+2 bytes), or -1 if the range is empty / won't fit.
 * Shared by the local fio_save and the .ndi nfio_disk_save (one 2-byte-header
 * staging path, two stores). */
int nfio_stage_save(unsigned char *buf, unsigned bufsz) {
    unsigned src = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8);
    unsigned end = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
    if (end <= src || (unsigned)(end - src) + 2 > bufsz) return -1;
    unsigned len = end - src;
    buf[0] = src & 0xFF; buf[1] = (src >> 8) & 0xFF;
    for (unsigned i = 0; i < len; i++) buf[2 + i] = peek((src + i) & 0xFFFF);
    return (int)len;
}

/* SAVE: store the 6502 RAM range as (name + type) on the resolved drive/subdir.
 * Type comes from FIO_DIRTYPE (the runtime stamps FORTH/LOGO/...). */
void nfio_disk_save(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    int len = nfio_stage_save(g_wbuf, WBUF_BYTES);
    if (len < 0) { fio_fail(FIO_ERR_IO); return; }
    uint8_t type = peek(FIO_DIRTYPE);
    if (ndi_find(img, fname, parent) >= 0) ndi_delete(img, fname, parent);  /* overwrite */
    int idx = ndi_create(img, fname, type, parent, (uint32_t)(len + 2));
    if (idx < 0) { fio_fail(FIO_ERR_DISKFULL); return; }
    if (ndi_write(img, idx, 0, g_wbuf, (uint32_t)(len + 2)) != 0) {
        ndi_delete(img, fname, parent);
        fio_fail(FIO_ERR_IO);
        return;
    }
    if (ndi_zero_tail(img, idx) != 0) {
        ndi_delete(img, fname, parent);
        fio_fail(FIO_ERR_IO);
        return;
    }
    ndi_flush(img);
    printf("[fio] SAVE(ndi) %s:%s type %d (%d bytes)\n", drive_slot_name(slot), path, type, len);
    fio_ok();
}

/* LOAD: resolve drive/subdir, read into 6502 RAM, set FIO_SRC/SIZE/DIRTYPE.
 * Returns 0 on hit, -1 not-found (caller falls through). */
int nfio_disk_load(const char *name) {
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) return -1;
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) return -1;
    int idx = ndi_find(img, fname, parent);
    if (idx < 0) return -1;
    return ndi_emit_to_ram(img, idx);   /* shared load tail (see nfio_image_load) */
}

/* ---- DIR: iterate one directory's entries ------------------------------- */
static ndi_t  *g_ddir_img    = NULL;
static uint16_t g_ddir_parent = NDI_ROOT;
static int      g_ddir_idx    = 0;

/* DIROPEN: bare -> the CWD dir; else the named drive:dir. */
void nfio_disk_diropen(void) {
    char name[80];
    int nl = fio_read_name(name, sizeof name);
    char path[200]; int slot;
    if (nl <= 0) { slot = nfio_cwd_slot(); snprintf(path, sizeof path, "%s", g_cwd_dir); }
    else         { slot = nfio_resolve(name, path, sizeof path); }
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t parent;
    if (nfio_resolve_dir(img, path, &parent) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    g_ddir_img = img; g_ddir_parent = parent; g_ddir_idx = 0;
    fio_ok();
}

/* DIRREAD: next entry -> FIO_NAME/NAMELEN, FIO_DIRTYPE, FIO_SIZE. EOD at end. */
void nfio_disk_dirread(void) {
    if (!g_ddir_img) { fio_fail(FIO_ERR_EOD); return; }
    ndi_entry_t e;
    while (g_ddir_idx < g_ddir_img->dir_entry_count) {
        int i = g_ddir_idx++;
        ndi_dir_read_entry(g_ddir_img, i, &e);
        if (!(e.flags & FL_ACTIVE)) continue;
        if (e.parent_index != g_ddir_parent) continue;
        int nl = 0; while (e.filename[nl] && nl < 63) nl++;
        poke(FIO_DIRTYPE, (e.flags & FL_DIRECTORY) ? NDI_DT_DIR : e.file_type);
        poke(FIO_NAMELEN, nl);
        for (int k = 0; k < nl; k++) poke(FIO_NAME + k, e.filename[k]);
        uint32_t display_size = e.size_bytes;
        if (!(e.flags & FL_DIRECTORY) &&
            (e.file_type == DT_BAS || e.file_type == DT_BIN) &&
            display_size >= 2) {
            display_size -= 2;
        }
        poke(FIO_SIZE_LO, display_size & 0xFF);
        poke(FIO_SIZE_HI, (display_size >> 8) & 0xFF);
        poke(FIO_SIZE2, (display_size >> 16) & 0xFF);
        fio_ok();
        return;
    }
    g_ddir_img = NULL;
    fio_fail(FIO_ERR_EOD);
}

/* DELETE: resolve + remove a file (not a directory). */
void nfio_disk_delete(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    if (ndi_delete(img, fname, parent) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    fio_ok();
}

/* CD "fd0:" | "fd0:dir" | "dir" | ".." | "/" — updates g_cwd_slot/g_cwd_dir. */
void nfio_cd(void) {
    char name[80];
    int nl = fio_read_name(name, sizeof name);
    if (nl <= 0) { fio_ok(); return; }       /* bare CD = stay */
    char buf[120]; snprintf(buf, sizeof buf, "%s", name);
    int slot = nfio_cwd_slot();
    char newdir[160];
    char *colon = strchr(buf, ':');
    if (colon) {
        *colon = 0;
        int s = drive_slot_index(buf);
        if (s < 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
        slot = s;
        char *rest = colon + 1; while (*rest == '/') rest++;
        snprintf(newdir, sizeof newdir, "%s", rest);          /* absolute on that drive */
    } else if (buf[0] == '/') {
        char *rest = buf; while (*rest == '/') rest++;
        snprintf(newdir, sizeof newdir, "%s", rest);          /* absolute on current drive */
    } else if (g_cwd_dir[0]) {
        size_t cwd_len = strlen(g_cwd_dir);
        size_t buf_len = strlen(buf);
        if (cwd_len + 1 + buf_len >= sizeof newdir) { fio_fail(FIO_ERR_IO); return; }
        memcpy(newdir, g_cwd_dir, cwd_len);
        newdir[cwd_len] = '/';
        memcpy(newdir + cwd_len + 1, buf, buf_len + 1);       /* relative */
    } else {
        snprintf(newdir, sizeof newdir, "%s", buf);
    }
    ndi_path_normalize(newdir);
    ndi_t *img = slot_image(slot);
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t pr;
    if (nfio_resolve_dir(img, newdir, &pr) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    g_cwd_slot = slot;
    snprintf(g_cwd_dir, sizeof g_cwd_dir, "%s", newdir);
    fio_ok();
}

/* PWD: write "fd0:dir/sub" into FIO_NAME / FIO_NAMELEN. */
void nfio_pwd(void) {
    int slot = nfio_cwd_slot();
    char out[200];
    const char *dn = (slot >= 0) ? drive_slot_name(slot) : "??";
    if (g_cwd_dir[0]) snprintf(out, sizeof out, "%s:%s", dn, g_cwd_dir);
    else              snprintf(out, sizeof out, "%s:", dn);
    int n = (int)strlen(out); if (n > 63) n = 63;
    poke(FIO_NAMELEN, n);
    for (int i = 0; i < n; i++) poke(FIO_NAME + i, out[i]);
    fio_ok();
}

void nfio_devstatus(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    char *colon = strchr(name, ':');
    if (colon) *colon = 0;
    int slot = drive_slot_index(name);
    if (slot < 0) { fio_fail(FIO_ERR_IO); return; }
    drives_refresh();
    if (!drive_path(slot)[0] || !slot_image(slot)) {
        fio_fail(FIO_ERR_NOTMOUNTED);
        return;
    }
    fio_ok();
}

/* ===========================================================================
 *  MKDIR / RMDIR + GSAVE / GLOAD — directory create/remove and VGC graphics
 *  memory save/load. Ports of the desktop FileIoController DoMkdir / DoRmdir /
 *  DoGSave / DoGLoad (e6502.Avalonia/Hardware/FileIoController.cs). Each owns
 *  the FIO mailbox (reads its params via peek(), reports via fio_ok/fio_fail);
 *  novavm.c just adds the matching `case FIO_CMD_*:` entries.
 * =========================================================================== */

/* ---- graphics-memory ABI registers (the FILE_GSAVE/GLOAD wire layout from
 *      runtime/asm/libgraphics.inc): FIO_GSPACE = VGC plane id, FIO_GADDR_* =
 *      16-bit byte offset within that plane, FIO_GLEN_* = 16-bit length. The
 *      defines (FIO_GSPACE/GADDR_LO/HI/GLEN_LO/HI) are already at the top of
 *      this file (shared with the XRAM commands). ------------------------- */

/* VGC vmem byte access through the PL vmem port (R_VMEM_ADDR/DATA). `space` is
 * a VGC plane id (1=char,2=color,3=gfx,4=sprite,7=text-attr); `addr` is the
 * byte offset within that plane. Mirrors novavm.c's vmem_read / vmem_write. */
static unsigned char vgc_mem_read(unsigned space, unsigned addr) {
    wr(R_VMEM_ADDR, ((space & 7u) << 17) | (addr & 0x1FFFFu));
    (void)rd(R_VMEM_DATA);                 /* discard pipeline/stale byte */
    return (unsigned char)rd(R_VMEM_DATA);
}
static void vgc_mem_write(unsigned space, unsigned addr, unsigned char v) {
    wr(R_VMEM_DATA, ((space & 7u) << 25) | ((addr & 0x1FFFFu) << 8) | v);
}

/* Byte length of a VGC memory space (mirrors VGC GetMemorySpaceLength): char /
 * color / text-attr = ScreenSize (80*50=4000), gfx = 320*200, sprite shapes =
 * 32768. 0 for an unknown space (-> FIO_ERR). */
static int vgc_space_len(unsigned space) {
    switch (space) {
        case 1: return 80 * 50;     /* char / screen RAM    */
        case 2: return 80 * 50;     /* color RAM            */
        case 3: return 320 * 200;   /* gfx bitmap (64000)   */
        case 4: return 32768;       /* sprite shape RAM     */
        case 7: return 80 * 50;     /* text attribute RAM   */
        default: return 0;
    }
}

/* Store a raw blob as (name + DT_GFX) on the mounted drive. 0 ok, else an
 * FIO_ERR_* code (port of nfio_disk_save without the 2-byte load-addr header —
 * a .gfx file is raw VGC bytes, matching DoGSave's File.WriteAllBytes). */
static int ndi_gfx_store(const char *name, const unsigned char *data, unsigned len) {
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) return FIO_ERR_NOTMOUNTED;
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) return FIO_ERR_NOTFOUND;
    if (ndi_find(img, fname, parent) >= 0) ndi_delete(img, fname, parent);  /* overwrite */
    int idx = ndi_create(img, fname, DT_GFX, parent, len);
    if (idx < 0) return FIO_ERR_DISKFULL;
    if (ndi_write(img, idx, 0, data, len) != 0) return FIO_ERR_IO;
    ndi_flush(img);
    printf("[fio] GSAVE(ndi) %s:%s (%u bytes)\n", drive_slot_name(slot), path, len);
    return 0;
}

/* Load a named blob from the mounted drive into buf (<= maxlen). >=0 bytes on
 * success, or a NEGATED FIO_ERR_* code on failure. */
static int ndi_gfx_fetch(const char *name, unsigned char *buf, unsigned maxlen) {
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) return -FIO_ERR_NOTMOUNTED;
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) return -FIO_ERR_NOTFOUND;
    int idx = ndi_find(img, fname, parent);
    if (idx < 0) return -FIO_ERR_NOTFOUND;
    int n = ndi_read(img, idx, 0, buf, maxlen);
    if (n < 0) return -FIO_ERR_IO;
    return n;
}

/* GSAVE: copy `glen` bytes of VGC plane `gspace` from `gaddr` to a .gfx file. */
void nfio_gsave(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned space = peek(FIO_GSPACE);
    unsigned addr  = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8);
    unsigned len   = peek(FIO_GLEN_LO)  | (peek(FIO_GLEN_HI)  << 8);

    int spacelen = vgc_space_len(space);
    if (len == 0 || spacelen <= 0 || addr + len > (unsigned)spacelen || len > sizeof g_fbuf) {
        fio_fail(FIO_ERR_IO); return;
    }
    for (unsigned i = 0; i < len; i++) g_fbuf[i] = vgc_mem_read(space, addr + i);

    int rc = ndi_gfx_store(name, g_fbuf, len);
    if (rc != 0) { fio_fail(rc); return; }
    poke(FIO_SIZE_LO, len & 0xFF);
    poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
    fio_ok();
}

/* GLOAD: read a .gfx file and stream it into VGC plane `gspace` at `gaddr`.
 * `glen` (FIO_GLEN) caps the transfer; 0 means "the whole file". */
void nfio_gload(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    unsigned space  = peek(FIO_GSPACE);
    unsigned addr   = peek(FIO_GADDR_LO) | (peek(FIO_GADDR_HI) << 8);
    unsigned reqlen = peek(FIO_GLEN_LO)  | (peek(FIO_GLEN_HI)  << 8);

    int n;
    n = ndi_gfx_fetch(name, g_fbuf, sizeof g_fbuf);
    if (n < 0) { fio_fail(-n); return; }

    unsigned len = (reqlen > 0 && (unsigned)n > reqlen) ? reqlen : (unsigned)n;
    int spacelen = vgc_space_len(space);
    if (spacelen <= 0 || addr + len > (unsigned)spacelen) { fio_fail(FIO_ERR_IO); return; }
    for (unsigned i = 0; i < len; i++) vgc_mem_write(space, addr + i, g_fbuf[i]);

    poke(FIO_SIZE_LO, len & 0xFF);
    poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
    printf("[fio] GLOAD %s space %u $%04x (%u bytes)\n", name, space, addr, len);
    fio_ok();
}

/* MKDIR: create a directory entry (parents auto-created via ndi_walk create=1).
 * Errors if the leaf already exists (mirrors DoMkdir -> AddDirectory). */
void nfio_mkdir(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    if (ndi_find(img, fname, parent) >= 0) { fio_fail(FIO_ERR_IO); return; }  /* already exists */
    if (ndi_mkdir(img, fname, parent) < 0) { fio_fail(FIO_ERR_DISKFULL); return; }
    printf("[fio] MKDIR(ndi) %s:%s\n", drive_slot_name(slot), path);
    fio_ok();
}

/* Does any active entry hang off `parent`? (Non-empty-directory guard.) */
static int ndi_dir_has_children(ndi_t *m, uint16_t parent) {
    ndi_entry_t e;
    for (int i = 0; i < m->dir_entry_count; i++) {
        ndi_dir_read_entry(m, i, &e);
        if (!(e.flags & FL_ACTIVE)) continue;
        if (e.parent_index == parent) return 1;
    }
    return 0;
}

/* Remove an empty directory entry. 0 ok, else FIO_ERR_* (port of DoRmdir /
 * NdiImage.RemoveDirectory: must exist, be a directory, and be empty). */
static int ndi_rmdir(ndi_t *m, const char *name, uint16_t parent) {
    int idx = ndi_find(m, name, parent);
    if (idx < 0) return FIO_ERR_NOTFOUND;
    ndi_entry_t e; ndi_dir_read_entry(m, idx, &e);
    if (!(e.flags & FL_DIRECTORY)) return FIO_ERR_IO;               /* not a directory */
    if (ndi_dir_has_children(m, (uint16_t)idx)) return FIO_ERR_IO;  /* not empty       */
    ndi_dir_clear_slot(m, idx);
    return (ndi_flush_metadata(m) == 0) ? 0 : FIO_ERR_IO;
}

/* RMDIR: remove an empty directory (guards against non-empty, mirrors DoRmdir). */
void nfio_rmdir(void) {
    char name[80];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    char path[200];
    int slot = nfio_resolve(name, path, sizeof path);
    ndi_t *img = (slot >= 0) ? slot_image(slot) : NULL;
    if (!img) { fio_fail(FIO_ERR_NOTMOUNTED); return; }
    uint16_t parent; char fname[NDI_MAX_NAME + 1];
    if (ndi_walk(img, path, 0, &parent, fname, sizeof fname) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    int rc = ndi_rmdir(img, fname, parent);
    if (rc != 0) { fio_fail(rc); return; }
    ndi_flush(img);
    printf("[fio] RMDIR(ndi) %s:%s\n", drive_slot_name(slot), path);
    fio_ok();
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
