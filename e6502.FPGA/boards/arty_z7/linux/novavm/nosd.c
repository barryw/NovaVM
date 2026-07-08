/* nosd.c — On-Screen Display config menu. See nosd.h + OSD_CONFIG_SPEC.md.
 *
 * Architecture (step 1 = this file, host-side, no bitstream):
 *   - A char-grid "framebuffer" (the same model the HDMI-path compositor will
 *     consume): each cell is {char, fg, bg, transparent}.
 *   - A small menu FSM with a screen stack for back-navigation.
 *   - Data from the real filesystem: slots from boot.json (nbootcfg), disks from
 *     /data/nova/disks folders filtered by size to floppy/hdd.
 *   - The register backend (OSD_FB/OSD_CTRL/BUTTONS) is gated behind g_osd_hw,
 *     which stays 0 until the compositor bitstream lands; in step 1 the grid is
 *     dumped to stdout by osd_selftest.
 */
#include "nosd.h"
#include "novavm.h"
#include "nbootcfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

/* ---- grid geometry (720x480 / 8x16 font) ---- */
#define OSD_COLS 90
#define OSD_ROWS 30

typedef struct { uint8_t ch, fg, bg, transp; } osd_cell;
static osd_cell g_grid[OSD_ROWS][OSD_COLS];

/* NovaVM 16-colour palette indices (C64-ish): 0 black,1 white,6 blue,7 yellow,
 * 14 light-blue. Panel = white-on-blue; selected row = blue-on-white. */
#define C_PANEL_FG   1
#define C_PANEL_BG   6
#define C_TITLE_FG   7
#define C_HILITE_FG  6
#define C_HILITE_BG  1
#define C_DIM_FG     14

/* ---- mount slots: the 4 the OSD exposes (boot.json schema names) ---- */
#define NSLOT 4
static const char *SLOT[NSLOT] = { "fd0", "fd1", "hd0", "hd1" };
static int slot_is_floppy(int s) { return s < 2; }      /* fd0,fd1 floppy; hd0,hd1 hdd */
#define FLOPPY_MAX 1474560u                             /* 1.44 MB, inclusive */

/* ---- menu model ---- */
typedef enum { SC_NONE, SC_CONFIG, SC_SLOTS, SC_SLOTMENU, SC_DISKLIST, SC_AFTER_UNMOUNT, SC_MOUSE } osd_screen;

enum { IT_GOTO_SLOTS, IT_GOTO_SLOTMENU, IT_GOTO_DISKLIST, IT_GOTO_DISKDIR, IT_UNMOUNT, IT_MOUNT, IT_CANCEL, IT_CLOSE, IT_REBOOT_BASIC,
       IT_GOTO_MOUSE, IT_MOUSE_SPEED, IT_MOUSE_ACCEL, IT_VOLUME };

extern void audio_set_master(int pct);   /* naudio.c: SID+WTS mix-point level */

typedef struct { char label[48]; int kind; int arg; } osd_item;

#define MAXITEM 64
#define DISK_DIR_MAX 88
static osd_item   g_items[MAXITEM];
static int        g_nitem;
static int        g_cursor;

typedef struct { osd_screen sc; int arg; int cursor; } scframe;
static scframe    g_stack[8];
static int        g_sp;
static osd_screen g_sc = SC_NONE;
static int        g_sc_arg;

/* disk-list cache (built/rebuilt in SC_DISKLIST; indexes referenced by IT_MOUNT/IT_GOTO_DISKDIR) */
typedef struct { char name[64]; uint32_t size; int is_dir; int is_parent; } disk_ent;
static disk_ent   g_disks[256];
static int        g_ndisk;
static char       g_disk_dir[DISK_DIR_MAX];

static int        g_osd_hw   = 0;   /* set in step 3 once the compositor exists  */
static int        g_testmode = 0;   /* 1 = dry-run mount-boot (no board reset)    */

/* Hooks fulfilled in step 3 (audio mute + clean VM reboot). Declared weak so the
 * step-1 build links without naudio/novavm providing them yet. */
__attribute__((weak)) void naudio_set_mute(int on)  { (void)on; }
__attribute__((weak)) void host_reboot_vm(void)       { }
__attribute__((weak)) void host_reboot_to_basic(void) { }

/* ---------- framebuffer backend ---------- */

static void grid_clear(void)
{
    for (int r = 0; r < OSD_ROWS; r++)
        for (int c = 0; c < OSD_COLS; c++)
            g_grid[r][c] = (osd_cell){ ' ', 0, 0, 1 };   /* transparent */
}

static void put(int r, int c, uint8_t ch, uint8_t fg, uint8_t bg)
{
    if (r < 0 || r >= OSD_ROWS || c < 0 || c >= OSD_COLS) return;
    g_grid[r][c] = (osd_cell){ ch, fg, bg, 0 };          /* opaque */
}

static void put_str(int r, int c, const char *s, uint8_t fg, uint8_t bg)
{
    for (; *s && c < OSD_COLS; s++, c++) put(r, c, (uint8_t)*s, fg, bg);
}

/* Push the grid to the compositor (step 3) or, in step 1, leave it for the
 * dumper. OSD_FILL clears to transparent, then each opaque cell via OSD_FB. */
static void grid_present(void)
{
    if (!g_osd_hw) return;
    /* Write every cell (incl. transparent) — race-free, ~270us, only on redraw. */
    for (int r = 0; r < OSD_ROWS; r++) {
        for (int c = 0; c < OSD_COLS; c++) {
            osd_cell *e = &g_grid[r][c];
            uint32_t addr = (uint32_t)r * OSD_COLS + c;
            uint32_t w = ((uint32_t)(e->transp & 1) << 29)
                       | ((uint32_t)(e->bg & 0xF)   << 25)
                       | ((uint32_t)(e->fg & 0xF)   << 21)
                       | ((uint32_t)e->ch           << 13)
                       | (addr & 0x1FFF);
            wr(R_OSD_FB, w);
        }
    }
}

static void osd_set_ctrl(int enable, int pause)
{
    if (!g_osd_hw) return;
    uint32_t v = (enable ? 1u : 0u) | (pause ? 2u : 0u);
    wr(R_OSD_CTRL, v);
}

/* ---------- rendering ---------- */

static const char *screen_title(void)
{
    switch (g_sc) {
        case SC_CONFIG:   return "NovaVM Config";
        case SC_SLOTS:    return "Mounts";
        case SC_SLOTMENU: return SLOT[g_sc_arg];
        case SC_DISKLIST: return slot_is_floppy(g_sc_arg) ? "Select Floppy" : "Select Disk";
        case SC_AFTER_UNMOUNT: return "Disk Unmounted";
        case SC_MOUSE:    return "Mouse";
        default:          return "";
    }
}

static void render(void)
{
    grid_clear();
    if (g_sc == SC_NONE) { grid_present(); return; }

    /* panel width: fit the widest of title/items, padded; clamp to the grid */
    const char *title = screen_title();
    int w = (int)strlen(title);
    for (int i = 0; i < g_nitem; i++) {
        int l = (int)strlen(g_items[i].label);
        if (l > w) w = l;
    }
    int pw = w + 6;                              /* 2 margin + 1 border each side */
    if (pw < 22)          pw = 22;
    if (pw > OSD_COLS - 2) pw = OSD_COLS - 2;

    int avail_max = OSD_ROWS - 2 - 3;            /* rows usable for items */
    int avail = g_nitem < avail_max ? g_nitem : avail_max;
    int ph = avail + 3;                          /* title + blank + items */
    int pr = (OSD_ROWS - ph) / 2;
    int pc = (OSD_COLS - pw) / 2;

    /* solid panel fill */
    for (int r = 0; r < ph; r++)
        for (int c = 0; c < pw; c++)
            put(pr + r, pc + c, ' ', C_PANEL_FG, C_PANEL_BG);

    /* centred title on row 0 */
    put_str(pr, pc + (pw - (int)strlen(title)) / 2, title, C_TITLE_FG, C_PANEL_BG);

    /* scroll window keeps the cursor visible */
    int top = 0;
    if (g_nitem > avail) {
        top = g_cursor - avail / 2;
        if (top < 0) top = 0;
        if (top > g_nitem - avail) top = g_nitem - avail;
    }

    for (int i = 0; i < avail; i++) {
        int idx = top + i;
        if (idx >= g_nitem) break;
        int row = pr + 2 + i;
        int hl  = (idx == g_cursor);
        uint8_t fg = hl ? C_HILITE_FG : C_PANEL_FG;
        uint8_t bg = hl ? C_HILITE_BG : C_PANEL_BG;
        /* highlight spans the full inner width */
        for (int c = 1; c < pw - 1; c++) put(row, pc + c, ' ', fg, bg);
        put_str(row, pc + 2, g_items[idx].label, fg, bg);
    }

    /* scroll hints */
    if (top > 0)                  put(pr + 2,            pc + pw - 2, '^', C_DIM_FG, C_PANEL_BG);
    if (top + avail < g_nitem)    put(pr + 2 + avail - 1, pc + pw - 2, 'v', C_DIM_FG, C_PANEL_BG);

    grid_present();
}

/* ---------- data sources ---------- */

static int disk_cmp(const void *a, const void *b)
{
    const disk_ent *da = (const disk_ent *)a;
    const disk_ent *db = (const disk_ent *)b;
    if (da->is_parent != db->is_parent) return db->is_parent - da->is_parent;
    if (da->is_dir != db->is_dir)       return db->is_dir - da->is_dir;
    return strcasecmp(da->name, db->name);
}

static void disk_host_path(char *out, size_t n, const char *rel, const char *name)
{
    if (rel && rel[0] && name && name[0])
        snprintf(out, n, NOVA_FS_ROOT "/disks/%s/%s", rel, name);
    else if (rel && rel[0])
        snprintf(out, n, NOVA_FS_ROOT "/disks/%s", rel);
    else if (name && name[0])
        snprintf(out, n, NOVA_FS_ROOT "/disks/%s", name);
    else
        snprintf(out, n, NOVA_FS_ROOT "/disks");
}

static void disk_mount_path(char *out, size_t n, const disk_ent *e)
{
    if (g_disk_dir[0])
        snprintf(out, n, "/disks/%s/%s", g_disk_dir, e->name);
    else
        snprintf(out, n, "/disks/%s", e->name);
}

/* Scan the current /data/nova/disks folder for subdirectories plus *.ndi of the
 * requested kind (0=floppy ≤1.44MB, 1=hdd >1.44MB). */
static void disks_scan(int want_kind)
{
    g_ndisk = 0;
    if (g_disk_dir[0] && g_ndisk < (int)(sizeof g_disks / sizeof g_disks[0])) {
        snprintf(g_disks[g_ndisk].name, sizeof g_disks[0].name, "..");
        g_disks[g_ndisk].size = 0;
        g_disks[g_ndisk].is_dir = 1;
        g_disks[g_ndisk].is_parent = 1;
        g_ndisk++;
    }

    char scan[400];
    disk_host_path(scan, sizeof scan, g_disk_dir, NULL);
    DIR *d = opendir(scan);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d)) && g_ndisk < (int)(sizeof g_disks / sizeof g_disks[0])) {
        if (de->d_name[0] == '.') continue;
        if (strchr(de->d_name, '/')) continue;
        char full[400];
        disk_host_path(full, sizeof full, g_disk_dir, de->d_name);
        struct stat st;
        if (stat(full, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            snprintf(g_disks[g_ndisk].name, sizeof g_disks[0].name, "%.63s", de->d_name);
            g_disks[g_ndisk].size = 0;
            g_disks[g_ndisk].is_dir = 1;
            g_disks[g_ndisk].is_parent = 0;
            g_ndisk++;
            continue;
        }
        if (!S_ISREG(st.st_mode)) continue;
        const char *ext = strrchr(de->d_name, '.');
        if (!ext || strcasecmp(ext, ".ndi") != 0) continue;   /* disks only */
        int kind = ((uint32_t)st.st_size > FLOPPY_MAX) ? 1 : 0;
        if (kind != want_kind) continue;
        snprintf(g_disks[g_ndisk].name, sizeof g_disks[0].name, "%.63s", de->d_name);
        g_disks[g_ndisk].size = (uint32_t)st.st_size;
        g_disks[g_ndisk].is_dir = 0;
        g_disks[g_ndisk].is_parent = 0;
        g_ndisk++;
    }
    closedir(d);
    qsort(g_disks, g_ndisk, sizeof g_disks[0], disk_cmp);
}

static int slot_mounted(int s, char *path, size_t n)
{
    if (!bootcfg_mount_get(SLOT[s], path, n)) { if (n) path[0] = 0; return 0; }
    return path[0] != 0;
}

/* ---------- menu builders ---------- */

static void add_item(const char *label, int kind, int arg)
{
    if (g_nitem >= MAXITEM) return;
    snprintf(g_items[g_nitem].label, sizeof g_items[0].label, "%.47s", label);
    g_items[g_nitem].kind = kind;
    g_items[g_nitem].arg  = arg;
    g_nitem++;
}

/* ---- mouse tuning: persisted to /data/nova/mouse.cfg and pushed live to
 * /run/mouse_{gain,accel}, which nmouse re-reads each poll. ---- */
static int g_mgain  = 96;
static int g_maccel = 0;
static const int MGAIN_PRESETS[]  = {48, 64, 96, 128, 192, 256};
static const int MACCEL_PRESETS[] = {0, 1, 2, 4, 6, 8};
static int g_master_vol = 130;   /* % at the SID+WTS mix point */
static const int VOL_PRESETS[] = {50, 75, 100, 130, 160, 200};
#define MNPRE(a) ((int)(sizeof(a) / sizeof((a)[0])))

static int mnext(const int *a, int n, int cur) {
    for (int i = 0; i < n; i++) if (a[i] == cur) return a[(i + 1) % n];
    return a[0];
}
static void mouse_prefs_load(void) {
    FILE *f = fopen("/data/nova/mouse.cfg", "r");
    if (!f) return;
    int g, a;
    if (fscanf(f, "%d %d", &g, &a) == 2) { g_mgain = g; g_maccel = a; }
    fclose(f);
}
static void mouse_prefs_save(void) {
    FILE *f;
    if ((f = fopen("/run/mouse_gain",  "w"))) { fprintf(f, "%d\n", g_mgain);  fclose(f); }
    if ((f = fopen("/run/mouse_accel", "w"))) { fprintf(f, "%d\n", g_maccel); fclose(f); }
    if ((f = fopen("/data/nova/mouse.cfg", "w"))) { fprintf(f, "%d %d\n", g_mgain, g_maccel); fclose(f); }
}
static void vol_prefs_load(void) {
    FILE *f = fopen("/data/nova/volume.cfg", "r");
    if (!f) return;
    int v;
    if (fscanf(f, "%d", &v) == 1) g_master_vol = v;
    fclose(f);
}
static void vol_prefs_apply(void) { audio_set_master(g_master_vol); }
static void vol_prefs_save(void) {
    FILE *f;
    if ((f = fopen("/data/nova/volume.cfg", "w"))) { fprintf(f, "%d\n", g_master_vol); fclose(f); }
    vol_prefs_apply();
}
static void build_mouse(void)
{
    g_nitem = 0;
    char buf[48];
    int si = 0;
    for (int i = 0; i < MNPRE(MGAIN_PRESETS); i++) if (MGAIN_PRESETS[i] == g_mgain) si = i;
    snprintf(buf, sizeof buf, "Speed:   %d of %d", si + 1, MNPRE(MGAIN_PRESETS));
    add_item(buf, IT_MOUSE_SPEED, 0);
    if (g_maccel) snprintf(buf, sizeof buf, "Accel:   %d", g_maccel);
    else          snprintf(buf, sizeof buf, "Accel:   off");
    add_item(buf, IT_MOUSE_ACCEL, 0);
    add_item("Back", IT_CANCEL, 0);
}

static void build_config(void)
{
    g_nitem = 0;
    char vb[48];
    add_item("Mounts", IT_GOTO_SLOTS, 0);
    add_item("Mouse",  IT_GOTO_MOUSE, 0);
    snprintf(vb, sizeof vb, "Volume:  %d%%", g_master_vol);
    add_item(vb, IT_VOLUME, 0);
    add_item("Close",  IT_CLOSE,      0);
}

static void build_slots(void)
{
    g_nitem = 0;
    for (int s = 0; s < NSLOT; s++) {
        char path[160], line[48];
        if (!slot_mounted(s, path, sizeof path)) {
            snprintf(line, sizeof line, "%-3s  [ empty ]", SLOT[s]);
        } else {
            const char *bn = strrchr(path, '/'); bn = bn ? bn + 1 : path;
            snprintf(line, sizeof line, "%-3s  %.38s", SLOT[s], bn);
        }
        add_item(line, IT_GOTO_SLOTMENU, s);
    }
    add_item("Cancel", IT_CANCEL, 0);
}

static void build_slotmenu(int s)
{
    g_nitem = 0;
    char path[160];
    if (slot_mounted(s, path, sizeof path)) {
        add_item("Unmount", IT_UNMOUNT,       s);
        add_item("Replace", IT_GOTO_DISKLIST, s);
    } else {
        add_item("Mount...", IT_GOTO_DISKLIST, s);
    }
    add_item("Cancel", IT_CANCEL, 0);
}

static void build_disklist(int s)
{
    g_nitem = 0;
    disks_scan(slot_is_floppy(s) ? 0 : 1);
    for (int i = 0; i < g_ndisk; i++) {
        if (g_disks[i].is_dir) {
            char label[48];
            if (g_disks[i].is_parent)
                snprintf(label, sizeof label, "../");
            else
                snprintf(label, sizeof label, "%.45s/", g_disks[i].name);
            add_item(label, IT_GOTO_DISKDIR, i);
        } else {
            add_item(g_disks[i].name, IT_MOUNT, (s << 8) | i);
        }
    }
    if (g_ndisk == 0)
        add_item("(no disks of this type)", IT_CANCEL, 0);
    add_item("Cancel", IT_CANCEL, 0);
}

/* Shown right after an unmount: offer to drop back to BASIC now (a running disk
 * keeps running until a reboot), or stay in the OSD. */
static void build_after_unmount(int s)
{
    (void)s;
    g_nitem = 0;
    add_item("Reboot to BASIC", IT_REBOOT_BASIC, 0);
    add_item("Back",            IT_CANCEL,       0);
}

static void rebuild(void)
{
    switch (g_sc) {
        case SC_CONFIG:   build_config();          break;
        case SC_SLOTS:    build_slots();           break;
        case SC_SLOTMENU: build_slotmenu(g_sc_arg); break;
        case SC_DISKLIST: build_disklist(g_sc_arg); break;
        case SC_AFTER_UNMOUNT: build_after_unmount(g_sc_arg); break;
        case SC_MOUSE:    build_mouse();           break;
        default:          g_nitem = 0;             break;
    }
    if (g_cursor >= g_nitem) g_cursor = g_nitem ? g_nitem - 1 : 0;
    render();
}

/* ---------- navigation ---------- */

static void close_osd(void)
{
    g_sc = SC_NONE; g_sp = 0; g_nitem = 0; g_cursor = 0;
    render();                 /* transparent clear */
    osd_set_ctrl(0, 0);       /* disable overlay + release the global pause */
    naudio_set_mute(0);
}

static void open_osd(void)
{
    g_sp = 0; g_cursor = 0;
    g_sc = SC_CONFIG; g_sc_arg = 0;
    rebuild();                /* render -> write all cells while the overlay is still off */
    naudio_set_mute(1);
    osd_set_ctrl(1, 1);       /* enable overlay + pause the whole machine */
}

static void push_to(osd_screen sc, int arg)
{
    if (g_sp < (int)(sizeof g_stack / sizeof g_stack[0]))
        g_stack[g_sp++] = (scframe){ g_sc, g_sc_arg, g_cursor };
    g_sc = sc; g_sc_arg = arg; g_cursor = 0;
    rebuild();
}

static void pop_screen(void)
{
    if (g_sp > 0) {
        scframe f = g_stack[--g_sp];
        g_sc = f.sc; g_sc_arg = f.arg; g_cursor = f.cursor;
        rebuild();
    } else {
        close_osd();          /* Cancel at the top level closes */
    }
}

static void do_unmount(int s)
{
    bootcfg_mount_set(SLOT[s], "");
    printf("[osd] unmount %s\n", SLOT[s]);
}

static void do_mount_boot(int packed)
{
    int s   = (packed >> 8) & 0xFF;
    int idx = packed & 0xFF;
    if (idx < 0 || idx >= g_ndisk) return;
    char path[160];
    disk_mount_path(path, sizeof path, &g_disks[idx]);
    bootcfg_mount_set(SLOT[s], path);
    printf("[osd] mount %s = %s%s\n", SLOT[s], path, g_testmode ? "  (dry-run, no reboot)" : "");
    close_osd();
    if (!g_testmode) host_reboot_vm();   /* clean dbg_system_reset + re-stage */
}

static void disk_enter_dir(int idx)
{
    if (idx < 0 || idx >= g_ndisk || !g_disks[idx].is_dir) return;
    if (g_disks[idx].is_parent) {
        char *slash = strrchr(g_disk_dir, '/');
        if (slash) *slash = 0;
        else g_disk_dir[0] = 0;
    } else {
        size_t base = strlen(g_disk_dir);
        size_t name_len = strlen(g_disks[idx].name);
        size_t slash = base ? 1 : 0;
        if (base + slash + name_len >= sizeof g_disk_dir) return;
        if (slash) g_disk_dir[base++] = '/';
        memcpy(g_disk_dir + base, g_disks[idx].name, name_len + 1);
    }
    g_cursor = 0;
    rebuild();
}

static void select_item(void)
{
    if (g_cursor < 0 || g_cursor >= g_nitem) return;
    osd_item *it = &g_items[g_cursor];
    switch (it->kind) {
        case IT_GOTO_SLOTS:    push_to(SC_SLOTS, 0);           break;
        case IT_GOTO_SLOTMENU: push_to(SC_SLOTMENU, it->arg);  break;
        case IT_GOTO_DISKLIST: g_disk_dir[0] = 0; push_to(SC_DISKLIST, it->arg); break;
        case IT_GOTO_DISKDIR:  disk_enter_dir(it->arg);        break;
        case IT_UNMOUNT:       do_unmount(it->arg); push_to(SC_AFTER_UNMOUNT, it->arg); break;
        case IT_MOUNT:         do_mount_boot(it->arg);         break;
        case IT_REBOOT_BASIC:  close_osd(); if (!g_testmode) host_reboot_to_basic(); break;
        case IT_CANCEL:        pop_screen();                   break;
        case IT_CLOSE:         close_osd();                    break;
        case IT_GOTO_MOUSE:    push_to(SC_MOUSE, 0);           break;
        case IT_MOUSE_SPEED:   g_mgain  = mnext(MGAIN_PRESETS,  MNPRE(MGAIN_PRESETS),  g_mgain);
                               mouse_prefs_save(); rebuild();  break;
        case IT_MOUSE_ACCEL:   g_maccel = mnext(MACCEL_PRESETS, MNPRE(MACCEL_PRESETS), g_maccel);
                               mouse_prefs_save(); rebuild();  break;
        case IT_VOLUME:        g_master_vol = mnext(VOL_PRESETS, MNPRE(VOL_PRESETS), g_master_vol);
                               vol_prefs_save(); rebuild();    break;
    }
}

/* ---------- public API ---------- */

void osd_button(int b)
{
    if (g_sc == SC_NONE) {            /* closed: only the toggle does anything */
        if (b == 1) open_osd();
        return;
    }
    switch (b) {
        case 1: close_osd(); break;                                          /* close from anywhere */
        case 2: g_cursor = g_cursor > 0 ? g_cursor - 1 : g_nitem - 1; render(); break;  /* up, wrap   */
        case 3: g_cursor = g_cursor < g_nitem - 1 ? g_cursor + 1 : 0; render(); break;  /* down, wrap */
        case 4: select_item(); break;
    }
}

int osd_is_open(void) { return g_sc != SC_NONE; }

static void *osd_btn_thread(void *arg)
{
    (void)arg;
    for (;;) {
        uint32_t b = rd(R_BUTTONS);
        uint8_t edge = (b >> 4) & 0xF;        /* 1-shot press edges, cleared on read */
        for (int i = 0; i < 4; i++)
            if (edge & (1u << i)) osd_button(i + 1);   /* btn0->B1 .. btn3->B4 */
        usleep(16000);                         /* ~60 Hz */
    }
    return NULL;
}

void osd_init(void)
{
    if (!g_reg) return;
    /* Probe: the OSD-compositor bitstream returns {edges,level} (<=0xFF) from
     * R_BUTTONS; an older bitstream has no such register and reads 0xDEAD0000.
     * Only go live when the compositor is actually present. */
    if (rd(R_BUTTONS) == 0xDEAD0000u) {
        printf("[osd] no OSD compositor in bitstream — config menu disabled\n");
        return;
    }
    g_osd_hw = 1;
    mouse_prefs_load();   /* reflect persisted Speed/Accel in the menu labels */
    vol_prefs_load();     /* reflect + apply persisted master volume */
    vol_prefs_apply();
    pthread_t t;
    if (pthread_create(&t, NULL, osd_btn_thread, NULL) == 0) pthread_detach(t);
    printf("[osd] config menu ready (4 buttons live)\n");
}

/* ---------- step-1 selftest ---------- */

static void dump_grid(const char *caption)
{
    /* find the panel's bounding box of opaque cells */
    int r0 = OSD_ROWS, r1 = -1, c0 = OSD_COLS, c1 = -1;
    for (int r = 0; r < OSD_ROWS; r++)
        for (int c = 0; c < OSD_COLS; c++)
            if (!g_grid[r][c].transp) {
                if (r < r0) r0 = r; if (r > r1) r1 = r;
                if (c < c0) c0 = c; if (c > c1) c1 = c;
            }
    printf("\n--- %s ---\n", caption);
    if (r1 < 0) { printf("   (overlay hidden / transparent)\n"); return; }
    for (int r = r0; r <= r1; r++) {
        /* mark the highlighted (blue-on-white) row with '>' */
        int hl = 0;
        for (int c = c0; c <= c1; c++)
            if (!g_grid[r][c].transp && g_grid[r][c].bg == C_HILITE_BG && g_grid[r][c].fg == C_HILITE_FG) { hl = 1; break; }
        printf("  %c |", hl ? '>' : ' ');
        for (int c = c0; c <= c1; c++) {
            osd_cell *e = &g_grid[r][c];
            putchar(e->transp ? ' ' : (e->ch < 32 ? ' ' : e->ch));
        }
        printf("|\n");
    }
}

void osd_selftest(void)
{
    g_testmode = 1;               /* never actually reset the board */
    g_osd_hw   = 0;               /* dump, don't touch (absent) registers */

    printf("=== OSD selftest (disks=%s, mounts from boot.json) ===\n", NOVA_FS_ROOT "/disks");

    osd_button(1); dump_grid("B1: open -> Config");
    osd_button(4); dump_grid("B4: select Mounts -> Slots");
    osd_button(3); osd_button(3); dump_grid("B3 x2: cursor -> hd0");
    osd_button(4); dump_grid("B4: select hd0 -> Slot menu (empty)");
    osd_button(4); dump_grid("B4: Mount... -> Disk list (hdd: >1.44MB only)");
    osd_button(4); dump_grid("B4: select first hdd image -> mount + (dry-run) boot; OSD closes");

    osd_button(1); osd_button(4); dump_grid("reopen -> Slots: hd0 now shows the mount");
    osd_button(3); osd_button(3); osd_button(4); dump_grid("nav hd0 -> Slot menu (mounted: Unmount/Replace)");
    osd_button(4); dump_grid("B4: Unmount -> back to Slots (hd0 empty again)");

    /* floppy filter: fd0 lists only <=1.44MB images */
    osd_button(2); /* wrap up from Cancel? ensure on fd0 */
    /* go to top (fd0) deterministically: cursor currently somewhere in Slots */
    while (g_cursor > 0) osd_button(2);
    dump_grid("cursor -> fd0");
    osd_button(4); dump_grid("B4: select fd0 -> Slot menu");
    osd_button(4); dump_grid("B4: Mount... -> Disk list (floppy: <=1.44MB only)");
    osd_button(1); dump_grid("B1: close everything");

    printf("\n=== selftest complete ===\n");
}
