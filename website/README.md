# NovaVM Website

Static project site for NovaVM.

## Preview

From the repository root:

```sh
python3 -m http.server 8080 --directory website
```

Then open:

```text
http://localhost:8080
```

## Assets

- `assets/screenshots/` contains copied project screenshots used by the page.
- `assets/books/` contains downloadable PDF books copied from `docs/books/`.
- `emulator/` contains the static browser emulator copied from
  `e6502.Browser/wwwroot/`. The hosted copy defaults to the Rust WebAssembly
  core so it can run without a .NET publish bundle.
- `emulator/forth.ndi` is mounted by the Rust browser core as the default `FD0`
  image. NovaForth uses it to load `forth/lib/core.4th` and
  `forth/autoexec.4th` during startup.
- `emulator/showcase/` contains NDI images exposed through the emulator's
  Quick Mount menu. These are copied from checked-in demo/example outputs.

When a new book PDF is built, copy it into `assets/books/` and add a link in
`index.html`.

When the browser emulator changes, refresh the hosted copy with:

```sh
rsync -a e6502.Browser/wwwroot/ website/emulator/
```

Then reapply any website-only shell changes in `website/emulator/index.html`.
