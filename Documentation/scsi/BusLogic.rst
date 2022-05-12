.. SPDX-License-Identifier: GPL-2.0

=========================================================
BusLogic MultiMaster and FlashPoint SCSI Driver for Linux
=========================================================

			 Version 2.0.15 for Linux 2.0

			 Version 2.1.15 for Linux 2.1

			      PRODUCTION RELEASE

				17 August 1998

			       Leonard N. Zubkoff

			       Dandelion Digital

			       lnz@dandelion.com

	 Copyright 1995-1998 by Leonard N. Zubkoff <lnz@dandelion.com>


Introduction
============

BusLogic, Inc. designed and manufactured a variety of high performance SCSI
host adapters which share a common programming interface across a diverse
collection of bus architectures by virtue of their MultiMaster ASIC technology.
BusLogic was acquired by Mylex Corporation in February 1996, but the products
supported by this driver originated under the BusLogic name and so that name is
retained in the source code and documentation.

This driver supports all present BusLogic MultiMaster Host Adapters, and should
support any future MultiMaster designs with little or no modification.  More
recently, BusLogic introduced the FlashPoint Host Adapters, which are less
costly and rely on the host CPU, rather than including an onboard processor.
Despite not having an onboard CPU, the FlashPoint Host Adapters perform very
well and have very low command latency.  BusLogic has recently provided me with
the FlashPoint Driver Developer's Kit, which comprises documentation and freely
redistributable source code for the FlashPoint SCCB Manager.  The SCCB Manager
is the library of code that runs on the host CPU and performs functions
analogous to the firmware on the MultiMaster Host Adapters.  Thanks to their
having provided the SCCB Manager, this driver now supports the FlashPoint Host
Adapters as well.

My primary goals in writing this completely new BusLogic driver for Linux are
to achieve the full performance that BusLogic SCSI Host Adapters and modern
SCSI peripherals are capable of, and to provide a highly robust driver that can
be depended upon for high performance mission critical applications.  All of
the major performance features can be configured from the Linux kernel command
line or at module initialization time, allowing individual installations to
tune driver performance and error recovery to their particular needs.

The latest information on Linux support for BusLogic SCSI Host Adapters, as
well as the most recent release of this driver and the latest firmware for the
BT-948/958/958D, will always be available from my Linux Home Page at URL
"http://sourceforge.net/projects/dandelion/".

Bug reports should be sent via electronic mail to "lnz@dandelion.com".  Please
include with the bug report the complete configuration messages reported by the
driver and SCSI subsystem at startup, along with any subsequent system messages
relevant to SCSI operations, and a detailed description of your system's
hardware configuration.

Mylex has been an excellent company to work with and I highly recommend their
products to the Linux community.  In November 1995, I was offered the
opportunity to become a beta test site for their latest MultiMaster product,
the BT-948 PCI Ultra SCSI Host Adapter, and then again for the BT-958 PCI Wide
Ultra SCSI Host Adapter in January 1996.  This was mutually beneficial since
Mylex received a degree and kind of testing that their own testing group cannot
readily achieve, and the Linux community has available high performance host
adapters that have been well tested with Linux even before being brought to
market.  This relationship has also given me the opportunity to interact
directly with their technical staff, to understand more about the internal
workings of their products, and in turn to educate them about the needs and
potential of the Linux community.

More recently, Mylex has reaffirmed the company's interest in supporting the
Linux community, and I am now working on a Linux driver for the DAC960 PCI RAID
Controllers.  Mylex's interest and support is greatly appreciated.

Unlike some other vendors, if you contact Mylex Technical Support with a
problem and are running Linux, they will not tell you that your use of their
products is unsupported.  Their latest product marketing literature even states
"Mylex SCSI host adapters are compatible with all major operating systems
including: ... Linux ...".

Mylex Corporation is located at 34551 Ardenwood Blvd., Fremont, California
94555, USA and can be reached at 510/796-6100 or on the World Wide Web at
http://www.mylex.com.  Mylex HBA Technical Support can be reached by electronic
mail at techsup@mylex.com, by Voice at 510/608-2400, or by FAX at 510/745-7715.
Contact information for offices in Europe and Japan is available on the Web
site.


Driver Features
===============

Configuration Reporting and Testing
-----------------------------------

  During system initialization, the driver reports extensively on the host
  adapter hardware configuration, including the synchronous transfer parameters
  requested and negotiated with each target device.  AutoSCSI settings for
  Synchronous Negotiation, Wide Negotiation, and Disconnect/Reconnect are
  reported for each target device, as well as the status of Tagged Queuing.
  If the same setting is in effect for all target devices, then a single word
  or phrase is used; otherwise, a letter is provided for each target device to
  indicate the individual status.  The following examples
  should clarify this reporting format:

    Synchronous Negotiation: Ultra

      Synchronous negotiation is enabled for all target devices and the host
      adapter will attempt to negotiate for 20.0 mega-transfers/second.

    Synchronous Negotiation: Fast

      Synchronous negotiation is enabled for all target devices and the host
      adapter will attempt to negotiate for 10.0 mega-transfers/second.

    Synchronous Negotiation: Slow

      Synchronous negotiation is enabled for all target devices and the host
      adapter will attempt to negotiate for 5.0 mega-transfers/second.

    Synchronous Negotiation: Disabled

      Synchronous negotiation is disabled and all target devices are limited to
      asynchronous operation.

    Synchronous Negotiation: UFSNUUU#UUUUUUUU

      Synchronous negotiation to Ultra speed is enabled for target devices 0
      and 4 through 15, to Fast speed for target device 1, to Slow speed for
      target device 2, and is not permitted to target device 3.  The host
      adapter's SCSI ID is represented by the "#".

    The status of Wide Negotiation, Disconnect/Reconnect, and Tagged Queuing
    are reported as "Enabled", Disabled", or a sequence of "Y" and "N" letters.

Performance Features
--------------------

  BusLogic SCSI Host Adapters directly implement SCSI-2 Tagged Queuing, and so
  support has been included in the driver to utilize tagged queuing with any
  target devices that report having the tagged queuing capability.  Tagged
  queuing allows for multiple outstanding commands to be issued to each target
  device or logical unit, and can improve I/O performance substantially.  In
  addition, BusLogic's Strict Round Robin Mode is used to optimize host adapter
  performance, and scatter/gather I/O can support as many segments as can be
  effectively utilized by the Linux I/O subsystem.  Control over the use of
  tagged queuing for each target device as well as individual selection of the
  tagged queue depth is available through driver options provided on the kernel
  command line or at module initialization time.  By default, the queue depth
  is determined automatically based on the host adapter's total queue depth and
  the number, type, speed, and capabilities of the target devices found.  In
  addition, tagged queuing is automatically disabled whenever the host adapter
  firmware version is known not to implement it correctly, or whenever a tagged
  queue depth of 1 is selected.  Tagged queuing is also disabled for individual
  target devices if disconnect/reconnect is disabled for that device.

Robustness Features
-------------------

  The driver implements extensive error recovery procedures.  When the higher
  level parts of the SCSI subsystem request that a timed out command be reset,
  a selection is made between a full host adapter hard reset and SCSI bus reset
  versus sending a bus device reset message to the individual target device
  based on the recommendation of the SCSI subsystem.  Error recovery strategies
  are selectable through driver options individually for each target device,
  and also include sending a bus device reset to the specific target device
  associated with the command being reset, as well as suppressing error
  recovery entirely to avoid perturbing an improperly functioning device.  If
  the bus device reset error recovery strategy is selected and sending a bus
  device reset does not restore correct operation, the next command that is
  reset will force a full host adapter hard reset and SCSI bus reset.  SCSI bus
  resets caused by other devices and detected by the host adapter are also
  handled by issuing a soft reset to the host adapter and re-initialization.
  Finally, if tagged queuing is active and more than one command reset occurs
  in a 10 minute interval, or if a command reset occurs within the first 10
  minutes of operation, then tagged queuing will be disabled for that target
  device.  These error recovery options improve overall system robustness by
  preventing individual errant devices from causing the system as a whole to
  lock up or crash, and thereby allowing a clean shutdown and restart after the
  offending component is removed.

PCI Configuration Support
-------------------------

  On PCI systems running kernels compiled with PCI BIOS support enabled, this
  driver will interrogate the PCI configuration space and use the I/O port
  addresses assigned by the system BIOS, rather than the ISA compatible I/O
  port addresses.  The ISA compatible I/O port address is then disabled by the
  driver.  On PCI systems it is also recommended that the AutoSCSI utility be
  used to disable the ISA compatible I/O port entirely as it is not necessary.
  The ISA compatible I/O port is disabled by default on the BT-948/958/958D.

/proc File System Support
-------------------------

  Copies of the host adapter configuration information together with updated
  data transfer and error recovery statistics are available through the
  /proc/scsi/BusLogic/<N> interface.

Shared Interrupts Support
-------------------------

  On systems that support shared interrupts, any number of BusLogic Host
  Adapters may share the same interrupt request channel.


Supported Host Adapters
=======================

The following list comprises the supported BusLogic SCSI Host Adapters as of
the date of this document.  It is recommended that anyone purchasing a BusLogic
Host Adapter not in the following table contact the author beforehand to verify
that it is or will be supported.

FlashPoint Series PCI Host Adapters:

=======================	=============================================
FlashPoint LT (BT-930)	Ultra SCSI-3
FlashPoint LT (BT-930R)	Ultra SCSI-3 with RAIDPlus
FlashPoint LT (BT-920)	Ultra SCSI-3 (BT-930 without BIOS)
FlashPoint DL (BT-932)	Dual Channel Ultra SCSI-3
FlashPoint DL (BT-932R)	Dual Channel Ultra SCSI-3 with RAIDPlus
FlashPoint LW (BT-950)	Wide Ultra SCSI-3
FlashPoint LW (BT-950R)	Wide Ultra SCSI-3 with RAIDPlus
FlashPoint DW (BT-952)	Dual Channel Wide Ultra SCSI-3
FlashPoint DW (BT-952R)	Dual Channel Wide Ultra SCSI-3 with RAIDPlus
=======================	=============================================

MultiMaster "W" Series Host Adapters:

=======     ===		==============================
BT-948	    PCI		Ultra SCSI-3
BT-958	    PCI		Wide Ultra SCSI-3
BT-958D	    PCI		Wide Differential Ultra SCSI-3
=======     ===		==============================

MultiMaster "C" Series Host Adapters:

========    ====	==============================
BT-946C	    PCI		Fast SCSI-2
BT-956C	    PCI		Wide Fast SCSI-2
BT-956CD    PCI		Wide Differential Fast SCSI-2
BT-445C	    VLB		Fast SCSI-2
BT-747C	    EISA	Fast SCSI-2
BT-757C	    EISA	Wide Fast SCSI-2
BT-757CD    EISA	Wide Differential Fast SCSI-2
========    ====	==============================

MultiMaster "S" Series Host Adapters:

=======     ====	==============================
BT-445S	    VLB		Fast SCSI-2
BT-747S	    EISA	Fast SCSI-2
BT-747D	    EISA	Differential Fast SCSI-2
BT-757S	    EISA	Wide Fast SCSI-2
BT-757D	    EISA	Wide Differential Fast SCSI-2
BT-742A	    EISA	SCSI-2 (742A revision H)
=======     ====	==============================

MultiMaster "A" Series Host Adapters:

=======     ====	==============================
BT-742A	    EISA	SCSI-2 (742A revisions A - G)
=======     ====	==============================

AMI FastDisk Host Adapters that are true BusLogic MultiMaster clones are also
supported by this driver.

BusLogic SCSI Host Adapters are available packaged both as bare boards and as
retail kits.  The BT- model numbers above refer to the bare board packaging.
The retail kit model numbers are found by replacing BT- with KT- in the above
list.  The retail kit includes the bare board and manual as well as cabling and
driver media and documentation that are not provided with bare boards.


FlashPoint Installation Notes
=============================

RAIDPlus Support
----------------

  FlashPoint Host Adapters now include RAIDPlus, Mylex's bootable software
  RAID.  RAIDPlus is not supported on Linux, and there are no plans to support
  it.  The MD driver in Linux 2.0 provides for concatenation (LINEAR) and
  striping (RAID-0), and support for mirroring (RAID-1), fixed parity (RAID-4),
  and distributed parity (RAID-5) is available separately.  The built-in Linux
  RAID support is generally more flexible and is expected to perform better
  than RAIDPlus, so there is little impetus to include RAIDPlus support in the
  BusLogic driver.

Enabling UltraSCSI Transfers
----------------------------

  FlashPoint Host Adapters ship with their configuration set to "Factory
  Default" settings that are conservative and do not allow for UltraSCSI speed
  to be negotiated.  This results in fewer problems when these host adapters
  are installed in systems with cabling or termination that is not sufficient
  for UltraSCSI operation, or where existing SCSI devices do not properly
  respond to synchronous transfer negotiation for UltraSCSI speed.  AutoSCSI
  may be used to load "Optimum Performance" settings which allow UltraSCSI
  speed to be negotiated with all devices, or UltraSCSI speed can be enabled on
  an individual basis.  It is recommended that SCAM be manually disabled after
  the "Optimum Performance" settings are loaded.


BT-948/958/958D Installation Notes
==================================

The BT-948/958/958D PCI Ultra SCSI Host Adapters have some features which may
require attention in some circumstances when installing Linux.

PCI I/O Port Assignments
------------------------

  When configured to factory default settings, the BT-948/958/958D will only
  recognize the PCI I/O port assignments made by the motherboard's PCI BIOS.
  The BT-948/958/958D will not respond to any of the ISA compatible I/O ports
  that previous BusLogic SCSI Host Adapters respond to.  This driver supports
  the PCI I/O port assignments, so this is the preferred configuration.
  However, if the obsolete BusLogic driver must be used for any reason, such as
  a Linux distribution that does not yet use this driver in its boot kernel,
  BusLogic has provided an AutoSCSI configuration option to enable a legacy ISA
  compatible I/O port.

  To enable this backward compatibility option, invoke the AutoSCSI utility via
  Ctrl-B at system startup and select "Adapter Configuration", "View/Modify
  Configuration", and then change the "ISA Compatible Port" setting from
  "Disable" to "Primary" or "Alternate".  Once this driver has been installed,
  the "ISA Compatible Port" option should be set back to "Disable" to avoid
  possible future I/O port conflicts.  The older BT-946C/956C/956CD also have
  this configuration option, but the factory default setting is "Primary".

PCI Slot Scanning Order
-----------------------

  In systems with multiple BusLogic PCI Host Adapters, the order in which the
  PCI slots are scanned may appear reversed with the BT-948/958/958D as
  compared to the BT-946C/956C/956CD.  For booting from a SCSI disk to work
  correctly, it is necessary that the host adapter's BIOS and the kernel agree
  on which disk is the boot device, which requires that they recognize the PCI
  host adapters in the same order.  The motherboard's PCI BIOS provides a
  standard way of enumerating the PCI host adapters, which is used by the Linux
  kernel.  Some PCI BIOS implementations enumerate the PCI slots in order of
  increasing bus number and device number, while others do so in the opposite
  direction.

  Unfortunately, Microsoft decided that Windows 95 would always enumerate the
  PCI slots in order of increasing bus number and device number regardless of
  the PCI BIOS enumeration, and requires that their scheme be supported by the
  host adapter's BIOS to receive Windows 95 certification.  Therefore, the
  factory default settings of the BT-948/958/958D enumerate the host adapters
  by increasing bus number and device number.  To disable this feature, invoke
  the AutoSCSI utility via Ctrl-B at system startup and select "Adapter
  Configuration", "View/Modify Configuration", press Ctrl-F10, and then change
  the "Use Bus And Device # For PCI Scanning Seq." option to OFF.

  This driver will interrogate the setting of the PCI Scanning Sequence option
  so as to recognize the host adapters in the same order as they are enumerated
  by the host adapter's BIOS.

Enabling UltraSCSI Transfers
----------------------------

  The BT-948/958/958D ship with their configuration set to "Factory Default"
  settings that are conservative and do not allow for UltraSCSI speed to be
  negotiated.  This results in fewer problems when these host adapters are
  installed in systems with cabling or termination that is not sufficient for
  UltraSCSI operation, or where existing SCSI devices do not properly respond
  to synchronous transfer negotiation for UltraSCSI speed.  AutoSCSI may be
  used to load "Optimum Performance" settings which allow UltraSCSI speed to be
  negotiated with all devices, or UltraSCSI speed can be enabled on an
  individual basis.  It is recommended that SCAM be manually disabled after the
  "Optimum Performance" settings are loaded.


Driver Options
==============

BusLogic Driver Options may be specified either via the Linux Kernel Command
Line or via the Loadable Kernel Module Installation Facility.  Driver Options
for multiple host adapters may be specified either by separating the option
strings by a semicolon, or by specifying multiple "BusLogic=" strings on the
command line.  Individual option specifications for a single host adapter are
separated by commas.  The Probing and Debugging Options apply to all host
adapters whereas the remaining options apply individually only to the
selected host adapter.

The BusLogic Driver Probing Options comprise the following:

NoProbe

  The "NoProbe" option disables all probing and therefore no BusLogic Host
  Adapters will be detected.

NoProbePCI

  The "NoProbePCI" options disables the interrogation of PCI Configuration
  Space and therefore only ISA Multimaster Host Adapters will be detected, as
  well as PCI Multimaster Host Adapters that have their ISA Compatible I/O
  Port set to "Primary" or "Alternate".

NoSortPCI

  The "NoSortPCI" option forces PCI MultiMaster Host Adapters to be
  enumerated in the order provided by the PCI BIOS, ignoring any setting of
  the AutoSCSI "Use Bus And Device # For PCI Scanning Seq." option.

MultiMasterFirst

  The "MultiMasterFirst" option forces MultiMaster Host Adapters to be probed
  before FlashPoint Host Adapters.  By default, if both FlashPoint and PCI
  MultiMaster Host Adapters are present, this driver will probe for
  FlashPoint Host Adapters first unless the BIOS primary disk is controlled
  by the first PCI MultiMaster Host Adapter, in which case MultiMaster Host
  Adapters will be probed first.

FlashPointFirst

  The "FlashPointFirst" option forces FlashPoint Host Adapters to be probed
  before MultiMaster Host Adapters.

The BusLogic Driver Tagged Queuing Options allow for explicitly specifying
the Queue Depth and whether Tagged Queuing is permitted for each Target
Device (assuming that the Target Device supports Tagged Queuing).  The Queue
Depth is the number of SCSI Commands that are allowed to be concurrently
presented for execution (either to the Host Adapter or Target Device).  Note
that explicitly enabling Tagged Queuing may lead to problems; the option to
enable or disable Tagged Queuing is provided primarily to allow disabling
Tagged Queuing on Target Devices that do not implement it correctly.  The
following options are available:

QueueDepth:<integer>

  The "QueueDepth:" or QD:" option specifies the Queue Depth to use for all
  Target Devices that support Tagged Queuing, as well as the maximum Queue
  Depth for devices that do not support Tagged Queuing.  If no Queue Depth
  option is provided, the Queue Depth will be determined automatically based
  on the Host Adapter's Total Queue Depth and the number, type, speed, and
  capabilities of the detected Target Devices.  Target Devices that
  do not support Tagged Queuing always have their Queue Depth set to
  BusLogic_UntaggedQueueDepth or BusLogic_UntaggedQueueDepthBB, unless a
  lower Queue Depth option is provided.  A Queue Depth of 1 automatically
  disables Tagged Queuing.

QueueDepth:[<integer>,<integer>...]

  The "QueueDepth:[...]" or "QD:[...]" option specifies the Queue Depth
  individually for each Target Device.  If an <integer> is omitted, the
  associated Target Device will have its Queue Depth selected automatically.

TaggedQueuing:Default

  The "TaggedQueuing:Default" or "TQ:Default" option permits Tagged Queuing
  based on the firmware version of the BusLogic Host Adapter and based on
  whether the Queue Depth allows queuing multiple commands.

TaggedQueuing:Enable

  The "TaggedQueuing:Enable" or "TQ:Enable" option enables Tagged Queuing for
  all Target Devices on this Host Adapter, overriding any limitation that
  would otherwise be imposed based on the Host Adapter firmware version.

TaggedQueuing:Disable

  The "TaggedQueuing:Disable" or "TQ:Disable" option disables Tagged Queuing
  for all Target Devices on this Host Adapter.

TaggedQueuing:<Target-Spec>

  The "TaggedQueuing:<Target-Spec>" or "TQ:<Target-Spec>" option controls
  Tagged Queuing individually for each Target Device.  <Target-Spec> is a
  sequence of "Y", "N", and "X" characters.  "Y" enables Tagged Queuing, "N"
  disables Tagged Queuing, and "X" accepts the default based on the firmware
  version.  The first character refers to Target Device 0, the second to
  Target Device 1, and so on; if the sequence of "Y", "N", and "X" characters
  does not cover all the Target Devices, unspecified characters are assumed
  to be "X".

The BusLogic Driver Miscellaneous Options comprise the following:

BusSettleTime:<seconds>

  The "BusSettleTime:" or "BST:" option specifies the Bus Settle Time in
  seconds.  The Bus Settle Time is the amount of time to wait between a Host
  Adapter Hard Reset which initiates a SCSI Bus Reset and issuing any SCSI
  Commands.  If unspecified, it defaults to BusLogic_DefaultBusSettleTime.

InhibitTargetInquiry

  The "InhibitTargetInquiry" option inhibits the execution of an Inquire
  Target Devices or Inquire Installed Devices command on MultiMaster Host
  Adapters.  This may be necessary with some older Target Devices that do not
  respond correctly when Logical Units above 0 are addressed.

The BusLogic Driver Debugging Options comprise the following:

TraceProbe

  The "TraceProbe" option enables tracing of Host Adapter Probing.

TraceHardwareReset

  The "TraceHardwareReset" option enables tracing of Host Adapter Hardware
  Reset.

TraceConfiguration

  The "TraceConfiguration" option enables tracing of Host Adapter
  Configuration.

TraceErrors

  The "TraceErrors" option enables tracing of SCSI Commands that return an
  error from the Target Device.  The CDB and Sense Data will be printed for
  each SCSI Command that fails.

Debug

  The "Debug" option enables all debugging options.

The following examples demonstrate setting the Queue Depth for Target Devices
1 and 2 on the first host adapter to 7 and 15, the Queue Depth for all Target
Devices on the second host adapter to 31, and the Bus Settle Time on the
second host adapter to 30 seconds.

Linux Kernel Command Line::

  linux BusLogic=QueueDepth:[,7,15];QueueDepth:31,BusSettleTime:30

LILO Linux Boot Loader (in /etc/lilo.conf)::

  append = "BusLogic=QueueDepth:[,7,15];QueueDepth:31,BusSettleTime:30"

INSMOD Loadable Kernel Module Installation Facility::

  insmod BusLogic.o \
      'BusLogic="QueueDepth:[,7,15];QueueDepth:31,BusSettleTime:30"'


.. Note::

      Module Utilities 2.1.71 or later is required for correct parsing
      of driver options containing commas.


Driver Installation
===================

This distribution was prepared for Linux kernel version 2.0.35, but should be
compatible with 2.0.4 or any later 2.0 series kernel.

To install the new BusLogic SCSI driver, you may use the following commands,
replacing "/usr/src" with wherever you keep your Linux kernel source tree::

  cd /usr/src
  tar -xvzf BusLogic-2.0.15.tar.gz
  mv README.* LICENSE.* BusLogic.[ch] FlashPoint.c linux/drivers/scsi
  patch -p0 < BusLogic.patch (only for 2.0.33 and below)
  cd linux
  make config
  make zImage

Then install "arch/x86/boot/zImage" as your standard kernel, run lilo if
appropriate, and reboot.


BusLogic Announcements Mailing List
===================================

The BusLogic Announcements Mailing List provides a forum for informing Linux
users of new driver releases and other announcements regarding Linux support
for BusLogic SCSI Host Adapters.  To join the mailing list, send a message to
"buslogic-announce-request@dandelion.com" with the line "subscribe" in the
message body.
