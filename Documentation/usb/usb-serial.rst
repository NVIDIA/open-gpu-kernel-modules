==========
USB serial
==========

Introduction
============

  The USB serial driver currently supports a number of different USB to
  serial converter products, as well as some devices that use a serial
  interface from userspace to talk to the device.

  See the individual product section below for specific information about
  the different devices.


Configuration
=============

  Currently the driver can handle up to 256 different serial interfaces at
  one time.

    The major number that the driver uses is 188 so to use the driver,
    create the following nodes::

	mknod /dev/ttyUSB0 c 188 0
	mknod /dev/ttyUSB1 c 188 1
	mknod /dev/ttyUSB2 c 188 2
	mknod /dev/ttyUSB3 c 188 3
		.
		.
		.
	mknod /dev/ttyUSB254 c 188 254
	mknod /dev/ttyUSB255 c 188 255

  When the device is connected and recognized by the driver, the driver
  will print to the system log, which node(s) the device has been bound
  to.


Specific Devices Supported
==========================


ConnectTech WhiteHEAT 4 port converter
--------------------------------------

  ConnectTech has been very forthcoming with information about their
  device, including providing a unit to test with.

  The driver is officially supported by Connect Tech Inc.
  http://www.connecttech.com

  For any questions or problems with this driver, please contact
  Connect Tech's Support Department at support@connecttech.com


HandSpring Visor, Palm USB, and Clié USB driver
-----------------------------------------------

  This driver works with all HandSpring USB, Palm USB, and Sony Clié USB
  devices.

  Only when the device tries to connect to the host, will the device show
  up to the host as a valid USB device. When this happens, the device is
  properly enumerated, assigned a port, and then communication _should_ be
  possible. The driver cleans up properly when the device is removed, or
  the connection is canceled on the device.

  NOTE:
    This means that in order to talk to the device, the sync button must be
    pressed BEFORE trying to get any program to communicate to the device.
    This goes against the current documentation for pilot-xfer and other
    packages, but is the only way that it will work due to the hardware
    in the device.

  When the device is connected, try talking to it on the second port
  (this is usually /dev/ttyUSB1 if you do not have any other usb-serial
  devices in the system.) The system log should tell you which port is
  the port to use for the HotSync transfer. The "Generic" port can be used
  for other device communication, such as a PPP link.

  For some Sony Clié devices, /dev/ttyUSB0 must be used to talk to the
  device.  This is true for all OS version 3.5 devices, and most devices
  that have had a flash upgrade to a newer version of the OS.  See the
  kernel system log for information on which is the correct port to use.

  If after pressing the sync button, nothing shows up in the system log,
  try resetting the device, first a hot reset, and then a cold reset if
  necessary.  Some devices need this before they can talk to the USB port
  properly.

  Devices that are not compiled into the kernel can be specified with module
  parameters.  e.g. modprobe visor vendor=0x54c product=0x66

  There is a webpage and mailing lists for this portion of the driver at:
  http://sourceforge.net/projects/usbvisor/

  For any questions or problems with this driver, please contact Greg
  Kroah-Hartman at greg@kroah.com


PocketPC PDA Driver
-------------------

  This driver can be used to connect to Compaq iPAQ, HP Jornada, Casio EM500
  and other PDAs running Windows CE 3.0 or PocketPC 2002 using a USB
  cable/cradle.
  Most devices supported by ActiveSync are supported out of the box.
  For others, please use module parameters to specify the product and vendor
  id. e.g. modprobe ipaq vendor=0x3f0 product=0x1125

  The driver presents a serial interface (usually on /dev/ttyUSB0) over
  which one may run ppp and establish a TCP/IP link to the PDA. Once this
  is done, you can transfer files, backup, download email etc. The most
  significant advantage of using USB is speed - I can get 73 to 113
  kbytes/sec for download/upload to my iPAQ.

  This driver is only one of a set of components required to utilize
  the USB connection. Please visit http://synce.sourceforge.net which
  contains the necessary packages and a simple step-by-step howto.

  Once connected, you can use Win CE programs like ftpView, Pocket Outlook
  from the PDA and xcerdisp, synce utilities from the Linux side.

  To use Pocket IE, follow the instructions given at
  http://www.tekguru.co.uk/EM500/usbtonet.htm to achieve the same thing
  on Win98. Omit the proxy server part; Linux is quite capable of forwarding
  packets unlike Win98. Another modification is required at least for the
  iPAQ - disable autosync by going to the Start/Settings/Connections menu
  and unchecking the "Automatically synchronize ..." box. Go to
  Start/Programs/Connections, connect the cable and select "usbdial" (or
  whatever you named your new USB connection). You should finally wind
  up with a "Connected to usbdial" window with status shown as connected.
  Now start up PIE and browse away.

  If it doesn't work for some reason, load both the usbserial and ipaq module
  with the module parameter "debug" set to 1 and examine the system log.
  You can also try soft-resetting your PDA before attempting a connection.

  Other functionality may be possible depending on your PDA. According to
  Wes Cilldhaire <billybobjoehenrybob@hotmail.com>, with the Toshiba E570,
  ...if you boot into the bootloader (hold down the power when hitting the
  reset button, continuing to hold onto the power until the bootloader screen
  is displayed), then put it in the cradle with the ipaq driver loaded, open
  a terminal on /dev/ttyUSB0, it gives you a "USB Reflash" terminal, which can
  be used to flash the ROM, as well as the microP code..  so much for needing
  Toshiba's $350 serial cable for flashing!! :D
  NOTE: This has NOT been tested. Use at your own risk.

  For any questions or problems with the driver, please contact Ganesh
  Varadarajan <ganesh@veritas.com>


Keyspan PDA Serial Adapter
--------------------------

  Single port DB-9 serial adapter, pushed as a PDA adapter for iMacs (mostly
  sold in Macintosh catalogs, comes in a translucent white/green dongle).
  Fairly simple device. Firmware is homebrew.
  This driver also works for the Xircom/Entrega single port serial adapter.

  Current status:

   Things that work:
     - basic input/output (tested with 'cu')
     - blocking write when serial line can't keep up
     - changing baud rates (up to 115200)
     - getting/setting modem control pins (TIOCM{GET,SET,BIS,BIC})
     - sending break (although duration looks suspect)

   Things that don't:
     - device strings (as logged by kernel) have trailing binary garbage
     - device ID isn't right, might collide with other Keyspan products
     - changing baud rates ought to flush tx/rx to avoid mangled half characters

   Big Things on the todo list:
     - parity, 7 vs 8 bits per char, 1 or 2 stop bits
     - HW flow control
     - not all of the standard USB descriptors are handled:
       Get_Status, Set_Feature, O_NONBLOCK, select()

  For any questions or problems with this driver, please contact Brian
  Warner at warner@lothar.com


Keyspan USA-series Serial Adapters
----------------------------------

  Single, Dual and Quad port adapters - driver uses Keyspan supplied
  firmware and is being developed with their support.

  Current status:

    The USA-18X, USA-28X, USA-19, USA-19W and USA-49W are supported and
    have been pretty thoroughly tested at various baud rates with 8-N-1
    character settings.  Other character lengths and parity setups are
    presently untested.

    The USA-28 isn't yet supported though doing so should be pretty
    straightforward.  Contact the maintainer if you require this
    functionality.

  More information is available at:

        http://www.carnationsoftware.com/carnation/Keyspan.html

  For any questions or problems with this driver, please contact Hugh
  Blemings at hugh@misc.nu


FTDI Single Port Serial Driver
------------------------------

  This is a single port DB-25 serial adapter.

  Devices supported include:

                - TripNav TN-200 USB GPS
                - Navis Engineering Bureau CH-4711 USB GPS

  For any questions or problems with this driver, please contact Bill Ryder.


ZyXEL omni.net lcd plus ISDN TA
-------------------------------

  This is an ISDN TA. Please report both successes and troubles to
  azummo@towertech.it


Cypress M8 CY4601 Family Serial Driver
--------------------------------------

  This driver was in most part developed by Neil "koyama" Whelchel.  It
  has been improved since that previous form to support dynamic serial
  line settings and improved line handling.  The driver is for the most
  part stable and has been tested on an smp machine. (dual p2)

    Chipsets supported under CY4601 family:

		CY7C63723, CY7C63742, CY7C63743, CY7C64013

    Devices supported:

		- DeLorme's USB Earthmate GPS (SiRF Star II lp arch)
		- Cypress HID->COM RS232 adapter

		Note:
			Cypress Semiconductor claims no affiliation with the
			hid->com device.

     Most devices using chipsets under the CY4601 family should
     work with the driver.  As long as they stay true to the CY4601
     usbserial specification.

    Technical notes:

        The Earthmate starts out at 4800 8N1 by default... the driver will
	upon start init to this setting.  usbserial core provides the rest
	of the termios settings, along with some custom termios so that the
	output is in proper format and parsable.

	The device can be put into sirf mode by issuing NMEA command::

		$PSRF100,<protocol>,<baud>,<databits>,<stopbits>,<parity>*CHECKSUM
		$PSRF100,0,9600,8,1,0*0C

		It should then be sufficient to change the port termios to match this
		to begin communicating.

	As far as I can tell it supports pretty much every sirf command as
	documented online available with firmware 2.31, with some unknown
	message ids.

	The hid->com adapter can run at a maximum baud of 115200bps.  Please note
	that the device has trouble or is incapable of raising line voltage properly.
	It will be fine with null modem links, as long as you do not try to link two
	together without hacking the adapter to set the line high.

	The driver is smp safe.  Performance with the driver is rather low when using
	it for transferring files.  This is being worked on, but I would be willing to
	accept patches.  An urb queue or packet buffer would likely fit the bill here.

	If you have any questions, problems, patches, feature requests, etc. you can
	contact me here via email:

					dignome@gmail.com

		(your problems/patches can alternately be submitted to usb-devel)


Digi AccelePort Driver
----------------------

  This driver supports the Digi AccelePort USB 2 and 4 devices, 2 port
  (plus a parallel port) and 4 port USB serial converters.  The driver
  does NOT yet support the Digi AccelePort USB 8.

  This driver works under SMP with the usb-uhci driver.  It does not
  work under SMP with the uhci driver.

  The driver is generally working, though we still have a few more ioctls
  to implement and final testing and debugging to do.  The parallel port
  on the USB 2 is supported as a serial to parallel converter; in other
  words, it appears as another USB serial port on Linux, even though
  physically it is really a parallel port.  The Digi Acceleport USB 8
  is not yet supported.

  Please contact Peter Berger (pberger@brimson.com) or Al Borchers
  (alborchers@steinerpoint.com) for questions or problems with this
  driver.


Belkin USB Serial Adapter F5U103
--------------------------------

  Single port DB-9/PS-2 serial adapter from Belkin with firmware by eTEK Labs.
  The Peracom single port serial adapter also works with this driver, as
  well as the GoHubs adapter.

  Current status:

    The following have been tested and work:

      - Baud rate    300-230400
      - Data bits    5-8
      - Stop bits    1-2
      - Parity       N,E,O,M,S
      - Handshake    None, Software (XON/XOFF), Hardware (CTSRTS,CTSDTR) [1]_
      - Break        Set and clear
      - Line control Input/Output query and control [2]_

  .. [1]
         Hardware input flow control is only enabled for firmware
         levels above 2.06.  Read source code comments describing Belkin
         firmware errata.  Hardware output flow control is working for all
         firmware versions.

  .. [2]
         Queries of inputs (CTS,DSR,CD,RI) show the last
         reported state.  Queries of outputs (DTR,RTS) show the last
         requested state and may not reflect current state as set by
         automatic hardware flow control.

  TO DO List:
    - Add true modem control line query capability.  Currently tracks the
      states reported by the interrupt and the states requested.
    - Add error reporting back to application for UART error conditions.
    - Add support for flush ioctls.
    - Add everything else that is missing :)

  For any questions or problems with this driver, please contact William
  Greathouse at wgreathouse@smva.com


Empeg empeg-car Mark I/II Driver
--------------------------------

  This is an experimental driver to provide connectivity support for the
  client synchronization tools for an Empeg empeg-car mp3 player.

  Tips:
    * Don't forget to create the device nodes for ttyUSB{0,1,2,...}
    * modprobe empeg (modprobe is your friend)
    * emptool --usb /dev/ttyUSB0 (or whatever you named your device node)

  For any questions or problems with this driver, please contact Gary
  Brubaker at xavyer@ix.netcom.com


MCT USB Single Port Serial Adapter U232
---------------------------------------

  This driver is for the MCT USB-RS232 Converter (25 pin, Model No.
  U232-P25) from Magic Control Technology Corp. (there is also a 9 pin
  Model No. U232-P9). More information about this device can be found at
  the manufacturer's web-site: http://www.mct.com.tw.

  The driver is generally working, though it still needs some more testing.
  It is derived from the Belkin USB Serial Adapter F5U103 driver and its
  TODO list is valid for this driver as well.

  This driver has also been found to work for other products, which have
  the same Vendor ID but different Product IDs. Sitecom's U232-P25 serial
  converter uses Product ID 0x230 and Vendor ID 0x711 and works with this
  driver. Also, D-Link's DU-H3SP USB BAY also works with this driver.

  For any questions or problems with this driver, please contact Wolfgang
  Grandegger at wolfgang@ces.ch


Inside Out Networks Edgeport Driver
-----------------------------------

  This driver supports all devices made by Inside Out Networks, specifically
  the following models:

       - Edgeport/4
       - Rapidport/4
       - Edgeport/4t
       - Edgeport/2
       - Edgeport/4i
       - Edgeport/2i
       - Edgeport/421
       - Edgeport/21
       - Edgeport/8
       - Edgeport/8 Dual
       - Edgeport/2D8
       - Edgeport/4D8
       - Edgeport/8i
       - Edgeport/2 DIN
       - Edgeport/4 DIN
       - Edgeport/16 Dual

  For any questions or problems with this driver, please contact Greg
  Kroah-Hartman at greg@kroah.com


REINER SCT cyberJack pinpad/e-com USB chipcard reader
-----------------------------------------------------

  Interface to ISO 7816 compatible contactbased chipcards, e.g. GSM SIMs.

  Current status:

    This is the kernel part of the driver for this USB card reader.
    There is also a user part for a CT-API driver available. A site
    for downloading is TBA. For now, you can request it from the
    maintainer (linux-usb@sii.li).

  For any questions or problems with this driver, please contact
  linux-usb@sii.li


Prolific PL2303 Driver
----------------------

  This driver supports any device that has the PL2303 chip from Prolific
  in it.  This includes a number of single port USB to serial converters,
  more than 70% of USB GPS devices (in 2010), and some USB UPSes. Devices
  from Aten (the UC-232) and IO-Data work with this driver, as does
  the DCU-11 mobile-phone cable.

  For any questions or problems with this driver, please contact Greg
  Kroah-Hartman at greg@kroah.com


KL5KUSB105 chipset / PalmConnect USB single-port adapter
--------------------------------------------------------

Current status:

  The driver was put together by looking at the usb bus transactions
  done by Palm's driver under Windows, so a lot of functionality is
  still missing.  Notably, serial ioctls are sometimes faked or not yet
  implemented.  Support for finding out about DSR and CTS line status is
  however implemented (though not nicely), so your favorite autopilot(1)
  and pilot-manager -daemon calls will work.  Baud rates up to 115200
  are supported, but handshaking (software or hardware) is not, which is
  why it is wise to cut down on the rate used is wise for large
  transfers until this is settled.

  See http://www.uuhaus.de/linux/palmconnect.html for up-to-date
  information on this driver.

Winchiphead CH341 Driver
------------------------

  This driver is for the Winchiphead CH341 USB-RS232 Converter. This chip
  also implements an IEEE 1284 parallel port, I2C and SPI, but that is not
  supported by the driver. The protocol was analyzed from the behaviour
  of the Windows driver, no datasheet is available at present.

  The manufacturer's website: http://www.winchiphead.com/.

  For any questions or problems with this driver, please contact
  frank@kingswood-consulting.co.uk.

Moschip MCS7720, MCS7715 driver
-------------------------------

  These chips are present in devices sold by various manufacturers, such as Syba
  and Cables Unlimited.  There may be others.  The 7720 provides two serial
  ports, and the 7715 provides one serial and one standard PC parallel port.
  Support for the 7715's parallel port is enabled by a separate option, which
  will not appear unless parallel port support is first enabled at the top-level
  of the Device Drivers config menu.  Currently only compatibility mode is
  supported on the parallel port (no ECP/EPP).

  TODO:
    - Implement ECP/EPP modes for the parallel port.
    - Baud rates higher than 115200 are currently broken.
    - Devices with a single serial port based on the Moschip MCS7703 may work
      with this driver with a simple addition to the usb_device_id table.  I
      don't have one of these devices, so I can't say for sure.

Generic Serial driver
---------------------

  If your device is not one of the above listed devices, compatible with
  the above models, you can try out the "generic" interface. This
  interface does not provide any type of control messages sent to the
  device, and does not support any kind of device flow control. All that
  is required of your device is that it has at least one bulk in endpoint,
  or one bulk out endpoint.

  To enable the generic driver to recognize your device, provide::

	echo <vid> <pid> >/sys/bus/usb-serial/drivers/generic/new_id

  where the <vid> and <pid> is replaced with the hex representation of your
  device's vendor id and product id.
  If the driver is compiled as a module you can also provide one id when
  loading the module::

	insmod usbserial vendor=0x#### product=0x####

  This driver has been successfully used to connect to the NetChip USB
  development board, providing a way to develop USB firmware without
  having to write a custom driver.

  For any questions or problems with this driver, please contact Greg
  Kroah-Hartman at greg@kroah.com


Contact
=======

  If anyone has any problems using these drivers, with any of the above
  specified products, please contact the specific driver's author listed
  above, or join the Linux-USB mailing list (information on joining the
  mailing list, as well as a link to its searchable archive is at
  http://www.linux-usb.org/ )


Greg Kroah-Hartman
greg@kroah.com
