=========================================
Dell Remote BIOS Update driver (dell_rbu)
=========================================

Purpose
=======

Document demonstrating the use of the Dell Remote BIOS Update driver
for updating BIOS images on Dell servers and desktops.

Scope
=====

This document discusses the functionality of the rbu driver only.
It does not cover the support needed from applications to enable the BIOS to
update itself with the image downloaded in to the memory.

Overview
========

This driver works with Dell OpenManage or Dell Update Packages for updating
the BIOS on Dell servers (starting from servers sold since 1999), desktops
and notebooks (starting from those sold in 2005).

Please go to  http://support.dell.com register and you can find info on
OpenManage and Dell Update packages (DUP).

Libsmbios can also be used to update BIOS on Dell systems go to
https://linux.dell.com/libsmbios/ for details.

Dell_RBU driver supports BIOS update using the monolithic image and packetized
image methods. In case of monolithic the driver allocates a contiguous chunk
of physical pages having the BIOS image. In case of packetized the app
using the driver breaks the image in to packets of fixed sizes and the driver
would place each packet in contiguous physical memory. The driver also
maintains a link list of packets for reading them back.

If the dell_rbu driver is unloaded all the allocated memory is freed.

The rbu driver needs to have an application (as mentioned above) which will
inform the BIOS to enable the update in the next system reboot.

The user should not unload the rbu driver after downloading the BIOS image
or updating.

The driver load creates the following directories under the /sys file system::

	/sys/class/firmware/dell_rbu/loading
	/sys/class/firmware/dell_rbu/data
	/sys/devices/platform/dell_rbu/image_type
	/sys/devices/platform/dell_rbu/data
	/sys/devices/platform/dell_rbu/packet_size

The driver supports two types of update mechanism; monolithic and packetized.
These update mechanism depends upon the BIOS currently running on the system.
Most of the Dell systems support a monolithic update where the BIOS image is
copied to a single contiguous block of physical memory.

In case of packet mechanism the single memory can be broken in smaller chunks
of contiguous memory and the BIOS image is scattered in these packets.

By default the driver uses monolithic memory for the update type. This can be
changed to packets during the driver load time by specifying the load
parameter image_type=packet.  This can also be changed later as below::

	echo packet > /sys/devices/platform/dell_rbu/image_type

In packet update mode the packet size has to be given before any packets can
be downloaded. It is done as below::

	echo XXXX > /sys/devices/platform/dell_rbu/packet_size

In the packet update mechanism, the user needs to create a new file having
packets of data arranged back to back. It can be done as follows:
The user creates packets header, gets the chunk of the BIOS image and
places it next to the packetheader; now, the packetheader + BIOS image chunk
added together should match the specified packet_size. This makes one
packet, the user needs to create more such packets out of the entire BIOS
image file and then arrange all these packets back to back in to one single
file.

This file is then copied to /sys/class/firmware/dell_rbu/data.
Once this file gets to the driver, the driver extracts packet_size data from
the file and spreads it across the physical memory in contiguous packet_sized
space.

This method makes sure that all the packets get to the driver in a single operation.

In monolithic update the user simply get the BIOS image (.hdr file) and copies
to the data file as is without any change to the BIOS image itself.

Do the steps below to download the BIOS image.

1) echo 1 > /sys/class/firmware/dell_rbu/loading
2) cp bios_image.hdr /sys/class/firmware/dell_rbu/data
3) echo 0 > /sys/class/firmware/dell_rbu/loading

The /sys/class/firmware/dell_rbu/ entries will remain till the following is
done.

::

	echo -1 > /sys/class/firmware/dell_rbu/loading

Until this step is completed the driver cannot be unloaded.

Also echoing either mono, packet or init in to image_type will free up the
memory allocated by the driver.

If a user by accident executes steps 1 and 3 above without executing step 2;
it will make the /sys/class/firmware/dell_rbu/ entries disappear.

The entries can be recreated by doing the following::

	echo init > /sys/devices/platform/dell_rbu/image_type

.. note:: echoing init in image_type does not change its original value.

Also the driver provides /sys/devices/platform/dell_rbu/data readonly file to
read back the image downloaded.

.. note::

   After updating the BIOS image a user mode application needs to execute
   code which sends the BIOS update request to the BIOS. So on the next reboot
   the BIOS knows about the new image downloaded and it updates itself.
   Also don't unload the rbu driver if the image has to be updated.

