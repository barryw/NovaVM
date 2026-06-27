/* novavm — NovaVM host on Zynq Linux (userspace port of bare-metal ps_fio).
 *
 * Loads the EHBASIC ROM through the fio_bridge, releases the 6502 (cold-starts
 * to NovaBASIC), then runs the FIO service loop: file LOAD/SAVE/DIR mapped to
 * the Linux nova-fs, and the autoboot probe (LOAD "AUTOBOOT" -> not found ->
 * BASIC drops to Ready). Talks to the PL via an mmap'd register window.
 *
 * Ported faithfully from ps_fio/src/main.c (same FIO protocol, same poke/peek).
 * TODO (next): drives(.ndi), soundfonts, runtimes, key injection (evdev->R_KEY),
 * the nova mgmt/debug/upload servers, audio.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include "loader_bin.h"     /* LOADER_BIN[248]: resident lib_call loader, staged @ $0320 */
#include "modules_embedded.h" /* EMBEDDED_MOD[1..8], 16KB each (3=MOD_SYSTEM: line input) */
#include "nfio.h"            /* XRAM/file/.ndi-drive FIO commands */
#include "nosd.h"            /* OSD config menu: buttons + mount/unmount UI */

#define R_AUDIO 0x30u        /* W [7:0] -> push 1 PCM byte to the HDMI audio FIFO */
#define R_AUDIO_SPACE 0x34u  /* R [15:0] -> free bytes in the HDMI audio FIFO     */

/* ---- fio_bridge registers (M_AXI_GP0 @ 0x40000000) ---- */
#define FIO_BASE 0x40000000u
#define R_POKE 0x00u
#define R_PEEK_ADDR 0x04u
#define R_PEEK_DATA 0x08u
#define R_KEY 0x0Cu
#define R_CTRL 0x10u
#define R_STATUS 0x14u
#define R_CPU_PC 0x18u
#define R_ROMW 0x24u
#define R_VMEM_ADDR 0x28u
#define R_VMEM_DATA 0x2Cu
#define CTRL_CPU_RESET 0x1u
#define CTRL_SYS_RESET 0x2u   /* full custom-chip reset: VGC/SID/WTS/math */
#define VGC_SPACE_CHAR 1u
#define XRAM_DDR_BASE 0x10000000u   /* PS-DDR XRAM shelf (DT reserved-memory @ 0x10000000) */
#define XRAM_BYTES    0x100000u     /* 1 MB carve-out */

/* ---- FIO mailbox in 6502 RAM ($B9A0+) ---- */
#define FIO_CMD 0xB9A0
#define FIO_STATUS 0xB9A1
#define FIO_ERRCODE 0xB9A2
#define FIO_NAMELEN 0xB9A3
#define FIO_SRC_LO 0xB9A4
#define FIO_SRC_HI 0xB9A5
#define FIO_END_LO 0xB9A6
#define FIO_END_HI 0xB9A7
#define FIO_SIZE_LO 0xB9A8
#define FIO_SIZE_HI 0xB9A9
#define FIO_DIRTYPE 0xB9AF
#define FIO_NAME 0xB9B0
#define AUTOBOOT_SKIP_ADDR 0xB9F0

#define FIO_OK 0x02
#define FIO_ERR 0x03
#define FIO_CMD_SAVE 0x01
#define FIO_CMD_LOAD 0x02
#define FIO_CMD_DIROPEN 0x03
#define FIO_CMD_DIRREAD 0x04
#define FIO_CMD_DELETE 0x05
#define FIO_CMD_LOAD_MODULE 0x2C
#define MODULE_BYTES 16384u
#define FIO_CMD_VOLUME  0x0C   /* -> naudio fio_volume  */
#define FIO_CMD_MIDPLAY 0x13   /* -> naudio fio_midplay */
#define FIO_CMD_MIDSTOP 0x14   /* -> naudio fio_midstop */
#define FIO_CMD_SFLOAD  0x15   /* -> naudio fio_sfload  */
#define FIO_CMD_XLOAD       0x18   /* -> nfio (file -> XRAM)              */
#define FIO_CMD_LOADRUNTIME 0x28   /* -> nfio (16KB ROM -> $C000)         */
#define FIO_CMD_XPAGE       0x29   /* -> nfio (file slice -> XRAM/CPU RAM)*/
#define FIO_CMD_FOPEN       0x2D   /* -> nfio file handles               */
#define FIO_CMD_FCREATE     0x2E
#define FIO_CMD_FCLOSE      0x2F
#define FIO_CMD_FREAD       0x30
#define FIO_CMD_FWRITE      0x31
#define FIO_CMD_FSEEK       0x32
#define FIO_CMD_FTELL       0x33
#define FIO_CMD_FSIZE       0x34
#define FIO_ERR_NOTFOUND 1
#define FIO_ERR_IO 2
#define FIO_ERR_EOD 3
#define DT_BAS 0
#define DT_BIN 2
#define DT_DIR 5

#define PROG_DIR "/data/nova/programs"

volatile uint32_t *g_reg;   /* non-static: shared with naudio.c/nservers.c/nkbd.c via novavm.h */
volatile uint8_t  *g_xram;  /* mmap'd PS-DDR XRAM shelf (nfio.c uses it), NULL if unreserved */
void kbd_init(void);        /* nkbd.c: physical USB keyboard reader thread */
void audio_init(void); void audio_start(void);              /* naudio.c */
void fio_volume(void); void fio_midplay(void); void fio_midstop(void); void fio_sfload(void);
void servers_init(void);    /* nservers.c: mgmt 6504 / debug 6503 / upload 6502 */
void boot_splash(void);     /* nsplash.c: render boot.json logo + fade (6502 held) */
static unsigned char g_fbuf[0x10000 + 4];
static DIR *g_dir;

/* NOTE: there used to be a set_fclk0_125mhz() here that poked the PS SLCR to
 * retune FCLK0. It is GONE: the PL design is clocked from the board's clean H16
 * 125 MHz oscillator, NOT FCLK0 (verified — poking FCLK0 to any value leaves the
 * 6502 running). The poke's only real effect was an MMCM re-lock / brief PL reset
 * that raced the boot-time autoboot and broke floppy LOADRUNTIME. Removing it
 * fixes cold-boot floppy launch and costs nothing (FCLK0 is unused). */

static inline void wr(unsigned o, uint32_t v) { g_reg[o >> 2] = v; }
static inline uint32_t rd(unsigned o) { return g_reg[o >> 2]; }
static inline void poke(unsigned a, unsigned char d) { wr(R_POKE, ((a & 0xFFFFu) << 8) | d); }
static inline unsigned char peek(unsigned a) { wr(R_PEEK_ADDR, a & 0xFFFFu); return (unsigned char)rd(R_PEEK_DATA); }
static inline void cpu_hold(int h) { wr(R_CTRL, h ? CTRL_CPU_RESET : 0u); }
static inline int fio_pending(void) { return rd(R_STATUS) & 1u; }
static inline void fio_clear(void) { wr(R_STATUS, 1u); }
static inline void fio_ok(void) { poke(FIO_ERRCODE, 0); poke(FIO_STATUS, FIO_OK); poke(FIO_CMD, 0); }
static inline void fio_fail(int c) { poke(FIO_ERRCODE, c); poke(FIO_STATUS, FIO_ERR); poke(FIO_CMD, 0); }

static unsigned char vmem_read(unsigned sp, unsigned a) {
    wr(R_VMEM_ADDR, ((sp & 7u) << 17) | (a & 0x1FFFFu));
    (void)rd(R_VMEM_DATA);
    return (unsigned char)rd(R_VMEM_DATA);
}

static int fio_read_name(char *out, int maxlen) {
    int n = peek(FIO_NAMELEN);
    if (n < 1 || n > 63 || n >= maxlen) return -1;
    for (int i = 0; i < n; i++) out[i] = (char)peek(FIO_NAME + i);
    out[n] = 0;
    while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == 0)) out[--n] = 0;
    return n;
}

/* "<name>" -> /data/nova/programs/<name> (+ .bas if no extension). */
static void fio_path(const char *name, char *path, int sz) {
    if (strrchr(name, '.')) snprintf(path, sz, "%s/%s", PROG_DIR, name);
    else snprintf(path, sz, "%s/%s.bas", PROG_DIR, name);
}

static int has_ext(const char *p, const char *ext) {
    const char *d = strrchr(p, '.');
    return d && !strcasecmp(d, ext);
}

/* Set by the OSD "reboot to BASIC" action; fio_load consumes it once to refuse
 * the autoboot probe so the next cold start lands at the BASIC prompt. */
static volatile int g_boot_to_basic = 0;

static void fio_load(void) {
    char name[80], path[160];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }

    /* OSD "reboot to BASIC": refuse the autoboot probe once, regardless of what
     * is still mounted -> BASIC sees not-found and drops to the prompt. */
    if (g_boot_to_basic && strlen(name) == 8 && !strcasecmp(name, "AUTOBOOT")) {
        g_boot_to_basic = 0;
        fio_fail(FIO_ERR_NOTFOUND);
        return;
    }

    /* A mounted .ndi boot image takes priority (games/runtimes): resolve the name
     * inside it first; nfio_image_load loads + sets FIO_SRC/SIZE/DIRTYPE on a hit.
     * -1 = nothing mounted or not in the image -> fall through to the SD load. */
    if (nfio_image_load(name) == 0) { fio_ok(); return; }

    /* AUTOBOOT contract: bare "AUTOBOOT" -> AUTOBOOT.BIN then AUTOBOOT.BAS. */
    if (strlen(name) == 8 && !strcasecmp(name, "AUTOBOOT")) {
        char t[160];
        snprintf(t, sizeof t, "%s/AUTOBOOT.BIN", PROG_DIR);
        if (access(t, R_OK) == 0) strcpy(name, "AUTOBOOT.BIN");
        else strcpy(name, "AUTOBOOT.BAS");
    }

    fio_path(name, path, sizeof path);
    int is_bin = has_ext(path, ".bin");

    FILE *f = fopen(path, "rb");
    if (!f) { fio_fail(FIO_ERR_NOTFOUND); return; }
    size_t br = fread(g_fbuf, 1, sizeof g_fbuf, f);
    fclose(f);
    if (br < 2) { fio_fail(FIO_ERR_IO); return; }

    unsigned dst = is_bin ? (g_fbuf[0] | (g_fbuf[1] << 8))
                          : (peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8));
    unsigned len = (unsigned)br - 2;
    if (dst + len > 0x10000) { fio_fail(FIO_ERR_IO); return; }
    for (unsigned i = 0; i < len; i++) poke((dst + i) & 0xFFFF, g_fbuf[2 + i]);
    if (is_bin) { poke(FIO_SRC_LO, dst & 0xFF); poke(FIO_SRC_HI, (dst >> 8) & 0xFF); }
    poke(FIO_SIZE_LO, len & 0xFF); poke(FIO_SIZE_HI, (len >> 8) & 0xFF);
    poke(FIO_DIRTYPE, is_bin ? DT_BIN : DT_BAS);
    printf("[fio] LOAD %s -> $%04x (%u bytes)\n", path, dst, len);
    fio_ok();
}

static void fio_save(void) {
    char name[80], path[160];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    fio_path(name, path, sizeof path);
    unsigned src = peek(FIO_SRC_LO) | (peek(FIO_SRC_HI) << 8);
    unsigned end = peek(FIO_END_LO) | (peek(FIO_END_HI) << 8);
    if (end <= src) { fio_fail(FIO_ERR_IO); return; }
    unsigned len = end - src;
    g_fbuf[0] = src & 0xFF; g_fbuf[1] = (src >> 8) & 0xFF;
    for (unsigned i = 0; i < len; i++) g_fbuf[2 + i] = peek((src + i) & 0xFFFF);
    mkdir(PROG_DIR, 0775);
    FILE *f = fopen(path, "wb");
    if (!f) { fio_fail(FIO_ERR_IO); return; }
    size_t bw = fwrite(g_fbuf, 1, len + 2, f);
    fclose(f);
    if (bw != len + 2) { fio_fail(FIO_ERR_IO); return; }
    printf("[fio] SAVE %s ($%04x-$%04x, %u bytes)\n", path, src, end, len);
    fio_ok();
}

static void fio_diropen(void) {
    if (g_dir) closedir(g_dir);
    g_dir = opendir(PROG_DIR);
    if (!g_dir) { fio_fail(FIO_ERR_IO); return; }
    fio_ok();
}

static void fio_dirread(void) {
    if (!g_dir) { fio_fail(FIO_ERR_EOD); return; }
    struct dirent *de;
    while ((de = readdir(g_dir))) {
        if (de->d_name[0] == '.') continue;
        char disp[64]; int dl = 0;
        for (; de->d_name[dl] && dl < 63; dl++) disp[dl] = de->d_name[dl];
        disp[dl] = 0;
        int type = has_ext(disp, ".bin") ? DT_BIN : DT_BAS;
        char *dot = strrchr(disp, '.'); if (dot) { *dot = 0; dl = (int)(dot - disp); }
        poke(FIO_DIRTYPE, type);
        poke(FIO_NAMELEN, dl);
        for (int i = 0; i < dl; i++) poke(FIO_NAME + i, disp[i]);
        fio_ok();
        return;
    }
    closedir(g_dir); g_dir = NULL;
    fio_fail(FIO_ERR_EOD);
}

static void fio_delete(void) {
    char name[80], path[160];
    if (fio_read_name(name, sizeof name) < 0) { fio_fail(FIO_ERR_IO); return; }
    fio_path(name, path, sizeof path);
    if (unlink(path) != 0) { fio_fail(FIO_ERR_NOTFOUND); return; }
    fio_ok();
}

static void dump_screen(void) {
    for (int row = 0; row < 50; row++) {
        char line[81]; int any = 0;
        for (int col = 0; col < 80; col++) {
            unsigned char ch = vmem_read(VGC_SPACE_CHAR, (unsigned)(row * 80 + col));
            if (ch >= 0x21 && ch < 0x7F) any = 1;
            line[col] = (ch >= 0x20 && ch < 0x7F) ? (char)ch : ' ';
        }
        line[80] = 0;
        if (any) printf("%02d|%s|\n", row, line);
    }
}

/* Keep the PL's HDMI audio FIFO fed with silence so the HDMI audio data islands
 * stay valid — strict sinks (Elgato capture) reject a stream whose audio FIFO
 * underruns; a TV tolerates it. (Real audio mixing replaces this later.) */
static void *audio_feed(void *arg) {
    (void)arg;
    for (;;) {
        unsigned space = rd(R_AUDIO_SPACE) & 0xFFFF;
        for (unsigned i = 0; i < space; i++) wr(R_AUDIO, 0);
        usleep(1000);
    }
    return NULL;
}

static int load_rom(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); return -1; }
    unsigned char rom[16384];
    size_t n = fread(rom, 1, sizeof rom, f);
    fclose(f);
    if (n != sizeof rom) { fprintf(stderr, "ROM short %zu\n", n); return -1; }
    cpu_hold(1);
    for (unsigned a = 0; a < 16384; a++) wr(R_ROMW, (a << 8) | rom[a]);
    printf("[novavm] ROM loaded (%zu bytes)\n", n);
    return 0;
}

/* Page an embedded 16KB SYSTEM/library module into ext_rom bank-1 (R_ROMW idx=1).
 * The resident loader ($0320) requests this via FIO_CMD_LOAD_MODULE when BASIC
 * lib-calls a routine (e.g. SYS_SCREEN_READLINE, the line editor) that lives in a
 * paged module. Mirrors ps_fio load_module() (main.c:357). id 1..8 per EMBEDDED_MOD. */
static int load_module(int id) {
    if (id < 1 || id > 8 || !EMBEDDED_MOD[id]) {
        fprintf(stderr, "[novavm] module %d not embedded\n", id);
        return -1;
    }
    const unsigned char *img = EMBEDDED_MOD[id];
    for (unsigned a = 0; a < MODULE_BYTES; a++)
        wr(R_ROMW, (1u << 22) | (a << 8) | img[a]);   /* idx=1 -> ext_rom bank */
    printf("[novavm] module %d -> bank-1 (%u bytes)\n", id, MODULE_BYTES);
    return 0;
}

/* "Please hold" wait screen — painted the instant the PL is up (via the init
 * script's `novavm bootmsg`), so the display is never blank during the slow Linux
 * boot. Writes char + colour RAM through the vmem port and sets the text regs; no
 * 6502 needed. The subsequent cold-boot resets the VGC, clearing this, and the
 * real boot splash takes over. */
#define VGC_SPACE_COLOR 2u
static void vmem_write(unsigned sp, unsigned a, unsigned char v) {
    wr(R_VMEM_DATA, ((sp & 7u) << 25) | ((a & 0x1FFFFu) << 8) | v);
}
static void boot_wait_msg(void) {
    poke(0xA000, 0x00);   /* VGCR_MODE   = text  */
    poke(0xA001, 0x06);   /* VGCR_BGCOL  = blue  */
    poke(0xA00D, 0x06);   /* VGCR_BORDER = blue  */
    poke(0xA00A, 0x00);   /* cursor off          */
    poke(0xA0E5, 0x0F);   /* VGCR_DIM    = full  */
    for (unsigned a = 0; a < 80u * 50u; a++) {
        vmem_write(VGC_SPACE_CHAR,  a, 0x20);   /* space */
        vmem_write(VGC_SPACE_COLOR, a, 0x01);   /* white */
    }
    static const char *L[] = {
        "N O V A V M",
        "",
        "",
        "Your call is VERY important to us!",
        "",
        "Please remain on the line and an operator will",
        "be with you just as soon as possible.",
        "",
        "",
        ". . . booting . . .",
    };
    int n = (int)(sizeof L / sizeof L[0]);
    int top = (50 - n) / 2;
    for (int r = 0; r < n; r++) {
        const char *s = L[r];
        int len = (int)strlen(s);
        int col = (80 - len) / 2;
        unsigned base = (unsigned)((top + r) * 80 + col);
        for (int j = 0; j < len; j++)
            vmem_write(VGC_SPACE_CHAR, base + (unsigned)j, (unsigned char)s[j]);
    }
}

/* Cold-boot the 6502: reset all chips, load the ROM, stage the $0320 resident
 * loader, run the boot splash, release, and bring the VGC up. Used at startup
 * and by host_reboot_vm() for the OSD's mount-and-boot. */
static int vm_cold_boot(void) {
    wr(R_CTRL, CTRL_CPU_RESET | CTRL_SYS_RESET);
    usleep(150000);
    if (load_rom("/data/nova/roms/ehbasic.bin") != 0) return 1;
    for (unsigned i = 0; i < sizeof(LOADER_BIN); i++) poke(0x0320 + i, LOADER_BIN[i]);
    printf("[novavm] staged resident loader @ $0320 (%zu bytes)\n", sizeof(LOADER_BIN));
    boot_splash();                 /* boot.json logo + fade while the 6502 is held */
    poke(AUTOBOOT_SKIP_ADDR, 0);
    cpu_hold(0);                    /* release -> cold start */
    usleep(600000);
    poke(0xA000, 0x00);            /* VGCR_MODE   = text       */
    poke(0xA0E5, 0x0F);            /* VGCR_DIM    = full bright */
    poke(0xA00A, 0x01);            /* VGCR_CURSEN = cursor on   */
    return 0;
}

/* OSD mount-and-boot: re-run the cold boot so the 6502 re-reads boot.json mounts
 * on autoboot (overrides nosd.c's weak stub; called from the OSD button thread). */
void host_reboot_vm(void) { vm_cold_boot(); }

/* OSD "reboot to BASIC" (offered after an unmount): cold-boot but make fio_load
 * refuse the one-shot autoboot probe, so the box lands at the BASIC prompt even
 * if other slots are still mounted. */
void host_reboot_to_basic(void) { g_boot_to_basic = 1; vm_cold_boot(); }

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);   /* line-buffer: make the boot log visible live */
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("/dev/mem"); return 1; }
    g_reg = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FIO_BASE);
    if (g_reg == MAP_FAILED) { perror("mmap"); return 1; }
    if (argc > 1 && !strcmp(argv[1], "screen")) { dump_screen(); return 0; }
    if (argc > 1 && !strcmp(argv[1], "osd-test")) { osd_selftest(); return 0; }
    if (argc > 1 && !strcmp(argv[1], "bootmsg")) { boot_wait_msg(); return 0; }
    /* test: inject a string into BASIC via R_KEY (then a CR). '~' -> CR.
     * Wait for the 6502 to consume each key (R_STATUS bit1 key_ready clears)
     * before the next, so they don't overwrite in the VGC's single key slot. */
    if (argc > 2 && !strcmp(argv[1], "type")) {
        /* Inject a string into the VGC key FIFO via R_KEY. No handshake needed:
         * the FIFO has 256 slots and key_ready = !fifo_full is normally 1.
         * ~20ms/char gives BASIC time to read each via $A00F. '~' -> CR. */
        const char *p = argv[2];
        for (;;) {
            unsigned char c = *p ? ((*p == '~') ? '\r' : (unsigned char)*p) : '\r';
            wr(R_KEY, c);
            usleep(20000);
            if (!*p) break;
            p++;
        }
        return 0;
    }
    mkdir("/data/nova/programs", 0775);

    /* Map the PS-DDR XRAM shelf (DT reserved-memory @ XRAM_DDR_BASE). NULL if the
     * reserve is absent -> nfio.c's XRAM commands return FIO_ERR instead of
     * scribbling on kernel RAM. */
    g_xram = mmap(NULL, XRAM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XRAM_DDR_BASE);
    if (g_xram == MAP_FAILED) {
        g_xram = NULL;
        fprintf(stderr, "[novavm] XRAM mmap @%#x failed (DT reserve missing? games/runtimes off)\n", XRAM_DDR_BASE);
    } else printf("[novavm] XRAM mapped: %u KB @ %#x\n", XRAM_BYTES >> 10, XRAM_DDR_BASE);


    /* Cold-boot the 6502 (reset chips, ROM + $0320 loader, splash, release, VGC
     * up). Factored so the OSD's mount-and-boot reuses the identical sequence. */
    if (vm_cold_boot() != 0) return 1;
    audio_init(); audio_start();   /* naudio.c: real SID/WTS+MIDI feeder (replaces the silence loop) */
    kbd_init();                    /* nkbd.c: physical USB keyboard -> R_KEY */
    servers_init();                /* nservers.c: mgmt/debug/upload servers for the nova CLI */
    nfio_init();                   /* nfio.c: XRAM/file/.ndi-drive command state */
    osd_init();                    /* nosd.c: OSD config menu (no-op until OSD-compositor bitstream) */
    printf("[novavm] 6502 released; VGC on; audio+keyboard+servers live; servicing FIO (programs -> %s)\n", PROG_DIR);

    for (;;) {
        if (!fio_pending()) { usleep(300); continue; }
        fio_clear();
        unsigned char cmd = peek(FIO_CMD);
        if (cmd && cmd != FIO_CMD_DIRREAD) poke(FIO_STATUS, 0);
        switch (cmd) {
            case FIO_CMD_LOAD:    fio_load(); break;
            case FIO_CMD_SAVE:    fio_save(); break;
            case FIO_CMD_DIROPEN: fio_diropen(); break;
            case FIO_CMD_DIRREAD: fio_dirread(); break;
            case FIO_CMD_DELETE:  fio_delete(); break;
            case FIO_CMD_LOAD_MODULE: {
                int id = peek(FIO_SRC_LO);
                if (load_module(id) == 0) {
                    for (int s = 0; s < 4; s++) poke(0x0418 + s, 0);   /* clear lib-call retvals */
                    poke(FIO_ERRCODE, 0); poke(FIO_STATUS, FIO_OK);    /* (no FIO_CMD clear: matches ps_fio) */
                } else fio_fail(FIO_ERR_NOTFOUND);
                break;
            }
            case FIO_CMD_VOLUME:  fio_volume();  break;
            case FIO_CMD_MIDPLAY: fio_midplay(); break;
            case FIO_CMD_MIDSTOP: fio_midstop(); break;
            case FIO_CMD_SFLOAD:  fio_sfload();  break;
            case FIO_CMD_XLOAD:       fio_xload();        break;   /* nfio: XRAM/file/.ndi */
            case FIO_CMD_LOADRUNTIME: fio_load_runtime(); break;
            case FIO_CMD_XPAGE:       fio_xpage();        break;
            case FIO_CMD_FOPEN:       fio_fopen();        break;
            case FIO_CMD_FCREATE:     fio_fcreate();      break;
            case FIO_CMD_FCLOSE:      fio_fclose();       break;
            case FIO_CMD_FREAD:       fio_fread();        break;
            case FIO_CMD_FWRITE:      fio_fwrite();       break;
            case FIO_CMD_FSEEK:       fio_fseek();        break;
            case FIO_CMD_FTELL:       fio_ftell();        break;
            case FIO_CMD_FSIZE:       fio_fsize();        break;
            case 0: break;
            default: fio_fail(FIO_ERR_NOTFOUND); break;
        }
    }
    return 0;
}
