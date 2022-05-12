.. SPDX-License-Identifier: GPL-2.0

===================================================
Firmware support for CPU hotplug under Linux/x86-64
===================================================

Linux/x86-64 supports CPU hotplug now. For various reasons Linux wants to
know in advance of boot time the maximum number of CPUs that could be plugged
into the system. ACPI 3.0 currently has no official way to supply
this information from the firmware to the operating system.

In ACPI each CPU needs an LAPIC object in the MADT table (5.2.11.5 in the
ACPI 3.0 specification).  ACPI already has the concept of disabled LAPIC
objects by setting the Enabled bit in the LAPIC object to zero.

For CPU hotplug Linux/x86-64 expects now that any possible future hotpluggable
CPU is already available in the MADT. If the CPU is not available yet
it should have its LAPIC Enabled bit set to 0. Linux will use the number
of disabled LAPICs to compute the maximum number of future CPUs.

In the worst case the user can overwrite this choice using a command line
option (additional_cpus=...), but it is recommended to supply the correct
number (or a reasonable approximation of it, with erring towards more not less)
in the MADT to avoid manual configuration.
