#!/usr/bin/env python3
"""End-to-end NovaLogo demo on live hardware:

  1. upload the freshly-built novalogo ROMs to the SD card
  2. cold-start into NovaLogo
  3. DRAW (init the turtle + split graphics)
  4. TO SQUARE  -> opens the shared EDITUI editor
       REPEAT 4 [FD 50 RT 90]   (typed into the editor)
       Ctrl-S (save) / Ctrl-Q (exit)
  5. run SQUARE and confirm the turtle drew a closed box in the gfx plane

Leaves the square on screen so you can look at the monitor.
"""
from __future__ import annotations

import sys
import time
import urllib.request
import urllib.error
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from novahost_client import NovaHostClient, NovaHostError  # noqa: E402

HOST = "192.168.1.65"
DEBUG_PORT = 6503
TIMEOUT = 30.0
REPO = Path(__file__).resolve().parents[1]
ROMS = [
    (REPO / "novalogo/novalogo.bin", "/roms/novalogo.bin"),
]

VGC_GFX_SPACE = 3
GFX_WIDTH = 320
TURTLE_STATE = 0x9F00
VGC_MODE = 0xA000


def step(msg: str) -> None:
    print(f">>> {msg}", flush=True)


def http_put(path: str, data: bytes) -> None:
    req = urllib.request.Request(f"http://{HOST}{path}", data=data, method="PUT",
                                 headers={"Content-Type": "application/octet-stream"})
    with urllib.request.urlopen(req, timeout=TIMEOUT) as resp:
        resp.read()


def upload_roms() -> None:
    for src, dest in ROMS:
        if not src.exists():
            raise FileNotFoundError(f"{src} missing — build novalogo first")
        deadline = time.monotonic() + TIMEOUT
        while True:
            try:
                step(f"upload {src.name} -> /sd{dest} ({src.stat().st_size} bytes)")
                http_put(f"/sd{dest}", src.read_bytes())
                break
            except (OSError, urllib.error.URLError) as exc:
                if time.monotonic() > deadline:
                    raise
                print(f"    retry: {exc}", flush=True)
                time.sleep(0.5)


def gfx_row(client: NovaHostClient, y: int, x: int, width: int) -> list[int]:
    out: list[int] = []
    off = 0
    while off < width:
        chunk = min(256, width - off)
        resp = client.read_vram(VGC_GFX_SPACE, y * GFX_WIDTH + x + off, chunk)
        out.extend(int(v) & 0xFF for v in resp.get("values", []))
        off += chunk
    return out


def count_gfx_nonzero(client: NovaHostClient, x: int, y: int, w: int, h: int) -> int:
    return sum(1 for row in range(y, y + h) for v in gfx_row(client, row, x, w) if v != 0)


def turtle_state(client: NovaHostClient) -> list[int]:
    return client.peek_block(TURTLE_STATE, 17)


def word(state: list[int], lo: int, hi: int) -> int:
    return state[lo] | (state[hi] << 8)


def screen_text(client: NovaHostClient) -> str:
    return "\n".join(str(l) for l in client.read_screen().get("lines", []))


def wait_screen(client: NovaHostClient, text: str, timeout: float) -> str:
    deadline = time.monotonic() + timeout
    last = ""
    while time.monotonic() < deadline:
        try:
            last = screen_text(client)
            if text in last:
                return last
        except (NovaHostError, OSError) as exc:
            client.close()
            last = str(exc)
        time.sleep(0.25)
    raise TimeoutError(f"screen never contained {text!r}\n{last}")


def wait_cond(desc: str, timeout: float, pred) -> None:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        try:
            if pred():
                return
        except (NovaHostError, OSError):
            pass
        time.sleep(0.1)
    raise TimeoutError(f"timed out waiting for {desc}")


def main() -> int:
    step("upload freshly-built ROMs")
    upload_roms()

    client = NovaHostClient(host=HOST, port=DEBUG_PORT, timeout=TIMEOUT)
    try:
        step("cold-start NovaLogo")
        try:
            client.cold_start(wait_ready=False)
        except OSError:
            pass
        finally:
            client.close()

        wait_screen(client, "Nova LOGO v1.0", TIMEOUT)
        wait_screen(client, "?", TIMEOUT)
        step("NovaLogo booted, prompt is up")

        step("DRAW — init turtle + split graphics")
        client.type_text("DRAW\r")
        wait_cond("turtle init", TIMEOUT,
                  lambda: turtle_state(client)[12] == 1 and turtle_state(client)[9] == 1)

        pix_before = count_gfx_nonzero(client, 152, 72, 16, 16)
        step(f"   turtle inited at center; turtle pixels={pix_before}")

        step("TO SQUARE — open editor, type body, Ctrl-S, Ctrl-Q")
        client.type_text("TO SQUARE\rREPEAT 4 [FD 50 RT 90]\r\x13\x11")
        wait_screen(client, "SQUARE DEFINED", TIMEOUT)
        step("   editor saved + exited: 'SQUARE DEFINED'")

        # The copper split rewrites VGC_MODE every scanline ($03 gfx top / $00
        # text bottom), so a single read is racy. Sample it: seeing the gfx
        # region ($03) proves the editor restored the split (not full text).
        modes = {client.peek(VGC_MODE) for _ in range(20)}
        pix_after = count_gfx_nonzero(client, 152, 72, 16, 16)
        if 0x03 not in modes:
            raise RuntimeError(f"editor did not restore the graphics split; modes seen={sorted(modes)}")
        if pix_after < pix_before:
            raise RuntimeError(f"editor cleared the turtle/graphics: {pix_before} -> {pix_after}")
        # Bottom split-text background must be the Logo color, not the editor's
        # blue panel. Color attr packs bg in the high nibble ($6x = blue leak).
        split_row = 42
        attrs = [int(v) & 0xFF for v in client.read_vram(2, split_row * 80, 16).get("values", [])]
        bg_nibbles = {a >> 4 for a in attrs}
        if 0x6 in bg_nibbles:
            raise RuntimeError(f"editor blue background leaked into the split text: attrs={[hex(a) for a in attrs]}")
        step(f"   display restored: split active, turtle pixels={pix_after}, "
             f"split-text bg nibbles={sorted(bg_nibbles)} (no blue leak)")

        step("run SQUARE — turtle draws the box")
        client.type_text("SQUARE\r")
        # Wait for the box to CLOSE: turtle back at origin facing north AND both
        # edges drawn (origin+heading-0 alone is also the start state).
        wait_cond("square closed", TIMEOUT,
                  lambda: word(turtle_state(client), 1, 2) == 160
                  and word(turtle_state(client), 4, 5) == 80
                  and word(turtle_state(client), 6, 7) == 0
                  and count_gfx_nonzero(client, 158, 28, 56, 5) >= 30
                  and count_gfx_nonzero(client, 208, 28, 5, 56) >= 30)

        client.pause()
        try:
            st = turtle_state(client)
            x, y = word(st, 1, 2), word(st, 4, 5)
            heading = word(st, 6, 7)
            top = count_gfx_nonzero(client, 158, 28, 56, 5)
            right = count_gfx_nonzero(client, 208, 28, 5, 56)
        finally:
            client.resume()

        ok = (x == 160 and y == 80 and heading == 0 and top >= 30 and right >= 30)
        verdict = "PASS" if ok else "FAIL"
        print(f"\n{verdict}: SQUARE drew a closed box — "
              f"turtle returned to x={x} y={y} heading={heading}; "
              f"top-edge pixels={top}, right-edge pixels={right}", flush=True)
        print("\n--- screen ---", flush=True)
        print(screen_text(client), flush=True)
        print("\nThe square is on the monitor now. boot.json already defaults to "
              "novalogo, so the board stays in NovaLogo.", flush=True)
        return 0 if ok else 1
    finally:
        client.close()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
