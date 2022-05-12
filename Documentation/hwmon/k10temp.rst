Kernel driver k10temp
=====================

Supported chips:

* AMD Family 10h processors:

  Socket F: Quad-Core/Six-Core/Embedded Opteron (but see below)

  Socket AM2+: Quad-Core Opteron, Phenom (II) X3/X4, Athlon X2 (but see below)

  Socket AM3: Quad-Core Opteron, Athlon/Phenom II X2/X3/X4, Sempron II

  Socket S1G3: Athlon II, Sempron, Turion II

* AMD Family 11h processors:

  Socket S1G2: Athlon (X2), Sempron (X2), Turion X2 (Ultra)

* AMD Family 12h processors: "Llano" (E2/A4/A6/A8-Series)

* AMD Family 14h processors: "Brazos" (C/E/G/Z-Series)

* AMD Family 15h processors: "Bulldozer" (FX-Series), "Trinity", "Kaveri",
  "Carrizo", "Stoney Ridge", "Bristol Ridge"

* AMD Family 16h processors: "Kabini", "Mullins"

* AMD Family 17h processors: "Zen", "Zen 2"

* AMD Family 18h processors: "Hygon Dhyana"

* AMD Family 19h processors: "Zen 3"

  Prefix: 'k10temp'

  Addresses scanned: PCI space

  Datasheets:

  BIOS and Kernel Developer's Guide (BKDG) For AMD Family 10h Processors:

    http://support.amd.com/us/Processor_TechDocs/31116.pdf

  BIOS and Kernel Developer's Guide (BKDG) for AMD Family 11h Processors:

    http://support.amd.com/us/Processor_TechDocs/41256.pdf

  BIOS and Kernel Developer's Guide (BKDG) for AMD Family 12h Processors:

    http://support.amd.com/us/Processor_TechDocs/41131.pdf

  BIOS and Kernel Developer's Guide (BKDG) for AMD Family 14h Models 00h-0Fh Processors:

    http://support.amd.com/us/Processor_TechDocs/43170.pdf

  Revision Guide for AMD Family 10h Processors:

    http://support.amd.com/us/Processor_TechDocs/41322.pdf

  Revision Guide for AMD Family 11h Processors:

    http://support.amd.com/us/Processor_TechDocs/41788.pdf

  Revision Guide for AMD Family 12h Processors:

    http://support.amd.com/us/Processor_TechDocs/44739.pdf

  Revision Guide for AMD Family 14h Models 00h-0Fh Processors:

    http://support.amd.com/us/Processor_TechDocs/47534.pdf

  AMD Family 11h Processor Power and Thermal Data Sheet for Notebooks:

    http://support.amd.com/us/Processor_TechDocs/43373.pdf

  AMD Family 10h Server and Workstation Processor Power and Thermal Data Sheet:

    http://support.amd.com/us/Processor_TechDocs/43374.pdf

  AMD Family 10h Desktop Processor Power and Thermal Data Sheet:

    http://support.amd.com/us/Processor_TechDocs/43375.pdf

Author: Clemens Ladisch <clemens@ladisch.de>

Description
-----------

This driver permits reading of the internal temperature sensor of AMD
Family 10h/11h/12h/14h/15h/16h processors.

All these processors have a sensor, but on those for Socket F or AM2+,
the sensor may return inconsistent values (erratum 319).  The driver
will refuse to load on these revisions unless you specify the "force=1"
module parameter.

Due to technical reasons, the driver can detect only the mainboard's
socket type, not the processor's actual capabilities.  Therefore, if you
are using an AM3 processor on an AM2+ mainboard, you can safely use the
"force=1" parameter.

For CPUs older than Family 17h, there is one temperature measurement value,
available as temp1_input in sysfs. It is measured in degrees Celsius with a
resolution of 1/8th degree.  Please note that it is defined as a relative
value; to quote the AMD manual::

  Tctl is the processor temperature control value, used by the platform to
  control cooling systems. Tctl is a non-physical temperature on an
  arbitrary scale measured in degrees. It does _not_ represent an actual
  physical temperature like die or case temperature. Instead, it specifies
  the processor temperature relative to the point at which the system must
  supply the maximum cooling for the processor's specified maximum case
  temperature and maximum thermal power dissipation.

The maximum value for Tctl is available in the file temp1_max.

If the BIOS has enabled hardware temperature control, the threshold at
which the processor will throttle itself to avoid damage is available in
temp1_crit and temp1_crit_hyst.

On some AMD CPUs, there is a difference between the die temperature (Tdie) and
the reported temperature (Tctl). Tdie is the real measured temperature, and
Tctl is used for fan control. While Tctl is always available as temp1_input,
the driver exports Tdie temperature as temp2_input for those CPUs which support
it.

Models from 17h family report relative temperature, the driver aims to
compensate and report the real temperature.

On Family 17h and Family 18h CPUs, additional temperature sensors may report
Core Complex Die (CCD) temperatures. Up to 8 such temperatures are reported
as temp{3..10}_input, labeled Tccd{1..8}. Actual support depends on the CPU
variant.

Various Family 17h and 18h CPUs report voltage and current telemetry
information. The following attributes may be reported.

Attribute	Label	Description
===============	=======	================
in0_input	Vcore	Core voltage
in1_input	Vsoc	SoC voltage
curr1_input	Icore	Core current
curr2_input	Isoc	SoC current
===============	=======	================

Current values are raw (unscaled) as reported by the CPU. Core current is
reported as multiples of 1A / LSB. SoC is reported as multiples of 0.25A
/ LSB. The real current is board specific. Reported currents should be seen
as rough guidance, and should be scaled using sensors3.conf as appropriate
for a given board.
