====================
Kernel driver eeprom
====================

Supported chips:

  * Any EEPROM chip in the designated address range

    Prefix: 'eeprom'

    Addresses scanned: I2C 0x50 - 0x57

    Datasheets: Publicly available from:

                Atmel (www.atmel.com),
                Catalyst (www.catsemi.com),
                Fairchild (www.fairchildsemi.com),
                Microchip (www.microchip.com),
                Philips (www.semiconductor.philips.com),
                Rohm (www.rohm.com),
                ST (www.st.com),
                Xicor (www.xicor.com),
                and others.

        ========= ============= ============================================
        Chip      Size (bits)   Address
        ========= ============= ============================================
        24C01     1K            0x50 (shadows at 0x51 - 0x57)
        24C01A    1K            0x50 - 0x57 (Typical device on DIMMs)
        24C02     2K            0x50 - 0x57
        24C04     4K            0x50, 0x52, 0x54, 0x56
                                (additional data at 0x51, 0x53, 0x55, 0x57)
        24C08     8K            0x50, 0x54 (additional data at 0x51, 0x52,
                                0x53, 0x55, 0x56, 0x57)
        24C16     16K           0x50 (additional data at 0x51 - 0x57)
        Sony      2K            0x57

        Atmel     34C02B  2K    0x50 - 0x57, SW write protect at 0x30-37
        Catalyst  34FC02  2K    0x50 - 0x57, SW write protect at 0x30-37
        Catalyst  34RC02  2K    0x50 - 0x57, SW write protect at 0x30-37
        Fairchild 34W02   2K    0x50 - 0x57, SW write protect at 0x30-37
        Microchip 24AA52  2K    0x50 - 0x57, SW write protect at 0x30-37
        ST        M34C02  2K    0x50 - 0x57, SW write protect at 0x30-37
        ========= ============= ============================================


Authors:
        - Frodo Looijaard <frodol@dds.nl>,
        - Philip Edelbrock <phil@netroedge.com>,
        - Jean Delvare <jdelvare@suse.de>,
        - Greg Kroah-Hartman <greg@kroah.com>,
        - IBM Corp.

Description
-----------

This is a simple EEPROM module meant to enable reading the first 256 bytes
of an EEPROM (on a SDRAM DIMM for example). However, it will access serial
EEPROMs on any I2C adapter. The supported devices are generically called
24Cxx, and are listed above; however the numbering for these
industry-standard devices may vary by manufacturer.

This module was a programming exercise to get used to the new project
organization laid out by Frodo, but it should be at least completely
effective for decoding the contents of EEPROMs on DIMMs.

DIMMS will typically contain a 24C01A or 24C02, or the 34C02 variants.
The other devices will not be found on a DIMM because they respond to more
than one address.

DDC Monitors may contain any device. Often a 24C01, which responds to all 8
addresses, is found.

Recent Sony Vaio laptops have an EEPROM at 0x57. We couldn't get the
specification, so it is guess work and far from being complete.

The Microchip 24AA52/24LCS52, ST M34C02, and others support an additional
software write protect register at 0x30 - 0x37 (0x20 less than the memory
location). The chip responds to "write quick" detection at this address but
does not respond to byte reads. If this register is present, the lower 128
bytes of the memory array are not write protected. Any byte data write to
this address will write protect the memory array permanently, and the
device will no longer respond at the 0x30-37 address. The eeprom driver
does not support this register.

Lacking functionality
---------------------

* Full support for larger devices (24C04, 24C08, 24C16). These are not
  typically found on a PC. These devices will appear as separate devices at
  multiple addresses.

* Support for really large devices (24C32, 24C64, 24C128, 24C256, 24C512).
  These devices require two-byte address fields and are not supported.

* Enable Writing. Again, no technical reason why not, but making it easy
  to change the contents of the EEPROMs (on DIMMs anyway) also makes it easy
  to disable the DIMMs (potentially preventing the computer from booting)
  until the values are restored somehow.

Use
---

After inserting the module (and any other required SMBus/i2c modules), you
should have some EEPROM directories in ``/sys/bus/i2c/devices/*`` of names such
as "0-0050". Inside each of these is a series of files, the eeprom file
contains the binary data from EEPROM.
