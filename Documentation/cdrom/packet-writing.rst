==============
Packet writing
==============

Getting started quick
---------------------

- Select packet support in the block device section and UDF support in
  the file system section.

- Compile and install kernel and modules, reboot.

- You need the udftools package (pktsetup, mkudffs, cdrwtool).
  Download from http://sourceforge.net/projects/linux-udf/

- Grab a new CD-RW disc and format it (assuming CD-RW is hdc, substitute
  as appropriate)::

	# cdrwtool -d /dev/hdc -q

- Setup your writer::

	# pktsetup dev_name /dev/hdc

- Now you can mount /dev/pktcdvd/dev_name and copy files to it. Enjoy::

	# mount /dev/pktcdvd/dev_name /cdrom -t udf -o rw,noatime


Packet writing for DVD-RW media
-------------------------------

DVD-RW discs can be written to much like CD-RW discs if they are in
the so called "restricted overwrite" mode. To put a disc in restricted
overwrite mode, run::

	# dvd+rw-format /dev/hdc

You can then use the disc the same way you would use a CD-RW disc::

	# pktsetup dev_name /dev/hdc
	# mount /dev/pktcdvd/dev_name /cdrom -t udf -o rw,noatime


Packet writing for DVD+RW media
-------------------------------

According to the DVD+RW specification, a drive supporting DVD+RW discs
shall implement "true random writes with 2KB granularity", which means
that it should be possible to put any filesystem with a block size >=
2KB on such a disc. For example, it should be possible to do::

	# dvd+rw-format /dev/hdc   (only needed if the disc has never
	                            been formatted)
	# mkudffs /dev/hdc
	# mount /dev/hdc /cdrom -t udf -o rw,noatime

However, some drives don't follow the specification and expect the
host to perform aligned writes at 32KB boundaries. Other drives do
follow the specification, but suffer bad performance problems if the
writes are not 32KB aligned.

Both problems can be solved by using the pktcdvd driver, which always
generates aligned writes::

	# dvd+rw-format /dev/hdc
	# pktsetup dev_name /dev/hdc
	# mkudffs /dev/pktcdvd/dev_name
	# mount /dev/pktcdvd/dev_name /cdrom -t udf -o rw,noatime


Packet writing for DVD-RAM media
--------------------------------

DVD-RAM discs are random writable, so using the pktcdvd driver is not
necessary. However, using the pktcdvd driver can improve performance
in the same way it does for DVD+RW media.


Notes
-----

- CD-RW media can usually not be overwritten more than about 1000
  times, so to avoid unnecessary wear on the media, you should always
  use the noatime mount option.

- Defect management (ie automatic remapping of bad sectors) has not
  been implemented yet, so you are likely to get at least some
  filesystem corruption if the disc wears out.

- Since the pktcdvd driver makes the disc appear as a regular block
  device with a 2KB block size, you can put any filesystem you like on
  the disc. For example, run::

	# /sbin/mke2fs /dev/pktcdvd/dev_name

  to create an ext2 filesystem on the disc.


Using the pktcdvd sysfs interface
---------------------------------

Since Linux 2.6.20, the pktcdvd module has a sysfs interface
and can be controlled by it. For example the "pktcdvd" tool uses
this interface. (see http://tom.ist-im-web.de/download/pktcdvd )

"pktcdvd" works similar to "pktsetup", e.g.::

	# pktcdvd -a dev_name /dev/hdc
	# mkudffs /dev/pktcdvd/dev_name
	# mount -t udf -o rw,noatime /dev/pktcdvd/dev_name /dvdram
	# cp files /dvdram
	# umount /dvdram
	# pktcdvd -r dev_name


For a description of the sysfs interface look into the file:

  Documentation/ABI/testing/sysfs-class-pktcdvd


Using the pktcdvd debugfs interface
-----------------------------------

To read pktcdvd device infos in human readable form, do::

	# cat /sys/kernel/debug/pktcdvd/pktcdvd[0-7]/info

For a description of the debugfs interface look into the file:

  Documentation/ABI/testing/debugfs-pktcdvd



Links
-----

See http://fy.chalmers.se/~appro/linux/DVD+RW/ for more information
about DVD writing.
