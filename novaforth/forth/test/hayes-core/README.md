# NovaForth Hayes Core Tests

These files adapt John Hayes' ANS Forth Core test program for NovaForth's
current source loader.

- `core-*.4th` are split from upstream `core.fr` because NovaForth currently
  loads one included file into an 8 KiB buffer.
- The upstream manual output and `ACCEPT` section is not part of the automated
  run because it requires visual inspection and keyboard input.
- `ttester-nova.4th` provides the Hayes `T{ ... -> ... }T` interface without
  requiring Core-Ext conditional compilation words.

Upstream sources:

- `core.fr`: https://github.com/gerryjackson/forth2012-test-suite
- `ttester.fs`: http://www.forth200x.org/tests/ttester.fs
