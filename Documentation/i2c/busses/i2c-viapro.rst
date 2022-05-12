========================
Kernel driver i2c-viapro
========================

Supported adapters:
  * VIA Technologies, Inc. VT82C596A/B
    Datasheet: Sometimes available at the VIA website

  * VIA Technologies, Inc. VT82C686A/B
    Datasheet: Sometimes available at the VIA website

  * VIA Technologies, Inc. VT8231, VT8233, VT8233A
    Datasheet: available on request from VIA

  * VIA Technologies, Inc. VT8235, VT8237R, VT8237A, VT8237S, VT8251
    Datasheet: available on request and under NDA from VIA

  * VIA Technologies, Inc. CX700
    Datasheet: available on request and under NDA from VIA

  * VIA Technologies, Inc. VX800/VX820
    Datasheet: available on http://linux.via.com.tw

  * VIA Technologies, Inc. VX855/VX875
    Datasheet: available on http://linux.via.com.tw

  * VIA Technologies, Inc. VX900
    Datasheet: available on http://linux.via.com.tw

Authors:
	- Kyösti Mälkki <kmalkki@cc.hut.fi>,
	- Mark D. Studebaker <mdsxyz123@yahoo.com>,
	- Jean Delvare <jdelvare@suse.de>

Module Parameters
-----------------

* force: int
  Forcibly enable the SMBus controller. DANGEROUS!
* force_addr: int
  Forcibly enable the SMBus at the given address. EXTREMELY DANGEROUS!

Description
-----------

i2c-viapro is a true SMBus host driver for motherboards with one of the
supported VIA south bridges.

Your ``lspci -n`` listing must show one of these :

 ================   ======================
 device 1106:3050   (VT82C596A function 3)
 device 1106:3051   (VT82C596B function 3)
 device 1106:3057   (VT82C686 function 4)
 device 1106:3074   (VT8233)
 device 1106:3147   (VT8233A)
 device 1106:8235   (VT8231 function 4)
 device 1106:3177   (VT8235)
 device 1106:3227   (VT8237R)
 device 1106:3337   (VT8237A)
 device 1106:3372   (VT8237S)
 device 1106:3287   (VT8251)
 device 1106:8324   (CX700)
 device 1106:8353   (VX800/VX820)
 device 1106:8409   (VX855/VX875)
 device 1106:8410   (VX900)
 ================   ======================

If none of these show up, you should look in the BIOS for settings like
enable ACPI / SMBus or even USB.

Except for the oldest chips (VT82C596A/B, VT82C686A and most probably
VT8231), this driver supports I2C block transactions. Such transactions
are mainly useful to read from and write to EEPROMs.

The CX700/VX800/VX820 additionally appears to support SMBus PEC, although
this driver doesn't implement it yet.
