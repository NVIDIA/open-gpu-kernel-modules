=========================
Kernel driver i2c-ali15x3
=========================

Supported adapters:
  * Acer Labs, Inc. ALI 1533 and 1543C (south bridge)

    Datasheet: Now under NDA
	http://www.ali.com.tw/

Authors:
	- Frodo Looijaard <frodol@dds.nl>,
	- Philip Edelbrock <phil@netroedge.com>,
	- Mark D. Studebaker <mdsxyz123@yahoo.com>

Module Parameters
-----------------

* force_addr: int
    Initialize the base address of the i2c controller


Notes
-----

The force_addr parameter is useful for boards that don't set the address in
the BIOS. Does not do a PCI force; the device must still be present in
lspci. Don't use this unless the driver complains that the base address is
not set.

Example::

    modprobe i2c-ali15x3 force_addr=0xe800

SMBus periodically hangs on ASUS P5A motherboards and can only be cleared
by a power cycle. Cause unknown (see Issues below).


Description
-----------

This is the driver for the SMB Host controller on Acer Labs Inc. (ALI)
M1541 and M1543C South Bridges.

The M1543C is a South bridge for desktop systems.

The M1541 is a South bridge for portable systems.

They are part of the following ALI chipsets:

 * "Aladdin Pro 2" includes the M1621 Slot 1 North bridge with AGP and
   100MHz CPU Front Side bus
 * "Aladdin V" includes the M1541 Socket 7 North bridge with AGP and 100MHz
   CPU Front Side bus

   Some Aladdin V motherboards:
	- Asus P5A
	- Atrend ATC-5220
	- BCM/GVC VP1541
	- Biostar M5ALA
	- Gigabyte GA-5AX (Generally doesn't work because the BIOS doesn't
	  enable the 7101 device!)
	- Iwill XA100 Plus
	- Micronics C200
	- Microstar (MSI) MS-5169

  * "Aladdin IV" includes the M1541 Socket 7 North bridge
    with host bus up to 83.3 MHz.

For an overview of these chips see http://www.acerlabs.com. At this time the
full data sheets on the web site are password protected, however if you
contact the ALI office in San Jose they may give you the password.

The M1533/M1543C devices appear as FOUR separate devices on the PCI bus. An
output of lspci will show something similar to the following::

  00:02.0 USB Controller: Acer Laboratories Inc. M5237 (rev 03)
  00:03.0 Bridge: Acer Laboratories Inc. M7101      <= THIS IS THE ONE WE NEED
  00:07.0 ISA bridge: Acer Laboratories Inc. M1533 (rev c3)
  00:0f.0 IDE interface: Acer Laboratories Inc. M5229 (rev c1)

.. important::

   If you have a M1533 or M1543C on the board and you get
   "ali15x3: Error: Can't detect ali15x3!"
   then run lspci.

   If you see the 1533 and 5229 devices but NOT the 7101 device,
   then you must enable ACPI, the PMU, SMB, or something similar
   in the BIOS.

   The driver won't work if it can't find the M7101 device.

The SMB controller is part of the M7101 device, which is an ACPI-compliant
Power Management Unit (PMU).

The whole M7101 device has to be enabled for the SMB to work. You can't
just enable the SMB alone. The SMB and the ACPI have separate I/O spaces.
We make sure that the SMB is enabled. We leave the ACPI alone.

Features
--------

This driver controls the SMB Host only. The SMB Slave
controller on the M15X3 is not enabled. This driver does not use
interrupts.


Issues
------

This driver requests the I/O space for only the SMB
registers. It doesn't use the ACPI region.

On the ASUS P5A motherboard, there are several reports that
the SMBus will hang and this can only be resolved by
powering off the computer. It appears to be worse when the board
gets hot, for example under heavy CPU load, or in the summer.
There may be electrical problems on this board.
On the P5A, the W83781D sensor chip is on both the ISA and
SMBus. Therefore the SMBus hangs can generally be avoided
by accessing the W83781D on the ISA bus only.
