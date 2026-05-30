# Nova Assembly Library Reference

This is the working reference for the reusable ca65 libraries under
`runtime/asm`. It is intentionally practical: use it to find the right library,
learn the build pattern, and locate the detailed ABI comments until the full
programmer's reference guide exists.

For labels already exported into the generated runtime ABI, also check
`docs/assembly/runtime-labels.md`. That file is generated from `@label`
comments in the assembly sources. Some newer link-time libraries may exist here
before they are folded into the generated reference.

## Source Layout

The canonical libraries live in `runtime/asm`:

| Kind | Path pattern | Purpose |
| --- | --- | --- |
| Interface | `runtime/asm/name.inc` | Constants, pseudo-register aliases, descriptor layouts, and `.global` declarations. |
| Implementation | `runtime/asm/name.s` | Code and library-owned BSS/ZEROPAGE state. Include or link this once. |
| Hardware constants | `runtime/asm/novavm.inc` | Generated hardware constants plus ROM entry labels. |

Macro-only libraries, such as `math_copro.inc`, `math_fixed8.inc`, and
`nova.inc`, do not have a matching implementation source file.

For distribution, run `make -C ndk package`. The generated Nova Developer Kit
places these libraries under `dist/nova-ndk/asm` alongside documentation and
example applications.

Standalone assembly applications live in `assembly/apps`. They use the same
runtime include root and are built with `make -C assembly`.

## Size-Stripped Runtime Includes

For resource-constrained binaries, runtime files that support stripping use it
by default. Application code does not need an extra opt-in include. Include the
implementation after code that references the routines:

```asm
.include "fio.inc"

    jsr fio_load

.include "fio.s"
```

This uses ca65 `.ifref`/`.refto` reference tracking. `fio.s` currently emits
only referenced FIO routines plus required dependencies by default. For
example, a program that calls only `fio_load` pulls in `fio_load`, `fio_exec`,
and `fio_check`, not every FIO helper.

Ordering matters: a stripped implementation included before its call sites has
not seen those references yet. Define `NOVA_EMIT_ALL_RUNTIME = 1` before
implementation includes for ROM/debug builds that intentionally need full
runtime emission.

## Build Pattern

For a small standalone program, include the interface near the top and include
the implementation once after your code:

```asm
.setcpu "65c02"

.include "anim.inc"

; application code

.include "anim.s"
```

Assemble with the shared runtime include root available:

```sh
ca65 --cpu 65c02 -I runtime/asm -o myprog.o myprog.s
ld65 -C myprog.cfg -o myprog.bin myprog.o
```

Larger projects can assemble a library `.s` file as an object and link it. The
same duplicate-definition rule applies either way: include or link each
implementation once.

Top-level implementation files include their dependencies. For example,
`anim.s` includes `dma.s`, `fio.s`, `pager.s`, and `msprite.s`. If your
program includes `anim.s`, do not also include `msprite.s` separately.
Use the highest-level implementation that owns the stack you need.

## Calling Conventions

Most shared routines follow these conventions unless their comments say
otherwise:

| Convention | Meaning |
| --- | --- |
| `A=0` | Success. |
| `A=1` | Generic error. Detailed status is usually in a library result byte or hardware status register. |
| `A`, `X`, `Y` | Scratch across calls unless documented as return values. |
| `NVR0L` through `NVR7H` | Shared pseudo-register argument/scratch bytes. A library may clobber them unless documented otherwise. |
| Persistent state | Library-owned state lives in BSS/ZEROPAGE symbols defined by the implementation file. |

Descriptor-heavy systems, such as meta-sprites, animation, virtual sprites,
overlays, and game-server frames, use caller-owned tables plus library-owned
runtime state.

## Quick Map

### Core

| Library | Files | Use For | Key Symbols |
| --- | --- | --- | --- |
| Nova constants | `nova.inc` | Hardware addresses, VGC/FIO/DMA/NIC/XMC constants, pseudo-registers, SYS mailbox bytes. | `NVR0L`, `VGC_CMD`, `DMA_CMD`, `SYS_REGA` |
| Math coprocessor macros | `math_copro.inc` | One-call register wrappers for math hardware. | `MATHC_MUL16`, `MATHC_MULFX`, `MATHC_SINCOS`, `MATHC_ATAN2`, `MATHC_DIST_APPROX_U8`, `MATHC_VEC_*` |
| Fixed-point macros | `math_fixed8.inc` | Cheap 8.8 and 16.8 game physics helpers. | `FIX8_ADD_CONST_X`, `FIX16_8_ADD_FIX8_X`, `FIX8_ABS_TO_X`, `FIX8_NEGATE_MEM` |
| Tween/easing | `tween.inc`, `tween.s` | Frame-based interpolation and quadratic easing. | `tween_begin`, `tween_eval`, `tween_step`, `TWEEN_*` |
| Random numbers | `rng.inc`, `rng.s` | Host-backed random values. | `rng_get8`, `rng_get16`, `rng_get32`, `RNG_VALUE0..3` |

### Display, Sprites, And Graphics

| Library | Files | Use For | Key Symbols |
| --- | --- | --- | --- |
| VGC | `vgc.inc`, `vgc.s` | Raw VGC commands, text controls, graphics primitives, VGC IRQ control. | `vgc_exec`, `vgc_vsync`, `vgc_gcls`, `vgc_plot`, `vgc_line`, `vgc_irq_enable` |
| Copper | `copper.inc`, `copper.s` | Building and enabling copper lists. | `copper_clear`, `copper_list`, `copper_add`, `copper_use`, `copper_on` |
| Hardware sprites | `sprite.inc`, `sprite.s` | Low-level 16 hardware sprite control and collision latches. | `sprite_pos`, `sprite_set_shape`, `sprite_enable`, `sprite_collision_read_clear` |
| Meta-sprites | `msprite.inc`, `msprite.s` | Treat multiple hardware sprites as one object. | `msprite_spawn`, `msprite_set_pos`, `msprite_set_frame`, `msprite_commit` |
| Animation | `anim.inc`, `anim.s` | Timed shape/frame animation for sprites and meta-sprites, plus bulk shape loading. | `anim_start`, `anim_tick`, `anim_set_frame`, `anim_load_xram_shapes`, `anim_load_disk_shapes` |
| Virtual sprites | `vsprite.inc`, `vsprite.s` | Blitter-backed rectangular software sprites and saved-background scenes. | `vsprite_gfx_save_bg`, `vsprite_gfx_blit`, `vsprite_scene_commit`, `vsprite_scene_commit_atomic` |
| VTEXT | `vtext.inc`, `vtext.s` | Rectangular text regions over VGC character/color/attribute planes. | `vtext_put_char`, `vtext_clear_region`, `vtext_scroll_up`, `vtext_select_region` |
| EDITUI | `editui.inc`, `editui.s` | Turbo-style TUI shell for native tools: default File/Edit/Help menus, title/help band, shortcut row, CP437 framed panels, and clipped panel body regions. | `editui_init`, `editui_draw_shell`, `editui_menu_open_hotkey`, `editui_select_box_body` |
| NUI | `nui.inc`, `nui.s` | First-pass modal dialogs over VTEXT and virtual sprites. | `nui_dialog_defaults`, `nui_show_dialog`, `nui_show_error`, `nui_wait_key` |
| NVG loader | `nvg.inc`, `nvg.s` | Load NovaDraw NVG1 images through NovaHost into graphics bitmap memory. | `nvg_load`, `nvg_load_at`, `nvg_load_named`, `nvg_load_named_at` |
| DMA | `dma.inc`, `dma.s` | Raw DMA copy/fill setup and wait/status handling. | `dma_copy`, `dma_fill`, `dma_wait` |
| Blitter | `blitter.inc`, `blitter.s` | Raw rectangular copy/fill setup and wait/status handling. | `blitter_copy`, `blitter_fill`, `blitter_wait` |

### Files, XRAM, And Overlays

| Library | Files | Use For | Key Symbols |
| --- | --- | --- | --- |
| FIO | `fio.inc`, `fio.s` | File commands, filename copying, runtime loading, graphics file calls, RNG command bridge. | `fio_copy_name`, `fio_load`, `fio_save`, `fio_dir_open`, `fio_load_runtime` |
| Pager | `pager.inc`, `pager.s` | Load a file slice into XRAM, CPU RAM, or VGC memory. | `pager_load_file_page`, `PAGER_TARGET_XRAM`, `PAGER_TARGET_RAM`, `PAGER_TARGET_VGC` |
| XRAM | `xram.inc`, `xram.s` | Flat 24-bit expansion RAM reads/writes, DMA copies, fills, and file streaming. | `xram_read8`, `xram_write8`, `xram_copy_from_ram`, `xram_xload`, `xram_xsave` |
| XMC | `xmc.s` | BASIC-compatible named XRAM allocator and XMC command processor. | `xmc_process`, `xmc_alloc_block`, `xmc_named_stash`, `xmc_named_fetch`, `xmc_map_window` |
| Overlays | `overlay.inc`, `overlay.s` | Fixed-address executable modules with NOVO headers. | `overlay_load_fixed`, `overlay_call_init`, `overlay_call_main`, `overlay_call_tick`, `overlay_unload` |

### Audio, Networking, And Game Services

| Library | Files | Use For | Key Symbols |
| --- | --- | --- | --- |
| Audio/music | `audio.inc`, `audio.s` | Sound effects, instruments, SID/MIDI playback, MML/music sequence control. | `audio_sound`, `audio_instrument`, `audio_sidplay_file`, `audio_midplay_file`, `audio_music_play` |
| NIC | `nic.inc`, `nic.s` | TCP-style network slots through NovaHost. | `nic_connect`, `nic_listen`, `nic_send`, `nic_recv`, `nic_status`, `nic_ready` |
| Game server | `gameserver.inc`, `gameserver.s` | Compact binary protocol over NIC for lobbies, tables, actions, and game state. | `ngs_connect_default`, `ngs_build_hello`, `ngs_wait_kind`, `ngs_build_player_action` |

## Sprite And Animation Stack

Use this stack for game objects that should remain hardware sprites:

| Layer | Responsibility |
| --- | --- |
| `sprite.s` | Direct hardware sprite commands and register writes. |
| `msprite.s` | Allocates a contiguous range of hardware sprites and updates them as one visual object. |
| `anim.s` | Owns timing tracks and applies frame changes to a hardware sprite or meta-sprite. |

Meta-sprite visual descriptors are caller-owned byte tables:

```asm
ship_visual:
        .byte 2,0              ; part count, flags
        .byte 0,0,0,0          ; dx, dy, shape base, part flags
        .byte 0,16,4,0         ; second 16x16 part below the first
```

To spawn and move one:

```asm
        lda #<ship_visual
        sta MSPRITE_DESC_L
        lda #>ship_visual
        sta MSPRITE_DESC_H
        jsr msprite_spawn
        cmp #MSPRITE_INVALID_HANDLE
        beq spawn_failed
        sta ship_handle

        lda #<120
        sta NVR0L
        lda #>120
        sta NVR0H
        ldy #80
        lda ship_handle
        jsr msprite_set_pos
        jsr msprite_commit
```

Animation descriptors can be linear, ping-pong, or frame-table driven. The
common linear case is:

```asm
ship_anim:
        .byte 4,6,ANIM_DESC_LOOP | ANIM_DESC_DEFER_COMMIT,0,1,0
        ; frames, ticks/frame, flags, first shape/frame, stride, reserved
```

Use `ANIM_DESC_DEFER_COMMIT` when many tracks update in one frame and you want
one `msprite_commit` at the end of the game loop.

Shape bytes should be preloaded into VGC sprite shape RAM. Use
`anim_load_xram_shapes` for XRAM-to-shape-RAM DMA, or `anim_load_disk_shapes`
to stream a file slice into shape RAM through FIO/pager.

## Virtual Sprite Stack

Use `vsprite.s` when you need more moving objects than the 16 hardware sprite
slots can provide, or when the object is naturally a rectangular graphics-plane
stamp.

The single-object pattern is:

1. Set `VSPRITE.XL/H`, `VSPRITE.Y`, width/height, source, and background buffer
   pseudo-registers.
2. Call `vsprite_gfx_save_bg`.
3. Call `vsprite_gfx_blit`.
4. On the next frame, call `vsprite_gfx_restore_bg`, move the object, save the
   new background, and draw again.

For multiple moving objects, prefer scene descriptors and
`vsprite_scene_commit`. For overlapping objects that must compose without
visible intermediate writes, use `vsprite_scene_commit_atomic` with a caller
supplied dirty rectangle and work buffer.

For in-place rotated virtual sprites, prefer `vsprite_gfx_rotate_blit`. It
rotates into the caller-owned `VSPRITE.ROT*` buffer offscreen, waits for the
next VGC frame tick, then copies the full rotated bounds to the graphics plane
in one visible blit. That clears old pixels without making applications perform
a separate visible erase.

## XRAM And File Loading Stack

Use this stack for assets and large runtime data:

| Layer | Responsibility |
| --- | --- |
| `fio.s` | File naming and host command transport. |
| `pager.s` | Load file slices into XRAM, CPU RAM, or VGC memory. |
| `xram.s` | Flat 24-bit XRAM byte/copy/fill/file-streaming runtime. |
| `overlay.s` | Fixed-address executable overlay loading on top of pager/FIO. |

Assembly code should prefer flat XRAM addresses through `xram.s`:

```asm
        lda #$00
        sta XRAM_ADDRL
        lda #$20
        sta XRAM_ADDRM
        lda #$02
        sta XRAM_ADDRH
        jsr xram_read8
        bne xram_error
        lda XRAM_DATA
```

Use `pager_load_file_page` when the source is a file offset and the destination
is selected by `PAGER_TARGET`. This is the path used by overlays and disk-backed
animation shape loads.

## Math And Physics

Use `math_copro.inc` for hardware-assisted math. The current macro surface
covers:

- signed 16x16 multiply
- signed Q8.8 multiply
- signed 32/16 division
- sine/cosine
- atan2 plus approximate hypotenuse
- approximate distance
- dot product, cross product, squared length, and vector scaling
- RNG/capability/status/version reads

Use `math_fixed8.inc` for cheap CPU-side fixed-point motion. The ball and
meta-sprite demos use signed 8.8 velocities plus unsigned 16.8 positions; that
keeps subpixel motion smooth while still writing integer coordinates to the VGC.

## Existing Detailed Docs

| Topic | Doc |
| --- | --- |
| Generated runtime labels | `docs/assembly/runtime-labels.md` |
| XRAM runtime | `docs/assembly/xram.md` |
| XMC notes | `docs/assembly/xmc.md` |
| Fixed-address overlays | `docs/assembly/overlay.md` |
| VTEXT runtime | `docs/assembly/vtext.md` |
| EDITUI runtime | `docs/assembly/editui.md` |
| Memory map and hardware registers | `docs/help/guides/memory-map.md` |
| DMA and blitter guide | `docs/help/guides/dma-and-blitter.md` |
| Graphics and display guide | `docs/help/guides/graphics-and-display.md` |
| Sprites guide | `docs/help/guides/sprites.md` |
| Sound and music guide | `docs/help/guides/sound-and-music.md` |
| Networking guide | `docs/help/guides/networking.md` |

## Useful Examples

| Example | Demonstrates |
| --- | --- |
| `docs/programs/msprite_demo.s` | Meta-sprites, shared animation tracks, fixed-point motion, math coprocessor distance/damping. |
| `docs/programs/sprite_balls_asm.s` | Hardware sprites, fixed-point bouncing physics, math coprocessor collision distance. |
| `docs/programs/sprite_sincos_asm.s` | Hardware sine/cosine through the math coprocessor. |
| `docs/programs/sprite_multiplex_asm.s` | Sprite/copper timing and multiplexing. |
| `docs/programs/xram_shared_runtime_demo.s` | Flat-address XRAM runtime use from assembly. |
| `docs/programs/vtext_status_scroll_demo.s` | VTEXT regions and scrolling text. |
| `docs/programs/math_demos/README.md` | Focused math coprocessor demos. |
