---
title: "Networking"
type: guide
category: Networking
---

> The value of a network grows with the number of machines connected to it.

-- Robert Metcalfe

NovaBASIC includes a built-in Network Interface Controller (NIC) that gives your
programs TCP client networking from ordinary BASIC lines. You can open outbound
connections to remote servers, send and receive text messages, and close the
connection -- all without touching a single hardware register.

The NIC provides four independent connection **slots** numbered 0 through 3.
Each slot can connect to a remote host independently. Messages are
automatically framed so that each `NSEND` call corresponds to exactly one
`NRECV$` call on the other end.

## The Network Controller

The NIC implements message-oriented TCP networking. Rather than giving you a raw
byte stream, it packages each message with a one-byte length prefix on the wire.
This framing is invisible to BASIC: you call `NSEND` with a string and the
receiver calls `NRECV$` to get that same string back intact. You never
manually prefix or parse message lengths.

Key characteristics of the NIC:

- **Four independent slots** (0--3), each with its own connection state.
- **Message queuing** -- up to 16 incoming messages may be queued per
slot before the sender is blocked at the hardware level.
- **Maximum message size** of 256 bytes per send/receive.
- **DMA-backed transfers** between NIC and 6502 RAM; BASIC hides this
from you entirely.

The NIC registers occupy `A100`--`A13F`. You will not normally
write to those addresses directly -- the BASIC networking commands issue all
necessary register writes on your behalf.

::: note
The NIC handles all TCP details: handshake, buffering, framing, and teardown.
From BASIC's point of view you simply send strings and receive strings. The
length-prefix framing on the wire is automatic and transparent.
:::

## Connecting as a Client

A client connection opens a TCP socket to a remote host. All four NIC commands
used for client operation -- `NOPEN`, `NSEND`, `NRECV$`, and
`NCLOSE` -- work with the same slot number from open to close.

### Opening a connection: `NOPEN slot, "hostname", port`

```basic
NOPEN 0, "127.0.0.1", 8080
```

Establishes a TCP connection on *slot* (0--3) to *hostname* at
*port* (1--65535). The hostname must be a quoted string; dotted-decimal
IPv4 addresses are the most reliable form. The command blocks briefly while the
TCP handshake completes; once it returns, the slot is ready to send and receive.

### Sending data: `NSEND slot, "message"`

```basic
NSEND 0, "Hello, server!"
```

Transmits *message* through *slot*. The NIC automatically adds the
one-byte length prefix before sending; the application on the far end sees only
the payload. Maximum payload length is 256 bytes.

### Receiving data: `NRECV$(slot)`

```basic
A$ = NRECV$(0)
```

Dequeues and returns the next waiting message on *slot* as a string. If no
message is queued, `NRECV$` returns an empty string immediately -- it does
not wait. Always poll with `NREADY` first to confirm a message is available
before calling `NRECV$`.

### Checking for messages: `NREADY(slot)`

`NREADY(slot)` returns `-1` (true) if at least one message is waiting
in the slot's receive queue, or `0` (false) if the queue is empty. Use it
in a tight polling loop:

```basic
40 IF NOT NREADY(0) THEN 40
```

### Closing a connection: `NCLOSE slot`

```basic
NCLOSE 0
```

Closes the TCP connection on *slot* and releases all associated resources.
Always call `NCLOSE` when you are finished with a slot, even if the remote
end has already disconnected. This is required to return the slot to a clean
state so it can be reused.

### Complete client example

```basic
10 REM -- Simple TCP Client --
20 NOPEN 0, "127.0.0.1", 8080
30 NSEND 0, "Hello, server!"
40 IF NOT NREADY(0) THEN 40
50 A$ = NRECV$(0)
60 PRINT "Reply: "; A$
70 NCLOSE 0
```

Line 20 connects to a server running on the same machine at port 8080.
Line 30 sends a greeting. Line 40 polls until a reply arrives. Line 50 reads
the reply into `A$`. Line 60 prints it. Line 70 closes the slot.

::: tip
`NREADY` and the polling loop on line 40 are the standard pattern for
waiting on a response. Because NovaBASIC is single-threaded, the loop will
spin tightly until data arrives. For interactive programs you can add a
`VSYNC` inside the loop to yield to the display system and keep the
screen responsive while waiting.
:::

## Status and Error Handling

### `NSTATUS(slot)` bitmask

`NSTATUS(slot)` returns a byte whose bits describe the current state of
*slot*:

| **Bit** | **Name** | **Meaning** |
| --- | --- | --- |
| 0 | Connected | An active TCP connection exists on this slot. |
| 1 | DataReady | At least one message is waiting in the receive queue. |
| 2 | SendReady | The slot is ready to accept an outbound message (normally always set when Connected). |
| 3 | Error | A connection or I/O error has occurred on this slot. |
| 4 | RemoteClosed | The remote peer has closed its end of the connection. |

### Testing individual bits

Use the `AND` operator to isolate a specific bit:

```basic
REM Test bit 0 (Connected)
IF (NSTATUS(0) AND 1) = 0 THEN PRINT "Disconnected"

REM Test bit 4 (RemoteClosed)
IF (NSTATUS(0) AND 16) <> 0 THEN GOSUB 900
```

### Detecting disconnection

There are two ways the remote end can disappear:

- **Clean close** -- the remote side called a close equivalent. Bit 4
(*RemoteClosed*) is set and bit 0 (*Connected*) clears.
- **Abrupt drop** -- the network connection is lost without a
graceful close. Bit 3 (*Error*) may be set; bit 0 clears.

In either case, when bit 0 drops to zero the connection is gone. Check
`(NSTATUS(slot) AND 1) = 0` as your primary disconnection test.

### Cleaning up after disconnection

Regardless of which party initiated the close, always call `NCLOSE slot`
once you detect a disconnection. This returns the slot to the idle state and
allows `NOPEN` to be called on it again.

::: warning
Always check `NSTATUS` before calling `NSEND`. If bit 0 (*Connected*)
is clear, the connection has dropped and `NSEND` will fail silently -- your
message will be discarded without any error being reported to BASIC. Test the
connection state at the top of every send/receive loop.
:::

## Try It Now

::: tryit
Start a TCP echo server on another machine, then enter this client and type
`RUN`:

```basic
10 REM -- Test Client --
20 NOPEN 0, "127.0.0.1", 8080
30 NSEND 0, "Hello from client!"
40 IF NOT NREADY(0) THEN 40
50 A$ = NRECV$(0)
60 PRINT "Echo: "; A$
70 NCLOSE 0
```

The server window should print the message and its byte length. The client
window should print `ECHO: HELLO FROM CLIENT!`.

**Experiments to try:**

- Open slot 1 in the client program with a second `NOPEN` call and send a
different message on each slot simultaneously to observe independent
operation.
- Extend the client into a loop that sends ten numbered messages and prints
each echo, then closes the connection.

:::
