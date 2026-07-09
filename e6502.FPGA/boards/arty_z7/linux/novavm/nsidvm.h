/* nsidvm.h — C port of the ESP32/ULX3S NovaHost SID VM (e6502.ESP32/novahost/
 * sid_vm.{h,cpp}). A tiny 6502 interpreter that runs a PSID/RSID tune's
 * init/play routines and captures every $D400/$D420 register write, so the host
 * can forward those writes to the PL reDIP-SID (and mirror them for the on-screen
 * keyboard visualizer).
 *
 * The Arty host build is C-only (no g++/libstdc++ in the arm-linux-gnueabihf
 * toolchain), so this is a faithful C translation of the tested C++ SidVm class,
 * not a C++ TU behind an extern "C" shim. One tune plays at a time -> the VM is a
 * file-static singleton in nsidvm.c (mirrors the single _sid_vm on the ESP). */
#ifndef NSIDVM_H
#define NSIDVM_H

#include <stddef.h>
#include <stdint.h>

/* Parsed PSID/RSID header (C mirror of nova_sid::SidFileInfo). */
typedef struct {
    int      valid;
    int      rsid;
    int      load_in_payload;   /* load address is the first 2 payload bytes */
    unsigned data_offset;
    unsigned load_addr;
    unsigned init_addr;
    unsigned play_addr;
    unsigned songs;
    unsigned start_song;
    unsigned speed;
    unsigned flags;
    unsigned payload_offset;    /* file offset of the first program byte */
    unsigned payload_bytes;
} nsid_info_t;

int      nsid_parse_header(const uint8_t *header, uint32_t file_size, nsid_info_t *out);
uint32_t nsid_frame_period_us(const nsid_info_t *info, uint8_t song);  /* ~20000 PAL / 16667 NTSC */
uint8_t  nsid_fpga_config(const nsid_info_t *info);                    /* bit0=8580 bit1=NTSC -> $D440 */

typedef enum {
    NSID_OK = 0,
    NSID_HUNG,
    NSID_OOM,
    NSID_BADOP,
    NSID_BADROM,
    NSID_INTERNAL,
} nsid_status;

const char *nsid_status_name(nsid_status st);

/* Called for every captured SID write, with the address already normalised to
 * $D400-$D41F (SID 1) or $D420-$D43F (SID 2). Return non-zero to continue. */
typedef int (*nsid_write_fn)(void *user, uint16_t addr, uint8_t value);

/* Singleton VM lifecycle (mirrors SidVm's public surface). */
void        nsid_reset(void);
void        nsid_set_write_handler(nsid_write_fn fn, void *user);
int         nsid_load_payload(uint16_t load_addr, const uint8_t *data, size_t len);
void        nsid_set_entry(uint16_t init_addr, uint16_t play_addr);
nsid_status nsid_run_init(uint8_t song);       /* song 0-based (song_number - 1) */
nsid_status nsid_run_play_frame(void);
uint16_t    nsid_last_pc(void);
uint8_t     nsid_last_opcode(void);
uint16_t    nsid_pages_allocated(void);

#endif /* NSIDVM_H */
