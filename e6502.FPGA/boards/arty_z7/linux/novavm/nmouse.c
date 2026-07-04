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

static unsigned char arrow_pixel(int x, int y) {
    if (y < 10 && x <= y) return 0x0F;
    if (y >= 10 && x >= 3 && x <= 5) return 0x0F;
    if (y >= 8 && y <= 12 && x >= 5 && x <= 8) return 0x0F;
    return 0x00;
}

static void load_default_pointer_shape(void) {
    unsigned base = MOUSE_SHAPE_SLOT * 128u;
    for (int y = 0; y < 16; y++) {
        for (int bx = 0; bx < 8; bx++) {
            unsigned char hi = arrow_pixel(bx * 2, y);
            unsigned char lo = arrow_pixel(bx * 2 + 1, y);
            wr(R_VMEM_ADDR, (VGC_SPACE_SPRITE << 17) | (base + (unsigned)y * 8u + (unsigned)bx));
            wr(R_VMEM_DATA, (unsigned char)((hi << 4) | lo));
        }
    }
}

static void commit_mouse(void) {
    poke(VGC_MOUSE_XL, (unsigned char)(mouse_x & 0xFF));
    poke(VGC_MOUSE_XH, (unsigned char)((mouse_x >> 8) & 0x01));
    poke(VGC_MOUSE_Y, (unsigned char)mouse_y);
    poke(VGC_MOUSE_BTN, buttons);
}

static void init_cursor_shape(void) {
    load_default_pointer_shape();
    poke(VGC_MOUSE_SHAPE, (unsigned char)MOUSE_SHAPE_SLOT);
    poke(VGC_MOUSE_HOTX, 0);
    poke(VGC_MOUSE_HOTY, 0);
    poke(VGC_MOUSE_COLOR, 0x0F);
    commit_mouse();
    poke(VGC_MOUSE_CTRL, 0);
}

static void set_cursor_visible(int visible) {
    if (cursor_enabled == visible) return;
    cursor_enabled = visible;
    poke(VGC_MOUSE_CTRL, visible ? (MOUSE_CTRL_ENABLE | MOUSE_CTRL_AUTO) : 0);
}

static void *mouse_thread(void *arg) {
    (void)arg;
    struct pollfd pfd[MAX_MOUSE];
    int fds[MAX_MOUSE], nfd = 0;

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
            if (nfd == 0) { sleep(2); continue; }
        }

        int pr = poll(pfd, nfd, 2000);
        if (pr <= 0) continue;
        for (int i = 0; i < nfd; i++) {
            if (!(pfd[i].revents & POLLIN)) continue;
            struct input_event ev;
            ssize_t n;
            int dirty = 0;
            while ((n = read(fds[i], &ev, sizeof ev)) == (ssize_t)sizeof ev) {
                if (ev.type == EV_REL) {
                    if (ev.code == REL_X) { mouse_x = clamp_int(mouse_x + ev.value, 0, 319); dirty = 1; }
                    else if (ev.code == REL_Y) { mouse_y = clamp_int(mouse_y + ev.value, 0, 199); dirty = 1; }
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
