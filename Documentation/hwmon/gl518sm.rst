Kernel driver gl518sm
=====================

Supported chips:

  * Genesys Logic GL518SM release 0x00

    Prefix: 'gl518sm'

    Addresses scanned: I2C 0x2c and 0x2d

  * Genesys Logic GL518SM release 0x80

    Prefix: 'gl518sm'

    Addresses scanned: I2C 0x2c and 0x2d

    Datasheet: http://www.genesyslogic.com/

Authors:
       - Frodo Looijaard <frodol@dds.nl>,
       - Kyösti Mälkki <kmalkki@cc.hut.fi>
       - Hong-Gunn Chew <hglinux@gunnet.org>
       - Jean Delvare <jdelvare@suse.de>

Description
-----------

.. important::

   For the revision 0x00 chip, the in0, in1, and in2  values (+5V, +3V,
   and +12V) CANNOT be read. This is a limitation of the chip, not the driver.

This driver supports the Genesys Logic GL518SM chip. There are at least
two revision of this chip, which we call revision 0x00 and 0x80. Revision
0x80 chips support the reading of all voltages and revision 0x00 only
for VIN3.

The GL518SM implements one temperature sensor, two fan rotation speed
sensors, and four voltage sensors. It can report alarms through the
computer speakers.

Temperatures are measured in degrees Celsius. An alarm goes off while the
temperature is above the over temperature limit, and has not yet dropped
below the hysteresis limit. The alarm always reflects the current
situation. Measurements are guaranteed between -10 degrees and +110
degrees, with a accuracy of +/-3 degrees.

Rotation speeds are reported in RPM (rotations per minute). An alarm is
triggered if the rotation speed has dropped below a programmable limit. In
case when you have selected to turn fan1 off, no fan1 alarm is triggered.

Fan readings can be divided by a programmable divider (1, 2, 4 or 8) to
give the readings more range or accuracy.  Not all RPM values can
accurately be represented, so some rounding is done. With a divider
of 2, the lowest representable value is around 1900 RPM.

Voltage sensors (also known as VIN sensors) report their values in volts.
An alarm is triggered if the voltage has crossed a programmable minimum or
maximum limit. Note that minimum in this case always means 'closest to
zero'; this is important for negative voltage measurements. The VDD input
measures voltages between 0.000 and 5.865 volt, with a resolution of 0.023
volt. The other inputs measure voltages between 0.000 and 4.845 volt, with
a resolution of 0.019 volt. Note that revision 0x00 chips do not support
reading the current voltage of any input except for VIN3; limit setting and
alarms work fine, though.

When an alarm is triggered, you can be warned by a beeping signal through your
computer speaker. It is possible to enable all beeping globally, or only the
beeping for some alarms.

If an alarm triggers, it will remain triggered until the hardware register
is read at least once (except for temperature alarms). This means that the
cause for the alarm may already have disappeared! Note that in the current
implementation, all hardware registers are read whenever any data is read
(unless it is less than 1.5 seconds since the last update). This means that
you can easily miss once-only alarms.

The GL518SM only updates its values each 1.5 seconds; reading it more often
will do no harm, but will return 'old' values.
