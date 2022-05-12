.. SPDX-License-Identifier: GPL-2.0

=====================
TLAN driver for Linux
=====================

:Version: 1.14a

(C) 1997-1998 Caldera, Inc.

(C) 1998 James Banks

(C) 1999-2001 Torben Mathiasen <tmm@image.dk, torben.mathiasen@compaq.com>

For driver information/updates visit http://www.compaq.com





I. Supported Devices
====================

    Only PCI devices will work with this driver.

    Supported:

    =========	=========	===========================================
    Vendor ID	Device ID	Name
    =========	=========	===========================================
    0e11	ae32		Compaq Netelligent 10/100 TX PCI UTP
    0e11	ae34		Compaq Netelligent 10 T PCI UTP
    0e11	ae35		Compaq Integrated NetFlex 3/P
    0e11	ae40		Compaq Netelligent Dual 10/100 TX PCI UTP
    0e11	ae43		Compaq Netelligent Integrated 10/100 TX UTP
    0e11	b011		Compaq Netelligent 10/100 TX Embedded UTP
    0e11	b012		Compaq Netelligent 10 T/2 PCI UTP/Coax
    0e11	b030		Compaq Netelligent 10/100 TX UTP
    0e11	f130		Compaq NetFlex 3/P
    0e11	f150		Compaq NetFlex 3/P
    108d	0012		Olicom OC-2325
    108d	0013		Olicom OC-2183
    108d	0014		Olicom OC-2326
    =========	=========	===========================================


    Caveats:

    I am not sure if 100BaseTX daughterboards (for those cards which
    support such things) will work.  I haven't had any solid evidence
    either way.

    However, if a card supports 100BaseTx without requiring an add
    on daughterboard, it should work with 100BaseTx.

    The "Netelligent 10 T/2 PCI UTP/Coax" (b012) device is untested,
    but I do not expect any problems.


II. Driver Options
==================

	1. You can append debug=x to the end of the insmod line to get
	   debug messages, where x is a bit field where the bits mean
	   the following:

	   ====		=====================================
	   0x01		Turn on general debugging messages.
	   0x02		Turn on receive debugging messages.
	   0x04		Turn on transmit debugging messages.
	   0x08		Turn on list debugging messages.
	   ====		=====================================

	2. You can append aui=1 to the end of the insmod line to cause
	   the adapter to use the AUI interface instead of the 10 Base T
	   interface.  This is also what to do if you want to use the BNC
	   connector on a TLAN based device.  (Setting this option on a
	   device that does not have an AUI/BNC connector will probably
	   cause it to not function correctly.)

	3. You can set duplex=1 to force half duplex, and duplex=2 to
	   force full duplex.

	4. You can set speed=10 to force 10Mbs operation, and speed=100
	   to force 100Mbs operation. (I'm not sure what will happen
	   if a card which only supports 10Mbs is forced into 100Mbs
	   mode.)

	5. You have to use speed=X duplex=Y together now. If you just
	   do "insmod tlan.o speed=100" the driver will do Auto-Neg.
	   To force a 10Mbps Half-Duplex link do "insmod tlan.o speed=10
	   duplex=1".

	6. If the driver is built into the kernel, you can use the 3rd
	   and 4th parameters to set aui and debug respectively.  For
	   example::

		ether=0,0,0x1,0x7,eth0

	   This sets aui to 0x1 and debug to 0x7, assuming eth0 is a
	   supported TLAN device.

	   The bits in the third byte are assigned as follows:

		====   ===============
		0x01   aui
		0x02   use half duplex
		0x04   use full duplex
		0x08   use 10BaseT
		0x10   use 100BaseTx
		====   ===============

	   You also need to set both speed and duplex settings when forcing
	   speeds with kernel-parameters.
	   ether=0,0,0x12,0,eth0 will force link to 100Mbps Half-Duplex.

	7. If you have more than one tlan adapter in your system, you can
	   use the above options on a per adapter basis. To force a 100Mbit/HD
	   link with your eth1 adapter use::

		insmod tlan speed=0,100 duplex=0,1

	   Now eth0 will use auto-neg and eth1 will be forced to 100Mbit/HD.
	   Note that the tlan driver supports a maximum of 8 adapters.


III. Things to try if you have problems
=======================================

	1. Make sure your card's PCI id is among those listed in
	   section I, above.
	2. Make sure routing is correct.
	3. Try forcing different speed/duplex settings


There is also a tlan mailing list which you can join by sending "subscribe tlan"
in the body of an email to majordomo@vuser.vu.union.edu.

There is also a tlan website at http://www.compaq.com

