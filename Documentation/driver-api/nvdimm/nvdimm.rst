===============================
LIBNVDIMM: Non-Volatile Devices
===============================

libnvdimm - kernel / libndctl - userspace helper library

nvdimm@lists.linux.dev

Version 13

.. contents:

	Glossary
	Overview
	    Supporting Documents
	    Git Trees
	LIBNVDIMM PMEM and BLK
	Why BLK?
	    PMEM vs BLK
	        BLK-REGIONs, PMEM-REGIONs, Atomic Sectors, and DAX
	Example NVDIMM Platform
	LIBNVDIMM Kernel Device Model and LIBNDCTL Userspace API
	    LIBNDCTL: Context
	        libndctl: instantiate a new library context example
	    LIBNVDIMM/LIBNDCTL: Bus
	        libnvdimm: control class device in /sys/class
	        libnvdimm: bus
	        libndctl: bus enumeration example
	    LIBNVDIMM/LIBNDCTL: DIMM (NMEM)
	        libnvdimm: DIMM (NMEM)
	        libndctl: DIMM enumeration example
	    LIBNVDIMM/LIBNDCTL: Region
	        libnvdimm: region
	        libndctl: region enumeration example
	        Why Not Encode the Region Type into the Region Name?
	        How Do I Determine the Major Type of a Region?
	    LIBNVDIMM/LIBNDCTL: Namespace
	        libnvdimm: namespace
	        libndctl: namespace enumeration example
	        libndctl: namespace creation example
	        Why the Term "namespace"?
	    LIBNVDIMM/LIBNDCTL: Block Translation Table "btt"
	        libnvdimm: btt layout
	        libndctl: btt creation example
	Summary LIBNDCTL Diagram


Glossary
========

PMEM:
  A system-physical-address range where writes are persistent.  A
  block device composed of PMEM is capable of DAX.  A PMEM address range
  may span an interleave of several DIMMs.

BLK:
  A set of one or more programmable memory mapped apertures provided
  by a DIMM to access its media.  This indirection precludes the
  performance benefit of interleaving, but enables DIMM-bounded failure
  modes.

DPA:
  DIMM Physical Address, is a DIMM-relative offset.  With one DIMM in
  the system there would be a 1:1 system-physical-address:DPA association.
  Once more DIMMs are added a memory controller interleave must be
  decoded to determine the DPA associated with a given
  system-physical-address.  BLK capacity always has a 1:1 relationship
  with a single-DIMM's DPA range.

DAX:
  File system extensions to bypass the page cache and block layer to
  mmap persistent memory, from a PMEM block device, directly into a
  process address space.

DSM:
  Device Specific Method: ACPI method to control specific
  device - in this case the firmware.

DCR:
  NVDIMM Control Region Structure defined in ACPI 6 Section 5.2.25.5.
  It defines a vendor-id, device-id, and interface format for a given DIMM.

BTT:
  Block Translation Table: Persistent memory is byte addressable.
  Existing software may have an expectation that the power-fail-atomicity
  of writes is at least one sector, 512 bytes.  The BTT is an indirection
  table with atomic update semantics to front a PMEM/BLK block device
  driver and present arbitrary atomic sector sizes.

LABEL:
  Metadata stored on a DIMM device that partitions and identifies
  (persistently names) storage between PMEM and BLK.  It also partitions
  BLK storage to host BTTs with different parameters per BLK-partition.
  Note that traditional partition tables, GPT/MBR, are layered on top of a
  BLK or PMEM device.


Overview
========

The LIBNVDIMM subsystem provides support for three types of NVDIMMs, namely,
PMEM, BLK, and NVDIMM devices that can simultaneously support both PMEM
and BLK mode access.  These three modes of operation are described by
the "NVDIMM Firmware Interface Table" (NFIT) in ACPI 6.  While the LIBNVDIMM
implementation is generic and supports pre-NFIT platforms, it was guided
by the superset of capabilities need to support this ACPI 6 definition
for NVDIMM resources.  The bulk of the kernel implementation is in place
to handle the case where DPA accessible via PMEM is aliased with DPA
accessible via BLK.  When that occurs a LABEL is needed to reserve DPA
for exclusive access via one mode a time.

Supporting Documents
--------------------

ACPI 6:
	https://www.uefi.org/sites/default/files/resources/ACPI_6.0.pdf
NVDIMM Namespace:
	https://pmem.io/documents/NVDIMM_Namespace_Spec.pdf
DSM Interface Example:
	https://pmem.io/documents/NVDIMM_DSM_Interface_Example.pdf
Driver Writer's Guide:
	https://pmem.io/documents/NVDIMM_Driver_Writers_Guide.pdf

Git Trees
---------

LIBNVDIMM:
	https://git.kernel.org/cgit/linux/kernel/git/djbw/nvdimm.git
LIBNDCTL:
	https://github.com/pmem/ndctl.git
PMEM:
	https://github.com/01org/prd


LIBNVDIMM PMEM and BLK
======================

Prior to the arrival of the NFIT, non-volatile memory was described to a
system in various ad-hoc ways.  Usually only the bare minimum was
provided, namely, a single system-physical-address range where writes
are expected to be durable after a system power loss.  Now, the NFIT
specification standardizes not only the description of PMEM, but also
BLK and platform message-passing entry points for control and
configuration.

For each NVDIMM access method (PMEM, BLK), LIBNVDIMM provides a block
device driver:

    1. PMEM (nd_pmem.ko): Drives a system-physical-address range.  This
       range is contiguous in system memory and may be interleaved (hardware
       memory controller striped) across multiple DIMMs.  When interleaved the
       platform may optionally provide details of which DIMMs are participating
       in the interleave.

       Note that while LIBNVDIMM describes system-physical-address ranges that may
       alias with BLK access as ND_NAMESPACE_PMEM ranges and those without
       alias as ND_NAMESPACE_IO ranges, to the nd_pmem driver there is no
       distinction.  The different device-types are an implementation detail
       that userspace can exploit to implement policies like "only interface
       with address ranges from certain DIMMs".  It is worth noting that when
       aliasing is present and a DIMM lacks a label, then no block device can
       be created by default as userspace needs to do at least one allocation
       of DPA to the PMEM range.  In contrast ND_NAMESPACE_IO ranges, once
       registered, can be immediately attached to nd_pmem.

    2. BLK (nd_blk.ko): This driver performs I/O using a set of platform
       defined apertures.  A set of apertures will access just one DIMM.
       Multiple windows (apertures) allow multiple concurrent accesses, much like
       tagged-command-queuing, and would likely be used by different threads or
       different CPUs.

       The NFIT specification defines a standard format for a BLK-aperture, but
       the spec also allows for vendor specific layouts, and non-NFIT BLK
       implementations may have other designs for BLK I/O.  For this reason
       "nd_blk" calls back into platform-specific code to perform the I/O.

       One such implementation is defined in the "Driver Writer's Guide" and "DSM
       Interface Example".


Why BLK?
========

While PMEM provides direct byte-addressable CPU-load/store access to
NVDIMM storage, it does not provide the best system RAS (recovery,
availability, and serviceability) model.  An access to a corrupted
system-physical-address address causes a CPU exception while an access
to a corrupted address through an BLK-aperture causes that block window
to raise an error status in a register.  The latter is more aligned with
the standard error model that host-bus-adapter attached disks present.

Also, if an administrator ever wants to replace a memory it is easier to
service a system at DIMM module boundaries.  Compare this to PMEM where
data could be interleaved in an opaque hardware specific manner across
several DIMMs.

PMEM vs BLK
-----------

BLK-apertures solve these RAS problems, but their presence is also the
major contributing factor to the complexity of the ND subsystem.  They
complicate the implementation because PMEM and BLK alias in DPA space.
Any given DIMM's DPA-range may contribute to one or more
system-physical-address sets of interleaved DIMMs, *and* may also be
accessed in its entirety through its BLK-aperture.  Accessing a DPA
through a system-physical-address while simultaneously accessing the
same DPA through a BLK-aperture has undefined results.  For this reason,
DIMMs with this dual interface configuration include a DSM function to
store/retrieve a LABEL.  The LABEL effectively partitions the DPA-space
into exclusive system-physical-address and BLK-aperture accessible
regions.  For simplicity a DIMM is allowed a PMEM "region" per each
interleave set in which it is a member.  The remaining DPA space can be
carved into an arbitrary number of BLK devices with discontiguous
extents.

BLK-REGIONs, PMEM-REGIONs, Atomic Sectors, and DAX
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One of the few
reasons to allow multiple BLK namespaces per REGION is so that each
BLK-namespace can be configured with a BTT with unique atomic sector
sizes.  While a PMEM device can host a BTT the LABEL specification does
not provide for a sector size to be specified for a PMEM namespace.

This is due to the expectation that the primary usage model for PMEM is
via DAX, and the BTT is incompatible with DAX.  However, for the cases
where an application or filesystem still needs atomic sector update
guarantees it can register a BTT on a PMEM device or partition.  See
LIBNVDIMM/NDCTL: Block Translation Table "btt"


Example NVDIMM Platform
=======================

For the remainder of this document the following diagram will be
referenced for any example sysfs layouts::


                               (a)               (b)           DIMM   BLK-REGION
            +-------------------+--------+--------+--------+
  +------+  |       pm0.0       | blk2.0 | pm1.0  | blk2.1 |    0      region2
  | imc0 +--+- - - region0- - - +--------+        +--------+
  +--+---+  |       pm0.0       | blk3.0 | pm1.0  | blk3.1 |    1      region3
     |      +-------------------+--------v        v--------+
  +--+---+                               |                 |
  | cpu0 |                                     region1
  +--+---+                               |                 |
     |      +----------------------------^        ^--------+
  +--+---+  |           blk4.0           | pm1.0  | blk4.0 |    2      region4
  | imc1 +--+----------------------------|        +--------+
  +------+  |           blk5.0           | pm1.0  | blk5.0 |    3      region5
            +----------------------------+--------+--------+

In this platform we have four DIMMs and two memory controllers in one
socket.  Each unique interface (BLK or PMEM) to DPA space is identified
by a region device with a dynamically assigned id (REGION0 - REGION5).

    1. The first portion of DIMM0 and DIMM1 are interleaved as REGION0. A
       single PMEM namespace is created in the REGION0-SPA-range that spans most
       of DIMM0 and DIMM1 with a user-specified name of "pm0.0". Some of that
       interleaved system-physical-address range is reclaimed as BLK-aperture
       accessed space starting at DPA-offset (a) into each DIMM.  In that
       reclaimed space we create two BLK-aperture "namespaces" from REGION2 and
       REGION3 where "blk2.0" and "blk3.0" are just human readable names that
       could be set to any user-desired name in the LABEL.

    2. In the last portion of DIMM0 and DIMM1 we have an interleaved
       system-physical-address range, REGION1, that spans those two DIMMs as
       well as DIMM2 and DIMM3.  Some of REGION1 is allocated to a PMEM namespace
       named "pm1.0", the rest is reclaimed in 4 BLK-aperture namespaces (for
       each DIMM in the interleave set), "blk2.1", "blk3.1", "blk4.0", and
       "blk5.0".

    3. The portion of DIMM2 and DIMM3 that do not participate in the REGION1
       interleaved system-physical-address range (i.e. the DPA address past
       offset (b) are also included in the "blk4.0" and "blk5.0" namespaces.
       Note, that this example shows that BLK-aperture namespaces don't need to
       be contiguous in DPA-space.

    This bus is provided by the kernel under the device
    /sys/devices/platform/nfit_test.0 when the nfit_test.ko module from
    tools/testing/nvdimm is loaded.  This not only test LIBNVDIMM but the
    acpi_nfit.ko driver as well.


LIBNVDIMM Kernel Device Model and LIBNDCTL Userspace API
========================================================

What follows is a description of the LIBNVDIMM sysfs layout and a
corresponding object hierarchy diagram as viewed through the LIBNDCTL
API.  The example sysfs paths and diagrams are relative to the Example
NVDIMM Platform which is also the LIBNVDIMM bus used in the LIBNDCTL unit
test.

LIBNDCTL: Context
-----------------

Every API call in the LIBNDCTL library requires a context that holds the
logging parameters and other library instance state.  The library is
based on the libabc template:

	https://git.kernel.org/cgit/linux/kernel/git/kay/libabc.git

LIBNDCTL: instantiate a new library context example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

	struct ndctl_ctx *ctx;

	if (ndctl_new(&ctx) == 0)
		return ctx;
	else
		return NULL;

LIBNVDIMM/LIBNDCTL: Bus
-----------------------

A bus has a 1:1 relationship with an NFIT.  The current expectation for
ACPI based systems is that there is only ever one platform-global NFIT.
That said, it is trivial to register multiple NFITs, the specification
does not preclude it.  The infrastructure supports multiple busses and
we use this capability to test multiple NFIT configurations in the unit
test.

LIBNVDIMM: control class device in /sys/class
---------------------------------------------

This character device accepts DSM messages to be passed to DIMM
identified by its NFIT handle::

	/sys/class/nd/ndctl0
	|-- dev
	|-- device -> ../../../ndbus0
	|-- subsystem -> ../../../../../../../class/nd



LIBNVDIMM: bus
--------------

::

	struct nvdimm_bus *nvdimm_bus_register(struct device *parent,
	       struct nvdimm_bus_descriptor *nfit_desc);

::

	/sys/devices/platform/nfit_test.0/ndbus0
	|-- commands
	|-- nd
	|-- nfit
	|-- nmem0
	|-- nmem1
	|-- nmem2
	|-- nmem3
	|-- power
	|-- provider
	|-- region0
	|-- region1
	|-- region2
	|-- region3
	|-- region4
	|-- region5
	|-- uevent
	`-- wait_probe

LIBNDCTL: bus enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Find the bus handle that describes the bus from Example NVDIMM Platform::

	static struct ndctl_bus *get_bus_by_provider(struct ndctl_ctx *ctx,
			const char *provider)
	{
		struct ndctl_bus *bus;

		ndctl_bus_foreach(ctx, bus)
			if (strcmp(provider, ndctl_bus_get_provider(bus)) == 0)
				return bus;

		return NULL;
	}

	bus = get_bus_by_provider(ctx, "nfit_test.0");


LIBNVDIMM/LIBNDCTL: DIMM (NMEM)
-------------------------------

The DIMM device provides a character device for sending commands to
hardware, and it is a container for LABELs.  If the DIMM is defined by
NFIT then an optional 'nfit' attribute sub-directory is available to add
NFIT-specifics.

Note that the kernel device name for "DIMMs" is "nmemX".  The NFIT
describes these devices via "Memory Device to System Physical Address
Range Mapping Structure", and there is no requirement that they actually
be physical DIMMs, so we use a more generic name.

LIBNVDIMM: DIMM (NMEM)
^^^^^^^^^^^^^^^^^^^^^^

::

	struct nvdimm *nvdimm_create(struct nvdimm_bus *nvdimm_bus, void *provider_data,
			const struct attribute_group **groups, unsigned long flags,
			unsigned long *dsm_mask);

::

	/sys/devices/platform/nfit_test.0/ndbus0
	|-- nmem0
	|   |-- available_slots
	|   |-- commands
	|   |-- dev
	|   |-- devtype
	|   |-- driver -> ../../../../../bus/nd/drivers/nvdimm
	|   |-- modalias
	|   |-- nfit
	|   |   |-- device
	|   |   |-- format
	|   |   |-- handle
	|   |   |-- phys_id
	|   |   |-- rev_id
	|   |   |-- serial
	|   |   `-- vendor
	|   |-- state
	|   |-- subsystem -> ../../../../../bus/nd
	|   `-- uevent
	|-- nmem1
	[..]


LIBNDCTL: DIMM enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note, in this example we are assuming NFIT-defined DIMMs which are
identified by an "nfit_handle" a 32-bit value where:

   - Bit 3:0 DIMM number within the memory channel
   - Bit 7:4 memory channel number
   - Bit 11:8 memory controller ID
   - Bit 15:12 socket ID (within scope of a Node controller if node
     controller is present)
   - Bit 27:16 Node Controller ID
   - Bit 31:28 Reserved

::

	static struct ndctl_dimm *get_dimm_by_handle(struct ndctl_bus *bus,
	       unsigned int handle)
	{
		struct ndctl_dimm *dimm;

		ndctl_dimm_foreach(bus, dimm)
			if (ndctl_dimm_get_handle(dimm) == handle)
				return dimm;

		return NULL;
	}

	#define DIMM_HANDLE(n, s, i, c, d) \
		(((n & 0xfff) << 16) | ((s & 0xf) << 12) | ((i & 0xf) << 8) \
		 | ((c & 0xf) << 4) | (d & 0xf))

	dimm = get_dimm_by_handle(bus, DIMM_HANDLE(0, 0, 0, 0, 0));

LIBNVDIMM/LIBNDCTL: Region
--------------------------

A generic REGION device is registered for each PMEM range or BLK-aperture
set.  Per the example there are 6 regions: 2 PMEM and 4 BLK-aperture
sets on the "nfit_test.0" bus.  The primary role of regions are to be a
container of "mappings".  A mapping is a tuple of <DIMM,
DPA-start-offset, length>.

LIBNVDIMM provides a built-in driver for these REGION devices.  This driver
is responsible for reconciling the aliased DPA mappings across all
regions, parsing the LABEL, if present, and then emitting NAMESPACE
devices with the resolved/exclusive DPA-boundaries for the nd_pmem or
nd_blk device driver to consume.

In addition to the generic attributes of "mapping"s, "interleave_ways"
and "size" the REGION device also exports some convenience attributes.
"nstype" indicates the integer type of namespace-device this region
emits, "devtype" duplicates the DEVTYPE variable stored by udev at the
'add' event, "modalias" duplicates the MODALIAS variable stored by udev
at the 'add' event, and finally, the optional "spa_index" is provided in
the case where the region is defined by a SPA.

LIBNVDIMM: region::

	struct nd_region *nvdimm_pmem_region_create(struct nvdimm_bus *nvdimm_bus,
			struct nd_region_desc *ndr_desc);
	struct nd_region *nvdimm_blk_region_create(struct nvdimm_bus *nvdimm_bus,
			struct nd_region_desc *ndr_desc);

::

	/sys/devices/platform/nfit_test.0/ndbus0
	|-- region0
	|   |-- available_size
	|   |-- btt0
	|   |-- btt_seed
	|   |-- devtype
	|   |-- driver -> ../../../../../bus/nd/drivers/nd_region
	|   |-- init_namespaces
	|   |-- mapping0
	|   |-- mapping1
	|   |-- mappings
	|   |-- modalias
	|   |-- namespace0.0
	|   |-- namespace_seed
	|   |-- numa_node
	|   |-- nfit
	|   |   `-- spa_index
	|   |-- nstype
	|   |-- set_cookie
	|   |-- size
	|   |-- subsystem -> ../../../../../bus/nd
	|   `-- uevent
	|-- region1
	[..]

LIBNDCTL: region enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sample region retrieval routines based on NFIT-unique data like
"spa_index" (interleave set id) for PMEM and "nfit_handle" (dimm id) for
BLK::

	static struct ndctl_region *get_pmem_region_by_spa_index(struct ndctl_bus *bus,
			unsigned int spa_index)
	{
		struct ndctl_region *region;

		ndctl_region_foreach(bus, region) {
			if (ndctl_region_get_type(region) != ND_DEVICE_REGION_PMEM)
				continue;
			if (ndctl_region_get_spa_index(region) == spa_index)
				return region;
		}
		return NULL;
	}

	static struct ndctl_region *get_blk_region_by_dimm_handle(struct ndctl_bus *bus,
			unsigned int handle)
	{
		struct ndctl_region *region;

		ndctl_region_foreach(bus, region) {
			struct ndctl_mapping *map;

			if (ndctl_region_get_type(region) != ND_DEVICE_REGION_BLOCK)
				continue;
			ndctl_mapping_foreach(region, map) {
				struct ndctl_dimm *dimm = ndctl_mapping_get_dimm(map);

				if (ndctl_dimm_get_handle(dimm) == handle)
					return region;
			}
		}
		return NULL;
	}


Why Not Encode the Region Type into the Region Name?
----------------------------------------------------

At first glance it seems since NFIT defines just PMEM and BLK interface
types that we should simply name REGION devices with something derived
from those type names.  However, the ND subsystem explicitly keeps the
REGION name generic and expects userspace to always consider the
region-attributes for four reasons:

    1. There are already more than two REGION and "namespace" types.  For
       PMEM there are two subtypes.  As mentioned previously we have PMEM where
       the constituent DIMM devices are known and anonymous PMEM.  For BLK
       regions the NFIT specification already anticipates vendor specific
       implementations.  The exact distinction of what a region contains is in
       the region-attributes not the region-name or the region-devtype.

    2. A region with zero child-namespaces is a possible configuration.  For
       example, the NFIT allows for a DCR to be published without a
       corresponding BLK-aperture.  This equates to a DIMM that can only accept
       control/configuration messages, but no i/o through a descendant block
       device.  Again, this "type" is advertised in the attributes ('mappings'
       == 0) and the name does not tell you much.

    3. What if a third major interface type arises in the future?  Outside
       of vendor specific implementations, it's not difficult to envision a
       third class of interface type beyond BLK and PMEM.  With a generic name
       for the REGION level of the device-hierarchy old userspace
       implementations can still make sense of new kernel advertised
       region-types.  Userspace can always rely on the generic region
       attributes like "mappings", "size", etc and the expected child devices
       named "namespace".  This generic format of the device-model hierarchy
       allows the LIBNVDIMM and LIBNDCTL implementations to be more uniform and
       future-proof.

    4. There are more robust mechanisms for determining the major type of a
       region than a device name.  See the next section, How Do I Determine the
       Major Type of a Region?

How Do I Determine the Major Type of a Region?
----------------------------------------------

Outside of the blanket recommendation of "use libndctl", or simply
looking at the kernel header (/usr/include/linux/ndctl.h) to decode the
"nstype" integer attribute, here are some other options.

1. module alias lookup
^^^^^^^^^^^^^^^^^^^^^^

    The whole point of region/namespace device type differentiation is to
    decide which block-device driver will attach to a given LIBNVDIMM namespace.
    One can simply use the modalias to lookup the resulting module.  It's
    important to note that this method is robust in the presence of a
    vendor-specific driver down the road.  If a vendor-specific
    implementation wants to supplant the standard nd_blk driver it can with
    minimal impact to the rest of LIBNVDIMM.

    In fact, a vendor may also want to have a vendor-specific region-driver
    (outside of nd_region).  For example, if a vendor defined its own LABEL
    format it would need its own region driver to parse that LABEL and emit
    the resulting namespaces.  The output from module resolution is more
    accurate than a region-name or region-devtype.

2. udev
^^^^^^^

    The kernel "devtype" is registered in the udev database::

	# udevadm info --path=/devices/platform/nfit_test.0/ndbus0/region0
	P: /devices/platform/nfit_test.0/ndbus0/region0
	E: DEVPATH=/devices/platform/nfit_test.0/ndbus0/region0
	E: DEVTYPE=nd_pmem
	E: MODALIAS=nd:t2
	E: SUBSYSTEM=nd

	# udevadm info --path=/devices/platform/nfit_test.0/ndbus0/region4
	P: /devices/platform/nfit_test.0/ndbus0/region4
	E: DEVPATH=/devices/platform/nfit_test.0/ndbus0/region4
	E: DEVTYPE=nd_blk
	E: MODALIAS=nd:t3
	E: SUBSYSTEM=nd

    ...and is available as a region attribute, but keep in mind that the
    "devtype" does not indicate sub-type variations and scripts should
    really be understanding the other attributes.

3. type specific attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^

    As it currently stands a BLK-aperture region will never have a
    "nfit/spa_index" attribute, but neither will a non-NFIT PMEM region.  A
    BLK region with a "mappings" value of 0 is, as mentioned above, a DIMM
    that does not allow I/O.  A PMEM region with a "mappings" value of zero
    is a simple system-physical-address range.


LIBNVDIMM/LIBNDCTL: Namespace
-----------------------------

A REGION, after resolving DPA aliasing and LABEL specified boundaries,
surfaces one or more "namespace" devices.  The arrival of a "namespace"
device currently triggers either the nd_blk or nd_pmem driver to load
and register a disk/block device.

LIBNVDIMM: namespace
^^^^^^^^^^^^^^^^^^^^

Here is a sample layout from the three major types of NAMESPACE where
namespace0.0 represents DIMM-info-backed PMEM (note that it has a 'uuid'
attribute), namespace2.0 represents a BLK namespace (note it has a
'sector_size' attribute) that, and namespace6.0 represents an anonymous
PMEM namespace (note that has no 'uuid' attribute due to not support a
LABEL)::

	/sys/devices/platform/nfit_test.0/ndbus0/region0/namespace0.0
	|-- alt_name
	|-- devtype
	|-- dpa_extents
	|-- force_raw
	|-- modalias
	|-- numa_node
	|-- resource
	|-- size
	|-- subsystem -> ../../../../../../bus/nd
	|-- type
	|-- uevent
	`-- uuid
	/sys/devices/platform/nfit_test.0/ndbus0/region2/namespace2.0
	|-- alt_name
	|-- devtype
	|-- dpa_extents
	|-- force_raw
	|-- modalias
	|-- numa_node
	|-- sector_size
	|-- size
	|-- subsystem -> ../../../../../../bus/nd
	|-- type
	|-- uevent
	`-- uuid
	/sys/devices/platform/nfit_test.1/ndbus1/region6/namespace6.0
	|-- block
	|   `-- pmem0
	|-- devtype
	|-- driver -> ../../../../../../bus/nd/drivers/pmem
	|-- force_raw
	|-- modalias
	|-- numa_node
	|-- resource
	|-- size
	|-- subsystem -> ../../../../../../bus/nd
	|-- type
	`-- uevent

LIBNDCTL: namespace enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Namespaces are indexed relative to their parent region, example below.
These indexes are mostly static from boot to boot, but subsystem makes
no guarantees in this regard.  For a static namespace identifier use its
'uuid' attribute.

::

  static struct ndctl_namespace
  *get_namespace_by_id(struct ndctl_region *region, unsigned int id)
  {
          struct ndctl_namespace *ndns;

          ndctl_namespace_foreach(region, ndns)
                  if (ndctl_namespace_get_id(ndns) == id)
                          return ndns;

          return NULL;
  }

LIBNDCTL: namespace creation example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Idle namespaces are automatically created by the kernel if a given
region has enough available capacity to create a new namespace.
Namespace instantiation involves finding an idle namespace and
configuring it.  For the most part the setting of namespace attributes
can occur in any order, the only constraint is that 'uuid' must be set
before 'size'.  This enables the kernel to track DPA allocations
internally with a static identifier::

  static int configure_namespace(struct ndctl_region *region,
                  struct ndctl_namespace *ndns,
                  struct namespace_parameters *parameters)
  {
          char devname[50];

          snprintf(devname, sizeof(devname), "namespace%d.%d",
                          ndctl_region_get_id(region), paramaters->id);

          ndctl_namespace_set_alt_name(ndns, devname);
          /* 'uuid' must be set prior to setting size! */
          ndctl_namespace_set_uuid(ndns, paramaters->uuid);
          ndctl_namespace_set_size(ndns, paramaters->size);
          /* unlike pmem namespaces, blk namespaces have a sector size */
          if (parameters->lbasize)
                  ndctl_namespace_set_sector_size(ndns, parameters->lbasize);
          ndctl_namespace_enable(ndns);
  }


Why the Term "namespace"?
^^^^^^^^^^^^^^^^^^^^^^^^^

    1. Why not "volume" for instance?  "volume" ran the risk of confusing
       ND (libnvdimm subsystem) to a volume manager like device-mapper.

    2. The term originated to describe the sub-devices that can be created
       within a NVME controller (see the nvme specification:
       https://www.nvmexpress.org/specifications/), and NFIT namespaces are
       meant to parallel the capabilities and configurability of
       NVME-namespaces.


LIBNVDIMM/LIBNDCTL: Block Translation Table "btt"
-------------------------------------------------

A BTT (design document: https://pmem.io/2014/09/23/btt.html) is a stacked
block device driver that fronts either the whole block device or a
partition of a block device emitted by either a PMEM or BLK NAMESPACE.

LIBNVDIMM: btt layout
^^^^^^^^^^^^^^^^^^^^^

Every region will start out with at least one BTT device which is the
seed device.  To activate it set the "namespace", "uuid", and
"sector_size" attributes and then bind the device to the nd_pmem or
nd_blk driver depending on the region type::

	/sys/devices/platform/nfit_test.1/ndbus0/region0/btt0/
	|-- namespace
	|-- delete
	|-- devtype
	|-- modalias
	|-- numa_node
	|-- sector_size
	|-- subsystem -> ../../../../../bus/nd
	|-- uevent
	`-- uuid

LIBNDCTL: btt creation example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Similar to namespaces an idle BTT device is automatically created per
region.  Each time this "seed" btt device is configured and enabled a new
seed is created.  Creating a BTT configuration involves two steps of
finding and idle BTT and assigning it to consume a PMEM or BLK namespace::

	static struct ndctl_btt *get_idle_btt(struct ndctl_region *region)
	{
		struct ndctl_btt *btt;

		ndctl_btt_foreach(region, btt)
			if (!ndctl_btt_is_enabled(btt)
					&& !ndctl_btt_is_configured(btt))
				return btt;

		return NULL;
	}

	static int configure_btt(struct ndctl_region *region,
			struct btt_parameters *parameters)
	{
		btt = get_idle_btt(region);

		ndctl_btt_set_uuid(btt, parameters->uuid);
		ndctl_btt_set_sector_size(btt, parameters->sector_size);
		ndctl_btt_set_namespace(btt, parameters->ndns);
		/* turn off raw mode device */
		ndctl_namespace_disable(parameters->ndns);
		/* turn on btt access */
		ndctl_btt_enable(btt);
	}

Once instantiated a new inactive btt seed device will appear underneath
the region.

Once a "namespace" is removed from a BTT that instance of the BTT device
will be deleted or otherwise reset to default values.  This deletion is
only at the device model level.  In order to destroy a BTT the "info
block" needs to be destroyed.  Note, that to destroy a BTT the media
needs to be written in raw mode.  By default, the kernel will autodetect
the presence of a BTT and disable raw mode.  This autodetect behavior
can be suppressed by enabling raw mode for the namespace via the
ndctl_namespace_set_raw_mode() API.


Summary LIBNDCTL Diagram
------------------------

For the given example above, here is the view of the objects as seen by the
LIBNDCTL API::

              +---+
              |CTX|    +---------+   +--------------+  +---------------+
              +-+-+  +-> REGION0 +---> NAMESPACE0.0 +--> PMEM8 "pm0.0" |
                |    | +---------+   +--------------+  +---------------+
  +-------+     |    | +---------+   +--------------+  +---------------+
  | DIMM0 <-+   |    +-> REGION1 +---> NAMESPACE1.0 +--> PMEM6 "pm1.0" |
  +-------+ |   |    | +---------+   +--------------+  +---------------+
  | DIMM1 <-+ +-v--+ | +---------+   +--------------+  +---------------+
  +-------+ +-+BUS0+---> REGION2 +-+-> NAMESPACE2.0 +--> ND6  "blk2.0" |
  | DIMM2 <-+ +----+ | +---------+ | +--------------+  +----------------------+
  +-------+ |        |             +-> NAMESPACE2.1 +--> ND5  "blk2.1" | BTT2 |
  | DIMM3 <-+        |               +--------------+  +----------------------+
  +-------+          | +---------+   +--------------+  +---------------+
                     +-> REGION3 +-+-> NAMESPACE3.0 +--> ND4  "blk3.0" |
                     | +---------+ | +--------------+  +----------------------+
                     |             +-> NAMESPACE3.1 +--> ND3  "blk3.1" | BTT1 |
                     |               +--------------+  +----------------------+
                     | +---------+   +--------------+  +---------------+
                     +-> REGION4 +---> NAMESPACE4.0 +--> ND2  "blk4.0" |
                     | +---------+   +--------------+  +---------------+
                     | +---------+   +--------------+  +----------------------+
                     +-> REGION5 +---> NAMESPACE5.0 +--> ND1  "blk5.0" | BTT0 |
                       +---------+   +--------------+  +---------------+------+
