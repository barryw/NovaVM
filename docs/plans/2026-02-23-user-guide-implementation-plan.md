# User Guide Update Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Bring the NovaBASIC LaTeX User Guide up to date with DMA, Blitter, Networking, SID 2, and all missing BASIC commands.

**Architecture:** Sequential edits to existing `.tex` files plus two new chapter files. Each task is a self-contained commit. Build verification after each task.

**Tech Stack:** LaTeX (latexmk), existing NovaBASIC document class and custom environments (notebox, warningbox, tipbox, tryitbox, retrobox, \cmd{}).

---

### Task 1: Update NovaBASIC Language Definition in Preamble

**Files:**
- Modify: `docs/manual/novabasic_user_guide.tex:56-79` (lstdefinelanguage block)
- Modify: `docs/manual/novabasic_user_guide.tex:337` (add chapter includes)

**Step 1: Add missing keywords to language definition**

In `novabasic_user_guide.tex`, update the `morekeywords=[1]` block (statements) to add:

```
NOPEN, NCLOSE, NLISTEN, NACCEPT, NSEND, DMACOPY, DMAFILL, BLITCOPY, BLITFILL, RESET, CURSOR
```

Update the `morekeywords=[2]` block (functions) to add:

```
NRECV\$, NSTATUS, NREADY, NLEN, DMASTATUS, DMAERR, DMACOUNT, BLITSTATUS, BLITERR, BLITCOUNT
```

**Step 2: Add new chapter includes**

After line `\input{chapters/07-assembly-and-special-chips}` add:

```latex
\input{chapters/08-dma-and-blitter}
\input{chapters/09-networking}
```

**Step 3: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

Expected: Warnings about missing chapter files (08, 09) — that's OK for now. No fatal errors from the preamble changes.

**Step 4: Commit**

```bash
git add docs/manual/novabasic_user_guide.tex
git commit -m "docs: add new keywords and chapter includes to user guide preamble"
```

---

### Task 2: Remove Phantom VSC and Add Missing Controllers to Memory Map Appendix

**Files:**
- Modify: `docs/manual/chapters/appendix-memory-map.tex`

**Step 1: Fix the address space overview table**

Replace line 34 (`\$A100--\$A1FF & 256 B & Virtual Sound Controller (VSC) registers`) with:

```latex
\$A100--\$A13F &      64 B & Network Interface Controller (NIC) registers \\
```

Also add these rows after the Timer Controller row (line 39), before `\$BA50`:

```latex
\$BA60--\$BA7F &      16 B & DMA Controller registers \\
\$BA80--\$BA9F &      32 B & Blitter Controller registers \\
```

Update the Music Status row (line 40) from `\$BA50--\$BA53 & 4 B` to:

```latex
\$BA50--\$BA56 &       7 B & Music Status and Voice Note Readback (6 voices) \\
```

Add after the SID chip row (line 43):

```latex
\$D420--\$D43C &      29 B & SID chip 2 registers (inside ROM range; writes intercepted) \\
```

Update the notebox (lines 46-52) to replace VSC references:

```latex
\begin{notebox}
The address range \$A01F--\$A0FF and \$A140--\$A9FF are not claimed by any
coprocessor and fall through to the underlying flat RAM.
The range \$BAA0--\$BBFF is similarly unallocated RAM.
SID registers at \$D400--\$D43C occupy space within the ROM address range
but are intercepted on write by the SID chip emulators.
\end{notebox}
```

Update the source-of-truth comment at the top (lines 6-8) to replace `VirtualSoundController.cs` with:

```latex
%                  e6502.Avalonia/Hardware/VirtualDmaController.cs
%                  e6502.Avalonia/Hardware/VirtualBlitterController.cs
%                  e6502.Avalonia/Hardware/VirtualNetworkController.cs
```

Update the introductory text (line 18) to replace `(VGC, VSC, FIO, XMC)` with `(VGC, NIC, FIO, XMC, DMA, Blitter)`.

**Step 2: Remove the entire VSC Register Map section**

Delete lines 290-331 (the entire `\section{VSC Register Map}` including VSC Registers and VSC Command Codes subsections).

**Step 3: Update SID section for dual SID**

After the existing SID filter/volume table (after line 255), replace the notebox with:

```latex
\subsection*{Second SID Chip (\$D420--\$D43C)}

A second SID chip is mapped at \$D420--\$D43C with an identical register
layout offset by \$20 from the primary. A legacy mirror at \$D500--\$D51C
routes transparently to \$D420.

Voice mapping: voices 1--3 use SID~1 (\$D400), voices 4--6 use SID~2
(\$D420). The music engine addresses all six voices via
\cmd{MUSIC voice,"mml"} where \textit{voice} is 1--6.

\begin{notebox}
The BASIC commands \cmd{INSTRUMENT}, \cmd{SOUND}, and \cmd{MUSIC} manage
SID registers automatically. Direct writes to \$D400+ or \$D420+ are for
advanced use only and may conflict with the music engine.
\end{notebox}
```

**Step 4: Expand Timer Controller section**

Replace the minimal timer subsection (lines 267-270) with:

```latex
\subsection*{Timer Controller (\$BA40--\$BA4F)}

The timer controller provides periodic interrupt generation. It is used
internally by \cmd{SIDPLAY} but can also be configured from assembly for
custom timing.

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.14\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.18\textwidth} p{0.10\textwidth} p{0.44\textwidth}}
\toprule
\normalfont\textbf{Address} & \normalfont\textbf{Name} & \textbf{Access} & \textbf{Description} \\
\midrule
\endhead
\bottomrule
\endfoot
\$BA40 & TimerCtrl   & R/W & Control register: bit 0 = enable. \\
\$BA41 & TimerStatus & RO  & Status: bit 0 = IRQ pending (reading clears). \\
\$BA42 & TimerDivL   & R/W & Divisor low byte. \\
\$BA43 & TimerDivH   & R/W & Divisor high byte. \\
\end{longtable}

The timer fires an IRQ every \textit{divisor} video frames (at 60~Hz).
A divisor of 1 fires every frame; a divisor of 60 fires once per second.
Reading \texttt{TimerStatus} clears the pending IRQ flag.
```

**Step 5: Update Music Status for 6 voices**

Replace the Music Status table (lines 274-285) with:

```latex
\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.14\textwidth} p{0.10\textwidth} p{0.62\textwidth}}
\toprule
\normalfont\textbf{Address} & \textbf{Access} & \textbf{Description} \\
\midrule
\endhead
\bottomrule
\endfoot
\$BA50 & RO & Status flags: bit 0 = SFX playing, bit 1 = music playing. \\
\$BA51 & RO & Voice 1 current MIDI note (0 = silent). SID 1, voice 1. \\
\$BA52 & RO & Voice 2 current MIDI note (0 = silent). SID 1, voice 2. \\
\$BA53 & RO & Voice 3 current MIDI note (0 = silent). SID 1, voice 3. \\
\$BA54 & RO & Voice 4 current MIDI note (0 = silent). SID 2, voice 1. \\
\$BA55 & RO & Voice 5 current MIDI note (0 = silent). SID 2, voice 2. \\
\$BA56 & RO & Voice 6 current MIDI note (0 = silent). SID 2, voice 3. \\
\end{longtable}
```

**Step 6: Add DMA Controller Register Map section**

Insert after the Timer/Music section (before `\section{FIO Register Map}`):

```latex
% =============================================================================
\section{DMA Controller Register Map}
% =============================================================================

The DMA Controller occupies \$BA60--\$BA7F and provides bulk memory transfers
across six unified memory spaces. Writing \$01 to \texttt{DmaCmd} triggers
the configured operation; poll \texttt{DmaStatus} for completion.

\subsection*{DMA Registers}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.14\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.18\textwidth} p{0.10\textwidth} p{0.44\textwidth}}
\toprule
\normalfont\textbf{Address} & \normalfont\textbf{Name} & \textbf{Access} & \textbf{Description} \\
\midrule
\endhead
\bottomrule
\endfoot
\$BA60 & DmaCmd       & W   & Command byte; write \$01 to start transfer. \\
\$BA61 & DmaStatus    & RO  & Status: \$00=idle, \$01=busy, \$02=ok, \$03=error. \\
\$BA62 & DmaErrCode   & RO  & Error detail code (see Appendix~\ref{chap:limits}). \\
\$BA63 & DmaSrcSpace  & R/W & Source memory space (0--5). \\
\$BA64 & DmaDstSpace  & R/W & Destination memory space (0--5). \\
\$BA65 & DmaSrcL      & R/W & Source address low byte. \\
\$BA66 & DmaSrcM      & R/W & Source address mid byte. \\
\$BA67 & DmaSrcH      & R/W & Source address high byte. \\
\$BA68 & DmaDstL      & R/W & Destination address low byte. \\
\$BA69 & DmaDstM      & R/W & Destination address mid byte. \\
\$BA6A & DmaDstH      & R/W & Destination address high byte. \\
\$BA6B & DmaLenL      & R/W & Transfer length low byte. \\
\$BA6C & DmaLenM      & R/W & Transfer length mid byte. \\
\$BA6D & DmaLenH      & R/W & Transfer length high byte. \\
\$BA6E & DmaMode      & R/W & Mode flags: bit 0 = fill mode. \\
\$BA6F & DmaFillValue & R/W & Fill byte (used when bit 0 of DmaMode is set). \\
\$BA70 & DmaCountL    & RO  & Bytes transferred, low byte. \\
\$BA71 & DmaCountM    & RO  & Bytes transferred, mid byte. \\
\$BA72 & DmaCountH    & RO  & Bytes transferred, high byte. \\
\end{longtable}

\subsection*{DMA Memory Spaces}

\begin{center}
\begin{tabular}{>{\ttfamily}cl}
\toprule
\normalfont\textbf{ID} & \textbf{Space} \\
\midrule
0 & CPU RAM (64 KB) \\
1 & VGC Character RAM (2,000 bytes) \\
2 & VGC Color RAM (2,000 bytes) \\
3 & VGC Graphics Bitmap (64,000 bytes) \\
4 & VGC Sprite Shapes (2,048 bytes) \\
5 & Expansion RAM (uses current XBANK) \\
\bottomrule
\end{tabular}
\end{center}
```

**Step 7: Add Blitter Controller Register Map section**

Insert after the DMA section:

```latex
% =============================================================================
\section{Blitter Controller Register Map}
% =============================================================================

The Blitter Controller occupies \$BA80--\$BA9F and provides 2D rectangular
copy and fill operations with configurable row stride. It shares the same
six memory spaces as the DMA controller. Writing \$01 to \texttt{BltCmd}
triggers the configured operation.

\subsection*{Blitter Registers}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.14\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.18\textwidth} p{0.10\textwidth} p{0.44\textwidth}}
\toprule
\normalfont\textbf{Address} & \normalfont\textbf{Name} & \textbf{Access} & \textbf{Description} \\
\midrule
\endhead
\bottomrule
\endfoot
\$BA80 & BltCmd        & W   & Command byte; write \$01 to start blit. \\
\$BA81 & BltStatus     & RO  & Status: \$00=idle, \$01=busy, \$02=ok, \$03=error. \\
\$BA82 & BltErrCode    & RO  & Error detail code (see Appendix~\ref{chap:limits}). \\
\$BA83 & BltSrcSpace   & R/W & Source memory space (0--5). \\
\$BA84 & BltDstSpace   & R/W & Destination memory space (0--5). \\
\$BA85 & BltSrcL       & R/W & Source base address low byte. \\
\$BA86 & BltSrcM       & R/W & Source base address mid byte. \\
\$BA87 & BltSrcH       & R/W & Source base address high byte. \\
\$BA88 & BltDstL       & R/W & Destination base address low byte. \\
\$BA89 & BltDstM       & R/W & Destination base address mid byte. \\
\$BA8A & BltDstH       & R/W & Destination base address high byte. \\
\$BA8B & BltWidthL     & R/W & Rectangle width low byte. \\
\$BA8C & BltWidthH     & R/W & Rectangle width high byte. \\
\$BA8D & BltHeightL    & R/W & Rectangle height low byte. \\
\$BA8E & BltHeightH    & R/W & Rectangle height high byte. \\
\$BA8F & BltSrcStrideL & R/W & Source row stride low byte. \\
\$BA90 & BltSrcStrideH & R/W & Source row stride high byte. \\
\$BA91 & BltDstStrideL & R/W & Destination row stride low byte. \\
\$BA92 & BltDstStrideH & R/W & Destination row stride high byte. \\
\$BA93 & BltMode       & R/W & Mode flags: bit 0 = fill, bit 1 = color-key. \\
\$BA94 & BltFillValue  & R/W & Fill byte (when bit 0 of BltMode is set). \\
\$BA95 & BltColorKey   & R/W & Transparent color (when bit 1 of BltMode is set). \\
\$BA96 & BltCountL     & RO  & Bytes written, low byte. \\
\$BA97 & BltCountM     & RO  & Bytes written, mid byte. \\
\$BA98 & BltCountH     & RO  & Bytes written, high byte. \\
\end{longtable}

\begin{notebox}
When color-key mode is active (bit 1 of \texttt{BltMode}), source pixels
matching \texttt{BltColorKey} are skipped and the destination byte is left
unchanged. This enables transparent sprite and tile overlays.
\end{notebox}
```

**Step 8: Add NIC Register Map section**

Insert after the Blitter section (before `\section{FIO Register Map}`):

```latex
% =============================================================================
\section{NIC Register Map}
% =============================================================================

The Network Interface Controller occupies \$A100--\$A13F and provides
message-oriented TCP networking over four connection slots. Writing a
command byte to \texttt{NicCmd} triggers the operation on the slot
selected by \texttt{NicSlot}.

\subsection*{NIC Registers}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.14\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.22\textwidth} p{0.10\textwidth} p{0.40\textwidth}}
\toprule
\normalfont\textbf{Address} & \normalfont\textbf{Name} & \textbf{Access} & \textbf{Description} \\
\midrule
\endhead
\bottomrule
\endfoot
\$A100 & NicCmd         & W   & Command byte; writing triggers execution. \\
\$A101 & NicStatus      & RO  & Global status flags. \\
\$A102 & NicSlot        & R/W & Active slot ID (0--3). \\
\$A103 & NicIrqCtrl     & R/W & IRQ enable mask (one bit per slot). \\
\$A104 & NicIrqStatus   & RO  & IRQ pending flags (reading clears). \\
\$A108 & NicRemotePortL & R/W & Remote port low byte (for connect). \\
\$A109 & NicRemotePortH & R/W & Remote port high byte. \\
\$A10A & NicLocalPortL  & R/W & Local port low byte (for listen). \\
\$A10B & NicLocalPortH  & R/W & Local port high byte. \\
\$A110 & NicDmaAddrL    & R/W & DMA RAM address low byte. \\
\$A111 & NicDmaAddrH    & R/W & DMA RAM address high byte. \\
\$A112 & NicDmaLen      & R/W & DMA length (1--255; \$00 = 256). \\
\$A113 & NicMsgLen      & RO  & Last received message length. \\
\$A118 & NicSlotStatus0 & RO  & Slot 0 status flags. \\
\$A119 & NicSlotStatus1 & RO  & Slot 1 status flags. \\
\$A11A & NicSlotStatus2 & RO  & Slot 2 status flags. \\
\$A11B & NicSlotStatus3 & RO  & Slot 3 status flags. \\
\$A120--\$A13F & NicNameBuf & R/W & Hostname buffer (32 bytes ASCII). \\
\end{longtable}

\subsection*{NIC Command Codes}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.10\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.22\textwidth} p{0.54\textwidth}}
\toprule
\normalfont\textbf{Code} & \normalfont\textbf{Name} & \textbf{Behavior} \\
\midrule
\endhead
\bottomrule
\endfoot
\$01 & NicCmdConnect    & Connect slot to remote host (hostname in NicNameBuf, port in NicRemotePortL/H). \\
\$02 & NicCmdDisconnect & Close active slot connection. \\
\$03 & NicCmdSend       & Send message from CPU RAM (NicDmaAddrL/H, NicDmaLen). \\
\$04 & NicCmdRecv       & Receive next queued message into CPU RAM. \\
\$05 & NicCmdListen     & Start TCP server on local port (NicLocalPortL/H). \\
\$06 & NicCmdAccept     & Accept pending connection on listening slot. \\
\end{longtable}

\subsection*{NIC Slot Status Bits}

\begin{center}
\begin{tabular}{>{\ttfamily}cll}
\toprule
\normalfont\textbf{Bit} & \textbf{Name} & \textbf{Meaning} \\
\midrule
0 & Connected    & Slot has an active TCP connection. \\
1 & DataReady    & Receive queue has at least one message (or pending accept). \\
2 & SendReady    & Slot is ready to accept a send operation. \\
3 & Error        & Connection failed, I/O error, or queue overflow. \\
4 & RemoteClosed & Remote peer closed the connection. \\
\bottomrule
\end{tabular}
\end{center}

\subsection*{NIC Global Status Bits (\$A101)}

\begin{center}
\begin{tabular}{>{\ttfamily}cll}
\toprule
\normalfont\textbf{Bit} & \textbf{Name} & \textbf{Meaning} \\
\midrule
0 & Ready    & NIC is initialized and ready. \\
1 & AnyData  & At least one slot has queued data. \\
7 & AnyError & At least one slot has an error flag set. \\
\bottomrule
\end{tabular}
\end{center}
```

**Step 9: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

Expected: Warnings about missing chapter files (08, 09). No errors from appendix changes. The VSC section should be gone. DMA, Blitter, NIC, SID 2, and Timer sections should appear.

**Step 10: Commit**

```bash
git add docs/manual/chapters/appendix-memory-map.tex
git commit -m "docs: update memory map with DMA, blitter, NIC, SID2; remove phantom VSC"
```

---

### Task 3: Update Command Reference Appendix

**Files:**
- Modify: `docs/manual/chapters/appendix-command-reference.tex`

**Step 1: Update Sound and Music section for 6 voices**

In the Sound and Music subsection, update line 229 from `voice (1--3)` to `voice (1--6)`. Also update line 216-217 description to mention two SID chips:

```latex
Two SID chips provide 6 voices. The music engine adds a six-voice MML
sequencer with instrument presets, tempo, looping, and per-frame effects.
```

Update `MUSIC PRIORITY` to show `v1{[},v2{[},\ldots{}v6{]}{]}`.

Update `MNOTE(voice)` in the Music Functions section to say `voice (1--6)`.

**Step 2: Add DMA section**

After the Expansion Memory subsection (after line 292, before Interrupts), add:

```latex
% ---- DMA (Direct Memory Access) -----------------------------------------------
\subsection*{DMA (Direct Memory Access)}

The DMA controller transfers data between six unified memory spaces
without CPU intervention. Space IDs: 0=CPU RAM, 1=character RAM,
2=color RAM, 3=graphics bitmap, 4=sprite shapes, 5=expansion RAM.

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.46\textwidth} p{0.44\textwidth}}
\toprule
\normalfont\textbf{Syntax} & \textbf{Purpose} \\
\midrule
\endhead
\bottomrule
\endfoot
DMACOPY ss,sa,ds,da,len         & Copy \textit{len} bytes from space \textit{ss} address \textit{sa} to space \textit{ds} address \textit{da}. \\[3pt]
DMAFILL ds,da,len,val           & Fill \textit{len} bytes in space \textit{ds} at address \textit{da} with byte \textit{val}. \\
\end{longtable}
```

**Step 3: Add Blitter section**

After the DMA section, add:

```latex
% ---- Blitter (2D Rectangle Operations) ----------------------------------------
\subsection*{Blitter (2D Rectangle Operations)}

The blitter performs stride-aware rectangular copies and fills across the
same six memory spaces as DMA. Stride is the byte offset from the start
of one row to the next.

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.55\textwidth} p{0.35\textwidth}}
\toprule
\normalfont\textbf{Syntax} & \textbf{Purpose} \\
\midrule
\endhead
\bottomrule
\endfoot
BLITCOPY ss,sa,sstr,ds,da,dstr,w,h   & Copy rectangle: \textit{w} bytes wide, \textit{h} rows, with source stride \textit{sstr} and destination stride \textit{dstr}. \\[3pt]
BLITFILL ds,da,dstr,w,h,val          & Fill rectangle with byte \textit{val}. \\
\end{longtable}
```

**Step 4: Add Network section**

After the Blitter section, add:

```latex
% ---- Network -------------------------------------------------------------------
\subsection*{Network}

The NIC provides message-oriented TCP networking over four connection slots
(0--3). Messages are limited to 256 bytes each.

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.42\textwidth} p{0.48\textwidth}}
\toprule
\normalfont\textbf{Syntax} & \textbf{Purpose} \\
\midrule
\endhead
\bottomrule
\endfoot
NOPEN slot,"host",port   & Connect to a remote TCP server. \\[3pt]
NCLOSE slot              & Close a connection. \\[3pt]
NLISTEN slot,port        & Start listening for incoming connections. \\[3pt]
NACCEPT slot             & Accept a pending incoming connection. \\[3pt]
NSEND slot,"msg"         & Send a message string. \\
\end{longtable}
```

**Step 5: Add RESET to Miscellaneous section**

In the Miscellaneous subsection, add a row:

```latex
RESET                      & Perform a full hardware and CPU reset. \\
```

**Step 6: Add DMA/Blitter/Network function sections**

In the Function Reference section, add after the Music Functions subsection:

```latex
% ---- DMA and Blitter Functions -------------------------------------------------
\subsection*{DMA and Blitter Functions}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.38\textwidth} p{0.52\textwidth}}
\toprule
\normalfont\textbf{Function} & \textbf{Returns} \\
\midrule
\endhead
\bottomrule
\endfoot
DMASTATUS       & DMA status: 0=idle, 1=busy, 2=ok, 3=error. \\
DMAERR          & DMA error code from last operation. \\
DMACOUNT        & Number of bytes transferred in last DMA operation. \\
BLITSTATUS      & Blitter status: 0=idle, 1=busy, 2=ok, 3=error. \\
BLITERR         & Blitter error code from last operation. \\
BLITCOUNT       & Number of bytes written in last blit operation. \\
\end{longtable}

% ---- Network Functions ---------------------------------------------------------
\subsection*{Network Functions}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.38\textwidth} p{0.52\textwidth}}
\toprule
\normalfont\textbf{Function} & \textbf{Returns} \\
\midrule
\endhead
\bottomrule
\endfoot
NRECV\$(slot)   & Receive next queued message as a string (max 255 chars). \\
NSTATUS(slot)   & Slot status bitmask (see Appendix~\ref{chap:memmap}). \\
NREADY(slot)    & $-1$ if a message is ready, 0 otherwise. \\
NLEN            & Length of the last received message (bytes). \\
\end{longtable}
```

**Step 7: Update Extended Two-Byte Token Index**

Replace the retrobox content in the extended token section (lines 487-491) with:

```latex
\begin{retrobox}
\ttfamily\small
DIR, DEL, XMEM, XBANK, XPOKE, XPEEK(), STASH, FETCH, XFREE, XRESET, XALLOC,
XDIR, XDEL, XMAP, XUNMAP, GSAVE, GLOAD, SIDPLAY, SIDSTOP, MUSIC, PLAYING,
MNOTE(), COPPER, RESET, NOPEN, NCLOSE, NLISTEN, NACCEPT, NSEND, NRECV\$(),
NSTATUS(), NREADY(), NLEN, DMACOPY, DMAFILL, DMASTATUS(), DMAERR(),
DMACOUNT(), BLITCOPY, BLITFILL, BLITSTATUS(), BLITERR(), BLITCOUNT()
\end{retrobox}
```

**Step 8: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

Expected: No errors from appendix changes. New sections should appear in the command reference.

**Step 9: Commit**

```bash
git add docs/manual/chapters/appendix-command-reference.tex
git commit -m "docs: add DMA, blitter, and network commands to reference appendix"
```

---

### Task 4: Update Limits and Errors Appendix

**Files:**
- Modify: `docs/manual/chapters/appendix-limits-and-errors.tex`

**Step 1: Update source-of-truth comment**

Replace `VirtualSoundController.cs` in the header (line 7) with:

```latex
%                  e6502.Avalonia/Hardware/VirtualDmaController.cs
%                  e6502.Avalonia/Hardware/VirtualBlitterController.cs
%                  e6502.Avalonia/Hardware/VirtualNetworkController.cs
```

**Step 2: Update Sound and Music section for 6 voices**

In the Sound and Music Limits section, update `MUSIC voice` from `Voice must be 1--3` to `Voice must be 1--6`.

Update the copper events entry from `Maximum 1024 events per program` to `Maximum 256 events per list; 128 lists available`.

**Step 3: Add DMA Limits section**

After the XRAM Limits section, before Status and Error Code Quick Reference, add:

```latex
% =============================================================================
\section{DMA and Blitter Limits}
% =============================================================================

\begin{longtable}{p{0.32\textwidth} p{0.58\textwidth}}
\toprule
\textbf{Command / Feature} & \textbf{Limit and edge behavior} \\
\midrule
\endhead
\bottomrule
\endfoot
Memory space IDs &
  Valid range: 0--5 (CPU RAM, Char, Color, Gfx, Sprite, XRAM).
  An invalid space ID sets the error code to \cmd{BadSpace} (\$02). \\
24-bit addressing &
  All DMA and blitter addresses are 24-bit (low/mid/high byte).
  For XRAM (space 5), the current \cmd{XBANK} value is used as the
  high address byte automatically by the BASIC commands. \\
\cmd{DMACOPY}/\cmd{DMAFILL} &
  Length is 24-bit; zero-length transfers are rejected with \cmd{BadArgs} (\$04).
  Out-of-range addresses set \cmd{Range} (\$03).
  Writes to ROM space set \cmd{WriteProt} (\$05). \\
\cmd{BLITCOPY}/\cmd{BLITFILL} &
  Width and height are 16-bit (1--65535 each).
  Zero width or height is rejected with \cmd{BadArgs} (\$04).
  Stride is the byte offset between row starts; it may be larger than width. \\
Color-key mode (blitter) &
  When bit 1 of \texttt{BltMode} is set, source bytes matching
  \texttt{BltColorKey} are skipped.
  In fill mode, color-key is ignored. \\
Row buffer &
  When source and destination overlap in the same space, the blitter uses
  a temporary row buffer to prevent read-after-write corruption. \\
\cmd{DMASTATUS}/\cmd{BLITSTATUS} &
  Returns the raw status register value: 0=idle, 1=busy, 2=ok, 3=error.
  BASIC commands check status automatically and raise a function-call
  error if the operation fails. \\
\end{longtable}
```

**Step 4: Add Network Limits section**

After the DMA and Blitter Limits section, add:

```latex
% =============================================================================
\section{Network Limits}
% =============================================================================

\begin{longtable}{p{0.32\textwidth} p{0.58\textwidth}}
\toprule
\textbf{Feature} & \textbf{Limit and edge behavior} \\
\midrule
\endhead
\bottomrule
\endfoot
Connection slots &
  4 slots (0--3). Each slot is independent and may be a client or server. \\
Message size &
  Maximum 256 bytes per message. Messages are length-prefixed on the wire
  (1 byte: \$00 = 256, \$01--\$FF = 1--255). \\
Receive queue &
  Up to 16 messages per slot. If the queue overflows, new messages are
  dropped and the slot error flag is set. \\
Hostname length &
  Maximum 31 characters (32-byte buffer, null-terminated). \\
Port range &
  1--65535 (16-bit). \\
Connect timeout &
  10 seconds. If the remote host does not respond, the slot error flag
  is set. \\
\cmd{NRECV\$(slot)} &
  Returns a string of up to 255 characters. If no message is queued,
  returns an empty string. \\
\cmd{NSTATUS(slot)} &
  Returns the slot status byte. Bit 4 (RemoteClosed) indicates the remote
  peer has disconnected; the slot should be closed with \cmd{NCLOSE}. \\
\cmd{NREADY(slot)} &
  Returns $-1$ (true) if at least one message is in the receive queue,
  0 (false) otherwise. \\
\cmd{NLISTEN} binding &
  TCP server binds to 127.0.0.1 (localhost). External connections require
  the remote end to connect to the host machine's loopback address. \\
\end{longtable}
```

**Step 5: Add DMA/Blitter/NIC to Status and Error Code section**

After the XMC Codes subsection, add:

```latex
\subsection*{DMA Controller Codes}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.12\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.26\textwidth} p{0.50\textwidth}}
\toprule
\normalfont\textbf{Value} & \normalfont\textbf{Symbol} & \textbf{Meaning} \\
\midrule
\endhead
\bottomrule
\endfoot
\multicolumn{3}{l}{\normalfont\textit{Status codes (\$BA61)}} \\
\midrule
\$00 & DmaStatusIdle  & No operation in progress. \\
\$01 & DmaStatusBusy  & Transfer is executing. \\
\$02 & DmaStatusOk    & Last transfer succeeded. \\
\$03 & DmaStatusError & Last transfer failed; check error code. \\
\midrule
\multicolumn{3}{l}{\normalfont\textit{Error codes (\$BA62)}} \\
\midrule
\$00 & DmaErrNone      & No error. \\
\$01 & DmaErrBadCmd    & Invalid command byte. \\
\$02 & DmaErrBadSpace  & Invalid source or destination space ID. \\
\$03 & DmaErrRange     & Address out of bounds for the given space. \\
\$04 & DmaErrBadArgs   & Invalid arguments (e.g., length $\leq$ 0). \\
\$05 & DmaErrWriteProt & Destination is write-protected (ROM). \\
\end{longtable}

\subsection*{Blitter Controller Codes}

\begin{longtable}{>{\ttfamily\raggedright\arraybackslash}p{0.12\textwidth} >{\ttfamily\raggedright\arraybackslash}p{0.26\textwidth} p{0.50\textwidth}}
\toprule
\normalfont\textbf{Value} & \normalfont\textbf{Symbol} & \textbf{Meaning} \\
\midrule
\endhead
\bottomrule
\endfoot
\multicolumn{3}{l}{\normalfont\textit{Status codes (\$BA81)}} \\
\midrule
\$00 & BltStatusIdle  & No operation in progress. \\
\$01 & BltStatusBusy  & Blit is executing. \\
\$02 & BltStatusOk    & Last blit succeeded. \\
\$03 & BltStatusError & Last blit failed; check error code. \\
\midrule
\multicolumn{3}{l}{\normalfont\textit{Error codes (\$BA82)}} \\
\midrule
\$00 & BltErrNone      & No error. \\
\$01 & BltErrBadCmd    & Invalid command byte. \\
\$02 & BltErrBadSpace  & Invalid source or destination space ID. \\
\$03 & BltErrRange     & Rectangular region extends out of bounds. \\
\$04 & BltErrBadArgs   & Invalid arguments (width $\leq$ 0, height $\leq$ 0). \\
\$05 & BltErrWriteProt & Destination is write-protected (ROM). \\
\end{longtable}
```

**Step 6: Update the tip box at the end**

Modify the existing tipbox to include DMA and Blitter:

```latex
\begin{tipbox}
The BASIC runtime maps FIO, XMC, DMA, and Blitter error returns to one of
three user-visible messages: ``\texttt{File not found}'',
``\texttt{I/O Error}'', or a function-call error.
For low-level programs that \cmd{POKE} the controller registers directly,
use the tables above to interpret the raw status and error bytes.
\end{tipbox}
```

**Step 7: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

**Step 8: Commit**

```bash
git add docs/manual/chapters/appendix-limits-and-errors.tex
git commit -m "docs: add DMA, blitter, and network limits and error codes"
```

---

### Task 5: Update Chapter 5 (Sound and Music) for SID 2

**Files:**
- Modify: `docs/manual/chapters/05-sound-and-music.tex`

**Step 1: Find and update voice count references**

Search for "3 voices" or "three-voice" or "voice (1--3)" and update to reflect 6 voices across two SID chips. Key changes:

- Update the introductory text to mention two SID chips and 6 voices
- Update `MUSIC voice,"mml"` syntax description from `voice (1--3)` to `voice (1--6)`
- Update `MNOTE(voice)` from `voice (1--3)` to `voice (1--6)`
- Update `MUSIC PRIORITY` from `v1,v2,v3` to `v1...v6`

**Step 2: Add SID 2 section**

After the section on music engine architecture (or wherever the SID register discussion is), add a brief section:

```latex
\subsection*{The Second SID Chip}

NovaBASIC includes two SID chips for a total of six voices. The first SID
(\$D400) handles voices 1--3; the second SID (\$D420) handles voices 4--6.
A legacy mirror at \$D500 routes transparently to the second SID.

From BASIC, the second SID is fully transparent --- simply use voice
numbers 4--6 with the \cmd{MUSIC} command:

\begin{lstlisting}[style=basiclisting]
10 INSTRUMENT 0,64,2,8,12,6
20 MUSIC 1,"T140 I0 O4 C4 E4 G4 C5 2"
30 MUSIC 4,"T140 I0 O3 C2 G2"
40 MUSIC PLAY
\end{lstlisting}

Voice 4 plays on the second SID chip. All instrument definitions, tempo,
and loop settings apply equally to both chips.
```

**Step 3: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

**Step 4: Commit**

```bash
git add docs/manual/chapters/05-sound-and-music.tex
git commit -m "docs: add SID 2 coverage and update voice counts in sound chapter"
```

---

### Task 6: Update Chapter 7 (Assembly and Special Chips)

**Files:**
- Modify: `docs/manual/chapters/07-assembly-and-special-chips.tex`

**Step 1: Add DMA from Assembly section**

Add a new section for DMA controller assembly access:

```latex
\section{DMA Controller from Assembly}
\label{sec:asm-dma}

The DMA controller at \$BA60 transfers data between six unified memory
spaces. The pattern is: load parameters into registers, then write \$01
to the command register to start.

\begin{lstlisting}[style=basiclisting]
10 REM -- DMA copy 2000 bytes from CPU $6000 to char RAM --
20 POKE $BA63, 0   : REM source space = CPU RAM
30 POKE $BA64, 1   : REM dest space = Char RAM
40 POKE $BA65, 0   : POKE $BA66, $60 : POKE $BA67, 0
50 REM source addr = $6000 (low, mid, high)
60 POKE $BA68, 0   : POKE $BA69, 0  : POKE $BA6A, 0
70 REM dest addr = 0 (start of char RAM)
80 POKE $BA6B, $D0 : POKE $BA6C, $07 : POKE $BA6D, 0
90 REM length = 2000 ($07D0)
100 POKE $BA6E, 0  : REM mode = copy (not fill)
110 POKE $BA60, 1  : REM start!
120 IF PEEK($BA61) = 1 THEN 120 : REM poll until not busy
130 IF PEEK($BA61) <> 2 THEN PRINT "Error:"; PEEK($BA62)
```

For fill mode, set bit 0 of \texttt{DmaMode} (\$BA6E) and load the fill byte
into \texttt{DmaFillValue} (\$BA6F).
```

**Step 2: Add Blitter from Assembly section**

```latex
\section{Blitter from Assembly}
\label{sec:asm-blitter}

The blitter at \$BA80 performs 2D rectangular copies and fills with row
stride. Set up source and destination addresses, width, height, and stride,
then write \$01 to \texttt{BltCmd}.

\begin{lstlisting}[style=basiclisting]
10 REM -- Scroll color RAM up by 1 row using blitter --
20 REM Source: row 1 (offset 80), Dest: row 0 (offset 0)
30 REM Width: 80, Height: 24, Stride: 80
40 POKE $BA83, 2  : POKE $BA84, 2   : REM src/dst = color RAM
50 POKE $BA85, 80 : POKE $BA86, 0   : POKE $BA87, 0
60 REM source offset = 80 (row 1)
70 POKE $BA88, 0  : POKE $BA89, 0   : POKE $BA8A, 0
80 REM dest offset = 0 (row 0)
90 POKE $BA8B, 80 : POKE $BA8C, 0   : REM width = 80
100 POKE $BA8D, 24 : POKE $BA8E, 0  : REM height = 24
110 POKE $BA8F, 80 : POKE $BA90, 0  : REM src stride = 80
120 POKE $BA91, 80 : POKE $BA92, 0  : REM dst stride = 80
130 POKE $BA93, 0  : REM mode = copy
140 POKE $BA80, 1  : REM start!
150 IF PEEK($BA81) = 1 THEN 150
```

Color-key mode: set bit 1 of \texttt{BltMode} (\$BA93) and load the
transparent color into \texttt{BltColorKey} (\$BA95). Source pixels
matching the color key are skipped.
```

**Step 3: Add NIC from Assembly section**

```latex
\section{Network Controller from Assembly}
\label{sec:asm-nic}

The NIC at \$A100 provides TCP networking. To connect as a client from
register-level code:

\begin{lstlisting}[style=basiclisting]
10 REM -- Connect to 127.0.0.1 port 8080 --
20 POKE $A102, 0          : REM slot 0
30 REM Write hostname to name buffer ($A120+)
40 H$ = "127.0.0.1"
50 FOR I = 1 TO LEN(H$)
60   POKE $A11F + I, ASC(MID$(H$, I, 1))
70 NEXT I
80 POKE $A11F + LEN(H$) + 1, 0 : REM null terminate
90 POKE $A108, $90 : POKE $A109, $1F : REM port 8080
100 POKE $A100, 1  : REM connect command
110 REM Poll slot status for connected bit
120 IF (PEEK($A118) AND 1) = 0 THEN 120
```

To send data, copy the message into CPU RAM, then set \texttt{NicDmaAddrL/H}
and \texttt{NicDmaLen}, and write \$03 to \texttt{NicCmd}. To receive, write
\$04 and read \texttt{NicMsgLen} for the actual length.
```

**Step 4: Expand Timer section**

Find the existing timer content in the chapter and ensure it covers assembly-level usage. If sparse, add:

```latex
\section{Timer from Assembly}
\label{sec:asm-timer}

The timer at \$BA40 fires an IRQ every \textit{N} video frames.
To set up a 1-second timer (60 frames):

\begin{lstlisting}[style=basiclisting]
10 IRQ 1000
20 POKE $BA42, 60 : POKE $BA43, 0  : REM divisor = 60
30 POKE $BA40, 1                    : REM enable timer
40 GOTO 40
1000 PRINT "TICK ";
1010 RETIRQ
```

Reading \texttt{TimerStatus} (\$BA41) clears the pending IRQ flag. The
timer must be disabled with \cmd{POKE \$BA40, 0} before changing the divisor.
```

**Step 5: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

**Step 6: Commit**

```bash
git add docs/manual/chapters/07-assembly-and-special-chips.tex
git commit -m "docs: add DMA, blitter, NIC, and timer assembly sections"
```

---

### Task 7: Write Chapter 8 — DMA and Blitter

**Files:**
- Create: `docs/manual/chapters/08-dma-and-blitter.tex`

**Step 1: Write the complete chapter**

Create `docs/manual/chapters/08-dma-and-blitter.tex` with the full chapter content. The chapter should follow the established style (epigraph, sections, \cmd{} formatting, code listings, tables, tryitbox). Sections:

- **8.1 The DMA Controller** — What DMA is, unified memory spaces, DMACOPY syntax and examples, DMAFILL syntax and examples, status functions (DMASTATUS, DMAERR, DMACOUNT), practical example (fast screen clear)
- **8.2 The Blitter** — What a blitter is, BLITCOPY with stride explanation, BLITFILL, status functions (BLITSTATUS, BLITERR, BLITCOUNT), color-key transparency mode, stride diagram
- **8.3 Memory Spaces Reference** — Table of all 6 space IDs with sizes and notes
- **8.4 Try It Now** — Worked example combining DMA clear + blitter color bars (reference dma_blitter_demo.bas)

Style requirements:
- Start with `\chapter{DMA and Blitter}` and `\label{chap:dma-blitter}`
- Use `\epigraph` for chapter opening quote
- Use `\cmd{}` for inline commands
- Use `\begin{lstlisting}[style=basiclisting]` for code blocks
- Use `\begin{notebox}`, `\begin{tipbox}`, `\begin{warningbox}` as appropriate
- Use `\begin{tryitbox}` for the Try It Now exercise
- Use `\begin{longtable}` or `\begin{tabular}` for tables with `\toprule/\midrule/\bottomrule`

**Step 2: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

Expected: Chapter 8 appears in the PDF. No LaTeX errors.

**Step 3: Commit**

```bash
git add docs/manual/chapters/08-dma-and-blitter.tex
git commit -m "docs: add Chapter 8 — DMA and Blitter"
```

---

### Task 8: Write Chapter 9 — Networking

**Files:**
- Create: `docs/manual/chapters/09-networking.tex`

**Step 1: Write the complete chapter**

Create `docs/manual/chapters/09-networking.tex` with the full chapter content. Sections:

- **9.1 The Network Controller** — What the NIC provides, 4 slots, message-oriented TCP, automatic length-prefixed framing, 256-byte max messages
- **9.2 Connecting as a Client** — NOPEN, NSEND, NRECV$, NCLOSE, polling with NREADY and NSTATUS, NLEN, simple client example
- **9.3 Running a Server** — NLISTEN, NACCEPT, server loop pattern, echo server example
- **9.4 Status and Error Handling** — NSTATUS bitmask breakdown, detecting disconnection and remote close, slot lifecycle
- **9.5 Try It Now** — Full echo server example (reference echo_server.bas)

Same style requirements as Task 7.

**Step 2: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

Expected: Chapter 9 appears in the PDF. No LaTeX errors.

**Step 3: Commit**

```bash
git add docs/manual/chapters/09-networking.tex
git commit -m "docs: add Chapter 9 — Networking"
```

---

### Task 9: Review Chapter 4 Accuracy

**Files:**
- Modify: `docs/manual/chapters/04-graphics-and-sprites.tex` (if needed)

**Step 1: Check SPRITEX/SPRITEY status**

Read the current ROM source to verify whether SPRITEX(n)/SPRITEY(n) still return 0. If so, confirm the existing warningbox is accurate. If they now work, update the documentation.

**Step 2: Check SPRITESHAPE/SPRITECOLOR status**

Same check — verify they're still stubs. Update if changed.

**Step 3: Check copper documentation accuracy**

Verify the copper writable registers list (BGCOL, MODE, SCROLLX, SCROLLY) is still accurate. Verify 128 lists, 256 events per list.

**Step 4: Build and verify**

Run: `cd docs/manual && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex`

**Step 5: Commit (only if changes were made)**

```bash
git add docs/manual/chapters/04-graphics-and-sprites.tex
git commit -m "docs: review and update Chapter 4 accuracy"
```

---

### Task 10: Final Build and Verification

**Step 1: Clean build**

```bash
cd docs/manual && latexmk -C && latexmk -pdf -interaction=nonstopmode novabasic_user_guide.tex
```

Expected: Clean PDF with no errors. All new chapters and appendix updates present.

**Step 2: Verify table of contents**

Open the PDF and confirm:
- Chapters 1-9 all appear in the TOC
- Chapter 8 is "DMA and Blitter"
- Chapter 9 is "Networking"
- Appendices A, B, C all appear
- No phantom VSC entries remain

**Step 3: Verify cross-references**

Search for `\ref{chap:limits}` — should resolve correctly.
Check that all `\label` entries have matching content.

**Step 4: Commit the built PDF (if tracked)**

```bash
git add docs/manual/novabasic_user_guide.pdf
git commit -m "docs: rebuild user guide PDF with all updates"
```

Or if the PDF is not tracked in git, skip this step.
