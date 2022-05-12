Userspace tools
===============

Introduction
------------

Most mainboards have sensor chips to monitor system health (like temperatures,
voltages, fans speed). They are often connected through an I2C bus, but some
are also connected directly through the ISA bus.

The kernel drivers make the data from the sensor chips available in the /sys
virtual filesystem. Userspace tools are then used to display the measured
values or configure the chips in a more friendly manner.

Lm-sensors
----------

Core set of utilities that will allow you to obtain health information,
setup monitoring limits etc. You can get them on their homepage
http://www.lm-sensors.org/ or as a package from your Linux distribution.

If from website:
Get lm-sensors from project web site. Please note, you need only userspace
part, so compile with "make user" and install with "make user_install".

General hints to get things working:

0) get lm-sensors userspace utils
1) compile all drivers in I2C and Hardware Monitoring sections as modules
   in your kernel
2) run sensors-detect script, it will tell you what modules you need to load.
3) load them and run "sensors" command, you should see some results.
4) fix sensors.conf, labels, limits, fan divisors
5) if any more problems consult FAQ, or documentation

Other utilities
---------------

If you want some graphical indicators of system health look for applications
like: gkrellm, ksensors, xsensors, wmtemp, wmsensors, wmgtemp, ksysguardd,
hardware-monitor

If you are server administrator you can try snmpd or mrtgutils.
