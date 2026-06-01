#!/usr/bin/env python3
"""Build a collection of clearly-named, auto-booting NovaZ disk images — one
per Infocom game found under the Infocom archive.

Each image boots straight into NovaZ running that game's story. Games that ship
a Blorb (.blb) with sampled sounds bundle their soundfont; the NovaZ runtime
preloads it at story start. Z6 graphical games and non-game folders are skipped.

Usage:
    examples/novaz/tools/build-infocom-collection.py [options]

Options:
    --root DIR     Infocom archive root (default: $INFOCOM_ROOT or the
                   standard mount path).
    --only SLUG    Build only games whose slug matches SLUG (repeatable).
    --no-smoke     Skip the boot smoke test (build only).
    --keep-failed  Keep images that fail the smoke test (default: kept anyway,
                   just reported; this flag is a no-op kept for clarity).
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

NOVAZ = Path(__file__).resolve().parents[1]            # examples/novaz
BUILD = NOVAZ / "build"
DIST = NOVAZ / "dist" / "collection"
PACKER_PROJ = NOVAZ / "tools/Nova.NovaZ.Packer/Nova.NovaZ.Packer.csproj"
SMOKE_PROJ = NOVAZ / "tools/Nova.NovaZ.Smoke/Nova.NovaZ.Smoke.csproj"
PACKER_DLL = NOVAZ / "tools/Nova.NovaZ.Packer/bin/Release/net10.0/Nova.NovaZ.Packer.dll"
SMOKE_DLL = NOVAZ / "tools/Nova.NovaZ.Smoke/bin/Release/net10.0/Nova.NovaZ.Smoke.dll"
AUTOBOOT = BUILD / "AUTOBOOT.bin"
RUNTIME = BUILD / "novaz.bin"

DEFAULT_ROOT = os.environ.get("INFOCOM_ROOT", "/Volumes/Software/Emulation/Infocom")

SUPPORTED_EXT = (".z3", ".z4", ".z5")
SKIP_DIRS = {"zzinvisiclues"}     # hint-book collection, not a single game
SOUND_EXT = ".blb"

# A few games open on an interactive intro screen (not a ">" command prompt)
# that the automated boot-gate cannot complete. They still boot and run; verify
# boot by asserting their intro screen renders instead of waiting for ">".
INTRO_SCREEN_GAMES = {
    "bureaucracy": "LICENCE APPLICATION",   # opens on a satirical license form
}


@dataclass
class Game:
    slug: str
    title: str
    label: str
    story: Path
    version: int
    sounds: Path | None


def slugify(name: str) -> str:
    name = re.sub(r"\([^)]*\)", "", name)             # drop "(1984)" etc.
    name = name.lower()
    name = re.sub(r"[^a-z0-9]+", "-", name)
    return name.strip("-")


def derive_names(folder: str) -> tuple[str, str]:
    """(slug, title) from a folder name, dropping any ' - subtitle' tail.

    Only a ' - ' separator is treated as a subtitle delimiter. An underscore
    (used as a ':' substitute, e.g. 'Zork_ The Undiscovered Underground') is
    kept so the slug stays unambiguous rather than collapsing to 'zork'."""
    head = re.split(r"\s+-\s+", folder, maxsplit=1)[0].strip()
    if not head:
        head = folder
    slug = slugify(head)
    return slug, head


def label_from_slug(slug: str) -> str:
    return re.sub(r"[^A-Z0-9]", "", slug.upper())[:11] or "NOVAZ"


def discover(root: Path) -> tuple[list[Game], list[tuple[str, str]]]:
    games: list[Game] = []
    skipped: list[tuple[str, str]] = []
    for entry in sorted(root.iterdir()):
        if not entry.is_dir() or entry.name.startswith("."):
            continue
        if entry.name.lower() in SKIP_DIRS:
            skipped.append((entry.name, "hint-book folder, not a game"))
            continue

        files = [f for f in entry.iterdir()
                 if f.is_file() and not f.name.startswith("._")]
        supported = [f for f in files if f.suffix.lower() in SUPPORTED_EXT]
        z6 = [f for f in files if f.suffix.lower() == ".z6"]

        if not supported:
            reason = "only Z6/graphical story (unsupported)" if z6 else "no supported story file"
            skipped.append((entry.name, reason))
            continue

        if len(supported) > 1:
            # Prefer a *PATCHED* (bug-fixed) release; then a story whose basename
            # matches the folder; else the largest.
            patched = [f for f in supported if "patched" in f.stem.lower()]
            if patched:
                story = max(patched, key=lambda f: f.stat().st_size)
            else:
                slug_head, _ = derive_names(entry.name)
                match = [f for f in supported if slugify(f.stem) in slug_head or slug_head in slugify(f.stem)]
                story = max(match or supported, key=lambda f: f.stat().st_size)
            note = f"multiple stories {[f.name for f in supported]}; chose {story.name}"
            skipped.append((entry.name, "WARN: " + note))
        else:
            story = supported[0]

        version = int(story.suffix.lower()[2])
        blb = next((f for f in files if f.suffix.lower() == SOUND_EXT), None)
        slug, title = derive_names(entry.name)
        games.append(Game(slug, title, label_from_slug(slug), story, version, blb))

    return games, skipped


def run(cmd: list[str], timeout: int | None = None) -> subprocess.CompletedProcess:
    return subprocess.run(
        [str(c) for c in cmd],
        capture_output=True, text=True, timeout=timeout,
    )


def ensure_build() -> bool:
    print("==> building runtime, packer, smoke ...")
    if run(["make", "-C", str(NOVAZ), "all"]).returncode != 0:
        print("    FAILED: make all", file=sys.stderr)
        return False
    for proj in (PACKER_PROJ, SMOKE_PROJ):
        r = run(["dotnet", "build", str(proj), "-c", "Release"])
        if r.returncode != 0:
            print(f"    FAILED: dotnet build {proj}\n{r.stdout}\n{r.stderr}", file=sys.stderr)
            return False
    return all(p.exists() for p in (AUTOBOOT, RUNTIME, PACKER_DLL, SMOKE_DLL))


def pack(game: Game, out: Path, with_sounds: bool) -> subprocess.CompletedProcess:
    cmd = [
        "dotnet", PACKER_DLL,
        "--output", out,
        "--autoboot", AUTOBOOT,
        "--runtime", RUNTIME,
        "--label", game.label,
        "--size-kb", "1440",
        "--story", game.story,
    ]
    if with_sounds and game.sounds is not None:
        cmd += ["--sounds", game.sounds]
    return run(cmd, timeout=120)


def build_image(game: Game) -> tuple[bool, str]:
    out = DIST / f"{game.slug}.ndi"
    sound_state = "none"
    if game.sounds is not None:
        r = pack(game, out, with_sounds=True)
        if r.returncode == 0:
            sound_state = "bundled"
        elif "No sound resources" in (r.stdout + r.stderr):
            r = pack(game, out, with_sounds=False)       # cover-art-only Blorb
            sound_state = "none(cover-only)"
        if r.returncode != 0:
            return False, f"pack failed: {r.stderr.strip() or r.stdout.strip()}"
    else:
        r = pack(game, out, with_sounds=False)
        if r.returncode != 0:
            return False, f"pack failed: {r.stderr.strip() or r.stdout.strip()}"
    return True, sound_state


def smoke(game: Game, has_sound: bool) -> tuple[bool, str]:
    out = DIST / f"{game.slug}.ndi"
    intro = INTRO_SCREEN_GAMES.get(game.slug)
    if intro is not None:
        # Boot proof for a game that opens on an interactive intro screen:
        # confirm NovaZ runs far enough to render that screen.
        args = ["dotnet", SMOKE_DLL, out,
                "--screen-only", "--skip-manifest-check", "--expect-screen", intro]
    else:
        args = ["dotnet", SMOKE_DLL, out,
                "--generic-boot", "--boot-only", "--no-status-line", "--skip-manifest-check"]
        if has_sound:
            args.append("--expect-soundfont")
    try:
        r = run(args, timeout=300)
    except subprocess.TimeoutExpired:
        return False, "smoke timed out"
    if r.returncode == 0:
        return True, "ok"
    tail = (r.stdout + r.stderr).strip().splitlines()
    return False, " / ".join(tail[-3:]) if tail else "smoke failed"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=DEFAULT_ROOT)
    ap.add_argument("--only", action="append", default=[])
    ap.add_argument("--no-smoke", action="store_true")
    ap.add_argument("--keep-failed", action="store_true")
    a = ap.parse_args()

    root = Path(a.root)
    if not root.is_dir():
        print(f"Infocom archive not found: {root}", file=sys.stderr)
        return 2

    if not ensure_build():
        return 1

    games, skipped = discover(root)
    if a.only:
        wanted = set(a.only)
        games = [g for g in games if g.slug in wanted]

    DIST.mkdir(parents=True, exist_ok=True)
    print(f"==> {len(games)} games to build, {len(skipped)} folders skipped\n")

    results = []
    for g in games:
        snd = "+sound" if g.sounds else ""
        print(f"--- {g.slug} (Z{g.version}{snd}) <- {g.story.name}")
        ok, state = build_image(g)
        if not ok:
            print(f"    BUILD FAIL: {state}")
            results.append((g, "BUILD-FAIL", state, "-"))
            continue
        has_sound = state == "bundled"
        if a.no_smoke:
            results.append((g, "BUILT", state, "skipped"))
            continue
        sok, smsg = smoke(g, has_sound)
        results.append((g, "PASS" if sok else "SMOKE-FAIL", state, smsg))
        print(f"    {'PASS' if sok else 'SMOKE-FAIL'}  sound={state}  {smsg if not sok else ''}")

    # Summary table
    print("\n=== collection summary ===")
    print(f"{'slug':28} {'ver':4} {'sound':16} {'boot':11} note")
    print("-" * 88)
    npass = 0
    for g, status, sound, note in results:
        if status == "PASS":
            npass += 1
        note = "" if status == "PASS" else note
        print(f"{g.slug:28} Z{g.version:<3} {sound:16} {status:11} {note[:30]}")
    print("-" * 88)
    print(f"{npass}/{len(results)} verified, images in {DIST}")

    if skipped:
        print("\nskipped folders:")
        for name, reason in skipped:
            print(f"  - {name}: {reason}")

    failures = [r for r in results if r[1] != "PASS" and not a.no_smoke]
    return 1 if (failures and not a.keep_failed) else 0


if __name__ == "__main__":
    sys.exit(main())
