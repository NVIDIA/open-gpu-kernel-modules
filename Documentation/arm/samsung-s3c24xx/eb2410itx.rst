===================================
Simtec Electronics EB2410ITX (BAST)
===================================

	http://www.simtec.co.uk/products/EB2410ITX/

Introduction
------------

  The EB2410ITX is a S3C2410 based development board with a variety of
  peripherals and expansion connectors. This board is also known by
  the shortened name of Bast.


Configuration
-------------

  To set the default configuration, use `make bast_defconfig` which
  supports the commonly used features of this board.


Support
-------

  Official support information can be found on the Simtec Electronics
  website, at the product page http://www.simtec.co.uk/products/EB2410ITX/

  Useful links:

    - Resources Page http://www.simtec.co.uk/products/EB2410ITX/resources.html

    - Board FAQ at http://www.simtec.co.uk/products/EB2410ITX/faq.html

    - Bootloader info http://www.simtec.co.uk/products/SWABLE/resources.html
      and FAQ http://www.simtec.co.uk/products/SWABLE/faq.html


MTD
---

  The NAND and NOR support has been merged from the linux-mtd project.
  Any problems, see http://www.linux-mtd.infradead.org/ for more
  information or up-to-date versions of linux-mtd.


IDE
---

  Both onboard IDE ports are supported, however there is no support for
  changing speed of devices, PIO Mode 4 capable drives should be used.


Maintainers
-----------

  This board is maintained by Simtec Electronics.


Copyright 2004 Ben Dooks, Simtec Electronics
