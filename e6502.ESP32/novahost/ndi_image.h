// Nova Disk Image (.ndi) parser — portable C++.
//
// Ports e6502.Storage/Ndi*.cs to C++ for the ESP32 NovaHost firmware.
// Builds on:
//   - host (clang++/gcc) via std::FILE* stream — for unit tests
//   - ESP32 Arduino via SD_MMC File stream wrapper — for production
//
// No Arduino, no STL containers in production path. Heap allocations
// are explicit (open() allocs BAM + dir buffers). All filesystem work
// is sector-by-sector against the underlying IStream.

#ifndef NOVAHOST_NDI_IMAGE_H
#define NOVAHOST_NDI_IMAGE_H

#include <stdint.h>
#include <stddef.h>

namespace ndi {

// ---------------------------------------------------------------------------
// Stream abstraction. Production: SD_MMC File. Test: FILE*.
// All offsets are absolute (start of file = 0). Returns bytes
// transferred, or -1 on error.
// ---------------------------------------------------------------------------
class IStream {
public:
    virtual ~IStream() = default;
    virtual bool seek(uint64_t offset) = 0;
    virtual int  read(uint8_t* buf, size_t len) = 0;
    virtual int  write(const uint8_t* buf, size_t len) = 0;
    virtual void flush() = 0;
};

// ---------------------------------------------------------------------------
// Header info (parsed view of the 256-byte sector 0).
// ---------------------------------------------------------------------------
struct HeaderInfo {
    uint8_t  format_version;
    uint16_t sector_size;
    uint32_t total_sectors;
    char     volume_label[33];           // 32 chars + null
    uint32_t directory_start_sector;
    uint32_t directory_sector_count;
    uint32_t data_start_sector;
    uint32_t free_sector_count;
};

// File / entry types — match the 6502 wire values.
enum FileType : uint8_t {
    FT_BAS = 0, FT_SID = 1, FT_BIN = 2,
    FT_MID = 3, FT_GFX = 4, FT_DIR = 5
};

enum EntryFlags : uint8_t {
    FL_ACTIVE    = 0x01,
    FL_DIRECTORY = 0x02,
    FL_LOCKED    = 0x80
};

// Root parent index sentinel.
static constexpr uint16_t ROOT_PARENT = 0xFFFF;

// ---------------------------------------------------------------------------
// Single directory entry.
// ---------------------------------------------------------------------------
struct DirEntry {
    int      index;
    uint8_t  flags;
    uint8_t  file_type;
    uint16_t parent_index;
    uint32_t start_sector;
    uint32_t size_bytes;
    char     filename[33];
    uint32_t sector_count;

    bool is_active()    const { return (flags & FL_ACTIVE)    != 0; }
    bool is_directory() const { return (flags & FL_DIRECTORY) != 0; }
};

// ---------------------------------------------------------------------------
// NdiImage — owns a stream for the lifetime of the mount. Caches header,
// BAM, and directory in heap-allocated buffers. Data sectors read on
// demand.
// ---------------------------------------------------------------------------
class NdiImage {
public:
    NdiImage();
    ~NdiImage();

    // Non-copyable.
    NdiImage(const NdiImage&) = delete;
    NdiImage& operator=(const NdiImage&) = delete;

    // Open a stream that's already positioned anywhere; reads sector 0
    // for the header and validates magic. Allocates BAM + dir buffers.
    // Stream lifetime is the caller's responsibility.
    bool open(IStream* stream);

    // Close + free buffers.
    void close();

    bool is_open() const { return _stream != nullptr; }

    const HeaderInfo& header() const { return _header; }
    int free_sectors() const         { return _free_count; }

    // ------- Lookup -------
    // Returns the entry index, or -1 if not found / not active.
    int find_entry(const char* name, uint16_t parent_index);

    // Fills out an entry by absolute index. Returns false if index out of range.
    bool get_entry(int index, DirEntry& out);

    // Walks the directory in slot order, returns up to `max_results`
    // entries whose parent matches `parent_index` and whose flags
    // include FL_ACTIVE. `out_count` is set to the number returned.
    void list_entries(uint16_t parent_index, DirEntry* out_array,
                      int max_results, int& out_count);

    // ------- Read -------
    // Reads a file by index into `buf` (must be at least `size_bytes` of
    // the entry). Returns bytes read or -1 on error.
    int read_file_by_index(int index, uint8_t* buf, size_t buf_size);

    // Convenience: find by (name,parent) then read into buf.
    int read_file(const char* name, uint16_t parent_index,
                  uint8_t* buf, size_t buf_size);

    // ------- Write -------
    // Writes a new file. Returns the new entry's index, or -1 on error
    // (no contig space or directory full).
    int write_file(const char* name, FileType type, uint16_t parent_index,
                   const uint8_t* data, uint32_t size);

    // Marks a file inactive and frees its sectors. Returns false if
    // the file doesn't exist or is a directory.
    bool delete_file(const char* name, uint16_t parent_index);

    // Adds an empty subdirectory. Returns the new entry index, or -1.
    int make_directory(const char* name, uint16_t parent_index);

    // Removes an empty subdirectory. Returns false if it has children.
    bool remove_directory(const char* name, uint16_t parent_index);

private:
    IStream*    _stream;
    HeaderInfo  _header;

    // BAM bitmap — 1 bit per data sector. Allocated as
    // ceil(data_sectors / 8) bytes.
    uint8_t*    _bam_bits;
    int         _bam_byte_count;
    int         _data_sector_count;
    int         _free_count;

    // Directory buffer — directory_sector_count * 256 bytes. 4 entries
    // per sector × 64-byte entries.
    uint8_t*    _dir_data;
    int         _dir_entry_count;     // total slots = sectors × 4

    // Read sector 0 → _header.
    bool read_header();

    // Read BAM into _bam_bits + _free_count.
    bool read_bam();

    // Read directory into _dir_data.
    bool read_directory();

    // Flush header + BAM + dir back to the stream.
    bool flush_metadata();

    // BAM helpers.
    bool bam_is_allocated(int sector) const;
    int  bam_alloc_contig(int count);          // returns start sector or -1
    void bam_free(int start, int count);

    // Directory helpers.
    int  dir_find_free_slot();
    void dir_write_entry(int slot, uint8_t flags, uint8_t type,
                         uint16_t parent, uint32_t start_sector,
                         uint32_t size, const char* name,
                         uint32_t sector_count);
    void dir_read_entry(int slot, DirEntry& out);
    void dir_clear_slot(int slot);
};

}  // namespace ndi

#endif
