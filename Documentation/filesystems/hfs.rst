.. SPDX-License-Identifier: GPL-2.0

==================================
Macintosh HFS Filesystem for Linux
==================================


.. Note:: This filesystem doesn't have a maintainer.


HFS stands for ``Hierarchical File System`` and is the filesystem used
by the Mac Plus and all later Macintosh models.  Earlier Macintosh
models used MFS (``Macintosh File System``), which is not supported,
MacOS 8.1 and newer support a filesystem called HFS+ that's similar to
HFS but is extended in various areas.  Use the hfsplus filesystem driver
to access such filesystems from Linux.


Mount options
=============

When mounting an HFS filesystem, the following options are accepted:

  creator=cccc, type=cccc
	Specifies the creator/type values as shown by the MacOS finder
	used for creating new files.  Default values: '????'.

  uid=n, gid=n
  	Specifies the user/group that owns all files on the filesystems.
	Default:  user/group id of the mounting process.

  dir_umask=n, file_umask=n, umask=n
	Specifies the umask used for all files , all directories or all
	files and directories.  Defaults to the umask of the mounting process.

  session=n
  	Select the CDROM session to mount as HFS filesystem.  Defaults to
	leaving that decision to the CDROM driver.  This option will fail
	with anything but a CDROM as underlying devices.

  part=n
  	Select partition number n from the devices.  Does only makes
	sense for CDROMS because they can't be partitioned under Linux.
	For disk devices the generic partition parsing code does this
	for us.  Defaults to not parsing the partition table at all.

  quiet
  	Ignore invalid mount options instead of complaining.


Writing to HFS Filesystems
==========================

HFS is not a UNIX filesystem, thus it does not have the usual features you'd
expect:

 * You can't modify the set-uid, set-gid, sticky or executable bits or the uid
   and gid of files.
 * You can't create hard- or symlinks, device files, sockets or FIFOs.

HFS does on the other have the concepts of multiple forks per file.  These
non-standard forks are represented as hidden additional files in the normal
filesystems namespace which is kind of a cludge and makes the semantics for
the a little strange:

 * You can't create, delete or rename resource forks of files or the
   Finder's metadata.
 * They are however created (with default values), deleted and renamed
   along with the corresponding data fork or directory.
 * Copying files to a different filesystem will loose those attributes
   that are essential for MacOS to work.


Creating HFS filesystems
========================

The hfsutils package from Robert Leslie contains a program called
hformat that can be used to create HFS filesystem. See
<https://www.mars.org/home/rob/proj/hfs/> for details.


Credits
=======

The HFS drivers was written by Paul H. Hargrovea (hargrove@sccm.Stanford.EDU).
Roman Zippel (roman@ardistech.com) rewrote large parts of the code and brought
in btree routines derived from Brad Boyer's hfsplus driver.
