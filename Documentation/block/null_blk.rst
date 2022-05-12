.. SPDX-License-Identifier: GPL-2.0

========================
Null block device driver
========================

Overview
========

The null block device (``/dev/nullb*``) is used for benchmarking the various
block-layer implementations. It emulates a block device of X gigabytes in size.
It does not execute any read/write operation, just mark them as complete in
the request queue. The following instances are possible:

  Multi-queue block-layer

    - Request-based.
    - Configurable submission queues per device.

  No block-layer (Known as bio-based)

    - Bio-based. IO requests are submitted directly to the device driver.
    - Directly accepts bio data structure and returns them.

All of them have a completion queue for each core in the system.

Module parameters
=================

queue_mode=[0-2]: Default: 2-Multi-queue
  Selects which block-layer the module should instantiate with.

  =  ============
  0  Bio-based
  1  Single-queue (deprecated)
  2  Multi-queue
  =  ============

home_node=[0--nr_nodes]: Default: NUMA_NO_NODE
  Selects what CPU node the data structures are allocated from.

gb=[Size in GB]: Default: 250GB
  The size of the device reported to the system.

bs=[Block size (in bytes)]: Default: 512 bytes
  The block size reported to the system.

nr_devices=[Number of devices]: Default: 1
  Number of block devices instantiated. They are instantiated as /dev/nullb0,
  etc.

irqmode=[0-2]: Default: 1-Soft-irq
  The completion mode used for completing IOs to the block-layer.

  =  ===========================================================================
  0  None.
  1  Soft-irq. Uses IPI to complete IOs across CPU nodes. Simulates the overhead
     when IOs are issued from another CPU node than the home the device is
     connected to.
  2  Timer: Waits a specific period (completion_nsec) for each IO before
     completion.
  =  ===========================================================================

completion_nsec=[ns]: Default: 10,000ns
  Combined with irqmode=2 (timer). The time each completion event must wait.

submit_queues=[1..nr_cpus]: Default: 1
  The number of submission queues attached to the device driver. If unset, it
  defaults to 1. For multi-queue, it is ignored when use_per_node_hctx module
  parameter is 1.

hw_queue_depth=[0..qdepth]: Default: 64
  The hardware queue depth of the device.

Multi-queue specific parameters
-------------------------------

use_per_node_hctx=[0/1]: Default: 0
  Number of hardware context queues.

  =  =====================================================================
  0  The number of submit queues are set to the value of the submit_queues
     parameter.
  1  The multi-queue block layer is instantiated with a hardware dispatch
     queue for each CPU node in the system.
  =  =====================================================================

no_sched=[0/1]: Default: 0
  Enable/disable the io scheduler.

  =  ======================================
  0  nullb* use default blk-mq io scheduler
  1  nullb* doesn't use io scheduler
  =  ======================================

blocking=[0/1]: Default: 0
  Blocking behavior of the request queue.

  =  ===============================================================
  0  Register as a non-blocking blk-mq driver device.
  1  Register as a blocking blk-mq driver device, null_blk will set
     the BLK_MQ_F_BLOCKING flag, indicating that it sometimes/always
     needs to block in its ->queue_rq() function.
  =  ===============================================================

shared_tags=[0/1]: Default: 0
  Sharing tags between devices.

  =  ================================================================
  0  Tag set is not shared.
  1  Tag set shared between devices for blk-mq. Only makes sense with
     nr_devices > 1, otherwise there's no tag set to share.
  =  ================================================================

zoned=[0/1]: Default: 0
  Device is a random-access or a zoned block device.

  =  ======================================================================
  0  Block device is exposed as a random-access block device.
  1  Block device is exposed as a host-managed zoned block device. Requires
     CONFIG_BLK_DEV_ZONED.
  =  ======================================================================

zone_size=[MB]: Default: 256
  Per zone size when exposed as a zoned block device. Must be a power of two.

zone_nr_conv=[nr_conv]: Default: 0
  The number of conventional zones to create when block device is zoned.  If
  zone_nr_conv >= nr_zones, it will be reduced to nr_zones - 1.
