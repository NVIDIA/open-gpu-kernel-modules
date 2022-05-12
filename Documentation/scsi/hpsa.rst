.. SPDX-License-Identifier: GPL-2.0

=========================================
HPSA - Hewlett Packard Smart Array driver
=========================================

This file describes the hpsa SCSI driver for HP Smart Array controllers.
The hpsa driver is intended to supplant the cciss driver for newer
Smart Array controllers.  The hpsa driver is a SCSI driver, while the
cciss driver is a "block" driver.  Actually cciss is both a block
driver (for logical drives) AND a SCSI driver (for tape drives). This
"split-brained" design of the cciss driver is a source of excess
complexity and eliminating that complexity is one of the reasons
for hpsa to exist.

Supported devices
=================

- Smart Array P212
- Smart Array P410
- Smart Array P410i
- Smart Array P411
- Smart Array P812
- Smart Array P712m
- Smart Array P711m
- StorageWorks P1210m

Additionally, older Smart Arrays may work with the hpsa driver if the kernel
boot parameter "hpsa_allow_any=1" is specified, however these are not tested
nor supported by HP with this driver.  For older Smart Arrays, the cciss
driver should still be used.

The "hpsa_simple_mode=1" boot parameter may be used to prevent the driver from
putting the controller into "performant" mode.  The difference is that with simple
mode, each command completion requires an interrupt, while with "performant mode"
(the default, and ordinarily better performing) it is possible to have multiple
command completions indicated by a single interrupt.

HPSA specific entries in /sys
=============================

  In addition to the generic SCSI attributes available in /sys, hpsa supports
  the following attributes:

HPSA specific host attributes
=============================

  ::

    /sys/class/scsi_host/host*/rescan
    /sys/class/scsi_host/host*/firmware_revision
    /sys/class/scsi_host/host*/resettable
    /sys/class/scsi_host/host*/transport_mode

  the host "rescan" attribute is a write only attribute.  Writing to this
  attribute will cause the driver to scan for new, changed, or removed devices
  (e.g. hot-plugged tape drives, or newly configured or deleted logical drives,
  etc.) and notify the SCSI midlayer of any changes detected.  Normally this is
  triggered automatically by HP's Array Configuration Utility (either the GUI or
  command line variety) so for logical drive changes, the user should not
  normally have to use this.  It may be useful when hot plugging devices like
  tape drives, or entire storage boxes containing pre-configured logical drives.

  The "firmware_revision" attribute contains the firmware version of the Smart Array.
  For example::

	root@host:/sys/class/scsi_host/host4# cat firmware_revision
	7.14

  The transport_mode indicates whether the controller is in "performant"
  or "simple" mode.  This is controlled by the "hpsa_simple_mode" module
  parameter.

  The "resettable" read-only attribute indicates whether a particular
  controller is able to honor the "reset_devices" kernel parameter.  If the
  device is resettable, this file will contain a "1", otherwise, a "0".  This
  parameter is used by kdump, for example, to reset the controller at driver
  load time to eliminate any outstanding commands on the controller and get the
  controller into a known state so that the kdump initiated i/o will work right
  and not be disrupted in any way by stale commands or other stale state
  remaining on the controller from the previous kernel.  This attribute enables
  kexec tools to warn the user if they attempt to designate a device which is
  unable to honor the reset_devices kernel parameter as a dump device.

HPSA specific disk attributes
-----------------------------

  ::

    /sys/class/scsi_disk/c:b:t:l/device/unique_id
    /sys/class/scsi_disk/c:b:t:l/device/raid_level
    /sys/class/scsi_disk/c:b:t:l/device/lunid

  (where c:b:t:l are the controller, bus, target and lun of the device)

  For example::

	root@host:/sys/class/scsi_disk/4:0:0:0/device# cat unique_id
	600508B1001044395355323037570F77
	root@host:/sys/class/scsi_disk/4:0:0:0/device# cat lunid
	0x0000004000000000
	root@host:/sys/class/scsi_disk/4:0:0:0/device# cat raid_level
	RAID 0

HPSA specific ioctls
====================

  For compatibility with applications written for the cciss driver, many, but
  not all of the ioctls supported by the cciss driver are also supported by the
  hpsa driver.  The data structures used by these are described in
  include/linux/cciss_ioctl.h

  CCISS_DEREGDISK, CCISS_REGNEWDISK, CCISS_REGNEWD
	The above three ioctls all do exactly the same thing, which is to cause the driver
	to rescan for new devices.  This does exactly the same thing as writing to the
	hpsa specific host "rescan" attribute.

  CCISS_GETPCIINFO
	Returns PCI domain, bus, device and function and "board ID" (PCI subsystem ID).

  CCISS_GETDRIVVER
	Returns driver version in three bytes encoded as::

		(major_version << 16) | (minor_version << 8) | (subminor_version)

  CCISS_PASSTHRU, CCISS_BIG_PASSTHRU
	Allows "BMIC" and "CISS" commands to be passed through to the Smart Array.
	These are used extensively by the HP Array Configuration Utility, SNMP storage
	agents, etc.  See cciss_vol_status at http://cciss.sf.net for some examples.
