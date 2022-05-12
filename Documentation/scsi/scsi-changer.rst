.. SPDX-License-Identifier: GPL-2.0

========================================
README for the SCSI media changer driver
========================================

This is a driver for SCSI Medium Changer devices, which are listed
with "Type: Medium Changer" in /proc/scsi/scsi.

This is for *real* Jukeboxes.  It is *not* supported to work with
common small CD-ROM changers, neither one-lun-per-slot SCSI changers
nor IDE drives.

Userland tools available from here:
	http://linux.bytesex.org/misc/changer.html


General Information
-------------------

First some words about how changers work: A changer has 2 (possibly
more) SCSI ID's. One for the changer device which controls the robot,
and one for the device which actually reads and writes the data. The
later may be anything, a MOD, a CD-ROM, a tape or whatever. For the
changer device this is a "don't care", he *only* shuffles around the
media, nothing else.


The SCSI changer model is complex, compared to - for example - IDE-CD
changers. But it allows to handle nearly all possible cases. It knows
4 different types of changer elements:

  ===============   ==================================================
  media transport   this one shuffles around the media, i.e. the
                    transport arm.  Also known as "picker".
  storage           a slot which can hold a media.
  import/export     the same as above, but is accessible from outside,
                    i.e. there the operator (you !) can use this to
                    fill in and remove media from the changer.
		    Sometimes named "mailslot".
  data transfer     this is the device which reads/writes, i.e. the
		    CD-ROM / Tape / whatever drive.
  ===============   ==================================================

None of these is limited to one: A huge Jukebox could have slots for
123 CD-ROM's, 5 CD-ROM readers (and therefore 6 SCSI ID's: the changer
and each CD-ROM) and 2 transport arms. No problem to handle.


How it is implemented
---------------------

I implemented the driver as character device driver with a NetBSD-like
ioctl interface. Just grabbed NetBSD's header file and one of the
other linux SCSI device drivers as starting point. The interface
should be source code compatible with NetBSD. So if there is any
software (anybody knows ???) which supports a BSDish changer driver,
it should work with this driver too.

Over time a few more ioctls where added, volume tag support for example
wasn't covered by the NetBSD ioctl API.


Current State
-------------

Support for more than one transport arm is not implemented yet (and
nobody asked for it so far...).

I test and use the driver myself with a 35 slot cdrom jukebox from
Grundig.  I got some reports telling it works ok with tape autoloaders
(Exabyte, HP and DEC).  Some People use this driver with amanda.  It
works fine with small (11 slots) and a huge (4 MOs, 88 slots)
magneto-optical Jukebox.  Probably with lots of other changers too, most
(but not all :-) people mail me only if it does *not* work...

I don't have any device lists, neither black-list nor white-list.  Thus
it is quite useless to ask me whenever a specific device is supported or
not.  In theory every changer device which supports the SCSI-2 media
changer command set should work out-of-the-box with this driver.  If it
doesn't, it is a bug.  Either within the driver or within the firmware
of the changer device.


Using it
--------

This is a character device with major number is 86, so use
"mknod /dev/sch0 c 86 0" to create the special file for the driver.

If the module finds the changer, it prints some messages about the
device [ try "dmesg" if you don't see anything ] and should show up in
/proc/devices. If not....  some changers use ID ? / LUN 0 for the
device and ID ? / LUN 1 for the robot mechanism. But Linux does *not*
look for LUNs other than 0 as default, because there are too many
broken devices. So you can try:

  1) echo "scsi add-single-device 0 0 ID 1" > /proc/scsi/scsi
     (replace ID with the SCSI-ID of the device)
  2) boot the kernel with "max_scsi_luns=1" on the command line
     (append="max_scsi_luns=1" in lilo.conf should do the trick)


Trouble?
--------

If you insmod the driver with "insmod debug=1", it will be verbose and
prints a lot of stuff to the syslog.  Compiling the kernel with
CONFIG_SCSI_CONSTANTS=y improves the quality of the error messages a lot
because the kernel will translate the error codes into human-readable
strings then.

You can display these messages with the dmesg command (or check the
logfiles).  If you email me some question because of a problem with the
driver, please include these messages.


Insmod options
--------------

debug=0/1
	Enable debug messages (see above, default: 0).

verbose=0/1
	Be verbose (default: 1).

init=0/1
	Send INITIALIZE ELEMENT STATUS command to the changer
	at insmod time (default: 1).

timeout_init=<seconds>
	timeout for the INITIALIZE ELEMENT STATUS command
	(default: 3600).

timeout_move=<seconds>
	timeout for all other commands (default: 120).

dt_id=<id1>,<id2>,... / dt_lun=<lun1>,<lun2>,...
	These two allow to specify the SCSI ID and LUN for the data
	transfer elements.  You likely don't need this as the jukebox
	should provide this information.  But some devices don't ...

vendor_firsts=, vendor_counts=, vendor_labels=
	These insmod options can be used to tell the driver that there
	are some vendor-specific element types.  Grundig for example
	does this.  Some jukeboxes have a printer to label fresh burned
	CDs, which is addressed as element 0xc000 (type 5).  To tell the
	driver about this vendor-specific element, use this::

		$ insmod ch			\
			vendor_firsts=0xc000	\
			vendor_counts=1		\
			vendor_labels=printer

	All three insmod options accept up to four comma-separated
	values, this way you can configure the element types 5-8.
	You likely need the SCSI specs for the device in question to
	find the correct values as they are not covered by the SCSI-2
	standard.


Credits
-------

I wrote this driver using the famous mailing-patches-around-the-world
method.  With (more or less) help from:

	- Daniel Moehwald <moehwald@hdg.de>
	- Dane Jasper <dane@sonic.net>
	- R. Scott Bailey <sbailey@dsddi.eds.com>
	- Jonathan Corbet <corbet@lwn.net>

Special thanks go to

	- Martin Kuehne <martin.kuehne@bnbt.de>

for a old, second-hand (but full functional) cdrom jukebox which I use
to develop/test driver and tools now.

Have fun,

   Gerd

Gerd Knorr <kraxel@bytesex.org>
