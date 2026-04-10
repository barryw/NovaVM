# Integration Testing Design — sim6502 + e6502

> **Goal:** Run BASIC and assembly programs against the live e6502 emulator via sim6502's test framework, asserting screen output, memory state, and hardware registers.

**Architecture:** NovaVM backend for sim6502 connects to the Avalonia emulator's TCP server (port 6502). Two interface levels: `IExecutionBackend` for assembly-level testing (peek/poke/jsr), `IHighLevelBackend` for BASIC-level testing (send text, read screen, wait for prompts). Same test files will eventually run against FPGA simulation via a Verilator backend.

**Tech Stack:** sim6502 (C# .NET 10, ANTLR4 DSL), e6502.Avalonia TCP server (JSON over TCP)

---

## Interfaces

### IHighLevelBackend (new, extends IExecutionBackend)

```csharp
public interface IHighLevelBackend
{
    void SendText(string text);
    void SendKey(string key);
    string[] ReadScreen();
    string ReadLine(int row);
    (int x, int y) GetCursor();
    void WaitForText(string text, int timeoutMs);
    void ColdStart();
    void Pause();
    void Resume();
    void RunCycles(int count);
    void WaitForMemory(int addr, byte value, int timeoutMs);
}
```

### NovaVmBackend

Implements both `IExecutionBackend` and `IHighLevelBackend`. Connects via TCP to the Avalonia emulator. Each method maps to one TCP JSON command:

| Method | TCP Command |
|--------|-------------|
| `ReadByte(addr)` | `peek` |
| `WriteByte(addr, val)` | `poke` |
| `SendText(text)` | `type_text` |
| `ReadScreen()` | `read_screen` |
| `WaitForText(text, ms)` | `wait_ready` |
| `ColdStart()` | `cold_start` |
| `Pause()` | `pause` (new) |
| `Resume()` | `resume` (new) |
| `RunCycles(n)` | `run_cycles` (new) |
| `WaitForMemory(addr, val, ms)` | `watch` (new) |

Test isolation: `ColdStart()` before each test suite (or test, configurable).

---

## DSL Extensions

### New commands (require IHighLevelBackend)

```
system(novavm)                          ; selects NovaVM backend

basic("10 PRINT X")                     ; queue BASIC line + ENTER
run()                                   ; send RUN + wait for Ready
run(wait = "DONE")                      ; send RUN + wait for custom text
wait_ready()                            ; wait for Ready prompt
wait_text("LOADING", timeout = 5000)    ; wait for arbitrary text
send_key("CTRL-C")                      ; special keys
cold_start()                            ; reboot to clean state

pause(cycles = 50000)                   ; run N cycles then freeze
pause(screen = "LOADING")               ; freeze when text appears
pause(watch = $A010, value = $00)       ; freeze on memory condition
resume()                                ; continue execution
```

### New assertion functions

```
assert(screen_contains("HELLO"), "text on screen")
assert(screen_line(0) == "Ready", "first line")
assert(!screen_contains("ERROR"), "no errors")
```

### Error handling

Commands that require `IHighLevelBackend` emit a clear error if the backend is `sim` or `vice`:
```
Error: 'basic()' requires a high-level backend (novavm). Current backend: sim
```

---

## TCP Server Extensions (e6502.Avalonia)

New commands needed in EmulatorTcpServer.cs:

| Command | Behavior |
|---------|----------|
| `pause` | Signal CPU thread to stop. Returns when paused. |
| `resume` | Signal CPU thread to continue. Returns immediately. |
| `run_cycles` | Execute exactly N cycles, then pause. `{"cmd":"run_cycles","cycles":50000}` |
| `watch` | Pause when `mem[addr] == value`. `{"cmd":"watch","address":40976,"value":0,"timeout":5000}` |

The CPU already runs in a background thread. Pausing = setting a flag the execution loop checks. Resuming = clearing the flag and signaling the thread.

---

## Test Suite Structure

```
e6502/tests/integration/
    vgc.6502          ; graphics: PLOT, LINE, FILL, CIRCLE, MODE, GCOLOR
    blitter.6502      ; BLITCOPY, BLITFILL, BLITSTATUS, color-key
    copper.6502       ; COPPERADD, COPPERON/OFF, multi-list
    xram.6502         ; STASH, FETCH, named blocks, XMEM
    tiles.6502        ; tile definition, nametable, scrolling, Mode 4
    text.6502         ; PRINT, cursor, scroll, HOME, CLS
    sprites.6502      ; sprite define, position, flip, collision
    math.6502         ; BASIC arithmetic, string ops, control flow
```

Each file contains 5-20 small tests. Each test flexes one command or one interaction. Total: ~100 integration tests.

---

## Example Test

```
suites {
  suite("VGC Graphics") {
    system(novavm)

    test("plot-pixel", "PLOT writes correct pixel to gfx RAM") {
      basic("10 MODE 3")
      basic("20 GCOLOR 5")
      basic("30 PLOT 100,50")
      run()
      ; gfx RAM pixel at (100,50) = row*320+col = 16100 = $3EE4
      ; VGC gfx is 4-bit, accessed via peek of gfx space
      assert(screen_contains("Ready"), "program completed")
    }

    test("fill-rect", "FILL draws solid rectangle") {
      basic("10 MODE 3")
      basic("20 GCOLOR 2")
      basic("30 FILL 10,10,19,19")
      run()
      assert(screen_contains("Ready"), "no errors")
    }

    test("mode-switch", "MODE changes display mode register") {
      basic("10 MODE 0")
      run()
      assert(peekbyte($A000) == 0, "mode 0")
      basic("10 MODE 3")
      run()
      assert(peekbyte($A000) == 3, "mode 3")
    }
  }

  suite("Blitter") {
    system(novavm)

    test("blitfill-ram", "BLITFILL fills CPU RAM") {
      basic("10 BLITFILL 0,$2000,256,256,1,$AA")
      run()
      assert(peekbyte($2000) == $AA, "first byte")
      assert(peekbyte($20FF) == $AA, "last byte")
      assert(memchk($2000, $100, $AA), "block filled")
    }

    test("blitcopy-stride", "BLITCOPY respects stride") {
      basic("10 FOR I=0 TO 3:POKE $2000+I,I+1:NEXT")
      basic("20 BLITCOPY 0,$2000,4,0,$3000,8,4,1")
      run()
      assert(peekbyte($3000) == 1, "col 0")
      assert(peekbyte($3003) == 4, "col 3")
      assert(peekbyte($3004) == 0, "stride gap clean")
    }
  }

  suite("Copper") {
    system(novavm)

    test("copper-bgcolor", "Copper changes bg at raster line") {
      basic("10 POKE $A001,6")
      basic("20 COPPERADD 0,50,1,2")
      basic("30 COPPERON")
      basic("40 FOR I=1 TO 500:NEXT")
      basic("50 COPPEROFF")
      run()
      ; Copper should have fired — bg register may have been written
      assert(screen_contains("Ready"), "completed without error")
    }
  }

  suite("XRAM") {
    system(novavm)

    test("stash-fetch-roundtrip", "STASH and FETCH preserve data") {
      basic("10 FOR I=0 TO 255:POKE $2000+I,I:NEXT")
      basic("20 STASH $2000,0,256")
      basic("30 FOR I=0 TO 255:POKE $2000+I,0:NEXT")
      basic("40 FETCH $2000,0,256")
      run()
      assert(peekbyte($2000) == 0, "byte 0 restored")
      assert(peekbyte($2080) == 128, "byte 128 restored")
      assert(peekbyte($20FF) == 255, "byte 255 restored")
    }

    test("named-blocks", "Named STASH/FETCH round-trip") {
      basic("10 FOR I=0 TO 99:POKE $2000+I,42:NEXT")
      basic("20 STASH \"mydata\",$2000,100")
      basic("30 FOR I=0 TO 99:POKE $2000+I,0:NEXT")
      basic("40 FETCH \"mydata\",$2000")
      run()
      assert(peekbyte($2000) == 42, "restored")
      assert(peekbyte($2063) == 42, "last byte")
    }
  }
}
```

---

## Implementation Phases

| Phase | Scope | Deliverable |
|-------|-------|-------------|
| 1 | NovaVM backend in sim6502 | `NovaVmBackend.cs`, `NovaVmConnection.cs`, factory + CLI flags |
| 2 | Grammar extensions | `basic()`, `run()`, `wait_*()`, `screen_*()`, `pause()`/`resume()` in ANTLR4 grammar + listener |
| 3 | TCP server extensions | `pause`, `resume`, `run_cycles`, `watch` commands in EmulatorTcpServer |
| 4 | Test suites | 8 test files, ~100 tests covering VGC, blitter, copper, XRAM, tiles, text, sprites, math |
| 5 | Verilator backend (future) | Same test files run against FPGA simulation |

---

## Running Tests

```bash
# Start emulator
dotnet run --project e6502.Avalonia &

# Run all integration tests
sim6502 -s tests/integration/vgc.6502 --backend novavm
sim6502 -s tests/integration/blitter.6502 --backend novavm

# Run with filtering
sim6502 -s tests/integration/vgc.6502 --backend novavm --filter "plot*"
sim6502 -s tests/integration/vgc.6502 --backend novavm --filter-tag smoke

# CI: run all suites
for f in tests/integration/*.6502; do sim6502 -s "$f" --backend novavm || exit 1; done
```
