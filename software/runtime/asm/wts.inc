; =====================================================================
;  Nova NDK — wts.inc
;
;  Nova wavetable-synth (WTS) note API — public declarations. Async,
;  voice-tracked one-shot notes on the 8 WTS voices ($A140-$A17F),
;  mirroring the SID audio_*_async model: wts_note_async allocates a FREE
;  voice and returns immediately; wts_tick releases it after its frame
;  count elapses.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef WTS_API_INCLUDED
WTS_API_INCLUDED = 1

.include "nova.inc"        ; WTS_* registers + NVR0 scratch pointer

.global wts_init
.global wts_note_on
.global wts_note_async
.global wts_note_off
.global wts_tick

.endif
