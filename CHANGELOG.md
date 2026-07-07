# Changelog
All notable changes to this project will be documented in this file. See [conventional commits](https://www.conventionalcommits.org/) for commit guidelines.

- - -
## v0.8.0 - 2026-07-07
#### Features
- (**ndk**) spritebank_open — parse + index an NSPR bank on the 6502 - (407f475) - Barry Walker

- - -

## v0.7.2 - 2026-07-07
#### Documentation
- (**nspr**) confirm nova export CLI contract + ack bank↔document model - (065805d) - Barry Walker

- - -

## v0.7.1 - 2026-07-07
#### Documentation
- (**nspr**) macOS agent kickoff §5.1–5.3 + bank↔document model - (d843192) - Barry Walker
#### Build
- (**novadraw**) real Xcode app target (NovaDraw.app), drop sectcreate hack - (522fe30) - Barry Walker

- - -

## v0.7.0 - 2026-07-07
#### Features
- (**novadraw**) NDI access via nova CLI + Settings UI for the CLI path - (3201d2d) - Barry Walker
- (**novadraw**) Swift NSPR codec (consume/persist sprite banks) - (02860ad) - Barry Walker
- (**sprite-editor**) NSPR golden fixture + cross-impl conformance + coordination doc - (255e71c) - Barry Walker
- (**storage**) NSPR metasprite sprite-bank format + codec - (b155f01) - Barry Walker

- - -

## v0.6.0 - 2026-07-07
#### Features
- (**ndk**) mouse event demo app + headless run-test - (faf2970) - Barry Walker
- (**ndk**) mouse event dispatch — click callbacks + areas-of-interest - (a1bf61c) - Barry Walker

- - -

## v0.5.1 - 2026-07-07
#### Bug Fixes
- (**nmouse**) handle USB mouse hotplug/disconnect (hub swaps) - (df47b4b) - Barry Walker

- - -

## v0.5.0 - 2026-07-07
#### Features
- (**novavm**) hw mouse pointer shapes, OSD mouse controls, single-instance guard - (84dbb53) - Barry Walker
#### Bug Fixes
- (**vgc**) single-buffer sprite shape RAM — delete the double-buffer - (3f448f2) - Barry Walker
- (**vgc**) pivot to double-buffer + dirty-RANGE shape copy (drop FIFO) - (3813610) - Barry Walker
- (**vgc**) stage 1 — single-buffer, vblank-gated sprite shape RAM (tear-free) - (1eaadf8) - Barry Walker
#### Tests
- (**vgc**) DUT-level sprite/mouse RENDER coverage + real shape-write path - (192caad) - Barry Walker

- - -

## v0.4.1 - 2026-07-06
#### Bug Fixes
- Zork Zero on Arty — hardware scroll text-wipe + maroon gutter bars - (2b3ffc5) - Barry Walker

- - -

## v0.4.0 - 2026-07-05
#### Features
- add hardware capture and fix novaz v6 clears - (4ac16b0) - Barry Walker

- - -

## v0.3.1 - 2026-07-04
#### Bug Fixes
- require mounted NDI storage for file IO - (b32af2d) - Barry Walker

- - -

## v0.3.0 - 2026-07-04
#### Features
- build native editor runtime - (a3ce3b4) - Barry Walker

- - -

## v0.2.6 - 2026-07-02
#### Refactoring
- share 6502 runtime helpers - (86499c4) - Barry Walker

- - -

## v0.2.5 - 2026-07-01
#### CI/CD
- (**release**) WAL-164 — self-provision cc65 on macOS agent (build-macos) - (ef78c1d) - Forge

- - -

## v0.2.4 - 2026-07-01
#### Bug Fixes
- (**release**) stop cog re-bump loop — chore must not bump [skip ci] - (37e45dd) - Keel
#### CI/CD
- (**release**) WAL-164 — drop AOT-cross arm64 GUI from build-linux matrix - (123f16c) - Forge

- - -

## v0.2.3 - 2026-07-01
#### Bug Fixes
- (**ci**) stage ROM/font resources in tag-release pipelines (build-gate/linux/macos) - (6c8b1b5) - Keel

- - -

## v0.2.2 - 2026-07-01
#### Miscellaneous
- (**version**) v0.2.1 - (dbf373d) - Woodpecker CI

- - -

## v0.2.1 - 2026-07-01
#### Miscellaneous
- (**version**) v0.2.0 - (b313c22) - Woodpecker CI

- - -

## v0.2.0 - 2026-07-01
#### Branding
- (**WAL-19**) add WHI editor-family note to NovaDraw README (#2) - (7fcae78) - Barry Walker
- (**WAL-40**) WHI endorsement + suite cross-link block (#9) - (42358fb) - Barry Walker
#### Licensing
- converge NovaVM GPLv3 -> MIT (WAL-21, CEO-approved) (#12) - (8f52d07) - Barry Walker
#### Features
- (**NovaDraw**) canonical 1280×840 window + dark-locked chrome (WAL-100) (#8) - (b1a7709) - Barry Walker
- (**NovaDraw**) consume PixelCanvasKit via SPM (WAL-35 Phase 0) - (dd580a4) - Barry Walker
- (**arty**) OSD disk-mount overlay — land the parked RTL (4-button BIOS menu) - (1061f01) - Barry Walker
- (**arty**) power-on monkey hold-screen painted from u-boot - (82ce7a3) - Barry Walker
- (**arty**) working read-only-rootfs appliance baseline — boots to BASIC - (b959b29) - Barry Walker
- (**arty**) Arty Z7-20 Linux NovaVM host + reproducible PetaLinux image - (8d5e1fb) - Barry Walker
- (**arty**) make_boot_bin.sh -- non-volatile SD/QSPI boot (FSBL + bit + app) - (3411097) - Barry Walker
- (**arty**) boot splash -- render novavm_logo.nvg on the PS at boot - (9480a17) - Barry Walker
- (**arty**) keyboard visualizer for .sid + .mid -- live notes + metadata - (6b9f11e) - Barry Walker
- (**arty**) faithful reDIP-SID in the fabric -- replaces the software SID - (8ced188) - Barry Walker
- (**arty**) restore 6581 filter curve + per-chip SID model select (toward reDIP-SID) - (c24e505) - Barry Walker
- (**arty**) .sid file playback via the reused ESP SidVm (sandboxed 6502) - (e54d04d) - Barry Walker
- (**arty**) 6502 register-level control of 2 SIDs + wavetable (software synth) - (99ec777) - Barry Walker
- (**arty**) guaranteed-priority audio via a bare-metal TTC0 interrupt - (ed385e5) - Barry Walker
- (**arty**) audio status MMIO ($BA50) + 6502 FIO control (MidPlay/SfLoad/Volume) - (bebef1d) - Barry Walker
- (**arty**) HDMI audio + wavetable MIDI engine (TinySoundFont + TinyMIDI) - (d847450) - Barry Walker
- (**arty**) FIO FCREATE/FWRITE -- write save files into the mounted .ndi - (773393d) - Barry Walker
- (**arty**) drive mount + Z-machine games -- Zork I plays on the Arty - (7060324) - Barry Walker
- (**arty**) NovaHost 6504 mgmt + 6503 debug servers -- `nova --remote` works - (d8dd07a) - Barry Walker
- (**arty**) VGC screen-dump over serial -- wire dbg_vmem read into fio_bridge - (35c6050) - Barry Walker
- (**arty**) USB keyboard polish -- lowercase default + Ctrl/Esc - (dcabd5d) - Barry Walker
- (**arty**) USB keyboard TYPES in NovaVM -- HID boot polling + decode (WORKS) - (e64447e) - Barry Walker
- (**arty**) USB keyboard ENUMERATES -- fix ULPI viewport bit positions - (f380e6a) - Barry Walker
- (**arty**) migrate DIR from extension overlay to FILES module (B migration, #1) - (8c8c252) - Barry Walker
- (**arty**) PS boot-loads basic_rom (idx=0) -> ROM changes w/o bitstream rebuild - (a84f26a) - Barry Walker
- (**arty**) FIO file I/O -- LOAD/SAVE/DIR/DELETE from microSD (LOAD+SAVE working) - (a8947ac) - Barry Walker
- (**arty**) U-Boot USB oracle boots on Arty -> proves USB failure is HW/device-level - (e4bc9f5) - Barry Walker
- (**arty**) USB host -- VBUS+detect+EHCI engine working; FS enum transaction WIP - (c8828c6) - Barry Walker
- (**arty**) USB host phase 1 -- VBUS + device detect (USB keyboard seen!) - (f315744) - Barry Walker
- (**arty**) PS writes modules directly to bank-1 (bypass flaky HP0 page-in) - (ada9bae) - Barry Walker
- (**arty**) status LEDs (CPU-alive/IO) + expose stream_left for page-in debug - (d13a91a) - Barry Walker
- (**arty**) PS console keyboard input + page-in stall diagnostics - (d77094d) - Barry Walker
- (**arty**) PS FIO host loads FILES+SYSTEM modules (embedded) -> page-in works - (374bcc9) - Barry Walker
- (**arty**) PS FIO host boots NovaBASIC to READY via SD module loading (PS=NovaHost) - (fb04776) - Barry Walker
- (**arty**) OOC-synth hdmi (black-box+dcp) to dodge tmds_channel synth crash; fix fio reset - (d100816) - Barry Walker
- (**arty**) PL fio_bridge (AXI-GP) — PS becomes the NovaVM host - (2c332a2) - Barry Walker
- (**arty**) stage SYSTEM module (slot 1) for the READY cursor/line-reader - (76672b5) - Barry Walker
- (**arty**) NovaBASIC reaches READY on Arty Z7-20 (banner+READY over HDMI) - (1098fcd) - Barry Walker
- (**arty**) PL staging FSM pokes resident lib_call loader to $0320 - (59c7a4e) - Barry Walker
- (**arty**) full NovaVM-on-Zynq integration builds (XRAM via PS DDR3), timing met - (2342494) - Barry Walker
- (**arty**) integration block design — PS7 + S_AXI_HP0 + SmartConnect (S_AXI_XRAM) - (7e2c7e2) - Barry Walker
- (**arty**) axi_xram bridge (XRAM-over-AXI into PS DDR3) + Zynq extras roadmap - (eaf656d) - Barry Walker
- (**arty**) Zynq PS smoke-test flow — Vitis Python CLI builds platform + app - (de8e1cf) - Barry Walker
- (**arty**) Zynq PS phase foundation — design doc + PS7 block design + XSA - (c32dd92) - Barry Walker
- (**arty**) boot full NovaVM SoC on Arty Z7-20 (6502+VGC over HDMI, BRAM-only) - (3ab51a0) - Barry Walker
- (**arty-host**) add GSAVE/GLOAD/MKDIR/RMDIR handlers (desktop parity) - (1b0011d) - Barry Walker
- (**autoboot**) runtime launcher + end-to-end /roms launch - (d26f5f6) - Barry Walker
- (**avalonia**) map keyboard to board input so GUI drives JOY() - (8c270ec) - Barry Walker
- (**avalonia**) wire NovaLogo ROM into emulator with boot selection - (f54446a) - Barry Walker
- (**basic**) JOY/SW board-input reporters + input.inc NDK - (afefeb9) - Barry Walker
- (**basic**) route line input through SYS_SCREEN_READLINE - (501456c) - Barry Walker
- (**basic**) carve $0320-$041F resident loader band in NovaBASIC (4c.0a) - (ee7fb00) - Barry Walker
- (**boot**) boot.json stages GRAPHICS module at slot 0 (4b.final) - (ccefa90) - Barry Walker
- (**browser**) add rust wasm core - (d40fafe) - Barry Walker
- (**emu**) resolve LoadRuntime from <saveDir>/roms (NovaHost parity) - (1c00e69) - Barry Walker
- (**emu**) stage runtime extension in XRAM for re-page (4c.0c) - (ca8bd05) - Barry Walker
- (**emu**) PGD page-in + XRAM shelf + resident lib_call in CompositeBusDevice (4c.0b) - (9dc49e9) - Barry Walker
- (**fio**) file-op parity across BASIC/Forth/Logo - (41d551a) - Barry Walker
- (**forth**) music visualizer demo (musicviz.4th) - (9012978) - Barry Walker
- (**forth**) one-word SFLOAD/MIDPLAY/SIDPLAY, GRAPHICS mode-3 fix, demos - (8f39f71) - Barry Walker
- (**forth**) add NovaForth runtime and libraries - (13b39b5) - Barry Walker
- (**fpga**) restructure to shared core + per-board dirs; add Arty Z7-20 (HDMI+VGC) - (1c68f7b) - Barry Walker
- (**fpga**) CPU MMIO page-in -> page_dma -> 16 KB resident ext_rom (Task 11d) - (6072088) - Barry Walker
- (**fpga**) page_dma writes ext_rom 8-bit, 2 bytes/word, self-paced (Task 11c) - (ec3dd21) - Barry Walker
- (**fpga**) dual-clock ext_rom + boot-bridge CDC into sdram_clk (Task 11b) - (6e75b61) - Barry Walker
- (**fpga**) page_dma — stream SDRAM page-mode read into ext_rom (Task 10) - (bb4d5b2) - Barry Walker
- (**fpga**) add ext_rom write-source mux (boot bridge vs page_dma) - (f87d544) - Barry Walker
- (**fpga**) mirror screen-base register + direct screen window to VGC RTL - (7faf397) - Barry Walker
- (**fpga**) $BA63 DMA controller + bridge-side SDRAM preload path - (cc72b77) - Barry Walker
- (**fpga**) Phase 2.5 Step 2 — XRAM on SDRAM (4KB→512KB) - (f28df2b) - Barry Walker
- (**fpga**) Phase 2.5 Step 1 — SDRAM controller bring-up - (e6c176d) - Barry Walker
- (**fpga**) 256-byte key input FIFO — fixes key drops under burst typing - (bf9ab22) - Barry Walker
- (**fs**) drive-aware DIR/CATALOG/SAVE/LOAD/CD/PWD across host, NDK, forth - (153dfb4) - Barry Walker
- (**fw**) boot-stage paged-library modules from SD into XRAM (graceful) - (360981c) - Barry Walker
- (**gfx**) anim $C0-$C7 + tween $D0-$DA (last driver batches) (4b.11) - (b427c00) - Barry Walker
- (**gfx**) image/mem $A0-$A9 (MEMREAD/WRITE/VPEEK/VPOKE/GSAVE/GLOAD/NVGload x4) (4b.9) - (cfaf9c9) - Barry Walker
- (**gfx**) msprite $80-$8B (spawn/destroy/show/hide/pos/frame/anim/priority/transcolor/tick/commit) (4b.8) - (a8c3b06) - Barry Walker
- (**gfx**) vsprite $60-$71 (blit/fill/rotate/gfx_*/save-restore-bg/scene compositor) (4b.7) - (60fc8d7) - Barry Walker
- (**gfx**) blit/dma $50-$5B (blitcopy/blitfill/start/wait/dmacopy/dmafill + status/err/count) (4b.6) - (767c233) - Barry Walker
- (**gfx**) copper $40-$49 (list/add/clear/on/off/use/end/split/set_reg/set_sprite_reg) (4b.5) - (5ea2472) - Barry Walker
- (**gfx**) hw-sprite $20-$3B (define/row/pos/flip/priority/shape/setreg/getx/gety + collision API) (4b.4) - (69cdd54) - Barry Walker
- (**gfx**) text/mode $10-$1B (COLOR/FONT/MODE/REVERSE/FLASH/LOCATE/CLS/DISPLAY) (4b.3) - (8fc5ca0) - Barry Walker
- (**gfx**) draw primitives $01-$09 (GCOLOR/PLOT/UNPLOT/LINE/CIRCLE/RECT/FILL/PAINT/GTEXT) (4b.2) - (4998fe3) - Barry Walker
- (**gfx**) GRAPHICS module skeleton + loader hook + GCLS proven end-to-end (4b.1) - (93acf95) - Barry Walker
- (**graphics**) turtle command engine GFN_TURTLE_OP (move-math+reporters in module) (4c.2-3-i) - (20e4314) - Barry Walker
- (**graphics**) turtle-render GFNs $B0-$B2 (INIT/DRAW/ERASE) (4c.2-2) - (bf5dc3d) - Barry Walker
- (**lib**) reserve module-BSS band $0420-$08FF cross-runtime (4c.2-1) - (686b6c6) - Barry Walker
- (**lib**) BYTES round-trip — testmod SUM fn + (zp),Y deref tests - (957cdd4) - Barry Walker
- (**lib**) lib_call loader — page-on-miss, validate, dispatch, errors - (a8fd0b0) - Barry Walker
- (**lib**) test module (id=$7F, ECHO/ADD) + 16KB image + header test - (d86a2ad) - Barry Walker
- (**lib**) reserve mailbox $0300-$031F in NovaLogo (cfg carve) - (d1175b2) - Barry Walker
- (**lib**) reserve mailbox $0300-$031F in BASIC (Ram_base->$0320) - (0bde97c) - Barry Walker
- (**lib**) shared paged-library ABI + module header macro - (febdd0f) - Barry Walker
- (**libabi**) reserve MODULE_ID_FILES($04)/MEMORY($05)/NET($06) - (99d6fb6) - Barry Walker
- (**logo**) MKDIR/RMDIR/CD/PWD dir-nav + full harness coverage - (4d6057c) - Barry Walker
- (**logo**) SAVE dumps whole workspace (procedures + global variables) - (076ebcf) - Barry Walker
- (**logo**) SAVE/LOAD/CATALOG/DIR + Logo file type 0x07 across hosts - (e9f0608) - Barry Walker
- (**logo**) route turtle commands to GRAPHICS module via thin adapter (4c.2-3-ii) - (aaab619) - Barry Walker
- (**logo**) route draws+SETCOLOR+sprites through lib_call(GRAPHICS) (4c.1-3) - (997ac91) - Barry Walker
- (**logo**) ensure_ext_resident re-page-on-demand for the runtime extension (4c.0c) - (3e5ac16) - Barry Walker
- (**logo**) marshalling branches ext_mod_id -> legacy trampoline | lib_call (4c.1-2) - (9543483) - Barry Walker
- (**logo**) add module_id column to ext_cmd_table (4c.1-1) - (019c74b) - Barry Walker
- (**logo**) carve $0320-$041F resident loader band in NovaLogo (4c.0a) - (4e42842) - Barry Walker
- (**math**) integer SQRT in the coprocessor, correct across BASIC/Forth/Logo - (9db3523) - Barry Walker
- (**modules**) extract turtle into a dedicated TURTLE module ($07) - (84ffd03) - Barry Walker
- (**modules**) wire production demand-load so >4 modules are runtime-reachable - (aad1238) - Barry Walker
- (**modules**) expand SYSTEM module with nui + rng + overlay - (c1b4fd3) - Barry Walker
- (**modules**) add NET module wrapping the nic + gameserver NDK - (fe6956c) - Barry Walker
- (**modules**) add MEMORY module wrapping the xram + xmc NDK - (1452096) - Barry Walker
- (**modules**) add FILES module wrapping the fio + pager NDK - (b738056) - Barry Walker
- (**modules**) expand SOUND module to the full audio NDK - (45dee45) - Barry Walker
- (**modules**) docs-from-NDK — map all 118 graphics wrappers to their NDK routines - (987c687) - Barry Walker
- (**modules**) docs-from-NDK — single-source fn summaries from the NDK (sound+system) - (ba53944) - Barry Walker
- (**modules**) delete Logo extension ROM — TONE/NOISE/VOLUME->SOUND, WAIT/WAITVBL/TIMER->SYSTEM - (b4f3332) - Barry Walker
- (**nmod**) phase 5 — web Modules panel with searchable, paged function modal - (d48ea1b) - Barry Walker
- (**nmod**) phase 4b+2 — graphics.nmod + firmware stage-first-N (HW verified) - (4893205) - Barry Walker
- (**nmod**) phase 4a — symbol resolution + sparse-dispatch validation - (d1b7655) - Barry Walker
- (**nmod**) phase 3 — NovaModule parser + `nova module` commands - (37f2c7c) - Barry Walker
- (**nmod**) phase 1 — self-documenting module packer + testmod end-to-end - (59b6749) - Barry Walker
- (**nova**) full-screen composite screenshot in `vm screenshot` - (631a4bc) - Barry Walker
- (**nova**) vm screenshot -- capture the VGC gfx plane to PNG - (b8cba64) - Barry Walker
- (**nova**) runtime boot-floppy command - (dc9651a) - Barry Walker
- (**novabasic**) add fun and games book - (17282c1) - Barry Walker
- (**novadraw**) WHI token layer + standard keymap + status strip (WAL-29) - (d686488) - Barry Walker
- (**novahost**) add NovaDebugClient with framing tests - (c5eb9bc) - Barry Walker
- (**novahost**) SD I/O Phase 3 — switch SDIO → SPI + 4 bringup fixes - (127f6be) - Barry Walker
- (**novahost**) SD I/O Phase 1+2 + FPGA SID test suite - (8b60e4c) - Barry Walker
- (**novahost**) static IP + Makefile absolute tool paths - (844481c) - Barry Walker
- (**novalogo**) shell-style up/down command history at the REPL prompt - (af163e9) - Barry Walker
- (**novalogo**) EDIT command — reopen a procedure in the editor - (669cdad) - Barry Walker
- (**novalogo**) forgiving lexer — multi-line statements, comments, REPL continuation - (19bc93f) - Barry Walker
- (**novalogo**) sprites, SID sound, and timing commands - (5983f84) - Barry Walker
- (**novalogo**) VGC graphics — LINE, CIRCLE, RECT, FILL, PAINT, PLOT - (31f9993) - Barry Walker
- (**novalogo**) screen modes, turtle queries, and pen commands - (1acefd2) - Barry Walker
- (**novalogo**) turtle graphics — FD, BK, RT, LT, CS, PU, PD, ST, HT, HOME - (796f030) - Barry Walker
- (**novalogo**) extension ROM architecture with pre-evaluated args - (22d46d6) - Barry Walker
- (**novalogo**) PO, POTS, ERASE, APPLY workspace commands - (abac239) - Barry Walker
- (**novalogo**) math functions, logic, and type predicates - (c4e593c) - Barry Walker
- (**novalogo**) READCHAR, READWORD, READLIST, CHAR, ASCII - (bbcac7a) - Barry Walker
- (**novalogo**) FOR, WHILE, UNTIL loops and REPCOUNT - (e51aa24) - Barry Walker
- (**novalogo**) CATCH/THROW structured error handling - (f662b36) - Barry Walker
- (**novalogo**) mark-and-sweep garbage collector - (7728bf2) - Barry Walker
- (**novalogo**) FPUT, LPUT, LIST, SENTENCE, WORD, and RUN - (de60601) - Barry Walker
- (**novalogo**) list literals and core list operations - (3de626b) - Barry Walker
- (**novalogo**) verified recursion — COUNTDOWN and FACT 5 = 120 - (117fe4b) - Barry Walker
- (**novalogo**) STOP and OUTPUT for procedure control flow - (4291a12) - Barry Walker
- (**novalogo**) TO/END procedure definition and invocation - (f1f2b39) - Barry Walker
- (**novalogo**) IF, IFELSE, and comparison operators - (37dd935) - Barry Walker
- (**novalogo**) REPEAT with nested body execution - (0f9ad6e) - Barry Walker
- (**novalogo**) variables with MAKE and :reference - (1ceea22) - Barry Walker
- (**novalogo**) arithmetic operators via math coprocessor - (cb6a7df) - Barry Walker
- (**novalogo**) core evaluator with PRINT and TYPE - (a76391b) - Barry Walker
- (**novalogo**) tokenizer — scan input into linked token list - (8e68665) - Barry Walker
- (**novalogo**) heap allocator and token data structures - (b4b5d80) - Barry Walker
- (**novalogo**) line input buffer with backspace handling - (fe258e7) - Barry Walker
- (**novalogo**) skeleton ROM with banner and input echo - (08363ad) - Barry Walker
- (**novapanel**) persist host and window bounds - (86b1e3e) - Barry Walker
- (**novapanel**) responsive main window wiring keyboard, panels, connection - (b8e07f5) - Barry Walker
- (**novapanel**) RuntimePanel, LibraryPanel, ModulesPanel - (9346825) - Barry Walker
- (**novapanel**) DrivesPanel and DiskImagesPanel - (3e87276) - Barry Walker
- (**novapanel**) ConnBar and StatusPanel - (248066f) - Barry Walker
- (**novapanel**) connection service with poll loop and state machine - (4cbddf3) - Barry Walker
- (**novapanel**) typed status parser - (550091f) - Barry Walker
- (**novapanel**) responsive size-class controller - (89181a0) - Barry Walker
- (**novapanel**) physical key capture and highlight - (1e9bc17) - Barry Walker
- (**novapanel**) keyboard view with click dispatch - (9a98d5c) - Barry Walker
- (**novapanel**) ordered key sender service - (f135ddc) - Barry Walker
- (**novapanel**) keyboard layout model - (96d9f9d) - Barry Walker
- (**novapanel**) key dispatch resolver with full coverage - (f8e9479) - Barry Walker
- (**novapanel**) scaffold Avalonia app shell - (4add6f9) - Barry Walker
- (**novaz**) render bold + reverse via the VGC text attribute, not the palette - (1fe00f0) - Barry Walker
- (**novaz**) Zork Zero with pictures — capture, re-pin, docs (M3 Task 7) - (4f43eb0) - Barry Walker
- (**novaz**) V6 carriage-return interrupt countdown (M3 Task 6) - (e5fce79) - Barry Walker
- (**novaz**) draw_picture/erase_picture render on the gfx layer (M3 Task 5) - (aa14190) - Barry Walker
- (**novaz**) picture_data + Flags1 answered from the real index (M3 Task 4) - (3e0b019) - Barry Walker
- (**novaz**) PICS.PAK index boot-load via existing FIO XPAGE (M3 Task 3) - (99dbd89) - Barry Walker
- (**novaz**) V6 EGA palette mode + live colour mapping (M3 Task 2) - (7a379f6) - Barry Walker
- (**novaz**) PICS.PAK picture pack + Packer --pictures (M3 Task 1) - (f7b52b0) - Barry Walker
- (**novaz**) zork-zero renders its real layout (M2) — smoke re-pinned - (58c7975) - Barry Walker
- (**novaz**) V6 scroll_window over vtext regions - (ea18326) - Barry Walker
- (**novaz**) V6 windows drive vtext regions — real banner/playfield layout - (eae0374) - Barry Walker
- (**novaz**) NZ6 dispatch trace mode in smoke harness - (f85cb1b) - Barry Walker
- (**novaz**) zork-zero project — V6 reaches first prompt (M1) - (fd31532) - Barry Walker
- (**novaz**) V6 picture/mouse/menu stubs report no-capability honestly - (0756938) - Barry Walker
- (**novaz**) V6 user stacks (push_stack/pop_stack) - (feb9a1f) - Barry Walker
- (**novaz**) minimal V6 8-window model in NOVAZ6 segment - (4f36726) - Barry Walker
- (**novaz**) V6 EXT/VAR dispatch routing into NOVAZ6 segment - (f39d042) - Barry Walker
- (**novaz**) RAM-resident NOVAZ6.BIN segment — build, pack, boot-load, magic check - (998ab34) - Barry Walker
- (**novaz**) V6 startup calls main routine; frame-0 return quits - (4ca41eb) - Barry Walker
- (**novaz**) V6 packed-address routine/string offset paths - (81283e9) - Barry Walker
- (**novaz**) accept V6 stories; read routine/string packed offsets - (67d07f2) - Barry Walker
- (**novaz**) auto-discovered Infocom NDI collection + soundfont preload at boot - (2eb3dbd) - Barry Walker
- (**novaz**) V5 sound finish-routine callback on hardware - (dd0e1a4) - Barry Walker
- (**platform**) stabilize ndk and novaz v6 - (53139fc) - Barry Walker
- (**platform**) add z6 display support - (31e7968) - Barry Walker
- (**runtime**) nova.lib feature-group library for NDK dead-code stripping - (8e65daa) - Barry Walker
- (**sdram**) add stream_ready back-pressure to page-mode read engine (Task 11a) - (f60acf2) - Barry Walker
- (**sdram**) page-mode streaming read branch (ACTIVATE/READ/PRECHARGE/REFRESH) - (ec9846e) - Barry Walker
- (**sdram**) add stream_* port stubs for page-mode burst read - (d154486) - Barry Walker
- (**shelf**) firmware CMD_LOAD_MODULE handler (SD->XRAM slot stream) - (ab0bd1d) - Barry Walker
- (**shelf**) runtime miss handler — load-on-demand + LRU eviction - (f7de123) - Barry Walker
- (**shelf**) emulator FIO LOAD_MODULE (demand-load stub for miss path) - (d45ac43) - Barry Walker
- (**shelf**) web module staging endpoints + libraries[] schema (no base) - (ad97efd) - Barry Walker
- (**shelf**) firmware slot=index allocation + directory seeding - (6acd732) - Barry Walker
- (**shelf**) directory-scan loader replaces static modtab_lookup - (c816645) - Barry Walker
- (**shelf**) reserve shelf_tag/shelf_lru directory in loader band - (1bf9839) - Barry Walker
- (**system**) SYS_SCREEN_READLINE reads multi-row wrapped logical lines on ENTER - (b250e56) - Barry Walker
- (**system**) cursor advance + scroll on ENTER - (ea20594) - Barry Walker
- (**system**) destructive backspace in the screen editor - (37795f0) - Barry Walker
- (**system**) arrow-key cursor navigation with edge clamping - (b3be024) - Barry Walker
- (**system**) echo typed chars to the screen window - (04eb29c) - Barry Walker
- (**system**) SYS_SCREEN_READLINE reads the cursor's screen row on ENTER - (b7a0f44) - Barry Walker
- (**system**) reserve SYS_SCREEN_READLINE fn id + skeleton - (217df10) - Barry Walker
- (**system**) move text editor from Logo extension ROM into shared SYSTEM module - (8fd6f2b) - Barry Walker
- (**tcp**) board_input command to inject joystick/switch state - (218527e) - Barry Walker
- (**tools**) beast-synth.sh — single-command FPGA synth wrapper - (9a48bb7) - Barry Walker
- (**tools**) complete xml2mml.py with key signature tracking and polish - (e69ebf2) - Barry Walker
- (**tools**) complete xml2mml.py pipeline with CLI, voice selection, and BASIC output - (65785a3) - Barry Walker
- (**tools**) add voice selection by density and GM instrument mapping - (cffde16) - Barry Walker
- (**tools**) add repeat expansion and transposing instrument support - (34cf2a6) - Barry Walker
- (**tools**) add ornament expansion — mordent, turn, fermata, grace notes - (afda873) - Barry Walker
- (**tools**) add chord splitting into monophonic sub-voices - (4e9ebc4) - Barry Walker
- (**tools**) parse MusicXML expressions — dynamics, slurs, staccato, trills, wedges - (407b4f4) - Barry Walker
- (**tools**) add MusicXML note parsing with pitch, duration, dots, ties - (f0304a5) - Barry Walker
- (**tools**) scaffold xml2mml.py with MXL/XML file loading - (47914e0) - Barry Walker
- (**turtle**) Logo-faithful coordinates — centered origin, +Y up, WRAP - (4b5cbdb) - Barry Walker
- (**vgc**) per-character transparent background + inline PETSCII style codes - (7425f3c) - Barry Walker
- (**vgc**) bold text + transactional mixed scroll; fix(novaforth): dictionary base - (7b0d6de) - Barry Walker
- (**vgc**) Phase 2 Batch B — gfx_mem 64000→76800, artist GFX_H=240 - (ae55285) - Barry Walker
- (**vgc**) Phase 2 Batch A — 80×60 text + 320×240 gfx geometry - (c25e02a) - Barry Walker
- (**vgc+sid**) Phase 2 finish — signed-16 sprites, 3-font, SID curve → SDRAM - (66ba78a) - Barry Walker
- (**wts**) add MCP tool methods for wavetable synth control - (aa2d823) - Barry Walker
- (**wts**) add WTS TCP commands for soundfont/voice control - (8541c86) - Barry Walker
- (**wts**) add @W MML command for WTS instrument selection - (ba29180) - Barry Walker
- (**wts**) add SfLoad command to FileIoController for soundfont loading - (371a70e) - Barry Walker
- (**wts**) add auto/manual/SID-only MIDI routing modes for 14 voices - (072d0a9) - Barry Walker
- (**wts**) expand MusicEngine to 14 voices with SID/WTS routing - (3010447) - Barry Walker
- (**wts**) wire WTS registers into CompositeBusDevice memory map - (49c8c37) - Barry Walker
- (**wts**) add WTS register address constants to VgcConstants - (1cdc420) - Barry Walker
- (**wts**) add stereo mode to OpenAlRenderer, wire up WTS audio output - (e2ca710) - Barry Walker
- (**wts**) add reverb and chorus effects, integrate into render pipeline - (4798c6b) - Barry Walker
- (**wts**) add 8-voice wavetable synth core with ADSR, volume, panning, pitch bend - (843d8be) - Barry Walker
- (**wts**) add SF2 soundfont loader with RIFF parser - (ef53168) - Barry Walker
- (**wts**) add SampleBank data model with instrument/region/key zone lookup - (567ce30) - Barry Walker
- share VGC palette helpers - (6c17526) - Barry Walker
- migrate NVG images to native packed format - (9de5965) - Barry Walker
- expose packed gfx blit helper - (dd79158) - Barry Walker
- support portable V6 graphics on hardware - (975d649) - Barry Walker
- add nova management control center - (0bca3a5) - Barry Walker
- add nova web control server - (794cbf8) - Barry Walker
- shared editor rewrite + NovaLogo turtle/graphics fixes + tooling - (8f4b34d) - Barry Walker
- editor minimal-update render + VGC text primitives + CPU store-prefetch fix - (439241c) - Barry Walker
- config-driven ROM loader + nova runtime CLI - (e832d8b) - Barry Walker
- add vsprite rotation demos and keyboard updates - (b68471b) - Barry Walker
- expose ULX3S board inputs - (813d799) - Barry Walker
- add hardware audio and transport stack - (b71c06d) - Barry Walker
- checkpoint sid audio and runtime libraries - (e439022) - Barry Walker
- add meta-sprite library and hardware demo - (f6b992f) - Barry Walker
- add math coprocessor and sprite demos - (1fe74fd) - Barry Walker
- add fixed-address overlay support - (f4de0bc) - Barry Walker
- advance nova networking and chess stack - (3e0bc8b) - Barry Walker
- add NovaDraw and Nova runtime updates - (4570c25) - Barry Walker
- checkpoint runtime and hardware stack - (abb3a6d) - Barry Walker
- checkpoint novaz z3 runtime - (5a947be) - Barry Walker
- add runtime infrastructure and shared library fixes - (1d0d171) - Barry Walker
- add wifi management and shared runtime libraries - (df30dc5) - Barry Walker
- extract shared nova runtime libraries - (2ed8174) - Barry Walker
- add shared xram and vtext runtimes - (f60d448) - Barry Walker
- add shared xram runtime and regressions - (ff4c8dc) - Barry Walker
- add display attributes and hardware tests - (c5991d5) - Barry Walker
- align video boot and runtime behavior - (1c2fd1a) - Barry Walker
- add SD boot splash logo fade - (2013452) - Barry Walker
- add SD-backed Nova asset management - (5d48db0) - Barry Walker
- stabilize fpga video and sprite pipeline - (b0d08ab) - Barry Walker
- runtime ROM load via NovaHost — eliminates 17-min synth per ROM change - (9ce32cb) - Barry Walker
- NovaHost OTA support — never touch serial again - (433f9e2) - Barry Walker
- FPGA debug bridge — ESP32 test infrastructure for hardware testing - (a665cfb) - Barry Walker
- FPGA keyboard input, case-insensitive tokenizer, VGC read mux - (1a91159) - Barry Walker
- vga2dvid HDMI, ROM single-source, ESP32 credentials cleanup - (88f80c4) - Barry Walker
- EhBASIC boots on ULX3S HDMI — splinedrive TMDS, seed 5 - (a93870e) - Barry Walker
- NovaHost ESP32 firmware v0.2 — WiFi debug log server - (c61a79b) - Barry Walker
- FPGA synthesis and first successful boot on ULX3S ECP5-85F - (1314563) - Barry Walker
- dual SID 6581/8580 FPGA implementation with runtime model select - (2ee4e04) - Barry Walker
- integration testing, FPGA Verilator sim, blitter sync, tokenizer fixes - (cb84a3e) - Barry Walker
- add TBUF/TBSET/TBFILL/TBPUT tile buffer commands, fix music timing - (e5de29c) - Barry Walker
- implement TILECOL/TSCROLLX/TSCROLLY functions, restore full keyword names - (cd7f746) - Barry Walker
- add NES-style tile map engine (Mode 4) with full stack - (933f16d) - Barry Walker
- migrate PWD/XMEM/XDIR to extension ROM, add ellipse and GTEXT commands - (3a3620c) - Barry Walker
- add extension ROM bank switching with SFLOAD command - (a4aa448) - Barry Walker
- support LOAD "file.sf2" for soundfont loading - (5d82681) - Barry Walker
- overhaul SID playback, expand demo disk, fix WTS pitch tuning - (9d0c464) - Barry Walker
- add keyboard loader, playback start/stop, and main browser loop - (6608cf1) - Barry Walker
- add tab bar, file list, scroll text, and nav bar subroutines - (7f4aafb) - Barry Walker
- add directory loading, graphics header, and copper raster animation - (9b9f3d9) - Barry Walker
- demo disk player build infrastructure, data layout, and utilities - (ac1d7e4) - Barry Walker
- add demo disk player skeleton (tasks 1-3) - (7878ebe) - Barry Walker
- add WTS routing, compact mode, and max-voices to MIDI-to-MML converter - (2da9347) - Barry Walker
- add all 5 appendices to Mapping the NovaVM - (9982422) - Barry Walker
- add Chapter 12 (SID Sound Chips) - (f3b310c) - Barry Walker
- add Chapter 10 (Expansion Memory) and Chapter 11 (ROM) - (26ce4da) - Barry Walker
- add Chapter 8 (Screen Memory) and Chapter 9 (File I/O) - (7fcb315) - Barry Walker
- add Chapter 6 (NIC) and Chapter 7 (WTS) - (e3cddd3) - Barry Walker
- add Chapter 5 (VGC) to Mapping the NovaVM - (5b391ad) - Barry Walker
- add Chapter 4 (BASIC Program Space) - (42c8e32) - Barry Walker
- add Chapter 2 (Stack) and Chapter 3 (Vector Table) - (f6caf52) - Barry Walker
- add Chapter 1 (Zero Page) to Mapping the NovaVM - (f1887a8) - Barry Walker
- add master document and build infrastructure for Mapping the NovaVM - (7c655f7) - Barry Walker
- add unified voice routing demo (invention8_unified.bas) - (1f02611) - Barry Walker
- auto-load soundfont when MML uses WTS voices - (8d35d41) - Barry Walker
- extend MNOTE() and MUSIC to support all 14 voices - (11d0661) - Barry Walker
- wire MidPlayParser into FileIoController and TCP server - (bd4bb10) - Barry Walker
- RouteVoices accepts explicit voice-to-channel mapping - (10ca0b9) - Barry Walker
- add MidPlayParser for MIDPLAY voice=channel mapping syntax - (5472519) - Barry Walker
- guard SID/WTS instrument and effect commands by voice type - (9148991) - Barry Walker
- replace @W with @I and @D MML commands for WTS instruments - (d412554) - Barry Walker
- keyboard visualizer shows 14 voices with SID/WTS legend - (eb32f9e) - Barry Walker
- keyboard visualizer shows 14 voices with SID/WTS legend - (82ef6b3) - Barry Walker
- auto-load GeneralUser_GS soundfont when MIDI needs WTS voices - (850f075) - Barry Walker
- CompositeBusDevice serves 14 voice note registers - (1ea0424) - Barry Walker
- extend VgcConstants with 14 voice note registers for WTS support - (3b5f374) - Barry Walker
- ScreenTextEditor keyboard handling with all shortcuts - (b72671f) - Barry Walker
- device-aware file browser with save/load - (136d6fd) - Barry Walker
- ScreenTextEditor rendering, screen save/restore, prompt, search - (da12c55) - Barry Walker
- add rendering, screen save/restore, prompt, and search to ScreenTextEditor - (5cfa001) - Barry Walker
- text selection with shift+movement, clipboard integration - (24489c4) - Barry Walker
- add ScreenTextEditor abstract base class with text buffer and cursor - (10ad3f9) - Barry Walker
- BasicEditor edge cases, line number cap, and additional tests - (6a17a40) - Barry Walker
- BasicEditor clipboard Ctrl+C/X/V - (afcd913) - Barry Walker
- BasicEditor Ctrl+F find, Ctrl+G go to line, status bar polish - (a15581e) - Barry Walker
- F5 run-return-to-editor loop and keyboard wiring - (3c748a1) - Barry Walker
- BasicEditor core — text buffer, keyboard, display, memory round-trip - (7b1887c) - Barry Walker
- add BASIC syntax highlighter for full-screen editor - (9de08a9) - Barry Walker
- add DIROPEN, DIRNEXT, DIRNAM$, DIRSIZ, DIRTYP, META$ to EhBASIC ROM - (d6aa0a6) - Barry Walker
- filtered DoDirOpen with metadata extraction (Tasks 4-6) - (bc2fa9b) - Barry Walker
- add FilterPattern record and ParseFilterPattern to FileIoController - (31877a3) - Barry Walker
- add GlobMatch wildcard utility to FileIoController - (486fd06) - Barry Walker
- add file metadata buffer constants to VgcConstants.cs - (f3a0c70) - Barry Walker
- autoboot routine in ROM + C# skip support - (b7d4c58) - Barry Walker
- add CD/MKDIR/RMDIR/FORMAT/MOUNT/UNMOUNT/PWD keywords to ROM - (4d2eb4e) - Barry Walker
- e6502.NDI CLI tool for disk image management - (6b12fcd) - Barry Walker
- wire DeviceManager into CompositeBusDevice - (bf6d078) - Barry Walker
- DeviceManager with auto-mount and autoboot detection - (3d6c2fb) - Barry Walker
- NdiFloppyDevice wraps NdiImage as IStorageDevice - (6d9f874) - Barry Walker
- IStorageDevice interface and HostDirectoryDevice - (cda23eb) - Barry Walker
- BasicTokenizer with tokenize/detokenize and tests - (1ae29cb) - Barry Walker
- gen_tokens.py extracts token table from basic.asm - (3e94fbb) - Barry Walker
- NdiImage create/open/read/write/delete with tests - (be6bb6b) - Barry Walker
- NdiDirectory entry CRUD with subdirectories - (baa64e9) - Barry Walker
- NdiBam block allocation map with tests - (d2428c8) - Barry Walker
- NdiHeader read/write with tests - (df68932) - Barry Walker
- add e6502.Storage class library project - (c76a5a4) - Barry Walker
- auto-rebuild keyboard viewer and novavm.inc during dotnet build - (811be10) - Barry Walker
- keyboard visualizer, MIDI instrument improvements, DIR/LOAD enhancements - (e2051e5) - Barry Walker
- keyboard visualizer assembly program - (28414e3) - Barry Walker
- add keyboard.cfg linker config and Makefile targets - (06fd123) - Barry Walker
- add gen_novavm_inc.py to auto-generate ca65 hardware constants - (cdc570b) - Barry Walker
- SidPlayer populates voice note registers via frequency reverse-mapping - (0a65450) - Barry Walker
- add .bin file extension support for binary executables - (f2e1706) - Barry Walker
- add elapsed/total frame registers at $BA57-$BA5A - (19b10ae) - Barry Walker
- rename CALL command to SYS for familiarity - (5b7f567) - Barry Walker
- integrate MIDPLAY/MIDSTOP with FileIoController and TCP server - (4827932) - Barry Walker
- add MIDPLAY/MIDSTOP BASIC command tokens and handlers - (7da2313) - Barry Walker
- add MidiPlayback real-time driver with direct voice control - (f4d79ea) - Barry Walker
- add mid2bas CLI verb to e6502.Tools - (a31935b) - Barry Walker
- add .bas program generation with MML splitting - (b2b85e7) - Barry Walker
- add MML generation with monophonic reduction - (202a9ee) - Barry Walker
- add MIDI duration quantization with tie support - (7e84b85) - Barry Walker
- add instrument bucket mapping and velocity conversion - (a00a050) - Barry Walker
- add MidiEngine with channel analysis and selection - (6c24ebc) - Barry Walker
#### Bug Fixes
- (**NovaVM**) WAL-148 ADR-0007 preStop drain — close rollout 502 window (#16) - (357e4aa) - Barry Walker
- (**arty**) disarm SWDT early in boot via watchdog-off init script (no brick risk) - (f6e7e80) - Barry Walker
- (**arty**) OSD/autoboot polish — clear screen on boot + reboot-to-BASIC on unmount - (9a17793) - Barry Walker
- (**arty**) boot splash via VGC dbg_vmem write port (not poke-the-command) - (4aef325) - Barry Walker
- (**arty**) repo-local ps7_init in boot_fio_noreset.tcl + document .ndi/NovaZ launch - (6fa6aeb) - Barry Walker
- (**arty**) 8580 SID static -- DC-blocker gain clipped the hot 8580 output - (1128fbe) - Barry Walker
- (**arty**) .sid auto-detects SID model/clock + subtune index; robust boot - (087a1eb) - Barry Walker
- (**arty**) axi_xram spurious re-read -- DMA/blitter XRAM off-by-one (Zork Zero) - (903d35e) - Barry Walker
- (**arty**) PS Ethernet RX works -- clear NVLANDISC + RTL8211F speed read - (3582558) - Barry Walker
- (**arty**) cap module shelf at 1 entry so multi-module alternation is correct - (995f754) - Barry Walker
- (**arty**) link OOC hdmi dcp via impl OPT_DESIGN pre-hook -> fio_bridge BITSTREAM builds - (5a5caed) - Barry Walker
- (**arty**) SYNTHESIS/VIDEO_720X480 via global-include defines.vh - (e0b1ca2) - Barry Walker
- (**arty**) set fileset verilog_define too; add PS FIO host app - (af2e11f) - Barry Walker
- (**arty**) stop pre-synth hook wiping rom/ (root cause of ROM=0 BRK loop) - (959a06f) - Barry Walker
- (**arty**) ROM BRAM loads under project-mode synth; isolate READY blocker - (051e4f3) - Barry Walker
- (**avalonia**) ROMSWAP_PRIMARY restores the active runtime, not always BASIC - (e6558b0) - Barry Walker
- (**browser**) link ZSoundController + MathCoprocessor into e6502.Browser - (2609ba1) - Barry Walker
- (**ci**) clean build artifacts before cog bump so the version tag cuts - (952fabc) - Keel
- (**ci**) pre-create staging dirs so 'make binaries' stage-resources cp succeeds - (662a173) - Keel
- (**ci**) run 'make binaries' to generate+stage ROM/font resources before build - (7fa40f6) - Keel
- (**ci**) serialize MSBuild (-maxcpucount:1) to avoid nova.lib ROM build race - (029cf2a) - Keel
- (**ci**) build w65c02-capable cc65 from source for ehbasic ROM build - (cb4f388) - Keel
- (**ci**) push bump commit+tag via HEAD:main --tags (WAL-41) - (8471473) - Forge
- (**ci**) gitignore minted .ci-token so cog bump sees a clean tree (WAL-41) - (a611009) - Forge
- (**ci**) disable osxkeychain helper in build-novadraw (WAL-99) (#7) - (03a45dc) - Barry Walker
- (**emu**) LOADRUNTIME resolves mounted devices before <saveDir>/roms - (664b88b) - Barry Walker
- (**forth**) restore PRIMARY home bank after lib calls (was FORTH bank) - (fafd35e) - Barry Walker
- (**fpga**) page-in byte-exact on HW — SDRAM tRFC + read pacing - (b339d49) - Barry Walker
- (**fpga**) coherent CDC FIFO for boot-bridge ext_rom writes (Task 11b regression) - (775f9ee) - Barry Walker
- (**fpga**) page_dma — words==0 immediate completion (no FSM wedge) - (816798e) - Barry Walker
- (**fpga**) SLB → explicit FF array — kills phantom sprite bar - (70bbfb6) - Barry Walker
- (**fpga**) GSR=ENABLED on every FF — root-cause POR determinism - (fa297d6) - Barry Walker
- (**fpga**) POR-init BRAM dout regs — kills last 20% rendering glitch - (c440eb8) - Barry Walker
- (**fpga**) POR determinism via FF init values + PLL_LOCK CDC sync - (c8a6673) - Barry Walker
- (**fpga**) XRAM write back-pressure + read handshake — xram 3/9 → 9/9 - (af7af6a) - Barry Walker
- (**fpga**) drop SID-over-ROM read intercept — arrays + xram STASH/FETCH - (3bc1454) - Barry Walker
- (**fpga**) blitter 1-cycle dpram read latency — TDD, same class as dma.sv - (924b415) - Barry Walker
- (**fpga**) DMA 1-cycle dpram read latency — DMACOPY stale-data bug - (2f21297) - Barry Walker
- (**fpga**) SDRAM clkref must be 16:1, not 4:1 as Step 1 shipped - (f891dc7) - Barry Walker
- (**fpga**) cpu_ce-gate 3 more capture regs — same class as r_vgc_cpu_rdata - (c1e75da) - Barry Walker
- (**fpga+novahost**) timing + boot safety - (30c7872) - Barry Walker
- (**gfx**) relocate GRAPHICS MODBSS to $0420 off the loader band (4c.0b) - (f1ca5da) - Barry Walker
- (**gitignore**) explicit test_shelf_alloc binary; don't re-include .dSYM/.o via test_*.* exception - (397bf18) - Barry Walker
- (**graphics**) erase turtle before pen line so BOB restore can't gap lines (4c.2-3) - (3c4936b) - Barry Walker
- (**host**) module owns cursor motion; host only queues arrow codes - (7d3a29e) - Barry Walker
- (**libabi**) guard FIO register defs with .ifndef to compose with nova.inc - (97ff8a1) - Barry Walker
- (**logo**) restore split-screen after EDIT exit (turtle render moved to module) (4c.2-3) - (10e0ddd) - Barry Walker
- (**mcp**) enable reflection JSON so e6502 MCP server starts (fixes 'Connection closed') - (06f7b7e) - Barry Walker
- (**nova**) readline is buffer-based -- typed chars now VISIBLE after a program - (e4b0ab0) - Barry Walker
- (**nova**) screen-editor wedge -- C64 readline no longer ingests stale program output - (748b75f) - Barry Walker
- (**novachess**) always reload title splash on menu entry - (260ed1b) - Barry Walker
- (**novaforth**) move input_buf into the $9800 safe band (was overwritten by the dictionary) - (3bc0100) - Barry Walker
- (**novahost**) bounded debug-bridge writes to stop TCP wedge; HW harness TCP - (fdb57f8) - Barry Walker
- (**novahost**) install resident lib_call loader at $0320 + ext shelf at $07C000 on HW - (5d20de8) - Barry Walker
- (**novahost**) send_file allow probe writes + config-driven ROM loader - (473079f) - Barry Walker
- (**novahost**) replace hand-rolled JSON with ArduinoJson + fix MOUNT NULL parse - (04a195e) - Barry Walker
- (**novalogo**) repaint on every history up/down so the cursor never drifts - (d7cf5c7) - Barry Walker
- (**novalogo**) enable flashing cursor + clean up boot diagnostic - (e8e42cd) - Barry Walker
- (**novaz**) clear only the border frame on a region change, spare text area - (2948db2) - Barry Walker
- (**novaz**) clear rows excluded from scroll - (8dfcba7) - Barry Walker
- (**novaz**) finish Zork Zero V6 display path - (0fe8b35) - Barry Walker
- (**novaz**) opaque black art renders — gfx transparency off for V6 (M3) - (5c3fbb6) - Barry Walker
- (**novaz**) the gfx plane IS the MCGA framebuffer — alignment fixed (M3) - (d7b4ea0) - Barry Walker
- (**novaz**) three MCGA-fidelity fixes from the first GUI session (M3) - (debfa8e) - Barry Walker
- (**novaz**) draw_picture blanks covered text cells — art composites (M3) - (b1f2ed6) - Barry Walker
- (**novaz**) smoke key-gates confirm-by-waiting; MORE gated on the wait loop - (5349fda) - Barry Walker
- (**novaz**) zobject_remove unlink wrote the wrong object (v4+ scratch clobber) - (80589e1) - Barry Walker
- (**novaz**) V6 units are cells — match the advertised header metrics - (91188e5) - Barry Walker
- (**novaz**) symbol-resolve remaining stale smoke ZP diagnostics; doc fixes - (9d32832) - Barry Walker
- (**novaz**) smoke z-trace read stale ZP addresses; add probe + crash trace - (724a97e) - Barry Walker
- (**novaz**) V6 pull (VAR:9) — store form, user-stack operand, evaluated operand - (45ff9d5) - Barry Walker
- (**novaz**) V6 main-routine return quits cleanly; pin with stop-reason smoke - (1a925af) - Barry Walker
- (**novaz**) wait for boot soundfont preload to complete (FIO race) - (7d710c3) - Barry Walker
- (**page_dma**) force skid FIFO to FF array + test the no-drop margin - (7616ade) - Barry Walker
- (**runtime**) correct 5 verified 65C02 bugs from NDK/runtime audit - (aa40c9f) - Barry Walker
- (**sdram**) page-mode stream row boundary, port mutex, 14-bit word count - (f77a127) - Barry Walker
- (**sdram-model**) correct CAS read latency to exactly 3 + harden timing checks - (792e0d6) - Barry Walker
- (**system**) double-read $A200 window cells in readline (HW off-by-one) - (7a82e07) - Barry Walker
- (**tools**) handle unpitched notes and missing pitch elements in MusicXML - (a9af4f5) - Barry Walker
- (**tools**) fix tie chain bug and move _SEMI constant above first use - (4b4527e) - Barry Walker
- (**turtle**) render the turtle every frame during fast motion - (0af92a9) - Barry Walker
- (**turtle**) re-install icon after a paged module clobbers the shared BSS - (9cea313) - Barry Walker
- (**vgc**) kill the column-0 copper seam + force full-synth builds - (2b3d991) - Barry Walker
- (**vgc**) align copper bars on the left edge + add reusable VGC render sim - (e906620) - Barry Walker
- (**vgc**) sprite register decode + readback — HW SPRITE command lands - (fd17414) - Barry Walker
- (**vgc**) clamp FONT n>=3 to slot 0 (cp437) — no more blank-screen UX - (2b26887) - Barry Walker
- (**vgc**) 7-row scroll-offset mirror — widen real_row arithmetic - (00a6365) - Barry Walker
- (**vgc**) add $A008 frame counter (VGC_FRAME) — unblocks SPRITESET - (c698a40) - Barry Walker
- route BASIC FIOCLR through shared helper - (bcb3477) - Barry Walker
- stabilize NovaLogo hardware input and turtle draw - (af2df09) - Barry Walker
- align ULX3S HDMI active video - (b7ee648) - Barry Walker
- make XMC allocator release pages - (27136d1) - Barry Walker
- harden nova chess network waits - (01fe41d) - Barry Walker
- harden nova chess network startup - (5c061c1) - Barry Walker
- paginate ozmoo text output - (24bc894) - Barry Walker
- stabilize ozmoo hardware scrolling - (c248af0) - Barry Walker
- stabilize ozmoo xram loading - (c147ccd) - Barry Walker
- CMD_MEMREAD dpram-latency stale read + broader VGC test coverage - (b992f15) - Barry Walker
- three VGC RTL bugs found by new graphics/sprite/register test suite - (d6f73d9) - Barry Walker
- expose CPU debug state via proper module ports - (ea510bd) - Barry Walker
- FPGA debug peek for ROM + CPU register indexing - (393c3ca) - Barry Walker
- NovaHost JSON parser tolerates whitespace around colons - (3aae075) - Barry Walker
- debug bridge uses L1/N3 (ESP32 GPIO26/27) on v3.1.x board - (1d65669) - Barry Walker
- ULX3S v3.1.x pinout — wifi_en on J5, wifi_gpio0 on F1 - (01c5d93) - Barry Walker
- correct LAB_SNER breakpoint address in tokenization tests - (86e69ac) - Barry Walker
- HDMI output chain — LVCMOS33D differential, synchronizer disable, font pipeline - (659ff05) - Barry Walker
- fold cog check into build-and-test step - (2698464) - Barry Walker
- use failure:ignore for cog check step in CI - (59fdcd7) - Barry Walker
- complete PETSCII font set, fix mapping guide docs - (3fab20c) - Barry Walker
- correct cursor key codes, first-entry skip, and filename null-termination - (e9fe353) - Barry Walker
- keyboard visualizer flood fill bleeding from palette conflict - (f9eb87e) - Barry Walker
- system reset stops MIDI playback and WTS voices - (6f79acb) - Barry Walker
- OpenAL echo, WTS drum bank routing, and audio effect improvements - (03e599a) - Barry Walker
- shift DMA registers to $BA63 to avoid overlap with music note registers - (01bd0fb) - Barry Walker
- CmdMidPlay TCP command uses WTS auto routing when soundfont loaded - (722bce6) - Barry Walker
- Deactivate() no longer calls RestoreScreen unconditionally - (4b66760) - Barry Walker
- BasicEditor uses correct EhBASIC zero-page pointers + missing features - (9db35d8) - Barry Walker
- Ctrl+F/D key conflicts and editor mutual exclusion - (4e38cf1) - Barry Walker
- autoboot indirect JMP, MOUNT null separator, autoboot auto-run - (f7cf77b) - Barry Walker
- ResolveDevice savedDir leak, 64KB truncation guard, bare catch, BAM bounds - (dc8d6ba) - Barry Walker
- ext param handling, path traversal guard, tokenizer validation, directory type enum, REM lookup, multi-level subdirs - (9d731fd) - Barry Walker
- MusicStatus register reflects MIDI and SID playback state - (f06f629) - Barry Walker
- DirectNoteOn/Off/Slide now updates CurrentMidi for voice note readback - (ded95e0) - Barry Walker
- resolve ROM overflow by refactoring FIO error handling into shared subroutine - (80d3d47) - Barry Walker
#### Performance
- (**blitter**) replace row*stride multiplier with stride accumulator - (e45706e) - Barry Walker
- (**fpga**) two-level grouped cpu_din mux for timing headroom - (ddfba5a) - Barry Walker
#### Documentation
- (**arty**) boot-splash handoff + current WIP (usleep timing, SWDT disable/kick) - (b3da371) - Barry Walker
- (**arty**) MILESTONE -- interactive NovaBASIC (cursor+input) on Arty Z7-20 - (80f0bb7) - Barry Walker
- (**arty**) ARTY_NATIVE.md — native optimization opportunities + ABI-compat principle - (a9dd74d) - Barry Walker
- (**arty**) autoboot-skip via dbg_pause fails; READY blocker is lib_call dispatch - (0620bb1) - Barry Walker
- (**arty**) XRAM bring-up status — 6502 stalls on first axi_xram XMC access - (87fabdb) - Barry Walker
- (**arty**) PS smoke test CONFIRMED — hello.elf on /dev/ttyUSB1 (JP4=JTAG) - (853f513) - Barry Walker
- (**autoboot**) runtime-launch design — boot any /roms runtime - (24289bd) - Barry Walker
- (**backlog**) concrete watchdog-removal recipe for next .wic build - (a288846) - Barry Walker
- (**backlog**) Arty SWDT watchdog races PL-reload boot (cold-boot loop diagnosis) - (16a54c8) - Barry Walker
- (**backlog**) NovaLogo SQRT returns |n| not sqrt(n) (assessment finding) - (a6dbefe) - Barry Walker
- (**backlog**) Logo workspace save/load hardware-verified for variables + LOGO file type - (43565d0) - Barry Walker
- (**graphics**) annotate all 132 implemented GFNs (full self-documenting module) - (954e32d) - Barry Walker
- (**handoff**) NDK-sync state + open work (docs-from-NDK, turtle repatriation) - (1e81f22) - Barry Walker
- (**handoff**) nmod self-documenting modules + 4c.2 HW loader fix session handoff - (3fc00b9) - Barry Walker
- (**handoff**) Stage 4c.2 emulator-side complete; HW bring-up start-here - (ea4340f) - Barry Walker
- (**handoff**) Stage 4c.2 turtle-phase handoff for a fresh session - (ccb407c) - Barry Walker
- (**input**) board-input design — input.inc NDK + BASIC JOY/SW - (73b0e92) - Barry Walker
- (**lib**) library shelf contract + modtab module->slot map - (1654482) - Barry Walker
- (**nova-cli**) document the vm, module, runtime, and webserver verbs - (d7e4b03) - Barry Walker
- (**novapanel**) TDD implementation plan - (b957989) - Barry Walker
- (**novapanel**) design for Avalonia remote-control app - (bc81e65) - Barry Walker
- (**novaz**) handoff — Zork Zero display corruption + golden-image plan - (ee4c297) - Barry Walker
- (**novaz**) M3 shipped — README current state + design milestone (M3 Task 8) - (dd144ee) - Barry Walker
- (**novaz**) M3 plan working dir → novaz-z6-m3 worktree - (c8272ca) - Barry Walker
- (**novaz**) Z6 M3 implementation plan — pictures on Avalonia - (6b30310) - Barry Walker
- (**novaz**) V6 M2 status — real window rendering shipped - (d9f5a32) - Barry Walker
- (**novaz**) Z6 M2 implementation plan — capture-driven window rendering - (f352b46) - Barry Walker
- (**novaz**) Zork Zero window-op capture analysis for M2 - (b223562) - Barry Walker
- (**novaz**) V6 M1 status, test-z6-suite target, README updates - (20b4ab7) - Barry Walker
- (**novaz**) Z6 M1 implementation plan — TDD task breakdown - (1d7e8d4) - Barry Walker
- (**novaz**) Z-machine V6 design — graphical Infocom games on Nova - (34a8732) - Barry Walker
- (**plan**) C64 screen-editor implementation plan (SYS_SCREEN_READLINE) - (2cb5d4a) - Barry Walker
- (**plan**) NDK harvest audit + deferred BASIC screen-editor design - (5a231c6) - Barry Walker
- (**plan**) ADDR table relocation into SYSTEM module (SYS_ADDR_LOOKUP) - (d22d69d) - Barry Walker
- (**plan**) BASIC-on-modules handoff + ADDR-table relocation plan - (8e4a2bb) - Barry Walker
- (**plan**) BASIC-on-modules Phase 0 (infra + coexistence bridge) - (c4baac1) - Barry Walker
- (**plan**) BASIC-on-modules refactor design - (306cb28) - Barry Walker
- (**plan**) NDK-remainder-as-modules design — domain-grouped, modules-only - (654292e) - Barry Walker
- (**plan**) recalibrate dedup audit — estimates optimistic, +121B banked - (29ecccb) - Barry Walker
- (**plan**) 6502 codebase dedup/squeeze audit — turtle budget is solvable - (68caf19) - Barry Walker
- (**plan**) docs-from-NDK design — single-source module summaries from NDK - (5f7cc6c) - Barry Walker
- (**plan**) re-baseline Stage 4c around the runtime<->loader bridge - (2e23ced) - Barry Walker
- (**plan**) Stage 4c — resident turtle + wire Logo onto lib_call(GRAPHICS) - (8285cc9) - Barry Walker
- (**plan**) record 4a result (1071 B reclaimed); carry seating to 4c (4a) - (c500d07) - Barry Walker
- (**plan**) Phase 4 GRAPHICS module + Logo bank-0 slim design - (6f94ba5) - Barry Walker
- (**plan**) Phase 3 (3b) paged-library loader implementation plan - (7112924) - Barry Walker
- (**plan**) Phase 3 (3b) paged-library loader + module format design - (96fc2ab) - Barry Walker
- (**plan**) reconcile 11c wording with skid-buffer pacing (not per-byte toggle) - (8d74557) - Barry Walker
- (**plan**) revise page-in to Tier 2 (dual-clock ext_rom + engine back-pressure) - (50cffe6) - Barry Walker
- (**plan**) record measured read-stream throughput (8676 cyc = 86.76us @100MHz) - (6cfc516) - Barry Walker
- (**plan**) widen stream_words to 14 bits (16K page-in = 8192 words) - (c568a60) - Barry Walker
- (**plan**) XRAM streaming-engine + Tier-1 page-in implementation plan (Phase 2) - (d736309) - Barry Walker
- (**plan**) lock XRAM streaming-engine RTL design (extend sdram.v + sdr_ctrl pattern) - (fbdc2a7) - Barry Walker
- (**plan**) streaming FSM design confirmed against sdram.v + real timing - (1b2ae3d) - Barry Walker
- (**plan**) scope streaming as the general XRAM bulk path (DMA+blitter+XMC+page-in) - (8c1cb87) - Barry Walker
- (**plan**) lock page-in as page-mode streaming SDRAM engine (~80us) - (6cc586c) - Barry Walker
- (**plan**) lock module dispatch ABI (flat command id + command→module map) - (d0fd309) - Barry Walker
- (**plan**) paged runtime libraries + authoritative XRAM design - (8fa728b) - Barry Walker
- (**plans**) dynamic module shelf Phase B plan (runtime miss/eviction) - (b0dc210) - Barry Walker
- (**plans**) dynamic module shelf Phase A implementation plan - (bd09eac) - Barry Walker
- (**plans**) dynamic module shelf — XRAM paged-library cache design - (ca56ff1) - Barry Walker
- (**plans**) self-documenting modules (.nmod) + nova module management design - (2043004) - Barry Walker
- (**plans**) math coprocessor design — MMIO, ops, CORDIC, test plan - (9c8cec1) - Barry Walker
- (**plans**) session handoff — DMA regression, SID firmware pending, fileio - (0e571e9) - Barry Walker
- (**plans**) Phase 2.5 closure — Step 3/4 deferred - (7560180) - Barry Walker
- (**plans**) Phase 2.5 SDRAM integration plan - (2ce25f6) - Barry Walker
- (**plans**) VGC geometry rewrite — 640×480 native across the board - (0391b57) - Barry Walker
- add BACKLOG.md (NovaLogo property lists, print_list fix, SAVE/LOAD hw-verify) - (f7bc5db) - Barry Walker
- rewrite README for current Nova runtimes and structure - (5cbb0c8) - Barry Walker
- handoff for remaining NDK reusable-code extractions - (7095e08) - Barry Walker
- list NovaPanel + NovaHost projects in solution table - (5b7b9fc) - Barry Walker
- handoff for extracting the REPL line-reader+history into shared replline.s - (9e4e342) - Barry Walker
- handoff — page-in shipped, Phase 3 (loader/modules) next - (964ebd9) - Barry Walker
- add NovaLogo language design - (f118876) - Barry Walker
- refresh README for current NovaVM status - (f44b902) - Barry Walker
- define math coprocessor ABI - (069e0bf) - Barry Walker
- update generated mapping artifacts - (3ad7487) - Barry Walker
- add session handoff docs for 2026-04-26..29 - (e75284c) - Barry Walker
- explicit v3.1.x vs v2.x pinout comparison in LPF and BOARD.md - (4f7e185) - Barry Walker
- ULX3S v3.0.8 silkscreen/FTDI lies about pinout - (20df16f) - Barry Walker
- add demo disk player design and implementation plan - (b744ebd) - Barry Walker
- add Mapping the NovaVM book and update misc artifacts - (be7ada6) - Barry Walker
- unified voice routing implementation plan - (aab8598) - Barry Walker
- unified voice routing design - (94c4f02) - Barry Walker
- add design and implementation plans - (77c5598) - Barry Walker
- update memory map and architecture notes for WTS - (4fe45e5) - Barry Walker
- add MIDPLAY/MIDSTOP documentation - (2fae19c) - Barry Walker
- add MusicXML converter implementation plan - (212450f) - Barry Walker
- add MusicXML to MML converter design - (a1b82c4) - Barry Walker
#### Tests
- (**dma**) assert XMC page-used INCREMENT, not a stale absolute count - (20bcfb7) - Barry Walker
- (**emu**) pin LOADRUNTIME device→roms two-stage fallback; doc touch-ups - (9fc9201) - Barry Walker
- (**emu**) re-page-on-demand coexistence (4c.0c) - (c26c996) - Barry Walker
- (**emu**) loader-path tests on CompositeBusDevice (4c.0b) - (b105add) - Barry Walker
- (**fpga**) characterize board-input controller ($BA9C-$BAA1) - (3a7a3e3) - Barry Walker
- (**fpga**) page in formatted module + CPU dispatch into it (real RTL) - (20dd3e6) - Barry Walker
- (**fpga**) regression for cpu_ce-gated SID readback (c1e75da) - (503d355) - Barry Walker
- (**hw**) GRAPHICS module silicon smoke — PASS (4b.final) - (3b01205) - Barry Walker
- (**hw**) lib_call smoke uses A=USR(0) trigger (PASS on board) - (74bb8a4) - Barry Walker
- (**hw**) lib_call hardware smoke — stage module, run real loader, assert RESULT - (1f4654c) - Barry Walker
- (**integration**) finish POKE-sentinel rollout across all test files - (e597229) - Barry Walker
- (**integration**) strengthen vgc + sprites with POKE-sentinel assertions - (9a7f2f4) - Barry Walker
- (**lib**) extend band sentinels to $0300-$08FF (4c.2-1) - (83cab14) - Barry Walker
- (**lib**) strengthen lib_call coverage (BadVersion + bank-restore asserts) - (a7342be) - Barry Walker
- (**lib**) behavioral bus — ROMSWAP overlay + $BA76 page-in - (69ed8eb) - Barry Walker
- (**loader**) re-page test uses still-legacy TIMER, not rerouted SETXY (4c.2-3) - (a80ef12) - Barry Walker
- (**logo**) characterize 1-arg reporter under-supply behavior (R1/4a-task5) - (3176284) - Barry Walker
- (**logo**) graphics via lib_call + coexistence + sprites (4c.1-3) - (43ff0b7) - Barry Walker
- (**logo**) characterize exact text of every error family (4a) - (328960b) - Barry Walker
- (**novalogo**) boot integration tests + fix CR/LF in banner - (3ac63d8) - Barry Walker
- (**novapanel**) headless app boot smoke test - (705029f) - Barry Walker
- (**novaz**) z4-styles fixture holds on read_char instead of quitting - (7a840eb) - Barry Walker
- (**novaz**) V6 in-window scroll/wrap — vacated rows blanked, rect-clipped - (214d7e2) - Barry Walker
- (**novaz**) Z6 spec story generator + red test-z6-spec target - (2694bf1) - Barry Walker
- (**page_dma**) tie off sdram stream_ready=1 (missed site in 11a) - (250316e) - Barry Walker
- (**sdram**) fix stale comment in port-B case (>=20 words, not '40 cycles') - (b94bf4c) - Barry Walker
- (**sdram**) add case (f) port-B deferral during stream (N1 hazard probe) - (d81aed0) - Barry Walker
- (**sdram**) add case (e) refresh-obligation proof to stream bench - (b420c70) - Barry Walker
- (**sdram**) add case (d) 16 KB stream cycle-count measurement - (4b76689) - Barry Walker
- (**sdram**) gate stream cmd-counter on stream_busy (robust exact counts) - (9d32971) - Barry Walker
- (**sdram**) add case (c) aligned 4-row burst with command-sequence proof - (00d84a7) - Barry Walker
- (**sdram**) failing byte-exact stream readback case - (ba4ce6d) - Barry Walker
- (**sdram**) page-mode MT48LC16M16 behavioral model with timing assertions - (f471e4f) - Barry Walker
- (**shelf**) add StageShelfModule emulator helper (stage + seed directory) - (ddc2abb) - Barry Walker
- (**tools**) add backup/forward multi-voice test for xml2mml - (2c23327) - Barry Walker
- (**wts**) add end-to-end integration tests for wavetable synth - (3302ce3) - Barry Walker
- update 2 stale assertions to current intended behavior - (91872a7) - Barry Walker
- guard Arty V6 save/restore (FCREATE/FWRITE dispatch + NDI chunked write) - (729ce60) - Barry Walker
- NVG fixtures use uppercase .NVG so they resolve on case-sensitive hosts - (2bd5d50) - Barry Walker
- align 5 readline tests with the buffer-based reader - (d2e504a) - Barry Walker
- drop stale .include "*.s" assertions in RuntimeLibraryAbiTests - (44f6a46) - Barry Walker
- fix vgc scroll high-bit expectation - (3fc86ed) - Barry Walker
- add zork iii ozmoo smoke - (f12f207) - Barry Walker
- add sprite motion diagnostics - (ba112e3) - Barry Walker
- exhaustive TXTCLS + GCLS coverage — no bugs found - (1edd85e) - Barry Walker
- integration tests for NovaHost ESP32 firmware - (cba12cc) - Barry Walker
- add end-to-end integration test for filtered enumeration - (603b070) - Barry Walker
- add tests for review fixes (bounds, traversal, disk full, ext disambiguation, etc.) - (e583ba6) - Barry Walker
#### Build
- (**arty**) refresh ROM + forth/logo disk images with char-bg words - (3f0b120) - Barry Walker
- (**arty**) add make-wic.sh — reproducible SD image despite PetaLinux wic bug - (4769f4d) - Barry Walker
- (**arty**) reDIP-SID synthesizes OOC on Vivado (the original stub blocker) - (4995ebd) - Barry Walker
- (**libcall**) canonical resident loader binary at ORG $0320 (4c.0a) - (ae6db08) - Barry Walker
- (**novahost**) scaffold e6502.NovaHost shared library - (d8e9ca5) - Barry Walker
- build all deployable binaries from source; stop tracking them - (d91e96d) - Barry Walker
- make pandoc/latexmk (PDF user-guide) non-fatal so ROM+tests build without them - (0a7fa17) - Barry Walker
#### CI/CD
- (**deploy**) trigger SSA verification redeploy (WAL-124) - (9bc17eb) - Keel
- (**deploy**) drop namespace.yaml from CI kustomize; namespace is infra-bootstrapped (WAL-66) - (60afa0a) - Keel (WHI Infra CI)
- (**deploy**) re-trigger after namespace/RBAC bootstrap (WAL-66) - (89fcf6e) - Keel (WHI Infra CI)
- (**deploy**) trigger release-static-site after CF secrets provisioned (WAL-66) - (fdf4b6d) - Keel (WHI Infra CI)
- (**nova**) adopt build/release standard — cog bump + full RID matrix (#3) - (9a2e928) - Barry Walker
- (**rbac**) grant PDB access to CI deployer Role (WAL-107) - (2289487) - Barry Walker
- (**release**) mint App token for macOS release upload (WAL-70) (#13) - (de78050) - Barry Walker
- (**woodpecker**) fix WAL-136 cross-ref in build-novadraw comment (WAL-123) (#15) - (e351a71) - Barry Walker
- (**woodpecker**) mint App token for NovaDraw; retire dead windows leg (WAL-123) (#14) - (29cf6b6) - Barry Walker
- mint GitHub App token for release steps, drop shared PAT (WAL-72) - (908faad) - Keel CI
#### Refactoring
- (**basic**) relocate thin-MMIO handlers from ext ROM into main (Group A) - (d3e41a6) - Barry Walker
- (**basic**) remove HELP command from ROM (docs move to LaTeX books) - (656a784) - Barry Walker
- (**basic**) prune 6 low-value keywords from main ROM (tombstone mode) - (3cd3a85) - Barry Walker
- (**basic**) route blitter+dma through lib_call(GRAPHICS); drop blitter.s from extension ROM - (5720890) - Barry Walker
- (**basic**) relocate ADDR table to lib_call(SYSTEM); drop runtime_labels.inc from extension ROM - (b669a7d) - Barry Walker
- (**basic**) relocate COPPER to lib_call(GRAPHICS); drop copper.s from extension ROM - (6659551) - Barry Walker
- (**basic**) route file keywords through lib_call(FILES); drop fio.s from main ROM - (f3d1c4d) - Barry Walker
- (**basic**) route NET keywords through lib_call(NET); drop nic.s - (159130a) - Barry Walker
- (**basic**) route audio keywords through lib_call(SOUND); drop audio.s from main ROM - (ff4f930) - Barry Walker
- (**basic**) route sprite keywords through lib_call(GRAPHICS); drop sprite.s - (0005557) - Barry Walker
- (**basic**) route graphics/text keywords through lib_call; drop vgc.s + add coexistence bridge - (11f45e8) - Barry Walker
- (**ehbasic**) relocate LAB_COPPER to extension ROM via RAM bridges - (321adcf) - Barry Walker
- (**graphics**) squeeze turtle code — shared tails, mul_dist_trig, dedup helpers - (a759c7f) - Barry Walker
- (**logo**) delete turtle drivers from extension; render is module-resident now (4c.2-3-iii) - (b785c12) - Barry Walker
- (**logo**) 1-arg reporters use dispatcher arity (R1/4a-task5) - (d02b758) - Barry Walker
- (**logo**) share proc_next; tighten do_erase unlink (4a) - (ce0190e) - Barry Walker
- (**logo**) merge PRINT/TYPE via newline-flag fall-through (4a) - (acc059e) - Barry Walker
- (**logo**) fold eval.s arg/unknown printers into composer (4a) - (d0078ca) - Barry Walker
- (**logo**) compose procedures.s NEI errors via err_nei (4a) - (9ac0c74) - Barry Walker
- (**logo**) compose builtins.s NEI errors via err_nei (4a) - (35df75b) - Barry Walker
- (**logo**) message composer; gut lists.s error strings (4a) - (4b7a0e1) - Barry Walker
- (**logo**) drop dead arity-read in lookup_builtin (4a) - (dc33115) - Barry Walker
- (**modules**) strip the SETPOS list-parsing Logo-ism out of the TURTLE module - (908e85d) - Barry Walker
- (**modules**) make sound/system thin NDK wrappers + add module/NDK drift test - (12014ae) - Barry Walker
- (**novahost**) extract management client into shared library - (e50b0f6) - Barry Walker
- (**novalogo**) extract REPL line-reader + history into shared replline.s - (abfbe2a) - Barry Walker
- (**novalogo**) squeeze base ROM — STZ eval_type, do_lib_call, fill_cons_from_eval - (cb4a020) - Barry Walker
- (**novalogo**) reclaim 250 base-ROM bytes (dead code + print consolidation) - (4e34156) - Barry Walker
- (**runtime**) add reusable vtext_print_at NDK primitive - (dd8374a) - Barry Walker
- (**shelf**) enforce loader/directory boundary (.assert) + doc fixes - (6256789) - Barry Walker
- (**shelf**) guard id=$00 in modtab_lookup; drop dead consts; test cleanup - (d6e18f0) - Barry Walker
- (**vgc**) remove dead border_color — V_BORDER=0 already hides it - (9286368) - Barry Walker
- unify MIDI voice routing into MidiEngine.RouteVoices - (81f7220) - Barry Walker
- inline NccEditor/BasicEditor lookups in EmulatorCanvas - (e1b15d7) - Barry Walker
- EmulatorCanvas uses ScreenTextEditor base for routing - (c875165) - Barry Walker
- NccEditor inherits from ScreenTextEditor - (230ad39) - Barry Walker
- BasicEditor inherits from ScreenTextEditor - (916676e) - Barry Walker
- extract ISyntaxHighlighter interface - (77d2ff9) - Barry Walker
- FileIoController routes through DeviceManager - (12d62d5) - Barry Walker
- loop-based MUSIC PRIORITY comma parsing - (ab7c43e) - Barry Walker
- convert JSR+RTS tail calls to JMP (2 bytes) - (2d71358) - Barry Walker
- add LAB_SKIPX helper to replace keyword skip chains - (7a69da7) - Barry Walker
- factor XMEM parameter loading into shared subroutines - (e7a6f2f) - Barry Walker
- factor VGC XY parameter loading into shared subroutines - (3e0e947) - Barry Walker
- remove dead XMC API stubs from ROM (~74 bytes) - (32c7e45) - Barry Walker
#### Miscellaneous
- (**arty**) tidy OSD sim dir — add run.sh, gitignore build junk - (d271638) - Barry Walker
- (**arty**) drop unused server/dev cruft from the appliance image - (8c3ed77) - Barry Walker
- (**arty**) purge sim + floppy build artifacts (*.pb/*.out/*.o), keep source - (a6537be) - Barry Walker
- (**arty**) drop Vivado xsim build junk from sim/ (gitignore xsim.dir) - (59237bf) - Barry Walker
- (**arty**) remove screen-dump diagnostic auto-inject/periodic hacks - (4dde496) - Barry Walker
- (**arty**) revert define experiments; clean MORE_OPTIONS define - (bcb1ec0) - Barry Walker
- (**arty**) drop stray Vivado NA/ artifact; gitignore it - (1b138e7) - Barry Walker
- (**basic**) sync ROM mirrors after Group A relocation - (217b456) - Barry Walker
- (**basic**) sync ROM mirrors after HELP removal - (edefb14) - Barry Walker
- (**basic**) finalize screen editor — sync mirrors + docs - (7f8883f) - Barry Walker
- (**build**) sync module binaries + runtime labels after vsprite/novalogo changes - (56dfe59) - Barry Walker
- (**ci**) trigger config-service merged-config release path (WAL-162) - (e53a399) - Keel
- (**deploy**) add website PodDisruptionBudget (ADR 0007, WAL-108) (#10) - (5f3bfe5) - Barry Walker
- (**forth**) rebuild novaforth.ndi with all current sources - (184a9f1) - Barry Walker
- (**fpga**) Task 11d cleanup — sim/synth page-in parity, comment + test tie-offs - (3f0ece5) - Barry Walker
- (**repo**) untrack .claude/settings.local.json (local per-user config) - (039b7c1) - Barry Walker
- (**repo**) gitignore build artifacts + untrack generated files - (47fe62d) - Barry Walker
- (**rom**) rebuild BASIC ROM mirrors for $0320 mailbox carve - (7841ddd) - Barry Walker
- (**rom**) sync FPGA ROM mirror with post-COPPER ehbasic build - (efaa817) - Barry Walker
- (**shelf**) novavm FIO/shelf/PGD constants + rebuilt artifacts - (735c014) - Barry Walker
- (**system**) resync system.bin copies to one label table - (37b722a) - Barry Walker
- reorganize software tree - (150597d) - Barry Walker
- gitignore MemPalace per-project files (mempalace.yaml, entities.json) - (65c76e0) - Barry Walker
- sync regenerated emulator Resources + ABI label docs with source - (8bfc6da) - Barry Walker
- ignore .worktrees/ for project-local git worktrees - (180e71c) - Barry Walker
- gitignore copyrighted Infocom story files and collection images - (a196221) - Barry Walker
- checkpoint nova hardware and tooling - (42fdbb8) - Barry Walker
- allowlist Bash commands accumulated during hw-audit session - (4c575c5) - Barry Walker
- update FPGA build artifacts for dual SID, user guide, and local settings - (a43d590) - Barry Walker
- add test data, update tools cache, and user guide - (f4c80ae) - Barry Walker
- regenerate novavm.inc with 14 voice note registers - (4774a7a) - Barry Walker
- update ROM binary and build artifacts (Tasks 7-13) - (4a1c8df) - Barry Walker
- update ROM binary with refactored EhBASIC (462 bytes recovered) - (e50c611) - Barry Walker
- add DryWetMIDI NuGet package - (232b89f) - Barry Walker
#### Style
- (**logo**) document why turtle adapter inlines the VGC wait (review nit) - (5f245a4) - Barry Walker

- - -

Changelog generated by [cocogitto](https://github.com/cocogitto/cocogitto).