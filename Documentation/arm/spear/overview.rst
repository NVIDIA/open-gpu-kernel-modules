========================
SPEAr ARM Linux Overview
========================

Introduction
------------

  SPEAr (Structured Processor Enhanced Architecture).
  weblink : http://www.st.com/spear

  The ST Microelectronics SPEAr range of ARM9/CortexA9 System-on-Chip CPUs are
  supported by the 'spear' platform of ARM Linux. Currently SPEAr1310,
  SPEAr1340, SPEAr300, SPEAr310, SPEAr320 and SPEAr600 SOCs are supported.

  Hierarchy in SPEAr is as follows:

  SPEAr (Platform)

	- SPEAr3XX (3XX SOC series, based on ARM9)
		- SPEAr300 (SOC)
			- SPEAr300 Evaluation Board
		- SPEAr310 (SOC)
			- SPEAr310 Evaluation Board
		- SPEAr320 (SOC)
			- SPEAr320 Evaluation Board
	- SPEAr6XX (6XX SOC series, based on ARM9)
		- SPEAr600 (SOC)
			- SPEAr600 Evaluation Board
	- SPEAr13XX (13XX SOC series, based on ARM CORTEXA9)
		- SPEAr1310 (SOC)
			- SPEAr1310 Evaluation Board
		- SPEAr1340 (SOC)
			- SPEAr1340 Evaluation Board

Configuration
-------------

  A generic configuration is provided for each machine, and can be used as the
  default by::

	make spear13xx_defconfig
	make spear3xx_defconfig
	make spear6xx_defconfig

Layout
------

  The common files for multiple machine families (SPEAr3xx, SPEAr6xx and
  SPEAr13xx) are located in the platform code contained in arch/arm/plat-spear
  with headers in plat/.

  Each machine series have a directory with name arch/arm/mach-spear followed by
  series name. Like mach-spear3xx, mach-spear6xx and mach-spear13xx.

  Common file for machines of spear3xx family is mach-spear3xx/spear3xx.c, for
  spear6xx is mach-spear6xx/spear6xx.c and for spear13xx family is
  mach-spear13xx/spear13xx.c. mach-spear* also contain soc/machine specific
  files, like spear1310.c, spear1340.c spear300.c, spear310.c, spear320.c and
  spear600.c.  mach-spear* doesn't contains board specific files as they fully
  support Flattened Device Tree.


Document Author
---------------

  Viresh Kumar <vireshk@kernel.org>, (c) 2010-2012 ST Microelectronics
