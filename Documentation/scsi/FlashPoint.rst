.. SPDX-License-Identifier: GPL-2.0

===================================
The BusLogic FlashPoint SCSI Driver
===================================

The BusLogic FlashPoint SCSI Host Adapters are now fully supported on Linux.
The upgrade program described below has been officially terminated effective
31 March 1997 since it is no longer needed.

::

  	  MYLEX INTRODUCES LINUX OPERATING SYSTEM SUPPORT FOR ITS
  	      BUSLOGIC FLASHPOINT LINE OF SCSI HOST ADAPTERS


  FREMONT, CA, -- October 8, 1996 -- Mylex Corporation has expanded Linux
  operating system support to its BusLogic brand of FlashPoint Ultra SCSI
  host adapters.  All of BusLogic's other SCSI host adapters, including the
  MultiMaster line, currently support the Linux operating system.  Linux
  drivers and information will be available on October 15th at
  http://sourceforge.net/projects/dandelion/.

  "Mylex is committed to supporting the Linux community," says Peter Shambora,
  vice president of marketing for Mylex.  "We have supported Linux driver
  development and provided technical support for our host adapters for several
  years, and are pleased to now make our FlashPoint products available to this
  user base."

The Linux Operating System
==========================

Linux is a freely-distributed implementation of UNIX for Intel x86, Sun
SPARC, SGI MIPS, Motorola 68k, Digital Alpha AXP and Motorola PowerPC
machines.  It supports a wide range of software, including the X Window
System, Emacs, and TCP/IP networking.  Further information is available at
http://www.linux.org and http://www.ssc.com/.

FlashPoint Host Adapters
========================

The FlashPoint family of Ultra SCSI host adapters, designed for workstation
and file server environments, are available in narrow, wide, dual channel,
and dual channel wide versions.  These adapters feature SeqEngine
automation technology, which minimizes SCSI command overhead and reduces
the number of interrupts generated to the CPU.

About Mylex
===========

Mylex Corporation (NASDAQ/NM SYMBOL: MYLX), founded in 1983, is a leading
producer of RAID technology and network management products.  The company
produces high performance disk array (RAID) controllers, and complementary
computer products for network servers, mass storage systems, workstations
and system boards.  Through its wide range of RAID controllers and its
BusLogic line of Ultra SCSI host adapter products, Mylex provides enabling
intelligent I/O technologies that increase network management control,
enhance CPU utilization, optimize I/O performance, and ensure data security
and availability.  Products are sold globally through a network of OEMs,
major distributors, VARs, and system integrators.  Mylex Corporation is
headquartered at 34551 Ardenwood Blvd., Fremont, CA.

Contact:
========

::

  Peter Shambora
  Vice President of Marketing
  Mylex Corp.
  510/796-6100
  peters@mylex.com


::

			       ANNOUNCEMENT
	       BusLogic FlashPoint LT/BT-948 Upgrade Program
			      1 February 1996

			  ADDITIONAL ANNOUNCEMENT
	       BusLogic FlashPoint LW/BT-958 Upgrade Program
			       14 June 1996

  Ever since its introduction last October, the BusLogic FlashPoint LT has
  been problematic for members of the Linux community, in that no Linux
  drivers have been available for this new Ultra SCSI product.  Despite its
  officially being positioned as a desktop workstation product, and not being
  particularly well suited for a high performance multitasking operating
  system like Linux, the FlashPoint LT has been touted by computer system
  vendors as the latest thing, and has been sold even on many of their high
  end systems, to the exclusion of the older MultiMaster products.  This has
  caused grief for many people who inadvertently purchased a system expecting
  that all BusLogic SCSI Host Adapters were supported by Linux, only to
  discover that the FlashPoint was not supported and would not be for quite
  some time, if ever.

  After this problem was identified, BusLogic contacted its major OEM
  customers to make sure the BT-946C/956C MultiMaster cards would still be
  made available, and that Linux users who mistakenly ordered systems with
  the FlashPoint would be able to upgrade to the BT-946C.  While this helped
  many purchasers of new systems, it was only a partial solution to the
  overall problem of FlashPoint support for Linux users.  It did nothing to
  assist the people who initially purchased a FlashPoint for a supported
  operating system and then later decided to run Linux, or those who had
  ended up with a FlashPoint LT, believing it was supported, and were unable
  to return it.

  In the middle of December, I asked to meet with BusLogic's senior
  management to discuss the issues related to Linux and free software support
  for the FlashPoint.  Rumors of varying accuracy had been circulating
  publicly about BusLogic's attitude toward the Linux community, and I felt
  it was best that these issues be addressed directly.  I sent an email
  message after 11pm one evening, and the meeting took place the next
  afternoon.  Unfortunately, corporate wheels sometimes grind slowly,
  especially when a company is being acquired, and so it's taken until now
  before the details were completely determined and a public statement could
  be made.

  BusLogic is not prepared at this time to release the information necessary
  for third parties to write drivers for the FlashPoint.  The only existing
  FlashPoint drivers have been written directly by BusLogic Engineering, and
  there is no FlashPoint documentation sufficiently detailed to allow outside
  developers to write a driver without substantial assistance.  While there
  are people at BusLogic who would rather not release the details of the
  FlashPoint architecture at all, that debate has not yet been settled either
  way.  In any event, even if documentation were available today it would
  take quite a while for a usable driver to be written, especially since I'm
  not convinced that the effort required would be worthwhile.

  However, BusLogic does remain committed to providing a high performance
  SCSI solution for the Linux community, and does not want to see anyone left
  unable to run Linux because they have a Flashpoint LT.  Therefore, BusLogic
  has put in place a direct upgrade program to allow any Linux user worldwide
  to trade in their FlashPoint LT for the new BT-948 MultiMaster PCI Ultra
  SCSI Host Adapter.  The BT-948 is the Ultra SCSI successor to the BT-946C
  and has all the best features of both the BT-946C and FlashPoint LT,
  including smart termination and a flash PROM for easy firmware updates, and
  is of course compatible with the present Linux driver.  The price for this
  upgrade has been set at US $45 plus shipping and handling, and the upgrade
  program will be administered through BusLogic Technical Support, which can
  be reached by electronic mail at techsup@buslogic.com, by Voice at +1 408
  654-0760, or by FAX at +1 408 492-1542.

  As of 14 June 1996, the original BusLogic FlashPoint LT to BT-948 upgrade
  program has now been extended to encompass the FlashPoint LW Wide Ultra
  SCSI Host Adapter.  Any Linux user worldwide may trade in their FlashPoint
  LW (BT-950) for a BT-958 MultiMaster PCI Ultra SCSI Host Adapter.  The
  price for this upgrade has been set at US $65 plus shipping and handling.

  I was a beta test site for the BT-948/958, and versions 1.2.1 and 1.3.1 of
  my BusLogic driver already included latent support for the BT-948/958.
  Additional cosmetic support for the Ultra SCSI MultiMaster cards was added
  subsequent releases.  As a result of this cooperative testing process,
  several firmware bugs were found and corrected.  My heavily loaded Linux
  test system provided an ideal environment for testing error recovery
  processes that are much more rarely exercised in production systems, but
  are crucial to overall system stability.  It was especially convenient
  being able to work directly with their firmware engineer in demonstrating
  the problems under control of the firmware debugging environment; things
  sure have come a long way since the last time I worked on firmware for an
  embedded system.  I am presently working on some performance testing and
  expect to have some data to report in the not too distant future.

  BusLogic asked me to send this announcement since a large percentage of the
  questions regarding support for the FlashPoint have either been sent to me
  directly via email, or have appeared in the Linux newsgroups in which I
  participate.  To summarize, BusLogic is offering Linux users an upgrade
  from the unsupported FlashPoint LT (BT-930) to the supported BT-948 for US
  $45 plus shipping and handling, or from the unsupported FlashPoint LW
  (BT-950) to the supported BT-958 for $65 plus shipping and handling.
  Contact BusLogic Technical Support at techsup@buslogic.com or +1 408
  654-0760 to take advantage of their offer.

  		Leonard N. Zubkoff
  		lnz@dandelion.com
