==============
Serial Devices
==============

Serial Device Naming
====================

    As of 2.6.10, serial devices on ia64 are named based on the
    order of ACPI and PCI enumeration.  The first device in the
    ACPI namespace (if any) becomes /dev/ttyS0, the second becomes
    /dev/ttyS1, etc., and PCI devices are named sequentially
    starting after the ACPI devices.

    Prior to 2.6.10, there were confusing exceptions to this:

	- Firmware on some machines (mostly from HP) provides an HCDP
	  table[1] that tells the kernel about devices that can be used
	  as a serial console.  If the user specified "console=ttyS0"
	  or the EFI ConOut path contained only UART devices, the
	  kernel registered the device described by the HCDP as
	  /dev/ttyS0.

	- If there was no HCDP, we assumed there were UARTs at the
	  legacy COM port addresses (I/O ports 0x3f8 and 0x2f8), so
	  the kernel registered those as /dev/ttyS0 and /dev/ttyS1.

    Any additional ACPI or PCI devices were registered sequentially
    after /dev/ttyS0 as they were discovered.

    With an HCDP, device names changed depending on EFI configuration
    and "console=" arguments.  Without an HCDP, device names didn't
    change, but we registered devices that might not really exist.

    For example, an HP rx1600 with a single built-in serial port
    (described in the ACPI namespace) plus an MP[2] (a PCI device) has
    these ports:

      ==========  ==========     ============    ============   =======
      Type        MMIO           pre-2.6.10      pre-2.6.10     2.6.10+
		  address
				 (EFI console    (EFI console
                                 on builtin)     on MP port)
      ==========  ==========     ============    ============   =======
      builtin     0xff5e0000        ttyS0           ttyS1         ttyS0
      MP UPS      0xf8031000        ttyS1           ttyS2         ttyS1
      MP Console  0xf8030000        ttyS2           ttyS0         ttyS2
      MP 2        0xf8030010        ttyS3           ttyS3         ttyS3
      MP 3        0xf8030038        ttyS4           ttyS4         ttyS4
      ==========  ==========     ============    ============   =======

Console Selection
=================

    EFI knows what your console devices are, but it doesn't tell the
    kernel quite enough to actually locate them.  The DIG64 HCDP
    table[1] does tell the kernel where potential serial console
    devices are, but not all firmware supplies it.  Also, EFI supports
    multiple simultaneous consoles and doesn't tell the kernel which
    should be the "primary" one.

    So how do you tell Linux which console device to use?

	- If your firmware supplies the HCDP, it is simplest to
	  configure EFI with a single device (either a UART or a VGA
	  card) as the console.  Then you don't need to tell Linux
	  anything; the kernel will automatically use the EFI console.

	  (This works only in 2.6.6 or later; prior to that you had
	  to specify "console=ttyS0" to get a serial console.)

	- Without an HCDP, Linux defaults to a VGA console unless you
	  specify a "console=" argument.

    NOTE: Don't assume that a serial console device will be /dev/ttyS0.
    It might be ttyS1, ttyS2, etc.  Make sure you have the appropriate
    entries in /etc/inittab (for getty) and /etc/securetty (to allow
    root login).

Early Serial Console
====================

    The kernel can't start using a serial console until it knows where
    the device lives.  Normally this happens when the driver enumerates
    all the serial devices, which can happen a minute or more after the
    kernel starts booting.

    2.6.10 and later kernels have an "early uart" driver that works
    very early in the boot process.  The kernel will automatically use
    this if the user supplies an argument like "console=uart,io,0x3f8",
    or if the EFI console path contains only a UART device and the
    firmware supplies an HCDP.

Troubleshooting Serial Console Problems
=======================================

    No kernel output after elilo prints "Uncompressing Linux... done":

	- You specified "console=ttyS0" but Linux changed the device
	  to which ttyS0 refers.  Configure exactly one EFI console
	  device[3] and remove the "console=" option.

	- The EFI console path contains both a VGA device and a UART.
	  EFI and elilo use both, but Linux defaults to VGA.  Remove
	  the VGA device from the EFI console path[3].

	- Multiple UARTs selected as EFI console devices.  EFI and
	  elilo use all selected devices, but Linux uses only one.
	  Make sure only one UART is selected in the EFI console
	  path[3].

	- You're connected to an HP MP port[2] but have a non-MP UART
	  selected as EFI console device.  EFI uses the MP as a
	  console device even when it isn't explicitly selected.
	  Either move the console cable to the non-MP UART, or change
	  the EFI console path[3] to the MP UART.

    Long pause (60+ seconds) between "Uncompressing Linux... done" and
    start of kernel output:

	- No early console because you used "console=ttyS<n>".  Remove
	  the "console=" option if your firmware supplies an HCDP.

	- If you don't have an HCDP, the kernel doesn't know where
	  your console lives until the driver discovers serial
	  devices.  Use "console=uart,io,0x3f8" (or appropriate
	  address for your machine).

    Kernel and init script output works fine, but no "login:" prompt:

	- Add getty entry to /etc/inittab for console tty.  Look for
	  the "Adding console on ttyS<n>" message that tells you which
	  device is the console.

    "login:" prompt, but can't login as root:

	- Add entry to /etc/securetty for console tty.

    No ACPI serial devices found in 2.6.17 or later:

	- Turn on CONFIG_PNP and CONFIG_PNPACPI.  Prior to 2.6.17, ACPI
	  serial devices were discovered by 8250_acpi.  In 2.6.17,
	  8250_acpi was replaced by the combination of 8250_pnp and
	  CONFIG_PNPACPI.



[1]
    http://www.dig64.org/specifications/agreement
    The table was originally defined as the "HCDP" for "Headless
    Console/Debug Port."  The current version is the "PCDP" for
    "Primary Console and Debug Port Devices."

[2]
    The HP MP (management processor) is a PCI device that provides
    several UARTs.  One of the UARTs is often used as a console; the
    EFI Boot Manager identifies it as "Acpi(HWP0002,700)/Pci(...)/Uart".
    The external connection is usually a 25-pin connector, and a
    special dongle converts that to three 9-pin connectors, one of
    which is labelled "Console."

[3]
    EFI console devices are configured using the EFI Boot Manager
    "Boot option maintenance" menu.  You may have to interrupt the
    boot sequence to use this menu, and you will have to reset the
    box after changing console configuration.
