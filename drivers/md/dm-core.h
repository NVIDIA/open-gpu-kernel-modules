/*
 * Internal header file _only_ for device mapper core
 *
 * Copyright (C) 2016 Red Hat, Inc. All rights reserved.
 *
 * This file is released under the LGPL.
 */

#ifndef DM_CORE_INTERNAL_H
#define DM_CORE_INTERNAL_H

#include <linux/kthread.h>
#include <linux/ktime.h>
#include <linux/genhd.h>
#include <linux/blk-mq.h>
#include <linux/keyslot-manager.h>

#include <trace/events/block.h>

#include "dm.h"

#define DM_RESERVED_MAX_IOS		1024

struct dm_kobject_holder {
	struct kobject kobj;
	struct completion completion;
};

/*
 * DM core internal structures used directly by dm.c, dm-rq.c and dm-table.c.
 * DM targets must _not_ deference a mapped_device or dm_table to directly
 * access their members!
 */

struct mapped_device {
	struct mutex suspend_lock;

	struct mutex table_devices_lock;
	struct list_head table_devices;

	/*
	 * The current mapping (struct dm_table *).
	 * Use dm_get_live_table{_fast} or take suspend_lock for
	 * dereference.
	 */
	void __rcu *map;

	unsigned long flags;

	/* Protect queue and type against concurrent access. */
	struct mutex type_lock;
	enum dm_queue_mode type;

	int numa_node_id;
	struct request_queue *queue;

	atomic_t holders;
	atomic_t open_count;

	struct dm_target *immutable_target;
	struct target_type *immutable_target_type;

	char name[16];
	struct gendisk *disk;
	struct dax_device *dax_dev;

	/*
	 * A list of ios that arrived while we were suspended.
	 */
	struct work_struct work;
	wait_queue_head_t wait;
	spinlock_t deferred_lock;
	struct bio_list deferred;

	void *interface_ptr;

	/*
	 * Event handling.
	 */
	wait_queue_head_t eventq;
	atomic_t event_nr;
	atomic_t uevent_seq;
	struct list_head uevent_list;
	spinlock_t uevent_lock; /* Protect access to uevent_list */

	/* the number of internal suspends */
	unsigned internal_suspend_count;

	/*
	 * io objects are allocated from here.
	 */
	struct bio_set io_bs;
	struct bio_set bs;

	/*
	 * Processing queue (flush)
	 */
	struct workqueue_struct *wq;

	/* forced geometry settings */
	struct hd_geometry geometry;

	/* kobject and completion */
	struct dm_kobject_holder kobj_holder;

	int swap_bios;
	struct semaphore swap_bios_semaphore;
	struct mutex swap_bios_lock;

	struct dm_stats stats;

	/* for blk-mq request-based DM support */
	struct blk_mq_tag_set *tag_set;
	bool init_tio_pdu:1;

	struct srcu_struct io_barrier;
};

void disable_discard(struct mapped_device *md);
void disable_write_same(struct mapped_device *md);
void disable_write_zeroes(struct mapped_device *md);

static inline sector_t dm_get_size(struct mapped_device *md)
{
	return get_capacity(md->disk);
}

static inline struct dm_stats *dm_get_stats(struct mapped_device *md)
{
	return &md->stats;
}

#define DM_TABLE_MAX_DEPTH 16

struct dm_table {
	struct mapped_device *md;
	enum dm_queue_mode type;

	/* btree table */
	unsigned int depth;
	unsigned int counts[DM_TABLE_MAX_DEPTH]; /* in nodes */
	sector_t *index[DM_TABLE_MAX_DEPTH];

	unsigned int num_targets;
	unsigned int num_allocated;
	sector_t *highs;
	struct dm_target *targets;

	struct target_type *immutable_target_type;

	bool integrity_supported:1;
	bool singleton:1;
	unsigned integrity_added:1;

	/*
	 * Indicates the rw permissions for the new logical
	 * device.  This should be a combination of FMODE_READ
	 * and FMODE_WRITE.
	 */
	fmode_t mode;

	/* a list of devices used by this table */
	struct list_head devices;

	/* events get handed up using this callback */
	void (*event_fn)(void *);
	void *event_context;

	struct dm_md_mempools *mempools;

#ifdef CONFIG_BLK_INLINE_ENCRYPTION
	struct blk_keyslot_manager *ksm;
#endif
};

static inline struct completion *dm_get_completion_from_kobject(struct kobject *kobj)
{
	return &container_of(kobj, struct dm_kobject_holder, kobj)->completion;
}

unsigned __dm_get_module_param(unsigned *module_param, unsigned def, unsigned max);

static inline bool dm_message_test_buffer_overflow(char *result, unsigned maxlen)
{
	return !maxlen || strlen(result) + 1 >= maxlen;
}

extern atomic_t dm_global_event_nr;
extern wait_queue_head_t dm_global_eventq;
void dm_issue_global_event(void);

#endif
