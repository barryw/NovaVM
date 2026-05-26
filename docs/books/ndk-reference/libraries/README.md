# NDK Library Notes

This directory is reserved for authored per-library notes. The generated API
reference comes from `runtime/asm`; this directory should hold the human
material: overview text, examples, caveats, and design notes.

Current authored chapters:

- `fio.tex`: FIO command lifecycle, filename handling, RAM/VGC/XRAM transfer
  paths, directory metadata, runtime services, and application guidance.
- `vgc.tex`: VGC architecture, command flow, memory paths, examples, and
  developer guidance for using `vgc.inc`/`vgc.s`.
