// Nova Disk Image parser — implementation.
//
// Mirrors e6502.Storage/Ndi{Header,Bam,Directory,Image}.cs. See header
// for layout. Read-side first; write-side at the bottom.

#include "ndi_image.h"

#include <stdlib.h>
#include <string.h>

namespace ndi {

// ===========================================================================
// On-disk layout constants
// ===========================================================================
static constexpr int      SECTOR_SIZE      = 256;
static constexpr int      ENTRY_SIZE       = 64;
static constexpr int      ENTRIES_PER_SEC  = 4;
static constexpr int      MAX_FILENAME_LEN = 32;

// Directory entry byte offsets (within a 64-byte slot).
static constexpr int OFF_FLAGS      = 0x00;
static constexpr int OFF_TYPE       = 0x01;
static constexpr int OFF_PARENT     = 0x02;  // 2 bytes LE
static constexpr int OFF_START_SEC  = 0x04;  // 2 bytes LE
static constexpr int OFF_SIZE_LO    = 0x06;  // 2 bytes LE
static constexpr int OFF_FILENAME   = 0x08;  // 32 bytes
static constexpr int OFF_SECCOUNT   = 0x28;  // 2 bytes LE
static constexpr int OFF_SIZE_HI    = 0x2A;  // 2 bytes LE

// ---------------------------------------------------------------------------
// LE helpers
// ---------------------------------------------------------------------------
static inline uint16_t rd_u16(const uint8_t* p) {
    return (uint16_t)(p[0] | (uint16_t)p[1] << 8);
}
static inline void wr_u16(uint8_t* p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)(v >> 8);
}

// Case-insensitive name compare (bounded by either a null byte or
// MAX_FILENAME_LEN). Used to match directory entries against the name
// supplied by the 6502.
static int ascii_lower(int c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}
static bool name_eq_ci(const char* a, const char* b) {
    for (int i = 0; i < MAX_FILENAME_LEN; i++) {
        char ca = a[i], cb = b[i];
        if (ca == 0 && cb == 0) return true;
        if (ascii_lower((unsigned char)ca) != ascii_lower((unsigned char)cb))
            return false;
        if (ca == 0) return true;
    }
    return true;
}

// ===========================================================================
// Constructor / destructor
// ===========================================================================
NdiImage::NdiImage()
    : _stream(nullptr),
      _bam_bits(nullptr),
      _bam_byte_count(0),
      _data_sector_count(0),
      _free_count(0),
      _dir_data(nullptr),
      _dir_entry_count(0) {
    memset(&_header, 0, sizeof(_header));
}

NdiImage::~NdiImage() {
    close();
}

void NdiImage::close() {
    if (_bam_bits) { free(_bam_bits); _bam_bits = nullptr; }
    if (_dir_data) { free(_dir_data); _dir_data = nullptr; }
    _stream = nullptr;
    _bam_byte_count    = 0;
    _data_sector_count = 0;
    _free_count        = 0;
    _dir_entry_count   = 0;
    memset(&_header, 0, sizeof(_header));
}

// ===========================================================================
// open()
// ===========================================================================
bool NdiImage::open(IStream* stream) {
    if (!stream) return false;
    close();
    _stream = stream;

    if (!read_header())    { close(); return false; }
    if (!read_bam())       { close(); return false; }
    if (!read_directory()) { close(); return false; }
    return true;
}

// ===========================================================================
// Header read
// ===========================================================================
bool NdiImage::read_header() {
    uint8_t buf[SECTOR_SIZE];
    if (!_stream->seek(0)) return false;
    if (_stream->read(buf, SECTOR_SIZE) != SECTOR_SIZE) return false;

    if (buf[0] != 'N' || buf[1] != 'D' || buf[2] != 'I' || buf[3] != 0x1A)
        return false;

    _header.format_version         = buf[4];
    _header.sector_size            = rd_u16(&buf[5]);
    _header.total_sectors          = rd_u16(&buf[7]);

    // Volume label: bytes 9..40, null-padded ASCII.
    int n = 0;
    for (; n < 32; n++) {
        char c = (char)buf[9 + n];
        if (c == 0) break;
        _header.volume_label[n] = c;
    }
    _header.volume_label[n] = 0;

    _header.directory_start_sector = rd_u16(&buf[0x29]);
    _header.directory_sector_count = rd_u16(&buf[0x2B]);
    _header.data_start_sector      = rd_u16(&buf[0x2D]);
    _header.free_sector_count      = rd_u16(&buf[0x2F]);

    if (_header.sector_size != SECTOR_SIZE) return false;
    if (_header.directory_start_sector < 1) return false;
    if (_header.data_start_sector <= _header.directory_start_sector) return false;
    if (_header.total_sectors < _header.data_start_sector) return false;

    return true;
}

// ===========================================================================
// BAM read
// ===========================================================================
bool NdiImage::read_bam() {
    int bam_sectors      = _header.directory_start_sector - 1;
    if (bam_sectors < 1) return false;
    _data_sector_count   = _header.total_sectors - _header.data_start_sector;
    _bam_byte_count      = (_data_sector_count + 7) >> 3;

    int payload_size     = bam_sectors * SECTOR_SIZE;
    uint8_t* payload     = (uint8_t*)malloc(payload_size);
    if (!payload) return false;

    if (!_stream->seek(SECTOR_SIZE)) { free(payload); return false; }
    if (_stream->read(payload, payload_size) != payload_size) {
        free(payload); return false;
    }

    _bam_bits = (uint8_t*)malloc(_bam_byte_count);
    if (!_bam_bits) { free(payload); return false; }
    int copy_n = (payload_size < _bam_byte_count) ? payload_size : _bam_byte_count;
    memcpy(_bam_bits, payload, copy_n);
    if (copy_n < _bam_byte_count)
        memset(_bam_bits + copy_n, 0, _bam_byte_count - copy_n);
    free(payload);

    // Recount free sectors from the bits — header value can drift if the
    // image was modified externally.
    _free_count = 0;
    for (int i = 0; i < _data_sector_count; i++)
        if (!bam_is_allocated(i)) _free_count++;

    return true;
}

// ===========================================================================
// Directory read
// ===========================================================================
bool NdiImage::read_directory() {
    int dir_bytes = _header.directory_sector_count * SECTOR_SIZE;
    _dir_data = (uint8_t*)malloc(dir_bytes);
    if (!_dir_data) return false;

    uint64_t offset = (uint64_t)_header.directory_start_sector * SECTOR_SIZE;
    if (!_stream->seek(offset)) return false;
    if (_stream->read(_dir_data, dir_bytes) != dir_bytes) return false;

    _dir_entry_count = _header.directory_sector_count * ENTRIES_PER_SEC;
    return true;
}

// ===========================================================================
// Flush metadata
// ===========================================================================
bool NdiImage::flush_metadata() {
    if (!_stream) return false;

    // Update free count in header.
    _header.free_sector_count = (uint16_t)_free_count;

    // ----- Header -----
    uint8_t hdr[SECTOR_SIZE];
    memset(hdr, 0, SECTOR_SIZE);
    hdr[0] = 'N'; hdr[1] = 'D'; hdr[2] = 'I'; hdr[3] = 0x1A;
    hdr[4] = _header.format_version;
    wr_u16(&hdr[5], _header.sector_size);
    wr_u16(&hdr[7], _header.total_sectors);
    int label_n = 0;
    while (label_n < 32 && _header.volume_label[label_n] != 0) {
        hdr[9 + label_n] = (uint8_t)_header.volume_label[label_n];
        label_n++;
    }
    wr_u16(&hdr[0x29], _header.directory_start_sector);
    wr_u16(&hdr[0x2B], _header.directory_sector_count);
    wr_u16(&hdr[0x2D], _header.data_start_sector);
    wr_u16(&hdr[0x2F], _header.free_sector_count);

    if (!_stream->seek(0)) return false;
    if (_stream->write(hdr, SECTOR_SIZE) != SECTOR_SIZE) return false;

    // ----- BAM (padded to sector boundary) -----
    int bam_sectors = _header.directory_start_sector - 1;
    int bam_bytes_padded = bam_sectors * SECTOR_SIZE;
    uint8_t* padded = (uint8_t*)malloc(bam_bytes_padded);
    if (!padded) return false;
    memset(padded, 0, bam_bytes_padded);
    int copy_n = (_bam_byte_count < bam_bytes_padded) ? _bam_byte_count
                                                       : bam_bytes_padded;
    memcpy(padded, _bam_bits, copy_n);
    if (!_stream->seek(SECTOR_SIZE)) { free(padded); return false; }
    if (_stream->write(padded, bam_bytes_padded) != bam_bytes_padded) {
        free(padded); return false;
    }
    free(padded);

    // ----- Directory -----
    int dir_bytes = _header.directory_sector_count * SECTOR_SIZE;
    if (!_stream->seek((uint64_t)_header.directory_start_sector * SECTOR_SIZE))
        return false;
    if (_stream->write(_dir_data, dir_bytes) != dir_bytes) return false;

    _stream->flush();
    return true;
}

// ===========================================================================
// BAM helpers
// ===========================================================================
bool NdiImage::bam_is_allocated(int sector) const {
    if (sector < 0 || sector >= _data_sector_count) return true;  // OOB = treat as occupied
    return (_bam_bits[sector >> 3] & (1 << (sector & 7))) != 0;
}

int NdiImage::bam_alloc_contig(int count) {
    if (count <= 0) return -1;
    int run_start = -1;
    int run_len   = 0;
    for (int i = 0; i < _data_sector_count; i++) {
        if (!bam_is_allocated(i)) {
            if (run_start < 0) run_start = i;
            run_len++;
            if (run_len == count) {
                for (int j = run_start; j < run_start + count; j++)
                    _bam_bits[j >> 3] |= (uint8_t)(1 << (j & 7));
                _free_count -= count;
                return run_start;
            }
        } else {
            run_start = -1;
            run_len   = 0;
        }
    }
    return -1;
}

void NdiImage::bam_free(int start, int count) {
    if (start < 0 || count <= 0) return;
    if (start + count > _data_sector_count) return;
    for (int i = start; i < start + count; i++) {
        if (bam_is_allocated(i)) {
            _bam_bits[i >> 3] &= (uint8_t)~(1 << (i & 7));
            _free_count++;
        }
    }
}

// ===========================================================================
// Directory helpers
// ===========================================================================
void NdiImage::dir_read_entry(int slot, DirEntry& out) {
    int o = slot * ENTRY_SIZE;
    out.index        = slot;
    out.flags        = _dir_data[o + OFF_FLAGS];
    out.file_type    = _dir_data[o + OFF_TYPE];
    out.parent_index = rd_u16(&_dir_data[o + OFF_PARENT]);
    out.start_sector = rd_u16(&_dir_data[o + OFF_START_SEC]);
    uint16_t size_lo = rd_u16(&_dir_data[o + OFF_SIZE_LO]);
    uint16_t size_hi = rd_u16(&_dir_data[o + OFF_SIZE_HI]);
    out.size_bytes   = (uint32_t)size_lo | ((uint32_t)size_hi << 16);
    int n = 0;
    for (; n < MAX_FILENAME_LEN; n++) {
        char c = (char)_dir_data[o + OFF_FILENAME + n];
        if (c == 0) break;
        out.filename[n] = c;
    }
    out.filename[n]  = 0;
    out.sector_count = rd_u16(&_dir_data[o + OFF_SECCOUNT]);
}

void NdiImage::dir_write_entry(int slot, uint8_t flags, uint8_t type,
                                uint16_t parent, uint16_t start_sector,
                                uint32_t size, const char* name,
                                uint16_t sector_count) {
    int o = slot * ENTRY_SIZE;
    _dir_data[o + OFF_FLAGS] = flags;
    _dir_data[o + OFF_TYPE]  = type;
    wr_u16(&_dir_data[o + OFF_PARENT],    parent);
    wr_u16(&_dir_data[o + OFF_START_SEC], start_sector);
    wr_u16(&_dir_data[o + OFF_SIZE_LO],   (uint16_t)(size & 0xFFFF));
    memset(&_dir_data[o + OFF_FILENAME], 0, MAX_FILENAME_LEN);
    int n = 0;
    while (name[n] != 0 && n < MAX_FILENAME_LEN) {
        _dir_data[o + OFF_FILENAME + n] = (uint8_t)name[n];
        n++;
    }
    wr_u16(&_dir_data[o + OFF_SECCOUNT], sector_count);
    wr_u16(&_dir_data[o + OFF_SIZE_HI],  (uint16_t)((size >> 16) & 0xFFFF));
}

void NdiImage::dir_clear_slot(int slot) {
    int o = slot * ENTRY_SIZE;
    _dir_data[o + OFF_FLAGS] = 0;
}

int NdiImage::dir_find_free_slot() {
    for (int i = 0; i < _dir_entry_count; i++) {
        if (_dir_data[i * ENTRY_SIZE + OFF_FLAGS] == 0)
            return i;
    }
    return -1;
}

// ===========================================================================
// Public lookup
// ===========================================================================
int NdiImage::find_entry(const char* name, uint16_t parent_index) {
    if (!name) return -1;
    DirEntry tmp;
    for (int i = 0; i < _dir_entry_count; i++) {
        dir_read_entry(i, tmp);
        if (!tmp.is_active())                         continue;
        if (tmp.parent_index != parent_index)         continue;
        if (!name_eq_ci(tmp.filename, name))          continue;
        return i;
    }
    return -1;
}

bool NdiImage::get_entry(int index, DirEntry& out) {
    if (index < 0 || index >= _dir_entry_count) return false;
    dir_read_entry(index, out);
    return true;
}

void NdiImage::list_entries(uint16_t parent_index, DirEntry* out_array,
                             int max_results, int& out_count) {
    out_count = 0;
    if (!out_array || max_results <= 0) return;
    DirEntry tmp;
    for (int i = 0; i < _dir_entry_count && out_count < max_results; i++) {
        dir_read_entry(i, tmp);
        if (tmp.is_active() && tmp.parent_index == parent_index) {
            out_array[out_count++] = tmp;
        }
    }
}

// ===========================================================================
// Public read
// ===========================================================================
int NdiImage::read_file_by_index(int index, uint8_t* buf, size_t buf_size) {
    DirEntry e;
    if (!get_entry(index, e))      return -1;
    if (!e.is_active())            return -1;
    if (e.is_directory())          return -1;
    if (buf_size < e.size_bytes)   return -1;

    uint64_t offset = (uint64_t)(_header.data_start_sector + e.start_sector)
                      * SECTOR_SIZE;
    if (!_stream->seek(offset))    return -1;
    int n = _stream->read(buf, e.size_bytes);
    return (n == (int)e.size_bytes) ? n : -1;
}

int NdiImage::read_file(const char* name, uint16_t parent_index,
                         uint8_t* buf, size_t buf_size) {
    int idx = find_entry(name, parent_index);
    if (idx < 0) return -1;
    return read_file_by_index(idx, buf, buf_size);
}

// ===========================================================================
// Public write
// ===========================================================================
int NdiImage::write_file(const char* name, FileType type, uint16_t parent,
                          const uint8_t* data, uint32_t size) {
    if (!_stream || !name) return -1;
    int sector_count = (int)((size + SECTOR_SIZE - 1) / SECTOR_SIZE);
    if (sector_count == 0) sector_count = 1;

    int start = bam_alloc_contig(sector_count);
    if (start < 0) return -1;

    int slot = dir_find_free_slot();
    if (slot < 0) {
        bam_free(start, sector_count);
        return -1;
    }

    // Write data sectors (zero-padded last sector).
    uint64_t offset = (uint64_t)(_header.data_start_sector + start) * SECTOR_SIZE;
    if (!_stream->seek(offset)) {
        bam_free(start, sector_count);
        return -1;
    }
    if ((int)size > 0) {
        if (_stream->write(data, size) != (int)size) {
            bam_free(start, sector_count);
            return -1;
        }
    }
    // Pad last sector if needed.
    int pad = sector_count * SECTOR_SIZE - (int)size;
    if (pad > 0) {
        uint8_t zero[SECTOR_SIZE];
        memset(zero, 0, SECTOR_SIZE);
        while (pad > 0) {
            int chunk = pad > SECTOR_SIZE ? SECTOR_SIZE : pad;
            if (_stream->write(zero, chunk) != chunk) {
                bam_free(start, sector_count);
                return -1;
            }
            pad -= chunk;
        }
    }

    dir_write_entry(slot, FL_ACTIVE, (uint8_t)type, parent,
                    (uint16_t)start, size, name, (uint16_t)sector_count);

    if (!flush_metadata()) return -1;
    return slot;
}

bool NdiImage::delete_file(const char* name, uint16_t parent_index) {
    int idx = find_entry(name, parent_index);
    if (idx < 0) return false;
    DirEntry e;
    dir_read_entry(idx, e);
    if (e.is_directory()) return false;
    bam_free(e.start_sector, e.sector_count);
    dir_clear_slot(idx);
    return flush_metadata();
}

int NdiImage::make_directory(const char* name, uint16_t parent_index) {
    int slot = dir_find_free_slot();
    if (slot < 0) return -1;
    dir_write_entry(slot, FL_ACTIVE | FL_DIRECTORY, FT_DIR,
                    parent_index, 0, 0, name, 0);
    if (!flush_metadata()) return -1;
    return slot;
}

bool NdiImage::remove_directory(const char* name, uint16_t parent_index) {
    int idx = find_entry(name, parent_index);
    if (idx < 0) return false;
    DirEntry e;
    dir_read_entry(idx, e);
    if (!e.is_directory()) return false;

    // Reject if any active entries claim this index as their parent.
    DirEntry tmp;
    for (int i = 0; i < _dir_entry_count; i++) {
        dir_read_entry(i, tmp);
        if (tmp.is_active() && tmp.parent_index == (uint16_t)idx)
            return false;
    }
    dir_clear_slot(idx);
    return flush_metadata();
}

}  // namespace ndi
