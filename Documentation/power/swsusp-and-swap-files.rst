===============================================
Using swap files with software suspend (swsusp)
===============================================

	(C) 2006 Rafael J. Wysocki <rjw@sisk.pl>

The Linux kernel handles swap files almost in the same way as it handles swap
partitions and there are only two differences between these two types of swap
areas:
(1) swap files need not be contiguous,
(2) the header of a swap file is not in the first block of the partition that
holds it.  From the swsusp's point of view (1) is not a problem, because it is
already taken care of by the swap-handling code, but (2) has to be taken into
consideration.

In principle the location of a swap file's header may be determined with the
help of appropriate filesystem driver.  Unfortunately, however, it requires the
filesystem holding the swap file to be mounted, and if this filesystem is
journaled, it cannot be mounted during resume from disk.  For this reason to
identify a swap file swsusp uses the name of the partition that holds the file
and the offset from the beginning of the partition at which the swap file's
header is located.  For convenience, this offset is expressed in <PAGE_SIZE>
units.

In order to use a swap file with swsusp, you need to:

1) Create the swap file and make it active, eg.::

    # dd if=/dev/zero of=<swap_file_path> bs=1024 count=<swap_file_size_in_k>
    # mkswap <swap_file_path>
    # swapon <swap_file_path>

2) Use an application that will bmap the swap file with the help of the
FIBMAP ioctl and determine the location of the file's swap header, as the
offset, in <PAGE_SIZE> units, from the beginning of the partition which
holds the swap file.

3) Add the following parameters to the kernel command line::

    resume=<swap_file_partition> resume_offset=<swap_file_offset>

where <swap_file_partition> is the partition on which the swap file is located
and <swap_file_offset> is the offset of the swap header determined by the
application in 2) (of course, this step may be carried out automatically
by the same application that determines the swap file's header offset using the
FIBMAP ioctl)

OR

Use a userland suspend application that will set the partition and offset
with the help of the SNAPSHOT_SET_SWAP_AREA ioctl described in
Documentation/power/userland-swsusp.rst (this is the only method to suspend
to a swap file allowing the resume to be initiated from an initrd or initramfs
image).

Now, swsusp will use the swap file in the same way in which it would use a swap
partition.  In particular, the swap file has to be active (ie. be present in
/proc/swaps) so that it can be used for suspending.

Note that if the swap file used for suspending is deleted and recreated,
the location of its header need not be the same as before.  Thus every time
this happens the value of the "resume_offset=" kernel command line parameter
has to be updated.
