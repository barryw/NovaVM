; Shared Nova NIC helper routines.

.include "nic.inc"

.ifndef NIC_IMPLEMENTATION_INCLUDED
NIC_IMPLEMENTATION_INCLUDED = 1

.ifndef NIC_SERVER_COMMANDS
NIC_SERVER_COMMANDS = 1
.endif

      .segment "CODE"

      .export nic_command
      .export nic_connect
      .export nic_disconnect
.if NIC_SERVER_COMMANDS
      .export nic_listen
      .export nic_accept
.endif
      .export nic_send
      .export nic_recv
      .export nic_status
      .export nic_ready
      .export nic_length
      .export nic_copy_name

; Input: A = NIC command.
; @label NIC.COMMAND
; @kind routine
; @symbol nic_command
; @summary Issue a raw NIC command.
; @in A: NIC command byte.
nic_command:
      STA   NIC_CMD
      RTS

; @label NIC.CONNECT
; @kind routine
; @symbol nic_connect
; @summary Connect NIC_SLOT to NIC_NAMEBUF:NIC_RPORTL/H.
; @requires NIC_SLOT NIC_NAMEBUF NIC_RPORTL NIC_RPORTH
nic_connect:
      LDA   #NIC_CMD_CONNECT
      BRA   nic_command

; @label NIC.DISCONNECT
; @kind routine
; @symbol nic_disconnect
; @summary Disconnect NIC_SLOT.
; @requires NIC_SLOT
nic_disconnect:
      LDA   #NIC_CMD_DISCONNECT
      BRA   nic_command

.if NIC_SERVER_COMMANDS
; @label NIC.LISTEN
; @kind routine
; @symbol nic_listen
; @summary Listen on NIC_SLOT using NIC_LPORTL/H.
nic_listen:
      LDA   #NIC_CMD_LISTEN
      BRA   nic_command

; @label NIC.ACCEPT
; @kind routine
; @symbol nic_accept
; @summary Accept a pending connection for NIC_SLOT.
nic_accept:
      LDA   #NIC_CMD_ACCEPT
      BRA   nic_command
.endif

; @label NIC.SEND
; @kind routine
; @symbol nic_send
; @summary Send NIC_DMALEN bytes from NIC_DMAL/H on NIC_SLOT.
; @requires NIC_SLOT NIC_DMAL NIC_DMAH NIC_DMALEN
nic_send:
      LDA   #NIC_CMD_SEND
      BRA   nic_command

; @label NIC.RECV
; @kind routine
; @symbol nic_recv
; @summary Receive data for NIC_SLOT into NIC_DMAL/H.
; @requires NIC_SLOT NIC_DMAL NIC_DMAH
nic_recv:
      LDA   #NIC_CMD_RECV
      BRA   nic_command

; Input: X = slot.  Returns A = clamped slot status.
; @label NIC.STATUS
; @kind routine
; @symbol nic_status
; @summary Read clamped NIC slot status.
; @in X: Slot number.
; @out A: Slot status byte.
nic_status:
      TXA
      AND   #$03
      TAX
      LDA   NIC_SLOTST0,X
      RTS

; Input: X = slot.  Returns A = 1 if data/accept is ready, otherwise 0.
; @label NIC.READY
; @kind routine
; @symbol nic_ready
; @summary Return 1 when NIC_SLOT has data/accept readiness, otherwise 0.
; @in X: Slot number.
; @out A: 1 when ready, 0 otherwise.
nic_ready:
      JSR   nic_status
      AND   #NIC_ST_DATAREADY
      BEQ   @not_ready
      LDA   #$01
@not_ready:
      RTS

; @label NIC.LENGTH
; @kind routine
; @symbol nic_length
; @summary Read the most recent NIC message length.
; @out A: Message length byte.
nic_length:
      LDA   NIC_MSGLEN
      RTS

; Copy a host name from NIC_ARG_NAMEPTR_L/H into NIC_NAMEBUF and append NUL.
; Returns A=0 on success, A=1 on invalid length.
; @label NIC.COPY_NAME
; @kind routine
; @symbol nic_copy_name
; @summary Copy a pointer-based host name to NIC_NAMEBUF and append NUL.
; @requires NIC_ARG_NAMELEN NIC_ARG_NAMEPTR_L NIC_ARG_NAMEPTR_H
; @out A: 0 on success, 1 on invalid length.
nic_copy_name:
      LDA   NIC_ARG_NAMELEN
      BEQ   @bad
      CMP   #NIC_NAME_LIMIT + 1
      BCS   @bad
      TAX
      LDY   #$00
@copy:
      LDA   (NIC_ARG_NAMEPTR_L),Y
      STA   NIC_NAMEBUF,Y
      INY
      DEX
      BNE   @copy
      LDA   #$00
      STA   NIC_NAMEBUF,Y
      LDA   #NIC_RESULT_OK
      RTS
@bad:
      LDA   #NIC_RESULT_ERROR
      RTS

.endif
