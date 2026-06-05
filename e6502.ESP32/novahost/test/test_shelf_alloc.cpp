#include "../shelf_alloc.h"
#include <cassert>
#include <cstdio>

int main() {
    // slot = index; base = SHELF_BASE + index*SLOT; tag table tracks id per slot.
    ShelfPlan p;
    shelf_plan_reset(&p);
    assert(shelf_plan_add(&p, /*id*/1) == 0);   // slot 0
    assert(shelf_plan_add(&p, /*id*/2) == 1);   // slot 1
    assert(p.base[0] == 0x060000u);
    assert(p.base[1] == 0x064000u);
    assert(p.tag[0] == 1 && p.tag[1] == 2);
    assert(p.tag[2] == 0 && p.tag[3] == 0);     // empty slots
    assert(p.lru[0] == 0 && p.lru[1] == 1 && p.lru[2] == 2 && p.lru[3] == 3);
    shelf_plan_add(&p, 3); shelf_plan_add(&p, 4);
    assert(shelf_plan_add(&p, 5) < 0);          // overflow past SHELF_N rejected
    uint8_t dir[SHELF_N * 2];
    shelf_plan_dir(&p, dir);
    assert(dir[0] == 1 && dir[1] == 2);          // tags first
    assert(dir[SHELF_N + 0] == 0 && dir[SHELF_N + 1] == 1);  // then lru
    printf("shelf_alloc OK\n");
    return 0;
}
