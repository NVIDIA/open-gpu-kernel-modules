==========================================
LDM - Logical Disk Manager (Dynamic Disks)
==========================================

:Author: Originally Written by FlatCap - Richard Russon <ldm@flatcap.org>.
:Last Updated: Anton Altaparmakov on 30 March 2007 for Windows Vista.

Overview
--------

Windows 2000, XP, and Vista use a new partitioning scheme.  It is a complete
replacement for the MSDOS style partitions.  It stores its information in a
1MiB journalled database at the end of the physical disk.  The size of
partitions is limited only by disk space.  The maximum number of partitions is
nearly 2000.

Any partitions created under the LDM are called "Dynamic Disks".  There are no
longer any primary or extended partitions.  Normal MSDOS style partitions are
now known as Basic Disks.

If you wish to use Spanned, Striped, Mirrored or RAID 5 Volumes, you must use
Dynamic Disks.  The journalling allows Windows to make changes to these
partitions and filesystems without the need to reboot.

Once the LDM driver has divided up the disk, you can use the MD driver to
assemble any multi-partition volumes, e.g.  Stripes, RAID5.

To prevent legacy applications from repartitioning the disk, the LDM creates a
dummy MSDOS partition containing one disk-sized partition.  This is what is
supported with the Linux LDM driver.

A newer approach that has been implemented with Vista is to put LDM on top of a
GPT label disk.  This is not supported by the Linux LDM driver yet.


Example
-------

Below we have a 50MiB disk, divided into seven partitions.

.. note::

   The missing 1MiB at the end of the disk is where the LDM database is
   stored.

+-------++--------------+---------+-----++--------------+---------+----+
|Device || Offset Bytes | Sectors | MiB || Size   Bytes | Sectors | MiB|
+=======++==============+=========+=====++==============+=========+====+
|hda    ||            0 |       0 |   0 ||     52428800 |  102400 |  50|
+-------++--------------+---------+-----++--------------+---------+----+
|hda1   ||     51380224 |  100352 |  49 ||      1048576 |    2048 |   1|
+-------++--------------+---------+-----++--------------+---------+----+
|hda2   ||        16384 |      32 |   0 ||      6979584 |   13632 |   6|
+-------++--------------+---------+-----++--------------+---------+----+
|hda3   ||      6995968 |   13664 |   6 ||     10485760 |   20480 |  10|
+-------++--------------+---------+-----++--------------+---------+----+
|hda4   ||     17481728 |   34144 |  16 ||      4194304 |    8192 |   4|
+-------++--------------+---------+-----++--------------+---------+----+
|hda5   ||     21676032 |   42336 |  20 ||      5242880 |   10240 |   5|
+-------++--------------+---------+-----++--------------+---------+----+
|hda6   ||     26918912 |   52576 |  25 ||     10485760 |   20480 |  10|
+-------++--------------+---------+-----++--------------+---------+----+
|hda7   ||     37404672 |   73056 |  35 ||     13959168 |   27264 |  13|
+-------++--------------+---------+-----++--------------+---------+----+

The LDM Database may not store the partitions in the order that they appear on
disk, but the driver will sort them.

When Linux boots, you will see something like::

  hda: 102400 sectors w/32KiB Cache, CHS=50/64/32
  hda: [LDM] hda1 hda2 hda3 hda4 hda5 hda6 hda7


Compiling LDM Support
---------------------

To enable LDM, choose the following two options: 

  - "Advanced partition selection" CONFIG_PARTITION_ADVANCED
  - "Windows Logical Disk Manager (Dynamic Disk) support" CONFIG_LDM_PARTITION

If you believe the driver isn't working as it should, you can enable the extra
debugging code.  This will produce a LOT of output.  The option is:

  - "Windows LDM extra logging" CONFIG_LDM_DEBUG

N.B. The partition code cannot be compiled as a module.

As with all the partition code, if the driver doesn't see signs of its type of
partition, it will pass control to another driver, so there is no harm in
enabling it.

If you have Dynamic Disks but don't enable the driver, then all you will see
is a dummy MSDOS partition filling the whole disk.  You won't be able to mount
any of the volumes on the disk.


Booting
-------

If you enable LDM support, then lilo is capable of booting from any of the
discovered partitions.  However, grub does not understand the LDM partitioning
and cannot boot from a Dynamic Disk.


More Documentation
------------------

There is an Overview of the LDM together with complete Technical Documentation.
It is available for download.

  http://www.linux-ntfs.org/

If you have any LDM questions that aren't answered in the documentation, email
me.

Cheers,
    FlatCap - Richard Russon
    ldm@flatcap.org

