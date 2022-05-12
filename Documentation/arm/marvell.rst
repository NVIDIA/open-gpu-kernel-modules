================
ARM Marvell SoCs
================

This document lists all the ARM Marvell SoCs that are currently
supported in mainline by the Linux kernel. As the Marvell families of
SoCs are large and complex, it is hard to understand where the support
for a particular SoC is available in the Linux kernel. This document
tries to help in understanding where those SoCs are supported, and to
match them with their corresponding public datasheet, when available.

Orion family
------------

  Flavors:
        - 88F5082
        - 88F5181
        - 88F5181L
        - 88F5182

               - Datasheet: https://web.archive.org/web/20210124231420/http://csclub.uwaterloo.ca/~board/ts7800/MV88F5182-datasheet.pdf
               - Programmer's User Guide: https://web.archive.org/web/20210124231536/http://csclub.uwaterloo.ca/~board/ts7800/MV88F5182-opensource-manual.pdf
               - User Manual: https://web.archive.org/web/20210124231631/http://csclub.uwaterloo.ca/~board/ts7800/MV88F5182-usermanual.pdf
        - 88F5281

               - Datasheet: https://web.archive.org/web/20131028144728/http://www.ocmodshop.com/images/reviews/networking/qnap_ts409u/marvel_88f5281_data_sheet.pdf
        - 88F6183
  Core:
	Feroceon 88fr331 (88f51xx) or 88fr531-vd (88f52xx) ARMv5 compatible
  Linux kernel mach directory:
	arch/arm/mach-orion5x
  Linux kernel plat directory:
	arch/arm/plat-orion

Kirkwood family
---------------

  Flavors:
        - 88F6282 a.k.a Armada 300

                - Product Brief  : https://web.archive.org/web/20111027032509/http://www.marvell.com/embedded-processors/armada-300/assets/armada_310.pdf
        - 88F6283 a.k.a Armada 310

                - Product Brief  : https://web.archive.org/web/20111027032509/http://www.marvell.com/embedded-processors/armada-300/assets/armada_310.pdf
        - 88F6190

                - Product Brief  : https://web.archive.org/web/20130730072715/http://www.marvell.com/embedded-processors/kirkwood/assets/88F6190-003_WEB.pdf
                - Hardware Spec  : https://web.archive.org/web/20121021182835/http://www.marvell.com/embedded-processors/kirkwood/assets/HW_88F619x_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20130730091033/http://www.marvell.com/embedded-processors/kirkwood/assets/FS_88F6180_9x_6281_OpenSource.pdf
        - 88F6192

                - Product Brief  : https://web.archive.org/web/20131113121446/http://www.marvell.com/embedded-processors/kirkwood/assets/88F6192-003_ver1.pdf
                - Hardware Spec  : https://web.archive.org/web/20121021182835/http://www.marvell.com/embedded-processors/kirkwood/assets/HW_88F619x_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20130730091033/http://www.marvell.com/embedded-processors/kirkwood/assets/FS_88F6180_9x_6281_OpenSource.pdf
        - 88F6182
        - 88F6180

                - Product Brief  : https://web.archive.org/web/20120616201621/http://www.marvell.com/embedded-processors/kirkwood/assets/88F6180-003_ver1.pdf
                - Hardware Spec  : https://web.archive.org/web/20130730091654/http://www.marvell.com/embedded-processors/kirkwood/assets/HW_88F6180_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20130730091033/http://www.marvell.com/embedded-processors/kirkwood/assets/FS_88F6180_9x_6281_OpenSource.pdf
        - 88F6281

                - Product Brief  : https://web.archive.org/web/20120131133709/http://www.marvell.com/embedded-processors/kirkwood/assets/88F6281-004_ver1.pdf
                - Hardware Spec  : https://web.archive.org/web/20120620073511/http://www.marvell.com/embedded-processors/kirkwood/assets/HW_88F6281_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20130730091033/http://www.marvell.com/embedded-processors/kirkwood/assets/FS_88F6180_9x_6281_OpenSource.pdf
  Homepage:
	https://web.archive.org/web/20160513194943/http://www.marvell.com/embedded-processors/kirkwood/
  Core:
	Feroceon 88fr131 ARMv5 compatible
  Linux kernel mach directory:
	arch/arm/mach-mvebu
  Linux kernel plat directory:
	none

Discovery family
----------------

  Flavors:
        - MV78100

                - Product Brief  : https://web.archive.org/web/20120616194711/http://www.marvell.com/embedded-processors/discovery-innovation/assets/MV78100-003_WEB.pdf
                - Hardware Spec  : https://web.archive.org/web/20141005120451/http://www.marvell.com/embedded-processors/discovery-innovation/assets/HW_MV78100_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20111110081125/http://www.marvell.com/embedded-processors/discovery-innovation/assets/FS_MV76100_78100_78200_OpenSource.pdf
        - MV78200

                - Product Brief  : https://web.archive.org/web/20140801121623/http://www.marvell.com/embedded-processors/discovery-innovation/assets/MV78200-002_WEB.pdf
                - Hardware Spec  : https://web.archive.org/web/20141005120458/http://www.marvell.com/embedded-processors/discovery-innovation/assets/HW_MV78200_OpenSource.pdf
                - Functional Spec: https://web.archive.org/web/20111110081125/http://www.marvell.com/embedded-processors/discovery-innovation/assets/FS_MV76100_78100_78200_OpenSource.pdf

        - MV76100

                Not supported by the Linux kernel.

  Core:
	Feroceon 88fr571-vd ARMv5 compatible

  Linux kernel mach directory:
	arch/arm/mach-mv78xx0
  Linux kernel plat directory:
	arch/arm/plat-orion

EBU Armada family
-----------------

  Armada 370 Flavors:
        - 88F6710
        - 88F6707
        - 88F6W11

    - Product Brief:   https://web.archive.org/web/20121115063038/http://www.marvell.com/embedded-processors/armada-300/assets/Marvell_ARMADA_370_SoC.pdf
    - Hardware Spec:   https://web.archive.org/web/20140617183747/http://www.marvell.com/embedded-processors/armada-300/assets/ARMADA370-datasheet.pdf
    - Functional Spec: https://web.archive.org/web/20140617183701/http://www.marvell.com/embedded-processors/armada-300/assets/ARMADA370-FunctionalSpec-datasheet.pdf

  Core:
	Sheeva ARMv7 compatible PJ4B

  Armada 375 Flavors:
	- 88F6720

    - Product Brief: https://web.archive.org/web/20131216023516/http://www.marvell.com/embedded-processors/armada-300/assets/ARMADA_375_SoC-01_product_brief.pdf

  Core:
	ARM Cortex-A9

  Armada 38x Flavors:
	- 88F6810	Armada 380
	- 88F6820 Armada 385
	- 88F6828 Armada 388

    - Product infos:   https://web.archive.org/web/20181006144616/http://www.marvell.com/embedded-processors/armada-38x/
    - Functional Spec: https://web.archive.org/web/20200420191927/https://www.marvell.com/content/dam/marvell/en/public-collateral/embedded-processors/marvell-embedded-processors-armada-38x-functional-specifications-2015-11.pdf

  Core:
	ARM Cortex-A9

  Armada 39x Flavors:
	- 88F6920 Armada 390
	- 88F6928 Armada 398

    - Product infos: https://web.archive.org/web/20181020222559/http://www.marvell.com/embedded-processors/armada-39x/

  Core:
	ARM Cortex-A9

  Armada XP Flavors:
        - MV78230
        - MV78260
        - MV78460

    NOTE:
	not to be confused with the non-SMP 78xx0 SoCs

    Product Brief:
	https://web.archive.org/web/20121021173528/http://www.marvell.com/embedded-processors/armada-xp/assets/Marvell-ArmadaXP-SoC-product%20brief.pdf

    Functional Spec:
	https://web.archive.org/web/20180829171131/http://www.marvell.com/embedded-processors/armada-xp/assets/ARMADA-XP-Functional-SpecDatasheet.pdf

    - Hardware Specs:

        - https://web.archive.org/web/20141127013651/http://www.marvell.com/embedded-processors/armada-xp/assets/HW_MV78230_OS.PDF
        - https://web.archive.org/web/20141222000224/http://www.marvell.com/embedded-processors/armada-xp/assets/HW_MV78260_OS.PDF
        - https://web.archive.org/web/20141222000230/http://www.marvell.com/embedded-processors/armada-xp/assets/HW_MV78460_OS.PDF

  Core:
	Sheeva ARMv7 compatible Dual-core or Quad-core PJ4B-MP

  Linux kernel mach directory:
	arch/arm/mach-mvebu
  Linux kernel plat directory:
	none

EBU Armada family ARMv8
-----------------------

  Armada 3710/3720 Flavors:
	- 88F3710
	- 88F3720

  Core:
	ARM Cortex A53 (ARMv8)

  Homepage:
	https://web.archive.org/web/20181103003602/http://www.marvell.com/embedded-processors/armada-3700/

  Product Brief:
	https://web.archive.org/web/20210121194810/https://www.marvell.com/content/dam/marvell/en/public-collateral/embedded-processors/marvell-embedded-processors-armada-37xx-product-brief-2016-01.pdf

  Hardware Spec:
	https://web.archive.org/web/20210202162011/http://www.marvell.com/content/dam/marvell/en/public-collateral/embedded-processors/marvell-embedded-processors-armada-37xx-hardware-specifications-2019-09.pdf

  Device tree files:
	arch/arm64/boot/dts/marvell/armada-37*

  Armada 7K Flavors:
	  - 88F7020 (AP806 Dual + one CP110)
	  - 88F7040 (AP806 Quad + one CP110)

  Core: ARM Cortex A72

  Homepage:
	https://web.archive.org/web/20181020222606/http://www.marvell.com/embedded-processors/armada-70xx/

  Product Brief:
	  - https://web.archive.org/web/20161010105541/http://www.marvell.com/embedded-processors/assets/Armada7020PB-Jan2016.pdf
	  - https://web.archive.org/web/20160928154533/http://www.marvell.com/embedded-processors/assets/Armada7040PB-Jan2016.pdf

  Device tree files:
	arch/arm64/boot/dts/marvell/armada-70*

  Armada 8K Flavors:
	- 88F8020 (AP806 Dual + two CP110)
	- 88F8040 (AP806 Quad + two CP110)
  Core:
	ARM Cortex A72

  Homepage:
	https://web.archive.org/web/20181022004830/http://www.marvell.com/embedded-processors/armada-80xx/

  Product Brief:
	  - https://web.archive.org/web/20210124233728/https://www.marvell.com/content/dam/marvell/en/public-collateral/embedded-processors/marvell-embedded-processors-armada-8020-product-brief-2017-12.pdf
	  - https://web.archive.org/web/20161010105532/http://www.marvell.com/embedded-processors/assets/Armada8040PB-Jan2016.pdf

  Device tree files:
	arch/arm64/boot/dts/marvell/armada-80*

Avanta family
-------------

  Flavors:
       - 88F6510
       - 88F6530P
       - 88F6550
       - 88F6560

  Homepage:
	https://web.archive.org/web/20181005145041/http://www.marvell.com/broadband/

  Product Brief:
	https://web.archive.org/web/20180829171057/http://www.marvell.com/broadband/assets/Marvell_Avanta_88F6510_305_060-001_product_brief.pdf

  No public datasheet available.

  Core:
	ARMv5 compatible

  Linux kernel mach directory:
	no code in mainline yet, planned for the future
  Linux kernel plat directory:
	no code in mainline yet, planned for the future

Storage family
--------------

  Armada SP:
	- 88RC1580

  Product infos:
	https://web.archive.org/web/20191129073953/http://www.marvell.com/storage/armada-sp/

  Core:
	Sheeva ARMv7 comatible Quad-core PJ4C

  (not supported in upstream Linux kernel)

Dove family (application processor)
-----------------------------------

  Flavors:
        - 88AP510 a.k.a Armada 510

   Product Brief:
	https://web.archive.org/web/20111102020643/http://www.marvell.com/application-processors/armada-500/assets/Marvell_Armada510_SoC.pdf

   Hardware Spec:
	https://web.archive.org/web/20160428160231/http://www.marvell.com/application-processors/armada-500/assets/Armada-510-Hardware-Spec.pdf

  Functional Spec:
	https://web.archive.org/web/20120130172443/http://www.marvell.com/application-processors/armada-500/assets/Armada-510-Functional-Spec.pdf

  Homepage:
	https://web.archive.org/web/20160822232651/http://www.marvell.com/application-processors/armada-500/

  Core:
	ARMv7 compatible

  Directory:
	- arch/arm/mach-mvebu (DT enabled platforms)
        - arch/arm/mach-dove (non-DT enabled platforms)

PXA 2xx/3xx/93x/95x family
--------------------------

  Flavors:
        - PXA21x, PXA25x, PXA26x
             - Application processor only
             - Core: ARMv5 XScale1 core
        - PXA270, PXA271, PXA272
             - Product Brief         : https://web.archive.org/web/20150927135510/http://www.marvell.com/application-processors/pxa-family/assets/pxa_27x_pb.pdf
             - Design guide          : https://web.archive.org/web/20120111181937/http://www.marvell.com/application-processors/pxa-family/assets/pxa_27x_design_guide.pdf
             - Developers manual     : https://web.archive.org/web/20150927164805/http://www.marvell.com/application-processors/pxa-family/assets/pxa_27x_dev_man.pdf
             - Specification         : https://web.archive.org/web/20140211221535/http://www.marvell.com/application-processors/pxa-family/assets/pxa_27x_emts.pdf
             - Specification update  : https://web.archive.org/web/20120111104906/http://www.marvell.com/application-processors/pxa-family/assets/pxa_27x_spec_update.pdf
             - Application processor only
             - Core: ARMv5 XScale2 core
        - PXA300, PXA310, PXA320
             - PXA 300 Product Brief : https://web.archive.org/web/20120111121203/http://www.marvell.com/application-processors/pxa-family/assets/PXA300_PB_R4.pdf
             - PXA 310 Product Brief : https://web.archive.org/web/20120111104515/http://www.marvell.com/application-processors/pxa-family/assets/PXA310_PB_R4.pdf
             - PXA 320 Product Brief : https://web.archive.org/web/20121021182826/http://www.marvell.com/application-processors/pxa-family/assets/PXA320_PB_R4.pdf
             - Design guide          : https://web.archive.org/web/20130727144625/http://www.marvell.com/application-processors/pxa-family/assets/PXA3xx_Design_Guide.pdf
             - Developers manual     : https://web.archive.org/web/20130727144605/http://www.marvell.com/application-processors/pxa-family/assets/PXA3xx_Developers_Manual.zip
             - Specifications        : https://web.archive.org/web/20130727144559/http://www.marvell.com/application-processors/pxa-family/assets/PXA3xx_EMTS.pdf
             - Specification Update  : https://web.archive.org/web/20150927183411/http://www.marvell.com/application-processors/pxa-family/assets/PXA3xx_Spec_Update.zip
             - Reference Manual      : https://web.archive.org/web/20120111103844/http://www.marvell.com/application-processors/pxa-family/assets/PXA3xx_TavorP_BootROM_Ref_Manual.pdf
             - Application processor only
             - Core: ARMv5 XScale3 core
        - PXA930, PXA935
             - Application processor with Communication processor
             - Core: ARMv5 XScale3 core
        - PXA955
             - Application processor with Communication processor
             - Core: ARMv7 compatible Sheeva PJ4 core

   Comments:

    * This line of SoCs originates from the XScale family developed by
      Intel and acquired by Marvell in ~2006. The PXA21x, PXA25x,
      PXA26x, PXA27x, PXA3xx and PXA93x were developed by Intel, while
      the later PXA95x were developed by Marvell.

    * Due to their XScale origin, these SoCs have virtually nothing in
      common with the other (Kirkwood, Dove, etc.) families of Marvell
      SoCs, except with the MMP/MMP2 family of SoCs.

   Linux kernel mach directory:
	arch/arm/mach-pxa
   Linux kernel plat directory:
	arch/arm/plat-pxa

MMP/MMP2/MMP3 family (communication processor)
----------------------------------------------

   Flavors:
        - PXA168, a.k.a Armada 168
             - Homepage             : https://web.archive.org/web/20110926014256/http://www.marvell.com/application-processors/armada-100/armada-168.jsp
             - Product brief        : https://web.archive.org/web/20111102030100/http://www.marvell.com/application-processors/armada-100/assets/pxa_168_pb.pdf
             - Hardware manual      : https://web.archive.org/web/20160428165359/http://www.marvell.com/application-processors/armada-100/assets/armada_16x_datasheet.pdf
             - Software manual      : https://web.archive.org/web/20160428154454/http://www.marvell.com/application-processors/armada-100/assets/armada_16x_software_manual.pdf
             - Specification update : https://web.archive.org/web/20150927160338/http://www.marvell.com/application-processors/armada-100/assets/ARMADA16x_Spec_update.pdf
             - Boot ROM manual      : https://web.archive.org/web/20130727205559/http://www.marvell.com/application-processors/armada-100/assets/armada_16x_ref_manual.pdf
             - App node package     : https://web.archive.org/web/20141005090706/http://www.marvell.com/application-processors/armada-100/assets/armada_16x_app_note_package.pdf
             - Application processor only
             - Core: ARMv5 compatible Marvell PJ1 88sv331 (Mohawk)
        - PXA910/PXA920
             - Homepage             : https://web.archive.org/web/20150928121236/http://www.marvell.com/communication-processors/pxa910/
             - Product Brief        : https://archive.org/download/marvell-pxa910-pb/Marvell_PXA910_Platform-001_PB.pdf
             - Application processor with Communication processor
             - Core: ARMv5 compatible Marvell PJ1 88sv331 (Mohawk)
        - PXA688, a.k.a. MMP2, a.k.a Armada 610 (OLPC XO-1.75)
             - Product Brief        : https://web.archive.org/web/20111102023255/http://www.marvell.com/application-processors/armada-600/assets/armada610_pb.pdf
             - Application processor only
             - Core: ARMv7 compatible Sheeva PJ4 88sv581x core
	- PXA2128, a.k.a. MMP3, a.k.a Armada 620 (OLPC XO-4)
	     - Product Brief	    : https://web.archive.org/web/20120824055155/http://www.marvell.com/application-processors/armada/pxa2128/assets/Marvell-ARMADA-PXA2128-SoC-PB.pdf
	     - Application processor only
	     - Core: Dual-core ARMv7 compatible Sheeva PJ4C core
	- PXA960/PXA968/PXA978 (Linux support not upstream)
	     - Application processor with Communication Processor
	     - Core: ARMv7 compatible Sheeva PJ4 core
	- PXA986/PXA988 (Linux support not upstream)
	     - Application processor with Communication Processor
	     - Core: Dual-core ARMv7 compatible Sheeva PJ4B-MP core
	- PXA1088/PXA1920 (Linux support not upstream)
	     - Application processor with Communication Processor
	     - Core: quad-core ARMv7 Cortex-A7
	- PXA1908/PXA1928/PXA1936
	     - Application processor with Communication Processor
	     - Core: multi-core ARMv8 Cortex-A53

   Comments:

    * This line of SoCs originates from the XScale family developed by
      Intel and acquired by Marvell in ~2006. All the processors of
      this MMP/MMP2 family were developed by Marvell.

    * Due to their XScale origin, these SoCs have virtually nothing in
      common with the other (Kirkwood, Dove, etc.) families of Marvell
      SoCs, except with the PXA family of SoCs listed above.

   Linux kernel mach directory:
	arch/arm/mach-mmp
   Linux kernel plat directory:
	arch/arm/plat-pxa

Berlin family (Multimedia Solutions)
-------------------------------------

  - Flavors:
	- 88DE3010, Armada 1000 (no Linux support)
		- Core:		Marvell PJ1 (ARMv5TE), Dual-core
		- Product Brief:	http://www.marvell.com.cn/digital-entertainment/assets/armada_1000_pb.pdf
	- 88DE3005, Armada 1500 Mini
		- Design name:	BG2CD
		- Core:		ARM Cortex-A9, PL310 L2CC
	- 88DE3006, Armada 1500 Mini Plus
		- Design name:	BG2CDP
		- Core:		Dual Core ARM Cortex-A7
	- 88DE3100, Armada 1500
		- Design name:	BG2
		- Core:		Marvell PJ4B-MP (ARMv7), Tauros3 L2CC
	- 88DE3114, Armada 1500 Pro
		- Design name:	BG2Q
		- Core:		Quad Core ARM Cortex-A9, PL310 L2CC
	- 88DE3214, Armada 1500 Pro 4K
		- Design name:	BG3
		- Core:		ARM Cortex-A15, CA15 integrated L2CC
	- 88DE3218, ARMADA 1500 Ultra
		- Core:		ARM Cortex-A53

  Homepage: https://www.synaptics.com/products/multimedia-solutions
  Directory: arch/arm/mach-berlin

  Comments:

   * This line of SoCs is based on Marvell Sheeva or ARM Cortex CPUs
     with Synopsys DesignWare (IRQ, GPIO, Timers, ...) and PXA IP (SDHCI, USB, ETH, ...).

   * The Berlin family was acquired by Synaptics from Marvell in 2017.

CPU Cores
---------

The XScale cores were designed by Intel, and shipped by Marvell in the older
PXA processors. Feroceon is a Marvell designed core that developed in-house,
and that evolved into Sheeva. The XScale and Feroceon cores were phased out
over time and replaced with Sheeva cores in later products, which subsequently
got replaced with licensed ARM Cortex-A cores.

  XScale 1
	CPUID 0x69052xxx
	ARMv5, iWMMXt
  XScale 2
	CPUID 0x69054xxx
	ARMv5, iWMMXt
  XScale 3
	CPUID 0x69056xxx or 0x69056xxx
	ARMv5, iWMMXt
  Feroceon-1850 88fr331 "Mohawk"
	CPUID 0x5615331x or 0x41xx926x
	ARMv5TE, single issue
  Feroceon-2850 88fr531-vd "Jolteon"
	CPUID 0x5605531x or 0x41xx926x
	ARMv5TE, VFP, dual-issue
  Feroceon 88fr571-vd "Jolteon"
	CPUID 0x5615571x
	ARMv5TE, VFP, dual-issue
  Feroceon 88fr131 "Mohawk-D"
	CPUID 0x5625131x
	ARMv5TE, single-issue in-order
  Sheeva PJ1 88sv331 "Mohawk"
	CPUID 0x561584xx
	ARMv5, single-issue iWMMXt v2
  Sheeva PJ4 88sv581x "Flareon"
	CPUID 0x560f581x
	ARMv7, idivt, optional iWMMXt v2
  Sheeva PJ4B 88sv581x
	CPUID 0x561f581x
	ARMv7, idivt, optional iWMMXt v2
  Sheeva PJ4B-MP / PJ4C
	CPUID 0x562f584x
	ARMv7, idivt/idiva, LPAE, optional iWMMXt v2 and/or NEON

Long-term plans
---------------

 * Unify the mach-dove/, mach-mv78xx0/, mach-orion5x/ into the
   mach-mvebu/ to support all SoCs from the Marvell EBU (Engineering
   Business Unit) in a single mach-<foo> directory. The plat-orion/
   would therefore disappear.

 * Unify the mach-mmp/ and mach-pxa/ into the same mach-pxa
   directory. The plat-pxa/ would therefore disappear.

Credits
-------

- Maen Suleiman <maen@marvell.com>
- Lior Amsalem <alior@marvell.com>
- Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
- Andrew Lunn <andrew@lunn.ch>
- Nicolas Pitre <nico@fluxnic.net>
- Eric Miao <eric.y.miao@gmail.com>
