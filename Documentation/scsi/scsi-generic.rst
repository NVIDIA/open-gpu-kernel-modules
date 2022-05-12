.. SPDX-License-Identifier: GPL-2.0

=======================================
Notes on Linux SCSI Generic (sg) driver
=======================================

                                                        20020126

Introduction
============
The SCSI Generic driver (sg) is one of the four "high level" SCSI device
drivers along with sd, st and sr (disk, tape and CDROM respectively). Sg
is more generalized (but lower level) than its siblings and tends to be
used on SCSI devices that don't fit into the already serviced categories.
Thus sg is used for scanners, CD writers and reading audio CDs digitally
amongst other things.

Rather than document the driver's interface here, version information
is provided plus pointers (i.e. URLs) where to find documentation
and examples.


Major versions of the sg driver
===============================
There are three major versions of sg found in the linux kernel (lk):
      - sg version 1 (original) from 1992 to early 1999 (lk 2.2.5) .
	It is based in the sg_header interface structure.
      - sg version 2 from lk 2.2.6 in the 2.2 series. It is based on
	an extended version of the sg_header interface structure.
      - sg version 3 found in the lk 2.4 series (and the lk 2.5 series).
	It adds the sg_io_hdr interface structure.


Sg driver documentation
=======================
The most recent documentation of the sg driver is kept at the Linux
Documentation Project's (LDP) site:

- http://www.tldp.org/HOWTO/SCSI-Generic-HOWTO

This describes the sg version 3 driver found in the lk 2.4 series.

The LDP renders documents in single and multiple page HTML, postscript
and pdf. This document can also be found at:

- http://sg.danny.cz/sg/p/sg_v3_ho.html

Documentation for the version 2 sg driver found in the lk 2.2 series can
be found at http://sg.danny.cz/sg/. A larger version
is at: http://sg.danny.cz/sg/p/scsi-generic_long.txt.

The original documentation for the sg driver (prior to lk 2.2.6) can be
found at http://www.torque.net/sg/p/original/SCSI-Programming-HOWTO.txt
and in the LDP archives.

A changelog with brief notes can be found in the
/usr/src/linux/include/scsi/sg.h file. Note that the glibc maintainers copy
and edit this file (removing its changelog for example) before placing it
in /usr/include/scsi/sg.h . Driver debugging information and other notes
can be found at the top of the /usr/src/linux/drivers/scsi/sg.c file.

A more general description of the Linux SCSI subsystem of which sg is a
part can be found at http://www.tldp.org/HOWTO/SCSI-2.4-HOWTO .


Example code and utilities
==========================
There are two packages of sg utilities:

    =========   ==========================================================
    sg3_utils   for the sg version 3 driver found in lk 2.4
    sg_utils    for the sg version 2 (and original) driver found in lk 2.2
                and earlier
    =========   ==========================================================

Both packages will work in the lk 2.4 series however sg3_utils offers more
capabilities. They can be found at: http://sg.danny.cz/sg/sg3_utils.html and
freecode.com

Another approach is to look at the applications that use the sg driver.
These include cdrecord, cdparanoia, SANE and cdrdao.


Mapping of Linux kernel versions to sg driver versions
======================================================
Here is a list of linux kernels in the 2.4 series that had new version
of the sg driver:

     - lk 2.4.0 : sg version 3.1.17
     - lk 2.4.7 : sg version 3.1.19
     - lk 2.4.10 : sg version 3.1.20 [#]_
     - lk 2.4.17 : sg version 3.1.22

.. [#] There were 3 changes to sg version 3.1.20 by third parties in the
       next six linux kernel versions.

For reference here is a list of linux kernels in the 2.2 series that had
new version of the sg driver:

     - lk 2.2.0 : original sg version [with no version number]
     - lk 2.2.6 : sg version 2.1.31
     - lk 2.2.8 : sg version 2.1.32
     - lk 2.2.10 : sg version 2.1.34 [SG_GET_VERSION_NUM ioctl first appeared]
     - lk 2.2.14 : sg version 2.1.36
     - lk 2.2.16 : sg version 2.1.38
     - lk 2.2.17 : sg version 2.1.39
     - lk 2.2.20 : sg version 2.1.40

The lk 2.5 development series has recently commenced and it currently
contains sg version 3.5.23 which is functionally equivalent to sg
version 3.1.22 found in lk 2.4.17.


Douglas Gilbert

26th January 2002

dgilbert@interlog.com
