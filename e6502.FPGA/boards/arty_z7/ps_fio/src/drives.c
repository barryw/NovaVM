// drives.c — Arty PS drive-slot mount table. See drives.h.

#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "xil_printf.h"
#include "drives.h"

static const char *SLOT_NAMES[DRIVE_SLOTS] = { "fd0", "fd1", "fd2", "fd3", "hd0", "hd1" };
static char g_path[DRIVE_SLOTS][160];

static const char *MOUNTS_FILE = "0:/config/mounts.txt";

int drive_slot_index(const char *name) {
    if (!name) return -1;
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (strcasecmp(name, SLOT_NAMES[i]) == 0) return i;
    return -1;
}

const char *drive_slot_name(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? SLOT_NAMES[idx] : 0;
}

const char *drive_path(int idx) {
    return (idx >= 0 && idx < DRIVE_SLOTS) ? g_path[idx] : "";
}

int drive_boot_slot(void) {
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (g_path[i][0]) return i;          // fd0..fd3 then hd0,hd1
    return -1;
}

// Persist all mounted slots as "name path" lines.
static void drives_save(void) {
    f_mkdir("0:/config");                     // ignore FR_EXIST
    FIL f;
    if (f_open(&f, MOUNTS_FILE, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        xil_printf("[drv] WARN: cannot write %s\r\n", MOUNTS_FILE);
        return;
    }
    char line[200];
    UINT bw;
    for (int i = 0; i < DRIVE_SLOTS; i++) {
        if (!g_path[i][0]) continue;
        int n = snprintf(line, sizeof line, "%s %s\n", SLOT_NAMES[i], g_path[i]);
        f_write(&f, line, n, &bw);
    }
    f_close(&f);
}

void drive_set(int idx, const char *path) {
    if (idx < 0 || idx >= DRIVE_SLOTS) return;
    snprintf(g_path[idx], sizeof g_path[idx], "%s", path ? path : "");
    drives_save();
}

void drives_load(void) {
    for (int i = 0; i < DRIVE_SLOTS; i++) g_path[i][0] = 0;
    FIL f;
    if (f_open(&f, MOUNTS_FILE, FA_READ) != FR_OK) return;   // no mounts yet
    static char buf[4096];
    UINT br = 0;
    f_read(&f, buf, sizeof buf - 1, &br);
    f_close(&f);
    buf[br] = 0;

    // Parse "name path\n" lines in place.
    char *p = buf;
    while (*p) {
        char *eol = strpbrk(p, "\r\n");
        if (eol) *eol = 0;
        char *sp = strchr(p, ' ');
        if (sp) {
            *sp = 0;
            int idx = drive_slot_index(p);
            const char *path = sp + 1;
            if (idx >= 0 && path[0]) snprintf(g_path[idx], sizeof g_path[idx], "%s", path);
        }
        if (!eol) break;
        p = eol + 1;
        while (*p == '\r' || *p == '\n') p++;
    }
    for (int i = 0; i < DRIVE_SLOTS; i++)
        if (g_path[i][0]) xil_printf("[drv] %s = %s\r\n", SLOT_NAMES[i], g_path[i]);
}
