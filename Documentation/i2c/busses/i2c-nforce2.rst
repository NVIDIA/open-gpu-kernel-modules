=========================
Kernel driver i2c-nforce2
=========================

Supported adapters:
  * nForce2 MCP                10de:0064
  * nForce2 Ultra 400 MCP      10de:0084
  * nForce3 Pro150 MCP         10de:00D4
  * nForce3 250Gb MCP          10de:00E4
  * nForce4 MCP                10de:0052
  * nForce4 MCP-04             10de:0034
  * nForce MCP51               10de:0264
  * nForce MCP55               10de:0368
  * nForce MCP61               10de:03EB
  * nForce MCP65               10de:0446
  * nForce MCP67               10de:0542
  * nForce MCP73               10de:07D8
  * nForce MCP78S              10de:0752
  * nForce MCP79               10de:0AA2

Datasheet:
           not publicly available, but seems to be similar to the
           AMD-8111 SMBus 2.0 adapter.

Authors:
	- Hans-Frieder Vogt <hfvogt@gmx.net>,
	- Thomas Leibold <thomas@plx.com>,
        - Patrick Dreker <patrick@dreker.de>

Description
-----------

i2c-nforce2 is a driver for the SMBuses included in the nVidia nForce2 MCP.

If your ``lspci -v`` listing shows something like the following::

  00:01.1 SMBus: nVidia Corporation: Unknown device 0064 (rev a2)
          Subsystem: Asustek Computer, Inc.: Unknown device 0c11
          Flags: 66Mhz, fast devsel, IRQ 5
          I/O ports at c000 [size=32]
          Capabilities: <available only to root>

then this driver should support the SMBuses of your motherboard.


Notes
-----

The SMBus adapter in the nForce2 chipset seems to be very similar to the
SMBus 2.0 adapter in the AMD-8111 south bridge. However, I could only get
the driver to work with direct I/O access, which is different to the EC
interface of the AMD-8111. Tested on Asus A7N8X. The ACPI DSDT table of the
Asus A7N8X lists two SMBuses, both of which are supported by this driver.
