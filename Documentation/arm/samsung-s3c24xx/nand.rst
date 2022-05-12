====================
S3C24XX NAND Support
====================

Introduction
------------

Small Page NAND
---------------

The driver uses a 512 byte (1 page) ECC code for this setup. The
ECC code is not directly compatible with the default kernel ECC
code, so the driver enforces its own OOB layout and ECC parameters

Large Page NAND
---------------

The driver is capable of handling NAND flash with a 2KiB page
size, with support for hardware ECC generation and correction.

Unlike the 512byte page mode, the driver generates ECC data for
each 256 byte block in an 2KiB page. This means that more than
one error in a page can be rectified. It also means that the
OOB layout remains the default kernel layout for these flashes.


Document Author
---------------

Ben Dooks, Copyright 2007 Simtec Electronics
