.. SPDX-License-Identifier: GPL-2.0

====
IPv6
====


Options for the ipv6 module are supplied as parameters at load time.

Module options may be given as command line arguments to the insmod
or modprobe command, but are usually specified in either
``/etc/modules.d/*.conf`` configuration files, or in a distro-specific
configuration file.

The available ipv6 module parameters are listed below.  If a parameter
is not specified the default value is used.

The parameters are as follows:

disable

	Specifies whether to load the IPv6 module, but disable all
	its functionality.  This might be used when another module
	has a dependency on the IPv6 module being loaded, but no
	IPv6 addresses or operations are desired.

	The possible values and their effects are:

	0
		IPv6 is enabled.

		This is the default value.

	1
		IPv6 is disabled.

		No IPv6 addresses will be added to interfaces, and
		it will not be possible to open an IPv6 socket.

		A reboot is required to enable IPv6.

autoconf

	Specifies whether to enable IPv6 address autoconfiguration
	on all interfaces.  This might be used when one does not wish
	for addresses to be automatically generated from prefixes
	received in Router Advertisements.

	The possible values and their effects are:

	0
		IPv6 address autoconfiguration is disabled on all interfaces.

		Only the IPv6 loopback address (::1) and link-local addresses
		will be added to interfaces.

	1
		IPv6 address autoconfiguration is enabled on all interfaces.

		This is the default value.

disable_ipv6

	Specifies whether to disable IPv6 on all interfaces.
	This might be used when no IPv6 addresses are desired.

	The possible values and their effects are:

	0
		IPv6 is enabled on all interfaces.

		This is the default value.

	1
		IPv6 is disabled on all interfaces.

		No IPv6 addresses will be added to interfaces.

