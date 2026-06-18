# VGC Transactional Blits

Nova currently scrolls mixed text/graphics regions in software by waiting for
vblank, then issuing one blitter operation per VGC plane. This is exposed
through the VTEXT automatic-scroll hook so applications can keep one scroll
event and batch graphics, color, text attributes, and character cells together.

Future hardware should add a transactional blitter command that accepts:

- a plane mask, for example graphics, char, color, and text attribute planes
- source rectangle
- destination rectangle
- optional fill rectangle/value for newly exposed rows or columns
- commit timing, with vblank commit as the default

The NDK API should stay stable. When hardware support exists, the VTEXT mixed
scroll hook can become a thin wrapper around the transactional command instead
of issuing separate per-plane blits.

## Hardware Text Attributes

Nova also needs a real hardware-rendered bold text attribute. Today VTEXT only
has flash and reverse attributes, so runtimes that want bold are tempted to
change the foreground palette index or swap fonts. That is wrong for custom
palettes: `fg | 8` is not guaranteed to be a brighter readable version of the
same colour, and font swapping is too application-specific.

Add a `VTXT_ATTR_BOLD` bit to the shared VTEXT/VGC attribute model and render it
in the VGC text pipeline. The first implementation can be a simple hardware
effect, for example drawing the glyph one pixel wider when there is room inside
the cell. The important contract is that applications request bold as text
style, not as palette remapping and not by selecting a different font asset.
