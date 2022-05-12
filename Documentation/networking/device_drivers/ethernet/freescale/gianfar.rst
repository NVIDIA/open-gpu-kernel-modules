.. SPDX-License-Identifier: GPL-2.0

===========================
The Gianfar Ethernet Driver
===========================

:Author: Andy Fleming <afleming@freescale.com>
:Updated: 2005-07-28


Checksum Offloading
===================

The eTSEC controller (first included in parts from late 2005 like
the 8548) has the ability to perform TCP, UDP, and IP checksums
in hardware.  The Linux kernel only offloads the TCP and UDP
checksums (and always performs the pseudo header checksums), so
the driver only supports checksumming for TCP/IP and UDP/IP
packets.  Use ethtool to enable or disable this feature for RX
and TX.

VLAN
====

In order to use VLAN, please consult Linux documentation on
configuring VLANs.  The gianfar driver supports hardware insertion and
extraction of VLAN headers, but not filtering.  Filtering will be
done by the kernel.

Multicasting
============

The gianfar driver supports using the group hash table on the
TSEC (and the extended hash table on the eTSEC) for multicast
filtering.  On the eTSEC, the exact-match MAC registers are used
before the hash tables.  See Linux documentation on how to join
multicast groups.

Padding
=======

The gianfar driver supports padding received frames with 2 bytes
to align the IP header to a 16-byte boundary, when supported by
hardware.

Ethtool
=======

The gianfar driver supports the use of ethtool for many
configuration options.  You must run ethtool only on currently
open interfaces.  See ethtool documentation for details.
