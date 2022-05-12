========================================
arkfb - fbdev driver for ARK Logic chips
========================================


Supported Hardware
==================

	ARK 2000PV chip
	ICS 5342 ramdac

	- only BIOS initialized VGA devices supported
	- probably not working on big endian


Supported Features
==================

	*  4 bpp pseudocolor modes (with 18bit palette, two variants)
	*  8 bpp pseudocolor mode (with 18bit palette)
	* 16 bpp truecolor modes (RGB 555 and RGB 565)
	* 24 bpp truecolor mode (RGB 888)
	* 32 bpp truecolor mode (RGB 888)
	* text mode (activated by bpp = 0)
	* doublescan mode variant (not available in text mode)
	* panning in both directions
	* suspend/resume support

Text mode is supported even in higher resolutions, but there is limitation to
lower pixclocks (i got maximum about 70 MHz, it is dependent on specific
hardware). This limitation is not enforced by driver. Text mode supports 8bit
wide fonts only (hardware limitation) and 16bit tall fonts (driver
limitation). Unfortunately character attributes (like color) in text mode are
broken for unknown reason, so its usefulness is limited.

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
	* DPMS support
	* MMIO support
	* interlaced mode variant
	* support for fontwidths != 8 in 4 bpp modes
	* support for fontheight != 16 in text mode
	* hardware cursor
	* vsync synchronization
	* feature connector support
	* acceleration support (8514-like 2D)


Known bugs
==========

	* character attributes (and cursor) in text mode are broken

--
Ondrej Zajicek <santiago@crfreenet.org>
