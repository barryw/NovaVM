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

#ifndef FIO_ERR_NOTMOUNTED
#define FIO_ERR_NOTMOUNTED 5
#endif

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
 * fio_ok()), -1 if nothing is mounted or the name is not in the image. Does
 * NOT itself call fio_ok/fio_fail; the caller owns the mailbox result. */
int  nfio_image_load(const char *name);

/* ---- drive-aware user filesystem (SAVE/LOAD/DIR/DELETE/CD/PWD) -------------
 * Routed to from novavm.c's FIO handlers when a disk is mounted (gate with
 * nfio_disk_active()); each reads the FIO mailbox + owns fio_ok()/fio_fail(),
 * except nfio_disk_load() which returns 0/-1 like nfio_image_load(). Files are
 * (name + type) with no extensions — the type comes from FIO_DIRTYPE on SAVE. */
int  nfio_disk_active(void);    /* 1 if the CWD is a mounted disk (routing gate) */
int  nfio_stage_save(unsigned char *buf, unsigned bufsz); /* RAM range -> save image (hdr+bytes); ret len or -1 */
void nfio_disk_save(void);      /* FIO_CMD_SAVE    when a disk is mounted        */
int  nfio_disk_load(const char *name); /* try a path/prefix load; 0 hit, -1 miss */
void nfio_disk_diropen(void);   /* FIO_CMD_DIROPEN when a disk is mounted        */
void nfio_disk_dirread(void);   /* FIO_CMD_DIRREAD: name + FIO_DIRTYPE + FIO_SIZE */
void nfio_disk_delete(void);    /* FIO_CMD_DELETE  when a disk is mounted        */
void nfio_cd(void);             /* FIO_CMD_CD: set the current drive/subdir      */
void nfio_pwd(void);            /* FIO_CMD_PWD: "drive:dir" -> FIO_NAME/NAMELEN   */
void nfio_devstatus(void);      /* FIO_CMD_DEVSTATUS: mounted slot probe         */

/* ---- directory create/remove + VGC graphics-memory save/load --------------
 * Ports of the desktop FileIoController DoMkdir/DoRmdir/DoGSave/DoGLoad. Each
 * reads the FIO mailbox + owns fio_ok()/fio_fail(). They require a mounted
 * .ndi disk and report FIO_ERR_NOTMOUNTED when no current disk is active. */
void nfio_mkdir(void);          /* FIO_CMD_MKDIR 0x21: create a directory        */
int  nfio_disk_read(const char *name, unsigned char *buf, int maxlen); /* named file from the mounted NDI -> bytes, or -1 */
void nfio_rmdir(void);          /* FIO_CMD_RMDIR 0x22: remove an empty directory */
void nfio_gsave(void);          /* FIO_CMD_GSAVE 0x06: VGC plane region -> .gfx   */
void nfio_gload(void);          /* FIO_CMD_GLOAD 0x07: .gfx -> VGC plane region   */

#endif /* NFIO_H */
