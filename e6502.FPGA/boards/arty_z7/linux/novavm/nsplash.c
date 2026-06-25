/* nsplash.c — NovaVM boot splash for the Linux host.
 *
 * Renders the NVG2 logo into the VGC gfx plane and fades it, driven by the
 * vm.bootSplash block of /config/boot.json (the ULX3S schema). The render logic
 * matches the ULX3S novahost (set gfx mode -> palette -> stream pixels -> fade
 * in/hold/fade out -> restore text); the only hardware adaptation is the poke
 * path: the ESP32 pokes the VGC over SPI, here we poke over the fio_bridge
 * (poke()/R_VMEM_DATA), exactly as the on-board boot already does. The logo
 * comes from /assets/boot/novavm_logo.nvg if present, else the embedded copy. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "novavm.h"
#include "nbootcfg.h"
#include "boot_logo_nvg.h"   /* NOVAVM_LOGO_NVG[], NOVAVM_LOGO_NVG_len */

#define VGCR_MODE      0xA000u
#define VGCR_BGCOL     0xA001u
#define VGCR_CURSEN    0xA00Au
#define VGCR_BORDER    0xA00Du
#define VGCR_DIM       0xA0E5u
#define VGCR_PALIDX    0xA0F4u
#define VGCR_PALDAT    0xA0F5u
#define VGC_MODE_GFX_ONLY 0x03u
#define VGC_SPACE_GFX  3u
#define NVG_FLAG_PALETTE 0x02u

typedef struct { FILE *f; const unsigned char *mem; unsigned len, pos; } nvg_src_t;

static int src_open(nvg_src_t *s, const char *asset) {
    memset(s, 0, sizeof *s);
    char path[256];
    snprintf(path, sizeof path, "%s%s", NOVA_FS_ROOT, asset);  /* e.g. /data/nova/assets/boot/novavm_logo.nvg */
    s->f = fopen(path, "rb");
    if (s->f) return 1;
    s->mem = NOVAVM_LOGO_NVG; s->len = NOVAVM_LOGO_NVG_len;     /* embedded fallback */
    return 1;
}
static int src_read(nvg_src_t *s, void *dst, unsigned len) {
    if (s->f) return fread(dst, 1, len, s->f) == len;
    if (s->pos + len > s->len) return 0;
    memcpy(dst, s->mem + s->pos, len); s->pos += len; return 1;
}
static void src_close(nvg_src_t *s) { if (s->f) fclose(s->f); }

/* fade VGC DIM from->to over duration_ms (ULX3S fadeBootSplash). DIM is 0..15. */
static void splash_fade(int from, int to, unsigned duration_ms) {
    int steps = (to > from) ? (to - from) : (from - to);
    int step  = (to > from) ? 1 : -1;
    unsigned d_us = steps ? (duration_ms * 1000u / (unsigned)steps) : (duration_ms * 1000u);
    for (int v = from;; v += step) {
        poke(VGCR_DIM, (unsigned char)v);
        if (v == to) break;
        usleep(d_us);
    }
}

/* Render + fade the boot splash. Call with the 6502 held; restores text mode on
 * exit so NovaBASIC starts clean. No-op if vm.bootSplash.enabled is false. */
void boot_splash(void) {
    BootSplashConfig cfg;
    bootcfg_splash(&cfg);
    if (!cfg.enabled) return;

    nvg_src_t src;
    src_open(&src, cfg.assetPath);

    unsigned char hdr[16];
    if (!src_read(&src, hdr, 16) || hdr[0] != 'N' || hdr[1] != 'V' || hdr[2] != 'G' || hdr[3] != '2') {
        printf("[splash] bad NVG2 header -- skipped\n");
        src_close(&src); return;
    }
    unsigned w = hdr[4] | (hdr[5] << 8), h = hdr[6] | (hdr[7] << 8), flags = hdr[8];
    if (w != 320 || h != 200) { printf("[splash] unsupported %ux%u -- skipped\n", w, h); src_close(&src); return; }

    poke(VGCR_DIM, 0x00);                          /* render dark, then fade up */
    poke(VGCR_BGCOL, cfg.background & 0x0F);
    poke(VGCR_BORDER, cfg.border & 0x0F);
    poke(VGCR_CURSEN, 0x00);
    poke(VGCR_MODE, VGC_MODE_GFX_ONLY);

    if (flags & NVG_FLAG_PALETTE) {                /* 16 colours x 3 bytes */
        unsigned char pal[48];
        if (src_read(&src, pal, sizeof pal)) {
            poke(VGCR_PALIDX, 0x00);
            for (int i = 0; i < (int)sizeof pal; i++) poke(VGCR_PALDAT, pal[i]);
        }
    }

    /* Stream 320x200 4-bit pixels into VGC gfx via the dbg_vmem write port:
     * one write per pixel {space[2:0]@25, addr[16:0]@8, data[7:0]}. */
    unsigned char row[160];                        /* 320 px / 2 px-per-byte */
    for (unsigned y = 0; y < 200; y++) {
        if (!src_read(&src, row, sizeof row)) break;
        unsigned base = y * 320u;
        for (unsigned x = 0; x < 320; x++) {
            unsigned char px = (x & 1u) ? (row[x >> 1] & 0x0F) : (row[x >> 1] >> 4);
            wr(R_VMEM_DATA, (VGC_SPACE_GFX << 25) | ((base + x) << 8) | px);
        }
    }
    src_close(&src);
    printf("[splash] %ux%u logo drawn (%s); fade %u/hold %u/fade %u ms\n",
           w, h, src.f ? "SD asset" : "embedded", cfg.fadeInMs, cfg.holdMs, cfg.fadeOutMs);

    splash_fade(0, 15, cfg.fadeInMs);              /* fade in  */
    usleep(cfg.holdMs * 1000u);                    /* hold     */
    splash_fade(15, 0, cfg.fadeOutMs);             /* fade out */

    /* restore power-on text state for NovaBASIC */
    poke(VGCR_MODE, 0x00);
    poke(VGCR_DIM, 0x0F);
    poke(VGCR_CURSEN, 0x01);
    wr(R_VMEM_ADDR, (1u << 17));                   /* vmem read target back to char[0] */
}
