=========================================
Sony Notebook Control Driver (SNC) Readme
=========================================

	- Copyright (C) 2004- 2005 Stelian Pop <stelian@popies.net>
	- Copyright (C) 2007 Mattia Dongili <malattia@linux.it>

This mini-driver drives the SNC and SPIC device present in the ACPI BIOS of the
Sony Vaio laptops. This driver mixes both devices functions under the same
(hopefully consistent) interface. This also means that the sonypi driver is
obsoleted by sony-laptop now.

Fn keys (hotkeys):
------------------

Some models report hotkeys through the SNC or SPIC devices, such events are
reported both through the ACPI subsystem as acpi events and through the INPUT
subsystem. See the logs of /proc/bus/input/devices to find out what those
events are and which input devices are created by the driver.
Additionally, loading the driver with the debug option will report all events
in the kernel log.

The "scancodes" passed to the input system (that can be remapped with udev)
are indexes to the table "sony_laptop_input_keycode_map" in the sony-laptop.c
module.  For example the "FN/E" key combination (EJECTCD on some models)
generates the scancode 20 (0x14).

Backlight control:
------------------
If your laptop model supports it, you will find sysfs files in the
/sys/class/backlight/sony/
directory. You will be able to query and set the current screen
brightness:

	======================	=========================================
	brightness		get/set screen brightness (an integer
				between 0 and 7)
	actual_brightness	reading from this file will query the HW
				to get real brightness value
	max_brightness		the maximum brightness value
	======================	=========================================


Platform specific:
------------------
Loading the sony-laptop module will create a
/sys/devices/platform/sony-laptop/
directory populated with some files.

You then read/write integer values from/to those files by using
standard UNIX tools.

The files are:

	======================	==========================================
	brightness_default	screen brightness which will be set
				when the laptop will be rebooted
	cdpower			power on/off the internal CD drive
	audiopower		power on/off the internal sound card
	lanpower		power on/off the internal ethernet card
				(only in debug mode)
	bluetoothpower		power on/off the internal bluetooth device
	fanspeed		get/set the fan speed
	======================	==========================================

Note that some files may be missing if they are not supported
by your particular laptop model.

Example usage::

	# echo "1" > /sys/devices/platform/sony-laptop/brightness_default

sets the lowest screen brightness for the next and later reboots

::

	# echo "8" > /sys/devices/platform/sony-laptop/brightness_default

sets the highest screen brightness for the next and later reboots

::

	# cat /sys/devices/platform/sony-laptop/brightness_default

retrieves the value

::

	# echo "0" > /sys/devices/platform/sony-laptop/audiopower

powers off the sound card

::

	# echo "1" > /sys/devices/platform/sony-laptop/audiopower

powers on the sound card.


RFkill control:
---------------
More recent Vaio models expose a consistent set of ACPI methods to
control radio frequency emitting devices. If you are a lucky owner of
such a laptop you will find the necessary rfkill devices under
/sys/class/rfkill. Check those starting with sony-* in::

	# grep . /sys/class/rfkill/*/{state,name}


Development:
------------

If you want to help with the development of this driver (and
you are not afraid of any side effects doing strange things with
your ACPI BIOS could have on your laptop), load the driver and
pass the option 'debug=1'.

REPEAT:
	**DON'T DO THIS IF YOU DON'T LIKE RISKY BUSINESS.**

In your kernel logs you will find the list of all ACPI methods
the SNC device has on your laptop.

* For new models you will see a long list of meaningless method names,
  reading the DSDT table source should reveal that:

(1) the SNC device uses an internal capability lookup table
(2) SN00 is used to find values in the lookup table
(3) SN06 and SN07 are used to call into the real methods based on
    offsets you can obtain iterating the table using SN00
(4) SN02 used to enable events.

Some values in the capability lookup table are more or less known, see
the code for all sony_call_snc_handle calls, others are more obscure.

* For old models you can see the GCDP/GCDP methods used to pwer on/off
  the CD drive, but there are others and they are usually different from
  model to model.

**I HAVE NO IDEA WHAT THOSE METHODS DO.**

The sony-laptop driver creates, for some of those methods (the most
current ones found on several Vaio models), an entry under
/sys/devices/platform/sony-laptop, just like the 'cdpower' one.
You can create other entries corresponding to your own laptop methods by
further editing the source (see the 'sony_nc_values' table, and add a new
entry to this table with your get/set method names using the
SNC_HANDLE_NAMES macro).

Your mission, should you accept it, is to try finding out what
those entries are for, by reading/writing random values from/to those
files and find out what is the impact on your laptop.

Should you find anything interesting, please report it back to me,
I will not disavow all knowledge of your actions :)

See also http://www.linux.it/~malattia/wiki/index.php/Sony_drivers for other
useful info.

Bugs/Limitations:
-----------------

* This driver is not based on official documentation from Sony
  (because there is none), so there is no guarantee this driver
  will work at all, or do the right thing. Although this hasn't
  happened to me, this driver could do very bad things to your
  laptop, including permanent damage.

* The sony-laptop and sonypi drivers do not interact at all. In the
  future, sonypi will be removed and replaced by sony-laptop.

* spicctrl, which is the userspace tool used to communicate with the
  sonypi driver (through /dev/sonypi) is deprecated as well since all
  its features are now available under the sysfs tree via sony-laptop.
