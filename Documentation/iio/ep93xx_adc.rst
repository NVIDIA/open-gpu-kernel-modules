==============================
Cirrus Logic EP93xx ADC driver
==============================

1. Overview
===========

The driver is intended to work on both low-end (EP9301, EP9302) devices with
5-channel ADC and high-end (EP9307, EP9312, EP9315) devices with 10-channel
touchscreen/ADC module.

2. Channel numbering
====================

Numbering scheme for channels 0..4 is defined in EP9301 and EP9302 datasheets.
EP9307, EP9312 and EP9315 have 3 channels more (total 8), but the numbering is
not defined. So the last three are numbered randomly, let's say.

Assuming ep93xx_adc is IIO device0, you'd find the following entries under
/sys/bus/iio/devices/iio:device0/:

  +-----------------+---------------+
  | sysfs entry     | ball/pin name |
  +=================+===============+
  | in_voltage0_raw | YM            |
  +-----------------+---------------+
  | in_voltage1_raw | SXP           |
  +-----------------+---------------+
  | in_voltage2_raw | SXM           |
  +-----------------+---------------+
  | in_voltage3_raw | SYP           |
  +-----------------+---------------+
  | in_voltage4_raw | SYM           |
  +-----------------+---------------+
  | in_voltage5_raw | XP            |
  +-----------------+---------------+
  | in_voltage6_raw | XM            |
  +-----------------+---------------+
  | in_voltage7_raw | YP            |
  +-----------------+---------------+
