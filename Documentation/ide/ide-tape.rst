===============================
IDE ATAPI streaming tape driver
===============================

This driver is a part of the Linux ide driver.

The driver, in co-operation with ide.c, basically traverses the
request-list for the block device interface. The character device
interface, on the other hand, creates new requests, adds them
to the request-list of the block device, and waits for their completion.

The block device major and minor numbers are determined from the
tape's relative position in the ide interfaces, as explained in ide.c.

The character device interface consists of the following devices::

  ht0		major 37, minor 0	first  IDE tape, rewind on close.
  ht1		major 37, minor 1	second IDE tape, rewind on close.
  ...
  nht0		major 37, minor 128	first  IDE tape, no rewind on close.
  nht1		major 37, minor 129	second IDE tape, no rewind on close.
  ...

The general magnetic tape commands compatible interface, as defined by
include/linux/mtio.h, is accessible through the character device.

General ide driver configuration options, such as the interrupt-unmask
flag, can be configured by issuing an ioctl to the block device interface,
as any other ide device.

Our own ide-tape ioctl's can be issued to either the block device or
the character device interface.

Maximal throughput with minimal bus load will usually be achieved in the
following scenario:

     1.	ide-tape is operating in the pipelined operation mode.
     2.	No buffering is performed by the user backup program.

Testing was done with a 2 GB CONNER CTMA 4000 IDE ATAPI Streaming Tape Drive.

Here are some words from the first releases of hd.c, which are quoted
in ide.c and apply here as well:

* Special care is recommended.  Have Fun!

Possible improvements
=====================

1. Support for the ATAPI overlap protocol.

In order to maximize bus throughput, we currently use the DSC
overlap method which enables ide.c to service requests from the
other device while the tape is busy executing a command. The
DSC overlap method involves polling the tape's status register
for the DSC bit, and servicing the other device while the tape
isn't ready.

In the current QIC development standard (December 1995),
it is recommended that new tape drives will *in addition*
implement the ATAPI overlap protocol, which is used for the
same purpose - efficient use of the IDE bus, but is interrupt
driven and thus has much less CPU overhead.

ATAPI overlap is likely to be supported in most new ATAPI
devices, including new ATAPI cdroms, and thus provides us
a method by which we can achieve higher throughput when
sharing a (fast) ATA-2 disk with any (slow) new ATAPI device.
