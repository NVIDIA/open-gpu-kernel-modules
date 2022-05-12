===========================================================
Release Notes for Linux on Intel's IXP4xx Network Processor
===========================================================

Maintained by Deepak Saxena <dsaxena@plexity.net>
-------------------------------------------------------------------------

1. Overview

Intel's IXP4xx network processor is a highly integrated SOC that
is targeted for network applications, though it has become popular
in industrial control and other areas due to low cost and power
consumption. The IXP4xx family currently consists of several processors
that support different network offload functions such as encryption,
routing, firewalling, etc. The IXP46x family is an updated version which
supports faster speeds, new memory and flash configurations, and more
integration such as an on-chip I2C controller.

For more information on the various versions of the CPU, see:

   http://developer.intel.com/design/network/products/npfamily/ixp4xx.htm

Intel also made the IXCP1100 CPU for sometime which is an IXP4xx
stripped of much of the network intelligence.

2. Linux Support

Linux currently supports the following features on the IXP4xx chips:

- Dual serial ports
- PCI interface
- Flash access (MTD/JFFS)
- I2C through GPIO on IXP42x
- GPIO for input/output/interrupts
  See arch/arm/mach-ixp4xx/include/mach/platform.h for access functions.
- Timers (watchdog, OS)

The following components of the chips are not supported by Linux and
require the use of Intel's proprietary CSR software:

- USB device interface
- Network interfaces (HSS, Utopia, NPEs, etc)
- Network offload functionality

If you need to use any of the above, you need to download Intel's
software from:

   http://developer.intel.com/design/network/products/npfamily/ixp425.htm

DO NOT POST QUESTIONS TO THE LINUX MAILING LISTS REGARDING THE PROPRIETARY
SOFTWARE.

There are several websites that provide directions/pointers on using
Intel's software:

   - http://sourceforge.net/projects/ixp4xx-osdg/
     Open Source Developer's Guide for using uClinux and the Intel libraries

   - http://gatewaymaker.sourceforge.net/
     Simple one page summary of building a gateway using an IXP425 and Linux

   - http://ixp425.sourceforge.net/
     ATM device driver for IXP425 that relies on Intel's libraries

3. Known Issues/Limitations

3a. Limited inbound PCI window

The IXP4xx family allows for up to 256MB of memory but the PCI interface
can only expose 64MB of that memory to the PCI bus. This means that if
you are running with > 64MB, all PCI buffers outside of the accessible
range will be bounced using the routines in arch/arm/common/dmabounce.c.

3b. Limited outbound PCI window

IXP4xx provides two methods of accessing PCI memory space:

1) A direct mapped window from 0x48000000 to 0x4bffffff (64MB).
   To access PCI via this space, we simply ioremap() the BAR
   into the kernel and we can use the standard read[bwl]/write[bwl]
   macros. This is the preffered method due to speed but it
   limits the system to just 64MB of PCI memory. This can be
   problamatic if using video cards and other memory-heavy devices.

2) If > 64MB of memory space is required, the IXP4xx can be
   configured to use indirect registers to access PCI This allows
   for up to 128MB (0x48000000 to 0x4fffffff) of memory on the bus.
   The disadvantage of this is that every PCI access requires
   three local register accesses plus a spinlock, but in some
   cases the performance hit is acceptable. In addition, you cannot
   mmap() PCI devices in this case due to the indirect nature
   of the PCI window.

By default, the direct method is used for performance reasons. If
you need more PCI memory, enable the IXP4XX_INDIRECT_PCI config option.

3c. GPIO as Interrupts

Currently the code only handles level-sensitive GPIO interrupts

4. Supported platforms

ADI Engineering Coyote Gateway Reference Platform
http://www.adiengineering.com/productsCoyote.html

   The ADI Coyote platform is reference design for those building
   small residential/office gateways. One NPE is connected to a 10/100
   interface, one to 4-port 10/100 switch, and the third to and ADSL
   interface. In addition, it also supports to POTs interfaces connected
   via SLICs. Note that those are not supported by Linux ATM. Finally,
   the platform has two mini-PCI slots used for 802.11[bga] cards.
   Finally, there is an IDE port hanging off the expansion bus.

Gateworks Avila Network Platform
http://www.gateworks.com/support/overview.php

   The Avila platform is basically and IXDP425 with the 4 PCI slots
   replaced with mini-PCI slots and a CF IDE interface hanging off
   the expansion bus.

Intel IXDP425 Development Platform
http://www.intel.com/design/network/products/npfamily/ixdpg425.htm

   This is Intel's standard reference platform for the IXDP425 and is
   also known as the Richfield board. It contains 4 PCI slots, 16MB
   of flash, two 10/100 ports and one ADSL port.

Intel IXDP465 Development Platform
http://www.intel.com/design/network/products/npfamily/ixdp465.htm

   This is basically an IXDP425 with an IXP465 and 32M of flash instead
   of just 16.

Intel IXDPG425 Development Platform

   This is basically and ADI Coyote board with a NEC EHCI controller
   added. One issue with this board is that the mini-PCI slots only
   have the 3.3v line connected, so you can't use a PCI to mini-PCI
   adapter with an E100 card. So to NFS root you need to use either
   the CSR or a WiFi card and a ramdisk that BOOTPs and then does
   a pivot_root to NFS.

Motorola PrPMC1100 Processor Mezanine Card
http://www.fountainsys.com

   The PrPMC1100 is based on the IXCP1100 and is meant to plug into
   and IXP2400/2800 system to act as the system controller. It simply
   contains a CPU and 16MB of flash on the board and needs to be
   plugged into a carrier board to function. Currently Linux only
   supports the Motorola PrPMC carrier board for this platform.

5. TODO LIST

- Add support for Coyote IDE
- Add support for edge-based GPIO interrupts
- Add support for CF IDE on expansion bus

6. Thanks

The IXP4xx work has been funded by Intel Corp. and MontaVista Software, Inc.

The following people have contributed patches/comments/etc:

- Lennerty Buytenhek
- Lutz Jaenicke
- Justin Mayfield
- Robert E. Ranslam

[I know I've forgotten others, please email me to be added]

-------------------------------------------------------------------------

Last Update: 01/04/2005
