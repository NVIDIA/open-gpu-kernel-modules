Kernel driver smsc47m1
======================

Supported chips:

  * SMSC LPC47B27x, LPC47M112, LPC47M10x, LPC47M13x, LPC47M14x,

    LPC47M15x and LPC47M192

    Addresses scanned: none, address read from Super I/O config space

    Prefix: 'smsc47m1'

    Datasheets:

	http://www.smsc.com/media/Downloads_Public/Data_Sheets/47b272.pdf

	http://www.smsc.com/media/Downloads_Public/Data_Sheets/47m10x.pdf

	http://www.smsc.com/media/Downloads_Public/Data_Sheets/47m112.pdf

	http://www.smsc.com/

  * SMSC LPC47M292

    Addresses scanned: none, address read from Super I/O config space

    Prefix: 'smsc47m2'

    Datasheet: Not public

  * SMSC LPC47M997

    Addresses scanned: none, address read from Super I/O config space

    Prefix: 'smsc47m1'

    Datasheet: none



Authors:

     - Mark D. Studebaker <mdsxyz123@yahoo.com>,
     - With assistance from Bruce Allen <ballen@uwm.edu>, and his
       fan.c program:

       - http://www.lsc-group.phys.uwm.edu/%7Eballen/driver/

     - Gabriele Gorla <gorlik@yahoo.com>,
     - Jean Delvare <jdelvare@suse.de>

Description
-----------

The Standard Microsystems Corporation (SMSC) 47M1xx Super I/O chips
contain monitoring and PWM control circuitry for two fans.

The LPC47M15x, LPC47M192 and LPC47M292 chips contain a full 'hardware
monitoring block' in addition to the fan monitoring and control. The
hardware monitoring block is not supported by this driver, use the
smsc47m192 driver for that.

No documentation is available for the 47M997, but it has the same device
ID as the 47M15x and 47M192 chips and seems to be compatible.

Fan rotation speeds are reported in RPM (rotations per minute). An alarm is
triggered if the rotation speed has dropped below a programmable limit. Fan
readings can be divided by a programmable divider (1, 2, 4 or 8) to give
the readings more range or accuracy. Not all RPM values can accurately be
represented, so some rounding is done. With a divider of 2, the lowest
representable value is around 2600 RPM.

PWM values are from 0 to 255.

If an alarm triggers, it will remain triggered until the hardware register
is read at least once. This means that the cause for the alarm may
already have disappeared! Note that in the current implementation, all
hardware registers are read whenever any data is read (unless it is less
than 1.5 seconds since the last update). This means that you can easily
miss once-only alarms.

------------------------------------------------------------------

The lm_sensors project gratefully acknowledges the support of
Intel in the development of this driver.
