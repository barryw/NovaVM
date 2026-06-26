# Arty Z7 On-Screen Config (OSD) — Design Spec

A BIOS-style on-screen config driven by the four Arty push buttons. First config
surface: mounting/unmounting floppies & hard drives. Rendered as a hardware
overlay in the HDMI path (independent of the VGC) and driven entirely by the PS
(novavm). The whole NovaVM machine pauses while the OSD is open.

Status: **design locked, pre-implementation.** Needs one bitstream rebuild.

---

## 1. Filesystem change

Collapse `/data/nova/fds/` + `/data/nova/hdds/` into a single **`/data/nova/disks/`**.
Every disk is a `.ndi`; its kind is decided by size:

| Size        | Kind   | Mountable slots |
|-------------|--------|-----------------|
| ≤ 1.44 MB   | floppy | `fd0`, `fd1`    |
| > 1.44 MB   | hdd    | `hd0`, `hd1`    |

Mounts persist in `boot.json` `mounts` (unchanged schema: `fd0/fd1/hd0/hd1`).
Migration: move existing images into `disks/`; update `nfio.c` + the
`nova-appliance` recipe paths; the games keep working (paths only).

---

## 2. UX / button scheme

Four buttons (all freed from any other use — see §4):

| Button | Role |
|--------|------|
| **B1** | Toggle OSD: open if closed, **close-everything** if open (from any depth) |
| **B2** | Navigate up |
| **B3** | Navigate down |
| **B4** | Select |

Every sub-menu has a **Cancel** item = back one level. **B1 always closes the
whole OSD.** Highlight bar marks the current item.

### Mount flow
```
B1 → [Config]
       Mounts
       Cancel
   → B4 on "Mounts" → [Slots]
       fd0  <empty | name>
       fd1  ...
       hd0  ...
       hd1  ...
       Cancel
   → B4 on a slot → [Slot menu]
       (mounted)   Unmount / Replace / Cancel
       (empty)     Mount... / Cancel
   → "Mount..." / "Replace" → [Disk list]   (size-filtered to the slot's kind)
       <disk A>
       <disk B>
       ...
       Cancel
   → B4 on a disk → mount it into boot.json AND boot it:
       assert system-pause off → pulse dbg_system_reset → novavm re-stages →
       6502 cold-starts → autoboot reads the new mount → runs the disk.
```
- **Unmount** = clear `boot.json mounts.<slot>` (immediate; takes effect next boot/reset).
- **Mount = boot** (per decision): selecting a disk mounts it and triggers a clean
  reset so the box boots it on the spot.

---

## 3. Architecture

```
            ┌────────── PL (bitstream) ──────────┐
 buttons ──▶│ debounce ──▶ fio_bridge BTN reg    │
            │                                     │
 6502 ─┐    │  VGC ──RGB──▶ OSD COMPOSITOR ──▶ HDMI encoder ──▶ HDMI
 SID  ─┤    │                   ▲   ▲                            │
 WTS  ─┼─ system_pause gates    │   │ dim                        │
 math ─┤    │   their clk_en    │   └─ OSD framebuffer (BRAM)    │
 VGC* ─┘    │                   └──── written by PS              │
            └─────────────────────────────────────┘
                         ▲ fio_bridge (GP0)
                         │
                    novavm (PS / Linux): buttons + menu FSM + FB writes +
                                         boot.json + reset + audio mute
* VGC command engine pauses; VGC video scan-out keeps running.
```

- **OSD is NOT drawn into the VGC.** It's a separate compositor between the VGC's
  RGB and the HDMI encoder → no VGC/6502 layer conflict, no save/restore.
- **novavm owns all logic**: reads buttons, runs the menu state machine, writes
  the OSD framebuffer, edits `boot.json`, drives reset, mutes audio while paused.

---

## 4. Bitstream changes (one rebuild)

1. **Free `btn[0]`** from `.RST(btn[0])` on the MMCM — drive the MMCM reset from
   the internal PL reset instead. (btn[0]-as-reset is a footgun: it resets the PL
   but not the PS, stranding the 6502. Gone.)
2. **Buttons → PS**: debounce `btn[3:0]` in RTL; expose level + 1-shot
   press-edge to a new fio_bridge read register.
3. **`dbg_system_reset` wired**: route the existing CTRL bit1 into the
   custom-chip + 6502 reset (it's currently a no-op) so the PS can do a *clean*
   reset from any state — fixes mount-and-boot and the runtime hot-switch.
4. **System pause**: a `system_pause` line off a CTRL bit. When high, gate
   `cpu_ce` and the clock-enables of SID1/SID2, WTS, math copro, and the VGC
   command/blitter/DMA engine. **VGC video scan-out + HDMI keep running.**
5. **OSD compositor + framebuffer**:
   - Char-grid framebuffer BRAM (8×16 font, ~90×30 cells @ 720×480).
   - Cell = `{transparent:1, char:8, fg:4, bg:4}`. Opaque → fill `bg`, draw
     `char` in `fg`; transparent → pass through.
   - Compositor per output pixel: `osd_enable & cell.opaque & font_bit` → `fg`;
     `osd_enable & cell.opaque` → `bg`; else → VGC pixel, **dimmed `>>1` when
     `osd_enable`** (so the program shows at 50% behind the panel).
   - (Future: blend instead of replace for a see-through panel — one-line change.)

---

## 5. fio_bridge register map additions

Decode widens to `addr[6:2]` (0x00–0x7C); existing 0x00–0x3C unchanged.

| Off  | R/W | Name        | Payload |
|------|-----|-------------|---------|
| 0x40 | R   | BUTTONS     | `{press_edge[3:0], level[3:0]}` (edge = 1-shot, cleared on read) |
| 0x44 | RW  | OSD_CTRL    | bit0 `osd_enable`, bit1 `system_pause`, bit2 `dbg_system_reset` (pulse), bits[7:4] reserved |
| 0x48 | W   | OSD_FB      | `{transparent, bg[3:0], fg[3:0], char[7:0], cell_addr[12:0]}` → one cell |
| 0x4C | W   | OSD_FILL    | `{transparent, bg[3:0], fg[3:0], char[7:0]}` → fill entire grid (fast clear) |

(`dbg_cpu_reset` stays CTRL.0 at 0x10; `dbg_system_reset` is promoted from the
old no-op CTRL.1 to the real reset in §4.3, also pulseable here.)

---

## 6. novavm: OSD module (`nosd.c`)

- `osd_init()` — spawn a thread polling BUTTONS (0x40) at ~60 Hz.
- Menu FSM: states = CLOSED, CONFIG, SLOTS, SLOTMENU, DISKLIST. Each state holds
  an item list + a cursor; B2/B3 move the cursor, B4 selects, B1 closes.
- Render: on any state/cursor change, redraw via OSD_FILL (clear, transparent) +
  OSD_FB writes (panel cells + text). Center the panel; highlight the cursor row
  (swap fg/bg).
- Open: assert `osd_enable` + `system_pause`, mute audio. Close: clear both,
  unmute.
- Disk list: `scandir("/data/nova/disks")`, stat each, filter by ≤/>1.44 MB to
  the slot kind.
- Mount: `bootcfg_mount_set(slot, "/disks/<name>")`; unmount: `bootcfg_mount_set(slot, "")`.
- Mount-and-boot: after mount_set, drop pause, pulse `dbg_system_reset`, re-run
  the host boot sequence (re-stage ROM/loader, release) so autoboot picks up the
  new mount.

---

## 7. Implementation order

1. **Host-side, no bitstream** (works against the current image):
   - `disks/` migration (`nfio.c` paths, the recipe, move images).
   - `nosd.c` menu FSM + rendering, tested by dumping the intended grid to a
     debug screen (validate the menu logic before the compositor exists).
2. **Bitstream bundle** (§4) — RTL + the register map (§5), one rebuild.
3. **Wire `nosd.c`** to the real BUTTONS / OSD_FB / OSD_CTRL registers; bake into
   the image; test on hardware end-to-end.
