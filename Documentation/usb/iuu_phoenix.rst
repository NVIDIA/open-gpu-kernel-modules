=============================
Infinity Usb Unlimited Readme
=============================

Hi all,


This module provide a serial interface to use your
IUU unit in phoenix mode. Loading this module will
bring a ttyUSB[0-x] interface. This driver must be
used by your favorite application to pilot the IUU

This driver is still in beta stage, so bugs can
occur and your system may freeze. As far I now,
I never had any problem with it, but I'm not a real
guru, so don't blame me if your system is unstable

You can plug more than one IUU. Every unit will
have his own device file(/dev/ttyUSB0,/dev/ttyUSB1,...)



How to tune the reader speed?
=============================

 A few parameters can be used at load time
 To use parameters, just unload the module if it is
 already loaded and use modprobe iuu_phoenix param=value.
 In case of prebuilt module, use the command
 insmod iuu_phoenix param=value.

 Example::

	modprobe iuu_phoenix clockmode=3

 The parameters are:

clockmode:
	1=3Mhz579,2=3Mhz680,3=6Mhz (int)
boost:
	overclock boost percent 100 to 500 (int)
cdmode:
	Card detect mode
	0=none, 1=CD, 2=!CD, 3=DSR, 4=!DSR, 5=CTS, 6=!CTS, 7=RING, 8=!RING (int)
xmas:
	xmas color enabled or not (bool)
debug:
	Debug enabled or not (bool)

-  clockmode will provide 3 different base settings commonly adopted by
   different software:

	1. 3Mhz579
	2. 3Mhz680
	3. 6Mhz

-  boost provide a way to overclock the reader ( my favorite :-)  )
   For example to have best performance than a simple clockmode=3, try this::

      modprobe boost=195

   This will put the reader in a base of 3Mhz579 but boosted a 195 % !
   the real clock will be now : 6979050 Hz ( 6Mhz979 ) and will increase
   the speed to a score 10 to 20% better than the simple clockmode=3 !!!


-  cdmode permit to setup the signal used to inform the userland ( ioctl answer )
   if the card is present or not. Eight signals are possible.

-  xmas is completely useless except for your eyes. This is one of my friend who was
   so sad to have a nice device like the iuu without seeing all color range available.
   So I have added this option to permit him to see a lot of color ( each activity change the color
   and the frequency randomly )

-  debug will produce a lot of debugging messages...


Last notes
==========

 Don't worry about the serial settings, the serial emulation
 is an abstraction, so use any speed or parity setting will
 work. ( This will not change anything ).Later I will perhaps
 use this settings to deduce de boost but is that feature
 really necessary ?
 The autodetect feature used is the serial CD. If that doesn't
 work for your software, disable detection mechanism in it.


 Have fun !

 Alain Degreffe

 eczema(at)ecze.com
