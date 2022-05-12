Kernel Driver IBMPOWERNV
========================

Supported systems:

  * Any recent IBM P servers based on POWERNV platform

Author: Neelesh Gupta

Description
-----------

This driver implements reading the platform sensors data like temperature/fan/
voltage/power for 'POWERNV' platform.

The driver uses the platform device infrastructure. It probes the device tree
for sensor devices during the __init phase and registers them with the 'hwmon'.
'hwmon' populates the 'sysfs' tree having attribute files, each for a given
sensor type and its attribute data.

All the nodes in the DT appear under "/ibm,opal/sensors" and each valid node in
the DT maps to an attribute file in 'sysfs'. The node exports unique 'sensor-id'
which the driver uses to make an OPAL call to the firmware.

Usage notes
-----------
The driver is built statically with the kernel by enabling the config
CONFIG_SENSORS_IBMPOWERNV. It can also be built as module 'ibmpowernv'.

Sysfs attributes
----------------

======================= =======================================================
fanX_input		Measured RPM value.
fanX_min		Threshold RPM for alert generation.
fanX_fault		- 0: No fail condition
			- 1: Failing fan

tempX_input		Measured ambient temperature.
tempX_max		Threshold ambient temperature for alert generation.
tempX_highest		Historical maximum temperature
tempX_lowest		Historical minimum temperature
tempX_enable		Enable/disable all temperature sensors belonging to the
			sub-group. In POWER9, this attribute corresponds to
			each OCC. Using this attribute each OCC can be asked to
			disable/enable all of its temperature sensors.

			- 1: Enable
			- 0: Disable

inX_input		Measured power supply voltage (millivolt)
inX_fault		- 0: No fail condition.
			- 1: Failing power supply.
inX_highest		Historical maximum voltage
inX_lowest		Historical minimum voltage
inX_enable		Enable/disable all voltage sensors belonging to the
			sub-group. In POWER9, this attribute corresponds to
			each OCC. Using this attribute each OCC can be asked to
			disable/enable all of its voltage sensors.

			- 1: Enable
			- 0: Disable

powerX_input		Power consumption (microWatt)
powerX_input_highest	Historical maximum power
powerX_input_lowest	Historical minimum power
powerX_enable		Enable/disable all power sensors belonging to the
			sub-group. In POWER9, this attribute corresponds to
			each OCC. Using this attribute each OCC can be asked to
			disable/enable all of its power sensors.

			- 1: Enable
			- 0: Disable

currX_input		Measured current (milliampere)
currX_highest		Historical maximum current
currX_lowest		Historical minimum current
currX_enable		Enable/disable all current sensors belonging to the
			sub-group. In POWER9, this attribute corresponds to
			each OCC. Using this attribute each OCC can be asked to
			disable/enable all of its current sensors.

			- 1: Enable
			- 0: Disable

energyX_input		Cumulative energy (microJoule)
======================= =======================================================
