.. SPDX-License-Identifier: GPL-2.0

==========================
Notes on Management Module
==========================

Overview
--------

Different classes of controllers from LSI Logic accept and respond to the
user applications in a similar way. They understand the same firmware control
commands. Furthermore, the applications also can treat different classes of
the controllers uniformly. Hence it is logical to have a single module that
interfaces with the applications on one side and all the low level drivers
on the other.

The advantages, though obvious, are listed for completeness:

	i.	Avoid duplicate code from the low level drivers.
	ii.	Unburden the low level drivers from having to export the
		character node device and related handling.
	iii.	Implement any policy mechanisms in one place.
	iv.	Applications have to interface with only module instead of
		multiple low level drivers.

Currently this module (called Common Management Module) is used only to issue
ioctl commands. But this module is envisioned to handle all user space level
interactions. So any 'proc', 'sysfs' implementations will be localized in this
common module.

Credits
-------

::

	"Shared code in a third module, a "library module", is an acceptable
	solution. modprobe automatically loads dependent modules, so users
	running "modprobe driver1" or "modprobe driver2" would automatically
	load the shared library module."

- Jeff Garzik (jgarzik@pobox.com), 02.25.2004 LKML

::

	"As Jeff hinted, if your userspace<->driver API is consistent between
	your new MPT-based RAID controllers and your existing megaraid driver,
	then perhaps you need a single small helper module (lsiioctl or some
	better name), loaded by both mptraid and megaraid automatically, which
	handles registering the /dev/megaraid node dynamically. In this case,
	both mptraid and megaraid would register with lsiioctl for each
	adapter discovered, and lsiioctl would essentially be a switch,
	redirecting userspace tool ioctls to the appropriate driver."

- Matt Domsch, (Matt_Domsch@dell.com), 02.25.2004 LKML

Design
------

The Common Management Module is implemented in megaraid_mm.[ch] files. This
module acts as a registry for low level hba drivers. The low level drivers
(currently only megaraid) register each controller with the common module.

The applications interface with the common module via the character device
node exported by the module.

The lower level drivers now understand only a new improved ioctl packet called
uioc_t. The management module converts the older ioctl packets from the older
applications into uioc_t. After driver handles the uioc_t, the common module
will convert that back into the old format before returning to applications.

As new applications evolve and replace the old ones, the old packet format
will be retired.

Common module dedicates one uioc_t packet to each controller registered. This
can easily be more than one. But since megaraid is the only low level driver
today, and it can handle only one ioctl, there is no reason to have more. But
as new controller classes get added, this will be tuned appropriately.
