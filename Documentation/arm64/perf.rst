.. SPDX-License-Identifier: GPL-2.0

.. _perf_index:

=====================
Perf Event Attributes
=====================

:Author: Andrew Murray <andrew.murray@arm.com>
:Date: 2019-03-06

exclude_user
------------

This attribute excludes userspace.

Userspace always runs at EL0 and thus this attribute will exclude EL0.


exclude_kernel
--------------

This attribute excludes the kernel.

The kernel runs at EL2 with VHE and EL1 without. Guest kernels always run
at EL1.

For the host this attribute will exclude EL1 and additionally EL2 on a VHE
system.

For the guest this attribute will exclude EL1. Please note that EL2 is
never counted within a guest.


exclude_hv
----------

This attribute excludes the hypervisor.

For a VHE host this attribute is ignored as we consider the host kernel to
be the hypervisor.

For a non-VHE host this attribute will exclude EL2 as we consider the
hypervisor to be any code that runs at EL2 which is predominantly used for
guest/host transitions.

For the guest this attribute has no effect. Please note that EL2 is
never counted within a guest.


exclude_host / exclude_guest
----------------------------

These attributes exclude the KVM host and guest, respectively.

The KVM host may run at EL0 (userspace), EL1 (non-VHE kernel) and EL2 (VHE
kernel or non-VHE hypervisor).

The KVM guest may run at EL0 (userspace) and EL1 (kernel).

Due to the overlapping exception levels between host and guests we cannot
exclusively rely on the PMU's hardware exception filtering - therefore we
must enable/disable counting on the entry and exit to the guest. This is
performed differently on VHE and non-VHE systems.

For non-VHE systems we exclude EL2 for exclude_host - upon entering and
exiting the guest we disable/enable the event as appropriate based on the
exclude_host and exclude_guest attributes.

For VHE systems we exclude EL1 for exclude_guest and exclude both EL0,EL2
for exclude_host. Upon entering and exiting the guest we modify the event
to include/exclude EL0 as appropriate based on the exclude_host and
exclude_guest attributes.

The statements above also apply when these attributes are used within a
non-VHE guest however please note that EL2 is never counted within a guest.


Accuracy
--------

On non-VHE hosts we enable/disable counters on the entry/exit of host/guest
transition at EL2 - however there is a period of time between
enabling/disabling the counters and entering/exiting the guest. We are
able to eliminate counters counting host events on the boundaries of guest
entry/exit when counting guest events by filtering out EL2 for
exclude_host. However when using !exclude_hv there is a small blackout
window at the guest entry/exit where host events are not captured.

On VHE systems there are no blackout windows.
