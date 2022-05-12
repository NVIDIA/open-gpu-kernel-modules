Kernel driver lineage-pem
=========================

Supported devices:

  * Lineage Compact Power Line Power Entry Modules

    Prefix: 'lineage-pem'

    Addresses scanned: -

    Documentation:

	http://www.lineagepower.com/oem/pdf/CPLI2C.pdf

Author: Guenter Roeck <linux@roeck-us.net>


Description
-----------

This driver supports various Lineage Compact Power Line DC/DC and AC/DC
converters such as CP1800, CP2000AC, CP2000DC, CP2100DC, and others.

Lineage CPL power entry modules are nominally PMBus compliant. However, most
standard PMBus commands are not supported. Specifically, all hardware monitoring
and status reporting commands are non-standard. For this reason, a standard
PMBus driver can not be used.


Usage Notes
-----------

This driver does not probe for Lineage CPL devices, since there is no register
which can be safely used to identify the chip. You will have to instantiate
the devices explicitly.

Example: the following will load the driver for a Lineage PEM at address 0x40
on I2C bus #1::

	$ modprobe lineage-pem
	$ echo lineage-pem 0x40 > /sys/bus/i2c/devices/i2c-1/new_device

All Lineage CPL power entry modules have a built-in I2C bus master selector
(PCA9541). To ensure device access, this driver should only be used as client
driver to the pca9541 I2C master selector driver.


Sysfs entries
-------------

All Lineage CPL devices report output voltage and device temperature as well as
alarms for output voltage, temperature, input voltage, input current, input power,
and fan status.

Input voltage, input current, input power, and fan speed measurement is only
supported on newer devices. The driver detects if those attributes are supported,
and only creates respective sysfs entries if they are.

======================= ===============================
in1_input		Output voltage (mV)
in1_min_alarm		Output undervoltage alarm
in1_max_alarm		Output overvoltage alarm
in1_crit		Output voltage critical alarm

in2_input		Input voltage (mV, optional)
in2_alarm		Input voltage alarm

curr1_input		Input current (mA, optional)
curr1_alarm		Input overcurrent alarm

power1_input		Input power (uW, optional)
power1_alarm		Input power alarm

fan1_input		Fan 1 speed (rpm, optional)
fan2_input		Fan 2 speed (rpm, optional)
fan3_input		Fan 3 speed (rpm, optional)

temp1_input
temp1_max
temp1_crit
temp1_alarm
temp1_crit_alarm
temp1_fault
======================= ===============================
