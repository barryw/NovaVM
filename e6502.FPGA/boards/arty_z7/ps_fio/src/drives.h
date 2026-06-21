// drives.h — Arty PS drive-slot mount table (fd0..fd3, hd0, hd1).
//
// `nova drive mount/unmount` (mgmt.c, CBOR 10/11) records which `.ndi` image is
// in which slot here; the table persists to /config/mounts.txt so it survives the
// reboot needed to boot a game. The FIO host (main.c / NDI routing) reads the
// boot slot to resolve the 6502's FILE_LOAD against the mounted image.

#ifndef DRIVES_H
#define DRIVES_H

#define DRIVE_SLOTS 6          // fd0,fd1,fd2,fd3,hd0,hd1

int         drive_slot_index(const char *name);  // name -> 0..5, or -1
const char *drive_slot_name(int idx);            // 0..5 -> "fd0".. , or 0
const char *drive_path(int idx);                 // mounted SD path, "" if none
void        drive_set(int idx, const char *path);// path="" unmounts; persists
int         drive_boot_slot(void);               // first mounted (fd0..hd1), -1
void        drives_load(void);                    // load /config/mounts.txt at boot

#endif
