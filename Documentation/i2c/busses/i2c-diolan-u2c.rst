============================
Kernel driver i2c-diolan-u2c
============================

Supported adapters:
  * Diolan U2C-12 I2C-USB adapter

    Documentation:
	http://www.diolan.com/i2c/u2c12.html

Author: Guenter Roeck <linux@roeck-us.net>

Description
-----------

This is the driver for the Diolan U2C-12 USB-I2C adapter.

The Diolan U2C-12 I2C-USB Adapter provides a low cost solution to connect
a computer to I2C slave devices using a USB interface. It also supports
connectivity to SPI devices.

This driver only supports the I2C interface of U2C-12. The driver does not use
interrupts.


Module parameters
-----------------

* frequency: I2C bus frequency
