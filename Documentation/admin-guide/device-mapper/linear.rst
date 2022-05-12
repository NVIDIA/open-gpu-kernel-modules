=========
dm-linear
=========

Device-Mapper's "linear" target maps a linear range of the Device-Mapper
device onto a linear range of another device.  This is the basic building
block of logical volume managers.

Parameters: <dev path> <offset>
    <dev path>:
	Full pathname to the underlying block-device, or a
        "major:minor" device-number.
    <offset>:
	Starting sector within the device.


Example scripts
===============

::

  #!/bin/sh
  # Create an identity mapping for a device
  echo "0 `blockdev --getsz $1` linear $1 0" | dmsetup create identity

::

  #!/bin/sh
  # Join 2 devices together
  size1=`blockdev --getsz $1`
  size2=`blockdev --getsz $2`
  echo "0 $size1 linear $1 0
  $size1 $size2 linear $2 0" | dmsetup create joined

::

  #!/usr/bin/perl -w
  # Split a device into 4M chunks and then join them together in reverse order.

  my $name = "reverse";
  my $extent_size = 4 * 1024 * 2;
  my $dev = $ARGV[0];
  my $table = "";
  my $count = 0;

  if (!defined($dev)) {
          die("Please specify a device.\n");
  }

  my $dev_size = `blockdev --getsz $dev`;
  my $extents = int($dev_size / $extent_size) -
                (($dev_size % $extent_size) ? 1 : 0);

  while ($extents > 0) {
          my $this_start = $count * $extent_size;
          $extents--;
          $count++;
          my $this_offset = $extents * $extent_size;

          $table .= "$this_start $extent_size linear $dev $this_offset\n";
  }

  `echo \"$table\" | dmsetup create $name`;
