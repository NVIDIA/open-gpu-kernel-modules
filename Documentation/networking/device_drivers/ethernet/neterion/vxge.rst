.. SPDX-License-Identifier: GPL-2.0

==============================================================================
Neterion's (Formerly S2io) X3100 Series 10GbE PCIe Server Adapter Linux driver
==============================================================================

.. Contents

  1) Introduction
  2) Features supported
  3) Configurable driver parameters
  4) Troubleshooting

1. Introduction
===============

This Linux driver supports all Neterion's X3100 series 10 GbE PCIe I/O
Virtualized Server adapters.

The X3100 series supports four modes of operation, configurable via
firmware:

	- Single function mode
	- Multi function mode
	- SRIOV mode
	- MRIOV mode

The functions share a 10GbE link and the pci-e bus, but hardly anything else
inside the ASIC. Features like independent hw reset, statistics, bandwidth/
priority allocation and guarantees, GRO, TSO, interrupt moderation etc are
supported independently on each function.

(See below for a complete list of features supported for both IPv4 and IPv6)

2. Features supported
=====================

i)   Single function mode (up to 17 queues)

ii)  Multi function mode (up to 17 functions)

iii) PCI-SIG's I/O Virtualization

       - Single Root mode: v1.0 (up to 17 functions)
       - Multi-Root mode: v1.0 (up to 17 functions)

iv)  Jumbo frames

       X3100 Series supports MTU up to 9600 bytes, modifiable using
       ip command.

v)   Offloads supported: (Enabled by default)

       - Checksum offload (TCP/UDP/IP) on transmit and receive paths
       - TCP Segmentation Offload (TSO) on transmit path
       - Generic Receive Offload (GRO) on receive path

vi)  MSI-X: (Enabled by default)

       Resulting in noticeable performance improvement (up to 7% on certain
       platforms).

vii) NAPI: (Enabled by default)

       For better Rx interrupt moderation.

viii)RTH (Receive Traffic Hash): (Enabled by default)

       Receive side steering for better scaling.

ix)  Statistics

       Comprehensive MAC-level and software statistics displayed using
       "ethtool -S" option.

x)   Multiple hardware queues: (Enabled by default)

       Up to 17 hardware based transmit and receive data channels, with
       multiple steering options (transmit multiqueue enabled by default).

3) Configurable driver parameters:
----------------------------------

i)  max_config_dev
       Specifies maximum device functions to be enabled.

       Valid range: 1-8

ii) max_config_port
       Specifies number of ports to be enabled.

       Valid range: 1,2

       Default: 1

iii) max_config_vpath
       Specifies maximum VPATH(s) configured for each device function.

       Valid range: 1-17

iv) vlan_tag_strip
       Enables/disables vlan tag stripping from all received tagged frames that
       are not replicated at the internal L2 switch.

       Valid range: 0,1 (disabled, enabled respectively)

       Default: 1

v)  addr_learn_en
       Enable learning the mac address of the guest OS interface in
       virtualization environment.

       Valid range: 0,1 (disabled, enabled respectively)

       Default: 0
