========
dm-delay
========

Device-Mapper's "delay" target delays reads and/or writes
and maps them to different devices.

Parameters::

    <device> <offset> <delay> [<write_device> <write_offset> <write_delay>
			       [<flush_device> <flush_offset> <flush_delay>]]

With separate write parameters, the first set is only used for reads.
Offsets are specified in sectors.
Delays are specified in milliseconds.

Example scripts
===============

::

	#!/bin/sh
	# Create device delaying rw operation for 500ms
	echo "0 `blockdev --getsz $1` delay $1 0 500" | dmsetup create delayed

::

	#!/bin/sh
	# Create device delaying only write operation for 500ms and
	# splitting reads and writes to different devices $1 $2
	echo "0 `blockdev --getsz $1` delay $1 0 0 $2 0 500" | dmsetup create delayed
