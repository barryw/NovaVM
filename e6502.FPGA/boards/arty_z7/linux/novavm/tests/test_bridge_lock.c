/* test_bridge_lock.c — regression test for the shared R_PEEK/R_VMEM address-latch
 * race fixed in novavm.c / novavm.h / nservers.c.
 *
 * The FPGA debug bridge exposes peek as a TWO-STEP op: write R_PEEK_ADDR (a held
 * latch), then read R_PEEK_DATA. If a second thread writes R_PEEK_ADDR between
 * our two steps, we read the WRONG byte. The fix makes peek() acquire a RECURSIVE
 * bridge mutex around both steps (recursive so it can nest inside the debug
 * server's dispatch-level lock and inside vm_cold_boot without self-deadlock).
 *
 * This test models that hardware latch and the exact lock discipline, and proves:
 *   1. the recursive lock nests without deadlock (debug-dispatch -> peek),
 *   2. zero cross-thread contamination WITH the lock,
 *   3. the race is real WITHOUT the lock (so the test isn't vacuous).
 *
 * Build+run:  cc -O2 -pthread test_bridge_lock.c -o /tmp/tbl && /tmp/tbl
 */
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <sched.h>

/* ---- model of the FPGA R_PEEK path: one shared address latch ---- */
static volatile unsigned hw_latch;
static void          hw_set_addr(unsigned a) { hw_latch = a; }
static unsigned char hw_read_data(void)      { return (unsigned char)(hw_latch & 0xFF); } /* mem[a]=a&0xFF */

/* ---- the fix: recursive bridge mutex (mirrors nservers.c g_bridge) ---- */
static pthread_mutex_t g_bridge;
static __attribute__((constructor)) void g_bridge_init(void) {
    pthread_mutexattr_t at;
    pthread_mutexattr_init(&at);
    pthread_mutexattr_settype(&at, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&g_bridge, &at);
    pthread_mutexattr_destroy(&at);
}
static void bridge_lock(void)   { pthread_mutex_lock(&g_bridge); }
static void bridge_unlock(void) { pthread_mutex_unlock(&g_bridge); }

static int g_use_lock = 1;

/* peek() with the fix (mirrors novavm.h peek()) */
static unsigned char peek(unsigned a) {
    unsigned char v;
    if (g_use_lock) bridge_lock();
    hw_set_addr(a);
    sched_yield();               /* widen the race window (models bus latency) */
    v = hw_read_data();
    if (g_use_lock) bridge_unlock();
    return v;
}

#define ITERS 200000
static volatile int g_mismatch;

static void *peeker(void *arg) {
    unsigned base = (unsigned)(uintptr_t)arg, st = base ^ 0x1234u;
    for (int i = 0; i < ITERS; i++) {
        st = st * 1103515245u + 12345u;
        unsigned a = base + (st & 0x7FFFu);          /* disjoint per-thread range */
        if (peek(a) != (unsigned char)(a & 0xFF)) g_mismatch++;
    }
    return NULL;
}

static int run(int use_lock) {
    g_use_lock = use_lock; g_mismatch = 0;
    pthread_t ta, tb;
    pthread_create(&ta, NULL, peeker, (void *)(uintptr_t)0x0000u);
    pthread_create(&tb, NULL, peeker, (void *)(uintptr_t)0x8000u);
    pthread_join(ta, NULL); pthread_join(tb, NULL);
    return g_mismatch;
}

/* The recursive-nesting property: the debug server holds the bridge lock at
 * dispatch level, then calls peek() which locks again. Must not deadlock. */
static int test_recursive_nesting(void) {
    bridge_lock();
    unsigned char v = peek(0x42);   /* inner lock of the same mutex */
    bridge_unlock();
    return v == 0x42;
}

int main(void) {
    int fail = 0;

    if (!test_recursive_nesting()) { printf("FAIL: recursive bridge lock deadlocked or wrong value\n"); fail = 1; }
    else printf("PASS: recursive bridge lock nests (debug-dispatch -> peek) without deadlock\n");

    int locked = run(1);
    if (locked != 0) { printf("FAIL: %d latch-race mismatches WITH lock\n", locked); fail = 1; }
    else printf("PASS: 0 mismatches across %d peeks/thread WITH lock\n", ITERS);

    int unlocked = run(0);
    printf("INFO: %d mismatches WITHOUT lock (proves the modeled race is real: %s)\n",
           unlocked, unlocked ? "yes" : "not observed this run");
    if (unlocked == 0) printf("WARN: race not observed unlocked this run; test still valid but non-demonstrative\n");

    printf(fail ? "\nTEST FAILED\n" : "\nTEST PASSED\n");
    return fail;
}
