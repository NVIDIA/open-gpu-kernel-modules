============================================
Framebuffer driver for Cirrus Logic chipsets
============================================

Copyright 1999 Jeff Garzik <jgarzik@pobox.com>


.. just a little something to get people going; contributors welcome!


Chip families supported:
	- SD64
	- Piccolo
	- Picasso
	- Spectrum
	- Alpine (GD-543x/4x)
	- Picasso4 (GD-5446)
	- GD-5480
	- Laguna (GD-546x)

Bus's supported:
	- PCI
	- Zorro

Architectures supported:
	- i386
	- Alpha
	- PPC (Motorola Powerstack)
	- m68k (Amiga)



Default video modes
-------------------
At the moment, there are two kernel command line arguments supported:

- mode:640x480
- mode:800x600
- mode:1024x768

Full support for startup video modes (modedb) will be integrated soon.

Version 1.9.9.1
---------------
* Fix memory detection for 512kB case
* 800x600 mode
* Fixed timings
* Hint for AXP: Use -accel false -vyres -1 when changing resolution


Version 1.9.4.4
---------------
* Preliminary Laguna support
* Overhaul color register routines.
* Associated with the above, console colors are now obtained from a LUT
  called 'palette' instead of from the VGA registers.  This code was
  modelled after that in atyfb and matroxfb.
* Code cleanup, add comments.
* Overhaul SR07 handling.
* Bug fixes.


Version 1.9.4.3
---------------
* Correctly set default startup video mode.
* Do not override ram size setting.  Define
  CLGEN_USE_HARDCODED_RAM_SETTINGS if you _do_ want to override the RAM
  setting.
* Compile fixes related to new 2.3.x IORESOURCE_IO[PORT] symbol changes.
* Use new 2.3.x resource allocation.
* Some code cleanup.


Version 1.9.4.2
---------------
* Casting fixes.
* Assertions no longer cause an oops on purpose.
* Bug fixes.


Version 1.9.4.1
---------------
* Add compatibility support.  Now requires a 2.1.x, 2.2.x or 2.3.x kernel.


Version 1.9.4
-------------
* Several enhancements, smaller memory footprint, a few bugfixes.
* Requires kernel 2.3.14-pre1 or later.


Version 1.9.3
-------------
* Bundled with kernel 2.3.14-pre1 or later.
