================================
Intel 810/815 Framebuffer driver
================================

Tony Daplas <adaplas@pol.net>

http://i810fb.sourceforge.net

March 17, 2002

First Released: July 2001
Last Update:    September 12, 2005

A. Introduction
===============

	This is a framebuffer driver for various Intel 810/815 compatible
	graphics devices.  These include:

	- Intel 810
	- Intel 810E
	- Intel 810-DC100
	- Intel 815 Internal graphics only, 100Mhz FSB
	- Intel 815 Internal graphics only
	- Intel 815 Internal graphics and AGP

B.  Features
============

	- Choice of using Discrete Video Timings, VESA Generalized Timing
	  Formula, or a framebuffer specific database to set the video mode

	- Supports a variable range of horizontal and vertical resolution and
	  vertical refresh rates if the VESA Generalized Timing Formula is
	  enabled.

	- Supports color depths of 8, 16, 24 and 32 bits per pixel

	- Supports pseudocolor, directcolor, or truecolor visuals

	- Full and optimized hardware acceleration at 8, 16 and 24 bpp

	- Robust video state save and restore

	- MTRR support

	- Utilizes user-entered monitor specifications to automatically
	  calculate required video mode parameters.

	- Can concurrently run with xfree86 running with native i810 drivers

	- Hardware Cursor Support

	- Supports EDID probing either by DDC/I2C or through the BIOS

C.  List of available options
=============================

   a. "video=i810fb"
	enables the i810 driver

	Recommendation: required

   b. "xres:<value>"
	select horizontal resolution in pixels. (This parameter will be
	ignored if 'mode_option' is specified.  See 'o' below).

	Recommendation: user preference
	(default = 640)

   c. "yres:<value>"
	select vertical resolution in scanlines. If Discrete Video Timings
	is enabled, this will be ignored and computed as 3*xres/4.  (This
	parameter will be ignored if 'mode_option' is specified.  See 'o'
	below)

	Recommendation: user preference
	(default = 480)

   d. "vyres:<value>"
	select virtual vertical resolution in scanlines. If (0) or none
	is specified, this will be computed against maximum available memory.

	Recommendation: do not set
	(default = 480)

   e. "vram:<value>"
	select amount of system RAM in MB to allocate for the video memory

	Recommendation: 1 - 4 MB.
	(default = 4)

   f. "bpp:<value>"
	select desired pixel depth

	Recommendation: 8
	(default = 8)

   g. "hsync1/hsync2:<value>"
	select the minimum and maximum Horizontal Sync Frequency of the
	monitor in kHz.  If using a fixed frequency monitor, hsync1 must
	be equal to hsync2. If EDID probing is successful, these will be
	ignored and values will be taken from the EDID block.

	Recommendation: check monitor manual for correct values
	(default = 29/30)

   h. "vsync1/vsync2:<value>"
	select the minimum and maximum Vertical Sync Frequency of the monitor
	in Hz. You can also use this option to lock your monitor's refresh
	rate. If EDID probing is successful, these will be ignored and values
	will be taken from the EDID block.

	Recommendation: check monitor manual for correct values
	(default = 60/60)

	IMPORTANT:  If you need to clamp your timings, try to give some
	leeway for computational errors (over/underflows).  Example: if
	using vsync1/vsync2 = 60/60, make sure hsync1/hsync2 has at least
	a 1 unit difference, and vice versa.

   i. "voffset:<value>"
	select at what offset in MB of the logical memory to allocate the
	framebuffer memory.  The intent is to avoid the memory blocks
	used by standard graphics applications (XFree86).  The default
	offset (16 MB for a 64 MB aperture, 8 MB for a 32 MB aperture) will
	avoid XFree86's usage and allows up to 7 MB/15 MB of framebuffer
	memory.  Depending on your usage, adjust the value up or down
	(0 for maximum usage, 31/63 MB for the least amount).  Note, an
	arbitrary setting may conflict with XFree86.

	Recommendation: do not set
	(default = 8 or 16 MB)

   j. "accel"
	enable text acceleration.  This can be enabled/reenabled anytime
	by using 'fbset -accel true/false'.

	Recommendation: enable
	(default = not set)

   k. "mtrr"
	enable MTRR.  This allows data transfers to the framebuffer memory
	to occur in bursts which can significantly increase performance.
	Not very helpful with the i810/i815 because of 'shared memory'.

	Recommendation: do not set
	(default = not set)

   l. "extvga"
	if specified, secondary/external VGA output will always be enabled.
	Useful if the BIOS turns off the VGA port when no monitor is attached.
	The external VGA monitor can then be attached without rebooting.

	Recommendation: do not set
	(default = not set)

   m. "sync"
	Forces the hardware engine to do a "sync" or wait for the hardware
	to finish before starting another instruction. This will produce a
	more stable setup, but will be slower.

	Recommendation: do not set
	(default = not set)

   n. "dcolor"
	Use directcolor visual instead of truecolor for pixel depths greater
	than 8 bpp.  Useful for color tuning, such as gamma control.

	Recommendation: do not set
	(default = not set)

   o. <xres>x<yres>[-<bpp>][@<refresh>]
	The driver will now accept specification of boot mode option.  If this
	is specified, the options 'xres' and 'yres' will be ignored. See
	Documentation/fb/modedb.rst for usage.

D. Kernel booting
=================

Separate each option/option-pair by commas (,) and the option from its value
with a colon (:) as in the following::

	video=i810fb:option1,option2:value2

Sample Usage
------------

In /etc/lilo.conf, add the line::

  append="video=i810fb:vram:2,xres:1024,yres:768,bpp:8,hsync1:30,hsync2:55, \
	  vsync1:50,vsync2:85,accel,mtrr"

This will initialize the framebuffer to 1024x768 at 8bpp.  The framebuffer
will use 2 MB of System RAM. MTRR support will be enabled. The refresh rate
will be computed based on the hsync1/hsync2 and vsync1/vsync2 values.

IMPORTANT:
  You must include hsync1, hsync2, vsync1 and vsync2 to enable video modes
  better than 640x480 at 60Hz. HOWEVER, if your chipset/display combination
  supports I2C and has an EDID block, you can safely exclude hsync1, hsync2,
  vsync1 and vsync2 parameters.  These parameters will be taken from the EDID
  block.

E.  Module options
==================

The module parameters are essentially similar to the kernel
parameters. The main difference is that you need to include a Boolean value
(1 for TRUE, and 0 for FALSE) for those options which don't need a value.

Example, to enable MTRR, include "mtrr=1".

Sample Usage
------------

Using the same setup as described above, load the module like this::

	modprobe i810fb vram=2 xres=1024 bpp=8 hsync1=30 hsync2=55 vsync1=50 \
		 vsync2=85 accel=1 mtrr=1

Or just add the following to a configuration file in /etc/modprobe.d/::

	options i810fb vram=2 xres=1024 bpp=16 hsync1=30 hsync2=55 vsync1=50 \
	vsync2=85 accel=1 mtrr=1

and just do a::

	modprobe i810fb


F.  Setup
=========

	a. Do your usual method of configuring the kernel

	   make menuconfig/xconfig/config

	b. Under "Code maturity level options" enable "Prompt for development
	   and/or incomplete code/drivers".

	c. Enable agpgart support for the Intel 810/815 on-board graphics.
	   This is required.  The option is under "Character Devices".

	d. Under "Graphics Support", select "Intel 810/815" either statically
	   or as a module.  Choose "use VESA Generalized Timing Formula" if
	   you need to maximize the capability of your display.  To be on the
	   safe side, you can leave this unselected.

	e. If you want support for DDC/I2C probing (Plug and Play Displays),
	   set 'Enable DDC Support' to 'y'. To make this option appear, set
	   'use VESA Generalized Timing Formula' to 'y'.

	f. If you want a framebuffer console, enable it under "Console
	   Drivers".

	g. Compile your kernel.

	h. Load the driver as described in sections D and E.

	i.  Try the DirectFB (http://www.directfb.org) + the i810 gfxdriver
	    patch to see the chipset in action (or inaction :-).

G.  Acknowledgment:
===================

	1.  Geert Uytterhoeven - his excellent howto and the virtual
	    framebuffer driver code made this possible.

	2.  Jeff Hartmann for his agpgart code.

	3.  The X developers.  Insights were provided just by reading the
	    XFree86 source code.

	4.  Intel(c).  For this value-oriented chipset driver and for
	    providing documentation.

	5. Matt Sottek.  His inputs and ideas  helped in making some
	   optimizations possible.

H.  Home Page:
==============

	A more complete, and probably updated information is provided at
	http://i810fb.sourceforge.net.

Tony
