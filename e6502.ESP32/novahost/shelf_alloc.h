#pragma once
#include <stdint.h>
#include <string.h>

// Dynamic module shelf — pure boot-time slot allocator (host-testable, no Arduino).
// slot = sequential index; base = SHELF_BASE + slot*SHELF_SLOT_SIZE. Mirrors
// runtime/asm/libabi.inc (SHELF_BASE=$060000, SHELF_SLOT_WORDS=$2000=16KB, N=4).
#define SHELF_BASE       0x060000u
#define SHELF_SLOT_SIZE  0x4000u
#define SHELF_N          4

typedef struct {
    int      count;
    uint32_t base[SHELF_N];   // XRAM byte address per used slot
    uint8_t  tag[SHELF_N];    // module id per slot (0 = empty)
    uint8_t  lru[SHELF_N];    // slot indices, MRU front -> LRU back
} ShelfPlan;

static inline void shelf_plan_reset(ShelfPlan* p) {
    memset(p, 0, sizeof(*p));
    for (int i = 0; i < SHELF_N; i++) p->lru[i] = (uint8_t)i;
}

// Returns the assigned slot index, or -1 if the shelf is full.
static inline int shelf_plan_add(ShelfPlan* p, uint8_t id) {
    if (p->count >= SHELF_N) return -1;
    int slot = p->count++;
    p->base[slot] = SHELF_BASE + (uint32_t)slot * SHELF_SLOT_SIZE;
    p->tag[slot]  = id;
    return slot;
}

// Serialize the 6502 directory image: tag[N] then lru[N] (SHELF_N*2 bytes),
// matching SHELF_TAG ($0418) immediately followed by SHELF_LRU ($041C).
static inline void shelf_plan_dir(const ShelfPlan* p, uint8_t* out /*[SHELF_N*2]*/) {
    for (int i = 0; i < SHELF_N; i++) out[i] = p->tag[i];
    for (int i = 0; i < SHELF_N; i++) out[SHELF_N + i] = p->lru[i];
}
