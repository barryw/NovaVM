# Network Controller (NIC) — Design

## Goal

Add message-oriented TCP networking to the virtual machine, enabling BASIC programs to connect to remote servers, send and receive messages, and build multiplayer networked games. The C# host handles all TCP/IP, DNS, framing, and buffering. The 6502 sees a clean register-based command interface.

## Architecture

The NIC sits at `$A100-$A13F` (64 bytes) following the standard controller pattern: `OwnsAddress` / `Read` / `Write` with command dispatch. It receives `busRead` / `busWrite` DMA delegates like FileIoController.

### Core Concepts

- **Slots (0-3):** 4 independent TCP connections. Commands take a slot ID parameter.
- **Messages:** The unit of communication. Complete messages, not byte streams. The host frames TCP data automatically.
- **DMA transfer:** Payloads copied between network buffers and 6502 RAM via address+length registers.
- **Notifications:** Polling via status registers, with optional IRQ on data arrival.
- **Max message size:** 256 bytes. Length register is a single byte (0 = 256).

### Wire Protocol

Between emulator instances or game servers, the TCP stream uses:

```
[1 byte: payload length N] [N bytes: payload]
```

Length 0 means 256 bytes. Handled entirely by the C# host.

---

## Register Map — $A100-$A13F

```
$A100  NicCmd           Command register (write triggers execution)
$A101  NicStatus        Global status (read)
$A102  NicSlot          Active slot ID (0-3)
$A103  NicIrqCtrl       IRQ enable mask (bit per slot)
$A104  NicIrqStatus     IRQ pending flags (bit per slot, read-clears)

$A108  NicRemotePort_L  Remote port low byte
$A109  NicRemotePort_H  Remote port high byte
$A10A  NicLocalPort_L   Local port low byte (for listen)
$A10B  NicLocalPort_H   Local port high byte

$A110  NicDmaAddr_L     DMA RAM address low byte
$A111  NicDmaAddr_H     DMA RAM address high byte
$A112  NicDmaLen        DMA length (1-255, 0 = 256)
$A113  NicMsgLen        Received message length (read-only)

$A118  NicSlotStatus_0  Slot 0 status
$A119  NicSlotStatus_1  Slot 1 status
$A11A  NicSlotStatus_2  Slot 2 status
$A11B  NicSlotStatus_3  Slot 3 status

$A120  NicNameBuf       Hostname string buffer (32 bytes, null-terminated)
 ...
$A13F  (end of name buffer)
```

### Slot Status Byte

```
Bit 0: Connected
Bit 1: Data available (message waiting in receive queue)
       — In listen state: pending connection waiting
Bit 2: Send ready (previous send complete)
Bit 3: Error occurred
Bit 4: Connection closed by remote
```

### Global Status Byte (NicStatus)

```
Bit 0: Controller ready
Bit 1: Any slot has data waiting
Bit 7: Error on any slot
```

### Commands

```
$01  NicConnect     Connect slot to hostname:port
$02  NicDisconnect  Close slot connection
$03  NicSend        Send message from DMA addr/len
$04  NicRecv        Receive message into DMA addr/len
$05  NicListen      Listen on local port
$06  NicAccept      Accept pending connection into slot
```

---

## Command Flow — Assembly

### Connect and Send

```asm
; Connect slot 0 to "game.nova.net" port 6502
LDA #0
STA NicSlot             ; select slot 0
LDA #<6502
STA NicRemotePort_L
LDA #>6502
STA NicRemotePort_H
; Store hostname at NicNameBuf ($A120)
; ... write "game.nova.net",0 ...
LDA #$01
STA NicCmd              ; NicConnect

; Poll until connected
.wait LDA NicSlotStatus_0
      AND #$01
      BEQ .wait

; Send 10-byte message from $2000
LDA #<$2000
STA NicDmaAddr_L
LDA #>$2000
STA NicDmaAddr_H
LDA #10
STA NicDmaLen
LDA #$03
STA NicCmd              ; NicSend
```

### Receive

```asm
; Poll for data on slot 0
.poll LDA NicSlotStatus_0
      AND #$02
      BEQ .poll

; Receive into $2080
LDA #<$2080
STA NicDmaAddr_L
LDA #>$2080
STA NicDmaAddr_H
LDA #$04
STA NicCmd              ; NicRecv
LDA NicMsgLen           ; actual received length
```

### IRQ-Driven Receive

```asm
LDA #$01                ; enable IRQ for slot 0
STA NicIrqCtrl

; In IRQ handler:
LDA NicIrqStatus        ; read-clears
AND #$01
BEQ .not_slot0
; ... do receive ...
.not_slot0
```

---

## BASIC Keywords

### Connection Management

```
NOPEN slot, "hostname", port      Connect slot to remote host
NCLOSE slot                       Disconnect slot
NLISTEN slot, port                Listen for incoming connections
NACCEPT slot                      Accept pending connection
```

### Data Transfer

```
NSEND slot, A$                    Send string
NSEND slot, addr, len             Send from RAM address (advanced)

A$ = NRECV$(slot)                 Receive as string (returns BASIC string)
NRECV slot, addr                  Receive into RAM address (advanced)
```

### Status

```
NSTATUS(slot)                     Returns slot status byte
NREADY(slot)                      Returns 1 if message waiting, 0 if not
NLEN                              Length of last received message
```

### BASIC Example

```basic
10 NOPEN 0,"game.nova.net",6502
20 IF NSTATUS(0) AND 1 = 0 THEN 20
30 NSEND 0,"HELLO"
40 IF NOT NREADY(0) THEN 40
50 PRINT NRECV$(0)
60 NCLOSE 0
```

---

## C# Implementation

### VirtualNetworkController

Standard controller pattern in `e6502.Avalonia/Hardware/VirtualNetworkController.cs`:

- `OwnsAddress($A100-$A13F)` / `Read` / `Write` with command dispatch on `NicCmd`
- Internal `byte[64]` register array
- `Func<ushort, byte> busRead` and `Action<ushort, byte> busWrite` delegates for DMA
- 4x `ConnectionSlot` objects, each with:
  - `TcpClient` / `NetworkStream`
  - `ConcurrentQueue<byte[]>` receive queue
  - Connection state flags mapped to slot status byte
- `IrqPending` bool, checked by main CPU loop

### Threading Model

- **Receive:** Each active connection gets a background read task. Reads 1-byte length prefix, then payload, enqueues complete message, updates slot status bits. Sets `IrqPending` if IRQ enabled for that slot.
- **Send:** Synchronous from 6502 perspective. `NicSend` DMA-reads payload from RAM, writes length-prefixed frame to `NetworkStream`, returns. At 256 bytes max this is effectively instant.
- **Connect:** `NicConnect` reads hostname from `NicNameBuf`, calls `Dns.GetHostAddressesAsync`, connects `TcpClient`. The 6502 polls `NicSlotStatus` until connected or error.

### CompositeBusDevice Integration

```csharp
// New field
private readonly VirtualNetworkController _nic;

// In Read():
if (_nic.OwnsAddress(address)) return _nic.Read(address);

// In Write():
if (_nic.OwnsAddress(address)) { _nic.Write(address, data); return; }

// In CPU loop, alongside timer IRQ check:
if (_nic.IrqPending && !_cpu.IrqDisabled)
    _cpu.IrqWaiting = true;
```

### TCP Server Commands

| TCP Command | Parameters | Maps To |
|---|---|---|
| `nic_connect` | slot, host, port | NicConnect |
| `nic_disconnect` | slot | NicDisconnect |
| `nic_send` | slot, data (base64) | NicSend |
| `nic_recv` | slot | NicRecv → returns base64 |
| `nic_listen` | slot, port | NicListen |
| `nic_accept` | slot | NicAccept |
| `nic_status` | slot | Read slot status |

MCP tools in `EmulatorTools` wrap these as `[McpServerTool]` methods.

---

## Error Handling

**Connection failures:** DNS failure, connection refused, timeout → sets error bit (bit 3) in slot status. Slot does not transition to connected. No exception surfaces to the 6502.

**Remote disconnect:** Background read task detects stream closed → sets bit 4 (closing), clears bit 0 (connected). Queued messages remain readable. Next `NRECV` after queue drains returns length 0.

**Send when not connected:** No-op, sets error bit.

**Receive when no data:** No-op, `NicMsgLen` set to 0.

**Buffer overflow:** Receive queue capped at 16 messages. Oldest dropped on overflow. Error bit set briefly to signal overflow.

**Slot reuse:** `NicDisconnect` tears down TCP connection, clears receive queue, resets all status bits. Slot immediately available.

**Listen/Accept:**
- `NicListen` binds a `TcpListener` on the host. Slot status bit 1 repurposed to mean "pending connection" in listen state.
- `NicAccept` takes pending connection, transitions to connected. No pending connection → no-op.
- One pending connection per listening slot. Additional attempts refused by host.
