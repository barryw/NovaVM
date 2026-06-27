// osd_logic_tb.cpp — Verilator unit test for the parked OSD debounce + pause gates.
#include "Vosd_logic.h"
#include "verilated.h"
#include <cstdio>
static Vosd_logic* d;
static int fails = 0;
static inline void tick() { d->clk = 0; d->eval(); d->clk = 1; d->eval(); }
#define CHECK(c, m) do { if (!(c)) { printf("  FAIL: %s\n", m); fails++; } else printf("  ok:   %s\n", m); } while (0)

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    d = new Vosd_logic;
    d->rst = 1; d->btn = 0; d->system_pause = 0; d->cpu_reset = 1; d->xram_stall = 0;
    for (int i = 0; i < 4; i++) tick();
    d->rst = 0; d->cpu_reset = 0;

    printf("[system_pause gate]\n");
    for (int i = 0; i < 4; i++) tick();
    int t0 = d->cpu_ce; tick(); int t1 = d->cpu_ce;
    CHECK(t0 != t1, "cpu_ce toggles while running");

    d->system_pause = 1; for (int i = 0; i < 6; i++) tick();
    int stuck = 1; for (int i = 0; i < 8; i++) { tick(); if (d->cpu_ce) stuck = 0; }
    CHECK(stuck, "cpu_ce frozen LOW while system_pause");

    int sid_fired = 0; for (int i = 0; i < 200; i++) { tick(); if (d->sid_ce_1m) sid_fired = 1; }
    CHECK(!sid_fired, "sid_ce_1m frozen while system_pause");

    d->system_pause = 0;
    int resumed = 0; for (int i = 0; i < 8; i++) { tick(); if (d->cpu_ce) resumed = 1; }
    CHECK(resumed, "cpu_ce resumes after pause released");
    int sid_resumed = 0; for (int i = 0; i < 200; i++) { tick(); if (d->sid_ce_1m) sid_resumed = 1; }
    CHECK(sid_resumed, "sid_ce_1m resumes after pause released");

    printf("[button debounce]\n");
    d->btn = 0x2; for (long i = 0; i < 300000; i++) tick();         // ~2.3 windows
    CHECK((d->btn_level & 0x2) != 0, "btn_level bit set after a stable press");
    d->btn = 0x0; for (long i = 0; i < 300000; i++) tick();
    CHECK((d->btn_level & 0x2) == 0, "btn_level bit clears after stable release");
    d->btn = 0x4; for (int i = 0; i < 50; i++) tick();             // a short glitch...
    d->btn = 0x0; for (long i = 0; i < 300000; i++) tick();        // ...gone before the window edge
    CHECK((d->btn_level & 0x4) == 0, "short glitch did NOT latch (debounced)");

    printf(fails ? "\n=== %d FAILED ===\n" : "\n=== ALL PASS ===\n", fails);
    delete d;
    return fails ? 1 : 0;
}
