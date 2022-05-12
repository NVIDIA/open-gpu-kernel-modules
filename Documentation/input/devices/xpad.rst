=======================================================
xpad - Linux USB driver for Xbox compatible controllers
=======================================================

This driver exposes all first-party and third-party Xbox compatible
controllers. It has a long history and has enjoyed considerable usage
as Window's xinput library caused most PC games to focus on Xbox
controller compatibility.

Due to backwards compatibility all buttons are reported as digital.
This only effects Original Xbox controllers. All later controller models
have only digital face buttons.

Rumble is supported on some models of Xbox 360 controllers but not of
Original Xbox controllers nor on Xbox One controllers. As of writing
the Xbox One's rumble protocol has not been reverse engineered but in
the future could be supported.


Notes
=====

The number of buttons/axes reported varies based on 3 things:

- if you are using a known controller
- if you are using a known dance pad
- if using an unknown device (one not listed below), what you set in the
  module configuration for "Map D-PAD to buttons rather than axes for unknown
  pads" (module option dpad_to_buttons)

If you set dpad_to_buttons to N and you are using an unknown device
the driver will map the directional pad to axes (X/Y).
If you said Y it will map the d-pad to buttons, which is needed for dance
style games to function correctly. The default is Y.

dpad_to_buttons has no effect for known pads. A erroneous commit message
claimed dpad_to_buttons could be used to force behavior on known devices.
This is not true. Both dpad_to_buttons and triggers_to_buttons only affect
unknown controllers.


Normal Controllers
------------------

With a normal controller, the directional pad is mapped to its own X/Y axes.
The jstest-program from joystick-1.2.15 (jstest-version 2.1.0) will report 8
axes and 10 buttons.

All 8 axes work, though they all have the same range (-32768..32767)
and the zero-setting is not correct for the triggers (I don't know if that
is some limitation of jstest, since the input device setup should be fine. I
didn't have a look at jstest itself yet).

All of the 10 buttons work (in digital mode). The six buttons on the
right side (A, B, X, Y, black, white) are said to be "analog" and
report their values as 8 bit unsigned, not sure what this is good for.

I tested the controller with quake3, and configuration and
in game functionality were OK. However, I find it rather difficult to
play first person shooters with a pad. Your mileage may vary.


Xbox Dance Pads
---------------

When using a known dance pad, jstest will report 6 axes and 14 buttons.

For dance style pads (like the redoctane pad) several changes
have been made.  The old driver would map the d-pad to axes, resulting
in the driver being unable to report when the user was pressing both
left+right or up+down, making DDR style games unplayable.

Known dance pads automatically map the d-pad to buttons and will work
correctly out of the box.

If your dance pad is recognized by the driver but is using axes instead
of buttons, see section 0.3 - Unknown Controllers

I've tested this with Stepmania, and it works quite well.


Unknown Controllers
-------------------

If you have an unknown xbox controller, it should work just fine with
the default settings.

HOWEVER if you have an unknown dance pad not listed below, it will not
work UNLESS you set "dpad_to_buttons" to 1 in the module configuration.


USB adapters
============

All generations of Xbox controllers speak USB over the wire.

- Original Xbox controllers use a proprietary connector and require adapters.
- Wireless Xbox 360 controllers require a 'Xbox 360 Wireless Gaming Receiver
  for Windows'
- Wired Xbox 360 controllers use standard USB connectors.
- Xbox One controllers can be wireless but speak Wi-Fi Direct and are not
  yet supported.
- Xbox One controllers can be wired and use standard Micro-USB connectors.



Original Xbox USB adapters
--------------------------

Using this driver with an Original Xbox controller requires an
adapter cable to break out the proprietary connector's pins to USB.
You can buy these online fairly cheap, or build your own.

Such a cable is pretty easy to build. The Controller itself is a USB
compound device (a hub with three ports for two expansion slots and
the controller device) with the only difference in a nonstandard connector
(5 pins vs. 4 on standard USB 1.0 connectors).

You just need to solder a USB connector onto the cable and keep the
yellow wire unconnected. The other pins have the same order on both
connectors so there is no magic to it. Detailed info on these matters
can be found on the net ([1]_, [2]_, [3]_).

Thanks to the trip splitter found on the cable you don't even need to cut the
original one. You can buy an extension cable and cut that instead. That way,
you can still use the controller with your X-Box, if you have one ;)



Driver Installation
===================

Once you have the adapter cable, if needed, and the controller connected
the xpad module should be auto loaded. To confirm you can cat
/sys/kernel/debug/usb/devices. There should be an entry like those:

.. code-block:: none
   :caption: dump from InterAct PowerPad Pro (Germany)

    T:  Bus=01 Lev=03 Prnt=04 Port=00 Cnt=01 Dev#=  5 Spd=12  MxCh= 0
    D:  Ver= 1.10 Cls=00(>ifc ) Sub=00 Prot=00 MxPS=32 #Cfgs=  1
    P:  Vendor=05fd ProdID=107a Rev= 1.00
    C:* #Ifs= 1 Cfg#= 1 Atr=80 MxPwr=100mA
    I:  If#= 0 Alt= 0 #EPs= 2 Cls=58(unk. ) Sub=42 Prot=00 Driver=(none)
    E:  Ad=81(I) Atr=03(Int.) MxPS=  32 Ivl= 10ms
    E:  Ad=02(O) Atr=03(Int.) MxPS=  32 Ivl= 10ms

.. code-block:: none
   :caption: dump from Redoctane Xbox Dance Pad (US)

    T:  Bus=01 Lev=02 Prnt=09 Port=00 Cnt=01 Dev#= 10 Spd=12  MxCh= 0
    D:  Ver= 1.10 Cls=00(>ifc ) Sub=00 Prot=00 MxPS= 8 #Cfgs=  1
    P:  Vendor=0c12 ProdID=8809 Rev= 0.01
    S:  Product=XBOX DDR
    C:* #Ifs= 1 Cfg#= 1 Atr=80 MxPwr=100mA
    I:  If#= 0 Alt= 0 #EPs= 2 Cls=58(unk. ) Sub=42 Prot=00 Driver=xpad
    E:  Ad=82(I) Atr=03(Int.) MxPS=  32 Ivl=4ms
    E:  Ad=02(O) Atr=03(Int.) MxPS=  32 Ivl=4ms


Supported Controllers
=====================

For a full list of supported controllers and associated vendor and product
IDs see the xpad_device[] array\ [4]_.

As of the historic version 0.0.6 (2006-10-10) the following devices
were supported::

 original Microsoft XBOX controller (US),    vendor=0x045e, product=0x0202
 smaller  Microsoft XBOX controller (US),    vendor=0x045e, product=0x0289
 original Microsoft XBOX controller (Japan), vendor=0x045e, product=0x0285
 InterAct PowerPad Pro (Germany),            vendor=0x05fd, product=0x107a
 RedOctane Xbox Dance Pad (US),              vendor=0x0c12, product=0x8809

Unrecognized models of Xbox controllers should function as Generic
Xbox controllers. Unrecognized Dance Pad controllers require setting
the module option 'dpad_to_buttons'.

If you have an unrecognized controller please see 0.3 - Unknown Controllers


Manual Testing
==============

To test this driver's functionality you may use 'jstest'.

For example::

    > modprobe xpad
    > modprobe joydev
    > jstest /dev/js0

If you're using a normal controller, there should be a single line showing
18 inputs (8 axes, 10 buttons), and its values should change if you move
the sticks and push the buttons.  If you're using a dance pad, it should
show 20 inputs (6 axes, 14 buttons).

It works? Voila, you're done ;)



Thanks
======

I have to thank ITO Takayuki for the detailed info on his site
    http://euc.jp/periphs/xbox-controller.ja.html.

His useful info and both the usb-skeleton as well as the iforce input driver
(Greg Kroah-Hartmann; Vojtech Pavlik) helped a lot in rapid prototyping
the basic functionality.



References
==========

.. [1] http://euc.jp/periphs/xbox-controller.ja.html (ITO Takayuki)
.. [2] http://xpad.xbox-scene.com/
.. [3] http://www.markosweb.com/www/xboxhackz.com/
.. [4] https://elixir.bootlin.com/linux/latest/ident/xpad_device


Historic Edits
==============

2002-07-16 - Marko Friedemann <mfr@bmx-chemnitz.de>
 - original doc

2005-03-19 - Dominic Cerquetti <binary1230@yahoo.com>
 - added stuff for dance pads, new d-pad->axes mappings

Later changes may be viewed with
'git log --follow Documentation/input/devices/xpad.rst'
