# VGC Transactional Blits

Nova currently scrolls mixed text/graphics regions in software by waiting for
vblank, then issuing one blitter operation per VGC plane. This is exposed
through the VTEXT automatic-scroll hook so applications can keep one scroll
event and batch graphics, color, text attributes, and character cells together.

Implemented hardware support adds `VCMD_SCROLLMIXED`/`CmdScrollMixed` (`$23`),
a vblank-aligned mixed-scroll command specialized for the active NovaZ/VTEXT
case. It accepts:

- a text rectangle, text rows-up count, fill color, and fill attribute
- a graphics rectangle, pixel rows-up count, and fill color
- implicit character, color, and text-attribute planes for the text rectangle
- vblank start as the transaction boundary

The NDK API stays stable. `vtext_scroll_mixed_up` now computes the matching
graphics rectangle and calls `vtext_scroll_composite_up`, which issues `$23`
instead of issuing separate per-plane blits.

## Hardware Text Attributes

Nova also has a real hardware-rendered bold text attribute. VTEXT previously
only had flash and reverse attributes, so runtimes that wanted bold were tempted
to change the foreground palette index or swap fonts. That is wrong for custom
palettes: `fg | 8` is not guaranteed to be a brighter readable version of the
same colour, and font swapping is too application-specific.

`VTXT_ATTR_BOLD` is bit 2 in the shared VTEXT/VGC text attribute model.
`VTXT_BOLD`/`TextFlagBold` is bit 3 in `RegTextFlags`; printed cells receive
the bold attribute, and `GTEXT` also renders bold while the flag is set. The
hardware effect draws glyph strokes one pixel wider inside the cell. The
important contract is that applications request bold as text style, not as
palette remapping and not by selecting a different font asset.
