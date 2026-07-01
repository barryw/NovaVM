# Nova Game Server

Generic TCP game server for Nova clients. The server speaks the same
1-byte-length-prefixed framing used by the Nova NIC and carries a compact binary
message envelope inside each frame.

Built-in game adapters currently include:

- `echo` (`gameId=1`) for protocol bring-up.
- `chess` (`gameId=2`) for Nova Chess transport. The first version is
  intentionally move-log based: it records and broadcasts chess move payloads
  and exposes observer state.

Built-in AI providers currently include:

- `stockfish` (`providerId=1`) for chess tables. Set `STOCKFISH_PATH` if the
  executable is not on `PATH`; `STOCKFISH_MOVETIME_MS` defaults to `500`.
  The provider starts a separate UCI process per AI move, so independent tables
  can make progress without sharing Stockfish process state.

Run locally:

```sh
dotnet run --project e6502.GameServer -- --port 6503
```

Run with Docker Compose:

```sh
docker compose up --build nova-game-server
```

The default port is `6503`; override it with `--port <port>` or
`NOVA_GAME_PORT`. The Docker image installs Stockfish and points
`STOCKFISH_PATH` at `/usr/games/stockfish`. Nova clients can connect through
the shared `nova-game-server` endpoint alias; Avalonia maps that alias to
`NOVA_GAME_SERVER_HOST` / `NOVA_GAME_SERVER_PORT` or localhost by default.

## Wire Format

Each TCP message uses the Nova NIC framing byte first: payload length in one
byte, with `0` meaning 256 bytes. The framed payload starts with a four-byte
game-server envelope:

| Offset | Field |
| --- | --- |
| 0 | Server protocol major |
| 1 | Message kind |
| 2 | Sequence |
| 3 | Flags |
| 4+ | Message payload |

Payload integers are little-endian. Strings are one-byte length plus UTF-8/ASCII
bytes. One NIC frame can carry 252 bytes of message payload after the envelope.

Nova assembly clients should use `software/runtime/asm/gameserver.inc` and
`software/runtime/asm/gameserver.s`. That layer wraps `software/runtime/asm/nic.s`, builds request frames,
parses envelopes, and leaves game-specific payload bytes to each game's own
adapter library.

Opponent discovery is typed. `OPPONENT_LIST_REQUEST` returns
`OPPONENT_LIST_ITEM` payloads shaped as
`[type, id, gameId, gameMajor, gameMinor, capabilities, code, displayName]`,
where `type=0` is a human session and `type=1` is an AI provider. Existing
`AI_LIST_REQUEST` remains available when a client explicitly wants provider
metadata only.

Minimal client flow:

```asm
.include "gameserver.inc"

        jsr ngs_init
        lda #<frame_buffer
        ldx #>frame_buffer
        jsr ngs_set_buffer

        ; Connect to the shared default endpoint alias.
        jsr ngs_connect_default
        jsr ngs_wait_connected

        ; HELLO: NGS_STRPTRL/H:NGS_STRLEN points at the requested handle.
        jsr ngs_build_hello
        jsr ngs_send_current

        ; Wait for the expected response or a parsed protocol error.
        lda #NGS_KIND_WELCOME
        jsr ngs_wait_kind
```
