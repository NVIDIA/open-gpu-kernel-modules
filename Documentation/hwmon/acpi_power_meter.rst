Kernel driver power_meter
=========================

This driver talks to ACPI 4.0 power meters.

Supported systems:

  * Any recent system with ACPI 4.0.

    Prefix: 'power_meter'

    Datasheet: https://uefi.org/specifications, section 10.4.

Author: Darrick J. Wong

Description
-----------

This driver implements sensor reading support for the power meters exposed in
the ACPI 4.0 spec (Chapter 10.4).  These devices have a simple set of
features--a power meter that returns average power use over a configurable
interval, an optional capping mechanism, and a couple of trip points.  The
sysfs interface conforms with the specification outlined in the "Power" section
of Documentation/hwmon/sysfs-interface.rst.

Special Features
----------------

The `power[1-*]_is_battery` knob indicates if the power supply is a battery.
Both `power[1-*]_average_{min,max}` must be set before the trip points will work.
When both of them are set, an ACPI event will be broadcast on the ACPI netlink
socket and a poll notification will be sent to the appropriate
`power[1-*]_average` sysfs file.

The `power[1-*]_{model_number, serial_number, oem_info}` fields display
arbitrary strings that ACPI provides with the meter.  The measures/ directory
contains symlinks to the devices that this meter measures.

Some computers have the ability to enforce a power cap in hardware.  If this is
the case, the `power[1-*]_cap` and related sysfs files will appear.  When the
average power consumption exceeds the cap, an ACPI event will be broadcast on
the netlink event socket and a poll notification will be sent to the
appropriate `power[1-*]_alarm` file to indicate that capping has begun, and the
hardware has taken action to reduce power consumption.  Most likely this will
result in reduced performance.

There are a few other ACPI notifications that can be sent by the firmware.  In
all cases the ACPI event will be broadcast on the ACPI netlink event socket as
well as sent as a poll notification to a sysfs file.  The events are as
follows:

`power[1-*]_cap` will be notified if the firmware changes the power cap.
`power[1-*]_interval` will be notified if the firmware changes the averaging
interval.
