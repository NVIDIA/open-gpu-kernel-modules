===============================================
Block layer statistics in /sys/block/<dev>/stat
===============================================

This file documents the contents of the /sys/block/<dev>/stat file.

The stat file provides several statistics about the state of block
device <dev>.

Q.
   Why are there multiple statistics in a single file?  Doesn't sysfs
   normally contain a single value per file?

A.
   By having a single file, the kernel can guarantee that the statistics
   represent a consistent snapshot of the state of the device.  If the
   statistics were exported as multiple files containing one statistic
   each, it would be impossible to guarantee that a set of readings
   represent a single point in time.

The stat file consists of a single line of text containing 11 decimal
values separated by whitespace.  The fields are summarized in the
following table, and described in more detail below.


=============== ============= =================================================
Name            units         description
=============== ============= =================================================
read I/Os       requests      number of read I/Os processed
read merges     requests      number of read I/Os merged with in-queue I/O
read sectors    sectors       number of sectors read
read ticks      milliseconds  total wait time for read requests
write I/Os      requests      number of write I/Os processed
write merges    requests      number of write I/Os merged with in-queue I/O
write sectors   sectors       number of sectors written
write ticks     milliseconds  total wait time for write requests
in_flight       requests      number of I/Os currently in flight
io_ticks        milliseconds  total time this block device has been active
time_in_queue   milliseconds  total wait time for all requests
discard I/Os    requests      number of discard I/Os processed
discard merges  requests      number of discard I/Os merged with in-queue I/O
discard sectors sectors       number of sectors discarded
discard ticks   milliseconds  total wait time for discard requests
flush I/Os      requests      number of flush I/Os processed
flush ticks     milliseconds  total wait time for flush requests
=============== ============= =================================================

read I/Os, write I/Os, discard I/0s
===================================

These values increment when an I/O request completes.

flush I/Os
==========

These values increment when an flush I/O request completes.

Block layer combines flush requests and executes at most one at a time.
This counts flush requests executed by disk. Not tracked for partitions.

read merges, write merges, discard merges
=========================================

These values increment when an I/O request is merged with an
already-queued I/O request.

read sectors, write sectors, discard_sectors
============================================

These values count the number of sectors read from, written to, or
discarded from this block device.  The "sectors" in question are the
standard UNIX 512-byte sectors, not any device- or filesystem-specific
block size.  The counters are incremented when the I/O completes.

read ticks, write ticks, discard ticks, flush ticks
===================================================

These values count the number of milliseconds that I/O requests have
waited on this block device.  If there are multiple I/O requests waiting,
these values will increase at a rate greater than 1000/second; for
example, if 60 read requests wait for an average of 30 ms, the read_ticks
field will increase by 60*30 = 1800.

in_flight
=========

This value counts the number of I/O requests that have been issued to
the device driver but have not yet completed.  It does not include I/O
requests that are in the queue but not yet issued to the device driver.

io_ticks
========

This value counts the number of milliseconds during which the device has
had I/O requests queued.

time_in_queue
=============

This value counts the number of milliseconds that I/O requests have waited
on this block device.  If there are multiple I/O requests waiting, this
value will increase as the product of the number of milliseconds times the
number of requests waiting (see "read ticks" above for an example).
