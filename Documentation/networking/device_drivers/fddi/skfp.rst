.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

========================
SysKonnect driver - SKFP
========================

|copy| Copyright 1998-2000 SysKonnect,

skfp.txt created 11-May-2000

Readme File for skfp.o v2.06


.. This file contains

   (1) OVERVIEW
   (2) SUPPORTED ADAPTERS
   (3) GENERAL INFORMATION
   (4) INSTALLATION
   (5) INCLUSION OF THE ADAPTER IN SYSTEM START
   (6) TROUBLESHOOTING
   (7) FUNCTION OF THE ADAPTER LEDS
   (8) HISTORY


1. Overview
===========

This README explains how to use the driver 'skfp' for Linux with your
network adapter.

Chapter 2: Contains a list of all network adapters that are supported by
this driver.

Chapter 3:
	   Gives some general information.

Chapter 4: Describes common problems and solutions.

Chapter 5: Shows the changed functionality of the adapter LEDs.

Chapter 6: History of development.


2. Supported adapters
=====================

The network driver 'skfp' supports the following network adapters:
SysKonnect adapters:

  - SK-5521 (SK-NET FDDI-UP)
  - SK-5522 (SK-NET FDDI-UP DAS)
  - SK-5541 (SK-NET FDDI-FP)
  - SK-5543 (SK-NET FDDI-LP)
  - SK-5544 (SK-NET FDDI-LP DAS)
  - SK-5821 (SK-NET FDDI-UP64)
  - SK-5822 (SK-NET FDDI-UP64 DAS)
  - SK-5841 (SK-NET FDDI-FP64)
  - SK-5843 (SK-NET FDDI-LP64)
  - SK-5844 (SK-NET FDDI-LP64 DAS)

Compaq adapters (not tested):

  - Netelligent 100 FDDI DAS Fibre SC
  - Netelligent 100 FDDI SAS Fibre SC
  - Netelligent 100 FDDI DAS UTP
  - Netelligent 100 FDDI SAS UTP
  - Netelligent 100 FDDI SAS Fibre MIC


3. General Information
======================

From v2.01 on, the driver is integrated in the linux kernel sources.
Therefore, the installation is the same as for any other adapter
supported by the kernel.

Refer to the manual of your distribution about the installation
of network adapters.

Makes my life much easier :-)

4. Troubleshooting
==================

If you run into problems during installation, check those items:

Problem:
	  The FDDI adapter cannot be found by the driver.

Reason:
	  Look in /proc/pci for the following entry:

	     'FDDI network controller: SysKonnect SK-FDDI-PCI ...'

	  If this entry exists, then the FDDI adapter has been
	  found by the system and should be able to be used.

	  If this entry does not exist or if the file '/proc/pci'
	  is not there, then you may have a hardware problem or PCI
	  support may not be enabled in your kernel.

	  The adapter can be checked using the diagnostic program
	  which is available from the SysKonnect web site:

	      www.syskonnect.de

	  Some COMPAQ machines have a problem with PCI under
	  Linux. This is described in the 'PCI howto' document
	  (included in some distributions or available from the
	  www, e.g. at 'www.linux.org') and no workaround is available.

Problem:
	  You want to use your computer as a router between
	  multiple IP subnetworks (using multiple adapters), but
	  you cannot reach computers in other subnetworks.

Reason:
	  Either the router's kernel is not configured for IP
	  forwarding or there is a problem with the routing table
	  and gateway configuration in at least one of the
	  computers.

If your problem is not listed here, please contact our
technical support for help.

You can send email to: linux@syskonnect.de

When contacting our technical support,
please ensure that the following information is available:

- System Manufacturer and Model
- Boards in your system
- Distribution
- Kernel version


5. Function of the Adapter LEDs
===============================

	The functionality of the LED's on the FDDI network adapters was
	changed in SMT version v2.82. With this new SMT version, the yellow
	LED works as a ring operational indicator. An active yellow LED
	indicates that the ring is down. The green LED on the adapter now
	works as a link indicator where an active GREEN LED indicates that
	the respective port has a physical connection.

	With versions of SMT prior to v2.82 a ring up was indicated if the
	yellow LED was off while the green LED(s) showed the connection
	status of the adapter. During a ring down the green LED was off and
	the yellow LED was on.

	All implementations indicate that a driver is not loaded if
	all LEDs are off.


6. History
==========

v2.06 (20000511) (In-Kernel version)
    New features:

	- 64 bit support
	- new pci dma interface
	- in kernel 2.3.99

v2.05 (20000217) (In-Kernel version)
    New features:

	- Changes for 2.3.45 kernel

v2.04 (20000207) (Standalone version)
    New features:

	- Added rx/tx byte counter

v2.03 (20000111) (Standalone version)
    Problems fixed:

	- Fixed printk statements from v2.02

v2.02 (991215) (Standalone version)
    Problems fixed:

	- Removed unnecessary output
	- Fixed path for "printver.sh" in makefile

v2.01 (991122) (In-Kernel version)
    New features:

	- Integration in Linux kernel sources
	- Support for memory mapped I/O.

v2.00 (991112)
    New features:

	- Full source released under GPL

v1.05 (991023)
    Problems fixed:

	- Compilation with kernel version 2.2.13 failed

v1.04 (990427)
    Changes:

	- New SMT module included, changing LED functionality

    Problems fixed:

	- Synchronization on SMP machines was buggy

v1.03 (990325)
    Problems fixed:

	- Interrupt routing on SMP machines could be incorrect

v1.02 (990310)
    New features:

	- Support for kernel versions 2.2.x added
	- Kernel patch instead of private duplicate of kernel functions

v1.01 (980812)
    Problems fixed:

	Connection hangup with telnet
	Slow telnet connection

v1.00 beta 01 (980507)
    New features:

	None.

    Problems fixed:

	None.

    Known limitations:

	- tar archive instead of standard package format (rpm).
	- FDDI statistic is empty.
	- not tested with 2.1.xx kernels
	- integration in kernel not tested
	- not tested simultaneously with FDDI adapters from other vendors.
	- only X86 processors supported.
	- SBA (Synchronous Bandwidth Allocator) parameters can
	  not be configured.
	- does not work on some COMPAQ machines. See the PCI howto
	  document for details about this problem.
	- data corruption with kernel versions below 2.0.33.
