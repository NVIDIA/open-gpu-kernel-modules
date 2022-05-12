=======================================
Analog Joystick Support on ALSA Drivers
=======================================

Oct. 14, 2003

Takashi Iwai <tiwai@suse.de>

General
-------

First of all, you need to enable GAMEPORT support on Linux kernel for
using a joystick with the ALSA driver.  For the details of gameport
support, refer to Documentation/input/joydev/joystick.rst.

The joystick support of ALSA drivers is different between ISA and PCI
cards.  In the case of ISA (PnP) cards, it's usually handled by the
independent module (ns558).  Meanwhile, the ALSA PCI drivers have the
built-in gameport support.  Hence, when the ALSA PCI driver is built
in the kernel, CONFIG_GAMEPORT must be 'y', too.  Otherwise, the
gameport support on that card will be (silently) disabled.

Some adapter modules probe the physical connection of the device at
the load time.  It'd be safer to plug in the joystick device before
loading the module.


PCI Cards
---------

For PCI cards, the joystick is enabled when the appropriate module
option is specified.  Some drivers don't need options, and the
joystick support is always enabled.  In the former ALSA version, there
was a dynamic control API for the joystick activation.  It was
changed, however, to the static module options because of the system
stability and the resource management.

The following PCI drivers support the joystick natively.

==============	=============	============================================
Driver		Module Option	Available Values
==============	=============	============================================
als4000		joystick_port	0 = disable (default), 1 = auto-detect,
	                        manual: any address (e.g. 0x200)
au88x0		N/A		N/A
azf3328		joystick	0 = disable, 1 = enable, -1 = auto (default)
ens1370		joystick	0 = disable (default), 1 = enable
ens1371		joystick_port	0 = disable (default), 1 = auto-detect,
	                        manual: 0x200, 0x208, 0x210, 0x218
cmipci		joystick_port	0 = disable (default), 1 = auto-detect,
	                        manual: any address (e.g. 0x200)
cs4281		N/A		N/A
cs46xx		N/A		N/A
es1938		N/A		N/A
es1968		joystick	0 = disable (default), 1 = enable
sonicvibes	N/A		N/A
trident		N/A		N/A
via82xx [#f1]_	joystick	0 = disable (default), 1 = enable
ymfpci		joystick_port	0 = disable (default), 1 = auto-detect,
	                        manual: 0x201, 0x202, 0x204, 0x205 [#f2]_
==============	=============	============================================

.. [#f1] VIA686A/B only
.. [#f2] With YMF744/754 chips, the port address can be chosen arbitrarily

The following drivers don't support gameport natively, but there are
additional modules.  Load the corresponding module to add the gameport
support.

=======	=================
Driver	Additional Module
=======	=================
emu10k1	emu10k1-gp
fm801	fm801-gp
=======	=================

Note: the "pcigame" and "cs461x" modules are for the OSS drivers only.
These ALSA drivers (cs46xx, trident and au88x0) have the
built-in gameport support.

As mentioned above, ALSA PCI drivers have the built-in gameport
support, so you don't have to load ns558 module.  Just load "joydev"
and the appropriate adapter module (e.g. "analog").


ISA Cards
---------

ALSA ISA drivers don't have the built-in gameport support.
Instead, you need to load "ns558" module in addition to "joydev" and
the adapter module (e.g. "analog").
