# Beyond Zork NovaZ fixture

Exercises Beyond Zork's VT220 question, BEGIN prompt, character setup menu, and initial Hilltop prompt.

Parser longplay coverage is intentionally not wired into the shared target yet; Beyond Zork currently exposes a separate vocabulary/tokenization compatibility issue in NovaZ.

Build with:

```sh
make -C examples/novaz ndi PROJECT=beyond-zork
```
