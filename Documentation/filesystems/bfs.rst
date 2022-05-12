.. SPDX-License-Identifier: GPL-2.0

========================
BFS Filesystem for Linux
========================

The BFS filesystem is used by SCO UnixWare OS for the /stand slice, which
usually contains the kernel image and a few other files required for the
boot process.

In order to access /stand partition under Linux you obviously need to
know the partition number and the kernel must support UnixWare disk slices
(CONFIG_UNIXWARE_DISKLABEL config option). However BFS support does not
depend on having UnixWare disklabel support because one can also mount
BFS filesystem via loopback::

    # losetup /dev/loop0 stand.img
    # mount -t bfs /dev/loop0 /mnt/stand

where stand.img is a file containing the image of BFS filesystem.
When you have finished using it and umounted you need to also deallocate
/dev/loop0 device by::

    # losetup -d /dev/loop0

You can simplify mounting by just typing::

    # mount -t bfs -o loop stand.img /mnt/stand

this will allocate the first available loopback device (and load loop.o
kernel module if necessary) automatically. If the loopback driver is not
loaded automatically, make sure that you have compiled the module and
that modprobe is functioning. Beware that umount will not deallocate
/dev/loopN device if /etc/mtab file on your system is a symbolic link to
/proc/mounts. You will need to do it manually using "-d" switch of
losetup(8). Read losetup(8) manpage for more info.

To create the BFS image under UnixWare you need to find out first which
slice contains it. The command prtvtoc(1M) is your friend::

    # prtvtoc /dev/rdsk/c0b0t0d0s0

(assuming your root disk is on target=0, lun=0, bus=0, controller=0). Then you
look for the slice with tag "STAND", which is usually slice 10. With this
information you can use dd(1) to create the BFS image::

    # umount /stand
    # dd if=/dev/rdsk/c0b0t0d0sa of=stand.img bs=512

Just in case, you can verify that you have done the right thing by checking
the magic number::

    # od -Ad -tx4 stand.img | more

The first 4 bytes should be 0x1badface.

If you have any patches, questions or suggestions regarding this BFS
implementation please contact the author:

Tigran Aivazian <aivazian.tigran@gmail.com>
