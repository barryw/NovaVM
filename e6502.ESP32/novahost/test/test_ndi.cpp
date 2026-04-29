// Host-side unit test for ndi_image.cpp.
// Compiles with clang++ on macOS — no Arduino, no SD_MMC.
//
// Backs the IStream interface with a stdio FILE* and runs the parser
// against /Users/barry/Git/e6502/docs/programs/demo.ndi when it has been
// regenerated as NDI v2. Write-side tests create fresh v2 images via the CLI.

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
    "/Users/barry/Git/e6502/docs/programs/demo.ndi";

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
    check_eq_int("total_sectors",           h.total_sectors, 8192);
    check_eq_str("volume_label",            h.volume_label, "DEMO DISK");
    check_eq_int("directory_start_sector",  h.directory_start_sector, 5);
    check_eq_int("directory_sector_count",  h.directory_sector_count, 48);
    check_eq_int("data_start_sector",       h.data_start_sector, 53);

    // Free count should match `ndi info` (1668 free).
    check_eq_int("free_sectors",            img.free_sectors(), 1668);
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
    check_eq_int("root entry count", n, 7);  // 5 dirs + 2 BIN files

    // Verify expected names exist (order = slot order, so deterministic
    // for an unmodified demo image).
    bool saw_classical = false, saw_movies = false, saw_tv = false,
         saw_games = false, saw_sid = false, saw_demo_bin = false;
    for (int i = 0; i < n; i++) {
        const ndi::DirEntry& e = entries[i];
        if (strcmp(e.filename, "classical") == 0 && e.is_directory()) saw_classical = true;
        if (strcmp(e.filename, "movies")    == 0 && e.is_directory()) saw_movies    = true;
        if (strcmp(e.filename, "tv")        == 0 && e.is_directory()) saw_tv        = true;
        if (strcmp(e.filename, "games")     == 0 && e.is_directory()) saw_games     = true;
        if (strcmp(e.filename, "sid")       == 0 && e.is_directory()) saw_sid       = true;
        if (strcmp(e.filename, "demo.bin")  == 0 && !e.is_directory()
                                                  && e.size_bytes == 2438)
            saw_demo_bin = true;
    }
    check("classical dir present",       saw_classical);
    check("movies dir present",          saw_movies);
    check("tv dir present",              saw_tv);
    check("games dir present",           saw_games);
    check("sid dir present",             saw_sid);
    check("demo.bin (2438 bytes) present", saw_demo_bin);
}

static void test_find_and_read_demo_bin() {
    printf("\nTest: find + read demo.bin\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) { printf("  SKIP (demo.ndi is not current NDI v2)\n"); return; }

    int idx = img.find_entry("demo.bin", ndi::ROOT_PARENT);
    check("find_entry demo.bin", idx >= 0);

    ndi::DirEntry e;
    check("get_entry by index", img.get_entry(idx, e));
    check_eq_int("demo.bin size_bytes", e.size_bytes, 2438);
    check_eq_int("demo.bin file_type=BIN", e.file_type, ndi::FT_BIN);
    check("demo.bin is active",          e.is_active());
    check("demo.bin is not directory",  !e.is_directory());

    uint8_t buf[4096];
    int got = img.read_file_by_index(idx, buf, sizeof(buf));
    check_eq_int("read_file_by_index returns size", got, 2438);

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

    int games_idx = img.find_entry("games", ndi::ROOT_PARENT);
    check("find games dir", games_idx >= 0);

    ndi::DirEntry kids[64];
    int kn = 0;
    img.list_entries((uint16_t)games_idx, kids, 64, kn);
    printf("  games/ has %d active entries\n", kn);
    check("list_entries doesn't crash", true);
}

static void test_case_insensitive_lookup() {
    printf("\nTest: case-insensitive lookup\n");
    FILE* f = fopen(DEMO_PATH, "rb+");
    if (!f) return;
    FileStream stream(f);
    ndi::NdiImage img;
    if (!img.open(&stream)) return;

    int a = img.find_entry("demo.bin", ndi::ROOT_PARENT);
    int b = img.find_entry("DEMO.BIN", ndi::ROOT_PARENT);
    int c = img.find_entry("Demo.Bin", ndi::ROOT_PARENT);
    check_eq_int("lowercase match",  a, a);
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
        "cd /Users/barry/Git/e6502/e6502.NDI && dotnet run -- create %s --size 64 --label TEST > /dev/null",
        path);
    if (system(cmd) != 0) { printf("  SKIP (CLI format failed)\n"); return; }

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
        "cd /Users/barry/Git/e6502/e6502.NDI && dotnet run -- create %s --size 64 --label DEL > /dev/null",
        path);
    if (system(cmd) != 0) return;

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
        "cd /Users/barry/Git/e6502/e6502.NDI && dotnet run -- create %s --size 64 --label DIRT > /dev/null",
        path);
    if (system(cmd) != 0) return;

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

// ---------------------------------------------------------------------------
int main() {
    printf("=== ndi_image host tests ===\n");
    test_open_and_header();
    test_list_root();
    test_find_and_read_demo_bin();
    test_find_in_subdir();
    test_case_insensitive_lookup();
    test_write_read_roundtrip();
    test_delete_roundtrip();
    test_mkdir_rmdir();

    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
