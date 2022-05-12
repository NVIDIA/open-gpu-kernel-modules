.. SPDX-License-Identifier: GPL-2.0

========================
VCPU Dispatch Statistics
========================

For Shared Processor LPARs, the POWER Hypervisor maintains a relatively
static mapping of the LPAR processors (vcpus) to physical processor
chips (representing the "home" node) and tries to always dispatch vcpus
on their associated physical processor chip. However, under certain
scenarios, vcpus may be dispatched on a different processor chip (away
from its home node).

/proc/powerpc/vcpudispatch_stats can be used to obtain statistics
related to the vcpu dispatch behavior. Writing '1' to this file enables
collecting the statistics, while writing '0' disables the statistics.
By default, the DTLB log for each vcpu is processed 50 times a second so
as not to miss any entries. This processing frequency can be changed
through /proc/powerpc/vcpudispatch_stats_freq.

The statistics themselves are available by reading the procfs file
/proc/powerpc/vcpudispatch_stats. Each line in the output corresponds to
a vcpu as represented by the first field, followed by 8 numbers.

The first number corresponds to:

1. total vcpu dispatches since the beginning of statistics collection

The next 4 numbers represent vcpu dispatch dispersions:

2. number of times this vcpu was dispatched on the same processor as last
   time
3. number of times this vcpu was dispatched on a different processor core
   as last time, but within the same chip
4. number of times this vcpu was dispatched on a different chip
5. number of times this vcpu was dispatches on a different socket/drawer
   (next numa boundary)

The final 3 numbers represent statistics in relation to the home node of
the vcpu:

6. number of times this vcpu was dispatched in its home node (chip)
7. number of times this vcpu was dispatched in a different node
8. number of times this vcpu was dispatched in a node further away (numa
   distance)

An example output::

    $ sudo cat /proc/powerpc/vcpudispatch_stats
    cpu0 6839 4126 2683 30 0 6821 18 0
    cpu1 2515 1274 1229 12 0 2509 6 0
    cpu2 2317 1198 1109 10 0 2312 5 0
    cpu3 2259 1165 1088 6 0 2256 3 0
    cpu4 2205 1143 1056 6 0 2202 3 0
    cpu5 2165 1121 1038 6 0 2162 3 0
    cpu6 2183 1127 1050 6 0 2180 3 0
    cpu7 2193 1133 1052 8 0 2187 6 0
    cpu8 2165 1115 1032 18 0 2156 9 0
    cpu9 2301 1252 1033 16 0 2293 8 0
    cpu10 2197 1138 1041 18 0 2187 10 0
    cpu11 2273 1185 1062 26 0 2260 13 0
    cpu12 2186 1125 1043 18 0 2177 9 0
    cpu13 2161 1115 1030 16 0 2153 8 0
    cpu14 2206 1153 1033 20 0 2196 10 0
    cpu15 2163 1115 1032 16 0 2155 8 0

In the output above, for vcpu0, there have been 6839 dispatches since
statistics were enabled. 4126 of those dispatches were on the same
physical cpu as the last time. 2683 were on a different core, but within
the same chip, while 30 dispatches were on a different chip compared to
its last dispatch.

Also, out of the total of 6839 dispatches, we see that there have been
6821 dispatches on the vcpu's home node, while 18 dispatches were
outside its home node, on a neighbouring chip.
