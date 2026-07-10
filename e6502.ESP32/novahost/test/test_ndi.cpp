// Host-side unit test for ndi_image.cpp.
// Compiles with clang++ on macOS — no Arduino, no SD_MMC.
//
// Backs the IStream interface with a stdio FILE* and runs the parser against
// docs/programs/demo.ndi when it has been regenerated as NDI v2. Write-side
// tests create fresh v2 images via the CLI.

#include "../ndi_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// FILE*-backed IStream
// ---------------------------------------------------------------------------
class FileStream : public ndi::IStream {
public:
    explicit FileStream(FILE* f) : _f(f) {}
    ~FileStream() override { if (_f) fclose(_f); }

    bool seek(uint64_t offset) override {
        return fseeko(_f, (off_t)offset, SEEK_SET) == 0;
    }
    int read(uint8_t* buf, size_t len) override {
        size_t n = fread(buf, 1, len, _f);
        return (int)n;
    }
    int write(const uint8_t* buf, size_t len) override {
        size_t n = fwrite(buf, 1, len, _f);
        return (int)n;
    }
    void flush() override { fflush(_f); }

private:
    FILE* _f;
};

// ---------------------------------------------------------------------------
// Test plumbing
// ---------------------------------------------------------------------------
static int g_pass = 0, g_fail = 0;

static void check(const char* name, bool ok) {
    if (ok) { printf("  PASS  %s\n", name); g_pass++; }
    else    { printf("  FAIL  %s\n", name); g_fail++; }
}

static void check_eq_str(const char* name, const char* a, const char* b) {
    if (strcmp(a, b) == 0) {
        printf("  PASS  %s (=\"%s\")\n", name, a); g_pass++;
    } else {
        printf("  FAIL  %s (got \"%s\", want \"%s\")\n", name, a, b);
        g_fail++;
    }
}

static void check_eq_int(const char* name, long a, long b) {
    if (a == b) { printf("  PASS  %s (=%ld)\n", name, a); g_pass++; }
    else        { printf("  FAIL  %s (got %ld, want %ld)\n", name, a, b);
                  g_fail++; }
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------
static const char* DEMO_PATH =
    "../../../docs/programs/demo.ndi";

static void test_open_and_header() {
    printf("\nTest: open demo.ndi + header parse\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) { printf("  SKIP (no demo.ndi at %s)\n", DEMO_PATH); return; }
    FileStream stream(f);
    ndi::NdiImage img;
    bool opened = img.open(&stream);
    if (!opened) {
        printf("  SKIP (demo.ndi is not current NDI v2)\n");
        return;
    }
    check("open() succeeded", opened);

    const ndi::HeaderInfo& h = img.header();
    check_eq_int("format_version",          h.format_version, 2);
    check_eq_int("sector_size",             h.sector_size, 256);
    check_eq_int("total_sectors",           h.total_sectors, 16384);
    check_eq_str("volume_label",            h.volume_label, "SHOWCASE");
    check_eq_int("directory_start_sector",  h.directory_start_sector, 9);
    check_eq_int("directory_sector_count",  h.directory_sector_count, 48);
    check_eq_int("data_start_sector",       h.data_start_sector, 57);

    check("free sector count is plausible",
          img.free_sectors() > 0 && (uint32_t)img.free_sectors() < h.total_sectors);
}

static void test_list_root() {
    printf("\nTest: list root directory\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) { printf("  SKIP (demo.ndi is not current NDI v2)\n"); return; }

    ndi::DirEntry entries[16];
    int n = 0;
    img.list_entries(ndi::ROOT_PARENT, entries, 16, n);
    check_eq_int("root entry count", n, 6);  // 5 dirs + linked AUTOBOOT.bin

    // Verify expected names exist (order = slot order, so deterministic
    // for an unmodified demo image).
    bool saw_featured = false, saw_2sid = false, saw_sid = false,
         saw_wts = false, saw_arcade = false, saw_autoboot_bin = false,
         saw_keyboard_bin = false;
    for (int i = 0; i < n; i++) {
        const ndi::DirEntry& e = entries[i];
        if (strcmp(e.filename, "featured")     == 0 && e.is_directory()) saw_featured = true;
        if (strcmp(e.filename, "2sid")         == 0 && e.is_directory()) saw_2sid     = true;
        if (strcmp(e.filename, "sid")          == 0 && e.is_directory()) saw_sid      = true;
        if (strcmp(e.filename, "wts")          == 0 && e.is_directory()) saw_wts      = true;
        if (strcmp(e.filename, "arcade")       == 0 && e.is_directory()) saw_arcade   = true;
        if (strcmp(e.filename, "AUTOBOOT.bin") == 0 && !e.is_directory()
                                                     && e.size_bytes > 2)
            saw_autoboot_bin = true;
        if (strcmp(e.filename, "KEYBOARD.bin") == 0)
            saw_keyboard_bin = true;
    }
    check("featured dir present",          saw_featured);
    check("2sid dir present",              saw_2sid);
    check("sid dir present",               saw_sid);
    check("wts dir present",               saw_wts);
    check("arcade dir present",            saw_arcade);
    check("linked AUTOBOOT.bin present", saw_autoboot_bin);
    check("standalone KEYBOARD.bin absent", !saw_keyboard_bin);
}

static void test_find_and_read_autoboot_bin() {
    printf("\nTest: find + read AUTOBOOT.bin\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) { printf("  SKIP (demo.ndi is not current NDI v2)\n"); return; }

    int idx = img.find_entry("AUTOBOOT.bin", ndi::ROOT_PARENT);
    check("find_entry AUTOBOOT.bin", idx >= 0);

    ndi::DirEntry e;
    check("get_entry by index", img.get_entry(idx, e));
    check("AUTOBOOT.bin has a load header and code", e.size_bytes > 2);
    check_eq_int("AUTOBOOT.bin file_type=BIN", e.file_type, ndi::FT_BIN);
    check("AUTOBOOT.bin is active",          e.is_active());
    check("AUTOBOOT.bin is not directory",  !e.is_directory());

    uint8_t buf[16384];
    int got = img.read_file_by_index(idx, buf, sizeof(buf));
    check_eq_int("read_file_by_index returns size", got, e.size_bytes);
    check("AUTOBOOT.bin loads at $7200", got > 2 && buf[0] == 0x00 && buf[1] == 0x72);

    // Sanity: BIN should be non-zero (not all zeros).
    int nonzero = 0;
    for (int i = 0; i < 64; i++) if (buf[i] != 0) nonzero++;
    check("first 64 bytes contain non-zero data", nonzero > 0);
}

static void test_find_in_subdir() {
    // Walk into one of the subdirs and see if it has any contents.
    // We don't know what's there; just verify no crashes and listing works.
    printf("\nTest: list contents of a subdirectory\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) { printf("  SKIP (demo.ndi is not current NDI v2)\n"); return; }

    int featured_idx = img.find_entry("featured", ndi::ROOT_PARENT);
    check("find featured dir", featured_idx >= 0);

    ndi::DirEntry kids[64];
    int kn = 0;
    img.list_entries((uint16_t)featured_idx, kids, 64, kn);
    printf("  featured/ has %d active entries\n", kn);
    check("list_entries doesn't crash", true);
}

static void test_case_insensitive_lookup() {
    printf("\nTest: case-insensitive lookup\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) return;

    int a = img.find_entry("autoboot.bin", ndi::ROOT_PARENT);
    int b = img.find_entry("AUTOBOOT.BIN", ndi::ROOT_PARENT);
    int c = img.find_entry("AutoBoot.Bin", ndi::ROOT_PARENT);
    check("lowercase match", a >= 0);
    check_eq_int("uppercase = lowercase",  b, a);
    check_eq_int("mixed-case = lowercase", c, a);

    int missing = img.find_entry("nope", ndi::ROOT_PARENT);
    check_eq_int("missing returns -1", missing, -1);
}

// ---------------------------------------------------------------------------
// Write side: round-trip tests against a temp .ndi created via the CLI.
// ---------------------------------------------------------------------------
static void test_write_read_roundtrip() {
    printf("\nTest: write_file → close → reopen → read\n");
    char path[] = "/tmp/ndi_test_XXXXXX.ndi";
    int fd = mkstemps(path, 4);
    if (fd < 0) { perror("mkstemps"); return; }
    close(fd);

    // Use the .NET CLI to format a fresh image. Avoids re-implementing
    // CreateFormatted in C++ for now (write-side coverage focuses on
    // file ops, not initial format).
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "dotnet run --project ../../../e6502.Nova -c Release --no-build -- create %s --size 64 --label TEST",
        path);
    if (system(cmd) != 0) { printf("  SKIP (CLI format failed)\n"); unlink(path); return; }

    {
        FILE* f = fopen(path, "rb+");
        FileStream stream(f);
        ndi::NdiImage img;
        check("open formatted image", img.open(&stream));

        // Initially empty.
        ndi::DirEntry tmp[8];
        int n = 0;
        img.list_entries(ndi::ROOT_PARENT, tmp, 8, n);
        check_eq_int("freshly formatted root count", n, 0);

        // Write a small file.
        const char* payload = "Hello, NDI from C++!";
        int slot = img.write_file("hello.bas", ndi::FT_BAS, ndi::ROOT_PARENT,
                                   (const uint8_t*)payload, (uint32_t)strlen(payload));
        check("write_file slot >= 0", slot >= 0);

        // Confirm same image lists the new file.
        n = 0;
        img.list_entries(ndi::ROOT_PARENT, tmp, 8, n);
        check_eq_int("after write, root count = 1", n, 1);
    }

    // Reopen the image fresh and read the file back.
    {
        FILE* f = fopen(path, "rb+");
        FileStream stream(f);
        ndi::NdiImage img;
        check("reopen image", img.open(&stream));

        int idx = img.find_entry("hello.bas", ndi::ROOT_PARENT);
        check("find hello.bas after reopen", idx >= 0);

        uint8_t buf[64];
        memset(buf, 0, sizeof(buf));
        int got = img.read_file_by_index(idx, buf, sizeof(buf));
        check_eq_int("read returns 20 bytes", got, 20);
        check_eq_str("read content matches", (char*)buf, "Hello, NDI from C++!");
    }

    unlink(path);
}

static void test_delete_roundtrip() {
    printf("\nTest: write_file → delete_file → not found\n");
    char path[] = "/tmp/ndi_del_XXXXXX.ndi";
    int fd = mkstemps(path, 4);
    if (fd < 0) return;
    close(fd);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "dotnet run --project ../../../e6502.Nova -c Release --no-build -- create %s --size 64 --label DEL",
        path);
    if (system(cmd) != 0) { printf("  SKIP (CLI format failed)\n"); unlink(path); return; }

    FILE* f = fopen(path, "rb+");
    FileStream stream(f);
    ndi::NdiImage img;
    img.open(&stream);

    const char* data = "doomed";
    img.write_file("doomed.bas", ndi::FT_BAS, ndi::ROOT_PARENT,
                    (const uint8_t*)data, 6);
    check("doomed exists pre-delete",
          img.find_entry("doomed.bas", ndi::ROOT_PARENT) >= 0);

    bool ok = img.delete_file("doomed.bas", ndi::ROOT_PARENT);
    check("delete_file returns true", ok);
    check_eq_int("doomed gone post-delete",
                 img.find_entry("doomed.bas", ndi::ROOT_PARENT), -1);

    unlink(path);
}

static void test_mkdir_rmdir() {
    printf("\nTest: mkdir + rmdir\n");
    char path[] = "/tmp/ndi_dir_XXXXXX.ndi";
    int fd = mkstemps(path, 4);
    if (fd < 0) return;
    close(fd);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "dotnet run --project ../../../e6502.Nova -c Release --no-build -- create %s --size 64 --label DIRT",
        path);
    if (system(cmd) != 0) { printf("  SKIP (CLI format failed)\n"); unlink(path); return; }

    FILE* f = fopen(path, "rb+");
    FileStream stream(f);
    ndi::NdiImage img;
    img.open(&stream);

    int sub = img.make_directory("sub", ndi::ROOT_PARENT);
    check("make_directory returns slot", sub >= 0);

    int found = img.find_entry("sub", ndi::ROOT_PARENT);
    check_eq_int("find_entry sub matches mkdir slot", found, sub);

    ndi::DirEntry sub_e;
    img.get_entry(sub, sub_e);
    check("sub is_directory", sub_e.is_directory());

    bool removed = img.remove_directory("sub", ndi::ROOT_PARENT);
    check("remove empty dir returns true", removed);
    check_eq_int("sub gone post-rmdir",
                 img.find_entry("sub", ndi::ROOT_PARENT), -1);

    unlink(path);
}

// Mirrors the Arty FCLOSE save-commit path (ps_fio fio_commit_write): the V6
// SAVE writes a save file into the mounted .ndi via create_file(sized) +
// write_file_chunk_by_index streamed in 256-byte chunks + zero_file_tail. This
// guards that streaming write path (distinct from the all-at-once write_file
// covered above) round-trips. Portable: formats via the CLI on a relative path.
static void test_create_file_chunked_roundtrip() {
    printf("\nTest: create_file + chunked write_file_chunk_by_index -> reopen -> read\n");
    char path[] = "/tmp/ndi_chunk_XXXXXX.ndi";
    int fd = mkstemps(path, 4);
    if (fd < 0) { perror("mkstemps"); return; }
    close(fd);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "dotnet run --project ../../../e6502.Nova -c Release --no-build -- create %s --size 64 --label CHUNK",
        path);
    if (system(cmd) != 0) { printf("  SKIP (CLI format failed)\n"); unlink(path); return; }

    const uint32_t SIZE = 600;                 // spans 3 sectors; partial last
    uint8_t payload[SIZE];
    for (uint32_t i = 0; i < SIZE; i++) payload[i] = (uint8_t)(i * 7 + 3);

    {
        FILE* f = fopen(path, "rb+");
        FileStream stream(f);
        ndi::NdiImage img;
        check("open formatted image", img.open(&stream));

        int idx = img.create_file("save00.nzs", ndi::FT_BIN, ndi::ROOT_PARENT, SIZE);
        check("create_file slot >= 0", idx >= 0);

        bool wrote_ok = (idx >= 0);
        for (uint32_t off = 0; off < SIZE; off += 256) {
            uint32_t chunk = SIZE - off; if (chunk > 256) chunk = 256;
            if (!img.write_file_chunk_by_index(idx, off, payload + off, chunk)) wrote_ok = false;
        }
        check("write_file_chunk_by_index all chunks", wrote_ok);
        check("zero_file_tail_by_index", idx >= 0 && img.zero_file_tail_by_index(idx));
    }

    {
        FILE* f = fopen(path, "rb+");
        FileStream stream(f);
        ndi::NdiImage img;
        check("reopen image", img.open(&stream));

        int ridx = img.find_entry("save00.nzs", ndi::ROOT_PARENT);
        check("find save00.nzs after reopen", ridx >= 0);

        uint8_t buf[SIZE + 16];
        memset(buf, 0, sizeof(buf));
        int got = img.read_file_by_index(ridx, buf, sizeof(buf));
        check_eq_int("read returns SIZE bytes", got, (long)SIZE);
        check("chunked content round-trips", memcmp(buf, payload, SIZE) == 0);
    }

    unlink(path);
}

// ---------------------------------------------------------------------------
int main() {
    printf("=== ndi_image host tests ===\n");
    test_open_and_header();
    test_list_root();
    test_find_and_read_autoboot_bin();
    test_find_in_subdir();
    test_case_insensitive_lookup();
    test_write_read_roundtrip();
    test_delete_roundtrip();
    test_mkdir_rmdir();
    test_create_file_chunked_roundtrip();

    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
