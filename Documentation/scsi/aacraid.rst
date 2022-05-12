.. SPDX-License-Identifier: GPL-2.0

===================================
AACRAID Driver for Linux (take two)
===================================

Introduction
============
The aacraid driver adds support for Adaptec (http://www.adaptec.com)
RAID controllers. This is a major rewrite from the original
Adaptec supplied driver. It has significantly cleaned up both the code
and the running binary size (the module is less than half the size of
the original).

Supported Cards/Chipsets
========================

	===================	=======	=======================================
	PCI ID (pci.ids)	OEM	Product
	===================	=======	=======================================
	9005:0285:9005:0285	Adaptec	2200S (Vulcan)
	9005:0285:9005:0286	Adaptec	2120S (Crusader)
	9005:0285:9005:0287	Adaptec	2200S (Vulcan-2m)
	9005:0285:9005:0288	Adaptec	3230S (Harrier)
	9005:0285:9005:0289	Adaptec	3240S (Tornado)
	9005:0285:9005:028a	Adaptec	2020ZCR (Skyhawk)
	9005:0285:9005:028b	Adaptec	2025ZCR (Terminator)
	9005:0286:9005:028c	Adaptec	2230S (Lancer)
	9005:0286:9005:028c	Adaptec	2230SLP (Lancer)
	9005:0286:9005:028d	Adaptec	2130S (Lancer)
	9005:0285:9005:028e	Adaptec	2020SA (Skyhawk)
	9005:0285:9005:028f	Adaptec	2025SA (Terminator)
	9005:0285:9005:0290	Adaptec	2410SA (Jaguar)
	9005:0285:103c:3227	Adaptec	2610SA (Bearcat HP release)
	9005:0285:9005:0293	Adaptec	21610SA (Corsair-16)
	9005:0285:9005:0296	Adaptec	2240S (SabreExpress)
	9005:0285:9005:0292	Adaptec	2810SA (Corsair-8)
	9005:0285:9005:0297	Adaptec	4005 (AvonPark)
	9005:0285:9005:0298	Adaptec	4000 (BlackBird)
	9005:0285:9005:0299	Adaptec	4800SAS (Marauder-X)
	9005:0285:9005:029a	Adaptec	4805SAS (Marauder-E)
	9005:0286:9005:029b	Adaptec	2820SA (Intruder)
	9005:0286:9005:029c	Adaptec	2620SA (Intruder)
	9005:0286:9005:029d	Adaptec	2420SA (Intruder HP release)
	9005:0286:9005:02ac	Adaptec	1800 (Typhoon44)
	9005:0285:9005:02b5	Adaptec	5445 (Voodoo44)
	9005:0285:15d9:02b5	SMC	AOC-USAS-S4i
	9005:0285:9005:02b6	Adaptec	5805 (Voodoo80)
	9005:0285:15d9:02b6	SMC	AOC-USAS-S8i
	9005:0285:9005:02b7	Adaptec	5085 (Voodoo08)
	9005:0285:9005:02bb	Adaptec	3405 (Marauder40LP)
	9005:0285:9005:02bc	Adaptec	3805 (Marauder80LP)
	9005:0285:9005:02c7	Adaptec	3085 (Marauder08ELP)
	9005:0285:9005:02bd	Adaptec	31205 (Marauder120)
	9005:0285:9005:02be	Adaptec	31605 (Marauder160)
	9005:0285:9005:02c3	Adaptec	51205 (Voodoo120)
	9005:0285:9005:02c4	Adaptec	51605 (Voodoo160)
	9005:0285:15d9:02c9	SMC	AOC-USAS-S4iR
	9005:0285:15d9:02ca	SMC	AOC-USAS-S8iR
	9005:0285:9005:02ce	Adaptec	51245 (Voodoo124)
	9005:0285:9005:02cf	Adaptec	51645 (Voodoo164)
	9005:0285:9005:02d0	Adaptec	52445 (Voodoo244)
	9005:0285:9005:02d1	Adaptec	5405 (Voodoo40)
	9005:0285:15d9:02d2	SMC	AOC-USAS-S8i-LP
	9005:0285:15d9:02d3	SMC	AOC-USAS-S8iR-LP
	9005:0285:9005:02d4	Adaptec	ASR-2045 (Voodoo04 Lite)
	9005:0285:9005:02d5	Adaptec	ASR-2405 (Voodoo40 Lite)
	9005:0285:9005:02d6	Adaptec	ASR-2445 (Voodoo44 Lite)
	9005:0285:9005:02d7	Adaptec	ASR-2805 (Voodoo80 Lite)
	9005:0285:9005:02d8	Adaptec	5405Z (Voodoo40 BLBU)
	9005:0285:9005:02d9	Adaptec	5445Z (Voodoo44 BLBU)
	9005:0285:9005:02da	Adaptec	5805Z (Voodoo80 BLBU)
	1011:0046:9005:0364	Adaptec	5400S (Mustang)
	1011:0046:9005:0365	Adaptec	5400S (Mustang)
	9005:0287:9005:0800	Adaptec	Themisto (Jupiter)
	9005:0200:9005:0200	Adaptec	Themisto (Jupiter)
	9005:0286:9005:0800	Adaptec	Callisto (Jupiter)
	1011:0046:9005:1364	Dell	PERC 2/QC (Quad Channel, Mustang)
	1011:0046:9005:1365	Dell	PERC 2/QC (Quad Channel, Mustang)
	1028:0001:1028:0001	Dell	PERC 2/Si (Iguana)
	1028:0003:1028:0003	Dell	PERC 3/Si (SlimFast)
	1028:0002:1028:0002	Dell	PERC 3/Di (Opal)
	1028:0004:1028:0004	Dell	PERC 3/SiF (Iguana)
	1028:0004:1028:00d0	Dell	PERC 3/DiF (Iguana)
	1028:0002:1028:00d1	Dell	PERC 3/DiV (Viper)
	1028:0002:1028:00d9	Dell	PERC 3/DiL (Lexus)
	1028:000a:1028:0106	Dell	PERC 3/DiJ (Jaguar)
	1028:000a:1028:011b	Dell	PERC 3/DiD (Dagger)
	1028:000a:1028:0121	Dell	PERC 3/DiB (Boxster)
	9005:0285:1028:0287	Dell	PERC 320/DC (Vulcan)
	9005:0285:1028:0291	Dell	CERC 2 (DellCorsair)
	1011:0046:103c:10c2	HP	NetRAID-4M (Mustang)
	9005:0285:17aa:0286	Legend	S220 (Crusader)
	9005:0285:17aa:0287	Legend	S230 (Vulcan)
	9005:0285:9005:0290	IBM	ServeRAID 7t (Jaguar)
	9005:0285:1014:02F2	IBM	ServeRAID 8i (AvonPark)
	9005:0286:1014:9540	IBM	ServeRAID 8k/8k-l4 (AuroraLite)
	9005:0286:1014:9580	IBM	ServeRAID 8k/8k-l8 (Aurora)
	9005:0285:1014:034d	IBM	ServeRAID 8s (Marauder-E)
	9005:0286:9005:029e	ICP	ICP9024RO (Lancer)
	9005:0286:9005:029f	ICP	ICP9014RO (Lancer)
	9005:0286:9005:02a0	ICP	ICP9047MA (Lancer)
	9005:0286:9005:02a1	ICP	ICP9087MA (Lancer)
	9005:0285:9005:02a4	ICP	ICP9085LI (Marauder-X)
	9005:0285:9005:02a5	ICP	ICP5085BR (Marauder-E)
	9005:0286:9005:02a6	ICP	ICP9067MA (Intruder-6)
	9005:0285:9005:02b2	ICP	(Voodoo 8 internal 8 external)
	9005:0285:9005:02b8	ICP	ICP5445SL (Voodoo44)
	9005:0285:9005:02b9	ICP	ICP5085SL (Voodoo80)
	9005:0285:9005:02ba	ICP	ICP5805SL (Voodoo08)
	9005:0285:9005:02bf	ICP	ICP5045BL (Marauder40LP)
	9005:0285:9005:02c0	ICP	ICP5085BL (Marauder80LP)
	9005:0285:9005:02c8	ICP	ICP5805BL (Marauder08ELP)
	9005:0285:9005:02c1	ICP	ICP5125BR (Marauder120)
	9005:0285:9005:02c2	ICP	ICP5165BR (Marauder160)
	9005:0285:9005:02c5	ICP	ICP5125SL (Voodoo120)
	9005:0285:9005:02c6	ICP	ICP5165SL (Voodoo160)
	9005:0286:9005:02ab		(Typhoon40)
	9005:0286:9005:02ad		(Aurora ARK)
	9005:0286:9005:02ae		(Aurora Lite ARK)
	9005:0285:9005:02b0		(Sunrise Lake ARK)
	9005:0285:9005:02b1	Adaptec	(Voodoo 8 internal 8 external)
	9005:0285:108e:7aac	SUN	STK RAID REM (Voodoo44 Coyote)
	9005:0285:108e:0286	SUN	STK RAID INT (Cougar)
	9005:0285:108e:0287	SUN	STK RAID EXT (Prometheus)
	9005:0285:108e:7aae	SUN	STK RAID EM (Narvi)
	===================	=======	=======================================

People
======

Alan Cox <alan@lxorguk.ukuu.org.uk>

Christoph Hellwig <hch@infradead.org>

- updates for new-style PCI probing and SCSI host registration,
  small cleanups/fixes

Matt Domsch <matt_domsch@dell.com>

- revision ioctl, adapter messages

Deanna Bonds

- non-DASD support, PAE fibs and 64 bit, added new adaptec controllers
  added new ioctls, changed scsi interface to use new error handler,
  increased the number of fibs and outstanding commands to a container
- fixed 64bit and 64G memory model, changed confusing naming convention
  where fibs that go to the hardware are consistently called hw_fibs and
  not just fibs like the name of the driver tracking structure

Mark Salyzyn <Mark_Salyzyn@adaptec.com>

- Fixed panic issues and added some new product ids for upcoming hbas.
- Performance tuning, card failover and bug mitigations.

Achim Leubner <Achim_Leubner@adaptec.com>

- Original Driver

-------------------------

Adaptec Unix OEM Product Group

Mailing List
============

linux-scsi@vger.kernel.org (Interested parties troll here)
Also note this is very different to Brian's original driver
so don't expect him to support it.

Adaptec does support this driver.  Contact Adaptec tech support or
aacraid@adaptec.com

Original by Brian Boerner February 2001

Rewritten by Alan Cox, November 2001
