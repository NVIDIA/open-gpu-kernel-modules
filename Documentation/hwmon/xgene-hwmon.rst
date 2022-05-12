Kernel driver xgene-hwmon
=========================

Supported chips:

 * APM X-Gene SoC

Description
-----------

This driver adds hardware temperature and power reading support for
APM X-Gene SoC using the mailbox communication interface.
For device tree, it is the standard DT mailbox.
For ACPI, it is the PCC mailbox.

The following sensors are supported

  * Temperature
      - SoC on-die temperature in milli-degree C
      - Alarm when high/over temperature occurs

  * Power
      - CPU power in uW
      - IO power in uW

sysfs-Interface
---------------

temp0_input
	- SoC on-die temperature (milli-degree C)
temp0_critical_alarm
	- An 1 would indicates on-die temperature exceeded threshold
power0_input
	- CPU power in (uW)
power1_input
	- IO power in (uW)
