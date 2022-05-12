.. SPDX-License-Identifier: GPL-2.0

=========================================================
AppleTalk-IP Decapsulation and AppleTalk-IP Encapsulation
=========================================================

Documentation ipddp.c

This file is written by Jay Schulist <jschlst@samba.org>

Introduction
------------

AppleTalk-IP (IPDDP) is the method computers connected to AppleTalk
networks can use to communicate via IP. AppleTalk-IP is simply IP datagrams
inside AppleTalk packets.

Through this driver you can either allow your Linux box to communicate
IP over an AppleTalk network or you can provide IP gatewaying functions
for your AppleTalk users.

You can currently encapsulate or decapsulate AppleTalk-IP on LocalTalk,
EtherTalk and PPPTalk. The only limit on the protocol is that of what
kernel AppleTalk layer and drivers are available.

Each mode requires its own user space software.

Compiling AppleTalk-IP Decapsulation/Encapsulation
==================================================

AppleTalk-IP decapsulation needs to be compiled into your kernel. You
will need to turn on AppleTalk-IP driver support. Then you will need to
select ONE of the two options; IP to AppleTalk-IP encapsulation support or
AppleTalk-IP to IP decapsulation support. If you compile the driver
statically you will only be able to use the driver for the function you have
enabled in the kernel. If you compile the driver as a module you can
select what mode you want it to run in via a module loading param.
ipddp_mode=1 for AppleTalk-IP encapsulation and ipddp_mode=2 for
AppleTalk-IP to IP decapsulation.

Basic instructions for user space tools
=======================================

I will briefly describe the operation of the tools, but you will
need to consult the supporting documentation for each set of tools.

Decapsulation - You will need to download a software package called
MacGate. In this distribution there will be a tool called MacRoute
which enables you to add routes to the kernel for your Macs by hand.
Also the tool MacRegGateWay is included to register the
proper IP Gateway and IP addresses for your machine. Included in this
distribution is a patch to netatalk-1.4b2+asun2.0a17.2 (available from
ftp.u.washington.edu/pub/user-supported/asun/) this patch is optional
but it allows automatic adding and deleting of routes for Macs. (Handy
for locations with large Mac installations)

Encapsulation - You will need to download a software daemon called ipddpd.
This software expects there to be an AppleTalk-IP gateway on the network.
You will also need to add the proper routes to route your Linux box's IP
traffic out the ipddp interface.

Common Uses of ipddp.c
----------------------
Of course AppleTalk-IP decapsulation and encapsulation, but specifically
decapsulation is being used most for connecting LocalTalk networks to
IP networks. Although it has been used on EtherTalk networks to allow
Macs that are only able to tunnel IP over EtherTalk.

Encapsulation has been used to allow a Linux box stuck on a LocalTalk
network to use IP. It should work equally well if you are stuck on an
EtherTalk only network.

Further Assistance
-------------------
You can contact me (Jay Schulist <jschlst@samba.org>) with any
questions regarding decapsulation or encapsulation. Bradford W. Johnson
<johns393@maroon.tc.umn.edu> originally wrote the ipddp.c driver for IP
encapsulation in AppleTalk.
