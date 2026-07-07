# NSPR sprite-bank format + sprite-editor coordination

This is the **single source of truth** for the `.nsp` metasprite sprite-bank
format and the working agreement for the two agents building the sprite editor
(one on Linux driving C#/asm/FPGA/board, one on macOS driving NovaDraw/Swift).

Read this at the start of a session; append to the **Status log** at the end.

---

## 1. Format spec (`NSPR`, `.nsp`)

One file is a **sprite sheet**: many **characters**, each a **metasprite** (a
fixed layout of 16×16 hardware-sprite **parts**) with named **animations**.
Animation is cheap — the part layout is fixed and each **frame** only reassigns
which **shape** (16×16 4bpp pixel cell, 128 bytes) each part shows. This mirrors
the on-device runtime: `software/runtime/asm/msprite.s` (visual descriptor =
parts) + `anim.s` (frame tables). A plain 16×16 sprite is a 1-part metasprite;
`kind` reserves room for vsprites, so v1 never needs a format break.

Little-endian; all counts are `u8` (≤ 255). Shapes are packed 16×16 4bpp,
**128 bytes each, low nibble = left pixel** (same as NovaDraw's `SpriteFormat`
and the VGC sprite RAM).

```
Header (16 B)
  0   "NSPR"
  4   version         (1)
  5   kind            (0 = hw metasprite, 1 = vsprite [reserved])
  6   flags           (bit0 = palette present)
  7   shapeCount
  8   charCount
  9   cellW           (16)
  10  cellH           (16)
  11  reserved[5]     (0)
Palette (48 B, only if flags bit0)   16 × {R,G,B}
Shape pool: shapeCount × 128 B
Characters × charCount {
  name[8]             (ASCII, NUL-padded, truncated to 8)
  partCount
  parts × { dx, dy, flags }
  animCount
  animations × {
    name[8]
    frameCount
    ticks             (delay per animation frame)
    flags             (bit0 loop, bit1 ping-pong)
    frames × ( partCount × shapeIndex )
  }
}
```

### Reference implementations (keep in sync)
- **C#** — `e6502.Storage/NsprBank.cs` (authoritative; generates the fixture).
- **Swift** — `NovaDraw/Sources/IO/NsprFormat.swift`.
- **6502** — `spritebank_load` (to be written) parses this into XRAM shapes +
  `msprite_spawn` + `anim` descriptors.

---

## 2. The correctness contract: the golden fixture

`testdata/nspr/sample.nsp` is the **canonical conformance fixture**. Every codec
must decode it to the sample below **and** re-encode that sample to the exact
same bytes. This is how the implementations stay in agreement without the agents
coordinating the wire format by hand — if any side drifts, its conformance test
goes red before merge.

### Canonical sample (what the fixture contains)
- `version 1`, `kind 0 (hw)`, palette **present** = bytes `0..47` (color *k* = RGB
  `{3k, 3k+1, 3k+2}`).
- **5 shapes**: shape *i* is 128 bytes all `0x10 + i`.
- **Character "HERO"** — 1 part `(dx0,dy0,flags0)`; anim **"IDLE"** `ticks 8`,
  flags `loop`, frames `[0] [1] [2]`.
- **Character "BOSS"** — 4 parts (2×2): `(0,0,0) (16,0,0) (0,16,0) (16,16,1)`;
  anim **"WALK"** `ticks 4`, flags `loop|pingPong`, frames `[0,1,2,3] [4,3,2,1]`.

### Conformance tests
- **C#** — `e6502UnitTests/NsprConformanceTests.cs`
  (`GoldenFixtureDecodesToCanonicalBank`, `CanonicalBankReencodesByteIdenticalToFixture`).
- **Swift** — `NovaDraw/Tests/NsprConformanceTests.swift` (the Mac agent's first
  job: make it green — see kickoff below).

### Regenerating the fixture (only when the format intentionally changes)
```
NSPR_GEN=1 dotnet test e6502UnitTests --filter GenerateGoldenFixture
```
Then update the canonical sample above + both conformance tests, and commit the
new `sample.nsp` in the same change. **Never** change the fixture bytes without
updating this doc and both sides.

---

## 3. Working agreement (two agents, one repo)

**Ownership (near-zero overlap → few conflicts):**
- *Linux agent:* `e6502.*/`, `software/**`, `e6502.FPGA/**`, the `nova` CLI,
  board deploys, C#/asm/headless tests.
- *macOS agent:* `NovaDraw/**` (Swift build + tests; I can't build Swift).
- *Shared surface:* this doc, the format spec, and `testdata/nspr/**`. Treat
  changes here as an interface change — announce them in the Status log.

**Flow:** work on `main` (small, frequent commits) or short-lived feature
branches. **Pull before you start.** Don't edit the other side's directory
without a Status-log note. The human steers at the seams and resolves conflicts.

**Communication is this doc + git.** There's no live agent-to-agent channel; the
Status log below is the async message board — the other agent pulls and reads it.

---

## 4. Status log (append; newest last)

- **Linux agent** — Scaffolded the harness: C# codec (`NsprBank`) is
  authoritative, golden fixture `testdata/nspr/sample.nsp` generated + committed,
  C# conformance tests green. Swift codec (`NsprFormat`) + `NdiBridge`
  (CLI-delegated NDI access) + Settings UI committed but **unbuilt** (no Swift
  toolchain here). Swift conformance test stubbed for the Mac agent.

---

## 5. Kickoff for the macOS agent

You own `NovaDraw/**`. First tasks, in order:

1. **Build & green the conformance test.** Wire `testdata/nspr/sample.nsp` into
   the `NovaDrawTests` target (a package resource, or resolve it from the repo
   root — the stub in `Tests/NsprConformanceTests.swift` shows the intent) and
   make it decode → assert the canonical sample → re-encode byte-identical. This
   proves `NsprFormat.swift` matches the C# authority.
2. **Shake out the untested Swift.** `NsprFormat.swift`, `NdiBridge.swift`,
   `NovaCLISettings.swift`, `SettingsView.swift`, and the `Settings` scene in
   `NovaDrawApp.swift` were written without a compiler here — expect a small fixup
   pass (imports, `onChange` signature, resource loading).
3. **Design the bank ↔ document mapping.** NovaDraw's document is one canvas; an
   `NsprBank` is many characters × animations × frames. Decide how NovaDraw
   represents/edits a whole bank, then wire "Open .nsp from NDI" / "Save .nsp to
   NDI" menu commands using `NovaCLISettings.bridge()`. Record the model decision
   here before building the UI.

Log what you change under **Status log** so the Linux agent sees it on pull.
