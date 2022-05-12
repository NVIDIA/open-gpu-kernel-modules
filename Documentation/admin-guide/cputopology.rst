===========================================
How CPU topology info is exported via sysfs
===========================================

Export CPU topology info via sysfs. Items (attributes) are similar
to /proc/cpuinfo output of some architectures.  They reside in
/sys/devices/system/cpu/cpuX/topology/:

physical_package_id:

	physical package id of cpuX. Typically corresponds to a physical
	socket number, but the actual value is architecture and platform
	dependent.

die_id:

	the CPU die ID of cpuX. Typically it is the hardware platform's
	identifier (rather than the kernel's).  The actual value is
	architecture and platform dependent.

core_id:

	the CPU core ID of cpuX. Typically it is the hardware platform's
	identifier (rather than the kernel's).  The actual value is
	architecture and platform dependent.

book_id:

	the book ID of cpuX. Typically it is the hardware platform's
	identifier (rather than the kernel's).	The actual value is
	architecture and platform dependent.

drawer_id:

	the drawer ID of cpuX. Typically it is the hardware platform's
	identifier (rather than the kernel's).	The actual value is
	architecture and platform dependent.

core_cpus:

	internal kernel map of CPUs within the same core.
	(deprecated name: "thread_siblings")

core_cpus_list:

	human-readable list of CPUs within the same core.
	(deprecated name: "thread_siblings_list");

package_cpus:

	internal kernel map of the CPUs sharing the same physical_package_id.
	(deprecated name: "core_siblings")

package_cpus_list:

	human-readable list of CPUs sharing the same physical_package_id.
	(deprecated name: "core_siblings_list")

die_cpus:

	internal kernel map of CPUs within the same die.

die_cpus_list:

	human-readable list of CPUs within the same die.

book_siblings:

	internal kernel map of cpuX's hardware threads within the same
	book_id.

book_siblings_list:

	human-readable list of cpuX's hardware threads within the same
	book_id.

drawer_siblings:

	internal kernel map of cpuX's hardware threads within the same
	drawer_id.

drawer_siblings_list:

	human-readable list of cpuX's hardware threads within the same
	drawer_id.

Architecture-neutral, drivers/base/topology.c, exports these attributes.
However, the book and drawer related sysfs files will only be created if
CONFIG_SCHED_BOOK and CONFIG_SCHED_DRAWER are selected, respectively.

CONFIG_SCHED_BOOK and CONFIG_SCHED_DRAWER are currently only used on s390,
where they reflect the cpu and cache hierarchy.

For an architecture to support this feature, it must define some of
these macros in include/asm-XXX/topology.h::

	#define topology_physical_package_id(cpu)
	#define topology_die_id(cpu)
	#define topology_core_id(cpu)
	#define topology_book_id(cpu)
	#define topology_drawer_id(cpu)
	#define topology_sibling_cpumask(cpu)
	#define topology_core_cpumask(cpu)
	#define topology_die_cpumask(cpu)
	#define topology_book_cpumask(cpu)
	#define topology_drawer_cpumask(cpu)

The type of ``**_id macros`` is int.
The type of ``**_cpumask macros`` is ``(const) struct cpumask *``. The latter
correspond with appropriate ``**_siblings`` sysfs attributes (except for
topology_sibling_cpumask() which corresponds with thread_siblings).

To be consistent on all architectures, include/linux/topology.h
provides default definitions for any of the above macros that are
not defined by include/asm-XXX/topology.h:

1) topology_physical_package_id: -1
2) topology_die_id: -1
3) topology_core_id: 0
4) topology_sibling_cpumask: just the given CPU
5) topology_core_cpumask: just the given CPU
6) topology_die_cpumask: just the given CPU

For architectures that don't support books (CONFIG_SCHED_BOOK) there are no
default definitions for topology_book_id() and topology_book_cpumask().
For architectures that don't support drawers (CONFIG_SCHED_DRAWER) there are
no default definitions for topology_drawer_id() and topology_drawer_cpumask().

Additionally, CPU topology information is provided under
/sys/devices/system/cpu and includes these files.  The internal
source for the output is in brackets ("[]").

    =========== ==========================================================
    kernel_max: the maximum CPU index allowed by the kernel configuration.
		[NR_CPUS-1]

    offline:	CPUs that are not online because they have been
		HOTPLUGGED off (see cpu-hotplug.txt) or exceed the limit
		of CPUs allowed by the kernel configuration (kernel_max
		above). [~cpu_online_mask + cpus >= NR_CPUS]

    online:	CPUs that are online and being scheduled [cpu_online_mask]

    possible:	CPUs that have been allocated resources and can be
		brought online if they are present. [cpu_possible_mask]

    present:	CPUs that have been identified as being present in the
		system. [cpu_present_mask]
    =========== ==========================================================

The format for the above output is compatible with cpulist_parse()
[see <linux/cpumask.h>].  Some examples follow.

In this example, there are 64 CPUs in the system but cpus 32-63 exceed
the kernel max which is limited to 0..31 by the NR_CPUS config option
being 32.  Note also that CPUs 2 and 4-31 are not online but could be
brought online as they are both present and possible::

     kernel_max: 31
        offline: 2,4-31,32-63
         online: 0-1,3
       possible: 0-31
        present: 0-31

In this example, the NR_CPUS config option is 128, but the kernel was
started with possible_cpus=144.  There are 4 CPUs in the system and cpu2
was manually taken offline (and is the only CPU that can be brought
online.)::

     kernel_max: 127
        offline: 2,4-127,128-143
         online: 0-1,3
       possible: 0-127
        present: 0-3

See cpu-hotplug.txt for the possible_cpus=NUM kernel start parameter
as well as more information on the various cpumasks.
