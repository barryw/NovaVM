# Forth 2012 standard test-suite staging for NovaForth

The files under `src/` are copied from:

https://github.com/gerryjackson/forth2012-test-suite

The upstream suite is based on the John Hayes tester and includes tests for
Core, Core Extensions, and several optional word sets. Keep the upstream files
unchanged so we can compare future updates mechanically.

Nova-specific runners live next to this README:

- `nova-core.4th` runs `tester.fr` and upstream `core.fr`, then prints
  `F2012-CORE-DONE`.
- `nova-core-ext.4th` runs the Core Extension support library and upstream
  `coreexttest.fth`, then prints `F2012-CORE-EXT-DONE`.
- `nova-file.4th` runs the File Access suite from the upstream `src/`
  directory, then prints `F2012-FILE-DONE`.
- `nova-string.4th` runs the String and String Extensions suite, then prints
  `F2012-STRING-DONE`.
- `nova-all.4th` runs the currently claimed suites: preliminary checks, Core,
  Core Plus, Core Extensions, String, and File Access. It then prints
  `F2012-CLAIMED-DONE`.

Do not run upstream `runtests.fth` as a conformance claim yet. It includes
optional word sets NovaForth does not provide.
