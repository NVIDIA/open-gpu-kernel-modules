Kernel driver k8temp
====================

Supported chips:

  * AMD Athlon64/FX or Opteron CPUs

    Prefix: 'k8temp'

    Addresses scanned: PCI space

    Datasheet: https://www.amd.com/system/files/TechDocs/32559.pdf

Author: Rudolf Marek

Contact: Rudolf Marek <r.marek@assembler.cz>

Description
-----------

This driver permits reading temperature sensor(s) embedded inside AMD K8
family CPUs (Athlon64/FX, Opteron). Official documentation says that it works
from revision F of K8 core, but in fact it seems to be implemented for all
revisions of K8 except the first two revisions (SH-B0 and SH-B3).

Please note that you will need at least lm-sensors 2.10.1 for proper userspace
support.

There can be up to four temperature sensors inside single CPU. The driver
will auto-detect the sensors and will display only temperatures from
implemented sensors.

Mapping of /sys files is as follows:

============= ===================================
temp1_input   temperature of Core 0 and "place" 0
temp2_input   temperature of Core 0 and "place" 1
temp3_input   temperature of Core 1 and "place" 0
temp4_input   temperature of Core 1 and "place" 1
============= ===================================

Temperatures are measured in degrees Celsius and measurement resolution is
1 degree C. It is expected that future CPU will have better resolution. The
temperature is updated once a second. Valid temperatures are from -49 to
206 degrees C.

Temperature known as TCaseMax was specified for processors up to revision E.
This temperature is defined as temperature between heat-spreader and CPU
case, so the internal CPU temperature supplied by this driver can be higher.
There is no easy way how to measure the temperature which will correlate
with TCaseMax temperature.

For newer revisions of CPU (rev F, socket AM2) there is a mathematically
computed temperature called TControl, which must be lower than TControlMax.

The relationship is following:

	temp1_input - TjOffset*2 < TControlMax,

TjOffset is not yet exported by the driver, TControlMax is usually
70 degrees C. The rule of the thumb -> CPU temperature should not cross
60 degrees C too much.
