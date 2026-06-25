/* nfio.h — NovaVM XRAM + file-handle + .ndi disk-image FIO commands.
 *
 * Linux userspace port of the bare-metal ps_fio/src/main.c FIO host (the parts
 * that touch the mounted .ndi boot image, the PS-DDR XRAM shelf, and low-level
 * file handles). This is what unlocks games, runtimes (Z-machine), mounted disk
 * images and streamed file I/O on the NovaVM Linux host.
 *
 * Faithful C-to-C port. The bare-metal sources of truth are:
 *   ps_fio/src/main.c   — the FIO handlers + boot-image resolution + g_fh[]/g_wbuf
 *   ps_fio/src/ndi.c    — the .ndi reader (a C shim over the C++ ndi::NdiImage)
 *   ps_fio/src/drives.c — the drive mount table
 * The actual .ndi parsing is ported here from the cross-platform parser in
 * e6502.ESP32/novahost/ndi_image.cpp (re-implemented in pure C so the Linux
 * host stays a single C translation unit — build.sh links C only).
 *
 * Adaptations vs. bare-metal:
 *   FatFs f_open/f_read/f_write/f_lseek  -> POSIX fopen/fread/fwrite/fseek/ftell
 *   SD path "0:/x"                       -> host path "/data/nova/x"
 *   Xil_Out32(XRAM_DDR_BASE+off,..)      -> g_xram[off] (mmap'd PS-DDR window)
 *   drives.c mount table                 -> reads the SAME /data/nova/config/
 *                                           mounts.txt that nservers.c writes,
 *                                           so `nova drive mount` (mgmt) and the
 *                                           image resolution here always agree.
 *
 * novavm.c owns g_reg + g_xram; this module reaches the PL/XRAM via the
 * poke()/peek()/wr()/rd()/fio_ok()/fio_fail() primitives in novavm.h. Do NOT
 * redefine those.
 */
#ifndef NFIO_H
#define NFIO_H

/* ---- lifecycle ------------------------------------------------------------ */
/* Reset module state (file handles, write staging, cached boot image, mount
 * table reload flag). Call once at boot from novavm.c before the service loop.
 * Optional — the handlers lazily load the mount table on first use. */
void nfio_init(void);

/* ---- FIO command handlers (dispatch from the novavm.c FIO switch) ----------
 * Each reads its parameters from the FIO mailbox in 6502 RAM via peek() and
 * reports the result with fio_ok()/fio_fail() — identical mailbox ABI to the
 * bare-metal. Add the matching `case FIO_CMD_*:` entries to novavm.c's switch
 * (see the opcode map in nfio.c's header comment / the delivered report). */
void fio_xload(void);         /* FIO_CMD_XLOAD       0x18: file -> flat XRAM            */
void fio_xpage(void);         /* FIO_CMD_XPAGE       0x29: file slice -> XRAM or CPU RAM */
void fio_load_runtime(void);  /* FIO_CMD_LOADRUNTIME 0x28: 16KB ROM -> $C000 primary bank */
void fio_fopen(void);         /* FIO_CMD_FOPEN       0x2D: open image file -> handle    */
void fio_fcreate(void);       /* FIO_CMD_FCREATE     0x2E: open write handle            */
void fio_fclose(void);        /* FIO_CMD_FCLOSE      0x2F: close (commit if writing)    */
void fio_fread(void);         /* FIO_CMD_FREAD       0x30: read chunk -> CPU RAM / XRAM */
void fio_fwrite(void);        /* FIO_CMD_FWRITE      0x31: append chunk to write handle */
void fio_fseek(void);         /* FIO_CMD_FSEEK       0x32: set handle position          */
void fio_ftell(void);         /* FIO_CMD_FTELL       0x33: report handle position       */
void fio_fsize(void);         /* FIO_CMD_FSIZE       0x34: report handle size           */

/* ---- mounted-image LOAD hook (wire into novavm.c's fio_load) ---------------
 * Resolve a bare 6502 filename inside the mounted .ndi boot image and, if found,
 * load it into 6502 RAM exactly as the bare-metal fio_load() does for an image
 * hit: writes the bytes via poke(), updates FIO_SRC (for .bin), FIO_SIZE and
 * FIO_DIRTYPE. Returns 0 on a successful image load (caller should then
 * fio_ok()), -1 if nothing is mounted or the name is not in the image (caller
 * falls through to its SD/programs LOAD). Does NOT itself call fio_ok/fio_fail —
 * the caller owns the mailbox result. See nfio.c for the exact wiring snippet. */
int  nfio_image_load(const char *name);

#endif /* NFIO_H */
