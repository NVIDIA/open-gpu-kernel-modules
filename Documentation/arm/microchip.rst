=============================
ARM Microchip SoCs (aka AT91)
=============================


Introduction
------------
This document gives useful information about the ARM Microchip SoCs that are
currently supported in Linux Mainline (you know, the one on kernel.org).

It is important to note that the Microchip (previously Atmel) ARM-based MPU
product line is historically named "AT91" or "at91" throughout the Linux kernel
development process even if this product prefix has completely disappeared from
the official Microchip product name. Anyway, files, directories, git trees,
git branches/tags and email subject always contain this "at91" sub-string.


AT91 SoCs
---------
Documentation and detailed datasheet for each product are available on
the Microchip website: http://www.microchip.com.

  Flavors:
    * ARM 920 based SoC
      - at91rm9200

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-1768-32-bit-ARM920T-Embedded-Microprocessor-AT91RM9200_Datasheet.pdf

    * ARM 926 based SoCs
      - at91sam9260

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-6221-32-bit-ARM926EJ-S-Embedded-Microprocessor-SAM9260_Datasheet.pdf

      - at91sam9xe

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-6254-32-bit-ARM926EJ-S-Embedded-Microprocessor-SAM9XE_Datasheet.pdf

      - at91sam9261

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-6062-ARM926EJ-S-Microprocessor-SAM9261_Datasheet.pdf

      - at91sam9263

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-6249-32-bit-ARM926EJ-S-Embedded-Microprocessor-SAM9263_Datasheet.pdf

      - at91sam9rl

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/doc6289.pdf

      - at91sam9g20

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/DS60001516A.pdf

      - at91sam9g45 family
        - at91sam9g45
        - at91sam9g46
        - at91sam9m10
        - at91sam9m11 (device superset)

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-6437-32-bit-ARM926-Embedded-Microprocessor-SAM9M11_Datasheet.pdf

      - at91sam9x5 family (aka "The 5 series")
        - at91sam9g15
        - at91sam9g25
        - at91sam9g35
        - at91sam9x25
        - at91sam9x35

          * Datasheet (can be considered as covering the whole family)

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-11055-32-bit-ARM926EJ-S-Microcontroller-SAM9X35_Datasheet.pdf

      - at91sam9n12

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/DS60001517A.pdf

      - sam9x60

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/SAM9X60-Data-Sheet-DS60001579A.pdf

    * ARM Cortex-A5 based SoCs
      - sama5d3 family

        - sama5d31
        - sama5d33
        - sama5d34
        - sama5d35
        - sama5d36 (device superset)

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-11121-32-bit-Cortex-A5-Microcontroller-SAMA5D3_Datasheet_B.pdf

    * ARM Cortex-A5 + NEON based SoCs
      - sama5d4 family

        - sama5d41
        - sama5d42
        - sama5d43
        - sama5d44 (device superset)

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/60001525A.pdf

      - sama5d2 family

        - sama5d21
        - sama5d22
        - sama5d23
        - sama5d24
        - sama5d26
        - sama5d27 (device superset)
        - sama5d28 (device superset + environmental monitors)

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/DS60001476B.pdf

    * ARM Cortex-M7 MCUs
      - sams70 family

        - sams70j19
        - sams70j20
        - sams70j21
        - sams70n19
        - sams70n20
        - sams70n21
        - sams70q19
        - sams70q20
        - sams70q21

      - samv70 family

        - samv70j19
        - samv70j20
        - samv70n19
        - samv70n20
        - samv70q19
        - samv70q20

      - samv71 family

        - samv71j19
        - samv71j20
        - samv71j21
        - samv71n19
        - samv71n20
        - samv71n21
        - samv71q19
        - samv71q20
        - samv71q21

          * Datasheet

          http://ww1.microchip.com/downloads/en/DeviceDoc/SAM-E70-S70-V70-V71-Family-Data-Sheet-DS60001527D.pdf


Linux kernel information
------------------------
Linux kernel mach directory: arch/arm/mach-at91
MAINTAINERS entry is: "ARM/Microchip (AT91) SoC support"


Device Tree for AT91 SoCs and boards
------------------------------------
All AT91 SoCs are converted to Device Tree. Since Linux 3.19, these products
must use this method to boot the Linux kernel.

Work In Progress statement:
Device Tree files and Device Tree bindings that apply to AT91 SoCs and boards are
considered as "Unstable". To be completely clear, any at91 binding can change at
any time. So, be sure to use a Device Tree Binary and a Kernel Image generated from
the same source tree.
Please refer to the Documentation/devicetree/bindings/ABI.rst file for a
definition of a "Stable" binding/ABI.
This statement will be removed by AT91 MAINTAINERS when appropriate.

Naming conventions and best practice:

- SoCs Device Tree Source Include files are named after the official name of
  the product (at91sam9g20.dtsi or sama5d33.dtsi for instance).
- Device Tree Source Include files (.dtsi) are used to collect common nodes that can be
  shared across SoCs or boards (sama5d3.dtsi or at91sam9x5cm.dtsi for instance).
  When collecting nodes for a particular peripheral or topic, the identifier have to
  be placed at the end of the file name, separated with a "_" (at91sam9x5_can.dtsi
  or sama5d3_gmac.dtsi for example).
- board Device Tree Source files (.dts) are prefixed by the string "at91-" so
  that they can be identified easily. Note that some files are historical exceptions
  to this rule (sama5d3[13456]ek.dts, usb_a9g20.dts or animeo_ip.dts for example).
