==============
CerfBoard/Cube
==============

*** The StrongARM version of the CerfBoard/Cube has been discontinued ***

The Intrinsyc CerfBoard is a StrongARM 1110-based computer on a board
that measures approximately 2" square. It includes an Ethernet
controller, an RS232-compatible serial port, a USB function port, and
one CompactFlash+ slot on the back. Pictures can be found at the
Intrinsyc website, http://www.intrinsyc.com.

This document describes the support in the Linux kernel for the
Intrinsyc CerfBoard.

Supported in this version
=========================

   - CompactFlash+ slot (select PCMCIA in General Setup and any options
     that may be required)
   - Onboard Crystal CS8900 Ethernet controller (Cerf CS8900A support in
     Network Devices)
   - Serial ports with a serial console (hardcoded to 38400 8N1)

In order to get this kernel onto your Cerf, you need a server that runs
both BOOTP and TFTP. Detailed instructions should have come with your
evaluation kit on how to use the bootloader. This series of commands
will suffice::

   make ARCH=arm CROSS_COMPILE=arm-linux- cerfcube_defconfig
   make ARCH=arm CROSS_COMPILE=arm-linux- zImage
   make ARCH=arm CROSS_COMPILE=arm-linux- modules
   cp arch/arm/boot/zImage <TFTP directory>

support@intrinsyc.com
