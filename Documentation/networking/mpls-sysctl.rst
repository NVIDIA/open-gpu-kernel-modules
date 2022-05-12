.. SPDX-License-Identifier: GPL-2.0

====================
MPLS Sysfs variables
====================

/proc/sys/net/mpls/* Variables:
===============================

platform_labels - INTEGER
	Number of entries in the platform label table.  It is not
	possible to configure forwarding for label values equal to or
	greater than the number of platform labels.

	A dense utilization of the entries in the platform label table
	is possible and expected as the platform labels are locally
	allocated.

	If the number of platform label table entries is set to 0 no
	label will be recognized by the kernel and mpls forwarding
	will be disabled.

	Reducing this value will remove all label routing entries that
	no longer fit in the table.

	Possible values: 0 - 1048575

	Default: 0

ip_ttl_propagate - BOOL
	Control whether TTL is propagated from the IPv4/IPv6 header to
	the MPLS header on imposing labels and propagated from the
	MPLS header to the IPv4/IPv6 header on popping the last label.

	If disabled, the MPLS transport network will appear as a
	single hop to transit traffic.

	* 0 - disabled / RFC 3443 [Short] Pipe Model
	* 1 - enabled / RFC 3443 Uniform Model (default)

default_ttl - INTEGER
	Default TTL value to use for MPLS packets where it cannot be
	propagated from an IP header, either because one isn't present
	or ip_ttl_propagate has been disabled.

	Possible values: 1 - 255

	Default: 255

conf/<interface>/input - BOOL
	Control whether packets can be input on this interface.

	If disabled, packets will be discarded without further
	processing.

	* 0 - disabled (default)
	* not 0 - enabled
