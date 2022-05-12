.. include:: <isonum.txt>

----------------------------------
Apple Touchpad Driver (appletouch)
----------------------------------

:Copyright: |copy| 2005 Stelian Pop <stelian@popies.net>

appletouch is a Linux kernel driver for the USB touchpad found on post
February 2005 and October 2005 Apple Aluminium Powerbooks.

This driver is derived from Johannes Berg's appletrackpad driver [#f1]_,
but it has been improved in some areas:

	* appletouch is a full kernel driver, no userspace program is necessary
	* appletouch can be interfaced with the synaptics X11 driver, in order
	  to have touchpad acceleration, scrolling, etc.

Credits go to Johannes Berg for reverse-engineering the touchpad protocol,
Frank Arnold for further improvements, and Alex Harper for some additional
information about the inner workings of the touchpad sensors. Michael
Hanselmann added support for the October 2005 models.

Usage
-----

In order to use the touchpad in the basic mode, compile the driver and load
the module. A new input device will be detected and you will be able to read
the mouse data from /dev/input/mice (using gpm, or X11).

In X11, you can configure the touchpad to use the synaptics X11 driver, which
will give additional functionalities, like acceleration, scrolling, 2 finger
tap for middle button mouse emulation, 3 finger tap for right button mouse
emulation, etc. In order to do this, make sure you're using a recent version of
the synaptics driver (tested with 0.14.2, available from [#f2]_), and configure
a new input device in your X11 configuration file (take a look below for an
example). For additional configuration, see the synaptics driver documentation::

	Section "InputDevice"
		Identifier      "Synaptics Touchpad"
		Driver          "synaptics"
		Option          "SendCoreEvents"        "true"
		Option          "Device"                "/dev/input/mice"
		Option          "Protocol"              "auto-dev"
		Option		"LeftEdge"		"0"
		Option		"RightEdge"		"850"
		Option		"TopEdge"		"0"
		Option		"BottomEdge"		"645"
		Option		"MinSpeed"		"0.4"
		Option		"MaxSpeed"		"1"
		Option		"AccelFactor"		"0.02"
		Option		"FingerLow"		"0"
		Option		"FingerHigh"		"30"
		Option		"MaxTapMove"		"20"
		Option		"MaxTapTime"		"100"
		Option		"HorizScrollDelta"	"0"
		Option		"VertScrollDelta"	"30"
		Option		"SHMConfig"		"on"
	EndSection

	Section "ServerLayout"
		...
		InputDevice	"Mouse"
		InputDevice	"Synaptics Touchpad"
	...
	EndSection

Fuzz problems
-------------

The touchpad sensors are very sensitive to heat, and will generate a lot of
noise when the temperature changes. This is especially true when you power-on
the laptop for the first time.

The appletouch driver tries to handle this noise and auto adapt itself, but it
is not perfect. If finger movements are not recognized anymore, try reloading
the driver.

You can activate debugging using the 'debug' module parameter. A value of 0
deactivates any debugging, 1 activates tracing of invalid samples, 2 activates
full tracing (each sample is being traced)::

	modprobe appletouch debug=1

or::

	echo "1" > /sys/module/appletouch/parameters/debug


.. Links:

.. [#f1] http://johannes.sipsolutions.net/PowerBook/touchpad/

.. [#f2] `<http://web.archive.org/web/*/http://web.telia.com/~u89404340/touchpad/index.html>`_
