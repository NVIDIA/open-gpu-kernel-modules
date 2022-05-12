========================
Kernel driver i2c-sis96x
========================

Replaces 2.4.x i2c-sis645

Supported adapters:

  * Silicon Integrated Systems Corp (SiS)

    Any combination of these host bridges:
	645, 645DX (aka 646), 648, 650, 651, 655, 735, 745, 746

    and these south bridges:
	961, 962, 963(L)

Author: Mark M. Hoffman <mhoffman@lightlink.com>

Description
-----------

This SMBus only driver is known to work on motherboards with the above
named chipset combinations. The driver was developed without benefit of a
proper datasheet from SiS. The SMBus registers are assumed compatible with
those of the SiS630, although they are located in a completely different
place. Thanks to Alexander Malysh <amalysh@web.de> for providing the
SiS630 datasheet (and  driver).

The command ``lspci`` as root should produce something like these lines::

  00:00.0 Host bridge: Silicon Integrated Systems [SiS]: Unknown device 0645
  00:02.0 ISA bridge: Silicon Integrated Systems [SiS] 85C503/5513
  00:02.1 SMBus: Silicon Integrated Systems [SiS]: Unknown device 0016

or perhaps this::

  00:00.0 Host bridge: Silicon Integrated Systems [SiS]: Unknown device 0645
  00:02.0 ISA bridge: Silicon Integrated Systems [SiS]: Unknown device 0961
  00:02.1 SMBus: Silicon Integrated Systems [SiS]: Unknown device 0016

(kernel versions later than 2.4.18 may fill in the "Unknown"s)

If you can't see it please look on quirk_sis_96x_smbus
(drivers/pci/quirks.c) (also if southbridge detection fails)

I suspect that this driver could be made to work for the following SiS
chipsets as well: 635, and 635T. If anyone owns a board with those chips
AND is willing to risk crashing & burning an otherwise well-behaved kernel
in the name of progress... please contact me at <mhoffman@lightlink.com> or
via the linux-i2c mailing list: <linux-i2c@vger.kernel.org>.  Please send bug
reports and/or success stories as well.


TO DOs
------

* The driver does not support SMBus block reads/writes; I may add them if a
  scenario is found where they're needed.


Thank You
---------

Mark D. Studebaker <mdsxyz123@yahoo.com>
 - design hints and bug fixes

Alexander Maylsh <amalysh@web.de>
 - ditto, plus an important datasheet... almost the one I really wanted

Hans-Günter Lütke Uphues <hg_lu@t-online.de>
 - patch for SiS735

Robert Zwerus <arzie@dds.nl>
 - testing for SiS645DX

Kianusch Sayah Karadji <kianusch@sk-tech.net>
 - patch for SiS645DX/962

Ken Healy
 - patch for SiS655

To anyone else who has written w/ feedback, thanks!
