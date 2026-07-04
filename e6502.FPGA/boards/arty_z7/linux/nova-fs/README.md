# nova filesystem layout (Linux host)

Canonical data tree for NovaVM-on-Linux. Mounted on the device at a fixed root
(`/data/nova`, TBD) — a dedicated partition (FAT if you want to edit the card on a PC,
else ext4). The host (`novavm`) resolves the `nova` CLI's logical `0:/...` paths and its
typed operations against these dirs. Data files carry over from the bare-metal SD unchanged.

```
hdds/        hard-disk images (.ndi)        -> `nova drive mount <x>` resolves here (HDD slots)
fds/         floppy images (.ndi)           -> `nova drive mount <x>` resolves here (FD slots)
soundfonts/  WTS soundfonts (.sf2)          -> `nova soundfont <x>` uploads/loads here
roms/        system ROMs                    -> ehbasic.bin (always loaded into basic_rom @ boot),
                                               F6581.BIN (SID filter curve -> XRAM)
disks/       mounted NDI images             -> SAVE/LOAD/DIR operate inside mounted disks
assets/      boot assets                    -> novavm_logo.nvg (boot splash)
config/      host state                     -> drive-mount table (which .ndi is in which slot)
```

## Boot / runtime model (owner decision — important)
- **We ALWAYS boot into NovaBASIC. Always.** There is **no runtime config** (no
  `defaultRuntime` / mempalace runtime selection) anymore.
- **Runtimes (NovaZ, Logo, etc.) load via AUTOBOOT**, exactly like NovaZ already does:
  a mounted `.ndi` carries an `AUTOBOOT` that issues `FILE_LOAD_RUNTIME` to stage a 16 KB
  runtime ROM into the `$C000` bank, then jumps in. To run a runtime, you `nova drive mount`
  the NDI; its AUTOBOOT does the rest. No NDI mounted (or no AUTOBOOT) => plain NovaBASIC `Ready`.
- The host MUST keep the **dynamic runtime-load path working** (port `fio_load_runtime`:
  stream a runtime ROM from an NDI into the `$C000` primary bank via the ROM-write port,
  then the 6502 `JMP ($FFFC)` into it). This is the same mechanism NovaZ uses today.
- `config/` holds only the **drive-mount table** (slot -> .ndi path), not runtime selection.

## SSH
Owner wants to `ssh` into the running Linux. Add **dropbear** (lean, fits Buildroot) to the
rootfs in Phase 1, with a stable host key + a login. (Full OpenSSH optional later.)
