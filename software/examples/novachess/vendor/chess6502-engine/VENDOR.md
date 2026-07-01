# chess6502-engine Vendor Note

Source: `https://github.com/barryw/chess6502-engine`
Revision: `60cb23b757f8509e1b0af83d7c0948c2c60f5b41`

This copy is vendored so Nova Chess can build repeatably without reaching out
to GitHub during the normal Nova build. The Nova wrapper lives outside the
vendor tree in `software/examples/novachess/src/engine_nova.s`.

Local patch:

- `src/constants.s` has an `ENGINE_NOVA_ZP` profile that remaps the engine's
  old low zero-page scratch to `$50-$83`, preserving Nova's `$20-$2f`
  pseudo-register mailbox.
- `Makefile` mounts `$(CURDIR)` into Docker so tests work when invoked through
  `make -C` from the Nova repository.
- `software/examples/novachess/src/engine_nova.s` defines `ENGINE_CPU_65C02` for
  future Nova-only fast paths while leaving the original 6502 harness build
  unchanged.
