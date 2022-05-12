=============================================================
Intel 830M/845G/852GM/855GM/865G/915G/945G Framebuffer driver
=============================================================

A. Introduction
===============

This is a framebuffer driver for various Intel 8xx/9xx compatible
graphics devices.  These would include:

	- Intel 830M
	- Intel 845G
	- Intel 852GM
	- Intel 855GM
	- Intel 865G
	- Intel 915G
	- Intel 915GM
	- Intel 945G
	- Intel 945GM
	- Intel 945GME
	- Intel 965G
	- Intel 965GM

B.  List of available options
=============================

   a. "video=intelfb"
	enables the intelfb driver

	Recommendation: required

   b. "mode=<xres>x<yres>[-<bpp>][@<refresh>]"
	select mode

	Recommendation: user preference
	(default = 1024x768-32@70)

   c. "vram=<value>"
	select amount of system RAM in MB to allocate for the video memory
	if not enough RAM was already allocated by the BIOS.

	Recommendation: 1 - 4 MB.
	(default = 4 MB)

   d. "voffset=<value>"
	select at what offset in MB of the logical memory to allocate the
	framebuffer memory.  The intent is to avoid the memory blocks
	used by standard graphics applications (XFree86). Depending on your
	usage, adjust the value up or down, (0 for maximum usage, 63/127 MB
	for the least amount).  Note, an arbitrary setting may conflict
	with XFree86.

	Recommendation: do not set
	(default = 48 MB)

   e. "accel"
	enable text acceleration.  This can be enabled/reenabled anytime
	by using 'fbset -accel true/false'.

	Recommendation: enable
	(default = set)

   f. "hwcursor"
	enable cursor acceleration.

	Recommendation: enable
	(default = set)

   g. "mtrr"
	enable MTRR.  This allows data transfers to the framebuffer memory
	to occur in bursts which can significantly increase performance.
	Not very helpful with the intel chips because of 'shared memory'.

	Recommendation: set
	(default = set)

   h. "fixed"
	disable mode switching.

	Recommendation: do not set
	(default = not set)

   The binary parameters can be unset with a "no" prefix, example "noaccel".
   The default parameter (not named) is the mode.

C. Kernel booting
=================

Separate each option/option-pair by commas (,) and the option from its value
with an equals sign (=) as in the following::

	video=intelfb:option1,option2=value2

Sample Usage
------------

In /etc/lilo.conf, add the line::

	append="video=intelfb:mode=800x600-32@75,accel,hwcursor,vram=8"

This will initialize the framebuffer to 800x600 at 32bpp and 75Hz. The
framebuffer will use 8 MB of System RAM. hw acceleration of text and cursor
will be enabled.

Remarks
-------

If setting this parameter doesn't work (you stay in a 80x25 text-mode),
you might need to set the "vga=<mode>" parameter too - see vesafb.txt
in this directory.


D.  Module options
==================

The module parameters are essentially similar to the kernel
parameters. The main difference is that you need to include a Boolean value
(1 for TRUE, and 0 for FALSE) for those options which don't need a value.

Example, to enable MTRR, include "mtrr=1".

Sample Usage
------------

Using the same setup as described above, load the module like this::

	modprobe intelfb mode=800x600-32@75 vram=8 accel=1 hwcursor=1

Or just add the following to a configuration file in /etc/modprobe.d/::

	options intelfb mode=800x600-32@75 vram=8 accel=1 hwcursor=1

and just do a::

	modprobe intelfb


E.  Acknowledgment:
===================

	1.  Geert Uytterhoeven - his excellent howto and the virtual
	    framebuffer driver code made this possible.

	2.  Jeff Hartmann for his agpgart code.

	3.  David Dawes for his original kernel 2.4 code.

	4.  The X developers.  Insights were provided just by reading the
	    XFree86 source code.

	5.  Antonino A. Daplas for his inspiring i810fb driver.

	6.  Andrew Morton for his kernel patches maintenance.

Sylvain
