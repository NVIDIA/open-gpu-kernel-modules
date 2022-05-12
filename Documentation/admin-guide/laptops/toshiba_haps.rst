====================================
Toshiba HDD Active Protection Sensor
====================================

Kernel driver: toshiba_haps

Author: Azael Avalos <coproscefalo@gmail.com>


.. 0. Contents

   1. Description
   2. Interface
   3. Accelerometer axes
   4. Supported devices
   5. Usage


1. Description
--------------

This driver provides support for the accelerometer found in various Toshiba
laptops, being called "Toshiba HDD Protection - Shock Sensor" officially,
and detects laptops automatically with this device.
On Windows, Toshiba provided software monitors this device and provides
automatic HDD protection (head unload) on sudden moves or harsh vibrations,
however, this driver only provides a notification via a sysfs file to let
userspace tools or daemons act accordingly, as well as providing a sysfs
file to set the desired protection level or sensor sensibility.


2. Interface
------------

This device comes with 3 methods:

====	=====================================================================
_STA    Checks existence of the device, returning Zero if the device does not
	exists or is not supported.
PTLV    Sets the desired protection level.
RSSS    Shuts down the HDD protection interface for a few seconds,
	then restores normal operation.
====	=====================================================================

Note:
  The presence of Solid State Drives (SSD) can make this driver to fail loading,
  given the fact that such drives have no movable parts, and thus, not requiring
  any "protection" as well as failing during the evaluation of the _STA method
  found under this device.


3. Accelerometer axes
---------------------

This device does not report any axes, however, to query the sensor position
a couple HCI (Hardware Configuration Interface) calls (0x6D and 0xA6) are
provided to query such information, handled by the kernel module toshiba_acpi
since kernel version 3.15.


4. Supported devices
--------------------

This driver binds itself to the ACPI device TOS620A, and any Toshiba laptop
with this device is supported, given the fact that they have the presence of
conventional HDD and not only SSD, or a combination of both HDD and SSD.


5. Usage
--------

The sysfs files under /sys/devices/LNXSYSTM:00/LNXSYBUS:00/TOS620A:00/ are:

================   ============================================================
protection_level   The protection_level is readable and writeable, and
		   provides a way to let userspace query the current protection
		   level, as well as set the desired protection level, the
		   available protection levels are::

		     ============   =======   ==========   ========
		     0 - Disabled   1 - Low   2 - Medium   3 - High
		     ============   =======   ==========   ========

reset_protection   The reset_protection entry is writeable only, being "1"
		   the only parameter it accepts, it is used to trigger
		   a reset of the protection interface.
================   ============================================================
