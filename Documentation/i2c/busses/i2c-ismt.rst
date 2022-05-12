======================
Kernel driver i2c-ismt
======================


Supported adapters:
  * Intel S12xx series SOCs

Authors:
	Bill Brown <bill.e.brown@intel.com>


Module Parameters
-----------------

* bus_speed (unsigned int)

Allows changing of the bus speed.  Normally, the bus speed is set by the BIOS
and never needs to be changed.  However, some SMBus analyzers are too slow for
monitoring the bus during debug, thus the need for this module parameter.
Specify the bus speed in kHz.

Available bus frequency settings:

  ====   =========
  0      no change
  80     kHz
  100    kHz
  400    kHz
  1000   kHz
  ====   =========


Description
-----------

The S12xx series of SOCs have a pair of integrated SMBus 2.0 controllers
targeted primarily at the microserver and storage markets.

The S12xx series contain a pair of PCI functions.  An output of lspci will show
something similar to the following::

  00:13.0 System peripheral: Intel Corporation Centerton SMBus 2.0 Controller 0
  00:13.1 System peripheral: Intel Corporation Centerton SMBus 2.0 Controller 1
