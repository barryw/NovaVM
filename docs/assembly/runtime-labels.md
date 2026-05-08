# Runtime Labels

Generated from `@label` comments in the assembly runtime sources.

## AUDIO.ATTACK

Attack nibble for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_ATTACK`
- Address: `$B9A6`

## AUDIO.DECAY

Decay nibble for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_DECAY`
- Address: `$B9A7`

## AUDIO.DURATION

Duration in 60 Hz frames for AUDIO.SOUND.

- Kind: `u8`
- Symbol: `AUDIO_DURATION`
- Address: `$B9A5`

## AUDIO.INSTRUMENT

Instrument slot for AUDIO.SOUND.

- Kind: `u8`
- Symbol: `AUDIO_INSTRUMENT`
- Address: `$B9A6`

## AUDIO.INSTRUMENT_SET

Define an instrument envelope and waveform.

- Kind: `routine`
- Symbol: `audio_instrument`
- Address: `$EE0B`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.INST_ID`
- `AUDIO.WAVEFORM`
- `AUDIO.ATTACK`
- `AUDIO.DECAY`
- `AUDIO.SUSTAIN`
- `AUDIO.RELEASE`

## AUDIO.INST_ID

Instrument slot to define for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_INST_ID`
- Address: `$B9A4`

## AUDIO.MIDPLAY

Play the MIDI file named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `audio_midplay`
- Address: `$EE1E`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`

## AUDIO.MIDSTOP

Stop MIDI file playback.

- Kind: `routine`
- Symbol: `audio_midstop`
- Address: `$EE23`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_LOOP

Enable or disable music looping. AUDIO.NOTE is 0=off, nonzero=on.

- Kind: `routine`
- Symbol: `audio_music_loop`
- Address: `$EE45`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`

## AUDIO.MUSIC_NOTE

Return the current MIDI note for a music voice.

- Kind: `routine`
- Symbol: `audio_music_note`
- Address: `$EE63`
- ABI: `register`

Inputs:
- `X`: Voice number, 1..14

Outputs:
- `A`: Current MIDI note, or 0 if out of range or silent

## AUDIO.MUSIC_PLAY

Start queued music playback.

- Kind: `routine`
- Symbol: `audio_music_play`
- Address: `$EE32`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_PLAYING

Return nonzero if music is playing.

- Kind: `routine`
- Symbol: `audio_music_playing`
- Address: `$EE59`
- ABI: `none`

Outputs:
- `A`: 1 if playing, 0 if idle

## AUDIO.MUSIC_PRIORITY

Set voice-stealing priority from AUDIO.NOTE and following pseudo-register bytes.

- Kind: `routine`
- Symbol: `audio_music_priority`
- Address: `$EE4A`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`

## AUDIO.MUSIC_SEQUENCE

Queue an MML sequence for AUDIO.VOICE using AUDIO.STRL/H and AUDIO.STRLEN.

- Kind: `routine`
- Symbol: `audio_music_sequence`
- Address: `$EE2D`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.VOICE`
- `AUDIO.STRL`
- `AUDIO.STRH`
- `AUDIO.STRLEN`

## AUDIO.MUSIC_STOP

Stop music playback.

- Kind: `routine`
- Symbol: `audio_music_stop`
- Address: `$EE3B`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_TEMPO

Set music tempo from AUDIO.NOTE/AUDIO.DURATION as a 16-bit BPM value.

- Kind: `routine`
- Symbol: `audio_music_tempo`
- Address: `$EE40`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`
- `AUDIO.DURATION`

## AUDIO.NOTE

MIDI note for AUDIO.SOUND.

- Kind: `u8`
- Symbol: `AUDIO_NOTE`
- Address: `$B9A4`

## AUDIO.PLAY_SOUND

Play a one-shot note using A/X/Y register arguments.

- Kind: `routine`
- Symbol: `audio_play_sound`
- Address: `$EDEE`
- ABI: `register`

Inputs:
- `A`: MIDI note
- `X`: Duration in 60 Hz frames
- `Y`: Instrument slot

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.RELEASE

Release nibble for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_RELEASE`
- Address: `$B9A9`

## AUDIO.SET_VOLUME

Set master or per-voice volume using A/X register arguments.

- Kind: `routine`
- Symbol: `audio_set_volume`
- Address: `$EDF9`
- ABI: `register`

Inputs:
- `A`: Volume level
- `X`: Voice selector, 0=master

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.SFLOAD

Load the soundfont named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `audio_sfload`
- Address: `$EE28`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`

## AUDIO.SFX_PLAYING

Return nonzero if a sound effect is playing.

- Kind: `routine`
- Symbol: `audio_sfx_playing`
- Address: `$EE53`
- ABI: `none`

Outputs:
- `A`: 1 if playing, 0 if idle

## AUDIO.SIDPLAY

Play the SID file named by FIO.NAME/FIO.NAMELEN. AUDIO.NOTE is reused as the 1-based song number.

- Kind: `routine`
- Symbol: `audio_sidplay`
- Address: `$EE10`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`
- `AUDIO.NOTE`

## AUDIO.SIDSTOP

Stop SID file playback.

- Kind: `routine`
- Symbol: `audio_sidstop`
- Address: `$EE19`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.SOUND

Play a one-shot note from AUDIO.NOTE, AUDIO.DURATION, and AUDIO.INSTRUMENT.

- Kind: `routine`
- Symbol: `audio_sound`
- Address: `$EE01`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`
- `AUDIO.DURATION`
- `AUDIO.INSTRUMENT`

## AUDIO.STATUS

Return raw music status bits.

- Kind: `routine`
- Symbol: `audio_status`
- Address: `$EE4F`
- ABI: `none`

Outputs:
- `A`: Raw MUSIC_STATUS byte

## AUDIO.STRH

High byte of MML string pointer for AUDIO.MUSIC_SEQUENCE.

- Kind: `u8`
- Symbol: `AUDIO_STRH`
- Address: `$B9A7`

## AUDIO.STRL

Low byte of MML string pointer for AUDIO.MUSIC_SEQUENCE.

- Kind: `u8`
- Symbol: `AUDIO_STRL`
- Address: `$B9A6`

## AUDIO.STRLEN

MML string length for AUDIO.MUSIC_SEQUENCE.

- Kind: `u8`
- Symbol: `AUDIO_STRLEN`
- Address: `$B9A3`

## AUDIO.SUSTAIN

Sustain nibble for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_SUSTAIN`
- Address: `$B9A8`

## AUDIO.VOICE

Voice selector. Use 0 for master volume or 1..14 for music voices.

- Kind: `u8`
- Symbol: `AUDIO_VOICE`
- Address: `$B9A5`

## AUDIO.VOLUME

Set volume from AUDIO.VOLUME_LEVEL and AUDIO.VOICE.

- Kind: `routine`
- Symbol: `audio_volume`
- Address: `$EE06`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.VOLUME_LEVEL`
- `AUDIO.VOICE`

## AUDIO.VOLUME_LEVEL

Volume level for AUDIO.VOLUME.

- Kind: `u8`
- Symbol: `AUDIO_VOLUME`
- Address: `$B9A4`

## AUDIO.WAVEFORM

SID waveform/control byte for AUDIO.INSTRUMENT_SET.

- Kind: `u8`
- Symbol: `AUDIO_WAVEFORM`
- Address: `$B9A5`

## BLITTER.COPY

Start a rectangular blitter copy using the BLT controller registers.

- Kind: `routine`
- Symbol: `blitter_copy`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `BLT_SRCSPACE`
- `BLT_DSTSPACE`
- `BLT_SRCL`
- `BLT_SRCM`
- `BLT_SRCH`
- `BLT_DSTL`
- `BLT_DSTM`
- `BLT_DSTH`
- `BLT_WIDTHL`
- `BLT_WIDTHH`
- `BLT_HEIGHTL`
- `BLT_HEIGHTH`
- `BLT_SRCSTRL`
- `BLT_SRCSTRH`
- `BLT_DSTSTRL`
- `BLT_DSTSTRH`

## BLITTER.FILL

Start a rectangular blitter fill using destination, size, stride, and fill registers.

- Kind: `routine`
- Symbol: `blitter_fill`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `BLT_DSTSPACE`
- `BLT_DSTL`
- `BLT_DSTM`
- `BLT_DSTH`
- `BLT_WIDTHL`
- `BLT_WIDTHH`
- `BLT_HEIGHTL`
- `BLT_HEIGHTH`
- `BLT_DSTSTRL`
- `BLT_DSTSTRH`
- `BLT_FILLVALUE`

## BLITTER.START_COPY

Low-level blitter copy start. Call BLITTER.COPY for the public wrapper.

- Kind: `routine`
- Symbol: `blitter_start_copy`

Outputs:
- `A`: 0 on success, 1 on error.

## BLITTER.START_FILL

Low-level blitter fill start. Call BLITTER.FILL for the public wrapper.

- Kind: `routine`
- Symbol: `blitter_start_fill`

Outputs:
- `A`: 0 on success, 1 on error.

## BLITTER.WAIT

Wait for the blitter controller to leave BUSY and return shared status.

- Kind: `routine`
- Symbol: `blitter_wait`

Outputs:
- `A`: 0 on success, 1 on error.

## COPPER.ADD

Add a copper operation using VGC.P0..VGC.P5.

- Kind: `routine`
- Symbol: `copper_add`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`

## COPPER.CLEAR

Clear the active copper list.

- Kind: `routine`
- Symbol: `copper_clear`

## COPPER.COMMAND

Issue a raw copper VGC command.

- Kind: `routine`
- Symbol: `copper_command`

Inputs:
- `A`: Copper VGC command byte.

## COPPER.LIST

Select a copper list using VGC.P0.

- Kind: `routine`
- Symbol: `copper_list`

Requires:
- `VGC_P0`

## COPPER.LIST_END

Finish the current copper list.

- Kind: `routine`
- Symbol: `copper_list_end`

## COPPER.OFF

Disable copper execution.

- Kind: `routine`
- Symbol: `copper_off`

## COPPER.ON

Enable copper execution.

- Kind: `routine`
- Symbol: `copper_on`

## COPPER.SET_REG_INDEX

Set VGC.P3/P4 to a direct VGC register index.

- Kind: `routine`
- Symbol: `copper_set_reg_index`

Inputs:
- `A`: VGC register index.

## COPPER.SET_SPRITE_REG

Set VGC.P3/P4 to a sprite attribute register address.

- Kind: `routine`
- Symbol: `copper_set_sprite_reg`

Inputs:
- `X`: Sprite index.
- `A`: Sprite field offset.

## COPPER.USE

Activate a copper list using VGC.P0.

- Kind: `routine`
- Symbol: `copper_use`

Requires:
- `VGC_P0`

## DMA.COPY

Start a DMA copy using the DMA controller registers.

- Kind: `routine`
- Symbol: `dma_copy`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `DMA_SRCSPACE`
- `DMA_DSTSPACE`
- `DMA_SRCL`
- `DMA_SRCM`
- `DMA_SRCH`
- `DMA_DSTL`
- `DMA_DSTM`
- `DMA_DSTH`
- `DMA_LENL`
- `DMA_LENM`
- `DMA_LENH`

## DMA.FILL

Start a DMA fill using the destination and length registers.

- Kind: `routine`
- Symbol: `dma_fill`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `DMA_DSTSPACE`
- `DMA_DSTL`
- `DMA_DSTM`
- `DMA_DSTH`
- `DMA_LENL`
- `DMA_LENM`
- `DMA_LENH`
- `DMA_FILLVALUE`

## DMA.START_COPY

Low-level DMA copy start. Call DMA.COPY for the stable public wrapper.

- Kind: `routine`
- Symbol: `dma_start_copy`

Outputs:
- `A`: 0 on success, 1 on error.

## DMA.START_FILL

Low-level DMA fill start. Call DMA.FILL for the stable public wrapper.

- Kind: `routine`
- Symbol: `dma_start_fill`

Outputs:
- `A`: 0 on success, 1 on error.

## DMA.WAIT

Wait for the DMA controller to leave BUSY and return shared status.

- Kind: `routine`
- Symbol: `dma_wait`

Outputs:
- `A`: 0 on success, 1 on error.

## FIO.ARG_ADDRH

High byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.

- Kind: `zp`
- Symbol: `FIO_ARG_ADDRH`
- Address: `$22`

## FIO.ARG_ADDRL

Low byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.

- Kind: `zp`
- Symbol: `FIO_ARG_ADDRL`
- Address: `$20`

## FIO.ARG_ADDRM

Middle byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.

- Kind: `zp`
- Symbol: `FIO_ARG_ADDRM`
- Address: `$21`

## FIO.ARG_LENH

High byte of transfer length for FIO.PREPARE_XRAM_TRANSFER.

- Kind: `zp`
- Symbol: `FIO_ARG_LENH`
- Address: `$27`

## FIO.ARG_LENL

Low byte of transfer length for FIO.PREPARE_XRAM_TRANSFER.

- Kind: `zp`
- Symbol: `FIO_ARG_LENL`
- Address: `$26`

## FIO.ARG_NAMELEN

Filename length for FIO.COPY_NAME.

- Kind: `zp`
- Symbol: `FIO_ARG_NAMELEN`
- Address: `$28`

## FIO.ARG_NAMEPTR_H

High byte of filename pointer for FIO.COPY_NAME.

- Kind: `zp`
- Symbol: `FIO_ARG_NAMEPTR_H`
- Address: `$2A`

## FIO.ARG_NAMEPTR_L

Low byte of filename pointer for FIO.COPY_NAME.

- Kind: `zp`
- Symbol: `FIO_ARG_NAMEPTR_L`
- Address: `$29`

## FIO.CD

Change directory to FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_cd`
- Address: `$EDD0`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.CHECK

Convert the current FIO status register to the shared A=0/1 result.

- Kind: `routine`
- Symbol: `fio_check`
- Address: `$ED51`

Outputs:
- `A`: 0 on success, 1 on error.

## FIO.CLEAR_ERROR

Clear the host-visible FIO status and error latch.

- Kind: `routine`
- Symbol: `fio_clear_error`
- Address: `$ED71`

Outputs:
- `A`: 0 on success.

## FIO.CMD_LOADRUNTIME

Host command that streams a 16K file into the primary runtime ROM bank. Callers must execute from RAM while issuing it.

- Kind: `const`
- Symbol: `FIO_CMD_LOADRUNTIME`
- Address: `$28`

## FIO.CMD_RNG

Host command that returns 32 bits from NovaHost's hardware-backed RNG.

- Kind: `const`
- Symbol: `FIO_CMD_RNG`
- Address: `$2A`

## FIO.CMD_XPAGE

Host command that streams a file slice into XRAM using FIO_SRC/FIO_ENDL as a 24-bit file offset.

- Kind: `const`
- Symbol: `FIO_CMD_XPAGE`
- Address: `$29`

## FIO.COPY_NAME

Copy a pointer-based filename into FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_copy_name`
- Address: `$ED90`

Outputs:
- `A`: 0 on success, 1 on invalid name.

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`

## FIO.DELETE

Delete the file named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_delete`
- Address: `$EDC1`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.DIR_OPEN

Open a directory listing using optional FIO.NAME/FIO.NAMELEN filter.

- Kind: `routine`
- Symbol: `fio_dir_open`
- Address: `$EDB7`

## FIO.DIR_READ

Read the next directory entry into the FIO directory result registers.

- Kind: `routine`
- Symbol: `fio_dir_read`
- Address: `$EDBC`

## FIO.EXEC

Issue an FIO command, wait for completion, and return A=0/1 status.

- Kind: `routine`
- Symbol: `fio_exec`
- Address: `$ED5E`

Inputs:
- `A`: FIO command byte.

Outputs:
- `A`: 0 on success, 1 on error.

## FIO.GLOAD

Load VGC/graphics memory using FIO graphics registers.

- Kind: `routine`
- Symbol: `fio_gload`
- Address: `$EDCB`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `FIO_GSPACE`
- `FIO_GADDRL`
- `FIO_GADDRH`
- `FIO_GLENL`
- `FIO_GLENH`

## FIO.GSAVE

Save VGC/graphics memory using FIO graphics registers.

- Kind: `routine`
- Symbol: `fio_gsave`
- Address: `$EDC6`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `FIO_GSPACE`
- `FIO_GADDRL`
- `FIO_GADDRH`
- `FIO_GLENL`
- `FIO_GLENH`

## FIO.ISSUE

Issue a raw FIO command without waiting for completion.

- Kind: `routine`
- Symbol: `fio_issue`
- Address: `$ED4D`

Inputs:
- `A`: FIO command byte.

## FIO.LOAD

Load a BASIC or binary file using FIO.NAME and FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_load`
- Address: `$EDB2`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `FIO_SRCL`
- `FIO_SRCH`

## FIO.LOAD_RUNTIME

Load a 16K runtime ROM image named by FIO.NAME/FIO.NAMELEN into the primary runtime ROM bank.

- Kind: `routine`
- Symbol: `fio_load_runtime`
- Address: `$EDE4`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.MKDIR

Create the directory named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_mkdir`
- Address: `$EDD5`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.NAME

Filename buffer used by file-backed runtime routines.

- Kind: `buffer`
- Symbol: `FIO_NAME`
- Address: `$B9B0`

## FIO.NAMELEN

Length of the filename in FIO.NAME, 1..63 bytes.

- Kind: `u8`
- Symbol: `FIO_NAMELEN`
- Address: `$B9A3`

## FIO.PREPARE_XRAM_TRANSFER

Copy pointer filename and XRAM transfer arguments into FIO registers.

- Kind: `routine`
- Symbol: `fio_prepare_xram_transfer`

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`
- `FIO_ARG_ADDRL`
- `FIO_ARG_ADDRM`
- `FIO_ARG_ADDRH`
- `FIO_ARG_LENL`
- `FIO_ARG_LENH`

## FIO.PWD

Read the current directory into FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_pwd`
- Address: `$EDDF`

## FIO.RMDIR

Remove the directory named by FIO.NAME/FIO_NAMELEN.

- Kind: `routine`
- Symbol: `fio_rmdir`
- Address: `$EDDA`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.RNG

Issue FIO.CMD_RNG and return random bytes in FIO_RNG0..3.

- Kind: `routine`
- Symbol: `fio_rng`
- Address: `$EDE9`

Outputs:
- `A`: 0 on success, 1 on error.
- `FIO_RNG0`: FIO_RNG1 FIO_RNG2 FIO_RNG3

## FIO.RNG0

Low byte returned by FIO.CMD_RNG.

- Kind: `u8`
- Symbol: `FIO_RNG0`
- Address: `$B9A4`

## FIO.RNG1

Second byte returned by FIO.CMD_RNG.

- Kind: `u8`
- Symbol: `FIO_RNG1`
- Address: `$B9A5`

## FIO.RNG2

Third byte returned by FIO.CMD_RNG.

- Kind: `u8`
- Symbol: `FIO_RNG2`
- Address: `$B9A6`

## FIO.RNG3

High byte returned by FIO.CMD_RNG.

- Kind: `u8`
- Symbol: `FIO_RNG3`
- Address: `$B9A7`

## FIO.RUN

Compatibility alias for FIO.EXEC.

- Kind: `routine`
- Symbol: `fio_run`

## FIO.SAVE

Save RAM using FIO.NAME plus FIO_SRCL/H and FIO_ENDL/H.

- Kind: `routine`
- Symbol: `fio_save`
- Address: `$EDAD`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `FIO_SRCL`
- `FIO_SRCH`
- `FIO_ENDL`
- `FIO_ENDH`

## FIO.SET_IO_ERROR

Set FIO_STATUS/FIO_ERRCODE to the shared I/O error state.

- Kind: `routine`
- Symbol: `fio_set_io_error`
- Address: `$ED83`

Outputs:
- `A`: 1 on error.

## NIC.ACCEPT

Accept a pending connection for NIC_SLOT.

- Kind: `routine`
- Symbol: `nic_accept`

## NIC.ARG_NAMELEN

Hostname length for NIC.COPY_NAME.

- Kind: `zp`
- Symbol: `NIC_ARG_NAMELEN`
- Address: `$20`

## NIC.ARG_NAMEPTR_H

High byte of hostname pointer for NIC.COPY_NAME.

- Kind: `zp`
- Symbol: `NIC_ARG_NAMEPTR_H`
- Address: `$22`

## NIC.ARG_NAMEPTR_L

Low byte of hostname pointer for NIC.COPY_NAME.

- Kind: `zp`
- Symbol: `NIC_ARG_NAMEPTR_L`
- Address: `$21`

## NIC.COMMAND

Issue a raw NIC command.

- Kind: `routine`
- Symbol: `nic_command`
- Address: `$EE6F`

Inputs:
- `A`: NIC command byte.

## NIC.CONNECT

Connect NIC_SLOT to NIC_NAMEBUF:NIC_RPORTL/H.

- Kind: `routine`
- Symbol: `nic_connect`
- Address: `$EE73`

Requires:
- `NIC_SLOT`
- `NIC_NAMEBUF`
- `NIC_RPORTL`
- `NIC_RPORTH`

## NIC.COPY_NAME

Copy a pointer-based host name to NIC_NAMEBUF and append NUL.

- Kind: `routine`
- Symbol: `nic_copy_name`
- Address: `$EE99`

Outputs:
- `A`: 0 on success, 1 on invalid length.

Requires:
- `NIC_ARG_NAMELEN`
- `NIC_ARG_NAMEPTR_L`
- `NIC_ARG_NAMEPTR_H`

## NIC.DISCONNECT

Disconnect NIC_SLOT.

- Kind: `routine`
- Symbol: `nic_disconnect`
- Address: `$EE77`

Requires:
- `NIC_SLOT`

## NIC.LENGTH

Read the most recent NIC message length.

- Kind: `routine`
- Symbol: `nic_length`
- Address: `$EE95`

Outputs:
- `A`: Message length byte.

## NIC.LISTEN

Listen on NIC_SLOT using NIC_LPORTL/H.

- Kind: `routine`
- Symbol: `nic_listen`

## NIC.READY

Return 1 when NIC_SLOT has data/accept readiness, otherwise 0.

- Kind: `routine`
- Symbol: `nic_ready`
- Address: `$EE8B`

Inputs:
- `X`: Slot number.

Outputs:
- `A`: 1 when ready, 0 otherwise.

## NIC.RECV

Receive data for NIC_SLOT into NIC_DMAL/H.

- Kind: `routine`
- Symbol: `nic_recv`
- Address: `$EE7F`

Requires:
- `NIC_SLOT`
- `NIC_DMAL`
- `NIC_DMAH`

## NIC.SEND

Send NIC_DMALEN bytes from NIC_DMAL/H on NIC_SLOT.

- Kind: `routine`
- Symbol: `nic_send`
- Address: `$EE7B`

Requires:
- `NIC_SLOT`
- `NIC_DMAL`
- `NIC_DMAH`
- `NIC_DMALEN`

## NIC.STATUS

Read clamped NIC slot status.

- Kind: `routine`
- Symbol: `nic_status`
- Address: `$EE83`

Inputs:
- `X`: Slot number.

Outputs:
- `A`: Slot status byte.

## PAGER.ADDRH

High byte of destination address for 24-bit targets.

- Kind: `zp`
- Symbol: `PAGER_ADDRH`
- Address: `$22`

## PAGER.ADDRL

Low byte of destination address.

- Kind: `zp`
- Symbol: `PAGER_ADDRL`
- Address: `$20`

## PAGER.ADDRM

Middle byte of destination address or VGC space selector.

- Kind: `zp`
- Symbol: `PAGER_ADDRM`
- Address: `$21`

## PAGER.FILEH

High byte of the source file offset.

- Kind: `zp`
- Symbol: `PAGER_FILEH`
- Address: `$2D`

## PAGER.FILEL

Low byte of the source file offset.

- Kind: `zp`
- Symbol: `PAGER_FILEL`
- Address: `$2B`

## PAGER.FILEM

Middle byte of the source file offset.

- Kind: `zp`
- Symbol: `PAGER_FILEM`
- Address: `$2C`

## PAGER.LENH

High byte of transfer length.

- Kind: `zp`
- Symbol: `PAGER_LENH`
- Address: `$27`

## PAGER.LENL

Low byte of transfer length.

- Kind: `zp`
- Symbol: `PAGER_LENL`
- Address: `$26`

## PAGER.LOAD_FILE_PAGE

Load PAGER_LEN bytes from PAGER_FILE offset in PAGER_NAME into PAGER_TARGET/PAGER_ADDR.

- Kind: `routine`
- Symbol: `pager_load_file_page`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `PAGER_FILEL`
- `PAGER_FILEM`
- `PAGER_FILEH`
- `PAGER_TARGET`
- `PAGER_ADDRL`
- `PAGER_ADDRM`
- `PAGER_ADDRH`
- `PAGER_LENL`
- `PAGER_LENH`
- `PAGER_NAMELEN`
- `PAGER_NAMEPTR_L`
- `PAGER_NAMEPTR_H`

## PAGER.NAMELEN

Filename length for the paged file.

- Kind: `zp`
- Symbol: `PAGER_NAMELEN`
- Address: `$28`

## PAGER.NAMEPTR_H

High byte of paged filename pointer.

- Kind: `zp`
- Symbol: `PAGER_NAMEPTR_H`
- Address: `$2A`

## PAGER.NAMEPTR_L

Low byte of paged filename pointer.

- Kind: `zp`
- Symbol: `PAGER_NAMEPTR_L`
- Address: `$29`

## PAGER.TARGET

Destination target selector: 0=XRAM, 1=CPU RAM, 2=VGC memory.

- Kind: `zp`
- Symbol: `PAGER_TARGET`
- Address: `$2E`

## PAGER.TARGET_RAM

PAGER.TARGET value for CPU RAM destinations.

- Kind: `const`
- Symbol: `PAGER_TARGET_RAM`
- Address: `$01`

## PAGER.TARGET_VGC

PAGER.TARGET value for VGC memory destinations.

- Kind: `const`
- Symbol: `PAGER_TARGET_VGC`
- Address: `$02`

## PAGER.TARGET_XRAM

PAGER.TARGET value for flat XRAM destinations.

- Kind: `const`
- Symbol: `PAGER_TARGET_XRAM`
- Address: `$00`

## RNG.GET16

Fill RNG.VALUE0..1 with 16 random bits from the Nova host.

- Kind: `routine`
- Symbol: `rng_get16`

Outputs:
- `A`: 0 on success, 1 on error.
- `RNG_VALUE0`: RNG_VALUE1

## RNG.GET32

Fill RNG.VALUE0..3 with 32 random bits from the Nova host.

- Kind: `routine`
- Symbol: `rng_get32`

Outputs:
- `A`: 0 on success, 1 on error.
- `RNG_VALUE0`: RNG_VALUE1 RNG_VALUE2 RNG_VALUE3

## RNG.GET8

Fill RNG.VALUE0 with an 8-bit random value from the Nova host.

- Kind: `routine`
- Symbol: `rng_get8`

Outputs:
- `A`: 0 on success, 1 on error.
- `RNG_VALUE0`: 

## RNG.VALUE0

Low byte of the most recent RNG.GET32 result.

- Kind: `u8`
- Symbol: `RNG_VALUE0`
- Address: `$B9A4`

## RNG.VALUE1

Second byte of the most recent RNG.GET32 result.

- Kind: `u8`
- Symbol: `RNG_VALUE1`
- Address: `$B9A5`

## RNG.VALUE2

Third byte of the most recent RNG.GET32 result.

- Kind: `u8`
- Symbol: `RNG_VALUE2`
- Address: `$B9A6`

## RNG.VALUE3

High byte of the most recent RNG.GET32 result.

- Kind: `u8`
- Symbol: `RNG_VALUE3`
- Address: `$B9A7`

## ROMSWAP.PRIMARY

Select the primary runtime ROM bank at $C000. At cold boot this is NovaBASIC; a RAM launcher may replace it with another runtime first.

- Kind: `const`
- Symbol: `ROMSWAP_PRIMARY`
- Address: `$02`

## SPRITE.BACKGROUND_STATUS

Read the sprite-background collision status register.

- Kind: `routine`
- Symbol: `sprite_background_collision_status`
- Address: `$E9E8`

Outputs:
- `A`: Background collision status byte.

## SPRITE.COLLISION_STATUS

Read the sprite-sprite collision status register.

- Kind: `routine`
- Symbol: `sprite_collision_status`
- Address: `$E9E4`

Outputs:
- `A`: Collision status byte.

## SPRITE.COMMAND

Issue a raw sprite VGC command without waiting for completion.

- Kind: `routine`
- Symbol: `sprite_command`
- Address: `$E98A`

Inputs:
- `A`: VGC sprite command byte.

## SPRITE.DISABLE

Disable the sprite in VGC.P0.

- Kind: `routine`
- Symbol: `sprite_disable`
- Address: `$E998`

Requires:
- `VGC_P0`

## SPRITE.ENABLE

Enable the sprite in VGC.P0.

- Kind: `routine`
- Symbol: `sprite_enable`
- Address: `$E993`

Requires:
- `VGC_P0`

## SPRITE.GET_X

Read a sprite's 16-bit X coordinate.

- Kind: `routine`
- Symbol: `sprite_get_x`
- Address: `$E9C9`

Inputs:
- `X`: Sprite index.

Outputs:
- `A`: X high byte.
- `Y`: X low byte.

## SPRITE.GET_Y

Read a sprite's Y coordinate.

- Kind: `routine`
- Symbol: `sprite_get_y`
- Address: `$E9DA`

Inputs:
- `X`: Sprite index.

Outputs:
- `Y`: Y coordinate byte.

## SPRITE.POS

Move a sprite using VGC.P0=sprite, VGC.P1/P2=x, VGC.P3=y.

- Kind: `routine`
- Symbol: `sprite_pos`
- Address: `$E98E`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`

## SPRITE.REG_OFFSET

Compute the VGC sprite attribute block offset for a sprite field.

- Kind: `routine`
- Symbol: `sprite_reg_offset`
- Address: `$E9A2`

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.

Outputs:
- `Y`: VGC_SPR_BASE-relative byte offset.

## SPRITE.ROW

Write one sprite bitmap row using VGC.P0..VGC.P9.

- Kind: `routine`
- Symbol: `sprite_row`
- Address: `$E99D`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `VGC_P6`
- `VGC_P7`
- `VGC_P8`
- `VGC_P9`

## SPRITE.SET_REG16

Write one 16-bit sprite attribute field.

- Kind: `routine`
- Symbol: `sprite_set_reg16`
- Address: `$E9BA`

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.
- `NVR0L`: Low byte.
- `NVR0H`: High byte.

## SPRITE.SET_REG8

Write one 8-bit sprite attribute field.

- Kind: `routine`
- Symbol: `sprite_set_reg8`
- Address: `$E9AF`

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.
- `A`: Field value.

## SPRITE.SET_SHAPE

Set a sprite's shape index.

- Kind: `routine`
- Symbol: `sprite_set_shape`
- Address: `$E9AD`

Inputs:
- `X`: Sprite index.
- `A`: Shape index.

## SYS.REGA

Returned A register from the most recent SYS call.

- Kind: `u8`
- Symbol: `SYS_REGA`
- Address: `$E7`

## SYS.REGX

Returned X register from the most recent SYS call.

- Kind: `u8`
- Symbol: `SYS_REGX`
- Address: `$E8`

## SYS.REGY

Returned Y register from the most recent SYS call.

- Kind: `u8`
- Symbol: `SYS_REGY`
- Address: `$E9`

## TILE.ATTR

Set a tile attribute using TileP0..TileP3.

- Kind: `routine`
- Symbol: `tile_attr`

## TILE.BUF_FILL

Fill the tile command buffer.

- Kind: `routine`
- Symbol: `tile_buf_fill`

## TILE.BUF_PUT

Put a tile command buffer entry.

- Kind: `routine`
- Symbol: `tile_buf_put`

## TILE.BUF_RANGE

Apply a range of tile command buffer entries.

- Kind: `routine`
- Symbol: `tile_buf_range`

## TILE.BUF_SET

Set a tile command buffer entry.

- Kind: `routine`
- Symbol: `tile_buf_set`

## TILE.CLS

Clear tile memory.

- Kind: `routine`
- Symbol: `tile_cls`

## TILE.COL

Copy a column of tile data from TileAddrL/H.

- Kind: `routine`
- Symbol: `tile_col`

## TILE.COMMAND

Issue a raw tile engine command.

- Kind: `routine`
- Symbol: `tile_cmd`

Inputs:
- `A`: Tile command byte.

## TILE.DEF

Define one tile from TileP0 and TileAddrL/H.

- Kind: `routine`
- Symbol: `tile_def`

## TILE.DEF_BULK

Define a run of tiles from TileP0/TileP1 and TileAddrL/H.

- Kind: `routine`
- Symbol: `tile_def_bulk`

## TILE.FILL

Fill tile memory using TileP0/TileP1.

- Kind: `routine`
- Symbol: `tile_fill`

## TILE.LOAD

Load tile data from TileAddrL/H.

- Kind: `routine`
- Symbol: `tile_load`

## TILE.PALETTE

Load tile palette data.

- Kind: `routine`
- Symbol: `tile_pal`

## TILE.PALETTE_COLOR

Set one tile palette colour.

- Kind: `routine`
- Symbol: `tile_palc`

## TILE.PUT

Put a tile using TileP0..TileP3.

- Kind: `routine`
- Symbol: `tile_put`

## TILE.ROW

Copy a row of tile data from TileAddrL/H.

- Kind: `routine`
- Symbol: `tile_row`

## TILE.SET_MIRROR

Set tile mirror mode from the low two bits of X.

- Kind: `routine`
- Symbol: `tile_set_mirror`

Inputs:
- `X`: Mirror mode.

## TILE.SET_SCROLL

Set tile scroll position.

- Kind: `routine`
- Symbol: `tile_set_scroll`

Inputs:
- `NVR0L`: Scroll X low byte.
- `NVR0H`: Scroll X high byte.
- `NVR1L`: Scroll Y low byte.
- `NVR1H`: Scroll Y high byte.

## TILE.SET_SIZE

Select 8x8 or 16x16 tile size.

- Kind: `routine`
- Symbol: `tile_set_size`

Inputs:
- `X`: 16 selects 16x16; any other value selects 8x8.

## TILE.SET_TRANS_COLOR

Set the tile transparent colour.

- Kind: `routine`
- Symbol: `tile_set_trans_color`

Inputs:
- `X`: Transparent colour byte.

## TWEEN.BEGIN

Reset TWEEN.FRAME/DONE and set TWEEN.VALUE to TWEEN.START.

- Kind: `routine`
- Symbol: `tween_begin`

Outputs:
- `TWEEN_FRAME`: Reset to 0.
- `TWEEN_DONE`: Cleared to 0.
- `TWEEN_VALUEL`: Low byte of TWEEN.START.
- `TWEEN_VALUEH`: High byte of TWEEN.START.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`

## TWEEN.DONE

Non-zero when the tween has reached its end value.

- Kind: `u8`
- Symbol: `TWEEN_DONE`

## TWEEN.DURATION

Tween length in frames. Zero snaps to the end value.

- Kind: `u8`
- Symbol: `TWEEN_DURATION`

## TWEEN.EASE

Last eased progress value, 0..255.

- Kind: `u8`
- Symbol: `TWEEN_EASE`

## TWEEN.ENDH

High byte of the tween end value.

- Kind: `u8`
- Symbol: `TWEEN_ENDH`

## TWEEN.ENDL

Low byte of the tween end value.

- Kind: `u8`
- Symbol: `TWEEN_ENDL`

## TWEEN.EVAL

Evaluate the current tween frame using TWEEN.MODE.

- Kind: `routine`
- Symbol: `tween_eval`

Outputs:
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`
- `TWEEN_MODE`

## TWEEN.EVAL_EASE_IN

Evaluate the current tween frame with slow-start dampening.

- Kind: `routine`
- Symbol: `tween_eval_ease_in`

Outputs:
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.EVAL_EASE_IN_OUT

Evaluate the current tween frame with dampening at both ends.

- Kind: `routine`
- Symbol: `tween_eval_ease_in_out`

Outputs:
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.EVAL_EASE_OUT

Evaluate the current tween frame with slow-end dampening.

- Kind: `routine`
- Symbol: `tween_eval_ease_out`

Outputs:
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.EVAL_LINEAR

Evaluate the current tween frame with a linear curve.

- Kind: `routine`
- Symbol: `tween_eval_linear`

Outputs:
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.FRAME

Current tween frame, from 0 through TWEEN.DURATION.

- Kind: `u8`
- Symbol: `TWEEN_FRAME`

## TWEEN.MODE

Easing mode: 0 linear, 1 ease-in, 2 ease-out, 3 ease-in-out.

- Kind: `u8`
- Symbol: `TWEEN_MODE`

## TWEEN.MODE_EASE_IN

Constant for quadratic ease-in interpolation.

- Kind: `const`
- Symbol: `TWEEN_MODE_EASE_IN`
- Address: `$01`

## TWEEN.MODE_EASE_IN_OUT

Constant for quadratic ease-in/ease-out interpolation.

- Kind: `const`
- Symbol: `TWEEN_MODE_EASE_IN_OUT`
- Address: `$03`

## TWEEN.MODE_EASE_OUT

Constant for quadratic ease-out interpolation.

- Kind: `const`
- Symbol: `TWEEN_MODE_EASE_OUT`
- Address: `$02`

## TWEEN.MODE_LINEAR

Constant for linear interpolation.

- Kind: `const`
- Symbol: `TWEEN_MODE_LINEAR`
- Address: `$00`

## TWEEN.PROGRESS

Last normalized progress value, 0..255.

- Kind: `u8`
- Symbol: `TWEEN_PROGRESS`

## TWEEN.STARTH

High byte of the tween start value.

- Kind: `u8`
- Symbol: `TWEEN_STARTH`

## TWEEN.STARTL

Low byte of the tween start value.

- Kind: `u8`
- Symbol: `TWEEN_STARTL`

## TWEEN.STEP

Increment TWEEN.FRAME by one, then evaluate using TWEEN.MODE.

- Kind: `routine`
- Symbol: `tween_step`

Outputs:
- `TWEEN_FRAME`: Advanced frame value.
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`
- `TWEEN_MODE`

## TWEEN.STEP_EASE_IN

Increment TWEEN.FRAME by one, then evaluate with slow-start dampening.

- Kind: `routine`
- Symbol: `tween_step_ease_in`

Outputs:
- `TWEEN_FRAME`: Advanced frame value.
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.STEP_EASE_IN_OUT

Increment TWEEN.FRAME by one, then evaluate with dampening at both ends.

- Kind: `routine`
- Symbol: `tween_step_ease_in_out`

Outputs:
- `TWEEN_FRAME`: Advanced frame value.
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.STEP_EASE_OUT

Increment TWEEN.FRAME by one, then evaluate with slow-end dampening.

- Kind: `routine`
- Symbol: `tween_step_ease_out`

Outputs:
- `TWEEN_FRAME`: Advanced frame value.
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.STEP_LINEAR

Increment TWEEN.FRAME by one, then evaluate with a linear curve.

- Kind: `routine`
- Symbol: `tween_step_linear`

Outputs:
- `TWEEN_FRAME`: Advanced frame value.
- `TWEEN_VALUEL`: Low byte of computed value.
- `TWEEN_VALUEH`: High byte of computed value.
- `TWEEN_DONE`: Non-zero when the end value has been reached.
- `TWEEN_PROGRESS`: Normalized progress, 0..255.
- `TWEEN_EASE`: Eased progress, 0..255.

Requires:
- `TWEEN_STARTL`
- `TWEEN_STARTH`
- `TWEEN_ENDL`
- `TWEEN_ENDH`
- `TWEEN_DURATION`
- `TWEEN_FRAME`

## TWEEN.VALUEH

High byte of the last tween result.

- Kind: `u8`
- Symbol: `TWEEN_VALUEH`

## TWEEN.VALUEL

Low byte of the last tween result.

- Kind: `u8`
- Symbol: `TWEEN_VALUEL`

## VGC.CIRCLE

Issue the VGC circle/ellipse command using VGC.P0..VGC.P7.

- Kind: `routine`
- Symbol: `vgc_circle`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `VGC_P6`
- `VGC_P7`

## VGC.CLS

Clear the text screen through the VGC character output register.

- Kind: `routine`
- Symbol: `vgc_cls`
- Address: `$E924`

## VGC.COMMAND

Issue a raw VGC command without waiting for completion.

- Kind: `routine`
- Symbol: `vgc_cmd`
- Address: `$E90F`

Inputs:
- `A`: VGC command byte.

## VGC.EXEC

Issue a raw VGC command and wait for completion.

- Kind: `routine`
- Symbol: `vgc_exec`

Inputs:
- `A`: VGC command byte.

## VGC.FILL

Issue the VGC filled rectangle command using VGC.P0..VGC.P7.

- Kind: `routine`
- Symbol: `vgc_fill`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `VGC_P6`
- `VGC_P7`

## VGC.FLASH_OFF

Disable flashing text.

- Kind: `routine`
- Symbol: `vgc_flash_off`
- Address: `$E981`

## VGC.FLASH_ON

Enable flashing text.

- Kind: `routine`
- Symbol: `vgc_flash_on`
- Address: `$E978`

## VGC.GCLS

Issue the VGC graphics clear command.

- Kind: `routine`
- Symbol: `vgc_gcls`

## VGC.GCOLOR

Issue the VGC graphics colour command using VGC.P0/P1.

- Kind: `routine`
- Symbol: `vgc_gcolor`

Requires:
- `VGC_P0`
- `VGC_P1`

## VGC.GFXTRANS

Graphics-plane transparent color register. Defaults to 0; set to another palette index when graphics code needs visible palette-0 black pixels.

- Kind: `const`
- Symbol: `VGC_GFXTRANS`
- Address: `$A0E8`

## VGC.GTEXT

Issue the VGC graphics text command using VGC.P0..VGC.P5 and FIO.NAME.

- Kind: `routine`
- Symbol: `vgc_gtext`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `FIO_NAME`
- `FIO_NAMELEN`

## VGC.LINE

Issue the VGC line command using VGC.P0..VGC.P7.

- Kind: `routine`
- Symbol: `vgc_line`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `VGC_P6`
- `VGC_P7`

## VGC.LOCATE

Set the text cursor position from VGC.P0/VGC.P1.

- Kind: `routine`
- Symbol: `vgc_locate`
- Address: `$E936`

Inputs:
- `VGC_P0`: Cursor X column.
- `VGC_P1`: Cursor Y row.

## VGC.MEM_READ

Read one byte from VGC memory using VGC.P0..VGC.P4.

- Kind: `routine`
- Symbol: `vgc_mem_read`
- Address: `$E94B`

Outputs:
- `VGC_P3`: Read byte.

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P4`

## VGC.MEM_WRITE

Write one byte to VGC memory using VGC.P0..VGC.P4.

- Kind: `routine`
- Symbol: `vgc_mem_write`
- Address: `$E94E`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`

## VGC.PAINT

Issue the VGC paint command using VGC.P0..VGC.P3.

- Kind: `routine`
- Symbol: `vgc_paint`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`

## VGC.PLOT

Issue the VGC plot command using VGC.P0..VGC.P3.

- Kind: `routine`
- Symbol: `vgc_plot`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`

## VGC.RECT

Issue the VGC rectangle command using VGC.P0..VGC.P7.

- Kind: `routine`
- Symbol: `vgc_rect`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`
- `VGC_P4`
- `VGC_P5`
- `VGC_P6`
- `VGC_P7`

## VGC.REVERSE

Enable reverse text using the current foreground/background colours.

- Kind: `routine`
- Symbol: `vgc_reverse_default`
- Address: `$E956`

## VGC.REVERSE_EXPLICIT

Enable reverse text with explicit packed foreground/background colours.

- Kind: `routine`
- Symbol: `vgc_reverse_explicit`
- Address: `$E961`

Inputs:
- `A`: Packed reverse attribute, low nibble foreground and high nibble background.

## VGC.REVERSE_OFF

Disable reverse text.

- Kind: `routine`
- Symbol: `vgc_reverse_off`
- Address: `$E96F`

## VGC.SET_BG

Set the current text background colour.

- Kind: `routine`
- Symbol: `vgc_set_bg`
- Address: `$E92E`

Inputs:
- `X`: Background colour byte.

## VGC.SET_BORDER

Set the VGC border colour.

- Kind: `routine`
- Symbol: `vgc_set_border`
- Address: `$E932`

Inputs:
- `X`: Border colour byte.

## VGC.SET_FG

Set the current text foreground colour.

- Kind: `routine`
- Symbol: `vgc_set_fg`
- Address: `$E92A`

Inputs:
- `X`: Foreground colour byte.

## VGC.SET_FONT

Select the active text font slot.

- Kind: `routine`
- Symbol: `vgc_set_font`
- Address: `$E947`

Inputs:
- `X`: Font slot, 0..7.

## VGC.SET_MODE

Set the active VGC graphics/text mode.

- Kind: `routine`
- Symbol: `vgc_set_mode`
- Address: `$E943`

Inputs:
- `X`: Mode byte.

## VGC.UNPLOT

Issue the VGC unplot command using VGC.P0..VGC.P3.

- Kind: `routine`
- Symbol: `vgc_unplot`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`

## VGC.VSYNC

Wait until the VGC frame counter advances.

- Kind: `routine`
- Symbol: `vgc_vsync`
- Address: `$E91B`

Outputs:
- `A`: Last observed frame counter value.

## VGC.WAIT_COMMAND

Wait for the active VGC command to complete.

- Kind: `routine`
- Symbol: `vgc_wait_cmd`
- Address: `$E913`

## VSPRITE.BGADDRH

High byte of the caller-owned saved-background buffer address. Use 0 for CPU RAM.

- Kind: `u8`
- Symbol: `VSPRITE_BGADDRH`

## VSPRITE.BGADDRL

Low byte of the caller-owned saved-background buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_BGADDRL`

## VSPRITE.BGADDRM

Middle byte of the caller-owned saved-background buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_BGADDRM`

## VSPRITE.BGSPACE

Blitter memory space for the saved-background buffer. Use BLT_SPACE_CPU for normal RAM buffers.

- Kind: `u8`
- Symbol: `VSPRITE_BGSPACE`

## VSPRITE.BGSTRH

High byte of the saved-background buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_BGSTRH`

## VSPRITE.BGSTRL

Low byte of the saved-background buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_BGSTRL`

## VSPRITE.BLIT

Copy the configured virtual sprite rectangle with the blitter.

- Kind: `routine`
- Symbol: `vsprite_blit`

Inputs:
- `VSPRITE_FLAGS`: Set bit 0 to enable color-key transparency.
- `VSPRITE_COLORKEY`: Source byte skipped when color-key mode is enabled.
- `VGC_GFXTRANS`: Graphics-plane color treated as transparent by display composition; set this to a non-black color when blitting visible palette-0 black pixels.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_SRCSPACE`
- `VSPRITE_DSTSPACE`
- `VSPRITE_SRCADDR*`
- `VSPRITE_DSTADDR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_SRCSTR*`
- `VSPRITE_DSTSTR*`

## VSPRITE.COLORKEY

Source byte skipped by color-keyed blits. For graphics-plane virtual sprites, normally match this to VGC.GFXTRANS.

- Kind: `u8`
- Symbol: `VSPRITE_COLORKEY`

## VSPRITE.DESC_FLAG_COLORKEY

Descriptor flag enabling source color-key transparency.

- Kind: `const`
- Symbol: `VSPRITE_DESC_FLAG_COLORKEY`
- Address: `$01`

## VSPRITE.DESC_SAVED

Internal descriptor flag set when old X/Y has a saved background to restore.

- Kind: `const`
- Symbol: `VSPRITE_DESC_SAVED`
- Address: `$40`

## VSPRITE.DESC_VISIBLE

Descriptor flag enabling scene save/draw for this virtual sprite.

- Kind: `const`
- Symbol: `VSPRITE_DESC_VISIBLE`
- Address: `$80`

## VSPRITE.DSTADDRH

High byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.

- Kind: `u8`
- Symbol: `VSPRITE_DSTADDRH`

## VSPRITE.DSTADDRL

Low byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.

- Kind: `u8`
- Symbol: `VSPRITE_DSTADDRL`

## VSPRITE.DSTADDRM

Middle byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.

- Kind: `u8`
- Symbol: `VSPRITE_DSTADDRM`

## VSPRITE.DSTSPACE

Blitter memory space for the virtual-sprite destination rectangle.

- Kind: `u8`
- Symbol: `VSPRITE_DSTSPACE`

## VSPRITE.DSTSTRH

High byte of the destination row stride.

- Kind: `u8`
- Symbol: `VSPRITE_DSTSTRH`

## VSPRITE.DSTSTRL

Low byte of the destination row stride.

- Kind: `u8`
- Symbol: `VSPRITE_DSTSTRL`

## VSPRITE.FILL

Fill the configured destination rectangle with VSPRITE.FILLVALUE.

- Kind: `routine`
- Symbol: `vsprite_fill`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_DSTSPACE`
- `VSPRITE_DSTADDR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_DSTSTR*`

## VSPRITE.FILLVALUE

Byte value used by VSPRITE.FILL and VSPRITE.GFX_FILL.

- Kind: `u8`
- Symbol: `VSPRITE_FILLVALUE`

## VSPRITE.FLAGS

Bitfield controlling virtual-sprite blits. Set VSPRITE.FLAG_COLORKEY to enable source transparency.

- Kind: `u8`
- Symbol: `VSPRITE_FLAGS`

## VSPRITE.FLAG_COLORKEY

Enable source color-key transparency for VSPRITE.BLIT and VSPRITE.GFX_BLIT.

- Kind: `const`
- Symbol: `VSPRITE_FLAG_COLORKEY`
- Address: `$01`

## VSPRITE.GFX_ADDR

Convert VSPRITE.X/Y to a linear VGC graphics-plane destination address.

- Kind: `routine`
- Symbol: `vsprite_gfx_addr`

Outputs:
- `VSPRITE_DSTADDR*`: y * 320 + x.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`

## VSPRITE.GFX_BLIT

Copy the configured virtual sprite to the VGC graphics plane at X/Y.

- Kind: `routine`
- Symbol: `vsprite_gfx_blit`

Inputs:
- `VSPRITE_COLORKEY`: Source transparent color when VSPRITE_FLAG_COLORKEY is set; normally match this to VGC_GFXTRANS for graphics-plane virtual sprites.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_SRCSPACE`
- `VSPRITE_SRCADDR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_SRCSTR*`

## VSPRITE.GFX_FILL

Fill a graphics-plane rectangle at VSPRITE.X/Y.

- Kind: `routine`
- Symbol: `vsprite_gfx_fill`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_FILLVALUE`

## VSPRITE.GFX_RESTORE_BG

Restore a caller-owned VSPRITE.BG* buffer into the graphics plane at VSPRITE.X/Y.

- Kind: `routine`
- Symbol: `vsprite_gfx_restore_bg`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_BGSPACE`
- `VSPRITE_BGADDR*`
- `VSPRITE_BGSTR*`

## VSPRITE.GFX_SAVE_BG

Save a graphics-plane rectangle under VSPRITE.X/Y into the caller-owned VSPRITE.BG* buffer.

- Kind: `routine`
- Symbol: `vsprite_gfx_save_bg`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_BGSPACE`
- `VSPRITE_BGADDR*`
- `VSPRITE_BGSTR*`

## VSPRITE.HEIGHTH

High byte of the rectangle height in rows.

- Kind: `u8`
- Symbol: `VSPRITE_HEIGHTH`

## VSPRITE.HEIGHTL

Low byte of the rectangle height in rows.

- Kind: `u8`
- Symbol: `VSPRITE_HEIGHTL`

## VSPRITE.SCENE_ADDRH

High byte of the caller-owned scene descriptor table.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_ADDRH`

## VSPRITE.SCENE_ADDRL

Low byte of the caller-owned scene descriptor table.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_ADDRL`

## VSPRITE.SCENE_BEGIN

Restore all saved old backgrounds in the configured scene descriptor list.

- Kind: `routine`
- Symbol: `vsprite_scene_begin`

Requires:
- `VSPRITE_SCENE_ADDRL`
- `VSPRITE_SCENE_ADDRH`
- `VSPRITE_SCENE_COUNT`

## VSPRITE.SCENE_COMMIT

Save all current backgrounds, then draw all visible descriptors in list order.

- Kind: `routine`
- Symbol: `vsprite_scene_commit`

Requires:
- `VSPRITE_SCENE_ADDRL`
- `VSPRITE_SCENE_ADDRH`
- `VSPRITE_SCENE_COUNT`

## VSPRITE.SCENE_COMMIT_ATOMIC

Compose a scene into a caller-owned work buffer, then commit the completed dirty rectangle to graphics memory.

- Kind: `routine`
- Symbol: `vsprite_scene_commit_atomic`

Requires:
- `VSPRITE_SCENE_ADDRL`
- `VSPRITE_SCENE_ADDRH`
- `VSPRITE_SCENE_COUNT`
- `VSPRITE_SCENE_X*`
- `VSPRITE_SCENE_Y`
- `VSPRITE_SCENE_WIDTH*`
- `VSPRITE_SCENE_HEIGHT*`
- `VSPRITE_SCENE_WORK*`

## VSPRITE.SCENE_COUNT

Number of descriptors in the scene table.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_COUNT`

## VSPRITE.SCENE_DRAW

Draw all visible descriptors in list order without saving backgrounds.

- Kind: `routine`
- Symbol: `vsprite_scene_draw`

Requires:
- `VSPRITE_SCENE_ADDRL`
- `VSPRITE_SCENE_ADDRH`
- `VSPRITE_SCENE_COUNT`

## VSPRITE.SCENE_HEIGHTH

High byte of the dirty rectangle height for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_HEIGHTH`

## VSPRITE.SCENE_HEIGHTL

Low byte of the dirty rectangle height for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_HEIGHTL`

## VSPRITE.SCENE_WIDTHH

High byte of the dirty rectangle width for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WIDTHH`

## VSPRITE.SCENE_WIDTHL

Low byte of the dirty rectangle width for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WIDTHL`

## VSPRITE.SCENE_WORKADDRH

High byte of the atomic scene work buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKADDRH`

## VSPRITE.SCENE_WORKADDRL

Low byte of the atomic scene work buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKADDRL`

## VSPRITE.SCENE_WORKADDRM

Middle byte of the atomic scene work buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKADDRM`

## VSPRITE.SCENE_WORKSPACE

Blitter memory space for the atomic scene work buffer.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKSPACE`

## VSPRITE.SCENE_WORKSTRH

High byte of the atomic scene work buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKSTRH`

## VSPRITE.SCENE_WORKSTRL

Low byte of the atomic scene work buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_WORKSTRL`

## VSPRITE.SCENE_XH

High byte of the graphics-plane dirty rectangle X coordinate for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_XH`

## VSPRITE.SCENE_XL

Low byte of the graphics-plane dirty rectangle X coordinate for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_XL`

## VSPRITE.SCENE_Y

Graphics-plane dirty rectangle Y coordinate for atomic scene commits.

- Kind: `u8`
- Symbol: `VSPRITE_SCENE_Y`

## VSPRITE.SRCADDRH

High byte of the virtual-sprite source address.

- Kind: `u8`
- Symbol: `VSPRITE_SRCADDRH`

## VSPRITE.SRCADDRL

Low byte of the virtual-sprite source address.

- Kind: `u8`
- Symbol: `VSPRITE_SRCADDRL`

## VSPRITE.SRCADDRM

Middle byte of the virtual-sprite source address.

- Kind: `u8`
- Symbol: `VSPRITE_SRCADDRM`

## VSPRITE.SRCSPACE

Blitter memory space for the virtual-sprite source rectangle.

- Kind: `u8`
- Symbol: `VSPRITE_SRCSPACE`

## VSPRITE.SRCSTRH

High byte of the source row stride.

- Kind: `u8`
- Symbol: `VSPRITE_SRCSTRH`

## VSPRITE.SRCSTRL

Low byte of the source row stride.

- Kind: `u8`
- Symbol: `VSPRITE_SRCSTRL`

## VSPRITE.WIDTHH

High byte of the rectangle width in pixels/bytes.

- Kind: `u8`
- Symbol: `VSPRITE_WIDTHH`

## VSPRITE.WIDTHL

Low byte of the rectangle width in pixels/bytes.

- Kind: `u8`
- Symbol: `VSPRITE_WIDTHL`

## VSPRITE.XH

High byte of the graphics-plane X coordinate used by VSPRITE.GFX_* helpers.

- Kind: `u8`
- Symbol: `VSPRITE_XH`

## VSPRITE.XL

Low byte of the graphics-plane X coordinate used by VSPRITE.GFX_* helpers.

- Kind: `u8`
- Symbol: `VSPRITE_XL`

## VSPRITE.Y

Graphics-plane Y coordinate used by VSPRITE.GFX_* helpers.

- Kind: `u8`
- Symbol: `VSPRITE_Y`

## XMC.ALLOC

Allocate an XRAM block using XMC_LENL/H.

- Kind: `routine`
- Symbol: `xmc_alloc_block`

Outputs:
- `XMC_XAL`: Allocated address low byte.
- `XMC_XAM`: Allocated address middle byte.
- `XMC_XAH`: Allocated address high byte.

Requires:
- `XMC_LENL`
- `XMC_LENH`

## XMC.COMMAND_STATUS

Process XMC_CMD and return A=0 on XMC_OK, A=1 otherwise.

- Kind: `routine`
- Symbol: `xmc_command_status`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `XMC_CMD`

## XMC.DIR_OPEN

Open the named XRAM block directory.

- Kind: `routine`
- Symbol: `xmc_dir_open`

## XMC.DIR_READ

Read the next named XRAM block directory entry.

- Kind: `routine`
- Symbol: `xmc_dir_read`

## XMC.FETCH_RAW

Copy XRAM to RAM using XMC_XAL/M/H, XMC_RAML/H, and XMC_LENL/H.

- Kind: `routine`
- Symbol: `xmc_fetch_raw`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_RAML`
- `XMC_RAMH`
- `XMC_LENL`
- `XMC_LENH`

## XMC.MAP_WINDOW

Map window X to XMC_XAL/M in the selected XMC bank.

- Kind: `routine`
- Symbol: `xmc_map_window`

Inputs:
- `X`: Window index, 0..3.

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_BANK`

## XMC.NAMED_DELETE

Delete a named XRAM block.

- Kind: `routine`
- Symbol: `xmc_named_delete`

Requires:
- `XMC_NAME`
- `XMC_NAMELEN`

## XMC.NAMED_FETCH

Fetch a named XRAM block into RAM.

- Kind: `routine`
- Symbol: `xmc_named_fetch`

Requires:
- `XMC_NAME`
- `XMC_NAMELEN`
- `XMC_RAML`
- `XMC_RAMH`

## XMC.NAMED_STASH

Allocate/update a named XRAM block and copy RAM into it.

- Kind: `routine`
- Symbol: `xmc_named_stash`

Requires:
- `XMC_NAME`
- `XMC_NAMELEN`
- `XMC_RAML`
- `XMC_RAMH`
- `XMC_LENL`
- `XMC_LENH`

## XMC.PROCESS

Process XMC_CMD and update XMC_STATUS/XMC_ERRCODE.

- Kind: `routine`
- Symbol: `xmc_process`

Requires:
- `XMC_CMD`

## XMC.PUT_BYTE

Write XMC_DATA to XMC_XAL/M/H.

- Kind: `routine`
- Symbol: `xmc_put_byte`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_DATA`

## XMC.RELEASE

Release an XMC allocation range.

- Kind: `routine`
- Symbol: `xmc_release`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_LENL`
- `XMC_LENH`

## XMC.RESET_USAGE

Reset XRAM usage and named allocation metadata.

- Kind: `routine`
- Symbol: `xmc_reset_usage`

## XMC.SELECT_BANK

Select the current 64KB XRAM bank.

- Kind: `routine`
- Symbol: `xmc_select_bank`

Inputs:
- `X`: Bank index, 0..XMC_BANKS-1.

Outputs:
- `A`: 0 on success, 1 if the bank is out of range.

## XMC.SET_WINDOW_ADDR

Set a mapped window base from XMC_XAL/M and the selected XMC bank.

- Kind: `routine`
- Symbol: `xmc_set_window_addr_current_bank`

Inputs:
- `X`: Window index, 0..3.

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_BANK`

## XMC.STASH_RAW

Copy RAM to XRAM using XMC_RAML/H, XMC_XAL/M/H, and XMC_LENL/H.

- Kind: `routine`
- Symbol: `xmc_stash_raw`

Requires:
- `XMC_RAML`
- `XMC_RAMH`
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_LENL`
- `XMC_LENH`

## XMC.UNMAP_WINDOW

Disable a mapped XRAM window.

- Kind: `routine`
- Symbol: `xmc_unmap_window`

Inputs:
- `X`: Window index, 0..3.

## XMC.WINDOW_BIT

Convert window index 0..3 to its XMC_WINCTL bit mask.

- Kind: `routine`
- Symbol: `xmc_window_bit`

Inputs:
- `X`: Window index, 0..3.

Outputs:
- `A`: Window bit mask.
- `C`: Clear on success, set on invalid index.

## XRAM.ADDRH

High byte of flat 24-bit XRAM address.

- Kind: `zp`
- Symbol: `XRAM_ADDRH`
- Address: `$22`

## XRAM.ADDRL

Low byte of flat 24-bit XRAM address.

- Kind: `zp`
- Symbol: `XRAM_ADDRL`
- Address: `$20`

## XRAM.ADDRM

Middle byte of flat 24-bit XRAM address.

- Kind: `zp`
- Symbol: `XRAM_ADDRM`
- Address: `$21`

## XRAM.ADDR_IN_RANGE

Validate XRAM_ADDRH is inside the 512KB XRAM device.

- Kind: `routine`
- Symbol: `xram_addr_is_in_range`

Requires:
- `XRAM_ADDRH`

## XRAM.COPY_FROM_RAM

Copy CPU RAM to XRAM using the XRAM pseudo-register ABI.

- Kind: `routine`
- Symbol: `xram_copy_from_ram`

Requires:
- `XRAM_RAML`
- `XRAM_RAMH`
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_LENL`
- `XRAM_LENH`

## XRAM.COPY_TO_RAM

Copy XRAM to CPU RAM using the XRAM pseudo-register ABI.

- Kind: `routine`
- Symbol: `xram_copy_to_ram`

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_RAML`
- `XRAM_RAMH`
- `XRAM_LENL`
- `XRAM_LENH`

## XRAM.DATA

Byte value used by XRAM read, write, and fill routines.

- Kind: `zp`
- Symbol: `XRAM_DATA`
- Address: `$23`

## XRAM.FILL

Fill XRAM using XRAM_ADDRL/M/H, XRAM_LENL/H, and XRAM_DATA.

- Kind: `routine`
- Symbol: `xram_fill`

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_LENL`
- `XRAM_LENH`
- `XRAM_DATA`

## XRAM.LENH

High byte of XRAM transfer length.

- Kind: `zp`
- Symbol: `XRAM_LENH`
- Address: `$27`

## XRAM.LENL

Low byte of XRAM transfer length.

- Kind: `zp`
- Symbol: `XRAM_LENL`
- Address: `$26`

## XRAM.MAP_WINDOW3

Map XRAM window 3 to XRAM_ADDRM/H.

- Kind: `routine`
- Symbol: `xram_map_window3`

Requires:
- `XRAM_ADDRM`
- `XRAM_ADDRH`

## XRAM.NAMELEN

Filename length for XRAM file streaming routines.

- Kind: `zp`
- Symbol: `XRAM_NAMELEN`
- Address: `$28`

## XRAM.NAMEPTR_H

High byte of filename pointer for XRAM file streaming routines.

- Kind: `zp`
- Symbol: `XRAM_NAMEPTR_H`
- Address: `$2A`

## XRAM.NAMEPTR_L

Low byte of filename pointer for XRAM file streaming routines.

- Kind: `zp`
- Symbol: `XRAM_NAMEPTR_L`
- Address: `$29`

## XRAM.RAMH

High byte of CPU RAM address for XRAM copy routines.

- Kind: `zp`
- Symbol: `XRAM_RAMH`
- Address: `$25`

## XRAM.RAML

Low byte of CPU RAM address for XRAM copy routines.

- Kind: `zp`
- Symbol: `XRAM_RAML`
- Address: `$24`

## XRAM.READ8

Read one byte from XRAM_ADDRL/M/H into XRAM_DATA.

- Kind: `routine`
- Symbol: `xram_read8`

Outputs:
- `XRAM_DATA`: Byte read from XRAM.

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`

## XRAM.SET_ERROR

Set XMC-compatible XRAM error status.

- Kind: `routine`
- Symbol: `xram_set_error`

Inputs:
- `A`: XMC-compatible error code.

Outputs:
- `A`: 1.

## XRAM.SET_OK

Set XMC-compatible XRAM status to OK and return A=0.

- Kind: `routine`
- Symbol: `xram_set_ok`

## XRAM.WAIT_DMA

Wait for the shared DMA controller and map the result to XRAM status.

- Kind: `routine`
- Symbol: `xram_wait_dma`

## XRAM.WRITE8

Write XRAM_DATA to XRAM_ADDRL/M/H.

- Kind: `routine`
- Symbol: `xram_write8`

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_DATA`

## XRAM.XLOAD

Stream a file directly into XRAM using the XRAM pseudo-register ABI.

- Kind: `routine`
- Symbol: `xram_xload`

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_LENL`
- `XRAM_LENH`
- `XRAM_NAMELEN`
- `XRAM_NAMEPTR_L`
- `XRAM_NAMEPTR_H`

## XRAM.XMC_COPY_FROM_RAM

Copy RAM to XRAM using the XMC register ABI.

- Kind: `routine`
- Symbol: `xram_xmc_copy_from_ram`

Requires:
- `XMC_RAML`
- `XMC_RAMH`
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_LENL`
- `XMC_LENH`

## XRAM.XMC_COPY_TO_RAM

Copy XRAM to RAM using the XMC register ABI.

- Kind: `routine`
- Symbol: `xram_xmc_copy_to_ram`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_RAML`
- `XMC_RAMH`
- `XMC_LENL`
- `XMC_LENH`

## XRAM.XMC_FILL

Fill XRAM using the XMC register ABI.

- Kind: `routine`
- Symbol: `xram_xmc_fill`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_LENL`
- `XMC_LENH`
- `XMC_DATA`

## XRAM.XMC_READ8

Read one byte from XMC_XAL/M/H into XMC_DATA.

- Kind: `routine`
- Symbol: `xram_xmc_read8`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`

## XRAM.XMC_WRITE8

Write XMC_DATA to XMC_XAL/M/H.

- Kind: `routine`
- Symbol: `xram_xmc_write8`

Requires:
- `XMC_XAL`
- `XMC_XAM`
- `XMC_XAH`
- `XMC_DATA`

## XRAM.XSAVE

Stream XRAM directly to a file using the XRAM pseudo-register ABI.

- Kind: `routine`
- Symbol: `xram_xsave`

Requires:
- `XRAM_ADDRL`
- `XRAM_ADDRM`
- `XRAM_ADDRH`
- `XRAM_LENL`
- `XRAM_LENH`
- `XRAM_NAMELEN`
- `XRAM_NAMEPTR_L`
- `XRAM_NAMEPTR_H`
