========================
Kernel driver i2c-sis630
========================

Supported adapters:
  * Silicon Integrated Systems Corp (SiS)
	630 chipset (Datasheet: available at http://www.sfr-fresh.com/linux)
	730 chipset
	964 chipset
  * Possible other SiS chipsets ?

Author:
        - Alexander Malysh <amalysh@web.de>
	- Amaury Decrême <amaury.decreme@gmail.com> - SiS964 support

Module Parameters
-----------------

==================      =====================================================
force = [1|0]           Forcibly enable the SIS630. DANGEROUS!
                        This can be interesting for chipsets not named
                        above to check if it works for you chipset,
                        but DANGEROUS!

high_clock = [1|0]      Forcibly set Host Master Clock to 56KHz (default,
			what your BIOS use). DANGEROUS! This should be a bit
			faster, but freeze some systems (i.e. my Laptop).
			SIS630/730 chip only.
==================      =====================================================


Description
-----------

This SMBus only driver is known to work on motherboards with the above
named chipsets.

If you see something like this::

  00:00.0 Host bridge: Silicon Integrated Systems [SiS] 630 Host (rev 31)
  00:01.0 ISA bridge: Silicon Integrated Systems [SiS] 85C503/5513

or like this::

  00:00.0 Host bridge: Silicon Integrated Systems [SiS] 730 Host (rev 02)
  00:01.0 ISA bridge: Silicon Integrated Systems [SiS] 85C503/5513

or like this::

  00:00.0 Host bridge: Silicon Integrated Systems [SiS] 760/M760 Host (rev 02)
  00:02.0 ISA bridge: Silicon Integrated Systems [SiS] SiS964 [MuTIOL Media IO]
							LPC Controller (rev 36)

in your ``lspci`` output , then this driver is for your chipset.

Thank You
---------
Philip Edelbrock <phil@netroedge.com>
- testing SiS730 support
Mark M. Hoffman <mhoffman@lightlink.com>
- bug fixes

To anyone else which I forgot here ;), thanks!
