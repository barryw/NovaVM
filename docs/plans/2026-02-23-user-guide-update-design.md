# User Guide Update Design

**Date:** 2026-02-23
**Status:** Approved
**Scope:** Bring NovaBASIC User Guide up to date with all features added since initial writing

## Problem

The LaTeX user guide covers features through expanded memory (XMC) thoroughly, but three major feature areas added recently are completely undocumented: DMA controller, Blitter controller, and Network Interface Controller. Additionally, the appendix memory map contains phantom documentation for a Virtual Sound Controller (VSC) that was never implemented — its address space ($A100-$A1FF) is now occupied by the NIC. The second SID chip is also undocumented.

## Gap Analysis

| Feature | Manual Status |
|---------|--------------|
| DMA Controller (DMACOPY, DMAFILL, etc.) | Not documented |
| Blitter Controller (BLITCOPY, BLITFILL, etc.) | Not documented |
| NIC Networking (NOPEN, NSEND, NRECV$, etc.) | Not documented |
| SID 2 ($D420, mirror $D500) | Not documented |
| VSC ($A100-$A1FF) | Documented but doesn't exist — must remove |
| Timer assembly-level docs | Minimal |
| 17+ BASIC commands | Missing from command reference appendix |
| RESET command | Undocumented |
| DMA/Blitter/NIC register maps | Missing from memory map appendix |
| DMA/Blitter/NIC error codes/limits | Missing from limits appendix |
| Assembly-level access for new controllers | Not documented |
| Simplified memory map overview | Doesn't exist |

## Chapter Structure (Updated)

1. Welcome *(no changes)*
2. First Session *(no changes)*
3. Language Fundamentals *(no changes)*
4. Graphics and Sprites *(accuracy review — SPRITEX/Y, SPRITESHAPE/SPRITECOLOR status)*
5. Sound and Music *(add SID 2 coverage)*
6. Expanded Memory *(no changes)*
7. Assembly and Special Chips *(add DMA/Blitter/NIC/Timer register sections)*
8. **DMA and Blitter** *(NEW)*
9. **Networking** *(NEW)*
- Appendix A: Command Reference *(add ~17 missing commands + RESET)*
- Appendix B: Limits and Errors *(add DMA/Blitter/NIC error codes and limits)*
- Appendix C: Memory Map *(remove VSC, add DMA/Blitter/NIC/SID2, add simplified overview)*

## New Chapter 8: DMA and Blitter

### 8.1 — The DMA Controller (~300 words)
- What DMA is and why it matters (moving data without CPU loops)
- Unified memory space concept: CPU RAM, VGC Char/Color/Gfx/Sprite, XRAM — all addressable as space IDs 0-5
- `DMACOPY src_space, src_addr, dst_space, dst_addr, length` — full syntax and examples
- `DMAFILL space, addr, length, value` — clearing screens, initializing buffers
- Status functions: `DMASTATUS()`, `DMAERR()`, `DMACOUNT()`
- Practical example: fast screen clear, copying tile data to character RAM

### 8.2 — The Blitter (~300 words)
- What a blitter is (2D-aware DMA with stride)
- `BLITCOPY src_space, src_addr, src_stride, dst_space, dst_addr, dst_stride, width, height`
- `BLITFILL space, addr, stride, width, height, value`
- Status functions: `BLITSTATUS()`, `BLITERR()`, `BLITCOUNT()`
- Color-key transparency mode (skip pixels matching a key color)
- Stride explained with a diagram: row pitch vs. region width

### 8.3 — Memory Spaces Reference (~150 words)
- Table of all 6 space IDs with address ranges and notes
- Which spaces are read-only vs read-write
- 24-bit addressing for XRAM

### 8.4 — Try It Now (~200 words)
- Worked example: scrolling color bars using blitter row copies or tile-painting demo
- Reference `dma_blitter_demo.bas`

## New Chapter 9: Networking

### 9.1 — The Network Controller (~200 words)
- What the NIC provides: message-oriented TCP over 4 connection slots
- Client vs server model overview
- Message framing is automatic (length-prefixed on wire, transparent to BASIC)
- Max message size: 256 bytes, up to 16 queued per slot

### 9.2 — Connecting as a Client (~250 words)
- `NOPEN slot, "hostname", port` — opening a connection
- `NSEND slot, "message"` — sending data
- `A$ = NRECV$(slot)` — receiving data
- `NCLOSE slot` — disconnecting
- Polling with `NREADY(slot)` and `NSTATUS(slot)`
- `NLEN` — length of last received message
- Simple example: connect, send, print reply

### 9.3 — Running a Server (~250 words)
- `NLISTEN slot, port` — bind and listen
- `NACCEPT slot` — accept incoming connection (polling pattern)
- Server loop pattern: listen → accept → recv → process → send → loop
- Example: minimal echo server

### 9.4 — Status and Error Handling (~200 words)
- `NSTATUS(slot)` bitmask breakdown: Connected, DataReady, SendReady, Error, RemoteClosed
- Detecting disconnection and remote close
- Slot lifecycle: idle → connecting → connected → closed

### 9.5 — Try It Now (~250 words)
- Full echo server example
- Reference `echo_server.bas`

## Existing Chapter Updates

### Chapter 5 — Sound and Music: SID 2 (~150 words)
- Second SID chip at $D420 (mirror at $D500)
- 6 total voices: 1-3 on SID 1, 4-6 on SID 2
- `MUSIC 4-6` target SID 2 voices
- Register layout identical, offset by $20
- MusicNote4-6 status registers at $BA53-$BA55

### Chapter 7 — Assembly and Special Chips (~400 words across additions)
- **DMA from Assembly:** Register map $BA60-$BA7F, setup + command + poll pattern
- **Blitter from Assembly:** Register map $BA80-$BA9F, same pattern, color-key via BltMode
- **NIC from Assembly:** Register map $A100-$A13F, hostname buffer, port/slot, commands, DMA address setup
- **Timer from Assembly:** Expand coverage — TimerCtrl, TimerDivL/H, TimerStatus (read-clears IRQ)

### Chapter 4 — Graphics and Sprites: Accuracy Review
- Confirm SPRITEX/SPRITEY status (still return 0 or fixed?)
- Confirm SPRITESHAPE/SPRITECOLOR status (still stubbed?)

## Appendix Updates

### Appendix A — Command Reference
- Add **Network** section: NOPEN, NCLOSE, NLISTEN, NACCEPT, NSEND, NRECV$(), NSTATUS(), NREADY(), NLEN
- Add **DMA** section: DMACOPY, DMAFILL, DMASTATUS(), DMAERR(), DMACOUNT()
- Add **Blitter** section: BLITCOPY, BLITFILL, BLITSTATUS(), BLITERR(), BLITCOUNT()
- Add RESET to Miscellaneous
- Update Extended Token Index with all tokens $17-$44

### Appendix B — Limits and Errors
- DMA: space IDs 0-5, 24-bit addresses, error codes (BadCmd, BadSpace, Range, BadArgs, WriteProt)
- Blitter: same spaces, width/height 1-65535, stride, color-key mode, same error codes
- NIC: 4 slots, 256-byte max message, 16 queue depth, 32-char hostname, port 1-65535, 10s timeout
- NIC status codes: slot bitmask (bits 0-4), global status bits

### Appendix C — Memory Map
- **Remove** entire VSC section (phantom — never implemented, address space is NIC)
- **Add** DMA register table ($BA60-$BA7F)
- **Add** Blitter register table ($BA80-$BA9F) including BltMode and BltColorKey
- **Verify** NIC register table ($A100-$A13F) completeness
- **Add** SID 2 at $D420-$D43C and $D500 legacy mirror
- **Add** simplified memory map overview at top of appendix: RAM / I/O / ROM visual with expanded I/O region

## Out of Scope

- No changes to front matter, foreword, chapters 1-3, chapter 6
- No MCP/TCP server documentation (developer-facing)
- No CLI project documentation
- No new demo programs (reference existing ones)
- No raster IRQ documentation (not fully implemented)
