.. SPDX-License-Identifier: GPL-2.0

=========================
3Com Vortex device driver
=========================

Andrew Morton

30 April 2000


This document describes the usage and errata of the 3Com "Vortex" device
driver for Linux, 3c59x.c.

The driver was written by Donald Becker <becker@scyld.com>

Don is no longer the prime maintainer of this version of the driver.
Please report problems to one or more of:

- Andrew Morton
- Netdev mailing list <netdev@vger.kernel.org>
- Linux kernel mailing list <linux-kernel@vger.kernel.org>

Please note the 'Reporting and Diagnosing Problems' section at the end
of this file.


Since kernel 2.3.99-pre6, this driver incorporates the support for the
3c575-series Cardbus cards which used to be handled by 3c575_cb.c.

This driver supports the following hardware:

	- 3c590 Vortex 10Mbps
	- 3c592 EISA 10Mbps Demon/Vortex
	- 3c597 EISA Fast Demon/Vortex
	- 3c595 Vortex 100baseTx
	- 3c595 Vortex 100baseT4
	- 3c595 Vortex 100base-MII
	- 3c900 Boomerang 10baseT
	- 3c900 Boomerang 10Mbps Combo
	- 3c900 Cyclone 10Mbps TPO
	- 3c900 Cyclone 10Mbps Combo
	- 3c900 Cyclone 10Mbps TPC
	- 3c900B-FL Cyclone 10base-FL
	- 3c905 Boomerang 100baseTx
	- 3c905 Boomerang 100baseT4
	- 3c905B Cyclone 100baseTx
	- 3c905B Cyclone 10/100/BNC
	- 3c905B-FX Cyclone 100baseFx
	- 3c905C Tornado
	- 3c920B-EMB-WNM (ATI Radeon 9100 IGP)
	- 3c980 Cyclone
	- 3c980C Python-T
	- 3cSOHO100-TX Hurricane
	- 3c555 Laptop Hurricane
	- 3c556 Laptop Tornado
	- 3c556B Laptop Hurricane
	- 3c575 [Megahertz] 10/100 LAN  CardBus
	- 3c575 Boomerang CardBus
	- 3CCFE575BT Cyclone CardBus
	- 3CCFE575CT Tornado CardBus
	- 3CCFE656 Cyclone CardBus
	- 3CCFEM656B Cyclone+Winmodem CardBus
	- 3CXFEM656C Tornado+Winmodem CardBus
	- 3c450 HomePNA Tornado
	- 3c920 Tornado
	- 3c982 Hydra Dual Port A
	- 3c982 Hydra Dual Port B
	- 3c905B-T4
	- 3c920B-EMB-WNM Tornado

Module parameters
=================

There are several parameters which may be provided to the driver when
its module is loaded.  These are usually placed in ``/etc/modprobe.d/*.conf``
configuration files.  Example::

    options 3c59x debug=3 rx_copybreak=300

If you are using the PCMCIA tools (cardmgr) then the options may be
placed in /etc/pcmcia/config.opts::

    module "3c59x" opts "debug=3 rx_copybreak=300"


The supported parameters are:

debug=N

  Where N is a number from 0 to 7.  Anything above 3 produces a lot
  of output in your system logs.  debug=1 is default.

options=N1,N2,N3,...

  Each number in the list provides an option to the corresponding
  network card.  So if you have two 3c905's and you wish to provide
  them with option 0x204 you would use::

    options=0x204,0x204

  The individual options are composed of a number of bitfields which
  have the following meanings:

  Possible media type settings

	==	=================================
	0	10baseT
	1	10Mbs AUI
	2	undefined
	3	10base2 (BNC)
	4	100base-TX
	5	100base-FX
	6	MII (Media Independent Interface)
	7	Use default setting from EEPROM
	8       Autonegotiate
	9       External MII
	10      Use default setting from EEPROM
	==	=================================

  When generating a value for the 'options' setting, the above media
  selection values may be OR'ed (or added to) the following:

  ======  =============================================
  0x8000  Set driver debugging level to 7
  0x4000  Set driver debugging level to 2
  0x0400  Enable Wake-on-LAN
  0x0200  Force full duplex mode.
  0x0010  Bus-master enable bit (Old Vortex cards only)
  ======  =============================================

  For example::

    insmod 3c59x options=0x204

  will force full-duplex 100base-TX, rather than allowing the usual
  autonegotiation.

global_options=N

  Sets the ``options`` parameter for all 3c59x NICs in the machine.
  Entries in the ``options`` array above will override any setting of
  this.

full_duplex=N1,N2,N3...

  Similar to bit 9 of 'options'.  Forces the corresponding card into
  full-duplex mode.  Please use this in preference to the ``options``
  parameter.

  In fact, please don't use this at all! You're better off getting
  autonegotiation working properly.

global_full_duplex=N1

  Sets full duplex mode for all 3c59x NICs in the machine.  Entries
  in the ``full_duplex`` array above will override any setting of this.

flow_ctrl=N1,N2,N3...

  Use 802.3x MAC-layer flow control.  The 3com cards only support the
  PAUSE command, which means that they will stop sending packets for a
  short period if they receive a PAUSE frame from the link partner.

  The driver only allows flow control on a link which is operating in
  full duplex mode.

  This feature does not appear to work on the 3c905 - only 3c905B and
  3c905C have been tested.

  The 3com cards appear to only respond to PAUSE frames which are
  sent to the reserved destination address of 01:80:c2:00:00:01.  They
  do not honour PAUSE frames which are sent to the station MAC address.

rx_copybreak=M

  The driver preallocates 32 full-sized (1536 byte) network buffers
  for receiving.  When a packet arrives, the driver has to decide
  whether to leave the packet in its full-sized buffer, or to allocate
  a smaller buffer and copy the packet across into it.

  This is a speed/space tradeoff.

  The value of rx_copybreak is used to decide when to make the copy.
  If the packet size is less than rx_copybreak, the packet is copied.
  The default value for rx_copybreak is 200 bytes.

max_interrupt_work=N

  The driver's interrupt service routine can handle many receive and
  transmit packets in a single invocation.  It does this in a loop.
  The value of max_interrupt_work governs how many times the interrupt
  service routine will loop.  The default value is 32 loops.  If this
  is exceeded the interrupt service routine gives up and generates a
  warning message "eth0: Too much work in interrupt".

hw_checksums=N1,N2,N3,...

  Recent 3com NICs are able to generate IPv4, TCP and UDP checksums
  in hardware.  Linux has used the Rx checksumming for a long time.
  The "zero copy" patch which is planned for the 2.4 kernel series
  allows you to make use of the NIC's DMA scatter/gather and transmit
  checksumming as well.

  The driver is set up so that, when the zerocopy patch is applied,
  all Tornado and Cyclone devices will use S/G and Tx checksums.

  This module parameter has been provided so you can override this
  decision.  If you think that Tx checksums are causing a problem, you
  may disable the feature with ``hw_checksums=0``.

  If you think your NIC should be performing Tx checksumming and the
  driver isn't enabling it, you can force the use of hardware Tx
  checksumming with ``hw_checksums=1``.

  The driver drops a message in the logfiles to indicate whether or
  not it is using hardware scatter/gather and hardware Tx checksums.

  Scatter/gather and hardware checksums provide considerable
  performance improvement for the sendfile() system call, but a small
  decrease in throughput for send().  There is no effect upon receive
  efficiency.

compaq_ioaddr=N,
compaq_irq=N,
compaq_device_id=N

  "Variables to work-around the Compaq PCI BIOS32 problem"....

watchdog=N

  Sets the time duration (in milliseconds) after which the kernel
  decides that the transmitter has become stuck and needs to be reset.
  This is mainly for debugging purposes, although it may be advantageous
  to increase this value on LANs which have very high collision rates.
  The default value is 5000 (5.0 seconds).

enable_wol=N1,N2,N3,...

  Enable Wake-on-LAN support for the relevant interface.  Donald
  Becker's ``ether-wake`` application may be used to wake suspended
  machines.

  Also enables the NIC's power management support.

global_enable_wol=N

  Sets enable_wol mode for all 3c59x NICs in the machine.  Entries in
  the ``enable_wol`` array above will override any setting of this.

Media selection
---------------

A number of the older NICs such as the 3c590 and 3c900 series have
10base2 and AUI interfaces.

Prior to January, 2001 this driver would autoeselect the 10base2 or AUI
port if it didn't detect activity on the 10baseT port.  It would then
get stuck on the 10base2 port and a driver reload was necessary to
switch back to 10baseT.  This behaviour could not be prevented with a
module option override.

Later (current) versions of the driver _do_ support locking of the
media type.  So if you load the driver module with

	modprobe 3c59x options=0

it will permanently select the 10baseT port.  Automatic selection of
other media types does not occur.


Transmit error, Tx status register 82
-------------------------------------

This is a common error which is almost always caused by another host on
the same network being in full-duplex mode, while this host is in
half-duplex mode.  You need to find that other host and make it run in
half-duplex mode or fix this host to run in full-duplex mode.

As a last resort, you can force the 3c59x driver into full-duplex mode
with

	options 3c59x full_duplex=1

but this has to be viewed as a workaround for broken network gear and
should only really be used for equipment which cannot autonegotiate.


Additional resources
--------------------

Details of the device driver implementation are at the top of the source file.

Additional documentation is available at Don Becker's Linux Drivers site:

     http://www.scyld.com/vortex.html

Donald Becker's driver development site:

     http://www.scyld.com/network.html

Donald's vortex-diag program is useful for inspecting the NIC's state:

     http://www.scyld.com/ethercard_diag.html

Donald's mii-diag program may be used for inspecting and manipulating
the NIC's Media Independent Interface subsystem:

     http://www.scyld.com/ethercard_diag.html#mii-diag

Donald's wake-on-LAN page:

     http://www.scyld.com/wakeonlan.html

3Com's DOS-based application for setting up the NICs EEPROMs:

	ftp://ftp.3com.com/pub/nic/3c90x/3c90xx2.exe


Autonegotiation notes
---------------------

  The driver uses a one-minute heartbeat for adapting to changes in
  the external LAN environment if link is up and 5 seconds if link is down.
  This means that when, for example, a machine is unplugged from a hubbed
  10baseT LAN plugged into a  switched 100baseT LAN, the throughput
  will be quite dreadful for up to sixty seconds.  Be patient.

  Cisco interoperability note from Walter Wong <wcw+@CMU.EDU>:

  On a side note, adding HAS_NWAY seems to share a problem with the
  Cisco 6509 switch.  Specifically, you need to change the spanning
  tree parameter for the port the machine is plugged into to 'portfast'
  mode.  Otherwise, the negotiation fails.  This has been an issue
  we've noticed for a while but haven't had the time to track down.

  Cisco switches    (Jeff Busch <jbusch@deja.com>)

    My "standard config" for ports to which PC's/servers connect directly::

	interface FastEthernet0/N
	description machinename
	load-interval 30
	spanning-tree portfast

    If autonegotiation is a problem, you may need to specify "speed
    100" and "duplex full" as well (or "speed 10" and "duplex half").

    WARNING: DO NOT hook up hubs/switches/bridges to these
    specially-configured ports! The switch will become very confused.


Reporting and diagnosing problems
---------------------------------

Maintainers find that accurate and complete problem reports are
invaluable in resolving driver problems.  We are frequently not able to
reproduce problems and must rely on your patience and efforts to get to
the bottom of the problem.

If you believe you have a driver problem here are some of the
steps you should take:

- Is it really a driver problem?

   Eliminate some variables: try different cards, different
   computers, different cables, different ports on the switch/hub,
   different versions of the kernel or of the driver, etc.

- OK, it's a driver problem.

   You need to generate a report.  Typically this is an email to the
   maintainer and/or netdev@vger.kernel.org.  The maintainer's
   email address will be in the driver source or in the MAINTAINERS file.

- The contents of your report will vary a lot depending upon the
  problem.  If it's a kernel crash then you should refer to
  'Documentation/admin-guide/reporting-issues.rst'.

  But for most problems it is useful to provide the following:

   - Kernel version, driver version

   - A copy of the banner message which the driver generates when
     it is initialised.  For example:

     eth0: 3Com PCI 3c905C Tornado at 0xa400,  00:50:da:6a:88:f0, IRQ 19
     8K byte-wide RAM 5:3 Rx:Tx split, autoselect/Autonegotiate interface.
     MII transceiver found at address 24, status 782d.
     Enabling bus-master transmits and whole-frame receives.

     NOTE: You must provide the ``debug=2`` modprobe option to generate
     a full detection message.  Please do this::

	modprobe 3c59x debug=2

   - If it is a PCI device, the relevant output from 'lspci -vx', eg::

       00:09.0 Ethernet controller: 3Com Corporation 3c905C-TX [Fast Etherlink] (rev 74)
	       Subsystem: 3Com Corporation: Unknown device 9200
	       Flags: bus master, medium devsel, latency 32, IRQ 19
	       I/O ports at a400 [size=128]
	       Memory at db000000 (32-bit, non-prefetchable) [size=128]
	       Expansion ROM at <unassigned> [disabled] [size=128K]
	       Capabilities: [dc] Power Management version 2
       00: b7 10 00 92 07 00 10 02 74 00 00 02 08 20 00 00
       10: 01 a4 00 00 00 00 00 db 00 00 00 00 00 00 00 00
       20: 00 00 00 00 00 00 00 00 00 00 00 00 b7 10 00 10
       30: 00 00 00 00 dc 00 00 00 00 00 00 00 05 01 0a 0a

   - A description of the environment: 10baseT? 100baseT?
     full/half duplex? switched or hubbed?

   - Any additional module parameters which you may be providing to the driver.

   - Any kernel logs which are produced.  The more the merrier.
     If this is a large file and you are sending your report to a
     mailing list, mention that you have the logfile, but don't send
     it.  If you're reporting direct to the maintainer then just send
     it.

     To ensure that all kernel logs are available, add the
     following line to /etc/syslog.conf::

	 kern.* /var/log/messages

     Then restart syslogd with::

	 /etc/rc.d/init.d/syslog restart

     (The above may vary, depending upon which Linux distribution you use).

    - If your problem is reproducible then that's great.  Try the
      following:

      1) Increase the debug level.  Usually this is done via:

	 a) modprobe driver debug=7
	 b) In /etc/modprobe.d/driver.conf:
	    options driver debug=7

      2) Recreate the problem with the higher debug level,
	 send all logs to the maintainer.

      3) Download you card's diagnostic tool from Donald
	 Becker's website <http://www.scyld.com/ethercard_diag.html>.
	 Download mii-diag.c as well.  Build these.

	 a) Run 'vortex-diag -aaee' and 'mii-diag -v' when the card is
	    working correctly.  Save the output.

	 b) Run the above commands when the card is malfunctioning.  Send
	    both sets of output.

Finally, please be patient and be prepared to do some work.  You may
end up working on this problem for a week or more as the maintainer
asks more questions, asks for more tests, asks for patches to be
applied, etc.  At the end of it all, the problem may even remain
unresolved.
