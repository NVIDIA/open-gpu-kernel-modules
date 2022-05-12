============================================================
WDT Watchdog Timer Interfaces For The Linux Operating System
============================================================

Last Reviewed: 10/05/2007

Alan Cox <alan@lxorguk.ukuu.org.uk>

	- ICS	WDT501-P
	- ICS	WDT501-P (no fan tachometer)
	- ICS	WDT500-P

All the interfaces provide /dev/watchdog, which when open must be written
to within a timeout or the machine will reboot. Each write delays the reboot
time another timeout. In the case of the software watchdog the ability to
reboot will depend on the state of the machines and interrupts. The hardware
boards physically pull the machine down off their own onboard timers and
will reboot from almost anything.

A second temperature monitoring interface is available on the WDT501P cards.
This provides /dev/temperature. This is the machine internal temperature in
degrees Fahrenheit. Each read returns a single byte giving the temperature.

The third interface logs kernel messages on additional alert events.

The ICS ISA-bus wdt card cannot be safely probed for. Instead you need to
pass IO address and IRQ boot parameters.  E.g.::

	wdt.io=0x240 wdt.irq=11

Other "wdt" driver parameters are:

	===========	======================================================
	heartbeat	Watchdog heartbeat in seconds (default 60)
	nowayout	Watchdog cannot be stopped once started (kernel
			build parameter)
	tachometer	WDT501-P Fan Tachometer support (0=disable, default=0)
	type		WDT501-P Card type (500 or 501, default=500)
	===========	======================================================

Features
--------

================   =======	   =======
		   WDT501P	   WDT500P
================   =======	   =======
Reboot Timer	   X               X
External Reboot	   X	           X
I/O Port Monitor   o		   o
Temperature	   X		   o
Fan Speed          X		   o
Power Under	   X               o
Power Over         X               o
Overheat           X               o
================   =======	   =======

The external event interfaces on the WDT boards are not currently supported.
Minor numbers are however allocated for it.


Example Watchdog Driver:

	see samples/watchdog/watchdog-simple.c
