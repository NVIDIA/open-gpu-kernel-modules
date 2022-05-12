Kernel driver twl4030-madc
==========================

Supported chips:

	* Texas Instruments TWL4030

	Prefix: 'twl4030-madc'


Authors:
	J Keerthy <j-keerthy@ti.com>

Description
-----------

The Texas Instruments TWL4030 is a Power Management and Audio Circuit. Among
other things it contains a 10-bit A/D converter MADC. The converter has 16
channels which can be used in different modes.


See this table for the meaning of the different channels

======= ==========================================================
Channel Signal
======= ==========================================================
0	Battery type(BTYPE)
1	BCI: Battery temperature (BTEMP)
2	GP analog input
3	GP analog input
4	GP analog input
5	GP analog input
6	GP analog input
7	GP analog input
8	BCI: VBUS voltage(VBUS)
9	Backup Battery voltage (VBKP)
10	BCI: Battery charger current (ICHG)
11	BCI: Battery charger voltage (VCHG)
12	BCI: Main battery voltage (VBAT)
13	Reserved
14	Reserved
15	VRUSB Supply/Speaker left/Speaker right polarization level
======= ==========================================================


The Sysfs nodes will represent the voltage in the units of mV,
the temperature channel shows the converted temperature in
degree Celsius. The Battery charging current channel represents
battery charging current in mA.
