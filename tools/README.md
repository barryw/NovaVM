# Tools Directory

This directory is being retired as an ad hoc command surface.

Use the Nova CLI for supported workflows:

```bash
dotnet run --project e6502.Nova -c Release -- help
dotnet run --project e6502.Nova -c Release -- convert help
dotnet run --project e6502.Nova -c Release -- codegen help
dotnet run --project e6502.Nova -c Release -- module help
dotnet run --project e6502.Nova -c Release -- arty help
dotnet run --project e6502.Nova -c Release -- docs help
dotnet run --project e6502.Nova -c Release -- build help
dotnet run --project e6502.Nova -c Release -- capture help
dotnet run --project e6502.Nova -c Release -- check help
```

Fixed-address overlays are still packaged by `e6502.Tools`:

```bash
dotnet run --project e6502.Tools -- pack-overlay \
  --input build/module.bin \
  --output build/MODULE.OVL \
  --load $7000 \
  --max-size $2000 \
  --sym build/module.sym \
  --main module_main
```

Do not add new shell or Python helper scripts here. Add the command to
`e6502.Nova` or use an existing `nova` command.

The remaining repo Python files are documented in `docs/script-inventory.md`;
they are external tool hooks or vendored upstream utilities, not NovaVM command
entry points.
