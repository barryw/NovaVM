---
title: "Networking"
type: guide
category: Networking
---

> The value of a network grows with the number of machines connected to it.

-- Robert Metcalfe

NovaBASIC includes a built-in Network Interface Controller (NIC) that gives your
programs full TCP networking from ordinary BASIC lines. You can open outbound
connections to remote servers, listen for incoming connections, send and receive
text messages, and build complete client/server applications -- all without
touching a single hardware register.

The NIC provides four independent connection **slots** numbered 0 through 3.
Each slot can operate as either a client (connecting out to a remote host) or a
server (listening for incoming connections). Slots are fully independent: you can
have a client on slot 0 and a server on slot 1 at the same time. Messages are
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
- **Client or server per slot** -- a slot is committed to one role
per connection but can switch role after being closed.
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

### Length of last received message: `NLEN`

After a successful `NRECV$` call, `NLEN` returns the byte length of the
message that was just dequeued. This is convenient when you need to know the
exact payload size without calling `LEN` on the returned string.

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

## Running a Server

A server slot listens for incoming TCP connections on a local port. The workflow
is slightly different from the client case because you must first bind the port,
then wait for and accept a connection, before the slot behaves like a normal
send/receive channel.

### Binding a port: `NLISTEN slot, port`

```basic
NLISTEN 0, 8080
```

Begins listening for incoming connections on *port* (bound to
`127.0.0.1`) using *slot*. The command returns immediately; the NIC
starts accepting the OS-level TCP listen in the background. A client connecting
to this port will be queued until you call `NACCEPT`.

### Accepting a connection: `NACCEPT slot`

```basic
NACCEPT 0
```

Promotes a pending incoming connection on *slot* from the listen queue to
an active connected state. You must poll `NREADY(slot)` before calling
`NACCEPT` -- `NREADY` returns `-1` when a client is waiting to
be accepted. After `NACCEPT` returns, the slot is fully connected and you
can use `NSEND` and `NRECV$` normally.

::: note
`NREADY` serves double duty on a listening slot: before `NACCEPT` it
signals a pending connection, and after `NACCEPT` it signals a pending
message. The meaning is determined by the slot's current state.
:::

### Server loop pattern

A typical server follows this sequence:

1. `NLISTEN` to bind the port.
2. Poll `NREADY` waiting for a client to connect.
3. `NACCEPT` to bring the connection live.
4. Inner loop: poll `NREADY`, call `NRECV$`, process, `NSEND`
reply. Exit when the connection drops.
5. `NCLOSE` the slot.
6. Go back to step 2 to accept the next client.

### Echo server example

```basic
10 REM -- Echo Server --
20 NLISTEN 0, 8080
30 PRINT "Listening on port 8080..."
40 IF NOT NREADY(0) THEN 40
50 NACCEPT 0
60 PRINT "Client connected!"
70 REM Main receive/echo loop
80 IF (NSTATUS(0) AND 1) = 0 THEN 200
90 IF NOT NREADY(0) THEN 80
100 A$ = NRECV$(0)
110 L = NLEN
120 PRINT "Received ("; L; " bytes): "; A$
130 NSEND 0, A$
140 GOTO 80
200 PRINT "Client disconnected."
210 NCLOSE 0
220 GOTO 30
```

Line 20 binds port 8080. Lines 30--40 wait for a client. Line 50 accepts it.
The inner loop at lines 80--140 checks that the connection is still alive (bit 0
of `NSTATUS`), polls for incoming data, reads each message, prints the
length and content, and echoes it straight back. When the connection drops,
execution falls through to line 200, the slot is closed, and the server loops
back to listen for the next client.

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
`(NSTATUS(slot) AND 1) = 0` as your primary disconnection test, as shown
in the echo server example above.

### Cleaning up after disconnection

Regardless of which party initiated the close, always call `NCLOSE slot`
once you detect a disconnection. This returns the slot to the idle state and
allows `NLISTEN` or `NOPEN` to be called on it again.

::: warning
Always check `NSTATUS` before calling `NSEND`. If bit 0 (*Connected*)
is clear, the connection has dropped and `NSEND` will fail silently -- your
message will be discarded without any error being reported to BASIC. Test the
connection state at the top of every send/receive loop.
:::

## Try It Now

::: tryit
Type and run the echo server program from Section \refsec:nic-server:

```basic
10 REM -- Echo Server --
20 NLISTEN 0, 8080
30 PRINT "Listening on port 8080..."
40 IF NOT NREADY(0) THEN 40
50 NACCEPT 0
60 PRINT "Client connected!"
70 REM Main receive/echo loop
80 IF (NSTATUS(0) AND 1) = 0 THEN 200
90 IF NOT NREADY(0) THEN 80
100 A$ = NRECV$(0)
110 L = NLEN
120 PRINT "Received ("; L; " bytes): "; A$
130 NSEND 0, A$
140 GOTO 80
200 PRINT "Client disconnected."
210 NCLOSE 0
220 GOTO 30
```

The server will print `LISTENING ON PORT 8080...` and wait. To test it,
open a second emulator instance and enter the client program below, then type
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
window should print `ECHO: HELLO FROM CLIENT!`. After the client closes
the connection the server will print `CLIENT DISCONNECTED.` and return
to listening -- ready for the next client.

**Experiments to try:**

- Modify the echo server to prefix every reply with `"ECHO: "` before
sending it back.
- Change the server to count messages received and print the count alongside
each one.
- Open slot 1 in the client program with a second `NOPEN` call and send a
different message on each slot simultaneously to observe independent
operation.
- Extend the client into a loop that sends ten numbered messages and prints
each echo, then closes the connection.

:::
