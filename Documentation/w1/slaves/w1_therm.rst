======================
Kernel driver w1_therm
======================

Supported chips:

  * Maxim ds18*20 based temperature sensors.
  * Maxim ds1825 based temperature sensors.
  * GXCAS GC20MH01 temperature sensor.

Author: Evgeniy Polyakov <johnpol@2ka.mipt.ru>


Description
-----------

w1_therm provides basic temperature conversion for ds18*20, ds28ea00, GX20MH01
devices.

Supported family codes:

====================	====
W1_THERM_DS18S20	0x10
W1_THERM_DS1822		0x22
W1_THERM_DS18B20	0x28
W1_THERM_DS1825		0x3B
W1_THERM_DS28EA00	0x42
====================	====

Support is provided through the sysfs entry ``w1_slave``. Each open and
read sequence will initiate a temperature conversion, then provide two
lines of ASCII output. The first line contains the nine hex bytes
read along with a calculated crc value and YES or NO if it matched.
If the crc matched the returned values are retained. The second line
displays the retained values along with a temperature in millidegrees
Centigrade after t=.

Alternatively, temperature can be read using ``temperature`` sysfs, it
returns only the temperature in millidegrees Centigrade.

A bulk read of all devices on the bus could be done writing ``trigger``
to ``therm_bulk_read`` entry at w1_bus_master level. This will
send the convert command to all devices on the bus, and if parasite
powered devices are detected on the bus (and strong pullup is enabled
in the module), it will drive the line high during the longer conversion
time required by parasited powered device on the line. Reading
``therm_bulk_read`` will return 0 if no bulk conversion pending,
-1 if at least one sensor still in conversion, 1 if conversion is complete
but at least one sensor value has not been read yet. Result temperature is
then accessed by reading the ``temperature`` entry of each device, which
may return empty if conversion is still in progress. Note that if a bulk
read is sent but one sensor is not read immediately, the next access to
``temperature`` on this device will return the temperature measured at the
time of issue of the bulk read command (not the current temperature).

A strong pullup will be applied during the conversion if required.

``conv_time`` is used to get current conversion time (read), and
adjust it (write). A temperature conversion time depends on the device type and
it's current resolution. Default conversion time is set by the driver according
to the device datasheet. A conversion time for many original device clones
deviate from datasheet specs. There are three options: 1) manually set the
correct conversion time by writing a value in milliseconds to ``conv_time``; 2)
auto measure and set a conversion time by writing ``1`` to
``conv_time``; 3) use ``features`` to enable poll for conversion
completion. Options 2, 3 can't be used in parasite power mode. To get back to
the default conversion time write ``0`` to ``conv_time``.

Writing a resolution value (in bits) to ``w1_slave`` will change the
precision of the sensor for the next readings. Allowed resolutions are defined by
the sensor. Resolution is reset when the sensor gets power-cycled.

To store the current resolution in EEPROM, write ``0`` to ``w1_slave``.
Since the EEPROM has a limited amount of writes (>50k), this command should be
used wisely.

Alternatively, resolution can be read or written using the dedicated
``resolution`` entry on each device, if supported by the sensor.

Some non-genuine DS18B20 chips are fixed in 12-bit mode only, so the actual
resolution is read back from the chip and verified.

Note: Changing the resolution reverts the conversion time to default.

The write-only sysfs entry ``eeprom_cmd`` is an alternative for EEPROM operations.
Write ``save`` to save device RAM to EEPROM. Write ``restore`` to restore EEPROM
data in device RAM.

``ext_power`` entry allows checking the power state of each device. Reads
``0`` if the device is parasite powered, ``1`` if the device is externally powered.

Sysfs ``alarms`` allow read or write TH and TL (Temperature High an Low) alarms.
Values shall be space separated and in the device range (typical -55 degC
to 125 degC). Values are integer as they are store in a 8bit register in
the device. Lowest value is automatically put to TL. Once set, alarms could
be search at master level.

The module parameter strong_pullup can be set to 0 to disable the
strong pullup, 1 to enable autodetection or 2 to force strong pullup.
In case of autodetection, the driver will use the "READ POWER SUPPLY"
command to check if there are pariste powered devices on the bus.
If so, it will activate the master's strong pullup.
In case the detection of parasite devices using this command fails
(seems to be the case with some DS18S20) the strong pullup can
be force-enabled.

If the strong pullup is enabled, the master's strong pullup will be
driven when the conversion is taking place, provided the master driver
does support the strong pullup (or it falls back to a pullup
resistor).  The DS18b20 temperature sensor specification lists a
maximum current draw of 1.5mA and that a 5k pullup resistor is not
sufficient.  The strong pullup is designed to provide the additional
current required.

The DS28EA00 provides an additional two pins for implementing a sequence
detection algorithm.  This feature allows you to determine the physical
location of the chip in the 1-wire bus without needing pre-existing
knowledge of the bus ordering.  Support is provided through the sysfs
``w1_seq``. The file will contain a single line with an integer value
representing the device index in the bus starting at 0.

``features`` sysfs entry controls optional driver settings per device.
Insufficient power in parasite mode, line noise and insufficient conversion
time may lead to conversion failure. Original DS18B20 and some clones allow for
detection of invalid conversion. Write bit mask ``1`` to ``features`` to enable
checking the conversion success. If byte 6 of scratchpad memory is 0xC after
conversion and temperature reads 85.00 (powerup value) or 127.94 (insufficient
power), the driver returns a conversion error. Bit mask ``2`` enables poll for
conversion completion (normal power only) by generating read cycles on the bus
after conversion starts. In parasite power mode this feature is not available.
Feature bit masks may be combined (OR). More details in
Documentation/ABI/testing/sysfs-driver-w1_therm

GX20MH01 device shares family number 0x28 with DS18*20. The device is generally
compatible with DS18B20. Added are lowest 2\ :sup:`-5`, 2\ :sup:`-6` temperature
bits in Config register; R2 bit in Config register enabling 13 and 14 bit
resolutions. The device is powered up in 14-bit resolution mode. The conversion
times specified in the datasheet are too low and have to be increased. The
device supports driver features ``1`` and ``2``.
