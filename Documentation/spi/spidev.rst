=================
SPI userspace API
=================

SPI devices have a limited userspace API, supporting basic half-duplex
read() and write() access to SPI slave devices.  Using ioctl() requests,
full duplex transfers and device I/O configuration are also available.

::

	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
	#include <linux/types.h>
	#include <linux/spi/spidev.h>

Some reasons you might want to use this programming interface include:

 * Prototyping in an environment that's not crash-prone; stray pointers
   in userspace won't normally bring down any Linux system.

 * Developing simple protocols used to talk to microcontrollers acting
   as SPI slaves, which you may need to change quite often.

Of course there are drivers that can never be written in userspace, because
they need to access kernel interfaces (such as IRQ handlers or other layers
of the driver stack) that are not accessible to userspace.


DEVICE CREATION, DRIVER BINDING
===============================
The simplest way to arrange to use this driver is to just list it in the
spi_board_info for a device as the driver it should use:  the "modalias"
entry is "spidev", matching the name of the driver exposing this API.
Set up the other device characteristics (bits per word, SPI clocking,
chipselect polarity, etc) as usual, so you won't always need to override
them later.

(Sysfs also supports userspace driven binding/unbinding of drivers to
devices.  That mechanism might be supported here in the future.)

When you do that, the sysfs node for the SPI device will include a child
device node with a "dev" attribute that will be understood by udev or mdev.
(Larger systems will have "udev".  Smaller ones may configure "mdev" into
busybox; it's less featureful, but often enough.)  For a SPI device with
chipselect C on bus B, you should see:

    /dev/spidevB.C ...
	character special device, major number 153 with
	a dynamically chosen minor device number.  This is the node
	that userspace programs will open, created by "udev" or "mdev".

    /sys/devices/.../spiB.C ...
	as usual, the SPI device node will
	be a child of its SPI master controller.

    /sys/class/spidev/spidevB.C ...
	created when the "spidev" driver
	binds to that device.  (Directory or symlink, based on whether
	or not you enabled the "deprecated sysfs files" Kconfig option.)

Do not try to manage the /dev character device special file nodes by hand.
That's error prone, and you'd need to pay careful attention to system
security issues; udev/mdev should already be configured securely.

If you unbind the "spidev" driver from that device, those two "spidev" nodes
(in sysfs and in /dev) should automatically be removed (respectively by the
kernel and by udev/mdev).  You can unbind by removing the "spidev" driver
module, which will affect all devices using this driver.  You can also unbind
by having kernel code remove the SPI device, probably by removing the driver
for its SPI controller (so its spi_master vanishes).

Since this is a standard Linux device driver -- even though it just happens
to expose a low level API to userspace -- it can be associated with any number
of devices at a time.  Just provide one spi_board_info record for each such
SPI device, and you'll get a /dev device node for each device.


BASIC CHARACTER DEVICE API
==========================
Normal open() and close() operations on /dev/spidevB.D files work as you
would expect.

Standard read() and write() operations are obviously only half-duplex, and
the chipselect is deactivated between those operations.  Full-duplex access,
and composite operation without chipselect de-activation, is available using
the SPI_IOC_MESSAGE(N) request.

Several ioctl() requests let your driver read or override the device's current
settings for data transfer parameters:

    SPI_IOC_RD_MODE, SPI_IOC_WR_MODE ...
	pass a pointer to a byte which will
	return (RD) or assign (WR) the SPI transfer mode.  Use the constants
	SPI_MODE_0..SPI_MODE_3; or if you prefer you can combine SPI_CPOL
	(clock polarity, idle high iff this is set) or SPI_CPHA (clock phase,
	sample on trailing edge iff this is set) flags.
	Note that this request is limited to SPI mode flags that fit in a
	single byte.

    SPI_IOC_RD_MODE32, SPI_IOC_WR_MODE32 ...
	pass a pointer to a uin32_t
	which will return (RD) or assign (WR) the full SPI transfer mode,
	not limited to the bits that fit in one byte.

    SPI_IOC_RD_LSB_FIRST, SPI_IOC_WR_LSB_FIRST ...
	pass a pointer to a byte
	which will return (RD) or assign (WR) the bit justification used to
	transfer SPI words.  Zero indicates MSB-first; other values indicate
	the less common LSB-first encoding.  In both cases the specified value
	is right-justified in each word, so that unused (TX) or undefined (RX)
	bits are in the MSBs.

    SPI_IOC_RD_BITS_PER_WORD, SPI_IOC_WR_BITS_PER_WORD ...
	pass a pointer to
	a byte which will return (RD) or assign (WR) the number of bits in
	each SPI transfer word.  The value zero signifies eight bits.

    SPI_IOC_RD_MAX_SPEED_HZ, SPI_IOC_WR_MAX_SPEED_HZ ...
	pass a pointer to a
	u32 which will return (RD) or assign (WR) the maximum SPI transfer
	speed, in Hz.  The controller can't necessarily assign that specific
	clock speed.

NOTES:

    - At this time there is no async I/O support; everything is purely
      synchronous.

    - There's currently no way to report the actual bit rate used to
      shift data to/from a given device.

    - From userspace, you can't currently change the chip select polarity;
      that could corrupt transfers to other devices sharing the SPI bus.
      Each SPI device is deselected when it's not in active use, allowing
      other drivers to talk to other devices.

    - There's a limit on the number of bytes each I/O request can transfer
      to the SPI device.  It defaults to one page, but that can be changed
      using a module parameter.

    - Because SPI has no low-level transfer acknowledgement, you usually
      won't see any I/O errors when talking to a non-existent device.


FULL DUPLEX CHARACTER DEVICE API
================================

See the spidev_fdx.c sample program for one example showing the use of the
full duplex programming interface.  (Although it doesn't perform a full duplex
transfer.)  The model is the same as that used in the kernel spi_sync()
request; the individual transfers offer the same capabilities as are
available to kernel drivers (except that it's not asynchronous).

The example shows one half-duplex RPC-style request and response message.
These requests commonly require that the chip not be deselected between
the request and response.  Several such requests could be chained into
a single kernel request, even allowing the chip to be deselected after
each response.  (Other protocol options include changing the word size
and bitrate for each transfer segment.)

To make a full duplex request, provide both rx_buf and tx_buf for the
same transfer.  It's even OK if those are the same buffer.
