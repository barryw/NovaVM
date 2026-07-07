/* nmouse.c - physical USB mouse for the NovaVM Linux host.
 *
 * Linux enumerates USB HID mice as evdev nodes. This reader owns the system
 * mouse position, writes pending VGC mouse registers, and lets the hardware
 * latch visible cursor changes at vblank. */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include "novavm.h"

#define VGC_MOUSE_XL     0xA0D0u
#define VGC_MOUSE_XH     0xA0D1u
#define VGC_MOUSE_Y      0xA0D2u
#define VGC_MOUSE_BTN    0xA0D3u
#define VGC_MOUSE_CTRL   0xA0D4u
#define VGC_MOUSE_COLOR  0xA0D5u
#define VGC_MOUSE_SHAPE  0xA0D6u
#define VGC_MOUSE_HOTX   0xA0D7u
#define VGC_MOUSE_HOTY   0xA0D8u

#define MOUSE_CTRL_ENABLE 0x01u
#define MOUSE_CTRL_AUTO   0x02u
#define MOUSE_SHAPE_SLOT  255u
#define MAX_MOUSE         8

static int mouse_x = 160;
static int mouse_y = 100;
static unsigned char buttons;
static int cursor_enabled;

/* ---- pointer shapes: 16x16, 0=transparent, 1=white fill, 0xB=dark outline.
 * Outline+fill stays visible on any background, so no AUTO contrast needed. ---- */
#define MOUSE_SLOT_ARROW  255u
#define MOUSE_SLOT_WAIT   254u
#define MOUSE_SLOT_IBEAM  253u

static const unsigned char cursor_arrow[128] = {
    0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x11, 0xB0, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x11, 0x1B, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x11, 0x11, 0xB0, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x11, 0x11, 0x1B, 0x00, 0x00, 0x00,
    0xB1, 0x11, 0x11, 0xBB, 0xBB, 0x00, 0x00, 0x00,
    0xB1, 0x1B, 0x11, 0xB0, 0x00, 0x00, 0x00, 0x00,
    0xB1, 0xB0, 0xB1, 0x1B, 0x00, 0x00, 0x00, 0x00,
    0xBB, 0x00, 0xB1, 0x1B, 0x00, 0x00, 0x00, 0x00,
    0xB0, 0x00, 0x0B, 0x11, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0B, 0x11, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char cursor_wait[128] = {
    0x00, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x00,
    0x00, 0xB1, 0x11, 0x11, 0x11, 0x11, 0x1B, 0x00,
    0x00, 0x0B, 0x11, 0x11, 0x11, 0x11, 0xB0, 0x00,
    0x00, 0x00, 0xB1, 0xBB, 0xBB, 0x1B, 0x00, 0x00,
    0x00, 0x00, 0x0B, 0x11, 0x11, 0xB0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0x1B, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0B, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0B, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0x1B, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0B, 0x1B, 0xB1, 0xB0, 0x00, 0x00,
    0x00, 0x00, 0xB1, 0xBB, 0xBB, 0x1B, 0x00, 0x00,
    0x00, 0x0B, 0x11, 0x11, 0x11, 0x11, 0xB0, 0x00,
    0x00, 0xB1, 0x11, 0x11, 0x11, 0x11, 0x1B, 0x00,
    0x00, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char cursor_ibeam[128] = {
    0x00, 0x00, 0xBB, 0xBB, 0xBB, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xB1, 0x11, 0x1B, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0B, 0xB1, 0xBB, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xB1, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0B, 0xB1, 0xBB, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xB1, 0x11, 0x1B, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xBB, 0xBB, 0xBB, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* ---- motion model (Phase 2 will expose speed/accel in the OSD) ---- */
static int accum_x, accum_y;      /* sub-pixel accumulators (1/256 px) */
static int mouse_gain  = 96;      /* base sensitivity: 1/256 px per count (256 = 1:1 logical
                                     = 2:1 screen; 96 ~= 0.75x screen). 8..1024. */
static int mouse_accel = 0;       /* acceleration strength 0..8 (0 = off). Fast flicks
                                     travel farther, gently and capped. */

#define MOUSE_PREFS_FILE "/data/nova/mouse.cfg"   /* persisted "gain accel" */

/* Live channel: the OSD writes /run/mouse_{gain,accel}; nmouse re-reads each poll. */
static int read_int_file(const char *path, int lo, int hi, int cur) {
    FILE *f = fopen(path, "r");
    if (!f) return cur;
    int v;
    if (fscanf(f, "%d", &v) == 1 && v >= lo && v <= hi) cur = v;
    fclose(f);
    return cur;
}
static void refresh_prefs(void) {
    mouse_gain  = read_int_file("/run/mouse_gain",  8, 1024, mouse_gain);
    mouse_accel = read_int_file("/run/mouse_accel", 0,    8, mouse_accel);
}

/* At boot, load the persisted prefs and seed the live /run files. */
static void load_mouse_prefs(void) {
    FILE *f = fopen(MOUSE_PREFS_FILE, "r");
    if (f) {
        int g, a;
        if (fscanf(f, "%d %d", &g, &a) == 2) {
            if (g >= 8 && g <= 1024) mouse_gain = g;
            if (a >= 0 && a <= 8)    mouse_accel = a;
        }
        fclose(f);
    }
    FILE *rg = fopen("/run/mouse_gain",  "w"); if (rg) { fprintf(rg, "%d\n", mouse_gain);  fclose(rg); }
    FILE *ra = fopen("/run/mouse_accel", "w"); if (ra) { fprintf(ra, "%d\n", mouse_accel); fclose(ra); }
}

static int errno_is_fatal(void) { return errno != EAGAIN && errno != EWOULDBLOCK; }

static int clamp_int(int value, int lo, int hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static int test_bit(const unsigned long *bits, int bit) {
    return (bits[bit / (8 * (int)sizeof(long))] & (1UL << (bit % (8 * (int)sizeof(long))))) != 0;
}

static int is_mouse(int fd) {
    unsigned long evbits[(EV_MAX + 1) / (8 * sizeof(long)) + 1];
    unsigned long relbits[(REL_MAX + 1) / (8 * sizeof(long)) + 1];
    unsigned long keybits[(KEY_MAX + 1) / (8 * sizeof(long)) + 1];

    memset(evbits, 0, sizeof evbits);
    memset(relbits, 0, sizeof relbits);
    memset(keybits, 0, sizeof keybits);
    if (ioctl(fd, EVIOCGBIT(0, sizeof evbits), evbits) < 0) return 0;
    if (!test_bit(evbits, EV_REL) || !test_bit(evbits, EV_KEY)) return 0;
    if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof relbits), relbits) < 0) return 0;
    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof keybits), keybits) < 0) return 0;
    return test_bit(relbits, REL_X) && test_bit(relbits, REL_Y) && test_bit(keybits, BTN_LEFT);
}

static void load_shape(unsigned slot, const unsigned char *sh) {
    /* A VMEM *write* is a single self-contained R_VMEM_DATA store carrying the
     * space, address and byte — NOT an R_VMEM_ADDR+R_VMEM_DATA pair (that form is
     * for reads). The old pair form wrote every byte to space 0 / addr 0, so the
     * shapes never reached the sprite RAM. Matches vmem_write() in novavm.c. */
    unsigned base = slot * 128u;
    for (unsigned i = 0; i < 128u; i++)
        wr(R_VMEM_DATA, ((unsigned)VGC_SPACE_SPRITE << 25) | (((base + i) & 0x1FFFFu) << 8) | sh[i]);
}

static void load_pointer_shapes(void) {
    vgc_bridge_lock();      /* serialise the VMEM burst vs a server screen dump/peek */
    load_shape(MOUSE_SLOT_ARROW, cursor_arrow);
    load_shape(MOUSE_SLOT_WAIT,  cursor_wait);
    load_shape(MOUSE_SLOT_IBEAM, cursor_ibeam);
    vgc_bridge_unlock();
}

/* Speed + acceleration + sub-pixel accumulation for one axis.
 * gain is in 1/256 px per mouse count; a fast flick raises gain so it travels
 * farther, and the accumulator carries the fractional remainder (no jump). */
/* Sub-pixel accumulation: fractional movement carries between reports so slow
 * motion is smooth and fast motion is proportional (no jumps). Optional gentle,
 * capped acceleration lets fast flicks travel farther without flinging. */
static void accel_move(int *pos, int *accum, int delta, int hi) {
    int gain = mouse_gain;
    if (mouse_accel) {
        int mag = delta < 0 ? -delta : delta;
        if (mag > 4) {
            int extra = (mag - 4) * mouse_accel;
            int cap = mouse_gain * 2;        /* total gain never exceeds ~3x base */
            if (extra > cap) extra = cap;
            gain += extra;
        }
    }
    *accum  += delta * gain;
    int step = *accum / 256;
    *accum  -= step * 256;
    *pos = clamp_int(*pos + step, 0, hi);
}

static void commit_mouse(void) {
    poke(VGC_MOUSE_XL, (unsigned char)(mouse_x & 0xFF));
    poke(VGC_MOUSE_XH, (unsigned char)((mouse_x >> 8) & 0x01));
    poke(VGC_MOUSE_Y, (unsigned char)mouse_y);
    poke(VGC_MOUSE_BTN, buttons);
#ifdef MOUSE_DEBUG
    { static FILE *ml; if (!ml) ml = fopen("/run/mouse.log", "w");
      if (ml) { fprintf(ml, "%d %d\n", mouse_x, mouse_y); fflush(ml); } }
#endif
}

static void init_cursor_shape(void) {
    load_pointer_shapes();
    poke(VGC_MOUSE_SHAPE, (unsigned char)MOUSE_SLOT_ARROW);
    poke(VGC_MOUSE_HOTX, 0);   /* arrow hotspot = tip */
    poke(VGC_MOUSE_HOTY, 0);
    commit_mouse();
    poke(VGC_MOUSE_CTRL, 0);
}

static void set_cursor_visible(int visible) {
    if (cursor_enabled == visible) return;
    cursor_enabled = visible;
    if (visible) load_pointer_shapes();   /* (re)load shapes here: mouse is detected after
                                             the boot VGC reset-clear, so they persist */
    poke(VGC_MOUSE_CTRL, visible ? MOUSE_CTRL_ENABLE : 0);
}

static void *mouse_thread(void *arg) {
    (void)arg;
    struct pollfd pfd[MAX_MOUSE];
    int fds[MAX_MOUSE], nfd = 0;

    load_mouse_prefs();
    init_cursor_shape();

    for (;;) {
        if (nfd == 0) {
            for (int i = 0; i < 256 && nfd < MAX_MOUSE; i++) {
                char path[32];
                snprintf(path, sizeof path, "/dev/input/event%d", i);
                int fd = open(path, O_RDONLY | O_NONBLOCK);
                if (fd < 0) continue;
                if (is_mouse(fd)) {
                    fds[nfd] = fd;
                    pfd[nfd].fd = fd; pfd[nfd].events = POLLIN;
                    nfd++;
                    set_cursor_visible(1);
                    printf("[novavm] mouse: %s\n", path);
                } else close(fd);
            }
            /* Re-assert the pointer shapes even with no mouse attached, so the
             * data is loaded and preserved regardless of runtime (the VGC
             * reset-clear at Zork's boot wipes the sprite RAM after our first
             * load; this reloads it and keeps it loaded). */
            if (nfd == 0) { load_pointer_shapes(); sleep(2); continue; }
        }

        int pr = poll(pfd, nfd, 2000);
        if (pr <= 0) {
            /* Mouse idle: same self-heal, unconditionally. */
            load_pointer_shapes();
            continue;
        }
        refresh_prefs();   /* pick up live OSD tuning from /run/mouse_{gain,accel} */
        for (int i = 0; i < nfd; i++) {
            if (!(pfd[i].revents & POLLIN)) continue;
            struct input_event ev;
            ssize_t n;
            int dirty = 0;
            while ((n = read(fds[i], &ev, sizeof ev)) == (ssize_t)sizeof ev) {
                if (ev.type == EV_REL) {
                    if (ev.code == REL_X) { accel_move(&mouse_x, &accum_x, ev.value, 319); dirty = 1; }
                    else if (ev.code == REL_Y) { accel_move(&mouse_y, &accum_y, ev.value, 199); dirty = 1; }
                } else if (ev.type == EV_KEY) {
                    unsigned char bit = 0;
                    if (ev.code == BTN_LEFT) bit = 0x01;
                    else if (ev.code == BTN_RIGHT) bit = 0x02;
                    else if (ev.code == BTN_MIDDLE) bit = 0x04;
                    if (bit) {
                        if (ev.value) buttons |= bit;
                        else buttons &= (unsigned char)~bit;
                        dirty = 1;
                    }
                } else if (ev.type == EV_SYN && ev.code == SYN_REPORT && dirty) {
                    commit_mouse();
                    dirty = 0;
                }
            }
            if (dirty) commit_mouse();
            if (n == 0 || (n < 0 && errno_is_fatal())) {
                close(fds[i]);
                for (int j = i; j < nfd - 1; j++) { fds[j] = fds[j+1]; pfd[j] = pfd[j+1]; }
                nfd--; i--;
                if (nfd == 0) set_cursor_visible(0);
            }
        }
    }
    return 0;
}

void mouse_init(void) {
    pthread_t t;
    if (pthread_create(&t, 0, mouse_thread, 0) == 0) pthread_detach(t);
}
