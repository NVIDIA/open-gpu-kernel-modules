.. SPDX-License-Identifier: GPL-2.0

=========================================
WorkBiT NinjaSCSI-3/32Bi driver for Linux
=========================================

1. Comment
==========

This is Workbit corp.'s(http://www.workbit.co.jp/) NinjaSCSI-3
for Linux.

2. My Linux environment
=======================

:Linux kernel: 2.4.7 / 2.2.19
:pcmcia-cs:    3.1.27
:gcc:          gcc-2.95.4
:PC card:      I-O data PCSC-F (NinjaSCSI-3),
               I-O data CBSC-II in 16 bit mode (NinjaSCSI-32Bi)
:SCSI device:  I-O data CDPS-PX24 (CD-ROM drive),
               Media Intelligent MMO-640GT (Optical disk drive)

3. Install
==========

(a) Check your PC card is true "NinjaSCSI-3" card.

    If you installed pcmcia-cs already, pcmcia reports your card as UNKNOWN
    card, and write ["WBT", "NinjaSCSI-3", "R1.0"] or some other string to
    your console or log file.

    You can also use "cardctl" program (this program is in pcmcia-cs source
    code) to get more info.

    ::

	# cat /var/log/messages
	...
	Jan  2 03:45:06 lindberg cardmgr[78]: unsupported card in socket 1
	Jan  2 03:45:06 lindberg cardmgr[78]:   product info: "WBT", "NinjaSCSI-3", "R1.0"
	...
	# cardctl ident
	Socket 0:
	  no product info available
	Socket 1:
	  product info: "IO DATA", "CBSC16       ", "1"


(b) Get the Linux kernel source, and extract it to /usr/src.
    Because the NinjaSCSI driver requires some SCSI header files in Linux 
    kernel source, I recommend rebuilding your kernel; this eliminates 
    some versioning problems.

    ::

	$ cd /usr/src
	$ tar -zxvf linux-x.x.x.tar.gz
	$ cd linux
	$ make config
	...

(c) If you use this driver with Kernel 2.2, unpack pcmcia-cs in some directory
    and make & install. This driver requires the pcmcia-cs header file.

    ::

	$ cd /usr/src
	$ tar zxvf cs-pcmcia-cs-3.x.x.tar.gz
	...

(d) Extract this driver's archive somewhere, and edit Makefile, then do make::

	$ tar -zxvf nsp_cs-x.x.tar.gz
	$ cd nsp_cs-x.x
	$ emacs Makefile
	...
	$ make

(e) Copy nsp_cs.ko to suitable place, like /lib/modules/<Kernel version>/pcmcia/ .

(f) Add these lines to /etc/pcmcia/config .

    If you use pcmcia-cs-3.1.8 or later, we can use "nsp_cs.conf" file.
    So, you don't need to edit file. Just copy to /etc/pcmcia/ .

    ::

	device "nsp_cs"
	  class "scsi" module "nsp_cs"

	card "WorkBit NinjaSCSI-3"
	  version "WBT", "NinjaSCSI-3", "R1.0"
	  bind "nsp_cs"

	card "WorkBit NinjaSCSI-32Bi (16bit)"
	  version "WORKBIT", "UltraNinja-16", "1"
	  bind "nsp_cs"

	# OEM
	card "WorkBit NinjaSCSI-32Bi (16bit) / IO-DATA"
	  version "IO DATA", "CBSC16       ", "1"
	  bind "nsp_cs"

	# OEM
	card "WorkBit NinjaSCSI-32Bi (16bit) / KME-1"
	  version "KME    ", "SCSI-CARD-001", "1"
	  bind "nsp_cs"
	card "WorkBit NinjaSCSI-32Bi (16bit) / KME-2"
	  version "KME    ", "SCSI-CARD-002", "1"
	  bind "nsp_cs"
	card "WorkBit NinjaSCSI-32Bi (16bit) / KME-3"
	  version "KME    ", "SCSI-CARD-003", "1"
	  bind "nsp_cs"
	card "WorkBit NinjaSCSI-32Bi (16bit) / KME-4"
	  version "KME    ", "SCSI-CARD-004", "1"
	  bind "nsp_cs"

(f) Start (or restart) pcmcia-cs::

	# /etc/rc.d/rc.pcmcia start        (BSD style)

    or::

	# /etc/init.d/pcmcia start         (SYSV style)


4. History
==========

See README.nin_cs .

5. Caution
==========

If you eject card when doing some operation for your SCSI device or suspend
your computer, you encount some *BAD* error like disk crash.

It works good when I using this driver right way. But I'm not guarantee
your data. Please backup your data when you use this driver.

6. Known Bugs
=============

In 2.4 kernel, you can't use 640MB Optical disk. This error comes from
high level SCSI driver.

7. Testing
==========

Please send me some reports(bug reports etc..) of this software.
When you send report, please tell me these or more.

	- card name
	- kernel version
	- your SCSI device name(hard drive, CD-ROM, etc...)

8. Copyright
============

 See GPL.


2001/08/08 yokota@netlab.is.tsukuba.ac.jp <YOKOTA Hiroshi>
