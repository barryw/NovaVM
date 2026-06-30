# NovaDraw

NovaDraw is a macOS pixel editor for Nova indexed-color assets. Projects are saved as `.novadraw` files and can contain up to 12 images on a shared canvas.

_Part of the WHI Mac-native editor family (NovaDraw · FamiForge · Miggy Draw)._

## MCP Server

NovaDraw includes a separate stdio MCP server, `NovaDrawMCP`, for AI agents that need to create, inspect, and touch up project images without driving the UI.

Build it:

```sh
swift build --product NovaDrawMCP
```

Run it as an MCP stdio server:

```sh
.build/debug/NovaDrawMCP
```

Release build:

```sh
make mcp
.build/release/NovaDrawMCP
```

Useful tools exposed by the server:

- `novadraw_create_project`: create a `.novadraw` file.
- `novadraw_inspect_project`: list dimensions, image names, and painted pixel counts.
- `novadraw_ascii_preview`: return a text preview of a region; hex digits are painted colors and dots are transparent pixels.
- `novadraw_set_pixels`: paint a block from text rows or row-major pixels.
- `novadraw_draw_line`, `novadraw_draw_rect`, `novadraw_draw_circle`: draw primitive shapes.
- `novadraw_flood_fill`, `novadraw_clear_region`, `novadraw_replace_color`: touch up existing art.
- `novadraw_rename_image`, `novadraw_duplicate_image`, `novadraw_select_image`: manage project images.
- `novadraw_set_backdrop_image`: embed a PNG/JPEG as a fit-to-canvas tracing backdrop.
- `novadraw_export_nvg`: export a 320x200 project image as native Nova NVG.

## Live Reload

When NovaDraw has a project open, it watches that project file for external changes. MCP tools write projects atomically, and the UI reloads the open file when the modification date changes, so an agent can edit the same `.novadraw` file and the canvas refreshes in the app.

This is file-based synchronization. Unsaved in-app edits should be saved before handing the file to an agent, because an external edit reloads the project from disk.
