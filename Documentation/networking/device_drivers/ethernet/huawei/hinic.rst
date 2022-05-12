.. SPDX-License-Identifier: GPL-2.0

============================================================
Linux Kernel Driver for Huawei Intelligent NIC(HiNIC) family
============================================================

Overview:
=========
HiNIC is a network interface card for the Data Center Area.

The driver supports a range of link-speed devices (10GbE, 25GbE, 40GbE, etc.).
The driver supports also a negotiated and extendable feature set.

Some HiNIC devices support SR-IOV. This driver is used for Physical Function
(PF).

HiNIC devices support MSI-X interrupt vector for each Tx/Rx queue and
adaptive interrupt moderation.

HiNIC devices support also various offload features such as checksum offload,
TCP Transmit Segmentation Offload(TSO), Receive-Side Scaling(RSS) and
LRO(Large Receive Offload).


Supported PCI vendor ID/device IDs:
===================================

19e5:1822 - HiNIC PF


Driver Architecture and Source Code:
====================================

hinic_dev - Implement a Logical Network device that is independent from
specific HW details about HW data structure formats.

hinic_hwdev - Implement the HW details of the device and include the components
for accessing the PCI NIC.

hinic_hwdev contains the following components:
===============================================

HW Interface:
=============

The interface for accessing the pci device (DMA memory and PCI BARs).
(hinic_hw_if.c, hinic_hw_if.h)

Configuration Status Registers Area that describes the HW Registers on the
configuration and status BAR0. (hinic_hw_csr.h)

MGMT components:
================

Asynchronous Event Queues(AEQs) - The event queues for receiving messages from
the MGMT modules on the cards. (hinic_hw_eqs.c, hinic_hw_eqs.h)

Application Programmable Interface commands(API CMD) - Interface for sending
MGMT commands to the card. (hinic_hw_api_cmd.c, hinic_hw_api_cmd.h)

Management (MGMT) - the PF to MGMT channel that uses API CMD for sending MGMT
commands to the card and receives notifications from the MGMT modules on the
card by AEQs. Also set the addresses of the IO CMDQs in HW.
(hinic_hw_mgmt.c, hinic_hw_mgmt.h)

IO components:
==============

Completion Event Queues(CEQs) - The completion Event Queues that describe IO
tasks that are finished. (hinic_hw_eqs.c, hinic_hw_eqs.h)

Work Queues(WQ) - Contain the memory and operations for use by CMD queues and
the Queue Pairs. The WQ is a Memory Block in a Page. The Block contains
pointers to Memory Areas that are the Memory for the Work Queue Elements(WQEs).
(hinic_hw_wq.c, hinic_hw_wq.h)

Command Queues(CMDQ) - The queues for sending commands for IO management and is
used to set the QPs addresses in HW. The commands completion events are
accumulated on the CEQ that is configured to receive the CMDQ completion events.
(hinic_hw_cmdq.c, hinic_hw_cmdq.h)

Queue Pairs(QPs) - The HW Receive and Send queues for Receiving and Transmitting
Data. (hinic_hw_qp.c, hinic_hw_qp.h, hinic_hw_qp_ctxt.h)

IO - de/constructs all the IO components. (hinic_hw_io.c, hinic_hw_io.h)

HW device:
==========

HW device - de/constructs the HW Interface, the MGMT components on the
initialization of the driver and the IO components on the case of Interface
UP/DOWN Events. (hinic_hw_dev.c, hinic_hw_dev.h)


hinic_dev contains the following components:
===============================================

PCI ID table - Contains the supported PCI Vendor/Device IDs.
(hinic_pci_tbl.h)

Port Commands - Send commands to the HW device for port management
(MAC, Vlan, MTU, ...). (hinic_port.c, hinic_port.h)

Tx Queues - Logical Tx Queues that use the HW Send Queues for transmit.
The Logical Tx queue is not dependent on the format of the HW Send Queue.
(hinic_tx.c, hinic_tx.h)

Rx Queues - Logical Rx Queues that use the HW Receive Queues for receive.
The Logical Rx queue is not dependent on the format of the HW Receive Queue.
(hinic_rx.c, hinic_rx.h)

hinic_dev - de/constructs the Logical Tx and Rx Queues.
(hinic_main.c, hinic_dev.h)


Miscellaneous
=============

Common functions that are used by HW and Logical Device.
(hinic_common.c, hinic_common.h)


Support
=======

If an issue is identified with the released source code on the supported kernel
with a supported adapter, email the specific information related to the issue to
aviad.krawczyk@huawei.com.
