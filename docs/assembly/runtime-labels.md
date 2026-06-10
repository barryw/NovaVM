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

## AUDIO.INIT

Initialize the SID sound-effect instrument table and scheduler state.

- Kind: `routine`
- Symbol: `audio_init`
- ABI: `none`

## AUDIO.INSTRUMENT

Instrument slot for AUDIO.SOUND.

- Kind: `u8`
- Symbol: `AUDIO_INSTRUMENT`
- Address: `$B9A6`

## AUDIO.INSTRUMENT_SET

Define a simple SID sound-effect instrument envelope and waveform.

- Kind: `routine`
- Symbol: `audio_instrument`
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

## AUDIO.META_AUTHOR

32-byte null-padded active music author/composer.

- Kind: `bytes`
- Symbol: `AUDIO_META_AUTHOR`
- Address: `$BAD3`

## AUDIO.META_COPYRIGHT

32-byte null-padded active music copyright/comment.

- Kind: `bytes`
- Symbol: `AUDIO_META_COPYRIGHT`
- Address: `$BAF3`

## AUDIO.META_DURATION_H

Duration seconds high byte when known.

- Kind: `u8`
- Symbol: `AUDIO_META_DURATION_H`
- Address: `$BB1B`

## AUDIO.META_DURATION_L

Duration seconds low byte when known.

- Kind: `u8`
- Symbol: `AUDIO_META_DURATION_L`
- Address: `$BB1A`

## AUDIO.META_FLAGS

Music-specific flags for the active metadata block.

- Kind: `u8`
- Symbol: `AUDIO_META_FLAGS`
- Address: `$BB1C`

## AUDIO.META_INIT_H

SID init address high byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_INIT_H`
- Address: `$BB16`

## AUDIO.META_INIT_L

SID init address low byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_INIT_L`
- Address: `$BB15`

## AUDIO.META_LOAD_H

SID load address high byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_LOAD_H`
- Address: `$BB14`

## AUDIO.META_LOAD_L

SID load address low byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_LOAD_L`
- Address: `$BB13`

## AUDIO.META_PLAY_H

SID play address high byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_PLAY_H`
- Address: `$BB18`

## AUDIO.META_PLAY_L

SID play address low byte when applicable.

- Kind: `u8`
- Symbol: `AUDIO_META_PLAY_L`
- Address: `$BB17`

## AUDIO.META_SIZE_H

High byte of active music file size.

- Kind: `u8`
- Symbol: `AUDIO_META_SIZE_H`
- Address: `$BAB2`

## AUDIO.META_SIZE_L

Low byte of active music file size.

- Kind: `u8`
- Symbol: `AUDIO_META_SIZE_L`
- Address: `$BAB1`

## AUDIO.META_SONGS

SID subtune count or MIDI track/song count when known.

- Kind: `u8`
- Symbol: `AUDIO_META_SONGS`
- Address: `$BB19`

## AUDIO.META_SOUNDFONT

64-byte null-padded active MIDI/WTS soundfont name.

- Kind: `bytes`
- Symbol: `AUDIO_META_SOUNDFONT`
- Address: `$BB8E`

## AUDIO.META_TITLE

32-byte null-padded active music title.

- Kind: `bytes`
- Symbol: `AUDIO_META_TITLE`
- Address: `$BAB3`

## AUDIO.META_TYPE

Active music metadata file type: 1=SID, 3=MIDI/WTS.

- Kind: `u8`
- Symbol: `AUDIO_META_TYPE`
- Address: `$BAB0`

## AUDIO.MIDPLAY

Play the MIDI file named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `audio_midplay`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`

## AUDIO.MIDPLAY_FILE

Copy a pointer-based MIDI filename into FIO.NAME and start MIDI playback.

- Kind: `routine`
- Symbol: `audio_midplay_file`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`

## AUDIO.MIDSTOP

Stop MIDI file playback.

- Kind: `routine`
- Symbol: `audio_midstop`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_LOOP

Enable or disable music looping. AUDIO.NOTE is 0=off, nonzero=on.

- Kind: `routine`
- Symbol: `audio_music_loop`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`

## AUDIO.MUSIC_NOTE

Return the current MIDI note for a music voice.

- Kind: `routine`
- Symbol: `audio_music_note`
- ABI: `register`

Inputs:
- `X`: Voice number, 1..14

Outputs:
- `A`: Current MIDI note, or 0 if out of range or silent

## AUDIO.MUSIC_PLAY

Start queued music playback.

- Kind: `routine`
- Symbol: `audio_music_play`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_PLAYING

Return nonzero if music is playing.

- Kind: `routine`
- Symbol: `audio_music_playing`
- ABI: `none`

Outputs:
- `A`: 1 if playing, 0 if idle

## AUDIO.MUSIC_PRIORITY

Set hosted MML priority from AUDIO.NOTE and following pseudo-register bytes.

- Kind: `routine`
- Symbol: `audio_music_priority`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`

## AUDIO.MUSIC_SEQUENCE

Queue an MML sequence for AUDIO.VOICE using AUDIO.STRL/H and AUDIO.STRLEN.

- Kind: `routine`
- Symbol: `audio_music_sequence`
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
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.MUSIC_TEMPO

Set music tempo from AUDIO.NOTE/AUDIO.DURATION as a 16-bit BPM value.

- Kind: `routine`
- Symbol: `audio_music_tempo`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `AUDIO.NOTE`
- `AUDIO.DURATION`

## AUDIO.NOISE

Play a noise burst on SID voice 0 for a number of video frames.

- Kind: `routine`
- Symbol: `audio_noise`
- ABI: `register`

Inputs:
- `A`: duration in frames.

## AUDIO.NOTE

MIDI note for AUDIO.SOUND.

- Kind: `u8`
- Symbol: `AUDIO_NOTE`
- Address: `$B9A4`

## AUDIO.PLAY_SOUND

Play a one-shot note using A/X/Y register arguments.

- Kind: `routine`
- Symbol: `audio_play_sound`
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
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`

## AUDIO.SFLOAD_FILE

Copy a pointer-based soundfont filename into FIO.NAME and load it.

- Kind: `routine`
- Symbol: `audio_sfload_file`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`

## AUDIO.SFX_PLAYING

Return nonzero if the hosted music engine reports a sound effect.

- Kind: `routine`
- Symbol: `audio_sfx_playing`
- ABI: `none`

Outputs:
- `A`: 1 if playing, 0 if idle

## AUDIO.SIDPLAY

Play the SID file named by FIO.NAME/FIO.NAMELEN. AUDIO.NOTE is reused as the 1-based song number.

- Kind: `routine`
- Symbol: `audio_sidplay`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO.NAME`
- `FIO.NAMELEN`
- `AUDIO.NOTE`

## AUDIO.SIDPLAY_FILE

Copy a pointer-based SID filename into FIO.NAME and start SID playback.

- Kind: `routine`
- Symbol: `audio_sidplay_file`
- ABI: `pseudo-register`

Outputs:
- `A`: 0 on success, nonzero on error

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`
- `AUDIO_NOTE`

## AUDIO.SIDSTOP

Stop SID file playback.

- Kind: `routine`
- Symbol: `audio_sidstop`
- ABI: `none`

Outputs:
- `A`: 0 on success, nonzero on error

## AUDIO.SOUND

Start a fire-and-forget SID note from AUDIO.NOTE, AUDIO.DURATION, and AUDIO.INSTRUMENT.

- Kind: `routine`
- Symbol: `audio_sound`
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
- ABI: `none`

Outputs:
- `A`: Raw MUSIC_STATUS byte

## AUDIO.STATUS_LOADING

MUSIC_STATUS bit set while hosted SID/MIDI/soundfont assets are loading.

- Kind: `const`
- Symbol: `AUDIO_STATUS_LOADING`
- Address: `$10`

## AUDIO.STATUS_MUSIC

MUSIC_STATUS bit set while hosted music playback is active.

- Kind: `const`
- Symbol: `AUDIO_STATUS_MUSIC`
- Address: `$02`

## AUDIO.STATUS_SFX

MUSIC_STATUS bit set while a hosted sound effect is active.

- Kind: `const`
- Symbol: `AUDIO_STATUS_SFX`
- Address: `$01`

## AUDIO.STATUS_SID

MUSIC_STATUS bit set when the current music source uses SID voices.

- Kind: `const`
- Symbol: `AUDIO_STATUS_SID`
- Address: `$04`

## AUDIO.STATUS_WTS

MUSIC_STATUS bit set when the current music source uses WTS/MIDI voices.

- Kind: `const`
- Symbol: `AUDIO_STATUS_WTS`
- Address: `$08`

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

## AUDIO.TICK

Advance fire-and-forget SID sound-effect durations by one frame.

- Kind: `routine`
- Symbol: `audio_tick`
- ABI: `none`

## AUDIO.TONE

Play a sawtooth tone on SID voice 0 for a number of video frames.

- Kind: `routine`
- Symbol: `audio_tone`
- ABI: `register`

Inputs:
- `NVR0L/NVR0H`: SID frequency word (lo/hi). A: duration in frames.

## AUDIO.VOICE

Voice selector. Use 0 for SID master volume or 1..6 for SID voice volume.

- Kind: `u8`
- Symbol: `AUDIO_VOICE`
- Address: `$B9A5`

## AUDIO.VOLUME

Set SID master or per-voice volume from AUDIO.VOLUME_LEVEL and AUDIO.VOICE.

- Kind: `routine`
- Symbol: `audio_volume`
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

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.CHECK

Convert the current FIO status register to the shared A=0/1 result.

- Kind: `routine`
- Symbol: `fio_check`

Outputs:
- `A`: 0 on success, 1 on error.

## FIO.CLEAR_ERROR

Clear the host-visible FIO status and error latch.

- Kind: `routine`
- Symbol: `fio_clear_error`

Outputs:
- `A`: 0 on success.

## FIO.CMD_LOADRUNTIME

Host command that streams a 16K file into the primary runtime ROM bank. Callers must execute from RAM while issuing it.

- Kind: `const`
- Symbol: `FIO_CMD_LOADRUNTIME`
- Address: `$28`

## FIO.CMD_NVGLOAD

Host command that decodes a Nova NVG1 file into the graphics bitmap plane.

- Kind: `const`
- Symbol: `FIO_CMD_NVGLOAD`
- Address: `$2B`

## FIO.CMD_RNG

Host command that returns 32 bits from NovaHost's host-backed RNG provider.

- Kind: `const`
- Symbol: `FIO_CMD_RNG`
- Address: `$2A`

## FIO.CMD_XPAGE

Host command that streams a file slice into XRAM, CPU RAM, or VGC memory using FIO_DIRTYPE as the target selector.

- Kind: `const`
- Symbol: `FIO_CMD_XPAGE`
- Address: `$29`

## FIO.COPY_NAME

Copy a pointer-based filename into FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_copy_name`

Outputs:
- `A`: 0 on success, 1 on invalid name.

Requires:
- `FIO_ARG_NAMELEN`
- `FIO_ARG_NAMEPTR_L`
- `FIO_ARG_NAMEPTR_H`

## FIO.DELETE

Delete the host file or BASIC program named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_delete`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.DIR_OPEN

Open a directory listing using optional FIO.NAME/FIO.NAMELEN filter.

- Kind: `routine`
- Symbol: `fio_dir_open`

## FIO.DIR_READ

Read the next directory entry into the FIO directory result registers.

- Kind: `routine`
- Symbol: `fio_dir_read`

## FIO.EXEC

Issue an FIO command, wait for completion, and return A=0/1 status.

- Kind: `routine`
- Symbol: `fio_exec`

Inputs:
- `A`: FIO command byte.

Outputs:
- `A`: 0 on success, 1 on error.

## FIO.FCLOSE

Close the fileid in FIO_SRCL/H, flushing dirty data first.

- Kind: `routine`
- Symbol: `fio_fclose`

## FIO.FCREATE

Create/truncate FIO.NAME using FIO_DIRTYPE as file access mode; returns fileid in FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_fcreate`

## FIO.FDELETE_EXACT

Delete exact FIO.NAME without BASIC's default .bas behavior.

- Kind: `routine`
- Symbol: `fio_fdelete_exact`

## FIO.FFLUSH

Flush dirty data for fileid FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_fflush`

## FIO.FOPEN

Open FIO.NAME using FIO_DIRTYPE as file access mode; returns fileid in FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_fopen`

## FIO.FREAD

Read from fileid FIO_SRCL/H into RAM at FIO_ENDL/H for FIO_GLENL/H bytes.

- Kind: `routine`
- Symbol: `fio_fread`

## FIO.FRENAME

Rename exact FIO.NAME to the CPU string pointed at by FIO_ENDL/H and FIO_GLENL/H.

- Kind: `routine`
- Symbol: `fio_frename`

## FIO.FRESIZE

Resize fileid FIO_SRCL/H to the low-24-bit size in FIO_SIZEL/H/SIZE2.

- Kind: `routine`
- Symbol: `fio_fresize`

## FIO.FSEEK

Reposition fileid FIO_SRCL/H to the low-24-bit offset in FIO_SIZEL/H/SIZE2.

- Kind: `routine`
- Symbol: `fio_fseek`

## FIO.FSIZE

Return fileid FIO_SRCL/H size in FIO_SIZEL/H/SIZE2.

- Kind: `routine`
- Symbol: `fio_fsize`

## FIO.FSTATUS

Check exact FIO.NAME status; returns implementation fam in FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_fstatus`

## FIO.FTELL

Return fileid FIO_SRCL/H position in FIO_SIZEL/H/SIZE2.

- Kind: `routine`
- Symbol: `fio_ftell`

## FIO.FWRITE

Write FIO_GLENL/H bytes from RAM at FIO_ENDL/H to fileid FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_fwrite`

## FIO.GLOAD

Load VGC/graphics memory using FIO graphics registers.

- Kind: `routine`
- Symbol: `fio_gload`

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

Inputs:
- `A`: FIO command byte.

## FIO.LOAD

Load a BASIC or binary file using FIO.NAME and FIO_SRCL/H.

- Kind: `routine`
- Symbol: `fio_load`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `FIO_SRCL`
- `FIO_SRCH`

## FIO.LOAD_RUNTIME

Load a 16K runtime ROM image named by FIO.NAME/FIO.NAMELEN into the primary runtime ROM bank.

- Kind: `routine`
- Symbol: `fio_load_runtime`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.MKDIR

Create the directory named by FIO.NAME/FIO.NAMELEN.

- Kind: `routine`
- Symbol: `fio_mkdir`

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

## FIO.RMDIR

Remove the directory named by FIO.NAME/FIO_NAMELEN.

- Kind: `routine`
- Symbol: `fio_rmdir`

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## FIO.RNG

Issue FIO.CMD_RNG and return random bytes in FIO_RNG0..3.

- Kind: `routine`
- Symbol: `fio_rng`

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

Outputs:
- `A`: 1 on error.

## NGS.BEGIN_MESSAGE

Begin a request frame and allocate the next non-zero sequence byte.

- Kind: `routine`
- Symbol: `ngs_begin_message`

## NGS.BUILD_AI_ADD_TO_TABLE

Build AI_ADD_TO_TABLE with NGS_TABLE_IDL/H and NGS_VALUE_L/H provider id.

- Kind: `routine`
- Symbol: `ngs_build_ai_add_to_table`

## NGS.BUILD_AI_LIST_REQUEST

Build AI_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_ai_list_request`

## NGS.BUILD_GAME_LIST_REQUEST

Build GAME_LIST_REQUEST.

- Kind: `routine`
- Symbol: `ngs_build_game_list_request`

## NGS.BUILD_HELLO

Build HELLO using NGS_STRPTRL/H:NGS_STRLEN as the requested handle.

- Kind: `routine`
- Symbol: `ngs_build_hello`

## NGS.BUILD_LOBBY_CHAT

Build LOBBY_CHAT from NGS_STRPTRL/H:NGS_STRLEN.

- Kind: `routine`
- Symbol: `ngs_build_lobby_chat`

## NGS.BUILD_OPPONENT_LIST_REQUEST

Build OPPONENT_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_opponent_list_request`

## NGS.BUILD_PING

Build an empty PING request.

- Kind: `routine`
- Symbol: `ngs_build_ping`

## NGS.BUILD_PLAYER_ACTION

Build PLAYER_ACTION with table id, action type, and raw payload bytes.

- Kind: `routine`
- Symbol: `ngs_build_player_action`

## NGS.BUILD_STATE_REQUEST

Build STATE_REQUEST with NGS_TABLE_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_state_request`

## NGS.BUILD_TABLE_CHAT

Build TABLE_CHAT with NGS_TABLE_IDL/H and string text.

- Kind: `routine`
- Symbol: `ngs_build_table_chat`

## NGS.BUILD_TABLE_CREATE

Build TABLE_CREATE using game id/version and NGS_STRPTRL/H table name.

- Kind: `routine`
- Symbol: `ngs_build_table_create`

## NGS.BUILD_TABLE_JOIN

Build TABLE_JOIN with NGS_TABLE_IDL/H and NGS_ROLE.

- Kind: `routine`
- Symbol: `ngs_build_table_join`

## NGS.BUILD_TABLE_LEAVE

Build TABLE_LEAVE with NGS_TABLE_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_table_leave`

## NGS.BUILD_TABLE_LIST_REQUEST

Build TABLE_LIST_REQUEST, optionally filtered by non-zero NGS_GAME_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_table_list_request`

## NGS.BUILD_TABLE_OBSERVE

Build TABLE_OBSERVE with NGS_TABLE_IDL/H.

- Kind: `routine`
- Symbol: `ngs_build_table_observe`

## NGS.BUILD_USER_LIST_REQUEST

Build USER_LIST_REQUEST.

- Kind: `routine`
- Symbol: `ngs_build_user_list_request`

## NGS.CONNECT

Connect NGS_SLOT to NGS_STRPTRL/H:NGS_STRLEN at NGS_PORTL/H.

- Kind: `routine`
- Symbol: `ngs_connect`

## NGS.CONNECT_DEFAULT

Connect NGS_SLOT to the default Nova game-server endpoint.

- Kind: `routine`
- Symbol: `ngs_connect_default`

## NGS.DISCONNECT

Disconnect NGS_SLOT.

- Kind: `routine`
- Symbol: `ngs_disconnect`

## NGS.INIT

Initialize game-server client state and default port 6503.

- Kind: `routine`
- Symbol: `ngs_init`

## NGS.KEEPALIVE_RESET

Reset the keepalive countdown to NGS_KEEPALIVE_INTERVAL_L/H.

- Kind: `routine`
- Symbol: `ngs_keepalive_reset`

## NGS.KEEPALIVE_SET_INTERVAL

Set the client keepalive ping interval used by NGS.KEEPALIVE_TICK.

- Kind: `routine`
- Symbol: `ngs_keepalive_set_interval`

## NGS.KEEPALIVE_TICK

Send periodic PING frames while an app is blocked in UI/input code.

- Kind: `routine`
- Symbol: `ngs_keepalive_tick`

## NGS.PARSE_ENVELOPE

Parse the frame envelope at NGS_BUFL/H using NGS_LAST_LEN.

- Kind: `routine`
- Symbol: `ngs_parse_envelope`

## NGS.PARSE_ERROR

Parse current ERROR payload into NGS_ERROR_CODE and NGS_ERROR_TEXT*.

- Kind: `routine`
- Symbol: `ngs_parse_error`

## NGS.PING_ROUNDTRIP

Send PING and wait for PONG using the shared timeout/error handling.

- Kind: `routine`
- Symbol: `ngs_ping_roundtrip`

## NGS.READY

Return A=1 when NGS_SLOT has a queued message, otherwise A=0.

- Kind: `routine`
- Symbol: `ngs_ready`

## NGS.READ_BYTE

Read one byte from the parsed payload into A.

- Kind: `routine`
- Symbol: `ngs_read_byte`

## NGS.READ_STRING

Read a length-prefixed string pointer into NGS_STRPTRL/H:NGS_STRLEN.

- Kind: `routine`
- Symbol: `ngs_read_string`

## NGS.READ_U16

Read a little-endian u16 into NGS_VALUE_L/H and A/X.

- Kind: `routine`
- Symbol: `ngs_read_u16`

## NGS.RECV_CURRENT

Receive and parse one queued frame from NGS_SLOT into NGS_BUFL/H.

- Kind: `routine`
- Symbol: `ngs_recv_current`

## NGS.SEND_CURRENT

Send the current frame through NGS_SLOT using NIC DMA registers.

- Kind: `routine`
- Symbol: `ngs_send_current`

## NGS.SET_BUFFER

Set the 256-byte CPU RAM frame buffer.

- Kind: `routine`
- Symbol: `ngs_set_buffer`

## NGS.SET_STRING

Set the pointer/length pair used by host, string, and raw payload routines.

- Kind: `routine`
- Symbol: `ngs_set_string`

## NGS.SET_TIMEOUT

Set shared wait timeout in video frames; zero waits indefinitely.

- Kind: `routine`
- Symbol: `ngs_set_timeout`

## NGS.STATUS

Return NGS_SLOT status in A.

- Kind: `routine`
- Symbol: `ngs_status`

## NGS.WAIT_CONNECTED

Wait for NGS_SLOT to connect or fail within NGS_TIMEOUT_L/H frames.

- Kind: `routine`
- Symbol: `ngs_wait_connected`

## NGS.WAIT_KIND

Wait for a non-error frame of the requested kind, skipping unrelated events.

- Kind: `routine`
- Symbol: `ngs_wait_kind`

## NGS.WAIT_MESSAGE

Wait for and receive one frame; protocol ERROR frames are parsed and returned as NGS_ERR_PROTOCOL_ERROR.

- Kind: `routine`
- Symbol: `ngs_wait_message`

## NGS.WRITE_BYTE

Append one payload byte to the current frame.

- Kind: `routine`
- Symbol: `ngs_write_byte`

## NGS.WRITE_BYTES

Append raw bytes from NGS_STRPTRL/H without a length prefix.

- Kind: `routine`
- Symbol: `ngs_write_bytes`

## NGS.WRITE_STRING

Append NGS_STRLEN plus bytes from NGS_STRPTRL/H.

- Kind: `routine`
- Symbol: `ngs_write_string`

## NGS.WRITE_U16

Append a little-endian 16-bit value.

- Kind: `routine`
- Symbol: `ngs_write_u16`

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

Inputs:
- `A`: NIC command byte.

## NIC.CONNECT

Connect NIC_SLOT to NIC_NAMEBUF:NIC_RPORTL/H.

- Kind: `routine`
- Symbol: `nic_connect`

Requires:
- `NIC_SLOT`
- `NIC_NAMEBUF`
- `NIC_RPORTL`
- `NIC_RPORTH`

## NIC.COPY_NAME

Copy a pointer-based host name to NIC_NAMEBUF and append NUL.

- Kind: `routine`
- Symbol: `nic_copy_name`

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

Requires:
- `NIC_SLOT`

## NIC.LENGTH

Read the most recent NIC message length.

- Kind: `routine`
- Symbol: `nic_length`

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

Inputs:
- `X`: Slot number.

Outputs:
- `A`: 1 when ready, 0 otherwise.

## NIC.RECV

Receive data for NIC_SLOT into NIC_DMAL/H.

- Kind: `routine`
- Symbol: `nic_recv`

Requires:
- `NIC_SLOT`
- `NIC_DMAL`
- `NIC_DMAH`

## NIC.SEND

Send NIC_DMALEN bytes from NIC_DMAL/H on NIC_SLOT.

- Kind: `routine`
- Symbol: `nic_send`

Requires:
- `NIC_SLOT`
- `NIC_DMAL`
- `NIC_DMAH`
- `NIC_DMALEN`

## NIC.STATUS

Read clamped NIC slot status.

- Kind: `routine`
- Symbol: `nic_status`

Inputs:
- `X`: Slot number.

Outputs:
- `A`: Slot status byte.

## NVG.ADDRH

High byte of the graphics bitmap destination offset for NVG.LOAD_AT.

- Kind: `u8`
- Symbol: `NVG_ADDRH`
- Address: `$B9AC`

## NVG.ADDRL

Low byte of the graphics bitmap destination offset for NVG.LOAD_AT.

- Kind: `u8`
- Symbol: `NVG_ADDRL`
- Address: `$B9AB`

## NVG.LOAD

Clear graphics bitmap memory, then decode FIO.NAME as an NVG1 file into graphics bitmap offset 0.

- Kind: `routine`
- Symbol: `nvg_load`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`

## NVG.LOAD_AT

Clear graphics bitmap memory, then decode FIO.NAME as an NVG1 file into graphics bitmap offset NVG.ADDRL/H.

- Kind: `routine`
- Symbol: `nvg_load_at`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `FIO_NAME`
- `FIO_NAMELEN`
- `NVG_ADDRL`
- `NVG_ADDRH`

## NVG.LOAD_NAMED

Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then clear graphics bitmap memory and decode the NVG1 file at offset 0.

- Kind: `routine`
- Symbol: `nvg_load_named`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `NVG_NAMELEN`
- `NVG_NAMEPTR_L`
- `NVG_NAMEPTR_H`

## NVG.LOAD_NAMED_AT

Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then clear graphics bitmap memory and decode the NVG1 file at NVG.ADDRL/H.

- Kind: `routine`
- Symbol: `nvg_load_named_at`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `NVG_NAMELEN`
- `NVG_NAMEPTR_L`
- `NVG_NAMEPTR_H`
- `NVG_ADDRL`
- `NVG_ADDRH`

## NVG.NAMELEN

Filename length argument for NVG.LOAD_NAMED.

- Kind: `zp`
- Symbol: `NVG_NAMELEN`
- Address: `$28`

## NVG.NAMEPTR_H

High byte of filename pointer argument for NVG.LOAD_NAMED.

- Kind: `zp`
- Symbol: `NVG_NAMEPTR_H`
- Address: `$2A`

## NVG.NAMEPTR_L

Low byte of filename pointer argument for NVG.LOAD_NAMED.

- Kind: `zp`
- Symbol: `NVG_NAMEPTR_L`
- Address: `$29`

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

## SPRITE.BACKGROUND_CLEAR

Clear the sprite-background collision mask and acknowledge its IRQ source.

- Kind: `routine`
- Symbol: `sprite_background_clear`

Outputs:
- `A`: VGC_IRQ_SPRBG.

## SPRITE.BACKGROUND_IRQ_DISABLE

Disable the VGC sprite-background IRQ source.

- Kind: `routine`
- Symbol: `sprite_background_irq_disable`

## SPRITE.BACKGROUND_IRQ_ENABLE

Clear stale sprite-background collisions, enable the VGC sprite-background IRQ source, and enable CPU IRQs.

- Kind: `routine`
- Symbol: `sprite_background_irq_enable`

## SPRITE.BACKGROUND_MASK

Read the 16-bit sprite-background collision status mask without clearing it.

- Kind: `routine`
- Symbol: `sprite_background_collision_mask`

Outputs:
- `A`: Background collision status high byte for sprites 8-15.
- `Y`: Background collision status low byte for sprites 0-7.

## SPRITE.BACKGROUND_READ_CLEAR

Read the 16-bit sprite-background collision mask, clear it, and acknowledge its IRQ source.

- Kind: `routine`
- Symbol: `sprite_background_read_clear`

Outputs:
- `A`: Background collision status high byte for sprites 8-15.
- `Y`: Background collision status low byte for sprites 0-7.

## SPRITE.BACKGROUND_STATUS

Read the low byte of the sprite-background collision status register.

- Kind: `routine`
- Symbol: `sprite_background_collision_status`

Outputs:
- `A`: Background collision status byte for sprites 0-7.

## SPRITE.CLEAR

Clear the shape data for the sprite in VGC.P0.

- Kind: `routine`
- Symbol: `sprite_clear`

Requires:
- `VGC_P0`

## SPRITE.COLLISION_CLEAR

Clear the sprite-sprite collision mask and acknowledge its IRQ source.

- Kind: `routine`
- Symbol: `sprite_collision_clear`

Outputs:
- `A`: VGC_IRQ_SPRCOLL.

## SPRITE.COLLISION_IRQ_DISABLE

Disable the VGC sprite collision IRQ source.

- Kind: `routine`
- Symbol: `sprite_collision_irq_disable`

## SPRITE.COLLISION_IRQ_ENABLE

Clear stale sprite-sprite collisions, enable the VGC sprite collision IRQ source, and enable CPU IRQs.

- Kind: `routine`
- Symbol: `sprite_collision_irq_enable`

## SPRITE.COLLISION_MASK

Read the 16-bit sprite-sprite collision status mask without clearing it.

- Kind: `routine`
- Symbol: `sprite_collision_mask`

Outputs:
- `A`: Collision status high byte for sprites 8-15.
- `Y`: Collision status low byte for sprites 0-7.

## SPRITE.COLLISION_READ_CLEAR

Read the 16-bit sprite-sprite collision mask, clear it, and acknowledge its IRQ source.

- Kind: `routine`
- Symbol: `sprite_collision_read_clear`

Outputs:
- `A`: Collision status high byte for sprites 8-15.
- `Y`: Collision status low byte for sprites 0-7.

## SPRITE.COLLISION_STATUS

Read the low byte of the sprite-sprite collision status register.

- Kind: `routine`
- Symbol: `sprite_collision_status`

Outputs:
- `A`: Collision status byte for sprites 0-7.

## SPRITE.COMMAND

Issue a raw sprite VGC command without waiting for completion.

- Kind: `routine`
- Symbol: `sprite_command`

Inputs:
- `A`: VGC sprite command byte.

## SPRITE.COPY

Copy sprite shape data from VGC.P0 to VGC.P1.

- Kind: `routine`
- Symbol: `sprite_copy`

Requires:
- `VGC_P0`
- `VGC_P1`

## SPRITE.DISABLE

Disable the sprite in VGC.P0.

- Kind: `routine`
- Symbol: `sprite_disable`

Requires:
- `VGC_P0`

## SPRITE.ENABLE

Enable the sprite in VGC.P0.

- Kind: `routine`
- Symbol: `sprite_enable`

Requires:
- `VGC_P0`

## SPRITE.FLIP

Set sprite flip flags using VGC.P0=sprite and VGC.P1=flags.

- Kind: `routine`
- Symbol: `sprite_flip`

Requires:
- `VGC_P0`
- `VGC_P1`

## SPRITE.GET_X

Read a sprite's 16-bit X coordinate.

- Kind: `routine`
- Symbol: `sprite_get_x`

Inputs:
- `X`: Sprite index.

Outputs:
- `A`: X high byte.
- `Y`: X low byte.

## SPRITE.GET_Y

Read a sprite's Y coordinate.

- Kind: `routine`
- Symbol: `sprite_get_y`

Inputs:
- `X`: Sprite index.

Outputs:
- `Y`: Y coordinate byte.

## SPRITE.POS

Move a sprite using VGC.P0=sprite, VGC.P1/P2=x, VGC.P3=y.

- Kind: `routine`
- Symbol: `sprite_pos`

Requires:
- `VGC_P0`
- `VGC_P1`
- `VGC_P2`
- `VGC_P3`

## SPRITE.PRIORITY

Set sprite priority using VGC.P0=sprite and VGC.P1=priority.

- Kind: `routine`
- Symbol: `sprite_priority`

Requires:
- `VGC_P0`
- `VGC_P1`

## SPRITE.REG_OFFSET

Compute the VGC sprite attribute block offset for a sprite field.

- Kind: `routine`
- Symbol: `sprite_reg_offset`

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.

Outputs:
- `Y`: VGC_SPR_BASE-relative byte offset.

## SPRITE.ROW

Write one sprite bitmap row using VGC.P0..VGC.P9.

- Kind: `routine`
- Symbol: `sprite_row`

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

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.
- `NVR0L`: Low byte.
- `NVR0H`: High byte.

## SPRITE.SET_REG8

Write one 8-bit sprite attribute field.

- Kind: `routine`
- Symbol: `sprite_set_reg8`

Inputs:
- `X`: Sprite index.
- `Y`: Sprite field offset.
- `A`: Field value.

## SPRITE.SET_SHAPE

Set a sprite's shape index.

- Kind: `routine`
- Symbol: `sprite_set_shape`

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

Clear the text screen through the VGC character output register and wait for completion.

- Kind: `routine`
- Symbol: `vgc_cls`

## VGC.COMMAND

Issue a raw VGC command without waiting for completion.

- Kind: `routine`
- Symbol: `vgc_cmd`

Inputs:
- `A`: VGC command byte.

## VGC.DISPLAY_OFF

Blank VGC output while keeping video timing active.

- Kind: `routine`
- Symbol: `vgc_display_off`

## VGC.DISPLAY_ON

Restore full VGC output brightness.

- Kind: `routine`
- Symbol: `vgc_display_on`

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

## VGC.FLASH_ON

Enable flashing text.

- Kind: `routine`
- Symbol: `vgc_flash_on`

## VGC.GCLS

Issue the VGC graphics clear command.

- Kind: `routine`
- Symbol: `vgc_gcls`

## VGC.GCOLOR

Issue the VGC graphics colour command using VGC.P0.

- Kind: `routine`
- Symbol: `vgc_gcolor`

Requires:
- `VGC_P0`

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

## VGC.IRQ_ACK

Acknowledge pending VGC IRQ source bits.

- Kind: `routine`
- Symbol: `vgc_irq_ack`

Inputs:
- `A`: VGC IRQ source mask.

## VGC.IRQ_DISABLE

Disable one or more VGC IRQ source bits.

- Kind: `routine`
- Symbol: `vgc_irq_disable`

Inputs:
- `A`: VGC IRQ source mask.

## VGC.IRQ_ENABLE

Enable one or more VGC IRQ source bits and enable maskable CPU IRQs.

- Kind: `routine`
- Symbol: `vgc_irq_enable`

Inputs:
- `A`: VGC IRQ source mask.

## VGC.IRQ_INSTALL

Install the CPU IRQ vector used when enabled VGC IRQ sources fire.

- Kind: `routine`
- Symbol: `vgc_irq_install`

Inputs:
- `A`: Handler address high byte.
- `Y`: Handler address low byte.

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

Inputs:
- `VGC_P0`: Cursor X column.
- `VGC_P1`: Cursor Y row.

## VGC.MEM_READ

Read one byte from VGC memory using VGC.P0..VGC.P4.

- Kind: `routine`
- Symbol: `vgc_mem_read`

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

## VGC.REVERSE_EXPLICIT

Enable reverse text with explicit packed foreground/background colours.

- Kind: `routine`
- Symbol: `vgc_reverse_explicit`

Inputs:
- `A`: Packed reverse attribute, low nibble foreground and high nibble background.

## VGC.REVERSE_OFF

Disable reverse text.

- Kind: `routine`
- Symbol: `vgc_reverse_off`

## VGC.SET_BG

Set the current text background colour.

- Kind: `routine`
- Symbol: `vgc_set_bg`

Inputs:
- `X`: Background colour byte.

## VGC.SET_BORDER

Set the VGC border colour.

- Kind: `routine`
- Symbol: `vgc_set_border`

Inputs:
- `X`: Border colour byte.

## VGC.SET_FG

Set the current text foreground colour.

- Kind: `routine`
- Symbol: `vgc_set_fg`

Inputs:
- `X`: Foreground colour byte.

## VGC.SET_FONT

Select the active text font slot.

- Kind: `routine`
- Symbol: `vgc_set_font`

Inputs:
- `X`: Font slot, 0..7.

## VGC.SET_MODE

Set the active VGC graphics/text mode.

- Kind: `routine`
- Symbol: `vgc_set_mode`

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

Outputs:
- `A`: Last observed frame counter value.

## VGC.WAIT_COMMAND

Wait for the active VGC command to complete.

- Kind: `routine`
- Symbol: `vgc_wait_cmd`

## VGC.WAIT_FRAMES

Wait A video frames by busy-waiting the frame counter A times.

- Kind: `routine`
- Symbol: `vgc_wait_frames`

Inputs:
- `A`: frame count (0 returns immediately)

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

## VSPRITE.BLIT_START

Start the configured virtual sprite blit without waiting for completion.

- Kind: `routine`
- Symbol: `vsprite_blit_start`

Inputs:
- `VSPRITE_FLAGS`: Set bit 0 to enable color-key transparency.
- `VSPRITE_COLORKEY`: Source byte skipped when color-key mode is enabled.

Outputs:
- `A`: 0 after the blit command is issued.

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

Source byte skipped by color-keyed blits. VSPRITE.ROTATE also uses this value to fill output pixels outside the rotated source bounds.

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

## VSPRITE.GFX_BLIT_START

Start copying the configured virtual sprite to the VGC graphics plane at X/Y without waiting for completion.

- Kind: `routine`
- Symbol: `vsprite_gfx_blit_start`

Outputs:
- `A`: 0 after the blit command is issued.

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

## VSPRITE.GFX_ROTATE_BLIT

Rotate the configured virtual sprite offscreen, wait for the next frame, then copy the full rotated bounds to the graphics plane at X/Y.

- Kind: `routine`
- Symbol: `vsprite_gfx_rotate_blit`

Inputs:
- `VSPRITE_COLORKEY`: Fill value for rotated output pixels outside the source bounds; normally match this to VGC_GFXTRANS.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_ORIGSPACE`
- `VSPRITE_ORIGADDR*`
- `VSPRITE_ORIGSTR*`
- `VSPRITE_ROTSPACE`
- `VSPRITE_ROTADDR*`
- `VSPRITE_ROTSTR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_ROTANGLE`

## VSPRITE.GFX_ROTATE_BLIT_KEYED

Rotate the configured virtual sprite, wait for the next frame, then blit it with the caller's VSPRITE.FLAGS.

- Kind: `routine`
- Symbol: `vsprite_gfx_rotate_blit_keyed`

Inputs:
- `VSPRITE_FLAGS`: Set VSPRITE_FLAG_COLORKEY to preserve existing graphics-plane pixels where the rotated sprite is transparent.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE.X/Y`
- `VSPRITE.ORIG*`
- `VSPRITE.ROT*`
- `VSPRITE.WIDTH/HEIGHT`
- `VSPRITE.ROTANGLE`

## VSPRITE.GFX_ROTATE_BLIT_KEYED_NOWF

Like vsprite_gfx_rotate_blit_keyed but does NOT wait for VGC.FRAME — it

- Kind: `routine`
- Symbol: `vsprite_gfx_rotate_blit_keyed_nowf`

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE.X/Y`
- `VSPRITE.ORIG*`
- `VSPRITE.ROT*`
- `VSPRITE.WIDTH/HEIGHT`
- `VSPRITE.ROTANGLE`

## VSPRITE.GFX_ROTATE_BLIT_NOWAIT

Rotate the configured virtual sprite offscreen, then start copying the full rotated bounds to graphics memory without waiting for VGC.FRAME or final blit completion.

- Kind: `routine`
- Symbol: `vsprite_gfx_rotate_blit_nowait`

Inputs:
- `VSPRITE_COLORKEY`: Fill value for rotated output pixels outside the source bounds; normally match this to VGC_GFXTRANS.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_XL`
- `VSPRITE_XH`
- `VSPRITE_Y`
- `VSPRITE_ORIGSPACE`
- `VSPRITE_ORIGADDR*`
- `VSPRITE_ORIGSTR*`
- `VSPRITE_ROTSPACE`
- `VSPRITE_ROTADDR*`
- `VSPRITE_ROTSTR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_ROTANGLE`

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

## VSPRITE.ORIGADDRH

High byte of the immutable source shape address used by VSPRITE.ROTATE. Use 0 for CPU RAM.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGADDRH`

## VSPRITE.ORIGADDRL

Low byte of the immutable source shape address used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGADDRL`

## VSPRITE.ORIGADDRM

Middle byte of the immutable source shape address used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGADDRM`

## VSPRITE.ORIGSPACE

Blitter memory space for the immutable source shape used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGSPACE`

## VSPRITE.ORIGSTRH

High byte of the immutable source shape row stride used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGSTRH`

## VSPRITE.ORIGSTRL

Low byte of the immutable source shape row stride used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ORIGSTRL`

## VSPRITE.ROTADDRH

High byte of the caller-owned rotated output buffer address. Use 0 for CPU RAM.

- Kind: `u8`
- Symbol: `VSPRITE_ROTADDRH`

## VSPRITE.ROTADDRL

Low byte of the caller-owned rotated output buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_ROTADDRL`

## VSPRITE.ROTADDRM

Middle byte of the caller-owned rotated output buffer address.

- Kind: `u8`
- Symbol: `VSPRITE_ROTADDRM`

## VSPRITE.ROTANGLE

Hardware rotation angle, where 256 steps is one full turn.

- Kind: `u8`
- Symbol: `VSPRITE_ROTANGLE`

## VSPRITE.ROTATE

Rotate the configured square virtual sprite from immutable source into the caller-owned rotated buffer.

- Kind: `routine`
- Symbol: `vsprite_rotate`

Inputs:
- `VSPRITE_COLORKEY`: Fill value for rotated output pixels outside the source bounds.

Outputs:
- `A`: 0 on success, 1 on error.

Requires:
- `VSPRITE_ORIGSPACE`
- `VSPRITE_ORIGADDR*`
- `VSPRITE_ORIGSTR*`
- `VSPRITE_ROTSPACE`
- `VSPRITE_ROTADDR*`
- `VSPRITE_ROTSTR*`
- `VSPRITE_WIDTH*`
- `VSPRITE_HEIGHT*`
- `VSPRITE_ROTANGLE`

## VSPRITE.ROTSPACE

Blitter memory space for the caller-owned rotated output buffer used by VSPRITE.ROTATE.

- Kind: `u8`
- Symbol: `VSPRITE_ROTSPACE`

## VSPRITE.ROTSTRH

High byte of the caller-owned rotated output buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_ROTSTRH`

## VSPRITE.ROTSTRL

Low byte of the caller-owned rotated output buffer row stride.

- Kind: `u8`
- Symbol: `VSPRITE_ROTSTRL`

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

## VSPRITE.USE_ORIGINAL

Point VSPRITE.SRC* at the configured immutable source shape.

- Kind: `routine`
- Symbol: `vsprite_use_original`

Outputs:
- `A`: 0.

Requires:
- `VSPRITE_ORIGSPACE`
- `VSPRITE_ORIGADDR*`
- `VSPRITE_ORIGSTR*`

## VSPRITE.USE_ROTATED

Point VSPRITE.SRC* at the configured rotated output buffer.

- Kind: `routine`
- Symbol: `vsprite_use_rotated`

Outputs:
- `A`: 0.

Requires:
- `VSPRITE_ROTSPACE`
- `VSPRITE_ROTADDR*`
- `VSPRITE_ROTSTR*`

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
