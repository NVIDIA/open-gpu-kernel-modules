.. SPDX-License-Identifier: GPL-2.0

============================
SCSI subsystem documentation
============================

The Linux Documentation Project (LDP) maintains a document describing
the SCSI subsystem in the Linux kernel (lk) 2.4 series. See:
http://www.tldp.org/HOWTO/SCSI-2.4-HOWTO . The LDP has single
and multiple page HTML renderings as well as postscript and pdf.
It can also be found at:
http://web.archive.org/web/%2E/http://www.torque.net/scsi/SCSI-2.4-HOWTO

Notes on using modules in the SCSI subsystem
============================================
The scsi support in the linux kernel can be modularized in a number of
different ways depending upon the needs of the end user.  To understand
your options, we should first define a few terms.

The scsi-core (also known as the "mid level") contains the core of scsi
support.  Without it you can do nothing with any of the other scsi drivers.
The scsi core support can be a module (scsi_mod.o), or it can be built into
the kernel. If the core is a module, it must be the first scsi module
loaded, and if you unload the modules, it will have to be the last one
unloaded.  In practice the modprobe and rmmod commands (and "autoclean")
will enforce the correct ordering of loading and unloading modules in
the SCSI subsystem.

The individual upper and lower level drivers can be loaded in any order
once the scsi core is present in the kernel (either compiled in or loaded
as a module).  The disk driver (sd_mod.o), cdrom driver (sr_mod.o),
tape driver [1]_ (st.o) and scsi generics driver (sg.o) represent the upper
level drivers to support the various assorted devices which can be
controlled.  You can for example load the tape driver to use the tape drive,
and then unload it once you have no further need for the driver (and release
the associated memory).

The lower level drivers are the ones that support the individual cards that
are supported for the hardware platform that you are running under. Those
individual cards are often called Host Bus Adapters (HBAs). For example the
aic7xxx.o driver is used to control all recent SCSI controller cards from
Adaptec. Almost all lower level drivers can be built either as modules or
built into the kernel.

.. [1] There is a variant of the st driver for controlling OnStream tape
       devices. Its module name is osst.o .

