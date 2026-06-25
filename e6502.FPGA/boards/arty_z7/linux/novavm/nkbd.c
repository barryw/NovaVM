/* nkbd.c — physical USB keyboard for the NovaVM Linux host.
 *
 * The bare-metal host (ps_fio/src/usb.c) drives the Zynq USB host controller +
 * a HID parser directly. On Linux the kernel already enumerates the USB HID
 * keyboard as an evdev node (/dev/input/event*); we just read EV_KEY events,
 * map Linux keycodes -> ASCII (US layout, with Shift/CapsLock), and inject into
 * the VGC key FIFO via R_KEY — same destination as `novavm type`. Ctrl held is
 * mirrored to AUTOBOOT_SKIP ($B9F0) each poll so holding Ctrl at boot skips
 * autoboot, matching usb.c's kbd_report() behavior. */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <poll.h>
#include <errno.h>
#include <linux/input.h>
#include "novavm.h"

/* read() on a yanked evdev node returns ENODEV/EIO; EAGAIN just means drained. */
static int errno_is_fatal(void) { return errno != EAGAIN && errno != EWOULDBLOCK; }

/* US keymap indexed by Linux keycode (linux/input-event-codes.h), 0..63. */
static const char km_base[64] = {
    /*0*/0,   27, '1','2','3','4','5','6','7','8','9','0','-','=', 8, '\t',
    /*16*/'q','w','e','r','t','y','u','i','o','p','[',']', '\r', 0, 'a','s',
    /*32*/'d','f','g','h','j','k','l',';','\'','`', 0, '\\','z','x','c','v',
    /*48*/'b','n','m',',','.','/', 0, '*', 0, ' ', 0,  0,  0,  0,  0,  0
};
static const char km_shift[64] = {
    /*0*/0,   27, '!','@','#','$','%','^','&','*','(',')','_','+', 8, '\t',
    /*16*/'Q','W','E','R','T','Y','U','I','O','P','{','}', '\r', 0, 'A','S',
    /*32*/'D','F','G','H','J','K','L',':','"','~', 0, '|','Z','X','C','V',
    /*48*/'B','N','M','<','>','?', 0, '*', 0, ' ', 0,  0,  0,  0,  0,  0
};

#define MAX_KBD 8

/* Does this evdev node look like a keyboard? (reports a span of letter keys.) */
static int is_keyboard(int fd) {
    unsigned long bits[(KEY_MAX + 1) / (8 * sizeof(long)) + 1];
    memset(bits, 0, sizeof bits);
    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof bits), bits) < 0) return 0;
    #define HASKEY(k) (bits[(k) / (8 * sizeof(long))] & (1UL << ((k) % (8 * sizeof(long)))))
    return HASKEY(KEY_A) && HASKEY(KEY_Z) && HASKEY(KEY_ENTER);
    #undef HASKEY
}

static void *kbd_thread(void *arg) {
    (void)arg;
    struct pollfd pfd[MAX_KBD];
    int fds[MAX_KBD], nfd = 0;
    int shift = 0, caps = 0, ctrl = 0;

    /* (Re)scan /dev/input for keyboards; retry forever so hot-plug works. */
    for (;;) {
        if (nfd == 0) {
            for (int i = 0; i < 256 && nfd < MAX_KBD; i++) {
                char path[32];
                snprintf(path, sizeof path, "/dev/input/event%d", i);
                int fd = open(path, O_RDONLY | O_NONBLOCK);
                if (fd < 0) continue;
                if (is_keyboard(fd)) {
                    fds[nfd] = fd;
                    pfd[nfd].fd = fd; pfd[nfd].events = POLLIN;
                    nfd++;
                    printf("[novavm] keyboard: %s\n", path);
                } else close(fd);
            }
            if (nfd == 0) { sleep(2); continue; }   /* no keyboard yet — wait + retry */
        }

        int pr = poll(pfd, nfd, 2000);
        if (pr <= 0) continue;
        for (int i = 0; i < nfd; i++) {
            if (!(pfd[i].revents & POLLIN)) continue;
            struct input_event ev;
            ssize_t n;
            while ((n = read(fds[i], &ev, sizeof ev)) == (ssize_t)sizeof ev) {
                if (ev.type != EV_KEY) continue;
                int code = ev.code, down = (ev.value != 0);   /* 1=press 2=repeat */
                switch (code) {
                    case KEY_LEFTSHIFT: case KEY_RIGHTSHIFT: shift = down; continue;
                    case KEY_LEFTCTRL:  case KEY_RIGHTCTRL:
                        ctrl = down; poke(AUTOBOOT_SKIP_ADDR, ctrl ? 1 : 0); continue;
                    case KEY_CAPSLOCK:  if (ev.value == 1) caps = !caps; continue;
                }
                if (!down || code < 0 || code >= 64) continue;
                char c = (shift ? km_shift : km_base)[code];
                if (!c) continue;
                /* CapsLock affects letters only; Ctrl maps A-Z -> control codes. */
                if (caps && c >= 'a' && c <= 'z') c -= 32;
                else if (caps && c >= 'A' && c <= 'Z') c += 32;
                if (ctrl) { if (c >= 'a' && c <= 'z') c &= 0x1F; else if (c >= 'A' && c <= 'Z') c &= 0x1F; }
                wr(R_KEY, (unsigned char)c);
            }
            if (n == 0 || (n < 0 && errno_is_fatal())) {  /* device vanished — rescan */
                close(fds[i]);
                for (int j = i; j < nfd - 1; j++) { fds[j] = fds[j+1]; pfd[j] = pfd[j+1]; }
                nfd--; i--;
            }
        }
    }
    return 0;
}

void kbd_init(void) {
    pthread_t t;
    if (pthread_create(&t, 0, kbd_thread, 0) == 0) pthread_detach(t);
}
