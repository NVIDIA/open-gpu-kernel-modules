.. SPDX-License-Identifier: GPL-2.0

=============================================
FORE Systems PCA-200E/SBA-200E ATM NIC driver
=============================================

This driver adds support for the FORE Systems 200E-series ATM adapters
to the Linux operating system. It is based on the earlier PCA-200E driver
written by Uwe Dannowski.

The driver simultaneously supports PCA-200E and SBA-200E adapters on
i386, alpha (untested), powerpc, sparc and sparc64 archs.

The intent is to enable the use of different models of FORE adapters at the
same time, by hosts that have several bus interfaces (such as PCI+SBUS,
or PCI+EISA).

Only PCI and SBUS devices are currently supported by the driver, but support
for other bus interfaces such as EISA should not be too hard to add.


Firmware Copyright Notice
-------------------------

Please read the fore200e_firmware_copyright file present
in the linux/drivers/atm directory for details and restrictions.


Firmware Updates
----------------

The FORE Systems 200E-series driver is shipped with firmware data being
uploaded to the ATM adapters at system boot time or at module loading time.
The supplied firmware images should work with all adapters.

However, if you encounter problems (the firmware doesn't start or the driver
is unable to read the PROM data), you may consider trying another firmware
version. Alternative binary firmware images can be found somewhere on the
ForeThought CD-ROM supplied with your adapter by FORE Systems.

You can also get the latest firmware images from FORE Systems at
https://en.wikipedia.org/wiki/FORE_Systems. Register TACTics Online and go to
the 'software updates' pages. The firmware binaries are part of
the various ForeThought software distributions.

Notice that different versions of the PCA-200E firmware exist, depending
on the endianness of the host architecture. The driver is shipped with
both little and big endian PCA firmware images.

Name and location of the new firmware images can be set at kernel
configuration time:

1. Copy the new firmware binary files (with .bin, .bin1 or .bin2 suffix)
   to some directory, such as linux/drivers/atm.

2. Reconfigure your kernel to set the new firmware name and location.
   Expected pathnames are absolute or relative to the drivers/atm directory.

3. Rebuild and re-install your kernel or your module.


Feedback
--------

Feedback is welcome. Please send success stories/bug reports/
patches/improvement/comments/flames to <lizzi@cnam.fr>.
