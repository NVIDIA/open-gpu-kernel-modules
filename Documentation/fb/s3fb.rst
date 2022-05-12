===========================================
s3fb - fbdev driver for S3 Trio/Virge chips
===========================================


Supported Hardware
==================

	S3 Trio32
	S3 Trio64 (and variants V+, UV+, V2/DX, V2/GX)
	S3 Virge  (and variants VX, DX, GX and GX2+)
	S3 Plato/PX		(completely untested)
	S3 Aurora64V+		(completely untested)

	- only PCI bus supported
	- only BIOS initialized VGA devices supported
	- probably not working on big endian

I tested s3fb on Trio64 (plain, V+ and V2/DX) and Virge (plain, VX, DX),
all on i386.


Supported Features
==================

	*  4 bpp pseudocolor modes (with 18bit palette, two variants)
	*  8 bpp pseudocolor mode (with 18bit palette)
	* 16 bpp truecolor modes (RGB 555 and RGB 565)
	* 24 bpp truecolor mode (RGB 888) on (only on Virge VX)
	* 32 bpp truecolor mode (RGB 888) on (not on Virge VX)
	* text mode (activated by bpp = 0)
	* interlaced mode variant (not available in text mode)
	* doublescan mode variant (not available in text mode)
	* panning in both directions
	* suspend/resume support
	* DPMS support

Text mode is supported even in higher resolutions, but there is limitation to
lower pixclocks (maximum usually between 50-60 MHz, depending on specific
hardware, i get best results from plain S3 Trio32 card - about 75 MHz). This
limitation is not enforced by driver. Text mode supports 8bit wide fonts only
(hardware limitation) and 16bit tall fonts (driver limitation). Text mode
support is broken on S3 Trio64 V2/DX.

There are two 4 bpp modes. First mode (selected if nonstd == 0) is mode with
packed pixels, high nibble first. Second mode (selected if nonstd == 1) is mode
with interleaved planes (1 byte interleave), MSB first. Both modes support
8bit wide fonts only (driver limitation).

Suspend/resume works on systems that initialize video card during resume and
if device is active (for example used by fbcon).


Missing Features
================
(alias TODO list)

	* secondary (not initialized by BIOS) device support
	* big endian support
	* Zorro bus support
	* MMIO support
	* 24 bpp mode support on more cards
	* support for fontwidths != 8 in 4 bpp modes
	* support for fontheight != 16 in text mode
	* composite and external sync (is anyone able to test this?)
	* hardware cursor
	* video overlay support
	* vsync synchronization
	* feature connector support
	* acceleration support (8514-like 2D, Virge 3D, busmaster transfers)
	* better values for some magic registers (performance issues)


Known bugs
==========

	* cursor disable in text mode doesn't work
	* text mode broken on S3 Trio64 V2/DX


--
Ondrej Zajicek <santiago@crfreenet.org>
