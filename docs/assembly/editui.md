# Nova EDITUI Runtime

`EDITUI` is a small text-mode TUI shell for native Nova tools. It builds on
`VTEXT` and owns common chrome: the blue workspace, top menu row, title/help
band, bottom status row, and framed panels.

Applications still own their document model, input dispatch, canvas drawing,
and command handlers. The intended pattern is:

1. Call `editui_init`.
2. Set `EDITUI_TITLEL/H`, `EDITUI_HELPL/H`, and optionally
   `EDITUI_STATUSL/H`.
3. Call `editui_draw_shell`.
4. Set `EDITUI_BOXX/Y/W/H` and `EDITUI_BOX_TITLEL/H`, then call
   `editui_draw_box` for each app-specific panel.
5. Call `editui_select_box_body` before writing panel text, or select an
   equivalent clipped `VTEXT` region yourself.
6. Draw the editor content inside those panels with `VTEXT` or graphics helpers.

## ABI

State bytes:

```asm
EDITUI_TITLEL/H       ; zero-terminated title string, optional
EDITUI_MENUSL/H       ; menu table pointer; defaults to File/Edit/Help
EDITUI_MENUL/H        ; legacy zero-terminated menu-bar string fallback
EDITUI_HELPL/H        ; zero-terminated help/context string, optional
EDITUI_STATUSL/H      ; zero-terminated bottom status string, optional
EDITUI_DIRTY          ; nonzero draws `*` next to the workspace title
EDITUI_BOXX/Y/W/H     ; panel frame rectangle in text cells
EDITUI_BOX_TITLEL/H   ; zero-terminated panel title string, optional
EDITUI_BOX_STYLE      ; EDITUI_BOX_SINGLE or EDITUI_BOX_DOUBLE
EDITUI_MENU_CMD       ; selected command after menu dispatch
EDITUI_MENU_SELECTED  ; selected item index while a menu is open
```

Routines:

```asm
jsr editui_init          ; text mode, blue background/border, cursor off
jsr editui_clear_screen  ; fill the full VTEXT screen with shell color
jsr editui_shutdown      ; clear shell chrome, home cursor, and return cursor on
jsr editui_draw_shell    ; draw menu/title/help/status shell rows
jsr editui_draw_status   ; redraw only the bottom shortcut/status row
jsr editui_draw_box      ; draw configured CP437 panel frame
jsr editui_select_box_body ; select the frame's clipped text body
jsr editui_menu_open_hotkey ; A=menu hotkey, returns selected command in A
```

`editui_init` does not change the active palette. EDITUI color constants are
packed `bg/fg` text attributes authored for the default Nova/C64 palette, so a
caller that switches to an asset-specific palette should restore the expected
palette before entering the editor.

Menu and status strings use `&` to mark hotkey characters. For example,
`"&File   &Edit"` renders the `F` and `E` in the configured hotkey color.

Menu hotkeys follow the traditional Alt-letter convention. Host input maps
`Alt+F`, `Alt+E`, and similar chords to a two-byte sequence: `ESC` (`$1B`)
followed by the lower-case hotkey byte. In terminals that do not send Option
as Meta, `nova keyboard` also accepts pressing `Esc` and then the menu letter.
Applications that need menu dispatch should treat `EDITUI_KEY_ALT_PREFIX`
followed by a menu hotkey as a menu-open request. The debug key injector also
accepts names like `ALT-F` for scripted tests.

## Menu Tables

`editui_init` installs `editui_default_menus` into `EDITUI_MENUSL/H`. The
default menu bar contains:

```text
File -> New, Open, Save, Quit
Edit -> Undo, Cut, Copy, Paste
Help -> Help, About
```

The menu opener blocks while the dropdown is active. Use up/down to move,
Enter to choose, Escape to cancel, or press an item hotkey such as `Q` in the
File menu. It returns `EDITUI_CMD_NONE` when canceled or when no menu matches.

Apps should handle returned commands explicitly. For the common quit path:

```asm
      LDA   VGC_CHARIN
      CMP   #EDITUI_KEY_ALT_PREFIX
      BNE   @not_menu
      LDA   VGC_CHARIN
      JSR   editui_menu_open_hotkey
      CMP   #EDITUI_CMD_QUIT
      BEQ   app_exit        ; or run your dirty-check first
      JSR   redraw_screen   ; dropdowns overlay application content
```

Custom menus use a compact table format:

```asm
my_menus:
      .byte 2
      .byte 'f', 12, <file_title, >file_title, <file_items, >file_items
      .byte 's', 14, <sprite_title, >sprite_title, <sprite_items, >sprite_items

file_items:
      .byte 2
      .byte EDITUI_CMD_NEW,  'n', <item_new, >item_new
      .byte EDITUI_CMD_QUIT, 'q', <item_quit, >item_quit

file_title:  .byte "&File",0
item_new:    .byte "&New",0
item_quit:   .byte "&Quit",0
```

Each menu entry is `hotkey, dropdown_width, title_ptr, item_table_ptr`.
Each item entry is `command, hotkey, label_ptr`. Use `&` in titles and labels
to mark the visible hotkey. To modify the default menus, copy the table shape,
point `EDITUI_MENUSL/H` at your table, and handle any new command bytes in
your app loop.

Panel frames use Nova's default CP437-style font. Titled boxes draw the title
inside the top border with tee pieces around it, so a single-line frame reads
visually as `-| TITLE |-` instead of overwriting the border with floating text.

`editui_draw_shell` intentionally does not call application hooks. Callers
redraw their own editor surface after the shell so the same library can support
sprite editors, code editors, map editors, and other tools without owning their
business logic.

## Overlay save/restore (dialogs)

The menu dropdown save/restore-under blitter mechanism is generalized so any
overlapping overlay (dialogs, popups) can be drawn without clearing what is
underneath:

```asm
LDA #x  : STA EDITUI_MENU_SAVE_X
LDA #y  : STA EDITUI_MENU_SAVE_Y
LDA #w  : STA EDITUI_MENU_SAVE_W
LDA #h  : STA EDITUI_MENU_SAVE_H
JSR editui_save_under       ; snapshot char/color/attr planes under the rect
; ... draw the overlay, run its input loop ...
JSR editui_restore_under    ; put the covered cells back exactly
```

The save buffers hold up to `80 x 16` cells per plane. Only one save level is
live at a time, so do not nest an overlay over an already-open dropdown.

## EDITBUF — shared text editing engine

`EDITBUF` (`software/runtime/asm/editbuf.s`, `editbuf.inc`) layers a real editable text
buffer and a three-choice modal dialog on top of EDITUI. It is **language
neutral**: it owns cursor movement, insert/delete/backspace (including across
line boundaries), line navigation, horizontal/vertical scrolling, selection,
cut/copy/paste, the dirty marker, and `Ctrl-S` / `Ctrl-Q` conventions — but it
knows nothing about any document model.

Everything language-specific is supplied by the host through **hook vectors**,
each defaulting to a no-op so the editor works standalone:

| Vector | When called | Contract |
|---|---|---|
| `EDITBUF_SAVE_VECL/H` | `Ctrl-S`, and the dialog "Save First" choice | Host validates/saves/installs; returns A = `EDITBUF_SAVE_OK` / `_DRAFT` / `_ERROR`, optionally pointing `EDITBUF_STATUS` at a message |
| `EDITBUF_INDENT_VECL/H` | after a newline | Host returns A = number of leading spaces to auto-indent the new line |
| `EDITBUF_HILITE_VECL/H` | once per visible line during render | Host fills `EDITBUF_HL_COLORS` (one VGC color byte per char) for syntax highlighting; inputs are `EDITBUF_HL_PTR` and `EDITBUF_HL_LEN` |

Usage:

```asm
; configure the session (buffer is caller-owned)
LDA #<buf : STA EDITBUF_BUFL
LDA #>buf : STA EDITBUF_BUFH
LDA #<cap : STA EDITBUF_CAPL
LDA #>cap : STA EDITBUF_CAPH
LDA #<len : STA EDITBUF_LENL
LDA #>len : STA EDITBUF_LENH
LDA #<title : STA EDITBUF_TITLEL
LDA #>title : STA EDITBUF_TITLEH
; point hook vectors at host routines (or leave 0 for no-ops)
LDA #<on_save : STA EDITBUF_SAVE_VECL
LDA #>on_save : STA EDITBUF_SAVE_VECH
JSR editbuf_reset_state
JSR editbuf_run             ; modal; returns A = exit reason
```

`editbuf_run` returns `EDITBUF_EXIT_QUIT` (clean `Ctrl-Q`),
`EDITBUF_EXIT_SAVED` (dialog "Save First" succeeded), or
`EDITBUF_EXIT_DISCARD` (dialog "Exit Anyway"). The buffer text and
`EDITBUF_LEN` are left in place so an invalid draft can be re-opened. The
dirty-exit dialog (`editbuf_dialog3`) is a true overlapping modal built on
`editui_save_under` / `editui_restore_under` with three choices: Exit Anyway /
Save First / Cancel.
