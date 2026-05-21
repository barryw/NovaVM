#include "../sid_vm.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

static int g_pass = 0;
static int g_fail = 0;

static void check(const char* name, bool ok) {
    if (ok) {
        printf("  PASS  %s\n", name);
        g_pass++;
    } else {
        printf("  FAIL  %s\n", name);
        g_fail++;
    }
}

static void put_be16(std::vector<uint8_t>& v, size_t off, uint16_t value) {
    v[off] = (uint8_t)(value >> 8);
    v[off + 1] = (uint8_t)value;
}

static std::vector<uint8_t> minimal_sid_header(uint16_t load,
                                               uint16_t init,
                                               uint16_t play) {
    std::vector<uint8_t> sid(124, 0);
    sid[0] = 'P';
    sid[1] = 'S';
    sid[2] = 'I';
    sid[3] = 'D';
    put_be16(sid, 4, 2);
    put_be16(sid, 6, 124);
    put_be16(sid, 8, load);
    put_be16(sid, 10, init);
    put_be16(sid, 12, play);
    put_be16(sid, 14, 1);
    put_be16(sid, 16, 1);
    return sid;
}

struct SidWrite {
    uint16_t addr;
    uint8_t value;
};

struct SidWriteLog {
    std::vector<SidWrite> writes;
};

static bool record_sid_write(void* user, uint16_t addr, uint8_t value) {
    auto* log = static_cast<SidWriteLog*>(user);
    log->writes.push_back({ addr, value });
    return true;
}

static void test_parse_sid_header() {
    printf("\nTest: parse PSID header\n");
    auto sid = minimal_sid_header(0x1000, 0x1000, 0x1008);
    sid.push_back(0xEA);

    nova_sid::SidFileInfo info;
    bool ok = nova_sid::parse_sid_header(sid.data(), sid.size(), info);
    check("header parses", ok);
    check("magic is PSID", strcmp(info.magic, "PSID") == 0);
    check("payload offset is data offset", info.payloadFileOffset == 124);
    check("load address preserved", info.loadAddress == 0x1000);
    check("payload byte count excludes header", info.payloadBytes == 1);
}

static void test_sparse_memory_allocates_touched_pages_only() {
    printf("\nTest: sparse C64 memory pages\n");
    nova_sid::SparseMemory mem;
    uint8_t data[] = { 1, 2, 3, 4 };

    bool ok = mem.load(0x10FE, data, sizeof(data));
    check("load across page boundary succeeds", ok);
    check("two pages allocated", mem.pageCount() == 2);
    check("allocated bytes are page-granular", mem.bytesAllocated() == 512);
    check("loaded byte before boundary", mem.read(0x10FF) == 2);
    check("loaded byte after boundary", mem.read(0x1100) == 3);
    check("untouched memory reads zero", mem.read(0x2200) == 0);
}

static void test_vm_runs_init_and_play_without_rom_blob() {
    printf("\nTest: SID VM init/play with clean-room KERNAL stub\n");
    // $1000: LDA #$0F / STA $D418 / JSR $FFD2 / RTS
    // $1009: LDA #$42 / STA $D400 / RTS
    uint8_t payload[] = {
        0xA9, 0x0F,
        0x8D, 0x18, 0xD4,
        0x20, 0xD2, 0xFF,
        0x60,
        0xA9, 0x42,
        0x8D, 0x00, 0xD4,
        0x60
    };

    SidWriteLog log;
    nova_sid::SidVm vm;
    vm.setSidWriteHandler(record_sid_write, &log);
    check("payload loads", vm.loadPayload(0x1000, payload, sizeof(payload)));
    vm.setEntryPoints(0x1000, 0x1009);

    nova_sid::RunResult init = vm.runInit(0);
    check("init returns through trap", init.status == nova_sid::RunStatus::Ok);
    check("init wrote SID volume", !log.writes.empty() &&
          log.writes[0].addr == 0xD418 && log.writes[0].value == 0x0F);

    size_t before_play = log.writes.size();
    nova_sid::RunResult play = vm.runPlayFrame();
    check("play returns through trap", play.status == nova_sid::RunStatus::Ok);
    check("play wrote SID frequency", log.writes.size() == before_play + 1 &&
          log.writes.back().addr == 0xD400 && log.writes.back().value == 0x42);
    check("KERNAL CHROUT did not require ROM code",
          vm.stats().unsupportedCalls == 0);
}

static void test_vm_rejects_unknown_rom_call() {
    printf("\nTest: unknown ROM call fails loud\n");
    // $2000: JSR $FFFF / RTS
    uint8_t payload[] = { 0x20, 0xFF, 0xFF, 0x60 };

    nova_sid::SidVm vm;
    check("payload loads", vm.loadPayload(0x2000, payload, sizeof(payload)));
    vm.setEntryPoints(0x2000, 0);

    nova_sid::RunResult init = vm.runInit(0);
    check("unsupported ROM call is reported",
          init.status == nova_sid::RunStatus::UnsupportedRomCall);
    check("unsupported call count increments", vm.stats().unsupportedCalls == 1);
}

static void test_unofficial_opcode_smoke() {
    printf("\nTest: common unofficial opcodes\n");
    // $3000: LDA #$AA / TAX / SAX $20 / LAX $20 / RTS
    uint8_t payload[] = {
        0xA9, 0xAA,
        0xAA,
        0x87, 0x20,
        0xA7, 0x20,
        0x60
    };

    nova_sid::SidVm vm;
    check("payload loads", vm.loadPayload(0x3000, payload, sizeof(payload)));
    vm.setEntryPoints(0x3000, 0);
    nova_sid::RunResult init = vm.runInit(0);
    check("unofficial LAX/SAX smoke returns",
          init.status == nova_sid::RunStatus::Ok);
}

int main() {
    test_parse_sid_header();
    test_sparse_memory_allocates_touched_pages_only();
    test_vm_runs_init_and_play_without_rom_blob();
    test_vm_rejects_unknown_rom_call();
    test_unofficial_opcode_smoke();

    printf("\nSID VM tests: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
